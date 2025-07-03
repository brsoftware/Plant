#ifndef PLEXECRST_H
#define PLEXECRST_H

typedef enum PlExecResult
{
    PL_RST_FINE,
    PL_RST_ERROR,
    PL_RST_EXCEPTION,
    PL_RST_UNTERMINATED = 64
} PlExecResult;

#endif // PLEXECRST_H
