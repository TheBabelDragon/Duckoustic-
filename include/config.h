#pragma once

#include <Arduino.h>

// ---------------------------------------------------------------------------
// Duckoustic v0.2.2 configuration
// All tunable parameters live here so the rest of the code stays clean.
// ---------------------------------------------------------------------------

namespace duckoustic {

// ---------------------------------------------------------------------------
// Optical input (BPW34 → AFE → ADC) — GPIO4 KEEP
// ---------------------------------------------------------------------------
#ifndef DUCK_ADC_PIN
#define DUCK_ADC_PIN            4          // GPIO4 = ADC1_CH3 (BPW34 array)
#endif

#ifndef DUCK_ADC_ATTEN
#define DUCK_ADC_ATTEN          ADC_11db
#endif

#ifndef DUCK_SAMPLE_RATE_HZ
#define DUCK_SAMPLE_RATE_HZ     8000
#endif

#ifndef DUCK_SAMPLES_PER_BLOCK
#define DUCK_SAMPLES_PER_BLOCK  256
#endif

#ifndef DUCK_DC_REMOVAL
#define DUCK_DC_REMOVAL         true
#endif

#ifndef DUCK_SIGNAL_THRESHOLD
#define DUCK_SIGNAL_THRESHOLD   40
#endif

#ifndef DUCK_TELEMETRY_INTERVAL_MS
#define DUCK_TELEMETRY_INTERVAL_MS  250
#endif

#ifndef DUCK_RAW_DEBUG
#define DUCK_RAW_DEBUG          false
#endif

#ifndef DUCK_SERIAL_BAUD
#define DUCK_SERIAL_BAUD        115200
#endif

// ---------------------------------------------------------------------------
// SoftAP / Wi-Fi
// ---------------------------------------------------------------------------
#ifndef DUCK_AP_SSID_PREFIX
#define DUCK_AP_SSID_PREFIX     "Duckoustic-"
#endif

#ifndef DUCK_AP_PASSWORD
#define DUCK_AP_PASSWORD        ""         // open network (empty)
#endif

#ifndef DUCK_AP_CHANNEL
#define DUCK_AP_CHANNEL         1
#endif

#ifndef DUCK_AP_MAX_CONN
#define DUCK_AP_MAX_CONN        4
#endif

// ---------------------------------------------------------------------------
// Stereo audio PWM outputs → PAM8403 INPUT stage (not speaker terminals)
//
//   ESP32 GPIO5 → PAM8403 IN-L
//   ESP32 GPIO6 → PAM8403 IN-R
//   ESP32 GND   → PAM8403 signal GND
//
// PAM L+/L- and R+/R- are differential power outputs — never wire to GPIO.
// An RC reconstruction filter between GPIO and PAM input is recommended.
// ---------------------------------------------------------------------------

// Legacy alias (left channel) — kept so older references still resolve
#ifndef DUCK_LASER_PWM_PIN
#define DUCK_LASER_PWM_PIN          5
#endif

#ifndef DUCK_AUDIO_LEFT_PWM_PIN
#define DUCK_AUDIO_LEFT_PWM_PIN     5
#endif

#ifndef DUCK_AUDIO_RIGHT_PWM_PIN
#define DUCK_AUDIO_RIGHT_PWM_PIN    6
#endif

#ifndef DUCK_AUDIO_LEFT_PWM_CHANNEL
#define DUCK_AUDIO_LEFT_PWM_CHANNEL  0
#endif

#ifndef DUCK_AUDIO_RIGHT_PWM_CHANNEL
#define DUCK_AUDIO_RIGHT_PWM_CHANNEL 1
#endif

// PWM carrier must be substantially above the audio sample rate (8 kHz)
#ifndef DUCK_AUDIO_PWM_FREQ_HZ
#define DUCK_AUDIO_PWM_FREQ_HZ      80000
#endif

#ifndef DUCK_AUDIO_PWM_RES_BITS
#define DUCK_AUDIO_PWM_RES_BITS     10         // 0..1023
#endif

// Legacy aliases → audio PWM
#ifndef DUCK_LASER_PWM_CHANNEL
#define DUCK_LASER_PWM_CHANNEL      DUCK_AUDIO_LEFT_PWM_CHANNEL
#endif

#ifndef DUCK_LASER_PWM_FREQ_HZ
#define DUCK_LASER_PWM_FREQ_HZ      DUCK_AUDIO_PWM_FREQ_HZ
#endif

#ifndef DUCK_LASER_PWM_RES_BITS
#define DUCK_LASER_PWM_RES_BITS     DUCK_AUDIO_PWM_RES_BITS
#endif

// Safety: maximum duty fraction (0.0–1.0) applied independently per channel
#ifndef DUCK_LASER_MAX_DUTY
#define DUCK_LASER_MAX_DUTY         0.85f
#endif

#ifndef DUCK_AUDIO_MAX_DUTY
#define DUCK_AUDIO_MAX_DUTY         DUCK_LASER_MAX_DUTY
#endif

// ---------------------------------------------------------------------------
// Audio storage / playback
// ---------------------------------------------------------------------------
#ifndef DUCK_AUDIO_PATH
#define DUCK_AUDIO_PATH         "/audio.wav"
#endif

#ifndef DUCK_MAX_UPLOAD_BYTES
#define DUCK_MAX_UPLOAD_BYTES   (1024 * 1024)
#endif

#ifndef DUCK_WAV_CHANNELS
#define DUCK_WAV_CHANNELS       1
#endif

#ifndef DUCK_WAV_SAMPLE_RATE
#define DUCK_WAV_SAMPLE_RATE    8000
#endif

#ifndef DUCK_WAV_BITS
#define DUCK_WAV_BITS           16
#endif

#ifndef DUCK_PLAYBACK_BUF_SAMPLES
#define DUCK_PLAYBACK_BUF_SAMPLES  512
#endif

// LISTEN passthrough: ADC counts (post-DC) that map to full-scale ±1.0
#ifndef DUCK_LISTEN_SCALE
#define DUCK_LISTEN_SCALE       512
#endif

#ifndef DUCK_DEFAULT_GAIN
#define DUCK_DEFAULT_GAIN       0.6f
#endif

} // namespace duckoustic
