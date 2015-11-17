#include "AStar.h"
#include <fstream>
#include <stdexcept>
#include <cstdlib>
#include <algorithm>
#include <limits>
#include "Config.h"

using namespace astar;
using namespace std;

NodePtrVector astar::readMapFile(string mapFilePath)
{
	ifstream inFile(mapFilePath);
	
	if (!inFile.is_open())
	{
		throw(runtime_error{ "Can't read " + mapFilePath + " file" });
	}

	NodePtrVector nodes;

	for (int i{ 0 }; i < MAP_SIZE_X; ++i)
	{
		for (int j{ 0 }; j < MAP_SIZE_Y; ++j)
		{
			char c;
			inFile >> c;

			if ((inFile.rdstate() & ios_base::failbit) == ios_base::failbit)
			{
				throw(runtime_error{ "Error reading row " + to_string(i) + ", column " + to_string(j) });
			}

			if (c != '.' && c != '*')
			{
				throw(runtime_error{ "Unsupported symbol " + string{ c } +" at row " + to_string(i) + ", column " + to_string(j) });
			}

			nodes.emplace_back(make_shared<Node>(Node{ j, i, (c == '*') }));
		}
	}

	return nodes;
}

astar::AStar::AStar(NodePtrVector nodes, int sizeW, int sizeH) : allNodes{ nodes }, SIZE_W{ sizeW }, SIZE_H{ sizeH }
{
	if (allNodes.size() < SIZE_W * SIZE_H)
	{
		throw runtime_error{ "Wrong array size" };
	}
}

const auto astar::AStar::getNodeIterator(const NodePtr node, const NodePtrVector& list) const
{
	const auto it = find_if(list.begin(), list.end(), [&node](const NodePtr n)
	{
		return n->xPos == node->xPos && n->yPos == node->yPos;
	});

	return it;
}

NodePtrVector astar::AStar::find(int startX, int startY, int endX, int endY)
{
	// reset to default - ugly
	for (auto& n : allNodes)
	{
		n->g = 0;
		n->h = 0;
		n->parent = nullptr;
	}

	opened.clear();
	closed.clear();

	NodePtr startNode{ getNodeByPosition(startX, startY) };

	if (startX == endX && startY == endY)
	{
		return { startNode };
	}

	NodePtr endNode;
	NodePtr node{ startNode };
	while (true)
	{
		const auto it = getNodeIterator(node, opened);

		if (it != opened.end())
		{
			opened.erase(it);
		}

		closed.push_back(node);

		endNode = updateNeighbors(node, endX, endY);

		if (endNode != nullptr)
		{
			break;
		}
		else
		{
			if (opened.size() > 0)
			{
				// get node with lowest g
				int currG{ numeric_limits<int>::max() };
				int currInd{ 0 };
				int ind{ 0 };
				for (auto n : opened)
				{
					if (n->g < currG)
					{
						node = n;
						currG = n->g;
						ind = currInd;
					}

					++currInd;
				}
				opened.erase(opened.begin() + ind);
			}
			else
			{
				break;
			}
		}
	}

	NodePtrVector result;
	if (endNode != nullptr)
	{
		NodePtr n{ endNode };
		while (n != nullptr)
		{
			result.push_back(n);

			n = n->parent;
		}

		reverse(result.begin(), result.end());
	}

	return result;
}

bool astar::AStar::isInList(const NodePtr node, const NodePtrVector& list) const
{
	return getNodeIterator(node, list) != list.end();
}

NodePtr astar::AStar::updateNeighbors(NodePtr parent, int endX, int endY)
{
	// check if a node have neighbors on the sides
	bool haveLeft{ parent->xPos > 0 };
	bool haveRight{ parent->xPos < SIZE_W - 1 };
	bool haveTop{ parent->yPos > 0 };
	bool haveBottom{ parent->yPos < SIZE_H - 1 };

	if (haveLeft)
	{
		NodePtr child{ getNodeByPosition(parent->xPos - 1, parent->yPos) }; // left
		if (!child->block)
		{
			if (updateNeighbor(parent, child, endX, endY)) return child;
		}

		if (haveTop)
		{
			NodePtr child{ getNodeByPosition(parent->xPos - 1, parent->yPos - 1) }; // top left
			if (!child->block)
			{
				if (updateNeighbor(parent, child, endX, endY)) return child;
			}
		}

		if (haveBottom)
		{
			NodePtr child{ getNodeByPosition(parent->xPos - 1, parent->yPos + 1) }; // bottom left
			if (!child->block)
			{
				if (updateNeighbor(parent, child, endX, endY)) return child;
			}
		}
	}

	if (haveRight)
	{
		NodePtr child{ getNodeByPosition(parent->xPos + 1, parent->yPos) }; // right
		if (!child->block)
		{
			if (updateNeighbor(parent, child, endX, endY)) return child;
		}

		if (haveTop)
		{
			NodePtr child{ getNodeByPosition(parent->xPos + 1, parent->yPos - 1) }; // top right
			if (!child->block)
			{
				if (updateNeighbor(parent, child, endX, endY)) return child;
			}
		}

		if (haveBottom)
		{
			NodePtr child{ getNodeByPosition(parent->xPos + 1, parent->yPos + 1) }; // bottom right
			if (!child->block)
			{
				if (updateNeighbor(parent, child, endX, endY)) return child;
			}
		}
	}

	if (haveTop)
	{
		NodePtr child{ getNodeByPosition(parent->xPos, parent->yPos - 1) }; // top
		if (!child->block)
		{
			if (updateNeighbor(parent, child, endX, endY)) return child;
		}
	}

	if (haveBottom)
	{
		NodePtr child{ getNodeByPosition(parent->xPos, parent->yPos + 1) }; // bottom
		if (!child->block)
		{
			if (updateNeighbor(parent, child, endX, endY)) return child;
		}
	}

	return nullptr;
}

bool astar::AStar::updateNeighbor(NodePtr parent, NodePtr child, int endX, int endY)
{
	// if a node is in a closed list - do nothing
	if (isInList(child, closed))
	{
		return false;
	}

	// move cost from the start
	int g{ parent->g + getG(parent, child) };

	if (!isInList(child, opened))
	{
		child->h = getH(child, endX, endY); // heuristic move cost to the end
		child->g = g;

		child->parent = parent;

		if (child->xPos == endX && child->yPos == endY)
		{
			return true;
		}

		opened.push_back(child);
	}
	else
	{
		// update move cost from the start if necessary
		if (child->g > g)
		{
			child->g = g;
			child->parent = parent;
		}
	}

	return false;
}

int astar::AStar::getG(const NodePtr parent, const NodePtr child) const
{
	// if diagonal
	if (parent->xPos != child->xPos && parent->yPos != child->yPos)
	{
		return 14;
	}

	return 10;
}

int astar::AStar::getH(const NodePtr node, const int endX, const int endY) const
{
	return 10 * (abs(endX - node->xPos) + abs(endY - node->yPos));
}

NodePtr astar::AStar::getNodeByPosition(const int posX, const int posY)
{
	return allNodes[posY * SIZE_W + posX];
}