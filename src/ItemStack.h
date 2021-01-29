#ifndef ITEMSTACK_H
#define ITEMSTACK_H

#include <allegro.h>
#include <vector>
#include "Item.h"

class ItemStack {
 public:
  ItemStack();
  ItemStack(Item* item, int quantity);
  virtual ~ItemStack();

  void setItem(Item* item, int quantity);
  Item* getItem();
  int getQuantity();

  void remove(int quantity);
  void add(int quantity);
  void clear();

  void draw(int x, int y, BITMAP* buffer);

 protected:
 private:
  Item* item;
  int quantity;
};

#endif  // ITEMSTACK_H
