//
// Created by vdi on 22.12.23.
//

#include <iostream>
#include <locale>

std::locale previousLoc;

class CustomStream : public std::basic_streambuf<char16_t> {
public:
    std::locale pubimbue(const std::locale& loc) {
        // Previous Locale
        std::locale previousLoc = this->getloc();
        std::cout << "Previous locale: " << previousLoc.name() << std::endl;

        // Return pubimbue
        return std::cout.rdbuf()->pubimbue(loc);
    }
};

int main() {
    CustomStream customStream;

    // New Locale
    std::locale newLoc = customStream.pubimbue(std::locale("en_US.UTF-8"));
    std::locale currentLocale = customStream.getloc();
    std::cout << "New locale: " << currentLocale.name() << std::endl;

    if (previousLoc != currentLocale ){
        std::cout<<"Passed";
    }

    return 0;
}