#ifndef TILE_H
#define TILE_H

#include <allegro.h>
#include <string>

#include "tile_type.h"

#define MAX_TILE_META 255

class tile
{
  public:
    // Ctor and Dtor
    tile (char newID, int newX, int newY, unsigned char meta = 0);
    virtual ~tile() {};

    // Define < operator for sorting
    virtual bool operator < (const tile &other) const { return (y < other.y); }

    // Gets size
    int getWidth() { return tile_pointer -> getWidth();}
    int getHeight() { return tile_pointer -> getHeight();}

    // Drawing
    virtual void draw (BITMAP *tempBuffer);
    void draw_at (int newX, int newY, BITMAP *tempBuffer);

    // Is this type solid?
    bool isSolid() { return tile_pointer -> getAttribute(); }

    // Modify ID
    unsigned char getID() { return tile_pointer -> getID();}

    // Get name of tile
    std::string getName() { return tile_pointer -> getName();}

    // Access and set meta data byte
    void setMeta (unsigned char meta ) { this -> meta = meta; }
    unsigned char getMeta() { return this -> meta; }
    void changeMeta (unsigned char amt ) { this -> meta += amt; }

    // Get position
    int getX() { return this -> x; }
    int getY() { return this -> y; }

    // Get image type
    bool needsBitmask() { return (this -> tile_pointer -> getImageType() == "dynamic"); }
  protected:
  private:
    // Positioning
    int x, y;

    // Metadata info
    unsigned char meta;

    // Ptr to tile type
    tile_type *tile_pointer;
};

#endif // TILE_H
