//
// Created by Vladimir on 2023-12-11.
//

#include "trace.hpp"

#include <initializer_list>
#include <any>
#include <list>
#include <utility>
#include <string>

using namespace std;

using Value = pair<string, any>;
//using Nested = pair<string, initializer_list<Value>>;

class Dict {
	list<any> ls_;
public:
//	template<typename ... Args> Dict(Args&& ... a);

	using Nested = pair<string, Dict>;
//	Dict(initializer_list<Nested>&& x);
	Dict(initializer_list<Dict>&& x) { TraceX(1); };
//	Dict(initializer_list<Dict> const& x);
//	Dict(Nested&& x) { TraceX(2); };
	Dict(const char*, any&&) { TraceX(3); };
	Dict(const char*, Dict&&) { TraceX(4); };
};


int main() {
	Dict d = {
		{"a", 1},
		{"a", 1},
		{"nested",
			{
				{"a", 1},
				{"b", 2},
				{"nested2",
					{
						{"a", 1},
						{"b", 2},
					}
				}
			}
		}
	};
	return 0;
}