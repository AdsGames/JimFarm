#include "TileType.h"

#include "item_defs.h"
#include "tile_defs.h"

#include <math.h>

// Init tile
TileType::TileType(unsigned char width,
                   unsigned char height,
                   unsigned char id,
                   const std::string& name,
                   int attribute,
                   unsigned char value)
    : id(id),
      width(width),
      height(height),
      name(name),
      attribute(attribute),
      value(value) {}

// Draw tile
void TileType::draw(int x, int y, unsigned char meta, char offset) {
  int imageNum = 0;

  if (image_type == "meta_map" || image_type == "animated") {
    imageNum = floor((float(num_images) / 256.0f) * (float)meta);
  } else if (image_type == "meta_map_2" || image_type == "dynamic") {
    imageNum = meta % num_images;
  }

  if (images[imageNum]) {
    asw::draw::sprite(images[imageNum], x, y - ((image_h * 16) - 16));
  } else {
    asw::draw::rectFill(x, y, 16, 16, asw::util::makeColor(0, 200, 0));
  }
}

// Give a sprite sheet to this tile
void TileType::setSpriteSheet(asw::Texture newSpriteSheet) {
  sprite_sheet = newSpriteSheet;
}

// Set special image stuff
void TileType::setImageType(std::string newImageType,
                            int newSheetWidth,
                            int newSheetHeight,
                            int newImageX,
                            int newImageY,
                            int newImageWidth,
                            int newImageHeight) {
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
  if (image_type == "dynamic" || image_type == "meta_map" ||
      image_type == "meta_map_2" || image_type == "animated") {
    for (int t = 0; t < sheet_height; t++) {
      for (int i = 0; i < sheet_width; i++) {
        images[(t * sheet_width) + i] =
            asw::assets::createTexture(image_w * 16, image_h * 16);

        SDL_SetTextureBlendMode(images[(t * sheet_width) + i].get(),
                                SDL_BLENDMODE_BLEND);

        SDL_SetRenderTarget(asw::display::renderer,
                            images[(t * sheet_width) + i].get());

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
