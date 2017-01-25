#include "CollisionSolver.h"
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
		_cells.emplace_back();
	}
}

void Grid::reset(vector<shared_ptr<Shape>> const & shapes)
{
	for (auto & cell : _cells)
	{
		cell.shapes.clear();
	}

	for (shared_ptr<Shape> const & shape : shapes)
	{
		shape->cellsRange = getCellRange(shape.get(), _width, _height, _numRows, _numColumns);

		for (int c{ shape->cellsRange.colStart }; c <= shape->cellsRange.colEnd; ++c)
		{
			for (int r{ shape->cellsRange.rowStart }; r <= shape->cellsRange.rowEnd; ++r)
			{
				Cell & cell{ _cells[r * _numColumns + c] };
				cell.shapes.push_back(shape);
			}
		}
	}
}

void Grid::solveCollisions(ThreadPool& threadPool) const
{
	vector<future<void>> results;
	results.reserve(threadPool.getSize());

	size_t const cellsPerThread{ _cells.size() / threadPool.getSize() };
	for (size_t t{ 0 }; t < threadPool.getSize(); ++t)
	{
		size_t indStart{ t * cellsPerThread };
		size_t indEnd{ t < (threadPool.getSize() - 1) ? (indStart + cellsPerThread - 1) : (_cells.size() - 1) };

		future<void> result{ threadPool.enqueue(
			[indStart, indEnd, this]()
		{
			for (size_t c{ indStart }; c < indEnd; ++c)
			{
				Cell const & cell{ _cells[c] };

				int const currRow{ static_cast<int>(c / _numColumns) };
				int const currCol{ static_cast<int>(c - currRow * _numColumns) };

				for (int i{ 0 }; i < static_cast<int>(cell.shapes.size()) - 1; ++i)
				{
					GridCellsRange const & rangeA{ cell.shapes[i]->cellsRange };

					for (int j{ i + 1 }; j < cell.shapes.size(); ++j)
					{
						if (cell.shapes[i]->massInverse == 0 && cell.shapes[j]->massInverse == 0)
							continue;

						GridCellsRange const & rangeB{ cell.shapes[j]->cellsRange };

						bool shouldCheck{ (rangeA.rowStart == currRow || rangeB.rowStart == currRow) &&
						(rangeA.colStart == currCol || rangeB.colStart == currCol) };

						if (shouldCheck)
							CollisionSolver::solveCollision(cell.shapes[i].get(), cell.shapes[j].get());
					}
				}
			}
		}
		) };

		results.push_back(move(result));
	}

	for (auto&& result : results)
	{
		result.get();
	}
}