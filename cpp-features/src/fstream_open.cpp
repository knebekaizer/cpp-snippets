#include "trace.hpp"

#include <cassert>
#include <fstream>
#include <array>

#include <thread>
#include <future>
#include <chrono>
using namespace std;

#if 0
const char* __make_mdstring(
		ios_base::openmode __mode) _NOEXCEPT {
	switch (__mode & ~ios_base::ate) {
		case ios_base::out:
		case ios_base::out | ios_base::trunc:
			return "w" _LIBCPP_FOPEN_CLOEXEC_MODE;
		case ios_base::out | ios_base::app:
		case ios_base::app:
			return "a" _LIBCPP_FOPEN_CLOEXEC_MODE;
		case ios_base::in:
			return "r" _LIBCPP_FOPEN_CLOEXEC_MODE;
		case ios_base::in | ios_base::out:
			return "r+" _LIBCPP_FOPEN_CLOEXEC_MODE;
		case ios_base::in | ios_base::out | ios_base::trunc:
			return "w+" _LIBCPP_FOPEN_CLOEXEC_MODE;
		case ios_base::in | ios_base::out | ios_base::app:
		case ios_base::in | ios_base::app:
			return "a+" _LIBCPP_FOPEN_CLOEXEC_MODE;
		case ios_base::out | ios_base::binary:
		case ios_base::out | ios_base::trunc | ios_base::binary:
			return "wb" _LIBCPP_FOPEN_CLOEXEC_MODE;
		case ios_base::out | ios_base::app | ios_base::binary:
		case ios_base::app | ios_base::binary:
			return "ab" _LIBCPP_FOPEN_CLOEXEC_MODE;
		case ios_base::in | ios_base::binary:
			return "rb" _LIBCPP_FOPEN_CLOEXEC_MODE;
		case ios_base::in | ios_base::out | ios_base::binary:
			return "r+b" _LIBCPP_FOPEN_CLOEXEC_MODE;
		case ios_base::in | ios_base::out | ios_base::trunc | ios_base::binary:
			return "w+b" _LIBCPP_FOPEN_CLOEXEC_MODE;
		case ios_base::in | ios_base::out | ios_base::app | ios_base::binary:
		case ios_base::in | ios_base::app | ios_base::binary:
			return "a+b" _LIBCPP_FOPEN_CLOEXEC_MODE;
		default:
			return nullptr;
	}
	__libcpp_unreachable();
}


using my_filebuf = basic_filebuf<char, char_traits<char>>;
int
my_filebuf_open(const char* __s, ios_base::openmode __mode)
{
	FILE* __file_ = nullptr;
	[[maybe_unused]] ios_base::openmode __om_;
	int __rt = 0;
	if (__file_ == 0)
	{
		if (const char* __mdstr = __make_mdstring(__mode)) {
//			__rt = this_;
			__file_ = fopen(__s, __mdstr);
			if (__file_) {
				__om_ = __mode;
//				std::this_thread::sleep_for(1ms);
remove(__s);
				std::this_thread::sleep_for(1ms);
//fclose(__file_);
				if (setvbuf(__file_, nullptr, _IONBF, 0) != 0) {
//				if (setvbuf(__file_, nullptr, 3, 0) != 0) {
					fclose(__file_);
					__file_ = 0;
					__rt = 1;
				} else if (__mode & ios_base::ate) {
					if (fseek(__file_, 0, SEEK_END)) {
						fclose(__file_);
						__file_ = 0;
						__rt = 2;
					}
				}
			} else
				__rt = 3;
		}
	}
	return __rt;
}

void fcreate(const char* s) {
	auto f = fopen(s, "w");
	const char buf[100] = {0};
	fwrite(buf, 1, 100, f);
	fclose(f);
}

int test_open_failure() {
	fcreate("1.txt");

//	ifstream f;
//	auto buf = f.rdbuf();
//	basic_filebuf<char, char_traits<char>> buf;
//	[[maybe_unused]] auto b = buf.open("3.txt", ios_base::in);

	array<int, 4> rc{};
	constexpr int N = 1000;
	const char* fn = "3.txt";
	for (int k = 0; k != N; ++k) {
		fcreate(fn);
		std::future<void> f1 = std::async(std::launch::async, [fn]{
			remove(fn);
		});
		std::future<int> f = std::async(std::launch::async, [fn]{
//			TraceX( std::this_thread::get_id() );
//			std::this_thread::sleep_for(1s);
			return my_filebuf_open(fn, ios_base::in | ios_base::ate);;
		});
//		log_trace << time() << " waiting for the future, f.valid() == " << f.valid();
		++rc.at(f.get());
	}
	for (auto err : rc) cout << err << endl;
	return N - rc[0];
}
#endif

int test_open_1() {
    basic_filebuf<char, char_traits<char>> fb;
    auto rc = fb.open("notExist", ios_base::in);
    if (!rc) log_error << "Failed as expected";
    return rc ? 0 : 1;
}

extern "C" int fseek(FILE *__stream, long int __off, int __whence) {
    return -1;
}

int test_open_2() {
    basic_filebuf<char, char_traits<char>> fb;
    auto rc = fb.open("good.txt", ios_base::out | ios_base::ate);
    if (!rc) log_error << "Failed as expected";
    return rc ? 0 : 1;
}

class MyBuf : public basic_filebuf<char, char_traits<char>> {
	int sync() override { return -1; }
};

int test_close_2() {
	MyBuf buf;
	buf.open("5.txt", ios_base::out);
	auto rc = buf.close();
	if (!rc) log_error << "Failed";
	return rc ? 0 : 1;
}

bool fclose_fail_once = false;
extern "C" int fclose(FILE* fp) {
    if (!fp) return 1;
    printf("my fclose\n");
    close(fp->_fileno);
    if (fclose_fail_once) {
        fclose_fail_once = false;
        log_trace << "throw std::runtime_error now";
        throw std::runtime_error("Failure injection");
    }
    return 1;
}


int test_close_3() {
    TraceF;
    basic_filebuf<char, char_traits<char>> buf;
	buf.open("5.txt", ios_base::out);
	auto rc = buf.close();
	if (!rc) log_error << "Failed";
	return rc ? 0 : 1;
}

void fcreate(const char* s) {
    auto f = fopen(s, "w");
    const char buf[100] = {0};
    fwrite(buf, 1, 100, f);
    fclose(f);
}

int test_close_4() {
    TraceF;
    {
        auto buf = new basic_filebuf<char, char_traits<char>>;
        buf->open("0.txt", ios_base::out);
        fclose_fail_once = true;
        try {
            delete buf;
        } catch (...) {
            assert(("Unexpected exception caught" && false));
        }
        assert(fclose_fail_once == false);
    }
    return 0;
}

int test_close_failure() {
	const char* fn = "4.txt";

//	ifstream f;
//	auto buf = f.rdbuf();
//	basic_filebuf<char, char_traits<char>> buf;
//	[[maybe_unused]] auto b = buf.open("3.txt", ios_base::in);

	constexpr int N = 100;
	int err = 0;
	for (int k = 0; k != N; ++k) {

		ofstream of(fn);
		auto buf = new char[1024];
		of.rdbuf()->pubsetbuf(buf, 1024);
		of << "bla-bla";
		std::future<void> f1 = std::async(std::launch::async, [&]{
//of.close();
			remove(fn);
		});

		std::future<int> f = std::async(std::launch::async, [&]{
//			TraceX( std::this_thread::get_id() );
			std::this_thread::sleep_for(1ms);
			return of.rdbuf()->close() ? 0 : 1;
		});
//		log_trace << time() << " waiting for the future, f.valid() == " << f.valid();
		if (f.get()) ++err;
	}
	return err;
}

int main() {
//	auto rc = test_open_failure();
	auto rc = test_close_failure();
//	rc = test_close_2();
	rc = test_close_3();
	rc = test_close_4();
    test_open_1();
    test_open_2();
	return rc;
}