/**
 * Duckoustic v0.2
 *
 * Browser-first optical microphone / optical audio player.
 *
 * Architecture:
 *
 *   PHONE (Safari / Chrome)
 *          |
 *     Wi-Fi SoftAP
 *          |
 *   ESP32-S3 Web UI  -- upload WAV / play / stop / laser / gain / mode
 *          |
 *     +----+----+
 *     |         |
 *  LISTEN     CLONE
 *     |         |
 *  BPW34 ->   PlaybackEngine -> LaserOutput (PWM)
 *  OpticalInput
 *
 * PlaybackEngine does not know about Wi-Fi.
 * OpticalInput path from v0.1 is preserved unchanged.
 */

#include <Arduino.h>
#include "config.h"
#include "optical_input.h"
#include "signal_processor.h"
#include "telemetry.h"
#include "laser_output.h"
#include "playback_engine.h"
#include "web_ui.h"

using namespace duckoustic;

OpticalInput     optical;
SignalProcessor  processor;
Telemetry        telemetry;
LaserOutput      laser;
PlaybackEngine   playback;
WebUI            web;

void setup() {
    telemetry.begin();
    telemetry.print_banner();

    if (!optical.initialize()) {
        Serial.println(F("FATAL: OpticalInput init failed"));
        while (true) delay(1000);
    }

    if (!laser.begin()) {
        Serial.println(F("WARN: LaserOutput init failed — optical playback disabled"));
    }

    playback.begin(&laser);

    if (!web.begin(&playback, &laser, &optical, &processor)) {
        Serial.println(F("FATAL: WebUI / SoftAP init failed"));
        while (true) delay(1000);
    }

    Serial.print(F("ADC pin GPIO"));
    Serial.print(DUCK_ADC_PIN);
    Serial.print(F("  sample_rate="));
    Serial.print(optical.get_sample_rate());
    Serial.print(F(" Hz  block="));
    Serial.print(optical.block_size());
    Serial.println(F(" samples"));
    Serial.print(F("Laser PWM pin GPIO"));
    Serial.println(DUCK_LASER_PWM_PIN);
    Serial.println(F("Ready — connect phone to SoftAP, open http://192.168.4.1/"));
    Serial.println();
}

void loop() {
    // HTTP
    web.handle();

    // Optical acquisition (always running; useful telemetry in both modes)
    if (optical.sample()) {
        BlockStats stats = processor.process(optical);
        telemetry.update(stats, optical.get_sample_rate());
    }

    // Clone-mode playback (sample-accurate when loop is tight enough)
    if (web.mode() == WebUI::Mode::Clone) {
        playback.tick();
    }

    // Short yield — keep HTTP + playback responsive
    delay(0);
}
