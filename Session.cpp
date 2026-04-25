#include "Session.h"
#include "AppState.h"
#include "Audio.h"
#include "Display.h"
#include "LedControl.h"
#include "SettingsMgr.h"

// -------------------------
// Public API
// -------------------------

void enterWaitAckMode() {
  appMode          = MODE_WAIT_ACK;
  waitAckStartedAt = millis();

  drawCenteredText("Seans", "basliyor", 2, 1);
  cuteChime();
  // Blocking intro – only called outside portal mode so Wi-Fi is off.
  renderIntroAnimationBlocking((uint16_t)settings.preAnimSeconds * 1000U);
  clearLeds();
}

void startSession() {
  sessionActive    = true;
  appMode          = MODE_SESSION;
  currentRound     = 1;
  currentPhase     = PHASE_INHALE;
  phaseStartedAt   = millis();
  lastToneUpdateAt = 0;
  confirmChime();
}

void endSession() {
  sessionActive = false;
  appMode       = MODE_IDLE;
  currentPhase  = PHASE_FINISHED;
  stopTone();
  finishChime();
  clearLeds();
  drawCenteredText("Tamamlandi", "Guzel nefes", 1, 1);
  delay(900);
  drawIdleScreen();
  scheduleNextReminder();
}

void updateSession() {
  if (!sessionActive) return;

  unsigned long now = millis();
  uint16_t      phaseDurSec = 0;
  const char*   label       = "";

  switch (currentPhase) {
    case PHASE_INHALE: phaseDurSec = settings.inhaleSec; label = "AL";  break;
    case PHASE_HOLD:   phaseDurSec = settings.holdSec;   label = "TUT"; break;
    case PHASE_EXHALE: phaseDurSec = settings.exhaleSec; label = "VER"; break;
    default: break;
  }

  // Skip zero-length hold phase immediately
  if (phaseDurSec == 0 && currentPhase == PHASE_HOLD) {
    currentPhase   = PHASE_EXHALE;
    phaseStartedAt = now;
    return;
  }

  unsigned long phaseElapsedMs = now - phaseStartedAt;
  float progress = (float)phaseElapsedMs / (float)(phaseDurSec * 1000UL);
  if (progress > 1.0f) progress = 1.0f;

  // Continuous tone sweep
  if (settings.soundEnabled && now - lastToneUpdateAt >= 80) {
    lastToneUpdateAt = now;
    switch (currentPhase) {
      case PHASE_INHALE: setTone(260 + (uint16_t)(progress * 440.0f)); break;
      case PHASE_EXHALE: setTone(700 - (uint16_t)(progress * 480.0f)); break;
      case PHASE_HOLD:   setTone(520);                                  break;
      default: break;
    }
  }

  // OLED update (throttled)
  if (now - lastDisplayUpdateAt >= 90) {
    lastDisplayUpdateAt = now;
    uint8_t secsLeft = (phaseDurSec > 0)
      ? (uint8_t)max(0L, (long)phaseDurSec - (long)(phaseElapsedMs / 1000UL))
      : 0;
    drawSessionScreen(label, secsLeft, currentRound, settings.rounds);
  }

  renderBreathLeds(progress, currentPhase);

  // Phase transition
  if (phaseElapsedMs >= (unsigned long)phaseDurSec * 1000UL) {
    stopTone();

    if (currentPhase == PHASE_INHALE) {
      currentPhase   = (settings.holdSec > 0) ? PHASE_HOLD : PHASE_EXHALE;
      phaseStartedAt = now;
      return;
    }
    if (currentPhase == PHASE_HOLD) {
      currentPhase   = PHASE_EXHALE;
      phaseStartedAt = now;
      return;
    }
    if (currentPhase == PHASE_EXHALE) {
      if (currentRound >= settings.rounds) {
        endSession();
      } else {
        currentRound++;
        currentPhase   = PHASE_INHALE;
        phaseStartedAt = now;
      }
      return;
    }
  }
}

void handleReminderScheduler() {
  if (portalActive) return;
  if (appMode != MODE_IDLE) return;
  if (!settings.reminderEnabled) return;
  if (nextReminderAt == 0) scheduleNextReminder();

  // Signed-cast comparison is safe against millis() rollover here
  if ((long)(millis() - nextReminderAt) >= 0) {
    enterWaitAckMode();
  }
}
