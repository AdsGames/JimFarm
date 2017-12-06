#ifndef CHARACTER_H
#define CHARACTER_H

#include <allegro.h>
#include <string>

#include "tile_map.h"
#include "inventory.h"

class character
{
  public:
    character();
    virtual ~character() {};

    void setPosition( int newX, int newY){ x = newX; y = newY;}
    void load_data();

    void draw( BITMAP *tempBuffer);
    void drawForeground( BITMAP *tempBuffer);

    void update();

    void setWorld( tile_map *newTileMap);
  protected:

  private:
    // Directions
    enum directions{ DIR_DOWN = 1, DIR_UP = 2, DIR_RIGHT = 3, DIR_LEFT = 4 };

    // Fonts
    FONT *pixelart;
    tile_map *map_pointer;

    inventory character_inv;

    int selected_item;
    int money;

    int x, y;
    int indicator_x, indicator_y;

    bool sound_step;
    char aniTick;
    char direction;
    bool moving;

    BITMAP *image;
    BITMAP *inventory_gui;
    BITMAP *indicator;
    BITMAP *coin;

    SAMPLE *pickup;
    SAMPLE *drop;
    SAMPLE *step[2];
};

#endif // CHARACTER_H
