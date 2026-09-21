#pragma once

#include <Arduino.h>
#include <FS.h>
#include "config.h"

namespace duckoustic {

/**
 * Minimal PCM WAV reader for the v0.2 contract:
 *   - mono
 *   - 8 kHz
 *   - 16-bit signed little-endian PCM
 *
 * Does not allocate large buffers; streams samples on demand from LittleFS.
 */
struct WavInfo {
    uint16_t channels      = 0;
    uint32_t sample_rate   = 0;
    uint16_t bits_per_sample = 0;
    uint32_t data_bytes    = 0;
    uint32_t data_offset   = 0;
    uint32_t num_samples   = 0;   // per channel
    bool     valid         = false;
};

class WavReader {
public:
    WavReader() = default;

    /** Open path, parse header, leave file positioned at first PCM sample. */
    bool open(fs::FS& fs, const char* path);

    void close();

    bool is_open() const { return file_; }
    const WavInfo& info() const { return info_; }

    /**
     * Read up to max_samples mono int16 samples into dest.
     * Returns number of samples actually read (0 = EOF or error).
     */
    size_t read_samples(int16_t* dest, size_t max_samples);

    /** Seek to first PCM sample (restart playback). */
    bool rewind();

    /** Seconds of audio (float). */
    float duration_sec() const;

private:
    bool parse_header();

    File    file_;
    WavInfo info_{};
};

} // namespace duckoustic
