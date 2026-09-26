#ifndef SRC_UI_UI_ELEMENT_H_
#define SRC_UI_UI_ELEMENT_H_

#include <asw/asw.h>

class UiElement {
 public:
  explicit UiElement(const asw::Vec2i& pos);
  explicit UiElement(const asw::Vec2i& pos, const asw::Vec2i& size);
  virtual ~UiElement() = default;

  virtual void draw(const asw::Vec2i& parent_pos) = 0;

  const asw::Vec2i& getPosition() const;
  const asw::Vec2i& getSize() const;

 protected:
  static asw::Font font;

 private:
  asw::Vec2i pos;
  asw::Vec2i size;
};

#endif  // SRC_UI_UI_ELEMENT_H_
