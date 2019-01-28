#ifndef TILEMAP_H
#define TILEMAP_H

#define M_PI 3.14159265

#include <vector>
#include <string>

#include "Tile.h"
#include "Item.h"
#include "Chunk.h"

class TileMap {
  public:
    TileMap();
    virtual ~TileMap();

    // Size
    int getWidth();
    int getHeight();

    // Chunk management
    Chunk *chunk_at(int x, int y);

    std::string get_biome_at(int x, int y);
    char get_temperature_at(int x, int y);

    // Tiles
    Tile *tile_at (int x, int y, int layer);
    void place_tile (Tile* newTile);
    void replace_tile (Tile *oldTile, Tile *newTile);
    void remove_tile (Tile *newTile);
    bool solid_at (int x, int y);

    // Items
    MapItem *item_at(int x, int y);
    void place_item(Item* item, int x, int y);
    void remove_item(MapItem *item);

    // Update
    void update(int x_1, int y_1, int x_2, int y_2) ;

    // Loading
    void load_map (std::string fileName);
    void generate_map();
    void clear_map();

    // Bitmasks
    void update_bitmask (Tile *newTile);
    void update_bitmask_surround (Tile *newTile);

    // Chunks
    std::vector<Chunk*> chunks;

  private:
    // Size
    int width;
    int height;

};

#endif // TILEMAP_H
