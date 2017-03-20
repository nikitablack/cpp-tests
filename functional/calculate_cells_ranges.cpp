#include <algorithm>
#include <vector>

#include "Shape.h"

using namespace std;

namespace
{
	CellsRange calculateCellsRange(float const width, float const height, uint32_t const rows, uint32_t const columns, Shape const shape)
	{
		CellsRange range;

		float const columnSize{ width / columns };
		float const rowSize{ height / rows };

		int const colStart{ static_cast<int>(shape.bounds.topLeft.x / columnSize) };
		int const rowStart{ static_cast<int>(shape.bounds.topLeft.y / rowSize) };
		int const colEnd{ static_cast<int>(shape.bounds.bottomRight.x / columnSize) };
		int const rowEnd{ static_cast<int>(shape.bounds.bottomRight.y / rowSize) };

		return { colStart, rowStart, colEnd, rowEnd };
	}
}

vector<Shape> calculateCellsRanges(float const width, float const height, uint32_t const rows, uint32_t const columns, vector<Shape> const shapes)
{
	vector<Shape> updatedShapes;
	updatedShapes.reserve(shapes.size());

	for_each(shapes.begin(), shapes.end(), [&updatedShapes, width, height, rows, columns](Shape const shape)
	{
		updatedShapes.emplace_back(shape.id, shape.vertices, shape.position, shape.velocity, shape.bounds, calculateCellsRange(width, height, rows, columns, shape), shape.color, shape.massInverse);
	});

	return updatedShapes;
}