// =============================================================================
// include/sequence_run_takeover.h
//
// The Run Takeover (#502, GLOSSARY.md): what each event ends among the things
// the Sequence Coordinator is doing. One row per event, and this is the only
// place a row is written. The Coordinator (src/tasks/sequence_dispatcher.cpp)
// looks the event up and ends what its row names; it decides nothing about
// endings by the order of its own code.
//
// What the Coordinator can be doing, one bit each:
//   a sequence                                   RUN_TAKEOVER_SEQUENCE
//   a back to centre sweep or the boot pass      RUN_TAKEOVER_CENTRE
//   a pose being reached                         RUN_TAKEOVER_POSE
//   the Gestures, flutters and takes a run fired RUN_TAKEOVER_GENERATED
//   a staged ring close                          RUN_TAKEOVER_RING_CLOSE
//
// A row's `ends` names what the event ends. What it leaves out keeps running.
// Three cells are "replaced" rather than ended: a pose press replaces a pose,
// back to centre replaces a sweep or the boot pass, and a later run replaces
// the sequence. The start function does the replacing (sequencePoseStart()
// keeps the old pose's spacing), so those cells are not `ends` bits, except
// the later run's: the old sequence is ended, with outcome `preempted`, before
// the new one is loaded, so its buffers are free for it.
//
// A STAGED RING CLOSE HOLDS DURING A HALT (operator, 2026-11-07). No row ends
// one on an estop or Sleep Mode, and no close command goes out while either
// is on. When the halt ends it starts again from the first panel, because
// DomeLinkTask drops a Coordinator panel command it would send under a halt
// (include/dome_tx_halt.h), so the close queued just before it may be lost.
//
// A HALT DROPS THE ENDED RUN'S OWN CLOSES (operator, 2026-11-07). A sequence's
// cleanup closes the ring panels it left open, one every 500 ms, after its
// instant resets. On an estop or Sleep Mode those staggered closes are dropped
// (`dropsRunCloses`), so nothing closes during the halt. The estop clear, or
// waking after Sleep Mode ended the run, stages one paced close of every ring
// panel instead: one close sequence, never two side by side. A dome
// reconnect drops them for the same reason: it stages its own close.
//
// Pure: no FreeRTOS, no Arduino, no clock.
// =============================================================================
#pragma once

#include <stdint.h>

#include "sequence_run_evidence.h"  // SeqRunOutcome - how an ended sequence is recorded

// What the Coordinator can be doing, as bits of a row's `ends`.
enum RunTakeoverRun : uint8_t {
    RUN_TAKEOVER_SEQUENCE   = 1u << 0,
    RUN_TAKEOVER_CENTRE     = 1u << 1,
    RUN_TAKEOVER_POSE       = 1u << 2,
    RUN_TAKEOVER_GENERATED  = 1u << 3,
    RUN_TAKEOVER_RING_CLOSE = 1u << 4,
};

constexpr uint8_t RUN_TAKEOVER_ALL = RUN_TAKEOVER_SEQUENCE | RUN_TAKEOVER_CENTRE | RUN_TAKEOVER_POSE |
                                     RUN_TAKEOVER_GENERATED | RUN_TAKEOVER_RING_CLOSE;

// The events, in table order. A row's index is its event.
enum RunTakeoverEvent : uint8_t {
    RUN_TAKEOVER_LATER_RUN = 0,  // a run is asked for and loads
    RUN_TAKEOVER_POSE_PRESS,     // a pose press is taken and its plan staged
    RUN_TAKEOVER_BACK_TO_CENTRE, // back to centre is pressed and not refused
    RUN_TAKEOVER_STOP,           // the non-latching Stop
    RUN_TAKEOVER_ESTOP,          // on every tick the estop is latched
    RUN_TAKEOVER_SLEEP_MODE,     // on every tick of Sleep Mode with no estop
    RUN_TAKEOVER_DOME_RECONNECT, // the dome link comes (back) up
    RUN_TAKEOVER_ESTOP_CLEAR,    // the estop clears
    RUN_TAKEOVER_WAKING,         // Sleep Mode ends, after it ended a sequence
    RUN_TAKEOVER_EVENT_COUNT
};

struct RunTakeoverRow {
    RunTakeoverEvent event;
    uint8_t ends;          // RunTakeoverRun bits
    bool stagesRingClose;  // and the dome's visual reset and the latches
    bool dropsRunCloses;   // an ended run's staggered ring closes not yet sent
    // How a sequence this ends is recorded (GET /api/seq/last-run), and the
    // reason, which is also the word the Coordinator's log lines use.
    SeqRunOutcome outcome;
    const char* why;
};

constexpr RunTakeoverRow kRunTakeover[RUN_TAKEOVER_EVENT_COUNT] = {
    {RUN_TAKEOVER_LATER_RUN, RUN_TAKEOVER_ALL, false, false, SEQ_RUN_PREEMPTED, "preempt"},
    {RUN_TAKEOVER_POSE_PRESS,
     RUN_TAKEOVER_SEQUENCE | RUN_TAKEOVER_CENTRE | RUN_TAKEOVER_GENERATED | RUN_TAKEOVER_RING_CLOSE, false, false,
     SEQ_RUN_ABORTED, "pose"},
    {RUN_TAKEOVER_BACK_TO_CENTRE, RUN_TAKEOVER_SEQUENCE | RUN_TAKEOVER_POSE | RUN_TAKEOVER_GENERATED, false, false,
     SEQ_RUN_ABORTED, "back to centre"},
    {RUN_TAKEOVER_STOP,
     RUN_TAKEOVER_SEQUENCE | RUN_TAKEOVER_CENTRE | RUN_TAKEOVER_POSE | RUN_TAKEOVER_GENERATED, false, false,
     SEQ_RUN_ABORTED, "web stop"},
    {RUN_TAKEOVER_ESTOP,
     RUN_TAKEOVER_SEQUENCE | RUN_TAKEOVER_CENTRE | RUN_TAKEOVER_POSE | RUN_TAKEOVER_GENERATED, false, true,
     SEQ_RUN_ESTOP, "estop"},
    {RUN_TAKEOVER_SLEEP_MODE,
     RUN_TAKEOVER_SEQUENCE | RUN_TAKEOVER_CENTRE | RUN_TAKEOVER_POSE | RUN_TAKEOVER_GENERATED, false, true,
     SEQ_RUN_ABORTED, "sleep mode"},
    {RUN_TAKEOVER_DOME_RECONNECT, RUN_TAKEOVER_SEQUENCE | RUN_TAKEOVER_POSE | RUN_TAKEOVER_GENERATED, true, true,
     SEQ_RUN_RECONNECT, "dome reconnect"},
    {RUN_TAKEOVER_ESTOP_CLEAR, RUN_TAKEOVER_POSE, true, false, SEQ_RUN_NONE, "estop clear"},
    {RUN_TAKEOVER_WAKING, RUN_TAKEOVER_POSE, true, false, SEQ_RUN_NONE, "waking"},
};

constexpr bool runTakeoverRowsInOrder(uint8_t i = 0) {
    return i >= RUN_TAKEOVER_EVENT_COUNT ||
           (kRunTakeover[i].event == (RunTakeoverEvent)i && runTakeoverRowsInOrder((uint8_t)(i + 1)));
}
static_assert(runTakeoverRowsInOrder(), "a Run Takeover row's index is its event");

inline const RunTakeoverRow& runTakeoverRow(RunTakeoverEvent event) {
    return kRunTakeover[event < RUN_TAKEOVER_EVENT_COUNT ? event : RUN_TAKEOVER_STOP];
}

// -----------------------------------------------------------------------------
// sequenceRunRefusal()
// Why a run may not start, in the words the surface shows, or nullptr when it
// may. Refused under either halt, never queued for later: the shape a pose
// press keeps (sequencePoseRefusal()), with words for a run. The web route and
// the Console rows ask it to answer the press, and the Coordinator asks it
// again when the request reaches it, because a halt can land in between.
// -----------------------------------------------------------------------------
inline const char* sequenceRunRefusal(bool estopLatched, bool sleepMode) {
    if (estopLatched) return "Estop latched. Clear it to run a sequence.";
    if (sleepMode) return "The droid is asleep. Wake it to run a sequence.";
    return nullptr;
}
