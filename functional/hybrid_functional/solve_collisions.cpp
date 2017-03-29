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

	bool areBothStatic(ShapesData const & data, size_t const indA, size_t const indB)
	{
		return data.massInverses[indA] == 0 && data.massInverses[indB] == 0;
	}

	bool shouldBeChecked(vector<CellsRange> const & cellsRanges, size_t const indA, size_t const indB, int const row, int const column)
	{
		CellsRange const & cellsRangeA{ cellsRanges[indA] };
		CellsRange const & cellsRangeB{ cellsRanges[indB] };

		return (cellsRangeA.rowStart == row || cellsRangeB.rowStart == row) &&
			(cellsRangeA.colStart == column || cellsRangeB.colStart == column);
	}

	bool boundsOverlap(vector<Bounds> const & bounds, size_t const indA, size_t const indB)
	{
		Bounds const & boundsA{ bounds[indA] };
		Bounds const & boundsB{ bounds[indB] };

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

	Projection project(ShapesData const & data, size_t const ind, Vec2 const n)
	{
		float minProj{ numeric_limits<float>::max() };
		float maxProj{ numeric_limits<float>::lowest() };

		vector<Vec2> const & vertices{ data.vertices[ind] };
		for_each(vertices.begin(), vertices.end(), [&minProj, &maxProj, n](Vec2 const & vertex)
		{
			float proj{ n.dot(vertex) };

			minProj = min(minProj, proj);
			maxProj = max(maxProj, proj);
		});

		return{ minProj, maxProj };
	}

	optional<Overlap> checkOverlap(ShapesData const & data, size_t const indA, size_t const indB)
	{
		Vec2 const & positionA{ data.positions[indA] };
		Vec2 const & positionB{ data.positions[indB] };
		vector<Vec2> const & verticesA{ data.vertices[indA] };
		vector<Vec2> const & verticesB{ data.vertices[indB] };

		Vec2 const d{ positionB - positionA };
		optional<Overlap> result;

		// for each edge of the shape A get it's normal and project both shapes on that normal
		for (size_t v1{ verticesA.size() - 1 }, v2{ 0 }; v2 < verticesA.size(); v1 = v2, ++v2)
		{
			Vec2 const vertex1{ verticesA[v1] };
			Vec2 const vertex2{ verticesA[v2] };

			Vec2 const edge{ vertex2 - vertex1 };
			Vec2 const n{ edge.getNormal() };

			// get the projection of the first shape on the normal n
			Projection const projA{ project(data, indA, n) };

			// get the projection of the second shape on the same normal by adding a distance projection on the normal
			Projection const projTmp{ project(data, indB, n) };
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

	optional<tuple<Vec2, Vec2>> solveCollision(ShapesData const & data, vector<Bounds> const & bounds, size_t const indA, size_t const indB)
	{
		if (areBothStatic(data, indA, indB) || !boundsOverlap(bounds, indA, indB))
			return {};

		optional<Overlap> const result1{ checkOverlap(data, indA, indB) };
		if (!result1.has_value())
			return {};

		optional<Overlap> const result2{ checkOverlap(data, indB, indA) };
		if (!result2.has_value())
			return {};

		Overlap const & overlap1{ result1.value() };
		Overlap const & overlap2{ result2.value() };

		Vec2 const & velocityA{ data.velocities[indA] };
		Vec2 const & velocityB{ data.velocities[indB] };

		Vec2 const n{ (overlap1.depth < overlap2.depth) ? overlap1.normal : overlap2.normal };
		Vec2 const vRelTmp{ (overlap1.depth < overlap2.depth) ? (velocityA - velocityB) : (velocityB - velocityA) };
		
		// if relative velocity is separating the objects will be apart in next couple of frames
		if(n.dot(vRelTmp) < 0.0f)
			return {};

		float const massInverseA{ data.massInverses[indA] };
		float const massInverseB{ data.massInverses[indB] };

		Vec2 const vRel{ velocityA - velocityB };
		float const j{ -n.dot(vRel) * 2 / (massInverseA + massInverseB) };
		
		return tuple<Vec2, Vec2>{n * (j * massInverseA), -(n * (j * massInverseB)) };
	}

	vector<tuple<size_t, Vec2>> const & solveCollisionsForTheCell(ShapesData const & data, vector<CellsRange> const & cellsRanges, vector<Bounds> const & bounds, vector<size_t> const & shapesInCell, int const row, int const column)
	{
		static vector<tuple<size_t, Vec2>> newVelocities;
		newVelocities.clear();
		newVelocities.reserve(shapesInCell.size());

		for (int i{ 0 }; i < static_cast<int>(shapesInCell.size()) - 1; ++i)
		{
			size_t const indA{ shapesInCell[i] };

			for (int j{ i + 1 }; j < shapesInCell.size(); ++j)
			{
				size_t const indB{ shapesInCell[j] };

				if (shouldBeChecked(cellsRanges, indA, indB, row, column))
				{
					optional<tuple<Vec2, Vec2>> const result{ solveCollision(data, bounds, indA, indB) };
					if (result.has_value())
					{
						newVelocities.emplace_back(indA, get<0>(result.value()));
						newVelocities.emplace_back(indB, get<1>(result.value()));
					}
				}
			}
		}

		return newVelocities;
	}
}

vector<Vec2> const & solveCollisions(vector<vector<size_t>> const & grid, ShapesData const & data, vector<CellsRange> const & cellsRanges, vector<Bounds> const & bounds, uint32_t const columns)
{
	static vector<Vec2> newVelocities;
	newVelocities.clear();
	newVelocities.resize(data.size());
	//fill_n(newVelocities.begin(), data.size(), Vec2{});

	for (size_t i{ 0 }; i < grid.size(); ++i)
	{
		int const currRow{ static_cast<int>(i / columns) };
		int const currCol{ static_cast<int>(i - currRow * columns) };
		solveCollisionsForTheCell(data, cellsRanges, bounds, grid[i], currRow, currCol);
		vector<tuple<size_t, Vec2>> const & tmp { solveCollisionsForTheCell(data, cellsRanges, bounds, grid[i], currRow, currCol) };

		for_each(tmp.begin(), tmp.end(), [](tuple<size_t, Vec2> const & indAndVel) {
			newVelocities[get<0>(indAndVel)] = newVelocities[get<0>(indAndVel)] + get<1>(indAndVel);
		});
	}

	return newVelocities;
}