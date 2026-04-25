#include "Button.h"
#include "AppState.h"
#include "Session.h"
#include "Portal.h"

// File-scope statics – persist between calls, not exposed to other modules.
static bool          lastButtonReading = HIGH;
static bool          stableButtonState = HIGH;
static unsigned long lastDebounceAt    = 0;
static unsigned long buttonPressedAt   = 0;
static bool          longPressHandled  = false;

static void onShortPress() {
  if (portalActive) return;

  if (appMode == MODE_IDLE) {
    enterWaitAckMode();
    return;
  }
  if (appMode == MODE_WAIT_ACK) {
    startSession();
    return;
  }
}

static void onLongPress() {
  if (portalActive) {
    stopPortal();
  } else {
    beginPortal();
  }
}

void handleButton() {
  bool reading = digitalRead(PIN_BUTTON);

  // Reset debounce timer on any change
  if (reading != lastButtonReading) {
    lastDebounceAt    = millis();
    lastButtonReading = reading;
  }

  if (millis() - lastDebounceAt > DEBOUNCE_MS) {
    if (reading != stableButtonState) {
      stableButtonState = reading;

      if (stableButtonState == LOW) {
        // Button just pressed down
        buttonPressedAt  = millis();
        longPressHandled = false;
      } else {
        // Button just released – fire short-press if long-press wasn't handled
        unsigned long pressedFor = millis() - buttonPressedAt;
        if (!longPressHandled && pressedFor < LONG_PRESS_MS) {
          onShortPress();
        }
      }
    }
  }

  // Fire long-press while still held
  if (stableButtonState == LOW && !longPressHandled) {
    if (millis() - buttonPressedAt >= LONG_PRESS_MS) {
      longPressHandled = true;
      onLongPress();
    }
  }
}
