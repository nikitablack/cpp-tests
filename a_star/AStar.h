#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <queue>

namespace astar
{
	struct Node
	{
		int xPos;
		int yPos;
		bool block;
		std::shared_ptr<Node> parent;
		int h;
		int g;
	};

	using NodePtrVector = std::vector<std::shared_ptr<Node>>;
	using NodePtr = std::shared_ptr<Node>;

	NodePtrVector readMapFile(std::string mapFilePath);

	class AStar
	{
	public:
		AStar(NodePtrVector nodes, int sizeW, int sizeH);
		NodePtrVector find(int startX, int startY, int endX, int endY);

	private:
		const int SIZE_W;
		const int SIZE_H;
		NodePtrVector allNodes;
		NodePtrVector opened;
		NodePtrVector closed;

	private:
		/**
		* Helper function for retrieving an iterator
		*/
		const auto getNodeIterator(const NodePtr node, const NodePtrVector& list) const;

		/**
		* Checks if a node is in list
		*/
		bool isInList(const NodePtr node, const NodePtrVector& list) const;

		/**
		* Collects node's neighbors
		*/
		NodePtr updateNeighbors(NodePtr parent, int endX, int endY);

		/**
		* Checks if a aneighbor is in closed or pened list, updates node's properties
		*/
		bool updateNeighbor(NodePtr parent, NodePtr child, int endX, int endY);
		int getG(const NodePtr parent, const NodePtr child) const;
		int getH(const NodePtr node, const int endX, const int endY) const;
		NodePtr getNodeByPosition(const int posX, const int posY);
	};
}