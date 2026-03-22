#ifndef SRC_GAMEMENU_H_
#define SRC_GAMEMENU_H_

#include <asw/asw.h>

#include "State.h"

class GameMenu : public asw::scene::Scene<ProgramState> {
 public:
  using asw::scene::Scene<ProgramState>::Scene;

  void init() override;
  void update(float dt) override;
  void draw() override;

 private:
  asw::Texture image_menu;

  int indicator_position{0};
};

#endif  // SRC_GAMEMENU_H_
