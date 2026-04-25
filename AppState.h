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
#include <FastLED.h>

#include "Config.h"

// -------------------------
// Hardware objects
// -------------------------
extern CRGB             leds[NUM_LEDS];
extern Adafruit_SSD1306 display;
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
