//#include <bits/stdc++.h>
// Add any extra import statements you may need here
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

// Add any helper functions you may need here

int less_(int a, int b) { return a < b; }

int numberOfWays(vector<int>& arr, int sum) {
	// Write your code here
	if (arr.size() < 2) return 0;
	int n = 0;
//	sort(arr.begin(), arr.end());
//	sort(arr.begin(), arr.end(), less_);
	sort(arr.begin(), arr.end(), [](int a, int b) { return a < b; });
	for (auto p = arr.begin(); p != arr.end() - 1; ++p) {
		for (auto q = p + 1; q != arr.end(); ++q) {
			auto s = *p + *q;
			if (s == sum) {
				++n;
			} else if (s > sum) {
				break;
			}
		}
	}
	return n;
}












// These are the tests we use to determine if the solution is correct.
// You can add your own at the bottom.

void printInteger(int n) {
	cout << "[" << n << "]";
}

int test_case_number = 1;

void check(int expected, int output) {
	bool result = (expected == output);
	auto rightTick = "PASSED "; //u8"\u2713";
	auto wrongTick = "FAILED "; // u8"\u2717";
	if (result) {
		cout << rightTick << "Test #" << test_case_number << "\n";
	}
	else {
		cout << wrongTick << "Test #" << test_case_number << ": Expected ";
		printInteger(expected);
		cout << " Your output: ";
		printInteger(output);
		cout << endl;
	}
	test_case_number++;
}

int main() {

	int k_1 = 6;
	vector <int> arr_1{1, 2, 3, 4, 3};
	int expected_1 = 2;
	int output_1 = numberOfWays(arr_1, k_1);
	check(expected_1, output_1);

	int k_2 = 6;
	vector <int> arr_2{1, 5, 3, 3, 3};
	int expected_2 = 4;
	int output_2 = numberOfWays(arr_2, k_2);
	check(expected_2, output_2);

	// Add your own test cases here

}