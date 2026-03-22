#ifndef SRC_ITEM_H_
#define SRC_ITEM_H_

#include <string>

#include "manager/TileType.h"

class Item {
 public:
  // Ctor and Dtor, negative meta uses the item's start meta
  explicit Item(const std::string& id, int meta = -1);

  // Draw to screen
  void draw(const asw::Vec2i& position) const;

  // Access and set meta data byte
  void setMeta(unsigned char meta);
  void changeMeta(unsigned char amt);
  unsigned char getMeta() const;

  const TileType& getType() const;

 private:
  // Metadata info
  unsigned char meta = 0;

  // Pointer to item type
  TileType& item_pointer;
};

#endif  // SRC_ITEM_H_
