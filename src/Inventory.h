#ifndef SRC_INVENTORY_H_
#define SRC_INVENTORY_H_

#include <vector>

#include "ItemStack.h"

class Inventory {
 public:
  Inventory();

  virtual ~Inventory();

  bool addItem(Item* item, int quantity);
  bool removeItem(int index);
  void addSpace();

  ItemStack* getStack(int index);
  ItemStack* getFirstItem();

  ItemStack* findStack(Item* item);
  int findFirstEmpty();

  int getSize();

  void empty();

 private:
  std::vector<ItemStack*> contents;
};

#endif  // SRC_INVENTORY_H_
