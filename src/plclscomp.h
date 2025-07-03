#ifndef PLCLSCOMP_H
#define PLCLSCOMP_H

#include <PlGlobals>

typedef struct PlClsComp
{
    struct PlClsComp *parent;
    bool hasSuper;
} PlClsComp;

#endif // PLCLSCOMP_H
