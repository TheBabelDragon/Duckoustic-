#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <ESPmDNS.h>
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
 * Primary:  https://192.168.4.1/
 * Secondary: https://duckoustic.local/
 *
 * Modes via POST /api/mode body:
 *   clone | listen | stereo
 * stereo = Listen + OpticalChannelMode::Stereo (GPIO4 L, GPIO7 R).
 */
class WebUI {
public:
    WebUI() : server_(443) {}

    bool begin(PlaybackEngine* playback, LaserOutput* laser,
               OpticalInput* optical, SignalProcessor* processor,
               ListenEngine* listen);

    void handle();

    enum class Mode : uint8_t { Listen = 0, Clone = 1 };
    Mode mode() const { return mode_; }

private:
    void setup_routes();
    bool start_softap();
    String make_ssid() const;
    void apply_mode(Mode m);

    void handle_root();
    void handle_status();
    void handle_network();
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
    bool             optical_stereo_ = false;
    bool             loop_en_   = false;
    String           last_filename_;
    bool             upload_ok_ = false;
    bool             tls_ok_    = false;
    String           ssid_;
};

} // namespace duckoustic
