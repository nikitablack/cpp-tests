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
}

ShapesOOP::ShapesOOP(uint32_t numShapes) : timeSamples(NUM_TIME_SAMPLES)
{
	// walls
	shapes.push_back(make_shared<Wall>(10.0f, 600.0f, Vec2{ 0.0f, 0.0f }));
	shapes.push_back(make_shared<Wall>(800.0f, 10.0f, Vec2{ 0.0f, 0.0f }));
	shapes.push_back(make_shared<Wall>(10.0f, 600.0f, Vec2{ 790.0f, 0.0f }));
	shapes.push_back(make_shared<Wall>(800.0f, 10.0f, Vec2{ 0.0f, 590.0f }));

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
		/*uint32_t numVertices{ 4 + i * 1 };
		float radius{ 100.0f };
		Vec2 pos{ 200.0f + 300.0f * i, 300.0f - 0.0f * i };
		Vec2 vel{ 100.0f - 200.0f * i, 0.0f + 0.0f * i };
		float mass{ 1.0f };
		Color col{ randRange(0.0f, 1.0f), randRange(0.0f, 1.0f), randRange(0.0f, 1.0f) };*/

		uint32_t numVertices{ static_cast<uint32_t>(randRange(3, 6)) };
		float radius{ randRange(5.0f, 10.0f) };
		Vec2 pos{ randRange(40.0f, 760.0f), randRange(40.0f, 560.0f) };
		Vec2 vel{ randRange(-50.0f, 50.0f), randRange(-50.0f, 50.0f) };
		float mass{ randRange(1.0f, 5.0f) };
		Color col{ randRange(0.0f, 1.0f), randRange(0.0f, 1.0f), randRange(0.0f, 1.0f) };

		shapes.push_back(make_shared<Shape>(numVertices, radius, pos, vel, mass, col));

		numTriangles += numVertices;
	}

	numVertices += numTriangles * 3;

	renderer.createVertexBuffer(static_cast<UINT>(numVertices * sizeof(ShapeShaderData)));
	shaderData.reserve(numVertices * 5);
}

const vector<shared_ptr<class Shape>>& ShapesOOP::getShapes()
{
	return shapes;
}

void ShapesOOP::update(float dt)
{
	auto start = chrono::high_resolution_clock::now();

	updatePositions(dt);

	for (int i{ 0 }; i < shapes.size() - 1; ++i)
	{
		for (int j{ i + 1 }; j < shapes.size(); ++j)
		{
			CollisionSolver::solveCollision(shapes[i].get(), shapes[j].get());
			//shapes[i]->solveCollision(shapes[j].get());
		}
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
	for (uint32_t i{ 0 }; i < shapes.size(); ++i)
	{
		shared_ptr<Shape> shape{ shapes[i] };

		shape->position += (shape->velocity * dt);
		/*if (i == 0)
		{
			//OutputDebugString((to_string(dt) + "\n").data());
			//OutputDebugString(("velocity: " + to_string(shape->velocity.x) + ", " + to_string(shape->velocity.y) + "\n").data());
			//OutputDebugString(("pos: " + to_string(shape->position.x) + ", " + to_string(shape->position.y) + "\n").data());
		}*/
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
