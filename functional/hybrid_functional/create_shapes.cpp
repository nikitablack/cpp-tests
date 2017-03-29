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

	Color getColor()
	{
		return { randRange(0.0f, 1.0f), randRange(0.0f, 1.0f), randRange(0.0f, 1.0f) };
	}

	float getMassInverse()
	{
		return 1 / randRange(1.0f, 5.0f);
	}
}

ShapesData createShapes(uint32_t const numShapes, float const canvasW, float const canvasH)
{
	ShapesData data;
	data.vertices.reserve(numShapes);
	data.positions.reserve(numShapes);
	data.velocities.reserve(numShapes);
	data.colors.reserve(numShapes);
	data.massInverses.reserve(numShapes);

	for (uint32_t i{ 0 }; i < numShapes; ++i)
	{
		uint32_t const numVertices{ static_cast<uint32_t>(randRange(3, 6)) };

		data.vertices.push_back(getVertices(numVertices));
		data.positions.push_back(getPosition(canvasW, canvasH));
		data.velocities.push_back(getVelocity());
		data.colors.push_back(getColor());
		data.massInverses.push_back(getMassInverse());
	}

	return data;
}

ShapesData createWall(float const w, float const h, Vec2 const pos)
{
	ShapesData data;

	data.vertices.emplace_back(initializer_list<Vec2>{ { 0.0f, 0.0f }, { w, 0.0f }, { w, h }, { 0.0f, h } });
	data.positions.push_back(pos);
	data.velocities.emplace_back();
	data.colors.emplace_back(1.0f, 0.0f, 0.0f);
	data.massInverses.push_back(0.0f);

	return data;
}