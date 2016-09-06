#pragma once

#include <vector>
#include <cstdint>
#include <cassert>
#include <limits>
#include <algorithm>
#include <utility>
#include "Vec2.h"
#include "Color.h"
#include "Math.h"

#undef max
#undef min

struct ShapeShaderData
{
	float x;
	float y;
	float r;
	float g;
	float b;
};

struct Shape
{
	Vec2 position{ 0.0f, 0.0f };
	Vec2 velocity{ 0.0f, 0.0f };
	float massInverse;
	float mass;
	Color color;
	std::vector<Vec2> vertices;

	Shape(uint32_t numVertices, float radius, Vec2 pos, Vec2 vel, float m, Color col) : position{ pos }, velocity{ vel }, massInverse{ 1 / m }, mass{ m }, color { col }
	{
		assert(numVertices > 2);
		assert(radius > 0.0f);

		float sector{ math::TWO_PI / numVertices };

		for (uint32_t i{ 0 }; i < numVertices; ++i)
		{
			vertices.emplace_back(radius * std::sin(sector * i), -radius * std::cos(sector * i));
		}
	}

	bool checkCollision(Shape* other)
	{
		if (this == other)
			return false;

		if (massInverse == 0 && other->massInverse == 0)
			return false;
		
		//Vec2 d{ other->position - position };
		Vec2 vr{ velocity - other->velocity };

		//if (d.dot(vr) <= 0)
			//return false;

		auto result1 = checkOverlap(this, other);
		if (!result1.first)
			return false;

		auto result2 = checkOverlap(other, this);
		if (!result2.first)
			return false;

		float d{ std::min(result1.second.first, result2.second.first) };
		Vec2 n{ (result1.second.first < result2.second.first) ? result1.second.second : result2.second.second };

		float d1{ d * massInverse / (massInverse + other->massInverse) };
		float d2{ d - d1 };

		if (result1.second.first >= result2.second.first)
		{
			d1 = -d1;
			d2 = -d2;
		}

		position -= n * d1;
		other->position += n * d2;

		//Vec2 vn{ vr - n * 2 * n.dot(vr) };
		
		float j{ -n.dot(vr) * 2 / (massInverse + other->massInverse) };

		velocity += n * (j * massInverse);
		other->velocity -= n * (j * other->massInverse);

		//velocity += vn * (massInverse) * (1 / (massInverse + other->massInverse));
		//other->velocity -= vn * (other->massInverse) * (1 / (massInverse + other->massInverse));

		//velocity = (u1 * (mass - other->mass) + u2 * 2 * other->mass) * (1 / (mass + other->mass));
		//other->velocity = (u2 * (other->mass - mass) + u1 * 2 * mass) * (1 / (mass + other->mass));

		return true;
	}

private:
	std::pair<bool, std::pair<float, Vec2>> checkOverlap(Shape* a, Shape* b)
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

			auto projA = project(a, n);
			auto projB = project(b, n);
			float projD{ n.dot(d) };

			projB.first += projD;
			projB.second += projD;

			if (projB.first >= projA.second || projB.second <= projA.first)
				return{ false, {} };

			float p{ std::abs(projA.second - projB.first) };
			if (p < minPenetration)
			{
				minPenetration = p;
				normal = n;
			}
		}

		return{ true, { minPenetration, normal} };
	}

	std::pair<float, float> project(Shape* shape, Vec2 n)
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
};