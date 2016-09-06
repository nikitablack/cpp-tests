#pragma once

#include "Shape.h"

struct Wall : public Shape
{
	Wall(float w, float h, Vec2 pos) : Shape{ 4, 1.0f, pos, { 0.0f, 0.0f }, std::numeric_limits<float>::infinity(), { 1.0f, 0.0f, 0.0f } }
	{
		vertices[0] = { 0.0f, 0.0f };
		vertices[1] = { w, 0.0f };
		vertices[2] = { w, h };
		vertices[3] = { 0.0f, h };
	}
};