#include "Graphics.h"

#include <algorithm>

Graphics* Graphics::instance = nullptr;

bool sortDrawableByZ(Sprite* A, Sprite* B) {
  return (*A < *B);
}

Graphics::Graphics() {
  should_sort = false;
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
  if (should_sort)
    sort();
}

// Remove sprites
void Graphics::remove(Sprite* sprite) {
  if (!sprite) {
    return;
  }
  sprites.erase(std::remove(sprites.begin(), sprites.end(), sprite),
                sprites.end());
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
  sort();
}

void Graphics::draw(BITMAP* buffer, int x_1, int y_1, int x_2, int y_2) {
  for (auto const& sprite : sprites) {
    if (sprite != nullptr) {
      sprite->draw(buffer, x_1, y_1, x_2, y_2);
    }
  }
}
