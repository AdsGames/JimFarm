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
  if (!sprite) {
    return;
  }

  sprites.insert(sprite);
}

// Remove sprites
void Graphics::remove(const unsigned int sprite_id) {
  auto it = std::find_if(sprites.begin(), sprites.end(),
                         [sprite_id](const std::shared_ptr<Sprite>& sprite) {
                           return sprite->getSpriteId() == sprite_id;
                         });

  if (it != sprites.end()) {
    sprites.erase(it);
  }
}

void Graphics::draw(int x_1, int y_1, int x_2, int y_2) const {
  for (auto const& sprite : sprites) {
    if (sprite != nullptr) {
      sprite->draw(x_1, y_1, x_2, y_2);
    }
  }
}
