#include "TileType.h"

#include "item_defs.h"
#include "tile_defs.h"

#include <math.h>

// Init tile
TileType::TileType(unsigned char width,
                   unsigned char height,
                   unsigned char id,
                   const std::string& name,
                   unsigned char attribute)
    : id(id), width(width), height(height), name(name), attribute(attribute) {}

// Draw tile
void TileType::draw(int x, int y, unsigned char meta) {
  int image_index = 0;

  if (image_type == "meta_map" || image_type == "animated") {
    image_index = static_cast<int>(floor((num_images / 256.0) * meta));
  } else if (image_type == "meta_map_2" || image_type == "dynamic") {
    image_index = meta % num_images;
  }

  if (images[image_index]) {
    asw::draw::sprite(images[image_index], x, y - ((image_h * 16) - 16));
  } else {
    asw::draw::rectFill(x, y, 16, 16, asw::util::makeColor(0, 200, 0));
  }
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

  // Split up those images
  if (image_type == "dynamic" || image_type == "meta_map" ||
      image_type == "meta_map_2" || image_type == "animated") {
    for (unsigned char t = 0; t < sheet_height; t++) {
      for (unsigned char i = 0; i < sheet_width; i++) {
        unsigned char image_index = (t * sheet_width) + i;

        images[image_index] =
            asw::assets::createTexture(image_w * 16, image_h * 16);

        SDL_SetTextureBlendMode(images[image_index].get(), SDL_BLENDMODE_BLEND);

        SDL_SetRenderTarget(asw::display::renderer, images[image_index].get());

        asw::draw::stretchSpriteBlit(sprite_sheet, image_cord_x * 16 + i * 16,
                                     image_cord_y * 16 + t * 16, image_w * 16,
                                     image_h * 16, 0, 0, image_w * 16,
                                     image_h * 16);

        SDL_SetRenderTarget(asw::display::renderer, nullptr);
      }
    }
  } else {
    images[0] = asw::assets::createTexture(image_w * 16, image_h * 16);

    SDL_SetTextureBlendMode(images[0].get(), SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(asw::display::renderer, images[0].get());

    asw::draw::stretchSpriteBlit(sprite_sheet, image_cord_x * 16,
                                 image_cord_y * 16, image_w * 16, image_h * 16,
                                 0, 0, image_w * 16, image_h * 16);

    SDL_SetRenderTarget(asw::display::renderer, nullptr);
  }
}
