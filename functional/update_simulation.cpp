#include <cmath>
#include <tuple>
#include <vector>

#include "Shape.h"

using namespace std;

namespace
{
	tuple<uint32_t, uint32_t> getNumberOfCells(float const width, float const height)
	{
		static float const minCellSize{ 10.0f };

		return { static_cast<uint32_t>(ceil(width / minCellSize)), static_cast<uint32_t>(ceil(height / minCellSize)) };
	}
}

extern vector<Shape> calculatePositionsAndBounds(float const dt, vector<Shape> const shapes);
extern vector<Shape> calculateCellsRanges(float const width, float const height, uint32_t const rows, uint32_t const columns, vector<Shape> const shapes);
extern vector<vector<Shape>> fillGrid(float const width, float const height, uint32_t const rows, uint32_t const columns, vector<Shape> const shapes);
extern vector<VelocityAfterImpact> solveCollisions(vector<vector<Shape>> const cellsWithShapes, uint32_t const columns);
extern vector<Shape> applyVelocities(vector<Shape> const shapes, vector<VelocityAfterImpact> const velocitiesAfterImpact);

vector<Shape> updateSimulation(float const dt, vector<Shape> const shapes, float const width, float const height)
{
	// step 1 - update calculate current positions
	vector<Shape> const updatedShapes1{ calculatePositionsAndBounds(dt, move(shapes)) };

	// step 2 - for each shape calculate cells it fits in
	uint32_t rows;
	uint32_t columns;
	tie(rows, columns) = getNumberOfCells(width, height); // auto [rows, columns] = getNumberOfCells(width, height); - c++17 structured bindings - not supported in vs2017 at the moment of writing

	vector<Shape> const updatedShapes2{ calculateCellsRanges(width, height, rows, columns, move(updatedShapes1)) };

	// step 3 - put shapes in corresponding cells
	vector<vector<Shape>> const cellsWithShapes{ fillGrid(width, height, rows, columns, updatedShapes2) };

	// step 4 - calculate collisions
	vector<VelocityAfterImpact> const velocityAfterImpact{ solveCollisions(move(cellsWithShapes), columns) };

	// step 5- apply velocities
	vector<Shape> const updatedShapes3{ applyVelocities(move(updatedShapes2), velocityAfterImpact) };

	return updatedShapes3;
}