#ifndef __TSENS_BSP_H__
#define __TSENS_BSP_H__
/*============================================================================
  @file TsensBsp.h
 
  Tsens BSP file
  
                Copyright (c) 2012, 2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/tsens/dal/src/TsensBsp.h#1 $ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define TSENS_MAX_NUM_SENSORS            16

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DALStdDef.h"

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef struct
{
   uint32 uNumSensors;
   uint32 uSensorEnableMask;
   DALBOOL bTurboMode;
   uint32 uSensorConvTime_us;
   const int32 *panX1_default;
   const int32 *panM_default; 
   int32 nY1; 
   int32 nY2; 
} TsensBspType;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

#endif

