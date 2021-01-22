#ifndef GAME_MENU_H
#define GAME_MENU_H

#include "State.h"

class GameMenu : public State {
 public:
  GameMenu();
  virtual ~GameMenu();

  void load_data();
  void trigger();
  virtual void update(StateEngine* engine) override;
  virtual void draw(BITMAP* tempImage) override;

 private:
  BITMAP* image_menu;

  int indicator_position;
};

#endif  // GAME_MENU_H
