#include "Graphics.h"

#include <algorithm>

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
  sorted_sprites.insert(sprite);
}

// Remove sprites
void Graphics::remove(std::shared_ptr<Sprite> sprite) {
  sprites.erase(sprite->getSpriteId());
}

void Graphics::prune() {
  std::erase_if(sprites,
                [](const auto& sprite) { return sprite.second.expired(); });

  std::erase_if(sorted_sprites, [this](const auto& sprite) {
    return sprite.expired() ||
           !this->sprites.contains(sprite.lock()->getSpriteId());
  });
}

void Graphics::draw(const Camera& camera) const {
  auto& camera_bounds = camera.getBounds();

  for (auto const& sprite : sorted_sprites) {
    if (sprite.expired()) {
      continue;
    }

    auto spr_pos = sprite.lock()->getPosition();
    Quad<int> quad(spr_pos.x, spr_pos.y, spr_pos.x + 64, spr_pos.y + 64);

    if (camera_bounds.intersects(quad)) {
      sprite.lock()->draw(camera);
    }
  }
}
