#include "tile_type.h"

// Init tile
tile_type::tile_type( std::string newName, unsigned char newType, BITMAP *defaultImage, unsigned char newAttribute, char newRandomness){
  // Set init variables
  type = newType;
  name = newName;

  image[0] = defaultImage;
  image[1] = defaultImage;

  randomness = newRandomness;

  attribute = newAttribute;
}

// Destroy tile
tile_type::~tile_type(){
  //dtor
}

// Set images from file
void tile_type::setImages( std::string path1, std::string path2){
  // Load image 1
  if( path1 != "NULL"){
    image[0] = load_bitmap( path1.c_str(), NULL);
  }

  // Load image 2
  if( path2 != "NULL"){
    image[1] = load_bitmap( path2.c_str(), NULL);
  }
  else{
    image[1] = image[0];
  }
}


// Draw tile
void tile_type::draw( int x, int y, bool newTick){

}
