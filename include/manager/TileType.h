/*
  Tile Type
  Allan Legemaate
  24/11/15
  Defenitions of the tiles ingame
*/
#ifndef TILE_TYPE_H
#define TILE_TYPE_H

#include <allegro.h>
#include <string>

#include "InteractBehaviour.h"

class TileType {
  public:
    TileType (unsigned char newWidth = 1, unsigned char newHeight = 1, unsigned char newID = 0,
               std::string newName = "", int newAttribute = 0, unsigned char newValue = 0);
    virtual ~TileType();

    // Get type
    unsigned char getID() { return id; }

    // Get name
    std::string getName() { return name; }

    // Get type
    unsigned char getAttribute() { return attribute; }

    // Tex
    unsigned char getImageX() { return image_cord_x; }
    unsigned char getImageY() { return image_cord_y; }
    unsigned char getWidth() { return width; }
    unsigned char getHeight() { return height; }
    unsigned char getValue() { return value; }

    // Draw
    void draw (int x, int y, BITMAP *tempBuffer, unsigned char meta = 0, char offset = 0);

    // Set sprite sheet
    void setSpriteSheet (BITMAP *newSpriteSheet);

    // Set special image stuff
    void setImageType (std::string newImageType, int newSheetWidth, int newSheetHeight, int newImageX, int newImageY, int newImageWidth, int newImageHeight);
    std::string getImageType() { return this -> image_type; }
  protected:
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

    BITMAP *sprite_sheet;

    BITMAP *images[16];
};

#endif // TILE_TYPE_H
