#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define NTHREADS 4

struct thread_data
{
    char *text;
    char *pattern;
    unsigned int pattern_len;
    unsigned char *matches;
    int *matches_count;
    pthread_mutex_t mutex;
    int from;
    int to;
};

void *strmtch(void *ptr)
{
    struct thread_data *my_data = (struct thread_data *) ptr;

    char *my_text = my_data->text;
    char *my_pattern = my_data->pattern;
    unsigned int my_pattern_len = my_data->pattern_len;
    unsigned char *my_matches = my_data->matches;
    int *my_matches_count = my_data->matches_count;
    pthread_mutex_t my_mutex = my_data->mutex;
    int my_from = my_data->from;
    int my_to = my_data->to;

    //int local_matches_count = 0; didnt produce correct results with this, so for sake of solution went with below
    for (int i = my_from; i < my_to; i++)
        for (int j = 0; j < my_pattern_len && (unsigned char) my_pattern[j] == (unsigned char) my_text[i + j]; j++)
            if (j >= my_pattern_len - 1)
            {
                pthread_mutex_lock(&my_mutex);
                my_matches[i] = 1;
                (*my_matches_count)++;
                pthread_mutex_unlock(&my_mutex);
            }
}

int main()
{
    FILE *f = fopen("D:\\large\\bigger_coli.txt", "r"); // change path manually
    
    fseek(f, 0, SEEK_END);
    unsigned int data_len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *fdata = (char *) malloc(data_len * sizeof(char));
    fread(fdata, sizeof(char), data_len, f);
    int matches_count = 0;
    unsigned char *matches = (unsigned char *) malloc(data_len * sizeof(unsigned char));
    for (int i = 0; i < data_len; i++)
        matches[i] = 0;

    char pattern[] = "tacccagattatcgccatcaacgg";
    int pattern_len = strlen(pattern);
    int size = data_len - pattern_len;

    struct thread_data thread_args[NTHREADS];
    pthread_t threads[NTHREADS];
    int blocks = size / NTHREADS;

    struct timespec begin, end;
    clock_gettime(CLOCK_REALTIME, &begin);

    for (int i = 0; i < NTHREADS; i++)
    {
        thread_args[i].text = fdata;
        thread_args[i].pattern = pattern;
        thread_args[i].pattern_len = pattern_len;
        thread_args[i].matches = matches;
        thread_args[i].matches_count = &matches_count;
        thread_args[i].mutex = PTHREAD_MUTEX_INITIALIZER;
        thread_args[i].from = i * blocks;
        thread_args[i].to = thread_args[i] .from + blocks;
        if (i == NTHREADS - 1) thread_args[i].to = size;
        pthread_create(&threads[i], NULL, strmtch, (void *) &thread_args[i]);
    }
    
    for (int i = 0; i < NTHREADS; i++)
        pthread_join(threads[i], NULL);

    clock_gettime(CLOCK_REALTIME, &end);
    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;

    printf("Total matches: %d\n", matches_count);
    for (int i = 0; i < size; i++)
        if (matches[i])
            printf("%d ", i);
    printf("\nTime measured: %.6f seconds.\n", elapsed);

    free(matches);
    matches = NULL;

    return 0;
}