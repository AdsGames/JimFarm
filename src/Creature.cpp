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

Creature::Creature(const asw::Vec2i& pos)
    : Sprite(pos, 2),
      fpos(static_cast<float>(pos.x), static_cast<float>(pos.y)) {}

asw::Vec2i Creature::getTile() const {
  return asw::Vec2i((pos.x + TILE_SIZE / 2) / TILE_SIZE,
                    (pos.y + TILE_SIZE / 2) / TILE_SIZE);
}

void Creature::draw(const Camera& camera) const {
  // Walk frames and a quick hop while moving, slow breathing when idle
  const int frame = moving ? static_cast<int>(anim * 8.0F) % 2 : 0;
  const int bob =
      moving ? anim::walkBob(getSpriteId()) : anim::idleBob(getSpriteId());

  const auto source =
      asw::Quadf((2 + frame) * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE);
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

  // Wolves do not swim
  const auto top = map.getTileAt(tile, LAYER_FOREGROUND);
  return top && top->getType().getId() == "tile:water";
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
  auto& state = world.getState();

  attack_cooldown -= dt;
  flee_timer -= dt;
  hurt_timer -= dt;
  anim += dt;
  moving = false;

  // Leave at dawn
  if (!state.isNight()) {
    leaving = true;
  }

  const auto me = centre(pos);
  const auto player = centre(player_pos);
  const auto to_player = asw::Vec2f(player.x - me.x, player.y - me.y);
  const float player_distance = length(to_player);

  // Leaving, run from the player until out of sight
  if (leaving) {
    move(world, normalize(to_player) * -FLEE_SPEED * dt);
    if (player_distance > SIGHT_TILES * 2 * TILE_SIZE) {
      gone = true;
    }
    return;
  }

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

  // Wander
  wander_timer -= dt;
  if (wander_timer <= 0.0F) {
    wander_timer = static_cast<float>(random(1, 3));
    wander_dir = normalize(asw::Vec2f(static_cast<float>(random(-10, 10)),
                                      static_cast<float>(random(-10, 10))));
  }
  move(world, wander_dir * WANDER_SPEED * dt);
}

bool Creature::hit(World& world, int damage, const asw::Vec2i& from) {
  hp -= damage;
  hurt_timer = HIT_FLASH;
  flee_timer = 0.8F;

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
