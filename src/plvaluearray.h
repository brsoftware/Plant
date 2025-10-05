#ifndef PLVALUEARRAY_H
#define PLVALUEARRAY_H

#include <PlGlobals>

typedef uint64_t PlValue;

typedef struct
{
    int capacity;
    int count;
    PlValue *values;
} PlValueArray;

void pl_initValueArray(PlValueArray *array);
void pl_writeValueArray(PlValueArray *array, PlValue value);
void pl_freeValueArray(PlValueArray *array);
void pl_insertValueArray(PlValueArray *array, PlValue input, int index);
void pl_reverseValueArray(PlValueArray *array);
void pl_concatValueArray(PlValueArray *first, PlValueArray *second);
bool pl_valueArrayContains(const PlValueArray *array, PlValue value);
bool pl_valueArrayEquals(const PlValueArray *first, const PlValueArray *second);

#endif // PLVALUEARRAY_H
