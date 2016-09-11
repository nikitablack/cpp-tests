#pragma once

#include <cstdlib>
#include <cmath>

namespace math
{
	const float PI{ 3.14159265359f };
	const float TWO_PI{ 2 * PI };

	inline int randRange(int from, int to)
	{
		return from + static_cast<int>(std::round((to - from) * static_cast<float>(rand()) / RAND_MAX));
	}

	inline float randRange(float from, float to)
	{
		return from + (to - from) * static_cast<float>(rand()) / RAND_MAX;
	}

	struct Bounds
	{
		Vec2 topLeft;
		Vec2 bottomRight;
	};
}