#include "TileMap.h"

#include <exception>
#include <iostream>

#include "Graphics.h"
#include "Item.h"
#include "utility/Tools.h"

const std::array<std::pair<int, int>, 4> TileMap::BITMASK_DIRECTIONS = {
    {{0, -1}, {1, 0}, {0, 1}, {-1, 0}}};

// Size
int TileMap::getWidth() const {
  return size.x * CHUNK_SIZE;
}

int TileMap::getHeight() const {
  return size.y * CHUNK_SIZE;
}

// Chunk lookup
std::shared_ptr<Chunk> TileMap::getChunkAt(Vec2<int> pos) {
  auto offset_x = pos.x / CHUNK_SIZE;
  auto offset_y = pos.y / CHUNK_SIZE;

  if (offset_y >= chunks.size()) {
    return nullptr;
  }

  if (offset_x >= chunks[offset_y].size()) {
    return nullptr;
  }

  return chunks.at(offset_y).at(offset_x);
}

std::string TileMap::getBiomeAt(Vec2<int> pos) {
  auto chunk = getChunkAt(pos);

  if (!chunk) {
    return "none";
  }
  return chunk->getBiomeAt(pos % CHUNK_SIZE);
}

char TileMap::getTemperatureAt(Vec2<int> pos) {
  auto chunk = getChunkAt(pos);

  if (!chunk) {
    return 0;
  }

  return chunk->getTemperatureAt(pos % CHUNK_SIZE);
}

// Get tile at position
std::shared_ptr<Tile> TileMap::getTileAt(Vec2<int> pos, int layer) {
  auto chunk = getChunkAt(pos);

  if (!chunk) {
    return nullptr;
  }

  return chunk->getTileAt(pos % CHUNK_SIZE, layer);
}

// Place tile on map (world gen)
void TileMap::placeTile(std::shared_ptr<Tile> tile) {
  if (!tile) {
    throw std::runtime_error("Can not place tile, tile is null");
  }

  auto chunk = getChunkAt(tile->getTilePosition());

  if (!chunk) {
    throw std::runtime_error("Can not place tile, chunk is null");
  }

  chunk->setTileAt(tile->getTilePosition() % CHUNK_SIZE, tile->getZ(), tile);

  updateBitmaskSurround(tile->getTilePosition(), tile->getZ());
}

// Remove tile from map
void TileMap::removeTile(std::shared_ptr<Tile> tile) {
  if (!tile) {
    throw std::runtime_error("Can not remove tile, tile is null");
  }

  auto chunk = getChunkAt(tile->getTilePosition());

  if (!chunk) {
    throw std::runtime_error("Can not place tile, chunk is null");
  }

  auto old_pos = tile->getTilePosition();
  auto old_z = tile->getZ();

  chunk->setTileAt(tile->getTilePosition() % CHUNK_SIZE, tile->getZ(), nullptr);

  updateBitmaskSurround(old_pos, old_z);
}

// Check for solid tile
bool TileMap::isSolidAt(Vec2<int> pos) {
  auto tile = getTileAt(pos, LAYER_FOREGROUND);
  return tile && tile->getType().getAttribute();
}

// Get item at position
std::shared_ptr<MapItem> TileMap::getItemAt(Vec2<int> pos) {
  auto chunk = getChunkAt(pos);

  if (!chunk) {
    throw std::runtime_error("Can not place tile, chunk is null");
  }

  return chunk->getItemAt(pos);
}

// Place item on map
void TileMap::placeItemAt(std::shared_ptr<Item> item, Vec2<int> pos) {
  if (!item) {
    throw std::runtime_error("Can not place item, item is null");
  }

  auto chunk = getChunkAt(pos);

  if (!chunk) {
    throw std::runtime_error("Can not place tile, chunk is null");
  }

  chunk->placeItemAt(item, pos);
}

// Remove item from map
void TileMap::removeItem(std::shared_ptr<MapItem> item) {
  if (!item) {
    throw std::runtime_error("Can not remove item, item is null");
  }

  auto chunk =
      getChunkAt(item->getPosition() / Vec2<int>(TILE_SIZE, TILE_SIZE));

  if (!chunk) {
    throw std::runtime_error("Can not place tile, chunk is null");
  }

  chunk->removeItem(item);
}

// Update chunks
void TileMap::tick(const Camera& camera) const {
  const auto& bounds = camera.getBounds();

  for (auto const& y_chunks : chunks) {
    for (auto const& chunk : y_chunks) {
      if (static_cast<unsigned int>(bounds.x_2) >=
              (chunk->getXIndex()) * CHUNK_SIZE * TILE_SIZE &&
          static_cast<unsigned int>(bounds.x_1) <=
              (chunk->getXIndex() + 1) * CHUNK_SIZE * TILE_SIZE &&
          static_cast<unsigned int>(bounds.y_2) >=
              (chunk->getYIndex()) * CHUNK_SIZE * TILE_SIZE &&
          static_cast<unsigned int>(bounds.y_1) <=
              (chunk->getYIndex() + 1) * CHUNK_SIZE * TILE_SIZE) {
        chunk->setDrawEnabled(true);
        chunk->tick();
      } else {
        chunk->setDrawEnabled(false);
      }
    }
  }
}

// Generate map
void TileMap::generateMap(Vec2<unsigned int> size) {
  this->size = size;

  // Generating chunk
  std::cout << "Generating World (" << size.x << "," << size.y << ")...  ";

  // Create some chunks
  Chunk::seed = random(-10000, 10000);

  for (unsigned int t = 0; t < size.y; t++) {
    if (chunks.size() <= t) {
      chunks.emplace_back();
    }

    for (unsigned int i = 0; i < size.x; i++) {
      chunks[t].push_back(std::make_shared<Chunk>(i, t));
    }
  }

  // Generating chunk
  std::cout << "done." << std::endl;
  std::cout << "Updating bitmasks...  ";

  // Update masks
  for (unsigned int x = 0; x < size.x * CHUNK_SIZE; x++) {
    for (unsigned int y = 0; y < size.y * CHUNK_SIZE; y++) {
      for (unsigned int z = 0; z < CHUNK_LAYERS; z++) {
        updateBitMask(getTileAt(Vec2<int>(x, y), z));
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
    const auto& [first, second] = TileMap::BITMASK_DIRECTIONS[i];

    auto current = getTileAt(tile->getTilePosition() + Vec2<int>(first, second),
                             tile->getZ());

    if (current && current->getType().getId() == tile->getType().getId()) {
      mask += static_cast<unsigned char>(pow(2, i));
    }
  }

  tile->setMeta(mask);
}

// Update bitmask (and neighbours)
void TileMap::updateBitmaskSurround(Vec2<int> pos, int z) {
  updateBitMask(getTileAt(pos + Vec2<int>(0, 0), z));
  updateBitMask(getTileAt(pos + Vec2<int>(0, -1), z));
  updateBitMask(getTileAt(pos + Vec2<int>(0, 1), z));
  updateBitMask(getTileAt(pos + Vec2<int>(-1, 0), z));
  updateBitMask(getTileAt(pos + Vec2<int>(1, 0), z));
}
