#ifndef TILE_H
#define TILE_H

#include <allegro.h>
#include <string>

#include "tile_type.h"

#define MAX_TILE_META 255

class tile
{
  public:
    tile(int newX, int newY, char newID);
    virtual ~tile();

    int getImgWidth(){ return tile_pointer -> getImgWidth();}
    int getImgHeight(){ return tile_pointer -> getImgHeight();}

    int getWidth(){ return tile_pointer -> getWidth();}
    int getHeight(){ return tile_pointer -> getHeight();}

    // Drawing
    virtual void draw( BITMAP *tempBuffer);
    void draw_at( int newX, int newY, BITMAP *tempBuffer);

    // Sorting stuff
    virtual bool operator < (const tile &other) const { return (y < other.y); }



    bool isSolid(){ return tile_pointer -> getAttribute(); }

    unsigned char getID(){ return tile_pointer -> getID();}
    void setID( unsigned char newID);

    std::string getName(){ return tile_pointer -> getName();}

    tile_type *tile_pointer;

    void setMeta( unsigned char meta ){ this -> meta = meta; }
    unsigned char getMeta(){ return this -> meta; }
    void addMeta( unsigned char amt ){ this -> meta += amt; }

    int getX(){ return this -> x; }
    int getY(){ return this -> y; }
  protected:
  private:
    // Positioning
    int x, y;

    // Metadata info
    unsigned char meta;
};

#endif // TILE_H
