#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#define LIMIT 128 

struct thread_data
{
    double h;
    double result;
    int from;
    int to;
};

double f(double a)
{
    return 4.0 / (1.0 + a * a);
}

void *calc_pi_recursive(void *ptr) {
    struct thread_data *my_data = (struct thread_data *) ptr;
    
    double my_h = my_data->h;
    double *my_result = &my_data->result;
    int my_from = my_data->from;
    int my_to = my_data->to;

    int range = my_to - my_from;
    if (range <= LIMIT)
    {
        double local_sum = 0.0;
        for (int i = my_from; i < my_to; i++)
        {
            double x = my_h * (i + 0.5);
            local_sum += f(x);
        }
        *my_result = local_sum;
    }
    else
    {
        int mid = my_from + range / 2;

        struct thread_data left_data;
        left_data.h       = my_h;
        left_data.result  = 0.0;
        left_data.from    = my_from;
        left_data.to      = mid;
        struct thread_data right_data;
        right_data.h      = my_h;
        right_data.result = 0.0;
        right_data.from   = mid;
        right_data.to     = my_to;

        pthread_t thread_left, thread_right;
        pthread_create(&thread_left, NULL, calc_pi_recursive, (void *) &left_data);
        pthread_create(&thread_right, NULL, calc_pi_recursive, (void *) &right_data);

        pthread_join(thread_left, NULL);
        pthread_join(thread_right, NULL);

        *my_result = left_data.result + right_data.result;
    }
    
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <number of intervals>\n", argv[0]);
        return 1;
    }

    long int n = strtol(argv[1], NULL, 10);
    double h = 1.0 / n;
    struct thread_data thread_args;
    thread_args.h      = h;
    thread_args.result = 0.0;
    thread_args.from   = 0;
    thread_args.to     = n;
    
    pthread_t thread;
    
    struct timespec begin, end;
    clock_gettime(CLOCK_REALTIME, &begin);
    
    pthread_create(&thread, NULL, calc_pi_recursive, (void *) &thread_args);
    pthread_join(thread, NULL);

    double pi = h * thread_args.result;

    clock_gettime(CLOCK_REALTIME, &end);
    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;

    double PI25DT = 3.141592653589793238462643;
    printf("pi is approximately %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));

    printf("Time measured: %.6f seconds.\n", elapsed);

    return 0;
}
