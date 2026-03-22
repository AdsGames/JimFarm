#include "GameMenu.h"

#include "State.h"
#include "utility/Tools.h"

void GameMenu::init() {
  image_menu = asw::assets::load_texture("assets/images/game_menu.png");
}

void GameMenu::update(float dt) {
  (void)dt;

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

  // Back to the game
  if (asw::input::get_key_down(asw::input::Key::Escape)) {
    pending_game_action = GameAction::Resume;
    manager.set_next_scene(ProgramState::GAME);
    return;
  }

  // Select
  if (asw::input::get_key_down(asw::input::Key::Space) ||
      asw::input::get_key_down(asw::input::Key::Return)) {
    // Menu, save first so nothing is lost
    if (indicator_position == 0) {
      if (save_current_game) {
        save_current_game();
      }
      manager.set_next_scene(ProgramState::MENU);
    }
    // Save
    else if (indicator_position == 1) {
      if (save_current_game) {
        save_current_game();
      }
      pending_game_action = GameAction::Resume;
      manager.set_next_scene(ProgramState::GAME);
    }
    // Back
    else {
      pending_game_action = GameAction::Resume;
      manager.set_next_scene(ProgramState::GAME);
    }
  }
}

// Draw menu
void GameMenu::draw() {
  asw::draw::stretch_sprite(image_menu, asw::Quadf(0, 0, 240 * 4, 160 * 4));
  asw::draw::rect_fill(asw::Quadf(84, 58 + (indicator_position * 17), 9, 9) * 4,
                       asw::color::black);
  asw::draw::rect_fill(
      asw::Quadf(136, 58 + (indicator_position * 17), 9, 9) * 4,
      asw::color::black);
}
