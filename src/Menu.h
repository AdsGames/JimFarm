#ifndef SRC_MENU_H_
#define SRC_MENU_H_

#include <asw/asw.h>
#include <vector>

#include "State.h"

#include "Character.h"
#include "Tile.h"

enum class MenuState {
  MAIN_MENU,
  HELP,
  OPTIONS,
  STORY,
};

class Menu : public State {
 public:
  Menu();
  ~Menu() final = default;

  void draw() override;

  void update(StateEngine* engine) override;

 private:
  void drawSlider(int x, int y, int value, const std::string& title) const;

  int tick{0};
  int coin_frame{0};
  bool coin_direction{false};
  int indicator_location{4};
  int music_volume{100};

  // Fonts
  asw::Font pixelart;

  asw::Texture menu_image;
  asw::Texture story_image;
  asw::Texture help_image;
  asw::Texture options_image;
  asw::Texture coin_flip;
  asw::Texture options_slider;
  asw::Texture options_slidee;
  asw::Texture options_indicator;

  asw::Sample music;
  asw::Sample blip;

  MenuState state{MenuState::MAIN_MENU};
};

#endif  // SRC_MENU_H_
