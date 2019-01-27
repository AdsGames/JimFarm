#ifndef TILE_MAP_H
#define TILE_MAP_H

#include <allegro.h>

#include "Item.h"
#include "MapItem.h"
#include "Messenger.h"

#include "TileMap.h"

class World {
  public:
    World();
    virtual ~World();

    // Init ticker
    void init_ticker();

    // Constant
    int timer;

    // Viewport
    static constexpr int VIEWPORT_WIDTH = 240*2;
    static constexpr int VIEWPORT_HEIGHT = 160*2;
    static constexpr float VIEWPORT_MAX_ZOOM = 2.0f;
    static constexpr float VIEWPORT_MIN_ZOOM = 0.5f;

    float VIEWPORT_ZOOM;

    // Drawing
    void draw (BITMAP *tempBuffer);

    void load_images();

    // Items
    Item *item_at (int positionX, int positionY);
    void place_item (Item* newItem, int x, int y);
    void remove_item (Item *newItem);

    // Interact with
    void interact (int inter_x, int inter_y, Item *inHand);

    // Map
    void update();
    void scroll (int player_x, int player_y);

    // Get x and y
    int getX() { return this -> x; }
    int getY() { return this -> y; }

    // Get messenger
    Messenger *getMessenger() { return this -> map_messages; }

    // Tile map
    TileMap *world_map;
  protected:

  private:
    // Items
    std::vector<MapItem*> map_items;

    // Scroll position
    int x, y;

    // Buffer that holds whole map image
    BITMAP *map_buffer;

    // Ticker for world
    static volatile int ticks;
    static void tick_counter();

    // Messager
    Messenger *map_messages;
};

#endif // TILE_MAP_H
