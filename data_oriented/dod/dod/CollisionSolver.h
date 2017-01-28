#pragma once

#include <vector>

#include "Math.h"

class CollisionSolver
{
public:
	static void solveCollision(float const massInverseA, float const massInverseB, math::Bounds const & boundsA, math::Bounds const & boundsB, math::Vec2 const posA, math::Vec2 const posB, std::vector<math::Vec2> const & verticesA, std::vector<math::Vec2> const & verticesB, math::Vec2 & velocityA, math::Vec2 & velocityB, math::Vec2 & overlapAccumulatorA, math::Vec2 & overlapAccumulatorB);
};