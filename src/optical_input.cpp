#include "optical_input.h"

namespace duckoustic {

OpticalInput* OpticalInput::instance_ = nullptr;

bool OpticalInput::initialize() {
    if (ready_) return true;
    instance_ = this;

#if DUCK_OPTICAL_DEFAULT_STEREO
    channel_mode_ = OpticalChannelMode::Stereo;
#else
    channel_mode_ = OpticalChannelMode::Mono;
#endif

    analogReadResolution(12);
    analogSetPinAttenuation(DUCK_ADC_LEFT_PIN, DUCK_ADC_ATTEN);
    analogSetPinAttenuation(DUCK_ADC_RIGHT_PIN, DUCK_ADC_ATTEN);
    pinMode(DUCK_ADC_LEFT_PIN, INPUT);
    pinMode(DUCK_ADC_RIGHT_PIN, INPUT);

    for (int i = 0; i < 16; ++i) {
        analogRead(DUCK_ADC_LEFT_PIN);
        analogRead(DUCK_ADC_RIGHT_PIN);
        delayMicroseconds(50);
    }

    const uint32_t alarm_us = 1000000UL / sample_rate_hz_;

#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
    timer_ = timerBegin(1000000UL);
    if (!timer_) return false;
    timerAttachInterrupt(timer_, &OpticalInput::on_timer);
    timerAlarm(timer_, alarm_us, true, 0);
#else
    timer_ = timerBegin(0, 80, true);
    if (!timer_) return false;
    timerAttachInterrupt(timer_, &OpticalInput::on_timer, true);
    timerAlarmWrite(timer_, alarm_us, true);
    timerAlarmEnable(timer_);
#endif

    for (size_t i = 0; i < DUCK_SAMPLES_PER_BLOCK; ++i) {
        left_a_[i] = left_b_[i] = right_a_[i] = right_b_[i] = 0;
    }

    ready_ = true;
    return true;
}

void OpticalInput::set_channel_mode(OpticalChannelMode m) {
    channel_mode_ = m;
}

void IRAM_ATTR OpticalInput::on_timer() {
    OpticalInput* self = instance_;
    if (!self || !self->ready_) return;

    int raw_l = analogRead(DUCK_ADC_LEFT_PIN);
    if (raw_l < 0) raw_l = 0;
    if (raw_l > 4095) raw_l = 4095;

    int raw_r = 0;
    if (self->channel_mode_ == OpticalChannelMode::Stereo) {
        raw_r = analogRead(DUCK_ADC_RIGHT_PIN);
        if (raw_r < 0) raw_r = 0;
        if (raw_r > 4095) raw_r = 4095;
    }

    self->last_raw_l_ = static_cast<int16_t>(raw_l);
    self->last_raw_r_ = static_cast<int16_t>(raw_r);

    size_t idx = self->write_idx_;
    self->active_left_[idx]  = static_cast<int16_t>(raw_l);
    self->active_right_[idx] = static_cast<int16_t>(raw_r);
    idx++;

    if (idx >= DUCK_SAMPLES_PER_BLOCK) {
        int16_t* filled_l = const_cast<int16_t*>(self->active_left_);
        int16_t* filled_r = const_cast<int16_t*>(self->active_right_);
        const bool use_a = (filled_l == self->left_a_);
        self->active_left_  = use_a ? self->left_b_  : self->left_a_;
        self->active_right_ = use_a ? self->right_b_ : self->right_a_;
        self->latest_left_  = filled_l;
        self->latest_right_ = filled_r;
        self->block_ts_us_  = micros();
        self->write_idx_    = 0;
        self->block_ready_  = true;
    } else {
        self->write_idx_ = idx;
    }
}

bool OpticalInput::sample() {
    if (!block_ready_) return false;
    block_ready_ = false;

    const int16_t* L = latest_left_;
    const int16_t* R = latest_right_;
    int32_t sum_l = 0, sum_r = 0;
    int16_t mn_l = 4095, mx_l = 0, mn_r = 4095, mx_r = 0;

    for (size_t i = 0; i < DUCK_SAMPLES_PER_BLOCK; ++i) {
        const int16_t vl = L[i];
        const int16_t vr = R[i];
        sum_l += vl;
        sum_r += vr;
        if (vl < mn_l) mn_l = vl;
        if (vl > mx_l) mx_l = vl;
        if (vr < mn_r) mn_r = vr;
        if (vr > mx_r) mx_r = vr;
    }

    dc_level_l_     = static_cast<int16_t>(sum_l / static_cast<int32_t>(DUCK_SAMPLES_PER_BLOCK));
    dc_level_r_     = static_cast<int16_t>(sum_r / static_cast<int32_t>(DUCK_SAMPLES_PER_BLOCK));
    signal_level_l_ = static_cast<int16_t>(mx_l - mn_l);
    signal_level_r_ = static_cast<int16_t>(mx_r - mn_r);
    return true;
}

} // namespace duckoustic
