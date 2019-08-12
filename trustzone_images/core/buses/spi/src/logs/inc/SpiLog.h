#ifndef _SPI_LOG_H
#define _SPI_LOG_H

/*
===========================================================================

FILE:   SpiLog.h

DESCRIPTION:
    This file contains functions data structure declarations for SPI logging
    driver
 
          Copyright (c) 20011 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary

===========================================================================

        Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spi/src/logs/inc/SpiLog.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
07/14/12 ag     Include headers common to both EA and DAL environments.
11/02/11 sg     Created

===========================================================================
        Copyright c 2011-2013 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/
//#include "comdef.h"a
#include "SpiDeviceTypes.h"
#include <stdlib.h>

extern int32 SpiLog_Init(void);
/*-------------------------------------------------------------------------
* Include Files
* ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* ----------------------------------------------------------------------*/
#define SPIDEVICE_LOG_LEVEL_MAX SPIDEVICE_LOG_LEVEL_VERBOSE
#define SPIDEVICE_LOG_LEVEL_VERBOSE 5
#define SPIDEVICE_LOG_LEVEL_INFO 4
#define SPIDEVICE_LOG_LEVEL_WARNING 3
#define SPIDEVICE_LOG_LEVEL_ERROR 2
#define SPIDEVICE_LOG_LEVEL_FATAL_ERROR 1
#define SPIDEVICE_LOG_LEVEL_NONE 0

#define SPIDEVICE_LOG(severity, format_str, args...) 

#endif /* _SPI_LOG_H */

