//
// Created by Vladimir Ivanov on 28.05.2021.
//

#include "graph.hpp"

#include <string>
#include <vector>
#include <iostream>

#include "trace.h"

using namespace std;
using namespace std::literals;

ostream& Node::print(ostream& os) const {
	const auto indent = "  ";
	const auto attr = "";
    os << indent << name << attr << ';';
    return os;
}

ostream& Edge::print(ostream& os) const {
	const auto indent = "  ";
	const auto attr = "";
	auto isDirected = false; // true
	const auto symbol = isDirected ? "->" : "--";

	os << indent << from().name
        << symbol << to().name;
	if (attr && attr[0]) os << " " << attr << ';';
	return os;
}


#ifndef NDEBUG
	Node::~Node() { TraceX(name); }
#endif

Node&
Graph::addNode(std::string name, Node::Edges e) {
	auto n = std::make_unique<Node>(move(name), move(e));
	nodes.push_back(move(n));
//	nodes.push_back(std::make_unique<Node*>(name, e));
	return *nodes.back();
}


ostream& Graph::print(ostream& os) const {
	const auto type = "graph" ; // "digraph
	os << type <<" "<< name << " {\n";
//  increase indent
//  if (getAttributes(AttrType::GRAPH).size() > 0) {
//      npl.os << npl.indent << "graph ";
//      getAttributes(AttrType::GRAPH).printTo(npl);
//      npl.os << endl;
//  }
//  if (getAttributes(AttrType::EDGE).size() > 0) {
//      npl.os << npl.indent << "edge ";
//      getAttributes(AttrType::EDGE).printTo(npl);
//      npl.os << endl;
//  }
//  if (getAttributes(AttrType::NODE).size() > 0) {
//      npl.os << npl.indent << "node ";
//      getAttributes(AttrType::NODE).printTo(npl);
//      npl.os << endl;
//  }
	for (auto& x : nodes) {
		x->print(os); os << endl;
		for (auto& to : x->edges) {
			Edge(*x, to).print(os);
			os << endl;
		}
	}
	os << "}";
	return os;
}

