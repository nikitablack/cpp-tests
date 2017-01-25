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
}

ShapesApp& ShapesApp::getInstance(uint32_t const numShapes, float const width, float const height)
{
	static ShapesApp app{ numShapes, width, height };
	return app;
}

ShapesApp::ShapesApp(uint32_t const numShapes, float const width, float const height) : _window{ make_shared<Window>(static_cast<LONG>(width), static_cast<LONG>(height), keyPressHandler) }, _renderer{ 3, _window }
{
	// walls
	_shapes.push_back(make_shared<Shape>(Shape::createWall(10.0f, height, Vec2{ 0.0f, 0.0f })));
	_shapes.push_back(make_shared<Shape>(Shape::createWall(width, 10.0f, Vec2{ 0.0f, 0.0f })));
	_shapes.push_back(make_shared<Shape>(Shape::createWall(10.0f, height, Vec2{ width - 10, 0.0f })));
	_shapes.push_back(make_shared<Shape>(Shape::createWall(width, 10.0f, Vec2{ 0.0f, height - 10 })));

	_numVertices += 16 * 3;

	addShapes(numShapes); 
}

void ShapesApp::addShapes(uint32_t const numShapes)
{
	POINT const wSize(_window->getSize());

	uint32_t numTriangles{ 0 };
	for (uint32_t i{ 0 }; i < numShapes; ++i)
	{
		/*uint32_t numShapeVertices{ 4 + i * 1 };
		float radius{ 100.0f };
		Vec2 pos{ 200.0f + 300.0f * i, 300.0f - 0.0f * i };
		Vec2 vel{ 100.0f - 200.0f * i, 0.0f + 0.0f * i };
		float mass{ 1.0f };
		Color col{ randRange(0.0f, 1.0f), randRange(0.0f, 1.0f), randRange(0.0f, 1.0f) };*/

		uint32_t const numShapeVertices{ static_cast<uint32_t>(randRange(3, 6)) };
		float const radius{ randRange(10.0f, 20.0f) };
		Vec2 const pos{ randRange(40.0f, wSize.x - 40.0f), randRange(40.0f, wSize.y - 40.0f) };
		Vec2 const vel{ randRange(-50.0f, 50.0f), randRange(-50.0f, 50.0f) };
		float const mass{ randRange(1.0f, 5.0f) };
		Color const col{ randRange(0.0f, 1.0f), randRange(0.0f, 1.0f), randRange(0.0f, 1.0f) };

		_shapes.push_back(make_shared<Shape>(numShapeVertices, radius, pos, vel, mass, col));

		numTriangles += numShapeVertices;
	}

	_numVertices += numTriangles * 3;

	_renderer.createVertexBuffer(static_cast<UINT>(_numVertices * sizeof(ShapeShaderData)));
	_shaderData.reserve(_numVertices * 5);
}

void ShapesApp::removeShapes(uint32_t const numShapes)
{
	if (numShapes < _shapes.size())
	{
		uint32_t numTriangles{ 0 };
		for (uint32_t i{ 0 }; i < numShapes; ++i)
		{
			size_t numShapeVertices{ _shapes[_shapes.size() - 1 - i]->vertices.size() };
			numTriangles += static_cast<uint32_t>(numShapeVertices);
		}

		_numVertices -= numTriangles * 3;

		_renderer.createVertexBuffer(static_cast<UINT>(_numVertices * sizeof(ShapeShaderData)));
		_shaderData.reserve(_numVertices * 5);

		_shapes.resize(_shapes.size() - numShapes);
	}
}

vector<shared_ptr<Shape>> const & ShapesApp::getShapes() const
{
	return _shapes;
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

		for (size_t i{ 0 }; i < _shapes.size() - 1; ++i)
		{
			for (size_t j{ i + 1 }; j < _shapes.size(); ++j)
			{
				CollisionSolver::solveCollision(_shapes[i].get(), _shapes[j].get());
			}
		}
	}

	auto const time{ duration_cast<milliseconds>(high_resolution_clock::now() - start) };

	static array<long long, NUM_TIME_SAMPLES> timeSamples;
	static int counter{ 0 };

	if (counter >= NUM_TIME_SAMPLES)
		counter = 0;

	timeSamples[counter++] = time.count();
	long long const sum{ accumulate(timeSamples.begin(), timeSamples.end(), 0LL) };
	string const title{ "num shapes: " + to_string(_shapes.size()) + ", update time: " + to_string(sum / NUM_TIME_SAMPLES) + "ms" };
	_window->setTitle(title);

	fillShaderData();
	_renderer.setVertices(_shaderData);
	_renderer.render(static_cast<UINT>(_shaderData.size() / 5));
}

void ShapesApp::updatePositions(float const dt)
{	
#undef max
#undef min
	for (shared_ptr<Shape> & shape : _shapes)
	{
		// move shape to resolve overlapping and reset accumulator
		shape->position += shape->overlapResolveAccumulator;
		shape->overlapResolveAccumulator *= 0.0f;

		shape->position += (shape->velocity * dt);

		float minX{ std::numeric_limits<float>::max() };
		float maxX{ std::numeric_limits<float>::lowest() };
		float minY{ std::numeric_limits<float>::max() };
		float maxY{ std::numeric_limits<float>::lowest() };

		for (Vec2 const & v : shape->vertices)
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

		shape->bounds = { { shape->position.x + minX, shape->position.y + minY },{ shape->position.x + maxX, shape->position.y + maxY } };
	}
}

void ShapesApp::fillShaderData()
{
	_shaderData.clear();

	for (size_t i{ 0 }; i < _shapes.size(); ++i)
	{
		shared_ptr<Shape> const shape{ _shapes[i] };

		for (size_t v1{ shape->vertices.size() - 1 }, v2{ 0 }; v2 < shape->vertices.size(); v1 = v2, ++v2)
		{
			Vec2 vertex1{ shape->vertices[v1] };
			Vec2 vertex2{ shape->vertices[v2] };

			_shaderData.push_back(shape->position.x);
			_shaderData.push_back(shape->position.y);
			_shaderData.push_back(shape->color.r);
			_shaderData.push_back(shape->color.g);
			_shaderData.push_back(shape->color.b);
			
			_shaderData.push_back(shape->position.x + vertex1.x);
			_shaderData.push_back(shape->position.y + vertex1.y);
			_shaderData.push_back(shape->color.r);
			_shaderData.push_back(shape->color.g);
			_shaderData.push_back(shape->color.b);

			_shaderData.push_back(shape->position.x + vertex2.x);
			_shaderData.push_back(shape->position.y + vertex2.y);
			_shaderData.push_back(shape->color.r);
			_shaderData.push_back(shape->color.g);
			_shaderData.push_back(shape->color.b);
		}
	}
}