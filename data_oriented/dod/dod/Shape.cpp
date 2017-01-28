#include <cassert>

#include "Shape.h"

/*using namespace std;
using namespace math;

Shape::Shape(uint32_t const numVertices, float const radius, Vec2 const pos, Vec2 const vel, float const m, Color const col) : position{ pos }, velocity{ vel }, massInverse{ 1 / m }, color{ col }
{
	assert(numVertices > 2);
	assert(radius > 0.0f);

	float const sector{ math::TWO_PI / numVertices };

	for (uint32_t i{ 0 }; i < numVertices; ++i)
	{
		vertices.emplace_back(radius * sin(sector * i), -radius * cos(sector * i));
	}
}

Shape Shape::createWall(float const w, float const h, Vec2 const pos)
{
	Shape wall{ 4, 1.0f, pos, { 0.0f, 0.0f }, numeric_limits<float>::infinity(), { 1.0f, 0.0f, 0.0f } };
	wall.vertices[0] = { 0.0f, 0.0f };
	wall.vertices[1] = { w, 0.0f };
	wall.vertices[2] = { w, h };
	wall.vertices[3] = { 0.0f, h };

	return wall;
}*/