#ifndef HAP_DEBUG_H
#define HAP_DEBUG_H
/*==============================================================================
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "AEEStdDef.h"
#include <stdarg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HAP_LEVEL_LOW 0
#define HAP_LEVEL_MEDIUM 1
#define HAP_LEVEL_HIGH 2
#define HAP_LEVEL_ERROR 3
#define HAP_LEVEL_FATAL 4

//Add a weak reference so shared objects work with older images
#pragma weak HAP_debug_v2

void HAP_debug_v2(int level, const char* file, int line, const char* format, ...);

// Keep these around to support older shared objects and older images
void HAP_debug(const char *msg, int level, const char *filename, int line);

static __inline void _HAP_debug_v2(int level, const char* file, int line,
                          const char* format, ...){
// Work around for hexagon-clang bug - 15123
#ifdef __llvm__
    HAP_debug(format, level, file, line);
#else
   char buf[256];
   va_list args;
   va_start(args, format);
   vsnprintf(buf, sizeof(buf), format, args);
   va_end(args);
   HAP_debug(buf, level, file, line);
#endif
}

#define HAP_DEBUG_TRACEME 0

long HAP_debug_ptrace(int req, unsigned int pid, void* addr, void* data);

#ifdef __cplusplus
}
#endif

#endif // HAP_DEBUG_H

