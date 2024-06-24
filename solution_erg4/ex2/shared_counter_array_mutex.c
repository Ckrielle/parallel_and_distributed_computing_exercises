#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 4

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct thread_data
{
    int end;
    int *array;
};


void *counter_thread(void *ptr)
{
    struct thread_data *my_data = (struct thread_data *) ptr;

    int my_end    = my_data->end;
    int *my_array = my_data->array;

    pthread_mutex_lock(&mutex);
     for (int i = 0; i < my_end; i++)
		for (int j = 0; j < i; j++)
		    my_array[i]++;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

void check_array(int end, int *array)
{
    printf("Checking...\n");

    int errors = 0;
    for (int i = 0; i < end; i++)
    {
        if (array[i] != NTHREADS * i)
        {
            errors++;
            printf("%d: %d should be %d\n", i, array[i], NTHREADS*i);
        }
    }
    printf("%d errors.\n", errors);
}

int main()
{
    int end = 1000;
    int array[end];

    for (int i = 0; i < end; i++)
        array[i] = 0;

    pthread_t threads[NTHREADS];
    struct thread_data thread_args[NTHREADS];

    for (int i = 0; i < NTHREADS; i++)
    {
        thread_args[i].end = end;
        thread_args[i].array = array;
        pthread_create(&threads[i], NULL, counter_thread, (void *) &thread_args[i]);
    }

    for (int i = 0; i < NTHREADS; i++)
        pthread_join(threads[i], NULL);

    check_array(end, array);
}