#ifndef PLCOMPILER_H
#define PLCOMPILER_H

#include <PlVM>

PlFunction *pl_compile(const char* source);
void pl_markCompilerRoots();

#endif // PLCOMPILER_H
