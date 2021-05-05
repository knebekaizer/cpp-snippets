
//#include "policy.h"

#include <iostream>
#include <vector>
#include <memory>
#include "trace.h"

using namespace std;

class CTest {
public:
	explicit CTest(int k) : v_(k) {}

	int getV() const { return v_; }
	int setV(int k) { swap(k, v_); return k; }
private:
	int v_ = 0;
};

void test() {
	using TList = vector<shared_ptr<CTest>>;
//	  vector<int> registry = {0, 1, 2, 42};
	TList regs;
	for (int k=0; k<8; ++k) regs.emplace_back( make_shared<CTest>(k) );

	for (auto p : regs) {
		cout << p.use_count() << " ";
		p->setV(0);
	}
	cout << endl;

//	  for (const auto p : regs) {
//		  cout << p.use_count() << " ";
//		  p->setV(0);
//	  }
//	  cout << endl;

	for (const shared_ptr<CTest>& p : regs) {
		cout << p.use_count() << " ";
		p->setV(0);
	}
	cout << endl;

	for (const shared_ptr<const CTest> p : regs) {
		cout << p.use_count() << " ";
	}
	cout << endl;

//	  //  error: loop variable 'p' has type 'const shared_ptr<const CTest> &' but is initialized with type 'std::__1::shared_ptr<CTest>' resulting in a copy [-Werror,-Wrange-loop-analysis]
//	  for (const shared_ptr<const CTest>& p : regs) {
//		  cout << p.use_count() << " ";
//	  }
//	  cout << endl;

}

int main() {
	test();
	return 0;
}