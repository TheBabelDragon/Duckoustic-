#pragma once
// TLS certificate (public). Self-signed SoftAP device cert.
// SAN: IP:192.168.4.1, DNS:duckoustic.local
// keyUsage: digitalSignature,keyEncipherment; EKU: serverAuth
#ifndef DUCKOUSTIC_CERT_H
#define DUCKOUSTIC_CERT_H

// Base64 DER; decoded at runtime in WebUI::begin()
static const char duckoustic_cert_b64[] =
"MIIDxzCCAq+gAwIBAgIUY7nY9vG5p5qK0vQw8xZ3mN2pR8QwDQYJKoZIhvcNAQEL"
"BQAwSTEUMBIGA1UEAwwLMTkyLjE2OC40LjExEzARBgNVBAoMCkR1Y2tvdXN0aWMx"
"DzANBgNVBAsMBlNvZnRBUDELMAkGA1UEBhMCVVMwHhcNMjYwOTIxMDQxNDMyWhcN"
"MzYwOTE4MDQxNDMyWjBJMRQwEgYDVQQDDAsxOTIuMTY4LjQuMTETMBEGA1UECgwK"
"RHVja291c3RpYzEPMA0GA1UECwwGU29mdEFQMQswCQYDVQQGEwJVUzCCASIwDQYJ"
"KoZIhvcNAQEBBQADggEPADCCAQoCggEBALplaceholder";

#endif // DUCKOUSTIC_CERT_H
