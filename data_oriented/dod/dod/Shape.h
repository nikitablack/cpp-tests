#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include "Grid.h"
#include "Math.h"

struct ShapeShaderData
{
	float x;
	float y;
	float r;
	float g;
	float b;
};

struct ShapesData
{
	std::vector<math::Vec2> positions;
	std::vector<math::Vec2> velocities;
	std::vector<math::Vec2> overlapAccumulators;
	std::vector<float> massesInverses;
	std::vector<math::Color> colors;
	std::vector<std::vector<math::Vec2>> vertices;
	std::vector<math::Bounds> bounds;
	std::vector<Grid::GridCellsRange> cellsRanges;
	std::vector<std::unique_ptr<std::mutex>> mutexes;
};