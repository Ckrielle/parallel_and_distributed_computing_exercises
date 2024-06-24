#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "protocol.h"

unsigned int convert_upper(char *src, char *dst)
{
    unsigned int s = 0;
    while (*src != '\0')
    {
        char c = *src++;
        if (0x61 <= c && c <= 0x7a)
            c -= 0x20;
        *dst++ = c;
        s++;
    }
    *dst = '\0';
    return s;
}

unsigned int convert_lower(char *src, char *dst)
{
    unsigned int s = 0;
    while (*src != '\0')
    {
        char c = *src++;
        if (0x41 <= c && c <= 0x5a)
            c += 0x20;
        *dst++ = c;
        s++;
    }
    *dst = '\0';
    return s;
}

int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

unsigned int convert_rot(char *src, char *dst, int ofst)
{
    unsigned int s = 0;
    while (*src != '\0')
    {
        char c = *src++;
        if (0x41 <= c && c <= 0x5a)
            c = mod(((c - 0x41) + ofst), 26) + 0x41;
        else if (0x61 <= c && c <= 0x7a)
            c = mod(((c - 0x61) + ofst), 26) + 0x61;
        *dst++ = c;
        s++;
    }
    *dst = '\0';
    return s;
}

unsigned int convert_unrot(char *src, char *dst, int ofst)
{
    unsigned int s = 0;
    while (*src != '\0')
    {
        char c = *src++;
        if (0x41 <= c && c <= 0x5a)
            c = mod(((c - 0x41) - ofst), 26) + 0x41;
        else if (0x61 <= c && c <= 0x7a)
            c = mod(((c - 0x61) - ofst), 26) + 0x61;
        *dst++ = c;
        s++;
    }
    *dst = '\0';
    return s;
}

char **uppercase_1_svc(text_input *argp, struct svc_req *rqstp)
{
    static char *result;
    result = malloc(256 * sizeof(char));
    convert_upper(argp->text, result);
    return &result;
}

char **lowercase_1_svc(text_input *argp, struct svc_req *rqstp)
{
    static char *result;
    result = malloc(256 * sizeof(char));
    convert_lower(argp->text, result);
    return &result;
}

char **rot_1_svc(text_input *argp, struct svc_req *rqstp)
{
    static char *result;
    result = malloc(256 * sizeof(char));
    convert_rot(argp->text, result, argp->offset);
    return &result;
}

char **unrot_1_svc(text_input *argp, struct svc_req *rqstp)
{
    static char *result;
    result = malloc(256 * sizeof(char));
    convert_unrot(argp->text, result, argp->offset);
    return &result;
}
