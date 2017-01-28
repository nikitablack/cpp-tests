#pragma once

#include <cmath>

namespace math
{
	const float PI{ 3.14159265359f };
	const float TWO_PI{ 2 * PI };

	inline int randRange(int const from, int const to)
	{
		return from + static_cast<int>(std::round((to - from) * static_cast<float>(rand()) / RAND_MAX));
	}

	inline float randRange(float const from, float const to)
	{
		return from + (to - from) * static_cast<float>(rand()) / RAND_MAX;
	}

	struct Vec2
	{
		float x;
		float y;

		Vec2(float const x = 0.0f, float const y = 0.0f) : x{ x }, y{ y } {}

		Vec2 normalize() const
		{
			float const len{ std::sqrt(x * x + y * y) };
			float const oneOverLen{ 1 / len };
			return{ x * oneOverLen , y * oneOverLen };
		}

		Vec2 getNormal() const
		{
			Vec2 const n{ normalize() };
			return{ n.y, -n.x };
		}

		Vec2 operator+(Vec2 const other) const
		{
			return{ x + other.x, y + other.y };
		}

		Vec2& operator+=(Vec2 const other)
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		Vec2 operator-(Vec2 const other) const
		{
			return{ x - other.x, y - other.y };
		}

		Vec2& operator-=(Vec2 const other)
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}

		Vec2 operator*(float const s) const
		{
			return{ x * s, y * s };
		}

		Vec2& operator*=(float const s)
		{
			x *= s;
			y *= s;
			return *this;
		}

		float dot(Vec2 const other) const
		{
			return x * other.x + y * other.y;
		}
	};

	struct Bounds
	{
		Vec2 topLeft;
		Vec2 bottomRight;
	};

	struct Color
	{
		Color(float const r = 0.0f, float const g = 0.0f, float const b = 0.0f) : r{ r }, g{ g }, b{ b } {}
		float r;
		float g;
		float b;
	};
}