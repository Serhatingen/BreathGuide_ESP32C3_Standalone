#include "Portal.h"
#include "AppState.h"
#include "Utils.h"
#include "Audio.h"
#include "Display.h"
#include "LedControl.h"
#include "SettingsMgr.h"
#include "Session.h"   // enterWaitAckMode

// ======================================================
// Internal: HTML + JSON helpers
// ======================================================

static String htmlPage() {
  return R"rawliteral(
<!DOCTYPE html>
<html lang="tr">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>BreathGuide</title>
<style>
:root{
  --bg:#07111f; --bg2:#0c1728; --card:#122238;
  --txt:#ecf4ff; --muted:#9db2cd; --acc:#39bdf8; --acc2:#22c55e;
}
*{box-sizing:border-box}
body{
  margin:0; padding:20px;
  background:radial-gradient(circle at top,#163253 0%,var(--bg) 55%,#040914 100%);
  color:var(--txt); font-family:Segoe UI,Arial,sans-serif;
}
.wrap{max-width:720px; margin:0 auto}
.card{
  background:linear-gradient(180deg,rgba(18,34,56,.98),rgba(10,20,35,.98));
  border:1px solid rgba(255,255,255,.08); border-radius:20px; padding:18px;
  box-shadow:0 18px 40px rgba(0,0,0,.28);
}
h1{margin:0 0 8px; font-size:28px}
p{color:var(--muted); line-height:1.5}
.grid{display:grid; grid-template-columns:1fr 1fr; gap:12px}
@media (max-width:680px){.grid{grid-template-columns:1fr}}
label{display:block; font-size:14px; margin:12px 0 6px}
input[type=number],input[type=range],select{
  width:100%; background:#0a1423; color:var(--txt);
  border:1px solid rgba(255,255,255,.09); border-radius:12px; padding:12px;
}
.row{
  display:flex; align-items:center; justify-content:space-between; gap:12px;
  background:#0a1423; border:1px solid rgba(255,255,255,.08); border-radius:12px; padding:12px;
}
button{
  border:0; border-radius:12px; padding:12px 16px; font-weight:700;
  cursor:pointer; color:white; margin:6px 8px 0 0;
}
.b1{background:linear-gradient(135deg,#0ea5e9,#2563eb)}
.b2{background:linear-gradient(135deg,#22c55e,#16a34a)}
.b3{background:linear-gradient(135deg,#ef4444,#dc2626)}
.small{font-size:13px; color:var(--muted)}
.status{
  margin-top:12px; background:#0a1423; border:1px solid rgba(255,255,255,.08);
  border-radius:14px; padding:12px; min-height:56px;
}
.badge{
  display:inline-block; padding:6px 10px; border-radius:999px;
  background:rgba(57,189,248,.12); border:1px solid rgba(57,189,248,.22);
  font-size:13px; margin-bottom:10px;
}
</style>
</head>
<body>
<div class="wrap">
  <div class="card">
    <div class="badge">ESP32-S3 BreathGuide</div>
    <h1>Nefes Cihazı Ayarları</h1>
    <p>Buradan seans temposunu ve cihaz davranışını ayarlayabilirsin.
       Kaydet dedikten sonra ayarlar cihazın içinde saklanır.</p>

    <div class="grid">
      <div>
        <label>Hatırlatma aralığı (dakika)</label>
        <input id="reminderMinutes" type="number" min="5" max="240">
      </div>
      <div>
        <label>Tur sayısı</label>
        <input id="rounds" type="number" min="1" max="12">
      </div>
      <div>
        <label>Alış süresi (sn)</label>
        <input id="inhaleSec" type="number" min="2" max="12">
      </div>
      <div>
        <label>Tutuş süresi (sn)</label>
        <input id="holdSec" type="number" min="0" max="12">
      </div>
      <div>
        <label>Veriş süresi (sn)</label>
        <input id="exhaleSec" type="number" min="2" max="16">
      </div>
      <div>
        <label>Intro animasyonu (sn)</label>
        <input id="preAnimSeconds" type="number" min="1" max="12">
      </div>
      <div>
        <label>LED parlaklığı</label>
        <input id="brightness" type="range" min="5" max="255">
      </div>
      <div>
        <label>Ses seviyesi</label>
        <input id="volume" type="range" min="0" max="100">
      </div>
      <div>
        <label>Portal otomatik kapanma (sn)</label>
        <input id="portalTimeoutSec" type="number" min="60" max="3600">
      </div>
      <div></div>
    </div>

    <div class="row" style="margin-top:12px">
      <span>Ses açık</span>
      <input id="soundEnabled" type="checkbox">
    </div>
    <div class="row" style="margin-top:12px">
      <span>Otomatik hatırlatma açık</span>
      <input id="reminderEnabled" type="checkbox">
    </div>

    <div style="margin-top:12px">
      <button class="b1" onclick="save()">Kaydet</button>
      <button class="b2" onclick="test()">Elle bekleme ekranı aç</button>
      <button class="b3" onclick="closePortal()">Portalı kapat</button>
    </div>

    <div class="status" id="status">Hazır.</div>
    <div class="small" style="margin-top:10px">
      Not: Portal açıkken LED animasyonları kısılır.
      Kısa basış elle seans başlatır, uzun basış portal açar/kapatır.
    </div>
  </div>
</div>

<script>
async function load() {
  const r = await fetch('/api/settings');
  const s = await r.json();
  for (const [k,v] of Object.entries(s)) {
    const el = document.getElementById(k);
    if (!el) continue;
    if (el.type === 'checkbox') el.checked = !!v;
    else el.value = v;
  }
}

async function save() {
  const params = new URLSearchParams();
  [
    'reminderMinutes','rounds','inhaleSec','holdSec','exhaleSec',
    'brightness','volume','preAnimSeconds','portalTimeoutSec'
  ].forEach(id => params.append(id, document.getElementById(id).value));
  params.append('soundEnabled',    document.getElementById('soundEnabled').checked    ? '1' : '0');
  params.append('reminderEnabled', document.getElementById('reminderEnabled').checked ? '1' : '0');
  const r = await fetch('/api/settings', {
    method: 'POST',
    headers: {'Content-Type': 'application/x-www-form-urlencoded'},
    body: params.toString()
  });
  document.getElementById('status').textContent = await r.text();
}

async function test() {
  const r = await fetch('/api/manual_wait', {method:'POST'});
  document.getElementById('status').textContent = await r.text();
}

async function closePortal() {
  const r = await fetch('/api/portal_close', {method:'POST'});
  document.getElementById('status').textContent = await r.text();
}

load();
</script>
</body>
</html>)rawliteral";
}

static void sendJsonSettings() {
  String json = "{";
  json += "\"reminderMinutes\":"  + String(settings.reminderMinutes)                  + ",";
  json += "\"rounds\":"           + String(settings.rounds)                            + ",";
  json += "\"inhaleSec\":"        + String(settings.inhaleSec)                         + ",";
  json += "\"holdSec\":"          + String(settings.holdSec)                           + ",";
  json += "\"exhaleSec\":"        + String(settings.exhaleSec)                         + ",";
  json += "\"brightness\":"       + String(settings.brightness)                        + ",";
  json += "\"volume\":"           + String(settings.volume)                            + ",";
  json += "\"soundEnabled\":"     + String(settings.soundEnabled    ? "true" : "false") + ",";
  json += "\"reminderEnabled\":"  + String(settings.reminderEnabled ? "true" : "false") + ",";
  json += "\"preAnimSeconds\":"   + String(settings.preAnimSeconds)                    + ",";
  json += "\"portalTimeoutSec\":" + String(settings.portalTimeoutSec);
  json += "}";
  server.send(200, "application/json", json);
}

// ======================================================
// Internal: HTTP route handlers
// ======================================================

static void handleRoot() {
  touchPortalActivity();
  server.send(200, "text/html; charset=utf-8", htmlPage());
}

static void handleGetSettings() {
  touchPortalActivity();
  sendJsonSettings();
}

static void handlePostSettings() {
  touchPortalActivity();

  if (server.hasArg("reminderMinutes"))
    settings.reminderMinutes  = clampValue<uint16_t>(server.arg("reminderMinutes").toInt(),  5, 240);
  if (server.hasArg("rounds"))
    settings.rounds           = clampValue<uint8_t>(server.arg("rounds").toInt(),            1,  12);
  if (server.hasArg("inhaleSec"))
    settings.inhaleSec        = clampValue<uint8_t>(server.arg("inhaleSec").toInt(),         2,  12);
  if (server.hasArg("holdSec"))
    settings.holdSec          = clampValue<uint8_t>(server.arg("holdSec").toInt(),           0,  12);
  if (server.hasArg("exhaleSec"))
    settings.exhaleSec        = clampValue<uint8_t>(server.arg("exhaleSec").toInt(),         2,  16);
  if (server.hasArg("brightness"))
    settings.brightness       = clampValue<uint8_t>(server.arg("brightness").toInt(),        5, 255);
  if (server.hasArg("volume"))
    settings.volume           = clampValue<uint8_t>(server.arg("volume").toInt(),            0, 100);
  if (server.hasArg("preAnimSeconds"))
    settings.preAnimSeconds   = clampValue<uint8_t>(server.arg("preAnimSeconds").toInt(),    1,  12);
  if (server.hasArg("portalTimeoutSec"))
    settings.portalTimeoutSec = clampValue<uint16_t>(server.arg("portalTimeoutSec").toInt(), 60, 3600);

  settings.soundEnabled    = server.hasArg("soundEnabled")
    ? server.arg("soundEnabled")    == "1" : false;
  settings.reminderEnabled = server.hasArg("reminderEnabled")
    ? server.arg("reminderEnabled") == "1" : false;

  strip.setBrightness(settings.brightness);
  saveSettings();
  scheduleNextReminder();
  server.send(200, "text/plain; charset=utf-8", "Ayarlar kaydedildi.");
}

static void handleManualWait() {
  touchPortalActivity();
  // Switch to wait-ack without the intro animation (portal is still open).
  appMode          = MODE_WAIT_ACK;
  waitAckStartedAt = millis();
  drawWaitingEyesFrame();
  server.send(200, "text/plain; charset=utf-8",
              "Bekleme ekrani acildi. Kisa fiziksel basis ile baslat.");
}

static void handlePortalClose() {
  touchPortalActivity();
  server.send(200, "text/plain; charset=utf-8", "Portal kapaniyor...");
  delay(200);
  stopPortal();
}

// ======================================================
// Public API
// ======================================================

void touchPortalActivity() {
  lastPortalActivityAt = millis();
}

void stopPortal() {
  if (!portalActive) return;
  dnsServer.stop();
  server.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  portalActive = false;
  appMode      = MODE_IDLE;
  clearLeds();
  drawIdleScreen();
  scheduleNextReminder();
}

void beginPortal() {
  if (portalActive) return;

  stopTone();
  sessionActive = false;
  appMode       = MODE_PORTAL;

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apGW, apMask);
  apSSID = "BreathGuide-" + shortChipId();
  WiFi.softAP(apSSID.c_str(), apPassword.c_str());

  dnsServer.start(DNS_PORT, "*", apIP);

  // Captive-portal catch-all routes
  server.on("/",                    HTTP_GET,  handleRoot);
  server.on("/generate_204",        HTTP_GET,  handleRoot);
  server.on("/hotspot-detect.html", HTTP_GET,  handleRoot);
  server.on("/connecttest.txt",     HTTP_GET,  handleRoot);
  server.on("/ncsi.txt",            HTTP_GET,  handleRoot);
  // Settings API
  server.on("/api/settings",        HTTP_GET,  handleGetSettings);
  server.on("/api/settings",        HTTP_POST, handlePostSettings);
  server.on("/api/manual_wait",     HTTP_POST, handleManualWait);
  server.on("/api/portal_close",    HTTP_POST, handlePortalClose);
  server.onNotFound(handleRoot);
  server.begin();

  portalActive         = true;
  portalStartedAt      = millis();
  lastPortalActivityAt = millis();

  clearLeds();
  drawPortalInfoScreen();
}

void handlePortalLoop() {
  if (!portalActive) return;

  dnsServer.processNextRequest();
  server.handleClient();

  // Auto-close on inactivity
  if (millis() - lastPortalActivityAt > (unsigned long)settings.portalTimeoutSec * 1000UL) {
    stopPortal();
    return;
  }

  // Refresh portal info on the OLED every 500 ms
  static unsigned long lastPortalScreenAt = 0;
  if (millis() - lastPortalScreenAt > 500) {
    lastPortalScreenAt = millis();
    drawPortalInfoScreen();
  }
}
