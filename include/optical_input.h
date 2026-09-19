#pragma once

#include <Arduino.h>
#include "config.h"

namespace duckoustic {

/**
 * Hardware abstraction for the optical microphone input path:
 *   BPW34 → external analog front-end → ESP32-S3 ADC
 *
 * Responsibilities:
 *   - initialise the ADC channel safely
 *   - acquire samples at a deterministic rate (timer + ISR)
 *   - expose the latest filled block and basic metadata
 *
 * Does NOT perform DC removal, filtering, or classification.
 * That lives in SignalProcessor.
 */
class OpticalInput {
public:
    OpticalInput() = default;

    /** Configure ADC pin, attenuation and start the sampling timer. */
    bool initialize();

    /**
     * Called from the main loop (or a dedicated task).
     * Returns true when a new complete block is ready for consumption.
     */
    bool sample();

    /** Pointer to the most recently completed block (valid until next sample()). */
    const int16_t* read_latest() const { return latest_block_; }

    size_t block_size() const { return DUCK_SAMPLES_PER_BLOCK; }

    uint32_t get_sample_rate() const { return sample_rate_hz_; }

    /** Approximate peak-to-peak of the latest block (raw ADC counts). */
    int16_t get_signal_level() const { return signal_level_; }

    /** Mean (DC) of the latest block (raw ADC counts). */
    int16_t get_dc_level() const { return dc_level_; }

    /** Timestamp (micros) of the start of the latest block. */
    uint32_t block_timestamp_us() const { return block_ts_us_; }

    /** True after a successful initialize(). */
    bool ready() const { return ready_; }

private:
    static void IRAM_ATTR on_timer();
    static OpticalInput* instance_;

    bool            ready_          = false;
    uint32_t        sample_rate_hz_ = DUCK_SAMPLE_RATE_HZ;

    // Double-buffer: ISR writes into active_, main loop swaps when full
    int16_t         buffer_a_[DUCK_SAMPLES_PER_BLOCK];
    int16_t         buffer_b_[DUCK_SAMPLES_PER_BLOCK];
    volatile int16_t* active_       = buffer_a_;
    int16_t*        latest_block_   = buffer_b_;
    volatile size_t write_idx_      = 0;
    volatile bool   block_ready_    = false;
    volatile uint32_t block_ts_us_  = 0;

    int16_t         signal_level_   = 0;
    int16_t         dc_level_       = 0;

    hw_timer_t*     timer_          = nullptr;
};

} // namespace duckoustic
