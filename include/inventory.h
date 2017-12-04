#ifndef INVENTORY_H
#define INVENTORY_H

#include <vector>

#include "tile.h"
#include "item.h"

class inventory
{
  public:
    inventory();
    inventory( int max_items);

    virtual ~inventory();

    bool addItem( item *newItem, int index);
    bool removeItem( int index);

    item *getItem( int index);
    item *getFirstItem();

    int getSize();
    int getMaxSize();

    void emptyInv();

  protected:

  private:
    std::vector<item*> contents;
    int max_items;
};

#endif // INVENTORY_H
