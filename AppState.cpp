#include "AppState.h"

// -------------------------
// Hardware objects
// -------------------------
const CRGB CRGB::Black(0, 0, 0);

CRGB             leds[NUM_LEDS];
Adafruit_NeoPixel strip(NUM_LEDS, PIN_LED_DATA, NEO_GRB + NEO_KHZ800);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
WebServer        server(80);
DNSServer        dnsServer;
Preferences      prefs;

// -------------------------
// Settings & app state
// -------------------------
Settings   settings;
AppMode    appMode     = MODE_IDLE;
BreathPhase currentPhase = PHASE_INHALE;

bool portalActive  = false;
bool sessionActive = false;

// -------------------------
// Timers
// -------------------------
unsigned long nextReminderAt       = 0;
unsigned long portalStartedAt      = 0;
unsigned long lastPortalActivityAt = 0;
unsigned long waitAckStartedAt     = 0;
unsigned long phaseStartedAt       = 0;
unsigned long lastToneUpdateAt     = 0;
unsigned long lastDisplayUpdateAt  = 0;
unsigned long lastIdleLedFrameAt   = 0;

uint8_t currentRound = 0;

// -------------------------
// AP credentials / network
// -------------------------
String    apSSID;
String    apPassword = "nefes123";
IPAddress apIP(192, 168, 4, 1);
IPAddress apGW(192, 168, 4, 1);
IPAddress apMask(255, 255, 255, 0);

// -------------------------
// LED breath order (centre → out, 26 LEDs)
// centres: 12, 13 then spreading outward
// -------------------------
const uint8_t breathOrder[NUM_LEDS] = {
  12, 13, 11, 14, 10, 15, 9, 16, 8, 17,
   7, 18,  6, 19,  5, 20, 4, 21, 3, 22,
   2, 23,  1, 24,  0, 25
};
