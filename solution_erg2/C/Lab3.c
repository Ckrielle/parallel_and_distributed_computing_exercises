#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 4

typedef struct
{
	int n;  // struct member
	int *a; // struct member
} shared_counter;

typedef struct
{
	int threadID; // struct member
	int threadN;  // struct member
	shared_counter threadCount; // struct member
} thread_data;

void *counter_thread(void *ptr)
{
	int tID; // thread function var
	int *tN; // thread function var | pointer to threadN struct member
	shared_counter *cnt; // thread function var | pointer to threadCount struct member (global in the context of every thread)

	thread_data *thread_args = (thread_data*) ptr;
	tID = thread_args->threadID; // value parameter
	tN  = &thread_args->threadN; // reference parameter
	cnt = &thread_args->threadCount; // reference parameter

	*tN += 1 + tID;
	cnt->a[tID] += 1 + tID;
	printf("Thread %d n = %d a[%d] = %d\n", tID, *tN, tID, cnt->a[tID]);
}

int main()
{
	int i; // main local var | pass-by-value
	shared_counter count; // main local var | pass-by-reference
        pthread_t threads[NTHREADS]; // main local var | not passed
	thread_data thread_args[NTHREADS]; // main local var | pass-by-reference
	
	count.n = 0; // pass to function pointer that returns val to main
	count.a = (int*) malloc(NTHREADS * sizeof(int)); // pass to function pointer that returns val to main
	for (i = 0; i < NTHREADS; i++)
		count.a[i] = 0;

	for (i = 0; i < NTHREADS; i++)
	{
		thread_args[i].threadID    = i;
		thread_args[i].threadCount = count;
		thread_args[i].threadN     = count.n;
		pthread_create(&threads[i], NULL, counter_thread, (void*) &thread_args[i]);
	}

	for (i = 0; i < NTHREADS; i++)
	{
		pthread_join(threads[i], NULL);
		count.n += thread_args[i].threadN;
	}

	for (i = 0; i < NTHREADS; i++)
		printf("a[%d] = %d\n", i, count.a[i]);
	printf("n = %d\n", count.n);

        return 0;
}
