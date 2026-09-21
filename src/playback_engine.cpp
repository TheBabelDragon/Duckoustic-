#include "playback_engine.h"

namespace duckoustic {

void PlaybackEngine::begin(LaserOutput* laser) {
    laser_ = laser;
    state_ = PlayState::Stopped;
    sample_period_us_ = 1000000UL / DUCK_WAV_SAMPLE_RATE;
}

bool PlaybackEngine::load(fs::FS& fs, const char* path) {
    stop();
    if (!reader_.open(fs, path)) {
        return false;
    }
    buf_len_ = 0;
    buf_pos_ = 0;
    return true;
}

void PlaybackEngine::play() {
    if (!reader_.is_open() || !laser_) return;
    if (!reader_.rewind()) return;
    buf_len_ = 0;
    buf_pos_ = 0;
    next_sample_us_ = micros();
    state_ = PlayState::Playing;
    laser_->set_enabled(true);
}

void PlaybackEngine::stop() {
    state_ = PlayState::Stopped;
    buf_len_ = 0;
    buf_pos_ = 0;
    if (laser_) {
        laser_->idle();
    }
}

void PlaybackEngine::emit_sample(int16_t pcm) {
    // int16 → float [-1, 1]
    float s = static_cast<float>(pcm) / 32768.0f;
    if (laser_) {
        laser_->write(s);
    }
}

void PlaybackEngine::tick() {
    if (state_ != PlayState::Playing || !laser_) return;

    const uint32_t now = micros();
    // Allow modest catch-up so we don't drift forever under load
    int guard = 8;
    while ((int32_t)(now - next_sample_us_) >= 0 && guard-- > 0) {
        if (buf_pos_ >= buf_len_) {
            buf_len_ = reader_.read_samples(buf_, DUCK_PLAYBACK_BUF_SAMPLES);
            buf_pos_ = 0;
            if (buf_len_ == 0) {
                if (loop_) {
                    reader_.rewind();
                    buf_len_ = reader_.read_samples(buf_, DUCK_PLAYBACK_BUF_SAMPLES);
                    buf_pos_ = 0;
                    if (buf_len_ == 0) {
                        state_ = PlayState::Finished;
                        laser_->idle();
                        return;
                    }
                } else {
                    state_ = PlayState::Finished;
                    laser_->idle();
                    return;
                }
            }
        }

        emit_sample(buf_[buf_pos_++]);
        next_sample_us_ += sample_period_us_;
    }
}

} // namespace duckoustic
