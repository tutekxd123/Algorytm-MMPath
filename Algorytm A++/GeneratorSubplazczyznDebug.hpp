#pragma once
#include "Graph.hpp"
#include <unordered_set>
struct PairHash {
	std::size_t operator()(const std::pair<int, int>& p) const noexcept {
		return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1); //Hash to pary <int,int> prosty(zeby set mogl wyszukiwac>)
	}
};
int MakeBadData(Graph& graph);
Graph MakeFixedGraph(int sizemap,int connectsmap,int minx,int maxx,int miny,int maxy, int chanceofcoll,std::mt19937& rng);

Graph MakeGraph(int sizemap, int minconnonmaps, int maxconnonmaps, int minx, int maxx, int miny, int maxy, int chanceofcoll,std::mt19937& rng);