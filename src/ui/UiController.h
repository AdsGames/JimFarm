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

  // Clicks are false when the hud already used this frame's click
  void update(GameState& state, bool clicks = true);

  void addElement(std::shared_ptr<UiElement> element);

  std::shared_ptr<Inventory> getInventory() const;

  std::string& getName();

  const asw::Vec2i& getSize() const { return size; }

  void setPosition(const asw::Vec2i& pos) { position = pos; }

  // Put anything held on the mouse back into an inventory
  static void returnMouseItem(Inventory& inventory);

  // Is an item being dragged with the mouse
  static bool isHoldingItem();

  // Stack held on the mouse
  static ItemStack& mouseStack() { return *mouse_item; }

  // Spread a left drag once the button is let go, call once per frame
  static void finishDrag();

  // Slot under a ui position, or nullptr
  std::shared_ptr<UiSlot> slotAt(const asw::Vec2i& ui_pos) const;

  // Is a ui position over the window or its drag bar
  bool contains(const asw::Vec2i& ui_pos) const;

  std::vector<std::shared_ptr<ItemStack>> stacksOfType(SlotType type) const;

  // Craft until the inputs run out or the bag is full, returns crafts made
  int craftAll(Inventory& bag, GameState& state);

 private:
  std::shared_ptr<UiElement> elementAt(const asw::Vec2i& pos) const;

  // Slot actions, true when the click was used
  bool clickBuy(const UiSlot& slot, GameState& state);
  bool clickSell(bool sell_all, GameState& state);
  bool clickOrder(GameState& state);
  bool clickOutput(GameState& state);

  // Keep the crafting output showing what the inputs make
  void updateRecipeOutput(const GameState& state);

  // Click on a slot that holds items
  void clickInput(ItemStack& stack, const asw::input::MouseState& mouse);

  // Shared pointer to one of this window's input stacks
  std::shared_ptr<ItemStack> ownStack(const ItemStack& stack) const;

  std::shared_ptr<Inventory> inv{nullptr};

  std::string name{""};

  asw::Vec2i size{0, 0};
  asw::Vec2i position{0, 0};

  std::vector<std::shared_ptr<UiElement>> elements{};

  static std::shared_ptr<ItemStack> mouse_item;

  // Mouse drag over slots: left spreads the held stack evenly, right puts
  // one in each slot
  struct SlotDrag {
    int button{0};
    int total{0};
    std::vector<std::shared_ptr<ItemStack>> slots{};
  };
  static SlotDrag slot_drag;

  int currently_bound{0};

  bool dragging{false};

  // Output shows a recipe that needs a better station
  bool output_locked{false};
};

#endif  // SRC_UI_UI_CONTROLLER_H_
