#pragma once

// Breath session life-cycle + the automatic reminder scheduler.
void enterWaitAckMode();       // plays intro, shows "Seans basliyor", waits for button
void startSession();           // transitions to MODE_SESSION
void endSession();             // cleans up and returns to MODE_IDLE
void updateSession();          // must be called every loop() iteration while in MODE_SESSION
void handleReminderScheduler();// checks if the reminder timer has elapsed; call from loop()
