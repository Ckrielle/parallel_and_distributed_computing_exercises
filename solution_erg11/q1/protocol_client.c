#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "protocol.h"

void call_uppercase(CLIENT *clnt, char *text)
{
    text_input input;
    input.text = text;
    input.offset = 0;
    char **result = uppercase_1(&input, clnt);
    if (result == NULL)
        fprintf(stderr, "Error: call failed\n");
    else
        printf("UPPER: %s\n", *result);
    free(*result);
    result = NULL;
}

void call_lowercase(CLIENT *clnt, char *text)
{
    text_input input;
    input.text = text;
    input.offset = 0;
    char **result = lowercase_1(&input, clnt);
    if (result == NULL)
        fprintf(stderr, "Error: call failed\n");
    else
        printf("LOWER: %s\n", *result);
    free(*result);
    result = NULL;
}

void call_rot(CLIENT *clnt, char *text, int offset)
{
    text_input input;
    input.text = text;
    input.offset = offset;
    char **result = rot_1(&input, clnt);
    if (result == NULL)
        fprintf(stderr, "Error: call failed\n");
    else
        printf("ROT (%d): %s\n", offset, *result);
    free(*result);
    result = NULL;
}

void call_unrot(CLIENT *clnt, char *text, int offset)
{
    text_input input;
    input.text = text;
    input.offset = offset;
    char **result = unrot_1(&input, clnt);
    if (result == NULL)
        fprintf(stderr, "Error: call failed\n");
    else
        printf("UNROT (%d): %s\n", offset, *result);
    free(*result);
    result = NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <server> <method> <text> <offset <ROT,UNROT>>\n", argv[0]);
        exit(1);
    }

    char *host = argv[1];
    char *method = argv[2];
    char *text = argv[3];
    int offset = 0;
    
    if (argc == 5) offset = atoi(argv[4]);

    CLIENT *clnt = clnt_create(host, PROTOCOL_PROG, PROTOCOL_VERS, "udp");
    if (clnt == NULL)
    {
        clnt_pcreateerror(host);
        exit(1);
    }

    if (strcmp(method, "UPPER") == 0)
        call_uppercase(clnt, text);
    else if (strcmp(method, "LOWER") == 0)
        call_lowercase(clnt, text);
    else if (strcmp(method, "ROT") == 0)
        call_rot(clnt, text, offset);
    else if (strcmp(method, "UNROT") == 0)
        call_unrot(clnt, text, offset);
    else
        fprintf(stderr, "ERR: %s\n", method);

    clnt_destroy(clnt);
    return 0;
}
