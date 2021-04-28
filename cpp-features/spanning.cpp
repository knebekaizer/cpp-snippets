//! @fiile ### Span from unique_ptr, with optional constness and initializer
#include "trace.h"

#include <memory>
#include <algorithm>
#include "gsl-lite.h"

using namespace std;
using gsl::span;
using gsl::make_span;

void spanning() {
    TraceF;
	class Foo {
	public:
	//	auto buf() { return span<std::remove_pointer<decltype(buf_)>::type>(buf_, count_); }
		auto buf() { return make_span(buf_, count_); }
		auto cbuf() { return make_span(cbuf_, count_); }

		explicit Foo(int32_t size)
			: count_(size)
			, buf_(new int64_t[size])
			, cbuf_( std::generate_n( new int64_t[size], size, [size]() { static int n = size; return --n; } ) - size )
		{
			int64_t value = 0;
			for (auto& x : buf()) x = value++;
		}
	private:
		int32_t count_;
	//	int64_t* buf_;  // works
		unique_ptr<int64_t> buf_;
		unique_ptr<int64_t const> cbuf_;
	};

	Foo foo(8);
	for (auto x : foo.buf()) cout << x << " "; cout << endl;
	for (auto x : foo.buf()) cout << x << " "; cout << endl; // unique_ptr is not destroyed by make_span
	for (auto x : foo.cbuf()) cout << x << " "; cout << endl;
//	for (auto& x : foo.cbuf()) x = 0; // this should fail
}
