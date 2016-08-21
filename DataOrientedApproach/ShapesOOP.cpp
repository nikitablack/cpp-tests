#include "ShapesOOP.h"
#include "Shape.h"

using namespace std;

ShapesOOP::ShapesOOP(uint32_t numShapes)
{
	uint32_t numTriangles{ 0 };
	for (uint32_t i{ 0 }; i < numShapes; ++i)
	{
		shapes.push_back(new Shape{ 4, 50.0f, 0x0000AA00 });
		shapes[0]->position = { 100.0f, 100.0f };
		numTriangles += 4;
	}

	uint32_t numVertices{ numTriangles * 3 };

	renderer.createVertexBuffer(static_cast<UINT>(numVertices * sizeof(ShapeShaderData)));
	shaderData.reserve(numVertices * 5);
}

void ShapesOOP::addShapes(uint32_t numToAdd)
{
	for (uint32_t i{ 0 }; i < numToAdd; ++i)
	{
		shapes.push_back(new Shape{ 4, 1.0f, 0x0000AA00 });
	}
}

const vector<Shape*>& ShapesOOP::getShapes()
{
	return shapes;
}

void ShapesOOP::update(float dt)
{
	fillShaderData();
	renderer.setVertices(shaderData);
	renderer.render();
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
