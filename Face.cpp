/**
 * Face.cpp – modified from playfultechnology/esp32-eyes
 *
 * Changes vs. original:
 *  • Removed U8G2_SSD1306 global declaration (we use the U8G2Shim in Common.h)
 *  • Removed u8g2.setI2CAddress() / u8g2.begin() calls (display already inited)
 *  • u8g2.clearBuffer() / u8g2.sendBuffer() delegate to Adafruit display via shim
 */

#include "Face.h"
#include "Common.h"

Face::Face(uint16_t screenWidth, uint16_t screenHeight, uint16_t eyeSize)
  : LeftEye(*this), RightEye(*this),
    Blink(*this), Look(*this),
    Behavior(*this), Expression(*this)
{
  Width   = screenWidth;
  Height  = screenHeight;
  EyeSize = eyeSize;

  CenterX = Width  / 2;
  CenterY = Height / 2;

  LeftEye.IsMirrored = true;

  Behavior.Clear();
  Behavior.Timer.Start();
}

void Face::LookFront()  { Look.LookAt( 0.0f,  0.0f); }
void Face::LookRight()  { Look.LookAt(-1.0f,  0.0f); }
void Face::LookLeft()   { Look.LookAt( 1.0f,  0.0f); }
void Face::LookTop()    { Look.LookAt( 0.0f,  1.0f); }
void Face::LookBottom() { Look.LookAt( 0.0f, -1.0f); }

void Face::Wait(unsigned long milliseconds) {
  unsigned long start = millis();
  while (millis() - start < milliseconds) Draw();
}

void Face::DoBlink() { Blink.Blink(); }

void Face::Update() {
  if (RandomBehavior) Behavior.Update();
  if (RandomLook)     Look.Update();
  if (RandomBlink)    Blink.Update();
  Draw();
}

void Face::Draw() {
  u8g2.clearBuffer();   // → display.clearDisplay() via shim

  LeftEye.CenterX  = CenterX - EyeSize / 2 - EyeInterDistance;
  LeftEye.CenterY  = CenterY;
  LeftEye.Draw();

  RightEye.CenterX = CenterX + EyeSize / 2 + EyeInterDistance;
  RightEye.CenterY = CenterY;
  RightEye.Draw();

  u8g2.sendBuffer();    // → display.display() via shim
}
