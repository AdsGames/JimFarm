#ifndef UI_CONTROLLER_H
#define UI_CONTROLLER_H

#include "../Inventory.h"
#include "../World.h"
#include "UI_Element.h"

class UI_Controller {
 public:
  UI_Controller(int width, int height);
  virtual ~UI_Controller();

  void draw(BITMAP* buffer);

  void update(World* wrld);

  void addElement(UI_Element* element);

  Inventory* getInventory();

 protected:
 private:
  UI_Element* elementAt(int x, int y);

  bool is_open;
  Inventory* inv;
  int width, height;

  int x, y;

  std::vector<UI_Element*> elements;

  static ItemStack* mouse_item;

  int currently_bound;
};

#endif  // UI_CONTROLLER_H
