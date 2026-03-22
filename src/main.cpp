#include <asw/asw.h>

#include <chrono>
#include <string_view>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Game.h"
#include "GameMenu.h"
#include "Menu.h"

#include "SelfTest.h"
#include "State.h"
#include "World.h"

// Main function
int main(int argc, char* argv[]) {
  // Load allegro library
  asw::core::init(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, 1);
  asw::core::print_info();
  asw::display::set_icon("assets/icon.ico");

#ifdef __EMSCRIPTEN__
  // Saves live in IndexedDB so they survive page reloads
  EM_ASM(FS.mkdir('/save'); FS.mount(IDBFS, {}, '/save');
         FS.syncfs(true, function(err){}););
#endif

  // Runs game logic without the window loop, for development
  if (argc > 1 && std::string_view(argv[1]) == "--selftest") {
    const int result = runSelfTest();
    asw::core::shutdown();
    return result;
  }

  asw::scene::SceneManager<ProgramState> app;

  // Game logic steps a fixed amount per update, tuned for 16ms updates
  app.set_timestep(std::chrono::milliseconds(16));
  app.register_scene<Menu>(ProgramState::MENU, app);
  app.register_scene<Game>(ProgramState::GAME, app);
  app.register_scene<GameMenu>(ProgramState::GAME_MENU, app);
  app.set_next_scene(ProgramState::MENU);
  app.start();

  asw::core::shutdown();

  return 0;
}
