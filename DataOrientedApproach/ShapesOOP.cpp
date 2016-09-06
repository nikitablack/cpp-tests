#include "ShapesOOP.h"
#include "Wall.h"
#include "Math.h"

using namespace std;
using namespace math;

ShapesOOP::ShapesOOP(uint32_t numShapes)
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
		float radius{ randRange(10.0f, 30.0f) };
		Vec2 pos{ randRange(40.0f, 760.0f), randRange(40.0f, 560.0f) };
		Vec2 vel{ randRange(-500.0f, 500.0f), randRange(-500.0f, 500.0f) };
		float mass{ randRange(1.0f, 5.0f) };
		Color col{ randRange(0.0f, 1.0f), randRange(0.0f, 1.0f), randRange(0.0f, 1.0f) };
		
		shapes.push_back(new Shape{ numVertices, radius, pos, vel, mass, col });

		numTriangles += numVertices;
	}

	uint32_t numVertices{ numTriangles * 3 };

	shapes.push_back(new Wall{ 10.0f, 600.0f,{ 0.0f, 0.0f } });
	shapes.push_back(new Wall{ 800.0f, 10.0f, {0.0f, 0.0f} });
	shapes.push_back(new Wall{ 10.0f, 600.0f,{ 790.0f, 0.0f } });
	shapes.push_back(new Wall{ 800.0f, 10.0f,{ 0.0f, 590.0f } });

	numVertices += 4 * 3;
	numVertices += 4 * 3;
	numVertices += 4 * 3;
	numVertices += 4 * 3;

	renderer.createVertexBuffer(static_cast<UINT>(numVertices * sizeof(ShapeShaderData)));
	shaderData.reserve(numVertices * 5);

	//shapes[0]->checkCollision(shapes[1]);
}

void ShapesOOP::addShapes(uint32_t numToAdd)
{
	/*for (uint32_t i{ 0 }; i < numToAdd; ++i)
	{
		shapes.push_back(new Shape{ 4, 1.0f, 0x0000AA00 });
	}*/
}

const vector<Shape*>& ShapesOOP::getShapes()
{
	return shapes;
}

void ShapesOOP::update(float dt)
{
	updatePositions(dt);

	for (int i{ 0 }; i < shapes.size() - 1; ++i)
	{
		for (int j{ i + 1 }; j < shapes.size(); ++j)
		{
			shapes[i]->checkCollision(shapes[j]);
		}
	}

	fillShaderData();
	renderer.setVertices(shaderData);
	renderer.render(static_cast<UINT>(shaderData.size() / 5));
}

void ShapesOOP::updatePositions(float dt)
{
	dt = 0.001f;
	for (uint32_t i{ 0 }; i < shapes.size(); ++i)
	{
		Shape* shape{ shapes[i] };

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
		Shape* shape{ shapes[i] };
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
