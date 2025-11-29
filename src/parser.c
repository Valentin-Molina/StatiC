#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define CREATE_STRING(s)                                                       \
    (String) { (s), sizeof(s) - 1 }

typedef struct
{
    char* content;
    size_t len;
} String;

static const String token_special_characters = {
    .content = "!#$%&'*+-.^_`|~", .len = sizeof("!#$%&'*+-.^_`|~") - 1};

bool IsDigit(char character) { return character >= '0' && character <= '9'; }

bool IsAlpha(char character)
{
    return (character >= 'a' && character <= 'z') ||
           (character >= 'A' && character <= 'Z');
}

bool IsOneOff(char character, const String* str)
{
    for (size_t i = 0; i < str->len; i++) {
        if (character == *(str->content + i))
            return true;
    }
    return false;
}

bool IsTokenCharacter(char character)
{
    return IsAlpha(character) || IsDigit(character) ||
           IsOneOff(character, &token_special_characters);
}

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

bool HasParserReachedEnd(HttpRequestParser* parser)
{
    return parser->len == parser->cursor;
}

bool ParseRequiredString(HttpRequestParser* parser, String required_string)
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

bool ParseMethod(HttpRequestParser* parser, String* method)
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
    if (HasParserReachedEnd(parser) || !(parser->src[parser->cursor] != ' ')) {
        return false;
    }
    parser->cursor++;
    return true;
}

bool ParseTarget(HttpRequestParser* parser, String* target)
{
    // The target comes after the method.
    size_t init_cursor = parser->cursor;
    target->content    = parser->src + init_cursor;

    // Iterate over the target to get its length
    while (!HasParserReachedEnd(parser) ||
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

bool ParseVersion(HttpRequestParser* parser, int* version_minor)
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
