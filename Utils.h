#pragma once

#include <Arduino.h>
#include <math.h>

// Template must live in the header so the compiler can instantiate it
// for every T used across translation units.
template<typename T>
inline T clampValue(T v, T lo, T hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

inline float easeInOutSine(float x) {
  return -(cosf(PI * x) - 1.0f) / 2.0f;
}

// Reads last 2 hex digits of the chip's fuse MAC address.
String shortChipId();
