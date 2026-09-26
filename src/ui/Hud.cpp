#include "Hud.h"

#include <algorithm>
#include <array>
#include <vector>

#include "../GameState.h"
#include "../manager/InterfaceTypeManager.h"
#include "../manager/SoundManager.h"
#include "UiSlot.h"
#include "UiScale.h"

Hud::Hud() {}

void Hud::draw(const GameState& state) {
  for (auto& [_key, ui] : ui_controllers) {
    ui.draw(state);
  }
}

void Hud::toggleUiController(const std::string& name,
                             const UiController& ui_controller) {
  if (ui_controllers.contains(name)) {
    ui_controllers.erase(name);
    returnMouseItem();
    return;
  }

  ui_controllers.insert({name, ui_controller});
}

void Hud::open(const std::string& name, float vertical) {
  if (ui_controllers.contains(name)) {
    return;
  }

  auto controller = InterfaceTypeManager::getInterfaceByName(name);

  if (vertical >= 0.0F) {
    const auto free = getUiSize() - controller.getSize();
    controller.setPosition(asw::Vec2i(
        free.x / 2, DRAG_BOX_HEIGHT + static_cast<int>(free.y * vertical)));
  }

  ui_controllers.insert({name, controller});
}

void Hud::closeAll() {
  ui_controllers.clear();
  returnMouseItem();
}

void Hud::returnMouseItem() {
  if (ui_controllers.empty()) {
    UiController::returnMouseItem(
        *InterfaceTypeManager::getInterfaceByName("inventory").getInventory());
  }
}

void Hud::update(GameState& state) {
  if (blocked) {
    return;
  }

  const bool used = !ui_controllers.empty() && shortcuts(state);

  for (auto& [_, ui] : ui_controllers) {
    ui.update(state, !used);
  }

  UiController::finishDrag();

  if (asw::input::get_key_down(asw::input::Key::E)) {
    this->toggleUiController(
        "inventory", InterfaceTypeManager::getInterfaceByName("inventory"));
  }

  if (asw::input::get_key_down(asw::input::Key::Q)) {
    this->toggleUiController(
        "crafting", InterfaceTypeManager::getInterfaceByName("crafting"));
  }

  if (asw::input::get_key_down(asw::input::Key::G)) {
    this->toggleUiController(
        "furnace", InterfaceTypeManager::getInterfaceByName("furnace"));
  }
}

namespace {
constexpr int MOUSE_LEFT = 1;
constexpr int MOUSE_RIGHT = 3;
constexpr Uint64 DOUBLE_CLICK_MS = 300;
constexpr int HOTBAR_SLOTS = 8;

Inventory& bag() {
  return *InterfaceTypeManager::getInterfaceByName("inventory").getInventory();
}

// Move a whole stack into targets, same item first then an empty slot
bool moveStack(ItemStack& from,
               const std::vector<std::shared_ptr<ItemStack>>& targets) {
  const auto item = from.getItem();
  if (!item) {
    return false;
  }

  for (auto const& target : targets) {
    if (target.get() != &from && target->getItem() &&
        target->getItem()->getType().getId() == item->getType().getId()) {
      target->add(from.getQuantity());
      from.clear();
      return true;
    }
  }

  for (auto const& target : targets) {
    if (target.get() != &from && !target->getItem()) {
      target->setItem(item, from.getQuantity());
      from.clear();
      return true;
    }
  }

  return false;
}

void swapStacks(ItemStack& a, ItemStack& b) {
  const auto item = a.getItem();
  const int quantity = a.getQuantity();

  if (b.getItem()) {
    a.setItem(b.getItem(), b.getQuantity());
  } else {
    a.clear();
  }

  if (item) {
    b.setItem(item, quantity);
  } else {
    b.clear();
  }
}
}  // namespace

UiController* Hud::openStation() {
  for (auto& [name, ui] : ui_controllers) {
    if (name != "inventory" && name != "shop" &&
        !ui.stacksOfType(SlotType::Input).empty()) {
      return &ui;
    }
  }
  return nullptr;
}

bool Hud::shortcuts(GameState& state) {
  const auto& mouse = asw::input::get_mouse();
  const auto mouse_pos = getUiMouse();
  auto& held = UiController::mouseStack();

  UiController* hovered_ui = nullptr;
  std::shared_ptr<UiSlot> hovered = nullptr;
  bool over_window = false;

  for (auto& [_, ui] : ui_controllers) {
    over_window = over_window || ui.contains(mouse_pos);
    if (auto slot = ui.slotAt(mouse_pos)) {
      hovered_ui = &ui;
      hovered = slot;
    }
  }

  // Click outside the windows throws held items, right click throws one
  if (!over_window && held.getItem() &&
      (mouse.pressed[MOUSE_LEFT] || mouse.pressed[MOUSE_RIGHT])) {
    dropFrom(held, mouse.pressed[MOUSE_LEFT] ? held.getQuantity() : 1);
    return true;
  }

  if (!hovered) {
    return false;
  }

  const bool input = hovered->getType() == SlotType::Input;
  auto& stack = *hovered->getStack();

  // Shift click moves the stack across, or crafts all
  const bool shift = asw::input::get_key(asw::input::Key::LShift) ||
                     asw::input::get_key(asw::input::Key::RShift);
  if (shift && mouse.pressed[MOUSE_LEFT]) {
    return quickMove(*hovered_ui, hovered->getType(), stack, state);
  }

  // Number keys swap the slot with that hotbar slot
  constexpr std::array<asw::input::Key, HOTBAR_SLOTS> hotbar_keys = {
      asw::input::Key::Num1, asw::input::Key::Num2, asw::input::Key::Num3,
      asw::input::Key::Num4, asw::input::Key::Num5, asw::input::Key::Num6,
      asw::input::Key::Num7, asw::input::Key::Num8};

  for (int i = 0; i < HOTBAR_SLOTS && input; i++) {
    if (asw::input::get_key_down(hotbar_keys[i])) {
      swapStacks(stack, *bag().getStack(i));
      SoundManager::play("pickup");
    }
  }

  // F drops one from the slot, with Ctrl the whole stack
  if (input && stack.getItem() &&
      asw::input::get_key_down(asw::input::Key::F)) {
    const bool all = asw::input::get_key(asw::input::Key::LCtrl) ||
                     asw::input::get_key(asw::input::Key::RCtrl);
    dropFrom(stack, all ? stack.getQuantity() : 1);
  }

  // Double click gathers every stack of the held item
  if (input && mouse.pressed[MOUSE_LEFT]) {
    const Uint64 now = SDL_GetTicks();
    const bool twice = last_click_stack == &stack &&
                       now - last_click_ms <= DOUBLE_CLICK_MS &&
                       held.getItem();
    last_click_ms = now;
    last_click_stack = &stack;

    if (twice) {
      last_click_stack = nullptr;
      collectAll(held.getItem()->getType().getId());
      return true;
    }
  }

  return false;
}

UiController* Hud::window(const std::string& name) {
  auto found = ui_controllers.find(name);
  return found == ui_controllers.end() ? nullptr : &found->second;
}

bool Hud::quickMove(UiController& from,
                    SlotType type,
                    ItemStack& stack,
                    GameState& state) {
  // Craft as many as the inputs allow, straight into the bag
  if (type == SlotType::Output) {
    if (from.craftAll(bag(), state) > 0) {
      SoundManager::play("shovel");
    } else {
      SoundManager::play("error");
    }
    return true;
  }

  if (type != SlotType::Input || !stack.getItem()) {
    return false;
  }

  bool moved = false;

  if (from.getName() == "inventory") {
    if (auto* station = openStation()) {
      // Bag to the open crafting, furnace or kiln window
      moved = moveStack(stack, station->stacksOfType(SlotType::Input));
    } else {
      // Hotbar to the bag rows, and back
      const auto slots = from.stacksOfType(SlotType::Input);
      const auto index = std::ranges::find_if(slots, [&](auto const& s) {
                           return s.get() == &stack;
                         }) -
                         slots.begin();
      const bool in_hotbar = index < HOTBAR_SLOTS;
      const std::vector<std::shared_ptr<ItemStack>> targets(
          in_hotbar ? slots.begin() + HOTBAR_SLOTS : slots.begin(),
          in_hotbar ? slots.end() : slots.begin() + HOTBAR_SLOTS);
      moved = moveStack(stack, targets);
    }
  } else {
    // Station back to the bag
    moved = bag().addItem(stack.getItem(), stack.getQuantity());
    if (moved) {
      stack.clear();
    }
  }

  SoundManager::play(moved ? "pickup" : "error");
  return true;
}

void Hud::collectAll(const std::string& id) {
  auto& held = UiController::mouseStack();

  for (auto& [_, ui] : ui_controllers) {
    for (auto const& stack : ui.stacksOfType(SlotType::Input)) {
      if (stack->getItem() && stack->getItem()->getType().getId() == id) {
        held.add(stack->getQuantity());
        stack->clear();
      }
    }
  }

  SoundManager::play("pickup");
}

void Hud::dropFrom(ItemStack& stack, int count) {
  if (!drop || !stack.getItem() || count <= 0) {
    return;
  }

  drop(*stack.getItem(), count);
  stack.remove(count);
}
