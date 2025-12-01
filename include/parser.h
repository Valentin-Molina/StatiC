#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdbool.h>

#include "request.h"

typedef struct
{
    const char* src;
    size_t cursor;
    size_t len;
} HttpRequestParser;

bool ParseRequest(HttpRequestParser* const parser, HttpRequest* const request);

#endif // PARSER_H
