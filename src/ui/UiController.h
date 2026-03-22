#ifndef SRC_UI_UI_CONTROLLER_H_
#define SRC_UI_UI_CONTROLLER_H_

#include <asw/asw.h>
#include <memory>
#include <vector>

#include "../Inventory.h"
#include "UiElement.h"

class GameState;
class UiSlot;
enum class SlotType;

constexpr int DRAG_BOX_HEIGHT = 10;

class UiController {
 public:
  UiController(const std::string& name, const asw::Vec2i& size);

  void draw(const GameState& state);

  void update(GameState& state);

  void addElement(std::shared_ptr<UiElement> element);

  std::shared_ptr<Inventory> getInventory() const;

  std::string& getName();

  const asw::Vec2i& getSize() const { return size; }

  void setPosition(const asw::Vec2i& pos) { position = pos; }

  // Put anything held on the mouse back into an inventory
  static void returnMouseItem(Inventory& inventory);

  // Is an item being dragged with the mouse
  static bool isHoldingItem();

 private:
  std::shared_ptr<UiElement> elementAt(const asw::Vec2i& pos) const;

  // Slot actions, true when the click was used
  bool clickBuy(const UiSlot& slot, GameState& state);
  bool clickSell(bool sell_all, GameState& state);
  bool clickOrder(GameState& state);
  bool clickOutput();

  // Keep the crafting output showing what the inputs make
  void updateRecipeOutput();

  std::vector<std::shared_ptr<ItemStack>> stacksOfType(SlotType type) const;

  std::shared_ptr<Inventory> inv{nullptr};

  std::string name{""};

  asw::Vec2i size{0, 0};
  asw::Vec2i position{0, 0};

  std::vector<std::shared_ptr<UiElement>> elements{};

  static std::shared_ptr<ItemStack> mouse_item;

  int currently_bound{0};

  bool dragging{false};
};

#endif  // SRC_UI_UI_CONTROLLER_H_
