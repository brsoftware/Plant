#include <stdlib.h>

#include <PlMemory>
#include <PlVM>

void pl_initSegment(PlSegment *segment)
{
    segment->count = 0;
    segment->capacity = 0;
    segment->code = NULL;
    segment->lineCount = 0;
    segment->lineCapacity = 0;
    segment->lines = NULL;
    pl_initValueArray(&segment->constants);
}

void pl_freeSegment(PlSegment *segment)
{
    PL_FREE_ARRAY(uint8_t, segment->code, segment->capacity);
    PL_FREE_ARRAY(int, segment->lines, segment->capacity);
    pl_freeValueArray(&segment->constants);
    pl_initSegment(segment);
    PL_FREE_ARRAY(PlLine, segment->lines, segment->lineCapacity);
}

void pl_writeSegment(PlSegment *segment, uint8_t byte, int line)
{
    if (segment->capacity < segment->count + 1)
    {
        int oldCapacity = segment->capacity;
        segment->capacity = PL_GROW_CAPAC(oldCapacity);
        segment->code = PL_GROW_ARRAY(uint8_t, segment->code, oldCapacity, segment->capacity);
    }

    segment->code[segment->count] = byte;
    segment->count++;

    if (segment->lineCount > 0 && segment->lines[segment->lineCount - 1].line == line)
    {
        return;
    }

    if (segment->lineCapacity < segment->lineCount + 1)
    {
        int oldCapacity = segment->lineCapacity;
        segment->lineCapacity = PL_GROW_CAPAC(oldCapacity);
        segment->lines = PL_GROW_ARRAY(PlLine, segment->lines, oldCapacity, segment->lineCapacity);
    }

    PlLine *lineStart = &segment->lines[segment->lineCount++];
    lineStart->offset = segment->count - 1;
    lineStart->line = line;
}

int pl_writeConstant(PlSegment *segment, PlValue value, int line)
{
    int index = pl_addConstant(segment, value);

    if (index < 256)
    {
        pl_writeSegment(segment, PL_CONSTANT, line);
        pl_writeSegment(segment, (uint8_t)index, line);
    }

    else
    {
        pl_writeSegment(segment, PL_CONSTANT_LONG, line);
        pl_writeSegment(segment, (uint8_t)(index & 0xff), line);
        pl_writeSegment(segment, (uint8_t)((index >> 8) & 0xff), line);
        pl_writeSegment(segment, (uint8_t)((index >> 16) & 0xff), line);
    }

    return index;
}

int pl_addConstant(PlSegment *segment, PlValue value)
{
    pl_push(value);
    pl_writeValueArray(&segment->constants, value);
    pl_pop();
    return segment->constants.count - 1;
}

int pl_getLine(const PlSegment *segment, int instruction)
{
    int start = 0;
    int end = segment->lineCount - 1;

    for (;;)
    {
        int mid = (start + end) / 2;
        PlLine *line = &segment->lines[mid];

        if (instruction < line->offset)
        {
            end = mid - 1;
        }

        else if (mid == segment->lineCount - 1 || instruction < segment->lines[mid + 1].offset)
        {
            return line->line;
        }

        else
        {
            start = mid + 1;
        }
    }
}
