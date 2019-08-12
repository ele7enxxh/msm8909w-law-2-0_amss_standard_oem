#ifndef BOOT_TEMP_CHECK_BSP_H
#define BOOT_TEMP_CHECK_BSP_H
/*============================================================================
  @file BootTempCheckBsp.h

  Boot temp check BSP file.

                Copyright (c) 2012, 2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/* $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/tsens/src/BootTempCheckBsp.h#1 $ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DALStdDef.h"

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef struct
{
   int32 nUpperThresholdDegC;
   int32 nLowerThresholdDegC;
   uint32 uBootSensorIdx;
} BootTempCheckBspType;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

#endif /* #ifndef BOOT_TEMP_CHECK_BSP_H */

