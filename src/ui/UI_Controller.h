#ifndef SRC_UI_UI_CONTROLLER_H_
#define SRC_UI_UI_CONTROLLER_H_

#include <memory>
#include <vector>

#include "../Inventory.h"
#include "../World.h"
#include "UI_Element.h"

class UI_Controller {
 public:
  UI_Controller(int width, int height);

  void draw();

  void update();

  void addElement(std::shared_ptr<UI_Element> element);

  std::shared_ptr<Inventory> getInventory() const;

 private:
  std::shared_ptr<UI_Element> elementAt(int x, int y);

  bool is_open;
  std::shared_ptr<Inventory> inv;
  int width;
  int height;

  int x{0};
  int y{0};

  std::vector<std::shared_ptr<UI_Element>> elements;

  static std::shared_ptr<ItemStack> mouse_item;

  int currently_bound{0};
};

#endif  // SRC_UI_UI_CONTROLLER_H_
