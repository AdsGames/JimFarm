#ifndef TILE_MAP_H
#define TILE_MAP_H

#include <allegro.h>
#include <vector>
#include <string>

#include "tile.h"

class tile_map
{
  public:
    tile_map();
    virtual ~tile_map();

    // Constant
    int MAP_WIDTH;
    int MAP_HEIGHT;
    int timer;

    static const int VIEWPORT_WIDTH = 240;
    static const int VIEWPORT_HEIGHT = 160;

    void draw( BITMAP *tempBuffer);
    void drawForeground( BITMAP *tempBuffer);

    int tile_at( int positionX, int positionY, bool foreground);
    void place_tile( tile* newTile);
    void replace_tile( int tileX, int tileY, int newID, bool foreground);

    item *item_at( int positionX, int positionY);
    void place_item( item* newItem);
    void remove_item( item *newItem);
    void remove_item_at( int positionX, int positionY);

    bool solid_at( int positionX, int positionY);

    void update();

    void load_map( std::string fileName);
    void generate_map();
    void load_images();
    void scroll( int player_x, int player_y);

    int x, y;
  protected:

  private:
    // Tiles
    std::vector<tile*> map_tiles;
    std::vector<tile*> map_tiles_foreground;

    std::vector<item*> map_items;

    SAMPLE* egg;
    BITMAP *map_buffer;
};

class map_item
{
  public:
    map_item();
    ~map_item();

    int x;
    int y;
    item *itemPtr;
};

#endif // TILE_MAP_H
