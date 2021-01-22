#ifndef MENU_H
#define MENU_H

#include <allegro.h>
#include <vector>

#include "State.h"

#include "Character.h"
#include "Tile.h"

class Menu : public State {
 public:
  Menu();
  virtual ~Menu();

  virtual void draw(BITMAP* tempBitmap) override;

  virtual void update(StateEngine* engine) override;

 private:
  void draw_slider(BITMAP* tempBitmap,
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
  FONT* f1;
  FONT* f2;
  FONT* f3;
  FONT* f4;
  FONT* f5;
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

#endif  // MENU_H
