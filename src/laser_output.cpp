#include "laser_output.h"

namespace duckoustic {

bool LaserOutput::begin() {
    if (ready_) return true;

#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
    // Arduino-ESP32 3.x LEDC
    ledcAttach(DUCK_LASER_PWM_PIN, DUCK_LASER_PWM_FREQ_HZ, DUCK_LASER_PWM_RES_BITS);
#else
    // Arduino-ESP32 2.x LEDC
    ledcSetup(DUCK_LASER_PWM_CHANNEL, DUCK_LASER_PWM_FREQ_HZ, DUCK_LASER_PWM_RES_BITS);
    ledcAttachPin(DUCK_LASER_PWM_PIN, DUCK_LASER_PWM_CHANNEL);
#endif

    max_duty_counts_ = (1u << DUCK_LASER_PWM_RES_BITS) - 1;
    // Bias at ~50 % of the allowed max so bipolar audio has headroom
    bias_duty_ = static_cast<uint32_t>(max_duty_counts_ * DUCK_LASER_MAX_DUTY * 0.5f);

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

void LaserOutput::write(float sample) {
    if (!ready_ || !enabled_) {
        idle();
        return;
    }

    // Clamp sample
    if (sample >  1.0f) sample =  1.0f;
    if (sample < -1.0f) sample = -1.0f;

    // Scale by gain and map onto [0, max_allowed]
    const float half_range = max_duty_counts_ * DUCK_LASER_MAX_DUTY * 0.5f;
    float duty_f = bias_duty_ + (sample * gain_ * half_range);

    if (duty_f < 0.0f) duty_f = 0.0f;
    if (duty_f > max_duty_counts_ * DUCK_LASER_MAX_DUTY) {
        duty_f = max_duty_counts_ * DUCK_LASER_MAX_DUTY;
    }

    const uint32_t duty = static_cast<uint32_t>(duty_f);

#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
    ledcWrite(DUCK_LASER_PWM_PIN, duty);
#else
    ledcWrite(DUCK_LASER_PWM_CHANNEL, duty);
#endif
}

void LaserOutput::idle() {
    if (!ready_) return;
#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
    ledcWrite(DUCK_LASER_PWM_PIN, 0);
#else
    ledcWrite(DUCK_LASER_PWM_CHANNEL, 0);
#endif
}

} // namespace duckoustic
