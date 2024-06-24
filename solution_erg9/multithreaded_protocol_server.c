#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXSIZE 256

char UPPER[] = "UPPER";
char LOWER[] = "LOWER";
char UNROT[] = "UNROT";
char QUIT[]  = "QUIT";
char ROT[]   = "ROT";

// kinda unecessary since it's only one argument, so we could easily have passed just a pointer to it
// kept it for the sake of sticking to my pattern from previous exercises
struct thread_data
{
    int client_fd;
};

int open_serverfd(int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)))
    {
        perror("bind error:");
        exit(2);
    }

    if (listen(fd, 1))
    {
        perror("listen error:");
        exit(2);
    }

    return fd;
}

void send_response(int cfd, char *rsp, int rsp_size)
{
    send(cfd, rsp, rsp_size, 0);
}

void sendline_response(int cfd, char *rsp, int rsp_size)
{
    send(cfd, rsp, rsp_size, 0);
    send(cfd, "\n", 1, 0);
}

void parse_request(char *request, char *method, char *trgt, char *ofst)
{
    sscanf(request, "%s", method);

    if (!strcmp(method, ROT) || !strcmp(method, UNROT)) sscanf(request, "%s %s %s", method, trgt, ofst);
    else sscanf(request, "%s %s", method, trgt);
}

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

    return s;
}

void *handle_client(void* ptr)
{
    struct thread_data *my_data = (struct thread_data *) ptr;
    int my_client_fd = my_data->client_fd;

    while (1)
    {
        char request[MAXSIZE];
        for (int i = 0; i < MAXSIZE; i++)
            request[i] = '\0';
        ssize_t request_size = recv(my_client_fd, request, sizeof(request), 0);
        request[request_size-1] = '\0';
        printf("%s\n", request);

        char method[MAXSIZE], target[MAXSIZE], offset[MAXSIZE];
        for (int i = 0; i < MAXSIZE; i++)
            method[i] = target[i] = offset[i] = '\0';
        parse_request(request, method, target, offset);
        int n_offset = atoi(offset);

        char resp[MAXSIZE*2];
        for (int i = 0; i < MAXSIZE * 2; i++)
            resp[i] = '\0';
        unsigned int resp_len;
        char status_code[MAXSIZE] = "OK \0";
        if (!strncmp(method, UPPER, sizeof(UPPER)))
            resp_len = convert_upper(target, resp);
        else if (!strncmp(method, LOWER, sizeof(LOWER)))
            resp_len = convert_lower(target, resp);
        else if (!strncmp(method, ROT, sizeof(ROT)))
            resp_len = convert_rot(target, resp, n_offset);
        else if (!strncmp(method, UNROT, sizeof(UNROT)))
            resp_len = convert_unrot(target, resp, n_offset);
        else if (!strncmp(method, QUIT, sizeof(QUIT)))
            break;
        else
        {
            snprintf(resp, MAXSIZE*2, "Protocol verb %s not supported", method);
            strcpy(status_code, "ERR \0");
            resp_len = strlen(resp);
        }
        resp[resp_len] = '\0';
        send_response(my_client_fd, status_code, strlen(status_code));
        sendline_response(my_client_fd, resp, resp_len);
    }

    char close_msg[] = "Connection closed...";
    size_t close_len = strlen(close_msg);
    sendline_response(my_client_fd, close_msg, close_len);
    close(my_client_fd);

    pthread_exit(NULL);
}

int main()
{
    int server_fd = open_serverfd(1337);

    while (1)
    {
        struct sockaddr_storage caddr;
        socklen_t caddr_len = sizeof(caddr);
        
        // should client_fd be a pointer? So that when it's closed in handle_client it closes in main as well
        int client_fd = accept(server_fd, (struct sockaddr *) &caddr, &caddr_len);

        struct thread_data thread_args;
        thread_args.client_fd = client_fd;
        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, (void *) &thread_args);

        // By testing them, this is better than join for a multithreaded server purpose
        pthread_detach(thread);
    }
    close(server_fd);

    return 0;
}