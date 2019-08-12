#ifndef ADC_SCALING_UTIL_H
#define ADC_SCALING_UTIL_H
/*============================================================================
  @file AdcScalingUtil.h
 
  Analog-to-Digital Converter utility functions.

  This file defines the logical ADC channel identifiers that are specific to
  a board design. The customer may add their own channel identifiers
  here if they wish to extend the BSP with additional ADC channels.
 
                Copyright (c) 2008-2012 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/adc/devices/common/protected/AdcScalingUtil.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DDIAdcDevice.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct
{
   int32 x;
   int32 y;
} AdcMapPtInt32toInt32Type;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
   @brief Piece-wise linear interpolation function

   This function uses linear interpolation to calculate an output value given
   an input value and a map table that maps input values to output values.

   @param paPts: Pointer to an array of points which define the
                 piece-wise linear interpolation function
   @param uTableSize: The number of points in the array
   @param nInput: The input value to the linear interpolation function
   @param pnOutput: The output of the linear interpolation function

   @return: ADC_DEVICE_RESULT_VALID on success, else an AdcDeviceResultStatusType error
*/
AdcDeviceResultStatusType 
AdcMapLinearInt32toInt32(
   const AdcMapPtInt32toInt32Type *paPts,
   uint32 uTableSize,
   int32 nInput,
   int32 *pnOutput
   );

AdcDeviceResultStatusType 
VAdcScaleTdkNTCGTherm(
   AdcDeviceResultType *pResult,
   const AdcMapPtInt32toInt32Type *paMap,
   uint32 uTableSize
   );

AdcDeviceResultStatusType 
VAdcScalePmicTherm(
    AdcDeviceResultType *pResult
    );

#endif /* #ifndef ADC_SCALING_UTIL_H */
