#include "TileMap.h"

#include <math.h>
#include <time.h>

#include "Graphics.h"
#include "Item.h"

#include "tile_defs.h"
#include "Tools.h"

TileMap::TileMap() {
  width = 1;
  height = 1;
}

TileMap::~TileMap() {
  //dtor
}

// Size
int TileMap::getWidth() {
  return width * CHUNK_WIDTH;
}

int TileMap::getHeight() {
  return height * CHUNK_HEIGHT;
}

// Chunk lookup
Chunk* TileMap::chunk_at(int x, int y) {
  for (unsigned int i = 0; i < chunks.size(); i++) {
    if (x >= (chunks.at(i) -> getX())     * CHUNK_WIDTH * TILE_WIDTH &&
        x <  (chunks.at(i) -> getX() + 1) * CHUNK_WIDTH * TILE_WIDTH &&
        y >= (chunks.at(i) -> getY())     * CHUNK_HEIGHT * TILE_HEIGHT &&
        y <  (chunks.at(i) -> getY() + 1) * CHUNK_HEIGHT * TILE_HEIGHT) {
      return chunks.at(i);
    }
  }
  return nullptr;
}

// Get tile at position
Tile* TileMap::tile_at(int x, int y, int layer) {
  Chunk* chunk = chunk_at(x, y);
  if (!chunk)
    return nullptr;
  return chunk -> get_tile_at(x, y, layer);
}

// Place tile on map (world gen)
void TileMap::place_tile(Tile* newTile) {
  if (!newTile)
    return;

  Chunk* chunk = chunk_at(newTile -> getX(), newTile -> getY());

  if (!chunk)
    return;

  return chunk -> set_tile_at(newTile -> getX(), newTile -> getY(), newTile -> getZ(), newTile);
}

// Remove tile from map
void TileMap::remove_tile(Tile* newTile) {
  if (!newTile)
    return;

  Chunk* chunk = chunk_at(newTile -> getX(), newTile -> getY());

  if (!chunk)
    return;

  chunk -> set_tile_at(newTile -> getX(), newTile -> getY(), newTile -> getZ(), nullptr);
}

// Replace tile on map
void TileMap::replace_tile(Tile *oldTile, Tile *newTile) {
  if (oldTile) {
    remove_tile (oldTile);
    place_tile (newTile);
    update_bitmask_surround(newTile);
  }
}

// Check for solid tile
bool TileMap::solid_at(int x, int y) {
  if (tile_at (x, y, LAYER_FOREGROUND))
    return tile_at (x, y, LAYER_FOREGROUND) -> isSolid();
  return false;
}


// Get item at position
MapItem *TileMap::item_at(int x, int y) {
  Chunk* chunk = chunk_at(x, y);

  if (!chunk)
    return nullptr;

  return chunk -> get_item_at(x, y);
}

// Place item on map
void TileMap::place_item(Item* item, int x, int y) {
  if (!item)
    return;

  Chunk* chunk = chunk_at(x, y);

  if (!chunk)
    return;

  chunk -> place_item_at(item, x, y);
}

// Remove item from map
void TileMap::remove_item(MapItem *item) {
  if (!item)
    return;

  Chunk* chunk = chunk_at(item -> getX(), item -> getY());

  if (!chunk)
    return;

  chunk -> remove_item(item);
}




// Update chunks
void TileMap::update(int x_1, int y_1, int x_2, int y_2) {
  for (unsigned int i = 0; i < chunks.size(); i++) {
    chunks.at(i) -> update(x_1, y_1, x_2, y_2);
  }
}

// Generate map
void TileMap::generate_map() {
  // Base map
  width = 10;
  height = 10;

  // Create some chunks
  srand(time(NULL));
  Chunk::seed = random(-10000, 10000);
  for (int i = 0; i < width; i++) {
    for (int t = 0; t < height; t++) {
      chunks.push_back(new Chunk(i, t));
    }
  }

  // Update masks
  for (int x = 0; x < width * CHUNK_WIDTH; x++) {
    for (int y = 0; y < height * CHUNK_HEIGHT; y++) {
      for (int z = 0; z < CHUNK_LAYERS; z++) {
        update_bitmask (tile_at(x * TILE_WIDTH, y * TILE_HEIGHT, z));
      }
    }
  }
}

// Manually load new file
void TileMap::load_map (std::string fileName) {

}

// Clear map
void TileMap::clear_map() {
  for (std::vector<Chunk*>::iterator i = chunks.begin(); i != chunks.end(); ++i) {
    delete *i;
  }
  chunks.clear();
}

// Update bitmask
void TileMap::update_bitmask (Tile *newTile) {
  if (newTile && newTile -> needsBitmask()) {
    int mask = 0;

    for (int i = 0; i < 4; i ++) {
      int offset_x = sin (M_PI *  (i / 2.0f)) *  16;
      int offset_y = cos (M_PI *  (i / 2.0f)) * -16;
      Tile *current = tile_at (newTile -> getX() + offset_x, newTile -> getY() + offset_y, newTile -> getZ());
      if (current && current -> getID() == newTile -> getID())
        mask += pow (2, i);
    }

    newTile -> setMeta (mask);
  }
}

// Update bitmask (and neighbours)
void TileMap::update_bitmask_surround (Tile *newTile) {
  if (newTile) {
    update_bitmask (newTile);
    for (int i = 0; i < 4; i ++) {
      int offset_x = sin (M_PI *  (i / 2.0f)) *  16;
      int offset_y = cos (M_PI *  (i / 2.0f)) * -16;
      Tile *current = tile_at (newTile -> getX() + offset_x, newTile -> getY() + offset_y, newTile -> getZ());
      if (current)
        update_bitmask (current);
    }
  }
}
