#ifndef PLHASHENTRY_H
#define PLHASHENTRY_H

#include <PlValue>

typedef struct PlHashEntry
{
    PlString *key;
    PlValue value;
} PlHashEntry;

#endif // PLHASHENTRY_H
