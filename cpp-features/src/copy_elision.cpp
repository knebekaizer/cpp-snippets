//
// Created by vdi on 27.02.25.
//

#include "trace.hpp"
#include <iostream>

using namespace std;

ostream& operator<<(ostream& os, const struct S& s);

struct S {
	static int count;
	int id = 0;
	int parent = 0;
	S() : id(++count)  { cout << "; default ctor: " << *this; }
	S(const S& x) : id(++count), parent(x.id) { cout << ";copy ctor: " << *this; }
	S(S&& x) : id(++count), parent(x.parent) { cout << "; move ctor: " << *this; }
	S& operator=(const S& x) { cout << "; copy to "  << *this << " from: " << x; return *this; }
	S& operator=(S&& x) { cout << "; copy to "  << *this << " from: " << x; return *this; }
	~S() { cout << "; dtor: " << *this; }
};

int S::count = 0;

ostream& operator<<(ostream& os, const S& s) {
	return os << "(" << s.id << '.' << s.parent << ")";
}

S f1() {
	return S();
}

bool fff;

S f2() {
	S s;
	if (fff)
		return s;
	else
		s.id--;
	return s;
}

void test_f1() {
	{
		cout << "test_f1> ignore return: ";
		f1();
		cout << "; end";
	}
	cout << "; done.\n";
	{
		cout << "test_f1> by value:      ";
		[[maybe_unused]] auto x = f1();
		cout << "; end";
	}
	cout << "; done.\n";
	{
		cout << "test_f1> by const ref:  ";
		[[maybe_unused]] auto const& r = f1();
		cout << "; end";
	}
	cout << "; done.\n";
}

void test_f2() {
	{
		cout << "test_f2> ignore return: ";
		f2();
		cout << "; end";
	}
	cout << "; done.\n";
	{
		cout << "test_f2> by value:      ";
		[[maybe_unused]] auto x = f2();
		cout << "; end";
	}
	cout << "; done.\n";
	{
		cout << "test_f2> by const ref:  ";
		[[maybe_unused]] auto const& r = f2();
		cout << "; end";
	}
	cout << "; done.\n";
}


int main() {
	test_f1();
	cout << endl;
	test_f2();
	return 0;
}