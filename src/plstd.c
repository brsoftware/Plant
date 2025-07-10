#include <math.h>
#include <stdlib.h>

#include <PlStd>
#include <PlVM>

PlHash pl_stdMembers;

static bool pl_builtin_abs(int argCount, PlValue *args);
static bool pl_builtin_iostream_cout(int argCount, PlValue *args);
static bool pl_builtin_plant_delAttr(int argCount, PlValue *args);
static bool pl_builtin_exit(int argCount, PlValue *args);
static bool pl_builtin_plant_getAttr(int argCount, PlValue *args);
static bool pl_builtin_iostream_getLine(int argCount, PlValue *args);
static bool pl_builtin_plant_hasAttr(int argCount, PlValue *args);
static bool pl_builtin_plant_setAttr(int argCount, PlValue *args);
static bool pl_builtin_any_toBool(int argCount, PlValue *args);
static bool pl_builtin_any_toChar(int argCount, PlValue *args);
static bool pl_builtin_any_toNum(int argCount, PlValue *args);
static bool pl_builtin_any_toString(int argCount, PlValue *args);

void pl_initStd()
{
#define PL_STD_SET(key, len, builtin) \
    pl_hashSet(&pl_stdMembers, pl_copyString(key, len), PL_OBJECT_VALUE(pl_newBuiltin(builtin)));

    pl_initHash(&pl_stdMembers);

    PL_STD_SET("abs", 3, pl_builtin_abs);
    PL_STD_SET("iostream.cout", 13, pl_builtin_iostream_cout);
    PL_STD_SET("plant.delAttr", 13, pl_builtin_plant_delAttr);
    PL_STD_SET("exit", 4, pl_builtin_exit);
    PL_STD_SET("plant.getAttr", 13, pl_builtin_plant_getAttr);
    PL_STD_SET("iostream.getLine", 15, pl_builtin_iostream_getLine);
    PL_STD_SET("plant.hasAttr", 13, pl_builtin_plant_hasAttr);
    PL_STD_SET("plant.setAttr", 13, pl_builtin_plant_setAttr);
    PL_STD_SET("any.toBool", 10, pl_builtin_any_toBool);
    PL_STD_SET("any.toChar", 10, pl_builtin_any_toChar);
    PL_STD_SET("any.toNum", 9, pl_builtin_any_toNum);
    PL_STD_SET("any.toString", 12, pl_builtin_any_toString);
#undef PL_STD_SET
}

void pl_freeStd()
{
    pl_freeHash(&pl_stdMembers);
}

static bool pl_builtin_abs(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'abs'.", 35));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);

        if (value < 0)
        {
            args[-1] = PL_NUMERAL_VALUE(0 - value);
            return true;
        }

        args[-1] = args[0];
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_iostream_cout(int argCount, PlValue *args)
{
    for (int index = 0; index < argCount; index++)
    {
        pl_reprValues(args[index]);
    }

    args[-1] = PL_NULL_VALUE;
    return true;
}

static bool pl_builtin_plant_delAttr(int argCount, PlValue *args)
{
    if (argCount != 2)
    {
        args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 2.", 40));
        return false;
    }

    if (!PL_IS_INSTANCE(args[0]))
    {
        args[-1] = PL_OBJECT_VALUE(pl_copyString("Expect an instance at the first argument.", 41));
        return false;
    }

    if (!PL_IS_STRING(args[1]))
    {
        args[-1] = PL_OBJECT_VALUE(pl_copyString("Expect a field name as string at the second argument.", 53));
        return false;
    }

    PlInstance *instance = PL_AS_INSTANCE(args[0]);
    args[-1] = PL_BOOL_VALUE(pl_hashDel(&instance->fields, PL_AS_STRING(args[1])));
    return true;
}

static bool pl_builtin_exit(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        int code = (int)PL_AS_NUMERAL(args[0]);
        exit(code);
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1 to indicate exit code.", 62));
    return false;
}

static bool pl_builtin_plant_getAttr(int argCount, PlValue *args)
{
    if (!(argCount == 2 || argCount == 3))
    {
        args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 2 or 3.", 45));
        return false;
    }

    if (!PL_IS_INSTANCE(args[0]))
    {
        args[-1] = PL_OBJECT_VALUE(pl_copyString("Expect an instance at the first argument.", 41));
        return false;
    }

    if (!PL_IS_STRING(args[1]))
    {
        args[-1] = PL_OBJECT_VALUE(pl_copyString("Expect a field name as string at the second argument.", 53));
        return false;
    }

    PlInstance *instance = PL_AS_INSTANCE(args[0]);
    PlValue value;
    bool result = pl_hashGet(&instance->fields, PL_AS_STRING(args[1]), &value);

    if (!result)
    {
        if (argCount == 2)
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("The field does not exist.", 25));
            return false;
        }

        args[-1] = args[2];
        return true;
    }

    args[-1] = value;
    return true;
}

static bool pl_builtin_iostream_getLine(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (PL_IS_STRING(args[0]))
        {
            printf("%s", PL_AS_STRING(args[0])->chars);

            char line[1024];

            if (!fgets(line, sizeof(line), stdin))
            {
                printf("\n");
                args[-1] = PL_OBJECT_VALUE(pl_copyString("", 0));
                return true;
            }

            args[-1] = PL_OBJECT_VALUE(pl_copyString(line, strlen(line) - 1));
            return true;
        }

        args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'getLine'.", 39));
        return false;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_plant_hasAttr(int argCount, PlValue *args)
{
    if (argCount != 2)
    {
        args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 2.", 40));
        return false;
    }

    if (!PL_IS_INSTANCE(args[0]))
    {
        args[-1] = PL_OBJECT_VALUE(pl_copyString("Expect an instance at the first argument.", 41));
        return false;
    }

    if (!PL_IS_STRING(args[1]))
    {
        args[-1] = PL_OBJECT_VALUE(pl_copyString("Expect a field name as string at the second argument.", 53));
        return false;
    }

    PlInstance *instance = PL_AS_INSTANCE(args[0]);
    PlValue dummy;
    args[-1] = PL_BOOL_VALUE(pl_hashGet(&instance->fields, PL_AS_STRING(args[1]), &dummy));
    return true;
}

static bool pl_builtin_plant_setAttr(int argCount, PlValue *args)
{
    if (argCount != 3)
    {
        args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 2.", 40));
        return false;
    }

    if (!PL_IS_INSTANCE(args[0]))
    {
        args[-1] = PL_OBJECT_VALUE(pl_copyString("Expect an instance at the first argument.", 41));
        return false;
    }

    if (!PL_IS_STRING(args[1]))
    {
        args[-1] = PL_OBJECT_VALUE(pl_copyString("Expect a field name as string at the second argument.", 53));
        return false;
    }

    PlInstance *instance = PL_AS_INSTANCE(args[0]);
    pl_hashSet(&instance->fields, PL_AS_STRING(args[1]), args[2]);
    args[-1] = args[2];
    return true;
}

static bool pl_builtin_any_toBool(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        args[-1] = pl_isFalse(args[0]) ? PL_FALSE_VALUE : PL_TRUE_VALUE;
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_any_toChar(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (PL_IS_STRING(args[0]))
        {
            if (PL_AS_STRING(args[0])->length != 0)
            {
                args[-1] = PL_NUMERAL_VALUE(PL_AS_STRING(args[0])->chars[0]);
                return true;
            }

            args[-1] = PL_OBJECT_VALUE(pl_copyString("Attempting to extract an empty string.", 38));
            return false;
        }

        args[-1] = PL_OBJECT_VALUE(pl_copyString("Cannot convert non-string to a character.", 41));
        return false;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_any_toNum(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (PL_IS_STRING(args[0]))
        {
            char *remaining;
            double value = strtod(PL_AS_STRING(args[0])->chars, &remaining);

            if (strcmp(remaining, "") != 0)
            {
                args[-1] = PL_OBJECT_VALUE(pl_copyString("Invalid literal for 'toNum'.", 28));
                return false;
            }

            args[-1] = PL_NUMERAL_VALUE(value);
            return true;
        }

        if (PL_IS_BOOL(args[0]))
        {
            args[-1] = PL_NUMERAL_VALUE(args[0] == PL_FALSE_VALUE ? 0 : 1);
            return true;
        }

        if (PL_IS_NUMERAL(args[0]))
        {
            args[-1] = args[0];
            return true;
        }

        if (PL_IS_NULL(args[0]))
        {
            args[-1] = PL_NUMERAL_VALUE(0);
            return true;
        }

        args[-1] = PL_OBJECT_VALUE(pl_copyString("Unable to cast to a numeral.", 28));
        return false;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_any_toString(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (PL_IS_NUMERAL(args[0]))
        {
            char buffer[1024];

            if (floor(PL_AS_NUMERAL(args[0])) == PL_AS_NUMERAL(args[0]))
                sprintf(buffer, "%.0f", PL_AS_NUMERAL(args[0]));
            else
                sprintf(buffer, "%f", PL_AS_NUMERAL(args[0]));

            args[-1] = PL_OBJECT_VALUE(pl_copyString(buffer, strlen(buffer)));
            return true;
        }

        if (PL_IS_BOOL(args[0]))
        {
            args[-1] = args[0] == PL_TRUE_VALUE
                ? PL_OBJECT_VALUE(pl_copyString("true", 4))
                : PL_OBJECT_VALUE(pl_copyString("false", 5));
            return true;
        }

        if (PL_IS_STRING(args[0]))
        {
            args[-1] = args[0];
            return true;
        }

        if (PL_IS_NULL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("null", 4));
            return true;
        }

        args[-1] = PL_OBJECT_VALUE(pl_copyString("Unable to cast to a string.", 28));
        return false;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}
