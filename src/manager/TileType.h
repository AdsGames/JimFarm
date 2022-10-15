/*
  Tile Type
  Allan Legemaate
  24/11/15
  Defenitions of the tiles ingame
*/
#ifndef TILE_TYPE_H
#define TILE_TYPE_H

#include <asw/asw.h>
#include <string>

class TileType {
 public:
  TileType(unsigned char newWidth = 1,
           unsigned char newHeight = 1,
           unsigned char newID = 0,
           std::string newName = "",
           int newAttribute = 0,
           unsigned char newValue = 0);

  // Get type
  unsigned char getID() const { return id; }

  // Get name
  std::string getName() const { return name; }

  // Get type
  unsigned char getAttribute() const { return attribute; }

  // Tex
  unsigned char getImageX() const { return image_cord_x; }
  unsigned char getImageY() const { return image_cord_y; }
  unsigned char getWidth() const { return width; }
  unsigned char getHeight() const { return height; }
  unsigned char getValue() const { return value; }

  // Draw
  void draw(int x, int y, unsigned char meta = 0, char offset = 0);

  // Set sprite sheet
  void setSpriteSheet(asw::Texture newSpriteSheet);

  // Set special image stuff
  void setImageType(std::string newImageType,
                    int newSheetWidth,
                    int newSheetHeight,
                    int newImageX,
                    int newImageY,
                    int newImageWidth,
                    int newImageHeight);
  std::string getImageType() { return this->image_type; }

 private:
  unsigned char id;
  std::string name;
  unsigned char attribute;

  std::string image_type;
  int sheet_width;
  int sheet_height;

  int width;
  int height;

  unsigned char image_cord_x, image_cord_y;
  unsigned char image_h, image_w;

  unsigned char value;

  unsigned char num_images;

  asw::Texture sprite_sheet;

  asw::Texture images[16];
};

#endif  // TILE_TYPE_H
