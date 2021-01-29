#include "TileMap.h"

#include <math.h>
#include <time.h>
#include <iostream>

#include "Graphics.h"
#include "Item.h"

#include "manager/tile_defs.h"
#include "utility/Tools.h"

TileMap::TileMap() {
  width = 1;
  height = 1;
}

TileMap::~TileMap() {
  // dtor
}

// Size
int TileMap::getWidth() {
  return width * CHUNK_WIDTH;
}

int TileMap::getHeight() {
  return height * CHUNK_HEIGHT;
}

// Chunk lookup
Chunk* TileMap::getChunkAt(int x, int y) {
  int pos_x = x / (CHUNK_WIDTH * TILE_WIDTH);
  int pos_y = y / (CHUNK_HEIGHT * TILE_HEIGHT);

  if (pos_y < 0 || pos_y >= (signed)chunks.size()) {
    return nullptr;
  }

  if (pos_x < 0 || pos_x >= (signed)chunks[pos_y].size()) {
    return nullptr;
  }

  return chunks.at(pos_y).at(pos_x);
}

std::string TileMap::getBiomeAt(int x, int y) {
  Chunk* chunk = getChunkAt(x, y);
  if (!chunk)
    return nullptr;
  return chunk->getBiomeAt(x, y);
}

char TileMap::getTemperatureAt(int x, int y) {
  Chunk* chunk = getChunkAt(x, y);
  if (!chunk)
    return 0;
  return chunk->getTemperatureAt(x, y);
}

// Get tile at position
Tile* TileMap::getTileAt(int x, int y, int layer) {
  Chunk* chunk = getChunkAt(x, y);
  if (!chunk)
    return nullptr;
  return chunk->getTileAt(x, y, layer);
}

// Place tile on map (world gen)
void TileMap::placeTile(Tile* newTile) {
  if (!newTile)
    return;

  Chunk* chunk = getChunkAt(newTile->getX(), newTile->getY());

  if (!chunk)
    return;

  chunk->setTileAt(newTile->getX(), newTile->getY(), newTile->getZ(), newTile);

  updateBitmaskSurround(newTile);
}

// Remove tile from map
void TileMap::removeTile(Tile* newTile) {
  if (!newTile)
    return;

  Chunk* chunk = getChunkAt(newTile->getX(), newTile->getY());

  if (!chunk)
    return;

  chunk->setTileAt(newTile->getX(), newTile->getY(), newTile->getZ(), nullptr);
}

// Replace tile on map
void TileMap::replaceTile(Tile* oldTile, Tile* newTile) {
  if (oldTile) {
    removeTile(oldTile);
    placeTile(newTile);
    updateBitmaskSurround(newTile);
  }
}

// Check for solid tile
bool TileMap::isSolidAt(int x, int y) {
  if (getTileAt(x, y, LAYER_FOREGROUND))
    return getTileAt(x, y, LAYER_FOREGROUND)->isSolid();
  return false;
}

// Get item at position
MapItem* TileMap::getItemAt(int x, int y) {
  Chunk* chunk = getChunkAt(x, y);

  if (!chunk)
    return nullptr;

  return chunk->getItemAt(x, y);
}

// Place item on map
void TileMap::placeItemAt(Item* item, int x, int y) {
  if (!item)
    return;

  Chunk* chunk = getChunkAt(x, y);

  if (!chunk)
    return;

  chunk->placeItemAt(item, x, y);
}

// Remove item from map
void TileMap::removeItem(MapItem* item) {
  if (!item)
    return;

  Chunk* chunk = getChunkAt(item->getX(), item->getY());

  if (!chunk)
    return;

  chunk->removeItem(item);
}

// Update chunks
void TileMap::tick(int x_1, int y_1, int x_2, int y_2) {
  for (auto const& chunk : chunks) {
    for (auto const& chunk2 : chunk) {
      if (chunk2->getInRange(x_1, y_1, x_2, y_2)) {
        chunk2->tick();
      }
    }
  }
}

// Generate map
void TileMap::generateMap() {
  // Base map
  width = 5;
  height = 5;

  // Generating chunk
  std::cout << "Generating World (" << width << "," << height << ")...  ";

  // Create some chunks
  srand(time(NULL));
  Chunk::seed = random(-10000, 10000);
  for (unsigned int t = 0; t < (unsigned)height; t++) {
    if (chunks.size() <= t) {
      std::vector<Chunk*> newVec;
      chunks.push_back(newVec);
    }

    for (int i = 0; i < width; i++) {
      chunks[t].push_back(new Chunk(i, t));
    }
  }

  // Generating chunk
  std::cout << "done." << std::endl;
  std::cout << "Updating bitmasks...  ";

  // Update masks
  for (int x = 0; x < width * CHUNK_WIDTH; x++) {
    for (int y = 0; y < height * CHUNK_HEIGHT; y++) {
      for (int z = 0; z < CHUNK_LAYERS; z++) {
        updateBitMask(getTileAt(x * TILE_WIDTH, y * TILE_HEIGHT, z));
      }
    }
  }

  std::cout << "done." << std::endl;
}

// Manually load new file
void TileMap::loadMap(std::string fileName) {}

// Clear map
void TileMap::clearMap() {
  for (auto const& chunk : chunks) {
    for (auto chunk2 : chunk) {
      delete chunk2;
    }
  }
  chunks.clear();
}

// Update bitmask
void TileMap::updateBitMask(Tile* newTile) {
  if (newTile && newTile->needsBitmask()) {
    int mask = 0;

    for (int i = 0; i < 4; i++) {
      int offset_x = sin(M_PI * (i / 2.0f)) * 16;
      int offset_y = cos(M_PI * (i / 2.0f)) * -16;
      Tile* current = getTileAt(newTile->getX() + offset_x,
                                newTile->getY() + offset_y, newTile->getZ());
      if (current && current->getID() == newTile->getID())
        mask += pow(2, i);
    }

    newTile->setMeta(mask);
  }
}

// Update bitmask (and neighbours)
void TileMap::updateBitmaskSurround(Tile* newTile) {
  if (newTile) {
    updateBitMask(newTile);
    for (int i = 0; i < 4; i++) {
      int offset_x = sin(M_PI * (i / 2.0f)) * 16;
      int offset_y = cos(M_PI * (i / 2.0f)) * -16;
      Tile* current = getTileAt(newTile->getX() + offset_x,
                                newTile->getY() + offset_y, newTile->getZ());
      if (current)
        updateBitMask(current);
    }
  }
}
