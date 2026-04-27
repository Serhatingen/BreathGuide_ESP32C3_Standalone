#pragma once

#include "Config.h"  // BreathPhase

void showLeds();
void clearLeds();
void renderIdleLeds();
void renderIntroAnimationBlocking(uint16_t durationMs);  // blocking – call only outside loop
void renderBreathLeds(float progress, BreathPhase phase);
