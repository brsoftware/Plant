#include <math.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <PlCompiler>
#include <PlLexer>
#include <PlMemory>

PlVM vm;

static bool pl_builtin_abs(int argCount, PlValue *args);
static bool pl_builtin_credits(int argCount, PlValue *args);
static bool pl_builtin_copyright(int argCount, PlValue *args);
static bool pl_builtin_cout(int argCount, PlValue *args);
static bool pl_builtin_delAttr(int argCount, PlValue *args);
static bool pl_builtin_exit(int argCount, PlValue *args);
static bool pl_builtin_getAttr(int argCount, PlValue *args);
static bool pl_builtin_getLine(int argCount, PlValue *args);
static bool pl_builtin_hasAttr(int argCount, PlValue *args);
static bool pl_builtin_setAttr(int argCount, PlValue *args);
static bool pl_builtin_toBool(int argCount, PlValue *args);
static bool pl_builtin_toChar(int argCount, PlValue *args);
static bool pl_builtin_toNum(int argCount, PlValue *args);
static bool pl_builtin_toString(int argCount, PlValue *args);

static bool pl_bindMethod(PlClass *cls, PlString *name);
static bool pl_call(PlClosure *closure, int argCount);
static bool pl_callValue(PlValue callee, int argCount);
static PlSurvalue *pl_captureSurvalue(PlValue *local);
static void pl_closeSurvalues(PlValue *last);
static void pl_concat();
static void pl_defMethod(PlString *name);
static void pl_defineBuiltin(const char *name, PlBuiltinFunc function);
static PlExecResult pl_exec();
static bool pl_invoke(PlString *name, int argCount, char errorOp);
static bool pl_invokeFromClass(PlClass *cls, PlString *name, int argCount, char errorOp);
static bool pl_isFalse(PlValue value);
static PlValue pl_peek(int distance);
static bool pl_repeat();
static void pl_resetStack();
static void pl_runtimeError(const char *format, ...);

void pl_initVM()
{
    pl_resetStack();

    vm.objects = NULL;
    vm.bytesAllocated = 0;
    vm.nextGC = 1024 * 1024;

    vm.grayCount = 0;
    vm.grayCapacity = 0;
    vm.grayStack = NULL;
    vm.markValue = true;

    pl_initHash(&vm.globals);
    pl_initHash(&vm.strings);

    vm.initString = pl_copyString("init", 4);
    vm.addString = pl_copyString("+", 1);
    vm.subString = pl_copyString("-", 1);
    vm.mulString = pl_copyString("*", 1);
    vm.divString = pl_copyString("/", 1);
    vm.modString = pl_copyString("%", 1);
    vm.bitandString = pl_copyString("&", 1);
    vm.bitorString = pl_copyString("|", 1);
    vm.bitxorString = pl_copyString("^", 1);
    vm.bitnotString = pl_copyString("~", 1);
    vm.negateString = pl_copyString("--", 2);
    vm.affirmString = pl_copyString("++", 2);
    vm.lshiftString = pl_copyString("<<", 2);
    vm.rshiftString = pl_copyString(">>", 2);
    vm.greaterString = pl_copyString(">", 1);
    vm.lessString = pl_copyString("<", 1);
    vm.equalString = pl_copyString("==", 2);

    pl_defineBuiltin("abs", pl_builtin_abs);
    pl_defineBuiltin("credits", pl_builtin_credits);
    pl_defineBuiltin("copyright", pl_builtin_copyright);
    pl_defineBuiltin("cout", pl_builtin_cout);
    pl_defineBuiltin("delAttr", pl_builtin_delAttr);
    pl_defineBuiltin("exit", pl_builtin_exit);
    pl_defineBuiltin("getAttr", pl_builtin_getAttr);
    pl_defineBuiltin("getLine", pl_builtin_getLine);
    pl_defineBuiltin("hasAttr", pl_builtin_hasAttr);
    pl_defineBuiltin("setAttr", pl_builtin_setAttr);
    pl_defineBuiltin("toBool", pl_builtin_toBool);
    pl_defineBuiltin("toChar", pl_builtin_toChar);
    pl_defineBuiltin("toNum", pl_builtin_toNum);
    pl_defineBuiltin("toString", pl_builtin_toString);
}

void pl_freeVM()
{
    pl_freeHash(&vm.globals);
    pl_freeHash(&vm.strings);
    vm.initString = NULL;
    pl_freeObjects();
}

void pl_push(PlValue value)
{
    *vm.stackTop = value;
    vm.stackTop++;
}

PlValue pl_pop()
{
    vm.stackTop--;
    return *vm.stackTop;
}

PlExecResult pl_interpret(const char *source)
{
    PlFunction *function = pl_compile(source);
    if (function == NULL)
        return PL_RST_ERROR;

    pl_push(PL_OBJECT_VALUE(function));

    PlClosure *closure = pl_newClosure(function);
    pl_pop();
    pl_push(PL_OBJECT_VALUE(closure));
    pl_call(closure, 0);

    return pl_exec();
}

void pl_handleSignal(int signal)
{
    switch (signal)
    {
    case SIGSEGV:
        pl_runtimeError("Segmentation fault (core dumped).");
        // exit(0);
    default:
        break;
    }
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

static bool pl_builtin_credits(int argCount, PlValue *args)
{
    if (argCount == 0)
    {
        printf("    Thank you Bell Labs, it was you who created the most beautiful programming\n");
        printf("    language in the world - C.\n");
        printf("\n");
        printf("    Dedication of Plant: To the Python development team, you've made me know what a\n");
        printf("    scripting language looks like. Python was the first language I picked up.\n");
        printf("\n");
        printf("    Lastly, I would like to thank everyone who is or has been involving in the\n");
        printf("    development of Plant. Had it not been for your support, Plant would not have\n");
        printf("    been here.\n");
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 0.", 40));
    return false;
}

static bool pl_builtin_copyright(int argCount, PlValue *args)
{
    if (argCount == 0)
    {
        printf("Copyright (c) 2025 Bright Software Foundation.\n");
        printf("All rights reserved.\n");
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 0.", 40));
    return false;
}

static bool pl_builtin_cout(int argCount, PlValue *args)
{
    for (int index = 0; index < argCount; index++)
    {
        pl_reprValues(args[index]);
    }

    args[-1] = PL_NULL_VALUE;
    return true;
}

static bool pl_builtin_delAttr(int argCount, PlValue *args)
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

static bool pl_builtin_getAttr(int argCount, PlValue *args)
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

static bool pl_builtin_getLine(int argCount, PlValue *args)
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

static bool pl_builtin_hasAttr(int argCount, PlValue *args)
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

static bool pl_builtin_setAttr(int argCount, PlValue *args)
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

static bool pl_builtin_toBool(int argCount, PlValue *args)
{
    if (argCount == 1)
    {
        args[-1] = pl_isFalse(args[0]) ? PL_FALSE_VALUE : PL_TRUE_VALUE;
        return true;
    }

    args[-1] = PL_OBJECT_VALUE(pl_copyString("Incorrect number of arguments: expect 1.", 40));
    return false;
}

static bool pl_builtin_toChar(int argCount, PlValue *args)
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

static bool pl_builtin_toNum(int argCount, PlValue *args)
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

static bool pl_builtin_toString(int argCount, PlValue *args)
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

static bool pl_bindMethod(PlClass *cls, PlString *name)
{
    PlValue method;
    if (!pl_hashGet(&cls->methods, name, &method))
    {
        pl_runtimeError("'class %s' has no member named '%s'.", cls->name->chars, name->chars);
        return false;
    }

    PlBoundMethod *bound = pl_newBoundMethod(pl_peek(0), PL_AS_CLOSURE(method));
    pl_pop();
    pl_push(PL_OBJECT_VALUE(bound));
    return true;
}

static bool pl_call(PlClosure *closure, int argCount)
{
    if (argCount != closure->function->arity)
    {
        pl_runtimeError("Incorrect arguments to call; expected %d arguments but got %d.",
            closure->function->arity, argCount);
        return false;
    }

    if (vm.frameCount == PL_FRAMES_MAX)
    {
        pl_runtimeError("Stack overflow.");
        return false;
    }

    PlCallFrame *frame = &vm.frames[vm.frameCount++];
    frame->closure = closure;
    frame->ip = closure->function->segment.code;
    frame->slots = vm.stackTop - argCount - 1;
    return true;
}

static bool pl_callValue(PlValue callee, int argCount)
{
    if (PL_IS_OBJECT(callee))
    {
        switch (PL_OBJ_TYPE(callee))
        {
        case PL_OBJ_BOUND_METHOD: {
            PlBoundMethod *bound = PL_AS_BOUND_METHOD(callee);
            vm.stackTop[-argCount - 1] = bound->receiver;
            return pl_call(bound->method, argCount);
        }

        case PL_OBJ_BUILTIN: {
            PlBuiltinFunc builtin = PL_AS_BUILTIN(callee);

            if (builtin(argCount, vm.stackTop - argCount))
            {
                vm.stackTop -= argCount;
                return true;
            }

            pl_runtimeError(PL_AS_STRING(vm.stackTop[-argCount - 1])->chars);
            return false;
        }

        case PL_OBJ_CLASS: {
            PlClass *cls = PL_AS_CLASS(callee);
            vm.stackTop[-argCount - 1] = PL_OBJECT_VALUE(pl_newInstance(cls));

            if (!PL_IS_NULL(cls->initializer))
            {
                return pl_call(PL_AS_CLOSURE(cls->initializer), argCount);
            }

            if (argCount != 0)
            {
                pl_runtimeError("Incorrect arguments to call; expected 0 arguments but got %d.", argCount);
                return false;
            }

            return true;
        }

        case PL_OBJ_CLOSURE:
            return pl_call(PL_AS_CLOSURE(callee), argCount);

        default:
            break;
        }
    }

    char *objString = (char*)malloc(sizeof(char) * 64);
    pl_reprValuesString(callee, objString);
    pl_runtimeError("'%s' cannot be used as a function.", objString);
    free(objString);
    return false;
}

static PlSurvalue *pl_captureSurvalue(PlValue *local)
{
    PlSurvalue *prev = NULL;
    PlSurvalue *survalue = vm.openSurvalues;

    while (survalue != NULL && survalue->location > local)
    {
        prev = survalue;
        survalue = survalue->next;
    }

    if (survalue != NULL && survalue->location == local)
    {
        return survalue;
    }

    PlSurvalue *created = pl_newSurvalue(local);
    created->next = survalue;

    if (prev == NULL)
    {
        vm.openSurvalues = created;
    }

    else
    {
        prev->next = created;
    }

    return created;
}

static void pl_closeSurvalues(PlValue *last)
{
    while (vm.openSurvalues != NULL && vm.openSurvalues->location >= last)
    {
        PlSurvalue *survalue = vm.openSurvalues;
        survalue->closed = *survalue->location;
        survalue->location = &survalue->closed;
        vm.openSurvalues = survalue->next;
    }
}

static void pl_concat()
{
    PlString *second = PL_AS_STRING(pl_peek(0));
    PlString *first = PL_AS_STRING(pl_peek(1));

    int length = first->length + second->length;
    char *chars = PL_ALLOC(char, length + 1);
    memcpy(chars, first->chars, first->length);
    memcpy(chars + first->length, second->chars, second->length);
    chars[length] = '\0';

    PlString *result = pl_takeString(chars, length);
    pl_pop();
    pl_pop();
    pl_push(PL_OBJECT_VALUE(result));
}

static void pl_defMethod(PlString *name)
{
    PlValue method = vm.stackTop[-1 - 0];
    PlClass *cls = PL_AS_CLASS(pl_peek(1));
    pl_hashSet(&cls->methods, name, method);
    if (name == vm.initString)
        cls->initializer = method;
    pl_pop();
}

static void pl_defineBuiltin(const char *name, PlBuiltinFunc function)
{
    pl_push(PL_OBJECT_VALUE(pl_copyString(name, (int)strlen(name))));
    pl_push(PL_OBJECT_VALUE(pl_newBuiltin(function)));
    pl_hashSet(&vm.globals, PL_AS_STRING(vm.stack[0]), vm.stack[1]);
    pl_pop();
    pl_pop();
}

static PlExecResult pl_exec()
{
  PlCallFrame *frame = &vm.frames[vm.frameCount - 1];

#define READ_BYTE() (*frame->ip++)
#define READ_SHORT() (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))
#define READ_LONG_BYTE() ((READ_BYTE()) | ((READ_BYTE() << 8) & 0xff) | ((READ_BYTE() << 16) & 0xff))
#define READ_CONSTANT() (frame->closure->function->segment.constants.values[READ_BYTE()])
#define READ_LONG_CONSTANT() (frame->closure->function->segment.constants.values[READ_LONG_BYTE()])
#define READ_STRING() PL_AS_STRING(READ_CONSTANT())
#define READ_LONG_STRING() PL_AS_STRING(READ_LONG_CONSTANT())

    for (;;)
    {
        uint8_t instruction = READ_BYTE();

        // printf("op: %s (%d) -> ", pl_codeToStr(instruction), instruction);
        // if (instruction == PL_GET_LOCAL || instruction == PL_SET_LOCAL
        //     || instruction == PL_SET_GLOBAL || instruction == PL_GET_GLOBAL)
        //     printf("(next: %d) ", (*(frame->ip + 1)));
        // for (PlValue *slot = vm.stack; slot < vm.stackTop; slot++)
        // {
        //     printf("[ ");
        //     pl_reprValues(*slot);
        //     printf(" ]");
        // }
        // printf("\n");

        switch (instruction)
        {
        case PL_CONSTANT: {
            PlValue constant = READ_CONSTANT();
            pl_push(constant);
            break;
        }

        case PL_CONSTANT_LONG: {
            PlValue constant = READ_LONG_CONSTANT();
            pl_push(constant);
            break;
        }

        case PL_NULL:
            pl_push(PL_NULL_VALUE);
            break;
        case PL_TRUE:
            pl_push(PL_BOOL_VALUE(true));
            break;
        case PL_FALSE:
            pl_push(PL_BOOL_VALUE(false));
            break;
        case PL_POP:
            pl_pop();
            break;
        case PL_DUPLICATE:
            pl_push(pl_peek(0));
            break;
        case PL_DUPLICATE_PREV:
            pl_push(pl_peek(1));
            break;

        case PL_GET_LOCAL: {
            uint8_t slot = READ_BYTE();
            pl_push(frame->slots[slot]);
            break;
        }

        case PL_SET_LOCAL: {
            uint8_t slot = READ_BYTE();
            frame->slots[slot] = pl_peek(0);
            break;
        }

        case PL_GET_GLOBAL: {
            PlString *name = READ_STRING();
            PlValue value;

            if (!pl_hashGet(&vm.globals, name, &value))
            {
                pl_runtimeError("'%s' was not declared in the global scope.", name->chars);
                return PL_RST_EXCEPTION;
            }

            pl_push(value);
            break;
        }

        case PL_DEFINE_GLOBAL: {
            PlString *name = READ_STRING();
            pl_hashSet(&vm.globals, name, pl_peek(0));
            pl_pop();
            break;
        }

        case PL_SET_GLOBAL: {
            PlString *name = READ_STRING();

            if (pl_hashSet(&vm.globals, name, pl_peek(0)))
            {
                pl_hashDel(&vm.globals, name);
                pl_runtimeError("'%s' was not declared in the global scope.", name->chars);
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_GET_SURVALUE: {
            uint8_t slot = READ_BYTE();
            pl_push(*frame->closure->survalues[slot]->location);
            break;
        }

        case PL_SET_SURVALUE: {
            uint8_t slot = READ_BYTE();
            *frame->closure->survalues[slot]->location = pl_peek(0);
            break;
        }

        case PL_GET_PROPERTY: {
            if (!PL_IS_INSTANCE(pl_peek(0)))
            {
                pl_runtimeError("Request for member of non-class type.");
                return PL_RST_EXCEPTION;
            }

            PlInstance *instance = PL_AS_INSTANCE(pl_peek(0));
            PlString *name = READ_STRING();

            PlValue value;

            if (pl_hashGet(&instance->fields, name, &value))
            {
                pl_pop();
                pl_push(value);
                break;
            }

            if (!pl_bindMethod(instance->cls, name))
            {
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_SET_PROPERTY: {
            if (!PL_IS_INSTANCE(pl_peek(1)))
            {
                pl_runtimeError("Request for setting member of non-class type.");
                return PL_RST_EXCEPTION;
            }

            PlInstance *instance = PL_AS_INSTANCE(pl_peek(1));
            pl_hashSet(&instance->fields, READ_STRING(), pl_peek(0));
            PlValue value = pl_pop();
            pl_pop();
            pl_push(value);
            break;
        }

        case PL_GET_SUPER: {
            PlString *name = READ_STRING();
            PlClass *superclass = PL_AS_CLASS(pl_pop());

            if (!pl_bindMethod(superclass, name))
            {
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_GET_INDEX: {
            PlValue index = pl_pop();
            PlValue object = pl_pop();

            if (PL_IS_VECTOR(object))
            {
                PlVector *vec = PL_AS_VECTOR(object);

                if (vec->items.count == 0)
                {
                    pl_runtimeError("Accessing the index of an empty vector.");
                    return PL_RST_EXCEPTION;
                }

                if (PL_IS_NUMERAL(index))
                {
                    double value = PL_AS_NUMERAL(index);

                    if (floor(value) == value)
                    {
                        if (value < 0)
                            value += vec->items.count;

                        if (value <= (double)LONG_LONG_MAX)
                        {
                            if (value < vec->items.count)
                            {
                                pl_push(vec->items.values[(long long)value]);
                                break;
                            }

                            pl_runtimeError("Vector index out of range; expected from 0 to %d",
                                vec->items.count - 1);
                            return PL_RST_EXCEPTION;
                        }

                        pl_runtimeError("Integer overflow.");
                        return PL_RST_EXCEPTION;
                    }

                    pl_runtimeError("Vector index must be integers but not reals.");
                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid vector index; expected integer.");
                return PL_RST_EXCEPTION;
            }

            if (PL_IS_MAPPING(object))
            {
                PlMapping *map = PL_AS_MAPPING(object);

                if (!pl_hashable(index))
                {
                    pl_runtimeError("Attempting to index a map by a non-hashable index.");
                    return PL_RST_EXCEPTION;
                }

                PlValue result;

                if (!pl_mapGet(&map->map, index, &result))
                {
                    char valText[1024];
                    pl_reprValuesString(index, valText);
                    pl_runtimeError("Non-exist key '%s'.", valText);
                    return PL_RST_EXCEPTION;
                }

                pl_push(result);
                break;
            }

            if (PL_IS_STRING(object))
            {
                PlString *str = PL_AS_STRING(object);

                if (str->length == 0)
                {
                    pl_runtimeError("Accessing the index of an empty string.");
                    return PL_RST_EXCEPTION;
                }

                if (PL_IS_NUMERAL(index))
                {
                    double value = PL_AS_NUMERAL(index);

                    if (floor(value) == value)
                    {
                        if (value < 0)
                            value += str->length;

                        if (value <= (double)LONG_LONG_MAX)
                        {
                            if (value < str->length)
                            {
                                pl_push(PL_OBJECT_VALUE(pl_copyString(&str->chars[(long long)value], 1)));
                                break;
                            }

                            pl_runtimeError("String index out of range; expected from 0 to %d", str->length - 1);
                            return PL_RST_EXCEPTION;
                        }

                        pl_runtimeError("Integer overflow.");
                        return PL_RST_EXCEPTION;
                    }

                    pl_runtimeError("String index must be integers but not reals.");
                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid string index; expected integer.");
                return PL_RST_EXCEPTION;
            }

            pl_runtimeError("Cannot index the object.");
            return PL_RST_EXCEPTION;
        }

        case PL_SET_INDEX: {
            PlValue thing = pl_pop();
            PlValue index = pl_pop();
            PlValue object = pl_peek(0);

            if (PL_IS_VECTOR(object))
            {
                PlVector *vec = PL_AS_VECTOR(object);

                if (PL_IS_NUMERAL(index))
                {
                    double value = PL_AS_NUMERAL(index);

                    if (floor(value) == value)
                    {
                        if (value < 0)
                            value += vec->items.count;

                        if (value <= (double)LONG_LONG_MAX)
                        {
                            if (value < vec->items.count)
                            {
                                vec->items.values[(long long)value] = thing;
                                break;
                            }

                            if (value == vec->items.count)
                            {
                                pl_writeValueArray(&vec->items, thing);
                                break;
                            }

                            pl_runtimeError("Vector index out of range; expected from 0 to %d",
                                vec->items.count);
                            return PL_RST_EXCEPTION;
                        }

                        pl_runtimeError("Integer overflow.");
                        return PL_RST_EXCEPTION;
                    }

                    pl_runtimeError("Vector index must be integers but not reals.");
                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid vector index; expected integer.");
                return PL_RST_EXCEPTION;
            }

            if (PL_IS_MAPPING(object))
            {
                PlMapping *map = PL_AS_MAPPING(object);

                if (!pl_hashable(index))
                {
                    pl_runtimeError("Attempting to set the index of a map with a non-hashable index.");
                    return PL_RST_EXCEPTION;
                }

                pl_mapSet(&map->map, index, thing);
                break;
            }

            if (PL_IS_STRING(object))
            {
                PlString *str = PL_AS_STRING(object);

                if (str->length == 0)
                {
                    pl_runtimeError("Accessing the index of an empty string.");
                    return PL_RST_EXCEPTION;
                }

                if (!PL_IS_STRING(thing))
                {
                    pl_runtimeError("Attempting to set a non-string value to part of a string.");
                    return PL_RST_EXCEPTION;
                }

                if (PL_AS_STRING(thing)->length > 1)
                {
                    pl_runtimeError("Setting more than 1 characters to part of a string.");
                    return PL_RST_EXCEPTION;
                }

                if (PL_IS_NUMERAL(index))
                {
                    double value = PL_AS_NUMERAL(index);

                    if (floor(value) == value)
                    {
                        if (value < 0)
                            value += str->length;

                        if (value <= (double)LONG_LONG_MAX)
                        {
                            if (value < str->length)
                            {
                                str->chars[(long long)value] = PL_AS_STRING(thing)->chars[0];
                                break;
                            }

                            pl_runtimeError("String index out of range; expected from 0 to %d", str->length - 1);
                            return PL_RST_EXCEPTION;
                        }

                        pl_runtimeError("Integer overflow.");
                        return PL_RST_EXCEPTION;
                    }

                    pl_runtimeError("String index must be integers but not reals.");
                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid string index; expected integer.");
                return PL_RST_EXCEPTION;
            }

            pl_runtimeError("Cannot index the object.");
            return PL_RST_EXCEPTION;
        }

        case PL_GET_LOCAL_LONG: {
            uint8_t slot = READ_LONG_BYTE();
            pl_push(frame->slots[slot]);
            break;
        }

        case PL_SET_LOCAL_LONG: {
            uint8_t slot = READ_LONG_BYTE();
            frame->slots[slot] = pl_peek(0);
            break;
        }

        case PL_GET_GLOBAL_LONG: {
            PlString *name = READ_LONG_STRING();
            PlValue value;

            if (!pl_hashGet(&vm.globals, name, &value))
            {
                pl_runtimeError("'%s' was not declared in the global scope.", name->chars);
                return PL_RST_EXCEPTION;
            }

            pl_push(value);
            break;
        }

        case PL_DEFINE_GLOBAL_LONG: {
            PlString *name = READ_LONG_STRING();
            pl_hashSet(&vm.globals, name, pl_peek(0));
            pl_pop();
            break;
        }

        case PL_SET_GLOBAL_LONG: {
            PlString *name = READ_LONG_STRING();

            if (pl_hashSet(&vm.globals, name, pl_peek(0)))
            {
                pl_hashDel(&vm.globals, name);
                pl_runtimeError("'%s' was not declared in the global scope.", name->chars);
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_GET_SURVALUE_LONG: {
            uint8_t slot = READ_LONG_BYTE();
            pl_push(*frame->closure->survalues[slot]->location);
            break;
        }

        case PL_SET_SURVALUE_LONG: {
            uint8_t slot = READ_LONG_BYTE();
            *frame->closure->survalues[slot]->location = pl_peek(0);
            break;
        }

        case PL_GET_PROPERTY_LONG: {
            if (!PL_IS_INSTANCE(pl_peek(0)))
            {
                pl_runtimeError("Request for member of non-class type.");
                return PL_RST_EXCEPTION;
            }

            PlInstance *instance = PL_AS_INSTANCE(pl_peek(0));
            PlString *name = READ_LONG_STRING();

            PlValue value;

            if (pl_hashGet(&instance->fields, name, &value))
            {
                pl_pop();
                pl_push(value);
                break;
            }

            if (!pl_bindMethod(instance->cls, name))
            {
                return PL_RST_EXCEPTION;
            }
            break;
        }

        case PL_SET_PROPERTY_LONG: {
            if (!PL_IS_INSTANCE(pl_peek(1)))
            {
                pl_runtimeError("Request for setting member of non-class type.");
                return PL_RST_EXCEPTION;
            }

            PlInstance *instance = PL_AS_INSTANCE(pl_peek(1));
            pl_hashSet(&instance->fields, READ_LONG_STRING(), pl_peek(0));
            PlValue value = pl_pop();
            pl_pop();
            pl_push(value);
            break;
        }

        case PL_GET_SUPER_LONG: {
            PlString *name = READ_LONG_STRING();
            PlClass *superclass = PL_AS_CLASS(pl_pop());

            if (!pl_bindMethod(superclass, name))
            {
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_EQUAL: {
            if (PL_IS_VECTOR(pl_peek(1)) && PL_IS_VECTOR(pl_peek(0)))
            {
                PlVector *second = PL_AS_VECTOR(pl_pop());
                PlVector *first = PL_AS_VECTOR(pl_pop());

                int c;

                if ((c = first->items.count) != second->items.count)
                {
                    pl_push(PL_FALSE_VALUE);
                    break;
                }

                pl_push(memcmp(first->items.values, second->items.values, c) == 0
                    ? PL_TRUE_VALUE : PL_FALSE_VALUE);
                break;
            }

            if (PL_IS_MAPPING(pl_peek(1)) && PL_IS_MAPPING(pl_peek(0)))
            {
                PlMapping *second = PL_AS_MAPPING(pl_pop());
                PlMapping *first = PL_AS_MAPPING(pl_pop());

                if (first->map.count != second->map.count)
                {
                    pl_push(PL_FALSE_VALUE);
                    break;
                }

                bool isEqual = true;

                for (int index = 0; index < first->map.capacity; index++)
                {
                    PlMapItem *item = &first->map.items[index];

                    if (!PL_IS_EMPTY(item->key))
                    {
                        PlValue secondOne;

                        if (!pl_mapGet(&second->map, item->key, &secondOne))
                        {
                            isEqual = false;
                            break;
                        }

                        if (item->value != secondOne)
                        {
                            isEqual = false;
                            break;
                        }
                    }
                }

                pl_push(isEqual ? PL_TRUE_VALUE : PL_FALSE_VALUE);
                break;
            }

            if (PL_IS_INSTANCE(pl_peek(1)))
            {
                if (pl_invoke(vm.equalString, 1, '='))
                {
                    frame = &vm.frames[vm.frameCount - 1];
                    break;
                }

                return PL_RST_EXCEPTION;
            }

            PlValue second = pl_pop();
            PlValue first = pl_pop();
            pl_push(PL_BOOL_VALUE(pl_compValues(first, second)));
            break;
        }

        case PL_GREATER: {
            if (PL_IS_STRING(pl_peek(0)) && PL_IS_STRING(pl_peek(1)))
            {
                PlString *second = PL_AS_STRING(pl_pop());
                PlString *first = PL_AS_STRING(pl_pop());
                pl_push(PL_BOOL_VALUE(strlen(first->chars) > strlen(second->chars)));
            }

            else if (PL_IS_NUMERAL(pl_peek(0)) && PL_IS_NUMERAL(pl_peek(1)))
            {
                double second = PL_AS_NUMERAL(pl_pop());
                double first = PL_AS_NUMERAL(pl_pop());
                pl_push(PL_BOOL_VALUE(first > second));
            }

            else if (PL_IS_VECTOR(pl_peek(0)) && PL_IS_VECTOR(pl_peek(1)))
            {
                PlVector *second = PL_AS_VECTOR(pl_pop());
                PlVector *first = PL_AS_VECTOR(pl_pop());
                pl_push(PL_BOOL_VALUE(first->items.count > second->items.count));
            }

            else
            {
                if (PL_IS_INSTANCE(pl_peek(1)))
                {
                    if (pl_invoke(vm.greaterString, 1, '>'))
                    {
                        frame = &vm.frames[vm.frameCount - 1];
                        break;
                    }

                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid operands to binary expression 'operator>'.");
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_LESS: {
            if (PL_IS_STRING(pl_peek(0)) && PL_IS_STRING(pl_peek(1)))
            {
                PlString *second = PL_AS_STRING(pl_pop());
                PlString *first = PL_AS_STRING(pl_pop());
                pl_push(PL_BOOL_VALUE(strlen(first->chars) < strlen(second->chars)));
            }

            else if (PL_IS_NUMERAL(pl_peek(0)) && PL_IS_NUMERAL(pl_peek(1)))
            {
                double second = PL_AS_NUMERAL(pl_pop());
                double first = PL_AS_NUMERAL(pl_pop());
                pl_push(PL_BOOL_VALUE(first < second));
            }

            else if (PL_IS_VECTOR(pl_peek(0)) && PL_IS_VECTOR(pl_peek(1)))
            {
                PlVector *second = PL_AS_VECTOR(pl_pop());
                PlVector *first = PL_AS_VECTOR(pl_pop());
                pl_push(PL_BOOL_VALUE(first->items.count < second->items.count));
            }

            else
            {
                if (PL_IS_INSTANCE(pl_peek(1)))
                {
                    if (pl_invoke(vm.lessString, 1, '<'))
                    {
                        frame = &vm.frames[vm.frameCount - 1];
                        break;
                    }

                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid operands to binary expression 'operator<'.");
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_ADD: {
            if (PL_IS_STRING(pl_peek(0)) && PL_IS_STRING(pl_peek(1)))
            {
                pl_concat();
            }

            else if (PL_IS_NUMERAL(pl_peek(0)) && PL_IS_NUMERAL(pl_peek(1)))
            {
                double second = PL_AS_NUMERAL(pl_pop());
                double first = PL_AS_NUMERAL(pl_pop());
                pl_push(PL_NUMERAL_VALUE(first + second));
            }

            else if (PL_IS_VECTOR(pl_peek(0)) && PL_IS_VECTOR(pl_peek(1)))
            {
                PlVector *second = PL_AS_VECTOR(pl_pop());
                PlVector *first = PL_AS_VECTOR(pl_pop());
                pl_concatValueArray(&first->items, &second->items);
                pl_push(PL_OBJECT_VALUE(first));
            }

            else
            {
                if (PL_IS_INSTANCE(pl_peek(1)))
                {
                    if (pl_invoke(vm.addString, 1, '+'))
                    {
                        frame = &vm.frames[vm.frameCount - 1];
                        break;
                    }

                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid operands to binary expression 'operator+'.");
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_SUBTRACT: {
            if (PL_IS_NUMERAL(pl_peek(0)) && PL_IS_NUMERAL(pl_peek(1)))
            {
                double second = PL_AS_NUMERAL(pl_pop());
                double first = PL_AS_NUMERAL(pl_pop());
                pl_push(PL_NUMERAL_VALUE(first - second));
            }

            else
            {
                if (PL_IS_INSTANCE(pl_peek(1)))
                {
                    if (pl_invoke(vm.subString, 1, '-'))
                    {
                        frame = &vm.frames[vm.frameCount - 1];
                        break;
                    }

                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid operands to binary expression 'operator-'.");
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_MULTIPLY: {
            if (PL_IS_NUMERAL(pl_peek(0)) && PL_IS_STRING(pl_peek(1))
                || PL_IS_NUMERAL(pl_peek(1)) && PL_IS_STRING(pl_peek(0)))
            {
                bool rst = pl_repeat();
                if (!rst)
                    return PL_RST_EXCEPTION;
            }

            else if (PL_IS_NUMERAL(pl_peek(0)) && PL_IS_VECTOR(pl_peek(1))
                     || PL_IS_NUMERAL(pl_peek(1)) && PL_IS_VECTOR(pl_peek(0)))
            {
                PlValue second = pl_peek(0);
                PlValue first = pl_peek(1);

                double times;
                PlVector *vector;

                if (PL_IS_NUMERAL(first))
                {
                    times = PL_AS_NUMERAL(first);
                    vector = PL_AS_VECTOR(second);
                }

                else
                {
                    times = PL_AS_NUMERAL(second);
                    vector = PL_AS_VECTOR(first);
                }

                if (times < 1 || floor(times) != times)
                {
                    pl_runtimeError("Can't repeat the vector by non-int or non-positive value %f", times);
                    return PL_RST_EXCEPTION;
                }

                if (times > (double)ULONG_LONG_MAX || times < 0)
                {
                    pl_runtimeError("Integer overflow.");
                    return PL_RST_EXCEPTION;
                }

                const int count = vector->items.count;

                while (--times > 0)
                {
                    for (int index = 0; index < count; index++)
                    {
                        pl_writeValueArray(&vector->items, vector->items.values[index]);
                    }
                }

                pl_pop();
                pl_pop();
                pl_push(PL_OBJECT_VALUE(vector));
            }

            else if (PL_IS_NUMERAL(pl_peek(0)) && PL_IS_NUMERAL(pl_peek(1)))
            {
                double second = PL_AS_NUMERAL(pl_pop());
                double first = PL_AS_NUMERAL(pl_pop());
                pl_push(PL_NUMERAL_VALUE(first * second));
            }

            else
            {
                if (PL_IS_INSTANCE(pl_peek(1)))
                {
                    if (pl_invoke(vm.mulString, 1, '*'))
                    {
                        frame = &vm.frames[vm.frameCount - 1];
                        break;
                    }

                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid operands to binary expression 'operator*'.");
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_DIVIDE: {
            if (PL_IS_NUMERAL(pl_peek(0)) && PL_IS_NUMERAL(pl_peek(1)))
            {
                double second = PL_AS_NUMERAL(pl_pop());
                double first = PL_AS_NUMERAL(pl_pop());
                pl_push(PL_NUMERAL_VALUE(first / second));
            }

            else
            {
                if (PL_IS_INSTANCE(pl_peek(1)))
                {
                    if (pl_invoke(vm.divString, 1, '/'))
                    {
                        frame = &vm.frames[vm.frameCount - 1];
                        break;
                    }

                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid operands to binary expression 'operator/'.");
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_MODULO: {
            if (PL_IS_NUMERAL(pl_peek(0)) && PL_IS_NUMERAL(pl_peek(1)))
            {
                double second = PL_AS_NUMERAL(pl_pop());
                double first = PL_AS_NUMERAL(pl_pop());

                if (first > (double)INT64_MAX || second > (double)INT64_MAX)
                {
                    pl_runtimeError("Integer Overflow.");
                    return PL_RST_EXCEPTION;
                }

                pl_push(PL_NUMERAL_VALUE((long long int)first % (long long int)second));
            }

            else
            {
                if (PL_IS_INSTANCE(pl_peek(1)))
                {
                    if (pl_invoke(vm.modString, 1, '%'))
                    {
                        frame = &vm.frames[vm.frameCount - 1];
                        break;
                    }

                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid operands to binary expression 'operator%'.");
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_NOT:
            pl_push(PL_BOOL_VALUE(pl_isFalse(pl_pop())));
            break;

        case PL_NEGATE: {
            if (PL_IS_NUMERAL(pl_peek(0)))
            {
                pl_push(PL_NUMERAL_VALUE(-PL_AS_NUMERAL(pl_pop())));
            }

            else
            {
                if (PL_IS_INSTANCE(pl_peek(0)))
                {
                    if (pl_invoke(vm.negateString, 0, 'n'))
                    {
                        frame = &vm.frames[vm.frameCount - 1];
                        break;
                    }

                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid operands to unary expression 'operator--'.");
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_AFFIRM: {
            if (PL_IS_NUMERAL(pl_peek(0)))
            {
                pl_push(PL_NUMERAL_VALUE(+PL_AS_NUMERAL(pl_pop())));
            }

            else
            {
                if (PL_IS_INSTANCE(pl_peek(0)))
                {
                    if (pl_invoke(vm.affirmString, 0, 'a'))
                    {
                        frame = &vm.frames[vm.frameCount - 1];
                        break;
                    }

                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid operands to unary expression 'operator++'.");
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_BITNOT: {
            if (PL_IS_NUMERAL(pl_peek(0)))
            {
                double first = PL_AS_NUMERAL(pl_pop());

                if (first > (double)INT64_MAX)
                {
                    pl_runtimeError("Integer Overflow.");
                    return PL_RST_EXCEPTION;
                }

                pl_push(PL_NUMERAL_VALUE(~(long long int)first));
            }

            else if (PL_IS_VECTOR(pl_peek(0)))
            {
                pl_reverseValueArray(&PL_AS_VECTOR(pl_peek(0))->items);
                break;
            }

            else if (PL_IS_STRING(pl_peek(0)))
            {
                PlString *str = PL_AS_STRING(pl_peek(0));

                for (int index = 0; index < str->length / 2; index++)
                {
                    char temp = str->chars[index];
                    str->chars[index] = str->chars[str->length - index - 1];
                    str->chars[str->length - index - 1] = temp;
                }
            }

            else
            {
                if (PL_IS_INSTANCE(pl_peek(0)))
                {
                    if (pl_invoke(vm.bitnotString, 0, '~'))
                    {
                        frame = &vm.frames[vm.frameCount - 1];
                        break;
                    }

                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid operands to unary expression 'operator~'.");
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_BITAND: {
            if (PL_IS_NUMERAL(pl_peek(0)) && PL_IS_NUMERAL(pl_peek(1)))
            {
                double second = PL_AS_NUMERAL(pl_pop());
                double first = PL_AS_NUMERAL(pl_pop());

                if (first > (double)INT64_MAX || second > (double)INT64_MAX)
                {
                    pl_runtimeError("Integer Overflow.");
                    return PL_RST_EXCEPTION;
                }

                pl_push(PL_NUMERAL_VALUE((long long int)first & (long long int)second));
            }

            else
            {
                if (PL_IS_INSTANCE(pl_peek(1)))
                {
                    if (pl_invoke(vm.bitandString, 1, '&'))
                    {
                        frame = &vm.frames[vm.frameCount - 1];
                        break;
                    }

                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid operands to binary expression 'operator&'.");
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_BITXOR: {
            if (PL_IS_NUMERAL(pl_peek(0)) && PL_IS_NUMERAL(pl_peek(1)))
            {
                double second = PL_AS_NUMERAL(pl_pop());
                double first = PL_AS_NUMERAL(pl_pop());

                if (first > (double)INT64_MAX || second > (double)INT64_MAX)
                {
                    pl_runtimeError("Integer Overflow.");
                    return PL_RST_EXCEPTION;
                }

                pl_push(PL_NUMERAL_VALUE((long long int)first ^ (long long int)second));
                break;
            }

            if (PL_IS_BOOL(pl_peek(0)) && PL_IS_BOOL(pl_peek(0)))
            {
                bool second = PL_AS_BOOL(pl_pop()) == true;
                bool first = PL_AS_BOOL(pl_pop()) == true;

                pl_push(first ^ second ? PL_TRUE_VALUE : PL_FALSE_VALUE);
                break;
            }

            else
            {
                if (PL_IS_INSTANCE(pl_peek(1)))
                {
                    if (pl_invoke(vm.bitxorString, 1, '^'))
                    {
                        frame = &vm.frames[vm.frameCount - 1];
                        break;
                    }

                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid operands to binary expression 'operator^'.");
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_BITOR: {
            if (PL_IS_NUMERAL(pl_peek(0)) && PL_IS_NUMERAL(pl_peek(1)))
            {
                double second = PL_AS_NUMERAL(pl_pop());
                double first = PL_AS_NUMERAL(pl_pop());

                if (first > (double)(double)INT64_MAX || second > (double)(double)INT64_MAX)
                {
                    pl_runtimeError("Integer Overflow.");
                    return PL_RST_EXCEPTION;
                }

                pl_push(PL_NUMERAL_VALUE((long long int)first | (long long int)second));
                break;
            }

            if (PL_IS_MAPPING(pl_peek(0)) && PL_IS_MAPPING(pl_peek(1)))
            {
                PlMapping *second = PL_AS_MAPPING(pl_pop());
                PlMapping *first = PL_AS_MAPPING(pl_peek(0));

                pl_mapAdd(&second->map, &first->map);
                break;
            }

            else
            {
                if (PL_IS_INSTANCE(pl_peek(1)))
                {
                    if (pl_invoke(vm.bitorString, 1, '|'))
                    {
                        frame = &vm.frames[vm.frameCount - 1];
                        break;
                    }

                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid operands to binary expression 'operator|'.");
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_LSHIFT: {
            if (PL_IS_NUMERAL(pl_peek(0)) && PL_IS_NUMERAL(pl_peek(1)))
            {
                double second = PL_AS_NUMERAL(pl_pop());
                double first = PL_AS_NUMERAL(pl_pop());

                if (first > (double)INT64_MAX || second > (double)INT64_MAX)
                {
                    pl_runtimeError("Integer Overflow.");
                    return PL_RST_EXCEPTION;
                }

                pl_push(PL_NUMERAL_VALUE((long long int)first << (long long int)second));
            }

            else
            {
                if (PL_IS_INSTANCE(pl_peek(1)))
                {
                    if (pl_invoke(vm.lshiftString, 1, 'l'))
                    {
                        frame = &vm.frames[vm.frameCount - 1];
                        break;
                    }

                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid operands to binary expression 'operator<<'.");
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_RSHIFT: {
            if (PL_IS_NUMERAL(pl_peek(0)) && PL_IS_NUMERAL(pl_peek(1)))
            {
                double second = PL_AS_NUMERAL(pl_pop());
                double first = PL_AS_NUMERAL(pl_pop());

                if (first > (double)INT64_MAX || second > (double)INT64_MAX)
                {
                    pl_runtimeError("Integer Overflow.");
                    return PL_RST_EXCEPTION;
                }

                pl_push(PL_NUMERAL_VALUE((long long int)first >> (long long int)second));
            }

            else
            {
                if (PL_IS_INSTANCE(pl_peek(1)))
                {
                    if (pl_invoke(vm.rshiftString, 1, 'r'))
                    {
                        frame = &vm.frames[vm.frameCount - 1];
                        break;
                    }

                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid operands to binary expression 'operator>>'.");
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_ADD_1: {
            if (PL_IS_NUMERAL(pl_peek(0)))
            {
                pl_push(PL_NUMERAL_VALUE(PL_AS_NUMERAL(pl_pop()) + PL_AS_NUMERAL(4607182418800017408llu)));
            }

            else
            {
                if (PL_IS_INSTANCE(pl_peek(1)))
                {
                    pl_push(PL_NUMERAL_VALUE(1));

                    if (pl_invoke(vm.addString, 1, '+'))
                    {
                        frame = &vm.frames[vm.frameCount - 1];
                        break;
                    }

                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid operands to binary expression 'operator+'.");
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_DEC_1: {
            if (PL_IS_NUMERAL(pl_peek(0)))
            {
                pl_push(PL_NUMERAL_VALUE(PL_AS_NUMERAL(pl_pop()) - PL_AS_NUMERAL(4607182418800017408llu)));
            }

            else
            {
                if (PL_IS_INSTANCE(pl_peek(1)))
                {
                    pl_push(PL_NUMERAL_VALUE(1));

                    if (pl_invoke(vm.subString, 1, '-'))
                    {
                        frame = &vm.frames[vm.frameCount - 1];
                        break;
                    }

                    return PL_RST_EXCEPTION;
                }

                pl_runtimeError("Invalid operands to binary expression 'operator-'.");
                return PL_RST_EXCEPTION;
            }

            break;
        }

        case PL_PRINT: {
            pl_reprValues(pl_pop());
            printf("\n");
            break;
        }

        case PL_PRINT_EMPTY: {
            printf("\n");
            break;
        }

        case PL_JUMP: {
            uint16_t offset = READ_SHORT();
            frame->ip += offset;
            break;
        }

        case PL_JUMP_IF_FALSE: {
            uint16_t offset = READ_SHORT();
            if (pl_isFalse(pl_peek(0)))
                frame->ip += offset;
            break;
        }

        case PL_LOOP: {
            uint16_t offset = READ_SHORT();
            frame->ip -= offset;
            break;
        }

        case PL_CALL: {
            int argCount = READ_BYTE();

            if (!pl_callValue(pl_peek(argCount), argCount))
            {
                return PL_RST_EXCEPTION;
            }

            frame = &vm.frames[vm.frameCount - 1];
            break;
        }

        case PL_INVOKE: {
            PlString *method = READ_STRING();
            int argCount = READ_BYTE();

            if (!pl_invoke(method, argCount, '\0'))
            {
                return PL_RST_EXCEPTION;
            }

            frame = &vm.frames[vm.frameCount - 1];
            break;
        }

        case PL_SUPER_INVOKE: {
            PlString *method = READ_STRING();
            int argCount = READ_BYTE();
            PlClass *superclass = PL_AS_CLASS(pl_pop());

            if (!pl_invokeFromClass(superclass, method, argCount, '\0'))
            {
                return PL_RST_EXCEPTION;
            }

            frame = &vm.frames[vm.frameCount - 1];
            break;
        }

        case PL_CLOSURE: {
            PlFunction *function = PL_AS_FUNCTION(READ_CONSTANT());
            PlClosure *closure = pl_newClosure(function);
            pl_push(PL_OBJECT_VALUE(closure));

            for (int index = 0; index < closure->survalueCount; index++)
            {
                uint8_t isLocal = READ_BYTE();
                uint8_t idx = READ_BYTE();

                if (isLocal)
                {
                    closure->survalues[index] =
                    pl_captureSurvalue(frame->slots + idx);
                }

                else
                {
                    closure->survalues[index] = frame->closure->survalues[idx];
                }
            }

            break;
        }

        case PL_CLOSE_SURVALUE:
            pl_closeSurvalues(vm.stackTop - 1);
            pl_pop();
            break;

        case PL_RETURN: {
            PlValue result = pl_pop();
            pl_closeSurvalues(frame->slots);
            vm.frameCount--;

            if (vm.frameCount == 0)
            {
                pl_pop();
                return PL_RST_FINE;
            }

            vm.stackTop = frame->slots;
            pl_push(result);
            frame = &vm.frames[vm.frameCount - 1];
            break;
        }

        case PL_CLASS: {
            PlClass *cls = pl_newClass(READ_STRING());
            pl_push(PL_OBJECT_VALUE(cls));
            break;
        }

        case PL_INHERIT: {
            PlValue superclass = pl_peek(1);

            if (!PL_IS_CLASS(superclass))
            {
                pl_runtimeError("Expected class-name.");
                return PL_RST_EXCEPTION;
            }

            PlClass *supercls = PL_AS_CLASS(superclass);
            PlClass *subclass = PL_AS_CLASS(pl_peek(0));

            pl_hashAdd(&supercls->methods, &subclass->methods);
            pl_pop();
            break;
        }

        case PL_METHOD:
            pl_defMethod(READ_STRING());
            break;

        case PL_INVOKE_LONG: {
            PlString *method = READ_LONG_STRING();
            int argCount = READ_BYTE();

            if (!pl_invoke(method, argCount, '\0'))
            {
                return PL_RST_EXCEPTION;
            }

            frame = &vm.frames[vm.frameCount - 1];
            break;
        }

        case PL_SUPER_INVOKE_LONG: {
            PlString *method = READ_LONG_STRING();
            int argCount = READ_BYTE();
            PlClass *superclass = PL_AS_CLASS(pl_pop());

            if (!pl_invokeFromClass(superclass, method, argCount, '\0'))
            {
                return PL_RST_EXCEPTION;
            }

            frame = &vm.frames[vm.frameCount - 1];
            break;
        }

        case PL_CLOSURE_LONG: {
            PlFunction *function = PL_AS_FUNCTION(READ_LONG_CONSTANT());
            PlClosure *closure = pl_newClosure(function);
            pl_push(PL_OBJECT_VALUE(closure));

            for (int i = 0; i < closure->survalueCount; i++)
            {
                uint8_t isLocal = READ_BYTE();
                uint8_t idx = READ_BYTE();

                if (isLocal)
                {
                    closure->survalues[i] =
                    pl_captureSurvalue(frame->slots + idx);
                }

                else
                {
                    closure->survalues[i] = frame->closure->survalues[idx];
                }
            }

            break;
        }

        case PL_CLASS_LONG: {
            PlClass *cls = pl_newClass(READ_LONG_STRING());
            pl_push(PL_OBJECT_VALUE(cls));
            break;
        }

        case PL_METHOD_LONG:
            pl_defMethod(READ_LONG_STRING());
            break;

        case PL_VECTOR: {
            PlVector *vector = pl_newVector();
            PlValueArray *array = &vector->items;
            uint8_t size = READ_BYTE();

            for (int index = 0; index < size; index++)
            {
                pl_writeValueArray(array, pl_pop());
            }

            pl_reverseValueArray(array);
            pl_push(PL_OBJECT_VALUE(vector));
            break;
        }

        case PL_MAP: {
            PlMapping *map = pl_newMapping();
            uint8_t size = READ_BYTE();

            for (int index = 0; index < size; index++)
            {
                PlValue value = pl_pop();
                PlValue key = pl_pop();

                if (!pl_hashable(key))
                {
                    pl_runtimeError("Unhashable key.");
                    return PL_RST_EXCEPTION;
                }

                pl_mapSet(&map->map, key, value);
            }

            pl_push(PL_OBJECT_VALUE(map));
            break;
        }

        case PL_SIZEOF: {
            PlValue value = pl_pop();

            if (PL_IS_NUMERAL(value))
            {
                pl_push(PL_NUMERAL_VALUE(sizeof(double)));
                break;
            }

            if (PL_IS_STRING(value))
            {
                pl_push(PL_NUMERAL_VALUE(PL_AS_STRING(value)->length));
                break;
            }

            if (PL_IS_BOOL(value) || PL_IS_NULL(value))
            {
                pl_push(4607182418800017408llu);
                break;
            }

            if (PL_IS_VECTOR(value))
            {
                pl_push(PL_NUMERAL_VALUE(PL_AS_VECTOR(value)->items.count));
                break;
            }

            if (PL_IS_MAPPING(value))
            {
                pl_push(PL_NUMERAL_VALUE(PL_AS_MAPPING(value)->map.count));
                break;
            }

            pl_push(PL_NUMERAL_VALUE(sizeof(value)));
            break;
        }

        default:
            break;
        }
    }

#undef READ_BYTE
#undef READ_SHORT
#undef READ_CONSTANT
#undef READ_STRING
#undef BINARY_OP
}

static bool pl_invoke(PlString *name, int argCount, char errorOp)
{
    PlValue receiver = pl_peek(argCount);

    if (!PL_IS_INSTANCE(receiver))
    {
        pl_runtimeError("Request for invoking '%s' of non-class type.", name->chars);
        return false;
    }

    PlInstance *instance = PL_AS_INSTANCE(receiver);
    PlValue value;

    if (pl_hashGet(&instance->fields, name, &value))
    {
        vm.stackTop[-argCount - 1] = value;
        return pl_callValue(value, argCount);
    }

    return pl_invokeFromClass(instance->cls, name, argCount, errorOp);
}

static bool pl_invokeFromClass(PlClass *cls, PlString *name, int argCount, char errorOp)
{
    PlValue method;

    if (!pl_hashGet(&cls->methods, name, &method))
    {
        if (errorOp != '\0')
        {
            switch (errorOp)
            {
            case '+':
                pl_runtimeError("No match for 'operator+'.");
                break;
            case '-':
                pl_runtimeError("No match for 'operator-'.");
                break;
            case '*':
                pl_runtimeError("No match for 'operator*'.");
                break;
            case '/':
                pl_runtimeError("No match for 'operator/'.");
                break;
            case '%':
                pl_runtimeError("No match for 'operator%'.");
                break;
            case '&':
                pl_runtimeError("No match for 'operator&'.");
                break;
            case '|':
                pl_runtimeError("No match for 'operator|'.");
                break;
            case '^':
                pl_runtimeError("No match for 'operator^'.");
                break;
            case '~':
                pl_runtimeError("No match for 'operator~'.");
                break;
            case 'l':
                pl_runtimeError("No match for 'operator<<'.");
                break;
            case 'r':
                pl_runtimeError("No match for 'operator>>'.");
                break;
            case '>':
                pl_runtimeError("No match for 'operator>'.");
                break;
            case '<':
                pl_runtimeError("No match for 'operator<'.");
                break;
            case '=':
                pl_runtimeError("No match for 'operator=='.");
                break;
            case 'n':
                pl_runtimeError("No match for 'operator--'.");
                break;
            case 'a':
                pl_runtimeError("No match for 'operator++'.");
                break;
            default:
                pl_runtimeError("No match for 'operator'.");
                break;
            }

            return false;
        }

        else
        {
            pl_runtimeError("Cannot resolve property '%s'.", name->chars);
            return false;
        }
    }

    return pl_call(PL_AS_CLOSURE(method), argCount);
}

static bool pl_isFalse(PlValue value)
{
    if (PL_IS_VECTOR(value))
        return PL_AS_VECTOR(value)->items.count == 0;
    if (PL_IS_MAPPING(value))
        return PL_AS_MAPPING(value)->map.count == 0;
    return PL_IS_NULL(value) || (PL_IS_BOOL(value) && !PL_AS_BOOL(value)) || PL_NUMERAL_VALUE(value) == 0;
}

static PlValue pl_peek(int distance)
{
    return vm.stackTop[-1 - distance];
}

static bool pl_repeat()
{
    PlValue second = pl_peek(0);
    PlValue first = pl_peek(1);

    double times;
    PlString *string;

    if (PL_IS_NUMERAL(first))
    {
        times = PL_AS_NUMERAL(first);
        string = PL_AS_STRING(second);
    }

    else
    {
        times = PL_AS_NUMERAL(second);
        string = PL_AS_STRING(first);
    }

    if (times < 1 || floor(times) != times)
    {
        pl_runtimeError("Can't repeat the string by non-int or non-positive value %f", times);
        return false;
    }

    if (times > (double)ULONG_LONG_MAX || times < 0)
    {
        pl_runtimeError("Integer Overflow.");
        return false;
    }

    char *rst = malloc(string->length * (unsigned long long)times);

    if (rst == NULL)
    {
        pl_runtimeError("Memory allocation failed.");
        return false;
    }

    strcpy(rst, string->chars);

    while (--times > 0)
    {
        strcat(rst, string->chars);
    }

    pl_pop();
    pl_pop();
    pl_push(PL_OBJECT_VALUE(pl_takeString(rst, strlen(rst))));

    return true;
}

static void pl_resetStack()
{
    vm.stackTop = vm.stack;
    vm.frameCount = 0;
    vm.openSurvalues = NULL;
}

static void pl_runtimeError(const char *format, ...)
{
    fprintf(stderr, "Callstack Traceback (most recent call first):\n");

    for (int index = vm.frameCount - 1; index >= 0; index--)
    {
        PlCallFrame *frame = &vm.frames[index];
        PlFunction *function = frame->closure->function;
        size_t instruction = frame->ip - function->segment.code - 1;
        fprintf(stderr, "  Line %d, in ", pl_getLine(&function->segment, (int)instruction));

        if (function->name == NULL)
        {
            fprintf(stderr, "<module>\n");
        }

        else
        {
            fprintf(stderr, "%s\n", function->name->chars);
        }
    }

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    pl_resetStack();
}
