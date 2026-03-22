#include <asw/asw.h>

#include "Game.h"
#include "GameMenu.h"
#include "Menu.h"

#include "State.h"
#include "World.h"

// Main function
int main(int argc, char* argv[]) {
  // Required for mac compatibility
  (void)argc;
  (void)argv;

  // Load allegro library
  asw::core::init(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, 1);
  asw::core::print_info();
  asw::display::set_icon("assets/icon.ico");

  asw::scene::SceneManager<ProgramState> app;
  app.register_scene<Menu>(ProgramState::MENU, app);
  app.register_scene<Game>(ProgramState::GAME, app);
  app.register_scene<GameMenu>(ProgramState::GAME_MENU, app);
  app.set_next_scene(ProgramState::MENU);
  app.start();

  asw::core::shutdown();

  return 0;
}
