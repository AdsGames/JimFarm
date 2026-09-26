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
#include "ui/UiController.h"

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

  // Tools and seeds for a new game
  void giveStarterItems();

  // Position character
  void setPosition(const asw::Vec2i& pos);

  // Draw
  void draw(const Camera& camera) const override;
  void drawInventory(const asw::Vec2i& screen_size) const;

  std::shared_ptr<Item> getSelectedItem() const;

  // Update
  void update(World& world, float dt);

  // Stack in the selected hotbar slot
  std::shared_ptr<ItemStack> getHeldStack() const;

  // Top left of the tile under the cursor, in world pixels
  const asw::Vec2i& getCursorTile() const { return indicator_pos; }

 private:
  // Character foreground
  std::shared_ptr<CharacterForeground> c_fore{nullptr};

  // Inventory UI
  UiController inventory_ui{
      InterfaceTypeManager::getInterfaceByName("inventory")};

  // Directions
  enum directions { DIR_DOWN = 1, DIR_UP = 2, DIR_RIGHT = 3, DIR_LEFT = 4 };

  // Fonts

  // Item in hand
  int selected_item{0};

  // What tile you are over
  asw::Vec2i indicator_pos{};

  // Holding right click on food eats it, -1 when not eating
  float eat_timer{-1.0F};
  int eat_slot{0};

  // Holding right click with a spear charges a throw, -1 when not charging
  float throw_charge{-1.0F};

  // Movement
  int move_speed{2};
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
