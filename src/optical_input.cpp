#include "optical_input.h"

namespace duckoustic {

OpticalInput* OpticalInput::instance_ = nullptr;

bool OpticalInput::initialize() {
    if (ready_) return true;

    instance_ = this;

    // Conservative ADC setup for ESP32-S3
    analogReadResolution(12);                     // 0…4095
    analogSetPinAttenuation(DUCK_ADC_PIN, DUCK_ADC_ATTEN);
    pinMode(DUCK_ADC_PIN, INPUT);

    // Warm-up reads so the first real samples are stable
    for (int i = 0; i < 16; ++i) {
        analogRead(DUCK_ADC_PIN);
        delayMicroseconds(50);
    }

    // Target sample period in microseconds
    const uint32_t alarm_us = 1000000UL / sample_rate_hz_;

#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
    // Arduino-ESP32 3.x: timerBegin(frequency_hz)
    timer_ = timerBegin(1000000UL);  // 1 MHz tick
    if (!timer_) {
        return false;
    }
    timerAttachInterrupt(timer_, &OpticalInput::on_timer);
    timerAlarm(timer_, alarm_us, true, 0);
#else
    // Arduino-ESP32 2.x: timerBegin(num, divider, countUp)
    // APB 80 MHz / divider 80 → 1 MHz tick; alarm every alarm_us ticks
    timer_ = timerBegin(0, 80, true);
    if (!timer_) {
        return false;
    }
    timerAttachInterrupt(timer_, &OpticalInput::on_timer, true);
    timerAlarmWrite(timer_, alarm_us, true);
    timerAlarmEnable(timer_);
#endif

    // Pre-fill so the first block is not all zeros
    for (size_t i = 0; i < DUCK_SAMPLES_PER_BLOCK; ++i) {
        buffer_a_[i] = 0;
        buffer_b_[i] = 0;
    }

    ready_ = true;
    return true;
}

void IRAM_ATTR OpticalInput::on_timer() {
    OpticalInput* self = instance_;
    if (!self || !self->ready_) return;

    // Fast path: single-channel oneshot read
    int raw = analogRead(DUCK_ADC_PIN);

    // Clamp to legal 12-bit range (safety net)
    if (raw < 0)     raw = 0;
    if (raw > 4095)  raw = 4095;

    size_t idx = self->write_idx_;
    self->active_[idx] = static_cast<int16_t>(raw);
    idx++;

    if (idx >= DUCK_SAMPLES_PER_BLOCK) {
        // Swap buffers
        int16_t* just_filled = const_cast<int16_t*>(self->active_);
        self->active_ = (just_filled == self->buffer_a_) ? self->buffer_b_
                                                         : self->buffer_a_;
        self->latest_block_ = just_filled;
        self->block_ts_us_  = micros();
        self->write_idx_    = 0;
        self->block_ready_  = true;
    } else {
        self->write_idx_ = idx;
    }
}

bool OpticalInput::sample() {
    if (!ready_ || !block_ready_) {
        return false;
    }

    // Consume the flag
    noInterrupts();
    block_ready_ = false;
    const int16_t* blk = latest_block_;
    interrupts();

    // Quick statistics on the finished block (still raw ADC counts)
    int32_t sum = 0;
    int16_t mn  = 4095;
    int16_t mx  = 0;
    for (size_t i = 0; i < DUCK_SAMPLES_PER_BLOCK; ++i) {
        int16_t v = blk[i];
        sum += v;
        if (v < mn) mn = v;
        if (v > mx) mx = v;
    }

    dc_level_     = static_cast<int16_t>(sum / static_cast<int32_t>(DUCK_SAMPLES_PER_BLOCK));
    signal_level_ = static_cast<int16_t>(mx - mn);

    return true;
}

} // namespace duckoustic
