#include "Game.h"

Game::Game()
{}

void Game::init(Adafruit_SSD1306 _display, Joystick _joystick)
{
  display = _display;
  screenWidth = display.width();
  screenHeight = display.height();
  joystick = _joystick;
  reset();
}

Target Game::newRandomTarget()
{
  uint8_t targetX, targetY;
  targetX = (uint8_t)(random(118)+5);
  targetY = (uint8_t)(random(54)+5);
  uint8_t playerX = player.position.x;
  uint8_t playerY = player.position.y;

  // Ensure the target isn't too close to the player or too
  // near the edge of the screen
  while(targetX <= playerX+10 && targetX >= playerX-10)
    targetX = (uint8_t)(random(118)+5);

  while(targetY <= playerY+10 && targetY >= playerY-10)
    targetY = (uint8_t)(random(54)+5);

  return Target{Coord{targetX, targetY}, 2, (int)(millis() + 8000, false)};
}

void Game::reset()
{
  score = 0;
  startTime = millis();
  lastTickTime = startTime;
  elapsedTime = 0;
  player = Player{40, Coord{64, 32}};
  targets[0] = newRandomTarget();
}

void Game::resume()
{
  lastTickTime = millis();
}

bool Game::outOfTime()
{
  return (elapsedTime/1000) > 30; // This should be a constant
}

void Game::renderTimer()
{
  long time = millis();
  elapsedTime += time - lastTickTime;
  lastTickTime = time;
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(elapsedTime/1000);
}

void Game::renderScore()
{
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Adjust position if score is double-digits
  if(score > 9)
    display.setCursor(116, 0);
  else
    display.setCursor(122, 0);

  display.println(score);
}

// This will display the joystick input, split into x and y
// as a bars along the left and bottom edges of the screen
void Game::renderInput()
{
  int8_t inputX = joystick.getX()/100.0 * 42.0;
  int8_t step = 1;
  if(inputX < 0)
    step = -1;
  for(int8_t i=0; abs(i)<abs(inputX); i+=step)
    display.drawPixel(64 + i, 63, SSD1306_INVERSE);
  display.setCursor(50, 54);
  //display.print(joystick.getX());
  int8_t inputY = joystick.getY()/100.0 * -20.0;
  step = 1;
  if(inputY < 0)
    step = -1;
  for(int8_t i=0; abs(i)<abs(inputY); i+=step)
    display.drawPixel(0, 32 + i, SSD1306_INVERSE);
  display.setCursor(3, 25);
  //display.print(joystick.getY());
}

bool playerCanMove(Player player)
{
  return (player.size == 5);
}

void Game::render()
{
  if(playerCanMove(player))
  {
    renderTimer();
    renderScore();
    //renderInput();
  }

  if(targets[0].visible && targets[0].position.x > 0 && targets[0].position.y > 0)
    display.fillCircle(targets[0].position.x, targets[0].position.y, targets[0].size, SSD1306_WHITE);

  // The start game animation. Reduce size until playable (5px).
  if(player.size > 5)
    player.size = max(5, player.size-3);

  // Display previous positions (player tail)
  for(int8_t i = 0; i < 4; i++)
    if(player.previousPositions[i].x > 0 && player.previousPositions[i].y > 0)
      display.fillCircle(player.previousPositions[i].x, player.previousPositions[i].y, 4-i, SSD1306_WHITE);
  
  // Display player
  display.fillCircle(player.position.x, player.position.y, player.size, SSD1306_WHITE);
}

Player updatePosition(Player player, Joystick joystick)
{
  uint8_t newX, newY;

  newX = max(min(player.position.x + (joystick.getX() / 100.0) * 5.0, 128.0), 0.0);
  newY = max(min(player.position.y + (joystick.getY() / 100.0) * -5.0, 64.0), 0.0);

  // Shift the previous positions, making room for the new position
  for(uint8_t i = 4; i > 0; i--)
    player.previousPositions[i] = player.previousPositions[i-1];
  // Save the current position
  player.previousPositions[0] = player.position;

  // Set the new position
  player.position = Coord{newX, newY};

  return player;
}

// Caluclate the distance between the player and a target and compare
// it to the sum of the two objects' radii
bool collision(Player player, Target target)
{
  uint8_t x1, y1, x2, y2;
  x1 = player.position.x;
  y1 = player.position.y;
  x2 = target.position.x;
  y2 = target.position.y;

  return std::sqrt(std::pow((x2 - x1), 2) + std::pow((y2 - y1), 2)) < (player.size + target.size);
}

int8_t Game::tick()
{
  Serial.println(" - game tick - ");
  joystick.update();
  display.clearDisplay();

  if(playerCanMove(player))
  {
    Serial.println("Player can move!"); // ie; the animation is over
    player = updatePosition(player, joystick);

    if(targets[0].visible == false)
    {
      targets[0].visible = true;
      targets[0].expirationTime = millis() + 2000;
    } else {
      if(targets[0].expirationTime < millis())
        targets[0] = newRandomTarget();
      else
        if(collision(player, targets[0]))
        {
          score += 1;
          targets[0] = newRandomTarget();
        }
    }
  }

  // Update the display
  render();

  if(outOfTime())
    return -1;
  else
    return 0;
}