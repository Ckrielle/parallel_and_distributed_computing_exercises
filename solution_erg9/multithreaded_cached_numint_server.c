#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXSIZE 256
#define CACHE_SIZE 5 // can be increased, used 5 for demonstration purposes

struct thread_data
{
    int client_fd;
};

typedef struct
{
    int n;
    double pi;
} cache_entry;

cache_entry cache[CACHE_SIZE];
int cache_idx = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

double f(double x)
{
    return 4.0 / (1.0 + x * x);
}

void add_to_cache(int n, double pi)
{
    pthread_mutex_lock(&mutex);
    if (cache_idx < CACHE_SIZE)
    {
        // we could also do, this was more optimal right?
        // pthread_mutex_lock(&mutex);
        cache[cache_idx].n = n;
        cache[cache_idx].pi = pi;
        // pthread_mutex_unlock(&mutex);
        cache_idx++;
    }
    else
    {
        // pthread_mutex_lock(&mutex);
        for (int i = 0; i < CACHE_SIZE - 1; i++)
            cache[i] = cache[i+1];
        cache[CACHE_SIZE-1].n = n;
        cache[CACHE_SIZE-1].pi = pi;
        // pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_unlock(&mutex);
}

int in_cache(int n, double *pi)
{
    int flag = 0;
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < cache_idx; i++)
    {
        if (cache[i].n == n)
        {
            *pi = cache[i].pi;
            flag = 1;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    return flag;
}

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

void sendline_response(int cfd, char *rsp, int rsp_size)
{
    send(cfd, rsp, rsp_size, 0);
    send(cfd, "\n", 1, 0);
}

void *handle_client(void *ptr)
{
    struct thread_data *my_data = (struct thread_data *) ptr;
    int my_client_fd = my_data->client_fd;

    while (1)
    {
        char request[MAXSIZE];
        for (int i = 0; i < MAXSIZE; i++)
            request[i] = '\0';
        // decided to implement a simple protocol, just sending the number
        ssize_t request_size = recv(my_client_fd, request, sizeof(request), 0);
        request[request_size-1] = '\0';
        printf("%s\n", request);

        int n = atoi(request);
        if (n == -1)
        {
            puts("Exiting...");
            break;
        }
        if (n <= 0)
        {
            char error_msg[] = "Invalid number. Please send a positive integer";
            sendline_response(my_client_fd, error_msg, strlen(error_msg));
            continue;
        }

        double pi;
        char resp[MAXSIZE*2];
        if (in_cache(n, &pi))
            snprintf(resp, sizeof(resp), "Cached pi = %.15f", pi);
        else
        {
            pi = 0.0;
            double h = 1.0 / (double) n;
            double sum = 0.0;

            for (int i = 1; i <= n; i++)
            {
                double x = h * ((double)i - 0.5);
                sum += f(x);
            }
            pi = h * sum;
            add_to_cache(n, pi);

            snprintf(resp, sizeof(resp), "Calculated pi = %.15f", pi);
        }
        sendline_response(my_client_fd, resp, strlen(resp));
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
        int client_fd = accept(server_fd, (struct sockaddr *) &caddr, &caddr_len);

        struct thread_data thread_args;
        thread_args.client_fd = client_fd;
        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, (void *) &thread_args);

        pthread_detach(thread);
    }
    close(server_fd);

    return 0;
}
