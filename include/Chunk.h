#ifndef CHUNK_H
#define CHUNK_H

#include "Tile.h"

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 16
#define CHUNK_LAYERS 4


#define LAYER_BACKGROUND 0
#define LAYER_CHARACTER 1
#define LAYER_ITEMS 2
#define LAYER_FOREGROUND 3

class Chunk {
  public:
    Chunk(int x, int y);
    virtual ~Chunk();

    Tile* getTiles();

    int getX();
    int getY();

    Tile* get_tile_at(int x, int y, int z);
    void set_tile_at(int x, int y, int z, Tile* tile);

    void update(int x_1, int y_1, int x_2, int y_2);

  protected:

  private:
    // Tiles
    Tile* tiles[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_LAYERS];

    // Near tiles
    Chunk *neighbours[8];

    // Generate
    void generate();

    int x;
    int y;

    bool is_drawing;
};

#endif // CHUNK_H
