#include "LedControl.h"
#include "AppState.h"
#include "Utils.h"

void clearLeds() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void renderIdleLeds() {
  unsigned long now = millis();
  if (now - lastIdleLedFrameAt < 24) return;
  lastIdleLedFrameAt = now;

  float s = (sinf(now * 0.0024f) + 1.0f) * 0.5f;
  uint8_t bri = 5 + (uint8_t)(s * 18.0f);

  fill_solid(leds, NUM_LEDS, CHSV(145, 180, bri));
  leds[0]            = CHSV(150, 180, bri / 2);
  leds[NUM_LEDS - 1] = CHSV(150, 180, bri / 2);
  FastLED.show();
}

void renderIntroAnimationBlocking(uint16_t durationMs) {
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    unsigned long now     = millis();
    uint8_t       baseHue = (now / 10) % 255;

    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      float   pos   = (float)i / (float)(NUM_LEDS - 1);
      float   wave1 = (sinf((now * 0.010f)  + pos * 5.2f) + 1.0f) * 0.5f;
      float   wave2 = (sinf((now * 0.0065f) - pos * 7.0f) + 1.0f) * 0.5f;
      uint8_t v     = (uint8_t)(18.0f + (wave1 * 110.0f) + (wave2 * 50.0f));
      leds[i]       = CHSV(baseHue + i * 10, 210, v);
    }
    leds[beatsin8(18, 0, NUM_LEDS - 1)] += CHSV(baseHue + 40, 40, 35);
    FastLED.show();
    delay(16);
  }
}

void renderBreathLeds(float progress, BreathPhase phase) {
  progress = clampValue<float>(progress, 0.0f, 1.0f);

  uint8_t litCount = 0;
  CRGB    color    = CRGB::Black;

  switch (phase) {
    case PHASE_INHALE:
      litCount = (uint8_t)roundf(easeInOutSine(progress) * NUM_LEDS);
      color    = CHSV(145, 220, 200);
      break;
    case PHASE_HOLD:
      litCount = NUM_LEDS;
      color    = CHSV(35, 220, 180);
      break;
    case PHASE_EXHALE:
      litCount = NUM_LEDS - (uint8_t)roundf(easeInOutSine(progress) * NUM_LEDS);
      color    = CHSV(170, 180, 180);
      break;
    default:
      break;
  }

  fill_solid(leds, NUM_LEDS, CRGB::Black);
  for (uint8_t i = 0; i < litCount; i++) {
    leds[breathOrder[i]] = color;
  }

  if (phase == PHASE_HOLD) {
    uint8_t sparkle = beatsin8(10, 10, 35);
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      leds[i].nscale8_video(180 + sparkle);
    }
  } else {
    uint8_t globalB = 50 + (uint8_t)(progress * 150.0f);
    if (phase == PHASE_EXHALE) {
      globalB = 50 + (uint8_t)((1.0f - progress) * 150.0f);
    }
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      leds[i].nscale8_video(globalB);
    }
  }

  FastLED.show();
}
