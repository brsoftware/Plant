#include <stdlib.h>

#include <PlMemory>
#include <PlValueArray>

void pl_initValueArray(PlValueArray *array)
{
    array->values = NULL;
    array->capacity = 0;
    array->count = 0;
}

void pl_writeValueArray(PlValueArray *array, PlValue value)
{
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;
        array->capacity = PL_GROW_CAPAC(oldCapacity);
        array->values = PL_GROW_ARRAY(PlValue, array->values, oldCapacity, array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

void pl_freeValueArray(PlValueArray *array)
{
    PL_FREE_ARRAY(PlValue, array->values, array->capacity);
    pl_initValueArray(array);
}

void pl_insertValueArray(PlValueArray *array, PlValue input, int index)
{
    if (index < 0 || index > array->count)
        return;

    if (array->count == array->capacity)
    {
        int oldCapacity = array->capacity;
        array->capacity = PL_GROW_CAPAC(array->capacity);
        array->values = PL_GROW_ARRAY(PlValue, array->values, oldCapacity, array->capacity);
    }

    for (int idx = array->count; idx > index; idx--)
    {
        array->values[idx] = array->values[idx - 1];
    }

    array->values[index] = input;
    array->count++;
}

void pl_reverseValueArray(PlValueArray *array)
{
    for (int index = 0; index < array->count / 2; index++)
    {
        PlValue temp = array->values[index];
        array->values[index] = array->values[array->count - index - 1];
        array->values[array->count - index - 1] = temp;
    }
}

void pl_concatValueArray(PlValueArray *first, PlValueArray *second)
{
    for (int index = 0; index < second->count; index++)
    {
        pl_writeValueArray(first, second->values[index]);
    }
}
