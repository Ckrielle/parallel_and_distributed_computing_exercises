#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXSIZE 256
#define MAX_CLIENTS 10

struct thread_data
{
    int client_fd;
    struct client_data *clients;
};

struct client_data {
    int client_fds[MAX_CLIENTS];
    int count;
    pthread_mutex_t mutex;
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

    if (listen(fd, MAX_CLIENTS))
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

void broadcast_message(struct client_data *clients, int sender_fd, char *message)
{
    char full_message[MAXSIZE * 2];
    pthread_mutex_lock(&clients->mutex);

    int sender_index = -1;
    for (int i = 0; i < clients->count; i++)
    {
        if (clients->client_fds[i] == sender_fd)
        {
            sender_index = i;
            break;
        }
    }

    snprintf(full_message, sizeof(full_message), "client%d: %s", sender_index + 1, message);

    for (int i = 0; i < clients->count; i++)
    {
        if (clients->client_fds[i] != sender_fd)
        {
            send_response(clients->client_fds[i], full_message, strlen(full_message));
        }
    }
    pthread_mutex_unlock(&clients->mutex);
}

void *handle_client(void *ptr)
{
    struct thread_data *my_data = (struct thread_data *)ptr;
    int my_client_fd = my_data->client_fd;
    struct client_data *clients = my_data->clients;

    while (1)
    {
        char request[MAXSIZE];
        ssize_t request_size = recv(my_client_fd, request, sizeof(request), 0);
        if (request_size <= 0) break;
        request[request_size] = '\0';
        printf("Received message: %s\n", request);

        broadcast_message(clients, my_client_fd, request);
    }

    close(my_client_fd);

    pthread_mutex_lock(&clients->mutex);
    for (int i = 0; i < clients->count; i++)
    {
        if (clients->client_fds[i] == my_client_fd)
        {
            clients->client_fds[i] = clients->client_fds[clients->count - 1];
            clients->count--;
            break;
        }
    }
    pthread_mutex_unlock(&clients->mutex);

    pthread_exit(NULL);
}

int main()
{
    struct client_data clients;
    clients.count = 0;
    pthread_mutex_init(&clients.mutex, NULL);

    int server_fd = open_serverfd(1337);

    while (1)
    {
        struct sockaddr_storage caddr;
        socklen_t caddr_len = sizeof(caddr);
        int client_fd = accept(server_fd, (struct sockaddr *)&caddr, &caddr_len);

        pthread_mutex_lock(&clients.mutex);
        if (clients.count < MAX_CLIENTS)
        {
            clients.client_fds[clients.count++] = client_fd;
            pthread_mutex_unlock(&clients.mutex);

            struct thread_data thread_args;
            thread_args.client_fd = client_fd;
            thread_args.clients = &clients;

            pthread_t thread;
            pthread_create(&thread, NULL, handle_client, (void *)&thread_args);
            pthread_detach(thread);
        }
        else
        {
            pthread_mutex_unlock(&clients.mutex);
            close(client_fd);
        }
    }
    close(server_fd);
    pthread_mutex_destroy(&clients.mutex);

    return 0;
}
