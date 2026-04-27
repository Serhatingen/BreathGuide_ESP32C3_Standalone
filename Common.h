/**
 * Common.h – U8G2 compatibility shim for Adafruit SSD1306.
 *
 * The esp32-eyes library calls u8g2.drawBox/drawHLine/drawTriangle/
 * setDrawColor/clearBuffer/sendBuffer.  This shim translates those calls
 * to the equivalent Adafruit_GFX / Adafruit_SSD1306 functions so we can
 * use the eye-animation library without the u8g2 dependency.
 */
#pragma once

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// Forward declaration – the real `display` object lives in AppState.cpp.
extern Adafruit_SSD1306 display;

class U8G2Shim {
public:
  uint8_t _color = 1;   // 1 = white (draw), 0 = black (erase)

  void setDrawColor(int c) { _color = (uint8_t)c; }

  void clearBuffer() { display.clearDisplay(); }
  void sendBuffer()  { display.display(); }

  // no-ops: display is already initialised by AppState
  void setI2CAddress(uint8_t) {}
  bool begin() { return true; }

  void drawHLine(int16_t x, int16_t y, int16_t w) {
    if (w > 0)
      display.drawFastHLine(x, y, w,
                            _color ? SSD1306_WHITE : SSD1306_BLACK);
  }

  // u8g2 drawBox(x, y, w, h) — origin = top-left, same convention as Adafruit
  void drawBox(int32_t x, int32_t y, int32_t w, int32_t h) {
    if (w > 0 && h > 0)
      display.fillRect((int16_t)x, (int16_t)y,
                       (int16_t)w, (int16_t)h,
                       _color ? SSD1306_WHITE : SSD1306_BLACK);
  }

  // u8g2 drawTriangle is a FILLED triangle (unlike Adafruit drawTriangle)
  void drawTriangle(int32_t x0, int32_t y0,
                    int32_t x1, int32_t y1,
                    int32_t x2, int32_t y2) {
    display.fillTriangle((int16_t)x0, (int16_t)y0,
                         (int16_t)x1, (int16_t)y1,
                         (int16_t)x2, (int16_t)y2,
                         _color ? SSD1306_WHITE : SSD1306_BLACK);
  }
};

extern U8G2Shim u8g2;
