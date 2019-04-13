#include "UI_Controller.h"
#include "UI_Label.h"
#include "UI_Slot.h"

#include "manager/item_defs.h"
#include "utility/tools.h"

UI_Controller::UI_Controller(int size, int width, int height) {
  // Create inventory
  this -> inv = Inventory(size);

  // Set dimensions
  this -> width = width;
  this -> height = height;

  // Caulculate x and y
  this -> x = 0;
  this -> y = 0;

  // Populate inventory
  inv.addItem(new Item(ITEM_HOE), 0);
  inv.addItem(new Item(ITEM_SHOVEL), 1);
  inv.addItem(new Item(ITEM_HAND), 2);
  inv.addItem(new Item(ITEM_HAY), 3);

  // Load font
  ui_font = load_font_ex("fonts/pixelart_condensed.pcx");

  // Add labels
  AddElement(new UI_Label(5, 0, "Inventory", ui_font));

  // Add slots
  AddElement(new UI_Slot(5, 15, inv.getItem(0)));
  AddElement(new UI_Slot(25, 15, inv.getItem(1)));
  AddElement(new UI_Slot(45, 15, inv.getItem(2)));
  AddElement(new UI_Slot(65, 15, inv.getItem(3)));
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
}

void UI_Controller::Update() {

}
