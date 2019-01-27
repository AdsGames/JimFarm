#include "Chunk.h"

#include "tile_defs.h"

#include "Graphics.h"

Chunk::Chunk(int x, int y) {
  this -> x = x;
  this -> y = y;

  this -> is_drawing = false;

  for (int i = 0; i < CHUNK_WIDTH; i++) {
    for (int t = 0; t < CHUNK_HEIGHT; t++) {
      for (int j = 0; j < CHUNK_LAYERS; j++) {
        tiles[i][t][j] = nullptr;
      }
    }
  }

  generate();
}

Chunk::~Chunk() {

}

int Chunk::getX() {
  return x;
}

int Chunk::getY() {
  return y;
}

Tile* Chunk::get_tile_at(int x, int y, int z) {
  int offset_x = x / TILE_WIDTH  - this -> x * CHUNK_WIDTH ;
  int offset_y = y / TILE_HEIGHT - this -> y * CHUNK_HEIGHT;

  if (offset_x < 0 || offset_x > CHUNK_WIDTH || offset_y < 0 || offset_y > CHUNK_HEIGHT || z < 0 || z > CHUNK_LAYERS) {
    return nullptr;
  }

  return tiles[offset_x][offset_y][z];
}

void Chunk::set_tile_at(int x, int y, int z, Tile* tile) {
  int offset_x = x / TILE_WIDTH  - this -> x * CHUNK_WIDTH;
  int offset_y = y / TILE_HEIGHT - this -> y * CHUNK_HEIGHT;

  if (offset_x < 0 || offset_x > CHUNK_WIDTH || offset_y < 0 || offset_y > CHUNK_HEIGHT || z < 0 || z > CHUNK_LAYERS) {
    return;
  }

  if (tiles[offset_x][offset_y][z]) {
    delete tiles[offset_x][offset_y][z];
    tiles[offset_x][offset_y][z] = tile;
    return;
  }

  tiles[offset_x][offset_y][z] = tile;
}

void Chunk::update(int x_1, int y_1, int x_2, int y_2) {
  if (x_2 >= (this -> x)     * CHUNK_WIDTH  * TILE_WIDTH  &&
      x_1 <= (this -> x + 1) * CHUNK_WIDTH  * TILE_WIDTH  &&
      y_2 >= (this -> y)     * CHUNK_HEIGHT * TILE_HEIGHT &&
      y_1 <= (this -> y + 1) * CHUNK_HEIGHT * TILE_HEIGHT) {
    // Add to draw pool
    if (!is_drawing) {
      Graphics::Instance() -> disableSort();
      for (int i = 0; i < CHUNK_WIDTH; i++) {
        for (int t = 0; t < CHUNK_HEIGHT; t++) {
          for (int j = 0; j < CHUNK_LAYERS; j++) {
            if (tiles[i][t][j])
              Graphics::Instance() -> add(tiles[i][t][j]);
          }
        }
      }
      Graphics::Instance() -> enableSort();
      is_drawing = true;
    }
  }
  else {
    // Remove from draw pool
    if (is_drawing) {
      Graphics::Instance() -> disableSort();
      for (int i = 0; i < CHUNK_WIDTH; i++) {
        for (int t = 0; t < CHUNK_HEIGHT; t++) {
          for (int j = 0; j < CHUNK_LAYERS; j++) {
            if (tiles[i][t][j])
              Graphics::Instance() -> remove(tiles[i][t][j]);
          }
        }
      }
      Graphics::Instance() -> enableSort();
      is_drawing = false;
    }
  }
}

void Chunk::generate() {
  for (int i = 0; i < CHUNK_WIDTH; i++) {
    for (int t = 0; t < CHUNK_HEIGHT; t++) {
      tiles[i][t][0] = new Tile(TILE_GRASS, i * TILE_WIDTH + x * CHUNK_WIDTH * TILE_WIDTH, t * TILE_HEIGHT + y * CHUNK_WIDTH * TILE_WIDTH, 0);
    }
  }
}

Tile* Chunk::getTiles() {
  return ***tiles;
}
