#include "TileType.h"

#include <math.h>

// Init tile
TileType::TileType(unsigned char width,
                   unsigned char height,
                   const std::string& id,
                   const std::string& name,
                   unsigned char attribute)
    : id(id), width(width), height(height), name(name), attribute(attribute) {}

// Draw tile
void TileType::draw(int x, int y, unsigned char meta) const {
  int image_index = 0;
  auto h_px = image_h * 16;
  auto w_px = image_w * 16;
  auto i_x_px = image_cord_x * 16;
  auto i_y_px = image_cord_y * 16;

  if (image_type == "meta_map" || image_type == "animated") {
    image_index = num_images / 256 * meta;
    i_x_px += (image_index % sheet_width) * 16;
    i_y_px += (image_index / sheet_width) * 16;

  } else if (image_type == "meta_map_2" || image_type == "dynamic") {
    image_index = meta % num_images;
    i_x_px += (image_index % sheet_width) * 16;
    i_y_px += (image_index / sheet_width) * 16;
  }

  asw::draw::stretchSpriteBlit(sprite_sheet, i_x_px, i_y_px, w_px, h_px, x,
                               y - h_px + 16, w_px, h_px);
}

// Give a sprite sheet to this tile
void TileType::setSpriteSheet(asw::Texture newSpriteSheet) {
  sprite_sheet = newSpriteSheet;
}

// Set special image stuff
void TileType::setImageType(const std::string& type,
                            unsigned char sheet_w,
                            unsigned char sheet_h,
                            unsigned char image_x,
                            unsigned char image_y,
                            unsigned char image_width,
                            unsigned char image_height) {
  // Default, dynamic or animated
  image_type = type;
  sheet_width = sheet_w;
  sheet_height = sheet_h;
  image_w = image_width;
  image_h = image_height;
  image_cord_x = image_x;
  image_cord_y = image_y;

  num_images = sheet_width * sheet_height;
}

void TileType::attachBehaviour(std::shared_ptr<TileBehaviour> behaviour) {
  behaviours.push_back(behaviour);
}