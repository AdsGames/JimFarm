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
    virtual ~character();

    void setPosition( int newX, int newY){ x = newX; y = newY;}
    void load_data();

    void draw( BITMAP *tempBuffer);
    void drawForeground( BITMAP *tempBuffer);

    void update();

    void remove_item();
    void give_item( char newItem);

    void setWorld( tile_map *newTileMap);

    inventory character_inv;
    int selected_item;

    int money;

    bool store_open;

    tile_map *map_pointer;

  protected:

  private:

    // Fonts
    FONT *f1;
    FONT *f2;
    FONT *f3;
    FONT *f4;
    FONT *f5;
    FONT *pixelart;

    int x, y;

    int water;
    int sound_step;

    bool draw_hud;

    BITMAP *image;
    BITMAP *watering_can[5];
    BITMAP *inventory_gui;
    BITMAP *hand;
    BITMAP *indicator;
    BITMAP *coin;


    SAMPLE *pickup;
    SAMPLE *drop;
    SAMPLE *step_1;
    SAMPLE *step_2;
    SAMPLE *cut_axe;
    SAMPLE *cut_scythe;
    SAMPLE *hoe;
    SAMPLE *dig;
    SAMPLE *error;
    SAMPLE *water_fill;
    SAMPLE *water_pour;

    int indicator_x, indicator_y;

    char gameTick;
    char direction;
    bool moving;
};

#endif // CHARACTER_H
