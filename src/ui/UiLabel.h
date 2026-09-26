#ifndef SRC_UI_UI_LABEL_H_
#define SRC_UI_UI_LABEL_H_

#include <asw/asw.h>
#include <string>

#include "UiElement.h"

class UiLabel : public UiElement {
 public:
  UiLabel(const asw::Vec2i& pos,
          const std::string& text,
          bool show_station = false);

  void draw(const asw::Vec2i& parent_pos, const GameState& state) override;
  std::string text;

  // Show the crafting station in use instead of the text
  bool show_station{false};
};

#endif  // SRC_UI_UI_LABEL_H_
