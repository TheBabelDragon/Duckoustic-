#pragma once

#include <Arduino.h>
#include "config.h"
#include "signal_processor.h"

namespace duckoustic {

/**
 * Simple serial diagnostic stream.
 * Prints a compact status line at a fixed interval; never dumps every sample.
 */
class Telemetry {
public:
    Telemetry() = default;

    void begin();

    /** Call regularly from loop(). Emits only when the interval has elapsed. */
    void update(const BlockStats& stats, uint32_t sample_rate_hz);

    /** One-time boot banner. */
    void print_banner();

private:
    uint32_t last_print_ms_ = 0;
};

} // namespace duckoustic
