#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include "Joystick.h"

class Menu
{
public:
  Menu();
  void init(Joystick joystick);
  void update();
  void addItem(String label);
  uint8_t getActiveItem();
  String getActiveItemLabel();
  uint8_t getItemCount();
  String getMenuItem(uint8_t i);

private:
  Joystick joystick;
  String menuItems[4];
  uint8_t itemCount;
  uint8_t activeItem;
};

#endif