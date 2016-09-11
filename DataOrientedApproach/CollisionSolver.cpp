#include "CollisionSolver.h"
#include <algorithm>

using namespace std;

void CollisionSolver::solveCollision(Shape* shapeA, Shape* shapeB)
{
	if (shapeA == shapeB)
		return;

	if (shapeA->massInverse == 0 && shapeB->massInverse == 0)
		return;

	math::Bounds boundsA(shapeA->bounds);
	math::Bounds boundsB(shapeB->bounds);
	bool boundsSeparated{ boundsA.bottomRight.x < boundsB.topLeft.x ||
		boundsA.topLeft.x > boundsB.bottomRight.x ||
		boundsA.bottomRight.y < boundsB.topLeft.y ||
		boundsA.topLeft.y > boundsB.bottomRight.y };

	if (boundsSeparated)
		return;

	Vec2 vr{ shapeA->velocity - shapeB->velocity };

	Overlap result1(checkOverlap(shapeA, shapeB));
	if (!result1.isOverlap)
		return;

	Overlap result2(checkOverlap(shapeB, shapeA));
	if (!result2.isOverlap)
		return;

	float d{ min(result1.depth, result2.depth) };
	Vec2 n{ (result1.depth < result2.depth) ? result1.direction : result2.direction };

	float d1{ d * shapeA->massInverse / (shapeA->massInverse + shapeB->massInverse) };
	float d2{ d - d1 };

	if (result1.depth >= result2.depth)
	{
		d1 = -d1;
		d2 = -d2;
	}

	shapeA->position -= n * d1;
	shapeB->position += n * d2;

	float j{ -n.dot(vr) * 2 / (shapeA->massInverse + shapeB->massInverse) };

	shapeA->velocity += n * (j * shapeA->massInverse);
	shapeB->velocity -= n * (j * shapeB->massInverse);
}

CollisionSolver::Overlap CollisionSolver::checkOverlap(Shape* a, Shape* b)
{
	Vec2 d{ b->position - a->position };
	float minPenetration{ std::numeric_limits<float>::max() };
	Vec2 normal;

	for (size_t v1{ a->vertices.size() - 1 }, v2{ 0 }; v2 < a->vertices.size(); v1 = v2, ++v2)
	{
		Vec2 vertex1{ a->vertices[v1] };
		Vec2 vertex2{ a->vertices[v2] };

		Vec2 edge{ vertex2 - vertex1 };
		Vec2 n{ edge.getNormal() };

		Projection projA = project(a, n);
		Projection projB = project(b, n);
		float projD{ n.dot(d) };

		projB.min += projD;
		projB.max += projD;

		if (projB.min >= projA.max || projB.max <= projA.min)
			return{ 0.0f,{}, false };

		float p{ abs(projA.max - projB.min) };
		if (p < minPenetration)
		{
			minPenetration = p;
			normal = n;
		}
	}

	return{ minPenetration, normal, true };
}

CollisionSolver::Projection CollisionSolver::project(Shape* shape, Vec2 n)
{
	float min{ std::numeric_limits<float>::max() };
	float max{ std::numeric_limits<float>::lowest() };

	for (size_t v{ 0 }; v < shape->vertices.size(); ++v)
	{
		float proj{ n.dot(shape->vertices[v]) };

		if (proj < min)
			min = proj;

		if (proj > max)
			max = proj;
	}

	return{ min, max };
}