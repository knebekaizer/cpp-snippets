#include <iostream>
#include <type_traits>

using namespace std;
int msb(int a) {
	if (a == 0) return -1;
	auto k = 0;
	while(a > 0) {
		a <<= 1;
		++k;
	}
	uint8_t
	return k - 1;
}

int main() {
	std::cout << "Hello, world!" << std::endl;

	assert(msb(1 << 5) == 5);
}