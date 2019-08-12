/*
 * Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#ifndef SSE_LOG_H
#define SSE_LOG_H

#include <stdlib.h>
#include <stdint.h>

#if (!defined(DR_BUILD) && \
     !defined(TL_BUILD) && \
     !defined(QSAPP_BUILD)) // Android
#include <stdio.h>
#include <android/log.h>
#endif

#if defined(__cplusplus)
  #define _EXTERN_C                extern "C"
#else
  #define _EXTERN_C
#endif // defined(__cplusplus)

#include "sse_error.h"

#ifndef __cplusplus
#ifndef min
#define min(a,b) (a)>(b)?(b):(a)
#endif
#ifndef max
#define max(a,b) (a)>(b)?(a):(b)
#endif
#ifndef LENGTH
#define LENGTH(X) (sizeof(X)/sizeof((X)[0]))
#endif

#endif

#if defined(SSE_LOG_GLOBAL)
#if defined(SSE_LOG_FILE) || defined(DEVLIB)
#define SSE_LOG
#endif
#endif

#ifdef MAIN_C
#ifdef LOG_TAG
#define QUOTE(x) #x
#define EXPAND_AND_QUOTE(str) QUOTE(str)
#define SSE_LOG_TAG EXPAND_AND_QUOTE(LOG_TAG)
#endif
const char *sse_log_tag=SSE_LOG_TAG;
#else
extern char *sse_log_tag;
#endif

_EXTERN_C void _log_x(unsigned int nl, const char* tag0, const char* tag1, const char* fmt, ... );
_EXTERN_C void dumpHex(const char *tag, const uint8_t *buffer, unsigned int len);
#ifdef ANDROID
_EXTERN_C void printLog(const uint8_t* buffer, size_t len);
_EXTERN_C void dumpBuffer(const char *tag, const char *buffer, size_t len);
#endif

#ifdef SSE_LOG

#define DUMPHEX(tag, buffer, len)     dumpHex(tag, buffer, len)
#define PRINTLOG(buffer, len)         printLog(buffer, len)
#define DUMPBUFFER(tag, buffer, len)  dumpBuffer(tag, buffer, len)

#ifndef __cplusplus // building TL or DR

extern char *logPtr;
extern size_t logSize;

/** Initialise log variables.
 * Set the log pointer to the beginning of the current log buffer and set the log size.
 */
#define _TL_LOG_INIT(buffer,size) \
          do {\
            logPtr = (char*)(buffer); \
            logSize = (size); \
          } while(0)

#define _DR_LOG_INIT(buffer,size) \
          do {\
            if ((size) > 0) { \
              if (*((size_t*)(buffer)) == ((size) - sizeof(size_t))) { \
                logPtr = (char*)(buffer) + sizeof(size_t); \
                logSize = (size) - sizeof(size_t); \
              } \
            } else { \
              logSize = 0; \
            } \
          } while(0)


#define _LOG_x(nl,_t0_,_t1_,...) \
          _log_x(nl,_t0_,_t1_,__VA_ARGS__)

#endif /* __cplusplus */

#else /* SSE_LOG not defined */

#define _TL_LOG_INIT(buffer,size) do {} while(0)
#define _DR_LOG_INIT(buffer,size) do {} while(0)

#define _LOG_x(_x_,...)               do {} while(0)
#define DUMPHEX(tag, buffer, len)     do {} while(0)
#define PRINTLOG(buffer, len)         do {} while(0)
#define DUMPBUFFER(tag, buffer, len)  do {} while(0)

#endif /* SSE_LOG */

#define TL_LOG_INIT(buffer,size) _TL_LOG_INIT(buffer,size)
#define DR_LOG_INIT(buffer,size) _DR_LOG_INIT(buffer,size)

#ifdef ANDROID /* built as part of SSE in Android */
#ifdef DEVLIB /* TL build as part of the SDK */
#define LOG_V(...)  _LOG_x(1,"V/",sse_log_tag,__VA_ARGS__);__android_log_print(4, "hack", __VA_ARGS__)
#define LOG_D(...)  _LOG_x(1,"D/",sse_log_tag,__VA_ARGS__);__android_log_print(5, "hack", __VA_ARGS__)
#define LOG_I(...)  _LOG_x(1,"I/",sse_log_tag,__VA_ARGS__);__android_log_print(4, "hack", __VA_ARGS__)
#define LOG_W(...)  _LOG_x(1,"W/",sse_log_tag,__VA_ARGS__);__android_log_print(5, "hack", __VA_ARGS__)
#define LOG_E(...)  _LOG_x(0,"E/",sse_log_tag,__VA_ARGS__); _LOG_x(1,NULL,NULL," %s: %d",__FILE__,__LINE__);__android_log_print(6, "hack", __VA_ARGS__);
#else /* Normal Android libs */
#ifdef SSE_LOG
#define LOG_V(...) __android_log_print(ANDROID_LOG_VERBOSE, sse_log_tag, __VA_ARGS__)
#define LOG_D(...) __android_log_print(ANDROID_LOG_DEBUG,   sse_log_tag, __VA_ARGS__)
#define LOG_I(...) __android_log_print(ANDROID_LOG_INFO,    sse_log_tag, __VA_ARGS__)
#define LOG_W(...) __android_log_print(ANDROID_LOG_WARN,    sse_log_tag, __VA_ARGS__)
#else
#define LOG_V(...) do {} while(0)
#define LOG_D(...) do {} while(0)
#define LOG_I(...) do {} while(0)
#define LOG_W(...) do {} while(0)
#endif
#define LOG_E(fmt, args...) LOGe("%s, line %d :"fmt , __FILE__, __LINE__ , ## args)
#define LOGe(...) __android_log_print(ANDROID_LOG_ERROR, sse_log_tag, __VA_ARGS__)
#endif
#else /* built as a TL or DR, use the local function for logging */
#define LOG_V(...)  _LOG_x(1,"V/",sse_log_tag,__VA_ARGS__);
#define LOG_D(...)  _LOG_x(1,"D/",sse_log_tag,__VA_ARGS__);
#define LOG_I(...)  _LOG_x(1,"I/",sse_log_tag,__VA_ARGS__);
#define LOG_W(...)  _LOG_x(1,"W/",sse_log_tag,__VA_ARGS__);
#define LOG_E(...)  _LOG_x(0,"E/",sse_log_tag,__VA_ARGS__); _LOG_x(1,NULL,NULL," %s: %d",__FILE__,__LINE__);
#endif

// helpers
#define ENTER   LOG_D("%s+",__FUNCTION__)
#define EXIT    LOG_D("%s-",__FUNCTION__)

#define ENTERRV   \
                  ENTER; \
                  tciReturnCode_t rv = SSE_OK

#define EXITRV    \
                    LOG_D("%s-: %08X",__FUNCTION__,rv); \
                    return rv

#define EXITV(x)   LOG_D("%s-: %08X",__FUNCTION__,(x)); return x;

#endif /* SSE_LOG_H */
