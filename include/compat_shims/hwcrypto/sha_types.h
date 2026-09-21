#pragma once
#include "esp_idf_version.h"
#if defined(ESP_IDF_VERSION_MAJOR) && (ESP_IDF_VERSION_MAJOR >= 4)
  #if __has_include("esp32/sha.h")
    #include "esp32/sha.h"
  #endif
#endif
