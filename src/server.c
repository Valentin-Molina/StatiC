#include "server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "connection.h"
#include "parser.h"
#include "request.h"

#define RECEPTION_BUFFER_SIZE 1024

void ShutdownServer(Server* server)
{
    printf("Shuting down server... ");
    close(server->socket_fd);
    printf("Done.\n");
}

void InitServer(Server* server, const char* address, uint32_t port,
                const char* root)
{
    printf("Initialising server on %s:%d... ", address, port);
    server->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket_fd == -1) {
        printf("Fail to create server socket.\n");
        return;
    }
    server->address.sin_family      = AF_INET;
    server->address.sin_port        = htons(port);
    server->address.sin_addr.s_addr = inet_addr(address);
    if ((bind(server->socket_fd, (struct sockaddr*)&server->address,
              sizeof(server->address))) != 0) {
        ShutdownServer(server);
        printf("Fail to bind server socket.\n");
        return;
    }
    if ((listen(server->socket_fd, 1)) != 0) {
        ShutdownServer(server);
        printf("Fail to start listening on socket.\n");
        return;
    }
    // TODO: Validate the root. Must be an existing folder.
    server->root = root;
    printf("Done.\n");
}

bool AcceptConnection(const Server* const server, Connection* connection)
{
    connection->address_len = sizeof(connection->address);
    connection->connection_fd =
        accept(server->socket_fd, (struct sockaddr*)&connection->address,
               &connection->address_len);
    if (connection->connection_fd < 0) {
        if (errno == EINTR) {
            printf("Connection listener interrupted.\n");
        } else {
            printf("Connection can't be accepted %s (%d)\n", strerror(errno),
                   errno);
        }
        return false;
    }
    return true;
}

void RunServer(Server* const server)
{
    printf("Waiting for connections... \n");
    while (true) {
        Connection connection = {0};
        if (!AcceptConnection(server, &connection)) {
            continue;
        }
        char* ip = inet_ntoa(connection.address.sin_addr);
        printf("Connection accepted from: %s\n", ip);

        char reception_buffer[RECEPTION_BUFFER_SIZE] = {0};
        ssize_t bytes_received                       = 0;
        printf("Receiving...\n");

        // FIXME(Valentin): Receive all the data and run parser while receiving
        // the data. Reception only of the first 1024 bytes
        bytes_received = recv(connection.connection_fd, reception_buffer,
                              RECEPTION_BUFFER_SIZE, 0);
        if (bytes_received == -1) {
            fprintf(stderr, "Error while receiving: %s (%d)\n", strerror(errno),
                    errno);
            CloseConnection(&connection);
            continue;
        }
        printf("Received:\n");
        fwrite(reception_buffer, 1, bytes_received, stdout);
        printf("\n");

        printf("Parsing...\n");
        HttpRequestParser parser = {0};
        parser.src               = reception_buffer;
        parser.len               = bytes_received;
        HttpRequest request      = {0};
        if (!ParseRequest(&parser, &request)) {
            printf("Unable to parse request !\n");
        } else {
            printf("Received %.*s for %.*s\n", (int)request.method.len,
                   request.method.data, (int)request.target.len,
                   request.target.data);
            printf("Headers:\n");
            for (size_t i = 0; i < request.headers_count; i++) {
                printf("* %.*s -> %.*s\n", (int)request.headers[i].name.len,
                       request.headers[i].name.data,
                       (int)request.headers[i].value.len,
                       request.headers[i].value.data);
            }
        }
        printf("\n");

        // HACK: This code should be sanitized. Only for testing purposes !!!
        char file_path[128] = {0};
        strcpy(file_path, server->root);
        const int root_len  = strlen(file_path);
        file_path[root_len] = '/';
        strncpy(file_path + root_len + 1, request.target.data,
                request.target.len);
        printf("Opening file %s...\n", file_path);
        FILE* file = fopen(file_path, "rb");
        if (file) {
            printf("File open.\n");
            printf("Responding... \n");

            // TODO: Add content size.
            const char* response =
                "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n";
            send(connection.connection_fd, response, strlen(response), 0);

            char buf[4096];
            while (!feof(file)) {
                size_t n = fread(buf, 1, sizeof(buf), file);
                if (n > 0)
                    send(connection.connection_fd, buf, n, 0);
            }
            fclose(file);

            const char* end = "\r\n";
            send(connection.connection_fd, end, strlen(end), 0);
        } else {
            printf("ERR: unable to open file\n");
            printf("Responding...\n ");
            const char* response = "HTTP/1.1 400 Bad Request\r\n\r\n";
            send(connection.connection_fd, response, strlen(response), 0);
        }

        printf("Done.\n");
        CloseConnection(&connection);
    }
}
