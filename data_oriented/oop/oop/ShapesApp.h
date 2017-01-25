#pragma once

#include <memory>
#include <vector>

#include "Grid.h"
#include "ShapesRenderer.h"
#include "ThreadPool.h"

class ShapesApp
{
public:
	static ShapesApp& getInstance(uint32_t const numShapes = 0, float const width = 0.0f, float const height = 0.0f);
	
	void addShapes(uint32_t const numShapes);
	void removeShapes(uint32_t const numShapes);
	std::vector<std::shared_ptr<class Shape>> const & getShapes() const;
	void update(float const dt);

private:
	ShapesApp(uint32_t const numShapes, float const width, float const height);
	void updatePositions(float const dt);
	void fillShaderData();

private:
	std::shared_ptr<class Window> _window;
	std::vector<std::shared_ptr<class Shape>> _shapes;
	std::vector<float> _shaderData;
	ShapesRenderer _renderer;
	uint32_t _numVertices{ 0 };
	Grid _grid;
	ThreadPool _threadPool;
};