#include <stdlib.h>
#include <string.h>

#include <PlMap>
#include <PlMemory>
#include <PlVM>

#define PL_MAP_LOAD 0.75

static void pl_adjustCapac(PlMap *map, int capacity);
static PlMapItem *pl_finditem(PlMapItem *items, int capacity, PlValue key);

void pl_initMap(PlMap *map)
{
    map->count = 0;
    map->capacity = 0;
    map->items = NULL;
}

void pl_freeMap(PlMap *map)
{
    PL_FREE_ARRAY(PlMapItem, map->items, map->capacity);
    pl_initMap(map);
}

bool pl_mapGet(PlMap *map, PlValue key, PlValue *value)
{
    if (map->count == 0)
        return false;

    PlMapItem *item = pl_finditem(map->items, map->capacity, key);

    if (item == NULL)
        return false;

    if (PL_IS_EMPTY(item->key))
        return false;

    *value = item->value;
    return true;
}

bool pl_mapSet(PlMap *map, PlValue key, PlValue value)
{
    if (map->count + 1 > map->capacity * PL_MAP_LOAD)
    {
        int capacity = PL_GROW_CAPAC(map->capacity);
        pl_adjustCapac(map, capacity);
    }

    PlMapItem *item = pl_finditem(map->items, map->capacity, key);

    if (item == NULL)
        return false;

    bool isNewKey = PL_IS_EMPTY(item->key);
    if (isNewKey)
        map->count++;

    item->key = key;
    item->value = value;
    return isNewKey;
}

bool pl_mapDel(PlMap *map, PlValue key)
{
    if (map->count == 0)
        return false;

    PlMapItem *item = pl_finditem(map->items, map->capacity, key);
    if (item == NULL)
        return false;

    if (PL_IS_EMPTY(item->key))
        return false;

    item->key = PL_EMPTY_VALUE;
    item->value = PL_BOOL_VALUE(true);
    return true;
}

void pl_mapAdd(PlMap *from, PlMap *to)
{
    for (int index = 0; index < from->capacity; index++)
    {
        PlMapItem *item = &from->items[index];

        if (!PL_IS_EMPTY(item->key))
        {
            pl_mapSet(to, item->key, item->value);
        }
    }
}

PlString *pl_mapFindString(PlMap *map, const char *chars, int length, uint32_t hash)
{
    if (map->count == 0)
        return NULL;

    uint32_t index = hash & (map->capacity - 1);

    for (;;)
    {
        PlMapItem *item = &map->items[index];

        if (PL_IS_EMPTY(item->key))
        {
            if (PL_IS_NULL(item->value))
                return NULL;
        }

        if (!PL_IS_STRING(item->key))
            return NULL;

        if (PL_AS_STRING(item->key)->length == length && PL_AS_STRING(item->key)->hash == hash &&
            memcmp(PL_AS_STRING(item->key)->chars, chars, length) == 0)
        {
            return PL_AS_STRING(item->key);
        }

        index = (index + 1) & (map->capacity - 1);
    }
}

void pl_mapRemoveWhite(PlMap *map)
{
    for (int index = 0; index < map->capacity; index++)
    {
        PlMapItem *item = &map->items[index];

        if (!PL_IS_OBJECT(item->key))
        {
            continue;
        }

        if (!PL_IS_EMPTY(item->key) && !PL_AS_OBJECT(item->key)->mark != vm.markValue)
        {
            pl_mapDel(map, item->key);
        }
    }
}

void pl_markMap(PlMap *map)
{
    for (int index = 0; index < map->capacity; index++)
    {
        PlMapItem *item = &map->items[index];

        if (!PL_IS_OBJECT(item->key))
        {
            continue;
        }

        pl_markObject((PlObject*)item->key);
        pl_markValue(item->value);
    }
}

static void pl_adjustCapac(PlMap *map, int capacity)
{
    PlMapItem *items = PL_ALLOC(PlMapItem, capacity);

    for (int index = 0; index < capacity; index++)
    {
        items[index].key = PL_EMPTY_VALUE;
        items[index].value = PL_NULL_VALUE;
    }

    map->count = 0;

    for (int index = 0; index < map->capacity; index++)
    {
        PlMapItem *item = &map->items[index];
        if (PL_IS_EMPTY(item->key))
            continue;

        PlMapItem *dest = pl_finditem(items, capacity, item->key);
        if (dest == NULL)
            continue;
        dest->key = item->key;
        dest->value = item->value;
        map->count++;
    }

    PL_FREE_ARRAY(PlMapItem, map->items, map->capacity);
    map->items = items;
    map->capacity = capacity;
}

static PlMapItem *pl_finditem(PlMapItem *items, int capacity, PlValue key)
{
    bool canPass = false;
    uint32_t index = pl_hashValue(key, &canPass) & (capacity - 1);

    if (!canPass)
        return NULL;

    PlMapItem *remark = NULL;

    for (;;)
    {
        PlMapItem *item = &items[index];

        if (PL_IS_EMPTY(item->key))
        {
            if (PL_IS_NULL(item->value))
            {
                return remark != NULL ? remark : item;
            }

            else
            {
                if (remark == NULL)
                    remark = item;
            }
        }

        else if (pl_compValues(key, item->key))
        {
            return item;
        }

        index = (index + 1) & (capacity - 1);
    }
}
