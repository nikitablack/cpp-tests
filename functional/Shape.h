#pragma once

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

struct VelocityAfterImpact
{
	VelocityAfterImpact(std::uint32_t const uniqueId, math::Vec2 const vel) :
		id{ uniqueId }, velocity{ vel }
	{}

	std::uint32_t const id;
	math::Vec2 const velocity;
};

struct Shape
{
	Shape(std::uint32_t const uniqueId, std::vector<math::Vec2> const vert, math::Vec2 const pos, math::Vec2 const vel, math::Bounds const b, CellsRange const cellRng, math::Color const col, float const massInv) :
		id{ uniqueId }, vertices { vert }, position{ pos }, velocity{ vel }, bounds{ b }, cellsRange{ cellRng }, color{ col }, massInverse{ massInv }
	{}

	std::uint32_t const id;
	std::vector<math::Vec2> const vertices;
	math::Vec2 const position;
	math::Vec2 const velocity;
	math::Bounds const bounds;
	CellsRange const cellsRange;
	math::Color const color;
	float const massInverse;
};