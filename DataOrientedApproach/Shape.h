#pragma once

#include <vector>
#include "Vec2.h"
#include "Color.h"

#undef max
#undef min

struct ShapeShaderData
{
	float x;
	float y;
	float r;
	float g;
	float b;
};

class Shape
{
public:
	Vec2 position{ 0.0f, 0.0f };
	Vec2 velocity{ 0.0f, 0.0f };
	float massInverse;
	Color color;
	std::vector<Vec2> vertices;

	Shape(uint32_t numVertices, float radius, Vec2 pos, Vec2 vel, float m, Color col);
};