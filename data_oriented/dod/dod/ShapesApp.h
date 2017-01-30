#pragma once

#include <memory>
#include <vector>

#include "Shape.h"
#include "ShapesRenderer.h"

class ShapesApp
{
public:
	static ShapesApp& getInstance(uint32_t const numShapes = 0, float const width = 0.0f, float const height = 0.0f);
	
	void addShapes(uint32_t const numShapes);
	void removeShapes(uint32_t const numShapes);
	void update(float const dt);

private:
	ShapesApp(uint32_t const numShapes, float const width, float const height);
	void updatePositions(float const dt);
	void fillShaderData();

private:
	ShapesData _data;
	std::shared_ptr<class Window> _window;
	std::vector<float> _shaderData;
	ShapesRenderer _renderer;
	uint32_t _numVertices{ 0 };
};