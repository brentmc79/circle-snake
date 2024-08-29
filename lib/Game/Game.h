#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "Constants.h"
#include "Joystick.h"

struct Coord {
  uint8_t x;
  uint8_t y;
};

struct Player {
  uint8_t size;
  Coord position;
  Coord previousPositions[4];
};

struct Target {
  Coord position;
  uint8_t size;
  int expirationTime;
  bool visible;
};

class Game
{
public:
  Game();
  void init(Adafruit_SSD1306 _display, Joystick _joystick);
  int8_t tick();
  void reset();
  void resume();
  uint8_t score;

private:
  Adafruit_SSD1306 display;
  Joystick joystick;
  uint8_t screenWidth;
  uint8_t screenHeight;
  long startTime;
  long elapsedTime;
  long lastTickTime;
  Player player;
  Target targets[3];

  void render();
  void renderScore();
  void renderTimer();
  void renderInput();
  bool outOfTime();
  Target newRandomTarget();
};

#endif