#pragma once

#include <random>

namespace math
{
	const float pi{ 3.14159265359f };
	const float two_pi{ 2 * pi };

	inline int randRange(int const from, int const to)
	{
		static std::mt19937 gen;
		std::uniform_int_distribution<> const distr(from, to);

		return distr(gen);
	}

	inline float randRange(float const from, float const to)
	{
		static std::mt19937 gen;
		std::uniform_real_distribution<float> const distr(from, to);

		return distr(gen);
	}

	struct Vec2
	{
		float /*const*/ x;
		float /*const*/ y;

		Vec2(float const x = 0.0f, float const y = 0.0f) : x{ x }, y{ y }
		{}

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

		Vec2 operator-(Vec2 const other) const
		{
			return{ x - other.x, y - other.y };
		}

		Vec2 operator-() const
		{
			return{ -x, -y };
		}

		Vec2 operator*(float const s) const
		{
			return{ x * s, y * s };
		}

		float dot(Vec2 const other) const
		{
			return x * other.x + y * other.y;
		}
	};

	struct Bounds
	{
		Bounds(Vec2 const tl = {}, Vec2 const br = {}) : topLeft{ tl }, bottomRight{ br }
		{}

		Vec2 const topLeft;
		Vec2 const bottomRight;
	};

	struct Color
	{
		Color(float const r = 0.0f, float const g = 0.0f, float const b = 0.0f) : r{ r }, g{ g }, b{ b } {}
		float const r;
		float const g;
		float const b;
	};
}