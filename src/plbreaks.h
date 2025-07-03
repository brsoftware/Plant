#ifndef PLBREAKS_H
#define PLBREAKS_H

typedef struct PlBreaks
{
    struct PlBreaks *parent;
    int *breaks;
    int count;
    int capacity;
} PlBreaks;

#endif // PLBREAKS_H
