#ifndef PLVM_H
#define PLVM_H

#include <PlCallFrame>
#include <PlExecRst>

#define PL_FRAMES_MAX 64
#define PL_STACK_MAX (PL_FRAMES_MAX * UINT8_COUNT)

typedef struct PlVM
{
    PlCallFrame frames[PL_FRAMES_MAX];
    int frameCount;

    PlValue stack[PL_STACK_MAX];
    PlValue *stackTop;
    PlHash globals;
    PlHash strings;

    PlString *initString;
    PlString *addString;
    PlString *subString;
    PlString *mulString;
    PlString *divString;
    PlString *modString;
    PlString *bitandString;
    PlString *bitorString;
    PlString *bitxorString;
    PlString *bitnotString;
    PlString *negateString;
    PlString *affirmString;
    PlString *lshiftString;
    PlString *rshiftString;
    PlString *greaterString;
    PlString *lessString;
    PlString *equalString;

    PlSurvalue *openSurvalues;

    size_t bytesAllocated;
    size_t nextGC;
    PlObject *objects;
    int grayCount;
    int grayCapacity;
    PlObject* *grayStack;
    bool markValue;
} PlVM;

extern PlVM vm;

void pl_initVM();
void pl_freeVM();
PlExecResult pl_interpret(const char *source);
void pl_push(PlValue value);
PlValue pl_pop();
bool pl_isFalse(PlValue value);

void pl_handleSignal(int signal);

#endif // PLVM_H
