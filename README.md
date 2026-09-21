# Duckoustic

**ESP32-S3 optical microphone + dual-channel audio output**

```
BPW34 → GPIO4 ADC → Listen / Clone DSP
                    ↓
            LaserOutput (stereo PWM)
                    ↓
        GPIO5 → PAM IN-L    GPIO6 → PAM IN-R
```

---

## Status: v0.2.2 — Stereo PAM audio outputs (GPIO5 L / GPIO6 R) + CLONE + LISTEN

| Feature                         | Status |
|---------------------------------|--------|
| ADC acquisition (BPW34, GPIO4)  | ✅      |
| Wi-Fi SoftAP + browser UI       | ✅      |
| WAV upload (PCM 8 kHz mono 16)  | ✅      |
| CLONE: file → dual mono L+R     | ✅      |
| LISTEN: BPW34 → dual mono L+R   | ✅      |
| Stereo WAV / independent L/R    | later  |
| MP3 / AAC decode                | later  |

---

## Architecture

```
                 PHONE
          Safari / Chrome
                 |
          Wi-Fi / SoftAP
                 |
                 v
          ESP32-S3 web UI
                 |
        +--------+--------+
        |                 |
    CLONE              LISTEN
        |                 |
   WAV / LittleFS    BPW34 → GPIO4 ADC
        |                 |
        v                 v
   PlaybackEngine    ListenEngine
        |                 |
        +--------+--------+
                 |
                 v
      LaserOutput::write()  (mono → L=R)
                 |
        +--------+--------+
        |                 |
   GPIO5 PWM         GPIO6 PWM
   PAM IN-L          PAM IN-R
        |                 |
        +-------- PAM power stage --------+
                 L+/L-          R+/R-
```

### Modes

| Mode   | Source                         | Output                          |
|--------|--------------------------------|---------------------------------|
| LISTEN | BPW34 → ADC (live)             | dual mono → GPIO5 L + GPIO6 R   |
| CLONE  | uploaded WAV → PlaybackEngine  | dual mono → GPIO5 L + GPIO6 R   |

`write(float)` keeps the mono pipeline; both PAM inputs get the same sample. `write_stereo(L,R)` is available for future independent channels.

---

## Hardware

### Input path

```
optical field → BPW34 → AFE → ESP32 GPIO4 (ADC)
```

### Output path (stereo PAM **input** stage)

```
LaserOutput PWM
    +-- GPIO5 → PAM8403 IN-L
    +-- GPIO6 → PAM8403 IN-R
              |
         PAM power stage
              |
         L+/L-  R+/R-  → external load / modulator
```

### Electrical boundary

| Connection | Correct |
|------------|---------|
| ESP32 GPIO5 | → PAM8403 **IN-L** |
| ESP32 GPIO6 | → PAM8403 **IN-R** |
| ESP32 GND | → PAM8403 signal **GND** |
| GPIO5 / GPIO6 | **Must not** connect to PAM L+/L- or R+/R- |
| PAM L+/L-, R+/R- | Differential **power** outputs only |

GPIO5/6 are **PWM audio-source** signals, not a conventional DAC. An RC reconstruction / low-pass filter between each GPIO and the PAM input is recommended.

PWM carrier: **80 kHz**. Audio sample rate: **8 kHz** (optical ADC / Listen compatible).

---

## Quick start

```bash
git pull
pio run -t upload
pio device monitor
```

Expected serial lines:

```
ADC input:        GPIO4
Audio L PWM:      GPIO5
Audio R PWM:      GPIO6
PWM carrier:      80000 Hz
Audio sample rate: 8000 Hz
Boundary: GPIO5/6 → PAM IN-L/IN-R (not speaker terminals)
```

---

## Configuration (`include/config.h`)

| Macro | Default | Meaning |
|-------|---------|---------|
| `DUCK_ADC_PIN` | 4 | BPW34 → ADC |
| `DUCK_AUDIO_LEFT_PWM_PIN` | 5 | PAM IN-L |
| `DUCK_AUDIO_RIGHT_PWM_PIN` | 6 | PAM IN-R |
| `DUCK_AUDIO_PWM_FREQ_HZ` | 80000 | PWM carrier |
| `DUCK_AUDIO_PWM_RES_BITS` | 10 | Duty resolution |
| `DUCK_SAMPLE_RATE_HZ` | 8000 | Audio / ADC rate |
| `DUCK_LISTEN_SCALE` | 512 | LISTEN sensitivity |
| `DUCK_AUDIO_MAX_DUTY` | 0.85 | Per-channel safety |

---

## License

MIT
