#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "server.h"

void print_usage(void)
{
    // FIXME: auto generate usage.
    printf("Usage: static [-p PORT] [-i IP_ADDRESS] [-r ROOT_SERVER_PATH]\n");
}

int main(int argc, char** argv)
{
    // FIXME: improve options
    // https://chatgpt.com/share/692f6235-2730-8006-b80f-990e9d07f05e
    const char* ip                 = "127.0.0.1";
    const char* root               = ".";
    uint32_t port                  = 8080;
    static struct option options[] = {
        {  "ip", required_argument, 0, 'i'},
        {"port", required_argument, 0, 'p'},
        {"root", required_argument, 0, 'r'},
        {     0,                 0, 0,   0}
    };
    int opt        = 0;
    int long_index = 0;
    while ((opt = getopt_long(argc, argv, "i:p:r:", options, &long_index)) !=
           -1) {
        switch (opt) {
        case 'i':
            ip = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case 'r':
            root = optarg;
            break;
        default:
            print_usage();
            exit(1);
        }
    }
    Server server = {0};
    InitServer(&server, ip, port, root);
    RunServer(&server);
    ShutdownServer(
        &server); // FIXME(Valentin): add signal handler to clean up server.
    return 0;
}
