#include "telemetry.h"

namespace duckoustic {

void Telemetry::begin() {
    Serial.begin(DUCK_SERIAL_BAUD);
    // Give USB-CDC a moment on boards that need it
    delay(300);
}

void Telemetry::print_banner() {
    Serial.println();
    Serial.println(F("DUCKOUSTIC v0.1"));
    Serial.println(F("BPW34 INPUT: READY"));
    Serial.println(F("LASER INPUT: EXTERNAL"));
    Serial.println(F("TARGET: OPTICAL VIBRATION"));
    Serial.println();
}

void Telemetry::update(const BlockStats& stats, uint32_t sample_rate_hz) {
    uint32_t now = millis();
    if (now - last_print_ms_ < DUCK_TELEMETRY_INTERVAL_MS) {
        return;
    }
    last_print_ms_ = now;

    Serial.print(F("DUCKOUSTIC"));
    Serial.print(F(" sample_rate="));
    Serial.print(sample_rate_hz);
    Serial.print(F(" dc="));
    Serial.print(stats.dc);
    Serial.print(F(" min="));
    Serial.print(stats.min_val);
    Serial.print(F(" max="));
    Serial.print(stats.max_val);
    Serial.print(F(" peak_to_peak="));
    Serial.print(stats.peak_to_peak);
    Serial.print(F(" rms="));
    Serial.print(stats.rms, 1);
    Serial.print(F(" signal="));
    Serial.println(stats.signal_present ? F("SIGNAL_DETECTED") : F("NO_SIGNAL"));

#if DUCK_RAW_DEBUG
    // Occasional raw peek (first 8 samples of the block)
    const int16_t* raw = nullptr; // caller would need to pass it; kept minimal
    (void)raw;
#endif
}

} // namespace duckoustic
