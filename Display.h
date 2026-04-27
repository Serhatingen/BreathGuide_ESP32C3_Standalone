#pragma once

#include <Arduino.h>

// Generic centred two-line text (default sizes 1/1).
void drawCenteredText(const String &line1,
                      const String &line2 = "",
                      uint8_t size1 = 1,
                      uint8_t size2 = 1);

void drawBootScreen();
void drawIdleScreen();
void drawPortalInfoScreen();

// Call once after display.begin() to create the animated-eye Face object.
void initEyeFace();

// Renders one frame of the animated-eye idle screen.  Safe to call every loop.
void drawWaitingEyesFrame();

void drawSessionScreen(const char* phaseLabel,
                       uint8_t     secondsLeft,
                       uint8_t     roundIndex,
                       uint8_t     roundTotal);
