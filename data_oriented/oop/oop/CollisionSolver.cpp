#include <algorithm>
#include <optional>

#include "CollisionSolver.h"
#include "Shape.h"

using namespace std;
using namespace math;

namespace
{
	struct Projection
	{
		float min;
		float max;
	};

	struct Overlap
	{
		float depth;
		Vec2 direction;
	};

	bool areTheSame(Shape const * const a, Shape const * const b)
	{
		return a == b;
	}

	bool areBothStatic(Shape const * const a, Shape const * const b)
	{
		return a->massInverse == 0 && b->massInverse == 0;
	}

	bool boundsOverlap(Shape const * const shapeA, Shape const * const shapeB)
	{
		Bounds const boundsA{ shapeA->bounds };
		Bounds const boundsB{ shapeB->bounds };

		if (boundsA.bottomRight.x <= boundsB.topLeft.x)
			return false;

		if (boundsA.topLeft.x >= boundsB.bottomRight.x)
			return false;

		if (boundsA.bottomRight.y <= boundsB.topLeft.y)
			return false;

		if (boundsA.topLeft.y >= boundsB.bottomRight.y)
			return false;

		return true;
	}

	Projection project(Shape const * const shape, Vec2 const n)
	{
		float min{ numeric_limits<float>::max() };
		float max{ numeric_limits<float>::lowest() };

		// project each vertex on normal and store minimum and maximum projection
		for(Vec2 const & v : shape->vertices)
		{
			float proj{ n.dot(v) };

			if (proj < min)
				min = proj;

			if (proj > max)
				max = proj;
		}

		return{ min, max };
	}

	optional<Overlap> checkOverlap(Shape * const a, Shape * const b)
	{
		Vec2 d;
		{
			lock_guard<mutex, mutex> lock(a->mtx, b->mtx);
			d = b->position - a->position;
		}
		float minPenetration{ std::numeric_limits<float>::max() };
		Vec2 normal;

		// for each edge of the shape A get it's normal and project both shapes on that normal
		for (size_t v1{ a->vertices.size() - 1 }, v2{ 0 }; v2 < a->vertices.size(); v1 = v2, ++v2)
		{
			Vec2 const vertex1{ a->vertices[v1] };
			Vec2 const vertex2{ a->vertices[v2] };

			Vec2 const edge{ vertex2 - vertex1 };
			Vec2 const n{ edge.getNormal() };

			Projection projA{ project(a, n) };
			Projection projB{ project(b, n) };
			float const projD{ n.dot(d) };

			projB.min += projD;
			projB.max += projD;

			// if projections on the same normal do not overlap that means that shapes are separated, i.e. no overlap 
			if (projB.min >= projA.max || projB.max <= projA.min)
				return {};

			// get the overlap size and store the minimum one as well as a normal for this minimum overlap.
			// later move shapes along this normal by the value of the minimum overlap
			float const p{ abs(projA.max - projB.min) };
			if (p < minPenetration)
			{
				minPenetration = p;
				normal = n;
			}
		}

		return Overlap{ minPenetration, normal };
	}
}

void CollisionSolver::solveCollision(Shape * const shapeA, Shape * const shapeB)
{
	if (areTheSame(shapeA, shapeB) || areBothStatic(shapeA, shapeB) || !boundsOverlap(shapeA, shapeB))
		return;

	optional<Overlap> const result1(checkOverlap(shapeA, shapeB));
	if (!result1.has_value())
		return;

	optional<Overlap> const result2(checkOverlap(shapeB, shapeA));
	if (!result2.has_value())
		return;

	Overlap const & overlap1{ result1.value() };
	Overlap const & overlap2{ result2.value() };

	float const d{ min(overlap1.depth, overlap2.depth) };
	Vec2 const n{ (overlap1.depth < overlap2.depth) ? overlap1.direction : overlap2.direction };

	float d1{ d * shapeA->massInverse / (shapeA->massInverse + shapeB->massInverse) };
	float d2{ d - d1 };

	if (overlap1.depth >= overlap2.depth)
	{
		d1 *= -1;
		d2 *= -1;
	}

	{
		lock_guard<mutex, mutex> lock{ shapeA->mtx, shapeB->mtx };

		Vec2 const vRel{ shapeA->velocity - shapeB->velocity };

		float const j{ -n.dot(vRel) * 2 / (shapeA->massInverse + shapeB->massInverse) };

		shapeA->velocity += n * (j * shapeA->massInverse);
		shapeB->velocity -= n * (j * shapeB->massInverse);

		// don't move shapes apart immediately but instead accumulate overlap resolution and apply it later in one go
		shapeA->overlapResolveAccumulator -= n * d1;
		shapeB->overlapResolveAccumulator += n * d2;
	}
}