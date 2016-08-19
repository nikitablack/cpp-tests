#pragma once

#include "Graphics.h"

class ShapesDemo : public Graphics
{
public:
	ShapesDemo(UINT bufferCount, std::string name, LONG width, LONG height);
	void render();

private:
};