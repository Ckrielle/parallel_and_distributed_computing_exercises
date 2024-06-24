#include <stdio.h>
#include <pthread.h>

#define NTHREADS 10

// experimented with passing arguments to thread functions

void *print_thread1(void *ptr)
{
	int t_id = (unsigned long int) ptr;
	printf("We are inside thread 1 with thread id %d\n", t_id);
}

void *print_thread2(void *ptr)
{
	int t_id = (unsigned long int) ptr;
	printf("We are inside thread 2 with thread id %d\n", t_id);
}

int main()
{
	pthread_t thread1_arr[NTHREADS], thread2_arr[NTHREADS];
	int i;

	for (i = 0; i < NTHREADS; i++)
	{
		pthread_create(&thread1_arr[i], NULL, print_thread1, (void *)(unsigned long int) i);
		pthread_create(&thread2_arr[i], NULL, print_thread2, (void *)(unsigned long int) i);
	}
	
	for (i = 0; i < NTHREADS; i++)
	{
		pthread_join(thread1_arr[i], NULL);
		pthread_join(thread2_arr[i], NULL);
	}

	return 0;
}
