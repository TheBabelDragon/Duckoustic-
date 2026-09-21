#include "laser_output.h"

namespace duckoustic {

bool LaserOutput::begin() {
    if (ready_) return true;

#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
    // Arduino-ESP32 3.x LEDC — pin-based API
    ledcAttach(DUCK_AUDIO_LEFT_PWM_PIN,  DUCK_AUDIO_PWM_FREQ_HZ, DUCK_AUDIO_PWM_RES_BITS);
    ledcAttach(DUCK_AUDIO_RIGHT_PWM_PIN, DUCK_AUDIO_PWM_FREQ_HZ, DUCK_AUDIO_PWM_RES_BITS);
#else
    // Arduino-ESP32 2.x LEDC — channel-based API
    ledcSetup(DUCK_AUDIO_LEFT_PWM_CHANNEL,  DUCK_AUDIO_PWM_FREQ_HZ, DUCK_AUDIO_PWM_RES_BITS);
    ledcAttachPin(DUCK_AUDIO_LEFT_PWM_PIN,  DUCK_AUDIO_LEFT_PWM_CHANNEL);
    ledcSetup(DUCK_AUDIO_RIGHT_PWM_CHANNEL, DUCK_AUDIO_PWM_FREQ_HZ, DUCK_AUDIO_PWM_RES_BITS);
    ledcAttachPin(DUCK_AUDIO_RIGHT_PWM_PIN, DUCK_AUDIO_RIGHT_PWM_CHANNEL);
#endif

    left_max_duty_counts_  = (1u << DUCK_AUDIO_PWM_RES_BITS) - 1;
    right_max_duty_counts_ = (1u << DUCK_AUDIO_PWM_RES_BITS) - 1;

    // Bias at ~50 % of the allowed max so bipolar audio has headroom
    left_bias_duty_  = static_cast<uint32_t>(left_max_duty_counts_  * DUCK_AUDIO_MAX_DUTY * 0.5f);
    right_bias_duty_ = static_cast<uint32_t>(right_max_duty_counts_ * DUCK_AUDIO_MAX_DUTY * 0.5f);

    idle();
    ready_ = true;
    return true;
}

void LaserOutput::set_enabled(bool on) {
    enabled_ = on;
    if (!on) {
        idle();
    }
}

void LaserOutput::set_gain(float g) {
    if (g < 0.0f) g = 0.0f;
    if (g > 1.0f) g = 1.0f;
    gain_ = g;
}

uint32_t LaserOutput::sample_to_duty(float sample, uint32_t bias, uint32_t max_counts) const {
    if (sample >  1.0f) sample =  1.0f;
    if (sample < -1.0f) sample = -1.0f;

    const float half_range = max_counts * DUCK_AUDIO_MAX_DUTY * 0.5f;
    float duty_f = static_cast<float>(bias) + (sample * gain_ * half_range);

    if (duty_f < 0.0f) duty_f = 0.0f;
    const float ceiling = max_counts * DUCK_AUDIO_MAX_DUTY;
    if (duty_f > ceiling) duty_f = ceiling;

    return static_cast<uint32_t>(duty_f);
}

void LaserOutput::write_duties(uint32_t left_duty, uint32_t right_duty) {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
    ledcWrite(DUCK_AUDIO_LEFT_PWM_PIN,  left_duty);
    ledcWrite(DUCK_AUDIO_RIGHT_PWM_PIN, right_duty);
#else
    ledcWrite(DUCK_AUDIO_LEFT_PWM_CHANNEL,  left_duty);
    ledcWrite(DUCK_AUDIO_RIGHT_PWM_CHANNEL, right_duty);
#endif
}

void LaserOutput::write(float sample) {
    // Mono compatibility path: identical L + R
    write_stereo(sample, sample);
}

void LaserOutput::write_stereo(float left, float right) {
    if (!ready_ || !enabled_) {
        idle();
        return;
    }

    const uint32_t ld = sample_to_duty(left,  left_bias_duty_,  left_max_duty_counts_);
    const uint32_t rd = sample_to_duty(right, right_bias_duty_, right_max_duty_counts_);
    write_duties(ld, rd);
}

void LaserOutput::idle() {
    if (!ready_) return;
    write_duties(0, 0);
}

} // namespace duckoustic
