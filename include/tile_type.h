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

class tile_type{
  public:
    tile_type( unsigned char newImageX = 0, unsigned char newImageY = 0, unsigned char newImageW = 1,
               unsigned char newImageH = 1, unsigned char newID = 0, std::string newName = "", int newAttribute = 0);
    virtual ~tile_type();

    // Get type
    char getID(){ return id; }

    // Get name
    std::string getName(){ return name; }

    // Get type
    unsigned char getAttribute(){ return attribute; }

    // Tex
    char getImageX(){ return image_cord_x; }
    char getImageY(){ return image_cord_y; }
    char getWidth(){ return image_w; }
    char getHeight(){ return image_h; }

    // Draw
    void draw( int x, int y);

  protected:
  private:
    unsigned char id;
    std::string name;
    unsigned char attribute;

    unsigned char image_cord_x, image_cord_y;
    unsigned char image_h, image_w;
};

#endif // TILE_TYPE_H
