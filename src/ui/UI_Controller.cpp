#include "UI_Controller.h"
#include "UI_Label.h"
#include "UI_Slot.h"

#include "../manager/item_defs.h"
#include "../utility/MouseListener.h"
#include "../utility/tools.h"

#include <math.h>

#include <iostream>

ItemStack* UI_Controller::mouse_item = nullptr;

UI_Controller::UI_Controller(int width, int height) {
  // Create inventory
  this->inv = new Inventory();

  // Set dimensions
  this->width = width;
  this->height = height;

  // Caulculate x and y
  this->x = 0;
  this->y = 0;

  this->currently_bound = 0;

  // Make default mouse stack
  if (!mouse_item)
    mouse_item = new ItemStack();
}

UI_Controller::~UI_Controller() {}

void UI_Controller::addElement(UI_Element* element) {
  if (element) {
    elements.push_back(element);

    // Is it a slot?
    UI_Slot* s = dynamic_cast<UI_Slot*>(element);
    if (s) {
      inv->addSpace();
      s->bindStack(inv->getStack(currently_bound));
      currently_bound++;
    }
  }
}

Inventory* UI_Controller::getInventory() {
  return inv;
}

void UI_Controller::draw(BITMAP* buffer) {
  // Caulculate x and y
  x = (buffer->w - width) / 2;
  y = (buffer->h - height) / 2;

  // Background
  rectfill(buffer, x, y, x + width, y + height, makecol(128, 128, 128));
  rect(buffer, x, y, x + width, y + height, makecol(64, 64, 64));

  // Draw elements
  for (auto const& element : elements) {
    element->draw(buffer, x, y);
  }

  // Cursor
  rectfill(buffer, mouse_x * ((float)buffer->w / SCREEN_W),
           mouse_y * ((float)buffer->h / SCREEN_H),
           mouse_x * ((float)buffer->w / SCREEN_W) + 2,
           mouse_y * ((float)buffer->h / SCREEN_H) + 2, makecol(255, 255, 255));

  // Item, if holding
  if (mouse_item && mouse_item->getItem()) {
    mouse_item->draw(mouse_x * ((float)buffer->w / SCREEN_W),
                     mouse_y * ((float)buffer->h / SCREEN_H), buffer);
  }
}

void UI_Controller::update(World* wrld) {
  // Select item
  if (MouseListener::mouse_pressed & 1 || MouseListener::mouse_pressed & 2) {
    int trans_x =
        mouse_x * ((wrld->VIEWPORT_WIDTH / wrld->VIEWPORT_ZOOM) / SCREEN_W);
    int trans_y =
        mouse_y * ((wrld->VIEWPORT_HEIGHT / wrld->VIEWPORT_ZOOM) / SCREEN_H);

    // Element at position
    UI_Element* elem = elementAt(trans_x, trans_y);

    // Check if move
    if (elem != nullptr) {
      // Cast to slot
      UI_Slot* slt = dynamic_cast<UI_Slot*>(elem);

      // Ensure that it is slot
      if (slt != nullptr) {
        if (MouseListener::mouse_pressed & 1) {
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
        } else if (MouseListener::mouse_pressed & 2) {
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

UI_Element* UI_Controller::elementAt(int x, int y) {
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
