#pragma once

#include <Arduino.h>

// ---------------------------------------------------------------------------
// Duckoustic configuration
// ---------------------------------------------------------------------------

namespace duckoustic {

// ---------------------------------------------------------------------------
// Optical input (BPW34 anode rails → ADC)
// LEFT  anode → GPIO4; RIGHT anode → GPIO7; cathode rails → +3.3V (HW)
// ---------------------------------------------------------------------------
#ifndef DUCK_ADC_PIN
#define DUCK_ADC_PIN            4          // GPIO4 LEFT BPW34 anode rail
#endif

#ifndef DUCK_ADC_LEFT_PIN
#define DUCK_ADC_LEFT_PIN       DUCK_ADC_PIN
#endif

#ifndef DUCK_ADC_RIGHT_PIN
#define DUCK_ADC_RIGHT_PIN      7          // GPIO7 RIGHT BPW34 anode rail
#endif

#ifndef DUCK_ADC_ATTEN
#define DUCK_ADC_ATTEN          ADC_11db
#endif

// Optical receive mode: mono (GPIO4 only) or stereo (GPIO4 L + GPIO7 R)
enum class OpticalChannelMode : uint8_t {
    Mono   = 0,
    Stereo = 1
};
#ifndef DUCK_OPTICAL_DEFAULT_STEREO
#define DUCK_OPTICAL_DEFAULT_STEREO  0
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

#ifndef DUCK_AP_SSID_PREFIX
#define DUCK_AP_SSID_PREFIX     "Duckoustic-"
#endif

#ifndef DUCK_AP_PASSWORD
#define DUCK_AP_PASSWORD        ""
#endif

#ifndef DUCK_AP_CHANNEL
#define DUCK_AP_CHANNEL         1
#endif

#ifndef DUCK_AP_MAX_CONN
#define DUCK_AP_MAX_CONN        4
#endif

// Stereo audio PWM → PAM8403 INPUT (not speaker terminals)
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

#ifndef DUCK_AUDIO_PWM_FREQ_HZ
#define DUCK_AUDIO_PWM_FREQ_HZ      80000
#endif

#ifndef DUCK_AUDIO_PWM_RES_BITS
#define DUCK_AUDIO_PWM_RES_BITS     10
#endif

#ifndef DUCK_LASER_PWM_CHANNEL
#define DUCK_LASER_PWM_CHANNEL      DUCK_AUDIO_LEFT_PWM_CHANNEL
#endif

#ifndef DUCK_LASER_PWM_FREQ_HZ
#define DUCK_LASER_PWM_FREQ_HZ      DUCK_AUDIO_PWM_FREQ_HZ
#endif

#ifndef DUCK_LASER_PWM_RES_BITS
#define DUCK_LASER_PWM_RES_BITS     DUCK_AUDIO_PWM_RES_BITS
#endif

#ifndef DUCK_LASER_MAX_DUTY
#define DUCK_LASER_MAX_DUTY         0.85f
#endif

#ifndef DUCK_AUDIO_MAX_DUTY
#define DUCK_AUDIO_MAX_DUTY         DUCK_LASER_MAX_DUTY
#endif

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

#ifndef DUCK_LISTEN_SCALE
#define DUCK_LISTEN_SCALE       512
#endif

#ifndef DUCK_DEFAULT_GAIN
#define DUCK_DEFAULT_GAIN       0.6f
#endif

} // namespace duckoustic
