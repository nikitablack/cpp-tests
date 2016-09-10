#include "Shape.h"
#include "Math.h"
#include <cassert>

using namespace std;

Shape::Shape(uint32_t numVertices, float radius, Vec2 pos, Vec2 vel, float m, Color col) : position{ pos }, velocity{ vel }, massInverse{ 1 / m }, color{ col }
{
	assert(numVertices > 2);
	assert(radius > 0.0f);

	float sector{ math::TWO_PI / numVertices };

	for (uint32_t i{ 0 }; i < numVertices; ++i)
	{
		vertices.emplace_back(radius * std::sin(sector * i), -radius * std::cos(sector * i));
	}
}