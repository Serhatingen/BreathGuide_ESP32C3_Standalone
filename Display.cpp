#include "Display.h"
#include "AppState.h"
#include "Face.h"

// Animated-eye face object (created in initEyeFace)
static Face *eyeFace = nullptr;

void drawCenteredText(const String &line1, const String &line2, uint8_t size1, uint8_t size2) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(size1);
  int16_t  x1, y1;
  uint16_t w1, h1;
  display.getTextBounds(line1, 0, 0, &x1, &y1, &w1, &h1);
  display.setCursor((SCREEN_WIDTH - (int)w1) / 2, 3);
  display.print(line1);

  if (line2.length()) {
    display.setTextSize(size2);
    int16_t  x2, y2;
    uint16_t w2, h2;
    display.getTextBounds(line2, 0, 0, &x2, &y2, &w2, &h2);
    display.setCursor((SCREEN_WIDTH - (int)w2) / 2, 18);
    display.print(line2);
  }

  display.display();
}

void drawBootScreen() {
  drawCenteredText("BreathGuide", "ESP32-S3", 1, 1);
}
void drawIdleScreen() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.print("Hazir");
  display.setCursor(68, 0);
  display.print(settings.reminderEnabled ? "R:Acik" : "R:Kapali");

  display.setCursor(0, 12);
  display.print("Uzun bas: AP");

  display.setCursor(0, 22);
  display.print("Kisa bas: Elle baslat");

  display.display();
}

void drawPortalInfoScreen() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.print("AP Acik");
  display.setCursor(64, 0);
  display.print("Uzun: Kapat");

  display.setCursor(0, 10);
  display.print(apSSID);

  display.setCursor(0, 20);
  display.print(apIP);
  display.setCursor(70, 20);
  display.print(apPassword);

  display.display();
}

void initEyeFace() {
  if (eyeFace) return;   // already initialised

  // Face(screenW, screenH, eyeSize):
  //   screenW=128, screenH=64 → CenterX=64, CenterY=32
  //   eyeSize=36  → left-eye X = 64-18-4 = 42, right-eye X = 64+18+4 = 86
  eyeFace = new Face(128, 64, 36);
  eyeFace->Expression.GoTo_Normal();

  eyeFace->RandomBehavior = true;
  eyeFace->RandomBlink    = true;
  eyeFace->RandomLook     = true;

  eyeFace->Blink.Timer.SetIntervalMillis(3200);
  eyeFace->Behavior.Timer.SetIntervalMillis(4000);
  eyeFace->Look.Timer.SetIntervalMillis(2500);
}

void drawWaitingEyesFrame() {
  if (!eyeFace) return;
  eyeFace->Update();
}

void drawSessionScreen(const char* phaseLabel, uint8_t secondsLeft,
                       uint8_t roundIndex, uint8_t roundTotal) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Round counter + portal hint
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Tur ");
  display.print(roundIndex);
  display.print("/");
  display.print(roundTotal);
  display.setCursor(80, 0);
  display.print("Uzun: AP");

  // Phase label (large)
  display.setTextSize(2);
  int16_t  x1, y1;
  uint16_t w1, h1;
  display.getTextBounds(phaseLabel, 0, 0, &x1, &y1, &w1, &h1);
  display.setCursor((SCREEN_WIDTH - (int)w1) / 2, 10);
  display.print(phaseLabel);

  // Countdown
  display.setTextSize(1);
  char buf[12];
  snprintf(buf, sizeof(buf), "%u sn", secondsLeft);
  int16_t  x2, y2;
  uint16_t w2, h2;
  display.getTextBounds(buf, 0, 0, &x2, &y2, &w2, &h2);
  display.setCursor((SCREEN_WIDTH - (int)w2) / 2, 25);
  display.print(buf);

  display.display();
}
