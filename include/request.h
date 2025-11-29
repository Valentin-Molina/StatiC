#ifndef REQUEST_H
#define REQUEST_H

#include <stddef.h>

#define CREATE_STRING(s)                                                       \
    (String) { (s), sizeof(s) - 1 }

// TODO(Valentin): is char* a good idea ?
typedef struct
{
    char* content;
    size_t len;
} String;

typedef struct {
    String name;
    String value;
} HttpHeader;

#define MAX_HEADERS_COUNT 64

/** Http Request dedicated to a simple static web server. The body is not considered.
 */
typedef struct {
    String method;
    String target;
    int http_version_minor;
    size_t headers_count;
    HttpHeader headers[MAX_HEADERS_COUNT];
} HttpRequest;

#endif // REQUEST_H
