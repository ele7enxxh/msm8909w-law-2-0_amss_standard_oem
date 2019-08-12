/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "sselog.h"
#include <stdarg.h>
#include <string.h>
#include "stringl.h"
#include <stdio.h>

char * logPtr = NULL;
size_t logSize = 0;

#define LINE_LEN  256

void _log_x(unsigned int nl, const char* tag0, const char* tag1, const char* fmt, ... )
{
#ifdef SSE_LOG_GLOBAL
  va_list args = {0};
  va_start(args,fmt);
  char * localLog = NULL;
  size_t localSize = 0;
  /* always compose the string first, then display it depending on the options */
  char line[LINE_LEN];
  localLog = line;
  localSize = sizeof(line);
  do {
    if (localSize > 0) {
      size_t written = 0;
      if (tag0 && tag1) {
        written = (size_t)snprintf(localLog,localSize,"%s%s ",tag0,tag1);
        if (written == (size_t)-1) {
          /* failed to write, no clue */
          break;
        } else if (written >= localSize) {
          localLog += localSize;
          localSize = 0;
        } else {
          localSize -= written;
          localLog += written;
        }
      }
      written = (size_t)vsnprintf(localLog,localSize,fmt,args);
      if (written == (size_t)-1) {
        /* failed to write, no clue */
        break;
      } else if (written >= localSize) {
        localLog += localSize;
        localSize = 0;
      } else {
        localSize -= written;
        localLog += written;
      }
      if (nl) {
        written = (size_t)snprintf(localLog,localSize,"\n");
        if (written == (size_t)-1) {
          /* failed to write, no clue */
          break;
        } else if (written >= localSize) {
          localLog += localSize;
          localSize = 0;
        } else {
          localSize -= written;
          localLog += written;
        }
      }
    }
    if ((logPtr != NULL) && (logSize > 0)) {
      /* we can write up to logSize bytes to the WSM log */
      size_t written = min(sizeof(line) - localSize,logSize);
      memscpy(logPtr,logSize,line,min(written,written));
      logPtr += written;
      logSize -= written;
    }
#ifdef MC_LOG_ENABLED
    {
      /* use Mobicore logging system */
      dbgS(line);
    }
#endif
  } while (0);
#endif
}

void dumpHex(const char *tag, const uint8_t *buffer, unsigned int len) {
  char output[256];
  memset(output,0,sizeof(output));
  LOG_I("dumpHex %s (%d bytes @ 0x%08X)",tag,len, (unsigned int)buffer);
  unsigned int j=0;
  for (unsigned int i=0; i<len; i++) {
    snprintf(output+j*3,4,"%02X ",buffer[i]);
    *(output+j*3+3)=0;
    if (j == 15) {
      LOG_I("%s",output);
      j=0;
    } else {
      j++;
    }
  }
  if (j>0 && j<15) {
    LOG_I("%s",output);
  }
}

#ifdef ANDROID
#include <ctype.h>
void printLog(const uint8_t* buffer, size_t len) {
  char *output=NULL;
  size_t i=0;
  size_t j=0;
  output=malloc(len * sizeof(char));
  unsigned int firstLineDisplayed = 0;
  if (output == NULL) {
    LOG_E("printLog: Out of memory?");
    return;
  }
  memset(output,0,len);
  for (i=0; (i<len) && (buffer[i] != 0) && (j < len); j++) {
    output[i] = buffer[j];
    if (output[i]=='\n') {
      output[i]=0;
      if (firstLineDisplayed == 0) {
        LOG_I("/************** Log from MC **************/");
        firstLineDisplayed = 1;
      }
      LOG_I("%s",output);
      i=0;
    } else {
      i++;
    }
  }
  if (firstLineDisplayed) {
    LOG_I("/************** End Log from MC (%d bytes) **************/",j);
  }
  if (output) {
    free(output);
    output = NULL;
  }
}

void dumpBuffer(const char *tag, const char* buffer, unsigned int len) {
  LOG_I("dumpBuffer %s (%d bytes):",tag,len);
  unsigned int i=0;
  for (i=0; i<len; i++) {
    if (isprint(buffer[i])) {
      putchar(buffer[i]);
    } else {
      putchar('-');
    }
  }
  putchar('\n');
}
#endif

