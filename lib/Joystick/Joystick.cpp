#include "Joystick.h"

Joystick::Joystick()
{}

Joystick::Joystick(uint8_t x_pin, uint8_t y_pin, uint8_t sw_pin)
{
  xPin = x_pin;
  yPin = y_pin;
  swPin = sw_pin;
  rotation = ROTATION_0;
  xMidpoint = 1255;
  yMidpoint = 2803;
  maxValue = 4095;
}

void Joystick::init()
{
  pinMode(swPin, INPUT_PULLUP);
  xInitValue = analogRead(xPin);
  yInitValue = analogRead(yPin);
  buttonValue = digitalRead(swPin);
  buttonPreviousValue = buttonValue;
}

void Joystick::setRotation(Rotation _rotation)
{
  rotation = _rotation;
}

void Joystick::update()
{
  int rawX = analogRead(xPin);
  int rawY = analogRead(yPin);
  Serial.printf("Joystick x:%d y:%d\n", rawX, rawY);
  int scaledX = 0;
  int scaledY = 0;

  // Ensure minor x-input deviations don't cause movement
  if(abs(xInitValue-rawX) >= 50) {
    Serial.println("stick movement! (X)");
    Serial.printf("rawX: %d xInitValue: %d\n", rawX, xInitValue);
    if(rawX > xInitValue)
      scaledX = map(rawX, xInitValue, 4095, 0, 100); //RIGHT
    else
      scaledX = map(rawX, 0, xInitValue, -100, 0); //LEFT
  }

  // Ensure minor y-input deviations don't cause movement
  if(abs(yInitValue-rawY) >= 35) {
    Serial.println("stick movement! (Y)");
    Serial.printf("rawY: %d yInitValue: %d\n", rawY, yInitValue);
    if(rawY > yInitValue)
      scaledY = map(rawY, yInitValue, 4095, 0, 100); //UP
    else
      scaledY = map(rawY, 0, yInitValue, -100, 0); //DOWN
  }
  
  // Adjust values based on the joystick orientation
  switch(rotation){
    case ROTATION_0:
      xValue = scaledX;
      yValue = scaledY;
      break;
    case ROTATION_90:
      xValue = -1 * scaledY;
      yValue = -1 * scaledX;
      break;
    case ROTATION_180:
      xValue = scaledY;
      yValue = scaledX;
      break;
    case ROTATION_270:
      xValue = -1 * scaledY;
      yValue = -1 * scaledX;
      break;
  }

  // Track the button's previous value to determine if it has changed, otherwise
  // buttonPressed() will be true on every loop while the button is pressed.
  buttonPreviousValue = buttonValue;
  buttonValue = digitalRead(swPin);

  Serial.printf("(updated) Joystick.xValue: %d yValue: %d\n", xValue, yValue);
}

int Joystick::getX()
{
  return xValue;
}

int Joystick::getY()
{
  return yValue;
}

bool Joystick::buttonPressed()
{
  return buttonValue == 0 && buttonValue != buttonPreviousValue;
}

// Sometimes we want a distinct direction, not a combination
// This could use some adjusting, but the values would vary from
// one joystick to the next.
Direction Joystick::direction()
{
  if(xValue < -40)
    return LEFT;
  else if(xValue > 40)
    return RIGHT;
  else if(yValue > 40)
    return UP;
  else if(yValue < -40)
    return DOWN;
  else
    return NONE;
}