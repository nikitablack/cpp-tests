#pragma once

#include <cstdint>
#include <vector>
#include "ShapesDX11.h"

class ShapesOOP
{
public:
	ShapesOOP(uint32_t numShapes);
	void addShapes(uint32_t numToAdd);
	const std::vector<struct Shape*>& getShapes();

	void update(float dt);

private:
	std::vector<struct Shape*> shapes;
	std::vector<float> shaderData;
	ShapesDX11 renderer{ 3, "Shapes OOP", 800, 600 };

private:
	void fillShaderData();
};