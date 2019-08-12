#ifndef HAP_FARF_H
#define HAP_FARF_H
/*==============================================================================
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "AEEStdDef.h"
#include "HAP_debug.h"

/**
 * FARF macro used for logging
 *
 * Logging is controlled via conditional compilation. A FARF
 * level should be defined to 1 for FARF macros to be compiled
 * in. For example:
 *
 *    #define FARF_LOW 1
 *    #include "HAP_farf.h"
 *
 *    FARF(LOW, "something happened: %s", (const char*)string);
 *
 * If FARF_LOW is defined to 0, as it is by default, the above
 * FARF string will not be compiled in, if it is defined to 1 it
 * will be compiled in.  Users can also define their own custom
 * levels. For example:
 *
 *    #include "HAP_farf.h"
 *    #define FARF_MYTRACE 1
 *    #define FARF_MYTRACE_LEVEL HAP_LEVEL_LOW
 *
 *    FARF(MYTRACE, "custom trace in file %s on line %d", __FILE__, __LINE__);
 *
 * The LEVEL define tells FARF what runtime logging level to
 * use.  These are  mapped to their diag level counterparts, in
 * the above example the message will be logged to diag's LOW
 * level.
 *
 * When building the Debug variant or builds defining _DEBUG the
 * following FARF levels will be enabled:
 *
 *    ALWAYS
 *    HIGH
 *    ERROR
 *    FATAL
 *
 * @param x, the FARF level defined to either 0 to disable compilation or 1 to enable.
 * @param ..., format string and arguments.
 */
#define FARF(x, ...) _FARF_PASTE(_FARF_,_FARF_VAL(FARF_##x))(x, ##__VA_ARGS__)


/* by default _DEBUG turns on ALWAYS, HIGH, ERROR, FATAL
 *
 */
#ifdef _DEBUG
#ifndef FARF_HIGH
#define FARF_HIGH 1
#endif
#ifndef FARF_ERROR
#define FARF_ERROR 1
#endif
#ifndef FARF_FATAL
#define FARF_FATAL 1
#endif
#endif

/* define macro's to 1 to enable diag messages for that level
 *
 */
#ifndef FARF_ALWAYS
#define FARF_ALWAYS        1    /* 0 turns me off */
#endif
#define FARF_ALWAYS_LEVEL  HAP_LEVEL_HIGH

#ifndef FARF_LOW
#define FARF_LOW           0    /* 0 turns me off */
#endif
#define FARF_LOW_LEVEL     HAP_LEVEL_LOW

#ifndef FARF_MEDIUM
#define FARF_MEDIUM        0    /* 0 turns me off */
#endif
#define FARF_MEDIUM_LEVEL  HAP_LEVEL_MEDIUM

#ifndef FARF_HIGH
#define FARF_HIGH          0    /* 0 turns me off */
#endif
#define FARF_HIGH_LEVEL    HAP_LEVEL_HIGH

#ifndef FARF_ERROR
#define FARF_ERROR         0    /* 0 turns me off */
#endif
#define FARF_ERROR_LEVEL   HAP_LEVEL_ERROR

#ifndef FARF_FATAL
#define FARF_FATAL         0    /* 0 turns me off */
#endif
#define FARF_FATAL_LEVEL   HAP_LEVEL_FATAL

//internal macros
#define _FARF_PASTE(a,b) _FARF_PASTE_(a,b)
#define _FARF_PASTE_(a,b) a##b
#define _FARF_VAL(a) a

//internal macro
//this one is used when farfs are not compiled in
#define _FARF_0(x, ...)

//lint -emacro(506,FARF) Constant Boolean Value
//lint -emacro(774,FARF) Boolean within always evaluates to True
//this one is used when farfs are compiled in
#define _FARF_1(x, ...) \
    do { \
        if(0 == (HAP_debug_v2)) { \
            _HAP_debug_v2(FARF_##x##_LEVEL, __FILENAME__, __LINE__, ##__VA_ARGS__); \
        } else { \
            HAP_debug_v2(FARF_##x##_LEVEL, __FILENAME__, __LINE__, ##__VA_ARGS__); \
        } \
      } while (0)

#endif /* #ifndef HAP_FARF_H */
