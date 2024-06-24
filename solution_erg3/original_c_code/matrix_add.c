#include <stdio.h>

#define SIZE 10

int main()
{
    int i, j;
    double a[SIZE][SIZE];
    double b[SIZE][SIZE];
    double c[SIZE][SIZE];

    for (i = 0; i < SIZE; i++)
    {
        for (j = 0; j < SIZE; j++)
        {
            a[i][j] = 0.1;
		    b[i][j] = 0.3;
            c[i][j] = 0.5;
        }
    }

    for(i = 0; i < SIZE; i++) 
        for(j = 0; j < SIZE; j++)  
            a[i][j] = b[i][j] + c[i][j];

    // for debugging 
    for(i = 0; i < SIZE; i++)
    {
        for(j = 0; j < SIZE; j++) 
            printf("%g ", a[i][j]); 
        printf("\n");
    }
}