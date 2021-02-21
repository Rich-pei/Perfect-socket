#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
int a = 1;
void error(char *err)
{
    perror(err);
    exit(1);
}

void *send_message(void *arg);
void *recieve_messages(void *arg);

int main(int argc, char *argv[])
{
    int sock, n;
    struct sockaddr_in serveradd;
    if (argc < 3)
    {
        error("missing port number or IP address\n");
    }
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        error("socket creation failed to do so\n");
    }
    memset((char *)&serveradd, 0, sizeof(serveradd));
    serveradd.sin_family = AF_INET;
    serveradd.sin_addr.s_addr = inet_addr(argv[1]);
    serveradd.sin_port = htons(atoi(argv[2]));
    printf("conecting to server...\n");
    if ((connect(sock, (struct sockaddr *)&serveradd, sizeof(serveradd))) < 0)
    {
        error("connection failed\n");
    }
    printf("server connected, start chatting\n");
    pthread_t sendthread, recvthread;
    pthread_create(&sendthread, NULL, send_message, &sock);
    pthread_create(&recvthread, NULL, recieve_messages, &sock);
    while(1)
    {
        if(a==0)
        {
            goto label;
        }
    }
    pthread_join(sendthread, NULL);
    pthread_join(recvthread, NULL);
label:
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
        printf("Server:%s\n", buffer);
        if (!strncmp(buffer, "bye", 3))
        {
            a = 0;
            break;
        }
    }
}