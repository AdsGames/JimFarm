#include "./Camera.h"

Camera::Camera(const Quad<int>& bounds, const Quad<int>& outer_bounds)
    : bounds(bounds),
      outer_bounds(outer_bounds),
      width(bounds.getWidth()),
      height(bounds.getHeight()) {}

void Camera::setZoom(float z) {
  this->zoom = z;

  auto new_width = static_cast<int>(1.0f / z * static_cast<float>(this->width));
  auto new_height =
      static_cast<int>(1.0f / z * static_cast<float>(this->height));

  this->bounds.x_1 = this->bounds.x_1 - (new_width - this->width) / 2;
  this->bounds.y_1 = this->bounds.y_1 - (new_height - this->height) / 2;
  this->bounds.x_2 = this->bounds.x_1 + new_width;
  this->bounds.y_2 = this->bounds.y_1 + new_height;
}

float Camera::getZoom() const {
  return zoom;
}

const Quad<int>& Camera::getBounds() const {
  return bounds;
}

void Camera::pan(Vec2<int> pos_to) {
  if (pos_to.x < outer_bounds.x_1) {
    pos_to.x = outer_bounds.x_1;
  } else if (pos_to.x + this->bounds.getWidth() > outer_bounds.x_2) {
    pos_to.x = outer_bounds.x_2 - this->bounds.getWidth();
  }

  if (pos_to.y < outer_bounds.y_1) {
    pos_to.y = outer_bounds.y_1;
  } else if (pos_to.y + this->bounds.getHeight() > outer_bounds.y_2) {
    pos_to.y = outer_bounds.y_2 - this->bounds.getHeight();
  }

  int prev_w = this->bounds.getWidth();
  int prev_h = this->bounds.getHeight();

  this->bounds.x_1 = pos_to.x;
  this->bounds.y_1 = pos_to.y;
  this->bounds.x_2 = pos_to.x + prev_w;
  this->bounds.y_2 = pos_to.y + prev_h;
}

Vec2<int> Camera::getSize() const {
  return Vec2<int>(this->bounds.getWidth(), this->bounds.getHeight());
};

Vec2<int> Camera::getPosition() const {
  return Vec2<int>(this->bounds.x_1, this->bounds.y_1);
};

Vec2<int> Camera::getCenter() const {
  return Vec2<int>(this->bounds.x_1 + this->bounds.getWidth() / 2,
                   this->bounds.y_1 + this->bounds.getHeight() / 2);
};
