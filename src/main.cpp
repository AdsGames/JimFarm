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

// State engine
StateEngine game_state;

// FPS system
int fps = 0;
int frames_done = 0;

// Setup game
void setup() {
  // Load allegro library
  asw::core::init(World::VIEWPORT_WIDTH, World::VIEWPORT_HEIGHT);
}

/*********************
 *   Update logic
 *********************/
void update() {
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
void draw() {
  // Clear screen
  SDL_RenderClear(asw::display::renderer);

  // Draw game state
  game_state.draw();

  // FPS
  // asw::draw::text(buffer, font, 0, 145, asw::util::makeColor(255, 255, 255),
  //               asw::util::makeColor(0, 0, 0), "FPS:%d", fps);

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
      update();
    }

    draw();
    frames_done++;
  }
#endif

  return 0;
}
