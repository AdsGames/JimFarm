#ifndef SRC_ITEMSTACK_H_
#define SRC_ITEMSTACK_H_

#include <asw/asw.h>
#include <memory>
#include <vector>

#include "Item.h"

class ItemStack {
 public:
  ItemStack() = default;
  ItemStack(std::shared_ptr<Item> item, int quantity);

  void setItem(std::shared_ptr<Item> item, int quantity);

  std::shared_ptr<Item> getItem() const;
  int getQuantity() const;

  void remove(int quantity);
  void add(int quantity);
  void clear();

  void draw(int x, int y) const;

 private:
  std::shared_ptr<Item> item = nullptr;
  int quantity = 0;
};

#endif  // SRC_ITEMSTACK_H_
