#include "inventory.h"

#include <iostream>

inventory::inventory(){
  this -> max_items = 0;
}

inventory::inventory( int max_items){
  this -> max_items = max_items;

  contents.reserve(max_items);

  for( int i = 0; i < max_items; i++)
    contents.push_back( NULL);
}

inventory::~inventory(){}

// Push item to contents (if it fits)
bool inventory::addItem( item *newItem, int index){
  if( index < getMaxSize() && newItem != NULL){
    contents[index] = newItem;
    return true;
  }
  return false;
}

bool inventory::removeItem( int index){
  if( getItem( index) != NULL){
    contents.erase( contents.begin() + index);
    return true;
  }
  return false;
}

// Gets item at index if exists
item* inventory::getItem( int index){
  if( getSize() > index)
    return contents[index];
  return NULL;
}

// Just returns first item
item* inventory::getFirstItem(){
  return getItem(0);
}

// Current item count
int inventory::getSize(){
  return contents.size();
}

// Just returns max size
int inventory::getMaxSize(){
  return max_items;
}

// Clear all contents
void inventory::emptyInv(){
  contents.clear();
}
