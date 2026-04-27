#pragma once

// Pull in all external libraries in one place so every module
// only needs to include this file to access the shared objects.
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

#include "Config.h"

// -------------------------
// Simple RGB framebuffer type (replaces FastLED CRGB)
// -------------------------
struct CRGB {
  uint8_t r, g, b;
  CRGB() : r(0), g(0), b(0) {}
  CRGB(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
  CRGB& operator+=(const CRGB& o) {
    r = ((uint16_t)r + o.r > 255) ? 255 : r + o.r;
    g = ((uint16_t)g + o.g > 255) ? 255 : g + o.g;
    b = ((uint16_t)b + o.b > 255) ? 255 : b + o.b;
    return *this;
  }
  void nscale8_video(uint8_t s) {
    uint8_t nr = (uint16_t)r * s >> 8;
    uint8_t ng = (uint16_t)g * s >> 8;
    uint8_t nb = (uint16_t)b * s >> 8;
    r = (r && !nr) ? 1 : nr;
    g = (g && !ng) ? 1 : ng;
    b = (b && !nb) ? 1 : nb;
  }
  static const CRGB Black;
};

// -------------------------
// Hardware objects
// -------------------------
extern CRGB              leds[NUM_LEDS];
extern Adafruit_NeoPixel strip;
extern Adafruit_SSD1306  display;
extern WebServer        server;
extern DNSServer        dnsServer;
extern Preferences      prefs;

// -------------------------
// Settings & app state
// -------------------------
extern Settings    settings;
extern AppMode     appMode;
extern BreathPhase currentPhase;

extern bool portalActive;
extern bool sessionActive;

// -------------------------
// Timers
// -------------------------
extern unsigned long nextReminderAt;
extern unsigned long portalStartedAt;
extern unsigned long lastPortalActivityAt;
extern unsigned long waitAckStartedAt;
extern unsigned long phaseStartedAt;
extern unsigned long lastToneUpdateAt;
extern unsigned long lastDisplayUpdateAt;
extern unsigned long lastIdleLedFrameAt;

extern uint8_t currentRound;

// -------------------------
// AP credentials / network
// -------------------------
extern String    apSSID;
extern String    apPassword;
extern IPAddress apIP;
extern IPAddress apGW;
extern IPAddress apMask;

// -------------------------
// LED breath order (centre → out)
// -------------------------
extern const uint8_t breathOrder[NUM_LEDS];
