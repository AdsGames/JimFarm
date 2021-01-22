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

void UI_Controller::AddElement(UI_Element* element) {
  if (element) {
    elements.push_back(element);

    // Is it a slot?
    UI_Slot* s = dynamic_cast<UI_Slot*>(element);
    if (s) {
      inv->addSpace();
      s->BindStack(inv->getStack(currently_bound));
      currently_bound++;
    }
  }
}

Inventory* UI_Controller::GetInventory() {
  return inv;
}

void UI_Controller::Draw(BITMAP* buffer) {
  // Caulculate x and y
  x = (buffer->w - width) / 2;
  y = (buffer->h - height) / 2;

  // Background
  rectfill(buffer, x, y, x + width, y + height, makecol(128, 128, 128));
  rect(buffer, x, y, x + width, y + height, makecol(64, 64, 64));

  // Draw elements
  for (unsigned int i = 0; i < elements.size(); i++) {
    elements.at(i)->Draw(buffer, x, y);
  }

  // Cursor
  rectfill(buffer, mouse_x * ((float)buffer->w / SCREEN_W),
           mouse_y * ((float)buffer->h / SCREEN_H),
           mouse_x * ((float)buffer->w / SCREEN_W) + 2,
           mouse_y * ((float)buffer->h / SCREEN_H) + 2, makecol(255, 255, 255));

  // Item, if holding
  if (mouse_item && mouse_item->GetItem()) {
    mouse_item->Draw(mouse_x * ((float)buffer->w / SCREEN_W),
                     mouse_y * ((float)buffer->h / SCREEN_H), buffer);
  }
}

void UI_Controller::Update(World* wrld) {
  // Select item
  if (MouseListener::mouse_pressed & 1 || MouseListener::mouse_pressed & 2) {
    int trans_x =
        mouse_x * ((wrld->VIEWPORT_WIDTH / wrld->VIEWPORT_ZOOM) / SCREEN_W);
    int trans_y =
        mouse_y * ((wrld->VIEWPORT_HEIGHT / wrld->VIEWPORT_ZOOM) / SCREEN_H);

    // Element at position
    UI_Element* elem = ElementAt(trans_x, trans_y);

    // Check if move
    if (elem != nullptr) {
      // Cast to slot
      UI_Slot* slt = dynamic_cast<UI_Slot*>(elem);

      // Ensure that it is slot
      if (slt != nullptr) {
        if (MouseListener::mouse_pressed & 1) {
          if (!(mouse_item->GetItem()) && slt->GetStack()->GetItem()) {
            mouse_item->SetItem(slt->GetStack()->GetItem(),
                                slt->GetStack()->GetQuantity());
            slt->GetStack()->Clear();
          } else if (mouse_item->GetItem() && !(slt->GetStack()->GetItem())) {
            slt->GetStack()->SetItem(mouse_item->GetItem(),
                                     mouse_item->GetQuantity());
            mouse_item->Clear();
          } else if (mouse_item->GetItem() &&
                     slt->GetStack()->GetItem()->getID() ==
                         mouse_item->GetItem()->getID()) {
            slt->GetStack()->Add(mouse_item->GetQuantity());
            mouse_item->Clear();
          }
        } else if (MouseListener::mouse_pressed & 2) {
          if (!(mouse_item->GetItem()) && slt->GetStack()->GetItem()) {
            int mouse_qty = ceil((float)slt->GetStack()->GetQuantity() / 2.0f);
            mouse_item->SetItem(slt->GetStack()->GetItem(), mouse_qty);
            slt->GetStack()->Remove(mouse_qty);
          } else if (mouse_item->GetItem() && !(slt->GetStack()->GetItem())) {
            slt->GetStack()->SetItem(mouse_item->GetItem(), 1);
            mouse_item->Remove(1);
          } else if (mouse_item->GetItem() &&
                     slt->GetStack()->GetItem()->getID() ==
                         mouse_item->GetItem()->getID()) {
            slt->GetStack()->Add(1);
            mouse_item->Remove(1);
          }
        }
      }
    }
  }
}

UI_Element* UI_Controller::ElementAt(int x, int y) {
  int trans_x = x - this->x;
  int trans_y = y - this->y;

  for (unsigned int i = 0; i < elements.size(); i++) {
    UI_Element* elem = elements.at(i);
    if (elem->GetX() < trans_x && elem->GetX() + elem->GetWidth() > trans_x &&
        elem->GetY() < trans_y && elem->GetY() + elem->GetHeight() > trans_y) {
      return elem;
    }
  }

  return nullptr;
}
