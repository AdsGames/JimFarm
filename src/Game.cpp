#include "Game.h"

#include "GameMenu.h"
#include "Menu.h"
#include "Tile.h"
#include "utility/Tools.h"

#include "Graphics.h"

Game::Game() {
  // Nice Map
  farm_world.loadImages();

  // Setup jim
  jim.setPosition(15 * 16, 15 * 16);

  jim.loadData();
  jim.setWorld(&farm_world);
}

Game::~Game() {}

void Game::update(StateEngine* engine) {
  // Update world
  farm_world.update();

  // Update character
  jim.update();

  // Go to menu
  if (asw::input::keyboard.pressed[SDL_SCANCODE_ESCAPE])
    setNextState(engine, StateEngine::STATE_GAME_MENU);
}

void Game::draw() {
  // Draw map
  farm_world.draw();

  // Draw inventory
  jim.drawInventory();
}
