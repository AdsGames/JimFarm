#ifndef SRC_UI_UI_CONTROLLER_H_
#define SRC_UI_UI_CONTROLLER_H_

#include <memory>
#include <vector>

#include "../Inventory.h"
#include "../utility/Vec2.h"
#include "UiElement.h"

constexpr int DRAG_BOX_HEIGHT = 10;

class UiController {
 public:
  UiController(const std::string& name, Vec2<int> size);

  void draw();

  void update();

  void addElement(std::shared_ptr<UiElement> element);

  std::shared_ptr<Inventory> getInventory() const;

  std::string& getName();

 private:
  std::shared_ptr<UiElement> elementAt(Vec2<int> pos) const;

  std::shared_ptr<Inventory> inv{nullptr};

  std::string name{""};

  Vec2<int> size{0, 0};
  Vec2<int> position{0, 0};

  std::vector<std::shared_ptr<UiElement>> elements{};

  static std::shared_ptr<ItemStack> mouse_item;

  int currently_bound{0};

  bool dragging{false};
};

#endif  // SRC_UI_UI_CONTROLLER_H_
