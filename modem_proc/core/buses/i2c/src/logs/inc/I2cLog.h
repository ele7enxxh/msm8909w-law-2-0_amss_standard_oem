#ifndef _I2CLOG_H_
#define _I2CLOG_H_
/*
===========================================================================

FILE:   I2cLog.h

DESCRIPTION:
    This file contains the API for the I2C Log services 


===========================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/logs/inc/I2cLog.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
08/20/14 MS     Created from 8916 MPSS

===========================================================================
        Copyright c 2012 - 2015 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/
#include "I2cTypes.h"
#include "ULogFront.h"

#define I2C_LOG_LEVEL_MAX 			I2C_LOG_LEVEL_VERBOSE
#define I2C_LOG_LEVEL_VERBOSE 		5
#define I2C_LOG_LEVEL_INFO 			4
#define I2C_LOG_LEVEL_WARNING 		3
#define I2C_LOG_LEVEL_ERROR 		2
#define I2C_LOG_LEVEL_FATAL_ERROR 	1
#define I2C_LOG_LEVEL_NONE 			0

#define I2CDEVICE_LOG(severity, format_str, args...) I2C_LOG_##severity(format_str, ##args)


#define I2C_LOG_VERBOSE(format_str, args...) \
    if((I2cLog_GetUlogHandle() != NULL) && (I2cLog_GetLogLevel() >= I2C_LOG_LEVEL_VERBOSE ))\
    {\
        ULogFront_RealTimePrintf(I2cLog_GetUlogHandle(), 0, "VERBOSE");\
        ULogFront_RealTimePrintf(I2cLog_GetUlogHandle(), 10, format_str,##args);\
    }

#define I2C_LOG_INFO(format_str, args...) \
    if((I2cLog_GetUlogHandle() != NULL) && (I2cLog_GetLogLevel() >= I2C_LOG_LEVEL_INFO))\
    {\
        ULogFront_RealTimePrintf(I2cLog_GetUlogHandle(), 1, "Device Id =  %x",devId);\
        ULogFront_RealTimePrintf(I2cLog_GetUlogHandle(),  I2C_NUM_ARGS(##args),##args);\
    }

#define I2C_LOG_WARNING(devId, args...)\
    if((I2cLog_GetUlogHandle() != NULL) && (I2cLog_GetLogLevel() >= I2C_LOG_LEVEL_WARNING))\
    {\
        ULogFront_RealTimePrintf(I2cLog_GetUlogHandle(), 0, "WARNING");\
        ULogFront_RealTimePrintf(I2cLog_GetUlogHandle(), 10, format_str , ##args);\
    }


#define I2C_LOG_ERROR(format_str, args...) \
    if((I2cLog_GetUlogHandle() != NULL) && (I2cLog_GetLogLevel() >= I2C_LOG_LEVEL_ERROR))\
    {\
        ULogFront_RealTimePrintf(I2cLog_GetUlogHandle(),  0, "ERROR");\
        ULogFront_RealTimePrintf(I2cLog_GetUlogHandle(), 10, format_str , ##args);\
    }

#define I2C_LOG_FATAL_ERROR(format_str, args...) \
    if((I2cLog_GetUlogHandle() != NULL) && (I2cLog_GetLogLevel() >= I2C_LOG_LEVEL_FATAL_ERROR))\
    {\
        ULogFront_RealTimePrintf(I2cLog_GetUlogHandle(), 0, "FATAL_ERROR");\
        ULogFront_RealTimePrintf(I2cLog_GetUlogHandle(), 10, format_str, ##args);\
    }

void * I2cLog_GetUlogHandle(void);
uint32 I2cLog_GetLogLevel(void);
void I2cLog_SetLogLevel(uint32 uLogLevel);
void I2cLog_Init(void);


#endif /* _I2CLOG_H_ */

