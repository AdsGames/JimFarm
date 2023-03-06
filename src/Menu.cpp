#include "Menu.h"

#include "utility/Tools.h"

Menu::Menu() {
  menu_image = asw::assets::loadTexture("assets/images/title_screen.png");
  help_image = asw::assets::loadTexture("assets/images/help.png");
  story_image = asw::assets::loadTexture("assets/images/story.png");
  options_image = asw::assets::loadTexture("assets/images/options.png");
  coin_flip = asw::assets::loadTexture("assets/images/coin_flip.png");
  options_slider = asw::assets::loadTexture("assets/images/options_slider.png");
  options_slidee = asw::assets::loadTexture("assets/images/options_slidee.png");
  options_indicator =
      asw::assets::loadTexture("assets/images/options_indicator.png");

  blip = asw::assets::loadSample("assets/sfx/blip.wav");

  music = asw::assets::loadSample("assets/sfx/farmy.ogg");
  asw::sound::play(music);

  // Load fonts
  pixelart = asw::assets::loadFont("assets/fonts/pixelart.ttf", 12);
}

void Menu::draw() {
  auto screenSize = asw::display::getLogicalSize();

  asw::draw::stretchSprite(menu_image, 0, 0, screenSize.x, screenSize.y);

  if (state == MenuState::MAIN_MENU) {
    asw::draw::stretchSpriteBlit(coin_flip, 9 * (coin_frame / 5), 0, 9, 9, 374,
                                 294 - (indicator_location * 22), 18, 18);
  } else if (state == MenuState::HELP) {
    asw::draw::stretchSprite(help_image, 0, 0, screenSize.x, screenSize.y);
  } else if (state == MenuState::STORY) {
    asw::draw::stretchSprite(story_image, 0, 0, screenSize.x, screenSize.y);
  } else if (state == MenuState::OPTIONS) {
    asw::draw::stretchSprite(options_image, 0, 0, screenSize.x, screenSize.y);
    drawSlider(90, 50, music_volume, "Music Volume");
    asw::draw::stretchSpriteBlit(options_indicator, 9 * (coin_frame / 5), 0, 9,
                                 9, 60, 102 - (indicator_location * 22), 18,
                                 18);
  }
}
void Menu::drawSlider(int x, int y, int value, std::string title) {
  std::string text = title + ": " + std::to_string(value);
  asw::draw::text(pixelart, text, x, y + 16, asw::util::makeColor(0, 0, 0));

  auto sliderSize = asw::util::getTextureSize(options_slider);
  asw::draw::stretchSprite(options_slider, x, y + 32, sliderSize.x * 2,
                           sliderSize.y * 2);

  auto slideeSize = asw::util::getTextureSize(options_slidee);
  asw::draw::stretchSprite(options_slidee, x - 4 + value * 2, y - 2 + 32,
                           slideeSize.x * 2, slideeSize.y * 2);
}

void Menu::update(StateEngine* engine) {
  tick++;
  if (state == MenuState::MAIN_MENU) {
    if ((asw::input::keyboard.down[SDL_SCANCODE_SPACE] ||
         asw::input::keyboard.down[SDL_SCANCODE_LCTRL] ||
         asw::input::keyboard.down[SDL_SCANCODE_RETURN]) &&
        tick > 10) {
      tick = 0;
      if (indicator_location == 4) {
        asw::sound::play(blip);
        setNextState(engine, ProgramState::GAME);
        return;
      } else if (indicator_location == 3) {
        asw::sound::play(blip);
        state = MenuState::OPTIONS;
        indicator_location = 1;
      } else if (indicator_location == 2) {
        state = MenuState::HELP;
        asw::sound::play(blip);
      } else if (indicator_location == 1) {
        state = MenuState::STORY;
        asw::sound::play(blip);
      } else if (indicator_location == 0) {
        setNextState(engine, ProgramState::EXIT);
        return;
      }
    }

    if (asw::input::keyboard.down[SDL_SCANCODE_DOWN] && tick > 10) {
      asw::sound::play(blip);
      tick = 0;
      indicator_location--;
    }
    if (asw::input::keyboard.down[SDL_SCANCODE_UP] && tick > 10) {
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
    if (asw::input::keyboard.down[SDL_SCANCODE_DOWN] && tick > 10) {
      asw::sound::play(blip);
      tick = 0;
      indicator_location--;
    }
    if (asw::input::keyboard.down[SDL_SCANCODE_UP] && tick > 10) {
      asw::sound::play(blip);
      tick = 0;
      indicator_location++;
    }
    if (asw::input::keyboard.down[SDL_SCANCODE_RIGHT] && tick > 1) {
      if (indicator_location == 1) {
        // asw::sound::play(blip,255,125,1000,0);
        tick = 0;
        music_volume++;
      }
    }
    if (asw::input::keyboard.down[SDL_SCANCODE_LEFT] && tick > 1) {
      if (indicator_location == 1) {
        // asw::sound::play(blip,255,125,1000,0);
        tick = 0;
        music_volume--;
      }
    }
    if (music_volume > 100) {
      music_volume = 100;
    }
    if (music_volume < 0) {
      music_volume = 0;
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

  if ((state == MenuState::HELP || state == MenuState::STORY) &&
      (asw::input::keyboard.down[SDL_SCANCODE_SPACE] ||
       asw::input::keyboard.down[SDL_SCANCODE_LCTRL] ||
       asw::input::keyboard.down[SDL_SCANCODE_UP] ||
       asw::input::keyboard.down[SDL_SCANCODE_DOWN] ||
       asw::input::keyboard.down[SDL_SCANCODE_LEFT] ||
       asw::input::keyboard.down[SDL_SCANCODE_RIGHT]) &&
      tick > 10) {
    asw::sound::play(blip);
    tick = 0;
    state = MenuState::MAIN_MENU;
  }
}
