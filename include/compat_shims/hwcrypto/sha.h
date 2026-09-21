#pragma once
/**
 * Compatibility shim for esp32_https_server (and similar) on modern
 * Arduino-ESP32 / ESP-IDF toolchains where <hwcrypto/sha.h> was removed.
 *
 * Old path:  #include <hwcrypto/sha.h>
 * IDF 4.x:   #include <esp32/sha.h>
 * IDF 5.x:   hardware SHA headers under sha/ or use mbedtls
 */
#include "esp_idf_version.h"

#if defined(ESP_IDF_VERSION_MAJOR) && (ESP_IDF_VERSION_MAJOR >= 5)
  #if __has_include("sha/sha_parallel_engine.h")
    #include "sha/sha_parallel_engine.h"
  #elif __has_include("esp32/sha.h")
    #include "esp32/sha.h"
  #elif __has_include("mbedtls/sha256.h")
    #include "mbedtls/sha256.h"
  #endif
#elif defined(ESP_IDF_VERSION_MAJOR) && (ESP_IDF_VERSION_MAJOR >= 4)
  #include "esp32/sha.h"
#else
  #include_next <hwcrypto/sha.h>
#endif
