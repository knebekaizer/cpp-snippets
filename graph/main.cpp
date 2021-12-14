#include "graph.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <future>

#include "maze.hpp"
#include "cgraph_api.hpp"
#include "magick.hpp"

#include "trace.h"

#include <range/v3/all.hpp>
using namespace ranges;
namespace rng = ranges;
namespace vs = ranges::views;

using namespace std;
using namespace std::literals;

template <typename T>
ostream& operator<<(ostream& os, const vector<T>& v) {
	os << "\n";
	for (auto& x : v) os << "  " << x << '\n';
	os << '}';
	return os;
}

Graph test_build() {
	Graph g("TestGraph");
	auto& a = g.addNode("A", Node::Edges{});

	auto& b = g.addNode("B", Node::Edges{});
	a.edges.emplace_back(b);

	auto& c = g.addNode("C", Node::Edges{});
	a.edges.emplace_back(c);

	auto& d = g.addNode("D", Node::Edges{});
	b.edges.emplace_back(d);
	c.edges.emplace_back(d);

	auto& e = g.addNode("E", Node::Edges{});
	d.edges.emplace_back(e);

//	TraceX(g.nodes | vs::transform([](auto& x) { return (intptr_t)x.get(); }));
	log_trace << "g.nodes:" << hex << "\n    "
			<< (g.nodes | vs::transform(&Graph::Nodes::value_type::get))
//			<< (g.nodes | vs::transform(&unique_ptr<Node>::get))
//			<< (g.nodes | vs::transform([](auto& x) { return (intptr_t)x.get(); }))
			<< dec << endl;
//	g.nodes | actions::sort; //({}, [](auto& x) { return (intptr_t)x.get(); });

	return g;
}

void gv_render(Graph& g) {

	class GvRenderData {
	public:
		~GvRenderData() { gvFreeRenderData(data_); }
		void render(GVC_t *gvc, Agraph_t* g, const char * format) { gvRenderData(gvc, g, (char*)format, &data_, &length_); }
		auto data() const { return data_; }
		auto size() const { return length_; }
	private:
		char* data_ = nullptr;
		unsigned int length_ = 0;
	};
//	gvRenderData (GVC_t *gvc, Agraph_t* g, char *format, char **result,
//	      unsigned int *length)
//	which writes the output of the rendering onto an allocated character buffer. A pointer to this buffer is returned
//	in *result and the number of bytes written is stored in length. After using the buffer, the memory should be freed
//	by the application. As the calling program may rely on a different run-time system than that used by Graphviz,
//	the library provides the function
//	    gvFreeRenderData (char *data);

	Agraph_t* G = agopen(const_cast<char*>("TEST"), Agdirected, nullptr);
	for (auto& x : g.nodes) {
		x->gvData = agnode(G, x->name.data(), 1);
	}
	for (auto& x : g.nodes) {
		for (auto& edge : x->edges) {
			[[maybe_unused]] Agedge_t* e = agedge(G, x->gvData, edge.to().gvData, (x->name + edge.to().name).data() , 1);
		}
	}

	GVC_t* gvc = gvContext();

	/*
	 * Create gvc and graph
	 */
	gvLayout (gvc, G, "dot"); // gvLayout (gvc, G, "nop"); //
	gvRender (gvc, G, "dot", nullptr);
	gvFreeLayout(gvc, G);
	gvLayout (gvc, G, "nop");
//	gvRender (gvc, G, "png", nullptr);
	GvRenderData r;
	r.render(gvc, G, "png");
//	ofstream png("test.png");
//	png.write(r.data(), r.size());
	display(r.data(), r.size());

	gvFreeLayout(gvc, G);
	agclose (G);
}

//#define index(x,y) buf[(y) * maze.width + (x)]

Graph maze2graph(const Maze& maze) {
	Graph g;
//	auto buf = make_unique<Node*[]>(maze.height * maze.width);
//	vector<Node*> buf(maze.height * maze.width);

//	auto index = [w = maze.width, &buf] (auto x, auto y) -> vector<Node*>::reference { return buf[y * w + x]; };
	auto index = [w = maze.width, buf = make_unique<Node*[]>(maze.height * maze.width)]
			(auto x, auto y) -> vector<Node*>::reference { return buf[y * w + x]; };
	for (size_t y = 0; y != maze.height; ++y) {
		for (size_t x = 0; x != maze.width; ++x) {
			if (maze[{x, y}]) {
				auto id = to_string(y) + ":"s + to_string((x));
				g.addNode(id, {});
				auto* that = g.nodes.back().get();
				index(x, y) = that;
				if (x > 0 && maze(x-1, y)) {
					index(x-1, y)->addEdgeTo(*that);
					that->addEdgeTo(*index(x-1, y));
				}
				if (y > 0 && maze(x, y-1)) {
					index(x, y-1)->addEdgeTo(*that);
				}
			}
		}
	}
	return g;
}

int main() {
	log_trace << "Start";

//	auto g = test_build();
//	gv_render(g);
	TraceX(test_build().nodes_());

//	TraceX(g);

//	for (auto seed : vs::ints(0,8)) {
//		displayMaze(Maze({8,8}, 0.3, seed));
//	}


//	auto f = std::async(std::launch::deferred, [&]{
//		displayMaze(maze);
//		return 0;
//	});

// 170130882 137273164
	Maze maze({8,8}, 0.3);
	auto g2 = maze2graph(maze);
	gv_render(g2);
	displayMaze(maze);

//	f.get();

	log_trace << "Done.";
	return 0;
}
