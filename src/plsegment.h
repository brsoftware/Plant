#ifndef PLSEGMENT_H
#define PLSEGMENT_H

#include <PlCode>
#include <PlLine>
#include <PlValue>

typedef struct PlSegment
{
    int count;
    int capacity;
    uint8_t *code;
    PlValueArray constants;
    int lineCount;
    int lineCapacity;
    PlLine *lines;
} PlSegment;

void pl_initSegment(PlSegment *segment);
void pl_freeSegment(PlSegment *segment);
void pl_writeSegment(PlSegment *segment, uint8_t byte, int line);

int pl_writeConstant(PlSegment *segment, PlValue value, int line);
int pl_addConstant(PlSegment *segment, PlValue value);

int pl_getLine(const PlSegment *segment, int instruction);

#endif // PLSEGMENT_H
