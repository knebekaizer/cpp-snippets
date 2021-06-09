#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <map>

#include "trace.h"

using namespace std;

using Domino = pair<int,int>;
using Dominoes = vector<Domino>;

ostream& operator<<(ostream& os, const Domino& d) {
	return os << '(' << d.first << ':' << d.second << ')';
}
template<typename T>
ostream& operator<<(ostream& os, const vector<T>& d) {
	os << '{';
	for (auto& x : d) os << x << ' ';
	return os << '}';
}

class DominoGraph {
public:
	explicit DominoGraph(Dominoes inp_);

	using Value = int; // 0..6
	using ID = int; //Dominoes::const_iterator; // index
	using Indices = vector<ID>;

	Dominoes byId(const Indices& ix) const {
		Dominoes ret; ret.reserve(ix.size());
		for (auto id : ix) ret.push_back(inp[id]); // back_inserter
		return ret;
	}

	bool dfs(ID id, int length, bool reverted = false);

	using IndexMap = multimap<Value, ID>;

	const Dominoes inp;
	IndexMap byFirst;
	IndexMap bySecond;
	vector<bool> used;

	int maxLen = 0;
	vector<pair<ID, bool>> maxPath;
};

DominoGraph::DominoGraph(Dominoes inp_) : inp(move(inp_)),  used(inp.size()) {
	for (auto i = 0; i < inp.size(); ++i) {
		byFirst.insert({inp[i].first, i});
		bySecond.insert({inp[i].second, i});
	}
}

bool DominoGraph::dfs(ID id, int length, bool reverted) {
	assert(used[id] == false);
//	TraceX(id, inp[id], reverted, length);
	bool longest = false;
	used[id] = true;
	auto v = reverted ? inp[id].first : inp[id].second;
	// find first unused
	auto rng = byFirst.equal_range(v);
	for (auto p = rng.first; p != rng.second; ++p) {
		if (!used[p->second])
			longest |= dfs(p->second, length + 1);
	}
	auto rng2 = bySecond.equal_range(v);
	for (auto p = rng2.first; p != rng2.second; ++p) {
		if (!used[p->second])
			longest |= dfs(p->second, length + 1, true);
	}
	if (maxLen < length) {
		maxLen = length;
		longest = true;
		maxPath.resize(0); // start recording path
	}
	if (longest) maxPath.emplace_back(id, reverted);
	return longest;
}


string dumpMaxPath(const DominoGraph& g) {
	ostringstream os;
	for (auto& x : g.maxPath) {
		 os << g.inp[x.first] << (x.second ? "^" : "") << ' ';
	}
	return os.str();
}


int longPath(Dominoes const& inp) {
	DominoGraph g(inp);
//	g.dfs(0, 0);
	for (auto id = 0; id < inp.size(); ++id) {
		for (auto rev : {false, true}) {
			fill(g.used.begin(), g.used.end(), false);
			auto longest = g.dfs(id, 1, rev);
			if (longest) {
				TraceX(id, rev, g.maxLen);
				TraceX(dumpMaxPath(g));
			}
		}
	}
	reverse(g.maxPath.begin(), g.maxPath.end());
	TraceX(dumpMaxPath(g));
	return g.maxLen;
}

void test() {
	Dominoes inp = {{1,2}, {1,3}, {2,3}, {2,5}, {3,4}};
	longPath(inp);
//		TraceX(inp[k], g.byId(g.edgesFrom(k)));

}


int main() {
	test();
	return 0;
}