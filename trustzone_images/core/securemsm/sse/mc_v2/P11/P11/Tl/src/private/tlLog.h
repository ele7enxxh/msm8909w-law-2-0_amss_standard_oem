/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TL_LOG_H
#define TL_LOG_H

#include <stdlib.h>
#include "tlP11Api.h"

#if defined(TL_LOG_GLOBAL)
#if defined(TL_LOG_FILE) || defined(DEVLIB)
#define TL_LOG
#endif
#endif

void _tl_log_x(unsigned int nl, const char* tag, const char* fmt, ... );

#ifdef TL_LOG

#ifndef __cplusplus // building TL

#include "tlStd.h"

extern char *logPtr;
extern size_t logSize;

/** Initialise log variables.
 * Set the log pointer to the beginning of the current log buffer and set the log size.
 */
#define _TL_LOG_INIT(buffer) \
          do {\
            logPtr = (char*)(buffer); \
            logSize = LOG_SIZE; \
          } while(0)

#define _TL_LOG_x(nl,_x_,...) \
          _tl_log_x(nl,_x_,__VA_ARGS__)

#endif /* __cplusplus */

#else /* TL_LOG */

#define _TL_LOG_INIT(buffer) do {} while(0)

#define _TL_LOG_x(_x_,...) do {} while(0)

#endif /* TL_LOG */

#define TL_LOG_INIT(buffer) _TL_LOG_INIT(buffer)
#ifdef DEVLIB
extern int __android_log_print(int prio, const char *tag,  const char *fmt, ...);
#define TL_LOG_I(...)  _TL_LOG_x(1,"I/p11",__VA_ARGS__);__android_log_print(4, "hack", __VA_ARGS__)
#define TL_LOG_W(...)  _TL_LOG_x(1,"W/p11",__VA_ARGS__);__android_log_print(5, "hack", __VA_ARGS__)
#define TL_LOG_E(...)  _TL_LOG_x(0,"E/p11",__VA_ARGS__); _TL_LOG_x(1,NULL," %s: %d",__FILE__,__LINE__);__android_log_print(6, "hack", __VA_ARGS__);
#else
#define TL_LOG_I(...)  _TL_LOG_x(1,"I/p11",__VA_ARGS__);
#define TL_LOG_W(...)  _TL_LOG_x(1,"W/p11",__VA_ARGS__);
#define TL_LOG_E(...)  _TL_LOG_x(0,"E/p11",__VA_ARGS__); _TL_LOG_x(1,NULL," %s: %d",__FILE__,__LINE__);
#endif

// helpers
#define ENTER   TL_LOG_I("%s+",__FUNCTION__)
#define EXIT    TL_LOG_I("%s-",__FUNCTION__)

#define ENTERRV   \
                  ENTER; \
                  tciReturnCode_t rv = RET_OK

#define EXITRV    \
                    TL_LOG_I("%s-: %08X",__FUNCTION__,rv); \
                    return rv

#define EXITV(x)   TL_LOG_I("%s-: %08X",__FUNCTION__,(x)); return x;

#endif /* TL_LOG_H */
