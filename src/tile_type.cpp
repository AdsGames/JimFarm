#include "tile_type.h"

// Init tile
tile_type::tile_type( unsigned char newWidth, unsigned char newHeight, unsigned char newID,
                      std::string newName, int newAttribute, unsigned char newValue){
  // Set init variables
  id = newID;
  name = newName;

  width = newWidth;
  height = newHeight;

  attribute = newAttribute;
  value = newValue;

  sprite_sheet = NULL;
  image_type = "";
  sheet_width = 1;
  sheet_height = 1;
  image_w = 1;
  image_h = 1;
  image_cord_x = 0;
  image_cord_y = 0;
}

// Destroy tile
tile_type::~tile_type(){}

// Draw tile
void tile_type::draw( int x, int y, BITMAP *tempBuffer, char offset){
  if( image_type == "dynamic"){
    draw_sprite( tempBuffer, images[0], x, y - ((image_h * 16) - 16));
  }
  else{
    draw_sprite( tempBuffer, images[0], x, y - ((image_h * 16) - 16));
  }
}

// Give a sprite sheet to this tile
void tile_type::setSpriteSheet( BITMAP *newSpriteSheet){
  sprite_sheet = newSpriteSheet;
}

// Set special image stuff
void tile_type::setImageType( std::string newImageType, int newSheetWidth, int newSheetHeight, int newImageX, int newImageY, int newImageWidth, int newImageHeight){
  // Default, dynamic or animated
  image_type = newImageType;
  sheet_width = newSheetWidth;
  sheet_height = newSheetHeight;
  image_w = newImageWidth;
  image_h = newImageHeight;
  image_cord_x = newImageX;
  image_cord_y = newImageY;

  // Split up those images
  if( image_type == "dynamic"){
    for( int i = 0; i < sheet_width; i++){
      for( int t = 0; t < sheet_height; t++){
        images[(i * sheet_width) + t] = create_bitmap( image_w * 16, image_h * 16);
        blit( sprite_sheet, images[(i * sheet_width) + t], image_cord_x * 16, image_cord_y * 16, 0, 0, image_w * 16, image_h * 16);
      }
    }
  }
  else{
    images[0] = create_bitmap( image_w * 16, image_h * 16);
    blit( sprite_sheet, images[0], image_cord_x * 16, image_cord_y * 16, 0, 0, image_w * 16, image_h * 16);
  }
}
