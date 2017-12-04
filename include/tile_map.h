#ifndef TILE_MAP_H
#define TILE_MAP_H

#include <allegro.h>
#include <vector>
#include <string>

#include "tile.h"
#include "item.h"

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

    // Map
    void update();
    void scroll( int player_x, int player_y);
    void load_map( std::string fileName);
    void generate_map();

    int x, y;
  protected:

  private:
    // Tiles
    std::vector<tile*> map_tiles;
    std::vector<tile*> map_tiles_foreground;

    std::vector<map_item*> map_items;

    SAMPLE* egg;
    BITMAP *map_buffer;
};

#endif // TILE_MAP_H
