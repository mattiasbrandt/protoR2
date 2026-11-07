// =============================================================================
// test/test_native/test_dispatch_cores/test_dispatch_cores.cpp
//
// The Dispatch Cores behind the action Operations (ADR 0036): every state
// combination is proved here, once. The web route suites and the Console suite
// keep one test per outcome, proving how each adapter words it.
// =============================================================================
#include <unity.h>

#include <freertos/queue.h>

#include "api_drive.h"
#include "config_cache.h"
#include "drive_arbiter.h"
#include "drive_arbiter_test_hooks.h"  // driveArbiterReset()
#include "robot_state.h"
#include "config_write_window_test_hooks.h"  // ConfigWriteWindowForTest

extern unsigned long g_test_millis;

namespace {

DriveOutput resolvedDriveOutput() {
    ConfigSnapshot snap = {};
    configCacheRead(&snap);
    DriveArbiterConfig cfg = {};
    cfg.speedLimitMax = snap.drive.speedLimitMax;
    cfg.webDriveTimeoutMs = 60000;
    return driveArbiterResolve(cfg, millis());
}

void setSavedConfig(int16_t speedLimitMax, bool domeSaved) {
    ConfigSnapshot snap = {};
    configCacheRead(&snap);
    snap.drive.speedLimitMax = speedLimitMax;
    snap.system.enable_dome_esc = domeSaved;
    const ConfigWriteWindowForTest seed;
    configCacheReplace(snap);
}

}  // namespace

void setUp() {
    // Non-zero: the arbiter reads timestamp 0 as "never submitted".
    g_test_millis = 1000;
    robotState = {};
    driveArbiterInit(&robotStateMux);
    driveArbiterReset();
    setSavedConfig(300, true);
    configCacheSetActiveDomeEnabled(true);
    testQueueSendResult() = pdTRUE;
}

void tearDown() {
    testQueueSendResult() = pdTRUE;
}

// --- driveMoveRequest ---------------------------------------------------------

// Every combination of the four inputs the gate reads. A move goes through when
// nothing holds it: no estop, not stationary, and either a healthy RC link or
// Non-RC Control.
void test_drive_gate_over_every_state() {
    for (int bits = 0; bits < 32; ++bits) {
        const bool estop = bits & 1;
        const bool stationary = bits & 2;
        const bool signalLost = bits & 4;
        const bool hwFailsafe = bits & 8;
        const bool nonRcControl = bits & 16;
        robotState = {};
        driveArbiterReset();
        robotState.estop = estop;
        robotState.stationary = stationary;
        robotState.sbusSignalLost = signalLost;
        robotState.sbusHwFailsafe = hwFailsafe;
        robotState.webControlEnabled = nonRcControl;

        const bool allowed = !estop && !stationary && (nonRcControl || (!signalLost && !hwFailsafe));
        const DriveMoveOutcome outcome = driveMoveRequest(100, -50);

        TEST_ASSERT_EQUAL_MESSAGE(allowed ? DriveMoveOutcome::Submitted : DriveMoveOutcome::Blocked,
                                  outcome, "the gate's answer");
        TEST_ASSERT_EQUAL_INT16_MESSAGE(allowed ? 100 : 0, resolvedDriveOutput().speed,
                                        "a blocked move submits nothing");
        TEST_ASSERT_EQUAL_INT16_MESSAGE(allowed ? -50 : 0, resolvedDriveOutput().steer,
                                        "a blocked move submits nothing");
    }
}

void test_drive_clamps_both_axes_to_the_saved_limit() {
    setSavedConfig(200, true);

    TEST_ASSERT_EQUAL(DriveMoveOutcome::Submitted, driveMoveRequest(900, -900));

    TEST_ASSERT_EQUAL_INT16(200, resolvedDriveOutput().speed);
    TEST_ASSERT_EQUAL_INT16(-200, resolvedDriveOutput().steer);
}

// --- domeMoveRequest ----------------------------------------------------------

void test_dome_move_queues_when_awake_and_on_this_boot() {
    TEST_ASSERT_EQUAL(DomeMoveOutcome::Queued, domeMoveRequest(0.5f, SRC_WEB_API));
}

void test_dome_move_is_refused_asleep_before_anything_else() {
    robotState.sleepMode = true;
    configCacheSetActiveDomeEnabled(false);
    testQueueSendResult() = pdFALSE;

    TEST_ASSERT_EQUAL(DomeMoveOutcome::Asleep, domeMoveRequest(0.5f, SRC_WEB_API));
}

// Saved on since boot, not restarted: no DomeTask reads the queue, so "queued"
// would be a move that never happens (ADR 0027).
void test_dome_move_is_refused_when_off_this_boot_though_saved_on() {
    setSavedConfig(300, true);
    configCacheSetActiveDomeEnabled(false);

    TEST_ASSERT_EQUAL(DomeMoveOutcome::DomeOff, domeMoveRequest(0.5f, SRC_WEB_API));
}

// Saved off since boot, not restarted: DomeTask still runs until the restart.
void test_dome_move_queues_when_on_this_boot_though_saved_off() {
    setSavedConfig(300, false);
    configCacheSetActiveDomeEnabled(true);

    TEST_ASSERT_EQUAL(DomeMoveOutcome::Queued, domeMoveRequest(0.5f, SRC_WEB_API));
}

void test_dome_move_says_when_the_queue_is_full() {
    testQueueSendResult() = pdFALSE;

    TEST_ASSERT_EQUAL(DomeMoveOutcome::QueueFull, domeMoveRequest(0.5f, SRC_WEB_API));
}

// --- executeManualCommand ------------------------------------------------------

void test_manual_command_sleep_holds_every_held_prefix() {
    const char* const held[] = {"$001", ":SE01", "#xx", "*ST00", "@1T1", "%T", "&1", "!x"};
    robotState.sleepMode = true;
    for (const char* line : held) {
        TEST_ASSERT_EQUAL_MESSAGE(ManualCommandResult::Asleep, executeManualCommand(line), line);
    }
}

void test_manual_command_sleep_does_not_hold_a_keyword() {
    robotState.sleepMode = true;
    TEST_ASSERT_EQUAL(ManualCommandResult::Unsupported, executeManualCommand("not_a_keyword"));
    TEST_ASSERT_EQUAL(ManualCommandResult::Applied, executeManualCommand("disable_web_control"));
}

void test_manual_command_awake_is_not_held() {
    TEST_ASSERT_NOT_EQUAL(ManualCommandResult::Asleep, executeManualCommand("*ST00"));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_drive_gate_over_every_state);
    RUN_TEST(test_drive_clamps_both_axes_to_the_saved_limit);
    RUN_TEST(test_dome_move_queues_when_awake_and_on_this_boot);
    RUN_TEST(test_dome_move_is_refused_asleep_before_anything_else);
    RUN_TEST(test_dome_move_is_refused_when_off_this_boot_though_saved_on);
    RUN_TEST(test_dome_move_queues_when_on_this_boot_though_saved_off);
    RUN_TEST(test_dome_move_says_when_the_queue_is_full);
    RUN_TEST(test_manual_command_sleep_holds_every_held_prefix);
    RUN_TEST(test_manual_command_sleep_does_not_hold_a_keyword);
    RUN_TEST(test_manual_command_awake_is_not_held);
    return UNITY_END();
}
