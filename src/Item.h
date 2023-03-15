#ifndef SRC_ITEM_H_
#define SRC_ITEM_H_

#include <string>

#include "manager/TileType.h"

const int MAX_ITEM_META = 255;

class Item {
 public:
  // Ctor and Dtor
  Item(const std::string& id, unsigned char meta = 0);

  // Draw to screen
  void draw(int x, int y) const;

  // Modify ID
  const std::string& getId() const;

  // Get the name of item
  const std::string& getName() const;

  // Access and set meta data byte
  void setMeta(unsigned char meta);
  void changeMeta(unsigned char amt);
  unsigned char getMeta() const;

 private:
  // Metadata info
  unsigned char meta = 0;

  // Pointer to item type
  TileType& item_pointer;
};

#endif  // SRC_ITEM_H_
