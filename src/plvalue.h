#ifndef PLVALUE_H
#define PLVALUE_H

#include <string.h>

#include <PlGlobals>
#include <PlValueArray>

typedef struct PlObject PlObject;
typedef struct PlString PlString;

#define PL_SIGN_BIT ((uint64_t)0x8000000000000000)
#define PL_QNAN     ((uint64_t)0x7ffc000000000000)

#define PL_TAG_NULL  1
#define PL_TAG_FALSE 2
#define PL_TAG_TRUE  3
#define PL_TAG_EMPTY 4

#define PL_IS_BOOL(value)       (((value) | 1)                       ==  PL_TRUE_VALUE)
#define PL_IS_EMPTY(value)       ((value)                            ==  PL_EMPTY_VALUE)
#define PL_IS_NULL(value)        ((value)                            ==  PL_NULL_VALUE)
#define PL_IS_NUMERAL(value)    (((value) &  PL_QNAN)                !=  PL_QNAN)
#define PL_IS_OBJECT(value)     (((value) & (PL_QNAN | PL_SIGN_BIT)) == (PL_QNAN | PL_SIGN_BIT))

#define PL_AS_BOOL(value)      ((value) == PL_TRUE_VALUE)
#define PL_AS_NUMERAL(value)     pl_valueToNum(value)
#define PL_AS_OBJECT(value)    ((PlObject*)(uintptr_t)((value) & ~(PL_SIGN_BIT | PL_QNAN)))

#define PL_BOOL_VALUE(b)      ((b) ? PL_TRUE_VALUE : PL_FALSE_VALUE)
#define PL_EMPTY_VALUE        ((PlValue)(uint64_t)(PL_QNAN | PL_TAG_EMPTY))
#define PL_FALSE_VALUE        ((PlValue)(uint64_t)(PL_QNAN | PL_TAG_FALSE))
#define PL_TRUE_VALUE         ((PlValue)(uint64_t)(PL_QNAN | PL_TAG_TRUE))
#define PL_NULL_VALUE         ((PlValue)(uint64_t)(PL_QNAN | PL_TAG_NULL))
#define PL_NUMERAL_VALUE(num) pl_numToValue(num)
#define PL_OBJECT_VALUE(obj)  (PlValue)(PL_SIGN_BIT | PL_QNAN | (uint64_t)(uintptr_t)(obj))

static inline double pl_valueToNum(PlValue value)
{
    double num;
    memcpy(&num, &value, sizeof(PlValue));
    return num;
}

static inline PlValue pl_numToValue(double numeral)
{
    PlValue value;
    memcpy(&value, &numeral, sizeof(double));
    return value;
}

bool pl_compValues(PlValue first, PlValue second);
uint32_t pl_hashValue(PlValue value, bool *ok);
bool pl_hashable(PlValue value);
void pl_reprValues(PlValue value);

void pl_reprValuesString(PlValue value, char *string);

#endif // PLVALUE_H
