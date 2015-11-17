#include "Config.h"

using namespace astar;
using namespace std;

namespace {
	const int MAP_MIN_X{ 0 };
	const int MAP_MIN_Y{ 0 };

	int intFromString(string str)
	{
		try
		{
			return stoi(str);
		}
		catch (...)
		{
			throw runtime_error("\"" + str + "\"" + " can not be converted to int");
		}
	}
}

Config astar::getConfig(int argc, char *argv[])
{
	if (argc < 6)
	{
		throw runtime_error("Not enough command line parameters");
	}

	Config config;
	config.mapFileName = argv[1];

	try
	{
		config.startX = intFromString(argv[2]);
		config.startY = intFromString(argv[3]);
		config.endX = intFromString(argv[4]);
		config.endY = intFromString(argv[5]);
	}
	catch (runtime_error err)
	{
		throw;
	}

	if (config.startX < MAP_MIN_X || config.startX >= MAP_MIN_X + MAP_SIZE_X || config.startY < MAP_MIN_Y || config.startY >= MAP_MIN_Y + MAP_SIZE_X)
	{
		throw runtime_error("Start position is out of bounds");
	}

	if (config.endX < MAP_MIN_X || config.endX >= MAP_MIN_X + MAP_SIZE_X || config.endY < MAP_MIN_Y || config.endY >= MAP_MIN_Y + MAP_SIZE_X)
	{
		throw runtime_error("End position is out of bounds");
	}

	config.showMap = (argc > 6 && strcmp(argv[6], "showmap") == 0);

	return config;
}