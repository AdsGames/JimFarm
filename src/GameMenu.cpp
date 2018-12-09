#include "GameMenu.h"

#include "Tools.h"
#include "KeyListener.h"

GameMenu::GameMenu() {
  open = false;
}

GameMenu::~GameMenu() {}

void GameMenu::load_data() {
  image_menu = load_bitmap_ex("images/game_menu.png");
}

void GameMenu::trigger() {
  open = !open;
  indicator_position = 0;
}

void GameMenu::update() {
  // Menu
  if (KeyListener::keyPressed[KEY_ESC]) {
    trigger();
  }

  // Change cursor location
  if (KeyListener::keyPressed[KEY_DOWN]) {
    indicator_position++;
    if (indicator_position > 2) {
      indicator_position = 0;
    }
  }
  else if (KeyListener::keyPressed[KEY_UP]) {
    indicator_position--;
    if (indicator_position < 0) {
      indicator_position = 2;
    }
  }

  // Select
  if (open && (key[KEY_SPACE] || key[KEY_ENTER])) {
    // Menu
    if (indicator_position == 0) {
      game_state = 0;
      open = false;
    }
    // Save
    else if (indicator_position == 1) {

    }
    // Exit
    else{
      open = false;
    }
  }
}

// Draw menu
void GameMenu::draw (BITMAP *tempImage) {
  if (open) {
    draw_sprite (tempImage, image_menu, 0, 0);
    rectfill (tempImage, 84, 58 + (indicator_position * 17), 84 + 9, 58 + (indicator_position * 17) + 9, makecol (0, 0, 0));
    rectfill (tempImage, 136, 58 + (indicator_position * 17), 136 + 9, 58 + (indicator_position * 17) + 9, makecol (0, 0, 0));
  }
}

// Return if opened
bool GameMenu::isOpen() {
  return open;
}
