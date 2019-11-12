#include <iostream>

#include <semaphore.h>

//int sem_trywait(sem_t *sem);
//int sem_wait(sem_t *sem);


int main() {
	std::cout << "Hello, World!" << std::endl;

	sem_t sem;
	int rc = sem_wait(sem);

	std::cout << "Done." << std::endl;
	return 0;
}