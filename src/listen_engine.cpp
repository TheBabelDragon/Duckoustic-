#include "listen_engine.h"

namespace duckoustic {

void ListenEngine::begin(LaserOutput* laser) {
    laser_ = laser;
    active_ = false;
    buf_len_ = 0;
    buf_pos_ = 0;
    sample_period_us_ = 1000000UL / DUCK_SAMPLE_RATE_HZ;
}

void ListenEngine::set_active(bool on) {
    active_ = on;
    buf_len_ = 0;
    buf_pos_ = 0;
    if (!on && laser_) {
        // Do not force idle here if Clone may own the laser;
        // main/WebUI stops the other engine on mode switch.
        laser_->idle();
    }
}

void ListenEngine::queue_block(const int16_t* data, size_t n, int16_t dc) {
    if (!data || n == 0 || n > DUCK_SAMPLES_PER_BLOCK) return;

    // If previous block still has unread samples, drop the tail and take the new block.
    // Prefer freshest optical data over backlog (low latency for feedback experiments).
    for (size_t i = 0; i < n; ++i) {
        int32_t ac = static_cast<int32_t>(data[i]) - static_cast<int32_t>(dc);
        // Clamp to int16 range after DC removal
        if (ac >  32767) ac =  32767;
        if (ac < -32768) ac = -32768;
        buf_[i] = static_cast<int16_t>(ac);
    }
    buf_len_ = n;
    buf_pos_ = 0;

    if (next_sample_us_ == 0) {
        next_sample_us_ = micros();
    }
}

void ListenEngine::on_block(const OpticalInput& input) {
    if (!active_ || !laser_) return;

    const int16_t* data = input.read_latest();
    const size_t n = input.block_size();
    if (!data || n == 0) return;

    queue_block(data, n, input.get_dc_level());
}

void ListenEngine::tick() {
    if (!active_ || !laser_ || buf_pos_ >= buf_len_) return;

    const uint32_t now = micros();
    int guard = 16;  // catch-up limit per loop iteration

    while (buf_pos_ < buf_len_ &&
           (int32_t)(now - next_sample_us_) >= 0 &&
           guard-- > 0) {

        // ADC residual after DC removal is typically small vs full 12-bit scale.
        // Map with DUCK_LISTEN_SCALE so moderate optical vibration reaches useful modulation depth.
        // Default scale treats ~±512 counts as full-scale (±1.0).
        const float s = static_cast<float>(buf_[buf_pos_]) * (1.0f / static_cast<float>(DUCK_LISTEN_SCALE));
        laser_->write(s);

        buf_pos_++;
        next_sample_us_ += sample_period_us_;
    }
}

} // namespace duckoustic
