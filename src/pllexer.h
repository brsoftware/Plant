#ifndef PLLEXER_H
#define PLLEXER_H

#include <PlGlobals>
#include <PlToken>

void pl_initLexer(const char *source);
PlToken pl_scanToken();

#endif // PLLEXER_H
