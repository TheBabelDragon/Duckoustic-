#include "web_ui.h"
#include <ArduinoJson.h>

namespace duckoustic {

// ---------------------------------------------------------------------------
// Embedded minimal UI (no external assets)
// ---------------------------------------------------------------------------
static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>DUCKOUSTIC</title>
<style>
  :root { --bg:#0d1117; --card:#161b22; --fg:#e6edf3; --muted:#8b949e; --acc:#3fb950; --warn:#d29922; --border:#30363d; }
  * { box-sizing:border-box; }
  body { margin:0; font-family:system-ui,-apple-system,sans-serif; background:var(--bg); color:var(--fg); min-height:100vh; display:flex; justify-content:center; padding:1.25rem; }
  .wrap { width:100%; max-width:420px; }
  h1 { font-size:1.35rem; letter-spacing:.12em; margin:0 0 1rem; text-align:center; color:var(--acc); }
  .card { background:var(--card); border:1px solid var(--border); border-radius:12px; padding:1rem 1.1rem; margin-bottom:.85rem; }
  label { display:block; font-size:.75rem; color:var(--muted); margin-bottom:.35rem; text-transform:uppercase; letter-spacing:.06em; }
  .row { display:flex; gap:.5rem; flex-wrap:wrap; margin-top:.6rem; }
  button, .file-btn { flex:1; min-width:90px; padding:.65rem .8rem; border-radius:8px; border:1px solid var(--border); background:#21262d; color:var(--fg); font-size:.9rem; cursor:pointer; }
  button:active, .file-btn:active { transform:scale(.98); }
  button.primary { background:var(--acc); color:#0d1117; border-color:var(--acc); font-weight:600; }
  button.danger { background:#da3633; border-color:#da3633; color:#fff; }
  input[type=file] { display:none; }
  input[type=range] { width:100%; accent-color:var(--acc); }
  .meta { font-size:.85rem; color:var(--muted); line-height:1.45; margin-top:.5rem; }
  .meta b { color:var(--fg); font-weight:500; }
  .status { font-size:.8rem; color:var(--muted); text-align:center; margin-top:.5rem; }
  .modes { display:flex; gap:.5rem; }
  .modes label { flex:1; display:flex; align-items:center; gap:.4rem; padding:.55rem .7rem; background:#21262d; border-radius:8px; border:1px solid var(--border); cursor:pointer; text-transform:none; font-size:.9rem; color:var(--fg); letter-spacing:0; }
  .modes input { accent-color:var(--acc); }
  .bar { height:6px; background:#21262d; border-radius:3px; overflow:hidden; margin-top:.5rem; }
  .bar > i { display:block; height:100%; width:0; background:var(--acc); transition:width .2s; }
</style>
</head>
<body>
<div class="wrap">
  <h1>DUCKOUSTIC</h1>

  <div class="card">
    <label>Audio file (PCM WAV · mono · 8 kHz · 16-bit)</label>
    <label class="file-btn" for="f">Choose Audio File</label>
    <input type="file" id="f" accept=".wav,audio/wav">
    <div class="bar"><i id="prog"></i></div>
    <div class="meta" id="filemeta">No file loaded</div>
    <div class="row">
      <button class="primary" id="btnPlay">PLAY</button>
      <button class="danger" id="btnStop">STOP</button>
    </div>
  </div>

  <div class="card">
    <label>Laser</label>
    <div class="row">
      <button id="btnLaser">ON / OFF</button>
    </div>
    <label style="margin-top:.85rem">Gain</label>
    <input type="range" id="gain" min="0" max="100" value="60">
  </div>

  <div class="card">
    <label>Mode</label>
    <div class="modes">
      <label><input type="radio" name="mode" value="listen"> Listen</label>
      <label><input type="radio" name="mode" value="clone" checked> Clone</label>
    </div>
  </div>

  <div class="status" id="status">connecting…</div>
</div>
<script>
const $ = id => document.getElementById(id);
const statusEl = $('status');
let laserOn = false;

async function api(path, opts) {
  try {
    const r = await fetch(path, opts);
    return await r.json();
  } catch (e) {
    statusEl.textContent = 'offline';
    return null;
  }
}

async function refresh() {
  const s = await api('/api/status');
  if (!s) return;
  laserOn = !!s.laser;
  $('btnLaser').textContent = laserOn ? 'Laser: ON' : 'Laser: OFF';
  $('gain').value = Math.round((s.gain || 0) * 100);
  if (s.file) {
    $('filemeta').innerHTML =
      'File: <b>' + (s.filename || 'audio.wav') + '</b><br>' +
      'Format: WAV · ' + s.sample_rate + ' Hz · ' + s.channels + ' ch · ' + s.bits + '-bit<br>' +
      'Duration: <b>' + (s.duration || 0).toFixed(2) + ' s</b> · state: ' + s.play_state;
  }
  document.querySelectorAll('input[name=mode]').forEach(r => {
    r.checked = (r.value === s.mode);
  });
  statusEl.textContent = 'AP ' + (s.ssid || '') + ' · clients ' + (s.clients || 0) +
    ' · mode ' + (s.mode || '') +
    (s.signal ? ' · optical SIGNAL' : ' · optical quiet');
}

$('f').onchange = async (e) => {
  const file = e.target.files[0];
  if (!file) return;
  const fd = new FormData();
  fd.append('file', file, file.name);
  $('prog').style.width = '30%';
  statusEl.textContent = 'uploading…';
  try {
    const r = await fetch('/api/upload', { method:'POST', body: fd });
    const j = await r.json();
    $('prog').style.width = '100%';
    if (!j.ok) {
      statusEl.textContent = 'upload failed: ' + (j.error || 'unknown');
      $('prog').style.width = '0';
      return;
    }
    statusEl.textContent = 'uploaded';
    setTimeout(() => { $('prog').style.width = '0'; }, 600);
    refresh();
  } catch (err) {
    statusEl.textContent = 'upload error';
    $('prog').style.width = '0';
  }
};

$('btnPlay').onclick = () => api('/api/play', { method:'POST' }).then(refresh);
$('btnStop').onclick = () => api('/api/stop', { method:'POST' }).then(refresh);
$('btnLaser').onclick = () => {
  api('/api/laser', { method:'POST', headers:{'Content-Type':'text/plain'}, body: laserOn ? 'off' : 'on' }).then(refresh);
};
$('gain').oninput = (e) => {
  const v = (e.target.value / 100).toFixed(2);
  api('/api/gain', { method:'POST', headers:{'Content-Type':'text/plain'}, body: v });
};
document.querySelectorAll('input[name=mode]').forEach(r => {
  r.onchange = () => {
    if (r.checked) api('/api/mode', { method:'POST', headers:{'Content-Type':'text/plain'}, body: r.value }).then(refresh);
  };
});

refresh();
setInterval(refresh, 1500);
</script>
</body>
</html>
)rawliteral";

bool WebUI::begin(PlaybackEngine* playback, LaserOutput* laser,
                  OpticalInput* optical, SignalProcessor* processor) {
    playback_  = playback;
    laser_     = laser;
    optical_   = optical;
    processor_ = processor;

    if (!LittleFS.begin(true)) {
        Serial.println(F("LittleFS mount failed"));
        return false;
    }

    start_softap();
    setup_routes();
    server_.begin();
    Serial.println(F("HTTP server on http://192.168.4.1/"));
    return true;
}

void WebUI::start_softap() {
    String ssid = make_ssid();
    WiFi.mode(WIFI_AP);
    // Open network when password empty
    if (strlen(DUCK_AP_PASSWORD) == 0) {
        WiFi.softAP(ssid.c_str(), nullptr, DUCK_AP_CHANNEL, 0, DUCK_AP_MAX_CONN);
    } else {
        WiFi.softAP(ssid.c_str(), DUCK_AP_PASSWORD, DUCK_AP_CHANNEL, 0, DUCK_AP_MAX_CONN);
    }
    delay(100);
    Serial.print(F("SoftAP SSID: "));
    Serial.println(ssid);
    Serial.print(F("SoftAP IP:   "));
    Serial.println(WiFi.softAPIP());
}

String WebUI::make_ssid() const {
    uint64_t mac = ESP.getEfuseMac();
    char suffix[5];
    snprintf(suffix, sizeof(suffix), "%04X", (unsigned)(mac & 0xFFFF));
    return String(DUCK_AP_SSID_PREFIX) + suffix;
}

void WebUI::setup_routes() {
    server_.on("/", HTTP_GET, [this]() { handle_root(); });
    server_.on("/api/status", HTTP_GET, [this]() { handle_status(); });
    server_.on("/api/play", HTTP_POST, [this]() { handle_play(); });
    server_.on("/api/stop", HTTP_POST, [this]() { handle_stop(); });
    server_.on("/api/laser", HTTP_POST, [this]() { handle_laser(); });
    server_.on("/api/gain", HTTP_POST, [this]() { handle_gain(); });
    server_.on("/api/mode", HTTP_POST, [this]() { handle_mode(); });
    server_.on("/api/loop", HTTP_POST, [this]() { handle_loop(); });

    // Multipart upload
    server_.on("/api/upload", HTTP_POST,
        [this]() { handle_upload_finish(); },
        [this]() { handle_upload(); }
    );

    server_.onNotFound([this]() { handle_not_found(); });
}

void WebUI::handle() {
    server_.handleClient();
}

void WebUI::handle_root() {
    server_.send_P(200, "text/html", INDEX_HTML);
}

void WebUI::handle_status() {
    JsonDocument doc;
    doc["ok"] = true;
    doc["ssid"] = make_ssid();
    doc["clients"] = WiFi.softAPgetStationNum();
    doc["mode"] = (mode_ == Mode::Listen) ? "listen" : "clone";
    doc["laser"] = laser_ ? laser_->enabled() : false;
    doc["gain"] = laser_ ? laser_->gain() : 0.0f;
    doc["loop"] = loop_en_;

    if (playback_ && playback_->info().valid) {
        doc["file"] = true;
        doc["filename"] = last_filename_.length() ? last_filename_ : "audio.wav";
        doc["sample_rate"] = playback_->info().sample_rate;
        doc["channels"] = playback_->info().channels;
        doc["bits"] = playback_->info().bits_per_sample;
        doc["duration"] = playback_->duration_sec();
        const char* st = "stopped";
        switch (playback_->state()) {
            case PlayState::Playing:  st = "playing"; break;
            case PlayState::Finished: st = "finished"; break;
            default: break;
        }
        doc["play_state"] = st;
    } else {
        doc["file"] = false;
    }

    // Optical path snapshot (LISTEN mode interest)
    if (optical_ && optical_->ready()) {
        doc["optical_dc"] = optical_->get_dc_level();
        doc["optical_p2p"] = optical_->get_signal_level();
        doc["signal"] = optical_->get_signal_level() >= DUCK_SIGNAL_THRESHOLD;
    } else {
        doc["signal"] = false;
    }

    String out;
    serializeJson(doc, out);
    server_.send(200, "application/json", out);
}

void WebUI::handle_upload() {
    HTTPUpload& upload = server_.upload();
    if (upload.status == UPLOAD_FILE_START) {
        upload_ok_ = false;
        last_filename_ = upload.filename;
        Serial.print(F("Upload start: "));
        Serial.println(last_filename_);
        if (playback_) playback_->stop();
        LittleFS.remove(DUCK_AUDIO_PATH);
        File f = LittleFS.open(DUCK_AUDIO_PATH, "w");
        if (!f) {
            Serial.println(F("Failed to open file for write"));
            return;
        }
        f.close();
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (upload.totalSize > DUCK_MAX_UPLOAD_BYTES) {
            Serial.println(F("Upload too large"));
            return;
        }
        File f = LittleFS.open(DUCK_AUDIO_PATH, "a");
        if (f) {
            f.write(upload.buf, upload.currentSize);
            f.close();
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        Serial.print(F("Upload end, bytes="));
        Serial.println(upload.totalSize);
        upload_ok_ = true;
    }
}

void WebUI::handle_upload_finish() {
    JsonDocument doc;
    if (!upload_ok_) {
        doc["ok"] = false;
        doc["error"] = "upload failed";
        String out;
        serializeJson(doc, out);
        server_.send(400, "application/json", out);
        return;
    }

    if (!playback_ || !playback_->load(LittleFS, DUCK_AUDIO_PATH)) {
        doc["ok"] = false;
        doc["error"] = "invalid WAV (need mono 8 kHz 16-bit PCM)";
        LittleFS.remove(DUCK_AUDIO_PATH);
        String out;
        serializeJson(doc, out);
        server_.send(400, "application/json", out);
        return;
    }

    doc["ok"] = true;
    doc["filename"] = last_filename_;
    doc["duration"] = playback_->duration_sec();
    doc["sample_rate"] = playback_->info().sample_rate;
    String out;
    serializeJson(doc, out);
    server_.send(200, "application/json", out);
}

void WebUI::handle_play() {
    if (mode_ == Mode::Clone && playback_) {
        playback_->set_loop(loop_en_);
        playback_->play();
    }
    server_.send(200, "application/json", "{\"ok\":true}");
}

void WebUI::handle_stop() {
    if (playback_) playback_->stop();
    server_.send(200, "application/json", "{\"ok\":true}");
}

void WebUI::handle_laser() {
    String body = server_.arg("plain");
    body.toLowerCase();
    bool on = (body.indexOf("on") >= 0);
    if (laser_) laser_->set_enabled(on);
    server_.send(200, "application/json", on ? "{\"ok\":true,\"laser\":true}" : "{\"ok\":true,\"laser\":false}");
}

void WebUI::handle_gain() {
    String body = server_.arg("plain");
    float g = body.toFloat();
    if (laser_) laser_->set_gain(g);
    server_.send(200, "application/json", "{\"ok\":true}");
}

void WebUI::handle_mode() {
    String body = server_.arg("plain");
    body.toLowerCase();
    if (body.indexOf("listen") >= 0) {
        mode_ = Mode::Listen;
        if (playback_) playback_->stop();
    } else {
        mode_ = Mode::Clone;
    }
    server_.send(200, "application/json", "{\"ok\":true}");
}

void WebUI::handle_loop() {
    String body = server_.arg("plain");
    loop_en_ = (body.toInt() != 0);
    if (playback_) playback_->set_loop(loop_en_);
    server_.send(200, "application/json", "{\"ok\":true}");
}

void WebUI::handle_not_found() {
    server_.send(404, "text/plain", "not found");
}

} // namespace duckoustic
