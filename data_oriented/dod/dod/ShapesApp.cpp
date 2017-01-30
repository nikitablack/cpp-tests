#include <array>
#include <chrono>
#include <numeric>

#include "CollisionSolver.h"
#include "Math.h"
#include "Shape.h"
#include "ShapesApp.h"
#include "Window.h"

using namespace math;
using namespace std;

namespace
{
	int const NUM_TIME_SAMPLES{ 16 };
	uint32_t const NUM_PHYSICS_STEPS{ 20 };

	auto keyPressHandler = [](WPARAM const wParam)
	{
		switch (wParam)
		{
		case 107:
			ShapesApp::getInstance().addShapes(50);
			break;
		case 109:
			ShapesApp::getInstance().removeShapes(50);
			break;
		}
	};

	void createRandomShape(shared_ptr<Window> const window, uint32_t const numVertices, ShapesData & data)
	{
		POINT const wSize(window->getSize());

		vector<Vec2> vert(numVertices);
		float const radius{ randRange(1.0f, 2.0f) };
		float const sector{ math::TWO_PI / numVertices };
		for (uint32_t i{ 0 }; i < numVertices; ++i)
		{
			vert[i] = Vec2{ radius * sin(sector * i), -radius * cos(sector * i) };
		}
		data.vertices.push_back(vert);

		data.positions.emplace_back(randRange(40.0f, wSize.x - 40.0f), randRange(40.0f, wSize.y - 40.0f));
		data.velocities.emplace_back(randRange(-50.0f, 50.0f), randRange(-50.0f, 50.0f));
		data.overlapAccumulators.emplace_back();
		data.massesInverses.push_back(1 / randRange(1.0f, 5.0f));
		data.colors.emplace_back(randRange(0.0f, 1.0f), randRange(0.0f, 1.0f), randRange(0.0f, 1.0f));
		data.bounds.emplace_back();
		data.cellsRanges.emplace_back();
	}

	void createWall(float const w, float const h, Vec2 const pos, ShapesData & data)
	{
		data.positions.push_back(pos);
		data.velocities.emplace_back();
		data.overlapAccumulators.emplace_back();
		data.massesInverses.push_back(1 / numeric_limits<float>::infinity());
		data.colors.emplace_back(1.0f, 0.0f, 0.0f);
		data.bounds.emplace_back();
		data.vertices.push_back({ Vec2{ 0.0f, 0.0f }, Vec2{ w, 0.0f }, Vec2{ w, h }, Vec2{ 0.0f, h } });
		data.cellsRanges.emplace_back();
	}
}

ShapesApp& ShapesApp::getInstance(uint32_t const numShapes, float const width, float const height)
{
	static ShapesApp app{ numShapes, width, height };
	return app;
}

ShapesApp::ShapesApp(uint32_t const numShapes, float const width, float const height) : _window{ make_shared<Window>(static_cast<LONG>(width), static_cast<LONG>(height), keyPressHandler) }, _renderer{ 3, _window }, _grid{ width, height, 100, 50 }
{
	POINT const wSize(_window->getSize());

	// walls
	createWall(10.0f, static_cast<float>(wSize.y), Vec2{ 0.0f, 0.0f }, _data);
	createWall(static_cast<float>(wSize.x), 10.0f, Vec2{ 0.0f, 0.0f }, _data);
	createWall(10.0f, static_cast<float>(wSize.y), Vec2{ wSize.x - 10.0f, 0.0f }, _data);
	createWall(static_cast<float>(wSize.x), 10.0f, Vec2{ 0.0f, wSize.y - 10.0f }, _data);

	_numVertices += 16 * 3;

	addShapes(numShapes); 
}

void ShapesApp::addShapes(uint32_t const numShapes)
{
	POINT const wSize(_window->getSize());

	uint32_t numTriangles{ 0 };
	for (uint32_t i{ 0 }; i < numShapes; ++i)
	{
		uint32_t const numShapeVertices{ static_cast<uint32_t>(randRange(3, 6)) };
		createRandomShape(_window, numShapeVertices, _data);

		numTriangles += numShapeVertices;
	}

	_numVertices += numTriangles * 3;

	_renderer.createVertexBuffer(static_cast<UINT>(_numVertices * sizeof(ShapeShaderData)));
	_shaderData.reserve(_numVertices * 5);
}

void ShapesApp::removeShapes(uint32_t const numShapes)
{
	if (numShapes < _data.positions.size())
	{
		uint32_t numTriangles{ 0 };
		for (uint32_t i{ 0 }; i < numShapes; ++i)
		{
			size_t numShapeVertices{ _data.vertices[_data.positions.size() - 1 - i].size() };
			numTriangles += static_cast<uint32_t>(numShapeVertices);
		}

		_numVertices -= numTriangles * 3;

		_renderer.createVertexBuffer(static_cast<UINT>(_numVertices * sizeof(ShapeShaderData)));
		_shaderData.reserve(_numVertices * 5);

		_data.positions.resize(_data.positions.size() - numShapes);
		_data.velocities.resize(_data.positions.size());
		_data.overlapAccumulators.resize(_data.positions.size());
		_data.massesInverses.resize(_data.positions.size());
		_data.colors.resize(_data.positions.size());
		_data.vertices.resize(_data.positions.size());
		_data.bounds.resize(_data.positions.size());
		_data.cellsRanges.resize(_data.positions.size());
	}
}

void ShapesApp::update(float const dt)
{
	using namespace chrono;

	//dt = 0.0003f;
	auto const start{ high_resolution_clock::now() };

	float const dtStep{ dt / NUM_PHYSICS_STEPS };
	for (uint32_t s{ 0 }; s < NUM_PHYSICS_STEPS; ++s)
	{
		updatePositions(dtStep);
		_grid.reset(_data);
		_grid.solveCollisions(_data);
	}

	auto const time{ duration_cast<milliseconds>(high_resolution_clock::now() - start) };

	static array<long long, NUM_TIME_SAMPLES> timeSamples;
	static int counter{ 0 };

	if (counter >= NUM_TIME_SAMPLES)
		counter = 0;

	timeSamples[counter++] = time.count();
	long long const sum{ accumulate(timeSamples.begin(), timeSamples.end(), 0LL) };
	string const title{ "num shapes: " + to_string(_data.positions.size()) + ", update time: " + to_string(sum / NUM_TIME_SAMPLES) + "ms" };
	_window->setTitle(title);

	fillShaderData();
	_renderer.setVertices(_shaderData);
	_renderer.render(static_cast<UINT>(_shaderData.size() / 5));
}

void ShapesApp::updatePositions(float const dt)
{	
#undef max
#undef min
	for (size_t i{ 0 }; i < _data.positions.size(); ++i)
	{
		Vec2 & pos{ _data.positions[i] };
		Vec2 & overlapAcc{ _data.overlapAccumulators[i] };
		vector<Vec2> const & vert{ _data.vertices[i] };
		Bounds & bounds{ _data.bounds[i] };

		// move shape to resolve overlapping and reset accumulator
		pos += overlapAcc;
		overlapAcc *= 0.0f;

		pos += (_data.velocities[i] * dt);

		float minX{ numeric_limits<float>::max() };
		float maxX{ numeric_limits<float>::lowest() };
		float minY{ numeric_limits<float>::max() };
		float maxY{ numeric_limits<float>::lowest() };

		for (Vec2 const & v : vert)
		{
			if (v.x < minX)
				minX = v.x;

			if (v.x > maxX)
				maxX = v.x;

			if (v.y < minY)
				minY = v.y;

			if (v.y > maxY)
				maxY = v.y;
		}

		bounds = { { pos.x + minX, pos.y + minY },{ pos.x + maxX, pos.y + maxY } };
	}
}

void ShapesApp::fillShaderData()
{
	_shaderData.clear();

	for (size_t i{ 0 }; i < _data.positions.size(); ++i)
	{
		vector<Vec2> const & vert{ _data.vertices[i] };
		Vec2 & pos{ _data.positions[i] };
		Color & col{ _data.colors[i] };

		for (size_t v1{ vert.size() - 1 }, v2{ 0 }; v2 < vert.size(); v1 = v2, ++v2)
		{
			Vec2 vertex1{ vert[v1] };
			Vec2 vertex2{ vert[v2] };

			_shaderData.push_back(pos.x);
			_shaderData.push_back(pos.y);
			_shaderData.push_back(col.r);
			_shaderData.push_back(col.g);
			_shaderData.push_back(col.b);
			
			_shaderData.push_back(pos.x + vertex1.x);
			_shaderData.push_back(pos.y + vertex1.y);
			_shaderData.push_back(col.r);
			_shaderData.push_back(col.g);
			_shaderData.push_back(col.b);

			_shaderData.push_back(pos.x + vertex2.x);
			_shaderData.push_back(pos.y + vertex2.y);
			_shaderData.push_back(col.r);
			_shaderData.push_back(col.g);
			_shaderData.push_back(col.b);
		}
	}
}