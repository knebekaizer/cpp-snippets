//
// Created by vdi on 12.08.24.
//

#include "trace.hpp"
#include <filesystem>
#include <string>

using namespace std;
namespace fs = std::filesystem;

void test_fs(string a0) {
    error_code ec;
    TraceX(fs::path(a0));
    TraceX(fs::path(a0).parent_path());
    TraceX(fs::absolute(fs::path(a0)));
    TraceX(fs::exists("/proc/self/exe"));
    if (fs::exists("/proc/self/exe")) {
        TraceX(fs::canonical("/proc/self/exe"));
    }

    TraceX(fs::canonical(fs::path(a0), ec), ec, ec.message());
    TraceX(fs::canonical(fs::absolute(fs::path(a0)), ec), ec, ec.message());
    TraceX(fs::absolute(fs::path(a0)).parent_path());
    TraceX(fs::canonical(fs::absolute(fs::path(a0))).parent_path());
}

int main(int argc, char const** argv) {
    test_fs(argv[0]);
    return 0;
}