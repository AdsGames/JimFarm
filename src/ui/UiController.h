#ifndef SRC_UI_UI_CONTROLLER_H_
#define SRC_UI_UI_CONTROLLER_H_

#include <memory>
#include <vector>

#include "../Inventory.h"
#include "../World.h"
#include "../utility/Vec2.h"
#include "UiElement.h"

class UiController {
 public:
  explicit UiController(Vec2<int> size);

  void draw();

  void update();

  void addElement(std::shared_ptr<UiElement> element);

  std::shared_ptr<Inventory> getInventory() const;

 private:
  std::shared_ptr<UiElement> elementAt(Vec2<int> pos) const;

  std::shared_ptr<Inventory> inv;

  Vec2<int> size;
  Vec2<int> position;

  std::vector<std::shared_ptr<UiElement>> elements;

  static std::shared_ptr<ItemStack> mouse_item;

  int currently_bound{0};
};

#endif  // SRC_UI_UI_CONTROLLER_H_
