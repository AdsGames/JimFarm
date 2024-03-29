#include "GameMenu.h"

#include "utility/Tools.h"

GameMenu::GameMenu() {
  image_menu = asw::assets::loadTexture("assets/images/game_menu.png");
}

void GameMenu::update(StateEngine* engine) {
  // Change cursor location
  if (asw::input::keyboard.pressed[SDL_SCANCODE_DOWN]) {
    indicator_position++;
    if (indicator_position > 2) {
      indicator_position = 0;
    }
  } else if (asw::input::keyboard.pressed[SDL_SCANCODE_UP]) {
    indicator_position--;
    if (indicator_position < 0) {
      indicator_position = 2;
    }
  }

  // Select
  if (asw::input::keyboard.down[SDL_SCANCODE_SPACE] ||
      asw::input::keyboard.down[SDL_SCANCODE_RETURN]) {
    // Menu
    if (indicator_position == 0) {
      setNextState(engine, ProgramState::MENU);
    }
    // Save
    else if (indicator_position == 1) {
      // Cant do this yet!
    }
    // Exit
    else {
      setNextState(engine, ProgramState::GAME);
    }
  }
}

// Draw menu
void GameMenu::draw() {
  asw::draw::sprite(image_menu, 0, 0);
  asw::draw::rectFill(84, 58 + (indicator_position * 17), 9, 9,
                      asw::util::makeColor(0, 0, 0));
  asw::draw::rectFill(136, 58 + (indicator_position * 17), 9, 9,
                      asw::util::makeColor(0, 0, 0));
}
