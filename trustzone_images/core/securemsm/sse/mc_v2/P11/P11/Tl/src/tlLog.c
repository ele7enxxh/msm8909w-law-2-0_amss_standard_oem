/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "tlStd.h"

char * logPtr = NULL;
size_t logSize = 0;

void _tl_log_x(unsigned int nl, const char* tag, const char* fmt, ... )
{
#ifdef TL_LOG_GLOBAL
  va_list args = {0};
  va_start(args,fmt);
  do {
    if ((logPtr != NULL) && (logSize > 0)) {
      size_t written = 0;
      if (tag) {
        written = (size_t)snprintf(logPtr,logSize,"%s ",tag);
        if (written == (size_t)-1) {
          // failed to write, no clue
          break;
        } else if (written >= logSize) {
          logPtr += logSize;
          logSize = 0;
        } else {
          logSize -= written;
          logPtr += written;
        }
      }
      written = (size_t)vsnprintf(logPtr,logSize,fmt,args);
      if (written == (size_t)-1) {
        // failed to write, no clue
        break;
      } else if (written >= logSize) {
        logPtr += logSize;
        logSize = 0;
      } else {
        logSize -= written;
        logPtr += written;
      }
      if (nl) {
        written = (size_t)snprintf(logPtr,logSize,"\n");
        if (written == (size_t)-1) {
          // failed to write, no clue
          break;
        } else if (written >= logSize) {
          logPtr += logSize;
          logSize = 0;
        } else {
          logSize -= written;
          logPtr += written;
        }
      }
    }
  } while (0);
#endif
}
