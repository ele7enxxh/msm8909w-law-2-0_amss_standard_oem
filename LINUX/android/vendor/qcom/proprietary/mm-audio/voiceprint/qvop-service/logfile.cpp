/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <map>
#include <string>
#include <unistd.h>
#include <utils/Log.h>

#define _ftime ftime
#define _access access
#define __cdecl

static char const* accessFileName = "logfileON";
static char const* logFileName = "logfile";

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "QVOP"

using namespace std;

//trace messages that map to cmd ids
// TODO not using for now
static const pair<uint32_t, string> trace_msgs[] = {
  /*  make_pair(QVOP_CMD_GET_VERSION, "QVOP_CMD_GET_VERSION"), */

};

static const map<uint32_t, string> trace_data(trace_msgs,
    trace_msgs + sizeof trace_msgs / sizeof trace_msgs[0]);

// ========================================================

#include "logfile.h"

#include <android/log.h>


static bool g_logToFile = false;
static bool printToScreen = true;

void __cdecl logfile_init(bool _printToScreen) {

  FILE* fp = fopen(accessFileName, "r");
  g_logToFile = (fp != 0);
  if (fp != 0) {
    fclose(fp);
  }
  printToScreen = _printToScreen;

}

// pton todo add log level
void __cdecl logfile_print(char const* pFormat, ...) {

  //logfile_init(true); // TODO remove

  if (g_logToFile || printToScreen) {

    // format log message
    char buf[256];
    va_list pArg;
    va_start(pArg, pFormat);
    vsnprintf(buf, sizeof buf, pFormat, pArg);
    va_end(pArg);

    if (printToScreen) {
      ALOGD(buf);
    }

    // TODO, not logging to file for now
    if (g_logToFile) {
      /*
      FILE* logfile = fopen(logFileName, "a+");

      if (logfile != NULL) {
        struct _timeb timebuffer;
        char *timeline;
        _ftime(&timebuffer);
        timeline = ctime(&(timebuffer.time));
        if (timeline != NULL && *timeline != '\0') {
            timeline[strlen(timeline)-1] = '\0'; // Chop off trailing \n.
            fprintf(logfile, "%.19s.%03hu %s %d: ", timeline, timebuffer.millitm, &timeline[20], getpid());
        }
        fprintf(logfile, "%s", buf);
        fclose(logfile);
      }
      */
    }
  }
}

// Extracts the short function name from a __func__ string
string logfile_shortfname(char const* long_fname) {
  string name(long_fname = 0 ? "unknown" : long_fname);

  // a long function name has the form of "void my_function_name(char*, ...)"
  // or "void my_class::my_function_name(char*, ...)"

  // find end of function name
  size_t pos = name.find('(');
  if (pos != string::npos) {
    name.erase(pos);
  }

  // find begining of qualified function name
  pos = name.rfind(' ');
  if (pos != string::npos) {
    name.erase(0, pos + 1);
  }

  return name;
}

void print_mem(char const* msg, void const* buf_start, size_t len) {
  char buf[50] = {};  // arbitrary size, fits on one logcat line
  size_t remaining = sizeof buf;
  uint8_t const* data = static_cast<uint8_t const*>(buf_start);
  for (size_t i = 0; i < len && remaining > 0; ++i) {
    remaining -= snprintf(buf + sizeof buf - remaining, remaining, "%02x", data[i]);
  }

  logfile_print("%s: %s", msg, buf);
}

void get_trace(const uint32_t key, string &msg) {
  map<uint32_t, string>::const_iterator it = trace_data.find(key);
  if (it != trace_data.end()) {
    msg.append(it->second);
  } else {
    char keyStr[40];
    snprintf(keyStr, sizeof keyStr, "QVOP_CMD_%d", key);
    msg.append(keyStr);
  }
}

