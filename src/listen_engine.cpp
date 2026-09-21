#include "listen_engine.h"

namespace duckoustic {

void ListenEngine::begin(LaserOutput* laser) {
    laser_ = laser;
    active_ = false;
    stereo_ = false;
    buf_len_ = 0;
    buf_pos_ = 0;
    sample_period_us_ = 1000000UL / DUCK_SAMPLE_RATE_HZ;
}

void ListenEngine::set_active(bool on) {
    active_ = on;
    buf_len_ = 0;
    buf_pos_ = 0;
    if (!on && laser_) {
        laser_->idle();
    }
}

void ListenEngine::queue_block(const OpticalInput& input) {
    const size_t n = input.block_size();
    if (n == 0 || n > DUCK_SAMPLES_PER_BLOCK) return;

    stereo_ = (input.channel_mode() == OpticalChannelMode::Stereo);
    const int16_t* L = input.read_latest_left();
    const int16_t* R = input.read_latest_right();
    const int16_t dc_l = input.get_dc_level_left();
    const int16_t dc_r = input.get_dc_level_right();

    for (size_t i = 0; i < n; ++i) {
        int32_t ac_l = static_cast<int32_t>(L[i]) - static_cast<int32_t>(dc_l);
        if (ac_l >  32767) ac_l =  32767;
        if (ac_l < -32768) ac_l = -32768;
        buf_l_[i] = static_cast<int16_t>(ac_l);

        if (stereo_) {
            int32_t ac_r = static_cast<int32_t>(R[i]) - static_cast<int32_t>(dc_r);
            if (ac_r >  32767) ac_r =  32767;
            if (ac_r < -32768) ac_r = -32768;
            buf_r_[i] = static_cast<int16_t>(ac_r);
        } else {
            buf_r_[i] = buf_l_[i];
        }
    }
    buf_len_ = n;
    buf_pos_ = 0;
    if (next_sample_us_ == 0) {
        next_sample_us_ = micros();
    }
}

void ListenEngine::on_block(const OpticalInput& input) {
    if (!active_ || !laser_) return;
    if (!input.read_latest_left()) return;
    queue_block(input);
}

void ListenEngine::tick() {
    if (!active_ || !laser_ || buf_pos_ >= buf_len_) return;

    const uint32_t now = micros();
    int guard = 16;
    const float inv_scale = 1.0f / static_cast<float>(DUCK_LISTEN_SCALE);

    while (buf_pos_ < buf_len_ &&
           (int32_t)(now - next_sample_us_) >= 0 &&
           guard-- > 0) {

        const float sl = static_cast<float>(buf_l_[buf_pos_]) * inv_scale;
        if (stereo_) {
            const float sr = static_cast<float>(buf_r_[buf_pos_]) * inv_scale;
            laser_->write_stereo(sl, sr);
        } else {
            laser_->write(sl);
        }

        buf_pos_++;
        next_sample_us_ += sample_period_us_;
    }
}

} // namespace duckoustic
