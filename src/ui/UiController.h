#ifndef SRC_UI_UI_CONTROLLER_H_
#define SRC_UI_UI_CONTROLLER_H_

#include <memory>
#include <vector>

#include "../Inventory.h"
#include "../World.h"
#include "UiElement.h"

class UiController {
 public:
  UiController(int width, int height);

  void draw();

  void update();

  void addElement(std::shared_ptr<UiElement> element);

  std::shared_ptr<Inventory> getInventory() const;

 private:
  std::shared_ptr<UiElement> elementAt(int x, int y) const;

  std::shared_ptr<Inventory> inv;

  int width{0};
  int height{0};

  int x{0};
  int y{0};

  std::vector<std::shared_ptr<UiElement>> elements;

  static std::shared_ptr<ItemStack> mouse_item;

  int currently_bound{0};
};

#endif  // SRC_UI_UI_CONTROLLER_H_
