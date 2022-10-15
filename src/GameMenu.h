#ifndef SRC_GAMEMENU_H_
#define SRC_GAMEMENU_H_

#include "State.h"

class GameMenu : public State {
 public:
  GameMenu();
  virtual ~GameMenu();

  void loadData();
  void trigger();

  void update(StateEngine* engine) override;
  void draw() override;

 private:
  asw::Texture image_menu;

  int indicator_position;
};

#endif  // SRC_GAMEMENU_H_
