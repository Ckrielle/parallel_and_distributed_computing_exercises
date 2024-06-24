#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 4

typedef struct
{
	int threadID; // struct member
	int threadN;  // struct member
} thread_data;

void *counter_thread(void *ptr)
{
	int tID; // thread function var
	int *tN; // thread function var | pointer to threadN struct member

	thread_data *thread_args = (thread_data*) ptr;
	tID = thread_args->threadID; // value parameter
	tN  = &thread_args->threadN; // reference parameter

	*tN += 1 + tID;
	printf("Thread %d n = %d\n", tID, *tN);
}

int main()
{
	int i; // local main var | pass-by-value
	int n; // local main var | pass to instance variable that returns val to main
        pthread_t threads[NTHREADS]; // local main var | not passed
	thread_data thread_args[NTHREADS]; // local main var | pass-by-reference

	n = 0;
	for (i = 0; i < NTHREADS; i++)
	{
		thread_args[i].threadID = i;
		thread_args[i].threadN  = n;
		pthread_create(&threads[i], NULL, counter_thread, (void*) &thread_args[i]);
	}

	for (i = 0; i < NTHREADS; i++)
	{
		pthread_join(threads[i], NULL);
		n += thread_args[i].threadN;
	}

	printf("Main n = %d\n", n);

        return 0;
}
