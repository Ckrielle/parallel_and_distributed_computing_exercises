#include <stdio.h>
#include <pthread.h>

void *print_thread1(void *ptr)
{
	printf("We are inside thread 1\n");
}

void *print_thread2(void *ptr)
{
	printf("We are inside thread 2\n");
}

int main()
{
	pthread_t thread1, thread2;

	pthread_create(&thread1, NULL, print_thread1, NULL);
	pthread_create(&thread2, NULL, print_thread2, NULL);
	
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	return 0;
}
