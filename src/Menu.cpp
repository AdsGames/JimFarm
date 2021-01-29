#include "Menu.h"

#include "utility/Tools.h"

Menu::Menu() {
  indicator_location = 4;
  music_volume = 100;

  tick = 0;
  coin_frame = 0;
  coin_direction = false;

  state = MAIN_MENU;

  menu_image = loadBitmap("images/title_screen.png");
  help_image = loadBitmap("images/help.png");
  story_image = loadBitmap("images/story.png");
  options_image = loadBitmap("images/options.png");
  coin_flip = loadBitmap("images/coin_flip.png");
  options_slider = loadBitmap("images/options_slider.png");
  options_slidee = loadBitmap("images/options_slidee.png");
  options_indicator = loadBitmap("images/options_indicator.png");

  blip = loadSample("sfx/blip.wav");

  // Load fonts
  pixelart = loadFont("fonts/pixelart_condensed.pcx");
}

Menu::~Menu() {
  // dtor
}

void Menu::draw(BITMAP* tempBitmap) {
  draw_sprite(tempBitmap, menu_image, 0, 0);

  if (state == MAIN_MENU) {
    masked_blit(coin_flip, tempBitmap, 9 * (coin_frame / 5), 0, 187,
                147 - (indicator_location * 11), 9, 9);
  } else if (state == HELP) {
    draw_sprite(tempBitmap, help_image, 0, 0);
  } else if (state == STORY) {
    draw_sprite(tempBitmap, story_image, 0, 0);
  } else if (state == OPTIONS) {
    draw_sprite(tempBitmap, options_image, 0, 0);
    drawSlider(tempBitmap, 45, 25, music_volume, "Music Volume");
    masked_blit(options_indicator, tempBitmap, 9 * (coin_frame / 5), 0, 30,
                52 - (indicator_location * 11), 9, 9);
  }
}
void Menu::drawSlider(BITMAP* tempBitmap,
                      int x,
                      int y,
                      int value,
                      std::string title) {
  std::string text = title + ": %i";
  textprintf_ex(tempBitmap, pixelart, x, y, makecol(0, 0, 0), -1, text.c_str(),
                value);

  draw_sprite(tempBitmap, options_slider, x, y + 16);
  draw_sprite(tempBitmap, options_slidee, x - 2 + value, y - 1 + 16);
}

void Menu::update(StateEngine* engine) {
  tick++;
  if (state == MAIN_MENU) {
    if ((key[KEY_SPACE] || key[KEY_LCONTROL] || key[KEY_ENTER]) && tick > 10) {
      tick = 0;
      if (indicator_location == 4) {
        play_sample(blip, 255, 125, 1000, 0);
        setNextState(engine, StateEngine::STATE_GAME);
        return;
      } else if (indicator_location == 3) {
        play_sample(blip, 255, 125, 1000, 0);
        state = OPTIONS;
        indicator_location = 1;
      } else if (indicator_location == 2) {
        state = HELP;
        play_sample(blip, 255, 125, 1000, 0);
      } else if (indicator_location == 1) {
        state = STORY;
        play_sample(blip, 255, 125, 1000, 0);
      } else if (indicator_location == 0) {
        setNextState(engine, StateEngine::STATE_EXIT);
        return;
      }
    }

    if (key[KEY_DOWN] && tick > 10) {
      play_sample(blip, 255, 125, 1000, 0);
      tick = 0;
      indicator_location--;
    }
    if (key[KEY_UP] && tick > 10) {
      play_sample(blip, 255, 125, 1000, 0);
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
  if (state == OPTIONS) {
    if (key[KEY_DOWN] && tick > 10) {
      play_sample(blip, 255, 125, 1000, 0);
      tick = 0;
      indicator_location--;
    }
    if (key[KEY_UP] && tick > 10) {
      play_sample(blip, 255, 125, 1000, 0);
      tick = 0;
      indicator_location++;
    }
    if (key[KEY_RIGHT] && tick > 1) {
      if (indicator_location == 1) {
        // play_sample(blip,255,125,1000,0);
        tick = 0;
        music_volume++;
      }
    }
    if (key[KEY_LEFT] && tick > 1) {
      if (indicator_location == 1) {
        // play_sample(blip,255,125,1000,0);
        tick = 0;
        music_volume--;
      }
    }
    if (music_volume > 100)
      music_volume = 100;
    if (music_volume < 0)
      music_volume = 0;
  }
  // Coin spin
  if (!coin_direction)
    coin_frame++;
  if (coin_direction)
    coin_frame--;
  if (coin_frame > 18)
    coin_direction = true;
  if (coin_frame < 0) {
    coin_frame = 5;
    coin_direction = false;
  }

  if ((state == HELP || state == STORY) &&
      (key[KEY_SPACE] || key[KEY_LCONTROL] || key[KEY_UP] || key[KEY_DOWN] ||
       key[KEY_LEFT] || key[KEY_RIGHT]) &&
      tick > 10) {
    play_sample(blip, 255, 125, 1000, 0);
    tick = 0;
    state = MAIN_MENU;
  }

  return;
}
