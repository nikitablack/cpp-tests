#pragma once

#include <vector>
#include <memory>
#include "Shape.h"

struct Cell
{
	std::vector<std::shared_ptr<Shape>> shapes;
};

class Grid
{
public:
	Grid(uint32_t width, uint32_t height);
	void addShape(Shape* shape);
	void clear();

private:
	uint32_t w;
	uint32_t h;
	std::vector<std::unique_ptr<Cell>> cells;
};