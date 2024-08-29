#include "Menu.h"

Menu::Menu()
{
  itemCount = 0;
}

void Menu::init(Joystick _joystick)
{
  joystick = _joystick;
  activeItem = 0;
}

uint8_t Menu::getItemCount()
{
  return itemCount;
}

String Menu::getMenuItem(uint8_t i)
{
  return menuItems[i];
}

void Menu::update()
{
  joystick.update();
  switch(joystick.direction())
  {
    case UP:
      Serial.println("UP");
      if(activeItem != 0)
        activeItem -= 1;
      break;
    case DOWN:
      Serial.println("DOWN");
      if(activeItem != (itemCount - 1))
        activeItem += 1;
      break;
    case LEFT:
      Serial.println("LEFT");
      break;
    case RIGHT:
      Serial.println("RIGHT");
      break;
  }
}

void Menu::addItem(String label)
{
  menuItems[itemCount++] = label;
}

uint8_t Menu::getActiveItem()
{
  return activeItem;
}

String Menu::getActiveItemLabel()
{
  return menuItems[activeItem];
}