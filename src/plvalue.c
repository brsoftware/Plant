#include <stdio.h>
#include <math.h>

#include <PlMemory>

static uint32_t pl_hashDouble(double value)
{
    union PlBitCast
    {
        double value;
        uint32_t ints[2];
    };

    union PlBitCast cast;
    cast.value = value + 1.0;
    return cast.ints[0] + cast.ints[1];
}

bool pl_compValues(PlValue first, PlValue second)
{
    return first == second;
}

uint32_t pl_hashValue(PlValue value, bool *ok)
{
    *ok = true;

    if (PL_IS_BOOL(value))
    {
        return PL_AS_BOOL(value) ? 3 : 5;
    }

    if (PL_IS_NULL(value))
    {
        return 7;
    }

    if (PL_IS_NUMERAL(value))
    {
        return pl_hashDouble(PL_AS_NUMERAL(value));
    }

    if (PL_IS_OBJECT(value))
    {
        if (PL_IS_STRING(value))
        {
            return PL_AS_STRING(value)->hash;
        }
    }

    if (PL_IS_EMPTY(value))
    {
        return 0;
    }

    *ok = false;
    return 0;
}

bool pl_hashable(PlValue value)
{
    if (PL_IS_BOOL(value))
        return true;

    if (PL_IS_NULL(value))
        return true;

    if (PL_IS_NUMERAL(value))
        return true;

    if (PL_IS_EMPTY(value))
        return true;

    if (PL_IS_STRING(value))
        return true;

    return false;
}

void pl_reprValues(PlValue value)
{
    if (PL_IS_BOOL(value))
    {
        printf(PL_AS_BOOL(value) ? "true" : "false");
    }

    else if (PL_IS_NULL(value))
    {
        printf("null");
    }

    else if (PL_IS_NUMERAL(value))
    {
        if (floor(PL_AS_NUMERAL(value)) == PL_AS_NUMERAL(value))
            printf("%.0f", PL_AS_NUMERAL(value));
        else
            printf("%f", PL_AS_NUMERAL(value));
    }

    else if (PL_IS_OBJECT(value))
    {
        pl_reprObject(value);
    }
}

void pl_reprValuesString(PlValue value, char *stream)
{
    if (PL_IS_BOOL(value))
    {
        sprintf(stream, PL_AS_BOOL(value) ? "true" : "false");
    }

    else if (PL_IS_NULL(value))
    {
        sprintf(stream, "null");
    }

    else if (PL_IS_NUMERAL(value))
    {
        sprintf(stream, "%g", PL_AS_NUMERAL(value));
    }

    else if (PL_IS_OBJECT(value))
    {
        pl_reprObjectString(value, stream);
    }
}
