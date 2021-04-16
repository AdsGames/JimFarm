#ifndef SRC_GAME_H_
#define SRC_GAME_H_

#include "State.h"

#include "Character.h"
#include "World.h"

class Game : public State {
 public:
  Game();
  virtual ~Game();

  void update(StateEngine* engine) override;
  void draw(BITMAP* buffer) override;

 protected:
 private:
  World farm_world;
  Character jim;
};

#endif  // SRC_GAME_H_
