#pragma once
#include <string>

namespace astar
{
	struct Config
	{
		std::string mapFileName;
		int startX;
		int startY;
		int endX;
		int endY;
		bool showMap;
	};

	Config getConfig(int argc, char *argv[]);

	const int MAP_SIZE_X{ 126 };
	const int MAP_SIZE_Y{ 126 };
};