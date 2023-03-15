#ifndef SRC_UI_UI_ELEMENT_H_
#define SRC_UI_UI_ELEMENT_H_

#include <asw/asw.h>

#include "../utility/Vec2.h"

class UiElement {
 public:
  explicit UiElement(Vec2<int> pos);
  explicit UiElement(Vec2<int> pos, Vec2<int> size);
  virtual ~UiElement() = default;

  virtual void draw(Vec2<int> parent_pos) = 0;

  Vec2<int> getPosition() const;
  Vec2<int> getSize() const;

 protected:
  static asw::Font font;

 private:
  Vec2<int> pos;
  Vec2<int> size;
};

#endif  // SRC_UI_UI_ELEMENT_H_
