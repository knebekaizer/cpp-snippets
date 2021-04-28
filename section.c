#include <stdio.h>

#define SECTION( S ) __attribute__ ((section ( S )))
void test(void) {
   printf("Hello\n");
}
void (*funcptr)(void) SECTION(".ctors") =test;
void (*funcptr2)(void) SECTION(".ctors") =test;
void (*funcptr3)(void) SECTION(".dtors") =test;

int main(void) {
	printf("Hello main>\n");
	return 0;
}