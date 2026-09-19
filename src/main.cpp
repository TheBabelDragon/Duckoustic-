/**
 * Duckoustic v0.1
 *
 * Hardware-first optical microphone test for ESP32-S3.
 *
 * Path under test:
 *   laser (external) → vibrating reflective target → reflected light
 *   → BPW34 → external analog front-end (e.g. XH-A901 / NE5532 stage)
 *   → ESP32-S3 ADC
 *
 * Scope of this milestone:
 *   - prove the ADC can acquire a usable optical vibration signal
 *   - emit clean serial diagnostics
 *   - keep acquisition, signal stats, telemetry and configuration separated
 *
 * Explicitly out of scope:
 *   speaker output, FFT, networking, servo, laser control
 */

#include <Arduino.h>
#include "config.h"
#include "optical_input.h"
#include "signal_processor.h"
#include "telemetry.h"

using namespace duckoustic;

OpticalInput     optical;
SignalProcessor  processor;
Telemetry        telemetry;

void setup() {
    telemetry.begin();
    telemetry.print_banner();

    if (!optical.initialize()) {
        Serial.println(F("FATAL: OpticalInput init failed"));
        while (true) {
            delay(1000);
        }
    }

    Serial.print(F("ADC pin GPIO"));
    Serial.print(DUCK_ADC_PIN);
    Serial.print(F("  sample_rate="));
    Serial.print(optical.get_sample_rate());
    Serial.print(F(" Hz  block="));
    Serial.print(optical.block_size());
    Serial.println(F(" samples"));
    Serial.println(F("Acquisition running…"));
    Serial.println();
}

void loop() {
    // Pull a completed block when the ISR has filled one
    if (optical.sample()) {
        BlockStats stats = processor.process(optical);
        telemetry.update(stats, optical.get_sample_rate());
    }

    // Keep the main loop light; acquisition is timer-driven
    // A short yield prevents watchdog issues on some cores
    delay(1);
}
