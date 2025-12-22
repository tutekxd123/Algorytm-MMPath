#include "testsUnit.hpp"

bool testsUnit::testAstarSingleGrid()
{
	Graph graphtest{};
	Graph morecomplex{};

	AstarGrid astar{};
	//Pierwsze co to trzeba wczytanie jsona zrobic "Glaze" i napisaæ struktury danych w sumie moge skopiwaæ z poprzedniego projektu
	auto ec = glz::read_file_json(graphtest, "./tests.json", std::string{});
	if (ec) {
		std::cout << "ERROR JSON LOAD: " << glz::format_error(ec) << '\n';
		std::cout << "FAILED TEST A* SINGLE GRID DUE TO JSON LOAD ERROR\n";
		return false;
	}
	//Pierwszy test na specialnej siatce z tests.json
	//Test A* na jednej siatce bez kolizji powinien zwrocic droge o dlugosci 19 (0+9)+(0+9)=18 krokow + 1 Startowy
	Point Start = graphtest.Grids[0].getPoint(0, 0);
	Point Target = graphtest.Grids[0].getPoint(9, 9);
	//Pierwsze co piszemy A* na jednej siatce
	//Z Racji ze pracujemy nie na wskaznikach a na kopiach trzeba uwzglednic to przy tworzeniu A*
	auto test1 = astar.GetWay(graphtest.Grids[0], Start, Target);
	if(test1.size() != 19)
	{
		std::cout << "TEST A* SINGLE GRID FAILED! Expected path length 19, got " << test1.size() << "\n";
		return false;
	}
	else {
		std::cout << "Test A* Single Grid Passed! nr.1 zero obstacles \n";
	}
	Start = graphtest.Grids[1].getPoint(0, 0);
	Target = graphtest.Grids[1].getPoint(10,10);
	auto test2 = astar.GetWay(graphtest.Grids[1], Start, Target);
	//Expected path length is 23 because of obstacles
	if (test2.size() != 25)
	{
		std::cout << "TEST A* SINGLE GRID FAILED! Expected path length 25, got " << test2.size() << "\n";
		return false;
	}
	else {
		std::cout << "Test A* Single Grid Passed! nr.2 with obstacles \n";
	}
	Start = graphtest.Grids[1].getPoint(0, 0);
	Target = graphtest.Grids[1].getPoint(0,15);
	auto test3 = astar.GetWay(graphtest.Grids[1], Start, Target);
	//Expected no path
	if (test3.size() != 0) {
		std::cout << "TEST A* SINGLE GRID FAILED! Expected no path, got path length " << test3.size() << "\n";
		return false;
	}
	else {
		std::cout << "Test A* Single Grid Passed! nr.3 no possible path \n";
	}
	
	auto ec2 = glz::read_file_json(morecomplex,"./testsmorecomplex.json", std::string{});
	return true;
}

void testsUnit::testBenchmark()
{
	Graph benchmark{};
	auto ec3 = glz::read_file_json(benchmark, "./benchmarktest.json", std::string{});

	auto obj = AstarPlusPlus();
	auto test = obj.getWay(benchmark, 4079, Point(0, 0), 1);
	std::cout << test.size() << "\n";
	return;
}

std::mutex mtx;

void testsUnit::testBenchmarkMultiple() {

	std::vector<benchmark> benchmarks;
	ctpl::thread_pool pool(20);

	for (int maps = 10; maps < 100; maps += 10) { //kiedys generowalem rozne rozmiary map w sensie grafu, lecz pod wykres pudelkowy lepiej miec stala mape
		pool.push([maps, &benchmarks](int id) {
			//Zmiana stale mapy pod wykres 
			//kwadratowe mapy
			std::mt19937 rng(123456 + id);
			// MakeGraph(int sizemap, int minconnonmaps, int maxconnonmaps, int minx, int maxx, int miny, int maxy, int chanceofcoll,std::mt19937& rng);
			//Graph benchmark = MakeGraph(1000, 1,(int)(std::log(maps) * 1.5),32, 32, 32, 32,0,rng);
			//Graph benchmark = MakeGraph(1000, 5,6, 32, 32, 32, 32, 0,rng); //tak jak tutaj //Bez kolizji!!
			Graph benchmark = MakeFixedGraph(maps, 4, 32, 32, 32, 32, 0, rng); //Tak jak tutaj //Bez kolizji!
			size_t sizeofEdges = 0;
			for (auto& grid : benchmark.Grids) {
				sizeofEdges += grid.Edges.size();
			}
			//int badmap = MakeBadData(benchmark);
			int map = GenerateNumber(1, benchmark.Grids.size() - 1);
			//int selectmap = map == 0 ? 1 : 0;
			auto bfs = AstarPlusPlus::GetAllPossibleBFS(benchmark);
			const auto cache = AstarPlusPlus::GetAstarCache(benchmark);
			std::unordered_map<std::tuple<int, Point, Point>, size_t, TupleHasher> emptycache;
			auto obj2 = AstarPlusPlus();
			auto obj3 = AstarPlusPlus();
			auto obj = AstarPlusPlus();
			auto verify = obj.getWay(benchmark, map, benchmark.Grids[map].getPoint(5, 5), map == 0 ? 1 : 0);
			std::cout << verify.size() << "\n";
			auto verify2 = obj2.getWayOptimized(benchmark, map, Point(5, 5), map == 0 ? 1 : 0, bfs, emptycache);
			auto verify3 = obj3.getWayOptimized(benchmark, map, Point(5, 5), map == 0 ? 1 : 0, bfs, cache);
			if (verify.size() != verify2.size()) {
				std::cout << "ERROR IN BENCHMARK SIZE MISMATCH NORMAL VS OPTIMIZED!\n";
				obj.getWay(benchmark, map, benchmark.Grids[map].getPoint(5, 5), map == 0 ? 1 : 0);
				obj2.getWayOptimized(benchmark, map, Point(5, 5), map == 0 ? 1 : 0, bfs, emptycache);
			}
			std::lock_guard<std::mutex> lock(mtx);
			benchmarks.emplace_back(sizeofEdges, obj.lengthoperations, obj3.lengthoperations, obj2.lengthoperations,100);
			});

		//Saving benchmarks to file using Glaze
	}
	pool.stop(true);
	auto ec = glz::write_file_json(benchmarks, "./benchmarksresults.json", std::string{});
	return;
}