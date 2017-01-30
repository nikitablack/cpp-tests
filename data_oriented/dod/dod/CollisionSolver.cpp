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

	bool areBothStatic(float const massInverseA, float const massInverseB)
	{
		return massInverseA == 0 && massInverseB == 0;
	}

	bool boundsOverlap(Bounds const & boundsA, Bounds const & boundsB)
	{
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

	Projection project(vector<Vec2> const & vertices, Vec2 const n)
	{
		float min{ numeric_limits<float>::max() };
		float max{ numeric_limits<float>::lowest() };

		// project each vertex on normal and store minimum and maximum projection
		for(Vec2 const & v : vertices)
		{
			float proj{ n.dot(v) };

			if (proj < min)
				min = proj;

			if (proj > max)
				max = proj;
		}

		return{ min, max };
	}

	optional<Overlap> checkOverlap(ShapesData & data, std::size_t const indA, std::size_t const indB)
	{
		Vec2 d;
		{
			lock_guard<mutex, mutex> lock(*data.mutexes[indA], *data.mutexes[indB]);
			d = data.positions[indB] - data.positions[indA];
		}
		float minPenetration{ numeric_limits<float>::max() };
		Vec2 normal;

		vector<Vec2> & verticesA{ data.vertices[indA] };
		vector<Vec2> & verticesB{ data.vertices[indB] };

		// for each edge of the shape A get it's normal and project both shapes on that normal
		for (size_t v1{ data.vertices[indA].size() - 1 }, v2{ 0 }; v2 < data.vertices[indA].size(); v1 = v2, ++v2)
		{
			Vec2 const vertex1{ verticesA[v1] };
			Vec2 const vertex2{ verticesA[v2] };

			Vec2 const edge{ vertex2 - vertex1 };
			Vec2 const n{ edge.getNormal() };

			Projection projA{ project(verticesA, n) };
			Projection projB{ project(verticesB, n) };
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

#include "Shape.h"
void CollisionSolver::solveCollision(ShapesData & data, std::size_t const indA, std::size_t const indB)
{
	float const massInverseA{ data.massesInverses[indA] };
	float const massInverseB{ data.massesInverses[indB] };

	if (areBothStatic(massInverseA, massInverseB) || !boundsOverlap(data.bounds[indA], data.bounds[indB]))
		return;

	Vec2 const & posA{ data.positions[indA] };
	Vec2 const & posB{ data.positions[indB] };
	vector<Vec2> const & verticesA{ data.vertices[indA] };
	vector<Vec2> const & verticesB{ data.vertices[indB] };

	optional<Overlap> const result1(checkOverlap(data, indA, indB));
	if (!result1.has_value())
		return;

	optional<Overlap> const result2(checkOverlap(data, indB, indA));
	if (!result2.has_value())
		return;

	Overlap const & overlap1{ result1.value() };
	Overlap const & overlap2{ result2.value() };

	float const d{ min(overlap1.depth, overlap2.depth) };
	Vec2 const & n{ (overlap1.depth < overlap2.depth) ? overlap1.direction : overlap2.direction };

	float d1{ d * massInverseA / (massInverseA + massInverseB) };
	float d2{ d - d1 };

	if (overlap1.depth >= overlap2.depth)
	{
		d1 *= -1;
		d2 *= -1;
	}

	Vec2 & velocityA{ data.velocities[indA] };
	Vec2 & velocityB{ data.velocities[indB] };

	Vec2 const vRel{ velocityA - velocityB };

	float const j{ -n.dot(vRel) * 2 / (massInverseA + massInverseB) };

	velocityA += n * (j * massInverseA);
	velocityB -= n * (j * massInverseB);

	// don't move shapes apart immediately but instead accumulate overlap resolution and apply it later in one go
	data.overlapAccumulators[indA] -= n * d1;
	data.overlapAccumulators[indB] += n * d2;
}