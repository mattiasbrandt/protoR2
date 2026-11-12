// =============================================================================
// src/dome_link_arbiter.cpp
//
// protoR2link Arbiter  --  transport selection, probe cadence, and sleep-sync
// decisions as a pure step function over explicit state.
//
// See include/dome_link_arbiter.h and docs/adr/0005-protor2link-arbiter-
// functional-core.md for design rationale.
// =============================================================================

#include "dome_link_arbiter.h"

DomeLinkArbiterActions domeLinkArbiterStep(DomeLinkArbiterState&        s,
                                           const DomeLinkArbiterInputs& in) {
    DomeLinkArbiterActions a = {};
    a.txRoute = DOME_LINK_TRANSPORT_UART;

    // -----------------------------------------------------------------------
    // 1. Latch UART heartbeat timestamp
    // -----------------------------------------------------------------------

    if (in.uartHeartbeatSeen) {
        s.lastUartHeartbeatMs = in.nowMs;
        s.uartEverSeen        = true;
    }

    // -----------------------------------------------------------------------
    // 2. Transport selection
    //
    // WiFi fallback is allowed only when UART has never established contact
    // this boot. Once UART contact is seen, a heartbeat loss means the dome
    // is off or rebooting  --  stay on UART and keep probing (via the normal
    // 1 Hz heartbeat) rather than silently migrating to WiFi (ADR 0005,
    // resolves the WiFi-steady-state deviation accepted for v1.0.0).
    // -----------------------------------------------------------------------

    const bool uartFresh =
        s.lastUartHeartbeatMs > 0 &&
        (in.nowMs - s.lastUartHeartbeatMs) < kDomeLinkHeartbeatTimeoutMs;

    const bool initialUartGrace =
        s.lastUartHeartbeatMs == 0 && in.nowMs < kDomeLinkHeartbeatTimeoutMs;

    const bool wantWifi = !uartFresh && !initialUartGrace &&
                          in.staConnected && in.peerKnown &&
                          !s.uartEverSeen;

    if (!wantWifi) {
        s.uartProbeWindowUntilMs = 0;
        a.acquireUart            = true;
        a.txRoute                = DOME_LINK_TRANSPORT_UART;
    } else {
        if ((in.nowMs - s.lastUartProbeMs) >= kDomeLinkUartProbeIntervalMs) {
            s.lastUartProbeMs        = in.nowMs;
            s.uartProbeWindowUntilMs = in.nowMs + kDomeLinkUartProbeWindowMs;
            a.acquireUart            = true;
            a.sendUartProbe          = true;
        } else if (s.uartProbeWindowUntilMs != 0 &&
                   (int32_t)(in.nowMs - s.uartProbeWindowUntilMs) >= 0) {
            a.releaseUartToAudio     = true;
            s.uartProbeWindowUntilMs = 0;
        }
        a.txRoute = DOME_LINK_TRANSPORT_WIFI;
    }

    // -----------------------------------------------------------------------
    // 3. 1 Hz heartbeat gate
    // -----------------------------------------------------------------------

    if ((in.nowMs - s.lastHeartbeatTxMs) >= kDomeLinkHeartbeatIntervalMs) {
        s.lastHeartbeatTxMs = in.nowMs;
        a.sendHeartbeat     = true;
    }

    // -----------------------------------------------------------------------
    // 4. Sleep-sync state machine
    //
    // Body sleep state is ground truth. Send #PASL/#PAWU on every fresh dome
    // connect (sleepSynced resets on disconnect) and on sleep-mode change
    // while connected. Polling avoids a stale #PASL firing on reconnect.
    // -----------------------------------------------------------------------

    if (!in.domeConnected) {
        s.sleepSynced = false;
    } else if (!s.sleepSynced || in.bodySleeping != s.lastSyncedSleepMode) {
        a.sleepSync           = in.bodySleeping ? SleepSyncAction::SendSleep
                                                : SleepSyncAction::SendWake;
        s.sleepSynced         = true;
        s.lastSyncedSleepMode = in.bodySleeping;
    }

    return a;
}

bool domeLinkPeerLookupDue(const DomeLinkArbiterState&    s,
                           const DomeLinkPeerLookupInputs& in) {
    if (in.manualPeerIp || !in.staConnected) {
        return false;
    }
    // The WiFi fallback never opens again this boot (see wantWifi above), so
    // a resolved address could not be sent to.
    if (s.uartEverSeen) {
        return false;
    }
    // A known peer answering from its cached address needs no re-lookup; once
    // that address goes quiet (dome off, or moved) the lookup comes back.
    if (in.peerKnown && in.peerAnswering) {
        return false;
    }
    return (uint32_t)(in.nowMs - in.lastLookupMs) >= kDomeLinkPeerLookupIntervalMs;
}

static void forgetPeer(DomeLinkPeerCache& c) {
    c.ipV4         = 0;
    c.known        = false;
    c.manual       = false;
    c.lastPeerHbMs = 0;
}

bool domeLinkPeerAnswering(const DomeLinkPeerCache& c, uint32_t nowMs) {
    return c.known && c.lastPeerHbMs != 0 &&
           (uint32_t)(nowMs - c.lastPeerHbMs) < kDomeLinkHeartbeatTimeoutMs;
}

bool domeLinkPeerCacheBeginTick(DomeLinkPeerCache& c, const DomeLinkArbiterState& s,
                                uint32_t nowMs, bool staConnected, uint32_t manualIpV4) {
    if (manualIpV4 != 0) {
        if (!c.manual || c.ipV4 != manualIpV4) {
            c.lastPeerHbMs = 0;
        }
        c.ipV4   = manualIpV4;
        c.known  = true;
        c.manual = true;
        return false;
    }
    // A cleared manual setting: its address and heartbeats are not discovery's.
    if (c.manual) {
        forgetPeer(c);
    }
    if (!staConnected) {
        forgetPeer(c);
        return false;
    }

    DomeLinkPeerLookupInputs in = {};
    in.nowMs         = nowMs;
    in.lastLookupMs  = c.lastLookupMs;
    in.manualPeerIp  = false;
    in.staConnected  = true;
    in.peerKnown     = c.known;
    in.peerAnswering = domeLinkPeerAnswering(c, nowMs);
    if (!domeLinkPeerLookupDue(s, in)) {
        return false;
    }
    c.lastLookupMs = nowMs;
    return true;
}

void domeLinkPeerCacheLookupResult(DomeLinkPeerCache& c, uint32_t resolvedIpV4) {
    if (resolvedIpV4 == 0) {
        return;
    }
    if (resolvedIpV4 != c.ipV4) {
        c.lastPeerHbMs = 0;
    }
    c.ipV4  = resolvedIpV4;
    c.known = true;
}

void domeLinkPeerCacheHeartbeat(DomeLinkPeerCache& c, uint32_t senderIpV4, uint32_t nowMs) {
    if (c.known && senderIpV4 != 0 && senderIpV4 == c.ipV4) {
        c.lastPeerHbMs = (nowMs == 0) ? 1 : nowMs;  // 0 means "none"
    }
}
