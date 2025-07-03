#ifndef PLLOCAL_H
#define PLLOCAL_H

#include <PlGlobals>
#include <PlToken>

typedef struct PlLocal
{
    PlToken name;
    int depth;
    bool captured;
} PlLocal;

#endif // PLLOCAL_H
