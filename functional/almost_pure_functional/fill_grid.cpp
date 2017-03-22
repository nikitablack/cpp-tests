#include <algorithm>
#include <vector>

#include "Shape.h"

using namespace std;

vector<vector<Shape>> fillGrid(float const width, float const height, uint32_t const rows, uint32_t const columns, vector<Shape> const shapes)
{
	vector<vector<Shape>> grid(rows * columns);

	for_each(shapes.begin(), shapes.end(), [columns, rows, &grid](Shape const shape)
	{
		for (int c{ shape.cellsRange.colStart }; c >= 0 && c <= shape.cellsRange.colEnd && c < static_cast<int>(columns); ++c)
		{
			for (int r{ shape.cellsRange.rowStart }; r >= 0 && r <= shape.cellsRange.rowEnd && r < static_cast<int>(rows); ++r)
			{
				grid[r * columns + c].push_back(shape);
			}
		}
	});

	return grid;
}