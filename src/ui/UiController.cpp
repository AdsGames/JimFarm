#include "UiController.h"

#include <cmath>
#include <format>

#include "../GameState.h"
#include "../manager/InterfaceTypeManager.h"
#include "../manager/ItemTypeManager.h"
#include "../manager/RecipeManager.h"
#include "../manager/SoundManager.h"
#include "../utility/Tools.h"
#include "UiLabel.h"
#include "UiScale.h"
#include "UiSlot.h"
#include "Tooltip.h"

std::shared_ptr<ItemStack> UiController::mouse_item = nullptr;

UiController::UiController(const std::string& name, const asw::Vec2i& size)
    : name(name), size(size) {
  // Create inventory
  this->inv = std::make_shared<Inventory>();

  // Make default mouse stack
  if (!mouse_item) {
    mouse_item = std::make_shared<ItemStack>();
  }

  // Caulculate initial x and y
  position = (getUiSize() - size) / 2;
}

void UiController::addElement(std::shared_ptr<UiElement> element) {
  if (element) {
    elements.push_back(element);

    // Is it a slot?
    auto s = std::dynamic_pointer_cast<UiSlot>(element);
    if (s) {
      inv->addSpace();
      s->bindStack(inv->getStack(currently_bound));
      currently_bound++;
    }
  }
}

std::shared_ptr<Inventory> UiController::getInventory() const {
  return inv;
}

void UiController::draw(const GameState& state) {
  const auto mouse_pos = getUiMouse();

  // Drag box
  asw::draw::rect_fill(asw::Quadf(position.x, position.y - DRAG_BOX_HEIGHT,
                                  size.x, DRAG_BOX_HEIGHT),
                       asw::Color(64, 64, 64));

  // Background
  asw::draw::rect_fill(asw::Quadf(position.x, position.y, size.x, size.y),
                       asw::Color(128, 128, 128));
  asw::draw::rect(asw::Quadf(position.x, position.y, size.x, size.y),
                  asw::Color(64, 64, 64));

  // Draw elements
  for (auto const& element : elements) {
    element->draw(position, state);

    // Recipe needs a better station, grey it out
    auto slot = std::dynamic_pointer_cast<UiSlot>(element);
    if (output_locked && slot && slot->getType() == SlotType::Output) {
      const auto at = position + slot->getPosition();
      asw::display::set_blend_mode(asw::BlendMode::Blend);
      asw::draw::rect_fill(asw::Quadf(at.x, at.y, SLOT_SIZE, SLOT_SIZE),
                           asw::Color(120, 30, 30, 150));
    }
  }

  // Tooltip for the slot under the cursor
  if (!isHoldingItem()) {
    auto slot = std::dynamic_pointer_cast<UiSlot>(elementAt(mouse_pos));
    if (slot && slot->getStack() && slot->getStack()->getItem()) {
      const bool store_stock = slot->getType() == SlotType::Buy;
      Tooltip::setItem(*slot->getStack()->getItem(),
                       store_stock ? ItemTypeManager::getInfo(slot->getItemId())
                                         .price
                                   : 0);
    }
  }

  // Cursor
  asw::draw::rect_fill(asw::Quadf(mouse_pos.x, mouse_pos.y, 2, 2),
                       asw::Color(255, 255, 255));

  // Item, if holding
  if (mouse_item && mouse_item->getItem()) {
    mouse_item->draw(mouse_pos);
  }
}

void UiController::update(GameState& state) {
  const auto& mouse = asw::input::get_mouse();
  const auto mouse_pos = getUiMouse();

  updateRecipeOutput(state);

  if (mouse.pressed[1] || mouse.down[3]) {
    // Element at position
    auto elem = elementAt(mouse_pos);
    // Check if move
    if (elem == nullptr) {
      return;
    }

    // Cast to slot
    auto slt = std::dynamic_pointer_cast<UiSlot>(elem);

    // Ensure that it is slot
    if (slt == nullptr) {
      return;
    }

    auto item = mouse_item->getItem();
    auto stack = slt->getStack();
    const auto type = slt->getType();

    // Special slots only react to a fresh click
    if (type != SlotType::Input) {
      if (!mouse.pressed[1] && !mouse.pressed[3]) {
        return;
      }

      switch (type) {
        case SlotType::Buy:
          clickBuy(*slt, state);
          break;
        case SlotType::Sell:
          clickSell(mouse.pressed[1], state);
          break;
        case SlotType::Order:
          clickOrder(state);
          break;
        case SlotType::Output:
          if (mouse.pressed[1]) {
            clickOutput(state);
          }
          break;
        case SlotType::Input:
          break;
      }
      return;
    }

    if (mouse.pressed[1]) {
      // Pick up item
      if (!item && stack->getItem()) {
        mouse_item->setItem(stack->getItem(), stack->getQuantity());
        stack->clear();
      }
      // Place item
      else if (item && !stack->getItem()) {
        stack->setItem(item, mouse_item->getQuantity());
        mouse_item->clear();
      }
      // Add to stack
      else if (item && stack->getItem() &&
               stack->getItem()->getType().getId() == item->getType().getId()) {
        stack->add(mouse_item->getQuantity());
        mouse_item->clear();
      }
    } else if (mouse.pressed[3]) {
      // Split stack
      if (!item && stack->getItem() && stack->getQuantity() > 1) {
        auto mouse_qty = static_cast<int>(ceil(stack->getQuantity() / 2.0));
        mouse_item->setItem(stack->getItem(), mouse_qty);
        stack->remove(mouse_qty);
      }
      // Stack one
      else if (item && stack->getItem() &&
               stack->getItem()->getType().getId() == item->getType().getId()) {
        stack->add(1);
        mouse_item->remove(1);
      }
    } else if (mouse.down[3]) {
      // Remove one
      if (item && !stack->getItem()) {
        stack->setItem(item, 1);
        mouse_item->remove(1);
      }
    }
  }

  // Drag window
  if (mouse.down[1]) {
    // Start drag condition
    if (mouse_pos.x > position.x &&
        mouse_pos.x < position.x + size.x &&
        mouse_pos.y > position.y - DRAG_BOX_HEIGHT &&
        mouse_pos.y < position.y) {
      dragging = true;
    }
  } else {
    dragging = false;
  }

  // Update position
  if (dragging) {
    position.x =
        std::max(std::min(mouse_pos.x - size.x / 2,
                          getUiSize().x - size.x),
                 0);
    position.y = std::max(
        std::min(mouse_pos.y + DRAG_BOX_HEIGHT / 2,
                 getUiSize().y - size.y),
        0);
  }
}

std::shared_ptr<UiElement> UiController::elementAt(
    const asw::Vec2i& at_pos) const {
  int trans_x = at_pos.x - this->position.x;
  int trans_y = at_pos.y - this->position.y;

  for (auto const& element : elements) {
    auto el_pos = element->getPosition();
    auto el_size = element->getSize();

    if (el_pos.x < trans_x && el_pos.x + el_size.x > trans_x &&
        el_pos.y < trans_y && el_pos.y + el_size.y > trans_y) {
      return element;
    }
  }

  return nullptr;
}

std::string& UiController::getName() {
  return name;
}

std::vector<std::shared_ptr<ItemStack>> UiController::stacksOfType(
    SlotType type) const {
  std::vector<std::shared_ptr<ItemStack>> stacks;
  for (auto const& element : elements) {
    auto slot = std::dynamic_pointer_cast<UiSlot>(element);
    if (slot && slot->getType() == type) {
      stacks.push_back(slot->getStack());
    }
  }
  return stacks;
}

void UiController::updateRecipeOutput(const GameState& state) {
  auto outputs = stacksOfType(SlotType::Output);
  if (outputs.empty()) {
    return;
  }

  auto output = outputs.front();
  const auto inputs = stacksOfType(SlotType::Input);
  const auto* recipe = RecipeManager::match(name, inputs, state.crafting_tier);

  // Show what a better station would make, locked
  output_locked = false;
  if (!recipe) {
    recipe = RecipeManager::needsTier(name, inputs, state.crafting_tier);
    output_locked = recipe != nullptr;
  }

  if (!recipe) {
    output->clear();
    return;
  }

  // Preview what the inputs make
  if (!output->getItem() ||
      output->getItem()->getType().getId() != recipe->output ||
      output->getQuantity() != recipe->count) {
    output->setItem(std::make_shared<Item>(recipe->output), recipe->count);
  }
}

bool UiController::clickOutput(GameState& state) {
  const auto inputs = stacksOfType(SlotType::Input);
  const auto* recipe = RecipeManager::match(name, inputs, state.crafting_tier);
  if (!recipe) {
    if (const auto* locked =
            RecipeManager::needsTier(name, inputs, state.crafting_tier)) {
      state.notify("Needs a " + RecipeManager::stationName(*locked));
      SoundManager::play("error");
    }
    return false;
  }

  auto item = mouse_item->getItem();
  if (!item) {
    mouse_item->setItem(std::make_shared<Item>(recipe->output), recipe->count);
  } else if (item->getType().getId() == recipe->output) {
    mouse_item->add(recipe->count);
  } else {
    return false;
  }

  RecipeManager::consume(*recipe, inputs);
  SoundManager::play("shovel");
  updateRecipeOutput(state);
  return true;
}

bool UiController::clickBuy(const UiSlot& slot, GameState& state) {
  const auto& id = slot.getItemId();
  if (id.empty()) {
    return false;
  }

  const int price = ItemTypeManager::getInfo(id).price;
  const auto name = ItemTypeManager::getItem(id).getName();

  if (!state.spend(price)) {
    state.notify("Not enough coins for " + name);
    SoundManager::play("error");
    return false;
  }

  auto& inventory =
      *InterfaceTypeManager::getInterfaceByName("inventory").getInventory();

  if (!inventory.addItem(std::make_shared<Item>(id), 1)) {
    state.refund(price);
    state.notify("Your bag is full");
    SoundManager::play("error");
    return false;
  }

  state.notify(std::format("Bought {} for ${}", name, price));
  SoundManager::play("buy");
  return true;
}

bool UiController::clickSell(bool sell_all, GameState& state) {
  auto item = mouse_item->getItem();
  if (!item) {
    state.notify("Drop items here to sell them");
    return false;
  }

  const int value = ItemTypeManager::getInfo(item->getType().getId()).value;
  if (value <= 0) {
    state.notify("The store will not buy that");
    SoundManager::play("error");
    return false;
  }

  const int quantity = sell_all ? mouse_item->getQuantity() : 1;
  state.earn(value * quantity);
  state.items_sold += quantity;
  state.notify(std::format("Sold {} {} for ${}", quantity,
                           item->getType().getName(), value * quantity));
  mouse_item->remove(quantity);
  SoundManager::play("sell");
  return true;
}

bool UiController::clickOrder(GameState& state) {
  auto& order = state.order;
  auto item = mouse_item->getItem();

  if (!order.active) {
    state.notify("No order today, come back tomorrow");
    return false;
  }

  const auto wanted = ItemTypeManager::getItem(order.item_id).getName();

  if (!item || item->getType().getId() != order.item_id) {
    state.notify(std::format("Order: {} {} for ${}, due day {}",
                             order.quantity - order.delivered, wanted,
                             order.reward, order.deadline));
    return false;
  }

  const int given =
      std::min(mouse_item->getQuantity(), order.quantity - order.delivered);
  mouse_item->remove(given);
  order.delivered += given;
  SoundManager::play("sell");

  if (order.delivered >= order.quantity) {
    order.active = false;
    state.earn(order.reward);
    state.notify(std::format("Order complete! +${}", order.reward));
  } else {
    state.notify(std::format("Delivered {} {}, {} to go", given, wanted,
                             order.quantity - order.delivered));
  }

  return true;
}

bool UiController::isHoldingItem() {
  return mouse_item && mouse_item->getItem();
}

void UiController::returnMouseItem(Inventory& inventory) {
  if (!mouse_item || !mouse_item->getItem()) {
    return;
  }

  if (inventory.addItem(mouse_item->getItem(), mouse_item->getQuantity())) {
    mouse_item->clear();
  }
}
