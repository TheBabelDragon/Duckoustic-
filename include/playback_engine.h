#pragma once

#include <Arduino.h>
#include <FS.h>
#include "config.h"
#include "wav_reader.h"
#include "laser_output.h"

namespace duckoustic {

enum class PlayState : uint8_t {
    Stopped = 0,
    Playing,
    Finished
};

/**
 * File → PCM → LaserOutput playback engine.
 *
 * Knows nothing about Wi-Fi or HTTP. Call tick() from the main loop
 * (or a dedicated task) at a rate faster than the sample period.
 */
class PlaybackEngine {
public:
    PlaybackEngine() = default;

    void begin(LaserOutput* laser);

    /** Load (or reload) the WAV at the given path. Does not start playback. */
    bool load(fs::FS& fs, const char* path);

    void play();
    void stop();
    void set_loop(bool loop) { loop_ = loop; }
    bool looping() const { return loop_; }

    PlayState state() const { return state_; }
    bool is_playing() const { return state_ == PlayState::Playing; }

    const WavInfo& info() const { return reader_.info(); }
    float duration_sec() const { return reader_.duration_sec(); }

    /** Drive one sample if it is time; call frequently from loop(). */
    void tick();

private:
    void emit_sample(int16_t pcm);

    LaserOutput* laser_ = nullptr;
    WavReader    reader_;
    PlayState    state_ = PlayState::Stopped;
    bool         loop_  = false;

    int16_t  buf_[DUCK_PLAYBACK_BUF_SAMPLES];
    size_t   buf_len_  = 0;
    size_t   buf_pos_  = 0;

    uint32_t next_sample_us_ = 0;
    uint32_t sample_period_us_ = 1000000UL / DUCK_WAV_SAMPLE_RATE;
};

} // namespace duckoustic
