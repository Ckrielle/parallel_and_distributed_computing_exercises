#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#define NTHREADS 4

int total_tasks;
int task_assigned = 2;
pthread_mutex_t mutex;

struct thread_data
{
    unsigned char *arr;
    int n;
};

int get_next_task()
{
    pthread_mutex_lock(&mutex);
    int next_task = task_assigned++;
    pthread_mutex_unlock(&mutex);
    return next_task;
}

void *sieve(void *ptr)
{
    struct thread_data *my_data = (struct thread_data *) ptr;
    unsigned char *my_arr = my_data->arr;
    int my_n = my_data->n; 

    int task;
    while ((task = get_next_task()) <= my_n)
        if (my_arr[task])
            for (int j = task * task; j <= my_n; j += task) 
                my_arr[j] = 0;

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <upper prime limit>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);
    unsigned char *array = (unsigned char *)malloc((size + 1) * sizeof(unsigned char));
    for (int i = 2; i <= size; i++)
        array[i] = 1;

    pthread_mutex_init(&mutex, NULL);

    pthread_t threads[NTHREADS];
    struct thread_data thread_args[NTHREADS];

    struct timespec begin, end;
    clock_gettime(CLOCK_REALTIME, &begin);

    for (int i = 0; i < NTHREADS; i++)
    {
        thread_args[i].arr = array;
        thread_args[i].n = size;
        pthread_create(&threads[i], NULL, sieve, (void *) &thread_args[i]);
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

    pthread_mutex_destroy(&mutex);
    
    return 0;
}


