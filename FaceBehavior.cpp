#include "Face.h"
#include "FaceBehavior.h"
#include "FaceEmotions.hpp"

FaceBehavior::FaceBehavior(Face &face) : _face(face), Timer(500) {
  Timer.Start();
  Clear();
  Emotions[(int)eEmotions::Normal] = 1.0f;
}

void FaceBehavior::SetEmotion(eEmotions emotion, float value) {
  Emotions[emotion] = value;
}

float FaceBehavior::GetEmotion(eEmotions emotion) {
  return Emotions[emotion];
}

void FaceBehavior::Clear() {
  for (int e = 0; e < eEmotions::EMOTIONS_COUNT; e++)
    Emotions[e] = 0.0f;
}

eEmotions FaceBehavior::GetRandomEmotion() {
  float total = 0.0f;
  for (int e = 0; e < eEmotions::EMOTIONS_COUNT; e++)
    total += Emotions[e];
  if (total == 0.0f) return eEmotions::Normal;

  float r = random(0, (long)(1000 * total)) / 1000.0f;
  float acc = 0.0f;
  for (int e = 0; e < eEmotions::EMOTIONS_COUNT; e++) {
    if (Emotions[e] == 0.0f) continue;
    acc += Emotions[e];
    if (r <= acc) return (eEmotions)e;
  }
  return eEmotions::Normal;
}

void FaceBehavior::Update() {
  Timer.Update();
  if (Timer.IsExpired()) {
    Timer.Reset();
    eEmotions next = GetRandomEmotion();
    if (CurrentEmotion != next) GoToEmotion(next);
  }
}

void FaceBehavior::GoToEmotion(eEmotions emotion) {
  CurrentEmotion = emotion;
  switch (emotion) {
    case eEmotions::Normal:      _face.Expression.GoTo_Normal();      break;
    case eEmotions::Angry:       _face.Expression.GoTo_Angry();       break;
    case eEmotions::Glee:        _face.Expression.GoTo_Glee();        break;
    case eEmotions::Happy:       _face.Expression.GoTo_Happy();       break;
    case eEmotions::Sad:         _face.Expression.GoTo_Sad();         break;
    case eEmotions::Worried:     _face.Expression.GoTo_Worried();     break;
    case eEmotions::Focused:     _face.Expression.GoTo_Focused();     break;
    case eEmotions::Annoyed:     _face.Expression.GoTo_Annoyed();     break;
    case eEmotions::Surprised:   _face.Expression.GoTo_Surprised();   break;
    case eEmotions::Skeptic:     _face.Expression.GoTo_Skeptic();     break;
    case eEmotions::Frustrated:  _face.Expression.GoTo_Frustrated();  break;
    case eEmotions::Unimpressed: _face.Expression.GoTo_Unimpressed(); break;
    case eEmotions::Sleepy:      _face.Expression.GoTo_Sleepy();      break;
    case eEmotions::Suspicious:  _face.Expression.GoTo_Suspicious();  break;
    case eEmotions::Squint:      _face.Expression.GoTo_Squint();      break;
    case eEmotions::Furious:     _face.Expression.GoTo_Furious();     break;
    case eEmotions::Scared:      _face.Expression.GoTo_Scared();      break;
    case eEmotions::Awe:         _face.Expression.GoTo_Awe();         break;
    default: break;
  }
}
