#ifndef SRC_UI_UI_LABEL_H_
#define SRC_UI_UI_LABEL_H_

#include <allegro.h>
#include <string>

#include "UI_Element.h"

class UI_Label : public UI_Element {
 public:
  UI_Label(int x, int y, std::string text);
  virtual ~UI_Label();

  void draw(BITMAP* buffer, int parent_x, int parent_y) override;
  std::string text;

 private:
};

#endif  // SRC_UI_UI_LABEL_H_
