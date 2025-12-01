#include <stdio.h>

#include "parser.h"
#include "request.h"

int main(void)
{
    /* clang-format off */
    char request[]             = "GET /hello.htm HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: www.tutorialspoint.com\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: Keep-Alive\r\n\r\n\r\n";
    /* clang-format on */
    HttpRequestParser parser   = {0};
    parser.src                 = request;
    parser.len                 = 198;
    HttpRequest parsed_request = {0};

    bool success = ParseRequest(&parser, &parsed_request);
    printf("parsing result: %d\n", success);

    printf("Received %.*s for %.*s\n", (int)parsed_request.method.len,
           parsed_request.method.data, (int)parsed_request.target.len,
           parsed_request.target.data);
    printf("Requested version is HTTP 1.%d\n",
           parsed_request.http_version_minor);
    for (size_t i = 0; i < parsed_request.headers_count; i++) {
        printf("Hearder %.*s -> %.*s\n",
               (int)parsed_request.headers[i].name.len,
               parsed_request.headers[i].name.data,
               (int)parsed_request.headers[i].value.len,
               parsed_request.headers[i].value.data);
    }

    return !success;
}
