#ifndef _EYETRANSFORMATION_h
#define _EYETRANSFORMATION_h

#include <Arduino.h>
#include "Animations.h"
#include "EyeConfig.h"

struct Transformation {
  float MoveX  = 0.0f;
  float MoveY  = 0.0f;
  float ScaleX = 1.0f;
  float ScaleY = 1.0f;
};

class EyeTransformation {
public:
  EyeTransformation();

  EyeConfig *Input;
  EyeConfig  Output;

  Transformation Origin;
  Transformation Current;
  Transformation Destin;

  RampAnimation Animation;

  void Update();
  void Apply();
  void SetDestin(Transformation transformation);
};

#endif
