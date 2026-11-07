// =============================================================================
// test/test_native/test_sequence_run_takeover/test_sequence_run_takeover.cpp
//
// The Run Takeover (#502, include/sequence_run_takeover.h) through the
// Coordinator's own tick (sequenceDispatcherTick()). Each case sets up what
// the Coordinator is doing the way the droid would - a request, a press, a
// halt in RobotState - fires one event and reads what is still running
// (sequenceDispatcherRunning()), what reached the dome (the domeQueueTx()
// stub) and how a sequence was recorded (seqEvidenceSummary()).
//
// The walk at the bottom fires every row's event at every run it can share
// the droid with, and holds the outcome to the row: ended where the row's
// `ends` names it, still running where it does not.
// =============================================================================

#include <string.h>

#include <unity.h>

#include "Preferences.h"
#include "config_cache.h"
#include "config_store.h"
#include "dome_tx_halt.h"
#include "drive_motion_test_hooks.h"  // g_test_millis
#include "marcduino_test_hooks.h"     // the dome link stub: connected, tx calls, last tx
#include "robot_state.h"
#include "seq_store_index.h"
#include "seq_store_test_hooks.h"  // g_test_seq_prepare_latches_estop
#include "sequence_dispatcher.h"
#include "sequence_dispatcher_test_hooks.h"
#include "sequence_run_evidence.h"
#include "sequence_run_takeover.h"


static uint32_t now = 1000;

// One tick, `ms` after the last, with or without a run asked for.
static void tick(uint32_t ms = 10, const char* run = nullptr) {
    now += ms;
    g_test_millis = now;
    SequenceRequest req = {};
    if (run != nullptr) {
        strncpy(req.name, run, sizeof(req.name) - 1);
        req.src = SRC_WEB_API;
    }
    sequenceDispatcherTick(run != nullptr ? &req : nullptr, now);
}

static void ticksFor(uint32_t ms) {
    for (uint32_t t = 0; t < ms; t += 10) {
        tick();
    }
}

static uint8_t running() {
    return sequenceDispatcherRunning();
}

static SeqRunSummary lastRun() {
    SeqRunSummary s = {};
    seqEvidenceSummary(s);
    return s;
}

// How many individual ring closes (:CLnn) reached the dome since the count
// was last zeroed, read one tick at a time.
static unsigned ringClosesSent = 0;
static void tickCountingCloses(uint32_t ms = 10) {
    const unsigned before = g_test_dome_tx_calls;
    tick(ms);
    if (g_test_dome_tx_calls != before && strncmp(g_test_dome_last_tx, ":CL", 3) == 0) {
        ringClosesSent++;
    }
}

// The droid's servo rows as a cleared store loads them, with one Part on the
// board's first Output so a flutter has something to swing.
static void loadRowsWithAPart() {
    Preferences prefs;
    prefs.begin("proto", false);
    prefs.clear();
    ServoOutputRepairReport report = {};
    configLoadServoOutputs(prefs, &report);
    prefs.end();
    ServoOutputPartMove move = {};
    snprintf(move.part, sizeof(move.part), "%s", "utilUp");
    move.fromOutput = false;
    move.toOutput = true;
    move.toDriver = boardOutputAddress(0).driver;
    move.toChannel = boardOutputAddress(0).channel;
    configCacheMoveServoOutputPart(move);
}

void setUp() {
    robotState = RobotState{};
    g_test_dome_connected = false;
    g_test_dome_tx_ok = true;
    g_test_seq_prepare_latches_estop = false;
    seqStoreIndexClear();
    loadRowsWithAPart();
    now += 100000;
    g_test_millis = now;
    sequenceDispatcherBegin();
    // The boot pass is a sweep like back to centre; Stop ends it, so every
    // case starts from a Coordinator doing nothing.
    sequenceStopRequest();
    tick();
    g_test_dome_tx_calls = 0;
    g_test_dome_last_tx[0] = '\0';
    ringClosesSent = 0;
}

void tearDown() {}

// -----------------------------------------------------------------------------
// Putting each run on the droid
// -----------------------------------------------------------------------------

static void startSequence(const char* name = "DM:VADER") {
    tick(10, name);
    TEST_ASSERT_TRUE_MESSAGE(running() & RUN_TAKEOVER_SEQUENCE, "a sequence is running");
}

static void startCentre() {
    taskENTER_CRITICAL(&robotStateMux);
    robotState.bulkCentreRequest = SRC_WEB_API;
    taskEXIT_CRITICAL(&robotStateMux);
    tick();
    TEST_ASSERT_TRUE_MESSAGE(running() & RUN_TAKEOVER_CENTRE, "back to centre is sweeping");
}

static void startPose() {
    TEST_ASSERT_TRUE(sequencePoseRequest("DM:OPENALL", 5000, SRC_WEB_API));
    tick();
    TEST_ASSERT_TRUE_MESSAGE(running() & RUN_TAKEOVER_POSE, "a pose is being reached");
}

static void startFlutter() {
    sequenceFlutterRequest(boardOutputAddress(0), 2000);
    tick();
    TEST_ASSERT_TRUE_MESSAGE(running() & RUN_TAKEOVER_GENERATED, "a flutter is swinging");
}

// A staged ring close, from a dome that comes up: the reconnect row stages it,
// and its first close goes on that same tick unless a halt holds it.
static void startRingClose() {
    g_test_dome_connected = false;
    tick();
    g_test_dome_connected = true;
    tick();
    TEST_ASSERT_TRUE_MESSAGE(running() & RUN_TAKEOVER_RING_CLOSE, "a ring close is staged");
}

// -----------------------------------------------------------------------------
// The walk: every row's event against every run it can share the droid with
// -----------------------------------------------------------------------------

typedef void (*StartFn)();

struct RunUnderTest {
    uint8_t bit;
    StartFn start;
};

static void startSequenceDefault() { startSequence(); }

static const RunUnderTest kRuns[] = {
    {RUN_TAKEOVER_SEQUENCE, startSequenceDefault},
    {RUN_TAKEOVER_CENTRE, startCentre},
    {RUN_TAKEOVER_POSE, startPose},
    {RUN_TAKEOVER_GENERATED, startFlutter},
    {RUN_TAKEOVER_RING_CLOSE, startRingClose},
};

static void fire(RunTakeoverEvent event) {
    switch (event) {
        case RUN_TAKEOVER_LATER_RUN:
            tick(10, "DM:ROCKMARCH");
            return;
        case RUN_TAKEOVER_POSE_PRESS:
            TEST_ASSERT_TRUE(sequencePoseRequest("DM:OPENALL", 5000, SRC_WEB_API));
            tick();
            return;
        case RUN_TAKEOVER_BACK_TO_CENTRE:
            robotState.bulkCentreRequest = SRC_WEB_API;
            tick();
            return;
        case RUN_TAKEOVER_STOP:
            sequenceStopRequest();
            tick();
            return;
        case RUN_TAKEOVER_ESTOP:
            robotState.estop = true;
            tick();
            return;
        case RUN_TAKEOVER_SLEEP_MODE:
            robotState.sleepMode = true;
            tick();
            return;
        case RUN_TAKEOVER_DOME_RECONNECT:
            g_test_dome_connected = false;
            tick();
            g_test_dome_connected = true;
            tick();
            return;
        case RUN_TAKEOVER_ESTOP_CLEAR:
            robotState.estop = true;
            tick();
            robotState.estop = false;
            tick();
            return;
        case RUN_TAKEOVER_WAKING:
            robotState.sleepMode = true;
            tick();
            robotState.sleepMode = false;
            tick();
            return;
        case RUN_TAKEOVER_EVENT_COUNT:
            break;
    }
    TEST_FAIL_MESSAGE("an event with no way to fire it");
}

// A run the event's row leaves running must still be running after it,
// unless the event cannot leave anything running at all to look at. Three
// cells are "replaced": the run is still there, a new one in the old one's
// place, which the row records as not ended (or, for a later run, ends and
// starts again).
static bool expectRunning(const RunTakeoverRow& row, uint8_t bit) {
    if (row.event == RUN_TAKEOVER_LATER_RUN && bit == RUN_TAKEOVER_SEQUENCE) return true;  // replaced
    return (row.ends & bit) == 0;
}

// Pairs the tick cannot put on the droid together, each for a reason the
// Coordinator itself gives:
// - an estop clear or a waking needs the halt first, and the halt ends every
//   run but a staged ring close, which holds;
// - a pose and a flutter are refused while a back to centre sweeps or a
//   pose is reached, so neither is ever running beside the other.
static bool pairCanBeMet(RunTakeoverEvent event, uint8_t bit) {
    if (event == RUN_TAKEOVER_ESTOP_CLEAR || event == RUN_TAKEOVER_WAKING) {
        return bit == RUN_TAKEOVER_RING_CLOSE;
    }
    return true;
}

void test_every_row_ends_what_it_names_and_leaves_the_rest_running() {
    for (uint8_t e = 0; e < RUN_TAKEOVER_EVENT_COUNT; ++e) {
        const RunTakeoverRow& row = runTakeoverRow((RunTakeoverEvent)e);
        for (const RunUnderTest& run : kRuns) {
            if (!pairCanBeMet(row.event, run.bit)) continue;
            setUp();
            run.start();
            fire(row.event);
            char msg[96];
            snprintf(msg, sizeof(msg), "event %s, run bit 0x%02x", row.why, (unsigned)run.bit);
            const bool stillRunning = (running() & run.bit) != 0;
            TEST_ASSERT_EQUAL_MESSAGE(expectRunning(row, run.bit), stillRunning, msg);
        }
    }
}

void test_a_row_that_stages_a_ring_close_starts_one() {
    for (uint8_t e = 0; e < RUN_TAKEOVER_EVENT_COUNT; ++e) {
        const RunTakeoverRow& row = runTakeoverRow((RunTakeoverEvent)e);
        if (row.event == RUN_TAKEOVER_WAKING) continue;  // needs a sequence to have been ended: its own case
        setUp();
        fire(row.event);
        char msg[64];
        snprintf(msg, sizeof(msg), "event %s", row.why);
        TEST_ASSERT_EQUAL_MESSAGE(row.stagesRingClose, (running() & RUN_TAKEOVER_RING_CLOSE) != 0, msg);
    }
}

// -----------------------------------------------------------------------------
// How each ending is recorded
// -----------------------------------------------------------------------------

static void expectRecorded(RunTakeoverEvent event) {
    const RunTakeoverRow& row = runTakeoverRow(event);
    const SeqRunSummary s = lastRun();
    TEST_ASSERT_TRUE(s.valid);
    TEST_ASSERT_EQUAL_STRING("DM:VADER", s.name);
    TEST_ASSERT_EQUAL_UINT8(row.outcome, s.outcome);
    TEST_ASSERT_EQUAL_STRING(row.why, s.reason);
    TEST_ASSERT_FALSE_MESSAGE(robotState.domeSeqActive, "the suppression window went with the run");
}

void test_each_ending_of_a_sequence_is_recorded_with_its_rows_outcome_and_reason() {
    const RunTakeoverEvent endings[] = {
        RUN_TAKEOVER_POSE_PRESS, RUN_TAKEOVER_BACK_TO_CENTRE, RUN_TAKEOVER_STOP,
        RUN_TAKEOVER_ESTOP,      RUN_TAKEOVER_SLEEP_MODE,     RUN_TAKEOVER_DOME_RECONNECT,
    };
    for (RunTakeoverEvent e : endings) {
        setUp();
        startSequence("DM:VADER");
        TEST_ASSERT_TRUE(robotState.domeSeqActive);
        fire(e);
        TEST_ASSERT_FALSE(running() & RUN_TAKEOVER_SEQUENCE);
        expectRecorded(e);
    }
}

void test_sleep_mode_ends_a_running_sequence_as_aborted_for_sleep_mode() {
    startSequence("DM:VADER");
    robotState.sleepMode = true;
    tick();
    TEST_ASSERT_EQUAL_UINT8(0, running());
    const SeqRunSummary s = lastRun();
    TEST_ASSERT_EQUAL_UINT8(SEQ_RUN_ABORTED, s.outcome);
    TEST_ASSERT_EQUAL_STRING("sleep mode", s.reason);
}

void test_estop_and_sleep_together_record_the_estop() {
    startSequence("DM:VADER");
    robotState.sleepMode = true;
    robotState.estop = true;
    tick();
    TEST_ASSERT_EQUAL_UINT8(SEQ_RUN_ESTOP, lastRun().outcome);
}

// A preempting Learned run that cannot get its run buffers: the run before it
// stays ended, and neither its suppression window nor its name is left behind
// for a run that never started. The host's store commit always fails, which
// is exactly that case.
// The same load failing leaves a staged ring close running: what a later run
// ends beyond the sequence goes only once it has started.
void test_a_later_run_with_no_memory_leaves_a_staged_ring_close_running() {
    SeqIndexEntry learned = {};
    snprintf(learned.name, sizeof(learned.name), "%s", "DM:MINE");
    learned.valid = true;
    TEST_ASSERT_TRUE(seqStoreIndexAdd(learned));
    startRingClose();
    tick(10, "DM:MINE");
    TEST_ASSERT_TRUE(running() & RUN_TAKEOVER_RING_CLOSE);
}

void test_a_later_run_with_no_memory_leaves_nothing_running_and_no_suppression() {
    SeqIndexEntry learned = {};
    snprintf(learned.name, sizeof(learned.name), "%s", "DM:MINE");
    learned.valid = true;
    TEST_ASSERT_TRUE(seqStoreIndexAdd(learned));
    TEST_ASSERT_EQUAL_INT(SEQ_RUNTIME, (int)sequenceLookup("DM:MINE").kind);

    startSequence("DM:VADER");
    tick(10, "DM:MINE");
    TEST_ASSERT_EQUAL_UINT8(0, running());
    TEST_ASSERT_FALSE(robotState.domeSeqActive);
    const SeqRunSummary s = lastRun();
    TEST_ASSERT_EQUAL_STRING("DM:VADER", s.name);
    TEST_ASSERT_EQUAL_UINT8(SEQ_RUN_PREEMPTED, s.outcome);
}

// -----------------------------------------------------------------------------
// A run asked for under a halt
// -----------------------------------------------------------------------------

void test_a_run_asked_for_under_either_halt_is_refused_and_nothing_starts() {
    for (int halt = 0; halt < 2; ++halt) {
        setUp();
        startSequence("DM:HELLO");
        sequenceStopRequest();
        tick();
        (halt == 0 ? robotState.estop : robotState.sleepMode) = true;
        tick(10, "DM:VADER");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, running(), halt == 0 ? "estop" : "sleep mode");
        TEST_ASSERT_EQUAL_STRING("DM:HELLO", lastRun().name);  // nothing new was recorded
        TEST_ASSERT_FALSE(robotState.domeSeqActive);
    }
}

void test_the_refusal_says_why_in_words_for_a_run() {
    TEST_ASSERT_EQUAL_STRING("Estop latched. Clear it to run a sequence.", sequenceRunRefusal(true, false));
    TEST_ASSERT_EQUAL_STRING("Estop latched. Clear it to run a sequence.", sequenceRunRefusal(true, true));
    TEST_ASSERT_EQUAL_STRING("The droid is asleep. Wake it to run a sequence.",
                             sequenceRunRefusal(false, true));
    TEST_ASSERT_NULL(sequenceRunRefusal(false, false));
}

void test_only_a_body_owned_run_is_refused_now() {
    robotState.estop = true;
    TEST_ASSERT_NOT_NULL(sequenceRunRefusalNow("DM:VADER"));
    TEST_ASSERT_NULL(sequenceRunRefusalNow("DM:WAVE"));  // an alias: the dome runs it
    TEST_ASSERT_NULL(sequenceRunRefusalNow("DM:NOT_A_THING"));
    robotState.estop = false;
    TEST_ASSERT_NULL(sequenceRunRefusalNow("DM:VADER"));
}

// -----------------------------------------------------------------------------
// The staged ring close holds during a halt (operator, 2026-11-07)
// -----------------------------------------------------------------------------

static const unsigned kRingPanels = 7;

void test_a_staged_ring_close_sends_nothing_under_an_estop_and_closes_after_the_clear() {
    startRingClose();  // the first panel's close went as it was staged
    robotState.estop = true;
    for (int i = 0; i < 500; ++i) tickCountingCloses();  // five seconds held
    TEST_ASSERT_EQUAL_UINT(0, ringClosesSent);
    TEST_ASSERT_TRUE_MESSAGE(running() & RUN_TAKEOVER_RING_CLOSE, "still staged, only held");
    robotState.estop = false;
    for (int i = 0; i < 500; ++i) tickCountingCloses();
    // The estop clear stages it again from the first panel.
    TEST_ASSERT_EQUAL_UINT(kRingPanels, ringClosesSent);
    TEST_ASSERT_FALSE(running() & RUN_TAKEOVER_RING_CLOSE);
}

void test_a_staged_ring_close_holds_through_sleep_mode_and_closes_every_panel_on_waking() {
    startRingClose();  // the first panel's close went as it was staged
    robotState.sleepMode = true;
    for (int i = 0; i < 500; ++i) tickCountingCloses();
    TEST_ASSERT_EQUAL_UINT(0, ringClosesSent);
    robotState.sleepMode = false;
    for (int i = 0; i < 500; ++i) tickCountingCloses();
    // A nap with no run ended: the held close starts again from the first
    // panel, since the dome link may have dropped the one queued before.
    TEST_ASSERT_EQUAL_UINT(kRingPanels, ringClosesSent);
    TEST_ASSERT_FALSE(running() & RUN_TAKEOVER_RING_CLOSE);
}

void test_a_dome_reconnect_under_a_halt_stages_a_close_that_waits() {
    robotState.estop = true;
    tick();
    startRingClose();
    for (int i = 0; i < 500; ++i) tickCountingCloses();
    TEST_ASSERT_EQUAL_UINT(0, ringClosesSent);
    robotState.estop = false;
    for (int i = 0; i < 500; ++i) tickCountingCloses();
    TEST_ASSERT_EQUAL_UINT(kRingPanels, ringClosesSent);
}

void test_a_held_ring_close_lets_the_task_wait_at_the_slow_pace() {
    startRingClose();
    robotState.estop = true;
    tick();
    g_test_millis = now + 10;
    TEST_ASSERT_EQUAL_UINT32(250, sequenceDispatcherTick(nullptr, now + 10));
}

// -----------------------------------------------------------------------------
// A halt drops the ended run's own ring closes (operator, 2026-11-07)
// -----------------------------------------------------------------------------

// DM:HELLO opens ring panels on its first tick, so its cleanup owes their
// closes, one every 500 ms after it ends.
static void startARunThatOpensTheRing() {
    startSequence("DM:HELLO");
    ringClosesSent = 0;
}

void test_a_stopped_run_sends_its_own_ring_closes() {
    startARunThatOpensTheRing();
    sequenceStopRequest();
    for (int i = 0; i < 400; ++i) tickCountingCloses();
    TEST_ASSERT_TRUE_MESSAGE(ringClosesSent > 0, "the control: a Stop leaves the run's closes to go");
}

void test_an_estop_drops_the_ended_runs_ring_closes_and_its_clear_closes_the_ring() {
    startARunThatOpensTheRing();
    robotState.estop = true;
    for (int i = 0; i < 400; ++i) tickCountingCloses();
    TEST_ASSERT_EQUAL_UINT(0, ringClosesSent);
    TEST_ASSERT_EQUAL_UINT32(250, sequenceDispatcherTick(nullptr, now + 10));  // nothing left owed
    robotState.estop = false;
    for (int i = 0; i < 500; ++i) tickCountingCloses();
    TEST_ASSERT_EQUAL_UINT(kRingPanels, ringClosesSent);
}

void test_sleep_mode_drops_the_ended_runs_ring_closes_and_waking_closes_the_ring() {
    startARunThatOpensTheRing();
    robotState.sleepMode = true;
    for (int i = 0; i < 400; ++i) tickCountingCloses();
    TEST_ASSERT_EQUAL_UINT(0, ringClosesSent);
    robotState.sleepMode = false;
    for (int i = 0; i < 500; ++i) tickCountingCloses();
    TEST_ASSERT_EQUAL_UINT(kRingPanels, ringClosesSent);
}

void test_a_halt_drops_the_closes_of_a_run_already_ended_by_a_stop() {
    startARunThatOpensTheRing();
    sequenceStopRequest();
    tickCountingCloses();
    robotState.estop = true;
    const unsigned before = ringClosesSent;
    for (int i = 0; i < 400; ++i) tickCountingCloses();
    TEST_ASSERT_EQUAL_UINT(before, ringClosesSent);
}

// The halt lands on the very tick one of the stopped run's closes falls due:
// it is dropped with the rest, not sent first.
void test_a_close_due_on_the_tick_a_halt_lands_is_dropped_not_sent() {
    startARunThatOpensTheRing();
    sequenceStopRequest();
    tickCountingCloses();
    const unsigned before = ringClosesSent;
    robotState.estop = true;
    tickCountingCloses(600);  // past the first close's 500 ms
    for (int i = 0; i < 300; ++i) tickCountingCloses();
    TEST_ASSERT_EQUAL_UINT(before, ringClosesSent);
}

// A close the dome queue turned away is held for a retry; a halt drops it too.
void test_a_close_held_on_a_full_dome_queue_is_dropped_by_a_halt() {
    startARunThatOpensTheRing();
    sequenceStopRequest();
    tickCountingCloses();
    g_test_dome_tx_ok = false;
    for (int i = 0; i < 60; ++i) tickCountingCloses();  // the first close falls due and is held
    const unsigned before = ringClosesSent;
    g_test_dome_tx_ok = true;
    robotState.estop = true;
    for (int i = 0; i < 300; ++i) tickCountingCloses();
    TEST_ASSERT_EQUAL_UINT(before, ringClosesSent);
}

// A dome reconnect stages its own close of every ring panel, so the ended
// run's closes are dropped: one close sequence, seven closes in all.
void test_a_dome_reconnect_runs_one_close_sequence_not_two() {
    startARunThatOpensTheRing();
    g_test_dome_connected = false;
    tickCountingCloses();
    g_test_dome_connected = true;
    for (int i = 0; i < 600; ++i) tickCountingCloses();
    TEST_ASSERT_EQUAL_UINT(kRingPanels, ringClosesSent);
}

// A halt that lands while a Learned run loads: the run is refused once the
// load is done, before it ends what is running, so the stopped run's close
// that falls due on that tick is dropped by the halt, never drained first.
void test_a_halt_that_lands_during_a_load_refuses_the_run_and_sends_no_close() {
    SeqIndexEntry learned = {};
    snprintf(learned.name, sizeof(learned.name), "%s", "DM:MINE");
    learned.valid = true;
    TEST_ASSERT_TRUE(seqStoreIndexAdd(learned));
    startARunThatOpensTheRing();
    sequenceStopRequest();
    tickCountingCloses();
    const unsigned before = ringClosesSent;
    g_test_seq_prepare_latches_estop = true;
    now += 600;  // the first of the stopped run's closes is due
    g_test_millis = now;
    const unsigned txBefore = g_test_dome_tx_calls;
    SequenceRequest req = {};
    snprintf(req.name, sizeof(req.name), "%s", "DM:MINE");
    req.src = SRC_WEB_API;
    sequenceDispatcherTick(&req, now);
    if (g_test_dome_tx_calls != txBefore && strncmp(g_test_dome_last_tx, ":CL", 3) == 0) ringClosesSent++;
    for (int i = 0; i < 300; ++i) tickCountingCloses();
    TEST_ASSERT_EQUAL_UINT(before, ringClosesSent);
    TEST_ASSERT_EQUAL_STRING("DM:HELLO", lastRun().name);  // the refused run was never recorded
}

// -----------------------------------------------------------------------------
// The dome link's own guard (include/dome_tx_halt.h)
// -----------------------------------------------------------------------------

void test_the_coordinator_queues_its_dome_commands_as_its_own() {
    startRingClose();
    TEST_ASSERT_EQUAL_STRING_LEN(":CL", g_test_dome_last_tx, 3);
    TEST_ASSERT_EQUAL_UINT8(DOME_TX_COORDINATOR, g_test_dome_last_tx_origin);
    TEST_ASSERT_TRUE(sequenceStart("DM:WAVE", SRC_WEB_API));  // an alias, forwarded for a person
    TEST_ASSERT_EQUAL_STRING(":SE02", g_test_dome_last_tx);
    TEST_ASSERT_EQUAL_UINT8(DOME_TX_OTHER, g_test_dome_last_tx_origin);
}

void test_the_dome_link_drops_a_coordinator_panel_command_under_either_halt() {
    static const char* const panels[] = {":CL01", ":OP02", ":OF03", ":SE04", ":MV05"};
    for (const char* cmd : panels) {
        TEST_ASSERT_TRUE_MESSAGE(domeTxHeldByHalt(DOME_TX_COORDINATOR, cmd, true, false), cmd);
        TEST_ASSERT_TRUE_MESSAGE(domeTxHeldByHalt(DOME_TX_COORDINATOR, cmd, false, true), cmd);
        TEST_ASSERT_FALSE_MESSAGE(domeTxHeldByHalt(DOME_TX_COORDINATOR, cmd, false, false), cmd);
        TEST_ASSERT_FALSE_MESSAGE(domeTxHeldByHalt(DOME_TX_OTHER, cmd, true, true), cmd);
    }
}

void test_the_dome_link_still_sends_the_coordinators_light_resets_under_a_halt() {
    for (const char* reset : SEQ_DOME_VISUAL_RESETS) {
        TEST_ASSERT_FALSE_MESSAGE(domeTxHeldByHalt(DOME_TX_COORDINATOR, reset, true, true), reset);
    }
    TEST_ASSERT_FALSE(domeTxHeldByHalt(DOME_TX_COORDINATOR, "DV:RESET_VISUALS", true, false));
    TEST_ASSERT_FALSE(domeTxHeldByHalt(DOME_TX_COORDINATOR, ":", true, false));
    TEST_ASSERT_FALSE(domeTxHeldByHalt(DOME_TX_COORDINATOR, nullptr, true, false));
}

// -----------------------------------------------------------------------------
// Waking
// -----------------------------------------------------------------------------

void test_waking_after_sleep_mode_ended_a_run_closes_the_ring_and_resets_the_visuals() {
    startSequence("DM:VADER");
    robotState.sleepMode = true;
    tick();
    g_test_dome_tx_calls = 0;
    for (int i = 0; i < 300; ++i) tickCountingCloses();
    TEST_ASSERT_EQUAL_UINT(0, g_test_dome_tx_calls);  // asleep: nothing to the dome
    robotState.sleepMode = false;
    tickCountingCloses();
    TEST_ASSERT_TRUE(running() & RUN_TAKEOVER_RING_CLOSE);
    for (int i = 0; i < 500; ++i) tickCountingCloses();
    TEST_ASSERT_EQUAL_UINT(kRingPanels, ringClosesSent);
    // Each visual reset went too, beside the closes.
    TEST_ASSERT_TRUE(g_test_dome_tx_calls > kRingPanels);
}

void test_a_nap_with_no_run_moves_nothing_on_waking() {
    robotState.sleepMode = true;
    tick();
    robotState.sleepMode = false;
    for (int i = 0; i < 300; ++i) tick();
    TEST_ASSERT_EQUAL_UINT(0, g_test_dome_tx_calls);
    TEST_ASSERT_EQUAL_UINT8(0, running());
}

void test_a_second_nap_after_the_close_moves_nothing() {
    startSequence("DM:VADER");
    robotState.sleepMode = true;
    tick();
    robotState.sleepMode = false;
    for (int i = 0; i < 500; ++i) tick();
    g_test_dome_tx_calls = 0;
    robotState.sleepMode = true;
    tick();
    robotState.sleepMode = false;
    for (int i = 0; i < 300; ++i) tick();
    TEST_ASSERT_EQUAL_UINT(0, g_test_dome_tx_calls);
}

// -----------------------------------------------------------------------------
// The table itself
// -----------------------------------------------------------------------------

void test_every_event_has_a_reason_and_a_row_ending_a_sequence_records_an_outcome() {
    for (uint8_t e = 0; e < RUN_TAKEOVER_EVENT_COUNT; ++e) {
        const RunTakeoverRow& row = runTakeoverRow((RunTakeoverEvent)e);
        TEST_ASSERT_NOT_NULL(row.why);
        TEST_ASSERT_TRUE(row.why[0] != '\0');
        const bool endsSequence = (row.ends & RUN_TAKEOVER_SEQUENCE) != 0;
        TEST_ASSERT_EQUAL(endsSequence, row.outcome != SEQ_RUN_NONE);
    }
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_every_row_ends_what_it_names_and_leaves_the_rest_running);
    RUN_TEST(test_a_row_that_stages_a_ring_close_starts_one);
    RUN_TEST(test_each_ending_of_a_sequence_is_recorded_with_its_rows_outcome_and_reason);
    RUN_TEST(test_sleep_mode_ends_a_running_sequence_as_aborted_for_sleep_mode);
    RUN_TEST(test_estop_and_sleep_together_record_the_estop);
    RUN_TEST(test_a_later_run_with_no_memory_leaves_nothing_running_and_no_suppression);
    RUN_TEST(test_a_later_run_with_no_memory_leaves_a_staged_ring_close_running);
    RUN_TEST(test_a_run_asked_for_under_either_halt_is_refused_and_nothing_starts);
    RUN_TEST(test_the_refusal_says_why_in_words_for_a_run);
    RUN_TEST(test_only_a_body_owned_run_is_refused_now);
    RUN_TEST(test_a_staged_ring_close_sends_nothing_under_an_estop_and_closes_after_the_clear);
    RUN_TEST(test_a_staged_ring_close_holds_through_sleep_mode_and_closes_every_panel_on_waking);
    RUN_TEST(test_a_dome_reconnect_under_a_halt_stages_a_close_that_waits);
    RUN_TEST(test_a_held_ring_close_lets_the_task_wait_at_the_slow_pace);
    RUN_TEST(test_a_stopped_run_sends_its_own_ring_closes);
    RUN_TEST(test_an_estop_drops_the_ended_runs_ring_closes_and_its_clear_closes_the_ring);
    RUN_TEST(test_sleep_mode_drops_the_ended_runs_ring_closes_and_waking_closes_the_ring);
    RUN_TEST(test_a_halt_drops_the_closes_of_a_run_already_ended_by_a_stop);
    RUN_TEST(test_a_close_due_on_the_tick_a_halt_lands_is_dropped_not_sent);
    RUN_TEST(test_a_close_held_on_a_full_dome_queue_is_dropped_by_a_halt);
    RUN_TEST(test_a_dome_reconnect_runs_one_close_sequence_not_two);
    RUN_TEST(test_a_halt_that_lands_during_a_load_refuses_the_run_and_sends_no_close);
    RUN_TEST(test_the_coordinator_queues_its_dome_commands_as_its_own);
    RUN_TEST(test_the_dome_link_drops_a_coordinator_panel_command_under_either_halt);
    RUN_TEST(test_the_dome_link_still_sends_the_coordinators_light_resets_under_a_halt);
    RUN_TEST(test_waking_after_sleep_mode_ended_a_run_closes_the_ring_and_resets_the_visuals);
    RUN_TEST(test_a_nap_with_no_run_moves_nothing_on_waking);
    RUN_TEST(test_a_second_nap_after_the_close_moves_nothing);
    RUN_TEST(test_every_event_has_a_reason_and_a_row_ending_a_sequence_records_an_outcome);
    return UNITY_END();
}
