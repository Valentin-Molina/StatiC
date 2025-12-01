#include <netinet/in.h>

#ifndef SERVER_H
#define SERVER_H

typedef struct
{
    int socket_fd;
    struct sockaddr_in address;
    const char* root;
} Server;

void InitServer(Server* server, const char* address, uint32_t port, const char* root);
void RunServer(Server* const server);
void ShutdownServer(Server* server);

#endif // SERVER_H
