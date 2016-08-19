#pragma once

#include <vector>
#include <cstdint>

struct Vec2
{
	float x;
	float y;
};

struct Shape
{
	Vec2 position;
	uint32_t material;
	std::vector<Vec2> vertices;

	Shape(uint32_t numVertices)
	{

	}
};