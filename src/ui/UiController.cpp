#include "UiController.h"

#include <cmath>

#include "../utility/Tools.h"
#include "UiLabel.h"
#include "UiSlot.h"

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
  auto screenSize = asw::display::get_logical_size();
  position = (asw::Vec2i(screenSize.x, screenSize.y) - size) / 2;
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

void UiController::draw() {
  const auto mouse = asw::input::get_mouse();

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
    element->draw(position);
  }

  // Cursor
  asw::draw::rect_fill(asw::Quadf(mouse.position.x, mouse.position.y, 2, 2),
                       asw::Color(255, 255, 255));

  // Item, if holding
  if (mouse_item && mouse_item->getItem()) {
    mouse_item->draw(asw::Vec2i(mouse.position.x, mouse.position.y));
  }
}

void UiController::update() {
  const auto& mouse = asw::input::get_mouse();

  if (mouse.pressed[1] || mouse.down[3]) {
    // Element at position
    auto elem = elementAt(asw::Vec2i(mouse.position.x, mouse.position.y));
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

    if (mouse.pressed[1]) {
      // Pick up item
      if (!item && stack->getItem()) {
        mouse_item->setItem(stack->getItem(), stack->getQuantity());
        stack->clear();
      }
      // Place item
      else if (item && !stack->getItem() && slt->getType() == "input") {
        stack->setItem(item, mouse_item->getQuantity());
        mouse_item->clear();
      }
      // Add to stack
      else if (item && stack->getItem() &&
               stack->getItem()->getType().getId() == item->getType().getId() &&
               slt->getType() == "input") {
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
    if (mouse.position.x > position.x &&
        mouse.position.x < position.x + size.x &&
        mouse.position.y > position.y - DRAG_BOX_HEIGHT &&
        mouse.position.y < position.y) {
      dragging = true;
    }
  } else {
    dragging = false;
  }

  // Update position
  if (dragging) {
    position.x =
        std::max(std::min(static_cast<int>(mouse.position.x) - size.x / 2,
                          asw::display::get_logical_size().x - size.x),
                 0);
    position.y = std::max(
        std::min(static_cast<int>(mouse.position.y) + DRAG_BOX_HEIGHT / 2,
                 asw::display::get_logical_size().y - size.y),
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
