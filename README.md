# Duckoustic

**ESP32-S3 optical microphone + optical audio player**

```
Laser → vibrating reflective target → BPW34 → AFE → ESP32-S3 ADC
                                                      ↕
Phone browser ── Wi-Fi SoftAP ── upload WAV / PLAY ── Laser PWM
```

---

## Status: v0.2 — Browser Audio Upload + Optical Playback

| Feature                    | v0.1 | v0.2 |
|----------------------------|------|------|
| ADC acquisition (BPW34)    | ✅   | ✅   |
| Timer-driven sampling      | ✅   | ✅   |
| Block statistics           | ✅   | ✅   |
| Serial diagnostics         | ✅   | ✅   |
| Wi-Fi SoftAP               | ❌   | ✅   |
| Browser UI                 | ❌   | ✅   |
| WAV upload (PCM 8 kHz)     | ❌   | ✅   |
| PlaybackEngine → laser PWM | ❌   | ✅   |
| Listen / Clone modes       | ❌   | ✅   |
| MP3 / AAC decode           | ❌   | later |
| Classic BT / A2DP          | ❌   | not planned (S3 has no Classic BT) |
| BLE control                | ❌   | later (discovery / config only) |

**Why Wi-Fi first, not Bluetooth?**  
ESP32-S3 has 2.4 GHz Wi-Fi and BLE, but **no Classic Bluetooth / A2DP**. BLE is awkward for arbitrary audio-file upload. SoftAP + browser gives:

> Power on → join `Duckoustic-XXXX` → open `http://192.168.4.1/` → choose WAV → PLAY.

No app. No pairing. No router.

---

## Architecture

```
                 PHONE
          Safari / Chrome
                 │
          Wi-Fi / SoftAP
                 │
                 ▼
          ESP32-S3 web UI
                 │
        ┌────────┐
        │                 │
    upload audio      controls
        │          play / stop / loop
        ▼
   LittleFS (audio.wav)
        │
        ▼
   PlaybackEngine  ──►  LaserOutput (PWM)
                              │
                              ▼
                         optical path
                              │
                              ▼
                           BPW34 → ADC   (LISTEN telemetry)
```

Firmware layers (intentionally decoupled):

```
web/          SoftAP, HTTP, upload, status, control
audio/        WavReader, PlaybackEngine
optical/      LaserOutput (modulation + safety limits)
input/        OpticalInput (BPW34 ADC) — unchanged from v0.1
telemetry/    serial diagnostics
```

`PlaybackEngine` does **not** know about Wi-Fi. Later it can be fed from live ADC, BLE chunks, or a learned clone representation without touching the web layer.

### Modes

| Mode   | Source                         | Output              |
|--------|--------------------------------|---------------------|
| LISTEN | BPW34 → ADC (live)             | telemetry only*     |
| CLONE  | uploaded WAV → PlaybackEngine  | LaserOutput (PWM)   |

\* Future: LISTEN can drive the same `LaserOutput` for a true optical passthrough / feedback experiment.

---

## Hardware

### Input path (v0.1, still active)

```
laser (external power / external enable)
    ↓
reflective / vibrating target
    ↓
BPW34 photodiode
    ↓
analog front-end (e.g. XH-A901 / NE5532 stage)
    ↓
ESP32-S3 ADC  (default GPIO4)
```

### Output path (v0.2)

```
PlaybackEngine PCM
    ↓
LaserOutput  (PWM, default GPIO5)
    ↓
laser modulator / driver transistor
    ↓
optical field  →  (optional) BPW34 again for closed-loop experiments
```

Optional acoustic monitoring (not required for v0.2 firmware):

```
AFE tap → XH-A901 → PAM8403 → speaker
```

### Safety

- Laser enable is software-gated (`LaserOutput::set_enabled`).
- PWM duty is hard-capped by `DUCK_LASER_MAX_DUTY` (default 85 %).
- Firmware never assumes the laser is eye-safe; treat the optical path as a Class-appropriate laser system under your control.

---

## Quick start

### Requirements

- PlatformIO
- ESP32-S3 board (`esp32-s3-devkitc-1` or compatible)
- BPW34 + AFE on `DUCK_ADC_PIN` (default GPIO4)
- Laser modulator on `DUCK_LASER_PWM_PIN` (default GPIO5)

### Build & flash

```bash
pio run -t upload
pio run -t uploadfs   # only if you later add data/ assets; UI is embedded
pio device monitor
```

### Phone workflow

1. Power the board.
2. Join Wi-Fi network **`Duckoustic-XXXX`** (XXXX = last 4 hex of MAC). Open network by default.
3. Open **http://192.168.4.1/**
4. Choose a **mono 8 kHz 16-bit PCM WAV**.
5. PLAY. Toggle laser, gain, Listen / Clone as needed.

### WAV contract (v0.2)

| Parameter     | Required        |
|---------------|-----------------|
| Container     | RIFF WAVE       |
| Format        | PCM (format 1)  |
| Channels      | 1 (mono)        |
| Sample rate   | 8000 Hz         |
| Bits          | 16 signed LE    |

Other rates / codecs can be added later behind a decoder → PCM stage; the playback engine stays the same.

---

## Configuration

All tunables: `include/config.h`

| Macro | Default | Meaning |
|-------|---------|---------|
| `DUCK_ADC_PIN` | 4 | BPW34 AFE → ADC |
| `DUCK_SAMPLE_RATE_HZ` | 8000 | Acquisition rate |
| `DUCK_LASER_PWM_PIN` | 5 | Laser modulator PWM |
| `DUCK_LASER_MAX_DUTY` | 0.85 | Safety duty ceiling |
| `DUCK_AP_SSID_PREFIX` | `Duckoustic-` | SoftAP name prefix |
| `DUCK_AP_PASSWORD` | `""` | Empty = open |
| `DUCK_AUDIO_PATH` | `/audio.wav` | LittleFS path |
| `DUCK_MAX_UPLOAD_BYTES` | 1 MiB | Upload size limit |
| `DUCK_DEFAULT_GAIN` | 0.6 | Initial laser gain |

---

## Serial banner (expected)

```
DUCKOUSTIC v0.2
BPW34 INPUT: READY
LASER OUTPUT: PWM
TRANSPORT: Wi-Fi SoftAP + browser
TARGET: optical vibration + clone playback

SoftAP SSID: Duckoustic-A1B2
SoftAP IP:   192.168.4.1
HTTP server on http://192.168.4.1/
ADC pin GPIO4  sample_rate=8000 Hz  block=256 samples
Laser PWM pin GPIO5
Ready — connect phone to SoftAP, open http://192.168.4.1/
```

---

## Acceptance criteria (v0.2)

- [x] SoftAP comes up as `Duckoustic-XXXX` at 192.168.4.1
- [x] Browser UI loads with no external CDN
- [x] Valid mono 8 kHz 16-bit WAV uploads to LittleFS
- [x] Invalid WAV is rejected with a clear JSON error
- [x] PLAY drives `LaserOutput` PWM at sample rate
- [x] STOP idles the laser output
- [x] Gain and laser enable are controllable from the UI
- [x] Listen / Clone mode switch exists (Clone uses file; Listen keeps ADC path)
- [x] v0.1 optical acquisition + serial telemetry still run
- [x] `PlaybackEngine` has no Wi-Fi includes

**Definition of done:** *Phone → browser → WAV → optical clone.*

---

## Roadmap notes

- **MP3/AAC**: decode → PCM → same `PlaybackEngine`.
- **BLE**: discovery, provisioning, small control messages — not primary audio transport.
- **Clone feedback**: LISTEN optical signal → estimate transfer function → compensated CLONE output.
- **Speaker path**: optional DAC/I2S → PAM8403 for acoustic monitoring of the same buffer.

---

## License

MIT
