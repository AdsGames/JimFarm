#include "Item.h"

#include "TileTypeManager.h"

// Ctor for item
Item::Item (char newID, unsigned char meta) {
  this -> item_pointer = TileTypeManager::getItemByID (newID);
  this -> meta = meta;
}

// Dtor
Item::~Item() {

}

// Draw item to screen
void Item::draw (int newX, int newY, BITMAP *tempBuffer) {
  item_pointer -> draw (newX, newY, tempBuffer, getMeta());
}
