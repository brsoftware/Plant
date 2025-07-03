#ifndef PLPARSERULE_H
#define PLPARSERULE_H

#include <PlPrecedence>

typedef void (*PlParseFunc)(bool assignable);

typedef struct PlParseRule
{
    PlParseFunc prefix;
    PlParseFunc infix;
    PlPrecedence precedence;
} PlParseRule;


#endif // PLPARSERULE_H
