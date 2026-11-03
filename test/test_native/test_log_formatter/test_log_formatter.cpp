// =============================================================================
// test/test_native/test_log_formatter/test_log_formatter.cpp
//
// #491: PA_LOG_* lines are formatted by one out-of-line paLogf()
// (src/logging.cpp) instead of an snprintf inlined at every call site. A log
// line must read the same on the wire as before: same "[millis][L][tag] "
// prefix, level letter, tag, text, and the same 255-character cap. The
// reference each case compares against is the inline format the macros used
// to expand to, written out here.
// =============================================================================

#include <unity.h>

#include <stdio.h>
#include <string.h>

#include "log_buffer.h"
#include "log_buffer_test_hooks.h"
#include "logging.h"

#include "drive_motion_test_hooks.h"  // g_test_millis, the settable millis() clock

// Empty the sink ring and hand the wire to the "Console", so a logged line is
// read back through the drain the Console uses, and nothing goes to the stub
// serial port.
static void resetSink(void) {
    logBufferInit(&g_test_log_sink_buffer, g_test_log_sink_storage, LOG_RING_MAX_LINES);
    paLogWireBindToConsole();
}

static void drainOne(char* out, size_t outSize) {
    uint32_t evicted = 0;
    TEST_ASSERT_TRUE_MESSAGE(paLogDrainNextLine(out, outSize, &evicted), "no line was logged");
    TEST_ASSERT_EQUAL_UINT32(0, evicted);
}

void setUp(void) {
    g_test_millis = 0;
    resetSink();
}

void tearDown(void) {}

static void test_a_line_reads_prefix_level_tag_and_text(void) {
    g_test_millis = 123456;
    paLogf('W', "Sbus", "lost %d frames on %s", 3, "UART2");

    char got[PA_LOG_SERIAL_LINE_MAX];
    drainOne(got, sizeof(got));
    TEST_ASSERT_EQUAL_STRING("[123456][W][Sbus] lost 3 frames on UART2", got);
}

static void test_each_macro_passes_its_level_letter(void) {
    // configCurrentLogLevel() in the native stubs is PA_LOG_LEVEL (4, debug),
    // so every level passes.
    PA_LOG_ERROR("T", "e");
    PA_LOG_WARN("T", "w");
    PA_LOG_INFO("T", "i");
    PA_LOG_DEBUG("T", "d");

    char got[PA_LOG_SERIAL_LINE_MAX];
    const char* expected[] = {"[0][E][T] e", "[0][W][T] w", "[0][I][T] i", "[0][D][T] d"};
    for (const char* line : expected) {
        drainOne(got, sizeof(got));
        TEST_ASSERT_EQUAL_STRING(line, got);
    }
}

// Every slot the drain gives back, joined with '|' so a slot boundary is part
// of what is compared: a line longer than one slot continues into the next.
static void drainAll(char* out, size_t outSize) {
    out[0] = '\0';
    char slot[PA_LOG_SERIAL_LINE_MAX];
    uint32_t evicted = 0;
    while (paLogDrainNextLine(slot, sizeof(slot), &evicted)) {
        TEST_ASSERT_EQUAL_UINT32(0, evicted);
        strncat(out, slot, outSize - strlen(out) - 1);
        strncat(out, "|", outSize - strlen(out) - 1);
    }
}

static void test_a_long_line_is_cut_where_the_inline_format_cut_it(void) {
    g_test_millis = 42;
    char message[400];
    memset(message, 'x', sizeof(message) - 1);
    message[sizeof(message) - 1] = '\0';

    // The macros' old expansion: one snprintf of prefix and message together,
    // cut at 255 characters, handed to paLogLine().
    char reference[PA_LOG_SERIAL_LINE_MAX];
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
    snprintf(reference, sizeof(reference), "[%lu][I][%s] %s", 42UL, "Web", message);
#pragma GCC diagnostic pop
    TEST_ASSERT_EQUAL_size_t(PA_LOG_SERIAL_LINE_MAX - 1, strlen(reference));
    paLogLine(reference);
    char before[4 * PA_LOG_SERIAL_LINE_MAX];
    drainAll(before, sizeof(before));

    paLogf('I', "Web", "%s", message);
    char after[4 * PA_LOG_SERIAL_LINE_MAX];
    drainAll(after, sizeof(after));

    TEST_ASSERT_EQUAL_STRING(before, after);
    int slots = 0;  // the case does span slots, so the cut is the one compared
    for (const char* p = before; *p != '\0'; p++) {
        slots += *p == '|';
    }
    TEST_ASSERT_GREATER_THAN_INT(1, slots);
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_a_line_reads_prefix_level_tag_and_text);
    RUN_TEST(test_each_macro_passes_its_level_letter);
    RUN_TEST(test_a_long_line_is_cut_where_the_inline_format_cut_it);
    return UNITY_END();
}
