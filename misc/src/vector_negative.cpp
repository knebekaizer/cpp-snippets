#include <vector>
#include "trace.h"

class MyErr : public std::runtime_error {
public:
	MyErr(const std::string& s) : std::runtime_error(s) {}
};

class MyClass {
public:
	MyClass() : x(s_x++) {};
	MyClass(const MyClass&) = delete; // default; //
	MyClass(MyClass&&) = default; // delete; //
	MyClass& operator=(const MyClass&) = delete;
	MyClass& operator=(MyClass&&) = delete; // default; //
	std::ostream& print(std::ostream& os) const { return os << "{" << x << "}"; }
	int getSCount() const { return s_x; }
private:
	int x = 0;
	static int s_x;
};
int MyClass::s_x = 0;
using namespace std;

std::ostream& operator<<(std::ostream& os, MyClass const& c) {
	return c.print(os);
}

void test_capacity() {
	for (int n = 0; n < 8; ++n) {
		vector<int> v;
		v.reserve(n);
		TraceX(v.size(), v.capacity());
	}
	for (int n = 0; n < 128; ++n) {
		vector<int> v;
		v.reserve(n);
		if (v.capacity() != n) TraceX(v.capacity());
	}
	vector<int> v;
	size_t cap = 0;
	for (int n = 0; n < 128; ++n) {
		v.resize(n);

		if (cap != v.capacity()) {
			cap = v.capacity();
			if (v.capacity() != n)
				TraceX(v.capacity());
		}
	}
}

void test_neg() {
	std::vector<int> v(3);
	TraceX(v.size(), v.capacity());

	v.resize(6);
	v.resize(v.capacity() + 1);
	TraceX(v.size(), v.capacity());

//	/* don't iterate over 4 elements of eq class 'Pure Sizes [EC-7]' (null),
//	 * only one eq class suffices for seg fault */
//	ec_typ_Sizes Sizes = static_cast<ec_typ_Sizes>(ec_Sizes_S0);
//	ec_typ_VC VC =static_cast<ec_typ_VC>(ec_VC_VC1);
//	/* Prelude of test 'float* [test_float_combination]': */
//	std::vector<float*> vinput = make_vector<float*>(VC, Sizes);
//	if (isPossible_test_float_combination(Sizes,VC)) {
//		memset(&vinput, 0, sizeof(vinput)); // destroying the vector
//		//should raise segmentation fault
//		rbt_assert(*vinput.front() , "expected to crash but didn't",false);
//	}
//	else {
//		rbt_assert(*vinput.front() ,"expected to crash but didn't", false);
//	}
}

int main() {
	test_capacity();
	test_neg();
}