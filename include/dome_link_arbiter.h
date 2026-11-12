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
// Step function
// ---------------------------------------------------------------------------

DomeLinkArbiterActions domeLinkArbiterStep(DomeLinkArbiterState&       s,
                                           const DomeLinkArbiterInputs& in);
