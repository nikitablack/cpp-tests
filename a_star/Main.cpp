#include <iostream>
#include <stdexcept>
#include <chrono>
#include <algorithm>
#include "Config.h"
#include "AStar.h"

using namespace astar;
using namespace std;

int main(int argc, char *argv[])
{
	Config config;
	try
	{
		config = getConfig(argc, argv);
	}
	catch (runtime_error err)
	{
		cout << err.what() << endl;
		cin.get();
		return 1;
	}

	NodePtrVector nodes;
	try
	{
		nodes = readMapFile(config.mapFileName);
	}
	catch (runtime_error err)
	{
		cout << err.what() << endl;
		cin.get();
		return 1;
	}

	AStar aStar{ nodes, MAP_SIZE_X, MAP_SIZE_Y };

	auto start = chrono::high_resolution_clock::now();
	NodePtrVector path{ aStar.find(config.startX, config.startY, config.endX, config.endY) };
	auto duration = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start);

	cout << "Start position: (" << config.startX << ", " << config.startY << ")" << endl;
	cout << "End position: (" << config.endX << ", " << config.endY << ")" << endl;
	cout << endl;
	cout << "Path found: " << boolalpha << (path.size() > 0) << endl;
	cout << endl;
	cout << "Total duration: " << duration.count() / 1000.0f << "ms" << endl;
	cout << endl;

	if (config.showMap && path.size() > 0)
	{
		int num{ 0 };
		for (auto& node : nodes)
		{
			auto it = find_if(path.begin(), path.end(), [&node](const NodePtr n)
			{
				return n->xPos == node->xPos && n->yPos == node->yPos;
			});

			if (it != path.end())
			{
				cout << ('+');
			}
			else
			{
				cout << (node->block ? '*' : '.');
			}

			++num;

			if (num % MAP_SIZE_X == 0) cout << endl;
		}
	}

	cin.get();
	return 0;
}