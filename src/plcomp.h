#ifndef PLCOMP_H
#define PLCOMP_H

#include <PlLocal>
#include <PlObject>
#include <PlSurval>

typedef enum PlFuncType
{
    PL_FUNCTYPE_FUNC,
    PL_FUNCTYPE_INIT,
    PL_FUNCTYPE_METHOD,
    PL_FUNCTYPE_SCRIPT,
    PL_FUNCTYPE_LAMBDA
} PlFuncType;

typedef struct PlComp
{
    struct PlComp *parent;
    PlFunction *func;
    PlFuncType type;

    PlLocal locals[UINT8_COUNT * 2];
    int localCount;
    PlSurval survalues[UINT8_COUNT * 2];
    int scopeDepth;
} PlComp;

#endif // PLCOMP_H
