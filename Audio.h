#pragma once

#include <Arduino.h>

// Stop / start the passive buzzer, and play the three canned chimes.
void stopTone();
void setTone(uint16_t freq);

void cuteChime();       // played on reminder trigger
void confirmChime();    // played when session starts
void finishChime();     // played when session ends
