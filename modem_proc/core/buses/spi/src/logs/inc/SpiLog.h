#ifndef _SPI_LOG_H
#define _SPI_LOG_H

/*
===========================================================================

FILE:   SpiLog.h

DESCRIPTION:
    This file contains functions data structure declarations for SPI logging
    driver

===========================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/spi/src/logs/inc/SpiLog.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
01/13/14 ms     Updated common logging mechanism
11/02/11 sg     Created

===========================================================================
        Copyright c 2011 - 2014 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/
#include "SpiDeviceTypes.h"
#include "ULogFront.h"

#define SPI_LOG_LEVEL_MAX 			SPI_LOG_LEVEL_VERBOSE
#define SPI_LOG_LEVEL_VERBOSE 		5
#define SPI_LOG_LEVEL_INFO 			4
#define SPI_LOG_LEVEL_WARNING 		3
#define SPI_LOG_LEVEL_ERROR 		2
#define SPI_LOG_LEVEL_FATAL_ERROR 	1
#define SPI_LOG_LEVEL_NONE 			0


#define SPIDEVICE_LOG(severity, format_str, args...) SPI_LOG_##severity(format_str, ##args)

#define SPI_LOG_VERBOSE(format_str, args...) \
   if((SpiLog_GetUlogHandle() != NULL) && (SpiLog_GetLogLevel() >= SPI_LOG_LEVEL_VERBOSE)) \
   {\
	   ULogFront_RealTimePrintf(SpiLog_GetUlogHandle(), 0, "VERBOSE"); \
       ULogFront_RealTimePrintf(SpiLog_GetUlogHandle(), 10, format_str, ##args); \
   }

#define SPI_LOG_INFO(format_str, args...) \
	if((SpiLog_GetUlogHandle() != NULL) && (SpiLog_GetLogLevel() >= SPI_LOG_LEVEL_INFO)) \
	{\
		ULogFront_RealTimePrintf(SpiLog_GetUlogHandle(), 0, "INFO"); \
		ULogFront_RealTimePrintf(SpiLog_GetUlogHandle(), 10, format_str, ##args); \
	}

#define SPI_LOG_WARNING(format_str, args...) \
	if((SpiLog_GetUlogHandle() != NULL) && (SpiLog_GetLogLevel() >= SPI_LOG_LEVEL_WARNING)) \
	{\
		ULogFront_RealTimePrintf(SpiLog_GetUlogHandle(), 0, "WARNING"); \
		ULogFront_RealTimePrintf(SpiLog_GetUlogHandle(), 10, format_str, ##args); \
	}

#define SPI_LOG_ERROR(format_str, args...) \
	if((SpiLog_GetUlogHandle() != NULL) && (SpiLog_GetLogLevel() >= SPI_LOG_LEVEL_ERROR)) \
	{\
		ULogFront_RealTimePrintf(SpiLog_GetUlogHandle(), 0, "ERROR"); \
		ULogFront_RealTimePrintf(SpiLog_GetUlogHandle(), 10, format_str, ##args); \
	}
    
#define SPI_LOG_FATAL_ERROR(format_str, args...) \
	if((SpiLog_GetUlogHandle() != NULL) && (SpiLog_GetLogLevel() >= SPI_LOG_LEVEL_FATAL_ERROR)) \
	{\
		ULogFront_RealTimePrintf(SpiLog_GetUlogHandle(), 0, "FATAL_ERROR"); \
		ULogFront_RealTimePrintf(SpiLog_GetUlogHandle(), 10, format_str, ##args); \
	}
    
void * SpiLog_GetUlogHandle(void);
void SpiLog_Init(void);
uint32 SpiLog_GetLogLevel(void);
void SpiLog_SetLogLevel(uint32 uLogLevel);
      
#endif /* _SPI_LOG_H */

