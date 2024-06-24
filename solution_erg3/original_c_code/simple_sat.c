#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

// Printing utility
static void printResult(bool *v, int size, int z)
{
    printf("%d ", z);
    for (int i=0; i < size * 2; i += 2) {
        if (v[i]) printf(" 1");
        else printf(" 0");
    }
    printf("\n");
}

void check_circuit(int z, int size) {
    
    // z: the combination number
    // v: the combination number in binar format 
    
    bool *v = (bool *) malloc(size * sizeof(bool));

    for (int i = size-1; i >= 0; i--) 
        v[i] = (z & (1 << i)) != 0;

    // Check the ouptut of the circuit for input v
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
    
    // If output == 1 print v and z
    if (value) printResult(v, size, z);

    free(v);
    v = NULL;
}	

int main()
{  
    // Circuit input size (number of bits)
    int size = 28;
    // Number of possible inputs (bit combinations)
    int iterations = pow(2, size);
    
    // Start timing
    //long start = System.currentTimeMillis();
    /* begin time */
    struct timespec begin, end; 
    clock_gettime(CLOCK_REALTIME, &begin);
    // Check all possible inputs        
    for (int i = 0; i < iterations; i++)
        check_circuit (i, size);
    
    // Stop timing   
    //long elapsedTimeMillis = System.currentTimeMillis()-start;
    /* end time */
    clock_gettime(CLOCK_REALTIME, &end);
    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;
    printf("Time measured: %.3f seconds.\n", elapsed); 
    puts("All done\n");
    //System.out.println("time in ms = "+ elapsedTimeMillis);
        
}
        
    
