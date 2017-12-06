#include "item.h"

#include "tile_type_manager.h"

// ITEM
item::item( char newID){
  this -> item_pointer = tile_type_manager::getItemByID( newID);
  this -> meta = 0;
}

item::~item(){
  //dtor
}

void item::draw( int newX, int newY, BITMAP *tempBuffer){
  item_pointer -> draw( newX, newY, tempBuffer, getMeta());
}

void item::setID( unsigned char newID){
  item_pointer = tile_type_manager::getTileByID( newID);
}
