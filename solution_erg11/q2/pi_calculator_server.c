#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>

#include "pi_calculator.h"

#define MAXSIZE 256

double f(double x)
{
    return 4.0 / (1.0 + x * x);
}

double get_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}

double *calculate_pi_1_svc(interval *argp, struct svc_req *rqstp) {
    static double pi;
    int n = argp->n;

    if (n <= 0)
    {
        fprintf(stderr, "Invalid number. Please send a positive integer.\n");
        pi = -1;
        return &pi;
    }

    double h = 1.0 / (double)n;
    double sum = 0.0;
    double total_time = get_time();
    for (int i = 1; i <= n; i++)
    {
        double x = h * ((double)i - 0.5);
        sum += f(x);
    }
    pi = h * sum;
    total_time = get_time() - total_time;

    printf("Calculated pi = %.15f in %.6f seconds\n", pi, total_time);

    return &pi;
}