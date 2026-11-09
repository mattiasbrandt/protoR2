// =============================================================================
// include/dome_rotation_backend.h
//
// The Dome Rotation backend seam (#504), shaped like the Foot Drive's
// (include/drive_backend.h, #339).
//
// WHERE THE LINE IS. Everything a Dome Rotation command passes through before
// it gets here is generic, and no backend can reach it:
//
//   - the 0.5 s silence stop, and a stop that never floats
//   - a stop on the latching estop and in Sleep Mode, and the queue they
//     discard while they hold
//   - the 30 s cap on a timed turn from a sender
//   - the speed limit, random moves, and the queue and its senders
//   - the Dome Bearing and its turn plan (include/dome_bearing.h)
//
// All of those live in the Dome Rotation task (src/tasks/dome_task.cpp). A
// backend gets a signed speed and the speed limit, and puts them on its wire;
// what it declares about itself is data, in its profile row below.
//
// WHAT COMES BACK. Every write reports what the backend put on the wire as a
// signed share of its FULL output (-1..1, after the speed limit), plus the
// pulse where it has one. The Dome Bearing integrates the share, never a
// pulse, so a backend that speaks packets instead of pulses can feed it.
//
// Below the line, per product: how a speed becomes a wire signal, the
// start-up rule, and what "stop" is on the wire.
// =============================================================================
#pragma once

#include <stdint.h>

#include "component_registry.h"  // componentPartExists(), componentPartProtocol()
#include "config_store.h"        // DomeConfig - the settings a backend reads

// What "stop" is on the wire.
enum class DomeRotationStop : uint8_t {
    // The neutral pulse, and the line is never released. A released RC line
    // carries no pulse at all, which an RC ESC reads as a lost receiver
    // rather than as a stop, so a Dome ESC's stop is a pulse like any other.
    NeutralPulseHeld = 0,
};

// One catalogue row per Dome Rotation backend, carrying its own rules.
// Adding a Dome Rotation product is adding a row: the start-up rule and what a
// stop is live here, never in the Dome Rotation task.
struct DomeRotationBackendProfile {
    const char* id;             // Component Member identifier
    const char* protocol;       // Component Protocol spoken on the wire
    uint16_t startupNeutralMs;  // stop held this long at start-up, before the first turn
    DomeRotationStop stop;      // what a stop puts on the wire
    bool reportsPulse;          // false -> DomeRotationOutput::pulseUs is always 0
};

// The ISDT ESC70's Component Registry id, named once so the profile's id and
// the row its protocol is read from cannot be two different spellings.
inline constexpr char kIsdtEsc70RegistryId[] = "isdt_esc70";
static_assert(componentPartExists(kIsdtEsc70RegistryId),
              "the Dome Rotation backend cites a product id no Component Registry row declares");

// A Dome ESC: one standard RC pulse on PIN_DOME_ESC, through the LEDC channel
// the servo outputs share (src/drivers/dome_rotation_backend_rc_pwm.cpp). The
// ISDT ESC70 is the row; a SyRen 10 in R/C mode speaks the same signal. The
// 2000 ms neutral hold is the ESC70's own start-up rule (ISDT FAQ: "After each
// startup, return throttle to center position and maintain for two seconds to
// clear the error"; docs/spec-sheets/isdt-esc70-dome-esc.md).
//
// Only one row exists, so it is selected unconditionally. A second Dome
// Rotation product - #309's SyRen 10 on packet serial - adds its own row here
// and the selection between them (a Board Capability Gate, as
// include/drive_backend.h selects its row, or the stored Component Member),
// with its own start-up rule (an autobaud byte) and its own stop.
inline constexpr DomeRotationBackendProfile kDomeRotationBackend = {
    .id = kIsdtEsc70RegistryId,
    .protocol = componentPartProtocol(kIsdtEsc70RegistryId),
    .startupNeutralMs = 2000,
    .stop = DomeRotationStop::NeutralPulseHeld,
    .reportsPulse = true,
};

// What a backend put on the wire.
struct DomeRotationOutput {
    float share;       // signed share of the backend's full output, -1..1; 0 is a stop
    uint16_t pulseUs;  // the RC pulse on the wire; 0 where the profile reports no pulse
};

#ifdef ARDUINO_ARCH_ESP32
// domeRotationBackendStart()
// The start-up rule: what the backend's far end needs before its first turn.
// For a Dome ESC, the stop on the wire, held for the profile's
// startupNeutralMs. Blocks for that long: call once from setup(), before the
// Dome Rotation task's loop starts.
void domeRotationBackendStart(const DomeConfig& cfg);

// domeRotationBackendWrite()
// Put one signed speed (-1..1, clamped) on the wire, scaled by the speed limit
// (0..100 %), and say what went out. A leaf: no log, no wait, no heap; it runs
// on the Dome Rotation task's Core 1 loop.
// thread-safe: must only be called from the Dome Rotation task.
DomeRotationOutput domeRotationBackendWrite(const DomeConfig& cfg, float speed,
                                            uint8_t speedLimitPct);

// domeRotationBackendStop()
// Put the profile's stop on the wire, and say what went out (share 0).
// thread-safe: must only be called from the Dome Rotation task.
DomeRotationOutput domeRotationBackendStop(const DomeConfig& cfg);
#endif
