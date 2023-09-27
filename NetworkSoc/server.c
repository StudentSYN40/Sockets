#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>

#include "socket.h"

#define MAX_COUNT_SOCK 10

int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);

    int arr_sock[MAX_COUNT_SOCK] = {0};
    struct sockaddr_in serv_addr;

    fd_set set;
    int sock, max_sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    max_sock = sock;

    if (sock < 0)
    {
        printf("Error: socket() failed: %d\n", errno);
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Error: bind() failed: %d\n", errno);
        return -1;
    }

    if (listen(sock, 1) < 0)
    {
        printf("Error: listen() failed: %d\n", errno);
        return -1;
    }

    char bufSend[BUFSIZ];
    sprintf(bufSend, "\nPID сервера: %d\n", getpid());

    printf("Сервер работает!\n");

    while (TRUE)
    {
        FD_ZERO(&set);
        FD_SET(sock, &set);

        max_sock = sock;

        for (int i = 0; i < MAX_COUNT_SOCK; i++)
        {
            if (arr_sock[i] > 0)
            {
                FD_SET(arr_sock[i], &set);
                max_sock = arr_sock[i] > max_sock ? arr_sock[i] : max_sock;
            }
        }

        int rc = select(max_sock + 1, &set, NULL, NULL, NULL);
        if (rc < 0)
        {
            printf("select() failed: %d\n", errno);
            return -1;
        }

        if (FD_ISSET(sock, &set))
        {
            printf("Новое подключение.\n");

            int newsock = accept(sock, NULL, NULL);

            if (newsock < 0)
            {
                printf("Error: accept() failed: %d\n", errno);
                return -1;
            }

            int flag = 1;

            for (int i = 0; i < MAX_COUNT_SOCK && flag; i++)
            {
                if (arr_sock[i] == 0)
                {
                    arr_sock[i] = newsock;
                    printf("Клиент номер %d\n", i);
                    flag = 0;
                }
            }

            if (flag)
            {
                printf("Больше нет места для новых клиентов.\n");
            }
        }

        for (int i = 0; i < MAX_COUNT_SOCK; i++)
        {
            if (arr_sock[i] && FD_ISSET(arr_sock[i], &set))
            {
                char buf[MAX_LEN_BUF];

                int rv = recv(arr_sock[i], buf, sizeof(buf), 0);

                if (rv == 0)
                {
                    printf("Отключение от сервера клиента номер %d\n", i);
                    close(arr_sock[i]);
                    arr_sock[i] = 0;
                }
                else
                {
                    printf("Клиент номер %d отправил название соксета: %s\n", i, buf);

                    if (send(arr_sock[i], bufSend, strlen(bufSend) + 1, 0) < 0)
                    {
                        perror("Error: sendto fail");
                        return -1;
                    }
                }
            }
        }
    }

    close(sock);
    return 0;
}
