#pragma once

#include <algorithm>
#include <vector>

#include "CellsRange.h"
#include "Math.h"

struct ShapeShaderData
{
	ShapeShaderData(float const xx, float const yy, float const rr, float const gg, float const bb) :
		x{ xx }, y{ yy }, r{ rr }, g{ gg }, b{ bb }
	{}

	float const x;
	float const y;
	float const r;
	float const g;
	float const b;
};

struct ShapesData
{
	std::vector<std::vector<math::Vec2>> vertices;
	std::vector<math::Vec2> positions;
	std::vector<math::Vec2> velocities;
	std::vector<math::Color> colors;
	std::vector<float> massInverses;

	void concat(ShapesData const & other)
	{
		std::copy(other.vertices.begin(), other.vertices.end(), std::back_inserter(vertices));
		std::copy(other.positions.begin(), other.positions.end(), std::back_inserter(positions));
		std::copy(other.velocities.begin(), other.velocities.end(), std::back_inserter(velocities));
		std::copy(other.colors.begin(), other.colors.end(), std::back_inserter(colors));
		std::copy(other.massInverses.begin(), other.massInverses.end(), std::back_inserter(massInverses));
	}

	size_t size() const
	{
		return positions.size();
	}
};