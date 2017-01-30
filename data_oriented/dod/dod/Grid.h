#pragma once

#include <memory>
#include <vector>

class Grid
{
public:
	struct Cell
	{
		std::vector<std::size_t> shapes;
	};

	struct GridCellsRange
	{
		int colStart;
		int rowStart;
		int colEnd;
		int rowEnd;
	};

public:
	Grid(float const width, float const height, std::uint32_t const rows, std::uint32_t const columns);
	void reset(struct ShapesData & data);
	void solveCollisions(struct ShapesData & data) const;

private:
	float _width;
	float _height;
	std::uint32_t _numRows;
	std::uint32_t _numColumns;
	std::vector<Cell> _cells;
};