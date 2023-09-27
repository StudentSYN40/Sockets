#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#define SOCKET_NAME "/tmp/client_socket" // Путь к клиентскому соксету

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("./client.out <socket_name>\n");
        return -1;
    }

    const char *socket_name = argv[1];

    int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
    {
        perror("socket call error");
        return errno;
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;

    strcpy(addr.sun_path, SOCKET_NAME);
    printf("sun_path=%s, len=%ld\n", addr.sun_path, strlen(SOCKET_NAME));
    if (connect(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("Can't set dest address");
        return errno;
    }

    if (send(sfd, socket_name, strlen(socket_name), 0) == -1)
    {
        perror("send call error");
        return errno;
    }
    else
    {
        printf("send socket name: %s\n", socket_name);
    }

    close(sfd);

    return 0;
}
