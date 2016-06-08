#include "tile_type.h"

// Init tile
tile_type::tile_type( unsigned char newImageX, unsigned char newImageY, unsigned char newImageW,
               unsigned char newImageH, unsigned char newID, std::string newName, int newAttribute, unsigned char newValue, std::string newScriptTick){
  // Set init variables
  id = newID;
  name = newName;

  image_cord_x = newImageX;
  image_cord_y = newImageY;

  image_h = newImageH;
  image_w = newImageW;

  attribute = newAttribute;

  sprite_sheet = NULL;

  value = newValue;

  script = newScriptTick;
}

// Destroy tile
tile_type::~tile_type(){
  //dtor
}

// Draw tile
void tile_type::draw( int x, int y, BITMAP *tempBuffer, char offset){
  if( sprite_sheet != NULL)
    masked_blit( sprite_sheet, tempBuffer, (image_cord_x + offset) * 16, image_cord_y * 16, x, y - ((image_h * 16) - 16), (image_w * 16), (image_h * 16));
}
