// =============================================================================
// src/logging.cpp
//
// paLogf(): the one place a PA_LOG_* line is formatted (#491).
// =============================================================================
#include "logging.h"

#include <stdarg.h>
#include <stdio.h>

// The line buffer lives in this frame, and only while a line is written. When
// the macros formatted inline, every function with a log line carried these
// 256 B in its own frame under every call it made (#490 measured 270-290 B a
// frame). noinline keeps it that way: inlined, the buffer would be back in
// the caller's frame.
//
// The output is the line the inline snprintf made: "[millis][L][tag] " and
// then the message, cut at PA_LOG_SERIAL_LINE_MAX - 1 characters. The prefix
// is formatted first and the message after it into what is left, which
// truncates at the same character a single snprintf of both would.
__attribute__((noinline)) void paLogf(char level, const char* tag, const char* fmt, ...) {
    char line[PA_LOG_SERIAL_LINE_MAX];
    int prefix = snprintf(line, sizeof(line), "[%lu][%c][%s] ", (unsigned long)millis(), level, tag);
    if (prefix < 0) {
        return;
    }
    if (static_cast<size_t>(prefix) < sizeof(line) - 1) {
        va_list args;
        va_start(args, fmt);
        vsnprintf(line + prefix, sizeof(line) - static_cast<size_t>(prefix), fmt, args);
        va_end(args);
    }
    paLogLine(line);
}
