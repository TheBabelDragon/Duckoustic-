#pragma once

#include <Arduino.h>

// ---------------------------------------------------------------------------
// Duckoustic v0.2 configuration
// All tunable parameters live here so the rest of the code stays clean.
// ---------------------------------------------------------------------------

namespace duckoustic {

// ---------------------------------------------------------------------------
// Optical input (BPW34 → AFE → ADC) — unchanged from v0.1
// ---------------------------------------------------------------------------
#ifndef DUCK_ADC_PIN
#define DUCK_ADC_PIN            4          // GPIO4 = ADC1_CH3
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
// Optical / laser output (PWM amplitude modulation)
// ---------------------------------------------------------------------------
#ifndef DUCK_LASER_PWM_PIN
#define DUCK_LASER_PWM_PIN      5          // GPIO5 — drive laser modulator / transistor
#endif

#ifndef DUCK_LASER_PWM_CHANNEL
#define DUCK_LASER_PWM_CHANNEL  0
#endif

#ifndef DUCK_LASER_PWM_FREQ_HZ
#define DUCK_LASER_PWM_FREQ_HZ  8000       // carrier; audio modulates duty
#endif

#ifndef DUCK_LASER_PWM_RES_BITS
#define DUCK_LASER_PWM_RES_BITS 10         // 0..1023
#endif

// Safety: maximum duty fraction (0.0–1.0) the laser is allowed to reach
#ifndef DUCK_LASER_MAX_DUTY
#define DUCK_LASER_MAX_DUTY     0.85f
#endif

// ---------------------------------------------------------------------------
// Audio storage / playback
// ---------------------------------------------------------------------------
#ifndef DUCK_AUDIO_PATH
#define DUCK_AUDIO_PATH         "/audio.wav"
#endif

// Maximum accepted upload size (bytes). 8 kHz mono 16-bit ≈ 16 kB/s
// 60 s → ~960 kB; leave headroom on a typical 2–4 MB LittleFS partition.
#ifndef DUCK_MAX_UPLOAD_BYTES
#define DUCK_MAX_UPLOAD_BYTES   (1024 * 1024)
#endif

// Required WAV format for v0.2 (decoder for other formats is later work)
#ifndef DUCK_WAV_CHANNELS
#define DUCK_WAV_CHANNELS       1
#endif

#ifndef DUCK_WAV_SAMPLE_RATE
#define DUCK_WAV_SAMPLE_RATE    8000
#endif

#ifndef DUCK_WAV_BITS
#define DUCK_WAV_BITS           16
#endif

// Playback buffer size in samples (kept small; refilled from LittleFS)
#ifndef DUCK_PLAYBACK_BUF_SAMPLES
#define DUCK_PLAYBACK_BUF_SAMPLES  512
#endif

// LISTEN passthrough: ADC counts (post-DC) that map to full-scale ±1.0
// Smaller → more sensitive. ~512 ≈ strong modulation from modest optical AC.
#ifndef DUCK_LISTEN_SCALE
#define DUCK_LISTEN_SCALE       512
#endif

// Default linear gain applied to PCM before laser modulation (0.0–1.0)
#ifndef DUCK_DEFAULT_GAIN
#define DUCK_DEFAULT_GAIN       0.6f
#endif

} // namespace duckoustic
