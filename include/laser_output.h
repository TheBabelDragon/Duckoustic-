#pragma once

#include <Arduino.h>
#include "config.h"

namespace duckoustic {

/**
 * Optical output abstraction.
 *
 * Responsibilities:
 *   - initialise PWM (or future DAC) channel for laser amplitude modulation
 *   - accept normalised sample values in [-1, +1]
 *   - enforce safety duty limits
 *   - know nothing about Wi-Fi, files, or modes
 *
 * PlaybackEngine and ListenEngine both drive this interface.
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
     * Write one sample.
     * @param sample  normalised PCM in [-1.0, +1.0]
     */
    void write(float sample);

    /** Force output to idle (laser off or bias only). */
    void idle();

private:
    bool  ready_   = false;
    bool  enabled_ = false;
    float gain_    = DUCK_DEFAULT_GAIN;

    uint32_t max_duty_counts_ = 0;
    uint32_t bias_duty_       = 0;   // mid-scale when idle / enabled but silent
};

} // namespace duckoustic
