#ifndef __TSENS_BOOT_H__
#define __TSENS_BOOT_H__
/*============================================================================
  @file TsensBoot.h

  MSM on-die temperature sensor boot API

  This file contains data structures and functions used to configure, control,
  and query temperature sensors in the boot

               Copyright (c) 2010-2012, 2014 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/* $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/hwengines/TsensBoot.h#1 $ */

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
typedef enum
{
   TSENS_SUCCESS,
   TSENS_ERROR_INVALID_CALIBRATION,
   TSENS_ERROR_TIMEOUT,
   TSENS_ERROR_NOT_INITIALIZED,
   TSENS_ERROR
} TsensResultType;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/**
   Initializes TSENS in the boot:
    - Enables critical thresholds
    - Enables all the sensors

   @return TSENS_SUCCESS, else an error.
 */
TsensResultType Tsens_Init(void);

/**
   Returns the temperature for the given sensor. The number of sensors
   can be obtained by calling Tsens_GetNumSensors.

   @param uSensor: [in] sensor index (zero-based).
   @param pnDegC: [out] sensor temperature in degrees C.

   @see Tsens_Init

   @return TSENS_SUCCESS, else an error.
 */
TsensResultType Tsens_GetTemp(uint32 uSensor, int32 *pnDegC);

/**
   Returns the number of sensors.

   @param puNumSensors: [out] number of sensors .

   @see Tsens_Init

   @return TSENS_SUCCESS, else an error.
 */
TsensResultType Tsens_GetNumSensors(uint32 *puNumSensors);

#endif
