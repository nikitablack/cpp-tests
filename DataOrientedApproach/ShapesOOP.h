#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "ShapesDX11.h"
#include "Grid.h"

class ShapesOOP
{
public:
	ShapesOOP(uint32_t numShapes, float width, float height);
	void addShapes(uint32_t numShapes);
	void removeShapes(uint32_t numShapes);
	const std::vector<std::shared_ptr<class Shape>>& getShapes();

	void update(float dt);

private:
	std::vector<std::shared_ptr<class Shape>> shapes;
	std::vector<float> shaderData;
	ShapesDX11 renderer;
	std::vector<long long> timeSamples;
	int counter{ 0 };
	uint32_t numVertices{ 0 };
	Grid grid;
	float width;
	float height;

private:
	void updatePositions(float dt);
	void updateGrid();
	void fillShaderData();
};