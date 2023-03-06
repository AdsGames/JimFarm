#include "TileMap.h"

#include <math.h>
#include <time.h>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "Graphics.h"
#include "Item.h"

#include "manager/tile_defs.h"
#include "utility/Tools.h"

// Size
int TileMap::getWidth() const {
  return width * CHUNK_WIDTH;
}

int TileMap::getHeight() const {
  return height * CHUNK_HEIGHT;
}

// Chunk lookup
std::shared_ptr<Chunk> TileMap::getChunkAt(int x, int y) {
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
  auto chunk = getChunkAt(x, y);
  if (!chunk)
    return nullptr;
  return chunk->getBiomeAt(x, y);
}

char TileMap::getTemperatureAt(int x, int y) {
  auto chunk = getChunkAt(x, y);
  if (!chunk)
    return 0;
  return chunk->getTemperatureAt(x, y);
}

// Get tile at position
std::shared_ptr<Tile> TileMap::getTileAt(int x, int y, int layer) {
  auto chunk = getChunkAt(x, y);
  if (!chunk)
    return nullptr;
  return chunk->getTileAt(x, y, layer);
}

// Place tile on map (world gen)
void TileMap::placeTile(std::shared_ptr<Tile> newTile) {
  if (!newTile) {
    return;
  }

  auto chunk = getChunkAt(newTile->getX(), newTile->getY());

  if (!chunk) {
    return;
  }

  chunk->setTileAt(newTile->getX(), newTile->getY(), newTile->getZ(), newTile);

  updateBitmaskSurround(newTile);
}

// Remove tile from map
void TileMap::removeTile(std::shared_ptr<Tile> newTile) {
  if (!newTile)
    return;

  auto chunk = getChunkAt(newTile->getX(), newTile->getY());

  if (!chunk)
    return;

  chunk->setTileAt(newTile->getX(), newTile->getY(), newTile->getZ(), nullptr);
}

// Replace tile on map
void TileMap::replaceTile(std::shared_ptr<Tile> oldTile,
                          std::shared_ptr<Tile> newTile) {
  if (oldTile) {
    removeTile(oldTile);
    placeTile(newTile);
    updateBitmaskSurround(newTile);
  }
}

// Check for solid tile
bool TileMap::isSolidAt(int x, int y) {
  if (getTileAt(x, y, LAYER_FOREGROUND)) {
    return getTileAt(x, y, LAYER_FOREGROUND)->isSolid();
  }
  return false;
}

// Get item at position
std::shared_ptr<MapItem> TileMap::getItemAt(int x, int y) {
  auto chunk = getChunkAt(x, y);

  if (!chunk) {
    return nullptr;
  }

  return chunk->getItemAt(x, y);
}

// Place item on map
void TileMap::placeItemAt(std::shared_ptr<Item> item, int x, int y) {
  if (!item)
    return;

  auto chunk = getChunkAt(x, y);

  if (!chunk)
    return;

  chunk->placeItemAt(item, x, y);
}

// Remove item from map
void TileMap::removeItem(std::shared_ptr<MapItem> item) {
  if (!item) {
    return;
  }

  auto chunk = getChunkAt(item->getX(), item->getY());

  if (!chunk) {
    return;
  }

  chunk->removeItem(item);
}

// Update chunks
void TileMap::tick(int x_1, int y_1, int x_2, int y_2) {
  Graphics::Instance()->disableSort();
  for (auto const& chunk : chunks) {
    for (auto const& chunk2 : chunk) {
      if (chunk2->getInRange(x_1, y_1, x_2, y_2)) {
        chunk2->tick();
      }
    }
  }
  Graphics::Instance()->enableSort();
}

// Generate map
void TileMap::generateMap() {
  // Base map
  width = 10;
  height = 10;

  // Generating chunk
  std::cout << "Generating World (" << width << "," << height << ")...  ";

  // Create some chunks
  srand(time(NULL));
  Chunk::seed = random(-10000, 10000);
  for (unsigned int t = 0; t < (unsigned)height; t++) {
    if (chunks.size() <= t) {
      std::vector<std::shared_ptr<Chunk>> newVec;
      chunks.push_back(newVec);
    }

    for (int i = 0; i < width; i++) {
      chunks[t].push_back(std::make_shared<Chunk>(i, t));
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

// Clear map
void TileMap::clearMap() {
  chunks.clear();
}

// Update bitmask
void TileMap::updateBitMask(std::shared_ptr<Tile> newTile) {
  if (newTile && newTile->needsBitmask()) {
    int mask = 0;

    for (int i = 0; i < 4; i++) {
      int offset_x = sin(M_PI * (i / 2.0f)) * 16;
      int offset_y = cos(M_PI * (i / 2.0f)) * -16;
      std::shared_ptr<Tile> current =
          getTileAt(newTile->getX() + offset_x, newTile->getY() + offset_y,
                    newTile->getZ());
      if (current && current->getID() == newTile->getID())
        mask += pow(2, i);
    }

    newTile->setMeta(mask);
  }
}

// Update bitmask (and neighbours)
void TileMap::updateBitmaskSurround(std::shared_ptr<Tile> newTile) {
  if (newTile) {
    updateBitMask(newTile);
    for (int i = 0; i < 4; i++) {
      int offset_x = sin(M_PI * (i / 2.0f)) * 16;
      int offset_y = cos(M_PI * (i / 2.0f)) * -16;
      std::shared_ptr<Tile> current =
          getTileAt(newTile->getX() + offset_x, newTile->getY() + offset_y,
                    newTile->getZ());
      if (current) {
        updateBitMask(current);
      }
    }
  }
}
