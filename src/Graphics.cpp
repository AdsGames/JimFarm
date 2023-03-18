#include "Graphics.h"

#include <algorithm>
#include <iostream>

std::shared_ptr<Graphics> Graphics::instance = nullptr;

// Get instance
std::shared_ptr<Graphics> Graphics::Instance() {
  if (!instance) {
    instance = std::make_shared<Graphics>();
  }

  return instance;
}

// Add sprites
void Graphics::add(std::shared_ptr<Sprite> sprite) {
  sprites[sprite->getSpriteId()] = sprite;
}

// Remove sprites
void Graphics::remove(std::shared_ptr<Sprite> sprite) {
  sprites.erase(sprite->getSpriteId());
}

void Graphics::prune() {
  std::erase_if(sprites,
                [](const auto& sprite) { return sprite.second.expired(); });
}

void Graphics::draw(const Camera& camera) const {
  auto sorted_sprites = std::set<std::weak_ptr<Sprite>, SpriteCmp>{};
  auto& camera_bounds = camera.getBounds();

  for (auto const& [index, sprite] : sprites) {
    if (sprite.expired()) {
      continue;
    }

    auto spr_pos = sprite.lock()->getPosition();
    Quad<int> quad(spr_pos.x, spr_pos.y, spr_pos.x + 64, spr_pos.y + 64);

    if (camera_bounds.intersects(quad)) {
      sorted_sprites.insert(sprite);
    }
  }

  for (auto const& sprite : sorted_sprites) {
    sprite.lock()->draw(camera);
  }
}
