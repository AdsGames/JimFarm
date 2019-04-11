#include "GameMenu.h"

#include "Tools.h"
#include "KeyListener.h"

GameMenu::GameMenu() {
  image_menu = load_bitmap_ex("images/game_menu.png");
  indicator_position = 0;
}

GameMenu::~GameMenu() {

}

void GameMenu::update(StateEngine *engine) {
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
  if (key[KEY_SPACE] || key[KEY_ENTER]) {
    // Menu
    if (indicator_position == 0) {
      setNextState(engine, StateEngine::STATE_MENU);
    }
    // Save
    else if (indicator_position == 1) {

    }
    // Exit
    else {
      setNextState(engine, StateEngine::STATE_GAME);
    }
  }
}

// Draw menu
void GameMenu::draw (BITMAP *tempImage) {
  draw_sprite (tempImage, image_menu, 0, 0);
  rectfill (tempImage, 84, 58 + (indicator_position * 17), 84 + 9, 58 + (indicator_position * 17) + 9, makecol (0, 0, 0));
  rectfill (tempImage, 136, 58 + (indicator_position * 17), 136 + 9, 58 + (indicator_position * 17) + 9, makecol (0, 0, 0));
}
