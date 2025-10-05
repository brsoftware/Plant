#ifndef PLSET_H
#define PLSET_H

#include <PlValue>

typedef struct PlSetObject PlSetObject;

typedef struct PlSet
{
    int count;
    int capacity;
    PlValue *elements;
} PlSet;

void pl_initSet(PlSet *set);
void pl_freeSet(PlSet *set);
bool pl_setSet(PlSet *set, PlValue element);
bool pl_setDel(const PlSet *set, PlValue element);
bool pl_setHas(const PlSet *set, PlValue element);
bool pl_isSubset(const PlSet *set, const PlSet *subset);
bool pl_isEqualSets(const PlSet *first, const PlSet *second);
PlSetObject *pl_setUnion(const PlSet *first, const PlSet *second);
PlSetObject *pl_setIntersection(const PlSet *first, const PlSet *second);
PlSetObject *pl_setDifference(const PlSet *first, const PlSet *second);
PlSetObject *pl_setSymmetricDifference(const PlSet *first, const PlSet *second);

void pl_setRemoveWhite(const PlSet *set);
void pl_markSet(const PlSet *set);

#endif // PLSET_H
