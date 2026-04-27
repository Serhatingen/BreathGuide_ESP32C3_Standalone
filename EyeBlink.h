#ifndef _EYEBLINK_h
#define _EYEBLINK_h

#include <Arduino.h>
#include "Animations.h"
#include "EyeConfig.h"

class EyeBlink {
public:
  EyeBlink();

  EyeConfig *Input;
  EyeConfig  Output;

  TrapeziumAnimation Animation;

  // BlinkWidth/Height tuned for the 128×32 display (original was 60/2)
  int32_t BlinkWidth  = 35;
  int32_t BlinkHeight = 1;

  void Update();
  void Apply(float t);
};

#endif
