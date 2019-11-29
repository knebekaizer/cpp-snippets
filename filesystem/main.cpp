#include <iostream>

#define DEF_LOG_LEVEL (LOG_LEVEL::trace)
#include "trace.h"

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

using namespace std;

int main() {

	string inputPath = ".";

	for (const directory_entry& entry : recursive_directory_iterator(inputPath)) {
		log_trace << entry.path().filename();
		log_trace << entry.path() << ":" << entry.status().type();
	}

	return 0;
}