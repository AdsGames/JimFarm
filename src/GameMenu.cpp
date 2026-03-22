#include "GameMenu.h"

#include "utility/Tools.h"

void GameMenu::init() {
  image_menu = asw::assets::load_texture("assets/images/game_menu.png");
}

void GameMenu::update(float dt) {
  // Change cursor location
  if (asw::input::get_key_down(asw::input::Key::Down)) {
    indicator_position++;
    if (indicator_position > 2) {
      indicator_position = 0;
    }
  } else if (asw::input::get_key_down(asw::input::Key::Up)) {
    indicator_position--;
    if (indicator_position < 0) {
      indicator_position = 2;
    }
  }

  // Select
  if (asw::input::get_key(asw::input::Key::Space) ||
      asw::input::get_key(asw::input::Key::Return)) {
    // Menu
    if (indicator_position == 0) {
      manager.set_next_scene(ProgramState::MENU);
    }
    // Save
    else if (indicator_position == 1) {
      // Cant do this yet!
    }
    // Exit
    else {
      manager.set_next_scene(ProgramState::GAME);
    }
  }
}

// Draw menu
void GameMenu::draw() {
  asw::draw::sprite(image_menu, asw::Vec2f(0, 0));
  asw::draw::rect_fill(asw::Quadf(84, 58 + (indicator_position * 17), 9, 9),
                       asw::color::black);
  asw::draw::rect_fill(asw::Quadf(136, 58 + (indicator_position * 17), 9, 9),
                       asw::color::black);
}
