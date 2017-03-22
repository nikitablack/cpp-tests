#include <algorithm>
#include <array>
#include <optional>
#include <vector>

#include "Shape.h"

using namespace math;
using namespace std;

namespace
{
	struct Projection
	{
		Projection(float const mn, float const mx) : min{ mn }, max{ mx }
		{}

		float const min;
		float const max;
	};

	struct Overlap
	{
		Overlap(float const d, Vec2 const n) : depth{ d }, normal{ n }
		{}

		float const depth;
		Vec2 const normal;
	};

	bool areBothStatic(Shape const shapeA, Shape const shapeB)
	{
		return shapeA.massInverse == 0 && shapeB.massInverse == 0;
	}

	bool shouldBeChecked(Shape const shapeA, Shape const shapeB, int const row, int const column)
	{
		return (shapeA.cellsRange.rowStart == row || shapeB.cellsRange.rowStart == row) &&
			(shapeA.cellsRange.colStart == column || shapeB.cellsRange.colStart == column);
	}

	bool boundsOverlap(Shape const shapeA, Shape const shapeB)
	{
		if (shapeA.bounds.bottomRight.x <= shapeB.bounds.topLeft.x)
			return false;

		if (shapeA.bounds.topLeft.x >= shapeB.bounds.bottomRight.x)
			return false;

		if (shapeA.bounds.bottomRight.y <= shapeB.bounds.topLeft.y)
			return false;

		if (shapeA.bounds.topLeft.y >= shapeB.bounds.bottomRight.y)
			return false;

		return true;
	}

	Projection project(Shape const shape, Vec2 const n)
	{
		float minProj{ numeric_limits<float>::max() };
		float maxProj{ numeric_limits<float>::lowest() };

		for_each(shape.vertices.begin(), shape.vertices.end(), [&minProj, &maxProj, n](Vec2 const vertex)
		{
			float proj{ n.dot(vertex) };

			minProj = min(minProj, proj);
			maxProj = max(maxProj, proj);
		});

		return{ minProj, maxProj };
	}

	optional<Overlap> checkOverlap(Shape const shapeA, Shape const shapeB)
	{
		Vec2 const d{ shapeB.position - shapeA.position };
		optional<Overlap> result;

		// for each edge of the shape A get it's normal and project both shapes on that normal
		for (size_t v1{ shapeA.vertices.size() - 1 }, v2{ 0 }; v2 < shapeA.vertices.size(); v1 = v2, ++v2)
		{
			Vec2 const vertex1{ shapeA.vertices[v1] };
			Vec2 const vertex2{ shapeA.vertices[v2] };

			Vec2 const edge{ vertex2 - vertex1 };
			Vec2 const n{ edge.getNormal() };

			// get the projection of the first shape on the normal n
			Projection const projA{ project(shapeA, n) };

			// get the projection of the second shape on the same normal by adding a distance projection on the normal
			Projection const projTmp{ project(shapeB, n) };
			float const projD{ n.dot(d) };
			Projection const projB{ projTmp.min + projD, projTmp.max + projD };

			// if projections on the same normal do not overlap that means that shapes are separated
			if (projB.min >= projA.max || projB.max <= projA.min)
				return {};

			// get the overlap size and store the minimum one as well as a normal for this minimum overlap.
			// later move shapes along this normal by the value of the minimum overlap
			float const p{ abs(projA.max - projB.min) };
			if (!result.has_value() || p < result.value().depth)
			{
				result.emplace(p, n);
			}
		}

		return result;
	}

	optional<array<VelocityAfterImpact, 2>> solveCollision(Shape const shapeA, Shape const shapeB)
	{
		if (areBothStatic(shapeA, shapeB) || !boundsOverlap(shapeA, shapeB))
			return {};

		optional<Overlap> const result1{ checkOverlap(shapeA, shapeB) };
		if (!result1.has_value())
			return {};

		optional<Overlap> const result2{ checkOverlap(shapeB, shapeA) };
		if (!result2.has_value())
			return {};

		Overlap const & overlap1{ result1.value() };
		Overlap const & overlap2{ result2.value() };

		Vec2 const n{ (overlap1.depth < overlap2.depth) ? overlap1.normal : overlap2.normal };
		Vec2 const vRelTmp{ (overlap1.depth < overlap2.depth) ? (shapeA.velocity - shapeB.velocity) : (shapeB.velocity - shapeA.velocity) };
		
		// if relative velocity is separating the objects will be apart in next couple of frames
		if(n.dot(vRelTmp) < 0.0f)
			return {};

		Vec2 const vRel{ shapeA.velocity - shapeB.velocity };
		float const j{ -n.dot(vRel) * 2 / (shapeA.massInverse + shapeB.massInverse) };
		
		array<VelocityAfterImpact, 2> arr{ { {shapeA.id, n * (j * shapeA.massInverse)}, { shapeB.id, -(n * (j * shapeB.massInverse)) } } };
		
		return arr;
	}

	vector<VelocityAfterImpact> solveCollisionsForTheCell(vector<Shape> const shapes, int const row, int const column)
	{
		vector<VelocityAfterImpact> newVelocities;

		for (int i{ 0 }; i < static_cast<int>(shapes.size()) - 1; ++i)
		{
			Shape const shapeA{ shapes[i] };

			for (int j{ i + 1 }; j < shapes.size(); ++j)
			{
				Shape const shapeB{ shapes[j] };

				if (shouldBeChecked(shapeA, shapeB, row, column))
				{
					optional<array<VelocityAfterImpact, 2>> const result{ solveCollision(shapeA, shapeB) };
					if (result.has_value())
					{
						newVelocities.push_back(result.value()[0]);
						newVelocities.push_back(result.value()[1]);
					}
				}
			}
		}

		return newVelocities;
	}
}

vector<VelocityAfterImpact> solveCollisions(vector<vector<Shape>> const cellsWithShapes, uint32_t const columns)
{
	vector<VelocityAfterImpact> newVelocities;

	for (size_t c{ 0 }; c < cellsWithShapes.size(); ++c)
	{
		int const currRow{ static_cast<int>(c / columns) };
		int const currCol{ static_cast<int>(c - currRow * columns) };

		vector<VelocityAfterImpact> tmp{ solveCollisionsForTheCell(move(cellsWithShapes[c]), currRow, currCol) };

		transform(tmp.begin(), tmp.end(), back_inserter(newVelocities), [](VelocityAfterImpact const vel)
		{
			return vel;
		});
	}

	return newVelocities;
}