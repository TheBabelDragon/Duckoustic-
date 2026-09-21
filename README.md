# Duckoustic

**ESP32-S3 optical microphone + optical audio player**

```
Laser → vibrating reflective target → BPW34 → AFE → ESP32-S3 ADC
                                                      ↕
Phone browser ── Wi-Fi SoftAP ── upload WAV / PLAY ── Laser PWM
```

---

## Status: v0.2.1 — Browser upload + CLONE + LISTEN optical passthrough

| Feature                         | Status |
|---------------------------------|--------|
| ADC acquisition (BPW34)         | ✅      |
| Wi-Fi SoftAP + browser UI       | ✅      |
| WAV upload (PCM 8 kHz mono 16)  | ✅      |
| CLONE: file → LaserOutput       | ✅      |
| LISTEN: BPW34 → LaserOutput     | ✅ v0.2.1 |
| MP3 / AAC decode                | later  |
| Classic BT / A2DP               | not on S3 |
| BLE control                     | later  |

**Why Wi-Fi first, not Bluetooth?**  
ESP32-S3 has 2.4 GHz Wi-Fi and BLE, but **no Classic Bluetooth / A2DP**. SoftAP + browser:

> Power on → join `Duckoustic-XXXX` → open `http://192.168.4.1/` → choose WAV → PLAY.

No app. No pairing. No router.

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
   WAV / LittleFS    BPW34 → ADC
        |                 |
        v                 v
   PlaybackEngine    ListenEngine
        |                 |
        +--------+--------+
                 |
                 v
           LaserOutput (PWM)
                 |
                 v
            optical field
                 |
                 v
              BPW34 (again)
```

Firmware layers (intentionally decoupled):

```
web/          SoftAP, HTTP, upload, status, control
audio/        WavReader, PlaybackEngine
listen/       ListenEngine (live optical passthrough)
optical/      LaserOutput (modulation + safety limits)
input/        OpticalInput (BPW34 ADC)
telemetry/    serial diagnostics
```

`PlaybackEngine` and `ListenEngine` do **not** know about Wi-Fi. They share one `LaserOutput`; mode switch is exclusive.

### Modes

| Mode   | Source                         | Output                          |
|--------|--------------------------------|---------------------------------|
| LISTEN | BPW34 → ADC (live)             | LaserOutput (PWM) passthrough   |
| CLONE  | uploaded WAV → PlaybackEngine  | LaserOutput (PWM)               |

**LISTEN passthrough:** when mode is Listen and laser is ON, each acquired ADC block is DC-removed, scaled by `DUCK_LISTEN_SCALE`, and streamed to the same `LaserOutput` used by Clone. That closes the optical loop for feedback experiments (vibrate target → measure → re-modulate laser → observe again).

Tune sensitivity with `DUCK_LISTEN_SCALE` (default 512 ADC counts → full scale). Smaller = more sensitive. Gain slider still applies.

---

## Hardware

### Input path

```
laser (or ambient optical field)
    |
reflective / vibrating target
    |
BPW34 photodiode
    |
analog front-end (e.g. XH-A901 / NE5532)
    |
ESP32-S3 ADC  (default GPIO4)
```

### Output path

```
PlaybackEngine or ListenEngine
    |
LaserOutput  (PWM, default GPIO5)
    |
laser modulator / driver transistor
    |
optical field  →  (optional) BPW34 again for closed-loop experiments
```

### Safety

- Laser enable is software-gated (`LaserOutput::set_enabled`).
- PWM duty is hard-capped by `DUCK_LASER_MAX_DUTY` (default 85 %).
- LISTEN only emits while laser is ON (no accidental optical loop with laser disabled).

---

## Quick start

```bash
pio run -t upload
pio device monitor
```

1. Join Wi-Fi **`Duckoustic-XXXX`**
2. Open **http://192.168.4.1/**
3. **Clone:** upload mono 8 kHz 16-bit PCM WAV → PLAY
4. **Listen:** select Listen (passthrough) → enable Laser → optical AC drives the laser

### WAV contract (CLONE)

| Parameter     | Required        |
|---------------|-----------------|
| Container     | RIFF WAVE       |
| Format        | PCM (format 1)  |
| Channels      | 1 (mono)        |
| Sample rate   | 8000 Hz         |
| Bits          | 16 signed LE    |

---

## Configuration (`include/config.h`)

| Macro | Default | Meaning |
|-------|---------|---------|
| `DUCK_ADC_PIN` | 4 | BPW34 AFE → ADC |
| `DUCK_SAMPLE_RATE_HZ` | 8000 | Acquisition rate |
| `DUCK_LASER_PWM_PIN` | 5 | Laser modulator PWM |
| `DUCK_LASER_MAX_DUTY` | 0.85 | Safety duty ceiling |
| `DUCK_LISTEN_SCALE` | 512 | ADC counts → full-scale in LISTEN |
| `DUCK_DEFAULT_GAIN` | 0.6 | Initial laser gain |
| `DUCK_AP_SSID_PREFIX` | `Duckoustic-` | SoftAP name prefix |

---

## LISTEN experiment notes

1. Start with **laser OFF**, mode **Listen** — confirm serial shows optical signal when you vibrate the target.
2. Enable laser at **low gain**.
3. Status line should show **PASSTHROUGH**.
4. If the loop runs away (optical howl), lower gain or increase `DUCK_LISTEN_SCALE`.
5. Next milestone: estimate transfer function while listening, then compensate in Clone.

---

## License

MIT
