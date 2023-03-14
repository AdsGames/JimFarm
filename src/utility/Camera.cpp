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

void Camera::translate(int x, int y) {
  this->bounds.x_1 += x;
  this->bounds.y_1 += y;
  this->bounds.x_2 += x;
  this->bounds.y_2 += y;
}

void Camera::pan(int x, int y) {
  if (x < outer_bounds.x_1) {
    x = outer_bounds.x_1;
  } else if (x + this->bounds.getWidth() > outer_bounds.x_2) {
    x = outer_bounds.x_2 - this->bounds.getWidth();
  }

  if (y < outer_bounds.y_1) {
    y = outer_bounds.y_1;
  } else if (y + this->bounds.getHeight() > outer_bounds.y_2) {
    y = outer_bounds.y_2 - this->bounds.getHeight();
  }

  int width = this->bounds.getWidth();
  int height = this->bounds.getHeight();

  this->bounds.x_1 = x;
  this->bounds.y_1 = y;
  this->bounds.x_2 = x + width;
  this->bounds.y_2 = y + height;
}

int Camera::getWidth() const {
  return this->bounds.getWidth();
};

int Camera::getHeight() const {
  return this->bounds.getHeight();
};

int Camera::getX() const {
  return this->bounds.x_1;
};

int Camera::getY() const {
  return this->bounds.y_1;
};