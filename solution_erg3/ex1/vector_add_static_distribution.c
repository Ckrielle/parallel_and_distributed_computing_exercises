#include <pthread.h>
#include <stdio.h>
#include <time.h>

#define SIZE 1000
#define NTHREADS 4

struct thread_data
{
   double *a;
   double *b;
   double *c;
   int	from;
   int	to;
};

void *add_vectors(void *ptr)
{
    struct thread_data *my_data = (struct thread_data *) ptr;

    double *my_a = my_data->a;
    double *my_b = my_data->b;
    double *my_c = my_data->c;
    int my_from  = my_data->from;
    int my_to    = my_data->to;

    for (int i = my_from; i < my_to; i++)
        my_a[i] = my_b[i] + my_c[i];

    pthread_exit(NULL);
}

int main()
{
    int i;

    double a[SIZE];
    double b[SIZE];
    double c[SIZE];
    for(i = 0; i < SIZE; i++) {
        a[i] = 0.0;
		b[i] = 1.0;
        c[i] = 0.5;
    }

    /* begin time */
   struct timespec begin, end; 
   clock_gettime(CLOCK_REALTIME, &begin);
	
    pthread_t threads[NTHREADS];
    struct thread_data thread_args[NTHREADS];

    int block = (int) SIZE / NTHREADS;
    for (i = 0; i < NTHREADS; i++)
    {
        thread_args[i].from = i * block;
        thread_args[i].to = thread_args[i].from + block;
        if (i == SIZE - 1) thread_args[i].to = SIZE;
        thread_args[i].a = a;
        thread_args[i].b = b;
        thread_args[i].c = c;
        pthread_create(&threads[i], NULL, add_vectors, (void *) &thread_args[i]);
    }

    for (i = 0; i < NTHREADS; i++)
        pthread_join(threads[i], NULL);

    /* end time */
    clock_gettime(CLOCK_REALTIME, &end);
    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;

    for(i = 0; i < SIZE; i++)
        printf("%g ", a[i]);
    printf("\n");
    printf("Time measured: %.3f seconds.\n", elapsed);
}