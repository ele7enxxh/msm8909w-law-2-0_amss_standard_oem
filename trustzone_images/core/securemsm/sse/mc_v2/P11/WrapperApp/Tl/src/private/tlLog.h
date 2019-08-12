/*
 * Copyright (c) 2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary.
 */

#ifndef TL_LOG_H
#define TL_LOG_H

#include <stdlib.h>
#include "tlWrapperApi.h"

#if defined(TL_LOG_GLOBAL)
#if defined(TL_LOG_FILE) || defined(DEVLIB)
#define TL_LOG
#endif
#endif

void dumpHex(const char *tag, const uint8_t *buffer, unsigned int len);

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
          } while(1!=1)

#define _TL_LOG_x(_x_,...) \
          do \
          { \
            if ((logPtr != NULL) && (logSize > 0)) { \
              size_t written = 0; \
              written = snprintf(logPtr,logSize,"%s ",_x_); \
              if (written >= logSize) { \
                logPtr += logSize; \
                logSize = 0; \
              } else { \
                logSize -= written; \
                logPtr += written; \
              } \
              written = snprintf(logPtr,logSize,__VA_ARGS__); \
              if (written >= logSize) { \
                logPtr += logSize; \
                logSize = 0; \
              } else { \
                logSize -= written; \
                logPtr += written; \
              } \
              if (_x_[0]=='E') { \
                written = snprintf(logPtr,logSize," %s: %d",__FILE__,__LINE__); \
                if (written >= logSize) { \
                  logPtr += logSize; \
                  logSize = 0; \
                } else { \
                  logSize -= written; \
                  logPtr += written; \
                } \
              } \
              written = snprintf(logPtr,logSize,"\n"); \
              if (written >= logSize) { \
                logPtr += logSize; \
                logSize = 0; \
              } else { \
                logSize -= written; \
                logPtr += written; \
              } \
            } \
          } while(1!=1)

#endif /* __cplusplus */

#else /* TL_LOG */

#define _TL_LOG_INIT(buffer) do {} while(0)

#define _TL_LOG_x(_x_,...) do {} while(0)

//#define LOG_SIZE 4

#endif /* TL_LOG */

#define TL_LOG_INIT(buffer) _TL_LOG_INIT(buffer)
#define TL_LOG_I(...)  _TL_LOG_x("I/TL",__VA_ARGS__)
#define TL_LOG_W(...)  _TL_LOG_x("W/TL",__VA_ARGS__)
#define TL_LOG_E(...)  _TL_LOG_x("E/TL",__VA_ARGS__)

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
