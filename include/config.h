#pragma once

#include <Arduino.h>

// ---------------------------------------------------------------------------
// Duckoustic v0.1 configuration
// All tunable parameters live here so the rest of the code stays clean.
// ---------------------------------------------------------------------------

namespace duckoustic {

// BPW34 / analog-front-end → ESP32-S3 ADC pin (must be an ADC1 channel)
// Common safe choices on ESP32-S3-DevKitC-1: GPIO1–GPIO10
#ifndef DUCK_ADC_PIN
#define DUCK_ADC_PIN            4          // GPIO4 = ADC1_CH3
#endif

// Attenuation / full-scale range
// ADC_11db ≈ 0 … ~3100 mV (conservative, safe for 3.3 V rail after AFE)
#ifndef DUCK_ADC_ATTEN
#define DUCK_ADC_ATTEN          ADC_11db
#endif

// Target sample rate (Hz). 8 kHz is the v0.1 milestone.
#ifndef DUCK_SAMPLE_RATE_HZ
#define DUCK_SAMPLE_RATE_HZ     8000
#endif

// Number of samples collected into one block before statistics are computed
#ifndef DUCK_SAMPLES_PER_BLOCK
#define DUCK_SAMPLES_PER_BLOCK  256
#endif

// Subtract measured DC (block mean) before reporting AC metrics
#ifndef DUCK_DC_REMOVAL
#define DUCK_DC_REMOVAL         true
#endif

// Peak-to-peak threshold (raw ADC counts) used to decide SIGNAL_DETECTED
// vs NO_SIGNAL.  Tune on the bench; start low so any real vibration is seen.
#ifndef DUCK_SIGNAL_THRESHOLD
#define DUCK_SIGNAL_THRESHOLD   40
#endif

// How often (ms) the serial diagnostic line is emitted
#ifndef DUCK_TELEMETRY_INTERVAL_MS
#define DUCK_TELEMETRY_INTERVAL_MS  250
#endif

// Optional: dump a few raw samples occasionally (debug only)
#ifndef DUCK_RAW_DEBUG
#define DUCK_RAW_DEBUG          false
#endif

// Serial baud (must match platformio.ini monitor_speed)
#ifndef DUCK_SERIAL_BAUD
#define DUCK_SERIAL_BAUD        115200
#endif

} // namespace duckoustic
