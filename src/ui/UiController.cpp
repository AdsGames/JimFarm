#include "UiController.h"
#include "UiLabel.h"
#include "UiSlot.h"

#include "../utility/Tools.h"

#include <math.h>

std::shared_ptr<ItemStack> UiController::mouse_item = nullptr;

UiController::UiController(int width, int height)
    : width(width), height(height) {
  // Create inventory
  this->inv = std::make_shared<Inventory>();

  // Make default mouse stack
  if (!mouse_item) {
    mouse_item = std::make_shared<ItemStack>();
  }
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
  auto screenSize = asw::display::getLogicalSize();

  // Caulculate x and y
  x = (screenSize.x - width) / 2;
  y = (screenSize.y - height) / 2;

  // Background
  asw::draw::rectFill(x, y, width, height, asw::util::makeColor(128, 128, 128));
  asw::draw::rect(x, y, width, height, asw::util::makeColor(64, 64, 64));

  // Draw elements
  for (auto const& element : elements) {
    element->draw(x, y);
  }

  // Cursor
  asw::draw::rectFill(asw::input::mouse.x, asw::input::mouse.y, 2, 2,
                      asw::util::makeColor(255, 255, 255));

  // Item, if holding
  if (mouse_item && mouse_item->getItem()) {
    mouse_item->draw(asw::input::mouse.x, asw::input::mouse.y);
  }
}

void UiController::update() {
  if (asw::input::mouse.pressed[1] || asw::input::mouse.pressed[3]) {
    int trans_x = asw::input::mouse.x;
    int trans_y = asw::input::mouse.y;

    // Element at position
    auto elem = elementAt(trans_x, trans_y);

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
      if (!item && stack->getItem()) {
        mouse_item->setItem(stack->getItem(), stack->getQuantity());
        stack->clear();
      } else if (item && !(stack->getItem())) {
        stack->setItem(item, mouse_item->getQuantity());
        mouse_item->clear();
      } else if (item && stack->getItem()->getType().getId() ==
                             item->getType().getId()) {
        stack->add(mouse_item->getQuantity());
        mouse_item->clear();
      }
    } else if (asw::input::mouse.pressed[3]) {
      if (!item && stack->getItem()) {
        auto mouse_qty = static_cast<int>(ceil(stack->getQuantity() / 2.0));
        mouse_item->setItem(stack->getItem(), mouse_qty);
        stack->remove(mouse_qty);
      } else if (item && !(stack->getItem())) {
        stack->setItem(item, 1);
        mouse_item->remove(1);
      } else if (item && stack->getItem()->getType().getId() ==
                             item->getType().getId()) {
        stack->add(1);
        mouse_item->remove(1);
      }
    }
  }
}

std::shared_ptr<UiElement> UiController::elementAt(int at_x, int at_y) const {
  int trans_x = at_x - this->x;
  int trans_y = at_y - this->y;

  for (auto const& element : elements) {
    if (element->getX() < trans_x &&
        element->getX() + element->getWidth() > trans_x &&
        element->getY() < trans_y &&
        element->getY() + element->getHeight() > trans_y) {
      return element;
    }
  }

  return nullptr;
}
