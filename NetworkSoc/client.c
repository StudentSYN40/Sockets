#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#include "socket.h"

int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);

    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)};

    char buf[MAX_LEN_BUF];
    int sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        perror("Error: socket() failed");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error: connect() failed");
        return -1;
    }

    char socket_name[MAX_LEN_BUF];
    printf("Введите название соксета: ");
    scanf("%s", socket_name);

    if (send(sock, socket_name, strlen(socket_name) + 1, 0) < 0)
    {
        perror("Error: sendto() failed");
        return -1;
    }
    else
    {
        printf("Отправлено название соксета: %s\n", socket_name);
    }

    close(sock);

    return 0;
}
