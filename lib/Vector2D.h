#pragma once

#include <cmath>

class Vector2D
{
public:
	double x{};
	double y{};

	constexpr Vector2D() = default;
	constexpr Vector2D(double x, double y) : x(x), y(y){};

	static Vector2D Zero() { return {0.0, 0.0}; }
	static Vector2D One() { return {1.0, 1.0}; }

	constexpr Vector2D operator+(const Vector2D rhs) const
	{
		return {this->x + rhs.x, this->y + rhs.y};
	}

	constexpr Vector2D operator-(const Vector2D rhs) const
	{
		return {this->x - rhs.x, this->y - rhs.y};
	}

	constexpr Vector2D operator*(const double mul) const
	{
		return {this->x * mul, this->y * mul};
	}

	constexpr Vector2D operator/(const double div) const
	{
		return {this->x / div, this->y / div};
	}

	double Dot(const Vector2D value)
	{
		return x * value.x + y * value.y;
	}

	double Mag() const
	{
		return sqrt(this->x * this->x + this->y * this->y);
	}

	constexpr double MagSquared() const
	{
		return this->x * this->x + this->y * this->y;
	}

	void Normalize()
	{
		auto magnitude = Mag();
		if (magnitude > 0.0)
		{
			this->x = this->x / magnitude;
			this->y = this->y / magnitude;
		}
	}

	void SetMag(double val)
	{
		Normalize();
		*this = *this * val;
	}

	void Limit(double value)
	{
		if (MagSquared() > (value * value))
			SetMag(value);
	}

	void Rotate(double angle)
	{
		auto x_temp = this->x * cos(angle) - this->y * sin(angle);
		y = this->x * sin(angle) + this->y * cos(angle);
		x = x_temp;
	}

private:
};

// move to cpp and remove inline
inline const Vector2D NormalPoint(Vector2D position, Vector2D line_start, Vector2D line_end)
{
	auto a = position - line_start;
	auto b = line_end - line_start;

	b.Normalize();
	b = b * (a.Dot(b));

	return line_start + b;
}

// move to cpp and remove inline
inline Vector2D operator*(double lhs, Vector2D rhs) { return rhs * lhs; }