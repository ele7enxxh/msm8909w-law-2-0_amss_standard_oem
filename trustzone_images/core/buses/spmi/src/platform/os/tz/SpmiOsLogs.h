/**
 * @file: SpmiOsLogs.h
 * 
 * @brief: This module implements logging functionality for the SPMI driver
 * 
 * Copyright (c) 2013-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/06/17 14:31:11 $
 * $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spmi/src/platform/os/tz/SpmiOsLogs.h#1 $
 * $Change: 10727476 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 07/10/15 Added new trace for debug
 * 10/1/13  Initial Version
 */

#ifndef __SPMI_OS_LOGS_H_
#define __SPMI_OS_LOGS_H_

#include "tzbsp_log.h"

#define SPMI_OS_LOG_FATAL(msg, args...) \
    TZBSP_LOG(TZBSP_MSG_FATAL, msg, ##args)
    
#define SPMI_OS_LOG_ERROR(msg, args...) \
    TZBSP_LOG(TZBSP_MSG_ERROR, msg, ##args)
    
#define SPMI_OS_LOG_WARNING(msg, args...) \
    TZBSP_LOG(TZBSP_MSG_HIGH, msg, ##args)

#define SPMI_OS_LOG_INFO(msg, args...) \
    TZBSP_LOG(TZBSP_MSG_MED, msg, ##args)

#define SPMI_OS_LOG_VERBOSE(msg, args...) \
    TZBSP_LOG(TZBSP_MSG_LOW, msg, ##args)
    
#define SPMI_OS_LOG_TRACE(msg, args...) \
    TZBSP_LOG(TZBSP_MSG_DEBUG, msg, ##args)
    
#define SPMI_OS_LOG_TRACE_RAW(msg, args...) SPMI_OS_LOG_TRACE(msg, ##args)

#endif
