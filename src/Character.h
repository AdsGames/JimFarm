#ifndef SRC_CHARACTER_H_
#define SRC_CHARACTER_H_

#include <asw/asw.h>
#include <array>
#include <memory>
#include <string>

#include "Inventory.h"
#include "Sprite.h"
#include "World.h"

#include "manager/InterfaceTypeManager.h"
#include "manager/interface_defs.h"
#include "ui/UiController.h"
#include "utility/Vec2.h"

const int HOTBAR_SIZE = 8;

class Character;

class CharacterForeground : public Sprite {
 public:
  explicit CharacterForeground(Character* charPtr);

  void draw(const Camera& camera) const override;

  void update();

 private:
  Character* char_ptr = nullptr;
};

class Character : public Sprite {
 public:
  // Ctor and dtor
  Character();

  // Load images and samples
  void loadData();

  // Position character
  void setPosition(Vec2<int> pos);

  // Draw
  void draw(const Camera& camera) const override;
  void drawInventory() const;

  std::shared_ptr<Item> getSelectedItem() const;

  // Update
  void update(World& world);

 private:
  // Character foreground
  std::shared_ptr<CharacterForeground> c_fore{nullptr};

  // Attached UI
  int attached_ui{-1};

  // Inventory UI
  UiController inventory_ui{
      InterfaceTypeManager::getInterfaceById(INTERFACE_INVENTORY)};

  // UI open
  bool ui_open{false};

  // Directions
  enum directions { DIR_DOWN = 1, DIR_UP = 2, DIR_RIGHT = 3, DIR_LEFT = 4 };

  // Fonts
  asw::Font pixelart{};

  // Item in hand
  int selected_item{0};

  // What tile you are over
  Vec2<int> indicator_pos{};

  // Movement
  char direction{1};
  bool moving{false};
  bool sound_step{false};
  char ani_ticker{0};

  // Images for ui and character
  asw::Texture image{};
  asw::Texture inventory_gui{};
  asw::Texture indicator{};
  asw::Texture coin{};

  // Sounds
  asw::Sample pickup{};
  asw::Sample drop{};
  std::array<asw::Sample, 2> step{};

  friend class CharacterForeground;
};

#endif  // SRC_CHARACTER_H_
