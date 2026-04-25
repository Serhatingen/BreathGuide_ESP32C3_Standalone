#pragma once

// NVS-backed settings persistence + reminder scheduler.
void loadSettings();
void saveSettings();
void scheduleNextReminder();
