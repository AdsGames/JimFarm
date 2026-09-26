#include "Game.h"

#include "GameMenu.h"
#include "Graphics.h"
#include "Menu.h"
#include "Tile.h"
#include "utility/Tools.h"

void Game::init() {
  // Nice Map
  farm_world = World();
  farm_world.loadImages();

  // Setup jim
  jim = std::make_shared<Character>();
  jim->setPosition(asw::Vec2i(15, 15) * TILE_SIZE);
  jim->loadData();
  Graphics::Instance()->add(jim, true);
}

void Game::update(float dt) {
  // Update world
  farm_world.update(dt);

  // Update character
  jim->update(farm_world);

  // Go to menu
  if (asw::input::get_key_down(asw::input::Key::Escape)) {
    manager.set_next_scene(ProgramState::GAME_MENU);
  }
}

void Game::draw() {
  // Draw map
  farm_world.draw();

  // Draw inventory
  jim->drawInventory();
}
