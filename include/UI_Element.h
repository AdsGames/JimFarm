#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include <allegro.h>

class UI_Element {
  public:
    UI_Element(int x, int y);
    virtual ~UI_Element();

    virtual void Draw(BITMAP *buffer, int parent_x, int parent_y) = 0;

    int GetX();
    int GetY();

  protected:

  private:
    int x, y;
};

#endif // UI_ELEMENT_H
