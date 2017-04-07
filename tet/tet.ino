#include <Arduboy2.h>
#include "tetmain.h"

Arduboy2 arduboy;
TetrisGame *game;

int count = 0;

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(60);
  arduboy.initRandomSeed();
  game = new TetrisGame();
  game->initialize();
}

void loop() {
  // pause render until it's time for the next frame
  if (!(arduboy.nextFrame()))
    return;

  arduboy.clear();
  game->update();
  game->draw();
  arduboy.display();
}
