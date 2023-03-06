#ifndef SRC_ITEM_H_
#define SRC_ITEM_H_

#include <string>

#include "manager/TileType.h"

const int MAX_ITEM_META = 255;

class Item {
 public:
  // Ctor and Dtor
  Item(char newID, unsigned char meta = 0);

  // Draw to screen
  void draw(int x, int y) const;

  // Get value of item
  unsigned char getValue() const;

  // Modify ID
  unsigned char getID() const;

  // Get the name of item
  std::string getName() const;

  // Access and set meta data byte
  void setMeta(unsigned char meta) { this->meta = meta; }
  void changeMeta(unsigned char amt) { this->meta += amt; }
  unsigned char getMeta() const { return this->meta; }

 private:
  // Metadata info
  unsigned char meta = 0;

  // Pointer to item type
  TileType& item_pointer;
};

#endif  // SRC_ITEM_H_
