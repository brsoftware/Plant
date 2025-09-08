#ifndef PLLOCAL_H
#define PLLOCAL_H

#include <PlGlobals>
#include <PlToken>

typedef enum PlLocalAttr
{
    PL_LOCAL_CAPTURED = 0x1,
    PL_LOCAL_DELETED = 0x2,
    PL_LOCAL_CONSTANT = 0x4
} PlLocalAttr;

typedef struct PlLocal
{
    PlToken name;
    int depth;
    uint8_t attr;
} PlLocal;

#endif // PLLOCAL_H
