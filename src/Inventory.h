#ifndef SRC_INVENTORY_H_
#define SRC_INVENTORY_H_

#include <memory>
#include <string>
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

  // Total quantity of an item across all stacks
  int count(const std::string& id) const;

  // Remove quantity of an item across stacks, false if there is not enough
  bool take(const std::string& id, int quantity);

 private:
  std::vector<std::shared_ptr<ItemStack>> contents;
};

#endif  // SRC_INVENTORY_H_
