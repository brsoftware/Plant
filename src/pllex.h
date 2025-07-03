#ifndef PLLEX_H
#define PLLEX_H

typedef struct PlLex
{
    const char *start;
    const char *current;
    int line;
} PlLex;

#endif // PLLEX_H
