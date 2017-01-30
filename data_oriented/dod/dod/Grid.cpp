#include "CollisionSolver.h"
#include "Constants.h"
#include "Grid.h"
#include "Shape.h"
#include "ThreadPool.h"

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
		_cells.push_back(make_unique<Cell>());
	}
}

void Grid::reset(ShapesData & data, ThreadPool & threadPool)
{
	clearCells(threadPool);
	assignShapesToCells(data, threadPool);
}

void Grid::solveCollisions(ShapesData & data, ThreadPool & threadPool) const
{
	auto doMath{ [this, &data](vector<unique_ptr<Cell>> const & cells, size_t const i)
	{
		unique_ptr<Cell> const & cell{ cells[i] };

		int const currRow{ static_cast<int>(i / _numColumns) };
		int const currCol{ static_cast<int>(i - currRow * _numColumns) };

		for (int i{ 0 }; i < static_cast<int>(cell->shapes.size()) - 1; ++i)
		{
			size_t indA{ cell->shapes[i] };
			GridCellsRange const & rangeA{ data.cellsRanges[indA] };

			for (int j{ i + 1 }; j < cell->shapes.size(); ++j)
			{
				size_t indB{ cell->shapes[j] };

				if (data.massesInverses[indA] == 0 && data.massesInverses[indB] == 0)
					continue;

				GridCellsRange const & rangeB{ data.cellsRanges[indB] };

				bool shouldCheck{ (rangeA.rowStart == currRow || rangeB.rowStart == currRow) &&
				(rangeA.colStart == currCol || rangeB.colStart == currCol) };

				if (shouldCheck)
					CollisionSolver::solveCollision(data, indA, indB);
			}
		}
	} };

	threadPool.runTasksSync<Constants::NUM_THREADS>(_cells, doMath, _cells.size());
}

void Grid::clearCells(ThreadPool& threadPool)
{
	auto clearCell{ [](vector<unique_ptr<Cell>> const & cells, size_t const i)
	{
		unique_ptr<Cell> const & cell{ cells[i] };
		cell->shapes.clear();
	} };

	threadPool.runTasksSync<Constants::NUM_THREADS>(_cells, clearCell, _cells.size());
}

void Grid::assignShapesToCells(ShapesData & data, ThreadPool& threadPool)
{
	auto assignShapeToCells{ [this](ShapesData & data, size_t const i)
	{
		data.cellsRanges[i] = getCellRange(data.bounds[i], _width, _height, _numRows, _numColumns);
		GridCellsRange cellsRange{ data.cellsRanges[i] };

		for (int c{ cellsRange.colStart }; c <= cellsRange.colEnd; ++c)
		{
			for (int r{ cellsRange.rowStart }; r <= cellsRange.rowEnd; ++r)
			{
				unique_ptr<Cell> const & cell{ _cells[r * _numColumns + c] };
				lock_guard<mutex> const lock{ cell->mtx };
				cell->shapes.push_back(i);
			}
		}
	} };

	threadPool.runTasksSync<Constants::NUM_THREADS>(data, assignShapeToCells, data.positions.size());
}