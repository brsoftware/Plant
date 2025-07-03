#ifndef PLMEMORY_H
#define PLMEMORY_H

#include <PlObject>

#define PL_ALLOC(type, count) (type*)pl_realloc(NULL, 0, sizeof(type) * (count))
#define PL_FREE(type, ptr) pl_realloc(ptr, sizeof(type), 0)
#define PL_GROW_CAPAC(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)
#define PL_GROW_ARRAY(type, ptr, old, n) (type*)pl_realloc(ptr, sizeof(type) * (old), sizeof(type) * (n))
#define PL_FREE_ARRAY(type, ptr, old) pl_realloc(ptr, sizeof(type) * (old), 0)

void *pl_realloc(void *ptr, size_t oldSize, size_t newSize);
void pl_markObject(PlObject *object);
void pl_markValue(PlValue value);
void pl_collectGarbage();
void pl_freeObjects();

char *pl_strndup(const char *str, int chars);

#endif // PLMEMORY_H
