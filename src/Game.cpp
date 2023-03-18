#include "Game.h"

#include "GameMenu.h"
#include "Graphics.h"
#include "Menu.h"
#include "Tile.h"
#include "utility/Tools.h"
#include "utility/Vec2.h"

Game::Game() {
  // Nice Map
  farm_world.loadImages();

  // Setup jim
  jim = std::make_shared<Character>();
  jim->setPosition(Vec2<int>(15, 15) * TILE_SIZE);
  jim->loadData();
  Graphics::Instance()->add(jim, true);
}

void Game::update(StateEngine* engine) {
  // Update world
  farm_world.update();

  // Update character
  jim->update(farm_world);

  // Go to menu
  if (asw::input::keyboard.pressed[SDL_SCANCODE_ESCAPE]) {
    setNextState(engine, ProgramState::GAME_MENU);
  }
}

void Game::draw() {
  // Draw map
  farm_world.draw();

  // Draw inventory
  jim->drawInventory();
}
