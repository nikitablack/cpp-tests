#include <vector>

#include "Math.h"
#include "Shape.h"

using namespace math;
using namespace std;

namespace
{
	vector<Vec2> getVertices(uint32_t const numVertices)
	{
		vector<Vec2> v;
		v.reserve(numVertices);

		float const radius{ randRange(2.0f, 4.0f) };
		float const sector{ two_pi / numVertices };

		for (uint32_t i{ 0 }; i < numVertices; ++i)
		{
			v.emplace_back(radius * sin(sector * i), -radius * cos(sector * i));
		}

		return v;
	}

	Vec2 getPosition(float const canvasW, float const canvasH)
	{
		return { randRange(40.0f, canvasW - 40.0f), randRange(40.0f, canvasH - 40.0f) };
	}

	Vec2 getVelocity()
	{
		return { randRange(-50.0f, 50.0f), randRange(-50.0f, 50.0f) };
	}

	float getMassInverse()
	{
		return 1 / randRange(1.0f, 5.0f);
	}

	Color getColor()
	{
		return { randRange(0.0f, 1.0f), randRange(0.0f, 1.0f), randRange(0.0f, 1.0f) };
	}

	Shape createShape(uint32_t const numVertices, float const canvasW, float const canvasH)
	{
		static uint32_t id{ 0 };
		return { ++id, getVertices(numVertices), getPosition(canvasW, canvasH), getVelocity(),{},{}, getColor(), getMassInverse() };
	}
}

vector<Shape> createShapes(uint32_t const numShapes, float const canvasW, float const canvasH)
{
	vector<Shape> v;
	v.reserve(numShapes);

	for (uint32_t i{ 0 }; i < numShapes; ++i)
	{
		uint32_t const numVertices{ static_cast<uint32_t>(randRange(3, 6)) };

		v.emplace_back(createShape(numVertices, canvasW, canvasH));
	}

	return v;
}

Shape createWall(float const w, float const h, Vec2 const pos)
{
	Shape shape{ createShape(4, 100, 100) };

	return { shape.id, { Vec2{ 0.0f, 0.0f }, Vec2{ w, 0.0f }, Vec2{ w, h }, Vec2{ 0.0f, h } }, pos, {}, {}, {}, { 1.0f, 0.0f, 0.0f }, 0.0f };
}