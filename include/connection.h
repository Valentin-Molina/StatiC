#ifndef CONNECTION_H
#define CONNECTION_H

#include <arpa/inet.h>

typedef struct
{
    int connection_fd;
    struct sockaddr_in address;
    socklen_t address_len;
} Connection;

void CloseConnection(Connection* connection);

#endif // CONNECTION_H
