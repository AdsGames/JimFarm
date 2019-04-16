#ifndef ITEMSTACK_H
#define ITEMSTACK_H

#include "Item.h"
#include <vector>
#include <allegro.h>

class ItemStack {
  public:
    ItemStack();
    ItemStack(Item *item, int quantity);
    virtual ~ItemStack();

    void SetItem(Item *item, int quantity);
    Item *GetItem();
    int GetQuantity();

    void Remove(int quantity);
    void Add(int quantity);
    void Clear();

    void Draw(int x, int y, BITMAP *buffer);

  protected:

  private:
    Item *item;
    int quantity;
};

#endif // ITEMSTACK_H
