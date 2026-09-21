#pragma once

#include <Arduino.h>
#include "config.h"
#include "optical_input.h"
#include "laser_output.h"

namespace duckoustic {

/**
 * Live optical passthrough: BPW34 → ADC block → LaserOutput.
 *
 * Responsibilities:
 *   - DC-remove each acquired block
 *   - normalise to [-1, +1]
 *   - pace samples out to LaserOutput at the acquisition sample rate
 *
 * Knows nothing about Wi-Fi or files. Shares LaserOutput with PlaybackEngine;
 * only one should be active at a time (mode switch in main/WebUI).
 *
 * This is the foundation for closed-loop experiments:
 *   optical field → measure → re-modulate → observe again.
 */
class ListenEngine {
public:
    ListenEngine() = default;

    void begin(LaserOutput* laser);

    /** Arm / disarm passthrough. When disarmed, does not touch the laser. */
    void set_active(bool on);
    bool active() const { return active_; }

    /**
     * Call after OpticalInput::sample() returns true.
     * Queues the latest block for paced emission.
     */
    void on_block(const OpticalInput& input);

    /**
     * Emit due samples. Call frequently from loop() while active.
     */
    void tick();

private:
    void queue_block(const int16_t* data, size_t n, int16_t dc);

    LaserOutput* laser_ = nullptr;
    bool         active_ = false;

    // Working buffer: DC-removed, ready to emit
    int16_t  buf_[DUCK_SAMPLES_PER_BLOCK];
    size_t   buf_len_ = 0;
    size_t   buf_pos_ = 0;

    uint32_t next_sample_us_   = 0;
    uint32_t sample_period_us_ = 1000000UL / DUCK_SAMPLE_RATE_HZ;
};

} // namespace duckoustic
