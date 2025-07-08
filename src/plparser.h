#ifndef PARSER_H
#define PARSER_H

#include <PlToken>

typedef struct PlParser
{
    PlToken cur;
    PlToken prev;
    PlToken prev2;
    PlToken prev3;
    bool error;
    bool panic;
} PlParser;

#endif // PLPARSER_H
