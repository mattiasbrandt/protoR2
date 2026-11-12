// =============================================================================
// include/dome_link_arbiter.h
//
// protoR2link Arbiter  --  pure decision module for dome-link transport selection,
// probe cadence, and sleep-sync.
//
// Design (ADR 0005):
//   - domeLinkArbiterStep() is a pure function over explicit state: no side
//     effects, no hardware access, no Arduino/FreeRTOS dependencies.
//   - The task loop (dome_link.cpp) gathers inputs, calls this function, and
//     executes the returned actions through the existing concrete transport
//     functions. The arbiter decides WHEN; the shell does HOW.
//   - Native timeline tests assert on DomeLinkArbiterActions directly  --  no
//     fakes, no mocks needed.
//
// This header is Arduino-free and compiles in the native test environment.
// =============================================================================
#pragma once

#include <stdint.h>

#include "dome_link_transport.h"

// ---------------------------------------------------------------------------
// Timing constants (moved here from dome_link.cpp)
// ---------------------------------------------------------------------------

constexpr uint32_t kDomeLinkHeartbeatIntervalMs  = 1000;
constexpr uint32_t kDomeLinkHeartbeatTimeoutMs   = 5000;
constexpr uint32_t kDomeLinkUartProbeIntervalMs  = 30000;
constexpr uint32_t kDomeLinkUartProbeWindowMs    = 150;
constexpr uint32_t kDomeLinkPeerLookupIntervalMs = 5000;  // mDNS dome peer re-lookup cadence

// ---------------------------------------------------------------------------
// Sleep-sync action
// ---------------------------------------------------------------------------

enum class SleepSyncAction : uint8_t {
    None,
    SendSleep,
    SendWake,
};

// ---------------------------------------------------------------------------
// Arbiter state (default-init == boot state)
// ---------------------------------------------------------------------------

struct DomeLinkArbiterState {
    uint32_t lastUartHeartbeatMs;    // 0 = never seen this boot
    uint32_t lastUartProbeMs;        // 0 = never probed
    uint32_t uartProbeWindowUntilMs; // 0 = not in probe window
    uint32_t lastHeartbeatTxMs;      // 0 = never sent
    bool     sleepSynced;            // reset on dome disconnect
    bool     lastSyncedSleepMode;    // last sleep mode sent to the dome
    bool     uartEverSeen;           // true once UART #APHB received this boot
};

// ---------------------------------------------------------------------------
// Arbiter inputs (gathered by the task shell each tick)
// ---------------------------------------------------------------------------

struct DomeLinkArbiterInputs {
    uint32_t nowMs;
    bool     uartHeartbeatSeen; // #APHB received on UART this tick
    bool     staConnected;      // WiFi STA connected
    bool     peerKnown;         // dome peer IP resolved
    bool     bodySleeping;      // current body sleep-mode state
    bool     domeConnected;     // dome heartbeat seen within timeout
};

// ---------------------------------------------------------------------------
// Arbiter actions (executed by the task shell after each step)
// ---------------------------------------------------------------------------

struct DomeLinkArbiterActions {
    DomeLinkTransport txRoute;            // route queue drain, heartbeat TX, sleep sync
    bool              acquireUart;        // shell calls acquireDomeUart()
    bool              releaseUartToAudio; // shell calls releaseUartToAudioRx()
    bool              sendUartProbe;      // shell prints MD_BODY_HB over UART
    bool              sendHeartbeat;      // shell sends 1 Hz #PAHB on txRoute
    SleepSyncAction   sleepSync;          // shell sends sleep/wake command on txRoute
};

// ---------------------------------------------------------------------------
// Peer lookup gate (#507)
//
// The mDNS dome peer lookup blocks the task loop that drains domeTxQueue (up
// to its 2000 ms timeout when nobody answers). It is only worth running when
// its answer could be used: the peer IP only carries traffic on the WiFi
// fallback, which is closed for the rest of the boot once the UART has been
// seen.
// ---------------------------------------------------------------------------

struct DomeLinkPeerLookupInputs {
    uint32_t nowMs;
    uint32_t lastLookupMs;   // when the last lookup ran; 0 at boot, so the first
                             // lookup waits one interval (covers the UART grace)
    bool     manualPeerIp;   // a dome peer IP is configured; no lookup needed
    bool     staConnected;   // WiFi STA connected
    bool     peerKnown;      // a peer IP is cached
    bool     peerAnswering;  // a heartbeat from the cached peer IP within timeout
};

// True when the shell should run the mDNS lookup this tick. False with a
// manual peer IP, without STA, once the UART has been seen this boot, and
// while the cached peer IP itself is answering heartbeats (a dome that moved
// to a new address does not count). Otherwise true every kDomeLinkPeerLookupIntervalMs.
bool domeLinkPeerLookupDue(const DomeLinkArbiterState&    s,
                           const DomeLinkPeerLookupInputs& in);

// ---------------------------------------------------------------------------
// Dome peer cache (#507)
//
// Which dome IP the WiFi fallback sends to, where it came from, and whether
// that address itself is answering. IPv4 addresses are plain uint32_t (the
// IPAddress operator uint32_t value; 0 = none) so this stays Arduino-free.
// The shell calls, each tick:
//   1. domeLinkPeerCacheBeginTick()  -- true: run the mDNS lookup now
//   2. domeLinkPeerCacheLookupResult() with what the lookup returned
//   3. domeLinkPeerCacheHeartbeat() for every UDP #APHB, with its sender
// ---------------------------------------------------------------------------

struct DomeLinkPeerCache {
    uint32_t ipV4;          // cached peer address (0 = none)
    uint32_t lastLookupMs;  // when the last lookup ran
    uint32_t lastPeerHbMs;  // last #APHB from ipV4 itself (0 = none)
    bool     known;         // ipV4 may carry traffic
    bool     manual;        // ipV4 came from the configured peer IP
};

// Applies the configured peer IP (manualIpV4, 0 = none) and STA state to the
// cache, then answers whether the lookup is due (domeLinkPeerLookupDue). A
// cleared manual IP or a lost STA forgets the cached peer and its heartbeats,
// so they cannot hold the lookup closed.
bool domeLinkPeerCacheBeginTick(DomeLinkPeerCache& c, const DomeLinkArbiterState& s,
                                uint32_t nowMs, bool staConnected, uint32_t manualIpV4);

// Records a lookup's answer (0 = not found: the cache is kept). A different
// address drops the old address's heartbeat stamp.
void domeLinkPeerCacheLookupResult(DomeLinkPeerCache& c, uint32_t resolvedIpV4);

// Records a dome heartbeat received over UDP from senderIpV4. Only one sent
// from the cached address counts: the dome keeps sending after it moves.
void domeLinkPeerCacheHeartbeat(DomeLinkPeerCache& c, uint32_t senderIpV4, uint32_t nowMs);

// True while the cached address has sent a heartbeat within the timeout.
bool domeLinkPeerAnswering(const DomeLinkPeerCache& c, uint32_t nowMs);

// ---------------------------------------------------------------------------
// Step function
// ---------------------------------------------------------------------------

DomeLinkArbiterActions domeLinkArbiterStep(DomeLinkArbiterState&       s,
                                           const DomeLinkArbiterInputs& in);
