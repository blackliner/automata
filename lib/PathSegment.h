#pragma once

#include "Vector2D.h"

class PathSegment {
 public:
  Vector2D<VectorT> p1{};
  Vector2D<VectorT> p2{};
  double size{30.0};

  constexpr bool IsPointOnSegment(const Vector2D<VectorT> point) const {
    double left{};
    double right{};
    double up{};
    double down{};

    if (p1.x < p2.x) {
      left = p1.x;
      right = p2.x;
    } else {
      left = p2.x;
      right = p1.x;
    }

    if (p1.y < p2.y) {
      up = p1.y;
      down = p2.y;
    } else {
      up = p2.y;
      down = p1.y;
    }
    return point.x >= left && point.x <= right && point.y >= up && point.y <= down;
  }

  constexpr Vector2D<VectorT> GetClosestPoint(const Vector2D<VectorT> point) const {
    auto distance_p1 = (point - p1).MagSquared();
    auto distance_p2 = (point - p2).MagSquared();
    if (distance_p1 < distance_p2) return p1;
    return p2;
  }
};
