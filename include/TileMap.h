#ifndef TILEMAP_H
#define TILEMAP_H

#define LAYER_BACKGROUND 0
#define LAYER_CHARACTER 1
#define LAYER_ITEMS 2
#define LAYER_FOREGROUND 3

#define M_PI 3.14159265

#include <vector>
#include <string>

#include "Tile.h"
#include "Chunk.h"

class TileMap {
  public:
    TileMap();
    virtual ~TileMap();

    // Size
    int getWidth();
    int getHeight();

    // Tiles
    Tile *tile_at (int positionX, int positionY, int layer);
    void place_tile (Tile* newTile);
    bool place_tile_safe (Tile* newTile, int opposite_layer_id = -1);
    void replace_tile (Tile *oldTile, Tile *newTile);
    void remove_tile (Tile *newTile);
    bool solid_at (int positionX, int positionY);

    // Update
    void update(int x_1, int y_1, int x_2, int y_2) ;

    // Loading
    void load_map (std::string fileName);
    void generate_map();
    void clear_map();

    // Bitmasks
    void update_bitmask (Tile *newTile, bool layer = LAYER_BACKGROUND);
    void update_bitmask_surround (Tile *newTile, bool layer = LAYER_BACKGROUND);

    // Tiles
    //std::vector<Tile*> map_tiles;

    std::vector<Chunk*> chunks;

  private:
    // Size
    int width;
    int height;

};

#endif // TILEMAP_H
