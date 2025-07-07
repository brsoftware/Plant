#include <stdlib.h>
#include <string.h>

#include <PlMemory>
#include <PlVM>

#define PL_HASH_LOAD 0.75

static void pl_adjustCapac(PlHash *hash, int capacity);
static PlHashEntry *pl_findEntry(PlHashEntry *entries, int capacity, const PlString *key);

void pl_initHash(PlHash *hash)
{
    hash->count = 0;
    hash->capacity = 0;
    hash->entries = NULL;
}

void pl_freeHash(PlHash *hash)
{
    PL_FREE_ARRAY(PlHashEntry, hash->entries, hash->capacity);
    pl_initHash(hash);
}

bool pl_hashGet(const PlHash *hash, const PlString *key, PlValue *value)
{
    if (hash->count == 0)
        return false;

    PlHashEntry *entry = pl_findEntry(hash->entries, hash->capacity, key);
    if (entry->key == NULL)
        return false;

    *value = entry->value;
    return true;
}

bool pl_hashSet(PlHash *hash, PlString *key, PlValue value)
{
    if (hash->count + 1 > hash->capacity * PL_HASH_LOAD)
    {
        int capacity = PL_GROW_CAPAC(hash->capacity);
        pl_adjustCapac(hash, capacity);
    }

    PlHashEntry *entry = pl_findEntry(hash->entries, hash->capacity, key);
    bool isNewKey = entry->key == NULL;
    if (isNewKey && PL_IS_NULL(entry->value))
        hash->count++;

    entry->key = key;
    entry->value = value;
    return isNewKey;
}

bool pl_hashDel(const PlHash *hash, const PlString *key)
{
    if (hash->count == 0)
        return false;

    PlHashEntry *entry = pl_findEntry(hash->entries, hash->capacity, key);
    if (entry->key == NULL)
        return false;

    entry->key = NULL;
    entry->value = PL_BOOL_VALUE(true);
    return true;
}

void pl_hashAdd(const PlHash *from, PlHash *to)
{
    for (int index = 0; index < from->capacity; index++)
    {
        PlHashEntry *entry = &from->entries[index];

        if (entry->key != NULL)
        {
            pl_hashSet(to, entry->key, entry->value);
        }
    }
}

PlString *pl_hashFindString(const PlHash *table, const char *chars, int length, uint32_t hash)
{
    if (table->count == 0)
        return NULL;

    uint32_t index = hash & (table->capacity - 1);

    for (;;)
    {
        PlHashEntry *entry = &table->entries[index];

        if (entry->key == NULL)
        {
            if (PL_IS_NULL(entry->value))
                return NULL;
        }

        else if (entry->key->length == length && entry->key->hash == hash &&
            memcmp(entry->key->chars, chars, length) == 0)
        {
            return entry->key;
        }

        index = (index + 1) & (table->capacity - 1);
    }
}

void pl_hashRemoveWhite(const PlHash *hash)
{
    for (int index = 0; index < hash->capacity; index++)
    {
        PlHashEntry *entry = &hash->entries[index];

        if (entry->key != NULL && !entry->key->obj.mark != vm.markValue)
        {
            pl_hashDel(hash, entry->key);
        }
    }
}

void pl_markHash(const PlHash *hash)
{
    for (int index = 0; index < hash->capacity; index++)
    {
        PlHashEntry *entry = &hash->entries[index];
        pl_markObject((PlObject*)entry->key);
        pl_markValue(entry->value);
    }
}

static void pl_adjustCapac(PlHash *hash, int capacity)
{
    PlHashEntry *entries = PL_ALLOC(PlHashEntry, capacity);

    for (int index = 0; index < capacity; index++)
    {
        entries[index].key = NULL;
        entries[index].value = PL_NULL_VALUE;
    }

    hash->count = 0;

    for (int index = 0; index < hash->capacity; index++)
    {
        PlHashEntry *entry = &hash->entries[index];
        if (entry->key == NULL)
            continue;

        PlHashEntry *dest = pl_findEntry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        hash->count++;
    }

    PL_FREE_ARRAY(PlHashEntry, hash->entries, hash->capacity);
    hash->entries = entries;
    hash->capacity = capacity;
}

static PlHashEntry *pl_findEntry(PlHashEntry *entries, int capacity, const PlString *key)
{
    uint32_t index = key->hash & (capacity - 1);
    PlHashEntry *remark = NULL;

    for (;;)
    {
        PlHashEntry *entry = &entries[index];

        if (entry->key == NULL)
        {
            if (PL_IS_NULL(entry->value))
            {
                return remark != NULL ? remark : entry;
            }

            else
            {
                if (remark == NULL)
                    remark = entry;
            }
        }

        else if (entry->key == key)
        {
            return entry;
        }

        index = (index + 1) & (capacity - 1);
    }
}
