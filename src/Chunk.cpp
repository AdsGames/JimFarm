#include "Chunk.h"

#include <sstream>

#include "manager/item_defs.h"
#include "manager/tile_defs.h"

#include "utility/Tools.h"

#include "Graphics.h"

#include "SimplexNoise.h"

int Chunk::seed = 0;

Chunk::Chunk(int x, int y) : x(x), y(y) {
  generate();
}

Chunk::~Chunk() {
  items.clear();

  for (auto const& tile : tiles) {
    Graphics::Instance()->remove(tile);
  }
}

int Chunk::getX() const {
  return x;
}

int Chunk::getY() const {
  return y;
}

std::string Chunk::getBiomeAt(int at_x, int at_y) const {
  int offset_x = at_x / TILE_WIDTH - this->x * CHUNK_WIDTH;
  int offset_y = at_y / TILE_HEIGHT - this->y * CHUNK_HEIGHT;

  if (offset_x < 0 || offset_x > CHUNK_WIDTH || offset_y < 0 ||
      offset_y > CHUNK_HEIGHT) {
    return "unknown";
  }

  int offset = offset_x + offset_y * CHUNK_WIDTH;

  std::stringstream stream;
  stream << "temp:" << temperature[offset] << " "
         << "rain:" << rainfall[offset] << " "
         << "height:" << height[offset];

  return stream.str();
}

char Chunk::getTemperatureAt(int x, int y) const {
  int offset_x = x / TILE_WIDTH - this->x * CHUNK_WIDTH;
  int offset_y = y / TILE_HEIGHT - this->y * CHUNK_HEIGHT;

  if (offset_x < 0 || offset_x > CHUNK_WIDTH || offset_y < 0 ||
      offset_y > CHUNK_HEIGHT) {
    return 0;
  }

  int offset = offset_x + offset_y * CHUNK_WIDTH;

  return temperature[offset];
}

std::shared_ptr<Tile> Chunk::getTileAt(int x, int y, int z) const {
  int offset_x = x / TILE_WIDTH - this->x * CHUNK_WIDTH;
  int offset_y = y / TILE_HEIGHT - this->y * CHUNK_HEIGHT;

  if (offset_x < 0 || offset_x > CHUNK_WIDTH || offset_y < 0 ||
      offset_y > CHUNK_HEIGHT || z < 0 || z > CHUNK_LAYERS) {
    return nullptr;
  }

  int offset =
      offset_x + offset_y * CHUNK_WIDTH + z * CHUNK_WIDTH * CHUNK_HEIGHT;

  return tiles[offset];
}

void Chunk::setTileAt(int tile_x,
                      int tile_y,
                      int tile_z,
                      std::shared_ptr<Tile> tile) {
  int offset_x = tile_x / TILE_WIDTH - this->x * CHUNK_WIDTH;
  int offset_y = tile_y / TILE_HEIGHT - this->y * CHUNK_HEIGHT;

  if (offset_x < 0 || offset_x > CHUNK_WIDTH || offset_y < 0 ||
      offset_y > CHUNK_HEIGHT || tile_z < 0 || tile_z > CHUNK_LAYERS) {
    return;
  }

  int offset =
      offset_x + offset_y * CHUNK_WIDTH + tile_z * CHUNK_WIDTH * CHUNK_HEIGHT;

  if (tiles[offset]) {
    Graphics::Instance()->remove(tiles[offset]);
    tiles[offset].reset();
  }

  tiles[offset] = tile;

  if (tile && is_drawing) {
    Graphics::Instance()->add(tiles[offset]);
  }
}

// Get item at position
std::shared_ptr<MapItem> Chunk::getItemAt(int item_x, int item_y) const {
  for (auto const& i : items) {
    if (i->getX() == item_x && i->getY() == item_y) {
      return i;
    }
  }
  return nullptr;
}

// Place item on map
void Chunk::placeItemAt(std::shared_ptr<Item> item, int item_x, int item_y) {
  if (item) {
    auto newMapItem = std::make_shared<MapItem>(item_x, item_y, item);
    items.push_back(newMapItem);

    if (is_drawing) {
      Graphics::Instance()->add(newMapItem);
    }
  }
}

// Remove item from map
void Chunk::removeItem(std::shared_ptr<MapItem> item) {
  if (item != nullptr) {
    for (unsigned int i = 0; i < items.size(); i++) {
      if (items.at(i) == item) {
        Graphics::Instance()->remove(items.at(i));
        items.erase(items.begin() + i);
        break;
      }
    }
  }
}

void Chunk::setDrawEnabled(bool enabled) {
  if (is_drawing != enabled) {
    for (auto const& tile : tiles) {
      if (tile && enabled) {
        Graphics::Instance()->add(tile);
      } else if (tile && !enabled) {
        Graphics::Instance()->remove(tile);
      }
    }

    for (auto const& item : items) {
      if (enabled) {
        Graphics::Instance()->add(item);
      } else {
        Graphics::Instance()->remove(item);
      }
    }

    is_drawing = enabled;
  }
}

bool Chunk::getInRange(int x_1, int y_1, int x_2, int y_2) {
  const int in_range = x_2 >= (this->x) * CHUNK_WIDTH * TILE_WIDTH &&
                       x_1 <= (this->x + 1) * CHUNK_WIDTH * TILE_WIDTH &&
                       y_2 >= (this->y) * CHUNK_HEIGHT * TILE_HEIGHT &&
                       y_1 <= (this->y + 1) * CHUNK_HEIGHT * TILE_HEIGHT;

  // Add to draw pool
  if (in_range && !is_drawing) {
    setDrawEnabled(true);
  }

  // Remove from draw pool
  if (!in_range && is_drawing) {
    setDrawEnabled(false);
  }

  return in_range;
}

void Chunk::tick() {
  // Tiles
  for (int i = 0; i < CHUNK_WIDTH; i++) {
    for (int t = 0; t < CHUNK_HEIGHT; t++) {
      int offset =
          i + t * CHUNK_WIDTH + LAYER_FOREGROUND * CHUNK_WIDTH * CHUNK_HEIGHT;

      // Current tile
      auto& current = tiles[offset];

      if (!current) {
        continue;
      }

      // Berries
      if (current->getId() == TILE_BERRY) {
        // Grow a bit
        if (true) {
          current->changeMeta(1);
        }

        // Done Growing
        if (current->getMeta() >= MAX_TILE_META) {
          placeItemAt(std::make_shared<Item>(ITEM_BERRY), current->getX(),
                      current->getY());
          setTileAt(current->getX(), current->getY(), current->getZ(), nullptr);
        }
      }
      // Tomatos
      else if (current->getId() == TILE_TOMATO) {
        // Grow a bit
        if (!random(0, 2)) {
          current->changeMeta(1);
        }

        // Done Growing
        if (current->getMeta() >= MAX_TILE_META) {
          placeItemAt(std::make_shared<Item>(ITEM_TOMATO), current->getX(),
                      current->getY());
          setTileAt(current->getX(), current->getY(), current->getZ(), nullptr);
        }
      }
      // Carrots
      else if (current->getId() == TILE_CARROT) {
        // Grow a bit
        if (!random(0, 5)) {
          current->changeMeta(1);
        }

        // Done Growing
        if (current->getMeta() >= MAX_TILE_META) {
          placeItemAt(std::make_shared<Item>(ITEM_CARROT), current->getX(),
                      current->getY());
          setTileAt(current->getX(), current->getY(), current->getZ(), nullptr);
        }
      }
      // Lavender
      else if (current->getId() == TILE_LAVENDER) {
        // Grow a bit
        if (!random(0, 10))
          current->changeMeta(1);

        // Done Growing
        if (current->getMeta() >= MAX_TILE_META) {
          placeItemAt(std::make_shared<Item>(ITEM_LAVENDER), current->getX(),
                      current->getY());
          setTileAt(current->getX(), current->getY(), current->getZ(), nullptr);
        }
      }
    }
  }
}

void Chunk::generate() {
  // Height
  auto sn_h = std::make_unique<SimplexNoise>(1.0f, 1.0f, 2.0f, 0.47f);

  // Rainfall
  auto sn_r = std::make_unique<SimplexNoise>(0.3f, 0.3f, 1.0f, 2.0f);

  // Temperature
  auto sn_t = std::make_unique<SimplexNoise>(0.3f, 0.3f, 1.0f, 2.0f);

  for (int i = 0; i < CHUNK_WIDTH; i++) {
    for (int t = 0; t < CHUNK_HEIGHT; t++) {
      const int t_x = (i + x * CHUNK_WIDTH) * TILE_WIDTH;
      const int t_y = (t + y * CHUNK_HEIGHT) * TILE_HEIGHT;
      const int pos_2 = (i + t * CHUNK_WIDTH);
      const int pos_3_background =
          pos_2 + LAYER_BACKGROUND * CHUNK_WIDTH * CHUNK_HEIGHT;
      const int pos_3_foreground =
          pos_2 + LAYER_FOREGROUND * CHUNK_WIDTH * CHUNK_HEIGHT;
      const int pos_3_midground =
          pos_2 + LAYER_MIDGROUND * CHUNK_WIDTH * CHUNK_HEIGHT;

      this->temperature[pos_2] =
          sn_t->fractal(10,
                        Chunk::seed % 897 +
                            static_cast<float>(i + x * CHUNK_WIDTH) / 100.0f,
                        Chunk::seed % 897 +
                            static_cast<float>(t + y * CHUNK_HEIGHT) / 100.0f) *
          64;
      this->rainfall[pos_2] =
          sn_r->fractal(10,
                        Chunk::seed % 477 +
                            static_cast<float>(i + x * CHUNK_WIDTH) / 100.0f,
                        Chunk::seed % 477 +
                            static_cast<float>(t + y * CHUNK_HEIGHT) / 100.0f) *
          64;
      this->height[pos_2] =
          sn_h->fractal(
              10,
              Chunk::seed + static_cast<float>(i + x * CHUNK_WIDTH) / 100.0f,
              Chunk::seed + static_cast<float>(t + y * CHUNK_HEIGHT) / 100.0f) *
          64;

      // High rainfall
      // Tundra
      if (temperature[pos_2] < -32 && rainfall[pos_2] >= 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[pos_3_midground] =
            std::make_shared<Tile>(TILE_SNOW, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 30) == 0) {
          tiles[pos_3_foreground] =
              std::make_shared<Tile>(TILE_TREE, t_x, t_y, LAYER_FOREGROUND, 2);
        }
      }
      // Forested tundra
      else if (temperature[pos_2] < 0 && rainfall[pos_2] >= 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[pos_3_midground] =
            std::make_shared<Tile>(TILE_SNOW, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 3) == 0) {
          tiles[pos_3_foreground] = std::make_shared<Tile>(
              TILE_TREE, t_x, t_y, LAYER_FOREGROUND, random(1, 2));
        }
      }
      // Forest
      else if (temperature[pos_2] < 24 && rainfall[pos_2] >= 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[pos_3_midground] =
            std::make_shared<Tile>(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 2) == 0) {
          tiles[pos_3_foreground] = std::make_shared<Tile>(
              TILE_TREE, t_x, t_y, LAYER_FOREGROUND, random(0, 1));
        }
      }
      // Dense forest
      else if (temperature[pos_2] <= 64 && rainfall[pos_2] >= 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[pos_3_midground] =
            std::make_shared<Tile>(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 4) != 0) {
          tiles[pos_3_foreground] =
              std::make_shared<Tile>(TILE_TREE, t_x, t_y, LAYER_FOREGROUND, 3);
        }
      }

      // Low rainfall
      // Wasteland
      else if (temperature[pos_2] < -32 && rainfall[pos_2] < 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
      }
      // Wasteland
      else if (temperature[pos_2] < -8 && rainfall[pos_2] < 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[pos_3_midground] =
            std::make_shared<Tile>(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
      }
      // Grassy wasteland
      else if (temperature[pos_2] < 0 && rainfall[pos_2] < 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[pos_3_midground] =
            std::make_shared<Tile>(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 8) == 0) {
          tiles[pos_3_foreground] = std::make_shared<Tile>(
              TILE_DENSE_GRASS, t_x, t_y, LAYER_FOREGROUND, random(0, 3));
        }
      }
      // Grassy wasteland
      else if (temperature[pos_2] < 24 && rainfall[pos_2] < 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[pos_3_midground] =
            std::make_shared<Tile>(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 1) == 0) {
          tiles[pos_3_foreground] = std::make_shared<Tile>(
              TILE_DENSE_GRASS, t_x, t_y, LAYER_FOREGROUND, random(0, 3));
          if (random(0, 100) == 0) {
            placeItemAt(std::make_shared<Item>(ITEM_CHICKEN), t_x, t_y);
          }
        }
      }
      // Savana
      else if (temperature[pos_2] < 32 && rainfall[pos_2] < 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[pos_3_midground] =
            std::make_shared<Tile>(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 5) == 0) {
          tiles[pos_3_foreground] = std::make_shared<Tile>(
              TILE_DENSE_GRASS, t_x, t_y, LAYER_FOREGROUND, 1);
        } else if (random(0, 10) == 0) {
          tiles[pos_3_foreground] =
              std::make_shared<Tile>(TILE_BUSH, t_x, t_y, LAYER_FOREGROUND, 1);
        } else if (random(0, 50) == 0) {
          tiles[pos_3_foreground] =
              std::make_shared<Tile>(TILE_BARN, t_x, t_y, LAYER_FOREGROUND, 1);
        }
      }
      // Desert
      else if (temperature[pos_2] <= 64 && rainfall[pos_2] < 0) {
        tiles[pos_3_background] =
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[pos_3_midground] =
            std::make_shared<Tile>(TILE_SAND, t_x, t_y, LAYER_MIDGROUND);
      }

      // Water deep
      if (height[pos_2] < -32) {
        setTileAt(t_x, t_y, LAYER_BACKGROUND,
                  std::make_shared<Tile>(TILE_UNDERWATER_SOIL, t_x, t_y,
                                         LAYER_BACKGROUND, 3));
        setTileAt(t_x, t_y, LAYER_MIDGROUND, nullptr);
        setTileAt(
            t_x, t_y, LAYER_FOREGROUND,
            std::make_shared<Tile>(TILE_WATER, t_x, t_y, LAYER_FOREGROUND));
      }
      // Water
      else if (height[pos_2] < -19) {
        setTileAt(t_x, t_y, LAYER_BACKGROUND,
                  std::make_shared<Tile>(TILE_UNDERWATER_SOIL, t_x, t_y,
                                         LAYER_BACKGROUND, 0));
        setTileAt(t_x, t_y, LAYER_MIDGROUND, nullptr);
        setTileAt(
            t_x, t_y, LAYER_FOREGROUND,
            std::make_shared<Tile>(TILE_WATER, t_x, t_y, LAYER_FOREGROUND));
      }
      // Water seaweed
      else if (height[pos_2] < -17) {
        setTileAt(t_x, t_y, LAYER_BACKGROUND,
                  std::make_shared<Tile>(TILE_UNDERWATER_SOIL, t_x, t_y,
                                         LAYER_BACKGROUND, 1));
        setTileAt(t_x, t_y, LAYER_MIDGROUND, nullptr);
        setTileAt(
            t_x, t_y, LAYER_FOREGROUND,
            std::make_shared<Tile>(TILE_WATER, t_x, t_y, LAYER_FOREGROUND));
      }
      // Shore
      else if (height[pos_2] < -14) {
        setTileAt(
            t_x, t_y, LAYER_BACKGROUND,
            std::make_shared<Tile>(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND));
        setTileAt(t_x, t_y, LAYER_FOREGROUND,
                  std::make_shared<Tile>(TILE_DENSE_GRASS, t_x, t_y,
                                         LAYER_FOREGROUND));
      }
      // Stone
      else if (height[pos_2] > 32) {
        setTileAt(
            t_x, t_y, LAYER_BACKGROUND,
            std::make_shared<Tile>(TILE_STONE, t_x, t_y, LAYER_BACKGROUND));
        setTileAt(
            t_x, t_y, LAYER_MIDGROUND,
            std::make_shared<Tile>(TILE_STONE, t_x, t_y, LAYER_MIDGROUND));
        setTileAt(
            t_x, t_y, LAYER_FOREGROUND,
            std::make_shared<Tile>(TILE_STONE_WALL, t_x, t_y, LAYER_FOREGROUND,
                                   (height[pos_2] - 32) / 6));
      }
    }
  }
}
