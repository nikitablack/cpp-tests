#include <algorithm>
#include <optional>
#include <vector>

#include "Shape.h"

using namespace math;
using namespace std;

namespace
{
	template<typename InputIterator, typename Function, typename Predicate>
	Function for_each_if(InputIterator first, InputIterator last, Predicate pred, Function f)
	{
		for (; first != last; ++first)
		{
			if (pred(*first))
				f(*first);
		}
		return f;
	};

	Vec2 calculateVelocity(Shape const shape, vector<VelocityAfterImpact> const velocitiesAfterImpact)
	{
		optional<Vec2> newVel{ {} };

		for_each_if(velocitiesAfterImpact.begin(), velocitiesAfterImpact.end(),
			[shape](VelocityAfterImpact const vel) {
				return shape.id == vel.id;
			},
			[&newVel](VelocityAfterImpact const vel) {
				newVel.emplace(newVel.value() + vel.velocity);
			});

		return shape.velocity + newVel.value();
	}
}

vector<Shape> applyVelocities(vector<Shape> const shapes, vector<VelocityAfterImpact> const velocitiesAfterImpact)
{
	vector<Shape> updatedShapes;
	updatedShapes.reserve(shapes.size());

	for_each(shapes.begin(), shapes.end(), [&updatedShapes, velocitiesAfterImpact](Shape const shape)
	{
		updatedShapes.emplace_back(shape.id, shape.vertices, shape.position, calculateVelocity(shape, velocitiesAfterImpact), shape.bounds, shape.cellsRange, shape.color, shape.massInverse);
	});

	return updatedShapes;
}