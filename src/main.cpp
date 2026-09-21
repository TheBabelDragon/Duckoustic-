/**
 * Duckoustic v0.2.2
 *
 * Browser-first optical microphone / dual-channel audio output.
 *
 *   BPW34 → GPIO4 ADC  →  ListenEngine / telemetry
 *   WAV   → PlaybackEngine
 *              ↓
 *        LaserOutput (stereo PWM)
 *              ↓
 *   GPIO5 → PAM8403 IN-L     GPIO6 → PAM8403 IN-R
 *              ↓                      ↓
 *        PAM power stage (L+/L-, R+/R- — not driven by GPIO)
 *
 *   LISTEN  BPW34 → ADC → ListenEngine → dual mono on GPIO5+GPIO6
 *   CLONE   WAV file    → PlaybackEngine → dual mono on GPIO5+GPIO6
 *
 * SoftAP WebUI is served over HTTPS (TCP 443), not plain HTTP.
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
ListenEngine     listen_eng;  // not "listen" — clashes with POSIX listen()
WebUI            web;

void setup() {
    telemetry.begin();
    telemetry.print_banner();

    if (!optical.initialize()) {
        Serial.println(F("FATAL: OpticalInput init failed"));
        while (true) delay(1000);
    }

    if (!laser.begin()) {
        Serial.println(F("WARN: Audio PWM output init failed"));
    }

    playback.begin(&laser);
    listen_eng.begin(&laser);

    if (!web.begin(&playback, &laser, &optical, &processor, &listen_eng)) {
        Serial.println(F("FATAL: HTTPS server initialization failed"));
        while (true) delay(1000);
    }

    Serial.print(F("ADC input:        GPIO"));
    Serial.println(DUCK_ADC_PIN);
    Serial.print(F("Audio L PWM:      GPIO"));
    Serial.println(DUCK_AUDIO_LEFT_PWM_PIN);
    Serial.print(F("Audio R PWM:      GPIO"));
    Serial.println(DUCK_AUDIO_RIGHT_PWM_PIN);
    Serial.print(F("PWM carrier:      "));
    Serial.print(DUCK_AUDIO_PWM_FREQ_HZ);
    Serial.println(F(" Hz"));
    Serial.print(F("Audio sample rate: "));
    Serial.print(DUCK_SAMPLE_RATE_HZ);
    Serial.println(F(" Hz"));
    Serial.println(F("Boundary: GPIO5/6 → PAM IN-L/IN-R (not speaker terminals)"));
    Serial.println(F("LISTEN: optical passthrough when mode=listen + laser ON"));
    Serial.println(F("Ready — connect phone to SoftAP, open https://192.168.4.1/"));
    Serial.println();
}

void loop() {
    web.handle();

    // Always acquire; telemetry + LISTEN both need blocks
    if (optical.sample()) {
        BlockStats stats = processor.process(optical);
        telemetry.update(stats, optical.get_sample_rate());

        if (web.mode() == WebUI::Mode::Listen) {
            listen_eng.on_block(optical);
        }
    }

    if (web.mode() == WebUI::Mode::Clone) {
        playback.tick();
    } else {
        listen_eng.tick();
    }

    delay(0);
}
