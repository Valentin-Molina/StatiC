#ifndef STRING_VIEW_H
#define STRING_VIEW_H

#include <stddef.h>
#include <stdbool.h>

#define CREATE_STRING(s)                                                       \
    (StringView) { (s), sizeof(s) - 1 }

typedef struct
{
    const char* data;
    size_t len;
} StringView;

#endif // STRING_VIEW_H
