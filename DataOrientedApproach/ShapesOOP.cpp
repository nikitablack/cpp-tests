#include <chrono>
#include <numeric>
#include "ShapesOOP.h"
#include "Wall.h"
#include "Math.h"
#include "Window.h"
#include "CollisionSolver.h"

using namespace std;
using namespace math;

namespace
{
	int NUM_TIME_SAMPLES{ 16 };
	uint32_t NUM_PHYSICS_STEPS{ 20 };
}

ShapesOOP::ShapesOOP(uint32_t numShapes, float width, float height) : renderer{ 3, "Shapes OOP", static_cast<LONG>(width), static_cast<LONG>(height) }, timeSamples(NUM_TIME_SAMPLES), grid{ width, height, /*10, 10*/100, 50 }, width{ width }, height{ height }
{
	// walls
	shapes.push_back(make_shared<Wall>(10.0f, height, Vec2{ 0.0f, 0.0f }));
	shapes.push_back(make_shared<Wall>(width, 10.0f, Vec2{ 0.0f, 0.0f }));
	shapes.push_back(make_shared<Wall>(10.0f, height, Vec2{ width - 10, 0.0f }));
	shapes.push_back(make_shared<Wall>(width, 10.0f, Vec2{ 0.0f, height - 10 }));

	numVertices += 16 * 3;

	addShapes(numShapes);

	auto lambda = [this](WPARAM wParam)
	{
		switch (wParam)
		{
		case 107:
			addShapes(50);
			break;
		case 109:
			removeShapes(50);
			break;
		}
	};
	shared_ptr<function<void(WPARAM)>> onKeyPress{ make_shared<function<void(WPARAM)>>(lambda) };
	renderer.getWindow()->addKeyPressCallback(onKeyPress);
}

void ShapesOOP::addShapes(uint32_t numShapes)
{
	uint32_t numTriangles{ 0 };
	for (uint32_t i{ 0 }; i < numShapes; ++i)
	{
		/*uint32_t numShapeVertices{ 6 };
		float radius{ 100.0f };
		Vec2 pos{ 500.0f, 400.0f };
		Vec2 vel{ -100.0f, 0.0f };
		float mass{ 1.0f };
		Color col{ randRange(0.0f, 1.0f), randRange(0.0f, 1.0f), randRange(0.0f, 1.0f) };*/

		/*uint32_t numShapeVertices{ 4 + i * 1 };
		float radius{ 100.0f };
		Vec2 pos{ 200.0f + 300.0f * i, 300.0f - 0.0f * i };
		Vec2 vel{ 100.0f - 200.0f * i, 0.0f + 0.0f * i };
		float mass{ 1.0f };
		Color col{ randRange(0.0f, 1.0f), randRange(0.0f, 1.0f), randRange(0.0f, 1.0f) };*/

		uint32_t numShapeVertices{ static_cast<uint32_t>(randRange(3, 6)) };
		float radius{ randRange(1.0f, 2.0f) };
		Vec2 pos{ randRange(40.0f, width - 40.0f), randRange(40.0f, height - 40.0f) };
		Vec2 vel{ randRange(-50.0f, 50.0f), randRange(-50.0f, 50.0f) };
		float mass{ randRange(1.0f, 5.0f) };
		Color col{ randRange(0.0f, 1.0f), randRange(0.0f, 1.0f), randRange(0.0f, 1.0f) };

		shapes.push_back(make_shared<Shape>(numShapeVertices, radius, pos, vel, mass, col));

		numTriangles += numShapeVertices;
	}

	numVertices += numTriangles * 3;

	renderer.createVertexBuffer(static_cast<UINT>(numVertices * sizeof(ShapeShaderData)));
	shaderData.reserve(numVertices * 5);
}

void ShapesOOP::removeShapes(uint32_t numShapes)
{
	if (numShapes < shapes.size())
	{
		uint32_t numTriangles{ 0 };
		for (uint32_t i{ 0 }; i < numShapes; ++i)
		{
			size_t numShapeVertices{ shapes[shapes.size() - 1 - i]->vertices.size() };
			numTriangles += static_cast<uint32_t>(numShapeVertices);
		}

		numVertices -= numTriangles * 3;

		renderer.createVertexBuffer(static_cast<UINT>(numVertices * sizeof(ShapeShaderData)));
		shaderData.reserve(numVertices * 5);

		shapes.resize(shapes.size() - numShapes);
	}
}

const vector<shared_ptr<class Shape>>& ShapesOOP::getShapes()
{
	return shapes;
}

void ShapesOOP::update(float dt)
{
	//dt = 0.003f;
	auto start = chrono::high_resolution_clock::now();

	float dtStep{ dt / NUM_PHYSICS_STEPS };
	for (uint32_t s{ 0 }; s < NUM_PHYSICS_STEPS; ++s)
	{
		updatePositions(dtStep);
		updateGrid();
		grid.solveCollisions();

		/*for (int i{ 0 }; i < shapes.size() - 1; ++i)
		{
			for (int j{ i + 1 }; j < shapes.size(); ++j)
			{
				CollisionSolver::solveCollision(shapes[i].get(), shapes[j].get());
			}
		}*/
	}

	auto end = chrono::high_resolution_clock::now();
	auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

	if (counter >= NUM_TIME_SAMPLES)
		counter = 0;

	timeSamples[counter] = time.count();
	long long sum{ accumulate(timeSamples.begin(), timeSamples.end(), 0LL) };
	++counter;
	string title{ "num shapes: " + to_string(shapes.size()) + ", update time: " + to_string(sum / NUM_TIME_SAMPLES) + "ms" };
	renderer.getWindow()->setTitle(title);

	fillShaderData();
	renderer.setVertices(shaderData);
	renderer.render(static_cast<UINT>(shaderData.size() / 5));
}

void ShapesOOP::updatePositions(float dt)
{
	//dt = 0.001f;
	for (shared_ptr<Shape>& shape : shapes)
	{
		shape->position += (shape->velocity * dt);

		float minX{ std::numeric_limits<float>::max() };
		float maxX{ std::numeric_limits<float>::lowest() };
		float minY{ std::numeric_limits<float>::max() };
		float maxY{ std::numeric_limits<float>::lowest() };

		for (const Vec2& v : shape->vertices)
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

void ShapesOOP::updateGrid()
{
	grid.clear();

	for (shared_ptr<Shape>& shape : shapes)
	{
		grid.addShape(shape);
	}
}

void ShapesOOP::fillShaderData()
{
	shaderData.clear();

	for (size_t i{ 0 }; i < shapes.size(); ++i)
	{
		shared_ptr<Shape> shape{ shapes[i] };
		Vec2 pos{ shape->position };

		for (size_t v1{ shape->vertices.size() - 1 }, v2{ 0 }; v2 < shape->vertices.size(); v1 = v2, ++v2)
		{
			Vec2 vertex1{ shape->vertices[v1] };
			Vec2 vertex2{ shape->vertices[v2] };

			shaderData.push_back(shape->position.x);
			shaderData.push_back(shape->position.y);
			shaderData.push_back(shape->color.r);
			shaderData.push_back(shape->color.g);
			shaderData.push_back(shape->color.b);

			shaderData.push_back(shape->position.x + vertex1.x);
			shaderData.push_back(shape->position.y + vertex1.y);
			shaderData.push_back(shape->color.r);
			shaderData.push_back(shape->color.g);
			shaderData.push_back(shape->color.b);

			shaderData.push_back(shape->position.x + vertex2.x);
			shaderData.push_back(shape->position.y + vertex2.y);
			shaderData.push_back(shape->color.r);
			shaderData.push_back(shape->color.g);
			shaderData.push_back(shape->color.b);
		}
	}
}
