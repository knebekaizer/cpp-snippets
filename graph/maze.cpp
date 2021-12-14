//
// Created by Vladimir Ivanov on 04.06.2021.
//

#include "maze.hpp"
#include <random>
#include "trace.h"

using std::make_unique;

Maze::Maze(Size size, float density) : Maze(size, density, (int)std::random_device()()) {}

Maze::Maze(Size size, float density, int seed) : width(size.w), height(size.h)
		, buf_(make_unique<T[]>(size.w * size.h)) {
	assert(density >= 0 && density <= 1);
	TraceX(seed);
	std::mt19937 gen(seed); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> dist(0, 99);

	int threshold = static_cast<int>(100 * density);
	for (auto& x : buf()) {
		x = static_cast<T>(dist(gen) > threshold);
	}
}
