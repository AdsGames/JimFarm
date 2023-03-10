#include <asw/asw.h>
#include <chrono>
#include <memory>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include "World.h"

#include "State.h"

// Images
using namespace std::chrono_literals;
using namespace std::chrono;
constexpr nanoseconds timestep(16ms);

// Setup game
void setup() {
  // Load allegro library
  asw::core::init(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
}

/*********************
 *   Update logic
 *********************/
void update(StateEngine& game_state) {
  // Update core
  asw::core::update();

  // Update state
  game_state.update();

  // Handle exit
  if (game_state.getStateId() == ProgramState::EXIT) {
    asw::core::exit = true;
  }
}

/*********************
 *  Draw to screen
 *********************/
void draw(StateEngine& game_state) {
  // Clear screen
  asw::draw::clearColor(asw::util::makeColor(0, 0, 0, 255));

  // Draw game state
  game_state.draw();

  // Update screen
  SDL_RenderPresent(asw::display::renderer);
}

// Loop (emscripten compatibility)
#ifdef __EMSCRIPTEN__
void loop() {
  update();
  draw();
}
#endif

// Main function*/
int main(int argc, char* argv[]) {
  // Setup basic functionality
  setup();

  // State engine
  StateEngine game_state;

  // Set the current state ID
  game_state.setNextState(ProgramState::MENU);

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(loop, 0, 1);
#else

  using clock = high_resolution_clock;
  nanoseconds lag(0ns);
  auto time_start = clock::now();

  while (!asw::core::exit) {
    auto delta_time = clock::now() - time_start;
    time_start = clock::now();
    lag += duration_cast<nanoseconds>(delta_time);

    while (lag >= timestep) {
      lag -= timestep;
      update(game_state);
    }

    draw(game_state);
  }
#endif

  return 0;
}
