#ifndef CHUNK_H
#define CHUNK_H

#include "Tile.h"

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 16
#define CHUNK_LAYERS 2

class Chunk {
  public:
    Chunk(int x, int y);
    virtual ~Chunk();

    Tile* getTiles();

    int getX();
    int getY();

    void update(int x_1, int y_1, int x_2, int y_2);

  protected:

  private:
    // Tiles
    Tile* tiles[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_LAYERS];

    // Generate
    void generate();

    int x;
    int y;

    bool is_drawing;
};

#endif // CHUNK_H
