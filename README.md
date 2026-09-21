# Duckoustic

**ESP32-S3 optical microphone + dual-channel audio output**

```
BPW34 → GPIO4 ADC → Listen / Clone DSP
                    ↓
            LaserOutput (stereo PWM)
                    ↓
        GPIO5 → PAM IN-L    GPIO6 → PAM IN-R

Phone → SoftAP Duckoustic-XXXX → https://192.168.4.1/  (TLS :443)
```

---

## Status: v0.2.2 — SoftAP **HTTPS** WebUI + stereo PAM + CLONE/LISTEN

| Feature                         | Status |
|---------------------------------|--------|
| ADC acquisition (BPW34, GPIO4)  | ✅      |
| SoftAP `Duckoustic-XXXX`        | ✅      |
| **HTTPS WebUI on TCP 443**      | ✅      |
| WAV upload (PCM 8 kHz mono 16)  | ✅      |
| CLONE / LISTEN → dual mono L+R  | ✅      |

---

### Phone / browser (HTTPS)

1. Power Duckoustic.
2. On the phone, join **Duckoustic-XXXX** (open SoftAP; no password by default).
3. Open **https://192.168.4.1/**
4. Accept / trust the **local self-signed certificate** warning if the browser presents one (expected on iOS/Safari).

**No WiFiManager. No Internet. No external router.**  
TLS terminates on the device. Relative `/api/*` fetches stay on HTTPS automatically.

---

## Architecture

```
iPhone
  |
  | Wi-Fi SoftAP
  v
Duckoustic-XXXX
  |
  | HTTPS / TCP 443
  v
https://192.168.4.1/
  |-- /api/status
  |-- /api/upload
  |-- /api/play | stop | laser | gain | mode | loop
```

Audio (unchanged):

```
BPW34 → GPIO4 ADC → ListenEngine / PlaybackEngine
                         |
              LaserOutput::write() (mono → L=R)
                         |
              GPIO5 PAM IN-L    GPIO6 PAM IN-R
```

PWM carrier **80 kHz**, audio sample rate **8 kHz**.

---

## Electrical boundary (PAM)

| ESP32 | PAM8403 |
|-------|---------|
| GPIO5 | **IN-L** |
| GPIO6 | **IN-R** |
| GND | signal GND |
| GPIO | **not** L+/L− or R+/R− power terminals |

---

## Build

```bash
git pull
pio run -t upload
pio device monitor
```

Expected serial:

```
SoftAP started
SoftAP SSID: Duckoustic-XXXX
SoftAP IP:   192.168.4.1
HTTPS server started
HTTPS port:  443
HTTPS URL:   https://192.168.4.1/
Ready — connect phone to SoftAP, open https://192.168.4.1/
```

Dependencies (pinned): `fhessel/esp32_https_server@1.0.0`, `fhessel/esp32_https_server_compat@1.0.0`.

---

## License

MIT
