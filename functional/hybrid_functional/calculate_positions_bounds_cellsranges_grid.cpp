#include <algorithm>
#include <tuple>
#include <vector>

#include "Shape.h"

using namespace math;
using namespace std;

namespace
{
	Vec2 calculatePosition(float const dt, ShapesData const & data, size_t const ind)
	{
		return { data.positions[ind] + data.velocities[ind] * dt };
	}

	Bounds calculateBounds(Vec2 const & position, ShapesData const & data, size_t const ind)
	{
		float minX{ numeric_limits<float>::max() };
		float maxX{ numeric_limits<float>::lowest() };
		float minY{ numeric_limits<float>::max() };
		float maxY{ numeric_limits<float>::lowest() };

		vector<Vec2> const & vertices{ data.vertices[ind] };
		for_each(vertices.begin(), vertices.end(), [&minX, &maxX, &minY, &maxY](Vec2 const & vertex)
		{
			minX = min(minX, vertex.x);
			maxX = max(maxX, vertex.x);
			minY = min(minY, vertex.y);
			maxY = max(maxY, vertex.y);
		});

		return{ { position.x + minX, position.y + minY },{ position.x + maxX, position.y + maxY } };
	}

	CellsRange calculateCellsRange(float const columnSize, float const rowSize, Bounds const & bounds, ShapesData const & shape, size_t const ind)
	{
		int const colStart{ static_cast<int>(bounds.topLeft.x / columnSize) };
		int const rowStart{ static_cast<int>(bounds.topLeft.y / rowSize) };
		int const colEnd{ static_cast<int>(bounds.bottomRight.x / columnSize) };
		int const rowEnd{ static_cast<int>(bounds.bottomRight.y / rowSize) };

		return { colStart, rowStart, colEnd, rowEnd };
	}
}

tuple<vector<Vec2> const &, vector<Bounds> const &, vector<CellsRange> const &, vector<vector<size_t>> const &> calculatePositionsAndBoundsAndCellsRangesAndGrid(float const dt, float const width, float const height, uint32_t const rows, uint32_t const columns, ShapesData const & data)
{
	static vector<Vec2> positions;
	positions.resize(data.size());

	static vector<Bounds> bounds;
	bounds.resize(data.size());

	static vector<CellsRange> cellsRanges;
	cellsRanges.resize(data.size());

	static vector<vector<size_t>> grid;
	grid.resize(rows * columns);
	for_each(grid.begin(), grid.end(), [](vector<size_t> & cell) {
		cell.clear();
	});

	float const columnSize{ width / columns };
	float const rowSize{ height / rows };

	for (size_t i{ 0 }; i < data.size(); ++i)
	{
		positions[i] = calculatePosition(dt, data, i);
		bounds[i] = calculateBounds(positions[i], data, i);
		cellsRanges[i] = calculateCellsRange(columnSize, rowSize, bounds[i], data, i);

		CellsRange const & cellsRange{ cellsRanges[i] };
		for (int c{ cellsRange.colStart }; c >= 0 && c <= cellsRange.colEnd && c < static_cast<int>(columns); ++c)
		{
			for (int r{ cellsRange.rowStart }; r >= 0 && r <= cellsRange.rowEnd && r < static_cast<int>(rows); ++r)
			{
				grid[r * columns + c].push_back(i);
			}
		}
	};

	return tie(positions, bounds, cellsRanges, grid);
}