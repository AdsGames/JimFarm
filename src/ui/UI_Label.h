#ifndef UI_LABEL_H
#define UI_LABEL_H

#include <allegro.h>
#include <string>

#include "UI_Element.h"

class UI_Label : public UI_Element {
 public:
  UI_Label(int x, int y, std::string text);
  virtual ~UI_Label();

  virtual void Draw(BITMAP* buffer, int parent_x, int parent_y) override;
  std::string text;

 private:
};

#endif  // UI_LABEL_H
