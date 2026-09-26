#ifndef SRC_WORLD_H_
#define SRC_WORLD_H_

#include <asw/asw.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Creature.h"
#include "GameState.h"
#include "Inventory.h"
#include "Item.h"
#include "MapItem.h"
#include "Messenger.h"
#include "manager/ItemTypeManager.h"
#include "TileMap.h"
#include "ui/Hud.h"
#include "utility/Camera.h"

// Viewport
constexpr int VIEWPORT_WIDTH = 240 * 4;
constexpr int VIEWPORT_HEIGHT = 160 * 4;
constexpr float VIEWPORT_MAX_ZOOM = 4.0f;
constexpr float VIEWPORT_MIN_ZOOM = 0.25f;

// Size of the world in chunks
constexpr unsigned int WORLD_CHUNKS = 8;

// Max tiles between player and the tile they use
constexpr int INTERACT_RANGE = 3;

// Campfires warm and scare wolves within this many tiles
constexpr int CAMPFIRE_RADIUS = 4;

// Weather particle
struct Particle {
  asw::Vec2f pos;
  float speed;
};

class World {
 public:
  World();

  // Load types, sprites and sounds. Only runs once.
  static void loadData();

  // Fresh world with a starter farm
  void newGame();

  // Saving
  nlohmann::json toJson() const;
  void fromJson(const nlohmann::json& data);

  // Drawing
  void draw();

  // Use held stack on the tile at pixel position
  void interact(const asw::Vec2i& inter_pos,
                const asw::Vec2i& player_pos,
                ItemStack& held);

  // Eat or drink the held stack
  void consume(ItemStack& held);

  // Map
  void update(float dt, const asw::Vec2i& player_pos);

  // Get map
  TileMap& getMap();

  // Get messenger
  Messenger& getMessenger();

  void resetCamera();

  Camera& getCamera();

  Hud& getHud();

  GameState& getState();
  const GameState& getState() const;

  // Player inventory (shared with the inventory window)
  static Inventory& playerInventory();

  // Items
  void dropItems(const std::string& id,
                 const asw::Vec2i& tile_pos,
                 int count = 1);
  void dropNear(const std::string& id, const asw::Vec2i& tile_pos);

  // Closest tile that is not solid, or tile_pos if there is none
  asw::Vec2i openTileNear(const asw::Vec2i& tile_pos);

  // Move items lying on a tile into the player's bag
  bool pickUpItems(const asw::Vec2i& tile_pos);

  // Give item to player, dropping it at their feet when full
  void giveItem(const std::string& id, int count = 1);

  // Counts shown in the end of day summary
  void countEvent(const std::string& name, int amount = 1);

  // Lights, registered by campfires each tick
  void addLight(const asw::Vec2i& tile_pos);
  bool nearLitCampfire(const asw::Vec2i& tile_pos, int radius) const;

  // Survival
  float ambientTemperature(const asw::Vec2i& tile_pos) const;

  // Stores
  void openShop();

  // Tile the player stands on, used so animals do not walk into them
  const asw::Vec2i& getPlayerTile() const { return player_tile; }

  // End the day, sleeping or passing out
  void endDay(bool passed_out);

  // Summary overlay
  bool isSummaryOpen() const { return summary_open; }
  const std::vector<std::string>& getSummary() const { return summary; }
  void closeSummary() { summary_open = false; }

  // Set when the player must be moved (sleep, faint)
  bool takePlayerWarp(asw::Vec2i& to);

  // Set at the end of each day so the game autosaves
  bool takeSaveRequest();

  // Creatures
  std::vector<std::shared_ptr<Creature>>& getCreatures() { return creatures; }
  void clearCreatures();

  // Draw the status bars and clock in ui units
  void drawStatus(const asw::Vec2i& ui_size) const;

  // Draw the end of day summary in ui units
  void drawSummary(const asw::Vec2i& ui_size) const;

 private:
  // Buffers and fonts, created on first use
  void ensureBuffers();

  // Clear windows, creatures and inventories for a new or loaded game
  void resetSession();

  // Starter farm
  void setupFarm();
  asw::Vec2i findFarmLocation();

  // Item rules
  bool applyAction(const ItemAction& action,
                   const asw::Vec2i& tile_pos,
                   ItemStack& held);
  bool actionMatches(const ItemAction& action,
                     const asw::Vec2i& tile_pos,
                     const ItemStack& held);

  // Hit creatures at tile, returns true if one was there
  bool attackAt(const asw::Vec2i& tile_pos, ItemStack& held);

  // Survival stats over elapsed game minutes
  void updateSurvival(float minutes);

  // Wolves
  void updateCreatures(float dt, const asw::Vec2i& player_pos);

  // Store order
  void updateOrder();

  // Weather particles
  void updateWeather(float dt);

  // Night and campfire light
  void drawLighting();

  // Tile map
  TileMap tile_map;

  // Buffer that holds whole map image
  asw::Texture map_buffer{nullptr};

  // Light map, the map is multiplied by it (ambient colour plus lights)
  asw::Texture light_buffer{nullptr};

  // Soft white circle, drawn additively for each light
  asw::Texture light_gradient{nullptr};

  // Status text
  asw::Font font{nullptr};

  // Ticker for world
  float ticker{};

  // Camera config
  Camera camera{};

  // Messager
  Messenger map_messages{Messenger(4, false, 2)};

  // Hud
  Hud hud{};

  // Clock, economy and player stats
  GameState state{};

  // Wolves and other creatures
  std::vector<std::shared_ptr<Creature>> creatures{};
  float spawn_timer{0.0F};

  // Lit campfires seen this tick
  std::vector<asw::Vec2i> lights{};

  // Weather particles in screen space
  std::vector<Particle> particles{};

  // Player position
  asw::Vec2i player_tile{0, 0};

  // Day summary
  std::map<std::string, int> day_events{};
  std::vector<std::string> summary{};
  bool summary_open{false};

  // Requests for the game
  bool warp_requested{false};
  bool save_requested{false};

  // Warnings already shown, so they do not repeat each frame
  bool warned_hunger{false};
  bool warned_thirst{false};
  bool warned_cold{false};
  bool warned_health{false};
};

#endif  // SRC_WORLD_H_
