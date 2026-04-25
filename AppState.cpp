#include "AppState.h"

// -------------------------
// Hardware objects
// -------------------------
CRGB             leds[NUM_LEDS];
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
// LED breath order (centre → out)
// -------------------------
const uint8_t breathOrder[NUM_LEDS] = {4, 5, 3, 6, 2, 7, 1, 8, 0, 9};
