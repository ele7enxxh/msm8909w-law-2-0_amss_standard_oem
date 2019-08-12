#ifndef TSENS_BOOT_BSP_H
#define TSENS_BOOT_BSP_H
/*============================================================================
  @file TsensBootBsp.h

  Tsens boot BSP file.

                Copyright (c) 2012-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/* $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/tsens/src/TsensBootBsp.h#1 $ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define TSENS_THRESHOLD_DISABLED 0x7fffffff
#define TSENS_MAX_NUM_SENSORS 16

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DALStdDef.h"

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum
{
   TSENS_BSP_SENSOR_CAL_NORMAL = 0,         /* Normal method, i.e. use QFPROM if
                                             * avail else use default char data */
   TSENS_BSP_SENSOR_CAL_IGNORE_DEVICE_CAL   /* Force using default char data */
} TsensBspSensorCalType;

typedef struct
{
   uint32 uTsensConfig;          /* Config value for the sensor */
   TsensBspSensorCalType eCal;   /* Which cal type to use */
   int32 nX1_default;            /* Default TSENS code at calibration point nY1 */
   int32 nM_default;             /* Default slope: factor * median slope [C/code] */
   int32 nCriticalMin;           /* Minimum temperature threshold for critical shutdown
                                  * or use TSENS_THRESHOLD_DISABLED to disable */
   int32 nCriticalMax;           /* Maximum temperature threshold for critical shutdown
                                  * or use TSENS_THRESHOLD_DISABLED to disable */
} TsensBootSensorType;

typedef struct
{
   const TsensBootSensorType *paSensors;
   uint32 uNumSensors;
   uint32 uPeriod;
   uint32 uSensorConvTime_us;
   uint32 uNumGetTempRetries;
   uint32 uGlobalConfig;
   int32 nY1;
   int32 nY2;
} TsensBootBspType;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

#endif /* #ifndef TSENS_BOOT_BSP_H */

