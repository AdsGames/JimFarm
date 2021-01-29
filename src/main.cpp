#include <allegro.h>

#include "utility/KeyListener.h"
#include "utility/MouseListener.h"

#include "World.h"

#include "State.h"

// Images
BITMAP* buffer;

bool close_button_pressed;

// Listen to the keys
KeyListener keys;
MouseListener mouses;

// State engine
StateEngine game_state;

// FPS System
volatile int ticks = 0;
const int updates_per_second = 60;
volatile int game_time = 0;

int fps;
int frames_done;
int old_time;

void ticker() {
  ticks++;
}
END_OF_FUNCTION(ticker)

void game_time_ticker() {
  game_time++;
}
END_OF_FUNCTION(ticker)

void close_button_handler(void) {
  close_button_pressed = TRUE;
}
END_OF_FUNCTION(close_button_handler)

// Game clock
int animationFrame = 0;

void animationTicker() {
  animationFrame++;
  if (animationFrame > 10)
    animationFrame = 0;
}
END_OF_FUNCTION(ticker)

/*********************
 *   Update logic
 *********************/
void update() {
  // Checks keys JUST pressed or JUST released
  keys.update();
  mouses.update();

  // Update state
  game_state.update();

  // Handle exit
  if (game_state.getStateId() == StateEngine::STATE_EXIT)
    close_button_pressed = true;
}

/*********************
 *  Draw to screen
 *********************/
void draw() {
  // Draw game state
  game_state.draw(buffer);

  // FPS
  textprintf_ex(buffer, font, 0, 145, makecol(255, 255, 255), makecol(0, 0, 0),
                "FPS:%d", fps);

  // Draw to screen
  stretch_sprite(screen, buffer, 0, 0, SCREEN_W, SCREEN_H);
}

/*********************
 *   Setup game
 *********************/
void setup() {
  allegro_init();
  install_timer();
  install_keyboard();
  install_mouse();
  install_joystick(JOY_TYPE_AUTODETECT);

  set_color_depth(32);

  int w, h;
  get_desktop_resolution(&w, &h);

  if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, World::VIEWPORT_WIDTH * 4,
                   World::VIEWPORT_HEIGHT * 4, 0, 0)) {
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, World::VIEWPORT_WIDTH * 3,
                     World::VIEWPORT_HEIGHT * 3, 0, 0)) {
      if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, World::VIEWPORT_WIDTH * 2,
                       World::VIEWPORT_HEIGHT * 2, 0, 0)) {
        if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, World::VIEWPORT_WIDTH * 1,
                         World::VIEWPORT_HEIGHT * 1, 0, 0)) {
        }
      }
    }
  }

  install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, ".");

  set_window_title("Jim Farm");

  // Create buffer
  buffer = create_bitmap(World::VIEWPORT_WIDTH, World::VIEWPORT_HEIGHT);

  srand(time(NULL));

  // Setup for FPS system
  LOCK_VARIABLE(ticks);
  LOCK_FUNCTION(ticker);
  install_int_ex(ticker, BPS_TO_TIMER(updates_per_second));

  LOCK_VARIABLE(game_time);
  LOCK_FUNCTION(game_time_ticker);
  install_int_ex(game_time_ticker, BPS_TO_TIMER(10));

  // Animation ticks
  LOCK_VARIABLE(animationFrame);
  LOCK_FUNCTION(animationTicker);
  install_int(animationTicker, 10);
  install_int_ex(animationTicker, BPS_TO_TIMER(100));

  // Close button
  LOCK_FUNCTION(close_button_handler);
  set_close_button_callback(close_button_handler);

  // main_game_menu.loadData();

  // Alpha blender
  set_alpha_blender();
}

/*********************
 *   Start here
 *********************/
int main() {
  // Basic init
  setup();

  // Set the current state ID
  game_state.setNextState(StateEngine::STATE_GAME);

  while (!close_button_pressed) {
    while (ticks == 0) {
      rest(1);
    }
    while (ticks > 0) {
      int old_ticks = ticks;

      update();

      ticks--;
      if (old_ticks <= ticks) {
        break;
      }
    }
    if (game_time - old_time >= 10) {
      fps = frames_done;
      frames_done = 0;
      old_time = game_time;
    }
    draw();
    frames_done++;
  }

  return 0;
}
END_OF_MAIN()
