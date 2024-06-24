#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#define MAXSIZE 256

void sendline_request(int sfd, char *rsp, int rsp_size)
{
    send(sfd, rsp, rsp_size, 0);
    send(sfd, "\n", 1, 0);
}

int main(int argc, char **argv)
{
    char url[] = "127.0.0.1";
  
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
    }

    uint16_t port = 1337;

    struct sockaddr_in server_fd;
    server_fd.sin_family = AF_INET;
    server_fd.sin_port = htons(port);
    server_fd.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(sockfd, (struct sockaddr *)&server_fd, sizeof(struct sockaddr_in)) == -1) {
        perror("connect");
        exit(2);
    }

    char request[MAXSIZE];
    char response[MAXSIZE];
    while (1)
    {
        for (int i = 0; i < MAXSIZE; i++)
            request[i] = response[i] = '\0';

        fgets(request, MAXSIZE - 1, stdin);
        int length = strlen(request);
        request[length-1] = '\0';
        sendline_request(sockfd, request, length);

        if (!strcmp(request, "QUIT"))
            break;

        int n = recv(sockfd, response, MAXSIZE, 0);
        response[n-1] = '\0';
        printf("%s\n", response);
    }

    close(sockfd);

    return 0;
}   