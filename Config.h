#pragma once

#include <Arduino.h>

// -------------------------
// Target board: ESP32-S3 N16R8
// (16 MB Flash / 8 MB OPI PSRAM)
//
// Pin definitions
// Adjust to match your board's silk-screen if needed.
//
// ⚠  GPIO 35, 36, 37 are reserved for the OPI PSRAM bus on N16R8.
//    Never route signals to those pads.
// -------------------------
static const uint8_t PIN_LED_DATA   = 38;  // WS2812 data  (any free GPIO)
static const uint8_t PIN_BUTTON     = 0;   // single button, INPUT_PULLUP (BOOT key)
static const uint8_t PIN_BUZZER     = 17;  // passive piezo / buzzer
static const uint8_t PIN_OLED_SDA   = 8;   // I2C SDA
static const uint8_t PIN_OLED_SCL   = 9;   // I2C SCL

// -------------------------
// Hardware constants
// -------------------------
static const uint8_t  NUM_LEDS      = 10;
static const uint8_t  OLED_ADDR     = 0x3C;
static const uint16_t SCREEN_WIDTH  = 128;
static const uint16_t SCREEN_HEIGHT = 32;

// -------------------------
// Button timing
// -------------------------
static const uint16_t DEBOUNCE_MS   = 35;
static const uint16_t LONG_PRESS_MS = 1400;

// -------------------------
// Portal
// -------------------------
static const byte DNS_PORT = 53;

// -------------------------
// Application mode
// -------------------------
enum AppMode : uint8_t {
  MODE_IDLE = 0,
  MODE_WAIT_ACK,
  MODE_SESSION,
  MODE_PORTAL
};

// -------------------------
// Breath phase
// -------------------------
enum BreathPhase : uint8_t {
  PHASE_INHALE = 0,
  PHASE_HOLD,
  PHASE_EXHALE,
  PHASE_FINISHED
};

// -------------------------
// User-adjustable settings
// -------------------------
struct Settings {
  uint16_t reminderMinutes;   // 5..240
  uint8_t  rounds;            // 1..12
  uint8_t  inhaleSec;         // 2..12
  uint8_t  holdSec;           // 0..12
  uint8_t  exhaleSec;         // 2..16
  uint8_t  brightness;        // 5..255
  uint8_t  volume;            // 0..100
  bool     soundEnabled;
  bool     reminderEnabled;
  uint8_t  preAnimSeconds;    // 1..12
  uint16_t portalTimeoutSec;  // 60..3600
};
