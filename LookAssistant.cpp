#include "LookAssistant.h"
#include "Face.h"
#include "EyeTransformation.h"

LookAssistant::LookAssistant(Face &face) : _face(face), Timer(4000) {
  Timer.Start();
}

void LookAssistant::LookAt(float x, float y) {
  // Scale movement to fit the 128×32 display.
  // Horizontal: ±15 px keeps eyes within the panel.
  // Vertical:   ±5  px keeps eyes within 32 px height.
  float moveX  = -15.0f * x;
  float moveY  =   5.0f * y;
  float scaleY_x = 1.0f - x * 0.2f;
  float scaleY_y = 1.0f - (y > 0 ? y : -y) * 0.3f;

  transformation.MoveX  = moveX;
  transformation.MoveY  = moveY;
  transformation.ScaleX = 1.0f;
  transformation.ScaleY = scaleY_x * scaleY_y;
  _face.RightEye.Transformation.SetDestin(transformation);

  float scaleY_x2 = 1.0f + x * 0.2f;
  transformation.ScaleY = scaleY_x2 * scaleY_y;
  _face.LeftEye.Transformation.SetDestin(transformation);

  _face.RightEye.Transformation.Animation.Restart();
  _face.LeftEye.Transformation.Animation.Restart();
}

void LookAssistant::Update() {
  Timer.Update();
  if (Timer.IsExpired()) {
    Timer.Reset();
    float x = random(-50, 50) / 100.0f;
    float y = random(-50, 50) / 100.0f;
    LookAt(x, y);
  }
}
