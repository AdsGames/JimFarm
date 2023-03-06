#include "UI_Controller.h"
#include "UI_Label.h"
#include "UI_Slot.h"

#include "../manager/item_defs.h"
#include "../utility/Tools.h"

#include <math.h>

std::shared_ptr<ItemStack> UI_Controller::mouse_item = nullptr;

UI_Controller::UI_Controller(int width, int height)
    : width(width), height(height) {
  // Create inventory
  this->inv = std::make_shared<Inventory>();

  // Make default mouse stack
  if (!mouse_item) {
    mouse_item = std::make_shared<ItemStack>();
  }
}

void UI_Controller::addElement(std::shared_ptr<UI_Element> element) {
  if (element) {
    elements.push_back(element);

    // Is it a slot?
    auto s = std::dynamic_pointer_cast<UI_Slot>(element);
    if (s) {
      inv->addSpace();
      s->bindStack(inv->getStack(currently_bound));
      currently_bound++;
    }
  }
}

std::shared_ptr<Inventory> UI_Controller::getInventory() const {
  return inv;
}

void UI_Controller::draw() {
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

void UI_Controller::update() {
  if (asw::input::mouse.pressed[1] || asw::input::mouse.pressed[3]) {
    int trans_x = asw::input::mouse.x;
    int trans_y = asw::input::mouse.y;

    // Element at position
    auto elem = elementAt(trans_x, trans_y);

    // Check if move
    if (elem != nullptr) {
      // Cast to slot
      auto slt = std::dynamic_pointer_cast<UI_Slot>(elem);

      // Ensure that it is slot
      if (slt != nullptr) {
        if (asw::input::mouse.pressed[1]) {
          if (!(mouse_item->getItem()) && slt->getStack()->getItem()) {
            mouse_item->setItem(slt->getStack()->getItem(),
                                slt->getStack()->getQuantity());
            slt->getStack()->clear();
          } else if (mouse_item->getItem() && !(slt->getStack()->getItem())) {
            slt->getStack()->setItem(mouse_item->getItem(),
                                     mouse_item->getQuantity());
            mouse_item->clear();
          } else if (mouse_item->getItem() &&
                     slt->getStack()->getItem()->getID() ==
                         mouse_item->getItem()->getID()) {
            slt->getStack()->add(mouse_item->getQuantity());
            mouse_item->clear();
          }
        } else if (asw::input::mouse.pressed[3]) {
          if (!(mouse_item->getItem()) && slt->getStack()->getItem()) {
            int mouse_qty = ceil((float)slt->getStack()->getQuantity() / 2.0f);
            mouse_item->setItem(slt->getStack()->getItem(), mouse_qty);
            slt->getStack()->remove(mouse_qty);
          } else if (mouse_item->getItem() && !(slt->getStack()->getItem())) {
            slt->getStack()->setItem(mouse_item->getItem(), 1);
            mouse_item->remove(1);
          } else if (mouse_item->getItem() &&
                     slt->getStack()->getItem()->getID() ==
                         mouse_item->getItem()->getID()) {
            slt->getStack()->add(1);
            mouse_item->remove(1);
          }
        }
      }
    }
  }
}

std::shared_ptr<UI_Element> UI_Controller::elementAt(int x, int y) {
  int trans_x = x - this->x;
  int trans_y = y - this->y;

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
