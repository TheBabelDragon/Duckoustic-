#pragma once

#include <Arduino.h>
#include "config.h"
#include "optical_input.h"
#include "laser_output.h"

namespace duckoustic {

/**
 * Live optical passthrough → LaserOutput (PAM PWM).
 * Mono: LEFT → write() dual mono. Stereo: L→GPIO5, R→GPIO6 (no summing).
 */
class ListenEngine {
public:
    ListenEngine() = default;

    void begin(LaserOutput* laser);
    void set_active(bool on);
    bool active() const { return active_; }

    void on_block(const OpticalInput& input);
    void tick();

private:
    void queue_block(const OpticalInput& input);

    LaserOutput* laser_ = nullptr;
    bool         active_ = false;
    bool         stereo_ = false;

    int16_t  buf_l_[DUCK_SAMPLES_PER_BLOCK];
    int16_t  buf_r_[DUCK_SAMPLES_PER_BLOCK];
    size_t   buf_len_ = 0;
    size_t   buf_pos_ = 0;

    uint32_t next_sample_us_   = 0;
    uint32_t sample_period_us_ = 1000000UL / DUCK_SAMPLE_RATE_HZ;
};

} // namespace duckoustic
