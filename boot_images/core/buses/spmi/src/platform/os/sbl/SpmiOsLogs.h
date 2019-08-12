/**
 * @file: SpmiOsLogs.h
 * 
 * @brief: This module implements logging functionality for the SPMI driver
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/09/01 00:30:35 $
 * $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/spmi/src/platform/os/sbl/SpmiOsLogs.h#1 $
 * $Change: 8935858 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */

#ifndef __SPMI_OS_LOGS_H_
#define __SPMI_OS_LOGS_H_

#include <stdio.h>
#include "boot_logger.h"

#define SPMI_MAX_LOG_LEN 128

extern char spmiLogBuf[SPMI_MAX_LOG_LEN];

#define SPMI_OS_LOG_FATAL(msg, args...) \
    snprintf(spmiLogBuf, SPMI_MAX_LOG_LEN, "SPMI Fatal: " msg, ##args); \
    boot_log_message(spmiLogBuf)
    
#define SPMI_OS_LOG_ERROR(msg, args...) \
    snprintf(spmiLogBuf, SPMI_MAX_LOG_LEN, "SPMI Error: " msg, ##args); \
    boot_log_message(spmiLogBuf)
    
#define SPMI_OS_LOG_WARNING(msg, args...) \
    snprintf(spmiLogBuf, SPMI_MAX_LOG_LEN, "SPMI Warn: " msg, ##args); \
    boot_log_message(spmiLogBuf)
    
#define SPMI_OS_LOG_INFO(msg, args...) \
    snprintf(spmiLogBuf, SPMI_MAX_LOG_LEN, "SPMI Info: " msg, ##args); \
    boot_log_message(spmiLogBuf)
    
#define SPMI_OS_LOG_VERBOSE(msg, args...) \
    snprintf(spmiLogBuf, SPMI_MAX_LOG_LEN, "SPMI Verbose: " msg, ##args); \
    boot_log_message(spmiLogBuf)
    
#define SPMI_OS_LOG_TRACE(msg, args...) \
    snprintf(spmiLogBuf, SPMI_MAX_LOG_LEN, "SPMI Trace: " msg, ##args); \
    boot_log_message(spmiLogBuf)
    
#define SPMI_OS_LOG_TRACE_RAW(msg, args...) SPMI_OS_LOG_TRACE(msg, ##args)

#endif
