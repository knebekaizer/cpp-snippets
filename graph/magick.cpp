#include "magick.hpp"

#include <Magick++.h>
#include <span>

#include "maze.hpp"
#include "trace.h"

using namespace Magick;
using namespace std;

void display(const void* data, size_t length) {
	Image image( Blob(data, length), "300x300");
	image.display();
}


void displayMaze(const Maze& maze) {
	Image image( "300x300", "white" );

	// Set the image type to TrueColor DirectClass representation.
	image.type(GrayscaleType);

	// Ensure that there is only one reference to underlying image
	// If this is not done, then image pixels will not be modified.
	image.modifyImage();

	// Allocate pixel view
	Pixels view(image);
	auto pp = view.set(0, 0, image.columns(), image.rows());

	for (auto y = 0; y < view.rows(); ++y) {
		auto j = y * maze.height / view.rows();
		for (auto x = 0; x < view.columns(); ++x) {
//			*pixels++ = QuantumRange * (x + y) / (columns + rows); // gradient
			auto i = x * maze.width / view.columns();
			*pp++ = QuantumRange * (float)maze(i,j); // green.quantumRed();
		}
	}
	// Save changes to image.
	view.sync();
	image.display();
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

/*
int main() {
	test();
	return 0;
}
*/
