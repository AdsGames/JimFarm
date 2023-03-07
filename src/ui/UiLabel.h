#ifndef SRC_UI_UI_LABEL_H_
#define SRC_UI_UI_LABEL_H_

#include <asw/asw.h>
#include <string>

#include "UiElement.h"

class UiLabel : public UiElement {
 public:
  UiLabel(int x, int y, const std::string& text);

  void draw(int parent_x, int parent_y) override;
  std::string text;
};

#endif  // SRC_UI_UI_LABEL_H_
