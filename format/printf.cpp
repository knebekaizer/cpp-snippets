#include <stdio.h>

int main() {
	
	printf("42WithPadding%03d\n", 42);
	printf("42WithPadding%.3d\n", 42);
	printf("addr = %p\n", main);
	printf("addr = %#lx\n", (unsigned long)main);
	
	int n = 0;
	printf("%d %d %e Now %n%d bytes written\n", 99, 112, 2.7182818284590, &n, n);
	printf("%d %d %e Now %n%d bytes written\n", 99, 112, 2.7182818284590, &n, n);
	
	return 0;
}
