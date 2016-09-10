#pragma once

#include "Shape.h"

class CollisionSolver
{
public:
	static void solveCollision(Shape* shapeA, Shape* shapeB);

private:
	struct Projection
	{
		float min;
		float max;
	};

	struct Overlap
	{
		float depth;
		Vec2 direction;
		bool isOverlap;
	};

	static Overlap checkOverlap(Shape* a, Shape* b);
	static Projection project(Shape* shape, Vec2 n);
};