#include <iostream>

#define cimg_OS 1
#define cimg_display 0
#include "CImg.h"
using namespace cimg_library;

int main() {
	CImg<unsigned char> img(640,400,1,3);
	img.fill(0);
	unsigned char purple[] = { 255,0,255 };
	img.draw_text(100,100,"Hello World",purple);
//	img.display("My first CImg code");
	return 0;
}
