#ifndef INVENTORY_H
#define INVENTORY_H

#include <vector>

#include "ItemStack.h"

class Inventory {
  public:
    Inventory();

    virtual ~Inventory();

    bool addItem (Item *item, int quantity);
    bool removeItem (int index);
    void addSpace();

    ItemStack *getStack (int index);
    ItemStack *getFirstItem();

    ItemStack *findStack(Item *item);
    int findFirstEmpty();

    int getSize();

    void empty();

  private:
    std::vector<ItemStack*> contents;
};

#endif // INVENTORY_H
