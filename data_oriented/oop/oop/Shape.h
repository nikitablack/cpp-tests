#pragma once

#include <vector>

#include "Math.h"

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
	Shape(uint32_t const numVertices, float radius, math::Vec2 const pos, math::Vec2 const vel, float m, math::Color const col);

	static Shape createWall(float const w, float const h, math::Vec2 const pos);

public:
	math::Vec2 position{ 0.0f, 0.0f };
	math::Vec2 velocity{ 0.0f, 0.0f };
	float massInverse;
	math::Color color;
	std::vector<math::Vec2> vertices;
	math::Bounds bounds;
};