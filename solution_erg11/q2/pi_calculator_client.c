#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pi_calculator.h"

int main(int argc, char *argv[])
{
    CLIENT *clnt;
    interval arg;
    double *result;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <server> <intervals num>\n", argv[0]);
        exit(1);
    }

    clnt = clnt_create(argv[1], PI_CALCULATOR_PROG, PI_CALCULATOR_VERS, "udp");
    if (clnt == NULL)
    {
        clnt_pcreateerror(argv[1]);
        exit(1);
    }

    arg.n = atoi(argv[2]);

    result = calculate_pi_1(&arg, clnt);
    if (result == NULL)
        clnt_perror(clnt, "call failed:");
    else if (*result == -1)
        printf("Error: Invalid number of intervals.\n");
    else
        printf("Calculated pi = %.15f\n", *result);

    clnt_destroy(clnt);

    return 0;
}