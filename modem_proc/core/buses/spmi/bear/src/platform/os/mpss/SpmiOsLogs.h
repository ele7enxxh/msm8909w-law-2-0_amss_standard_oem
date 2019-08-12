/**
 * @file: SpmiOsLogs.h
 * 
 * @brief: This module implements logging functionality for the SPMI driver
 * 
 * Copyright (c) 2013-2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/12/13 07:59:23 $
 * $Header: //components/rel/core.mpss/3.5.c12.3/buses/spmi/bear/src/platform/os/mpss/SpmiOsLogs.h#1 $
 * $Change: 11985146 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */

#ifndef __SPMI_OS_LOGS_H_
#define __SPMI_OS_LOGS_H_

#include "ULogFront.h"

#define SPMI_LOG_NAME "Spmi Log"
#define SPMI_LOG_SIZE 2048

extern ULogHandle spmiLogHandle;

#define SPMI_OS_LOG_INIT() ULogFront_RealTimeInit(&spmiLogHandle, \
                                                  SPMI_LOG_NAME, \
                                                  SPMI_LOG_SIZE, \
                                                  ULOG_MEMORY_LOCAL, \
                                                  ULOG_LOCK_OS)
                                                  
#define GET_COUNT_MACRO(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,COUNT,...) COUNT
#define DO_ULOG(...) ULogFront_RealTimePrintf(spmiLogHandle, GET_COUNT_MACRO(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1,0), __VA_ARGS__)
                                                  
#define SPMI_OS_LOG_FATAL(msg, args...)   DO_ULOG("SPMI Fatal: "msg, ##args)
#define SPMI_OS_LOG_ERROR(msg, args...)   DO_ULOG("SPMI Error: "msg, ##args)
#define SPMI_OS_LOG_WARNING(msg, args...) DO_ULOG("SPMI Warn: "msg, ##args)
#define SPMI_OS_LOG_INFO(msg, args...)    DO_ULOG("SPMI Info: "msg, ##args)
#define SPMI_OS_LOG_VERBOSE(msg, args...) DO_ULOG("SPMI Verbose: "msg, ##args)

#endif
