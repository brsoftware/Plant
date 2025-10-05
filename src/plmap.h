#ifndef PLMAP_H
#define PLMAP_H

#include <PlMapItem>

typedef struct PlMap
{
    int count;
    int capacity;
    PlMapItem *items;
} PlMap;

void pl_initMap(PlMap *hash);
void pl_freeMap(PlMap *hash);
bool pl_mapGet(const PlMap *hash, PlValue key, PlValue *value);
bool pl_mapSet(PlMap *hash, PlValue key, PlValue value);
bool pl_mapDel(const PlMap *hash, PlValue key);
bool pl_mapHas(const PlMap *hash, PlValue key);
void pl_mapAdd(const PlMap *from, PlMap *to);
bool pl_mapIsEqual(const PlMap *first, const PlMap *second);
PlString *pl_mapFindString(const PlMap *map, const char *chars, int length, uint32_t hash);

void pl_mapRemoveWhite(const PlMap *map);
void pl_markMap(const PlMap *map);

#endif // PLMAP_H
