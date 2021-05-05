//
// Created by Vladimir Ivanov on 01.05.2021.
//

#ifndef CPP_FEATURES_SCOPED_HPP
#define CPP_FEATURES_SCOPED_HPP

/// Scope-based init / teardown
template <class Exit, class Enter = Exit > class Scoped {
	const Exit atExit_;
	Scoped(const Scoped&) = delete;
	Scoped(Scoped&&) = delete;
	Scoped& operator=(const Scoped& other) = delete;
	Scoped& operator=(Scoped&&) = delete;
public:
	explicit Scoped(const Exit& teardown) : atExit_(teardown) {}
	Scoped(const Enter& init, const Exit& teardown) : Scoped(teardown) { init(); }
	~Scoped() { atExit_(); }
};

template <class F2> Scoped(F2) -> Scoped<F2, F2>;
template <class F1, class F2> Scoped(F1, F2) -> Scoped<F2, F1>;

#endif //CPP_FEATURES_SCOPED_HPP
