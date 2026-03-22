#include "Menu.h"

#include "utility/Tools.h"

void Menu::init() {
  menu_image = asw::assets::load_texture("assets/images/title_screen.png");
  help_image = asw::assets::load_texture("assets/images/help.png");
  story_image = asw::assets::load_texture("assets/images/story.png");
  options_image = asw::assets::load_texture("assets/images/options.png");
  coin_flip = asw::assets::load_texture("assets/images/coin_flip.png");
  options_slider =
      asw::assets::load_texture("assets/images/options_slider.png");
  options_slidee =
      asw::assets::load_texture("assets/images/options_slidee.png");
  options_indicator =
      asw::assets::load_texture("assets/images/options_indicator.png");

  blip = asw::assets::load_sample("assets/sfx/blip.wav");

  music = asw::assets::load_music("assets/sfx/farmy.ogg");
  asw::sound::play_music(music, 0.8F);

  // Load fonts
  pixelart = asw::assets::load_font("assets/fonts/pixelart.ttf", 12);
}

void Menu::draw() {
  auto screenSize = asw::display::get_logical_size();

  asw::draw::stretch_sprite(menu_image,
                            asw::Quadf(0, 0, screenSize.x, screenSize.y));

  if (state == MenuState::MAIN_MENU) {
    asw::draw::stretch_sprite_blit(
        coin_flip, asw::Quadf(9 * (coin_frame / 5), 0, 9, 9),
        asw::Quadf(374, 294 - (indicator_location * 22), 18, 18));
  } else if (state == MenuState::HELP) {
    asw::draw::stretch_sprite(help_image,
                              asw::Quadf(0, 0, screenSize.x, screenSize.y));
  } else if (state == MenuState::STORY) {
    asw::draw::stretch_sprite(story_image,
                              asw::Quadf(0, 0, screenSize.x, screenSize.y));
  } else if (state == MenuState::OPTIONS) {
    asw::draw::stretch_sprite(options_image,
                              asw::Quadf(0, 0, screenSize.x, screenSize.y));
    drawSlider(90, 50, asw::sound::get_music_volume() * 100.0F, "Music Volume");
    asw::draw::stretch_sprite_blit(
        options_indicator, asw::Quadf(9 * (coin_frame / 5), 0, 9, 9),
        asw::Quadf(60, 102 - (settings_indicator * 22), 18, 18));
  }
}

void Menu::drawSlider(int x, int y, int value, const std::string& title) const {
  std::string text = title + ": " + std::to_string(value);
  asw::draw::text(pixelart, text, asw::Vec2f(x, y + 16), asw::Color(0, 0, 0));

  auto sliderSize = asw::util::get_texture_size(options_slider);
  asw::draw::stretch_sprite(
      options_slider,
      asw::Quadf(x, y + 32, sliderSize.x * 2, sliderSize.y * 2));
  auto slideeSize = asw::util::get_texture_size(options_slidee);
  asw::draw::stretch_sprite(options_slidee,
                            asw::Quadf(x - 4 + value * 2, y - 2 + 32,
                                       slideeSize.x * 2, slideeSize.y * 2));
}

void Menu::update(float dt) {
  tick++;

  if (state == MenuState::MAIN_MENU) {
    if ((asw::input::get_key(asw::input::Key::Space) ||
         asw::input::get_key(asw::input::Key::LCtrl) ||
         asw::input::get_key(asw::input::Key::Return)) &&
        tick > 10) {
      tick = 0;

      if (indicator_location == 4) {
        asw::sound::play(blip);
        manager.set_next_scene(ProgramState::GAME);
      } else if (indicator_location == 3) {
        state = MenuState::OPTIONS;
        asw::sound::play(blip);
      } else if (indicator_location == 2) {
        state = MenuState::HELP;
        asw::sound::play(blip);
      } else if (indicator_location == 1) {
        state = MenuState::STORY;
        asw::sound::play(blip);
      } else if (indicator_location == 0) {
        manager.set_next_scene(ProgramState::EXIT);
      }
    }

    if (asw::input::get_key(asw::input::Key::Down) && tick > 10) {
      asw::sound::play(blip);
      tick = 0;
      indicator_location--;
    }

    if (asw::input::get_key(asw::input::Key::Up) && tick > 10) {
      asw::sound::play(blip);
      tick = 0;
      indicator_location++;
    }

    if (indicator_location > 4) {
      indicator_location = 0;
    }

    if (indicator_location < 0) {
      indicator_location = 4;
    }
  }

  if (state == MenuState::OPTIONS) {
    if (asw::input::get_key_down(asw::input::Key::Down) &&
        settings_indicator > 1) {
      asw::sound::play(blip);
      settings_indicator--;
    }

    if (asw::input::get_key_down(asw::input::Key::Up) &&
        settings_indicator < 1) {
      asw::sound::play(blip);
      settings_indicator++;
    }

    if (asw::input::get_key(asw::input::Key::Right) &&
        settings_indicator == 1 && asw::sound::get_music_volume() < 1.0F) {
      asw::sound::set_music_volume(asw::sound::get_music_volume() + 0.01F);
    }

    if (asw::input::get_key(asw::input::Key::Left) && settings_indicator == 1 &&
        asw::sound::get_music_volume() > 0) {
      asw::sound::set_music_volume(asw::sound::get_music_volume() - 0.01F);
    }
  }

  // Coin spin
  if (!coin_direction) {
    coin_frame++;
  }
  if (coin_direction) {
    coin_frame--;
  }
  if (coin_frame > 18) {
    coin_direction = true;
  }
  if (coin_frame < 0) {
    coin_frame = 5;
    coin_direction = false;
  }

  if ((state == MenuState::HELP || state == MenuState::STORY ||
       state == MenuState::OPTIONS) &&
      (asw::input::get_key(asw::input::Key::Space) ||
       asw::input::get_key(asw::input::Key::LCtrl) ||
       asw::input::get_key(asw::input::Key::M)) &&
      tick > 10) {
    asw::sound::play(blip);
    tick = 0;
    state = MenuState::MAIN_MENU;
  }
}
