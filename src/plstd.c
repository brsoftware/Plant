#include <math.h>
#include <stdlib.h>

#include <PlStd>
#include <PlSupplementary>
#include <PlVM>

PlHash pl_stdMembers;

static bool pl_builtin_abs(int argCount, PlValue *args);
static bool pl_builtin_math_acos(int argCount, PlValue *args);
static bool pl_builtin_math_acosh(int argCount, PlValue *args);
static bool pl_builtin_math_asin(int argCount, PlValue *args);
static bool pl_builtin_math_asinh(int argCount, PlValue *args);
static bool pl_builtin_math_atan(int argCount, PlValue *args);
static bool pl_builtin_math_atan2(int argCount, PlValue *args);
static bool pl_builtin_math_atanh(int argCount, PlValue *args);
static bool pl_builtin_math_cbrt(int argCount, PlValue *args);
static bool pl_builtin_math_ceil(int argCount, PlValue *args);
static bool pl_builtin_math_comb(int argCount, PlValue *args);
static bool pl_builtin_math_copySign(int argCount, PlValue *args);
static bool pl_builtin_math_cos(int argCount, PlValue *args);
static bool pl_builtin_math_cosh(int argCount, PlValue *args);
static bool pl_builtin_iostream_cout(int argCount, PlValue *args);
static bool pl_builtin_math_degrees(int argCount, PlValue *args);
static bool pl_builtin_plant_delAttr(int argCount, PlValue *args);
static bool pl_builtin_math_erf(int argCount, PlValue *args);
static bool pl_builtin_math_erfc(int argCount, PlValue *args);
static bool pl_builtin_exit(int argCount, PlValue *args);
static bool pl_builtin_math_exp(int argCount, PlValue *args);
static bool pl_builtin_math_exp2(int argCount, PlValue *args);
static bool pl_builtin_math_expm1(int argCount, PlValue *args);
static bool pl_builtin_math_factorial(int argCount, PlValue *args);
static bool pl_builtin_math_floor(int argCount, PlValue *args);
static bool pl_builtin_math_fmod(int argCount, PlValue *args);
static bool pl_builtin_plant_getAttr(int argCount, PlValue *args);
static bool pl_builtin_iostream_getLine(int argCount, PlValue *args);
static bool pl_builtin_plant_hasAttr(int argCount, PlValue *args);
static bool pl_builtin_math_hypot(int argCount, PlValue *args);
static bool pl_builtin_math_ilogb(int argCount, PlValue *args);
static bool pl_builtin_math_isFinite(int argCount, PlValue *args);
static bool pl_builtin_math_isInf(int argCount, PlValue *args);
static bool pl_builtin_math_isNan(int argCount, PlValue *args);
static bool pl_builtin_math_isNormal(int argCount, PlValue *args);
static bool pl_builtin_math_lGamma(int argCount, PlValue *args);
static bool pl_builtin_math_log(int argCount, PlValue *args);
static bool pl_builtin_math_log10(int argCount, PlValue *args);
static bool pl_builtin_math_log1p(int argCount, PlValue *args);
static bool pl_builtin_math_log2(int argCount, PlValue *args);
static bool pl_builtin_math_logb(int argCount, PlValue *args);
static bool pl_builtin_math_nearbyInt(int argCount, PlValue *args);
static bool pl_builtin_math_nextAfter(int argCount, PlValue *args);
static bool pl_builtin_math_perm(int argCount, PlValue *args);
static bool pl_builtin_math_pow(int argCount, PlValue *args);
static bool pl_builtin_math_radians(int argCount, PlValue *args);
static bool pl_builtin_math_remainder(int argCount, PlValue *args);
static bool pl_builtin_math_rInt(int argCount, PlValue *args);
static bool pl_builtin_math_round(int argCount, PlValue *args);
static bool pl_builtin_plant_setAttr(int argCount, PlValue *args);
static bool pl_builtin_math_sin(int argCount, PlValue *args);
static bool pl_builtin_math_sinh(int argCount, PlValue *args);
static bool pl_builtin_math_sqrt(int argCount, PlValue *args);
static bool pl_builtin_math_tan(int argCount, PlValue *args);
static bool pl_builtin_math_tanh(int argCount, PlValue *args);
static bool pl_builtin_math_tGamma(int argCount, PlValue *args);
static bool pl_builtin_math_trunc(int argCount, PlValue *args);
static bool pl_builtin_any_toBool(int argCount, PlValue *args);
static bool pl_builtin_any_toChar(int argCount, PlValue *args);
static bool pl_builtin_any_toNum(int argCount, PlValue *args);
static bool pl_builtin_any_toString(int argCount, PlValue *args);

void pl_initStd()
{
#define PL_STD_SET(key, len, builtin) \
    pl_hashSet(&pl_stdMembers, pl_copyString(key, len), PL_OBJECT_VALUE(pl_newBuiltin(builtin)));
#define PL_STD_SET_OBJ(key, len, obj) \
    pl_hashSet(&pl_stdMembers, pl_copyString(key, len), obj);

    pl_initHash(&pl_stdMembers);

    PL_STD_SET("abs", 3, pl_builtin_abs);
    PL_STD_SET("math.acos", 9, pl_builtin_math_acos);
    PL_STD_SET("math.acosh", 10, pl_builtin_math_acosh);
    PL_STD_SET("math.asin", 9, pl_builtin_math_asin);
    PL_STD_SET("math.asinh", 10, pl_builtin_math_asinh);
    PL_STD_SET("math.atan", 9, pl_builtin_math_atan);
    PL_STD_SET("math.atan2", 10, pl_builtin_math_atan2);
    PL_STD_SET("math.atanh", 10, pl_builtin_math_atanh);
    PL_STD_SET("math.cbrt", 9, pl_builtin_math_cbrt);
    PL_STD_SET("math.ceil", 9, pl_builtin_math_ceil);
    PL_STD_SET("math.comb", 9, pl_builtin_math_comb);
    PL_STD_SET("math.copySign", 13, pl_builtin_math_copySign);
    PL_STD_SET("math.cos", 8, pl_builtin_math_cos);
    PL_STD_SET("math.cosh", 9, pl_builtin_math_cosh);
    PL_STD_SET("iostream.cout", 13, pl_builtin_iostream_cout);
    PL_STD_SET("math.degrees", 12, pl_builtin_math_degrees);
    PL_STD_SET("plant.delAttr", 13, pl_builtin_plant_delAttr);
    PL_STD_SET_OBJ("math.e", 6, PL_NUMERAL_VALUE(M_E));
    PL_STD_SET("math.erf", 8, pl_builtin_math_erf);
    PL_STD_SET("math.erfc", 9, pl_builtin_math_erfc);
    PL_STD_SET("exit", 4, pl_builtin_exit);
    PL_STD_SET("math.exp", 8, pl_builtin_math_exp);
    PL_STD_SET("math.exp2", 9, pl_builtin_math_exp2);
    PL_STD_SET("math.expm1", 10, pl_builtin_math_expm1);
    PL_STD_SET("math.factorial", 14, pl_builtin_math_factorial);
    PL_STD_SET("math.floor", 10, pl_builtin_math_floor);
    PL_STD_SET("math.fmod", 9, pl_builtin_math_fmod);
    PL_STD_SET("plant.getAttr", 13, pl_builtin_plant_getAttr);
    PL_STD_SET("iostream.getLine", 15, pl_builtin_iostream_getLine);
    PL_STD_SET("plant.hasAttr", 13, pl_builtin_plant_hasAttr);
    PL_STD_SET("math.hypot", 10, pl_builtin_math_hypot);
    PL_STD_SET("math.ilogb", 10, pl_builtin_math_ilogb);
    PL_STD_SET("math.isFinite", 13, pl_builtin_math_isFinite);
    PL_STD_SET("math.isInf", 10, pl_builtin_math_isInf);
    PL_STD_SET("math.isNan", 10, pl_builtin_math_isNan);
    PL_STD_SET("math.isNormal", 13, pl_builtin_math_isNormal);
    PL_STD_SET_OBJ("math.inf", 8, PL_NUMERAL_VALUE(INFINITY));
    PL_STD_SET("math.lGamma", 11, pl_builtin_math_lGamma);
    PL_STD_SET_OBJ("math.ln10", 9, PL_NUMERAL_VALUE(M_LN10));
    PL_STD_SET_OBJ("math.ln2", 8, PL_NUMERAL_VALUE(M_LN2));
    PL_STD_SET("math.log", 8, pl_builtin_math_log);
    PL_STD_SET("math.log10", 10, pl_builtin_math_log10);
    PL_STD_SET_OBJ("math.log10e", 11, PL_NUMERAL_VALUE(M_LOG10E));
    PL_STD_SET("math.log1p", 10, pl_builtin_math_log1p);
    PL_STD_SET("math.log2", 9, pl_builtin_math_log2);
    PL_STD_SET_OBJ("math.log2e", 10, PL_NUMERAL_VALUE(M_LOG2E));
    PL_STD_SET("math.logb", 9, pl_builtin_math_logb);
    PL_STD_SET_OBJ("math.nan", 8, PL_NUMERAL_VALUE(NAN));
    PL_STD_SET("math.nearbyInt", 14, pl_builtin_math_nearbyInt);
    PL_STD_SET("math.nextAfter", 14, pl_builtin_math_nextAfter);
    PL_STD_SET_OBJ("math.oneOverPi", 14, PL_NUMERAL_VALUE(M_1_PI));
    PL_STD_SET("math.perm", 9, pl_builtin_math_perm);
    PL_STD_SET_OBJ("math.pi", 7, PL_NUMERAL_VALUE(M_PI));
    PL_STD_SET_OBJ("math.pi2", 8, PL_NUMERAL_VALUE(M_PI_2));
    PL_STD_SET_OBJ("math.pi4", 8, PL_NUMERAL_VALUE(M_PI_4));
    PL_STD_SET("math.pow", 8, pl_builtin_math_pow);
    PL_STD_SET("math.radians", 12, pl_builtin_math_radians);
    PL_STD_SET("math.remainder", 13, pl_builtin_math_remainder);
    PL_STD_SET("math.rInt", 9, pl_builtin_math_rInt);
    PL_STD_SET("math.round", 10, pl_builtin_math_round);
    PL_STD_SET("plant.setAttr", 13, pl_builtin_plant_setAttr);
    PL_STD_SET("math.sin", 8, pl_builtin_math_sin);
    PL_STD_SET("math.sinh", 9, pl_builtin_math_sinh);
    PL_STD_SET("math.sqrt", 9, pl_builtin_math_sqrt);
    PL_STD_SET_OBJ("math.sqrt2", 10, PL_NUMERAL_VALUE(M_SQRT2));
    PL_STD_SET_OBJ("math.sqrtZeroPointFive", 22, PL_NUMERAL_VALUE(M_SQRT1_2));
    PL_STD_SET("math.tan", 8, pl_builtin_math_tan);
    PL_STD_SET("math.tanh", 9, pl_builtin_math_tanh);
    PL_STD_SET_OBJ("math.tau", 8, PL_NUMERAL_VALUE(M_PI * 2))
    PL_STD_SET("math.tGamma", 11, pl_builtin_math_tGamma);
    PL_STD_SET("math.trunc", 10, pl_builtin_math_trunc);
    PL_STD_SET_OBJ("math.twoOverPi", 14, PL_NUMERAL_VALUE(M_2_PI));
    PL_STD_SET_OBJ("math.twoSqrtPi", 14, PL_NUMERAL_VALUE(M_2_SQRTPI));
    PL_STD_SET("any.toBool", 10, pl_builtin_any_toBool);
    PL_STD_SET("any.toChar", 10, pl_builtin_any_toChar);
    PL_STD_SET("any.toNum", 9, pl_builtin_any_toNum);
    PL_STD_SET("any.toString", 12, pl_builtin_any_toString);
#undef PL_STD_SET
#undef PL_STD_SET_OBJ
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

static bool pl_builtin_math_acos(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'acos'.", 36));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(acos(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_acosh(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'acosh'.", 37));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(acosh(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_asin(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'asin'.", 36));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(asin(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_asinh(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'asinh'.", 37));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(asinh(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_atan(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'atan'.", 36));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(atan(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_atan2(int argCount, PlValue *args)
{
    if (argCount == 2)
    {
        if (!PL_IS_NUMERAL(args[0]) || !PL_IS_NUMERAL(args[1]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching types for call to 'atan2'.", 38));
            return false;
        }

        args[-1] = PL_NUMERAL_VALUE(atan2(PL_AS_NUMERAL(args[0]), PL_AS_NUMERAL(args[1])));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 2.", 40));
    return false;
}

static bool pl_builtin_math_atanh(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'atanh'.", 37));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(atanh(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_cbrt(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'cbrt'.", 36));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(cbrt(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_cos(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'cos'.", 35));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(cos(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_cosh(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'cosh'.", 36));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(cosh(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_ceil(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'ceil'.", 36));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(ceil(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_comb(int argCount, PlValue *args)
{
    if (argCount == 2)
    {
        if (!PL_IS_NUMERAL(args[0]) || !PL_IS_NUMERAL(args[1]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching types for call to 'comb'.", 37));
            return false;
        }

        args[-1] = PL_NUMERAL_VALUE(pl_combination(PL_AS_NUMERAL(args[0]), PL_AS_NUMERAL(args[1])));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 2.", 40));
    return false;
}

static bool pl_builtin_math_copySign(int argCount, PlValue *args)
{
    if (argCount == 2)
    {
        if (!PL_IS_NUMERAL(args[0]) || !PL_IS_NUMERAL(args[1]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching types for call to 'copySign'.", 41));
            return false;
        }

        args[-1] = PL_NUMERAL_VALUE(copysign(PL_AS_NUMERAL(args[0]), PL_AS_NUMERAL(args[1])));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 2.", 40));
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

static bool pl_builtin_math_degrees(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'degrees'.", 39));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(value * 180.0 / M_PI);
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
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

static bool pl_builtin_math_erf(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'erf'.", 35));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(erf(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_erfc(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'erfc'.", 36));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(erfc(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
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

static bool pl_builtin_math_exp(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[1]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'exp'.", 35));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(exp(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_exp2(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[1]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'exp2'.", 36));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(exp2(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_expm1(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[1]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'expm1'.", 37));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(expm1(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_factorial(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'factorial'.", 41));
            return false;
        }

        args[-1] = PL_NUMERAL_VALUE(pl_factorial(PL_AS_NUMERAL(args[0])));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_floor(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'floor'.", 37));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(floor(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_fmod(int argCount, PlValue *args)
{
    if (argCount == 2)
    {
        if (!PL_IS_NUMERAL(args[0]) || !PL_IS_NUMERAL(args[1]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching types for call to 'fmod'.", 37));
            return false;
        }

        args[-1] = PL_NUMERAL_VALUE(fmod(PL_AS_NUMERAL(args[0]), PL_AS_NUMERAL(args[1])));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 2.", 40));
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

static bool pl_builtin_math_hypot(int argCount, PlValue *args)
{
    if (argCount == 2)
    {
        if (!PL_IS_NUMERAL(args[0]) || !PL_IS_NUMERAL(args[1]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching types for call to 'hypot'.", 38));
            return false;
        }

        args[-1] = PL_NUMERAL_VALUE(hypot(PL_AS_NUMERAL(args[0]), PL_AS_NUMERAL(args[1])));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 2.", 40));
    return false;
}

static bool pl_builtin_math_ilogb(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'ilogb'.", 37));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(ilogb(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_isFinite(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'isFinite'.", 40));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(isfinite(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_isInf(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'isInf'.", 37));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(isinf(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_isNan(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'isNan'.", 37));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(isnan(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_isNormal(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'isNormal'.", 40));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(isnormal(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_lGamma(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'lGamma'.", 38));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(lgamma(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_log(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'log'.", 35));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(log(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_log10(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'log10'.", 37));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(log10(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_log1p(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[1]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'log1p'.", 37));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(log1p(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_log2(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[1]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'log2'.", 36));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(log2(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_logb(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[1]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'logb'.", 36));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(logb(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_nearbyInt(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'nearbyInt'.", 41));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(nearbyint(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_nextAfter(int argCount, PlValue *args)
{
    if (argCount == 2)
    {
        if (!PL_IS_NUMERAL(args[0]) || !PL_IS_NUMERAL(args[1]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching types for call to 'nextAfter'.", 42));
            return false;
        }

        args[-1] = PL_NUMERAL_VALUE(nextafter(PL_AS_NUMERAL(args[0]), PL_AS_NUMERAL(args[1])));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 2.", 40));
    return false;
}

static bool pl_builtin_math_perm(int argCount, PlValue *args)
{
    if (argCount == 2)
    {
        if (!PL_IS_NUMERAL(args[0]) || !PL_IS_NUMERAL(args[1]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching types for call to 'perm'.", 37));
            return false;
        }

        args[-1] = PL_NUMERAL_VALUE(pl_permutation(PL_AS_NUMERAL(args[0]), PL_AS_NUMERAL(args[1])));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 2.", 40));
    return false;
}

static bool pl_builtin_math_pow(int argCount, PlValue *args)
{
    if (argCount == 2)
    {
        if (!PL_IS_NUMERAL(args[0]) || !PL_IS_NUMERAL(args[1]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching types for call to 'pow'.", 36));
            return false;
        }

        args[-1] = PL_NUMERAL_VALUE(pow(PL_AS_NUMERAL(args[0]), PL_AS_NUMERAL(args[1])));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 2.", 40));
    return false;
}

static bool pl_builtin_math_radians(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'radians'.", 39));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(value * M_PI / 180.0);
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_remainder(int argCount, PlValue *args)
{
    if (argCount == 2)
    {
        if (!PL_IS_NUMERAL(args[0]) || !PL_IS_NUMERAL(args[1]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching types for call to 'remainder'.", 41));
            return false;
        }

        args[-1] = PL_NUMERAL_VALUE(remainder(PL_AS_NUMERAL(args[0]), PL_AS_NUMERAL(args[1])));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 2.", 40));
    return false;
}

static bool pl_builtin_math_rInt(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'rInt'.", 36));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(rint(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_round(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'round'.", 37));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(round(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
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

static bool pl_builtin_math_sin(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'sin'.", 35));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(sin(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_sinh(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'sinh'.", 36));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(sinh(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_sqrt(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'sqrt'.", 36));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(sqrt(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_tan(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'tan'.", 35));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(tan(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_tanh(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'tanh'.", 36));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(tanh(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_tGamma(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'tGamma'.", 38));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(tgamma(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_math_trunc(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        if (!PL_IS_NUMERAL(args[0]))
        {
            args[-1] = PL_OBJECT_VALUE(pl_copyString("No matching type for call to 'trunc'.", 37));
            return false;
        }

        double value = PL_AS_NUMERAL(args[0]);
        args[-1] = PL_NUMERAL_VALUE(trunc(value));
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
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
