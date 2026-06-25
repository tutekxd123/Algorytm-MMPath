#include "A++.hpp"



//std::vector<AstarPlusPlusNode> getNeighbors(const std::vector<AstarPlusPlusNode>&AllNodes,const AstarPlusPlusNode* currentNode, const Graph& graph);
std::vector<std::tuple<AstarPlusPlusNode*, size_t, Point>> AstarPlusPlus::getNeighbors(std::deque<AstarPlusPlusNode>& AllNodes, const AstarPlusPlusNode* currentNode, const Graph& graph)
{

	std::vector<std::tuple<AstarPlusPlusNode*,size_t,Point>> result;
	std::vector<Edge> edges = currentNode->grid->Edges;
	AstarGrid AstarObject = AstarGrid();
	Point Start = currentNode->point;
	this->lengthoperations += 4;
	for (const auto& edge : edges) {
		Point Target = edge.Grid1Point;
		size_t realdistance = AstarObject.GetWay(*currentNode->grid, Start, Target).size(); ////z dystansem cos trzeba zrobic?
		this->lengthoperations += 1 + AstarObject.lengthoperations; //size () tez kosztuje?
		if(realdistance == 0) //W tym kodzie gscore jezeli ==0 to znaczy ze nie ma drogi nawet jednokratkowej(wlacznie z Startem), wiec nie trzeba sprawdzac czy wezel jest startowym
		{
			this->lengthoperations += 3;
			continue;
		}
		//mamy dystans mamy ID mapy Trzeba z ALlNodes wyciagnac te ID i dodac do wektora wskaznik wskazujacy z allNodes
		//Aczkolwiek jezeli id mamy ponumerowane od 0 w allnodes i sa po kolei to znamy indeks tego prawda?, zalozmy ze tak jest ze index==map.id
		result.emplace_back(&AllNodes[edge.idMapConnect],realdistance,edge.Grid2Point);
		this->lengthoperations += 4;
	}
	return result;
}


std::vector<Point> AstarPlusPlus::ReconstructPath(const Graph& graph, AstarPlusPlusNode* currentNode) {
	std::vector<Point>Wynik;
	std::vector<int>Grupy;
	AstarGrid AstarObj = AstarGrid();
	this->lengthoperations += 3;
	while (currentNode->parent != nullptr) {
		Grupy.emplace_back(currentNode->grid->id);
		currentNode = currentNode->parent;
		this->lengthoperations += 2;
	}
	Grupy.emplace_back(currentNode->grid->id);
	this->lengthoperations += Utility::reverse(&Grupy[0], &Grupy[0] + Grupy.size()) + 2;
	Point CurrentPoint = currentNode->point; //startNode
	for (int i = 0; i < Grupy.size() - 1; i++) {
		Edge edge = graph.Grids[Grupy[i]].getEdge(Grupy[i + 1], this->lengthoperations);
		Point targetPoint = edge.Grid1Point;
		auto droga = AstarObj.GetWay(graph.Grids[Grupy[i]], CurrentPoint, targetPoint);
		this->lengthoperations += 6;
		for (const auto& node : droga) {
			this->lengthoperations += 1;
			mindistance = static_cast<int>(astarCacheItem.second); //miedzy krawedziami to jest z Cache ale jeszcze sprawdzmy miedzy Startem a edges :D(duza zlozonosc?
		}
		CurrentPoint = edge.Grid2Point;
	}
	return Wynik;
}


std::vector<Point> AstarPlusPlus::getWay(const Graph& graph, int GrupaWezlowCel, const Point& StartPoint, int GrupaWezlowStart)
{
	//Zmiana podejscia robimy Graph konwersje na NodeAstarPlusPlus zeby latwiej zarz¹dzac w reconstruct Path
	if (graph.Grids.size() <= GrupaWezlowStart || graph.Grids.size() <= GrupaWezlowCel)
	{
		this->lengthoperations += 4;
		throw std::invalid_argument("Grupa Wezlow nie istnieje w grafie");
	}
	this->lengthoperations += 2;
	//Okay Create Vector?
	std::deque<AstarPlusPlusNode> allNodes;
	MinHeap<std::tuple<int,Point,int>, AstarPlusPlusNode*, NodeHasherPTR> OpenSet;

	//Zmiana na MiniHeap
	std::unordered_set<AstarPlusPlusNode*, NodeHasherPTR> ClosedSet;
	//allNodes.reserve(10000);
	this->lengthoperations += 4;
	//NieUwzgledniam Kopiowania punktow jako iloæ kroków spowodu ze w samym algorytmie nie sa liczone jako krok!
	//W algorytmie mamy gotowe punkty do uzycia(GRAF)
	for (const auto& grid : graph.Grids) {
		auto node = AstarPlusPlusNode(&grid);
		if (grid.id == GrupaWezlowStart) {
			node.point = StartPoint;
			node.gCost = 0;
			allNodes.emplace_back(node);
			OpenSet.insert(std::make_tuple(node.grid->id, node.point, 0), &allNodes.back());
			//this->lengthoperations += 7;
		}
		else {
			//this->lengthoperations += 2;
			allNodes.emplace_back(&grid);
		}
	}
	while (!OpenSet.isEmpty()) {
		AstarPlusPlusNode* current;
		OpenSet.getMin(current);
		ClosedSet.insert(current);
		OpenSet.deleteMin();
		this->lengthoperations += 2;
		if (current->grid->id == GrupaWezlowCel) {
			this->lengthoperations += 2;
			return ReconstructPath(graph, current);
		}
		std::vector<std::tuple<AstarPlusPlusNode*, size_t, Point>> neighbours = getNeighbors(allNodes, current, graph);
		this->lengthoperations += 1;
		for (auto& [neigh,dystans,newPoint] : neighbours) {
			//para wskazniku i dystansu!
			if (!OpenSet.contains(neigh) && !ClosedSet.contains(neigh))
			{
					neigh->gCost = current->gCost + dystans;
					neigh->parent = current;
					neigh->point = newPoint;
					OpenSet.insert(std::make_tuple(neigh->grid->id, neigh->point, neigh->gCost), neigh);
					this->lengthoperations += 8;
					continue;
			}
			if (OpenSet.contains(neigh)) {
					//Napisac update func!?
					int tempgscore = dystans + current->gCost;
					if (neigh->gCost > tempgscore) {
						neigh->gCost = tempgscore;
						neigh->point = newPoint;
						neigh->parent = current;
						OpenSet.update(std::make_tuple(neigh->grid->id, neigh->point, neigh->gCost), neigh);
						this->lengthoperations += 8;
					}

			}


		}

	};
	return std::vector<Point>();
};
