#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 10
#define NTHREADS 4

struct thread_data
{
   double **a;
   double **b;
   double **c;
   int	from;
   int	to;
};

void *add_matrices(void *ptr)
{
    struct thread_data *my_data = (struct thread_data *) ptr;

    double **my_a = my_data->a;
    double **my_b = my_data->b;
    double **my_c = my_data->c;
    int my_from  = my_data->from;
    int my_to    = my_data->to;

    for (int i = my_from; i < my_to; i++)
        for (int j = 0; j < SIZE; j++)
            my_a[i][j] = my_b[i][j] + my_c[i][j];

    pthread_exit(NULL);
}

int main()
{
    int i, j;

    //double a[SIZE][SIZE];
    //double b[SIZE][SIZE];
    //double c[SIZE][SIZE];
    double **a = malloc(SIZE * sizeof(double*));
    double **b = malloc(SIZE * sizeof(double*));
    double **c = malloc(SIZE * sizeof(double*));
    for (i = 0; i < SIZE; i++)
    {
        a[i] = malloc(SIZE * sizeof(double));
        b[i] = malloc(SIZE * sizeof(double));
        c[i] = malloc(SIZE * sizeof(double));
        for (j = 0; j < SIZE; j++)
        {
            a[i][j] = 0.1;
		    b[i][j] = 0.3;
            c[i][j] = 0.5;
        }
    }

    /* begin time */
   struct timespec begin, end; 
   clock_gettime(CLOCK_REALTIME, &begin);
	
    pthread_t threads[NTHREADS];
    struct thread_data thread_args[NTHREADS];

    int block = SIZE / NTHREADS;
    for (i = 0; i < NTHREADS; i++)
    {
        thread_args[i].from = i * block;
        thread_args[i].to = thread_args[i].from + block;
        if (i == NTHREADS - 1) thread_args[i].to = SIZE;
        thread_args[i].a = a;
        thread_args[i].b = b;
        thread_args[i].c = c;
        pthread_create(&threads[i], NULL, add_matrices, (void *) &thread_args[i]);
    }

    for (i = 0; i < NTHREADS; i++)
        pthread_join(threads[i], NULL);

    /* end time */
    clock_gettime(CLOCK_REALTIME, &end);
    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;

    // for debugging 
    for(i = 0; i < SIZE; i++)
    {
        for(j = 0; j < SIZE; j++) 
            printf("%g ", a[i][j]); 
        printf("\n");
    }
    printf("\n");
    printf("Time measured: %.3f seconds.\n", elapsed);
    
    for (i = 0; i < SIZE; i++)
    {
        free(a[i]);
        a[i] = NULL;
        free(b[i]);
        b[i] = NULL;
        free(c[i]);
        c[i] = NULL;
    }
    free(a);
    a = NULL;
    free(b);
    b = NULL;
    free(c);
    c = NULL;
}