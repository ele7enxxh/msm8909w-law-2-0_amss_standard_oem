#ifndef _I2CTRANSFERCFG_H_
#define _I2CTRANSFERCFG_H_
/*
===========================================================================

FILE:   I2cTransferCfg.h

DESCRIPTION:
    This file contains transfer configuration data structures.

===========================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/api/i2c/I2cTransferCfg.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
08/25/15 VG     Added changes for Lite I2c Driver
09/26/11 LK     Created

===========================================================================
        Copyright c 2015 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "I2cTypes.h"

/* This feature may be added latter. */
//typedef   enum _I2cTrCfgCmd{
//   I2cTrCfgCmd_Null             = 0x0,  /**< Don't do anything . */
//   I2cTrCfgCmd_TimeStampStart   = 0x1,  /**< Timestamps when the start is issued on the bus . */
//   I2cTrCfgCmd_TimeStampStop    = 0x2,  /**< Timestamps when the stop is issued on the bus . */
//   I2cTrCfgCmd_InsertDelayAfter = 0x4,  /**< Not implemented. */
//}  I2cTrCfgCmd ;


typedef struct I2cClientConfig
{
   uint32 uBusFreqKhz;
   /**< I2C bus frequency. */
   uint32 uByteTransferTimeoutUs;
   /**< I2C byte timeout. The transfer timeout will be calculated from this value. */
   uint32 uSlaveAddr;
   /**< Seven bit address isleft shifted and the r/w bit added by the driver.*/
} I2cClientConfig;

typedef struct I2cTransferConfig
{
   uint32      uSlaveAddr;
   /**< Deprecated member. Do not use.*/ 
   uint16      aWordVal[4];
   /**< Internal use.*/
} I2cTransferConfig;

#endif // _I2CTRANSFERCFG_H_
