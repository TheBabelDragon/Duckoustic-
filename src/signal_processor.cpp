#include "signal_processor.h"
#include <math.h>

namespace duckoustic {

float SignalProcessor::compute_rms(const int16_t* data, size_t n, int16_t dc) const {
    if (n == 0) return 0.0f;

    double acc = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double v = static_cast<double>(data[i]);
        if (DUCK_DC_REMOVAL) {
            v -= static_cast<double>(dc);
        }
        acc += v * v;
    }
    return static_cast<float>(sqrt(acc / static_cast<double>(n)));
}

BlockStats SignalProcessor::process(const OpticalInput& input) {
    BlockStats s{};
    const int16_t* data = input.read_latest();
    const size_t   n    = input.block_size();

    if (!data || n == 0) {
        return s;
    }

    int32_t sum = 0;
    int16_t mn  = 4095;
    int16_t mx  = 0;

    for (size_t i = 0; i < n; ++i) {
        int16_t v = data[i];
        sum += v;
        if (v < mn) mn = v;
        if (v > mx) mx = v;
    }

    s.dc            = static_cast<int16_t>(sum / static_cast<int32_t>(n));
    s.min_val       = mn;
    s.max_val       = mx;
    s.peak_to_peak  = static_cast<int16_t>(mx - mn);
    s.rms           = compute_rms(data, n, s.dc);
    s.signal_present = (s.peak_to_peak >= DUCK_SIGNAL_THRESHOLD);
    s.timestamp_us  = input.block_timestamp_us();

    return s;
}

} // namespace duckoustic
