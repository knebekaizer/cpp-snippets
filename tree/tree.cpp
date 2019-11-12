#include "wmc_trace.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <vector>

//#include <hash_map>

using namespace std;

/*
void test(istream const& inp)
{
	istream input(inp);
    while (input.good()) {
    	string s;
    	input >> s;
    	cout << s;
    }

}
*/

typedef int	T;

class Node {
public:
	Node(const T& t) : v(t), child(0), sibling(0) {}
	T	v;
	class Node* child;
	class Node* sibling;
	~Node() { cout << "dtor " << v << endl; child = sibling = 0; }
};

template<class Visitor>
void walk(Visitor visit, const Node* p, bool depth=true)
{
	if (!p) return;
	while (p) {
		const Node* sibling = p->sibling;
		if (!depth) {
			const Node* child = p->child;
			visit(p);
			walk(visit+1, child, depth);
		} else {
			walk(visit+1, p->child, depth);
			visit(p);
		}
		p = sibling;
	}
}

/*
template<class Visitor>
void walk(Visitor visit, const Node* p, bool depth=true)
{
	if (!p) return;
	while (p) {
	//	const Node* sibling = p->sibling;
		if (!depth) {
			visit(p);
			walk(visit+1, p->child, depth);
		} else {
			walk(visit+1, p->child, depth);
			visit(p);
		}
		p = p->sibling;
	}
}
*/
void walk(void (*visit)(const Node*), const Node* p, bool depth=true)
{
	if (!p) return;
	while (p) {
		const Node* sibling = p->sibling;
		if (!depth) {
			const Node* child = p->child;
			visit(p);
			walk(visit, child, depth);
		} else {
			walk(visit, p->child, depth);
			visit(p);
		}
		p = sibling;
	}
}


// return [0, n)
int random(int n = RAND_MAX)
{
	return int(0.5 + rand()/float(RAND_MAX) * (n-1));
}

Node* buildTest(int n)
{
	vector<Node*> buf;
	buf.push_back(new Node(0));
	for (int k=1; k!=n; ++k) {
		Node* that = new Node(k);
		int r = random(k);
		Node* p = buf.at(r);
		tr_stream << r << "->child=" << k;
		if (!p->child) {
			p->child = that;
		} else {
			p = p->child; 
			while (p->sibling) p = p->sibling;	// get to the end of chain
			p->sibling = that;
		}
		buf.push_back(that);
	}
	return buf[0];		
}

void foo(const Node* x)
{
	cout << x->v << endl;
}

class FOut {
	int offset;
public:
	FOut() : offset(0) {}
	FOut operator+(int k) const { FOut tmp = *this; tmp.offset += k; return tmp; }
	void operator()(const Node* x) {
		for (int k=0; k!=offset; ++k) cout << ' ';
		cout << x->v << endl;
	} 
};

void del(const Node* x) {
	delete(x);
}

int main( void )
{
	tr_stream << "This is a test";
	
	Node* root = buildTest(32 );
	walk(FOut(), root);
	tr_stream;
	walk(FOut(), root, false);

	walk(del, root);
	tr_stream;
//	walk(del, root, false);

	tr_stream << "Test finished";
	return 0;
}


