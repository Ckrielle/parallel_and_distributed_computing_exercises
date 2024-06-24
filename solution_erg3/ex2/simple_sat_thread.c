#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NTHREADS 4 // Adjust based on your system's capabilities

#define SIZE 28

struct thread_data {
    int from;
    int to;
};

// Printing utility, modified to handle threading
static void printResult(bool *v, int z) {
    printf("%d ", z);
    for (int i = 0; i < SIZE; i++) {
        if (v[i]) printf(" 1");
        else printf(" 0");
    }
    printf("\n");
}

void check_circuit(int z) {
    bool *v = (bool *)malloc(SIZE * sizeof(bool));
    for (int i = SIZE - 1; i >= 0; i--)
        v[i] = (z & (1 << i)) != 0;

    // Simplified for brevity. Include your logical checks here.
    bool value = 
        (  v[0]  ||  v[1]  )
    && ( !v[1]  || !v[3]  )
    && (  v[2]  ||  v[3]  )
    && ( !v[3]  || !v[4]  )
    && (  v[4]  || !v[5]  )
    && (  v[5]  || !v[6]  )
    && (  v[5]  ||  v[6]  )
    && (  v[6]  || !v[15] )
    && (  v[7]  || !v[8]  )
    && ( !v[7]  || !v[13] )
    && (  v[8]  ||  v[9]  )
    && (  v[8]  || !v[9]  )
    && ( !v[9]  || !v[10] )
    && (  v[9]  ||  v[11] )
    && (  v[10] ||  v[11] )
    && (  v[12] ||  v[13] )
    && (  v[13] || !v[14] )
    && (  v[14] ||  v[15] )
    && (  v[14] ||  v[16] )
    && (  v[17] ||  v[1]  )
    && (  v[18] || !v[0]  )
    && (  v[19] ||  v[1]  )
    && (  v[19] || !v[18] )
    && ( !v[19] || !v[9]  )
    && (  v[0]  ||  v[17] )
    && ( !v[1]  ||  v[20] )
    && ( !v[21] ||  v[20] )
    && ( !v[22] ||  v[20] )
    && ( !v[21] || !v[20] )
    && (  v[22] || !v[20] );

    if (value)
        printResult(v, z);
    free(v);
    v = NULL;
}

void* thread_wrapper(void* ptr) {
    struct thread_data *my_data = (struct thread_data *) ptr;
    for (int i = my_data->from; i < my_data->to; i++)
        check_circuit(i);
    pthread_exit(NULL);
}

int main() {
    int iterations = pow(2, SIZE);

    pthread_t threads[NTHREADS];
    struct thread_data thread_args[NTHREADS];
    int block = iterations / NTHREADS;

    /* begin time */
    struct timespec begin, end; 
    clock_gettime(CLOCK_REALTIME, &begin);

    for (int i = 0; i < NTHREADS; i++) {
        thread_args[i].from = i * block;
        thread_args[i].to = thread_args[i].from + block;
        if (i == NTHREADS - 1) thread_args[i].to = SIZE;
        pthread_create(&threads[i], NULL, thread_wrapper, &thread_args[i]);
    }

    for (int i = 0; i < NTHREADS; i++)
        pthread_join(threads[i], NULL);

    /* end time */
    clock_gettime(CLOCK_REALTIME, &end);
    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;
    printf("Time measured: %.3f seconds.\n", elapsed);

    puts("All done\n");

    return 0;
}
