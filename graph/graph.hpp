//
// Created by Vladimir Ivanov on 28.05.2021.
//

#ifndef GRAPH_GRAPH_HPP
#define GRAPH_GRAPH_HPP


#include <vector>
#include <string>
#include <memory>
#include <iosfwd>

#include <range/v3/all.hpp>

class Node;
class Edge;

using Cost = int;
using NodeRef = std::reference_wrapper<const Node>; // Node*; //

class EdgeTo {
public:
	explicit EdgeTo(NodeRef n, Cost v = 1) : cost(v), to_(n) {}
	const Cost cost;
	const Node& to() const { return to_; }

	NodeRef to_;
};

class Edge : public EdgeTo {
public:
	Edge(NodeRef n, NodeRef m, Cost v = 1) : EdgeTo(m, v), from_(n) {}
	Edge(NodeRef n, const EdgeTo& to) : EdgeTo(to), from_(n) {}
	const Node& from() const { return from_; }

	std::ostream& print(std::ostream& os) const;

	NodeRef from_;
};

class Node {
public:
	using Edges = std::vector<EdgeTo>;
	using id_type = uintptr_t;

	Node(std::string n, Edges e) : name(std::move(n)), edges(std::move(e)) {}
	auto addEdgeTo(NodeRef n, Cost v = 1) { return edges.emplace_back(n, v); }

	std::ostream& print(std::ostream& os) const;

	id_type id() const { return reinterpret_cast<id_type>(this); }

	std::string name;
	Edges edges;
	struct Agnode_s* gvData = nullptr;

#ifndef NDEBUG
	~Node();
#endif
};

class Graph {
public:
	using Nodes = std::vector<std::unique_ptr<Node>>;
	Graph() = default;
	explicit Graph(std::string id, Nodes ns = {}) : name(std::move(id)), nodes(std::move(ns)) {}

	void add(Node* n) { nodes.emplace_back(n); }
	Node& addNode(std::string n, Node::Edges e);

	auto nodes_() const {
		return nodes | ranges::views::transform( [](auto& p) -> Node& { return *p.get(); } );
	}


	std::ostream& print(std::ostream& os) const;

	const std::string name;
	Nodes nodes;
};

inline std::ostream& operator<<(std::ostream& os, Node const& x) { return x.print(os); }
inline std::ostream& operator<<(std::ostream& os, Edge const& x) { return x.print(os); }
inline std::ostream& operator<<(std::ostream& os, Graph const& x) { return x.print(os); }

#endif //GRAPH_GRAPH_HPP
