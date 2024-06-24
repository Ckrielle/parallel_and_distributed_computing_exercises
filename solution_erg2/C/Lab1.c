#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 4

int num; // global var | pass to function pointer that returns val to main
int *arr; // global var | pass-by-global

typedef struct
{
	int threadID; // struct member
	int threadN; // struct member
} thread_data;

void *counter_thread(void *ptr) // reference parameter
{
	int tID; // thread function var
	int *tN; // thread function var | pointer to threadN struct member

	thread_data *thread_args = (thread_data*) ptr;
	tID = thread_args->threadID; // value parameter
	tN = &thread_args->threadN;  // reference parameter 

	*tN = *tN + 1 + tID;
	arr[tID] = arr[tID] + 1 + tID;
	printf("Thread %d num = %d arr[%d] = %d\n", tID, *tN, tID, arr[tID]);
}

int main()
{
	int i; // main local var | pass-by-value
        pthread_t threads[NTHREADS]; // main local var | not passed
	thread_data thread_args[NTHREADS]; // main local var | pass-by-reference

	num = 0;
	arr = (int*) malloc(NTHREADS * sizeof(int));
	for (i = 0; i < NTHREADS; i++)
		arr[i] = 0;

	for (i = 0; i < NTHREADS; i++)
	{
		thread_args[i].threadID = i;
		thread_args[i].threadN  = num;
		pthread_create(&threads[i], NULL, counter_thread, (void*) &thread_args[i]);
	}

	for (i = 0; i < NTHREADS; i++)
	{
		pthread_join(threads[i], NULL);
		num += thread_args[i].threadN;
	}

	for (i = 0; i < NTHREADS; i++)
		printf("arr[%d] = %d\n", i, arr[i]);
	printf("num = %d\n", num);

        return 0;
}
