# Duckoustic

**ESP32-S3 optical microphone + dual-channel audio output**

SoftAP HTTPS WebUI (no router, no Internet, no WiFiManager).

## Connect from iPhone

1. Power Duckoustic.
2. Join Wi-Fi **Duckoustic-XXXX** (open network).
3. Open **https://192.168.4.1/**
4. Optionally (Bonjour/mDNS): **https://duckoustic.local/**
5. Accept the **self-signed certificate** warning if iOS presents one (expected until the device CA is trusted).

TLS terminates on the ESP32 (TCP **443**). Relative `/api/*` fetches stay on HTTPS with the page.

## Network diagnostics

```
GET https://192.168.4.1/api/network
```

Example:

```json
{
  "ssid": "Duckoustic-A1B2",
  "ip": "192.168.4.1",
  "gateway": "192.168.4.1",
  "subnet": "255.255.255.0",
  "https_port": 443,
  "mdns": "duckoustic.local",
  "tls": true
}
```

## SoftAP

| Setting | Value |
|---------|--------|
| SSID | `Duckoustic-XXXX` (MAC suffix) |
| Password | open (`DUCK_AP_PASSWORD` empty) |
| Channel | 1 |
| Max clients | 4 |
| IP | **192.168.4.1** |
| Gateway | 192.168.4.1 |
| Subnet | 255.255.255.0 |

## Certificate

Self-signed RSA-2048 device cert embedded in firmware:

- SAN: `IP:192.168.4.1`, `DNS:duckoustic.local`
- keyUsage: digitalSignature, keyEncipherment
- extendedKeyUsage: serverAuth

iOS may warn until the cert is trusted; encryption still works after acceptance.

## Audio path (unchanged)

```
BPW34 → GPIO4 ADC → Listen / Clone
                 → GPIO5 PAM IN-L
                 → GPIO6 PAM IN-R
```

PWM carrier 80 kHz · sample rate 8 kHz.

## Build

```bash
git pull
pio run -t upload
pio device monitor
```

Expected serial:

```
mDNS: https://duckoustic.local/
HTTPS server listening on 443
Duckoustic network
------------------
SSID: Duckoustic-XXXX
AP IP: 192.168.4.1
HTTPS: https://192.168.4.1/
mDNS:  https://duckoustic.local/
TLS:   certificate loaded
```

## License

MIT
