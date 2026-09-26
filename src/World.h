#ifndef SRC_WORLD_H_
#define SRC_WORLD_H_

#include <asw/asw.h>

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "ActionParticles.h"
#include "Creature.h"
#include "FloatingText.h"
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

// Largest room, bigger enclosed areas count as outdoors
constexpr int MAX_ROOM_TILES = 80;

// What a right click did, the character carries on with eating or throwing
enum class InteractResult { None, Handled, Eat, Throw };

// Words shown under the cursor for each mouse button
struct HoverVerbs {
  std::string left{};
  std::string right{};
};

// Thrown item in flight, world pixels
struct Projectile {
  std::string item_id;
  unsigned char meta{0};
  asw::Vec2f pos{};
  asw::Vec2f velocity{};
  float travelled{0.0F};
  float range{0.0F};
  int damage{0};
};

// Weapon swing shown for a moment, world pixels
struct Swing {
  asw::Vec2f from{};
  asw::Vec2f dir{};
  float reach{0.0F};
  float arc{0.0F};
  float timer{0.0F};
};

// Weather particle
struct Particle {
  asw::Vec2f pos;
  float speed;

  // Snow flake, otherwise rain drop
  bool snow{false};

  // Rain drop streak length and alpha
  float length{9.0F};
  unsigned char alpha{160};

  // Screen y where a rain drop lands and splashes
  float land_y{0.0F};

  // Snow flake size in pixels and sway phase in radians
  float size{3.0F};
  float phase{0.0F};
};

// Short lived rain splash
struct Splash {
  asw::Vec2f pos;
  float life;
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

  // Left click: use the held item on the tile at pixel position (tools,
  // seeds, weapons)
  void use(const asw::Vec2i& inter_pos,
           const asw::Vec2i& player_pos,
           ItemStack& held);

  // Right click: interact with the tile at pixel position (open, harvest,
  // pick up, drink). Tells the caller to eat or throw when nothing is there.
  InteractResult interact(const asw::Vec2i& inter_pos,
                          const asw::Vec2i& player_pos,
                          ItemStack& held);

  // Throw one of the held stack toward a pixel position, charge 0 to 1
  void throwItem(ItemStack& held,
                 const asw::Vec2i& player_pos,
                 const asw::Vec2i& target,
                 float charge);

  // What each click would do at pixel position
  HoverVerbs hoverVerbs(const asw::Vec2i& inter_pos,
                        const asw::Vec2i& player_pos,
                        const ItemStack& held);

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

  // Particle burst on a tile, preset names are in ActionParticles.cpp
  void burstParticles(const std::string& preset, const asw::Vec2i& tile_pos);

  // Lights, registered by campfires each tick
  void addLight(const asw::Vec2i& tile_pos);
  bool nearLitCampfire(const asw::Vec2i& tile_pos, int radius) const;

  // Survival
  float ambientTemperature(const asw::Vec2i& tile_pos) const;

  // Stores
  void openShop();

  // Crafting station window (crafting, kiln) at a tier
  void openStation(const std::string& window, int tier);


  // First foreground tile within radius (square) that match accepts
  std::shared_ptr<Tile> findTileNear(
      const asw::Vec2i& tile_pos,
      int radius,
      const std::function<bool(const std::shared_ptr<Tile>&)>& match);

  // Walking into a closed door opens it, true when it did
  bool tryOpenDoor(const asw::Vec2i& tile_pos);

  // Is a creature standing on the tile
  bool creatureAt(const asw::Vec2i& tile_pos) const;

  // Rooms: floor space closed in by walls, windows and a door
  bool isIndoors(const asw::Vec2i& tile_pos) const;

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

  // Swing the held weapon toward a pixel position, true when it swung
  bool swing(const asw::Vec2i& player_pos,
             const asw::Vec2i& target,
             ItemStack& held);

  // Closest creature in a weapon's reach and arc toward target, or nullptr
  std::shared_ptr<Creature> swingTarget(const asw::Vec2i& player_pos,
                                        const asw::Vec2i& target,
                                        const ItemInfo& info) const;

  // Damage a creature, dropping its loot when it dies
  void hurtCreature(const std::shared_ptr<Creature>& creature,
                    int damage,
                    const asw::Vec2i& from);

  // Weapons
  void updateProjectiles(float dt);
  void drawWeapons() const;

  // Is the tile close enough to the player to use
  bool inReach(const asw::Vec2i& tile_pos, const asw::Vec2i& player_pos) const;

  // Run an item's rules on a tile, true when one matched
  bool runItemRules(const std::string& item_id,
                    const asw::Vec2i& tile_pos,
                    ItemStack& held);

  // First rule of an item that would act on the tile, or nullptr
  const ItemAction* matchingAction(const std::string& item_id,
                                   const asw::Vec2i& tile_pos,
                                   const ItemStack& held);

  // Survival stats over elapsed game minutes
  void updateSurvival(float minutes);

  // Wolves at night, deer and rabbits by day
  void updateCreatures(float dt, const asw::Vec2i& player_pos);
  void spawnWildlife();

  // Open ground a creature can appear on, out of sight of the player
  bool findSpawnTile(asw::Vec2i& at);

  // Store order
  void updateOrder();

  // Weather particles
  void updateWeather(float dt);

  // Night and campfire light
  void drawLighting();

  // Rooms
  void updateRooms();
  int roomAt(const asw::Vec2i& tile_pos) const;
  void drawRoofs();

  // Show recipes for items the player picks up
  void updateDiscovery();

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
  float wildlife_timer{0.0F};

  // Weapon swings and thrown items
  std::vector<Projectile> projectiles{};
  Swing last_swing{};
  float attack_cooldown{0.0F};

  // Damage numbers over creatures
  FloatingTexts floating_texts{};

  // Dirt, water, chips and coins from player actions
  ActionParticles action_particles{};

  // Total earned last tick, coins burst when it goes up, -1 until known
  int last_total_earned{-1};

  // Lit campfires seen this tick
  std::vector<asw::Vec2i> lights{};

  // Weather particles in screen space
  std::vector<Particle> particles{};
  std::vector<Splash> splashes{};

  // Seconds of weather animation, drives snow sway and fog pulse
  float weather_time{0.0F};

  // Player position
  asw::Vec2i player_tile{0, 0};

  // Room id per tile (x + y * width), 0 outdoors
  std::vector<int> rooms{};
  unsigned int rooms_version{0};
  bool rooms_ready{false};

  // First discovery pass after a new game or load is silent
  bool discovery_primed{false};

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
