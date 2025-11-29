#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdbool.h>

#include "request.h"

// Minimum requirements:
// * HTTP 1.0 and 1.1
// * Parse GET
// * Parse HEAD
// * Validate Host header
typedef struct
{
    char* src;
    size_t cursor;
    size_t len;
} HttpRequestParser;

bool ParseRequest(HttpRequestParser* parser, HttpRequest* request);

#endif // PARSER_H
