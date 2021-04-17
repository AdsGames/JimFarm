#include "Graphics.h"

#include <algorithm>
#include <utility>

Graphics* Graphics::instance = nullptr;

bool sortDrawableByZ(Sprite* A, Sprite* B) {
  return (*A < *B);
}

Graphics::Graphics() {
  should_sort = false;
  need_sort = false;
}

// Get instance
Graphics* Graphics::Instance() {
  if (!instance)
    instance = new Graphics();

  return instance;
}

// Add sprites
void Graphics::add(Sprite* sprite) {
  if (!sprite) {
    return;
  }

  sprites.push_back(sprite);

  if (should_sort) {
    sort();
  } else {
    need_sort = true;
  }
}

// Remove sprites
void Graphics::remove(Sprite* sprite) {
  if (!sprite) {
    return;
  }

  auto it = std::find(sprites.begin(), sprites.end(), sprite);

  if (it == sprites.end()) {
    return;
  }

  // Use back-swapping
  *it = std::move(sprites.back());
  sprites.pop_back();

  if (should_sort) {
    sort();
  } else {
    need_sort = true;
  }
}

// Sort sprites
void Graphics::sort() {
  std::sort(sprites.begin(), sprites.end(), sortDrawableByZ);
}

void Graphics::disableSort() {
  should_sort = false;
}

void Graphics::enableSort() {
  should_sort = true;
  if (need_sort) {
    sort();
  }
}

void Graphics::draw(BITMAP* buffer, int x_1, int y_1, int x_2, int y_2) {
  for (auto const& sprite : sprites) {
    if (sprite != nullptr) {
      sprite->draw(buffer, x_1, y_1, x_2, y_2);
    }
  }
}
