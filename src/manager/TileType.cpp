#include "manager/TileType.h"

#include "manager/item_defs.h"
#include "manager/tile_defs.h"

#include <math.h>

// Init tile
TileType::TileType (unsigned char newWidth, unsigned char newHeight, unsigned char newID,
                      std::string newName, int newAttribute, unsigned char newValue) {
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
TileType::~TileType() {}

// Draw tile
void TileType::draw (int x, int y, BITMAP *tempBuffer, unsigned char meta, char offset) {
  if (image_type == "meta_map" || image_type == "animated") {
    int imageNum = floor((float(num_images) / 256.0f) * (float)meta);
    if (images[imageNum])
      draw_trans_sprite (tempBuffer, images[imageNum], x, y - ((image_h * 16) - 16));
    //textprintf_ex (tempBuffer, font, x, y, 0xFFFFFF, -1, "%d", meta);
  }
  else if (image_type == "meta_map_2" || image_type == "dynamic") {
    int imageNum = meta % num_images;
    if (images[imageNum])
      draw_trans_sprite (tempBuffer, images[imageNum], x, y - ((image_h * 16) - 16));
    //textprintf_ex (tempBuffer, font, x, y, 0xFFFFFF, -1, "%d", meta);
  }
  else{
    if (images[0])
      draw_trans_sprite (tempBuffer, images[0], x, y - ((image_h * 16) - 16));
  }
}

// Give a sprite sheet to this tile
void TileType::setSpriteSheet (BITMAP *newSpriteSheet) {
  sprite_sheet = newSpriteSheet;
}

// Set special image stuff
void TileType::setImageType (std::string newImageType, int newSheetWidth, int newSheetHeight, int newImageX, int newImageY, int newImageWidth, int newImageHeight) {
  // Default, dynamic or animated
  image_type = newImageType;
  sheet_width = newSheetWidth;
  sheet_height = newSheetHeight;
  image_w = newImageWidth;
  image_h = newImageHeight;
  image_cord_x = newImageX;
  image_cord_y = newImageY;

  num_images = sheet_width * sheet_height;

  // Split up those images
  if (image_type == "dynamic" || image_type == "meta_map" || image_type == "meta_map_2" || image_type == "animated") {
    for (int t = 0; t < sheet_height; t++) {
      for (int i = 0; i < sheet_width; i++) {
        images[(t * sheet_width) + i] = create_bitmap (image_w * 16, image_h * 16);
        blit (sprite_sheet, images[(t * sheet_width) + i], image_cord_x * 16 + i * 16, image_cord_y * 16 + t * 16, 0, 0, image_w * 16, image_h * 16);
      }
    }
  }
  else{
    images[0] = create_bitmap (image_w * 16, image_h * 16);
    blit (sprite_sheet, images[0], image_cord_x * 16, image_cord_y * 16, 0, 0, image_w * 16, image_h * 16);
  }
}
