#pragma once
#include "Graph.hpp"
#include "Astar.hpp"
#include <iostream>
#include "A++.hpp"
#include "GeneratorSubplazczyznDebug.hpp"
#include <future>
#include <ctpl_stl.h>
#include <random>
struct benchmark {
	int edges;
	size_t lengthoperations;
	size_t lengthoptimizedwithcache;
	size_t lengthoptimizedwithoutcache;
	int nodes;
	benchmark(int edges, size_t lengthoperations, size_t lengthoptimizedwithcache, size_t lengthoptimizedwithoutcache, int nodes)
		: edges(edges), lengthoperations(lengthoperations), lengthoptimizedwithcache(lengthoptimizedwithcache), lengthoptimizedwithoutcache(lengthoptimizedwithoutcache), nodes(nodes) {}
};
template<>
struct glz::meta<benchmark> {
	using T = benchmark;
	static constexpr auto value = glz::object(
		"edges", &T::edges,
		"lengthoperations", &T::lengthoperations,
		"lengthoptimizedwithcache", &T::lengthoptimizedwithcache,
		"lengthoptimizedwithoutcache", &T::lengthoptimizedwithoutcache,
		"nodes", &T::nodes
	);
};
class testsUnit {
public:
	static bool testAstarSingleGrid();
	static void testBenchmark();
	static void testBenchmarkMultiple();
};