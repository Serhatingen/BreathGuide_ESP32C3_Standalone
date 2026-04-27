/*
  BreathGuide_ESP32S3_N16R8.ino
  ------------------------------------------------------------
  Standalone breath-guide device — ESP32-S3 N16R8 variant
  - ESP32-S3 N16R8 (16 MB Flash / 8 MB OPI PSRAM)
  - 10 × WS2812
  - 0.91" I2C 128×32 OLED (SSD1306, address 0x3C)
  - 1 button (INPUT_PULLUP — default: GPIO 0 / BOOT key)
  - 1 passive piezo / buzzer

  Entry point – only setup() and loop() live here.
  All functionality is split into dedicated modules.

  Arduino IDE board settings:
    Board            : ESP32S3 Dev Module
    Flash Size       : 16MB
    Partition Scheme : 16M Flash (3MB APP / 9.9MB FATFS)
    PSRAM            : OPI PSRAM

  Note:
  ESP32-S3 + WS2812 + Wi-Fi/AP simultaneously can cause LED flicker
  on some boards. Heavy LED animations are therefore suppressed while
  the portal is open.

  ⚠  GPIO 35, 36, 37 are reserved for the OPI PSRAM bus — never use them.
*/

// Module headers (each pulls in AppState.h → all required libs)
#include "Config.h"
#include "AppState.h"
#include "Utils.h"
#include "Audio.h"
#include "Display.h"
#include "LedControl.h"
#include "SettingsMgr.h"
#include "Session.h"
#include "Portal.h"
#include "Button.h"

// ======================================================
void setup() {
  Serial.begin(115200);
  delay(80);

  pinMode(PIN_BUTTON, INPUT_PULLUP);

  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED baslatilamadi");
    // Continue without display rather than hang.
  }
  display.clearDisplay();
  display.display();

  loadSettings();

  // Initialise the animated-eye face (must be after display.begin())
  initEyeFace();

  strip.begin();
  strip.setBrightness(settings.brightness);
  clearLeds();

  // ESP32-Arduino v3 API: ledcAttach(pin, freq, resolution)
  ledcAttach(PIN_BUZZER, 2000, 8);
  stopTone();

  WiFi.mode(WIFI_OFF);

  drawBootScreen();
  cuteChime();
  delay(700);
  drawIdleScreen();

  scheduleNextReminder();
}

// ======================================================
void loop() {
  handleButton();

  if (portalActive) {
    handlePortalLoop();
    return;
  }

  switch (appMode) {
    case MODE_IDLE:
      renderIdleLeds();
      handleReminderScheduler();
      drawWaitingEyesFrame();
      break;

    case MODE_WAIT_ACK:
      clearLeds();
      drawWaitingEyesFrame();
      break;

    case MODE_SESSION:
      updateSession();
      break;

    default:
      break;
  }
}
