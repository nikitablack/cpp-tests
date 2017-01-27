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
	void reset(std::vector<std::shared_ptr<class Shape>> const & shapes, class ThreadPool& threadPool);
	void solveCollisions(class ThreadPool& threadPool) const;

private:
	void clearCells(class ThreadPool& threadPool);
	void assignShapesToCells(std::vector<std::shared_ptr<class Shape>> const & shapes, class ThreadPool& threadPool);

private:
	float _width;
	float _height;
	uint32_t _numRows;
	uint32_t _numColumns;
	std::vector<std::unique_ptr<Cell>> _cells;
};