#ifndef TILE_H
#define TILE_H

#include <string>

#include "TileType.h"
#include "Sprite.h"

#define MAX_TILE_META 255

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

class Tile : public Sprite {
  public:
    // Ctor and Dtor
    Tile (char id, int x, int y, int z, unsigned char meta = 0);
    virtual ~Tile() {};

    // Define < operator for sorting
    virtual bool operator < (const Tile &other) const { return (y < other.y); }

    // Gets size
    int getWidth() { return tile_pointer -> getWidth();}
    int getHeight() { return tile_pointer -> getHeight();}

    // Drawing
    virtual void draw (BITMAP *tempBuffer, float x_1, float y_1, float x_2, float y_2);
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

    // Get image type
    bool needsBitmask() { return (this -> tile_pointer -> getImageType() == "dynamic"); }
  protected:
  private:
    // Metadata info
    unsigned char meta;

    // Ptr to tile type
    TileType *tile_pointer;
};

#endif // TILE_H
