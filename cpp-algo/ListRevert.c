#include <stdio.h>

struct List;

typedef int Payload;
typedef struct List {
	Payload x;
	struct List* next;
} List;

/*
 * In this function there are 2 lines where pointer used to access the memory
 * prev, tmp and next are merely used as integral type variables
 */
List* revertList(List* head) {
	List* prev = (List*)0;
	List* cur = head;
	while (cur) {
		List *tmp = cur->next;
		cur->next = prev;
		prev = cur;
		cur = tmp;
	}
	return prev;
}

#define N 4u
List* createList() {
	static List a[N];
	for (unsigned i = 0; i < N; ++i) {
		a[i].x = i + 1;
		a[i].next = i ? &a[i - 1] : 0;
	}
	return &a[N - 1];
}

void printList(List* p, int n) {
	for (; n-->0; p = p->next) {
		printf("%d ", p->x);
	}
	printf("\n");
}

int main() {
	List* head = createList();
	printList(head, N);
	head = revertList(head);
	printList(head, N);
}
