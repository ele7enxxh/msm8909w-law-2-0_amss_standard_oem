/**
 * @file: SpmiOsLogs.h
 * 
 * @brief: This module implements logging functionality for the SPMI driver
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/02/25 02:53:43 $
 * $Header: //components/rel/rpm.bf/2.1.1/core/buses/spmi/src/platform/os/rpm/SpmiOsLogs.h#1 $
 * $Change: 7538753 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 6/19/14  Abort on fatal log
 * 10/1/13  Initial Version
 */

#ifndef __SPMI_OS_LOGS_H_
#define __SPMI_OS_LOGS_H_

#include <stdbool.h>
#include "ULogFront.h"
#include "CoreVerify.h"

#define SPMI_LOG_NAME "Spmi Log"
#define SPMI_LOG_SIZE 128

extern ULogHandle spmiLogHandle;

#define SPMI_OS_LOG_INIT() ULogFront_RealTimeInit(&spmiLogHandle, \
                                                  SPMI_LOG_NAME, \
                                                  SPMI_LOG_SIZE, \
                                                  ULOG_MEMORY_LOCAL, \
                                                  ULOG_LOCK_OS)
                                                  
#define GET_COUNT_MACRO(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,COUNT,...) COUNT
#define DO_ULOG(...) ULogFront_RealTimePrintf(spmiLogHandle, GET_COUNT_MACRO(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1,0), __VA_ARGS__)
                                                  
#define SPMI_OS_LOG_FATAL(msg, args...)   do { DO_ULOG("Fatal: "msg, ##args); CORE_VERIFY(FALSE); } while(FALSE)
#define SPMI_OS_LOG_ERROR(msg, args...)   DO_ULOG("Error: "msg, ##args)
#define SPMI_OS_LOG_WARNING(msg, args...) DO_ULOG("Warn: "msg, ##args)
#define SPMI_OS_LOG_INFO(msg, args...)    DO_ULOG("Info: "msg, ##args)
#define SPMI_OS_LOG_VERBOSE(msg, args...) DO_ULOG("Verbose: "msg, ##args)

#endif
