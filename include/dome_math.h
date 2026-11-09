// =============================================================================
// include/dome_math.h
//
// Pure-logic Dome ESC pulse mapping, the Pause before reversing decision and
// random-move pause arithmetic  --  no hardware, no FreeRTOS.
// Extracted for testability. The pulse mapping is the Dome ESC backend's
// (src/drivers/dome_rotation_backend_rc_pwm.cpp); the reversing pause and the
// random-move arithmetic are DomeRotationTask's. The mapping and the random
// moves are exercised by native tests.
//
// Dome ESC pulse semantics (standard RC PWM, 50 Hz):
//   1000us = full reverse in a forward-and-reverse running mode; in a mode
//            with a brake, a reverse from forward brakes first
//   1500us = neutral / stop
//   2000us = full forward
// =============================================================================
#pragma once
#include <stdint.h>

// -----------------------------------------------------------------------------
// The bounds every Dome ESC pulse Setting (neutral, min, max) is held to, at
// the config door and on load (src/config_settings.cpp). With the pulses in
// order, no half-range can be wider than these two apart: 1,000 us. The
// status document leans on that (src/web/status_json.cpp): a pulse moves in
// whole us, so one us past neutral - or past Smallest turn, when one is set -
// is at least 0.1 % of a half-range, and one decimal never prints it as 0.0,
// which reads as Stopped. A pulse exactly AT Smallest turn reads 0.0 on
// purpose: Smallest turn is set where the dome starts to move, so the Dome
// Bearing counts nothing there (domePulseToShare()).
// -----------------------------------------------------------------------------
inline constexpr uint16_t kDomePulseSettingMinUs = 1000;
inline constexpr uint16_t kDomePulseSettingMaxUs = 2000;

// -----------------------------------------------------------------------------
// domePulsesInOrder()
// The one rule a dome ESC pulse set must keep: min <= neutral <= max.
//
// Out of order, the mapping below cannot put neutral out for a stop. With min
// 1800, neutral 1500 and max 1200, speed 0 computes 1500, is raised to min
// (1800) and then lowered to max: every speed, 0 included, drives 1200 us -
// about 60% reverse on an ESC in a forward/reverse mode, with nobody
// commanding it (#417). So the config door refuses such a set, the loader
// replaces a stored one with the defaults, and the mapping falls back to
// neutral rather than trust it. data/dome.js sends what the builder typed and
// leaves this rule to the droid, whose refusal it shows.
// -----------------------------------------------------------------------------
inline bool domePulsesInOrder(uint16_t minPulseUs, uint16_t neutralUs, uint16_t maxPulseUs) {
    return minPulseUs <= neutralUs && neutralUs <= maxPulseUs;
}

// -----------------------------------------------------------------------------
// domeSpeedToPulseUs()
// Map normalized speed (-1.0..1.0) to ESC PWM pulse width (us).
//
// The speed limit percentage scales the usable pulse range symmetrically around
// neutral. Asymmetric neutral trimming (neutral != midpoint of min..max) is
// handled correctly: forward and reverse half-ranges are computed independently.
//
// Smallest turn (#504): an ESC ignores pulses near neutral, and the width of
// that dead band is not published (ISDT ESC70 spec sheet, section 6.4), so a
// slow turn can put a pulse out that moves nothing. With `smallestTurnUs` set,
// every non-zero speed starts that far from neutral and grows from there to the
// limited end: neutral +/- (offset + |speed| x (limited half-range - offset)).
// Speed 0 is still exactly neutral. The offset is clamped to the limited
// half-range on its side, so it never pushes a pulse past the speed limit; a
// limit of 0 still sends neutral for every speed. 0 is off, and gives the
// mapping as it always was.
//
// Returns pulse width clamped to [minPulseUs, maxPulseUs] - or neutralUs for
// every speed when the set is out of order (domePulsesInOrder()), since then
// there is no range to clamp into and a stop must still be a stop. A safety
// invariant, not a nicety: the door and the loader keep such a set out, and
// this is what holds if one gets past them anyway.
// -----------------------------------------------------------------------------
inline uint16_t domeSpeedToPulseUs(float speed, uint16_t neutralUs, uint16_t minPulseUs,
                                   uint16_t maxPulseUs, uint8_t speedLimitPct,
                                   uint16_t smallestTurnUs = 0) {
    if (!domePulsesInOrder(minPulseUs, neutralUs, maxPulseUs)) {
        return neutralUs;
    }
    if (speed < -1.0f)
        speed = -1.0f;
    if (speed > 1.0f)
        speed = 1.0f;

    float limitScale = (float)speedLimitPct / 100.0f;

    float reverseRange = (float)(neutralUs - minPulseUs) * limitScale;
    float forwardRange = (float)(maxPulseUs - neutralUs) * limitScale;

    const float offsetUs = (float)smallestTurnUs;
    int16_t pulseUs = (int16_t)neutralUs;
    if (speed > 0.0f) {
        const float edgeUs = (offsetUs < forwardRange) ? offsetUs : forwardRange;
        pulseUs = (int16_t)((float)neutralUs + edgeUs + speed * (forwardRange - edgeUs));
    } else if (speed < 0.0f) {
        const float edgeUs = (offsetUs < reverseRange) ? offsetUs : reverseRange;
        pulseUs = (int16_t)((float)neutralUs - edgeUs + speed * (reverseRange - edgeUs));
    }

    if (pulseUs < (int16_t)minPulseUs)
        pulseUs = (int16_t)minPulseUs;
    if (pulseUs > (int16_t)maxPulseUs)
        pulseUs = (int16_t)maxPulseUs;

    return (uint16_t)pulseUs;
}

// -----------------------------------------------------------------------------
// domePulseToShare()
// The pulse on the wire as a signed share of the ESC's FULL output (-1..1):
// its distance from neutral over the full half-range on its side, never over
// the speed limit's. This is what a Dome ESC reports back through the Dome
// Rotation seam (include/dome_rotation_backend.h), and the scale the Dome
// Bearing's full-turn speed is recorded on. 0 at neutral, and on a side with
// no range (min == neutral or max == neutral).
//
// With Smallest turn set (#504) the share is measured from its edge, not from
// neutral: a pulse at or inside it is 0, and the full end is still 1. The
// dome stands still inside the dead band Smallest turn is set against, and a
// share counted there would move the Dome Bearing while the dome does not.
// The edge is clamped as domeSpeedToPulseUs() clamps it, against the full
// half-range here; an edge at the full end leaves only the end itself, as 1.
// -----------------------------------------------------------------------------
inline float domePulseShareOfSide(uint16_t distUs, uint16_t sideUs, uint16_t smallestTurnUs) {
    if (distUs >= sideUs) {
        return 1.0f;
    }
    const uint16_t edgeUs = (smallestTurnUs < sideUs) ? smallestTurnUs : sideUs;
    if (distUs <= edgeUs) {
        return 0.0f;
    }
    return (float)(distUs - edgeUs) / (float)(sideUs - edgeUs);
}

inline float domePulseToShare(uint16_t pulseUs, uint16_t neutralUs, uint16_t minPulseUs,
                              uint16_t maxPulseUs, uint16_t smallestTurnUs = 0) {
    if (pulseUs > neutralUs && maxPulseUs > neutralUs) {
        return domePulseShareOfSide(pulseUs - neutralUs, maxPulseUs - neutralUs, smallestTurnUs);
    }
    if (pulseUs < neutralUs && neutralUs > minPulseUs) {
        return -domePulseShareOfSide(neutralUs - pulseUs, neutralUs - minPulseUs, smallestTurnUs);
    }
    return 0.0f;
}

// -----------------------------------------------------------------------------
// Pause before reversing (#504)
//
// A Dome ESC has no ramp of its own (spec sheet, section 8.3), so a turn one
// way replaced by a turn the other way goes out in one 20 ms tick. With a
// pause set, the stop is held on the wire first. The rule is about the WIRE,
// for every source alike (stick, sequence, Dome Controller line, random moves,
// bearing turns):
//
//   - a turn the same way as the last turn on the wire, or a stop, goes at once;
//   - a turn the other way waits until a stop has been on the wire for the
//     pause: the whole pause while the old turn is still on it, the rest of
//     the pause when a stop has been on it for less. A stick flicked through
//     the centre in one tick still gets its pause;
//   - from rest - a stop held for the pause or longer, or no turn since boot -
//     there is no pause, so a turn from a standstill is unchanged.
//
// `wireSign` is the sign of the last non-zero output put on the wire (0 for
// none), and `stopHeldMs` how long a stop has been on the wire since it (0
// while that output is still there). Returns how long the turn must still
// wait, in ms; 0 means it goes now. `pauseMs` 0 is off.
// -----------------------------------------------------------------------------
static constexpr uint16_t DOME_REVERSE_PAUSE_MAX_MS = 2000;

inline uint32_t domeReversePauseMs(int8_t wireSign, uint32_t stopHeldMs, float speed,
                                   uint16_t pauseMs) {
    const int8_t sign = (speed > 0.0f) ? 1 : (speed < 0.0f) ? -1 : 0;
    if (pauseMs == 0 || sign == 0 || wireSign == 0 || sign == wireSign) {
        return 0;
    }
    return (stopHeldMs >= pauseMs) ? 0 : (uint32_t)pauseMs - stopHeldMs;
}

// -----------------------------------------------------------------------------
// Random dome movement: the Mood sets how often (#452)
//
// The pause window the builder stores (dome_rnd_pause_min/max, whole seconds)
// is Full-Awake's pace. The other Moods scale both ends of it, by the same
// proportions the shipped chatter defaults use (30 : 20 : 10 s):
//
//   Quiet      (10)          no random move starts
//   Mid-Awake  (13)          window x 1.5
//   Full-Awake (11), unset 0 window as stored   (default:, as the chatter does)
//   Awake+     (14)          window x 0.5
//
// Both ends are floored at DOME_RND_MIN_PAUSE_MS before the draw, so a 1-2 s
// window in Awake+ draws from 1000..1000 ms rather than from 500..1000 ms and
// then clamping. Speed and move duration do not scale: this is how often, not
// how much.
// -----------------------------------------------------------------------------
static constexpr uint32_t DOME_RND_MIN_PAUSE_MS = 1000;

inline bool domeRndMoodStartsMoves(uint8_t mood) {
    return mood != 10;
}

// Random movement is standing down - switched off, Quiet chosen, a dome
// sequence or a timed one-shot turn starting, the droid driving, the estop or
// Sleep Mode - while one of
// its turns is running. Whether the dome goes to neutral with it: yes, unless
// a manual command was taken on this very tick, which owns the dome now and
// must not be overwritten by a turn that is only ending (#450). The estop and
// Sleep Mode still win, and need no word here: DomeRotationTask takes no command
// under either, so no manual command exists on such a tick.
inline bool domeRndStandDownGoesNeutral(bool manualCommandThisTick) {
    return !manualCommandThisTick;
}

// Next pause in ms, drawn from the Mood-scaled window with the caller's random
// value (esp_random() on the device; passed in so the endpoints are testable).
// Returns 0 when the Mood starts no move (Quiet): a real pause is never under
// DOME_RND_MIN_PAUSE_MS, so 0 cannot be mistaken for one. pauseMax <= pauseMin
// gives the scaled pauseMin every time, as the task always did.
inline uint32_t domeRndPauseMsForMood(uint8_t pauseMinS, uint8_t pauseMaxS, uint8_t mood,
                                      uint32_t randomValue) {
    if (!domeRndMoodStartsMoves(mood)) {
        return 0;
    }
    uint32_t num = 1;
    uint32_t den = 1;
    switch (mood) {
        case 13:
            num = 3;
            den = 2;
            break;
        case 14:
            num = 1;
            den = 2;
            break;
        default:
            break;
    }
    // Scale in ms, never in whole seconds: 7 s x 0.5 is 3500 ms, not 3000.
    uint32_t minMs = (uint32_t)pauseMinS * 1000UL * num / den;
    uint32_t maxMs = (uint32_t)pauseMaxS * 1000UL * num / den;
    if (minMs < DOME_RND_MIN_PAUSE_MS)
        minMs = DOME_RND_MIN_PAUSE_MS;
    if (maxMs < DOME_RND_MIN_PAUSE_MS)
        maxMs = DOME_RND_MIN_PAUSE_MS;
    const uint32_t rangeMs = (maxMs > minMs) ? (maxMs - minMs) : 0UL;
    return minMs + (rangeMs > 0 ? (randomValue % rangeMs) : 0UL);
}
