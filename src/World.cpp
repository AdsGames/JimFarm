#include "World.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <format>
#include <limits>

#include "Graphics.h"
#include "behaviours/TileBehaviour.h"
#include "manager/BehaviourTypeManager.h"
#include "manager/InterfaceTypeManager.h"
#include "manager/ItemTypeManager.h"
#include "manager/RecipeManager.h"
#include "manager/SoundManager.h"
#include "manager/TileTypeManager.h"
#include "utility/Tools.h"

namespace {
// Starter farm size in tiles
constexpr int FARM_WIDTH = 24;
constexpr int FARM_HEIGHT = 18;

// Light buffer is a quarter of the viewport, it is scaled up smoothly
constexpr int LIGHT_SCALE = 4;

// Survival rates, per game minute
constexpr float HUNGER_RATE = 0.06F;
constexpr float THIRST_RATE = 0.08F;
constexpr float COLD_THRESHOLD = 8.0F;
constexpr float HOT_THRESHOLD = 28.0F;
constexpr float CHILL_RATE = 0.02F;
constexpr float WARM_UP_RATE = 0.3F;
constexpr float STARVE_DAMAGE = 0.1F;
constexpr float THIRST_DAMAGE = 0.15F;
constexpr float FREEZE_DAMAGE = 0.25F;
constexpr float HEAL_RATE = 0.03F;
constexpr float WARNING_LEVEL = 25.0F;

constexpr int FIRST_WOLF_DAY = 2;
constexpr int MAX_WOLVES = 6;
constexpr int MAX_WILDLIFE = 4;

// Rooms are warm and dry
constexpr float INDOOR_WARMTH = 12.0F;

// Roof sprite in the placeholder sheet
constexpr int ROOF_X = 4;
constexpr int ROOF_Y = 1;

// Light gradient texture size in pixels
constexpr int LIGHT_GRADIENT_SIZE = 64;

// Ambient light, the map is multiplied by it
const asw::Color DAY_LIGHT(255, 255, 255);
const asw::Color GOLDEN_LIGHT(255, 180, 120);
const asw::Color OVERCAST_LIGHT(190, 195, 210);
const asw::Color NIGHT_LIGHT(50, 58, 110);

// Lights, added to the ambient light
const asw::Color LANTERN_LIGHT(255, 225, 180);
const asw::Color FIRE_LIGHT(255, 130, 40);

// Golden hour lengths in game minutes
constexpr float DAWN_MINUTES = 120.0F;
constexpr float EVENING_MINUTES = 90.0F;

// Light strengths at full night
constexpr float LANTERN_STRENGTH = 0.55F;
constexpr float FIRE_STRENGTH = 0.9F;

// Share of the fire light also added straight onto the screen at night
constexpr float FIRE_GLOW = 0.35F;

// Smooth scaling, asw textures default to nearest
void setLinearScaling(const asw::Texture& texture) {
  SDL_SetTextureScaleMode(texture.get(), SDL_SCALEMODE_LINEAR);
}

float smoothstep(float edge_0, float edge_1, float value) {
  const float t = std::clamp((value - edge_0) / (edge_1 - edge_0), 0.0F, 1.0F);
  return t * t * (3.0F - 2.0F * t);
}

asw::Color mix(const asw::Color& from, const asw::Color& to, float amount) {
  auto channel = [&](uint8_t a, uint8_t b) {
    return static_cast<uint8_t>(static_cast<float>(a) +
                                (static_cast<float>(b) - a) * amount);
  };
  return asw::Color(channel(from.r, to.r), channel(from.g, to.g),
                    channel(from.b, to.b));
}

// Golden after sunrise and before dusk, blue at night, grey when overcast
asw::Color ambientLight(const GameState& state) {
  const float minutes = state.getMinutes();
  const float dawn =
      1.0F - smoothstep(DAY_START_MINUTES, DAY_START_MINUTES + DAWN_MINUTES,
                        minutes);
  const float evening =
      smoothstep(DUSK_MINUTES - EVENING_MINUTES, DUSK_MINUTES, minutes);

  auto light = mix(DAY_LIGHT, GOLDEN_LIGHT, std::max(dawn, evening));

  if (state.getWeather() != Weather::Sunny) {
    light = mix(light, OVERCAST_LIGHT, 0.8F);
  }

  return mix(light, NIGHT_LIGHT, state.darkness());
}

// Seconds since start, drives the fire flicker
float flickerClock() {
  static const auto start = std::chrono::steady_clock::now();
  return std::chrono::duration<float>(std::chrono::steady_clock::now() - start)
      .count();
}

// White circle, opaque in the middle and clear at the edge
asw::Texture makeLightGradient() {
  auto texture =
      asw::assets::create_texture(LIGHT_GRADIENT_SIZE, LIGHT_GRADIENT_SIZE);

  asw::display::set_render_target(texture);
  asw::display::set_blend_mode(asw::BlendMode::None);
  asw::display::clear(asw::Color(255, 255, 255, 0));

  const float half = LIGHT_GRADIENT_SIZE / 2.0F;
  for (int y = 0; y < LIGHT_GRADIENT_SIZE; y++) {
    for (int x = 0; x < LIGHT_GRADIENT_SIZE; x++) {
      const float distance =
          std::hypot(x + 0.5F - half, y + 0.5F - half) / half;
      const float strength = smoothstep(1.0F, 0.0F, distance);
      if (strength <= 0.0F) {
        continue;
      }

      asw::draw::point(asw::Vec2f(x, y),
                       asw::Color(255, 255, 255,
                                  static_cast<uint8_t>(strength * 255.0F)));
    }
  }

  asw::display::reset_render_target();
  asw::display::set_blend_mode(asw::BlendMode::Blend);
  asw::draw::set_blend_mode(texture, asw::BlendMode::Add);
  setLinearScaling(texture);
  return texture;
}

// Seconds a rain splash stays on screen
constexpr float SPLASH_LIFE = 0.3F;

float clampStat(float value) {
  return std::clamp(value, 0.0F, MAX_STAT);
}

void setStat(float& stat, float change) {
  stat = clampStat(stat + change);
}
}  // namespace

/************
 * TILE MAP *
 ************/
World::World() {
  resetCamera();
}

void World::resetCamera() {
  Quad<int> bounds = {
      .x_1 = 0,
      .y_1 = 0,
      .x_2 = VIEWPORT_WIDTH,
      .y_2 = VIEWPORT_HEIGHT,
  };

  Quad<int> outer_bounds = {
      .x_1 = 0,
      .y_1 = 0,
      .x_2 = tile_map.getWidth() * TILE_SIZE,
      .y_2 = tile_map.getHeight() * TILE_SIZE,
  };

  camera = Camera(bounds, outer_bounds);
  camera.setZoom(2.0f);
}

/*
 * DATA
 */
void World::loadData() {
  static bool loaded = false;
  if (loaded) {
    return;
  }
  loaded = true;

  TileTypeManager::addSheet("tiles",
                            asw::assets::load_texture("assets/images/tiles.png"));
  TileTypeManager::addSheet("items",
                            asw::assets::load_texture("assets/images/items.png"));
  TileTypeManager::addSheet(
      "placeholders",
      asw::assets::load_texture("assets/images/placeholders.png"));

  BehaviourTypeManager::loadBehaviours();

  asw::log::info("Loading assets/data/tiles.json");
  if (TileTypeManager::loadTiles("assets/data/tiles.json")) {
    asw::util::abort_on_error("Could not load assets/data/tiles.json");
  }

  asw::log::info("Loading assets/data/items.json");
  if (ItemTypeManager::loadItems("assets/data/items.json")) {
    asw::util::abort_on_error("Could not load assets/data/items.json");
  }

  asw::log::info("Loading assets/data/interfaces.json");
  if (InterfaceTypeManager::loadInterfaces("assets/data/interfaces.json")) {
    asw::util::abort_on_error("Could not load assets/data/interfaces.json");
  }

  asw::log::info("Loading assets/data/sounds.json");
  if (SoundManager::load("assets/data/sounds.json")) {
    asw::util::abort_on_error("Could not load assets/data/sounds.json");
  }

  asw::log::info("Loading assets/data/recipes.json");
  if (RecipeManager::loadRecipes("assets/data/recipes.json")) {
    asw::util::abort_on_error("Could not load assets/data/recipes.json");
  }

  Creature::loadImages();
}

void World::ensureBuffers() {
  if (!map_buffer) {
    // Large enough for the widest (min zoom) camera view
    map_buffer = asw::assets::create_texture(
        static_cast<int>(VIEWPORT_WIDTH / VIEWPORT_MIN_ZOOM),
        static_cast<int>(VIEWPORT_HEIGHT / VIEWPORT_MIN_ZOOM));
  }

  if (!light_buffer) {
    light_buffer = asw::assets::create_texture(VIEWPORT_WIDTH / LIGHT_SCALE,
                                               VIEWPORT_HEIGHT / LIGHT_SCALE);
    asw::draw::set_blend_mode(light_buffer, asw::BlendMode::Modulate);
    setLinearScaling(light_buffer);
  }

  if (!light_gradient) {
    light_gradient = makeLightGradient();
  }

  if (!font) {
    font = asw::assets::load_font("assets/fonts/pixelart.ttf", 8,
                                  asw::FontStyle::Pixel);
  }
}

void World::resetSession() {
  ensureBuffers();
  clearCreatures();
  hud.closeAll();
  day_events.clear();
  summary.clear();
  summary_open = false;
  warp_requested = false;
  save_requested = false;

  for (const auto* name : {"inventory", "crafting", "furnace", "kiln"}) {
    InterfaceTypeManager::getInterfaceByName(name).getInventory()->empty();
  }

  rooms_ready = false;
  discovery_primed = false;
}

void World::newGame() {
  resetSession();
  state = GameState();

  tile_map.clearMap();
  tile_map.generateMap(asw::Vec2<unsigned int>(WORLD_CHUNKS, WORLD_CHUNKS));
  setupFarm();
  resetCamera();
  updateOrder();

  summary = {
      "Welcome to JimFarm!",
      "",
      std::format("Goal: earn ${} by the end of Spring", GOAL_COINS),
      "Till soil, plant seeds, water them each day.",
      "Sell crops at the store, sleep in the barn.",
      "",
      "Eat, drink and stay warm to survive.",
      "Wolves hunt at night. Campfires keep them away.",
      "",
      "Press H for controls",
  };
  summary_open = true;
}

nlohmann::json World::toJson() const {
  return {{"state", state.toJson()}, {"map", tile_map.toJson()}};
}

void World::fromJson(const nlohmann::json& data) {
  resetSession();
  state = GameState();
  state.fromJson(data["state"]);
  tile_map.fromJson(data["map"]);
  resetCamera();
}

/*
 * FARM
 */
asw::Vec2i World::findFarmLocation() {
  asw::Vec2i best_pos(2, 2);
  int best_score = std::numeric_limits<int>::max();

  for (int y = 2; y + FARM_HEIGHT + 2 < tile_map.getHeight(); y += 4) {
    for (int x = 2; x + FARM_WIDTH + 2 < tile_map.getWidth(); x += 4) {
      int score = 0;

      // Sample every other tile, water and rock are hard to farm
      for (int dx = 0; dx < FARM_WIDTH; dx += 2) {
        for (int dy = 0; dy < FARM_HEIGHT; dy += 2) {
          const auto at = asw::Vec2i(x + dx, y + dy);
          const auto top = tile_map.getTileAt(at, LAYER_FOREGROUND);
          if (top && (top->getType().getId() == "tile:water" ||
                      top->getType().getId() == "tile:stone_wall")) {
            score += 10;
          }

          // Mild climate suits most crops
          score += std::abs(tile_map.getTemperatureAt(at) - 12) / 4;
        }
      }

      if (score < best_score) {
        best_score = score;
        best_pos = asw::Vec2i(x, y);
      }
    }
  }

  return best_pos;
}

void World::setupFarm() {
  const auto origin = findFarmLocation();

  auto set = [&](const asw::Vec2i& offset, int z, const std::string& id,
                 unsigned char meta = 0) {
    const auto at = origin + offset;
    if (auto existing = tile_map.getTileAt(at, z)) {
      tile_map.removeTile(existing);
    }
    if (!id.empty()) {
      tile_map.placeTile(std::make_shared<Tile>(id, at * TILE_SIZE, z, meta));
    }
  };

  // Clear to grass
  for (int x = 0; x < FARM_WIDTH; x++) {
    for (int y = 0; y < FARM_HEIGHT; y++) {
      set(asw::Vec2i(x, y), LAYER_FOREGROUND, "");
      set(asw::Vec2i(x, y), LAYER_MIDGROUND, "tile:grass");
      set(asw::Vec2i(x, y), LAYER_BACKGROUND, "tile:soil");
    }
  }
  tile_map.clearItems(origin,
                      origin + asw::Vec2i(FARM_WIDTH - 1, FARM_HEIGHT - 1));

  // Road
  for (int x = 1; x < FARM_WIDTH - 1; x++) {
    set(asw::Vec2i(x, 7), LAYER_MIDGROUND, "tile:path");
  }

  // Barn is home, the store buys and sells
  tile_map.placeStructure("tile:barn", origin + asw::Vec2i(3, 6));
  tile_map.placeStructure("tile:store", origin + asw::Vec2i(18, 6));

  // Well, fill the watering can on the stones in front of it
  set(asw::Vec2i(11, 5), LAYER_FOREGROUND, "tile:well");
  for (int x = 10; x <= 12; x++) {
    set(asw::Vec2i(x, 6), LAYER_MIDGROUND, "tile:well_path");
  }

  // Field, some of it ready to plant
  for (int x = 5; x <= 8; x++) {
    for (int y = 10; y <= 11; y++) {
      set(asw::Vec2i(x, y), LAYER_MIDGROUND, "tile:plowed_soil");
    }
  }

  // Campfire
  set(asw::Vec2i(14, 10), LAYER_FOREGROUND, "tile:campfire", 200);

  // Chicken pen
  for (int x = 16; x <= 22; x++) {
    set(asw::Vec2i(x, 11), LAYER_FOREGROUND, "tile:fence");
    set(asw::Vec2i(x, 16), LAYER_FOREGROUND, "tile:fence");
  }
  for (int y = 12; y <= 15; y++) {
    set(asw::Vec2i(16, y), LAYER_FOREGROUND, "tile:fence");
    set(asw::Vec2i(22, y), LAYER_FOREGROUND, "tile:fence");
  }
  set(asw::Vec2i(19, 11), LAYER_FOREGROUND, "tile:fence_door");

  state.home = origin + asw::Vec2i(4, 7);
}

/*
 * IMAGES
 */
// Draw bottom tiles
void World::draw() {
  // Clear buffer
  asw::display::set_render_target(map_buffer);

  // Drawable
  Graphics::Instance().draw(camera);
  drawRoofs();
  floating_texts.draw(font, camera);

  asw::display::reset_render_target();

  asw::draw::set_blend_mode(map_buffer, asw::BlendMode::Blend);

  // Draw buffer
  asw::draw::stretch_sprite_blit(
      map_buffer, asw::Quadf(0, 0, camera.getSize().x, camera.getSize().y),
      asw::Quadf(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT));

  // Draw temperature indicator
  const char temp = tile_map.getTemperatureAt(camera.getCenter() / TILE_SIZE);
  const char r_val = (temp > 0) ? (temp / 2) : 0;
  const char b_val = (temp < 0) ? (temp / 2 * -1) : 0;

  asw::display::set_blend_mode(asw::BlendMode::Blend);

  if (r_val > 0) {
    asw::draw::rect_fill(asw::Quadf(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT),
                         asw::Color(255, 0, 0, r_val));
  }
  if (b_val > 0) {
    asw::draw::rect_fill(asw::Quadf(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT),
                         asw::Color(0, 0, 255, b_val));
  }

  // Night
  drawLighting();

  // Rain and snow
  asw::display::set_blend_mode(asw::BlendMode::Blend);

  // Light fog while raining, slowly pulsing
  if (state.getWeather() == Weather::Rain) {
    const float pulse = 0.5F + 0.5F * std::sin(weather_time * 0.35F);
    const auto fog_alpha = static_cast<unsigned char>(20.0F + 14.0F * pulse);
    asw::draw::rect_fill(asw::Quadf(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT),
                         asw::Color(170, 185, 205, fog_alpha));
  }

  for (auto const& p : particles) {
    if (!p.snow) {
      // Streak slants with the wind, 0.3 px across per px down
      asw::draw::line(p.pos,
                      asw::Vec2f(p.pos.x - p.length * 0.3F, p.pos.y + p.length),
                      asw::Color(160, 180, 255, p.alpha));
    } else {
      asw::draw::rect_fill(asw::Quadf(p.pos.x, p.pos.y, p.size, p.size),
                           asw::Color(255, 255, 255, 200));
    }
  }

  // Splashes, a ring that grows and fades with two droplets
  for (auto const& s : splashes) {
    const float t = 1.0F - s.life / SPLASH_LIFE;
    const auto alpha = static_cast<unsigned char>(150.0F * (1.0F - t));
    const asw::Color color(170, 190, 255, alpha);
    const float spread = 1.0F + 4.0F * t;

    asw::draw::circle(s.pos, spread, color);
    asw::draw::point(
        asw::Vec2f(s.pos.x - spread - 1.0F, s.pos.y - 2.0F - 3.0F * t), color);
    asw::draw::point(
        asw::Vec2f(s.pos.x + spread + 1.0F, s.pos.y - 2.0F - 3.0F * t), color);
  }

  // Dim map while hud is open, hud itself is drawn in the ui pass
  if (hud.isOpen()) {
    asw::draw::rect_fill(asw::Quadf(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT),
                         asw::Color(0, 0, 0, 64));
  }
}

void World::drawLighting() {
  const auto ambient = ambientLight(state);

  // Full daylight leaves the map as it is
  if (ambient.r == 255 && ambient.g == 255 && ambient.b == 255) {
    return;
  }

  // Lights only show once it gets dark
  const float dark = state.darkness();
  const float zoom = camera.getZoom();
  const auto cam = camera.getPosition();
  const float seconds = flickerClock();

  // Screen area of a light, scale is 1 for the screen and less for the buffer
  auto lightQuad = [&](const asw::Vec2i& tile, float radius_tiles,
                       float scale) {
    const float x = (tile.x * TILE_SIZE + TILE_SIZE / 2 - cam.x) * zoom * scale;
    const float y = (tile.y * TILE_SIZE + TILE_SIZE / 2 - cam.y) * zoom * scale;
    const float radius = radius_tiles * TILE_SIZE * zoom * scale;
    return asw::Quadf(x - radius, y - radius, radius * 2.0F, radius * 2.0F);
  };

  // -1 to 1, each fire out of step with the others
  auto flicker = [&](const asw::Vec2i& tile) {
    return std::sin(seconds * 9.0F + tile.x * 1.7F) * 0.6F +
           std::sin(seconds * 23.0F + tile.y * 2.3F) * 0.4F;
  };

  auto light = [&](const asw::Quadf& area, const asw::Color& color,
                   float strength) {
    asw::draw::set_tint(light_gradient, color);
    asw::draw::set_alpha(light_gradient, std::clamp(strength, 0.0F, 1.0F));
    asw::draw::stretch_sprite(light_gradient, area);
  };

  // Light map: ambient colour, then lights added on top
  asw::display::set_render_target(light_buffer);
  asw::display::clear(ambient);

  const float buffer_scale = 1.0F / LIGHT_SCALE;

  // Player carries a small lantern
  light(lightQuad(player_tile, 3.5F, buffer_scale), LANTERN_LIGHT,
        dark * LANTERN_STRENGTH);

  for (auto const& pos : lights) {
    const float f = flicker(pos);
    light(lightQuad(pos, (CAMPFIRE_RADIUS + 2.0F) * (1.0F + 0.04F * f),
                    buffer_scale),
          FIRE_LIGHT, dark * FIRE_STRENGTH * (0.92F + 0.08F * f));
  }

  asw::display::reset_render_target();
  asw::display::set_blend_mode(asw::BlendMode::Blend);

  // Multiply the map by the light map
  asw::draw::stretch_sprite(light_buffer,
                            asw::Quadf(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT));

  // Fires also glow, so they look warm and not only less dark
  for (auto const& pos : lights) {
    const float f = flicker(pos);
    light(lightQuad(pos, CAMPFIRE_RADIUS * 0.6F * (1.0F + 0.06F * f), 1.0F),
          FIRE_LIGHT, dark * FIRE_GLOW * (0.85F + 0.15F * f));
  }

  asw::draw::set_tint(light_gradient, asw::color::white);
  asw::draw::set_alpha(light_gradient, 1.0F);
}

/*
 * MAP
 */
bool World::actionMatches(const ItemAction& action,
                          const asw::Vec2i& tile_pos,
                          const ItemStack& held) {
  if (!tile_map.inBounds(tile_pos)) {
    return false;
  }

  const auto item = held.getItem();
  const int meta = item ? item->getMeta() : 0;

  if (action.consume && !item) {
    return false;
  }
  if (action.min_meta >= 0 && meta < action.min_meta) {
    return false;
  }
  if (action.max_meta >= 0 && meta > action.max_meta) {
    return false;
  }

  if (action.layer >= 0) {
    const auto tile = tile_map.getTileAt(tile_pos, action.layer);

    if (action.empty) {
      if (tile) {
        return false;
      }
    } else {
      if (!tile) {
        return false;
      }

      const auto& id = tile->getType().getId();
      const bool listed = std::ranges::any_of(action.tiles, [&](auto& t) {
        return t == "*" || t == id;
      });
      if (!listed) {
        return false;
      }
    }
  }

  for (const int layer : action.requires_empty) {
    if (tile_map.getTileAt(tile_pos, layer)) {
      return false;
    }
  }

  for (const int layer : action.requires_present) {
    if (!tile_map.getTileAt(tile_pos, layer)) {
      return false;
    }
  }

  return true;
}

bool World::applyAction(const ItemAction& action,
                        const asw::Vec2i& tile_pos,
                        ItemStack& held) {
  if (action.has_replace && action.layer >= 0) {
    if (auto tile = tile_map.getTileAt(tile_pos, action.layer)) {
      tile_map.replaceTile(tile, action.replace, action.replace_meta);
    } else if (!action.replace.empty()) {
      tile_map.placeTile(std::make_shared<Tile>(action.replace,
                                                tile_pos * TILE_SIZE,
                                                action.layer,
                                                action.replace_meta));
    }
  }

  if (!action.place.empty() &&
      !tile_map.getTileAt(tile_pos, action.place_layer)) {
    tile_map.placeTile(std::make_shared<Tile>(action.place,
                                              tile_pos * TILE_SIZE,
                                              action.place_layer,
                                              action.place_meta));
  }

  for (auto const& drop : action.drops) {
    dropItems(drop, tile_pos);
  }

  for (auto const& give : action.gives) {
    giveItem(give);
  }

  if (auto item = held.getItem()) {
    if (action.set_meta >= 0) {
      item->setMeta(static_cast<unsigned char>(action.set_meta));
    }
    if (action.add_meta != 0) {
      item->setMeta(static_cast<unsigned char>(
          std::clamp(item->getMeta() + action.add_meta, 0, 255)));
    }
  }

  if (action.consume) {
    held.remove(1);
  }

  setStat(state.hunger, action.hunger);
  setStat(state.thirst, action.thirst);
  setStat(state.health, action.health);

  return true;
}

bool World::attackAt(const asw::Vec2i& tile_pos, ItemStack& held) {
  const auto item = held.getItem();
  const int attack =
      item ? ItemTypeManager::getInfo(item->getType().getId()).attack : 0;

  for (auto const& creature : creatures) {
    // Creatures move between tiles, accept a click near them
    const auto offset = creature->getPosition() - tile_pos * TILE_SIZE;
    if (creature->isGone() || std::abs(offset.x) > TILE_SIZE * 3 / 4 ||
        std::abs(offset.y) > TILE_SIZE * 3 / 4) {
      continue;
    }

    if (attack <= 0) {
      state.notify("You need a weapon, try a spear or axe");
      SoundManager::play("error");
      return true;
    }

    if (!state.useEnergy(2.0F)) {
      SoundManager::play("error");
      return true;
    }

    SoundManager::play("axe");

    floating_texts.add(std::to_string(attack),
                       asw::Vec2f(creature->getPosition().x + TILE_SIZE / 2.0F,
                                  creature->getPosition().y - 2.0F),
                       asw::Color(255, 220, 90));

    if (creature->hit(*this, attack, player_tile * TILE_SIZE)) {
      const auto at = creature->getTile();
      switch (creature->getKind()) {
        case CreatureKind::Wolf:
          dropItems("item:pelt", at);
          state.wolves_defeated++;
          state.notify("Wolf defeated!");
          break;
        case CreatureKind::Deer:
          dropItems("item:meat", at, 2);
          dropItems("item:hide", at);
          state.notify("You hunted a deer");
          break;
        case CreatureKind::Rabbit:
          dropItems("item:meat", at);
          state.notify("You caught a rabbit");
          break;
      }
    }

    return true;
  }

  return false;
}

// Interact with
void World::interact(const asw::Vec2i& inter_pos,
                     const asw::Vec2i& player_pos,
                     ItemStack& held) {
  const auto tile_pos = inter_pos / TILE_SIZE;
  const auto player =
      (player_pos + asw::Vec2i(TILE_SIZE / 2, TILE_SIZE / 2)) / TILE_SIZE;

  if (!tile_map.inBounds(tile_pos)) {
    return;
  }

  if (std::max(std::abs(tile_pos.x - player.x),
               std::abs(tile_pos.y - player.y)) > INTERACT_RANGE) {
    state.notify("Too far away");
    return;
  }

  // Creatures first
  if (attackAt(tile_pos, held)) {
    return;
  }

  // Empty hand picks up items lying on the tile
  if (!held.getItem() && pickUpItems(tile_pos)) {
    SoundManager::play("pickup");
    return;
  }

  // Tile behaviours, top layer first
  for (int z = LAYER_FOREGROUND; z >= static_cast<int>(LAYER_BACKGROUND);
       z--) {
    auto tile = tile_map.resolveStructure(tile_map.getTileAt(tile_pos, z));
    if (!tile) {
      continue;
    }

    for (auto const& behaviour : tile->getType().getBehaviours()) {
      if (behaviour->onInteract(*this, tile, held)) {
        return;
      }
    }
  }

  // Item rules, empty hands use the hand's rules
  const auto item = held.getItem();
  const auto& info =
      ItemTypeManager::getInfo(item ? item->getType().getId() : "item:hand");

  for (auto const& action : info.actions) {
    // Centre tile first, then the rest of the area
    std::vector<asw::Vec2i> cells;
    for (int dx = -action.area; dx <= action.area; dx++) {
      for (int dy = -action.area; dy <= action.area; dy++) {
        const auto at = tile_pos + asw::Vec2i(dx, dy);
        if (actionMatches(action, at, held)) {
          cells.push_back(at);
        }
      }
    }

    if (cells.empty()) {
      continue;
    }

    if (action.fail) {
      SoundManager::play(action.sound.empty() ? "error" : action.sound);
      if (!action.message.empty()) {
        state.notify(action.message);
      }
      return;
    }

    if (!state.useEnergy(action.energy)) {
      SoundManager::play("error");
      return;
    }

    for (auto const& cell : cells) {
      // Stack or meta may run out part way through an area
      if (actionMatches(action, cell, held)) {
        applyAction(action, cell, held);
      }
    }

    if (!action.sound.empty()) {
      SoundManager::play(action.sound);
    }
    if (!action.message.empty()) {
      state.notify(action.message);
    }
    return;
  }

  if (item) {
    SoundManager::play("error");
  }
}

void World::consume(ItemStack& held) {
  const auto item = held.getItem();
  if (!item) {
    return;
  }

  const auto& info = ItemTypeManager::getInfo(item->getType().getId());
  if (!info.edible) {
    state.notify("You can not eat that");
    return;
  }

  // Drinking from a container uses its meta instead of the item
  if (info.food_uses_meta) {
    if (item->getMeta() == 0) {
      state.notify("It is empty");
      SoundManager::play("error");
      return;
    }
    item->setMeta(item->getMeta() - 1);
  } else {
    held.remove(1);
  }

  setStat(state.hunger, info.hunger);
  setStat(state.thirst, info.thirst);
  setStat(state.health, info.health);
  setStat(state.warmth, info.warmth);
  setStat(state.energy, info.hunger * 0.5F);

  state.notify(info.hunger > 0 ? "Ate the " + item->getType().getName()
                               : "Used the " + item->getType().getName());
  SoundManager::play("egg");
}

// Update tile map
void World::update(float dt, const asw::Vec2i& player_pos) {
  // Re-sort dynamic sprites (character position changes each frame)
  Graphics::Instance().prune();

  player_tile =
      (player_pos + asw::Vec2i(TILE_SIZE / 2, TILE_SIZE / 2)) / TILE_SIZE;

  map_messages.update(dt);

  // Seasonal tint and wind sway for tiles
  TileType::updateEnvironment(dt, state);

  // Everything waits while the summary is up
  if (summary_open) {
    return;
  }

  // Update hud
  hud.update(state);

  // Crafting by hand unless a station opened the window
  if (!hud.isOpen("crafting")) {
    state.crafting_tier = TIER_HAND;
  }

  updateRooms();
  updateDiscovery();

  // Clock
  const float before = state.getMinutes();
  const bool out_of_time = state.advanceClock(dt);
  updateSurvival(state.getMinutes() - before);

  // One game tick (20x second, 50ms)
  ticker += dt;
  if (ticker >= 0.050F) {
    ticker -= 0.050F;

    // Campfires register their light as they tick
    lights.clear();
    tile_map.tick(camera, *this);
  }

  updateCreatures(dt, player_pos);
  floating_texts.update(dt);
  updateWeather(dt);

  // End of day
  if (state.health <= 0.0F) {
    endDay(true);
  } else if (state.sleep_requested) {
    endDay(false);
  } else if (out_of_time) {
    endDay(true);
  }

  // Messages
  for (auto const& message : state.takeMessages()) {
    map_messages.pushMessage(message);
  }

  // Zooming
  const auto zoom = camera.getZoom();

  // "=" is the unshifted "+" key
  if ((asw::input::get_key_down(asw::input::Key::KpPlus) ||
       asw::input::get_key_down(asw::input::Key::Equals)) &&
      zoom < VIEWPORT_MAX_ZOOM) {
    camera.setZoom(zoom * 2.0f);
  }

  if ((asw::input::get_key_down(asw::input::Key::KpMinus) ||
       asw::input::get_key_down(asw::input::Key::Minus)) &&
      zoom > VIEWPORT_MIN_ZOOM) {
    camera.setZoom(zoom * 0.5f);
  }
}

/*
 * SURVIVAL
 */
float World::ambientTemperature(const asw::Vec2i& tile_pos) const {
  // Biome temperature is -64 to 64, map to roughly -9C to 33C
  float temperature =
      static_cast<float>(
          const_cast<TileMap&>(tile_map).getTemperatureAt(tile_pos)) /
          3.0F +
      12.0F;

  switch (state.getSeason()) {
    case Season::Summer:
      temperature += 8.0F;
      break;
    case Season::Fall:
      temperature -= 3.0F;
      break;
    case Season::Winter:
      temperature -= 15.0F;
      break;
    case Season::Spring:
      break;
  }

  temperature -= 6.0F * state.darkness();

  if (state.getWeather() == Weather::Rain) {
    temperature -= 3.0F;
  } else if (state.getWeather() == Weather::Snow) {
    temperature -= 5.0F;
  }

  if (nearLitCampfire(tile_pos, CAMPFIRE_RADIUS)) {
    temperature += 18.0F;
  }

  if (isIndoors(tile_pos)) {
    temperature += INDOOR_WARMTH;
  }

  // Best warm item carried (e.g. coat)
  float carried = 0.0F;
  auto& inventory = playerInventory();
  for (int i = 0; i < inventory.getSize(); i++) {
    const auto item = inventory.getStack(i)->getItem();
    if (item) {
      carried = std::max(
          carried,
          ItemTypeManager::getInfo(item->getType().getId()).carry_warmth);
    }
  }

  return temperature + carried;
}

void World::updateSurvival(float minutes) {
  if (minutes <= 0.0F) {
    return;
  }

  const float ambient = ambientTemperature(player_tile);

  setStat(state.hunger, -HUNGER_RATE * minutes);
  setStat(state.thirst, -THIRST_RATE * minutes *
                            (ambient > HOT_THRESHOLD ? 2.0F : 1.0F));

  if (ambient < COLD_THRESHOLD) {
    setStat(state.warmth, -(COLD_THRESHOLD - ambient) * CHILL_RATE * minutes);
  } else {
    setStat(state.warmth, WARM_UP_RATE * minutes);
  }

  float damage = 0.0F;
  if (state.hunger <= 0.0F) {
    damage += STARVE_DAMAGE;
  }
  if (state.thirst <= 0.0F) {
    damage += THIRST_DAMAGE;
  }
  if (state.warmth <= 0.0F) {
    damage += FREEZE_DAMAGE;
  }
  setStat(state.health, -damage * minutes);

  if (state.hunger > 40.0F && state.thirst > 40.0F && state.warmth > 40.0F) {
    setStat(state.health, HEAL_RATE * minutes);
  }

  // Warn once as each stat gets low
  auto warn = [&](float value, bool& warned, const std::string& message) {
    if (value < WARNING_LEVEL && !warned) {
      state.notify(message);
      warned = true;
    } else if (value >= WARNING_LEVEL + 10.0F) {
      warned = false;
    }
  };

  warn(state.hunger, warned_hunger, "You are hungry, eat something (C)");
  warn(state.thirst, warned_thirst, "You are thirsty, drink at water or well");
  warn(state.warmth, warned_cold, "You are freezing, find a fire");
  warn(state.health, warned_health, "You are badly hurt!");
}

/*
 * CREATURES
 */
void World::updateCreatures(float dt, const asw::Vec2i& player_pos) {
  for (auto const& creature : creatures) {
    creature->update(*this, player_pos, dt);
  }

  std::erase_if(creatures, [](auto const& creature) {
    if (creature->isGone()) {
      Graphics::Instance().remove(creature);
      return true;
    }
    return false;
  });

  // Deer and rabbits by day
  if (!state.isNight()) {
    wildlife_timer -= dt;
    if (wildlife_timer <= 0.0F) {
      wildlife_timer = static_cast<float>(random(6, 12));
      spawnWildlife();
    }
    return;
  }

  if (state.absoluteDay() < FIRST_WOLF_DAY) {
    return;
  }

  spawn_timer -= dt;
  if (spawn_timer > 0.0F) {
    return;
  }
  spawn_timer = static_cast<float>(random(8, 15));

  const int max_wolves =
      std::min(MAX_WOLVES, 1 + state.absoluteDay() / DAYS_PER_SEASON * 2 +
                               state.absoluteDay() / 5);
  const auto wolves = std::ranges::count_if(
      creatures, [](auto const& creature) { return creature->isHostile(); });
  if (wolves >= max_wolves) {
    return;
  }

  asw::Vec2i at;
  if (findSpawnTile(at) && !nearLitCampfire(at, CAMPFIRE_RADIUS)) {
    auto wolf = std::make_shared<Creature>(at * TILE_SIZE);
    creatures.push_back(wolf);
    Graphics::Instance().add(wolf, true);
  }
}

bool World::findSpawnTile(asw::Vec2i& at) {
  // Try a few spots out of sight
  for (int attempt = 0; attempt < 10; attempt++) {
    const float angle = static_cast<float>(random(0, 359)) * 3.14159F / 180.0F;
    const float distance = static_cast<float>(random(12, 18));
    at = player_tile + asw::Vec2i(static_cast<int>(std::cos(angle) * distance),
                                  static_cast<int>(std::sin(angle) * distance));

    if (!tile_map.inBounds(at) || tile_map.isSolidAt(at) ||
        !tile_map.getTileAt(at, LAYER_MIDGROUND) || isIndoors(at)) {
      continue;
    }

    const auto top = tile_map.getTileAt(at, LAYER_FOREGROUND);
    if (top && (top->getType().getId() == "tile:water" ||
                top->getType().getEncloses())) {
      continue;
    }

    return true;
  }

  return false;
}

void World::spawnWildlife() {
  const auto grazers = std::ranges::count_if(
      creatures, [](auto const& creature) { return !creature->isHostile(); });
  if (grazers >= MAX_WILDLIFE) {
    return;
  }

  asw::Vec2i at;
  if (!findSpawnTile(at)) {
    return;
  }

  // Deer live in woods, rabbits on open grass
  const bool woods = findTileNear(at, 2, [](const std::shared_ptr<Tile>& t) {
                       return t->getType().getId() == "tile:tree";
                     }) != nullptr;
  const auto ground = tile_map.getTileAt(at, LAYER_MIDGROUND);
  const bool grass = ground && ground->getType().getId() == "tile:grass";

  if (!woods && !grass) {
    return;
  }

  auto animal = std::make_shared<Creature>(
      at * TILE_SIZE, woods ? CreatureKind::Deer : CreatureKind::Rabbit);
  creatures.push_back(animal);
  Graphics::Instance().add(animal, true);
}

void World::clearCreatures() {
  for (auto const& creature : creatures) {
    Graphics::Instance().remove(creature);
  }
  creatures.clear();
  floating_texts.clear();
}

/*
 * WEATHER
 */
void World::updateWeather(float dt) {
  const bool raining = state.getWeather() == Weather::Rain;
  const bool snowing = state.getWeather() == Weather::Snow;

  weather_time += dt;

  // Age splashes, they finish even after the rain stops
  for (auto& s : splashes) {
    s.life -= dt;
  }
  std::erase_if(splashes, [](const Splash& s) { return s.life <= 0.0F; });

  // Drop particles left over from other weather
  std::erase_if(particles,
                [snowing](const Particle& p) { return p.snow != snowing; });

  if (!raining && !snowing) {
    particles.clear();
    return;
  }

  // New particle, anywhere on screen or just above it
  const auto respawn = [snowing](Particle& p, bool on_screen) {
    p.snow = snowing;

    if (snowing) {
      p.speed = static_cast<float>(random(25, 80));
      p.size = static_cast<float>(random(1, 3));
      p.phase = static_cast<float>(random(0, 628)) / 100.0F;
    } else {
      p.speed = static_cast<float>(random(350, 520));
      p.length = static_cast<float>(random(5, 14));
      p.alpha = static_cast<unsigned char>(random(80, 190));
      p.land_y =
          static_cast<float>(random(VIEWPORT_HEIGHT / 5, VIEWPORT_HEIGHT + 10));
    }

    const float top = on_screen && !snowing ? p.land_y : VIEWPORT_HEIGHT;
    p.pos = asw::Vec2f(
        static_cast<float>(random(0, VIEWPORT_WIDTH + 100)),
        on_screen ? static_cast<float>(random(0, static_cast<int>(top)))
                  : -static_cast<float>(random(10, 40)));
  };

  const size_t count = raining ? 220 : 140;
  while (particles.size() < count) {
    Particle p{asw::Vec2f(0.0F, 0.0F), 0.0F};
    respawn(p, true);
    particles.push_back(p);
  }

  constexpr size_t MAX_SPLASHES = 120;

  for (auto& p : particles) {
    p.pos.y += p.speed * dt;

    if (snowing) {
      // Drift with the wind and sway on the flake's own phase
      p.pos.x -= p.speed * dt * 0.1F;
      p.pos.x += std::sin(weather_time * 1.6F + p.phase) * 18.0F * dt;
    } else {
      p.pos.x -= p.speed * dt * 0.3F;
    }

    if (!snowing && p.pos.y >= p.land_y) {
      if (splashes.size() < MAX_SPLASHES) {
        splashes.push_back({asw::Vec2f(p.pos.x, p.land_y), SPLASH_LIFE});
      }
      respawn(p, false);
    } else if (p.pos.y > VIEWPORT_HEIGHT || p.pos.x < -20.0F ||
               p.pos.x > VIEWPORT_WIDTH + 120.0F) {
      respawn(p, false);
    }
  }
}

/*
 * DAYS
 */
void World::updateOrder() {
  auto& order = state.order;

  if (order.active && state.absoluteDay() > order.deadline) {
    order.active = false;
    countEvent("Store order expired");
  }

  if (order.active) {
    return;
  }

  // Ask for something that grows this season
  std::vector<std::string> wanted;
  switch (state.getSeason()) {
    case Season::Spring:
      wanted = {"item:berry", "item:carrot"};
      break;
    case Season::Summer:
      wanted = {"item:berry", "item:tomato", "item:lavender"};
      break;
    case Season::Fall:
      wanted = {"item:tomato", "item:carrot", "item:lavender"};
      break;
    case Season::Winter:
      wanted = {"item:egg", "item:cooked_egg", "item:pelt"};
      break;
  }

  const auto& id = wanted[random<size_t>(0, wanted.size() - 1)];
  const int value = std::max(1, ItemTypeManager::getInfo(id).value);

  order.item_id = id;
  order.quantity = std::clamp(120 / value + random(-2, 3), 3, 20);
  order.delivered = 0;
  order.reward = value * order.quantity * 3 / 2 + 25;
  order.deadline = state.absoluteDay() + 5;
  order.active = true;
}

void World::endDay(bool passed_out) {
  const bool fainted = state.health <= 0.0F;
  const int earned = state.getEarnedToday();
  const int finished_day = state.getDay();
  const auto finished_season = state.getSeason();

  hud.closeAll();
  clearCreatures();
  state.sleep_requested = false;

  // Crops grow, soil dries, animals produce
  day_events.clear();
  tile_map.dayEnd(*this);

  // Overnight
  setStat(state.hunger, -15.0F);
  setStat(state.thirst, -20.0F);
  state.warmth = MAX_STAT;

  state.nextDay();

  summary.clear();
  summary.push_back(std::format("{} {} is over", GameState::seasonName(
                                                     finished_season),
                                finished_day));
  summary.push_back("");

  if (fainted) {
    const int lost = state.lose(state.getCoins() / 4);
    state.health = 35.0F;
    state.energy = MAX_STAT / 2.0F;
    summary.push_back(std::format("You collapsed. Doctor bill: ${}", lost));
  } else if (passed_out) {
    const int lost = state.lose(std::min(state.getCoins() / 10, 200));
    state.energy = MAX_STAT / 2.0F;
    summary.push_back(
        std::format("You passed out outside. Lost ${} on the way home", lost));
  } else {
    state.energy = MAX_STAT;
    setStat(state.health, 20.0F);
  }

  summary.push_back(std::format("Earned today: ${}", earned));

  for (auto const& [event, count] : day_events) {
    summary.push_back(std::format("{}: {}", event, count));
  }

  // Rain waters the fields
  if (state.getWeather() == Weather::Rain) {
    tile_map.replaceAll("tile:plowed_soil", "tile:watered_soil");
    summary.push_back("It is raining, your fields are watered");
  } else if (state.getWeather() == Weather::Snow) {
    summary.push_back("It is snowing, stay warm");
  }

  if (state.getDay() == 1) {
    summary.push_back(std::format("{} has begun!", GameState::seasonName(
                                                       state.getSeason())));
  }

  // Goal at the end of the first spring
  if (!state.goal_checked && state.getSeason() != Season::Spring) {
    state.goal_checked = true;
    state.goal_met = state.getTotalEarned() >= GOAL_COINS;
    summary.push_back("");
    summary.push_back(
        state.goal_met
            ? std::format("GOAL MET! You earned ${} in Spring",
                          state.getTotalEarned())
            : std::format("Goal missed: ${} of ${}. Keep farming!",
                          state.getTotalEarned(), GOAL_COINS));
  }

  updateOrder();

  summary_open = true;
  warp_requested = true;
  save_requested = true;
}

bool World::takePlayerWarp(asw::Vec2i& to) {
  if (!warp_requested) {
    return false;
  }

  warp_requested = false;
  to = state.home * TILE_SIZE;
  return true;
}

bool World::takeSaveRequest() {
  return std::exchange(save_requested, false);
}

/*
 * HELPERS
 */
Inventory& World::playerInventory() {
  return *InterfaceTypeManager::getInterfaceByName("inventory").getInventory();
}

void World::dropItems(const std::string& id,
                      const asw::Vec2i& tile_pos,
                      int count) {
  if (id.empty()) {
    return;
  }

  // Items under solid tiles (fires, bushes) can not be picked up
  const auto at = tile_map.isSolidAt(tile_pos) ? openTileNear(tile_pos)
                                               : tile_pos;

  for (int i = 0; i < count; i++) {
    tile_map.placeItemAt(std::make_shared<Item>(id), at);
  }
}

void World::dropNear(const std::string& id, const asw::Vec2i& tile_pos) {
  dropItems(id, openTileNear(tile_pos));
}

asw::Vec2i World::openTileNear(const asw::Vec2i& tile_pos) {
  const std::array<asw::Vec2i, 8> around = {
      asw::Vec2i(0, 1),  asw::Vec2i(1, 0),  asw::Vec2i(-1, 0),
      asw::Vec2i(0, -1), asw::Vec2i(1, 1),  asw::Vec2i(-1, 1),
      asw::Vec2i(1, -1), asw::Vec2i(-1, -1)};

  for (auto const& offset : around) {
    const auto at = tile_pos + offset;
    if (tile_map.inBounds(at) && !tile_map.isSolidAt(at)) {
      return at;
    }
  }

  return tile_pos;
}

bool World::pickUpItems(const asw::Vec2i& tile_pos) {
  bool picked = false;

  while (auto item = tile_map.getItemAt(tile_pos)) {
    if (!playerInventory().addItem(item->itemPtr, 1)) {
      state.notify("Your bag is full");
      break;
    }

    tile_map.removeItem(item);
    picked = true;
  }

  return picked;
}

void World::giveItem(const std::string& id, int count) {
  if (!playerInventory().addItem(std::make_shared<Item>(id), count)) {
    dropItems(id, player_tile, count);
  }
}

void World::countEvent(const std::string& name, int amount) {
  day_events[name] += amount;
}

void World::addLight(const asw::Vec2i& tile_pos) {
  lights.push_back(tile_pos);
}

bool World::nearLitCampfire(const asw::Vec2i& tile_pos, int radius) const {
  return std::ranges::any_of(lights, [&](const asw::Vec2i& light) {
    return std::abs(light.x - tile_pos.x) <= radius &&
           std::abs(light.y - tile_pos.y) <= radius;
  });
}

void World::openStation(const std::string& window, int tier) {
  hud.open(window, 0.15F);
  hud.open("inventory", 0.8F);
  state.crafting_tier = tier;
}

bool World::itemCanUse(const ItemStack& held, const asw::Vec2i& tile_pos) {
  const auto item = held.getItem();
  if (!item) {
    return false;
  }

  return std::ranges::any_of(
      ItemTypeManager::getInfo(item->getType().getId()).actions,
      [&](const ItemAction& action) {
        return !action.fail && action.layer >= 0 &&
               actionMatches(action, tile_pos, held);
      });
}

std::shared_ptr<Tile> World::findTileNear(
    const asw::Vec2i& tile_pos,
    int radius,
    const std::function<bool(const std::shared_ptr<Tile>&)>& match) {
  for (int dx = -radius; dx <= radius; dx++) {
    for (int dy = -radius; dy <= radius; dy++) {
      auto tile = tile_map.getTileAt(tile_pos + asw::Vec2i(dx, dy),
                                     LAYER_FOREGROUND);
      if (tile && match(tile)) {
        return tile;
      }
    }
  }

  return nullptr;
}

/*
 * ROOMS
 */
int World::roomAt(const asw::Vec2i& tile_pos) const {
  if (!tile_map.inBounds(tile_pos) || rooms.empty()) {
    return 0;
  }

  return rooms[tile_pos.x + tile_pos.y * tile_map.getWidth()];
}

bool World::isIndoors(const asw::Vec2i& tile_pos) const {
  return roomAt(tile_pos) > 0;
}

void World::updateRooms() {
  // Only after a wall, window or door changed
  if (rooms_ready && rooms_version == tile_map.getEnclosureVersion()) {
    return;
  }
  rooms_ready = true;
  rooms_version = tile_map.getEnclosureVersion();

  const int width = tile_map.getWidth();
  const int height = tile_map.getHeight();
  rooms.assign(static_cast<size_t>(width * height), 0);

  auto encloses = [&](const asw::Vec2i& at) {
    const auto tile = tile_map.getTileAt(at, LAYER_FOREGROUND);
    return tile && tile->getType().getEncloses();
  };

  const std::array<asw::Vec2i, 4> around = {
      asw::Vec2i(0, -1), asw::Vec2i(1, 0), asw::Vec2i(0, 1), asw::Vec2i(-1, 0)};

  int next_room = 1;

  // Rooms start next to a door, a fill that escapes is outdoors
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      const auto door = tile_map.getTileAt(asw::Vec2i(x, y), LAYER_FOREGROUND);
      if (!door || door->getType().getId() != "tile:door") {
        continue;
      }

      for (auto const& step : around) {
        const auto start = asw::Vec2i(x, y) + step;
        if (!tile_map.inBounds(start) || encloses(start) || roomAt(start)) {
          continue;
        }

        std::vector<asw::Vec2i> filled{start};
        std::vector<bool> seen(static_cast<size_t>(width * height), false);
        seen[start.x + start.y * width] = true;
        bool closed = true;

        for (size_t i = 0; i < filled.size() && closed; i++) {
          for (auto const& next_step : around) {
            const auto next = filled[i] + next_step;
            if (!tile_map.inBounds(next)) {
              closed = false;
              break;
            }

            const auto index = static_cast<size_t>(next.x + next.y * width);
            if (seen[index] || encloses(next)) {
              continue;
            }

            seen[index] = true;
            filled.push_back(next);
            if (filled.size() > MAX_ROOM_TILES) {
              closed = false;
              break;
            }
          }
        }

        if (!closed) {
          continue;
        }

        for (auto const& at : filled) {
          rooms[at.x + at.y * width] = next_room;
        }
        next_room++;
      }
    }
  }
}

void World::drawRoofs() {
  if (rooms.empty()) {
    return;
  }

  const auto sheet = TileTypeManager::getSheet("placeholders");
  const auto cam = camera.getPosition();
  const auto& bounds = camera.getBounds();
  const int inside = roomAt(player_tile);

  const int x_1 = std::max(0, bounds.x_1 / TILE_SIZE - 1);
  const int y_1 = std::max(0, bounds.y_1 / TILE_SIZE - 1);
  const int x_2 = std::min(tile_map.getWidth() - 1, bounds.x_2 / TILE_SIZE + 1);
  const int y_2 =
      std::min(tile_map.getHeight() - 1, bounds.y_2 / TILE_SIZE + 1);

  // Covers the room and its walls, hidden while the player is inside
  auto covered = [&](int x, int y) {
    const int own = roomAt(asw::Vec2i(x, y));
    if (own > 0) {
      return own != inside;
    }

    const auto wall = tile_map.getTileAt(asw::Vec2i(x, y), LAYER_FOREGROUND);
    if (!wall || !wall->getType().getEncloses()) {
      return false;
    }

    for (int dx = -1; dx <= 1; dx++) {
      for (int dy = -1; dy <= 1; dy++) {
        const int room = roomAt(asw::Vec2i(x + dx, y + dy));
        if (room > 0 && room != inside) {
          return true;
        }
      }
    }
    return false;
  };

  for (int x = x_1; x <= x_2; x++) {
    for (int y = y_1; y <= y_2; y++) {
      if (!covered(x, y)) {
        continue;
      }

      asw::draw::stretch_sprite_blit(
          sheet,
          asw::Quadf(ROOF_X * TILE_SIZE, ROOF_Y * TILE_SIZE, TILE_SIZE,
                     TILE_SIZE),
          asw::Quadf(x * TILE_SIZE - cam.x, y * TILE_SIZE - cam.y - TILE_SIZE,
                     TILE_SIZE, TILE_SIZE));
    }
  }
}

/*
 * RECIPE DISCOVERY
 */
void World::updateDiscovery() {
  auto& inventory = playerInventory();
  std::vector<std::string> found;

  for (int i = 0; i < inventory.getSize(); i++) {
    const auto item = inventory.getStack(i)->getItem();
    if (item && !state.known_items.contains(item->getType().getId())) {
      found.push_back(item->getType().getId());
    }
  }

  if (found.empty()) {
    discovery_primed = true;
    return;
  }

  auto visible = [&](const Recipe& recipe) {
    return std::ranges::any_of(recipe.inputs, [&](auto const& input) {
      return state.known_items.contains(input.first);
    });
  };

  std::vector<const Recipe*> hidden;
  for (auto const& recipe : RecipeManager::getRecipes()) {
    if (!visible(recipe)) {
      hidden.push_back(&recipe);
    }
  }

  state.known_items.insert(found.begin(), found.end());

  // Starting items and loaded saves do not announce recipes
  if (!discovery_primed) {
    discovery_primed = true;
    return;
  }

  int learned = 0;
  for (auto const* recipe : hidden) {
    if (visible(*recipe)) {
      learned++;
    }
  }

  if (learned > 0) {
    state.notify(std::format("{} new recipe{}! Press R to see them", learned,
                             learned == 1 ? "" : "s"));
  }
}

void World::openShop() {
  hud.open("shop", 0.15F);
  hud.open("inventory", 0.8F);
}

/*
 * STATUS
 */
void World::drawStatus(const asw::Vec2i& ui_size) const {
  const auto shadow = asw::Color(0, 0, 0, 180);

  auto text = [&](const std::string& value, int x, int y,
                  asw::Color color = asw::Color(255, 255, 255),
                  asw::TextJustify justify = asw::TextJustify::Left) {
    asw::draw::text(font, value, asw::Vec2f(x + 1, y + 1), shadow, justify);
    asw::draw::text(font, value, asw::Vec2f(x, y), color, justify);
  };

  // Clock and money
  asw::draw::rect_fill(asw::Quadf(4, 4, 96, 34), asw::Color(0, 0, 0, 120));
  text(state.dateString(), 8, 6);
  text(state.timeString() + "  " + state.weatherName(), 8, 16);
  text(std::format("${}", state.getCoins()), 8, 26,
       asw::Color(255, 220, 90));

  // Stat bars
  struct Bar {
    const char* label;
    float value;
    asw::Color color;
  };

  const std::array<Bar, 5> bars = {{
      {"HP", state.health, asw::Color(220, 60, 60)},
      {"Food", state.hunger, asw::Color(230, 150, 50)},
      {"Water", state.thirst, asw::Color(70, 140, 230)},
      {"Warm", state.warmth, asw::Color(240, 210, 80)},
      {"Energy", state.energy, asw::Color(90, 200, 90)},
  }};

  const int bar_x = ui_size.x - 70;
  asw::draw::rect_fill(asw::Quadf(bar_x - 34, 4, 100, 58),
                       asw::Color(0, 0, 0, 120));

  for (size_t i = 0; i < bars.size(); i++) {
    const int y = 7 + static_cast<int>(i) * 11;
    text(bars[i].label, bar_x - 31, y - 2);
    asw::draw::rect_fill(asw::Quadf(bar_x, y, 60, 6), asw::Color(40, 40, 40));
    asw::draw::rect_fill(asw::Quadf(bar_x, y, 60.0F * bars[i].value / MAX_STAT,
                                    6),
                         bars[i].value < WARNING_LEVEL
                             ? asw::Color(255, 60, 60)
                             : bars[i].color);
  }

  // Temperature where the player stands
  text(std::format("{:.0f}C", ambientTemperature(player_tile)), bar_x - 31,
       64);

  // Goal and order
  int line_y = 42;
  if (!state.goal_checked) {
    text(std::format("Goal: ${}/{} by end of Spring", state.getTotalEarned(),
                     GOAL_COINS),
         6, line_y, asw::Color(200, 230, 255));
    line_y += 10;
  }

  if (state.order.active) {
    text(std::format("Order: {} {}/{} for ${} (day {})",
                     ItemTypeManager::getItem(state.order.item_id).getName(),
                     state.order.delivered, state.order.quantity,
                     state.order.reward, state.order.deadline),
         6, line_y, asw::Color(160, 210, 255));
  }

  // Messages above the hotbar
  map_messages.draw(6, ui_size.y - 34);
}

void World::drawSummary(const asw::Vec2i& ui_size) const {
  if (!summary_open) {
    return;
  }

  const int width = 260;
  const int height = static_cast<int>(summary.size()) * 11 + 30;
  const int x = (ui_size.x - width) / 2;
  const int y = (ui_size.y - height) / 2;

  asw::draw::rect_fill(asw::Quadf(0, 0, ui_size.x, ui_size.y),
                       asw::Color(0, 0, 0, 120));
  asw::draw::rect_fill(asw::Quadf(x, y, width, height),
                       asw::Color(40, 50, 40, 235));
  asw::draw::rect(asw::Quadf(x, y, width, height), asw::Color(150, 200, 120));

  for (size_t i = 0; i < summary.size(); i++) {
    asw::draw::text(font, summary[i],
                    asw::Vec2f(x + width / 2, y + 10 + static_cast<int>(i) * 11),
                    asw::Color(255, 255, 255), asw::TextJustify::Center);
  }

  asw::draw::text(font, "Press Space to continue",
                  asw::Vec2f(x + width / 2, y + height - 14),
                  asw::Color(200, 230, 160), asw::TextJustify::Center);
}

// Get map
TileMap& World::getMap() {
  return this->tile_map;
}

// Get messenger
Messenger& World::getMessenger() {
  return this->map_messages;
}

Hud& World::getHud() {
  return hud;
}

Camera& World::getCamera() {
  return camera;
}

GameState& World::getState() {
  return state;
}

const GameState& World::getState() const {
  return state;
}
