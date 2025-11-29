#include "connection.h"

#include <stdlib.h>
#include <unistd.h>

void CloseConnection(Connection* connection)
{
    close(connection->connection_fd);
    connection = NULL;
}
