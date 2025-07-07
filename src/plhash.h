#ifndef PLHASH_H
#define PLHASH_H

#include <PlHashEntry>

typedef struct PlHash
{
    int count;
    int capacity;
    PlHashEntry *entries;
} PlHash;

void pl_initHash(PlHash *hash);
void pl_freeHash(PlHash *hash);
bool pl_hashGet(const PlHash *hash, const PlString *key, PlValue *value);
bool pl_hashSet(PlHash *hash, PlString *key, PlValue value);
bool pl_hashDel(const PlHash *hash, const PlString *key);
void pl_hashAdd(const PlHash *from, PlHash *to);
PlString *pl_hashFindString(const PlHash *table, const char *chars, int length, uint32_t hash);

void pl_hashRemoveWhite(const PlHash *hash);
void pl_markHash(const PlHash *hash);

#endif // PLHASH_H
