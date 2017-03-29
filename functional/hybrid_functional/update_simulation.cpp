#include <cmath>
#include <optional>
#include <tuple>
#include <vector>

#include "Shape.h"

using namespace math;
using namespace std;

namespace
{
	tuple<uint32_t, uint32_t> getNumberOfCells(float const width, float const height)
	{
		static float const minCellSize{ 10.0f };

		return { static_cast<uint32_t>(ceil(width / minCellSize)), static_cast<uint32_t>(ceil(height / minCellSize)) };
	}
}

extern tuple<vector<Vec2> const &, vector<Bounds> const &, vector<CellsRange> const &, vector<vector<size_t>> const &> calculatePositionsAndBoundsAndCellsRangesAndGrid(float const dt, float const width, float const height, uint32_t const rows, uint32_t const columns, ShapesData const & data);
extern vector<Vec2> const & solveCollisions(vector<vector<size_t>> const & grid, ShapesData const & data, vector<CellsRange> const & cellsRanges, vector<Bounds> const & bounds, uint32_t const columns);

tuple<vector<Vec2> const &, vector<Vec2> const &> updateSimulation(float const dt, ShapesData const & data, float const width, float const height)
{
	uint32_t rows;
	uint32_t columns;
	tie(rows, columns) = getNumberOfCells(width, height); // auto [rows, columns] = getNumberOfCells(width, height); - c++17 structured bindings - not supported in vs2017 at the moment of writing

	auto positionsAndBoundsAndCellsRangesAndGrid{ calculatePositionsAndBoundsAndCellsRangesAndGrid(dt, width, height, rows, columns, data) };

	vector<Vec2> const & newPositions{ get<0>(positionsAndBoundsAndCellsRangesAndGrid) };

	vector<Bounds> const & newBounds{ get<1>(positionsAndBoundsAndCellsRangesAndGrid) };
	vector<CellsRange> const & newCellsRanges{ get<2>(positionsAndBoundsAndCellsRangesAndGrid) };
	vector<vector<size_t>> const & newGrid{ get<3>(positionsAndBoundsAndCellsRangesAndGrid) };
	
	vector<Vec2> const & newVelocities{ solveCollisions(newGrid, data, newCellsRanges, newBounds, columns) };

	return tie(newPositions, newVelocities);
}