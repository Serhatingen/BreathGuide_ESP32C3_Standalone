#include "LedControl.h"
#include "AppState.h"
#include "Utils.h"

// -------------------------
// Internal helpers
// -------------------------

// Gamma 2.2 lookup – makes brightness feel perceptually linear
static const uint8_t GAMMA[256] PROGMEM = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255
};

static inline uint8_t gamma8(uint8_t v) { return pgm_read_byte(&GAMMA[v]); }

// HSV → CRGB (hue 0-255, sat 0-255, val 0-255)
static CRGB chsv(uint8_t h, uint8_t s, uint8_t v) {
  if (!s) return CRGB(gamma8(v), gamma8(v), gamma8(v));
  uint8_t region    = h / 43;
  uint8_t remainder = (h - region * 43) * 6;
  uint8_t p = (uint16_t)v * (255 - s) >> 8;
  uint8_t q = (uint16_t)v * (255 - ((uint16_t)s * remainder >> 8)) >> 8;
  uint8_t t = (uint16_t)v * (255 - ((uint16_t)s * (255 - remainder) >> 8)) >> 8;
  uint8_t r, g, b;
  switch (region) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    default: r = v; g = p; b = q; break;
  }
  return CRGB(gamma8(r), gamma8(g), gamma8(b));
}

// Scale a pixel by (scale/255), video mode (never goes to 0 if non-zero)
static CRGB scalePixel(const CRGB& c, uint8_t scale) {
  uint8_t r = (uint16_t)c.r * scale >> 8;
  uint8_t g = (uint16_t)c.g * scale >> 8;
  uint8_t b = (uint16_t)c.b * scale >> 8;
  if (c.r && !r) r = 1;
  if (c.g && !g) g = 1;
  if (c.b && !b) b = 1;
  return CRGB(r, g, b);
}

static void fill_solid(CRGB* arr, uint8_t count, const CRGB& color) {
  for (uint8_t i = 0; i < count; i++) arr[i] = color;
}

// Beat-synchronised sine LFO, returns value in [lowest, highest]
static float beatsinf(float bpm, float lowest, float highest) {
  float phase = (float)(millis() % (uint32_t)(60000.0f / bpm)) / (60000.0f / bpm);
  float s = (sinf(phase * 2.0f * (float)M_PI) + 1.0f) * 0.5f;
  return lowest + s * (highest - lowest);
}

// Push leds[] framebuffer with gamma correction to NeoPixel hardware
void showLeds() {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, leds[i].r, leds[i].g, leds[i].b);
  }
  strip.show();
}

// -------------------------

void clearLeds() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  showLeds();
}

// Gentle teal breath-glow idle animation at 60fps
void renderIdleLeds() {
  unsigned long now = millis();
  if (now - lastIdleLedFrameAt < 16) return;   // 60 fps
  lastIdleLedFrameAt = now;

  // Slow inhale/exhale sine with slight shoulder rounding
  float raw = sinf(now * 0.0018f);               // ~5.8 s cycle
  float s   = (raw + 1.0f) * 0.5f;
  s         = s * s * (3.0f - 2.0f * s);         // smoothstep
  // Range 80-180: after gamma8 these map to ~20-128, visible at strip brightness 90
  uint8_t bri = 80 + (uint8_t)(s * 100.0f);

  // Subtle shimmer: tiny individual offsets per LED
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    float shimmer = sinf(now * 0.003f + i * 0.55f);
    int8_t delta  = (int8_t)(shimmer * 10.0f);
    uint8_t v     = (uint8_t)constrain((int)bri + delta, 60, 200);
    leds[i] = chsv(148 + (int8_t)(shimmer * 2), 185, v);
  }
  showLeds();
}

// Rainbow plasma wave intro at 60fps
void renderIntroAnimationBlocking(uint16_t durationMs) {
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    unsigned long now     = millis();
    float         t       = now * 0.001f;
    uint8_t       baseHue = (uint8_t)((now / 8) % 256);

    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      float pos   = (float)i / (float)(NUM_LEDS - 1);
      float wave1 = (sinf(t * 1.7f  + pos * 6.5f) + 1.0f) * 0.5f;
      float wave2 = (sinf(t * 1.1f  - pos * 9.0f) + 1.0f) * 0.5f;
      float wave3 = (sinf(t * 2.3f  + pos * 3.2f) + 1.0f) * 0.5f;
      float v_f   = 14.0f + wave1 * 80.0f + wave2 * 50.0f + wave3 * 30.0f;
      uint8_t v   = (uint8_t)constrain((int)v_f, 0, 255);
      leds[i]     = chsv(baseHue + (uint8_t)(pos * 40.0f), 220, v);
    }

    // Travelling bright comet
    float  cpos = fmodf(t * 8.0f, (float)(NUM_LEDS + 4)) - 2.0f;
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      float dist = fabsf((float)i - cpos);
      if (dist < 3.0f) {
        uint8_t tail = (uint8_t)((1.0f - dist / 3.0f) * 60.0f);
        leds[i] += chsv(baseHue + 50, 80, tail);
      }
    }

    showLeds();
    delay(16);
  }
}

// Breath animation with anti-aliased fractional leading edge
void renderBreathLeds(float progress, BreathPhase phase) {
  progress = clampValue<float>(progress, 0.0f, 1.0f);

  // eased fill fraction (0.0 .. NUM_LEDS)
  float filled   = 0.0f;
  CRGB  color    = CRGB::Black;
  uint8_t baseHue = 0;

  switch (phase) {
    case PHASE_INHALE:
      filled  = easeInOutSine(progress) * NUM_LEDS;
      baseHue = 148;                        // teal-blue
      color   = chsv(baseHue, 215, 200);
      break;
    case PHASE_HOLD:
      filled  = (float)NUM_LEDS;
      baseHue = 35;                         // warm amber
      color   = chsv(baseHue, 215, 185);
      break;
    case PHASE_EXHALE:
      filled  = (1.0f - easeInOutSine(progress)) * NUM_LEDS;
      baseHue = 172;                        // violet-blue
      color   = chsv(baseHue, 185, 185);
      break;
    default:
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      showLeds();
      return;
  }

  uint8_t fullCount = (uint8_t)filled;
  float   fraction  = filled - (float)fullCount;   // 0.0..1.0 anti-alias factor

  fill_solid(leds, NUM_LEDS, CRGB::Black);

  // Fully lit pixels
  for (uint8_t i = 0; i < fullCount && i < NUM_LEDS; i++) {
    leds[breathOrder[i]] = color;
  }

  // Anti-aliased edge pixel: partial brightness between 0 and full
  if (fullCount < NUM_LEDS && fraction > 0.001f) {
    leds[breathOrder[fullCount]] = scalePixel(color, (uint8_t)(fraction * 255.0f));
  }

  // Per-phase brightness envelopes
  if (phase == PHASE_HOLD) {
    // Slow organic shimmer across all pixels during hold
    float t = (float)millis() * 0.001f;
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      float wave = (sinf(t * 1.4f + i * 0.38f) + 1.0f) * 0.5f;
      uint8_t sc = (uint8_t)(165.0f + wave * 70.0f);
      leds[i].nscale8_video(sc);
    }
  } else {
    // Breathing brightness envelope that fades with the fill
    float bright_f;
    if (phase == PHASE_INHALE) {
      bright_f = 40.0f + easeInOutSine(progress) * 160.0f;
    } else {
      bright_f = 40.0f + (1.0f - easeInOutSine(progress)) * 160.0f;
    }
    uint8_t sc = (uint8_t)constrain((int)bright_f, 30, 255);
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      leds[i].nscale8_video(sc);
    }
  }

  showLeds();
}
