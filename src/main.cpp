#include <asw/asw.h>

#include <array>
#include <chrono>
#include <iostream>
#include <numeric>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include "State.h"
#include "World.h"

// Images
using namespace std::chrono_literals;
using namespace std::chrono;
constexpr nanoseconds timestep(16ms);
constexpr nanoseconds fps_count(100ms);

// State engine
StateEngine game_state;

asw::Font font = nullptr;

// Setup game
void setup() {
  // Load allegro library
  asw::core::init(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, 2);

  asw::core::print_info();

  font = asw::assets::loadFont("assets/fonts/pixelart.ttf", 12);
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
void draw(int fps) {
  // Clear screen
  asw::draw::clearColor(asw::util::makeColor(0, 0, 0, 255));

  // Draw game state
  game_state.draw();

  asw::draw::text(font, "FPS: " + std::to_string(fps), 0, 0,
                  asw::util::makeColor(255, 255, 255));

  // Update screen
  SDL_RenderPresent(asw::display::renderer);
}

// Loop (emscripten compatibility)
#ifdef __EMSCRIPTEN__
void loop() {
  update();
  draw(0);
}
#endif

// Main function
int main(int argc, char* argv[]) {
  // Setup basic functionality
  setup();

  // Set the current state ID
  game_state.setNextState(ProgramState::MENU);

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(loop, 0, 1);
#else

  using clock = high_resolution_clock;
  nanoseconds accumulator(0ns);
  auto current_time = clock::now();

  // FPS
  auto frame_acc = 0ns;
  auto frame_buff = std::array<int, 10>();
  auto frame_buff_index = 0;
  auto frames = 0;
  auto fps = 0;

  while (!asw::core::exit) {
    auto new_time = clock::now();
    auto delta_time = new_time - current_time;
    current_time = new_time;

    accumulator += duration_cast<nanoseconds>(delta_time);

    // FPS calculation
    frame_acc += duration_cast<nanoseconds>(delta_time);

    // Calculate FPS every 100ms
    if (frame_acc >= fps_count) {
      frame_buff[frame_buff_index] = frames;
      frame_buff_index = (frame_buff_index + 1) % frame_buff.size();
      frames = 0;
      frame_acc -= fps_count;

      fps = std::accumulate(std::begin(frame_buff), std::end(frame_buff), 0);
      fps = fps * (1s / fps_count) / frame_buff.size();
    }

    while (accumulator >= timestep) {
      accumulator -= timestep;
      update();
    }

    frames += 1;

    draw(fps);
  }
#endif

  return 0;
}
