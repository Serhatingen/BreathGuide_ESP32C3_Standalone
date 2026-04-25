#include "SettingsMgr.h"
#include "AppState.h"
#include "Utils.h"

void scheduleNextReminder() {
  if (!settings.reminderEnabled) {
    nextReminderAt = 0;
    return;
  }
  nextReminderAt = millis() + (unsigned long)settings.reminderMinutes * 60UL * 1000UL;
}

void saveSettings() {
  prefs.begin("breath", false);
  prefs.putUShort("rmin",  settings.reminderMinutes);
  prefs.putUChar( "rounds",settings.rounds);
  prefs.putUChar( "inh",   settings.inhaleSec);
  prefs.putUChar( "hold",  settings.holdSec);
  prefs.putUChar( "exh",   settings.exhaleSec);
  prefs.putUChar( "bri",   settings.brightness);
  prefs.putUChar( "vol",   settings.volume);
  prefs.putBool(  "snd",   settings.soundEnabled);
  prefs.putBool(  "rem",   settings.reminderEnabled);
  prefs.putUChar( "pre",   settings.preAnimSeconds);
  prefs.putUShort("ptout", settings.portalTimeoutSec);
  prefs.end();
}

void loadSettings() {
  prefs.begin("breath", true);
  settings.reminderMinutes  = prefs.getUShort("rmin",   45);
  settings.rounds           = prefs.getUChar( "rounds",  4);
  settings.inhaleSec        = prefs.getUChar( "inh",     4);
  settings.holdSec          = prefs.getUChar( "hold",    1);
  settings.exhaleSec        = prefs.getUChar( "exh",     6);
  settings.brightness       = prefs.getUChar( "bri",    90);
  settings.volume           = prefs.getUChar( "vol",    40);
  settings.soundEnabled     = prefs.getBool(  "snd",  true);
  settings.reminderEnabled  = prefs.getBool(  "rem",  true);
  settings.preAnimSeconds   = prefs.getUChar( "pre",    3);
  settings.portalTimeoutSec = prefs.getUShort("ptout", 600);
  prefs.end();

  // Clamp to valid ranges after reading from NVS
  settings.reminderMinutes  = clampValue<uint16_t>(settings.reminderMinutes,  5, 240);
  settings.rounds           = clampValue<uint8_t> (settings.rounds,           1,  12);
  settings.inhaleSec        = clampValue<uint8_t> (settings.inhaleSec,        2,  12);
  settings.holdSec          = clampValue<uint8_t> (settings.holdSec,          0,  12);
  settings.exhaleSec        = clampValue<uint8_t> (settings.exhaleSec,        2,  16);
  settings.brightness       = clampValue<uint8_t> (settings.brightness,       5, 255);
  settings.volume           = clampValue<uint8_t> (settings.volume,           0, 100);
  settings.preAnimSeconds   = clampValue<uint8_t> (settings.preAnimSeconds,   1,  12);
  settings.portalTimeoutSec = clampValue<uint16_t>(settings.portalTimeoutSec, 60, 3600);
}
