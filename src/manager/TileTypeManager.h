/*
  Tile Type Manager
  Allan Legemaate
  24/11/15
  This loads all the types of tiles into a container for access by tile objects.
*/
#ifndef TILE_TYPE_MANAGER_H
#define TILE_TYPE_MANAGER_H

#include <map>
#include <string>

#include "../ui/UiController.h"
#include "TileType.h"

class TileTypeManager {
 public:
  // Load tile types
  static int loadTiles(const std::string& path);

  // Allows communication
  static TileType& getTile(const std::string& id);

  static bool exists(const std::string& id);

  static const std::map<std::string, TileType>& getTiles() { return tile_defs; }

  // Named sprite sheets ("tiles", "items", "placeholders")
  static void addSheet(const std::string& name, asw::Texture texture);
  static asw::Texture getSheet(const std::string& name);

 private:
  static std::map<std::string, TileType> tile_defs;
  static std::map<std::string, asw::Texture> sheets;
};

#endif  // TILE_TYPE_MANAGER_H
