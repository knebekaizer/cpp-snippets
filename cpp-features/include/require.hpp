//
// Created by Vladimir on 2025-01-17.
//

#pragma once

#include <string>
#include <stdexcept>

// `require` support ***************************************************************
/// usage: 	require(exc_type, condition) << "Bla-bla: " << whatever;
/// throws an exception of type exc_type with corresponding what() and where() text
namespace err {

class tr_error : public std::runtime_error {
public:
	explicit tr_error(const std::string& what, std::string from = "")
			: runtime_error(what)
			  , where_(std::move(from)) {}
	virtual std::string const& where() const { return where_; }

protected:
	const std::string where_;
};

template <class error = tr_error>
class require_ {
public:
	require_(const std::string& where = "")
			: where_(where) {}

	template <typename T>
	require_& operator<<(const T& x) {
		os_ << x;
		return *this;
	}
	explicit operator bool() const { return static_cast<bool>(os_); }

	~require_() noexcept(false) {
		if (!std::uncaught_exceptions())
			throw error(os_.str(), where_);
	}
private:
	std::string where_;
	mutable std::ostringstream os_;
};
} // namespace err

#define FILE_AND_LINE (std::string(__FILE__ ":") +  std::to_string(__LINE__))
//#define STACKTRACE (FILE_AND_LINE + "\n" + Backtrace())
// No backtrace available
#define STACKTRACE (FILE_AND_LINE)

#define require_1(cond) (cond) || (err::require_<>(STACKTRACE) << "Condition failed: "#cond"\n" \
<< "at line " __FILE__":" << std::to_string(__LINE__) + "\n")

#define require_2(err_type, cond) (cond) || (err::require_<err_type>(STACKTRACE) << "Condition failed: "#cond"\n" \
<< "at line " __FILE__":" << std::to_string(__LINE__) + "\n")

#define get_require_override(_1, _2, NAME, ...) NAME

// Define a macro that concats either 3 or 2 strings together.
#define require(...) get_require_override(__VA_ARGS__, \
    require_2, require_1)(__VA_ARGS__)
// ^^^ End of `require` macro *********************************************************

