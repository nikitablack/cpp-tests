#include "AStar.h"
#include <fstream>
#include <stdexcept>
#include <algorithm>

using namespace astar;
using namespace std;

vector<Node> astar::readMapFile(string mapFilePath, int sizeW, int sizeH)
{
	ifstream inFile(mapFilePath);

	if (!inFile.is_open())
	{
		throw(runtime_error{ "Can't read " + mapFilePath + " file" });
	}

	vector<Node> nodes;

	for (int i{ 0 }; i < sizeW; ++i)
	{
		for (int j{ 0 }; j < sizeH; ++j)
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

			nodes.push_back(Node{ j, i, (c == '*') });
		}
	}

	return nodes;
}

AStar::AStar(vector<Node>& InNodes, int sizeW, int sizeH) : SIZE_W{ sizeW }, SIZE_H{ sizeH }
{
	for (const Node& node : InNodes)
	{
		nodes.push_back(NodeInternal{ node, nullptr, 0, 0, false, false });
	}
}

vector<Node> AStar::find(int startX, int startY, int endX, int endY)
{
	if (startX == endX && startY == endY)
	{
		return{};
	}

	// prepare the internal data
	opened.clear();
	for (NodeInternal& n : nodes)
	{
		n.g = 0;
		n.h = 0;
		n.opened = false;
		n.closed = false;
		n.parent = nullptr;
	}

	NodeInternal* startNode{ getNodeByPosition(startX, startY) };
	if (startNode->node.block)
	{
		return{};
	}

	NodeInternal* endNode{ getNodeByPosition(endX, endY) };
	if (endNode->node.block)
	{
		return{};
	}

	NodeInternal* node{ startNode };
	opened.push_back(node);

	while (true)
	{
		// Search a node with a lowest f. Usually opened list is not big and traversing it will be fast
		auto it = min_element(opened.begin(), opened.end(), [](const NodeInternal* a, const NodeInternal* b)
		{
			return a->g + a->h < b->g + b->h;
		});

		// remove a node from opened list and mark it as closed
		iter_swap(it, opened.end() - 1);
		node = opened.back();
		opened.pop_back();

		node->closed = true;

		// if endNode is not null we found a path!
		endNode = updateNeighbors(node, endX, endY);

		if (endNode != nullptr)
		{
			break;
		}
		else
		{
			// if we didn't find endNode and the opened list is empty - the path does not exist
			if (opened.empty())
			{
				break;
			}
		}
	}

	// collecting a path nodes
	vector<Node> result;
	if (endNode != nullptr)
	{
		NodeInternal* n{ endNode };
		while (n != nullptr)
		{
			result.push_back(n->node);

			n = n->parent;
		}

		reverse(result.begin(), result.end());
	}

	return result;
}

AStar::NodeInternal* AStar::updateNeighbors(NodeInternal* parent, int endX, int endY)
{
	bool haveLeft{ parent->node.xPos > 0 };
	bool haveRight{ parent->node.xPos < SIZE_W - 1 };
	bool haveTop{ parent->node.yPos > 0 };
	bool haveBottom{ parent->node.yPos < SIZE_H - 1 };

	if (haveLeft)
	{
		NodeInternal* child{ getNodeByPosition(parent->node.xPos - 1, parent->node.yPos) }; // left
		if (!child->node.block && !child->closed)
		{
			if (updateNeighbor(parent, child, endX, endY)) return child;
		}

		if (haveTop)
		{
			NodeInternal* child{ getNodeByPosition(parent->node.xPos - 1, parent->node.yPos - 1) }; // top left
			if (!child->node.block && !child->closed)
			{
				if (updateNeighbor(parent, child, endX, endY)) return child;
			}
		}

		if (haveBottom)
		{
			NodeInternal* child{ getNodeByPosition(parent->node.xPos - 1, parent->node.yPos + 1) }; // bottom left
			if (!child->node.block && !child->closed)
			{
				if (updateNeighbor(parent, child, endX, endY)) return child;
			}
		}
	}

	if (haveRight)
	{
		NodeInternal* child{ getNodeByPosition(parent->node.xPos + 1, parent->node.yPos) }; // right
		if (!child->node.block && !child->closed)
		{
			if (updateNeighbor(parent, child, endX, endY)) return child;
		}

		if (haveTop)
		{
			NodeInternal* child{ getNodeByPosition(parent->node.xPos + 1, parent->node.yPos - 1) }; // top right
			if (!child->node.block && !child->closed)
			{
				if (updateNeighbor(parent, child, endX, endY)) return child;
			}
		}

		if (haveBottom)
		{
			NodeInternal* child{ getNodeByPosition(parent->node.xPos + 1, parent->node.yPos + 1) }; // bottom right
			if (!child->node.block && !child->closed)
			{
				if (updateNeighbor(parent, child, endX, endY)) return child;
			}
		}
	}

	if (haveTop)
	{
		NodeInternal* child{ getNodeByPosition(parent->node.xPos, parent->node.yPos - 1) }; // top
		if (!child->node.block && !child->closed)
		{
			if (updateNeighbor(parent, child, endX, endY)) return child;
		}
	}

	if (haveBottom)
	{
		NodeInternal* child{ getNodeByPosition(parent->node.xPos, parent->node.yPos + 1) }; // bottom
		if (!child->node.block && !child->closed)
		{
			if (updateNeighbor(parent, child, endX, endY)) return child;
		}
	}

	return nullptr;
}

bool AStar::updateNeighbor(NodeInternal* parent, NodeInternal* child, int endX, int endY)
{
	// move cost from the start
	int g{ parent->g + calculateG(parent, child) };

	if (!child->opened)
	{
		child->parent = parent;

		if (child->node.xPos == endX && child->node.yPos == endY)
		{
			return true;
		}

		child->h = calculateH(child, endX, endY);
		child->g = g;

		child->opened = true;
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
};

int AStar::calculateG(const NodeInternal* parent, const NodeInternal* child) const
{
	// if diagonal
	if (parent->node.xPos != child->node.xPos && parent->node.yPos != child->node.yPos)
	{
		return 14;
	}

	return 10;
}

int AStar::calculateH(const NodeInternal* node, const int endX, const int endY) const
{
	return 10 * (abs(endX - node->node.xPos) + abs(endY - node->node.yPos));
}

AStar::NodeInternal* AStar::getNodeByPosition(const int posX, const int posY)
{
	return &nodes[posY * SIZE_W + posX];
}

int AStar::getNodeIndex(const NodeInternal* node) const
{
	return node->node.yPos * SIZE_W + node->node.xPos;
}