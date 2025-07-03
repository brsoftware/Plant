#ifndef PLTOKEN_H
#define PLTOKEN_H

#include <PlTokenType>

typedef struct PlToken
{
    PlTokenType type;
    const char *start;
    int length;
    int line;
    bool alloc;
} PlToken;

#endif // PLTOKEN_H
