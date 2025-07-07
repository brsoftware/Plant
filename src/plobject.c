#include <stdio.h>
#include <string.h>

#include <PlMemory>
#include <PlVM>

#define PL_ALLOC_OBJ(type, objectType) (type*)pl_allocObject(sizeof(type), objectType)

static PlObject *pl_allocObject(size_t size, PlObjectType type);
static PlString *pl_allocString(char *chars, int length, uint32_t hash);
static uint32_t pl_hashString(const char *key, int length);
static void pl_printFunc(PlFunction *function);
static void pl_printFuncString(PlFunction *function, char *string);

PlBoundMethod *pl_newBoundMethod(PlValue receiver, PlClosure *method)
{
    PlBoundMethod *bound = PL_ALLOC_OBJ(PlBoundMethod, PL_OBJ_BOUND_METHOD);
    bound->receiver = receiver;
    bound->method = method;
    return bound;
}

PlBuiltin *pl_newBuiltin(PlBuiltinFunc function)
{
    PlBuiltin *native = PL_ALLOC_OBJ(PlBuiltin, PL_OBJ_BUILTIN);
    native->function = function;
    return native;
}

PlClass *pl_newClass(PlString *name)
{
    PlClass *cls = PL_ALLOC_OBJ(PlClass, PL_OBJ_CLASS);
    cls->name = name;
    cls->initializer = PL_NULL_VALUE;
    pl_initHash(&cls->methods);
    return cls;
}

PlClosure *pl_newClosure(PlFunction *function)
{
    PlSurvalue **survalues = PL_ALLOC(PlSurvalue*, function->survalueCount);

    for (int index = 0; index < function->survalueCount; index++)
    {
        survalues[index] = NULL;
    }

    PlClosure *closure = PL_ALLOC_OBJ(PlClosure, PL_OBJ_CLOSURE);
    closure->function = function;
    closure->survalues = survalues;
    closure->survalueCount = function->survalueCount;
    return closure;
}

PlFunction *pl_newFunction()
{
    PlFunction *function = PL_ALLOC_OBJ(PlFunction, PL_OBJ_FUNCTION);
    function->arity = 0;
    function->survalueCount = 0;
    function->name = NULL;
    pl_initSegment(&function->segment);
    return function;
}

PlInstance *pl_newInstance(PlClass *cls)
{
    PlInstance *instance = PL_ALLOC_OBJ(PlInstance, PL_OBJ_INSTANCE);
    instance->cls = cls;
    pl_initHash(&instance->fields);
    return instance;
}

PlMapping *pl_newMapping()
{
    PlMapping *mapping = PL_ALLOC_OBJ(PlMapping, PL_OBJ_MAPPING);
    pl_initMap(&mapping->map);
    return mapping;
}

PlString *pl_takeString(char *chars, int length)
{
    uint32_t hash = pl_hashString(chars, length);
    PlString *interned = pl_hashFindString(&vm.strings, chars, length, hash);

    if (interned != NULL)
    {
        PL_FREE_ARRAY(char, chars, length + 1);
        return interned;
    }

    return pl_allocString(chars, length, hash);
}

PlString *pl_copyString(const char *chars, int length)
{
    uint32_t hash = pl_hashString(chars, length);
    PlString *interned = pl_hashFindString(&vm.strings, chars, length, hash);
    if (interned != NULL)
        return interned;

    char *heapChars = PL_ALLOC(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';

    return pl_allocString(heapChars, length, hash);
}

PlSurvalue *pl_newSurvalue(PlValue *slot)
{
    PlSurvalue *survalue = PL_ALLOC_OBJ(PlSurvalue, PL_OBJ_SURVALUE);
    survalue->closed = PL_NULL_VALUE;
    survalue->location = slot;
    survalue->next = NULL;
    return survalue;
}

PlVector *pl_newVector()
{
    PlVector *vector = PL_ALLOC_OBJ(PlVector, PL_OBJ_VECTOR);
    pl_initValueArray(&vector->items);
    return vector;
}

void pl_reprObject(PlValue value)
{
    switch (PL_OBJ_TYPE(value))
    {
    case PL_OBJ_BOUND_METHOD:
        printf("<bound method %s at %p>", PL_AS_BOUND_METHOD(value)->method->function->name->chars,
            PL_AS_BOUND_METHOD(value));
        break;
    case PL_OBJ_BUILTIN:
        printf("<builtin function>");
        break;
    case PL_OBJ_CLASS:
        printf("<class %s>", PL_AS_CLASS(value)->name->chars);
        break;
    case PL_OBJ_CLOSURE:
        pl_printFunc(PL_AS_CLOSURE(value)->function);
        break;
    case PL_OBJ_FUNCTION:
        pl_printFunc(PL_AS_FUNCTION(value));
        break;
    case PL_OBJ_INSTANCE:
        printf("<%s instance at %p>", PL_AS_INSTANCE(value)->cls->name->chars, PL_AS_INSTANCE(value));
        break;

    case PL_OBJ_MAPPING: {
        PlMapping *map = PL_AS_MAPPING(value);
        printf("{");
        bool precedeEmpty = true;

        for (int index = 0; index < map->map.capacity; index++)
        {
            PlMapItem *item = &map->map.items[index];

            if (item == NULL)
                continue;

            if (!PL_IS_EMPTY(item->key))
            {
                if (index > 0 && !precedeEmpty)
                    printf(", ");
                precedeEmpty = false;
                if (PL_IS_STRING(item->key))
                    printf(PL_AS_STRING(item->key)->length == 1 ? "'" : "\"");
                pl_reprValues(item->key);
                if (PL_IS_STRING(item->key))
                    printf(PL_AS_STRING(item->key)->length == 1 ? "'" : "\"");
                printf(": ");
                if (PL_IS_STRING(item->value))
                    printf(PL_AS_STRING(item->value)->length == 1 ? "'" : "\"");
                pl_reprValues(item->value);
                if (PL_IS_STRING(item->value))
                    printf(PL_AS_STRING(item->value)->length == 1 ? "'" : "\"");
            }
        }

        printf("}");
        break;
    }

    case PL_OBJ_STRING:
        printf("%s", PL_AS_CSTRING(value));
        break;
    case PL_OBJ_SURVALUE:
        printf("<survalue at %p>", &value);
        break;

    case PL_OBJ_VECTOR: {
        PlVector *vector = PL_AS_VECTOR(value);
        printf("[");

        for (int index = 0; index < vector->items.count; index++)
        {
            if (index > 0)
                printf(", ");

            if (PL_IS_STRING(vector->items.values[index]))
                printf(PL_AS_STRING(vector->items.values[index])->length == 1 ? "'" : "\"");
            pl_reprValues(vector->items.values[index]);
            if (PL_IS_STRING(vector->items.values[index]))
                printf(PL_AS_STRING(vector->items.values[index])->length == 1 ? "'" : "\"");
        }

        printf("]");
        break;
    }
    }
}

void pl_reprObjectString(PlValue value, char *stream)
{
    switch (PL_OBJ_TYPE(value))
    {
    case PL_OBJ_BOUND_METHOD:
        sprintf(stream, "<bound method %s at %p>", PL_AS_BOUND_METHOD(value)->method->function->name->chars,
            PL_AS_BOUND_METHOD(value));
        break;
    case PL_OBJ_BUILTIN:
        sprintf(stream, "<builtin function>");
        break;
    case PL_OBJ_CLASS:
        sprintf(stream, "<class %s>", PL_AS_CLASS(value)->name->chars);
        break;
    case PL_OBJ_CLOSURE:
        pl_printFuncString(PL_AS_CLOSURE(value)->function, stream);
        break;
    case PL_OBJ_FUNCTION:
        pl_printFuncString(PL_AS_FUNCTION(value), stream);
        break;
    case PL_OBJ_INSTANCE:
        sprintf(stream, "<%s instance at %p>", PL_AS_INSTANCE(value)->cls->name->chars, PL_AS_INSTANCE(value));
        break;
    case PL_OBJ_MAPPING:
        sprintf(stream, "{...}");
        break;
    case PL_OBJ_STRING:
        sprintf(stream, "%s", PL_AS_CSTRING(value));
        break;
    case PL_OBJ_SURVALUE:
        sprintf(stream, "<survalue at %p>", &value);
        break;
    case PL_OBJ_VECTOR: {
        sprintf(stream, "[...]");
        break;
    }
    }
}

static PlObject *pl_allocObject(size_t size, PlObjectType type)
{
    PlObject *object = pl_realloc(NULL, 0, size);
    object->type = type;
    object->mark = !vm.markValue;
    object->next = vm.objects;
    vm.objects = object;
    return object;
}

static PlString *pl_allocString(char *chars, int length, uint32_t hash)
{
    PlString *string = PL_ALLOC_OBJ(PlString, PL_OBJ_STRING);
    string->length = length;
    string->chars = chars;
    string->hash = hash;

    pl_push(PL_OBJECT_VALUE(string));
    pl_hashSet(&vm.strings, string, PL_NULL_VALUE);
    pl_pop();

    return string;
}

static uint32_t pl_hashString(const char *key, int length)
{
    uint32_t hash = 2166136261u;

    for (int index = 0; index < length; index++)
    {
        hash ^= (uint8_t)key[index];
        hash *= 16777619;
    }

    return hash;
}

static void pl_printFunc(PlFunction *function)
{
    if (function->name == NULL)
    {
        printf("<module>");
        return;
    }

    if (strcmp(function->name->chars, "]") == 0)
    {
        printf("<lambda function at %p>", function);
        return;
    }

    printf("<function %s at %p>", function->name->chars, function);
}

static void pl_printFuncString(PlFunction *function, char *string)
{
    if (function->name == NULL)
    {
        sprintf(string, "<module>");
        return;
    }

    if (strcmp(function->name->chars, "]") == 0)
    {
        sprintf(string, "<lambda function at %p>", function);
        return;
    }

    sprintf(string, "<function %s at %p>", function->name->chars, function);
}
