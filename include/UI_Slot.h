#ifndef UI_SLOT_H
#define UI_SLOT_H

#define SLOT_SIZE 16

#include <allegro.h>

#include "UI_Element.h"
#include "Item.h"

class UI_Slot : public UI_Element {
  public:
    UI_Slot(int x, int y);
    UI_Slot(int x, int y, Item *item);
    virtual ~UI_Slot() {};

    void BindItem(Item *item);

    Item *GetItem();

    virtual void Draw(BITMAP *buffer, int parent_x, int parent_y) override;

  protected:

  private:
    Item *item;
};

#endif // UI_SLOT_H
