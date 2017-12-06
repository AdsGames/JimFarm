#ifndef TILE_MAP_H
#define TILE_MAP_H

#include <allegro.h>
#include <vector>
#include <string>

#include "tile.h"
#include "item.h"
#include "messenger.h"

class map_item
{
  public:
    map_item();
    map_item( int x, int y, item *itemPtr);
    ~map_item();

    void draw( BITMAP *tempBuffer);

    int x;
    int y;
    item *itemPtr;
};

class tile_map
{
  public:
    tile_map();
    virtual ~tile_map();

    // Init ticker
    void init_ticker();

    // Constant
    int MAP_WIDTH;
    int MAP_HEIGHT;
    int timer;

    // Viewport
    static const int VIEWPORT_WIDTH = 240;
    static const int VIEWPORT_HEIGHT = 160;

    // Drawing
    void draw( BITMAP *tempBuffer);
    void drawForeground( BITMAP *tempBuffer);
    void load_images();

    // Tiles
    tile *tile_at( int positionX, int positionY, bool foreground);
    void place_tile( tile* newTile);
    void replace_tile( int tileX, int tileY, int newID, bool foreground);
    bool solid_at( int positionX, int positionY);

    // Items
    item *item_at( int positionX, int positionY);
    void place_item( item* newItem, int x, int y);
    void remove_item( item *newItem);
    void remove_item_at( int positionX, int positionY);

    // Interact with
    void interact( int inter_x, int inter_y, item *inHand);

    // Map
    void update();
    void scroll( int player_x, int player_y);
    void load_map( std::string fileName);
    void generate_map();

    // Get x and y
    int getX(){ return this -> x; }
    int getY(){ return this -> y; }

    // Get messenger
    messenger *getMessenger(){ return this -> map_messages; }
  protected:

  private:
    // Tiles
    std::vector<tile*> map_tiles;
    std::vector<tile*> map_tiles_foreground;

    std::vector<map_item*> map_items;

    int x, y;

    BITMAP *map_buffer;

    static volatile int ticks;
    static void tick_counter();

    messenger *map_messages;
};

#endif // TILE_MAP_H
