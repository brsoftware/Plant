#include <PlMemory>
#include <PlObject>
#include <PlSet>
#include <PlVM>

#define PL_SET_LOAD 0.75

static void pl_adjustCapac(PlSet *set, int capacity);
static PlValue *pl_findElement(PlValue *elements, int capacity, PlValue element);
static bool pl_internalSetHas(const PlSet *set, PlValue *value);

void pl_initSet(PlSet *set)
{
    set->count = 0;
    set->capacity = 0;
    set->elements = NULL;
}

void pl_freeSet(PlSet *set)
{
    PL_FREE_ARRAY(PlValue, set->elements, set->capacity);
    pl_initSet(set);
}

bool pl_setSet(PlSet *set, PlValue element)
{
    if (set->count + 1 > set->capacity * PL_SET_LOAD)
    {
        int capacity = PL_GROW_CAPAC(set->capacity);
        pl_adjustCapac(set, capacity);
    }

    PlValue *elem = pl_findElement(set->elements, set->capacity, element);

    if (elem == NULL)
        return false;

    bool isNewElem = PL_IS_EMPTY(*elem);
    if (isNewElem)
        set->count++;

    *elem = element;
    return isNewElem;
}

bool pl_setDel(const PlSet *set, PlValue element)
{
    if (set->count == 0)
        return false;

    PlValue *elem = pl_findElement(set->elements, set->capacity, element);
    if (elem == NULL)
        return false;

    if (PL_IS_EMPTY(*elem))
        return false;

    *elem = PL_EMPTY_VALUE;
    return true;
}

bool pl_setHas(const PlSet *set, PlValue element)
{
    if (set->count == 0)
        return false;

    PlValue *elem = pl_findElement(set->elements, set->capacity, element);
    if (elem == NULL)
        return false;

    if (PL_IS_EMPTY(*elem))
        return false;

    return true;
}

bool pl_isSubset(const PlSet *set, const PlSet *subset)
{
    if (subset->count > set->count)
        return false;

    if (subset->count == 0)
        return true;

    for (int index = 0; index < subset->capacity; index++)
    {
        if (PL_IS_EMPTY(subset->elements[index]))
            continue;

        if (!pl_internalSetHas(set, &subset->elements[index]))
            return false;
    }

    return true;
}

bool pl_isEqualSets(const PlSet *first, const PlSet *second)
{
    if (first->count != second->count)
        return false;

    for (int index = 0; index < first->capacity; index++)
    {
        if (PL_IS_EMPTY(first->elements[index]))
            continue;

        if (!pl_internalSetHas(second, &first->elements[index]))
            return false;
    }

    return true;
}

PlSetObject *pl_setUnion(const PlSet *first, const PlSet *second)
{
    PlSetObject *setObject = pl_newSet();
    PlSet *set = &setObject->set;

    for (int index = 0; index < first->capacity; index++)
    {
        PlValue *value = &first->elements[index];

        if (!PL_IS_EMPTY(*value))
        {
            pl_setSet(set, *value);
        }
    }

    for (int index = 0; index < second->capacity; index++)
    {
        PlValue *value = &second->elements[index];

        if (!PL_IS_EMPTY(*value))
        {
            pl_setSet(set, *value);
        }
    }

    return setObject;
}

PlSetObject *pl_setIntersection(const PlSet *first, const PlSet *second)
{
    PlSetObject *setObject = pl_newSet();
    PlSet *set = &setObject->set;

    for (int index = 0; index < first->capacity; index++)
    {
        PlValue *value = &first->elements[index];

        if (!PL_IS_EMPTY(*value))
        {
            if (pl_internalSetHas(second, value))
            {
                pl_setSet(set, *value);
            }
        }
    }

    return setObject;
}

PlSetObject *pl_setDifference(const PlSet *first, const PlSet *second)
{
    PlSetObject *setObject = pl_newSet();
    PlSet *set = &setObject->set;

    for (int index = 0; index < first->capacity; index++)
    {
        PlValue *value = &first->elements[index];

        if (!PL_IS_EMPTY(*value))
        {
            if (pl_internalSetHas(second, value))
            {
                continue;
            }

            pl_setSet(set, *value);
        }
    }

    return setObject;
}

PlSetObject *pl_setSymmetricDifference(const PlSet *first, const PlSet *second)
{
    PlSetObject *setObject = pl_newSet();
    PlSet *set = &setObject->set;

    for (int index = 0; index < first->capacity; index++)
    {
        PlValue *value = &first->elements[index];

        if (!PL_IS_EMPTY(*value))
        {
            if (pl_internalSetHas(second, value))
            {
                continue;
            }

            pl_setSet(set, *value);
        }
    }

    for (int index = 0; index < second->capacity; index++)
    {
        PlValue *value = &second->elements[index];

        if (!PL_IS_EMPTY(*value))
        {
            if (pl_internalSetHas(first, value))
            {
                continue;
            }

            pl_setSet(set, *value);
        }
    }

    return setObject;
}

void pl_setRemoveWhite(const PlSet *set)
{
    for (int index = 0; index < set->capacity; index++)
    {
        PlValue *elem = &set->elements[index];

        if (!PL_IS_OBJECT(*elem))
        {
            continue;
        }

        if (!PL_IS_EMPTY(*elem) && !PL_AS_OBJECT(*elem)->mark != vm.markValue)
        {
            pl_setDel(set, *elem);
        }
    }
}

void pl_markSet(const PlSet *set)
{
    for (int index = 0; index < set->capacity; index++)
    {
        PlValue *elem = &set->elements[index];

        if (!PL_IS_OBJECT(*elem))
        {
            continue;
        }

        pl_markObject((PlObject*)elem);
        pl_markValue(*elem);
    }
}

static void pl_adjustCapac(PlSet *set, int capacity)
{
    PlValue *elem = PL_ALLOC(PlValue, capacity);

    for (int index = 0; index < capacity; index++)
    {
        elem[index] = PL_EMPTY_VALUE;
    }

    set->count = 0;

    for (int index = 0; index < set->capacity; index++)
    {
        PlValue *element = &set->elements[index];
        if (PL_IS_EMPTY(*element))
            continue;

        PlValue *dest = pl_findElement(elem, capacity, *element);
        if (dest == NULL)
            continue;
        *dest = *element;
        set->count++;
    }

    PL_FREE_ARRAY(PlMapItem, set->elements, set->capacity);
    set->elements = elem;
    set->capacity = capacity;
}

static PlValue *pl_findElement(PlValue *elements, int capacity, PlValue element)
{
    bool canPass = false;
    uint32_t index = pl_hashValue(element, &canPass) & (capacity - 1);

    if (!canPass)
        return NULL;

    for (;;)
    {
        PlValue *elem = &elements[index];

        if (PL_IS_EMPTY(*elem))
        {
            return elem;
        }

        else if (pl_compValues(element, *elem))
        {
            return elem;
        }

        index = (index + 1) & (capacity - 1);
    }
}

static bool pl_internalSetHas(const PlSet *set, PlValue *value)
{
    PlValue *elem = pl_findElement(set->elements, set->capacity, *value);
    if (elem == NULL || PL_IS_EMPTY(*elem))
        return false;
    return true;
}
