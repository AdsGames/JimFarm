#include "Chunk.h"

#include <sstream>

#include "manager/item_defs.h"
#include "manager/tile_defs.h"

#include "utility/Tools.h"

#include "Graphics.h"

#include "SimplexNoise.h"

int Chunk::seed = 0;

Chunk::Chunk(int x, int y) {
  this->x = x;
  this->y = y;

  this->is_drawing = false;

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
  setDrawEnabled(false);

  for (int i = 0; i < CHUNK_WIDTH; i++) {
    for (int t = 0; t < CHUNK_HEIGHT; t++) {
      for (int j = 0; j < CHUNK_LAYERS; j++) {
        if (tiles[i][t][j]) {
          delete tiles[i][t][j];
        }
      }
    }
  }

  for (auto const& i : items) {
    delete i;
  }

  items.clear();
}

int Chunk::getX() {
  return x;
}

int Chunk::getY() {
  return y;
}

std::string Chunk::getBiomeAt(int x, int y) {
  int offset_x = x / TILE_WIDTH - this->x * CHUNK_WIDTH;
  int offset_y = y / TILE_HEIGHT - this->y * CHUNK_HEIGHT;

  if (offset_x < 0 || offset_x > CHUNK_WIDTH || offset_y < 0 ||
      offset_y > CHUNK_HEIGHT) {
    return "unknown";
  }

  std::stringstream stream;
  stream << "temp:" << static_cast<int>(temperature[offset_x][offset_y])
         << " rain:" << static_cast<int>((rainfall[offset_x][offset_y]) + 64)
         << " height:" << static_cast<int>(height[offset_x][offset_y]);

  return stream.str();
}

char Chunk::getTemperatureAt(int x, int y) {
  int offset_x = x / TILE_WIDTH - this->x * CHUNK_WIDTH;
  int offset_y = y / TILE_HEIGHT - this->y * CHUNK_HEIGHT;

  if (offset_x < 0 || offset_x > CHUNK_WIDTH || offset_y < 0 ||
      offset_y > CHUNK_HEIGHT) {
    return 0;
  }

  return temperature[offset_x][offset_y];
}

Tile* Chunk::getTileAt(int x, int y, int z) {
  int offset_x = x / TILE_WIDTH - this->x * CHUNK_WIDTH;
  int offset_y = y / TILE_HEIGHT - this->y * CHUNK_HEIGHT;

  if (offset_x < 0 || offset_x > CHUNK_WIDTH || offset_y < 0 ||
      offset_y > CHUNK_HEIGHT || z < 0 || z > CHUNK_LAYERS) {
    return nullptr;
  }

  return tiles[offset_x][offset_y][z];
}

void Chunk::setTileAt(int x, int y, int z, Tile* tile) {
  int offset_x = x / TILE_WIDTH - this->x * CHUNK_WIDTH;
  int offset_y = y / TILE_HEIGHT - this->y * CHUNK_HEIGHT;

  if (offset_x < 0 || offset_x > CHUNK_WIDTH || offset_y < 0 ||
      offset_y > CHUNK_HEIGHT || z < 0 || z > CHUNK_LAYERS) {
    return;
  }

  if (tiles[offset_x][offset_y][z]) {
    Graphics::Instance()->remove(tiles[offset_x][offset_y][z]);
    delete tiles[offset_x][offset_y][z];
  }

  tiles[offset_x][offset_y][z] = tile;

  if (tile && is_drawing) {
    Graphics::Instance()->add(tiles[offset_x][offset_y][z]);
  }
}

// Get item at position
MapItem* Chunk::getItemAt(int x, int y) {
  for (auto const& i : items) {
    if (i->x == x && i->y == y) {
      return i;
    }
  }
  return nullptr;
}

// Place item on map
void Chunk::placeItemAt(Item* item, int x, int y) {
  if (item) {
    MapItem* newMapItem = new MapItem(x, y, item);
    items.push_back(newMapItem);

    if (is_drawing)
      Graphics::Instance()->add(newMapItem);
  }
}

// Remove item from map
void Chunk::removeItem(MapItem* item) {
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
    for (int i = 0; i < CHUNK_WIDTH; i++) {
      for (int t = 0; t < CHUNK_HEIGHT; t++) {
        for (int j = 0; j < CHUNK_LAYERS; j++) {
          if (tiles[i][t][j] && enabled) {
            Graphics::Instance()->add(tiles[i][t][j]);
          } else if (tiles[i][t][j] && !enabled) {
            Graphics::Instance()->remove(tiles[i][t][j]);
          }
        }
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
      // Current tile
      Tile* current = tiles[i][t][LAYER_FOREGROUND];

      if (!current)
        continue;

      // Berries
      if (current->getID() == TILE_BERRY) {
        // Grow a bit
        if (true)
          current->changeMeta(1);

        // Done Growing
        if (current->getMeta() >= MAX_TILE_META) {
          placeItemAt(new Item(ITEM_BERRY), current->getX(), current->getY());
          setTileAt(current->getX(), current->getY(), current->getZ(), nullptr);
        }
      }
      // Tomatos
      else if (current->getID() == TILE_TOMATO) {
        // Grow a bit
        if (!random(0, 2))
          current->changeMeta(1);

        // Done Growing
        if (current->getMeta() >= MAX_TILE_META) {
          placeItemAt(new Item(ITEM_TOMATO), current->getX(), current->getY());
          setTileAt(current->getX(), current->getY(), current->getZ(), nullptr);
        }
      }
      // Carrots
      else if (current->getID() == TILE_CARROT) {
        // Grow a bit
        if (!random(0, 5))
          current->changeMeta(1);

        // Done Growing
        if (current->getMeta() >= MAX_TILE_META) {
          placeItemAt(new Item(ITEM_CARROT), current->getX(), current->getY());
          setTileAt(current->getX(), current->getY(), current->getZ(), nullptr);
        }
      }
      // Lavender
      else if (current->getID() == TILE_LAVENDER) {
        // Grow a bit
        if (!random(0, 10))
          current->changeMeta(1);

        // Done Growing
        if (current->getMeta() >= MAX_TILE_META) {
          placeItemAt(new Item(ITEM_LAVENDER), current->getX(),
                      current->getY());
          setTileAt(current->getX(), current->getY(), current->getZ(), nullptr);
        }
      }
    }
  }
}

void Chunk::generate() {
  // Height
  SimplexNoise* sn_h = new SimplexNoise(1.0f, 1.0f, 2.0f, 0.47f);

  // Rainfall
  SimplexNoise* sn_r = new SimplexNoise(0.3f, 0.3f, 1.0f, 2.0f);

  // Temperature
  SimplexNoise* sn_t = new SimplexNoise(0.3f, 0.3f, 1.0f, 2.0f);

  for (int i = 0; i < CHUNK_WIDTH; i++) {
    for (int t = 0; t < CHUNK_HEIGHT; t++) {
      const int t_x = (i + x * CHUNK_WIDTH) * TILE_WIDTH;
      const int t_y = (t + y * CHUNK_HEIGHT) * TILE_HEIGHT;

      this->temperature[i][t] =
          sn_t->fractal(10,
                        Chunk::seed % 897 +
                            static_cast<float>(i + x * CHUNK_WIDTH) / 100.0f,
                        Chunk::seed % 897 +
                            static_cast<float>(t + y * CHUNK_HEIGHT) / 100.0f) *
          64;
      this->rainfall[i][t] =
          sn_r->fractal(10,
                        Chunk::seed % 477 +
                            static_cast<float>(i + x * CHUNK_WIDTH) / 100.0f,
                        Chunk::seed % 477 +
                            static_cast<float>(t + y * CHUNK_HEIGHT) / 100.0f) *
          64;
      this->height[i][t] =
          sn_h->fractal(
              10,
              Chunk::seed + static_cast<float>(i + x * CHUNK_WIDTH) / 100.0f,
              Chunk::seed + static_cast<float>(t + y * CHUNK_HEIGHT) / 100.0f) *
          64;

      // High rainfall
      // Tundra
      if (temperature[i][t] < -32 && rainfall[i][t] >= 0) {
        tiles[i][t][LAYER_BACKGROUND] =
            new Tile(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[i][t][LAYER_MIDGROUND] =
            new Tile(TILE_SNOW, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 30) == 0)
          tiles[i][t][LAYER_FOREGROUND] =
              new Tile(TILE_TREE, t_x, t_y, LAYER_FOREGROUND, 2);
      }
      // Forested tundra
      else if (temperature[i][t] < 0 && rainfall[i][t] >= 0) {
        tiles[i][t][LAYER_BACKGROUND] =
            new Tile(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[i][t][LAYER_MIDGROUND] =
            new Tile(TILE_SNOW, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 3) == 0)
          tiles[i][t][LAYER_FOREGROUND] =
              new Tile(TILE_TREE, t_x, t_y, LAYER_FOREGROUND, random(1, 2));
      }
      // Forest
      else if (temperature[i][t] < 24 && rainfall[i][t] >= 0) {
        tiles[i][t][LAYER_BACKGROUND] =
            new Tile(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[i][t][LAYER_MIDGROUND] =
            new Tile(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 2) == 0)
          tiles[i][t][LAYER_FOREGROUND] =
              new Tile(TILE_TREE, t_x, t_y, LAYER_FOREGROUND, random(0, 1));
      }
      // Dense forest
      else if (temperature[i][t] <= 64 && rainfall[i][t] >= 0) {
        tiles[i][t][LAYER_BACKGROUND] =
            new Tile(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[i][t][LAYER_MIDGROUND] =
            new Tile(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 4) != 0)
          tiles[i][t][LAYER_FOREGROUND] =
              new Tile(TILE_TREE, t_x, t_y, LAYER_FOREGROUND, 3);
      }

      // Low rainfall
      // Wasteland
      else if (temperature[i][t] < -32 && rainfall[i][t] < 0) {
        tiles[i][t][LAYER_BACKGROUND] =
            new Tile(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
      }
      // Wasteland
      else if (temperature[i][t] < -8 && rainfall[i][t] < 0) {
        tiles[i][t][LAYER_BACKGROUND] =
            new Tile(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[i][t][LAYER_MIDGROUND] =
            new Tile(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
      }
      // Grassy wasteland
      else if (temperature[i][t] < 0 && rainfall[i][t] < 0) {
        tiles[i][t][LAYER_BACKGROUND] =
            new Tile(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[i][t][LAYER_MIDGROUND] =
            new Tile(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 8) == 0) {
          tiles[i][t][LAYER_FOREGROUND] = new Tile(
              TILE_DENSE_GRASS, t_x, t_y, LAYER_FOREGROUND, random(0, 3));
        }
      }
      // Grassy wasteland
      else if (temperature[i][t] < 24 && rainfall[i][t] < 0) {
        tiles[i][t][LAYER_BACKGROUND] =
            new Tile(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[i][t][LAYER_MIDGROUND] =
            new Tile(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 1) == 0) {
          tiles[i][t][LAYER_FOREGROUND] = new Tile(
              TILE_DENSE_GRASS, t_x, t_y, LAYER_FOREGROUND, random(0, 3));
          if (random(0, 100) == 0) {
            placeItemAt(new Item(ITEM_CHICKEN), t_x, t_y);
          }
        }
      }
      // Savana
      else if (temperature[i][t] < 32 && rainfall[i][t] < 0) {
        tiles[i][t][LAYER_BACKGROUND] =
            new Tile(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[i][t][LAYER_MIDGROUND] =
            new Tile(TILE_GRASS, t_x, t_y, LAYER_MIDGROUND);
        if (random(0, 5) == 0) {
          tiles[i][t][LAYER_FOREGROUND] =
              new Tile(TILE_DENSE_GRASS, t_x, t_y, LAYER_FOREGROUND, 1);
        } else if (random(0, 10) == 0) {
          tiles[i][t][LAYER_FOREGROUND] =
              new Tile(TILE_BUSH, t_x, t_y, LAYER_FOREGROUND, 1);
        } else if (random(0, 50) == 0) {
          tiles[i][t][LAYER_FOREGROUND] =
              new Tile(TILE_BARN, t_x, t_y, LAYER_FOREGROUND, 1);
        }
      }
      // Desert
      else if (temperature[i][t] <= 64 && rainfall[i][t] < 0) {
        tiles[i][t][LAYER_BACKGROUND] =
            new Tile(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND);
        tiles[i][t][LAYER_MIDGROUND] =
            new Tile(TILE_SAND, t_x, t_y, LAYER_MIDGROUND);
      }

      // Water deep
      if (height[i][t] < -32) {
        setTileAt(
            t_x, t_y, LAYER_BACKGROUND,
            new Tile(TILE_UNDERWATER_SOIL, t_x, t_y, LAYER_BACKGROUND, 3));
        setTileAt(t_x, t_y, LAYER_MIDGROUND, nullptr);
        setTileAt(t_x, t_y, LAYER_FOREGROUND,
                  new Tile(TILE_WATER, t_x, t_y, LAYER_FOREGROUND));
      }
      // Water
      else if (height[i][t] < -19) {
        setTileAt(
            t_x, t_y, LAYER_BACKGROUND,
            new Tile(TILE_UNDERWATER_SOIL, t_x, t_y, LAYER_BACKGROUND, 0));
        setTileAt(t_x, t_y, LAYER_MIDGROUND, nullptr);
        setTileAt(t_x, t_y, LAYER_FOREGROUND,
                  new Tile(TILE_WATER, t_x, t_y, LAYER_FOREGROUND));
      }
      // Water seaweed
      else if (height[i][t] < -17) {
        setTileAt(
            t_x, t_y, LAYER_BACKGROUND,
            new Tile(TILE_UNDERWATER_SOIL, t_x, t_y, LAYER_BACKGROUND, 1));
        setTileAt(t_x, t_y, LAYER_MIDGROUND, nullptr);
        setTileAt(t_x, t_y, LAYER_FOREGROUND,
                  new Tile(TILE_WATER, t_x, t_y, LAYER_FOREGROUND));
      }
      // Shore
      else if (height[i][t] < -14) {
        setTileAt(t_x, t_y, LAYER_BACKGROUND,
                  new Tile(TILE_SOIL, t_x, t_y, LAYER_BACKGROUND));
        setTileAt(t_x, t_y, LAYER_FOREGROUND,
                  new Tile(TILE_DENSE_GRASS, t_x, t_y, LAYER_FOREGROUND));
      }
      // Stone
      else if (height[i][t] > 32) {
        setTileAt(t_x, t_y, LAYER_BACKGROUND,
                  new Tile(TILE_STONE, t_x, t_y, LAYER_BACKGROUND));
        setTileAt(t_x, t_y, LAYER_MIDGROUND,
                  new Tile(TILE_STONE, t_x, t_y, LAYER_MIDGROUND));
        setTileAt(t_x, t_y, LAYER_FOREGROUND,
                  new Tile(TILE_STONE_WALL, t_x, t_y, LAYER_FOREGROUND,
                           (height[i][t] - 32) / 6));
      }
    }
  }
}
