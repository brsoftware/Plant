#ifndef PLCALLFRAME_H
#define PLCALLFRAME_H

#include <PlObject>

typedef struct PlCallFrame
{
    PlClosure *closure;
    uint8_t *ip;
    PlValue *slots;
} PlCallFrame;

#endif // PLCALLFRAME_H
