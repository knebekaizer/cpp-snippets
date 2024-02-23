//
// Created by Vladimir on 2024-02-12.
//

#include <bitset>
#include <iostream>
using namespace std;

void foo(bool x) {
	cout << "Good " << x << endl;
}

template <typename T>
void foo(T x) {
	cout << "Bad " << !x << endl;
}

int main() {
	const bitset<8> bs{"10110111"};
	foo(bs[2]);
	return 0;
}