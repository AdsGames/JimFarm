#ifndef TILE_H
#define TILE_H

#include <allegro.h>
#include <string>

#include "tile_type.h"

class tile
{
  public:
    tile(int newX, int newY, char newID);
    virtual ~tile();

    // Positioning
    int x, y;
    int getWidth(){ return tile_pointer -> getWidth();}
    int getHeight(){ return tile_pointer -> getHeight();}

    // Drawing
    virtual void draw( BITMAP *tempBuffer);
    void draw_at( int newX, int newY, BITMAP *tempBuffer);

    // Sorting stuff
    virtual bool operator< (const tile &other) const { return (y < other.y); }

    virtual void run_tick();
    bool requirements_met;
    char tile_data;

    tile_type *tile_pointer;

    bool isSolid(){ return tile_pointer -> getAttribute(); }

    unsigned char getID(){ return tile_pointer -> getID();}
    void setID( unsigned char newID);

    std::string getName(){ return tile_pointer -> getName();}
  protected:
  private:
};

#endif // TILE_H
