#include "Chunk.h"

#include "tile_defs.h"
#include "item_defs.h"

#include "Tools.h"

#include "Graphics.h"

#include "SimplexNoise.h"

int Chunk::seed = 0;

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
  set_draw_enabled(false);

  for (int i = 0; i < CHUNK_WIDTH; i++) {
    for (int t = 0; t < CHUNK_HEIGHT; t++) {
      for (int j = 0; j < CHUNK_LAYERS; j++) {
        if (tiles[i][t][j]) {
          delete tiles[i][t][j];
        }
      }
    }
  }

  for (std::vector<MapItem*>::iterator i = items.begin(); i != items.end(); ++i) {
    delete *i;
  }
  items.clear();
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
    Graphics::Instance() -> remove(tiles[offset_x][offset_y][z]);
    delete tiles[offset_x][offset_y][z];
  }

  tiles[offset_x][offset_y][z] = tile;

  if (tile && is_drawing) {
    Graphics::Instance() -> add(tiles[offset_x][offset_y][z]);
  }
}

// Get item at position
MapItem *Chunk::get_item_at(int x, int y) {
  for (unsigned int i = 0; i < items.size(); i++)
    if (items.at(i) -> x == x && items.at(i) -> y == y)
      return items.at(i);
  return nullptr;
}

// Place item on map
void Chunk::place_item_at(Item* item, int x, int y) {
  if (item) {
    MapItem *newMapItem = new MapItem (x, y, item);
    items.push_back (newMapItem);

    if (is_drawing)
      Graphics::Instance() -> add(newMapItem);
  }
}

// Remove item from map
void Chunk::remove_item(MapItem *item) {
  if (item != nullptr) {
    for (unsigned int i = 0; i < items.size(); i++) {
      if (items.at(i) == item) {
        Graphics::Instance() -> remove(items.at(i));
        items.erase (items.begin() + i);
        break;
      }
    }
  }
}

void Chunk::set_draw_enabled(bool enabled) {
  if (is_drawing != enabled) {
    Graphics::Instance() -> disableSort();
    for (int i = 0; i < CHUNK_WIDTH; i++) {
      for (int t = 0; t < CHUNK_HEIGHT; t++) {
        for (int j = 0; j < CHUNK_LAYERS; j++) {
          if (tiles[i][t][j] && enabled) {
            Graphics::Instance() -> add(tiles[i][t][j]);
          }
          else if(tiles[i][t][j] && !enabled) {
            Graphics::Instance() -> remove(tiles[i][t][j]);
          }
        }
      }
    }

    for (std::vector<MapItem*>::iterator i = items.begin(); i != items.end(); ++i) {
      if (enabled) {
        Graphics::Instance() -> add(*i);
      }
      else {
        Graphics::Instance() -> remove(*i);
      }
    }
  }

  Graphics::Instance() -> enableSort();
  is_drawing = enabled;
}

void Chunk::update(int x_1, int y_1, int x_2, int y_2) {
  if (x_2 >= (this -> x)     * CHUNK_WIDTH  * TILE_WIDTH  &&
      x_1 <= (this -> x + 1) * CHUNK_WIDTH  * TILE_WIDTH  &&
      y_2 >= (this -> y)     * CHUNK_HEIGHT * TILE_HEIGHT &&
      y_1 <= (this -> y + 1) * CHUNK_HEIGHT * TILE_HEIGHT) {
    // Add to draw pool
    if (!is_drawing) {
      set_draw_enabled(true);
    }
  }
  else {
    // Remove from draw pool
    if (is_drawing) {
      set_draw_enabled(false);
    }
  }
}

void Chunk::generate() {
  // Noise
  SimplexNoise *sn_h = new SimplexNoise(1.0f, 1.0f, 2.0f, 0.47f);
  SimplexNoise *sn_b = new SimplexNoise(1.0f, 1.0f, 1.0f, 0.47f);

  for (int i = 0; i < CHUNK_WIDTH; i++) {
    for (int t = 0; t < CHUNK_HEIGHT; t++) {
      float height = sn_h -> fractal(10, Chunk::seed + (float)(i + x * CHUNK_WIDTH) / 100.0f, Chunk::seed + (float)(t + y * CHUNK_HEIGHT) / 100.0f);
      float biome  = sn_b -> fractal(10, Chunk::seed % 1000 + (float)(i + x * CHUNK_WIDTH) / 100.0f, Chunk::seed % 1000 + (float)(t + y * CHUNK_HEIGHT) / 100.0f);

      int t_x = (i + x * CHUNK_WIDTH) * TILE_WIDTH;
      int t_y = (t + y * CHUNK_HEIGHT) * TILE_HEIGHT;

      // Water deep
      if (height < -0.5f){
        tiles[i][t][LAYER_BACKGROUND] = new Tile(TILE_UNDERWATER_SOIL, t_x, t_y, LAYER_BACKGROUND, 3);
        tiles[i][t][LAYER_MIDGROUND] = nullptr;
        tiles[i][t][LAYER_FOREGROUND] = new Tile(TILE_WATER, t_x, t_y, LAYER_FOREGROUND);
      }
      // Water
      else if (height < -0.35f){
        tiles[i][t][LAYER_BACKGROUND] = new Tile(TILE_UNDERWATER_SOIL, t_x, t_y, LAYER_BACKGROUND, 0);
        tiles[i][t][LAYER_MIDGROUND] = nullptr;
        tiles[i][t][LAYER_FOREGROUND] = new Tile(TILE_WATER, t_x, t_y, LAYER_FOREGROUND);
      }
      // Water seaweed
      else if (height < -0.3f){
        tiles[i][t][LAYER_BACKGROUND] = new Tile(TILE_UNDERWATER_SOIL, t_x, t_y, LAYER_BACKGROUND, 1);
        tiles[i][t][LAYER_MIDGROUND] = nullptr;
        tiles[i][t][LAYER_FOREGROUND] = new Tile(TILE_WATER, t_x, t_y, LAYER_FOREGROUND);
      }
      // Shore
      else if (height < -0.25f){
        tiles[i][t][LAYER_BACKGROUND] = new Tile(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[i][t][LAYER_MIDGROUND] = new Tile(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
        tiles[i][t][LAYER_FOREGROUND] = new Tile(TILE_DENSE_GRASS, t_x, t_y, LAYER_FOREGROUND);
      }
      else {
        tiles[i][t][LAYER_BACKGROUND] = new Tile(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
      }

      // Plains
      if (biome < 0.0f) {
        if (tiles[i][t][LAYER_FOREGROUND] == nullptr) {
          tiles[i][t][LAYER_MIDGROUND]  = new Tile(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
          if (random(0, 10) == 0) {
            tiles[i][t][LAYER_FOREGROUND] = new Tile(TILE_DENSE_GRASS, t_x, t_y, LAYER_FOREGROUND, random(0, 3));
            if (random(0, 10) == 0) {
              place_item_at(new Item(ITEM_CHICKEN), t_x, t_y);
            }
          }
          else if (random(0, 40) == 0) {
            tiles[i][t][LAYER_FOREGROUND] = new Tile(TILE_TREE, t_x, t_y, LAYER_FOREGROUND, random(0, 2));
          }
        }
      }
      // Forest
      else if (biome < 0.3f) {
        if (tiles[i][t][LAYER_FOREGROUND] == nullptr) {
          tiles[i][t][LAYER_MIDGROUND]  = new Tile(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
          if (random(0, 4) != 0) {
            tiles[i][t][LAYER_FOREGROUND] = new Tile(TILE_TREE, t_x, t_y, LAYER_FOREGROUND, random(0, 2));
          }
        }
      }
      // Tundra
      else {
        if (tiles[i][t][LAYER_FOREGROUND] == nullptr) {
          tiles[i][t][LAYER_MIDGROUND]  = new Tile(TILE_SNOW, t_x, t_y, LAYER_MIDGROUND);
        }
      }
    }
  }
}
