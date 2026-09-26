#ifndef SRC_UI_UI_LABEL_H_
#define SRC_UI_UI_LABEL_H_

#include <asw/asw.h>
#include <string>

#include "UiElement.h"

class UiLabel : public UiElement {
 public:
  UiLabel(const asw::Vec2i& pos, const std::string& text);

  void draw(const asw::Vec2i& parent_pos) override;
  std::string text;
};

#endif  // SRC_UI_UI_LABEL_H_
