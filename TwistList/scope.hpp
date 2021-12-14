#ifndef TWISTLIST_SCOPE_HPP
#define TWISTLIST_SCOPE_HPP

/// Scope-based init / teardown
template <class Exit, class Enter = Exit > class Scope {
	const Exit atExit_;
	Scope(const Scope&) = delete;
	Scope(Scope&&) = delete;
	Scope& operator=(const Scope& other) = delete;
	Scope& operator=(Scope&&) = delete;
public:
	explicit Scope(const Exit& teardown) : atExit_(teardown) {}
	Scope(const Enter& init, const Exit& teardown) : Scope(teardown) { init(); }
	~Scope() { atExit_(); }
};

template <class F2> Scope(F2) -> Scope<F2, F2>;
template <class F1, class F2> Scope(F1, F2) -> Scope<F2, F1>;

#endif //TWISTLIST_SCOPE_HPP
