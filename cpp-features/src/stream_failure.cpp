//
// Created by Vladimir on 2023-11-13.
//

/// @file Test failbit condition on ostream

#include <ios>
#include <sstream>
//#include <format>

#include "trace.hpp"

using namespace std;

#if 0
template <class _CharT, class _Traits, class _Allocator>
class my_stringbuf
		: public basic_streambuf<_CharT, _Traits>
{
public:
	typedef _CharT                         char_type;
	typedef _Traits                        traits_type;
	typedef typename traits_type::int_type int_type;
	typedef typename traits_type::pos_type pos_type;
	typedef typename traits_type::off_type off_type;
	typedef _Allocator                     allocator_type;

	typedef basic_string<char_type, traits_type, allocator_type> string_type;

private:

	string_type __str_;
	mutable char_type* __hm_;
	ios_base::openmode __mode_;

public:
	// 30.8.2.1 [stringbuf.cons], constructors
	_LIBCPP_INLINE_VISIBILITY
	basic_stringbuf()
			: __hm_(nullptr), __mode_(ios_base::in | ios_base::out) {}

	_LIBCPP_INLINE_VISIBILITY
	explicit basic_stringbuf(ios_base::openmode __wch)
			: __hm_(nullptr), __mode_(__wch) {}

	_LIBCPP_INLINE_VISIBILITY
	explicit basic_stringbuf(const string_type& __s,
	                         ios_base::openmode __wch = ios_base::in | ios_base::out)
			: __str_(__s.get_allocator()), __hm_(nullptr), __mode_(__wch)
	{
		str(__s);
	}

	basic_stringbuf(basic_stringbuf&& __rhs);

	// 27.8.1.2 Assign and swap:
	basic_stringbuf& operator=(basic_stringbuf&& __rhs);
	void swap(basic_stringbuf& __rhs);

	// 27.8.1.3 Get and set:
	string_type str() const;
	void str(const string_type& __s);

protected:
	// 27.8.1.4 Overridden virtual functions:
	virtual int_type underflow();
	virtual int_type pbackfail(int_type __c = traits_type::eof());
	virtual int_type overflow (int_type __c = traits_type::eof());
	virtual pos_type seekoff(off_type __off, ios_base::seekdir __way,
	                         ios_base::openmode __wch = ios_base::in | ios_base::out);
	_LIBCPP_INLINE_VISIBILITY
	virtual pos_type seekpos(pos_type __sp,
	                         ios_base::openmode __wch = ios_base::in | ios_base::out) {
		return seekoff(__sp, ios_base::beg, __wch);
	}
};

//template <class _CharT, class _Traits = char_traits<_CharT>,
//		class _Allocator = allocator<_CharT> >
class my_ostringstream
		: public basic_ostream<char, char_traits<char>>
{
public:
	using _CharT = char;
	using _Allocator = allocator<_CharT>;
	using _Traits = char_traits<char>;
	typedef _CharT                         char_type;
	typedef _Traits                        traits_type;
	typedef typename traits_type::int_type int_type;
	typedef typename traits_type::pos_type pos_type;
	typedef typename traits_type::off_type off_type;
	typedef _Allocator                     allocator_type;

	typedef basic_string<char_type, traits_type, allocator_type> string_type;

private:
	my_stringbuf __sb_;

public:
	// 30.8.4.1 [ostringstream.cons], constructors
	_LIBCPP_INLINE_VISIBILITY
	basic_ostringstream()
			: basic_ostream<_CharT, _Traits>(&__sb_), __sb_(ios_base::out) {}

	_LIBCPP_INLINE_VISIBILITY
	explicit basic_ostringstream(ios_base::openmode __wch)
			: basic_ostream<_CharT, _Traits>(&__sb_), __sb_(__wch | ios_base::out) {}

	_LIBCPP_INLINE_VISIBILITY
	explicit basic_ostringstream(const string_type& __s,
	                             ios_base::openmode __wch = ios_base::out)
			: basic_ostream<_CharT, _Traits>(&__sb_)
			  , __sb_(__s, __wch | ios_base::out)
	{ }

	_LIBCPP_INLINE_VISIBILITY
	basic_ostringstream(basic_ostringstream&& __rhs)
			: basic_ostream<_CharT, _Traits>(_VSTD::move(__rhs))
			  , __sb_(_VSTD::move(__rhs.__sb_))
	{
		basic_ostream<_CharT, _Traits>::set_rdbuf(&__sb_);
	}

	// 27.8.2.2 Assign and swap:
	basic_ostringstream& operator=(basic_ostringstream&& __rhs) {
		basic_ostream<char_type, traits_type>::operator=(_VSTD::move(__rhs));
		__sb_ = _VSTD::move(__rhs.__sb_);
		return *this;
	}

	_LIBCPP_INLINE_VISIBILITY
	void swap(basic_ostringstream& __rhs) {
		basic_ostream<char_type, traits_type>::swap(__rhs);
		__sb_.swap(__rhs.__sb_);
	}

	// 27.8.2.3 Members:
	_LIBCPP_INLINE_VISIBILITY
	basic_stringbuf<char_type, traits_type, allocator_type>* rdbuf() const {
		return const_cast<basic_stringbuf<char_type, traits_type, allocator_type>*>(&__sb_);
	}
	_LIBCPP_INLINE_VISIBILITY
	string_type str() const {
		return __sb_.str();
	}
	_LIBCPP_INLINE_VISIBILITY
	void str(const string_type& __s) {
		__sb_.str(__s);
	}
};
#endif



void test_out() {
	ostringstream oss;
	oss << "0123456789";
	TraceX(oss.str());

	TraceX((int) oss.rdstate());
	oss.seekp(5);
	oss << "qwe";
	TraceX(oss.str());

	try {
		oss.seekp(12);
		oss << 42;
		TraceX(1, bitset<8>(oss.rdstate()));
	} catch (ios::failure& e) {
		TraceX(11, bitset<8>(oss.rdstate()));
		TraceX(11, e.what(), e.code());
	} catch (std::exception& e) {
		TraceX(12, bitset<8>(oss.rdstate()));
		TraceX(12, e.what());
	}

	try {
		oss.clear();
		oss.exceptions(ios::failbit | ios::badbit);
		oss.exceptions(ios::failbit);
		oss.seekp(12);
		TraceX(2, bitset<8>(oss.rdstate()));
	} catch (std::exception& e) {
		TraceX(2, bitset<8>(oss.rdstate()), e.what());
	}
//	TraceX(std::format("{:b}", oss.rdstate()));
}

void test_in() {
	istringstream iss{"qwertyuiop"};
	TraceX(iss.str());

	TraceX((int) iss.rdstate());
	int dec;

	try {
		iss >> dec;
		TraceX(1, bitset<8>(iss.rdstate()));
	} catch (std::exception& e) {
		TraceX((1, e.what()));
	}

	try {
		iss.clear();
		iss.exceptions(ios::failbit);
		iss >> dec;
		TraceX(2, bitset<8>(iss.rdstate()));
	} catch (std::exception& e) {
		TraceX(2, bitset<8>(iss.rdstate()), e.what());
//		try {
//			iss >> dec;
//		} catch (std::exception& e) {
//			TraceX(3, bitset<8>(iss.rdstate()), e.what());
//		}
	}

	try {
		iss.clear();
//		iss.exceptions(ios::failbit | ios::badbit);
log_trace << "3: expecting badbit exception";
		iss.exceptions(ios::badbit);
		iss >> dec;
		log_trace << "3: " << "Not thrown; failbit: " << iss.fail() << "; badbit: " << iss.bad();
		TraceX(3, bitset<8>(iss.rdstate()));
	} catch (std::exception& e) {
		TraceX(3, bitset<8>(iss.rdstate()), e.what());
//		try {
//			iss >> dec;
//		} catch (std::exception& e) {
//			TraceX(3, bitset<8>(iss.rdstate()), e.what());
//		}
	}

	istringstream is2;

	try {
		log_trace << "4: Reading from empty stream will cause badbit state";
//		is2.exceptions(ios::failbit | ios::badbit);
		is2.exceptions(ios::failbit);
		is2 >> dec;
		TraceX(4, bitset<8>(is2.rdstate()));
	} catch (std::ios_base::failure& e) {
		TraceX(4, bitset<8>(is2.rdstate()), e.what(), e.code());
		try {
			is2 >> dec;
		} catch (std::exception& e) {
			TraceX(5, bitset<8>(is2.rdstate()), e.what());
		}
	}
//	TraceX(std::format("{:b}", iss.rdstate()));
}

int main() {
//	test_0();
//	test_out();
	test_in();

	return 0;
}