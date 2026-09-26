#ifndef HUD_H_
#define HUD_H_

#include <functional>
#include <map>
#include <memory>
#include <string>

#include "UiController.h"
#include "UiSlot.h"

class Hud {
 public:
  Hud();

  void draw(const GameState& state);
  void update(GameState& state);

  void toggleUiController(const std::string& name,
                          const UiController& ui_controller);

  // Open a window from interfaces.json by name, if not already open.
  // Vertical position as a fraction of the free screen height, -1 centres.
  void open(const std::string& name, float vertical = -1.0F);

  // Close every window
  void closeAll();

  bool isOpen() const { return ui_controllers.size() > 0; }

  // Open window by name, or nullptr
  UiController* window(const std::string& name);

  // Shift click: move a stack to the other window, or craft all from an
  // output. True when the click was used.
  bool quickMove(UiController& from,
                 SlotType type,
                 ItemStack& stack,
                 GameState& state);

  // Drops items on the ground next to the player
  using DropHandler = std::function<void(const Item& item, int count)>;
  void setDropHandler(DropHandler handler) { drop = std::move(handler); }

  // Ignore input, e.g. while the recipe book is on top
  void setBlocked(bool value) { blocked = value; }
  bool isOpen(const std::string& name) const {
    return ui_controllers.contains(name);
  }

 private:
  // Return items held on the mouse once no windows are open
  void returnMouseItem();

  // Inventory shortcuts, true when they used this frame's click
  bool shortcuts(GameState& state);
  void collectAll(const std::string& id);
  void dropFrom(ItemStack& stack, int count);

  // Other open window with input slots (crafting, furnace, kiln), or nullptr
  UiController* openStation();

  DropHandler drop{};

  // Double click detection
  Uint64 last_click_ms{0};
  const ItemStack* last_click_stack{nullptr};

  std::map<std::string, UiController> ui_controllers;

  bool blocked{false};
};

#endif  // HUD_H_
