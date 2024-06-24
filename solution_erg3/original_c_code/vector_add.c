#include <stdio.h>

#define SIZE 1000

int main()
{
    int i;
    double a[SIZE];
    double b[SIZE];
    double c[SIZE];

    for(i = 0; i < SIZE; i++) {
        a[i] = 0.0;
		b[i] = 1.0;
        c[i] = 0.5;
    }
		
    for (i = 0; i < SIZE; i++)
        a[i] = b[i] + c[i];
 
    for(i = 0; i < SIZE; i++)
        printf("%g ", a[i]);
    printf("\n"); 
}