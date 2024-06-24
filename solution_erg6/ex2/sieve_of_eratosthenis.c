#include <stdio.h>
#include <stdlib.h>

void sieve(int *a, int n)
{
    int i=0, j=0;

    for(i = 2; i <= n; i++)
        a[i] = 1;

    for(i = 2; i <= n; i++)
        if (a[i])
            for(j = i; (i * j) <= n; j++)
                a[(i * j)] = 0;
}

int main(int argc, char *argv)
{
    int n=10;
    int *array = (int *) malloc((n + 1) * sizeof(int));
    sieve(array,n);

    printf("\nPrimes numbers from 1 to %d are : ", n);
    for(int i = 2; i <= n; i++)
        if (array[i])
            printf("%d ", i);

    return 0;
}