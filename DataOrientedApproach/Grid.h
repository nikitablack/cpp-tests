#pragma once

#include <vector>
#include <memory>

struct Cell
{
	std::vector<std::shared_ptr<class Shape>> shapes;
};

class Grid
{
public:
	Grid(float width, float height, uint32_t rows, uint32_t columns);
	void addShape(std::shared_ptr<class Shape>);
	void clear();
	void solveCollisions();

private:
	float width;
	float height;
	uint32_t numRows;
	uint32_t numColumns;
	std::vector<std::unique_ptr<Cell>> cells;

private:
	struct CellRange
	{
		int colStart;
		int rowStart;
		int colEnd;
		int rowEnd;
	};

	CellRange getCellRange(class Shape* shape);
};