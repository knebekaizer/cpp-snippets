#include <iostream>
#include <sys/types.h>
#include <md5.h>

#define MAX_STR 100

using namespace std;
typedef unsigned short int usi;

int main(int argc, char* argv[])
{
	char Str[MAX_STR];
	char const* s; 
	if (argc > 1) {
		s = argv[1];
	} else {
		s = Str;
        cout << "Enter a string to md5:" << endl;
        cin.getline(Str, MAX_STR);
	}

	char Result[33];
	MD5Data(s, strlen(s), Result);
	cout << Result << endl;

	return 0;
}
