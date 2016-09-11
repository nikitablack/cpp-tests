#include "Grid.h"
#include "CollisionSolver.h"
#include <cassert>
#include <Windows.h>
#include <string>
#include "Shape.h"

using namespace std;

Grid::Grid(float width, float height, uint32_t numRows, uint32_t numColumns) : width{ width }, height{ height }, numRows{ numRows }, numColumns{ numColumns }
{
	cells.reserve(numRows * numColumns);
	for (uint32_t i{ 0 }; i < numRows * numColumns; ++i)
	{
		cells.push_back(make_unique<Cell>());
	}
}

void Grid::addShape(shared_ptr<Shape> shape)
{
	CellRange range = getCellRange(shape.get());

	for (int c{ range.colStart }; c <= range.colEnd; ++c)
	{
		for (int r{ range.rowStart }; r <= range.rowEnd; ++r)
		{
			unique_ptr<Cell>& cell{ cells[r * numColumns + c] };
			cell->shapes.push_back(shape);
		}
	}
}

void Grid::clear()
{
	for (const auto& cell : cells)
	{
		//OutputDebugString((to_string(cell->shapes.size()) + "\n").data());
		cell->shapes.clear();
	}
	//OutputDebugString(string("\n").data());
}

void Grid::solveCollisions()
{
	for (uint32_t c{ 0 }; c < cells.size(); ++c)
	{
		unique_ptr<Cell>& cell{ cells[c] };

		int currRow{ static_cast<int>(c / numColumns) };
		int currCol{ static_cast<int>(c - currRow * numColumns) };

		for (int i{ 0 }; i < static_cast<int>(cell->shapes.size()) - 1; ++i)
		{
			CellRange rangeA = getCellRange(cell->shapes[i].get());

			for (int j{ i + 1 }; j < cell->shapes.size(); ++j)
			{
				if (cell->shapes[i]->massInverse == 0 && cell->shapes[j]->massInverse == 0)
					continue;

				CellRange rangeB = getCellRange(cell->shapes[j].get());

				bool shouldCheck{ (rangeA.rowStart == currRow || rangeB.rowStart == currRow) &&
					(rangeA.colStart == currCol || rangeB.colStart == currCol) };

				if(shouldCheck)
					CollisionSolver::solveCollision(cell->shapes[i].get(), cell->shapes[j].get());
			}
		}
	}
}

Grid::CellRange Grid::getCellRange(Shape* shape)
{
	CellRange range;

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
