#include <iostream>
#include <vector>
#include <list>
#include <string>

#undef NDEBUG
#include <cassert>

#include "range/v3/all.hpp"

#include "scope.hpp"
#include "trace.h"

using namespace std;
using namespace std::literals;

using namespace ranges;
namespace rng = ranges;
namespace vs = ranges::views;


struct Node {
	Node(Node* n, Node* r) : value(++incr), next(n), right(r) {}
	Node(Node* n, Node* r, int v) : value(v), next(n), right(r) {}

	Node(const Node&) = delete;
	Node& operator=(const Node&) = delete;

	int value;
	Node* next = nullptr;
	Node* right = nullptr;

	constexpr static uintptr_t fMoved = 1;
	inline static int incr = 0;

	string dump() const;
};

namespace Tagged {
	enum class Flag : uintptr_t { NONE = 0, ONE = 1, TWO = 2, MASK = 7 };
	uintptr_t set(const void* p, Flag f) { return (uintptr_t)p | (uintptr_t)f; }
	uintptr_t clear(uintptr_t p, Flag f) { return p & ~(uintptr_t)f; }
	void* get(uintptr_t p) { return (void*)(p & ~(uintptr_t)Flag::MASK); }
	bool has(uintptr_t p, Flag f) { return p & (uintptr_t)f; }

class Ptr {
	using T = Node;
public:
	Ptr(const T* p, Flag f = Flag::NONE) : ptr_(set(p, f)) {}
	Ptr& operator =(const T* p) { ptr_ = (uintptr_t)(p); return *this; }
//	operator T*() const { return (T*)get(ptr_); }
	bool has(Flag f) const { return Tagged::has(ptr_, f); }
	T* operator ->() { return static_cast<T*>(Tagged::get(ptr_)); }
	const T* operator ->() const { return static_cast<const T*>(Tagged::get(ptr_)); }
private:
	uintptr_t   ptr_;
};
} //namespace Tagged
using Tagged::Flag;
using TaggedPtr = Tagged::Ptr;

string dump(TaggedPtr n)  {
	constexpr uintptr_t mask = 7;
	Node* next = n->next ? (Node*)((uintptr_t)n->next & ~mask) : nullptr;
	Node* right = n->right ?
		             (Node*)((uintptr_t)n->right & ~mask)
		             : nullptr;
	return to_string(n->value) + "->(" +
	(next ? to_string(next->value) : "null") + ", " +
	(right ? to_string(right->value) : "null") +
	")";
}

string Node::dump() const { return ::dump(this); }

ostream& operator<<(ostream& os, const Node& x) {
	return os << x.value;
}

/*
void moveTwisted(Node* head) {

	copy to newOne // next_new = next
	mark as copied // next.setMoved(next_new) /
		next.next = next_new & fMoved
	patch current // thiz.next = next_new
	if ((r = thiz.right) && r.isMoved)
		thiz.right = r.next & ~fMoved
		delete r

	// cleanup pass
	 if ((r = thiz.right) && r.isMoved)
		thiz.right = r.next & ~fMoved
		delete r

}
*/
struct XNode {
	XNode(Node* n, Node* r) : value(++incr), next(n), right(r) {}
	XNode(Node* n, Node* r, int v) : value(v), next(n), right(r) {}

	int value;
	Node* next = nullptr;
	Node* right = nullptr;

	constexpr static uintptr_t fMoved = 1;
	inline static int incr = 0;
};

void moveTwisted_2(Node* head) {
	for (auto p = head; p; p = p->next)
}


template <typename T>
ostream& operator<<(ostream& os, const vector<T>& v) {
	if (!v.empty()) {
		os << "{" << v[0];
		std::for_each(1 + v.begin(), v.end(), [&os](const auto& x){ os << ", " << x; });
		os << "}";
	}
	return os;
}

void test_gen() {
	// fill std::list
	{
		auto ls = std::list<int>{} | actions::push_back(views::iota(0, 10));
		auto ls_v = ls | vs::all;
		TraceX(ls_v);
	}
	constexpr size_t N = 8;
//	auto gen_v = vs::generate_n([head = (Node*)nullptr]() mutable { head = new Node(head); return head; }, N );
//	Node* head = (gen_v | vs::take_last(1) | to_vector)[0];
//	TraceX(gen_v);

	auto ls_gen = [](size_t n){ Node* head = nullptr; while (n--) head = new Node(head, nullptr); return head; };
	Node* head = ls_gen(N);

	auto v_next = [](auto head) {; return vs::generate([p=head]() mutable { auto tmp = p; if (p) p = p->next; return tmp; } )
			| vs::take_while([](auto p) { return p != nullptr; }); };
	TraceX(v_next(head));
	auto toValue = vs::transform([](auto p) -> Node& { return *p; });
	TraceX(v_next(head) | toValue);

	auto p = head;
	for (auto r : v_next(head) | vs::drop(1) | to_vector | actions::shuffle(mt19937{})) {
		assert(p);
		p->right = r;
		p = r;
	}

	auto v_right = [](auto head) {; return vs::generate( [p=head]() mutable { auto tmp = p; if (p) p = p->right; return tmp; } )
			| vs::take_while([](auto p) { return p != nullptr; }); };
	TraceX(v_right(head) | toValue);
}

namespace twist::utils {

Node* genTwisted(size_t n) {
	Node::incr = 0;

	Node* head = nullptr;
	vector<Node*> v(n);
	while (n--) {
		head = new Node(head, nullptr);
		v[n] = head;
	}

	auto shuffle = vs::drop(1) | actions::shuffle(mt19937{});
	auto p = head;
	for (auto r :  v | shuffle) {
		p->right = r;
		p = r;
	}

	return head;
}

auto v_next = [](auto head) {; return vs::generate([p=head]() mutable { auto tmp = p; if (p) p = p->next; return tmp; } )
		| vs::take_while([](auto p) { return p != nullptr; }); };

auto v_right = [](auto head) {; return vs::generate( [p=head]() mutable { auto tmp = p; if (p) p = p->right; return tmp; } )
		| vs::take_while([](auto p) { return p != nullptr; }); };

auto to_value = vs::indirect; //vs::transform([](auto p) -> Node& { return *p; });

auto to_dump = vs::transform([](auto* p) -> string { return p->dump(); });

// `node->dump()` generates temporary strings; so does `to_dump`; use cache1 to keep them for transform
auto dumpAll = to_dump | vs::cache1 | vs::transform([](const string& s){ return s | vs::all; }) | vs::join(vs::c_str("\n    "));
}
using namespace twist::utils;

/*
void test_gen2() {

	Node::incr = 0;
	constexpr size_t N = 8;

	auto ls_gen = [](size_t n) {
		Node* head = nullptr;
		vector<Node*> v(n);
		while (n--) {
			head = new Node(head);
			v[n] = head;
		}

		auto shuffle = vs::drop(1) | actions::shuffle(mt19937{});
		auto p = head;
		for (auto r :  v | shuffle) {
			p->right = r;
			p = r;
		}

		return head;
	};
	Node* head = ls_gen(N);

	TraceX(v_next(head) | toValue);
	TraceX(v_right(head) | toValue);

	TraceX(v_next(head) | to_dump);
	TraceX(v_right(head) | to_dump);

	log_trace << "v_next(head) | dumpAll | to<string> =\n    " << (v_next(head) | dumpAll | to<string>);
}
*/

namespace twist::utils {
	auto revertNext = [](Node* head) {
		Node* prev = nullptr;
		for (auto p = head; p;) {
			auto next = p->next;
			p->next = prev;
			prev = p;
			p = next;
		}
		return prev;
	};

	auto revertRight = [](Node* head) {
		Node* prev = nullptr;
		for (auto p = head; p;) {
			auto next = p->right;
			p->right = prev;
			prev = p;
			p = next;
		}
		return prev;
	};

//	 auto xorList = [=](Node* head) {
//		Node* prev = nullptr;
//		for (auto p = head; p;) {
//			TraceX(*p);
//			auto next = p->right;
//			p->right = xorAddr(prev, next);
//			prev = p;
//			p = next;
//		}
//		return prev;
//	};
//	[[maybe_unused]]
//	Node* tail = xorList(head);

}

void test_reverse() {
	Node* head = genTwisted(8);
	TraceX(v_next(head) | to_value);

	head = revertNext(head);
	TraceX(v_next(head) | to_value);
	head = revertNext(head);
	TraceX(v_next(head) | to_value);

	TraceX(v_right(head) | to_value);
	Node* r = revertRight(head);
	TraceX(v_next(head) | to_dump);
	TraceX(v_right(r) | to_value);
	Node* l = revertRight(r);

#ifdef NDEBUG
	log_error <<  "assert is turned off";
#endif
	assert(head == l);

	TraceX(v_right(head) | to_value);
}

Node* moveTwisted(Node* head) {
	uintptr_t fXored = 1;
	uintptr_t fNotMoved = 1;
	uintptr_t fBackward = 2;
	uintptr_t fProceed = 2;

	uintptr_t mask = 3;
	auto hasFlag = [](Node* p, auto flag) -> bool { return (uintptr_t)p & flag; };
	auto setFlag = [](Node*& p, auto flag) { *(uintptr_t*)&p |= flag; };
	auto clearFlag = [](Node*& p, auto flag) { return *(uintptr_t*)&p &= ~flag; };

	auto setXor = [fXored](Node*& a, Node* b) { *(uintptr_t*)&a ^= (uintptr_t)b ^ fXored; };
	auto unXor = [=](Node* p, Node* o) {
		if (hasFlag(p, fXored)) {
			assert(!hasFlag(o, fXored));
			return (Node*)((uintptr_t)p ^ (uintptr_t)o ^ fXored);
		}
		return p;
	};

	auto getAddr = [mask](Node* p) { return (Node*)((uintptr_t)p & ~mask); };

	auto v_next = [=](auto head) {; return vs::generate([=,p=head]() mutable { auto tmp = p; if (p) p = getAddr(p->next); return tmp; } )
			| vs::take_while([](auto p) { return p != nullptr; }); };

	auto v_right = [=,prev = (Node*)nullptr](auto head) mutable { return vs::generate(
			[=,p=head]() mutable {
				if (!p) return (Node*)nullptr;
//				TraceX(p->value, p->next, p->right);
				auto tmp = p;
				p->right = unXor(p->right, prev);
				p = getAddr(p->right);
				prev = tmp;
				return tmp;
			}) | vs::take_while([](auto p) { return p != nullptr; });
	};

	auto setNotMoved = [=](Node* p) { setFlag(p->next, fNotMoved); };
	auto clearNotMoved = [=](Node* p) { clearFlag(p->next, fNotMoved); };
	auto isNotMoved = [=](Node* p) { return hasFlag(p->next, fNotMoved); };

	TraceX(v_right(head) | to_value);
    // revert rightList
	Node* r_tail = revertRight(head);
	log_trace << "reverted right";
	TraceX(v_right(r_tail) | to_value);
	TraceX(v_right(r_tail) | to_dump);
	TraceX(v_next(head) | to_dump);
	TraceX(r_tail);

	// mark backward
	for (auto p = head; p; p = getAddr(p->next)) {
		if (p->right && hasFlag(p->right->next, fProceed)) {
			setFlag(p->right, fBackward);
		}
		setFlag(p->next, fProceed);
	}

	{    // walk by next
		Node* prev = nullptr;
		for (auto p = head; p; p = getAddr(p->next)) {
			clearFlag(p->next, fProceed);
//			TraceX(1, *p);
			// if isRev(right) {
			//      move
			//      xorPatch(right)
			auto r = p->right;
			if (!hasFlag(r, fXored) && !hasFlag(r, fBackward)) { // movable
				TraceX("Movable", p->dump(), p->next, p->right);
				auto n = new Node(p->next, p->right, -p->value);
//				TraceX("Movable 2", p->dump(), p->next, p->right, n->dump());

				if (p == r_tail) {
					r_tail = n;
					TraceX("new r_tail", r_tail, r_tail->dump(), n->dump());
				}
				if (r) {
					TraceX("xoring:", r->dump());
					setXor(r->right, n);
				}
				if (prev) {
					prev->next = n; // skip the very first
				 } else {
					assert(p == head);
					head = n; // update head!!!
					log_trace << "Updating head: " << head->dump();
				}
				prev = n;
				p->value += 90;
//				delete p;
				p = n;
			} else {
				TraceX("setNotMoved", *p);
				setNotMoved(p);
			}
		}
	}

	TraceX(v_right(r_tail) | to_value);
	auto restoreRight = [=](Node* head) {
		Node* prev = nullptr;
		cerr << "restoreRight> ";
		int n = 12;
		for (auto p = head; p;) {
//			TraceX(p, p->next, p->right);
			if (--n == 0) throw runtime_error("cycled");
			cerr << *p << ", ";
			p->right = unXor(p->right, prev);
//			TraceX(p->right);
			prev = p;
			p = getAddr(p->right);
		}
		cerr << endl;
	};

    restoreRight(r_tail); // set fMoved

    // revert next
    auto tail = revertNext(head);
	{
		// walk by next  (backward)
		Node* prev = nullptr;
		for (auto p = tail; p; p = getAddr(p->next)) {
			auto r = unXor(p->right, prev);
			if (isNotMoved(p) && hasFlag(r, fBackward)) {
				TraceX("Moving", *p);
				r = getAddr(r);
				auto n = new Node(p->next, p->right, -p->value);
				clearNotMoved(n);
				if (p == tail) {
					tail = n;
					log_trace << "moving tail: " << tail->dump();
				}
				if (p == r_tail) {
					r_tail = n;
					log_trace << "moving r_tail: " << r_tail->dump();
				}
//				delete p;
				setXor(r->right, n);
				if (prev) prev->next = n;
				prev = n;
				p = getAddr(n->next);
			}
		}
	}
	restoreRight(r_tail); // check all moved
    // restore
	head = revertNext(tail);
//	assert(head_ == head);
	auto r_head_ = revertRight(r_tail);
	TraceX(r_head_->dump(), head->dump());
	assert(r_head_ == head);

	return head;
}


void test(size_t test_size) {
	Node* head = genTwisted(test_size);
	TraceX(v_next(head) | to_dump);
	TraceX(v_right(head) | to_dump);

	head = moveTwisted(head);
	TraceX(v_next(head) | to_dump);
	TraceX(v_right(head) | to_dump);
}


int main() try {
	Scope _{
		[](){ log_trace_(main) << "Starting..."; },
		[](){ log_trace_(main) << "Done."; }
	};

//	test_gen();
//	test_gen2();
//	test_reverse();

	test(8);

	return 0;
}
catch (exception& e) {
	log_error << e.what();
}
