#include <iostream>

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

using namespace std;

int main() {
	std::cout << "Hello, World!" << std::endl;

	string inputPath = ".";

	for (directory_entry& entry : directory_iterator(inputPath))
	    std::cout << entry.path() << '\n';

	return 0;
}