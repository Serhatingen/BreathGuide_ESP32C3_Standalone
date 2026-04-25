#include "Display.h"
#include "AppState.h"

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

void drawWaitingEyesFrame() {
  static unsigned long lastFrameMs = 0;
  unsigned long now = millis();
  if (now - lastFrameMs < 45) return;
  lastFrameMs = now;

  const uint16_t cycleMs = 4200;
  uint16_t t = (uint16_t)((now - waitAckStartedAt) % cycleMs);
  int openness = 12;

  if      (t < 80)                    openness = map(t, 0,    80,   12, 2);
  else if (t < 160)                   openness = map(t, 80,   160,  2, 12);
  else if (t > 2800 && t < 2880)      openness = map(t, 2800, 2880, 12, 3);
  else if (t >= 2880 && t < 2960)     openness = map(t, 2880, 2960, 3, 12);

  float a      = (now - waitAckStartedAt) * 0.0042f;
  int   pupilX = (int)(sinf(a) * 4.5f);
  int   pupilY = (int)(cosf(a * 1.23f) * 2.6f);

  const int leftX  = 18;
  const int rightX = 74;
  const int eyeY   = 10;
  const int eyeW   = 36;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(18, 0);
  display.print("Butona bas :)");

  // Eye whites
  display.fillRoundRect(leftX,  eyeY, eyeW, openness, 8, SSD1306_WHITE);
  display.fillRoundRect(rightX, eyeY, eyeW, openness, 8, SSD1306_WHITE);

  if (openness > 5) {
    int cy = eyeY + openness / 2;
    // Pupils
    display.fillCircle(leftX  + eyeW / 2 + pupilX, cy + pupilY, 4, SSD1306_BLACK);
    display.fillCircle(rightX + eyeW / 2 + pupilX, cy + pupilY, 4, SSD1306_BLACK);
    // Highlights
    display.drawPixel(leftX  + eyeW / 2 + pupilX - 1, cy + pupilY - 1, SSD1306_WHITE);
    display.drawPixel(rightX + eyeW / 2 + pupilX - 1, cy + pupilY - 1, SSD1306_WHITE);
  }

  // Eyebrows
  int browLift = (int)(sinf(a * 0.8f) * 2.0f);
  display.drawLine(leftX  +  6, eyeY - 2 - browLift, leftX  + 26, eyeY - 5 + browLift, SSD1306_WHITE);
  display.drawLine(rightX + 10, eyeY - 5 + browLift, rightX + 30, eyeY - 2 - browLift, SSD1306_WHITE);

  display.display();
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
