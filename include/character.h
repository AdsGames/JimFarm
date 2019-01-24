#ifndef CHARACTER_H
#define CHARACTER_H

#include <allegro.h>
#include <string>

#include "World.h"
#include "Inventory.h"
#include "Sprite.h"

class Character : public Sprite {
  public:
    // Ctor and dtor
    Character();
    virtual ~Character() {};

    // Set world pointer
    void setWorld (World *newTileMap);

    // Load images and samples
    void load_data();

    // Position character
    void setPosition (int newX, int newY) { x = newX; y = newY;}

    // Draw
    virtual void draw (BITMAP *tempBuffer, float x_1, float y_1, float x_2, float y_2) override;
    void drawForeground (BITMAP *tempBuffer);

    // Update
    void update();
  protected:

  private:
    // Directions
    enum directions{ DIR_DOWN = 1, DIR_UP = 2, DIR_RIGHT = 3, DIR_LEFT = 4 };

    // Fonts
    FONT *pixelart;
    World *map_pointer;

    // Inventory
    Inventory character_inv;

    // Item in hand
    int selected_item;

    // Money
    int money;

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
