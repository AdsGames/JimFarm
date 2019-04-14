#include "UI_Controller.h"
#include "UI_Label.h"
#include "UI_Slot.h"

#include "manager/item_defs.h"
#include "utility/tools.h"
#include "utility/MouseListener.h"

#include <iostream>

Item* UI_Controller::mouse_item = nullptr;

UI_Controller::UI_Controller(int size, int width, int height) {
  // Create inventory
  this -> inv = Inventory(size);

  // Set dimensions
  this -> width = width;
  this -> height = height;

  // Caulculate x and y
  this -> x = 0;
  this -> y = 0;

  // Load font
  ui_font = load_font_ex("fonts/pixelart_condensed.pcx");

  // Add labels
  AddElement(new UI_Label(5, 0, "Inventory", ui_font));

  // Add slots
  AddElement(new UI_Slot(5, 15, new Item(ITEM_HOE)));
  AddElement(new UI_Slot(25, 15, new Item(ITEM_SHOVEL)));
  AddElement(new UI_Slot(45, 15, new Item(ITEM_HAND)));
  AddElement(new UI_Slot(65, 15, new Item(ITEM_HAY)));
  AddElement(new UI_Slot(85, 15));
  AddElement(new UI_Slot(105, 15));
  AddElement(new UI_Slot(125, 15));
  AddElement(new UI_Slot(145, 15));
}

UI_Controller::~UI_Controller() {

}

void UI_Controller::AddElement(UI_Element *element) {
  if (element) {
    elements.push_back(element);
  }
}

void UI_Controller::Draw(BITMAP *buffer) {
  // Caulculate x and y
  x = (buffer -> w - width) / 2;
  y = (buffer -> h - height) / 2;

  // Background
  rectfill(buffer, x, y, x + width, y + height, makecol(128, 128, 128));
  rect(buffer, x, y, x + width, y + height, makecol(64, 64, 64));

  // Draw elements
  for (unsigned int i = 0; i < elements.size(); i++) {
    elements.at(i) -> Draw(buffer, x, y);
  }

  // Cursor
  rectfill(buffer, mouse_x * ((float)buffer -> w/SCREEN_W),
                   mouse_y * ((float)buffer -> h/SCREEN_H),
                   mouse_x * ((float)buffer -> w/SCREEN_W) + 2,
                   mouse_y * ((float)buffer -> h/SCREEN_H) + 2,
                   makecol(255, 255, 255));

  // Item, if holding
  if (mouse_item) {
    mouse_item -> draw(mouse_x * ((float)buffer -> w/SCREEN_W), mouse_y * ((float)buffer -> h/SCREEN_H), buffer);
  }
}

void UI_Controller::Update(World *wrld) {
  // Select item
  if (MouseListener::mouse_pressed & 1) {
    int trans_x = mouse_x * ((wrld -> VIEWPORT_WIDTH  / wrld -> VIEWPORT_ZOOM) / SCREEN_W);
    int trans_y = mouse_y * ((wrld -> VIEWPORT_HEIGHT / wrld -> VIEWPORT_ZOOM) / SCREEN_H);

    // Element at position
    UI_Element *elem = ElementAt(trans_x, trans_y);

    // Check if move
    if (elem != nullptr) {
      // Cast to slot
      UI_Slot *slt = dynamic_cast<UI_Slot*>(elem);

      // Ensure that it is slot
      if (slt != nullptr) {
        if (mouse_item == nullptr && slt -> GetItem() != nullptr) {
          mouse_item = slt -> GetItem();
          slt -> BindItem(nullptr);
        }
        else {
          slt -> BindItem(mouse_item);
          mouse_item = nullptr;
        }
      }
    }
  }
}

UI_Element* UI_Controller::ElementAt(int x, int y) {
  int trans_x = x - this -> x;
  int trans_y = y - this -> y;

  for (unsigned int i = 0; i < elements.size(); i++) {
    UI_Element *elem = elements.at(i);
    if (elem -> GetX() < trans_x && elem -> GetX() + elem -> GetWidth() > trans_x &&
        elem -> GetY() < trans_y && elem -> GetY() + elem -> GetHeight() > trans_y) {
      return elem;
    }
  }

  return nullptr;
}
