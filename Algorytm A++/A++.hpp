#pragma once
#include <iostream>
#include "Graph.hpp"
#include <unordered_set>
#include "Astar.hpp"
#include "MiniHeap.hpp"
struct AstarPlusPlusNode {
	const Grid* grid = nullptr;
	Point point;
	double gCost = std::numeric_limits<int>::max();
	double fCost = std::numeric_limits<int>::max();
	AstarPlusPlusNode* parent = nullptr;

	AstarPlusPlusNode(const Grid* grid): grid(grid) {}
	AstarPlusPlusNode(const Grid* grid, const Point& point, int gCost, AstarPlusPlusNode* parent)
		: grid(grid), point(point), gCost(gCost), parent(parent) {
	}
	AstarPlusPlusNode(const Grid*, const Point& point): point(point), gCost(std::numeric_limits<int>::max()), parent(nullptr) {
	}
	//	OpenSet.emplace_back(&startGrid, StartPoint, 0, nullptr);
	bool operator==(const AstarPlusPlusNode& point) const {
		return this->grid == point.grid && this->point == point.point;
	}
	bool operator>(const AstarPlusPlusNode* otherpoint) const {
		return this->gCost > otherpoint->gCost;
	}
	bool operator<(const AstarPlusPlusNode* otherpoint) const {
		return this->gCost < otherpoint->gCost;
	}

};

static bool operator>(const std::tuple<double, Point, int>& a1, const std::tuple<int, Point, int>& a2)
{
	return std::get<2>(a1) > std::get<2>(a2);
}

static bool operator<(const std::tuple<double, Point, int>& a1, const std::tuple<int, Point, int>& a2)
{
	return std::get<2>(a1) < std::get<2>(a2);
}
struct NodePTRComparator {
	bool operator()(AstarPlusPlusNode* a, AstarPlusPlusNode* b) const {
			if (a->gCost == b->gCost) {
				if (a->grid->id == b->grid->id) {
					return a->grid->id < b->grid->id;
				}
				return a->grid->id < b->grid->id;
			}
			return a->gCost < b->gCost;
		}
};
struct NodeHasherPTR {
	size_t operator()(const AstarPlusPlusNode* p) const noexcept {
		return std::hash<const AstarPlusPlusNode*>()(p);
	}
};

struct GridHasher {
	size_t operator()(const std::pair<size_t, const Grid*>& pair) const
	{
		return (std::hash<size_t>()(pair.second->id));
	}
};
struct PairHasher{
	size_t operator()(const std::pair<int, int>& pair) const
	{
		// Combine hashes of x and y using the bitwise XOR
		return (std::hash<int>()(pair.first) ^ (std::hash<int>()(pair.second) << 1));
	}
};
struct PairHasherPoint {
	size_t operator()(const std::pair<int, Point>& pair) const
	{
		return (std::hash<int>()(pair.first) ^
			(std::hash<int>()(pair.second.x) << 1) ^
			(std::hash<int>()(pair.second.y) << 2)
			);
	}
};
struct TupleHasher {
	size_t operator()(const std::tuple<int, Point, Point>& tuple) const
	{
		return (std::hash<int>()(std::get<0>(tuple)) ^
			(std::hash<int>()(std::get<1>(tuple).x) << 1) ^
			(std::hash<int>()(std::get<1>(tuple).y) << 2) ^
			(std::hash<int>()(std::get<2>(tuple).x) << 3) ^
			(std::hash<int>()(std::get<2>(tuple).y) << 4)
			);
	}
};
class AstarPlusPlus {
public:
	int lengthoperations = 0;
	std::vector<Point> getWay(const Graph& graph, int GrupaWezlowCel, const Point& StartPoint, int GrupaWezlowStart);
	std::vector<std::tuple<AstarPlusPlusNode*, size_t, Point>> getNeighbors( std::vector<AstarPlusPlusNode>&AllNodes,const AstarPlusPlusNode* currentNode, const Graph& graph);
	std::vector<Point> ReconstructPath(const Graph& graph, AstarPlusPlusNode* currentNode);
};