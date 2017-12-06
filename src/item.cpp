#include "item.h"

#include "tile_type_manager.h"

// Ctor for item
item::item( char newID){
  this -> item_pointer = tile_type_manager::getItemByID( newID);
  this -> meta = 0;
}

// Draw item to screen
void item::draw( int newX, int newY, BITMAP *tempBuffer){
  item_pointer -> draw( newX, newY, tempBuffer, getMeta());
}
