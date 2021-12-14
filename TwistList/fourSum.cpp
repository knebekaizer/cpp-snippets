#include <vector>
#include <array>
#include <iostream>
#include <algorithm>
#include <numeric>
#include "trace.h"

using namespace std;


template <typename T>
ostream& operator<<(ostream& os, const vector<T>& v) {
	if (!v.empty()) {
		os << "{" << v[0];
		std::for_each(1 + v.begin(), v.end(), [&os](const auto& x){ os << ", " << x; });
		os << "}";
	}
	return os;
}

vector<vector<int>> fourSum(vector<int>& nums, int target) {
    vector<vector<int>> result;
    if (nums.size() < 4) return {}; // TODO fix magic num
    sort(nums.begin(), nums.end());
    TraceX(nums);

    if (nums.back() * 4 < target) return {};
    if (nums[0] * 4 > target) return {};
    if (nums.size() == 4) {
        if (accumulate(nums.cbegin(), nums.cend(), 0) == target) {
            result.push_back(nums);
        }
        return result;
    }

    auto p0 = nums.cbegin();
    using Iter = decltype(p0);
    using Iters = array<Iter, 4>;
    Iters it{p0, p0+1, p0+2, nums.cend() - 1};

	auto sum3 = [&](){ return *it[0] + *it[1] + *it[2]; }; // use accumulate
	auto end_ = upper_bound(it[2] + 1, nums.cend(), target - sum3());
	it[3] = end_;

    auto advance = [&](auto p, auto end) { return p == end ? end : lower_bound(p + 1, end, 1 + *p); }; // TODO return std::optional
    auto ix = [&](auto p) { return p - nums.cbegin(); };
    auto quat = [&it](){ return vector<int>{*it[0], *it[1], *it[2], *it[3]}; };

    // iterate:
    for (it[0] = nums.cbegin(); it[0] != it[1]; it[0] = advance(it[0], it[1])) {
        for (it[1] = it[0] + 1; it[1] != it[2]; it[1] = advance(it[1], it[2])) {
	        it[2] = it[1] + 1;
	        if (it[2] == end_) break;
	        auto rem = target - sum3();
            it[3] = upper_bound(it[2] + 1, end_, rem);
            TraceX(quat());
            log_trace << ix(it[0]) << " " << ix(it[1]) << " " << ix(it[2]) << " " << ix(it[3]);
            for (; it[2] != it[3]; it[2] = advance(it[2], it[3])) {
                cout << ix(it[0]) << " " << ix(it[1]) << " " << ix(it[2]) << " " << ix(it[3]) << endl;
                auto found = find(it[2] + 1, it[3], target - sum3());
                if (found != it[3]) {
	                it[3] = found;
                    result.emplace_back(vector<int>{*it[0], *it[1], *it[2], *it[3]});
                    log_trace << result.back();
                }
            }
        }
    }
    return result;
}
class Solution {
public:
    vector<vector<int>> fourSum(vector<int>& nums, int target) {
	    vector<vector<int>> result;
	    if (nums.size() < 4) return {}; // TODO fix magic num
	    sort(nums.begin(), nums.end());

	    if (nums.back() * 4 < target) return {};
	    if (nums[0] * 4 > target) return {};
	    if (nums.size() == 4) {
	        if (accumulate(nums.cbegin(), nums.cend(), 0) == target) {
	            result.push_back(nums);
	        }
	        return result;
	    }

	    auto p0 = nums.cbegin();
	    using Iter = decltype(p0);
	    using Iters = array<Iter, 4>;
	    Iters it{p0, p0+1, p0+2, nums.cend() - 1};

		auto sum3 = [&](){ return *it[0] + *it[1] + *it[2]; }; // use accumulate
		auto end_ = upper_bound(it[2] + 1, nums.cend(), target - sum3());

	    auto advance = [&](auto p, auto end) { return p == end ? end : lower_bound(p + 1, end, 1 + *p); }; // TODO return std::optional

	    for (it[0] = nums.cbegin(); it[0] != it[1]; it[0] = advance(it[0], it[1])) {
	        for (it[1] = it[0] + 1; it[1] != it[2]; it[1] = advance(it[1], it[2])) {
		        it[2] = it[1] + 1;
		        if (it[2] == end_) break;
		        auto rem = target - sum3();
	            it[3] = upper_bound(it[2] + 1, end_, rem);
	            for (; it[2] != it[3]; it[2] = advance(it[2], it[3])) {
	                auto found = find(it[2] + 1, it[3], target - sum3());
	                if (found != it[3]) {
		                it[3] = found;
	                    result.emplace_back(vector<int>{*it[0], *it[1], *it[2], *it[3]});
	                }
	            }
	        }
	    }
	    return result;
    }
};


int main() {

	vector<int> inp{-3,-2,-1,0,0,1,2,3};
	TraceX(inp);
	auto target = 0;
	auto res = Solution().fourSum((inp), target);
	log_trace << res;
	return 0;
}
