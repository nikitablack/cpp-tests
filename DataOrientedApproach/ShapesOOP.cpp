#include "ShapesOOP.h"
#include "Shape.h"
#include <random>

using namespace std;

ShapesOOP::ShapesOOP(uint32_t numShapes)
{
	random_device r;
	default_random_engine engine(r());
	uniform_int_distribution<uint32_t> numVerticesDistr(3, 6);
	uniform_real_distribution<float> radiusDistr(10.0f, 30.0f);
	uniform_real_distribution<float> velDistrX(-50.0f, 50.0f);
	uniform_real_distribution<float> velDistrY(-50.0f, 50.0f);
	uniform_real_distribution<float> posDistrX(20.0f, 780.0f);
	uniform_real_distribution<float> posDistrY(20.0f, 580.0f);
	uniform_real_distribution<float> massDistr(1.0f, 5.0f);
	uniform_int_distribution<uint32_t> colorDistr(0, 0xffffffff);

	uint32_t numTriangles{ 0 };
	for (uint32_t i{ 0 }; i < numShapes; ++i)
	{
		uint32_t numVertices{ numVerticesDistr(engine) };

		shapes.push_back(new Shape{ numVertices, radiusDistr(engine), { posDistrX(engine), posDistrY(engine) }, { velDistrX(engine), velDistrY(engine) }, massDistr(engine), colorDistr(engine) });
		numTriangles += numVertices;
	}

	uint32_t numVertices{ numTriangles * 3 };

	renderer.createVertexBuffer(static_cast<UINT>(numVertices * sizeof(ShapeShaderData)));
	shaderData.reserve(numVertices * 5);
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
	fillShaderData();
	renderer.setVertices(shaderData);
	renderer.render(shaderData.size() / 5);
}

void ShapesOOP::updatePositions(float dt)
{
	for (uint32_t i{ 0 }; i < shapes.size(); ++i)
	{
		Shape* shape{ shapes[i] };

		shape->position += (shape->velocity * dt);
	}
}

void ShapesOOP::fillShaderData()
{
	shaderData.clear();

	for (size_t i{ 0 }; i < shapes.size(); ++i)
	{
		Shape* shape{ shapes[i] };
		Vec2 pos{ shape->position };
		uint32_t col{ shape->color };

		for (size_t v1{ shape->vertices.size() - 1 }, v2{ 0 }; v2 < shape->vertices.size(); v1 = v2, ++v2)
		{
			static float oneOver255{ 1.0f / 0xff };

			Vec2 vertex1{ shape->vertices[v1] };
			Vec2 vertex2{ shape->vertices[v2] };

			shaderData.push_back(shape->position.x);
			shaderData.push_back(shape->position.y);
			shaderData.push_back(((shape->color >> 16) & 0xff) * oneOver255);
			shaderData.push_back(((shape->color >> 8) & 0xff) * oneOver255);
			shaderData.push_back((shape->color & 0xff) * oneOver255);

			shaderData.push_back(shape->position.x + vertex2.x);
			shaderData.push_back(shape->position.y + vertex2.y);
			shaderData.push_back(((shape->color >> 16) & 0xff) * oneOver255);
			shaderData.push_back(((shape->color >> 8) & 0xff) * oneOver255);
			shaderData.push_back((shape->color & 0xff) * oneOver255);

			shaderData.push_back(shape->position.x + vertex1.x);
			shaderData.push_back(shape->position.y + vertex1.y);
			shaderData.push_back(((shape->color >> 16) & 0xff) * oneOver255);
			shaderData.push_back(((shape->color >> 8) & 0xff) * oneOver255);
			shaderData.push_back((shape->color & 0xff) * oneOver255);
		}
	}
}
