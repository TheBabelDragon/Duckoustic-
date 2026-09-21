#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <ESPWebServerSecure.hpp>
#include "config.h"
#include "playback_engine.h"
#include "listen_engine.h"
#include "laser_output.h"
#include "optical_input.h"
#include "signal_processor.h"

namespace duckoustic {

/**
 * SoftAP + HTTPS browser UI (TLS on TCP 443).
 *
 * Endpoints (all over HTTPS):
 *   GET  /           → HTML UI
 *   GET  /api/status → JSON status
 *   POST /api/upload → multipart WAV upload
 *   POST /api/play   → start playback (CLONE)
 *   POST /api/stop   → stop playback
 *   POST /api/laser  → body: on|off
 *   POST /api/gain   → body: 0.0–1.0
 *   POST /api/mode   → body: listen|clone
 *   POST /api/loop   → body: 0|1
 *
 * Self-signed device cert for https://192.168.4.1/ — browsers may warn.
 * No external CA, router, or Internet required.
 */
class WebUI {
public:
    WebUI() : server_(443) {}

    bool begin(PlaybackEngine* playback, LaserOutput* laser,
               OpticalInput* optical, SignalProcessor* processor,
               ListenEngine* listen);

    /** Call from loop() — services HTTPS clients. */
    void handle();

    enum class Mode : uint8_t { Listen = 0, Clone = 1 };
    Mode mode() const { return mode_; }

private:
    void setup_routes();
    void start_softap();
    String make_ssid() const;
    void apply_mode(Mode m);

    void handle_root();
    void handle_status();
    void handle_upload();
    void handle_upload_finish();
    void handle_play();
    void handle_stop();
    void handle_laser();
    void handle_gain();
    void handle_mode();
    void handle_loop();
    void handle_not_found();

    ESPWebServerSecure server_;
    PlaybackEngine*  playback_  = nullptr;
    ListenEngine*    listen_    = nullptr;
    LaserOutput*     laser_     = nullptr;
    OpticalInput*    optical_   = nullptr;
    SignalProcessor* processor_ = nullptr;
    Mode             mode_      = Mode::Clone;
    bool             loop_en_   = false;
    String           last_filename_;
    bool             upload_ok_ = false;
};

} // namespace duckoustic
