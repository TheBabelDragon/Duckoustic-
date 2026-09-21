#pragma once

#include <Arduino.h>
#include "config.h"

namespace duckoustic {

/**
 * Stereo audio-output abstraction (PWM → PAM8403 input stage).
 *
 *   write(sample)        — mono compatibility: same sample → L and R
 *   write_stereo(L, R)   — independent left / right duties
 *
 * Electrical boundary (firmware does not enforce wiring):
 *   GPIO5 → PAM IN-L, GPIO6 → PAM IN-R, GND common.
 *   GPIO is NOT a conventional DAC; RC reconstruction before PAM is recommended.
 *   Do not connect GPIO to PAM speaker (L+/L-, R+/R-) terminals.
 *
 * Enforces normalised sample range [-1, +1], gain, and per-channel duty limits.
 */
class LaserOutput {
public:
    LaserOutput() = default;

    bool begin();
    void set_enabled(bool on);
    bool enabled() const { return enabled_; }

    /** Linear gain applied after sample scaling (0.0 … 1.0). */
    void set_gain(float g);
    float gain() const { return gain_; }

    /**
     * Mono write — identical sample on left and right (GPIO5 + GPIO6).
     * @param sample  normalised PCM in [-1.0, +1.0]
     */
    void write(float sample);

    /**
     * Independent stereo write.
     * @param left   normalised PCM in [-1.0, +1.0] → GPIO5
     * @param right  normalised PCM in [-1.0, +1.0] → GPIO6
     */
    void write_stereo(float left, float right);

    /** Force both channels to idle (inactive). */
    void idle();

private:
    uint32_t sample_to_duty(float sample, uint32_t bias, uint32_t max_counts) const;
    void     write_duties(uint32_t left_duty, uint32_t right_duty);

    bool  ready_   = false;
    bool  enabled_ = false;
    float gain_    = DUCK_DEFAULT_GAIN;

    uint32_t left_max_duty_counts_  = 0;
    uint32_t right_max_duty_counts_ = 0;
    uint32_t left_bias_duty_        = 0;
    uint32_t right_bias_duty_       = 0;
};

} // namespace duckoustic
