#include <netinet/in.h>

#ifndef SERVER_H
#define SERVER_H

typedef struct
{
    int socket_fd;
    struct sockaddr_in address;
} Server;

void InitServer(Server* server, char* address, uint32_t port);
void RunServer(Server* const server);
void ShutdownServer(Server* server);

#endif // SERVER_H
