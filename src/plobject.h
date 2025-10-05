#ifndef PLOBJECT_H
#define PLOBJECT_H

#include <PlSegment>
#include <PlHash>
#include <PlMap>
#include <PlSet>
#include <PlValue>

#define PL_OBJ_TYPE(value)         (PL_AS_OBJECT(value)->type)

#define PL_IS_BOUND_METHOD(value) pl_isObjectType(value, PL_OBJ_BOUND_METHOD)
#define PL_IS_BUILTIN(value)      pl_isObjectType(value, PL_OBJ_BUILTIN)
#define PL_IS_CLASS(value)        pl_isObjectType(value, PL_OBJ_CLASS)
#define PL_IS_CLOSURE(value)      pl_isObjectType(value, PL_OBJ_CLOSURE)
#define PL_IS_FUNCTION(value)     pl_isObjectType(value, PL_OBJ_FUNCTION)
#define PL_IS_INSTANCE(value)     pl_isObjectType(value, PL_OBJ_INSTANCE)
#define PL_IS_MAPPING(value)      pl_isObjectType(value, PL_OBJ_MAPPING)
#define PL_IS_SET(value)          pl_isObjectType(value, PL_OBJ_SET)
#define PL_IS_STRING(value)       pl_isObjectType(value, PL_OBJ_STRING)
#define PL_IS_VECTOR(value)       pl_isObjectType(value, PL_OBJ_VECTOR)

#define PL_AS_BOUND_METHOD(value)  ((PlBoundMethod*)PL_AS_OBJECT(value))
#define PL_AS_BUILTIN(value)      (((PlBuiltin*)    PL_AS_OBJECT(value))->function)
#define PL_AS_CLASS(value)         ((PlClass*)      PL_AS_OBJECT(value))
#define PL_AS_CLOSURE(value)       ((PlClosure*)    PL_AS_OBJECT(value))
#define PL_AS_FUNCTION(value)      ((PlFunction*)   PL_AS_OBJECT(value))
#define PL_AS_INSTANCE(value)      ((PlInstance*)   PL_AS_OBJECT(value))
#define PL_AS_MAPPING(value)       ((PlMapping*)    PL_AS_OBJECT(value))
#define PL_AS_SET(value)           ((PlSetObject*)  PL_AS_OBJECT(value))
#define PL_AS_STRING(value)        ((PlString*)     PL_AS_OBJECT(value))
#define PL_AS_CSTRING(value)      (((PlString*)     PL_AS_OBJECT(value))->chars)
#define PL_AS_VECTOR(value)        ((PlVector*)     PL_AS_OBJECT(value))

typedef enum PlObjectType
{
    PL_OBJ_BOUND_METHOD,
    PL_OBJ_BUILTIN,
    PL_OBJ_CLASS,
    PL_OBJ_CLOSURE,
    PL_OBJ_FUNCTION,
    PL_OBJ_INSTANCE,
    PL_OBJ_MAPPING,
    PL_OBJ_SET,
    PL_OBJ_STRING,
    PL_OBJ_SURVALUE,
    PL_OBJ_VECTOR
} PlObjectType;

struct PlObject
{
    PlObjectType type;
    bool mark;
    struct PlObject *next;
};

typedef struct PlFunction
{
    PlObject obj;
    int arity;
    int survalueCount;
    PlSegment segment;
    PlString *name;
} PlFunction;

typedef bool (*PlBuiltinFunc)(int argCount, PlValue *args);

typedef struct PlBuiltin
{
    PlObject obj;
    PlBuiltinFunc function;
} PlBuiltin;

struct PlString
{
    PlObject obj;
    int length;
    char *chars;
    uint32_t hash;
};

typedef struct PlSurvalue
{
    PlObject obj;
    PlValue *location;
    PlValue closed;
    struct PlSurvalue *next;
} PlSurvalue;

typedef struct PlClosure
{
    PlObject obj;
    PlFunction *function;
    PlSurvalue **survalues;
    int survalueCount;
} PlClosure;

typedef struct PlClass
{
    PlObject obj;
    PlString *name;
    PlValue initializer;
    bool isFinal;
    PlHash methods;
} PlClass;

typedef struct PlInstance
{
    PlObject obj;
    PlClass *cls;
    PlHash fields;
} PlInstance;

typedef struct PlBoundMethod
{
    PlObject obj;
    PlValue receiver;
    PlClosure *method;
} PlBoundMethod;

typedef struct PlVector
{
    PlObject obj;
    PlValueArray items;
} PlVector;

typedef struct PlMapping
{
    PlObject obj;
    PlMap map;
} PlMapping;

struct PlSetObject
{
    PlObject obj;
    PlSet set;
};

PlBoundMethod *pl_newBoundMethod(PlValue receiver, PlClosure *method);
PlBuiltin *pl_newBuiltin(PlBuiltinFunc function);
PlClass *pl_newClass(PlString *name);
PlClosure *pl_newClosure(PlFunction *function);
PlFunction *pl_newFunction();
PlInstance *pl_newInstance(PlClass *cls);
PlMapping *pl_newMapping();
PlSetObject *pl_newSet();
PlString *pl_takeString(char *chars, int length);
PlString *pl_copyString(const char *chars, int length);
PlSurvalue *pl_newSurvalue(PlValue *slot);
PlVector *pl_newVector();

void pl_reprObject(PlValue value);

void pl_reprObjectString(PlValue value, char *string);

static inline bool pl_isObjectType(PlValue value, PlObjectType type)
{
    return PL_IS_OBJECT(value) && PL_AS_OBJECT(value)->type == type;
}

#endif  // PLOBJECT_H
