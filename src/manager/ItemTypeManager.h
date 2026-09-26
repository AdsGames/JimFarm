/*
  Item Type Manager
  Allan Legemaate
  24/11/15
  This loads all the types of items into a container for access by item objects.
*/
#ifndef ITEM_TYPE_MANAGER_H
#define ITEM_TYPE_MANAGER_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../ui/UiController.h"
#include "TileType.h"

const int NON_SOLID = 0;
const int SOLID = 1;

// A rule describing what an item does to the tile it is used on.
// Rules are checked in order, the first one that matches is applied.
struct ItemAction {
  // Layer the rule looks at, -1 means the rule always matches
  int layer{-1};

  // Tile ids accepted at layer, "*" accepts any tile
  std::vector<std::string> tiles{};

  // Layer must have no tile
  bool empty{false};

  // Other layers that must be empty or present
  std::vector<int> requires_empty{};
  std::vector<int> requires_present{};

  // Item meta limits (e.g. water left in a can)
  int min_meta{-1};
  int max_meta{-1};

  // Replace tile at layer, empty string removes it
  bool has_replace{false};
  std::string replace{};
  unsigned char replace_meta{0};

  // Place a tile on another layer
  std::string place{};
  int place_layer{-1};
  unsigned char place_meta{0};

  // Results, drops land on the tile, gives go to the player
  std::vector<std::string> drops{};
  std::vector<std::string> gives{};
  bool consume{false};
  int set_meta{-1};
  int add_meta{0};
  std::string sound{};
  std::string message{};
  float energy{0.0F};

  // Apply to every matching tile within this radius
  int area{0};

  // Player stat changes
  float hunger{0.0F};
  float thirst{0.0F};
  float health{0.0F};

  // Rule only reports failure (error sound + message)
  bool fail{false};
};

// Non visual item data
struct ItemInfo {
  // Short text shown in the tooltip
  std::string description{};

  // Store buy price, 0 when not sold
  int price{0};

  // Sell value, 0 when the store will not buy it
  int value{0};

  // Eating
  bool edible{false};
  float hunger{0.0F};
  float thirst{0.0F};
  float health{0.0F};
  float warmth{0.0F};

  // Drinking uses one meta (e.g. water in a can) instead of the item
  bool food_uses_meta{false};

  // Ambient warmth while carried
  float carry_warmth{0.0F};

  // Damage dealt to creatures
  int attack{0};

  // Tool strength, trees and rocks take fewer hits. 0 for non tools.
  float power{0.0F};

  // Starting meta for new items (e.g. full watering can)
  unsigned char start_meta{0};

  std::vector<ItemAction> actions{};
};

class ItemTypeManager {
 public:
  // Load tile types
  static int loadItems(const std::string& path);

  // Allows communication
  static TileType& getItem(const std::string& tileID);

  static const ItemInfo& getInfo(const std::string& id);

  static bool exists(const std::string& id);

 private:
  // Stores all tiles and items
  static std::map<std::string, TileType> item_defs;
  static std::map<std::string, ItemInfo> item_info;
};

// Layer name ("background", "midground", "foreground") to layer index
int layerFromName(const std::string& name);

#endif  // ITEM_TYPE_MANAGER_H
