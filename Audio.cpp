#include "Audio.h"
#include "AppState.h"

void stopTone() {
  ledcWriteTone(PIN_BUZZER, 0);
}

void setTone(uint16_t freq) {
  if (!settings.soundEnabled || settings.volume == 0) {
    stopTone();
    return;
  }
  ledcWriteTone(PIN_BUZZER, freq);
}

void cuteChime() {
  if (!settings.soundEnabled || settings.volume == 0) return;
  const uint16_t tones[] = {740, 988, 1319, 988};
  const uint16_t durs[]  = { 70,  80,   95,  90};
  for (uint8_t i = 0; i < 4; i++) {
    setTone(tones[i]);
    delay(durs[i]);
    stopTone();
    delay(30);
  }
}

void confirmChime() {
  if (!settings.soundEnabled || settings.volume == 0) return;
  const uint16_t tones[] = {660, 990};
  const uint16_t durs[]  = { 80, 120};
  for (uint8_t i = 0; i < 2; i++) {
    setTone(tones[i]);
    delay(durs[i]);
    stopTone();
    delay(25);
  }
}

void finishChime() {
  if (!settings.soundEnabled || settings.volume == 0) return;
  const uint16_t tones[] = {523, 659, 784, 1047};
  const uint16_t durs[]  = { 60,  60,  80,  140};
  for (uint8_t i = 0; i < 4; i++) {
    setTone(tones[i]);
    delay(durs[i]);
    stopTone();
    delay(20);
  }
}
