#ifndef SRC_GAME_H_
#define SRC_GAME_H_

#include <asw/asw.h>
#include <memory>

#include "Character.h"
#include "State.h"
#include "World.h"

class Game : public asw::scene::Scene<ProgramState> {
 public:
  using asw::scene::Scene<ProgramState>::Scene;

  void init() override;
  void update(float dt) override;
  void draw() override;

 private:
  World farm_world{};
  std::shared_ptr<Character> jim = nullptr;
};

#endif  // SRC_GAME_H_
