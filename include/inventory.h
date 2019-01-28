#ifndef INVENTORY_H
#define INVENTORY_H

#include <vector>

#include "Tile.h"
#include "Item.h"

class Inventory {
  public:
    Inventory();
    Inventory (int max_items);

    virtual ~Inventory();

    bool addItem (Item *newItem, int index);
    bool removeItem (int index);

    Item *getItem (int index);
    Item *getFirstItem();

    int getSize();
    int getMaxSize();

    void emptyInv();

  private:
    std::vector<Item*> contents;
    int max_items;
};

#endif // INVENTORY_H
