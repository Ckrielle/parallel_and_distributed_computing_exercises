#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 4

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct thread_data
{
    int end;
    int *array;
    int *counter;
};


void *counter_thread(void *ptr)
{
    struct thread_data *my_data = (struct thread_data *) ptr;

    int my_end      = my_data->end;
    int *my_array   = my_data->array;
    int *my_counter = my_data->counter;

    pthread_mutex_lock(&mutex);
    while (1)
    {
        if (*my_counter >= my_end) 
            break;
        my_array[*my_counter]++;
        (*my_counter)++;
    }
    pthread_mutex_unlock(&mutex);
    
    pthread_exit(NULL);
}

void check_array(int end, int *array)
{
    printf("Checking...\n");

    int errors = 0;
    for (int i = 0; i < end; i++)
    {
        if (array[i] != 1)
        {
            errors++;
            printf("%d: %d should be 1\n", i, array[i]);
        }
    }
    printf("%d errors.\n", errors);
}

int main()
{
    int end = 10000;
    int array[end];
    int counter = 0;

    for (int i = 0; i < end; i++)
        array[i] = 0;

    pthread_t threads[NTHREADS];
    struct thread_data thread_args[NTHREADS];

    for (int i = 0; i < NTHREADS; i++)
    {
        thread_args[i].end = end;
        thread_args[i].array = array;
        thread_args[i].counter = &counter;
        pthread_create(&threads[i], NULL, counter_thread, (void *) &thread_args[i]);
    }

    for (int i = 0; i < NTHREADS; i++)
        pthread_join(threads[i], NULL);

    check_array(end, array);
}