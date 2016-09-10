#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "ShapesDX11.h"

class ShapesOOP
{
public:
	ShapesOOP(uint32_t numShapes);
	void addShapes(uint32_t numShapes);
	const std::vector<std::shared_ptr<class Shape>>& getShapes();

	void update(float dt);

private:
	std::vector<std::shared_ptr<class Shape>> shapes;
	std::vector<float> shaderData;
	ShapesDX11 renderer{ 3, "Shapes OOP", 800, 600 };
	std::vector<long long> timeSamples;
	int counter{ 0 };
	uint32_t numVertices{ 0 };

private:
	void updatePositions(float dt);
	void fillShaderData();
};