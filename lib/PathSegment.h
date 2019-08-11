#pragma once

#include "Vector2D.h"

class PathSegment
{
public:
	Vector2D p1{};
	Vector2D p2{};
	double size{30.0};

	bool IsPointOnSegment(const Vector2D &point) const;

	Vector2D GetClosestPoint(const Vector2D &point) const;
};
