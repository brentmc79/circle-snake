#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Constants.h"
#include "Joystick.h"
#include "Game.h"
#include "Menu.h"

#define OLED_ADDR 0x3C

// Screen pins
const uint8_t OLED_WIDTH = 128;
const uint8_t OLED_HEIGHT = 64;

// Joystick pins
const uint8_t X_PIN = 27;
const uint8_t Y_PIN = 26;
const uint8_t SW_PIN = 25;

const bool DEBUG = true;
uint8_t menuItem = 0;

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
Joystick joystick(X_PIN, Y_PIN, SW_PIN);
Game game;
View currentView;

void debug(String str)
{
  if(DEBUG)
    Serial.println(str);
}

void setup()
{
  debug("- Setup -");
  Serial.begin(115200);
  randomSeed(analogRead(X_PIN));

  joystick.init();
  joystick.setRotation(ROTATION_90);

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();

  game.init(display, joystick);
  currentView = START_VIEW;
}

void loop()
{
  debug("- Loop -");
  display.clearDisplay();
  joystick.update();

  switch(currentView)
  {
    case(START_VIEW):
    {
      debug(" - Start Menu");
      display.fillScreen(SSD1306_BLACK);
      display.setTextColor(SSD1306_INVERSE);

      display.fillRoundRect(13, 10, 100, 20, 4, SSD1306_WHITE);
      if(menuItem == 1)
        display.fillRoundRect(14, 11, 98, 18, 4, SSD1306_INVERSE);
      display.setTextSize(1);
      display.setCursor(46, 16);
      display.println("Play");

      display.fillRoundRect(13, 34, 100, 20, 4, SSD1306_WHITE);
      if(menuItem == 0)
        display.fillRoundRect(14, 35, 98, 18, 4, SSD1306_INVERSE);
      display.setTextSize(1);
      display.setCursor(32, 40);
      display.println("High Scores");

      if(joystick.buttonPressed())
      {
        if(menuItem == 0)
        {
          game.reset();
          currentView = PLAY_VIEW;
        }
        if(menuItem == 1)
        {
          menuItem = 0;
          // No action here since "High Scores" isn't implemented yet
        }
      } else {
        // Navigate the menu
        if(joystick.direction() == DOWN)
          menuItem = 1;
        if(joystick.direction() == UP)
          menuItem = 0;
      }
      break;
    }
    case(PLAY_VIEW):
    {
      debug(" - Play View >>>>>>>>>>>>>>>");

      // Call game.tick() once per loop. If it returns -1 the game has ended.
      if(game.tick() == -1)
      {
        currentView = GAME_OVER_VIEW;
        menuItem = 0;
      }
      else
        if(joystick.buttonPressed())
        {
          currentView = PAUSE_VIEW;
          menuItem = 0;
        }
      break;
    }
    case(PAUSE_VIEW):
    {
      debug(" - Pause View >>>>>>>>>>>>>>>");
      display.fillScreen(SSD1306_BLACK);
      display.setTextColor(SSD1306_INVERSE);

      // Show the Resume button
      display.fillRoundRect(13, 10, 100, 20, 4, SSD1306_WHITE);
      if(menuItem == 1)
        display.fillRoundRect(14, 11, 98, 18, 4, SSD1306_INVERSE);
      display.setTextSize(1);
      display.setCursor(41, 16);
      display.println("Resume");

      // Show the Quit button
      display.fillRoundRect(13, 34, 100, 20, 4, SSD1306_WHITE);
      if(menuItem == 0)
        display.fillRoundRect(14, 35, 98, 18, 4, SSD1306_INVERSE);
      display.setTextSize(1);
      display.setCursor(47, 40);
      display.println("Quit");

      if(joystick.buttonPressed())
      {
        if(menuItem == 0)
        {
          game.resume();
          currentView = PLAY_VIEW;
        }
        if(menuItem == 1)
        {
          game.reset();
          currentView = START_VIEW;
          menuItem = 0;
        }
      } else {
        // Navigate the menu
        if(joystick.direction() == DOWN)
          menuItem = 1;
        if(joystick.direction() == UP)
          menuItem = 0;
      }
      break;
    }
    case(GAME_OVER_VIEW):
    {
      debug(" - Game Over View >>>>>>>>>>>>>>>");
      display.fillScreen(SSD1306_BLACK);
      display.setTextColor(SSD1306_INVERSE);
      display.setTextSize(2);
      display.setCursor(10, 10);
      display.println("Game Over");
      display.setCursor(10, 30);
      display.printf("Score: %d", game.score);
      if(joystick.buttonPressed())
      {
        currentView = START_VIEW;
        game.reset();
      }
      break;
    }
  }

  display.display();
  delay(20);
}