#include <algorithm>
#include <vector>

#include "Shape.h"

using namespace math;
using namespace std;

vector<float> const & getShaderData(ShapesData const & shapes)
{
	static vector<float> data;
	data.clear();
	data.reserve(shapes.size() * 3 * 3 * 5); // each shape have at least 3 triangles, each triangle have 3 vertices and each vertex holds 5 floats data (see ShapeShaderData)

	for (size_t i{ 0 }; i < shapes.size(); ++i)
	{
		vector<Vec2> const & vertices{ shapes.vertices[i] };
		Vec2 const & position{ shapes.positions[i] };
		Color const & color{ shapes.colors[i] };

		for (size_t v1{ vertices.size() - 1 }, v2{ 0 }; v2 < vertices.size(); v1 = v2, ++v2)
		{
			data.push_back(position.x);
			data.push_back(position.y);
			data.push_back(color.r);
			data.push_back(color.g);
			data.push_back(color.b);
			
			data.push_back(position.x + vertices[v1].x);
			data.push_back(position.y + vertices[v1].y);
			data.push_back(color.r);
			data.push_back(color.g);
			data.push_back(color.b);
			
			data.push_back(position.x + vertices[v2].x);
			data.push_back(position.y + vertices[v2].y);
			data.push_back(color.r);
			data.push_back(color.g);
			data.push_back(color.b);
		}
	};

	return data;
}