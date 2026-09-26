#include "Creature.h"

#include <cmath>

#include "World.h"
#include "manager/SoundManager.h"
#include "manager/TileTypeManager.h"
#include "utility/Anim.h"
#include "utility/Shadow.h"
#include "utility/Tools.h"

asw::Texture Creature::sheet{nullptr};

namespace {
constexpr float CHASE_SPEED = 52.0F;
constexpr float WANDER_SPEED = 24.0F;
constexpr float FLEE_SPEED = 70.0F;
constexpr float SIGHT_TILES = 9.0F;
constexpr float BITE_RANGE = 13.0F;
constexpr float BITE_DAMAGE = 12.0F;
constexpr float BITE_COOLDOWN = 1.5F;
constexpr int PREY_RANGE = 8;

// Grazers bolt once the player is this close, calm down once the player is
// this far, and leave when very far away
constexpr float SHY_TILES = 5.0F;
constexpr float CALM_TILES = 10.0F;
constexpr float ROAM_TILES = 32.0F;

// Longest run to one flee spot before picking another, seconds
constexpr float MAX_FLEE_TIME = 5.0F;

// Close enough to the flee spot, pixels
constexpr float ARRIVE_DISTANCE = 4.0F;

// Per kind stats
struct KindInfo {
  int hp;
  float flee_speed;
  float wander_speed;
  int sprite_x;
  int sprite_y;

  // How far a flee spot is, tiles
  int flee_min;
  int flee_max;
};

KindInfo infoOf(CreatureKind kind) {
  switch (kind) {
    case CreatureKind::Deer:
      return {4, 62.0F, 14.0F, 0, 1, 9, 13};
    case CreatureKind::Rabbit:
      return {1, 78.0F, 20.0F, 2, 1, 6, 9};
    case CreatureKind::Wolf:
      break;
  }
  return {3, FLEE_SPEED, WANDER_SPEED, 2, 0, 0, 0};
}
constexpr float HIT_FLASH = 0.15F;

float length(const asw::Vec2f& v) {
  return std::sqrt(v.x * v.x + v.y * v.y);
}

asw::Vec2f normalize(const asw::Vec2f& v) {
  const float len = length(v);
  if (len <= 0.001F) {
    return asw::Vec2f(0, 0);
  }
  return asw::Vec2f(v.x / len, v.y / len);
}

asw::Vec2f centre(const asw::Vec2i& pos) {
  return asw::Vec2f(pos.x + TILE_SIZE / 2.0F, pos.y + TILE_SIZE / 2.0F);
}
}  // namespace

void Creature::loadImages() {
  sheet = TileTypeManager::getSheet("placeholders");
}

Creature::Creature(const asw::Vec2i& pos, CreatureKind kind)
    : Sprite(pos, 2),
      kind(kind),
      fpos(static_cast<float>(pos.x), static_cast<float>(pos.y)),
      hp(infoOf(kind).hp) {}

asw::Vec2i Creature::getTile() const {
  return asw::Vec2i((pos.x + TILE_SIZE / 2) / TILE_SIZE,
                    (pos.y + TILE_SIZE / 2) / TILE_SIZE);
}

void Creature::draw(const Camera& camera) const {
  // Walk frames and a quick hop while moving, slow breathing when idle
  const int frame = moving ? static_cast<int>(anim * 8.0F) % 2 : 0;
  const int bob =
      moving ? anim::walkBob(getSpriteId()) : anim::idleBob(getSpriteId());

  const auto info = infoOf(kind);
  const auto source = asw::Quadf((info.sprite_x + frame) * TILE_SIZE,
                                 info.sprite_y * TILE_SIZE, TILE_SIZE,
                                 TILE_SIZE);
  const auto dest =
      asw::Quadf(pos.x - camera.getPosition().x,
                 pos.y - camera.getPosition().y + bob, TILE_SIZE, TILE_SIZE);

  shadow::draw(pos.x - camera.getPosition().x + TILE_SIZE / 2.0F,
               pos.y - camera.getPosition().y + TILE_SIZE - 2.0F, 12.0F);

  // Hit flash, white then red. Tint only darkens, so the white half draws
  // the sprite again additively to brighten it.
  const bool flash_white = hurt_timer > HIT_FLASH / 2.0F;
  const bool flash_red = hurt_timer > 0.0F && !flash_white;

  if (flash_red) {
    asw::draw::set_tint(sheet, asw::Color(255, 70, 70));
  }

  asw::draw::stretch_sprite_blit(sheet, source, dest);

  if (flash_red) {
    asw::draw::set_tint(sheet, asw::color::white);
  }

  if (flash_white) {
    asw::draw::set_blend_mode(sheet, asw::BlendMode::Add);
    asw::draw::stretch_sprite_blit(sheet, source, dest);
    asw::draw::stretch_sprite_blit(sheet, source, dest);
    asw::draw::set_blend_mode(sheet, asw::BlendMode::Blend);
  }
}

bool Creature::blocked(World& world, const asw::Vec2f& at) const {
  auto& map = world.getMap();
  const auto tile =
      asw::Vec2i(static_cast<int>(at.x + TILE_SIZE / 2.0F) / TILE_SIZE,
                 static_cast<int>(at.y + TILE_SIZE / 2.0F) / TILE_SIZE);

  if (!map.inBounds(tile) || map.isSolidAt(tile)) {
    return true;
  }

  // Animals do not swim or open doors
  const auto top = map.getTileAt(tile, LAYER_FOREGROUND);
  return top && (top->getType().getId() == "tile:water" ||
                 top->getType().getEncloses());
}

bool Creature::move(World& world, const asw::Vec2f& delta) {
  bool moved = false;

  const auto next_x = asw::Vec2f(fpos.x + delta.x, fpos.y);
  if (!blocked(world, next_x)) {
    fpos = next_x;
    moved = true;
  }

  const auto next_y = asw::Vec2f(fpos.x, fpos.y + delta.y);
  if (!blocked(world, next_y)) {
    fpos = next_y;
    moved = true;
  }

  pos = asw::Vec2i(static_cast<int>(fpos.x), static_cast<int>(fpos.y));
  moving = moving || moved;
  return moved;
}

bool Creature::findPrey(World& world, asw::Vec2i& prey) const {
  auto& map = world.getMap();
  const auto here = getTile();
  int best = PREY_RANGE * PREY_RANGE + 1;

  for (int dx = -PREY_RANGE; dx <= PREY_RANGE; dx++) {
    for (int dy = -PREY_RANGE; dy <= PREY_RANGE; dy++) {
      const auto at = here + asw::Vec2i(dx, dy);
      if (!map.inBounds(at)) {
        continue;
      }

      const auto tile = map.getTileAt(at, LAYER_FOREGROUND);
      if (tile && tile->getType().getId() == "tile:chicken" &&
          dx * dx + dy * dy < best) {
        best = dx * dx + dy * dy;
        prey = at;
      }
    }
  }

  return best <= PREY_RANGE * PREY_RANGE;
}

void Creature::update(World& world, const asw::Vec2i& player_pos, float dt) {
  attack_cooldown -= dt;
  flee_timer -= dt;
  hurt_timer -= dt;
  anim += dt;
  moving = false;

  // Wolves leave at dawn, grazers at night
  if (world.getState().isNight() != isHostile()) {
    leaving = true;
  }

  const auto me = centre(pos);
  const auto player = centre(player_pos);
  const auto to_player = asw::Vec2f(player.x - me.x, player.y - me.y);
  const float player_distance = length(to_player);

  // Leaving, run from the player until out of sight
  if (leaving) {
    move(world, normalize(to_player) * -infoOf(kind).flee_speed * dt);
    if (player_distance > SIGHT_TILES * 2 * TILE_SIZE) {
      gone = true;
    }
    return;
  }

  if (isHostile()) {
    updateHunter(world, to_player, dt);
  } else {
    updateGrazer(world, to_player, dt);
  }
}

void Creature::updateHunter(World& world,
                            const asw::Vec2f& to_player,
                            float dt) {
  auto& state = world.getState();
  const auto me = centre(pos);
  const float player_distance = length(to_player);

  // Afraid of fire and recently hurt
  if (flee_timer > 0.0F || world.nearLitCampfire(getTile(), CAMPFIRE_RADIUS)) {
    move(world, normalize(to_player) * -FLEE_SPEED * dt);
    return;
  }

  // Hunt the player when close
  if (player_distance < SIGHT_TILES * TILE_SIZE) {
    if (player_distance < BITE_RANGE) {
      if (attack_cooldown <= 0.0F) {
        attack_cooldown = BITE_COOLDOWN;
        state.health = std::max(0.0F, state.health - BITE_DAMAGE);
        state.notify("A wolf bites you!");
        SoundManager::play("error");
      }
      return;
    }

    move(world, normalize(to_player) * CHASE_SPEED * dt);
    return;
  }

  // Otherwise go for chickens
  asw::Vec2i prey;
  if (findPrey(world, prey)) {
    const auto target = centre(prey * TILE_SIZE);
    const auto to_prey = asw::Vec2f(target.x - me.x, target.y - me.y);

    if (length(to_prey) < BITE_RANGE + TILE_SIZE / 2.0F) {
      if (auto chicken = world.getMap().getTileAt(prey, LAYER_FOREGROUND)) {
        world.getMap().removeTile(chicken);
        state.notify("A wolf took one of your chickens!");
        world.countEvent("Chickens taken by wolves");
        leaving = true;
      }
      return;
    }

    // Fences stop the hunt
    if (move(world, normalize(to_prey) * CHASE_SPEED * dt)) {
      return;
    }
  }

  wander(world, WANDER_SPEED, dt);
}

void Creature::updateGrazer(World& world,
                            const asw::Vec2f& to_player,
                            float dt) {
  const float player_distance = length(to_player);
  const auto info = infoOf(kind);
  const auto away = normalize(to_player) * -1.0F;

  // Wandered too far from the player, gone for good
  if (!fleeing && player_distance > ROAM_TILES * TILE_SIZE) {
    gone = true;
    return;
  }

  // Startled by the player coming close or a hit, bolt to a spot away
  if (startled || (!fleeing && player_distance < SHY_TILES * TILE_SIZE)) {
    startled = false;
    pickFleeTarget(world, away);
  }

  if (!fleeing) {
    wander(world, info.wander_speed, dt);
    return;
  }

  flee_time += dt;
  const auto to_target = flee_target - fpos;
  const auto before = fpos;

  if (length(to_target) > ARRIVE_DISTANCE) {
    move(world, normalize(to_target) * info.flee_speed * dt);
  }

  const bool arrived = length(flee_target - fpos) <= ARRIVE_DISTANCE;
  const bool stuck = length(fpos - before) < 0.01F && !arrived;

  if (arrived || stuck || flee_time > MAX_FLEE_TIME) {
    // Far enough, calm down and graze for a moment
    if (player_distance >= CALM_TILES * TILE_SIZE) {
      fleeing = false;
      wander_dir = asw::Vec2f(0, 0);
      wander_timer = static_cast<float>(random(2, 4));
      return;
    }

    // Still too close, keep running
    pickFleeTarget(world, away);
  }
}

bool Creature::canStandAt(World& world, const asw::Vec2f& at) const {
  return !blocked(world, at);
}

void Creature::pickFleeTarget(World& world, const asw::Vec2f& away) {
  const auto info = infoOf(kind);
  const float heading = std::atan2(away.y, away.x);

  fleeing = true;
  flee_time = 0.0F;

  // Best reachable spot roughly away from the player, spread so a herd
  // scatters instead of running in a line
  float best_score = -1.0F;
  for (int attempt = 0; attempt < 12; attempt++) {
    const float spread =
        static_cast<float>(random(-70, 70)) * 3.14159F / 180.0F;
    const float distance =
        static_cast<float>(random(info.flee_min, info.flee_max) * TILE_SIZE);
    const auto candidate =
        fpos + asw::Vec2f(std::cos(heading + spread) * distance,
                          std::sin(heading + spread) * distance);

    if (!canStandAt(world, candidate)) {
      continue;
    }

    // Straighter away is better, and so is a clear first step
    const float score = std::cos(spread) +
                        (canStandAt(world, fpos + normalize(candidate - fpos) *
                                                      static_cast<float>(
                                                          TILE_SIZE))
                             ? 1.0F
                             : 0.0F);
    if (score > best_score) {
      best_score = score;
      flee_target = candidate;
    }
  }

  // Nowhere good, just run straight away
  if (best_score < 0.0F) {
    flee_target = fpos + away * static_cast<float>(info.flee_min * TILE_SIZE);
  }
}

void Creature::wander(World& world, float speed, float dt) {
  wander_timer -= dt;
  if (wander_timer <= 0.0F) {
    wander_timer = static_cast<float>(random(1, 3));

    // Grazers often stop to eat
    if (!isHostile() && random(0, 2) == 0) {
      wander_dir = asw::Vec2f(0, 0);
    } else {
      wander_dir = normalize(asw::Vec2f(static_cast<float>(random(-10, 10)),
                                        static_cast<float>(random(-10, 10))));
    }
  }
  move(world, wander_dir * speed * dt);
}

bool Creature::hit(World& world, int damage, const asw::Vec2i& from) {
  hp -= damage;
  hurt_timer = HIT_FLASH;
  flee_timer = 0.8F;
  startled = !isHostile();

  // Knock back
  const auto me = centre(pos);
  const auto source = centre(from);
  move(world, normalize(asw::Vec2f(me.x - source.x, me.y - source.y)) * 10.0F);

  if (hp <= 0) {
    gone = true;
    return true;
  }

  return false;
}
