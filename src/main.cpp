/**
 * Duckoustic — stereo optical receiver + SoftAP HTTPS
 *
 * LEFT  BPW34 anode → GPIO4 ADC → 10k → XH L
 * RIGHT BPW34 anode → GPIO7 ADC → 10k → XH R
 * Cathode rails → +3.3 V
 *
 * Audio out: GPIO5 → PAM IN-L, GPIO6 → PAM IN-R
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
ListenEngine     listen_eng;
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

    Serial.println(F("Duckoustic"));
    Serial.println(F("-----------------------------"));
    Serial.print(F("Optical L ADC: GPIO")); Serial.println(DUCK_ADC_LEFT_PIN);
    Serial.print(F("Optical R ADC: GPIO")); Serial.println(DUCK_ADC_RIGHT_PIN);
    Serial.print(F("Audio L PWM:   GPIO")); Serial.println(DUCK_AUDIO_LEFT_PWM_PIN);
    Serial.print(F("Audio R PWM:   GPIO")); Serial.println(DUCK_AUDIO_RIGHT_PWM_PIN);
    Serial.print(F("PWM carrier:   ")); Serial.print(DUCK_AUDIO_PWM_FREQ_HZ); Serial.println(F(" Hz"));
    Serial.print(F("Audio rate:    ")); Serial.print(DUCK_SAMPLE_RATE_HZ); Serial.println(F(" Hz"));
    Serial.println(F("BPW34 receiver:"));
    Serial.println(F("  L anode → GPIO4 → 10k → XH L"));
    Serial.println(F("  R anode → GPIO7 → 10k → XH R"));
    Serial.println(F("  cathode rails → +3.3V"));
    Serial.println(F("HTTPS: https://192.168.4.1/"));
    Serial.println(F("mDNS:  https://duckoustic.local/"));
    Serial.println(F("Ready — join SoftAP, open HTTPS URL (accept self-signed cert if prompted)"));
    Serial.println();
}

void loop() {
    web.handle();

    if (optical.sample()) {
        BlockStats stats = processor.process(optical);
        telemetry.update(stats, optical.get_sample_rate());

        static uint32_t last_opt_ms = 0;
        const uint32_t now = millis();
        if (now - last_opt_ms >= 500) {
            last_opt_ms = now;
            Serial.print(F("Optical receiver:  L ADC GPIO"));
            Serial.print(DUCK_ADC_LEFT_PIN);
            Serial.print(F(": "));
            Serial.print(optical.last_raw_left());
            Serial.print(F("  R ADC GPIO"));
            Serial.print(DUCK_ADC_RIGHT_PIN);
            Serial.print(F(": "));
            Serial.println(optical.last_raw_right());
        }

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
