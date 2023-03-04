#ifndef SRC_INVENTORY_H_
#define SRC_INVENTORY_H_

#include <memory>
#include <vector>

#include "ItemStack.h"

class Inventory {
 public:
  bool addItem(std::shared_ptr<Item> item, int quantity);
  bool removeItem(int index);
  void addSpace();

  std::shared_ptr<ItemStack> getStack(int index);
  std::shared_ptr<ItemStack> getFirstItem();

  std::shared_ptr<ItemStack> findStack(std::shared_ptr<Item> item) const;
  int findFirstEmpty();

  int getSize() const;

  void empty();

 private:
  std::vector<std::shared_ptr<ItemStack>> contents;
};

#endif  // SRC_INVENTORY_H_
