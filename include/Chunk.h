#ifndef CHUNK_H
#define CHUNK_H

#include "Tile.h"
#include "MapItem.h"

#include <vector>

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 16
#define CHUNK_LAYERS 5

#define LAYER_BACKGROUND 0
#define LAYER_MIDGROUND 1
#define LAYER_CHARACTER 2
#define LAYER_ITEMS 3
#define LAYER_FOREGROUND 4

class Chunk {
  public:
    Chunk(int x, int y);
    virtual ~Chunk();

    int getX();
    int getY();

    Tile* get_tile_at(int x, int y, int z);
    void set_tile_at(int x, int y, int z, Tile* tile);

    MapItem* get_item_at(int x, int y);
    void place_item_at(Item* item, int x, int y);
    void remove_item(MapItem* item);


    void set_draw_enabled(bool enabled);

    void update(int x_1, int y_1, int x_2, int y_2);

    static int seed;

  private:
    // Tiles
    Tile* tiles[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_LAYERS];

    // Items
    std::vector<MapItem*> items;

    // Near tiles
    Chunk *neighbours[8];

    // Generate
    void generate();

    int x;
    int y;

    bool is_drawing;
};

#endif // CHUNK_H
