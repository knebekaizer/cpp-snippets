#ifndef CPP_ALG_PRIORITYQUEUE_HPP
#define CPP_ALG_PRIORITYQUEUE_HPP

#include <vector>
#include <string>
#include <sstream>

namespace PQ {

}
enum class Ordering { LIFO = -1, NONE = 0, FIFO = 1};

template <typename T>
class PriorityQueue : std::vector<T> {
	using Base = std::vector<T>;
	using Base::empty;
	using Base::cbegin;
	using Base::cend;
public:
	auto insert(const T& v, Ordering = Ordering::NONE);
	auto insert_std(const T& v, Ordering = Ordering::NONE);
	auto begin() const { return Base::begin(); }
	auto end() const { return Base::end(); }

	std::string dump() const {
		std::ostringstream out;
		for (const auto& x : *this) out << x << " ";
		return out.str();
	}
private:
	auto bin_search(const T& v) const;
	auto lower_bound(const T& v) const;
	auto upper_bound(const T& v) const;
};

template <typename T>
auto PriorityQueue<T>::bin_search(const T& v) const {
	if (empty()) return cend();

	auto lo = cbegin();
	if (v < *lo) return lo; // if (v < *lo) return lo;
	auto hi = cend() - 1;
	if (*hi < v) return cend();
	while (hi - lo > 1) {
		auto mid = lo + (hi - lo)/2;
		if (v < *mid) {
			hi = mid;
		} else if (*mid < v) {
			lo = mid;
		} else {
			return mid;
		}
	}
	return hi;
}

template <typename T>
auto PriorityQueue<T>::lower_bound(const T& v) const {
	if (empty()) return cend();

//	auto equal = [](const auto& a, const auto& b) { return !(a < b) && !(b < a); };
	auto lo = cbegin();
	if (!(*lo < v)) return lo;
	auto hi = cend() - 1;
	if (*hi < v) return cend();
	while (hi - lo > 1) {
		auto mid = lo + (hi - lo)/2;
		if (*mid < v) {
			lo = mid;
		} else {
			hi = mid;
		}
	}
	return hi;
}

template <typename T>
auto PriorityQueue<T>::upper_bound(const T& v) const {
	if (empty()) return cend();

	auto lo = cbegin();
	if (v < *lo) return lo;
	auto hi = cend() - 1;
	if (*hi < v) return cend();
	while (hi - lo > 1) {
		auto mid = lo + (hi - lo)/2;
		if (v < *mid) {
			hi = mid;
		} else {
			lo = mid;
		}
	}
	return hi;
}

template <typename T>
auto PriorityQueue<T>::insert(const T& v, Ordering ord) {
	auto it = ord == Ordering::NONE ? bin_search(v)
			: (ord == Ordering::FIFO ? lower_bound(v)
			: upper_bound(v));
	Base::insert(it, v);
}

template <typename T>
auto PriorityQueue<T>::insert_std(const T& v, Ordering ord) {
	// lower_bound -> FIFO for equal keys; upper_bound -> LIFO
	if (ord == Ordering::NONE) {
		auto [lo, hi] = std::equal_range(cbegin(), cend(), v);
		Base::insert(lo + (hi - lo)/2, v);
	} else if (ord == Ordering::FIFO)
		Base::insert(std::lower_bound(cbegin(), cend(), v), v);
	else
		Base::insert(std::upper_bound(cbegin(), cend(), v), v);
}

#endif //CPP_ALG_PRIORITYQUEUE_HPP
