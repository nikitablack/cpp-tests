#pragma once

#include <vector>
#include <string>

namespace astar
{
	struct Node
	{
		int xPos;
		int yPos;
		bool block;
	};

	/**
	* Helper function to read a map file
	*
	* @param mapFilePath
	* @param sizeW Number of nodes horizontally
	* @param sizeH Number of nodes vertically
	* @return List of nodes
	*/
	std::vector<Node> readMapFile(std::string mapFilePath, int sizeW, int sizeH);

	class AStar
	{
	private:
		/**
		* Internal node structure with additional data
		*/
		struct NodeInternal
		{
			Node node;
			NodeInternal* parent;
			int h; // heuristic move cost to the end node
			int g; // move cost from the start node
			bool opened; // is waiting in the list for processing?
			bool closed; // is already visited?
		};

	public:
		/**
		* Constructor creates a copy of passed nodes list.
		*/
		AStar(std::vector<Node>& InNodes, int sizeW, int sizeH);

		/**
		* Searches for the path.
		*
		* @return Return the list of path nodes. Returns empty list if there's no path.
		*/
		std::vector<Node> find(int startX, int startY, int endX, int endY);

	private:
		const int SIZE_W;
		const int SIZE_H;
		std::vector<NodeInternal> nodes;
		std::vector<NodeInternal*> opened;

	private:
		/**
		* Checks and updates if necessary every neighbor of a given node
		*
		* @return An end node if found, nullptr otherwise
		*/
		NodeInternal* updateNeighbors(NodeInternal* parent, int endX, int endY);

		/**
		* Updates a given node
		*
		* @return True if the end node was found
		*/
		bool updateNeighbor(NodeInternal* parent, NodeInternal* child, int endX, int endY);

		/**
		* Calculates the movement cost from the start
		*/
		int calculateG(const NodeInternal* parent, const NodeInternal* child) const;

		/**
		* Calculates the heuristic movement cost to the end
		*/
		int calculateH(const NodeInternal* node, const int endX, const int endY) const;

		/**
		* Given the position returns a node
		*/
		NodeInternal* getNodeByPosition(const int posX, const int posY);

		/**
		* Given the node returns an index in the nodes list
		*/
		int getNodeIndex(const NodeInternal* node) const;
	};
}