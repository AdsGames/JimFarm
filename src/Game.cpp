#include "Game.h"


#include "Tile.h"
#include "utility/Tools.h"
#include "Menu.h"
#include "utility/KeyListener.h"
#include "utility/MouseListener.h"
#include "GameMenu.h"

#include "Graphics.h"

Game::Game() {
  // Nice Map
  farm_world.init_ticker();
  farm_world.load_images();

  // Setup jim
  jim.setPosition (15 * 16, 15 * 16);

  jim.load_data();
  jim.setWorld (&farm_world);
}

Game::~Game() {

}

void Game::update(StateEngine *engine) {
  // Update world
  farm_world.update();

  // Update character
  jim.update();

  // Go to menu
  if (KeyListener::keyPressed[KEY_ESC])
    setNextState(engine, StateEngine::STATE_GAME_MENU);
}


void Game::draw(BITMAP *buffer) {
  // Draw background
  rectfill (buffer, 0, 0, SCREEN_W, SCREEN_H, makecol (0, 0, 0));

  // Draw map
  farm_world.draw(buffer);

  // Draw inventory
  jim.draw_inventory(buffer);
}
