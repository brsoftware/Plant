#include <stdlib.h>

#include <PlCompiler>
#include <PlMemory>
#include <PlVM>

#define PL_GC_GROWTH_FACTOR 2

static void pl_blackenObject(PlObject *object);
static void pl_freeObject(PlObject *object);
static void pl_markArray(PlValueArray *array);
static void pl_markRoots();
static void pl_sweep();
static void pl_traceRef();

void *pl_realloc(void *ptr, size_t oldSize, size_t newSize)
{
    vm.bytesAllocated += newSize - oldSize;

    if (newSize > oldSize)
    {
        if (vm.bytesAllocated > vm.nextGC)
        {
            pl_collectGarbage();
        }
    }

    if (newSize == 0)
    {
        free(ptr);
        return NULL;
    }

    void *result = realloc(ptr, newSize);
    if (result == NULL)
        exit(1);
    return result;
}

void pl_markObject(PlObject *object)
{
    if (object == NULL)
        return;
    if (object->mark == vm.markValue)
        return;

    object->mark = vm.markValue;

    if (vm.grayCapacity < vm.grayCount + 1)
    {
        vm.grayCapacity = PL_GROW_CAPAC(vm.grayCapacity);
        vm.grayStack = (PlObject**)realloc(vm.grayStack, sizeof(PlObject*) * vm.grayCapacity);

        if (vm.grayStack == NULL)
            exit(1);
    }

    vm.grayStack[vm.grayCount++] = object;
}

void pl_markValue(PlValue value)
{
    if (PL_IS_OBJECT(value))
        pl_markObject(PL_AS_OBJECT(value));
}


void pl_collectGarbage()
{
    pl_markRoots();
    pl_traceRef();
    pl_hashRemoveWhite(&vm.strings);
    pl_sweep();

    vm.nextGC = vm.bytesAllocated * PL_GC_GROWTH_FACTOR;
    vm.markValue = !vm.markValue;
}

void pl_freeObjects()
{
    PlObject *object = vm.objects;

    while (object != NULL)
    {
        PlObject *next = object->next;
        pl_freeObject(object);
        object = next;
    }

    free(vm.grayStack);
}

char *pl_strndup(const char *str, int chars)
{
    char *buffer = (char*)malloc(chars+1);

    if (buffer)
    {
        int n;

        for (n = 0; ((n < chars) && (str[n] != 0)); n++)
            buffer[n] = str[n];

        buffer[n] = 0;
    }

    return buffer;
}

static void pl_blackenObject(PlObject *object)
{
    switch (object->type)
    {
    case PL_OBJ_BOUND_METHOD: {
        PlBoundMethod *bound = (PlBoundMethod*)object;
        pl_markValue(bound->receiver);
        pl_markObject((PlObject*)bound->method);
        break;
    }

    case PL_OBJ_CLASS: {
        PlClass *cls = (PlClass*)object;
        pl_markObject((PlObject*)cls->name);
        pl_markHash(&cls->methods);
        break;
    }

    case PL_OBJ_CLOSURE: {
        PlClosure *closure = (PlClosure*)object;
        pl_markObject((PlObject*)closure->function);

        for (int index = 0; index < closure->survalueCount; index++)
        {
            pl_markObject((PlObject*)closure->survalues[index]);
        }

        break;
    }

    case PL_OBJ_FUNCTION: {
        PlFunction *function = (PlFunction*)object;
        pl_markObject((PlObject*)function->name);
        pl_markArray(&function->segment.constants);
        break;
    }

    case PL_OBJ_INSTANCE: {
        PlInstance *instance = (PlInstance*)object;
        pl_markObject((PlObject*)instance->cls);
        pl_markHash(&instance->fields);
        break;
    }

    case PL_OBJ_MAPPING: {
        PlMapping *map = (PlMapping*)object;
        pl_markMap(&map->map);
        break;
    }

    case PL_OBJ_SURVALUE:
        pl_markValue(((PlSurvalue*)object)->closed);
        break;

    case PL_OBJ_VECTOR: {
        PlVector *vector = (PlVector*)object;

        for (int index = 0; index < vector->items.count; index++)
        {
            pl_markValue(vector->items.values[index]);
        }

        break;
    }

    case PL_OBJ_BUILTIN:
    case PL_OBJ_STRING:
        break;
    }
}

static void pl_freeObject(PlObject *object)
{
    switch (object->type)
    {
    case PL_OBJ_BOUND_METHOD:
        PL_FREE(PlBoundMethod, object);
        break;

    case PL_OBJ_BUILTIN:
        PL_FREE(PlBuiltin, object);
        break;

    case PL_OBJ_CLASS: {
        PlClass *cls = (PlClass*)object;
        pl_freeHash(&cls->methods);
        PL_FREE(PlClass, object);
        break;
    }

    case PL_OBJ_CLOSURE: {
        PlClosure *closure = (PlClosure*)object;
        PL_FREE_ARRAY(PlSurvalue*, closure->survalues, closure->survalueCount);
        PL_FREE(PlClosure, object);
        break;
    }

    case PL_OBJ_FUNCTION: {
        PlFunction *function = (PlFunction*)object;
        pl_freeSegment(&function->segment);
        PL_FREE(PlFunction, object);
        break;
    }

    case PL_OBJ_INSTANCE: {
        PlInstance *instance = (PlInstance*)object;
        pl_freeHash(&instance->fields);
        PL_FREE(PlInstance, object);
        break;
    }

    case PL_OBJ_MAPPING: {
        PlMapping *map = (PlMapping*)object;
        pl_freeMap(&map->map);
        PL_FREE(PlMapping, object);
        break;
    }

    case PL_OBJ_STRING: {
        PlString *string = (PlString*)object;
        PL_FREE_ARRAY(char, string->chars, string->length + 1);
        PL_FREE(PlString, object);
        break;
    }

    case PL_OBJ_SURVALUE:
        PL_FREE(PlSurvalue, object);
        break;

    case PL_OBJ_VECTOR: {
        PlVector *vector = (PlVector*)object;
        PL_FREE_ARRAY(PlValue, &vector->items, vector->items.capacity);
        PL_FREE(PlVector, object);
        break;
    }
    }
}

static void pl_markArray(PlValueArray *array)
{
    for (int index = 0; index < array->count; index++)
    {
        pl_markValue(array->values[index]);
    }
}

static void pl_markRoots()
{
    for (PlValue *slot = vm.stack; slot < vm.stackTop; slot++)
    {
        pl_markValue(*slot);
    }

    for (int index = 0; index < vm.frameCount; index++)
    {
        pl_markObject((PlObject*)vm.frames[index].closure);
    }

    for (PlSurvalue *survalue = vm.openSurvalues; survalue != NULL; survalue = survalue->next)
    {
        pl_markObject((PlObject*)survalue);
    }

    pl_markHash(&vm.globals);
    pl_markCompilerRoots();
    pl_markObject((PlObject*)vm.initString);
}

static void pl_sweep()
{
    PlObject *prev = NULL;
    PlObject *object = vm.objects;

    while (object != NULL)
    {
        if (object->mark == vm.markValue)
        {
            prev = object;
            object = object->next;
        }

        else
        {
            PlObject *unreached = object;
            object = object->next;

            if (prev != NULL)
            {
                prev->next = object;
            }

            else
            {
                vm.objects = object;
            }

            pl_freeObject(unreached);
        }
    }
}

static void pl_traceRef()
{
    while (vm.grayCount > 0)
    {
        PlObject *object = vm.grayStack[--vm.grayCount];
        pl_blackenObject(object);
    }
}
