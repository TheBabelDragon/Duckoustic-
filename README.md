# Duckoustic

**ESP32-S3 optical L/R receiver + SoftAP HTTPS WebUI**

## Connect from iPhone

1. Join **Duckoustic-XXXX**
2. Open **https://192.168.4.1/** (or **https://duckoustic.local/**)
3. Accept the **self-signed** cert warning if shown (SAN alone does not make iOS trust the cert)

No WiFiManager. No Internet. No captive portal.

## Optical receiver wiring (BPW34 L/R rows)

**No TIA. No anode-to-ground load.** Experimental front-end:

### LEFT BPW34 row
- Cathode rail → **+3.3 V**
- Anode rail → **GPIO4** (ADC)
- Anode rail → **10 kΩ series** → XH-A901 **L** input

### RIGHT BPW34 row
- Cathode rail → **+3.3 V**
- Anode rail → **GPIO7** (ADC)
- Anode rail → **10 kΩ series** → XH-A901 **R** input

The 10 kΩ parts are **series** resistors into the XH inputs, not pull-downs.

### Audio out
- GPIO5 → PAM8403 **IN-L**
- GPIO6 → PAM8403 **IN-R**
- PAM L+/L− and R+/R− are BTL/differential — **not** connected to ESP32 GND

### WebUI modes (`POST /api/mode`)
| Body | Behavior |
|------|----------|
| `listen` | GPIO4 → dual mono GPIO5+GPIO6 |
| `stereo` | GPIO4→GPIO5, GPIO7→GPIO6 (no summing) |
| `clone` | uploaded WAV → dual mono |

`/api/status` includes `optical.mode`, `left_adc`, `right_adc`, pins 4 and 7.

## Network

| Item | Value |
|------|--------|
| SoftAP IP | 192.168.4.1 |
| HTTPS | TCP 443 |
| mDNS | duckoustic.local |

## Build

```bash
git pull
pio run -t upload
```

## License

MIT
