#include <algorithm>
#include <vector>

#include "Shape.h"

using namespace math;
using namespace std;

vector<float> getShaderData(vector<Shape> const shapes)
{
	vector<float> data;
	data.reserve(shapes.size() * 3 * 3 * 5); // each shape have at least 3 triangles, each triangle have 3 vertices and each vertex holds 5 floats data (see ShapeShaderData)

	for_each(shapes.begin(), shapes.end(), [&data](Shape const shape)
	{
		for (size_t v1{ shape.vertices.size() - 1 }, v2{ 0 }; v2 < shape.vertices.size(); v1 = v2, ++v2)
		{
			data.push_back(shape.position.x);
			data.push_back(shape.position.y);
			data.push_back(shape.color.r);
			data.push_back(shape.color.g);
			data.push_back(shape.color.b);
			
			data.push_back(shape.position.x + shape.vertices[v1].x);
			data.push_back(shape.position.y + shape.vertices[v1].y);
			data.push_back(shape.color.r);
			data.push_back(shape.color.g);
			data.push_back(shape.color.b);
			
			data.push_back(shape.position.x + shape.vertices[v2].x);
			data.push_back(shape.position.y + shape.vertices[v2].y);
			data.push_back(shape.color.r);
			data.push_back(shape.color.g);
			data.push_back(shape.color.b);
		}
	});

	return data;
}