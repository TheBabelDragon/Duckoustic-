/**
 * Duckoustic v0.2.1
 *
 * Browser-first optical microphone / optical audio player.
 *
 *   LISTEN  BPW34 → ADC → ListenEngine → LaserOutput  (live optical passthrough)
 *   CLONE   WAV file   → PlaybackEngine → LaserOutput
 *
 * Both modes share the same LaserOutput. Mode switch is exclusive.
 * PlaybackEngine / ListenEngine do not know about Wi-Fi.
 */

#include <Arduino.h>
#include "config.h"
#include "optical_input.h"
#include "signal_processor.h"
#include "telemetry.h"
#include "laser_output.h"
#include "playback_engine.h"
#include "listen_engine.h"
#include "web_ui.h"

using namespace duckoustic;

OpticalInput     optical;
SignalProcessor  processor;
Telemetry        telemetry;
LaserOutput      laser;
PlaybackEngine   playback;
ListenEngine     listen;
WebUI            web;

void setup() {
    telemetry.begin();
    telemetry.print_banner();

    if (!optical.initialize()) {
        Serial.println(F("FATAL: OpticalInput init failed"));
        while (true) delay(1000);
    }

    if (!laser.begin()) {
        Serial.println(F("WARN: LaserOutput init failed — optical output disabled"));
    }

    playback.begin(&laser);
    listen.begin(&laser);

    if (!web.begin(&playback, &laser, &optical, &processor, &listen)) {
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
    Serial.println(F("LISTEN: optical passthrough armed when mode=listen + laser ON"));
    Serial.println(F("Ready — connect phone to SoftAP, open http://192.168.4.1/"));
    Serial.println();
}

void loop() {
    web.handle();

    // Always acquire; telemetry + LISTEN both need blocks
    if (optical.sample()) {
        BlockStats stats = processor.process(optical);
        telemetry.update(stats, optical.get_sample_rate());

        if (web.mode() == WebUI::Mode::Listen) {
            listen.on_block(optical);
        }
    }

    if (web.mode() == WebUI::Mode::Clone) {
        playback.tick();
    } else {
        listen.tick();
    }

    delay(0);
}
