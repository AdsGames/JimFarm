#ifndef GAME_H
#define GAME_H

#include "State.h"

#include "Character.h"
#include "World.h"

class Game : public State {
 public:
  Game();
  virtual ~Game();

  virtual void update(StateEngine* engine) override;
  virtual void draw(BITMAP* buffer) override;

 protected:
 private:
  World farm_world;
  Character jim;
};

#endif  // GAME_H
