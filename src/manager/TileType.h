/*
  Tile Type
  Allan Legemaate
  24/11/15
  Defenitions of the tiles ingame
*/
#ifndef TILE_TYPE_H
#define TILE_TYPE_H

#include <asw/asw.h>
#include <array>
#include <string>

class TileType {
 public:
  TileType(unsigned char width,
           unsigned char height,
           unsigned char id,
           const std::string& name,
           unsigned char attribute = 0);

  // Get type
  unsigned char getId() const { return id; }

  // Get name
  std::string getName() const { return name; }

  // Get type
  unsigned char getAttribute() const { return attribute; }

  // Tex
  unsigned char getImageX() const { return image_cord_x; }
  unsigned char getImageY() const { return image_cord_y; }
  unsigned char getWidth() const { return width; }
  unsigned char getHeight() const { return height; }

  // Draw
  void draw(int x, int y, unsigned char meta = 0);

  // Set sprite sheet
  void setSpriteSheet(asw::Texture spriteSheet);

  // Set special image stuff
  void setImageType(const std::string& imageType,
                    unsigned char sheetWidth,
                    unsigned char sheetHeight,
                    unsigned char imageX,
                    unsigned char imageY,
                    unsigned char imageWidth,
                    unsigned char imageHeight);

  std::string getImageType() const { return this->image_type; }

 private:
  unsigned char id;

  unsigned char width;
  unsigned char height;

  std::string name;
  unsigned char attribute;

  unsigned char num_images = 0;

  std::string image_type = "";
  unsigned char sheet_width = 1;
  unsigned char sheet_height = 1;

  unsigned char image_cord_x = 0;
  unsigned char image_cord_y = 0;

  unsigned char image_h = 1;
  unsigned char image_w = 1;

  asw::Texture sprite_sheet = nullptr;

  std::array<asw::Texture, 16> images{};
};

#endif  // TILE_TYPE_H
