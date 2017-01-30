#include "CollisionSolver.h"
#include "Grid.h"
#include "Shape.h"

using namespace math;
using namespace std;

namespace
{
	Grid::GridCellsRange getCellRange(Bounds const & bounds, float const width, float const height, uint32_t const numRows, uint32_t const numColumns)
	{
		Grid::GridCellsRange range;

		static float const columnSize{ width / numColumns };
		static float const rowSize{ height / numRows };

		range.colStart = static_cast<int>(bounds.topLeft.x / columnSize);
		if (range.colStart < 0)
			range.colStart = 0;

		range.rowStart = static_cast<int>(bounds.topLeft.y / rowSize);
		if (range.rowStart < 0)
			range.rowStart = 0;

		range.colEnd = static_cast<int>(bounds.bottomRight.x / columnSize);
		if (range.colEnd >= static_cast<int>(numColumns))
			range.colEnd = numColumns - 1;

		range.rowEnd = static_cast<int>(bounds.bottomRight.y / rowSize);
		if (range.rowEnd >= static_cast<int>(numRows))
			range.rowEnd = numRows - 1;

		return range;
	}
}

Grid::Grid(float const width, float const height, uint32_t const numRows, uint32_t const numColumns) : _width{ width }, _height{ height }, _numRows{ numRows }, _numColumns{ numColumns }
{
	_cells.reserve(numRows * numColumns);
	for (uint32_t i{ 0 }; i < numRows * numColumns; ++i)
	{
		_cells.emplace_back();
	}
}

void Grid::reset(ShapesData & data)
{
	for (auto & cell : _cells)
	{
		cell.shapes.clear();
	}

	vector<Bounds> const & bounds{ data.bounds };
	for(size_t i{0}; i < data.bounds.size(); ++i)
	{
		data.cellsRanges[i] = getCellRange(data.bounds[i], _width, _height, _numRows, _numColumns);
		GridCellsRange cellsRange{ data.cellsRanges[i] };

		for (int c{ cellsRange.colStart }; c <= cellsRange.colEnd; ++c)
		{
			for (int r{ cellsRange.rowStart }; r <= cellsRange.rowEnd; ++r)
			{
				Cell & cell{ _cells[r * _numColumns + c] };
				cell.shapes.push_back(i);
			}
		}
	}
}

void Grid::solveCollisions(ShapesData & data) const
{
	for (size_t c{ 0 }; c < _cells.size(); ++c)
	{
		Cell const & cell{ _cells[c] };

		int const currRow{ static_cast<int>(c / _numColumns) };
		int const currCol{ static_cast<int>(c - currRow * _numColumns) };

		for (int i{ 0 }; i < static_cast<int>(cell.shapes.size()) - 1; ++i)
		{
			size_t indA{ cell.shapes[i] };
			GridCellsRange const & rangeA{ data.cellsRanges[indA] };

			for (int j{ i + 1 }; j < cell.shapes.size(); ++j)
			{
				size_t indB{ cell.shapes[j] };

				if (data.massesInverses[indA] == 0 && data.massesInverses[indB] == 0)
					continue;

				GridCellsRange const & rangeB{ data.cellsRanges[indB] };

				bool shouldCheck{ (rangeA.rowStart == currRow || rangeB.rowStart == currRow) &&
				(rangeA.colStart == currCol || rangeB.colStart == currCol) };

				if (shouldCheck)
					CollisionSolver::solveCollision(data, indA, indB);
			}
		}
	}
}