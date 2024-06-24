#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int strmtch(char *data, unsigned long int data_len, char *pattern, unsigned long int pattern_len, unsigned char *matches)
{
    int matches_count = 0;
    for (int i = 0; i < data_len - pattern_len; i++)
        for (int j = 0; j < pattern_len && (unsigned char) pattern[j] == (unsigned char) data[i + j]; j++)
            if (j >= pattern_len - 1)
            {
                matches[i] = 1;
                matches_count += 1;
            }
    return matches_count;
}

int main()
{
    FILE *f = fopen("D:\\large\\bigger_coli.txt", "r");
    
    fseek(f, 0, SEEK_END);
    unsigned long int size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *fdata = (char *) malloc(size * sizeof(char));
    fread(fdata, sizeof(char), size, f);
    unsigned char *matches = (unsigned char *) malloc(size * sizeof(unsigned char));
    for (int i = 0; i < size; i++)
        matches[i] = 0;

    char pattern[] = "tacccagattatcgccatcaacgg";
    struct timespec begin, end;
    clock_gettime(CLOCK_REALTIME, &begin);
    int matches_count = strmtch(fdata, size, pattern, strlen(pattern), matches);

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