#pragma once

#include <cmath>

using VectorT = double;

template <typename T>
class Vector2D {
 public:
  T x{};
  T y{};

  constexpr Vector2D() = default;
  constexpr Vector2D(T _x, T _y) : x(_x), y(_y){};

  static Vector2D Zero() {
    return {0.0, 0.0};
  }
  static Vector2D One() {
    return {1.0, 1.0};
  }

  constexpr Vector2D operator+(const Vector2D rhs) const {
    return {this->x + rhs.x, this->y + rhs.y};
  }

  constexpr Vector2D operator-(const Vector2D rhs) const {
    return {this->x - rhs.x, this->y - rhs.y};
  }

  constexpr Vector2D operator*(const T mul) const {
    return {this->x * mul, this->y * mul};
  }

  constexpr Vector2D operator/(const T div) const {
    return {this->x / div, this->y / div};
  }

  T Dot(const Vector2D value) {
    return x * value.x + y * value.y;
  }

  T Mag() const {
    return sqrt(this->x * this->x + this->y * this->y);
  }

  constexpr T MagSquared() const {
    return this->x * this->x + this->y * this->y;
  }

  void Normalize() {
    auto magnitude = Mag();
    if (magnitude > 0.0) {
      this->x = this->x / magnitude;
      this->y = this->y / magnitude;
    }
  }

  void SetMag(T val) {
    Normalize();
    *this = *this * val;
  }

  void Limit(T value) {
    if (MagSquared() > (value * value)) SetMag(value);
  }

  void Rotate(T angle) {
    auto x_temp = this->x * cos(angle) - this->y * sin(angle);
    y = this->x * sin(angle) + this->y * cos(angle);
    x = x_temp;
  }

 private:
};

// move to cpp and remove inline
template <typename T>
constexpr Vector2D<T> NormalPoint(Vector2D<T> position, Vector2D<T> line_start, Vector2D<T> line_end) {
  auto a = position - line_start;
  auto b = line_end - line_start;

  b.Normalize();
  b = b * (a.Dot(b));

  return line_start + b;
}

// move to cpp and remove inline
template <typename T>
constexpr Vector2D<T> operator*(float lhs, Vector2D<T> rhs) {
  return rhs * lhs;
}

template <typename T>
constexpr Vector2D<T> operator*(double lhs, Vector2D<T> rhs) {
  return rhs * lhs;
}