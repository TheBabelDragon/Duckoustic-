#pragma once

#include <Arduino.h>
#include "config.h"
#include "optical_input.h"

namespace duckoustic {

/**
 * Lightweight statistics computed on each OpticalInput block.
 * Intentionally free of filtering — v0.1 wants to see the raw physical signal.
 */
struct BlockStats {
    int16_t  dc;            // mean
    int16_t  min_val;
    int16_t  max_val;
    int16_t  peak_to_peak;
    float    rms;           // RMS of (optionally DC-removed) samples
    bool     signal_present; // peak_to_peak >= threshold
    uint32_t timestamp_us;
};

class SignalProcessor {
public:
    SignalProcessor() = default;

    /** Process the latest block from the optical input. */
    BlockStats process(const OpticalInput& input);

private:
    float compute_rms(const int16_t* data, size_t n, int16_t dc) const;
};

} // namespace duckoustic
