#include "connection.h"

#include <unistd.h>
#include <stdlib.h>


void CloseConnection(Connection* connection)
{
    close(connection->connection_fd);
    free(connection);
    connection = NULL;
}
