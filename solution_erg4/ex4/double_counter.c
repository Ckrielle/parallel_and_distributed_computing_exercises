#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 4

#define LIMIT 10000

struct thread_data
{
    int c1;
    int c2;
    pthread_mutex_t mutex1;
    pthread_mutex_t mutex2;
};

void inc_c1(int *ctr, pthread_mutex_t *mutex)
{
    pthread_mutex_lock(mutex);
    (*ctr)++;
    pthread_mutex_unlock(mutex);
}

void inc_c2(int *ctr, pthread_mutex_t *mutex)
{
    pthread_mutex_lock(mutex);
    (*ctr)++;
    pthread_mutex_unlock(mutex);
}

void *double_counter(void *ptr)
{
    struct thread_data *my_data = (struct thread_data *) ptr;

    int *my_c1 = &my_data->c1;
    int *my_c2 = &my_data->c2;
    pthread_mutex_t *my_mutex1 = &my_data->mutex1;
    pthread_mutex_t *my_mutex2 = &my_data->mutex2;

    for (int i = 0; i < LIMIT; i++)
    {
        inc_c1(my_c1, my_mutex1);
        inc_c2(my_c2, my_mutex2);
    }

    pthread_exit(NULL);
}

int main()
{
    struct thread_data thread_args;
    thread_args.c1 = 0;
    thread_args.c2 = 0;
    pthread_mutex_init(&thread_args.mutex1, NULL);
    pthread_mutex_init(&thread_args.mutex2, NULL);

    pthread_t threads[NTHREADS];
    for (int i = 0; i < NTHREADS; i++)
        pthread_create(&threads[i], NULL, double_counter, (void *) &thread_args);

    for (int i = 0; i < NTHREADS; i++)
        pthread_join(threads[i], NULL);

    pthread_mutex_destroy(&thread_args.mutex1);
    pthread_mutex_destroy(&thread_args.mutex2);

    printf("Counter1: %d\n", thread_args.c1);
    printf("Counter2: %d\n", thread_args.c2);
}