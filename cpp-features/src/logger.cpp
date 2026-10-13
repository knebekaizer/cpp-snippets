#include <iostream>
#include <sstream>
#include <string>
#include "trace.hpp"

namespace my {
	extern "C" void val_log_flush(const char* buf) {
		// set breakpoint here
		printf("%s\n", buf);
	}

	class Logger {
	private:
		std::ostringstream buf_;

	public:
		// Upon destruction, store the log messages in the global buffer
		~Logger() {
			val_log_flush(buf_.str().c_str());
		}

		// Stream insertion operator to add content to the logger
		template <typename T>
			Logger& operator<<(const T& x) {
			buf_ << x;
			return *this;
		}
		template<typename _CharT, typename _Traits>
			Logger&
			operator<<(std::basic_ostream<_CharT, _Traits>& (*pf)(std::basic_ostream<_CharT, _Traits>&)) {
			buf_ << pf;
			return *this;
		}

		typedef std::ostream& (*PF)(std::ostream&);
		Logger&
		operator<<(std::ostream& (*pf)(std::ostream&)) {
			if (pf == (PF)std::endl) {  // -fpermissive ?
				log_trace << "std::endl";
			} else if (pf == (PF)std::flush) {
				log_trace << "std::flush";
			}
			buf_ << pf;
			return *this;
		}

	};

	Logger cout;
}

void test_logger() {
	my::cout << "bla-bla " << 1234 << std::endl;
	my::cout << std::flush;
}


int main()
{
	// string<const char> s; // error: multiple overloads of '_S_copy_chars'
	test_logger();
	return 0;
}