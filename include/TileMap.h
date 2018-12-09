#ifndef TILE_MAP_H
#define TILE_MAP_H

#include <allegro.h>
#include <vector>
#include <string>

#define LAYER_FOREGROUND 1
#define LAYER_BACKGROUND 0

#include "Tile.h"
#include "Item.h"
#include "Messenger.h"

class map_item
{
  public:
    map_item (int x = 0, int y = 0, item *itemPtr = NULL);
    ~map_item();

    void draw (BITMAP *tempBuffer);

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
    void draw (BITMAP *tempBuffer);
    void drawForeground (BITMAP *tempBuffer);
    void load_images();

    // Tiles
    tile *tile_at (int positionX, int positionY, bool foreground);
    void place_tile (tile* newTile, bool foreground);
    bool place_tile_safe (tile* newTile, bool foreground, int opposite_layer_id = -1);
    void replace_tile (tile *oldTile, int newID, bool foreground);
    void remove_tile (tile *newTile, bool foreground);
    bool solid_at (int positionX, int positionY);

    // Items
    item *item_at (int positionX, int positionY);
    void place_item (item* newItem, int x, int y);
    void remove_item (item *newItem);

    // Interact with
    void interact (int inter_x, int inter_y, item *inHand);

    // Map
    void update();
    void scroll (int player_x, int player_y);
    void load_map (std::string fileName);
    void generate_map();
    void clear_map();
    void update_bitmask (tile *newTile, bool layer = LAYER_BACKGROUND);
    void update_bitmask_surround (tile *newTile, bool layer = LAYER_BACKGROUND);

    // Get x and y
    int getX() { return this -> x; }
    int getY() { return this -> y; }

    // Get messenger
    messenger *getMessenger() { return this -> map_messages; }
  protected:

  private:
    // Tiles
    std::vector<tile*> map_tiles;
    std::vector<tile*> map_tiles_foreground;

    // Items
    std::vector<map_item*> map_items;

    // Scroll position
    int x, y;

    // Buffer that holds whole map image
    BITMAP *map_buffer;

    // Ticker for world
    static volatile int ticks;
    static void tick_counter();

    // Messager
    messenger *map_messages;
};

#endif // TILE_MAP_H
