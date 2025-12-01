#ifndef REQUEST_H
#define REQUEST_H

#include "string_view.h"

typedef struct {
    StringView name;
    StringView value;
} HttpHeader;

#define MAX_HEADERS_COUNT 64

/** Http Request dedicated to a simple static web server. The body is not considered.
 */
typedef struct {
    StringView method;
    StringView target;
    int http_version_minor;
    size_t headers_count;
    HttpHeader headers[MAX_HEADERS_COUNT];
} HttpRequest;

#endif // REQUEST_H
