#ifndef SRC_GAME_H_
#define SRC_GAME_H_

#include <asw/asw.h>
#include <memory>
#include <string>
#include <vector>

#include "Character.h"
#include "State.h"
#include "World.h"
#include "ui/RecipeBook.h"

class Game : public asw::scene::Scene<ProgramState> {
 public:
  using asw::scene::Scene<ProgramState>::Scene;

  void init() override;
  void update(float dt) override;
  void draw() override;

  // Save the running game
  bool save();

 private:
  // Controls overlay
  void drawHelp(const asw::Vec2i& ui_size) const;

  World farm_world{};
  std::shared_ptr<Character> jim = nullptr;

  // UI is drawn at 1x here, then stretched to the screen by UI_SCALE
  asw::Texture ui_buffer{nullptr};

  asw::Font font{nullptr};

  // Scenes are re-initialised on every switch, keep the running game
  bool initialized{false};

  bool show_help{false};

  RecipeBook recipe_book{};
};

#endif  // SRC_GAME_H_
