#include <algorithm>
#include <vector>

#include "Shape.h"

using namespace math;
using namespace std;

namespace
{
	Vec2 calculatePosition(Shape const shape, float const dt)
	{
		return { shape.position + shape.velocity * dt };
	}

	Bounds calculateBounds(Shape const shape)
	{
		float minX{ numeric_limits<float>::max() };
		float maxX{ numeric_limits<float>::lowest() };
		float minY{ numeric_limits<float>::max() };
		float maxY{ numeric_limits<float>::lowest() };

		for_each(shape.vertices.begin(), shape.vertices.end(), [&minX, &maxX, &minY, &maxY](Vec2 const vertex)
		{
			minX = min(minX, vertex.x);
			maxX = max(maxX, vertex.x);
			minY = min(minY, vertex.y);
			maxY = max(maxY, vertex.y);
		});

		return{ { shape.position.x + minX, shape.position.y + minY },{ shape.position.x + maxX, shape.position.y + maxY } };
	}
}

vector<Shape> calculatePositionsAndBounds(float const dt, vector<Shape> const shapes)
{
	vector<Shape> updatedShapes;
	updatedShapes.reserve(shapes.size());

	transform(shapes.begin(), shapes.end(), back_inserter(updatedShapes), [dt](Shape const shape)
	{
		Shape const newShape{ shape.id, shape.vertices, calculatePosition(shape, dt), shape.velocity, shape.bounds, shape.cellsRange, shape.color, shape.massInverse };
		return Shape{ newShape.id, newShape.vertices, newShape.position, newShape.velocity, calculateBounds(newShape), newShape.cellsRange, newShape.color, newShape.massInverse };
	});

	return updatedShapes;
}