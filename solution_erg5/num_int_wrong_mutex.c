#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

#define NTHREADS 4

struct thread_data
{
    double h;
    int from;
    int to;
};

double sum = 0.0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

double f( double a )
{
    return (4.0 / (1.0 + a*a));
}

void *calc_pi_sum(void *ptr)
{
    struct thread_data *my_data = (struct thread_data *) ptr;

    double my_h    = my_data->h;
    double my_to   = my_data->to;
    double my_from = my_data->from;

    double x;
    double local_sum = 0;
    pthread_mutex_lock(&mutex);
    for (int i = my_from; i < my_to; i++)
    {
        x = my_h * ((double) i + 0.5);
        local_sum += f(x);
    }

    sum += local_sum;
    pthread_mutex_unlock(&mutex);
}

int main(int argc, char *argv[])
{
    double PI25DT = 3.141592653589793238462643;

    if (argc != 2)
    {
        printf("Usage: %s <number of intervals>", argv[0]);
        return 1;
    }

    long int n = strtol(argv[1], NULL, 10);
    struct thread_data thread_args[NTHREADS];
    pthread_t threads[NTHREADS];
    int block = n / NTHREADS;
    double h = 1.0 / (double) n;

    struct timespec begin, end;
    clock_gettime(CLOCK_REALTIME, &begin);
    for (int i = 0; i < NTHREADS; i++)
    {
        thread_args[i].from = i * block;
        thread_args[i].to = thread_args[i] .from + block;
        if (i == NTHREADS - 1) thread_args[i].to = n;
        thread_args[i].h = h;
        pthread_create(&threads[i], NULL, calc_pi_sum, (void *) &thread_args[i]);
    }

    for (int i = 0; i < NTHREADS; i++)
        pthread_join(threads[i], NULL);

    double pi = h * sum;

    clock_gettime(CLOCK_REALTIME, &end);
    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;
    printf("pi is approximately %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
    printf("Time measured: %.6f seconds.\n", elapsed);

    return 0;    
}