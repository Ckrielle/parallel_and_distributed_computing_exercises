#include <stdio.h>
#include <pthread.h>

#define NTHREADS 10

void *multiples_of_num(void *num)
{
	int n = (unsigned long int) num + 1;

	for (int i = 0; i < 20; i++)
		printf("%d * %d  = %d\n", i, n, i * n);
}

int main()
{
	pthread_t threads[NTHREADS];
	int i;

	for (i = 0; i < NTHREADS; i++)
		pthread_create(&threads[i], NULL, multiples_of_num, (void *)(unsigned long int) i);

	for (i = 0; i < NTHREADS; i++)
		pthread_join(threads[i], NULL);

	return 0;
}
