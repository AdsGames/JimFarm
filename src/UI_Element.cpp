#include "UI_Element.h"

UI_Element::UI_Element(int x, int y) {
  this -> x = x;
  this -> y = y;
}

UI_Element::~UI_Element() {

}


int UI_Element::GetX() {
  return x;
}

int UI_Element::GetY() {
  return y;
}
