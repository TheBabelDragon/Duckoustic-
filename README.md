# Duckoustic

**ESP32-S3 optical microphone firmware**

Laser → vibrating reflective target → BPW34 → analog front end → ESP32-S3 ADC

---

## Status: v0.1 — "ESP32-S3 can see the duck"

This is a hardware-first milestone. The firmware proves that the ESP32-S3 can acquire a usable optical vibration signal. Nothing else is implemented yet.

| Feature              | v0.1 |
|----------------------|------|
| ADC acquisition      | ✅   |
| Timer-driven sampling| ✅   |
| Block statistics     | ✅   |
| Serial diagnostics   | ✅   |
| Signal / no-signal   | ✅   |
| Speaker output       | ❌   |
| FFT / spectrum       | ❌   |
| Networking           | ❌   |
| Servo / laser control| ❌   |

---

## Hardware test path

```
laser (externally powered)
    ↓
reflective / vibrating target
    ↓
reflected light
    ↓
BPW34 photodiode
    ↓
external analog front-end
  (e.g. XH-A901 board with NE5532 stage —
   treated as a black-box gain/buffer stage;
   not assumed to be a classic TIA)
    ↓
ESP32-S3 ADC (configurable GPIO)
```

The firmware **never** drives the laser. The laser is under external control. The ESP32-S3 only observes the BPW34 signal after the analog front-end.

### Safety notes

- Keep the analog front-end output within the ESP32-S3 ADC input range (see `DUCK_ADC_ATTEN`).
- Default attenuation is set conservatively (`ADC_11db` ≈ 0–3.1 V).
- Do not exceed the absolute maximum ratings of the GPIO.

---

## Quick start

### Requirements

- PlatformIO
- ESP32-S3 board (tested target: `esp32-s3-devkitc-1`)
- BPW34 (or equivalent) + analog front-end wired to the configured ADC pin

### Build & flash

```bash
pio run -t upload
pio device monitor
```

### Expected serial output

```
DUCKOUSTIC v0.1
BPW34 INPUT: READY
LASER INPUT: EXTERNAL
TARGET: OPTICAL VIBRATION

ADC pin GPIO4  sample_rate=8000 Hz  block=256 samples
Acquisition running…

DUCKOUSTIC sample_rate=8000 dc=1842 min=1831 max=1855 peak_to_peak=24 rms=4.2 signal=NO_SIGNAL
DUCKOUSTIC sample_rate=8000 dc=1840 min=1720 max=1965 peak_to_peak=245 rms=38.1 signal=SIGNAL_DETECTED
```

- With the laser off / beam blocked you should see low `peak_to_peak` and `NO_SIGNAL`.
- Blocking/unblocking the BPW34 or vibrating the reflective target should raise `peak_to_peak` and flip the flag to `SIGNAL_DETECTED`.

---

## Configuration

All tunables live in `include/config.h`:

| Macro                      | Default | Meaning                                      |
|----------------------------|---------|----------------------------------------------|
| `DUCK_ADC_PIN`             | 4       | GPIO connected to AFE output (ADC1 channel)  |
| `DUCK_ADC_ATTEN`           | ADC_11db| Attenuation / full-scale range               |
| `DUCK_SAMPLE_RATE_HZ`      | 8000    | Target sample rate                           |
| `DUCK_SAMPLES_PER_BLOCK`   | 256     | Samples per statistics block                 |
| `DUCK_DC_REMOVAL`          | true    | Subtract block mean before RMS               |
| `DUCK_SIGNAL_THRESHOLD`    | 40      | peak-to-peak counts → SIGNAL_DETECTED        |
| `DUCK_TELEMETRY_INTERVAL_MS`| 250    | Serial report interval                       |
| `DUCK_RAW_DEBUG`           | false   | Reserved for optional raw dumps              |

---

## Architecture (kept intentionally small)

```
OpticalInput          ← timer ISR + double-buffer ADC acquisition
    ↓
SignalProcessor       ← DC / min / max / peak-to-peak / RMS / threshold
    ↓
Telemetry             ← periodic serial diagnostics

Configuration         ← include/config.h
```

Future versions can insert stages between OpticalInput and any audio/FFT/network layer without touching the ADC code:

```
OpticalInput
    ↓
DC removal / filtering
    ↓
Audio buffer
    ↓
FFT / spectral analysis
    ↓
audio output / visualisation / network stream
```

---

## Acceptance criteria (v0.1)

- [x] ESP32-S3 boots and prints the banner
- [x] ADC acquisition runs continuously at the configured rate
- [x] Serial diagnostics show stable values with the optical path quiet
- [x] Blocking / unblocking the BPW34 changes the reported signal level
- [x] Moving / vibrating the reflective target produces measurable variation
- [x] Firmware stays responsive during continuous acquisition
- [x] No ADC value is reported outside the configured safe range
- [x] Acquisition layer is cleanly separated from signal processing & telemetry

**Definition of done:** *ESP32-S3 can see the duck.*

---

## License

MIT (or project default)
