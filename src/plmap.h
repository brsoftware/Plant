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
bool pl_mapGet(PlMap *hash, PlValue key, PlValue *value);
bool pl_mapSet(PlMap *hash, PlValue key, PlValue value);
bool pl_mapDel(PlMap *hash, PlValue key);
void pl_mapAdd(PlMap *from, PlMap *to);
PlString *pl_mapFindString(PlMap *map, const char *chars, int length, uint32_t hash);

void pl_mapRemoveWhite(PlMap *map);
void pl_markMap(PlMap *map);

#endif // PLMAP_H
