#pragma once

#include <memory>
#include <mutex>
#include <vector>

class Grid
{
public:
	struct Cell
	{
		std::mutex mtx;
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
	void reset(struct ShapesData & data, class ThreadPool & threadPool);
	void solveCollisions(struct ShapesData & data, class ThreadPool & threadPool) const;

private:
	void clearCells(class ThreadPool& threadPool);
	void assignShapesToCells(struct ShapesData & data, class ThreadPool& threadPool);

private:
	float _width;
	float _height;
	std::uint32_t _numRows;
	std::uint32_t _numColumns;
	std::vector<std::unique_ptr<Cell>> _cells;
};