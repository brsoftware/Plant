#ifndef PLGLOBALS_H
#define PLGLOBALS_H

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define UINT8_COUNT (UINT8_MAX + 1)

#define PL_VERSION_MAJOR 0
#define PL_VERSION_MINOR 8
#define PL_VERSION_MICRO 0

#if defined(__GNUC__)
    #if defined(__llvm__)
        #if defined(__clang__)
            #define PL_COMPILER_NAME "GNU/LLVM/Clang v.%d.%d.%d"
        #elif defined(__APPLE_CC__)
            #define PL_COMPILER_NAME "GNU/LLVM/Applc GCC v.%d.%d.%d"
        #else
            #define PL_COMPILER_NAME "GNU/LLVM/GCC v.%d.%d.%d"
        #endif
    #elif defined(__APPLE_CC__)
        #define PL_COMPILER_NAME "GNU/Apple GCC v.%d.%d.%d"
    #elif defined(__MINGW32__) || defined(__MINGW64__)
        #define PL_COMPILER_NAME "GNU/Mingw GCC v.%d.%d.%d"
    #else
        #define PL_COMPILER_NAME "GNU/GCC v.%d.%d.%d"
    #endif
    #define PL_COMPILER_FORMAT __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__
#elif defined(_MSC_VER)
    #define PL_COMPILER_NAME "MSVC v.%d.%d"
    #define PL_COMPILER_FORMAT _MSC_VER / 100, _MSC_VER % 100
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
    #if defined(__SUNPRO_C)
        #define PL_COMPILER_NAME "Sun C v.%x"
        #define PL_COMPILER_FORMAT __SUNPRO_C
    #else
        #define PL_COMPILER_NAME "Sun C++ v.%x"
        #define PL_COMPILER_FORMAT __SUNPRO_CC
    #endif
#elif defined(__INTEL_COMPILER)
    #define PL_COMPILER_NAME "Intel v.%d.%d"
    #define PL_COMPILER_FORMAT __INTEL_COMPILER / 100, (__INTEL_COMPILER % 100) / 10
#elif defined(__WATCOMC__)
    #define PL_COMPILER_NAME "Watcom v.%d.%d"
    #define PL_COMPILER_FORMAT __WATCOMC__ / 100, __WATCOMC__ % 100
#elif defined(__DMC__)
    #define PL_COMPILER_NAME "Digital Mars v.%x"
    #define PL_COMPILER_FORMAT __DMC__
#else
    #define PL_COMPILER_FORMAT "unknown"
    #if defined(__PCC__)
        #define PL_COMPILER "Portable C v.%s"
    #elif defined(__HP_cc) || defined (__HP_aCC)
        #if defined(__HP_cc)
            #define PL_COMPILER "HP v.%s"
        #else
            #define PL_COMPILER "HP C++ v.%s"
        #endif
    #elif defined(__BORLANDC__)
        #define PL_COMPILER "Borland v.%s"
    #elif defined(KENC) || defined(KENCC)
        #define PL_COMPILER "Plan 9 v.%s"
    #elif defined (__sgi) || defined(sgi)
        #define PL_COMPILER "MIPS v.%s"
    #elif defined(__MWERKS__)
        #define PL_COMPILER "Metrowerks CodeWarrior v.%s"
    #elif defined(__TURBOC__)
        #define PL_COMPILER "Borland/Turbo C v.%s"
    #elif defined(__QC))
        #define PL_COMPILER "QuickC v.%s"
    #elif defined(__SC__)
        #define PL_COMPILER "Symantec v.%s"
    #elif defined(__ZTC__)
        #define PL_COMPILER "Zortech C v.%s"
    #else
        #define PL_COMPILER "Unknown v.%s"
    #endif
#endif

#if defined(__APPLE__)
    #define PL_OS "darwin"
#elif defined(__FreeBSD__)
    #define PL_OS "freebsd"
#elif defined(__NetBSD__)
    #define PL_OS "netbsd"
#elif defined(__OpenBSD__)
    #define PL_OS "openbsd"
#elif defined(__DragonFly__)
    #define PL_OS "dragonfly"
#elif defined(__linux) || defined(linux)
    #define PL_OS "linux"
#elif defined(sun) || defined(__sun)
#if defined(__SVR4) || defined(__svr4__)
    #define PL_OS "solaris"
#else
    #define PL_OS "sun"
#endif
#elif defined(__HAIKU__)
    #define PL_OS "haiku"
#elif defined(__WIN32) || defined(__WIN64)
    #define PL_OS "win32"
#elif defined(__REACTOS__)
    #define PL_OS "reactos"
#elif defined(__GNU__)
    #define PL_OS "hurd"
#elif defined(_AIX)
    #define PL_OS "aix"
#elif defined(__hpux) || defined(hpux)
    #define PL_OS "hpux"
#elif defined(__minix)
    #define PL_OS "minix"
#elif defined(PLAN9)
    #define PL_OS "plan9"
#elif defined(__QNXNTO__)
    #define PL_OS "qnx neutrino"
#elif defined(__QNX__)
    #define PL_OS "qnx"
#elif defined(_MIPS_ISA) || defined(_SGI_SOURCE)
    #define PL_OS "irix"
#elif defined(__BEOS__)
    #define PL_OS "beos"
#elif defined(__unix__)
    #define PL_OS "unix"
#elif defined(NEXTSTEP)
    #define PL_OS "nextstep"
#elif defined(OPENSTEP)
    #define PL_OS "openstep"
#elif defined(macintosh)
    #define PL_OS "mac"
#elif defined(__OS2__) || defined(OS2) || defined(_OS2)
    #define PL_OS "os2"
#elif defined(AMIGA) || defined(__amigaos__)
    #define PL_OS "amiga"
#elif defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
    #define PL_OS "dos"
#else
    #define PL_OS "Unknown Operating System"
#endif

#endif // PLGLOBALS_H
