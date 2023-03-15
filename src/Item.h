#ifndef SRC_ITEM_H_
#define SRC_ITEM_H_

#include <string>

#include "manager/TileType.h"
#include "utility/Vec2.h"

class Item {
 public:
  // Ctor and Dtor
  Item(const std::string& id, unsigned char meta = 0);

  // Draw to screen
  void draw(Vec2<int> position) const;

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
