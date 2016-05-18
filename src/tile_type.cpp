#include "tile_type.h"

// Init tile
tile_type::tile_type( unsigned char newImageX, unsigned char newImageY, unsigned char newImageW,
               unsigned char newImageH, unsigned char newID, std::string newName, int newAttribute){
  // Set init variables
  id = newID;
  name = newName;

  image_cord_x = newImageX;
  image_cord_y = newImageY;

  image_h = newImageH;
  image_w = newImageW;

  attribute = newAttribute;
}

// Init item
tile_type::tile_type( unsigned char newImageX, unsigned char newImageY, unsigned char newID, std::string newName){
  // Set init variables
  id = newID;
  name = newName;

  image_cord_x = newImageX;
  image_cord_y = newImageY;

  image_h = 1;
  image_w = 1;
  attribute = 0;
}

// Destroy tile
tile_type::~tile_type(){
  //dtor
}

// Draw tile
void tile_type::draw( int x, int y){

}
