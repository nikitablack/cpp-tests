#include "Grid.h"

using namespace std;

Grid::Grid(uint32_t width, uint32_t height) : w{ width }, h{ height }
{
	cells.reserve(width * height);
	for (uint32_t i{ 0 }; i < width * height; ++i)
	{
		cells.push_back(make_unique<Cell>());
	}
}

void Grid::addShape(Shape * shape)
{
}

void Grid::clear()
{
	for (const auto& cell : cells)
	{
		cell->shapes.clear();
	}
}
