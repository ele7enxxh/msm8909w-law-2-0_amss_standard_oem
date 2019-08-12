/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef ANDROID_IQVOPLOGFILE_H
#define ANDROID_IQVOPLOGFILE_H

#include <string>

#ifdef QVOP_DEBUG
 #define ATRACE_TAG ATRACE_TAG_ALWAYS

 #include "cutils/trace.h"

 #define QVOPTRACE_BEGIN(str) ATRACE_BEGIN(str)
 #define QVOPTRACE_END() ATRACE_END()
#else  // !QVOP_DEBUG
 #define QVOPTRACE_BEGIN(str)
 #define QVOPTRACE_END()
#endif // QVOP_DEBUG

#define QVOP_FN logfile_shortfname(__func__).c_str()

extern void logfile_init(bool _printToScreen = true);
extern void logfile_print(char const* pFormat, ...);
extern std::string logfile_shortfname(char const* long_fname);
void print_mem(char const* msg, void const* buf_start, size_t len);
void get_trace(const uint32_t key, std::string &msg);

#endif // ANDROID_IQVOPLOGFILE_H
