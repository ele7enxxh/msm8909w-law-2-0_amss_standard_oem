/*============================================================================
  FILE:         AdcScalingUtil.c

  OVERVIEW:  This file provides utility functions for the Analog-to-Digital
            Converter driver Board Support Package.

  DEPENDENCIES: None
 
                Copyright (c) 2008-2012 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/adc/devices/common/src/AdcScalingUtil.c#1 $$DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2012-06-11  jjo  Added BSP scaling functions.
  2008-03-16  jdt  Initial revision.
============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DalVAdc.h"
#include "AdcScalingUtil.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/*===========================================================================

  FUNCTION        AdcMapLinearInt32toInt32

  DESCRIPTION     This function uses linear interpolation to calculate an
                  output value given an input value and a map table that maps
                  input values to output values.

  DEPENDENCIES  	Requires the ADC DAL

  PARAMETERS      paPts      [in] Mapping between sensor output voltage and the
                                  actual temp (must be ascending or descending)
                  uTableSize [in] Number of entries in paPts
                  nInput     [in] Input value
                  pnOutput   [out] Result (y) interpolated using input value and table

  RETURN VALUE   	ADC_DEVICE_RESULT_INVALID if there is an error with
                  the parameters or in performing the calculation.

                  ADC_DEVICE_RESULT_VALID if successful.

  SIDE EFFECTS   	None

===========================================================================*/
AdcDeviceResultStatusType AdcMapLinearInt32toInt32(
   const AdcMapPtInt32toInt32Type *paPts,
   uint32 uTableSize,
   int32 nInput,
   int32 *pnOutput
)
{
   DALBOOL bDescending = TRUE;
   uint32 uSearchIdx = 0;

   if ((paPts == NULL) || (pnOutput == NULL))
   {
      return ADC_DEVICE_RESULT_INVALID;
   }

   /* Check if table is descending or ascending */
   if (uTableSize > 1)
   {
      if (paPts[0].x < paPts[1].x)
      {
         bDescending = FALSE;
      }
   }

   while (uSearchIdx < uTableSize)
   {
      if ( (bDescending == TRUE) && (paPts[uSearchIdx].x < nInput) )
      {
        /* table entry is less than measured value and table is descending, stop */
        break;
      }
      else if ( (bDescending == FALSE) && (paPts[uSearchIdx].x > nInput) )
      {
        /* table entry is greater than measured value and table is ascending, stop */
        break;
      }
      else
      {
        uSearchIdx++;
      }
   }

   if (uSearchIdx == 0)
   {
      *pnOutput = paPts[0].y;
   }
   else if (uSearchIdx == uTableSize)
   {
      *pnOutput = paPts[uTableSize-1].y;
   }
   else
   {
      /* result is between search_index and search_index-1 */
      /* interpolate linearly */
      *pnOutput = (
               ( (int32)
                   (
                    (paPts[uSearchIdx].y - paPts[uSearchIdx-1].y)
                     * (nInput - paPts[uSearchIdx-1].x)
                   )
                   / (paPts[uSearchIdx].x - paPts[uSearchIdx-1].x)
               )
               + paPts[uSearchIdx-1].y
             );
   }
   return ADC_DEVICE_RESULT_VALID;
}

/*===========================================================================

  FUNCTION        VAdcScaleTdkNTCGTherm

  DESCRIPTION     Scales the raw ADC result using the calibration data
                  and the Rt/R100-ratio-to-temperature table.

                  This calculation assumes that the ADC calibration readings
                  have a range of at least 2^16 between the positive to
                  negative reference measurements, otherwise it will return
                  ADC_DEVICE_RESULT_INVALID.

  DEPENDENCIES  	None

  PARAMETERS      pResult [in, out] Pointer to the ADC result data
                     ->uMicrovolts [in]
                     ->nPhysical   [out]
                  paMap        [in] Pointer to linear map table to use
                  uTableSize   [in] The number of points in the map table

  RETURN VALUE   	AdcDeviceResultStatusType

  SIDE EFFECTS   	None

===========================================================================*/
AdcDeviceResultStatusType VAdcScaleTdkNTCGTherm(
   AdcDeviceResultType *pResult,
   const AdcMapPtInt32toInt32Type *paMap,
   uint32 uTableSize
)
{
   uint32 uPercent = pResult->uPercent;
   uint32 uRt_R25;
   uint32 uNum1, uNum2, uDenom;
   AdcDeviceResultStatusType resultStatus;

   /*
   * The ratio R_t' / R_25 is used to determine the temperature per the 
   * mapping table.
   *
   * Here we calculate the ratio of R_t' / R_25, where:
   *
   * R_t' is resistance of the XO thermistor in parallel with the input 
   * impedance of the ADC.
   *
   * R_25 is the reference resistor in the voltage divider.
   *
   * V_t is the voltage of the thermistor.
   *
   * V_ref is the XO ADC reference voltage.
   *
   *                 V_ref ___
   *                          |
   *                          >
   *                    R_25  <   100 kOhms
   *                          >
   *                          |
   *                          |
   *                          |- - - V_t
   *                          |     
   *                          >     
   *                    R_t'  <     
   *                          >     
   *                          |     
   *                          |
   *                         Gnd
   *
   * Voltage divider equation
   *     V_t = V_ref * R_t' / (R_t' + R_25)
   *
   * Solving for R_t
   *     R_t' = (V_t * R_25) / (V_ref - V_t)
   *
   * Subtituting into R_t' / R_25
   *     R_t' / R_25 = V_t / (V_ref - V_t)
   *
   * The shift factors scale nRt_R25 into units of 2^-14 and the
   * extra term in the numerator provides rounding.
   *
   */
   uNum1 = uPercent << 14;
   uNum2 = (0xFFFF - uPercent) >> 1;
   uDenom = (0xFFFF - uPercent);
   
   if (uDenom == 0)
   {
      uRt_R25 = 0x7FFFFFFF;
   }
   else
   {
      uRt_R25 = (uNum1 + uNum2) / uDenom;
   }
   
   if (uRt_R25 > 0x7FFFFFFF)
   {
      uRt_R25 = 0x7FFFFFFF;
   }

   resultStatus = AdcMapLinearInt32toInt32(paMap, 
                                           uTableSize, 
                                           (int32)uRt_R25,
                                           &pResult->nPhysical);

   return resultStatus;
}

/*===========================================================================

  FUNCTION        VAdcScalePmicTherm

  DESCRIPTION     Scales the ADC result from millivolts to 0.001 degrees
                  Celsius using the PMIC thermistor conversion equation.

  DEPENDENCIES    None

  PARAMETERS      pResult [in, out] Pointer to the ADC result data
                     ->uMicrovolts [in]
                     ->nPhysical   [out]

  RETURN VALUE    AdcDeviceResultStatusType

  SIDE EFFECTS    pResult->nPhysical is modified.

===========================================================================*/
AdcDeviceResultStatusType VAdcScalePmicTherm(AdcDeviceResultType *pResult)
{
   /*
    * Divide by two to convert from microvolt reading to micro-Kelvin.
    *
    * Subtract 273160 to convert the temperature from Kelvin to
    * 0.001 degrees Celsius.
    */
   pResult->nPhysical = (pResult->uMicrovolts / 2) - 273160;

   return ADC_DEVICE_RESULT_VALID;
}

