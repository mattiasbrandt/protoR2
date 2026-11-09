// =============================================================================
// src/drivers/dome_rotation_backend_rc_pwm.cpp
//
// The Dome ESC backend: the seam in include/dome_rotation_backend.h wired to
// one RC pulse on the dome's LEDC channel.
//
// This translation unit is the only place in the Dome Rotation path that knows
// the dome is turned by a pulse. The Dome Rotation task above it decides what
// to send and when; the estop, Sleep Mode, the silence stop and the 30 s cap
// are settled before it calls in here.
// =============================================================================

#include "dome_rotation_backend.h"

#include <Arduino.h>

#include "dome_math.h"  // domeSpeedToPulseUs(), domePulseToShare()
#include "ledc_pwm.h"

void domeRotationBackendStart(const DomeConfig& cfg) {
    ledcPwmSetPulseWidth(LEDC_CH_DOME, cfg.dome_neutral_us);
    delay(kDomeRotationBackend.startupNeutralMs);
}

DomeRotationOutput domeRotationBackendWrite(const DomeConfig& cfg, float speed,
                                            uint8_t speedLimitPct) {
    const uint16_t pulseUs = domeSpeedToPulseUs(speed, cfg.dome_neutral_us, cfg.dome_min_pulse_us,
                                                cfg.dome_max_pulse_us, speedLimitPct);
    ledcPwmSetPulseWidth(LEDC_CH_DOME, pulseUs);
    return {domePulseToShare(pulseUs, cfg.dome_neutral_us, cfg.dome_min_pulse_us,
                             cfg.dome_max_pulse_us),
            pulseUs};
}

DomeRotationOutput domeRotationBackendStop(const DomeConfig& cfg) {
    ledcPwmSetPulseWidth(LEDC_CH_DOME, cfg.dome_neutral_us);
    return {0.0f, cfg.dome_neutral_us};
}
