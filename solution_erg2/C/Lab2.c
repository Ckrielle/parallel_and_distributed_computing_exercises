#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 4

typedef struct
{
	int threadID; // struct member
	int n;        // struct member
	int *a;       // struct member
} thread_data;

void *counter_thread(void *ptr) // reference parameter
{
	int tID; // thread function var
	int n;   // thread function var
	int *a;  // thread function var | pointer to struct member a

	thread_data *thread_args = (thread_data*) ptr;
	tID = thread_args->threadID; // value parameter
	n   = thread_args->n; // value parameter
	a   = thread_args->a; // reference parameter

	n += tID;
	a[tID] = a[tID] + n;
	printf("Thread %d my a = %d\n", tID, a[tID]);
}

int main()
{
	int i; // main local var | pass-by-value
	int n; // main local var | pass-by-value
	int a[NTHREADS]; // main local var | pass-by-reference
        pthread_t threads[NTHREADS]; // main local var | not passed
	thread_data thread_args[NTHREADS]; // main local var | pass-by-reference

	n = 0;
	for (i = 0; i < NTHREADS; i++)
		a[i] = 0;

	for (i = 0; i < NTHREADS; i++)
	{
		thread_args[i].threadID = i;
		thread_args[i].n = n;
		thread_args[i].a = a;
		pthread_create(&threads[i], NULL, counter_thread, (void*) &thread_args[i]);
	}

	for (i = 0; i < NTHREADS; i++)
		pthread_join(threads[i], NULL);

	printf("n = %d\n", n);
	for (i = 0; i < NTHREADS; i++)
		printf("a[%d] = %d\n", i, a[i]);

        return 0;
}
