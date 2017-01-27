#include <array>

#include "CollisionSolver.h"
#include "Constants.h"
#include "Grid.h"
#include "Shape.h"
#include "ThreadPool.h"

using namespace std;

namespace
{
	Grid::GridCellsRange getCellRange(Shape const * const shape, float const width, float const height, uint32_t const numRows, uint32_t const numColumns)
	{
		Grid::GridCellsRange range;

		static float columnSize{ width / numColumns };
		static float rowSize{ height / numRows };

		range.colStart = static_cast<int>(shape->bounds.topLeft.x / columnSize);
		if (range.colStart < 0)
			range.colStart = 0;

		range.rowStart = static_cast<int>(shape->bounds.topLeft.y / rowSize);
		if (range.rowStart < 0)
			range.rowStart = 0;

		range.colEnd = static_cast<int>(shape->bounds.bottomRight.x / columnSize);
		if (range.colEnd >= static_cast<int>(numColumns))
			range.colEnd = numColumns - 1;

		range.rowEnd = static_cast<int>(shape->bounds.bottomRight.y / rowSize);
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

void Grid::reset(vector<shared_ptr<Shape>> const & shapes, ThreadPool& threadPool)
{
	clearCells(threadPool);
	assignShapesToCells(shapes, threadPool);
}

void Grid::solveCollisions(ThreadPool& threadPool) const
{
	auto doMath{ [this](vector<unique_ptr<Cell>> const & cells, size_t const i)
	{
		unique_ptr<Cell> const & cell{ cells[i] };

		int const currRow{ static_cast<int>(i / _numColumns) };
		int const currCol{ static_cast<int>(i - currRow * _numColumns) };

		for (int i{ 0 }; i < static_cast<int>(cell->shapes.size()) - 1; ++i)
		{
			GridCellsRange const & rangeA{ cell->shapes[i]->cellsRange };

			for (int j{ i + 1 }; j < cell->shapes.size(); ++j)
			{
				if (cell->shapes[i]->massInverse == 0 && cell->shapes[j]->massInverse == 0)
					continue;

				GridCellsRange const & rangeB{ cell->shapes[j]->cellsRange };

				bool shouldCheck{ (rangeA.rowStart == currRow || rangeB.rowStart == currRow) &&
					(rangeA.colStart == currCol || rangeB.colStart == currCol) };

				if (shouldCheck)
					CollisionSolver::solveCollision(cell->shapes[i].get(), cell->shapes[j].get());
			}
		}
	} };

	threadPool.runTasksSync<Constants::NUM_THREADS>(_cells, doMath);
}

void Grid::clearCells(ThreadPool& threadPool)
{
	auto clearCell{ [](vector<unique_ptr<Cell>> const & cells, size_t const i)
	{
		unique_ptr<Cell> const & cell{ cells[i] };
		cell->shapes.clear();
	} };

	threadPool.runTasksSync<Constants::NUM_THREADS>(_cells, clearCell);
}

void Grid::assignShapesToCells(vector<shared_ptr<Shape>> const & shapes, ThreadPool& threadPool)
{
	auto assignShapeToCells{ [this](vector<shared_ptr<Shape>> const & shapes, size_t const i)
	{
		shared_ptr<Shape> const & shape{ shapes[i] };

		shape->cellsRange = getCellRange(shape.get(), _width, _height, _numRows, _numColumns);

		for (int c{ shape->cellsRange.colStart }; c <= shape->cellsRange.colEnd; ++c)
		{
			for (int r{ shape->cellsRange.rowStart }; r <= shape->cellsRange.rowEnd; ++r)
			{
				unique_ptr<Cell> const & cell{ _cells[r * _numColumns + c] };
				lock_guard<mutex> const lock{ cell->mtx };
				cell->shapes.push_back(shape);
			}
		}
	} };

	threadPool.runTasksSync<Constants::NUM_THREADS>(shapes, assignShapeToCells);
}