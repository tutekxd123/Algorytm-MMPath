
#include "GeneratorSubplazczyznDebug.hpp"

int MakeBadData(Graph& graph) {
	//Robimy wredne dane wiec generujemy losowa mape
	//Potem przechodzimy do wszystkich krawedzi prowadzaca do tej mapy i wokol wezlow granicznych robimy kolizje co spowoduje najgorsze dane/przypadek
	int randomMapIndex = GenerateNumber(0, graph.Grids.size() - 1);
	int length = 0; // w sumie to ignoruje
	Grid& badGrid = graph.Grids[randomMapIndex];
	for (Edge& edge : badGrid.Edges) {
		Grid& otherGrid = graph.Grids[edge.idMapConnect]; //Okay i szukamy  edge do tego
		auto edgefind = otherGrid.getEdge(randomMapIndex,length);
		Point& pointinGrid = edgefind.Grid1Point;
		/*
		int diff[5][2] = { {0,1},{1,0},{-1,0},{0,-1},{0,0} };
		for (int i = 0; i < 5; i++) {
			int dx = pointinGrid.x + diff[i][0];
			int dy = pointinGrid.y + diff[i][1];
			if (dx >= 0 && dx < otherGrid.width && dy < otherGrid.height && dy >= 0) {
				otherGrid.getPoint(dx, dy).collision = true;
			}
		}
		*/
		otherGrid.getPoint(pointinGrid.x,pointinGrid.y).collision = true;
		//Robimy kolizje wokol tego punktu

	}
	return randomMapIndex;
}

Graph MakeFixedGraph(int sizemap, int connectsmap, int minx, int maxx, int miny, int maxy, int chanceofcoll, std::mt19937& rng)
{
	Graph newgraph;
	newgraph.Grids.reserve(sizemap);
	for (int i = 0; i < sizemap; i++)
	{
		Grid tempgrid = Grid(i, maxx - minx, maxy - miny);
		tempgrid.GenerateNewGrid(minx, maxx, miny, maxy, chanceofcoll);
		newgraph.Grids.emplace_back(tempgrid);
	}
	std::unordered_set<std::pair<int, int>, PairHash> AlreadyEdges;
	std::unordered_map<int, std::unordered_set<std::string>> PointsonGridTaken; //Punkty na mapie ktore maja krawedz
	//Metoda na sztywno lacze mapy z nastepnymi
	for (int i = 0; i < sizemap; i++)
	{
		for(int j=i;j<i+connectsmap&&j<sizemap;j++)
		{
			if(j==i) continue;
			//Sprawdzmy czy mamy takie polaczenie O(1) bo to jest SET
			std::pair<int, int> EdgeCheck1 = std::make_pair(i, j);
			std::pair<int, int> EdgeCheck2 = std::make_pair(j, i);
			//Duplikat bo trzeba byloby napisaæ wlasny komparator?
			if (AlreadyEdges.contains(EdgeCheck1)) {
				continue;
			} //jezeli mamy taka krawedz to wywalamy
			AlreadyEdges.insert(EdgeCheck1);
			AlreadyEdges.insert(EdgeCheck2);
			//Krawdzecie do setu dodane, tera Trzeba koordynaty wygenerowac z jednej i drugiej mapy ktore nie sa kolizja i nie s¹ tak¿e granicznym wezlem
			Point* PointinGrid1 = newgraph.Grids[i].getRandomPoint(false, PointsonGridTaken[i],rng);
			Point* PointinGrid2 = newgraph.Grids[j].getRandomPoint(false, PointsonGridTaken[j],rng);
			if (PointinGrid1 == nullptr || PointinGrid2 == nullptr) { //jezeli jakis nullptr to powinnismy to jakos obsluzyc narazie pomijam tutaj powinno byc throw
				continue;
			}
			//Dobra finalnie dodajemy krwadzie
			newgraph.Grids[i].Edges.emplace_back(Edge(newgraph.Grids[j].id, PointinGrid1, PointinGrid2));
			newgraph.Grids[j].Edges.emplace_back(Edge(newgraph.Grids[i].id, PointinGrid2, PointinGrid1)); //odwotnosc punktow dajemy ze w 1 miejscu punkt w naszej plazczyznie
		}
	}
	
	return newgraph;
}

Graph MakeGraph(int sizemap,int minconnonmaps,int maxconnonmaps, int minx, int maxx, int miny, int maxy, int chanceofcoll,std::mt19937& rng) {
	Graph newgraph;
	newgraph.Grids.reserve(sizemap);
	for (int i = 0; i < sizemap; i++) {
		Grid tempgrid = Grid(i, maxx - minx, maxy - miny);
		tempgrid.GenerateNewGrid(minx, maxx, miny, maxy, chanceofcoll);
		newgraph.Grids.emplace_back(tempgrid);
	}
	std::unordered_set<std::pair<int, int>, PairHash> AlreadyEdges;
	std::unordered_map<int, std::unordered_set<std::string>> PointsonGridTaken; //Punkty na mapie ktore maja krawedz

	for (int i = 0; i < sizemap; i++) {
		//Indexy powinny byc rowne z id w vectorze
		int max = newgraph.Grids[i].points.size() / 2 > newgraph.Grids.size() ? newgraph.Grids.size() / 2 : newgraph.Grids[i].points.size() / 2;

		int CountConnect = GenerateNumberWithRNG(minconnonmaps, maxconnonmaps-1,rng); //Ilosc polaczen miedzy mapa X a Mapami //uwzgledniam kolzije teoretycznie powinnismy wyliczyc mozliwe punkty, ale przyblizenie moze byc
		CountConnect -= newgraph.Grids[i].Edges.size(); //Odejujemy juz krawedzie ktore mamy bo generujemy krawedzie symetrycznie!

		for (int j = 0; j < CountConnect; j++) {
			int ConnectTO = GenerateNumberWithRNG(0, newgraph.Grids.size() - 1,rng); //ID mapy do ktorej sie laczymy
			//Musimy wygenerowaæ Punkt graniczny na mapie nr.1 i mapie nr.2, który nie jest kolizja


			if (ConnectTO == i || newgraph.Grids[ConnectTO].Edges.size() >= maxconnonmaps-1) {
				j--;
				continue;
			} //Ta sama mapa to nie bedziemy sie laczyc :D sam ze soba
			

			//Sprawdzmy czy mamy takie polaczenie O(1) bo to jest SET
			std::pair<int, int> EdgeCheck1 = std::make_pair(i, ConnectTO);
			std::pair<int, int> EdgeCheck2 = std::make_pair(ConnectTO, i);
			//Duplikat bo trzeba byloby napisaæ wlasny komparator?

			if (AlreadyEdges.contains(EdgeCheck1)) {
				j--;
				continue;
			} //jezeli mamy taka krawedz to wywalamy

			AlreadyEdges.insert(EdgeCheck1);
			AlreadyEdges.insert(EdgeCheck2);
			//Krawdzecie do setu dodane, tera Trzeba koordynaty wygenerowac z jednej i drugiej mapy ktore nie sa kolizja i nie s¹ tak¿e granicznym wezlem
			// Point* PointinGrid1 = newgraph.Grids[i].getRandomPoint(false, PointsonGridTaken[newgraph.Grids[i].id]);
			//Point* PointinGrid2 = newgraph.Grids[ConnectTO].getRandomPoint(false, PointsonGridTaken[newgraph.Grids[ConnectTO].id]);
			Point* PointinGrid1 = newgraph.Grids[i].getRandomPoint(false,PointsonGridTaken[i],rng);
			Point* PointinGrid2 = newgraph.Grids[ConnectTO].getRandomPoint(false,PointsonGridTaken[ConnectTO],rng);
			if (PointinGrid1 == nullptr || PointinGrid2 == nullptr) { //jezeli jakis nullptr to powinnismy to jakos obsluzyc narazie pomijam tutaj powinno byc throw
				j--;
				AlreadyEdges.erase(EdgeCheck1);
				AlreadyEdges.erase(EdgeCheck2);
				continue;
			}
			//Dobra finalnie dodajemy krwadzie
			newgraph.Grids[i].Edges.emplace_back(Edge(newgraph.Grids[ConnectTO].id, PointinGrid1, PointinGrid2));
			newgraph.Grids[ConnectTO].Edges.emplace_back(Edge(newgraph.Grids[i].id, PointinGrid2, PointinGrid1)); //odwotnosc punktow dajemy ze w 1 miejscu punkt w naszej plazczyznie
		}
	}
	return newgraph;
}
