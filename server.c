#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
void error(char *err)
{
    perror(err);
    exit(1);
}
int a = 1;

void *send_message(void *arg);
void *recieve_messages(void *arg);

int main(int argc, char *argv[])
{
    int sock, othersock;
    struct sockaddr_in serveradd, clientadd;
    socklen_t client;
    if (argc < 2)
    {
        error("somthing is missing");
    }
    memset((char *)&serveradd, 0, sizeof(serveradd));
    memset((char *)&clientadd, 0, sizeof(clientadd));
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        error("couldnt create socket.\n");
    }
    serveradd.sin_family = AF_INET;
    serveradd.sin_addr.s_addr = INADDR_ANY;
    serveradd.sin_port = htons(atoi(argv[1]));
    if ((bind(sock, (struct sockaddr *)&serveradd, sizeof(serveradd))) < 0)
    {
        error("binding failed.\n");
    }
    listen(sock, 5);
    client = sizeof(clientadd);
    printf("waiting for a client to connect...\n");
    if ((othersock = accept(sock, (struct sockaddr *)&clientadd, &client)) < 0)
    {
        error("connection failed.\n");
    }
    printf("connection sucessful.IP:%s\n", (char *)inet_ntoa(clientadd.sin_addr));

    pthread_t sendthread, recvthread;
    pthread_create(&sendthread, NULL, send_message, &othersock);
    pthread_create(&recvthread, NULL, recieve_messages, &othersock);
    while (1)
    {
        if (a == 0)
        {
            goto label;
        }
    }

    pthread_join(sendthread, NULL);
    pthread_join(recvthread, NULL);
label:
    close(othersock);
    close(sock);
}

void *send_message(void *arg)
{
    char buffer[255];
    while (1)
    {
        memset(buffer, 0, 255);
        fgets(buffer, sizeof(buffer), stdin);
        printf("\n");
        send(*((int *)arg), buffer, sizeof(buffer), 0);
        if (!strncmp(buffer, "bye", 3))
        {
            a = 0;
            break;
        }
    }
}

void *recieve_messages(void *arg)
{
    char buffer[255];
    while (1)
    {
        memset(buffer, 0, 255);
        recv(*((int *)arg), buffer, 255, 0);
        printf("Client:%s\n", buffer);
        if (!strncmp(buffer, "bye", 3))
        {
            a = 0;
            break;
        }
    }
}