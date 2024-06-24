#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NTHREADS 4

struct thread_data
{
    unsigned char *arr;
    int n;
    int rank;
};

void *sieve(void *ptr)
{
    struct thread_data *my_data = (struct thread_data *) ptr;
    
    unsigned char *my_arr = my_data->arr;
    int my_n = my_data->n;
    int my_rank = my_data->rank;

    for (int i = my_rank; i <= my_n; i += NTHREADS)
        if (my_arr[i])
            for (int j = i; (i * j) <= my_n; j++)
                my_arr[(i * j)] = 0;

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("usage: %s <upper prime limit>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);
    unsigned char *array = (unsigned char *)malloc((size + 1) * sizeof(unsigned char));
    for(int i = 2; i <= size; i++)
        array[i] = 1;

    struct thread_data thread_args[NTHREADS];
    pthread_t threads[NTHREADS];
    
    struct timespec begin, end;
    clock_gettime(CLOCK_REALTIME, &begin);

    for (int i = 0; i < NTHREADS; i++)
    {
        thread_args[i].arr = array;
        thread_args[i].n = size;
        thread_args[i].rank = i + 2;
        pthread_create(&threads[i], NULL, sieve, (void *)&thread_args[i]);
    }
    
    for (int i = 0; i < NTHREADS; i++)
        pthread_join(threads[i], NULL);

    clock_gettime(CLOCK_REALTIME, &end);
    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;
    printf("Prime numbers from 1 to %d are: ", size);
    for(int i = 2; i <= size; i++)
        if (array[i])
            printf("%d ", i);
    printf("\nTime measured: %.6f seconds.\n", elapsed);

    free(array);
    array = NULL;

    return 0;
}
