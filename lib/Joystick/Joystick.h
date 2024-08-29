#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <Arduino.h>

enum Direction
{
  UP, DOWN, LEFT, RIGHT, NONE
};
enum Rotation
{
  ROTATION_0, ROTATION_90, ROTATION_180, ROTATION_270
};

class Joystick
{
public:
  Joystick();
  Joystick(uint8_t xPin, uint8_t yPin, uint8_t swPin);
  void init();
  void update();
  int getX();
  int getY();
  bool buttonPressed();
  Direction direction();
  void setRotation(Rotation);

private:
  Rotation rotation;
  uint8_t xPin;
  uint8_t yPin;
  uint8_t swPin;
  int xInitValue;
  int yInitValue;
  int xMidpoint;
  int yMidpoint;
  int xValue;
  int yValue;
  int maxValue;
  uint8_t buttonPreviousValue;
  uint8_t buttonValue;
};

#endif