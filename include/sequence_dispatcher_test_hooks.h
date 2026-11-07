// =============================================================================
// include/sequence_dispatcher_test_hooks.h
//
// The Sequence Coordinator's task in two halves, so a native test can drive it
// one tick at a time (#502). sequenceDispatcherTask() calls
// sequenceDispatcherBegin() once, then waits on sequenceQueue and calls
// sequenceDispatcherTick() on every wake. The native queue stub never delivers
// a request (test/stubs/include/freertos/queue.h), so a test hands the tick
// its request itself; everything else the tick reads from RobotState and the
// existing stubs, as the task does.
//
// Declared here rather than as an `extern` in a test, the shape the other
// *_test_hooks.h headers keep.
// =============================================================================
#pragma once

#include <stdint.h>

#include "sequence_dispatcher.h"  // SequenceRequest

// Everything the Coordinator holds put back to power-up, and the boot pass
// started, or refused under either halt.
void sequenceDispatcherBegin();

// One tick: `request` is the run asked for on this wake, or nullptr. Returns
// how long the task may wait for the next request, in ms.
uint32_t sequenceDispatcherTick(const SequenceRequest* request, uint32_t now);

// What the Coordinator is doing now, as RunTakeoverRun bits
// (include/sequence_run_takeover.h). Read-only. A sequence that has ended and
// is still sending its cleanup does not count as running.
uint8_t sequenceDispatcherRunning();
