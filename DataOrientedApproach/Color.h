#pragma once

struct Color
{
	Color(float r = 0.0f, float g = 0.0f, float b = 0.0f) : r{ r }, g{ g }, b{ b } {}
	float r;
	float g;
	float b;
};