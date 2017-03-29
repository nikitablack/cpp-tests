#pragma once

#include <algorithm>
#include <vector>

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
		vertices.insert(vertices.end(), other.vertices.begin(), other.vertices.end());
		positions.insert(positions.end(), other.positions.begin(), other.positions.end());
		velocities.insert(velocities.end(), other.velocities.begin(), other.velocities.end());
		colors.insert(colors.end(), other.colors.begin(), other.colors.end());
		massInverses.insert(massInverses.end(), other.massInverses.begin(), other.massInverses.end());
	}

	size_t size() const
	{
		return positions.size();
	}
};