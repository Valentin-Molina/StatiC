#include "parser.h"
#include "request.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static const String token_special_characters = {
    .content = "!#$%&'*+-.^_`|~", .len = sizeof("!#$%&'*+-.^_`|~") - 1};

static bool IsDigit(char character)
{
    return character >= '0' && character <= '9';
}

static bool IsAlpha(char character)
{
    return (character >= 'a' && character <= 'z') ||
           (character >= 'A' && character <= 'Z');
}

static bool IsOneOff(char character, const String* str)
{
    for (size_t i = 0; i < str->len; i++) {
        if (character == *(str->content + i))
            return true;
    }
    return false;
}

static bool IsTokenCharacter(char character)
{
    return IsAlpha(character) || IsDigit(character) ||
           IsOneOff(character, &token_special_characters);
}

static bool HasParserReachedEnd(HttpRequestParser* parser)
{
    return parser->len == parser->cursor;
}

static bool ParseRequiredString(HttpRequestParser* parser,
                                String required_string)
{
    if (required_string.len == 0) {
        return false;
    }

    // Parser should content at least the size of the string.
    if (required_string.len > (parser->len - parser->cursor)) {
        return false;
    }

    for (size_t i = 0; i < required_string.len; i++) {
        if (parser->src[parser->cursor + i] != required_string.content[i]) {
            return false;
        }
    }
    parser->cursor += required_string.len;

    return true;
}

static bool ParseMethod(HttpRequestParser* parser, String* method)
{
    // The method is the first element of the HTTP request.
    parser->cursor  = 0;
    method->content = parser->src;

    // The method should not be empty.
    if (HasParserReachedEnd(parser) ||
        !IsTokenCharacter(parser->src[parser->cursor])) {
        return false;
    }
    parser->cursor++;

    // Iterate over the method to get its length.
    while (!HasParserReachedEnd(parser) &&
           IsTokenCharacter(parser->src[parser->cursor])) {
        parser->cursor++;
    }
    method->len = parser->cursor;

    // The method should be separated from the target by a space
    if (HasParserReachedEnd(parser) || (parser->src[parser->cursor] != ' ')) {
        return false;
    }
    parser->cursor++;
    return true;
}

static bool ParseTarget(HttpRequestParser* parser, String* target)
{
    // The target comes after the method.
    size_t init_cursor = parser->cursor;
    target->content    = parser->src + init_cursor;

    // Iterate over the target to get its length
    while (!HasParserReachedEnd(parser) &&
           (parser->src[parser->cursor] != ' ')) {
        parser->cursor++;
    }
    target->len = parser->cursor - init_cursor;

    // The target should be separated from the version by a space.
    if (HasParserReachedEnd(parser)) {
        return false;
    }
    parser->cursor++;
    return true;
}

static bool ParseVersion(HttpRequestParser* parser, int* version_minor)
{
    if (ParseRequiredString(parser, CREATE_STRING("HTTP/1.0\r\n"))) {
        *version_minor = 0;
        return true;
    }
    if (ParseRequiredString(parser, CREATE_STRING("HTTP/1.1\r\n"))) {
        *version_minor = 1;
        return true;
    }
    return false;
}

static bool ParseHeaderName(HttpRequestParser* parser, String* header_name)
{
    size_t init_cursor   = parser->cursor;
    header_name->content = parser->src + init_cursor;
    // Header name can't be empty
    if (HasParserReachedEnd(parser) ||
        !IsTokenCharacter(parser->src[parser->cursor])) {
        return false;
    }
    parser->cursor++;

    // Iterate over the hearder name to get its length
    while (!HasParserReachedEnd(parser) &&
           IsTokenCharacter(parser->src[parser->cursor])) {
        parser->cursor++;
    }
    header_name->len = parser->cursor - init_cursor;

    // Header name ends with ':'
    if (HasParserReachedEnd(parser) || (parser->src[parser->cursor] != ':')) {
        return false;
    }
    parser->cursor++;
    return true;
}

/** Only support ASCII characters */
static bool IsVisibleCharacter(char character)
{
    return (character >= ' ' && character <= '~');
}

static void IgnoreWhiteSpacesAndTabs(HttpRequestParser* parser)
{
    while (!HasParserReachedEnd(parser) &&
           ((parser->src[parser->cursor] == ' ') ||
            parser->src[parser->cursor] == '\t')) {
        parser->cursor++;
    }
}

static bool ParseHeaderValue(HttpRequestParser* parser, String* header_value)
{
    IgnoreWhiteSpacesAndTabs(parser);

    size_t init_cursor    = parser->cursor;
    header_value->content = parser->src + init_cursor;

    // Iterate over the hearder value to get its length
    while (!HasParserReachedEnd(parser) &&
           IsVisibleCharacter(parser->src[parser->cursor])) {
        parser->cursor++;
    }
    // Remove trailing white spaces and tabs
    while ((parser->cursor - 1 != init_cursor) &&
           ((parser->src[parser->cursor - 1] == ' ') ||
            parser->src[parser->cursor - 1] == '\t')) {
        parser->cursor--;
    }
    header_value->len = parser->cursor - init_cursor;

    IgnoreWhiteSpacesAndTabs(parser);

    if (!ParseRequiredString(parser, CREATE_STRING("\r\n"))) {
        return false;
    }
    return true;
}

static bool ParseHeader(HttpRequestParser* parser, HttpHeader* header)
{
    if (!ParseHeaderName(parser, &header->name)) {
        return false;
    }
    if (!ParseHeaderValue(parser, &header->value)) {
        return false;
    }
    return true;
}

static bool ParseHeaderList(HttpRequestParser* parser, HttpHeader* headers,
                            size_t* headers_count)
{
    size_t current_headers_count = 0;
    while (!ParseRequiredString(parser, CREATE_STRING("\r\n"))) {
        if (current_headers_count == MAX_HEADERS_COUNT) {
            return false;
        }
        if (!ParseHeader(parser, &headers[current_headers_count])) {
            return false;
        }
        current_headers_count++;
    }
    *headers_count = current_headers_count;
    return true;
}

bool ParseRequest(HttpRequestParser* parser, HttpRequest* request)
{
    if (!ParseMethod(parser, &request->method)) {
        printf("ERR: Method parsing fail\n");
        return false;
    }
    if (!ParseTarget(parser, &request->target)) {
        printf("ERR: Target parsing fail\n");
        return false;
    }
    if (!ParseVersion(parser, &request->http_version_minor)) {
        printf("ERR: Version parsing fail\n");
        return false;
    }
    if (!ParseHeaderList(parser, request->headers, &request->headers_count)) {
        printf("ERR: Header list parsing fail\n");
        return false;
    }
    return true;
}
