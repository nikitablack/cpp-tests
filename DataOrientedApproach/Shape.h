#pragma once

#include <vector>
#include <cstdint>
#include <cassert>
#include <cmath>

namespace math
{
	const float PI{ 3.14159265359f };
	const float TWO_PI{ 2 * PI };
}

struct Vec2
{
	float x;
	float y;

	Vec2(float x, float y) : x{ x }, y{ y } {}
};

struct ShapeShaderData
{
	float x;
	float y;
	float r;
	float g;
	float b;
};

struct Shape
{
	Vec2 position{ 0.0f, 0.0f };
	Vec2 velocity{ 0.0f, 0.0f };
	float massInverse;
	uint32_t color;
	std::vector<Vec2> vertices;

	Shape(uint32_t numVertices, float radius, uint32_t color) : color{ color }
	{
		assert(numVertices > 2);
		assert(radius > 0.0f);

		float sector{ math::TWO_PI / numVertices };

		for (uint32_t i{ 0 }; i < numVertices; ++i)
		{
			vertices.emplace_back(radius * std::sin(sector * i), radius * std::cos(sector * i));
		}
	}
};