#ifndef SRC_CHARACTER_H_
#define SRC_CHARACTER_H_

#include <asw/asw.h>
#include <array>
#include <memory>
#include <string>

#include "Inventory.h"
#include "Sprite.h"
#include "World.h"

#include "ui/UI_Controller.h"

const int HOTBAR_SIZE = 8;

class Character;

class CharacterForeground : public Sprite {
 public:
  explicit CharacterForeground(Character* charPtr);

  void draw(float x_1, float y_1, float x_2, float y_2) override;

  Character* char_ptr = nullptr;
};

class Character : public Sprite {
 public:
  // Ctor and dtor
  Character();

  virtual ~Character() = default;

  // Set world pointer
  void setWorld(World* newTileMap);

  // Load images and samples
  void loadData();

  // Position character
  void setPosition(int newX, int newY) {
    x = newX;
    y = newY;
  }

  // Draw
  void draw(float x_1, float y_1, float x_2, float y_2) override;
  void drawInventory();

  // Update
  void update();

  // Character foreground
  CharacterForeground* c_fore = nullptr;

 private:
  // Attatched UI
  UI_Controller* attatched_ui = nullptr;

  // Inventory UI
  UI_Controller* inventory_ui = nullptr;
  UI_Controller* hotbar_ui = nullptr;

  // UI open
  bool ui_open = false;

  // Directions
  enum directions { DIR_DOWN = 1, DIR_UP = 2, DIR_RIGHT = 3, DIR_LEFT = 4 };

  // Fonts
  asw::Font pixelart{};
  World* map_pointer = nullptr;

  // Inventory
  std::shared_ptr<Inventory> character_inv = nullptr;

  // Item in hand
  int selected_item = 0;

  // What tile you are over
  int indicator_x = 0;
  int indicator_y = 0;

  // Movement
  char direction = 1;
  bool moving = false;
  bool sound_step = false;
  char ani_ticker = 0;

  // Images for ui and character
  asw::Texture image{};
  asw::Texture inventory_gui{};
  asw::Texture indicator{};
  asw::Texture coin{};

  // Sounds
  asw::Sample pickup{};
  asw::Sample drop{};
  asw::Sample step[2]{};

  friend class CharacterForeground;
};

#endif  // SRC_CHARACTER_H_
