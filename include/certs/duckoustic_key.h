#pragma once
// TLS private key (RSA 2048). Device-local SoftAP only.
// Do not reuse outside this firmware image.
#ifndef DUCKOUSTIC_KEY_H
#define DUCKOUSTIC_KEY_H

// Base64 DER; decoded at runtime in WebUI::begin()
static const char duckoustic_key_b64[] =
"MIIEvwIBADANBgkqhkiG9w0BAQEFAASCBKkwggSlAgEAAoIBAQDU8fI9JoYOPLow"
"ov28KANM7lfcBOHU950WYtkSjJmHkTqZucesNDlHAiuYAia5kLuIFhYy7YdKwW+5"
"Bk4apw3HPUWNQRykrim0s2ONIEDuYl1roNfEfwFbEttKLNuEOHEvzIMNdjZbvb5v"
"7RrFUQEH1hVaEdljeZ1HHDtsvQ29Am4Zbgmy8Bhkz0KEDp3wkFlkGCaduQqDtLI/"
"PkmdCj2K9gUrJ59QY6vC5sp2LituySF596040rHvO8I5Ype1X5NPC5pEh69nZNiq"
"6ZPRtqSVpoFMUJe7TfXJ6WEUlLBS0XG8oumb37zKYdv1mfNhvKdMtko2Zgl1OQHi"
"MWQSPeJ/AgMBAAECggEAPuHnr8klgyTJsr0fAqYqO1iS12UVr0cju02bXo/ocpEf"
"Ze/nl+GgHGj2qJ/SAbBMwKYNDiL4dcnjjRbgj5xPREvCszDMHZFUvYjP2AfzRLIF"
"dlf7/4mlw9gwq5nNhSEJw5y4AoyrBrJkfzbcVfJSxJainYNx1589I5ehsL2vReXc"
"W+5VrIJ+vI3zKG58HQGyimr/4aPocO+EppBp0RWptSLh2Q916jwyehNK/ldRaPPw"
"dMzFG6qJT9VSdPpP3k6fndVXEyFhoKNfq7OzYAm2gWD5I/BMlZ7kvQYp1pPv1iDg"
"glz88Wq0ZKAm0FLf3NYbq1A4rZlpgzjVPasBvT9k8QKBgQD/Y0pQAzSs9YMd/qP5"
"RNodNzOcq5C3tNfvsJEbxOSaRTz9Uq+8sYev10yWABaf1ernBcVC+/8mdputqzDr"
"AvSo3EVAQnU47xtPa+p4aXryfDeacAa0I6A4+gCALqkIQxqht9ol8SlJnYzovJTu"
"XoE13G/jpNvaAKJ/9YJD2c2M6QKBgQDVdJzK4oMJ++azlt5yTjgqbAXADYrHH42f"
"b6SW0IV6Ee16xUHc23EUbZU+WO76gntYHUX0L5OgtdlXEx/fMWYxx83yFVNxCG6R"
"+brt6BR0WaYAoTGVGVvg31/aLogVnTrwF92UvfbHoYw+XUFIla3OGDfZnTgGd4h+"
"Ns9iJSMzJwKBgQCx/RYum31byaP/BJhtQqIs2GsdFqk+j0grocgQLXIRXo36E2Wt"
"QaFKWOfkZt1dui4Doo7vd0dFHEa1i+kE8UNoaFPPTEH8UxCakqCQGRt54j6yYL8b"
"TvT8oKg2rc5alsH50XO1G9Vlm+s3zS6SzsSIsEA+zIf48JZQTd/iv+o0+QKBgQDM"
"UCRMJmgbRFxchgpCCxDDukX+wHeXhWI/u9Mk8E7yuvo+y1Txm7IvRXzXArUsX3zE"
"qLBuvKNn7ugr6ohVUKlq+L7t+u+1f8amPw1n7v61J6I3dBNLjcxeADF1W5iyZ58k"
"dROI7hOqnbzpbMWyPmLwpEnBKDWaKwwhE0GJxdZdfQKBgQCZLI/WYmfaSTqhzR4+"
"tWHxBamy4QWOl8w9t7/tZHjn7O1MXZ2ko3UAiUKByN2jEsp0E+6DQ5cub+kCeEg2"
"RD5GjZV+og/q5uTyFRotZPsb7vwiININCPllC6+IRgHAQJbkblyQDbet044Sn3HC"
"iTK2r1LWSesLHBWTse5eMbGw9g==";

#endif // DUCKOUSTIC_KEY_H
