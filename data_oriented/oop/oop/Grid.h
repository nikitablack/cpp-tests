#pragma once

#include <memory>
#include <vector>

class Grid
{
public:
	struct Cell
	{
		std::vector<std::shared_ptr<class Shape>> shapes;
	};

	struct GridCellsRange
	{
		int colStart;
		int rowStart;
		int colEnd;
		int rowEnd;
	};

public:
	Grid(float const width, float const height, uint32_t const rows, uint32_t const columns);
	void reset(std::vector<std::shared_ptr<class Shape>> const & shapes);
	void solveCollisions() const;

private:
	float _width;
	float _height;
	uint32_t _numRows;
	uint32_t _numColumns;
	std::vector<Cell> _cells;
};