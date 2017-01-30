#pragma once

#include <vector>

#include "Math.h"

class CollisionSolver
{
public:
	static void solveCollision(struct ShapesData & data, std::size_t const indA, std::size_t const indB);
};