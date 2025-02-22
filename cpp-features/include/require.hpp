// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2007 - 2025 Vladimir Ivanov <knebekaizer@gmail.com>.
//
// Permission is hereby  granted, free of charge, to any  person obtaining a copy
// of this software and associated  documentation files (the "Software"), to deal
// in the Software  without restriction, including without  limitation the rights
// to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
// copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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

