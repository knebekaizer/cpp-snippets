#include <vector>

#include "trace.h"

using namespace std;

class Solution {
public:
    void moveZeroes2(vector<int>& nums) const {
	    constexpr int target = 0;
	    auto e = nums.end();
        auto to = nums.begin();
	    auto p = to;
	    while (p != e && *p == target) ++p;
		for (; p != e; ++p) {
			if (*p != target) {
				*to++ = *p;
			}
		}
		while (to != e) *to++ = target;
    }

    void moveZeroes(vector<int>& nums) const {
    	constexpr int target = 0;
	    auto e = nums.end();
        auto to = nums.begin();
        auto from = to;
	    while (from != e && *from == target) ++from;
        while (true) {
	        auto p = find(from, e, target);
        	if (from != to) copy(from, p, to);
	        to += p - from;
		    if (p == e) break;
		    from = p + 1;
		    while (from != e && *from == target) ++from;
	    }
        if (to != e) fill(to, e, target);
    }
};

using Vec = vector<int>;

ostream& operator<<(ostream& os, const Vec& v) {
	os << '{';
	if (!v.empty()) {
		os << v[0];
		for_each(v.begin() + 1, v.end(), [&os](auto& x){ os << ',' << x;});
	}
	os << '}';
	return os;
}

void test(Vec nums) {
	Solution().moveZeroes2(nums);
	TraceX(nums);
}
int main() {
	test({0, 1, 0, 3, 12});
	test({1,2,3,4});
	test({0,0,0,0,0});
	test({1,2,3,4,0});
	test({1,2,3,4,0,0,0});
	test({0,0,0,1,2,3,4,0,0,0});
}