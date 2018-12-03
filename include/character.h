#ifndef CHARACTER_H
#define CHARACTER_H

#include <allegro.h>
#include <string>

#include "tile_map.h"
#include "inventory.h"

class character
{
  public:
    // Ctor and dtor
    character();
    virtual ~character() {};

    // Set world pointer
    void setWorld (tile_map *newTileMap);

    // Load images and samples
    void load_data();

    // Position character
    void setPosition (int newX, int newY) { x = newX; y = newY;}

    // Draw
    void draw (BITMAP *tempBuffer);
    void drawForeground (BITMAP *tempBuffer);

    // Update
    void update();
  protected:

  private:
    // Directions
    enum directions{ DIR_DOWN = 1, DIR_UP = 2, DIR_RIGHT = 3, DIR_LEFT = 4 };

    // Fonts
    FONT *pixelart;
    tile_map *map_pointer;

    // Inventory
    inventory character_inv;

    // Item in hand
    int selected_item;

    // Money
    int money;

    // Position
    int x, y;

    // What tile you are over
    int indicator_x, indicator_y;

    // Movement
    char direction;
    bool moving;
    bool sound_step;
    char ani_ticker;

    // Images for ui and character
    BITMAP *image;
    BITMAP *inventory_gui;
    BITMAP *indicator;
    BITMAP *coin;

    // Sounds
    SAMPLE *pickup;
    SAMPLE *drop;
    SAMPLE *step[2];
};

#endif // CHARACTER_H
