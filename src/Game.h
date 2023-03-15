#ifndef SRC_GAME_H_
#define SRC_GAME_H_

#include <memory>

#include "Character.h"
#include "State.h"
#include "World.h"

class Game : public State {
 public:
  Game();
  ~Game() final = default;

  void update(StateEngine* engine) override;
  void draw() override;

 private:
  World farm_world{};
  std::shared_ptr<Character> jim = nullptr;
};

#endif  // SRC_GAME_H_
