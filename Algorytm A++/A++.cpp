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
		//std::cout << "Dodaje grupe: " << currentNode->grid->id << "\n";
		currentNode = currentNode->parent;
	}
	Grupy.emplace_back(currentNode->grid->id);
	//std::cout << currentNode->grid->id<<std::endl;
	//std::cout << "END" << std::endl;
	Utility::reverse(&Grupy[0], &Grupy[0] + Grupy.size());
	Point CurrentPoint = currentNode->point; //startNode
	for (int i = 0; i < Grupy.size()-1; i++) {
		Edge edge = graph.Grids[Grupy[i]].getEdge(Grupy[i + 1], this->lengthoperations);
		Point targetPoint = edge.Grid1Point;
		auto droga = AstarObj.GetWay(graph.Grids[Grupy[i]], CurrentPoint, targetPoint);
		if (droga.size() == 0) {
			std::cout << "NIE DA SIE WTF?";
		}
		for (const auto& node : droga) {
			Wynik.emplace_back(node);
		}
		CurrentPoint = edge.Grid2Point;
	}
	return Wynik;
}



std::unordered_map<std::pair<int,int>,size_t,PairHasher> AstarPlusPlus::GetAllPossibleBFS(const Graph& graph)
{
	std::unordered_map<std::pair<int, int>, size_t,PairHasher> result;
	//id1->id2 and distance
	for (int i = 0; i < graph.Grids.size(); i++) {
		for (int j = i; j < graph.Grids.size(); j++) {
			//mamy jedna z druga mape i BFS liczmy dystans
			size_t distance = getDistanceBfs(i, j, graph);
			if (distance != size_t()) {
				result[std::make_pair(i, j)] = distance;
				result[std::make_pair(j,i)] = distance;
			}
			else {
				result[std::make_pair(i, j)] = 0;
				result[std::make_pair(j,i)] = 0;
			}

		}
	}
	return result;
}

std::unordered_map<std::tuple<int, Point, Point>, size_t,TupleHasher> AstarPlusPlus::GetAstarCache(const Graph& graph)
{
	std::unordered_map<std::tuple<int, Point, Point>, size_t, TupleHasher> result;
	for (auto& grid : graph.Grids) {
		AstarGrid AstarObj = AstarGrid();
		for (int i = 0; i < grid.Edges.size(); i++) {
			for (int j = i; j < grid.Edges.size(); j++) {
				if(i==j){
					continue;
				}
				Point start = grid.Edges[i].Grid1Point;
				Point target = grid.Edges[j].Grid1Point;
				size_t distance = AstarObj.GetWay(grid, start, target).size();
				if (distance != 0) {
					result[std::make_tuple(grid.id, start, target)] = distance;
					result[std::make_tuple(grid.id, target,start)] = distance;
				}
				else {
					result[std::make_tuple(grid.id, start, target)] = 0;
					result[std::make_tuple(grid.id, target,start)] = 0;
				}
			}
		}
	}
	return result;
}

size_t AstarPlusPlus::getDistanceBfs(int map1, int map2, const Graph& graph)
{
	const Grid& grid1 = graph.Grids[map1];
	std::unordered_set<int> ClosedSet;
	MinHeap<size_t, std::pair<size_t,const Grid*>,GridHasher> OpenSet;
	OpenSet.insert(0, std::make_pair(0, &grid1));
	while (!OpenSet.isEmpty()) {
		std::pair<size_t, const Grid*>current;
		OpenSet.getMin(current);
		OpenSet.deleteMin();
		if (current.second->id == map2) {
			return current.first;
		}
		ClosedSet.insert(current.second->id);
		for (const auto& edge : current.second->Edges) {
			if (ClosedSet.contains(edge.idMapConnect)) {
				continue;
			}
			OpenSet.insert(current.first + 1, std::make_pair(current.first + 1, &graph.Grids[edge.idMapConnect]));
		}
	}
	return size_t();
}

std::vector<Point> AstarPlusPlus::getWayOptimized(const Graph& graph, int GrupaWezlowCel, const Point& StartPoint, int GrupaWezlowStart, const std::unordered_map<std::pair<int, int>, size_t, PairHasher>& bfsDistances, const std::unordered_map<std::tuple<int, Point, Point>, size_t, TupleHasher>& astarCache)
{
	//Wyznaczmy minimalny odleglosc miedzy przejsciami do heurystyki
	int mindistance = std::numeric_limits<int>::max();
	for (auto& astarCacheItem : astarCache) {
		this->lengthoperations += 2;
		if (astarCacheItem.second < mindistance && astarCacheItem.second > 0) {
			this->lengthoperations += 1;
			mindistance = static_cast<int>(astarCacheItem.second); //miedzy krawedziami to jest z Cache ale jeszcze sprawdzmy miedzy Startem a edges :D(duza zlozonosc?
		}
	}

	if (mindistance == std::numeric_limits<int>::max()) {
		this->lengthoperations += 2;
		mindistance = 1; //Brak przejsc to dajmy 1
	}
	//Zmiana podejscia robimy Graph konwersje na NodeAstarPlusPlus zeby latwiej zarz¹dzac w reconstruct Path
	if (graph.Grids.size() <= GrupaWezlowStart || graph.Grids.size() <= GrupaWezlowCel)
	{
		this->lengthoperations += 4;
		throw std::invalid_argument("Grupa Wezlow nie istnieje w grafie");
	}
	this->lengthoperations += 3;
	//Okay Create Vector?
	std::vector<AstarPlusPlusNode> allNodes;
	MinHeap<std::tuple<double, Point, int>, AstarPlusPlusNode*, NodeHasherPTR> OpenSet;
	std::unordered_set<AstarPlusPlusNode*, NodeHasherPTR> ClosedSet;


	allNodes.reserve(graph.Grids.size());
	this->lengthoperations += 8;
	//NieUwzgledniam Kopiowania punktow jako iloœæ kroków spowodu ze w samym algorytmie nie sa liczone jako krok!
	//W algorytmie mamy gotowe punkty do uzycia(GRAF)
	for (const auto& grid : graph.Grids) {
		auto node = AstarPlusPlusNode(&grid);
		if (grid.id == GrupaWezlowStart) {
			node.point = StartPoint;
			node.gCost = 0;
			node.fCost = getHeuristic(GrupaWezlowStart, GrupaWezlowCel, bfsDistances,mindistance);
			allNodes.emplace_back(node);
			OpenSet.insert(std::make_tuple(node.grid->id, node.point, 0), &allNodes.back());
		}
		else {
			allNodes.emplace_back(&grid);
		}
	}
	//Zapiszmy edge koncowego targetu (Punkty?) jezeli odwiedzilismy wszystkich sasiadow targetu to znaczy ze nie ma drogi
	this->lengthoperations += 1;
	while (!OpenSet.isEmpty()) {
		//Fragment czy istnieje droga zeby wczesniej zakonczyc
		AstarPlusPlusNode* current;
		OpenSet.getMin(current);
		ClosedSet.insert(current);
		OpenSet.deleteMin();
		this->lengthoperations +=1;
		if (current->grid->id == GrupaWezlowCel) {
			this->lengthoperations += 2;
			return ReconstructPath(graph, current);
		}
		
		this->lengthoperations += 1;
		//Koniec
		std::vector<std::tuple<AstarPlusPlusNode*, size_t, Point>> neighbours = getNeighborsOptimized(allNodes, current, graph,astarCache);
		this->lengthoperations += 1;
		for (auto& [neigh, dystans, newPoint] : neighbours) {
			//para wskazniku i dystansu!
			if (!OpenSet.contains(neigh) && !ClosedSet.contains(neigh))
			{
				neigh->gCost = current->gCost + dystans;
				neigh->fCost = static_cast<double>(neigh->gCost) + getHeuristic(neigh->grid->id, GrupaWezlowCel, bfsDistances,mindistance);
				neigh->parent = current;
				neigh->point = newPoint;
				OpenSet.insert(std::make_tuple(neigh->grid->id, neigh->point, neigh->fCost), neigh);
				this->lengthoperations += 9;
				continue;
			}
			if (OpenSet.contains(neigh)) {
				//Napisac update func!?
				int tempgscore = dystans + current->gCost;
				if (neigh->gCost > tempgscore) {
					neigh->gCost = tempgscore;
					neigh->fCost = static_cast<double>(neigh->gCost) + getHeuristic(neigh->grid->id, GrupaWezlowCel, bfsDistances, mindistance);
					neigh->point = newPoint;
					neigh->parent = current;
					OpenSet.update(std::make_tuple(neigh->grid->id, neigh->point, neigh->fCost), neigh);
					this->lengthoperations += 8;
				}

			}


		}


	};
	return std::vector<Point>();
};


double AstarPlusPlus::getHeuristic(int map1, int map2, const std::unordered_map<std::pair<int, int>, size_t, PairHasher>& bfsDistances, const int constnumber)
{
	//Zakladam Ze zawsze mamy dystans w map!'
	auto find = bfsDistances.find(std::make_pair(map1, map2));
	//find O(1) mapa
	if (find != bfsDistances.end()) {
		size_t distance = find->second;
		this->lengthoperations += 5;
		return distance * constnumber;
	}
	else {
		throw std::invalid_argument("Brak dystansu w bfsDistances");
	}
	return 0.0;
}

std::vector<std::tuple<AstarPlusPlusNode*, size_t, Point>> AstarPlusPlus::getNeighborsOptimized(std::vector<AstarPlusPlusNode>& AllNodes, const AstarPlusPlusNode* currentNode, const Graph& graph, const std::unordered_map<std::tuple<int, Point, Point>, size_t, TupleHasher>& astarCache)
{
	std::vector<std::tuple<AstarPlusPlusNode*, size_t, Point>> result;
	std::vector<Edge> edges = currentNode->grid->Edges;
	AstarGrid AstarObject = AstarGrid();
	Point Start = currentNode->point;
	this->lengthoperations += 4;
	for (const auto& edge : edges) {
		Point Target = edge.Grid1Point;
		auto tuplekey = std::make_tuple((int)currentNode->grid->id, Start, Target);
		auto it = astarCache.find(tuplekey);
		size_t realdistance;
		if (it != astarCache.end()) {
			realdistance = it->second;
		}
		else {
			realdistance = AstarObject.GetWay(*currentNode->grid, Start, Target).size();
		}
		//Zakladam ze find w mapach jest O(1)
		this->lengthoperations += 6 + AstarObject.lengthoperations; //size () tez kosztuje?
		if (realdistance == 0) //W tym kodzie gscore jezeli ==0 to znaczy ze nie ma drogi nawet jednokratkowej(wlacznie z Startem), wiec nie trzeba sprawdzac czy wezel jest startowym
		{
			this->lengthoperations += 1;
			continue;
		}
		//mamy dystans mamy ID mapy Trzeba z ALlNodes wyciagnac te ID i dodac do wektora wskaznik wskazujacy z allNodes
		//Aczkolwiek jezeli id mamy ponumerowane od 0 w allnodes i sa po kolei to znamy indeks tego prawda?, zalozmy ze tak jest ze index==map.id
		result.emplace_back(&AllNodes[edge.idMapConnect], realdistance, edge.Grid2Point);
		this->lengthoperations += 2;
	}
	return result;
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
	//NieUwzgledniam Kopiowania punktow jako iloœæ kroków spowodu ze w samym algorytmie nie sa liczone jako krok!
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
