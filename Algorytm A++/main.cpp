#include <iostream>
#include "Graph.hpp"
#include "Astar.hpp"
#include "testsUnit.hpp"
#include "A++.hpp"
#include "Utility.hpp"
int main()
{
	srand(time(NULL));
#ifdef _DEBUG
	bool tests = testsUnit::testAstarSingleGrid();
	if (tests) {
		std::cout << "ALL TESTS PASSED!\n";
	}
	else {
		std::cout << "TESTS FAILED!\n";
		return 1;
	}
	//testsUnit::testBenchmark();

#endif // DEBUG
	//std::vector<int> test = { 1,2,3,4,5 };
	//reverse(test.begin(), test.end());
	//testsUnit::testBenchmark(); //przykladowy po wygenerenowaniu grafu z innego programu

	testsUnit::testBenchmarkMultiple();

	return 0;
}
