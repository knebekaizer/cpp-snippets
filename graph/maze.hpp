#ifndef GRAPH_MAZE_HPP
#define GRAPH_MAZE_HPP

#include <cassert>
#include <cstdint>
#include <cstddef>
#include <memory>
#include <span>

class Maze {
public:
	using T = uint8_t;
	struct Size { size_t w, h;};
	Maze() = default;
	Maze(Size size, float density); //!< Fill random
	Maze(Size size, float density, int seed); //!< Fill random
	const size_t width = 0;
	const size_t height = 0;

	auto buf() { return std::span(buf_.get(), width * height); }
	auto buf() const { return std::span(buf_.get(), width * height); }

	auto row(size_t y) { return std::span(buf_.get(), y * width); }
	auto row(size_t y) const { return std::span(buf_.get(), y * width); }

	T operator()(size_t x, size_t y) const {
		assert(x < width && y < height);
		return buf()[y * width + x];
	}

	using Coor = struct {size_t x, y;};
	T& operator[](Coor c) {
		assert(c.x < width && c.y < height);
		return *(buf_.get() + c.y * width + c.x);
	}
	T operator[](Coor c) const {
		assert(c.x < width && c.y < height);
		return *(buf_.get() + c.y * width + c.x);
	}
private:
	std::unique_ptr<T[]> buf_;
};

#endif //GRAPH_MAZE_HPP
