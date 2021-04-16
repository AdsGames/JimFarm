#ifndef SRC_MENU_H_
#define SRC_MENU_H_

#include <allegro.h>
#include <vector>

#include "State.h"

#include "Character.h"
#include "Tile.h"

class Menu : public State {
 public:
  Menu();
  virtual ~Menu();

  void draw(BITMAP* tempBitmap) override;

  void update(StateEngine* engine) override;

 private:
  void drawSlider(BITMAP* tempBitmap,
                  int x,
                  int y,
                  int value,
                  std::string title);

  int tick;
  int coin_frame;
  bool coin_direction;
  int indicator_location;
  int music_volume;

  // Fonts
  FONT* pixelart;

  BITMAP* menu_image;
  BITMAP* story_image;
  BITMAP* help_image;
  BITMAP* options_image;
  BITMAP* coin_flip;
  BITMAP* options_slider;
  BITMAP* options_slidee;
  BITMAP* options_indicator;

  SAMPLE* blip;

  int state;
};

#endif  // SRC_MENU_H_
