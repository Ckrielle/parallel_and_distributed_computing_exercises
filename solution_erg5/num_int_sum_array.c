#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define NTHREADS 4

struct thread_data
{
    double *sums;
    double h;
    int tid;
    int from;
    int to;
};

double f( double a )
{
    return (4.0 / (1.0 + a*a));
}

void *calc_pi(void *ptr)
{
    struct thread_data *my_data = (struct thread_data *) ptr;

    double *my_sums = my_data->sums;
    double my_h = my_data->h;
    int my_tid = my_data->tid;
    int my_from = my_data->from;
    int my_to = my_data->to;

    double x;
    double local_sum = 0;
    for (int i = my_from; i < my_to; i++)
    {
        x = my_h * ((double) i + 0.5);
        local_sum += f(x);
    }

    my_sums[my_tid] = local_sum;
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <number of intervals>", argv[0]);
        return 1;
    }

    struct thread_data thread_args[NTHREADS];
    double sums[NTHREADS];
    for (int i = 0; i < NTHREADS; i++)
        sums[i] = 0.0;
    long int n = strtol(argv[1], NULL, 10);
    double h = 1.0 / (double) n;

    int block = n / NTHREADS;
    pthread_t threads[NTHREADS];

    struct timespec begin, end;
    clock_gettime(CLOCK_REALTIME, &begin);
    for (int i = 0; i < NTHREADS; i++)
    {
        thread_args[i].sums = sums;
        thread_args[i].h = h;
        thread_args[i].tid = i;
        thread_args[i].from = i * block;
        thread_args[i].to = thread_args[i] .from + block;
        if (i == NTHREADS - 1) thread_args[i].to = n;
        pthread_create(&threads[i], NULL, calc_pi, (void *) &thread_args[i]);
    }

    double sum = 0.0;
    for (int i = 0; i < NTHREADS; i++)
    {
        pthread_join(threads[i], NULL);
        sum += sums[i];
    }

    double PI25DT = 3.141592653589793238462643;
    double pi = h * sum;

    clock_gettime(CLOCK_REALTIME, &end);
    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;
    printf("pi is approximately %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
    printf("Time measured: %.6f seconds.\n", elapsed);

    return 0;
}