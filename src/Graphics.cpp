#include "Graphics.h"

#include <vector>

// Get instance
Graphics& Graphics::Instance() {
  static Graphics instance;
  return instance;
}

// Add sprites
void Graphics::add(const std::shared_ptr<Sprite>& sprite, bool dynamic) {
  const auto id = sprite->getSpriteId();

  // Re-adding refreshes the sort key
  erase(id);

  auto [it, _] = sorted_sprites.insert(
      {sprite->getZ(), sprite->getPosition().y, id, sprite});
  sprites[id] = it;

  if (dynamic) {
    dynamic_sprites.insert(id);
  }
}

// Remove sprites
void Graphics::remove(const std::shared_ptr<Sprite>& sprite) {
  const auto id = sprite->getSpriteId();
  erase(id);
  dynamic_sprites.erase(id);
}

void Graphics::erase(unsigned int id) {
  auto found = sprites.find(id);
  if (found == sprites.end()) {
    return;
  }

  sorted_sprites.erase(found->second);
  sprites.erase(found);
}

void Graphics::prune() {
  // Dynamic sprites move, so re-insert them with a fresh sort key
  std::vector<unsigned int> dead;

  for (auto const& id : dynamic_sprites) {
    auto found = sprites.find(id);
    if (found == sprites.end()) {
      dead.push_back(id);
      continue;
    }

    auto sprite = found->second->sprite.lock();
    sorted_sprites.erase(found->second);

    if (!sprite) {
      sprites.erase(found);
      dead.push_back(id);
      continue;
    }

    found->second = sorted_sprites
                        .insert({sprite->getZ(), sprite->getPosition().y, id,
                                 sprite})
                        .first;
  }

  for (auto const& id : dead) {
    dynamic_sprites.erase(id);
  }
}

void Graphics::draw(const Camera& camera) const {
  auto& camera_bounds = camera.getBounds();

  for (auto const& entry : sorted_sprites) {
    auto sprite = entry.sprite.lock();
    if (!sprite) {
      continue;
    }

    if (camera_bounds.intersects(sprite->getDrawBounds())) {
      sprite->draw(camera);
    }
  }
}
