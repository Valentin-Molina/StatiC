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

typedef struct {
    String method;
    String target;
    int http_version_minor;
    // TODO(Valentin): Add headers.
} HttpRequest;

#endif // REQUEST_H
