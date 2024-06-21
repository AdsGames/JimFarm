#include "UiController.h"

#include "../utility/Tools.h"
#include "UiLabel.h"
#include "UiSlot.h"

std::shared_ptr<ItemStack> UiController::mouse_item = nullptr;

UiController::UiController(const std::string& name, Vec2<int> size)
    : name(name), size(size) {
  // Create inventory
  this->inv = std::make_shared<Inventory>();

  // Make default mouse stack
  if (!mouse_item) {
    mouse_item = std::make_shared<ItemStack>();
  }

  // Caulculate initial x and y
  auto screenSize = asw::display::getLogicalSize();
  position = (Vec2<int>(screenSize.x, screenSize.y) - size) / 2;
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
  // Drag box
  asw::draw::rectFill(position.x, position.y - DRAG_BOX_HEIGHT, size.x,
                      DRAG_BOX_HEIGHT, asw::util::makeColor(64, 64, 64));

  // Background
  asw::draw::rectFill(position.x, position.y, size.x, size.y,
                      asw::util::makeColor(128, 128, 128));
  asw::draw::rect(position.x, position.y, size.x, size.y,
                  asw::util::makeColor(64, 64, 64));

  // Draw elements
  for (auto const& element : elements) {
    element->draw(position);
  }

  // Cursor
  asw::draw::rectFill(asw::input::mouse.x, asw::input::mouse.y, 2, 2,
                      asw::util::makeColor(255, 255, 255));

  // Item, if holding
  if (mouse_item && mouse_item->getItem()) {
    mouse_item->draw(Vec2<int>(asw::input::mouse.x, asw::input::mouse.y));
  }
}

void UiController::update() {
  if (asw::input::mouse.pressed[1] || asw::input::mouse.down[3]) {
    // Element at position
    auto elem = elementAt(Vec2<int>(asw::input::mouse.x, asw::input::mouse.y));

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

    if (asw::input::mouse.pressed[1]) {
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
    } else if (asw::input::mouse.pressed[3]) {
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
    } else if (asw::input::mouse.down[3]) {
      // Remove one
      if (item && !stack->getItem()) {
        stack->setItem(item, 1);
        mouse_item->remove(1);
      }
    }
  }

  // Drag window
  if (asw::input::mouse.down[1]) {
    // Start drag condition
    if (asw::input::mouse.x > position.x &&
        asw::input::mouse.x < position.x + size.x &&
        asw::input::mouse.y > position.y - DRAG_BOX_HEIGHT &&
        asw::input::mouse.y < position.y) {
      dragging = true;
    }
  } else {
    dragging = false;
  }

  // Update position
  if (dragging) {
    position.x = std::max(std::min(asw::input::mouse.x - size.x / 2,
                                   asw::display::getLogicalSize().x - size.x),
                          0);
    position.y = std::max(std::min(asw::input::mouse.y + DRAG_BOX_HEIGHT / 2,
                                   asw::display::getLogicalSize().y - size.y),
                          0);
  }
}

std::shared_ptr<UiElement> UiController::elementAt(Vec2<int> at_pos) const {
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
