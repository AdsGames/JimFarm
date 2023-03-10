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
  int pos_x = x / CHUNK_WIDTH_PX;
  int pos_y = y / CHUNK_HEIGHT_PX;

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

  if (!chunk) {
    return nullptr;
  }

  return chunk->getBiomeAt(x, y);
}

char TileMap::getTemperatureAt(int x, int y) {
  auto chunk = getChunkAt(x, y);

  if (!chunk) {
    return 0;
  }

  return chunk->getTemperatureAt(x, y);
}

// Get tile at position
std::shared_ptr<Tile> TileMap::getTileAt(int x, int y, int layer) {
  auto chunk = getChunkAt(x, y);

  if (!chunk) {
    return nullptr;
  }

  return chunk->getTileAt(x, y, layer);
}

// Place tile on map (world gen)
void TileMap::placeTile(std::shared_ptr<Tile> tile) {
  if (!tile) {
    return;
  }

  auto chunk = getChunkAt(tile->getX(), tile->getY());

  if (!chunk) {
    return;
  }

  chunk->setTileAt(tile->getX(), tile->getY(), tile->getZ(), tile);

  updateBitmaskSurround(tile->getX(), tile->getY(), tile->getZ());
}

// Remove tile from map
void TileMap::removeTile(std::shared_ptr<Tile> tile) {
  if (!tile) {
    return;
  }

  auto chunk = getChunkAt(tile->getX(), tile->getY());

  if (!chunk) {
    return;
  }

  auto old_x = tile->getX();
  auto old_y = tile->getY();
  auto old_z = tile->getZ();

  chunk->setTileAt(tile->getX(), tile->getY(), tile->getZ(), nullptr);

  updateBitmaskSurround(old_x, old_y, old_z);
}

// Replace tile on map
void TileMap::replaceTile(std::shared_ptr<Tile> tile_old,
                          std::shared_ptr<Tile> tile_new) {
  if (!tile_old) {
    return;
  }

  removeTile(tile_old);
  placeTile(tile_new);
  updateBitmaskSurround(tile_new->getX(), tile_new->getY(), tile_new->getZ());
}

// Check for solid tile
bool TileMap::isSolidAt(int x, int y) {
  auto tile = getTileAt(x, y, LAYER_FOREGROUND);
  return tile && tile->isSolid();
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
  if (!item) {
    return;
  }

  auto chunk = getChunkAt(x, y);

  if (!chunk) {
    return;
  }

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
void TileMap::tick(int x_1, int y_1, int x_2, int y_2) const {
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
  srand(time(nullptr));
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
void TileMap::updateBitMask(std::shared_ptr<Tile> tile) {
  if (!tile || !tile->needsBitmask()) {
    return;
  }

  unsigned char mask = 0;

  for (unsigned char i = 0; i < 4; i++) {
    int offset_x = static_cast<int>(sin(M_PI * (i / 2.0))) * 16;
    int offset_y = static_cast<int>(cos(M_PI * (i / 2.0))) * -16;

    std::shared_ptr<Tile> current = getTileAt(
        tile->getX() + offset_x, tile->getY() + offset_y, tile->getZ());

    if (current && current->getId() == tile->getId()) {
      mask += static_cast<unsigned char>(pow(2, i));
    }
  }

  tile->setMeta(mask);
}

// Update bitmask (and neighbours)
void TileMap::updateBitmaskSurround(int x, int y, int z) {
  updateBitMask(getTileAt(x, y, z));

  for (unsigned char i = 0; i < 4; i++) {
    int offset_x = static_cast<int>(sin(M_PI * (i / 2.0))) * 16;
    int offset_y = static_cast<int>(cos(M_PI * (i / 2.0))) * -16;

    std::shared_ptr<Tile> current = getTileAt(x + offset_x, y + offset_y, z);

    if (current) {
      updateBitMask(current);
    }
  }
}
