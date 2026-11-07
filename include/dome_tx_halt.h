// =============================================================================
// include/dome_tx_halt.h
//
// No dome panel moves on the Sequence Coordinator's word during a halt (#502,
// operator, 2026-11-07). The Coordinator stops queueing panel commands the
// moment it sees an estop or Sleep Mode, but a command it queued just before
// the halt landed would still go out when DomeLinkTask drains domeTxQueue.
// So every command the Coordinator queues carries its origin, and DomeLinkTask
// asks this rule, with the halts as they are when it sends, before each one.
//
// Only the Coordinator's panel commands are held back: an open, a close, a
// flutter, a dome sequence and a move. Its light and sound resets still go,
// because an estop is when the dome's lights should come back to rest. A
// command a person sends (POST /api/dome/cmd, the Console, an alias a
// sequenceStart() forwards) carries no Coordinator origin and is never
// touched here.
//
// Pure: no FreeRTOS, no Arduino.
// =============================================================================
#pragma once

#include <stdint.h>
#include <string.h>

// Who queued a dome command (DomeTxCmd::origin, include/dome_link.h).
enum DomeTxOrigin : uint8_t {
    DOME_TX_OTHER = 0,        // a person's command, a forward, a link message
    DOME_TX_COORDINATOR = 1,  // the Sequence Coordinator (src/tasks/sequence_dispatcher.cpp)
};

// Whether `cmd` moves a dome panel: the Marcduino open, close, flutter, dome
// sequence and move commands, ":OPnn", ":CLnn", ":OFnn", ":SEnn", ":MVnn".
inline bool domeTxMovesPanel(const char* cmd) {
    if (cmd == nullptr || cmd[0] != ':' || cmd[1] == '\0' || cmd[2] == '\0') {
        return false;
    }
    static const char* const kPanelVerbs[] = {"OP", "CL", "OF", "SE", "MV"};
    for (const char* verb : kPanelVerbs) {
        if (cmd[1] == verb[0] && cmd[2] == verb[1]) {
            return true;
        }
    }
    return false;
}

// Whether DomeLinkTask must drop this command rather than send it now.
inline bool domeTxHeldByHalt(uint8_t origin, const char* cmd, bool estopLatched, bool sleepMode) {
    return origin == DOME_TX_COORDINATOR && (estopLatched || sleepMode) && domeTxMovesPanel(cmd);
}
