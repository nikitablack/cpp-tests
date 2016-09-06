#pragma once

#include <cmath>

struct Vec2
{
	float x;
	float y;

	Vec2(float x = 0.0f, float y = 0.0f) : x{ x }, y{ y } {}

	Vec2 normalize()
	{
		float len{ std::sqrt(x * x + y * y) };
		float oneOverLen{ 1 / len };
		return{ x * oneOverLen , y * oneOverLen };
	}

	Vec2 getNormal()
	{
		Vec2 n{ normalize() };
		return{ n.y, -n.x };
	}

	Vec2 operator+(Vec2 other)
	{
		return{ x + other.x, y + other.y };
	}

	Vec2& operator+=(Vec2 other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	Vec2 operator-(Vec2 other)
	{
		return{ x - other.x, y - other.y };
	}

	Vec2& operator-=(Vec2 other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vec2 operator*(float s)
	{
		return{ x * s, y * s };
	}

	Vec2& operator*=(float s)
	{
		x *= s;
		y *= s;
		return *this;
	}

	float dot(Vec2 other)
	{
		return x * other.x + y * other.y;
	}
};