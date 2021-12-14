#include <Magick++.h>

#include <vector>
#include <random>
#include <span>

#include "trace.h"

using namespace Magick;
using namespace std;


class Maze {
public:
	using T = uint8_t;
	struct Size { size_t w, h;};
	Maze() = default;
	explicit Maze(Size size, float density = 0.4);
	const size_t width = 0;
	const size_t height = 0;

	auto buf() { return span(buf_.get(), width * height); }
	auto buf() const { return span(buf_.get(), width * height); }

	T operator()(size_t x, size_t y) const {
		assert(x < width && y < height);
		return buf()[y * width + x];
	}
private:
	unique_ptr<uint8_t[]> buf_;
};

Maze::Maze(Size size, float density) : width(size.w), height(size.h)
		, buf_(make_unique<T[]>(size.w * size.h)) {
	assert(density >= 0 && density <= 1);
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> dist(0, 99);

	int threshold = static_cast<int>(100 * density);
	for (auto& x : buf()) {
		x = static_cast<T>(dist(gen) > threshold);
	}
}

void test() {
	Image image( "300x300", "white" );

	// Set the image type to TrueColor DirectClass representation.
	image.type(GrayscaleType);
	TraceX(image.depth());

	// Ensure that there is only one reference to underlying image
	// If this is not done, then image pixels will not be modified.
//	image.modifyImage();

	// Allocate pixel view
	Pixels view(image);

	// Set all pixels in region anchored at 38x36, with size 160x230 to green.
//	size_t cols = 196;
//	size_t rows = 162;
	Maze maze({64, 64}, 0.4);

//	Color green("green");

//	auto pp = view.set(38, 36, cols, rows);
	auto pp = view.set(0, 0, image.columns(), image.rows());

	for (auto y = 0; y < view.rows(); ++y) {
		auto j = y * maze.height / view.rows();
		for (auto x = 0; x < view.columns(); ++x) {
//			*pixels++ = QuantumRange * (x + y) / (columns + rows); // gradient
			auto i = x * maze.width / view.columns();
			*pp++ = QuantumRange * maze(i,j); // green.quantumRed();

//			*pixels++ = QuantumRange * green.quantumGreen();
//			*pixels++ = QuantumRange * green.quantumBlue();
		}
	}
	// Save changes to image.
	view.sync();

	image.display();
}

int main() {
	test();
	return 0;
}