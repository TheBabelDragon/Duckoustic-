#include "wav_reader.h"

namespace duckoustic {

bool WavReader::open(fs::FS& fs, const char* path) {
    close();
    file_ = fs.open(path, "r");
    if (!file_) {
        return false;
    }
    if (!parse_header()) {
        close();
        return false;
    }
    return true;
}

void WavReader::close() {
    if (file_) {
        file_.close();
    }
    info_ = WavInfo{};
}

bool WavReader::parse_header() {
    info_ = WavInfo{};
    if (!file_ || file_.size() < 44) {
        return false;
    }

    auto read_u16 = [this]() -> uint16_t {
        uint8_t b[2];
        if (file_.read(b, 2) != 2) return 0;
        return static_cast<uint16_t>(b[0] | (b[1] << 8));
    };
    auto read_u32 = [this]() -> uint32_t {
        uint8_t b[4];
        if (file_.read(b, 4) != 4) return 0;
        return static_cast<uint32_t>(b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24));
    };

    // RIFF header
    char riff[4];
    if (file_.read(reinterpret_cast<uint8_t*>(riff), 4) != 4) return false;
    if (strncmp(riff, "RIFF", 4) != 0) return false;
    (void)read_u32(); // chunk size
    char wave[4];
    if (file_.read(reinterpret_cast<uint8_t*>(wave), 4) != 4) return false;
    if (strncmp(wave, "WAVE", 4) != 0) return false;

    // Walk chunks until we find "fmt " and "data"
    bool have_fmt  = false;
    bool have_data = false;

    while (file_.available() >= 8) {
        char id[4];
        if (file_.read(reinterpret_cast<uint8_t*>(id), 4) != 4) break;
        uint32_t sz = read_u32();

        if (strncmp(id, "fmt ", 4) == 0) {
            if (sz < 16) return false;
            uint16_t audio_format = read_u16();
            info_.channels        = read_u16();
            info_.sample_rate     = read_u32();
            (void)read_u32(); // byte rate
            (void)read_u16(); // block align
            info_.bits_per_sample = read_u16();
            // skip any remaining fmt bytes
            if (sz > 16) {
                file_.seek(file_.position() + (sz - 16));
            }
            if (audio_format != 1) { // PCM only
                return false;
            }
            have_fmt = true;
        } else if (strncmp(id, "data", 4) == 0) {
            info_.data_offset = file_.position();
            info_.data_bytes  = sz;
            have_data = true;
            // don't consume data; leave cursor at start of PCM
            break;
        } else {
            // skip unknown chunk (pad to even)
            uint32_t skip = sz + (sz & 1);
            file_.seek(file_.position() + skip);
        }
    }

    if (!have_fmt || !have_data) return false;

    // Enforce v0.2 contract
    if (info_.channels != DUCK_WAV_CHANNELS) return false;
    if (info_.sample_rate != DUCK_WAV_SAMPLE_RATE) return false;
    if (info_.bits_per_sample != DUCK_WAV_BITS) return false;

    const uint32_t bytes_per_sample = (info_.bits_per_sample / 8) * info_.channels;
    if (bytes_per_sample == 0) return false;
    info_.num_samples = info_.data_bytes / bytes_per_sample;
    info_.valid = true;

    // Position at PCM start
    file_.seek(info_.data_offset);
    return true;
}

size_t WavReader::read_samples(int16_t* dest, size_t max_samples) {
    if (!file_ || !info_.valid || max_samples == 0) return 0;

    const size_t bytes_needed = max_samples * sizeof(int16_t);
    size_t got = file_.read(reinterpret_cast<uint8_t*>(dest), bytes_needed);
    return got / sizeof(int16_t);
}

bool WavReader::rewind() {
    if (!file_ || !info_.valid) return false;
    return file_.seek(info_.data_offset);
}

float WavReader::duration_sec() const {
    if (!info_.valid || info_.sample_rate == 0) return 0.0f;
    return static_cast<float>(info_.num_samples) / static_cast<float>(info_.sample_rate);
}

} // namespace duckoustic
