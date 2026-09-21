#pragma once

#include <Arduino.h>
#include "config.h"

namespace duckoustic {

struct OpticalStereoSample {
    int16_t left;
    int16_t right;
};

/**
 * Optical microphone input:
 *   LEFT  BPW34 anode rail → GPIO4
 *   RIGHT BPW34 anode rail → GPIO7
 *   Cathode rails → +3.3 V (hardware)
 *
 * Mono: LEFT only. Stereo: both channels, never summed.
 */
class OpticalInput {
public:
    OpticalInput() = default;

    bool initialize();
    void set_channel_mode(OpticalChannelMode m);
    OpticalChannelMode channel_mode() const { return channel_mode_; }

    bool sample();

    const int16_t* read_latest() const { return latest_left_; }
    const int16_t* read_latest_left() const { return latest_left_; }
    const int16_t* read_latest_right() const { return latest_right_; }

    size_t block_size() const { return DUCK_SAMPLES_PER_BLOCK; }
    uint32_t get_sample_rate() const { return sample_rate_hz_; }

    int16_t get_signal_level() const { return signal_level_l_; }
    int16_t get_dc_level() const { return dc_level_l_; }
    int16_t get_signal_level_left() const { return signal_level_l_; }
    int16_t get_signal_level_right() const { return signal_level_r_; }
    int16_t get_dc_level_left() const { return dc_level_l_; }
    int16_t get_dc_level_right() const { return dc_level_r_; }

    int16_t last_raw_left() const { return last_raw_l_; }
    int16_t last_raw_right() const { return last_raw_r_; }

    uint32_t block_timestamp_us() const { return block_ts_us_; }
    bool ready() const { return ready_; }

private:
    static void IRAM_ATTR on_timer();
    static OpticalInput* instance_;

    bool               ready_          = false;
    uint32_t           sample_rate_hz_ = DUCK_SAMPLE_RATE_HZ;
    OpticalChannelMode channel_mode_   = OpticalChannelMode::Mono;

    int16_t  left_a_[DUCK_SAMPLES_PER_BLOCK];
    int16_t  left_b_[DUCK_SAMPLES_PER_BLOCK];
    int16_t  right_a_[DUCK_SAMPLES_PER_BLOCK];
    int16_t  right_b_[DUCK_SAMPLES_PER_BLOCK];

    volatile int16_t* active_left_  = left_a_;
    volatile int16_t* active_right_ = right_a_;
    int16_t* latest_left_  = left_b_;
    int16_t* latest_right_ = right_b_;

    volatile size_t   write_idx_   = 0;
    volatile bool     block_ready_ = false;
    volatile uint32_t block_ts_us_ = 0;

    volatile int16_t  last_raw_l_ = 0;
    volatile int16_t  last_raw_r_ = 0;

    int16_t signal_level_l_ = 0;
    int16_t signal_level_r_ = 0;
    int16_t dc_level_l_     = 0;
    int16_t dc_level_r_     = 0;

    hw_timer_t* timer_ = nullptr;
};

} // namespace duckoustic
