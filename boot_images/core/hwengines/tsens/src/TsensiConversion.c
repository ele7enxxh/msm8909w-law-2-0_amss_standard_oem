/*============================================================================
  FILE:         TsensiConversion.c

  OVERVIEW:     Implementation of the Tsens Adc Code to/from Degree C.

  DEPENDENCIES: None

                Copyright (c) 2013-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/tsens/src/TsensiConversion.c#1 $$DateTime: 2015/09/01 00:30:35 $$Author: pwbldsvc $


  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-07-09  jjo  Add support for 20 nm TSENS.
  2013-11-25  PR   Ported to Bear.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "TsensiConversion.h"
#include "HALtsens.h"

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
static uint32 uNumSensors;
static int32 nTsensY1;
static int32 nTsensY2;
static int32 anTsensX1[TSENS_MAX_NUM_SENSORS];        /* Tsens value of calibration point 1 */
static int32 anTsensX2[TSENS_MAX_NUM_SENSORS];        /* Tsens value of calibration point 2 (only used for 2 point cal) */
static int32 anTsensMFactor[TSENS_MAX_NUM_SENSORS];   /* Slope [degrees C / Tsens unit] = factor * median slope [C/code] */
static DALBOOL abTsensCalValid[TSENS_MAX_NUM_SENSORS];
static uint32 uTsensMaxAdcCode;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  Tsensi_InterpolateForY
**
**  Description:
**    Linear interpolaton: y - y1 = (m / nSlopeFactor) * (x - x1)
**
**  Parameters:
**   m
**   nSlopeFactor
**   x1
**   y1
**   x
**
**  Return: y
**
**  Dependencies: None
**
** ========================================================================= */
static int32 Tsensi_InterpolateForY(int32 m, int32 nSlopeFactor, int32 x1, int32 y1, int32 x)
{
   int32 nNum1;
   int32 nNum2;
   int32 nDen;

   /* Add den / 2 to num for rounding */
   nNum1 = m * (x - x1);
   nDen = nSlopeFactor;
   nNum2 = nDen >> 1;

   if (nNum1 < 0)
   {
      nNum2 *= -1;
   }

   return (nNum1 + nNum2) / nDen + y1;
}

/* ============================================================================
**
**  Tsensi_InterpolateForX
**
**  Description:
**    Linear interpolaton: y - y1 = (m / nSlopeFactor) * (x - x1)
**
**  Parameters:
**   m
**   nSlopeFactor
**   x1
**   y1
**   y
**
**  Return: x
**
**  Dependencies: None
**
** ========================================================================= */
static int32 Tsensi_InterpolateForX(int32 m, int32 nSlopeFactor, int32 x1, int32 y1, int32 y)
{
   int32 nNum1;
   int32 nNum2;
   int32 nDen;

   /* Add den / 2 to num for rounding */
   nNum1 = nSlopeFactor * (y - y1);
   nDen = m;
   nNum2 = nDen >> 1;

   if (nNum1 < 0)
   {
      nNum2 *= -1;
   }

   return (nNum1 + nNum2) / nDen + x1;
}

/* ============================================================================
**
**  Tsensi_InterpolateCalcSlope
**
**  Description:
**    Linear interpolaton: y - y1 = (m / nSlopeFactor) * (x - x1)
**
**  Parameters:
**   nSlopeFactor
**   x1
**   y1
**   x2
**   y2
**
**  Return: x
**
**  Dependencies: None
**
** ========================================================================= */
static int32 Tsensi_InterpolateCalcSlope(int32 nSlopeFactor, int32 x1, int32 y1, int32 x2, int32 y2)
{
   return (nSlopeFactor * (y2 - y1)) / (x2 - x1);
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  Tsensi_CalibrationInit
**
**  Description:
**
**  Reads the QFPROM Calibration for single-point Tsens calibration. During
**  manufacturing, the Tsens is read at 30 degrees C. This value is programmed
**  into QFPROM. This value is used to compensate for the offset error of the
**  Tsens ADC using standard y = mx + b equation where y is temperature in deg C and
**  x is the Tsens code. We solve for b using the value of y (30) and x
**  (Tsens value programmed into QFPROM). The slope, m, is empirically
**  determined during device characterization.
**
**  Parameters:
**    paSensors
**    uNumTsensSensors
**    nY1
**    nY2
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void Tsensi_CalibrationInit(const TsensBootSensorType *paSensors, uint32 uNumTsensSensors, int32 nY1, int32 nY2)
{
   HAL_tsens_Calibration tsensSensorCalibration = HAL_TSENS_NOT_CALIBRATED;
   HAL_tsens_Calibration tsensCalibration;
   const TsensBootSensorType *pSensor;
   DALBOOL bUseRedundant;
   uint32 uSensor;
   int32 nTsensBaseX1 = 0;
   int32 nTsensBaseX2 = 0;
   int32 nTsensPoint;
   int32 nTsensMFactor = 0;

   uNumSensors = uNumTsensSensors;
   uTsensMaxAdcCode = HAL_tsens_GetMaxCode();
   nTsensY1 = nY1;
   nTsensY2 = nY2;

   /* Check to see if the redundant area is used */
   bUseRedundant = HAL_tsens_UseRedundant();

   /* Now determine the Tsens calibration method */
   tsensCalibration = HAL_tsens_CalSelect(bUseRedundant);

   /* Get the base points as applicable */
   switch (tsensCalibration)
   {
      case HAL_TSENS_ONE_POINT:
         nTsensBaseX1 = (int32)HAL_tsens_GetBaseX1(bUseRedundant);
         break;

      case HAL_TSENS_TWO_POINT:
         nTsensBaseX1 = (int32)HAL_tsens_GetBaseX1(bUseRedundant);
         nTsensBaseX2 = (int32)HAL_tsens_GetBaseX2(bUseRedundant);
         break;

      case HAL_TSENS_20_ONE_POINT:
         nTsensBaseX1 = (int32)HAL_tsens_GetBaseX1(bUseRedundant);
         break;

      case HAL_TSENS_20_TWO_POINT:
         nTsensBaseX1 = (int32)HAL_tsens_GetBaseX1(bUseRedundant);
         nTsensBaseX2 = (int32)HAL_tsens_GetBaseX2(bUseRedundant);
         nTsensMFactor = (TSENS_FACTOR * (nY2 - nY1)) / (nTsensBaseX2 - nTsensBaseX1);
         break;

      default:
         break;
   }

   /* Get the cal data per sensor */
   for (uSensor = 0; uSensor < uNumSensors; uSensor++)
   {
      pSensor = &paSensors[uSensor];

      switch (pSensor->eCal)
      {
         case TSENS_BSP_SENSOR_CAL_NORMAL:
            /* Use the normal method */
            tsensSensorCalibration = tsensCalibration;
            break;

         case TSENS_BSP_SENSOR_CAL_IGNORE_DEVICE_CAL:
            /* Force default char data */
            tsensSensorCalibration = HAL_TSENS_NOT_CALIBRATED;
            break;

         default:
            return;
      }

      switch (tsensSensorCalibration)
      {
         case HAL_TSENS_ONE_POINT:
            nTsensPoint = (int32)HAL_tsens_GetPointX1(bUseRedundant, uSensor);
            anTsensX1[uSensor] = nTsensBaseX1 + nTsensPoint ;
            anTsensMFactor[uSensor] = pSensor->nM_default;
            abTsensCalValid[uSensor] = TRUE;
            break;

         case HAL_TSENS_TWO_POINT:
            nTsensPoint = (int32)HAL_tsens_GetPointX1(bUseRedundant, uSensor);
            anTsensX1[uSensor] = nTsensBaseX1 + nTsensPoint ;
            nTsensPoint = (int32)HAL_tsens_GetPointX2(bUseRedundant, uSensor);
            anTsensX2[uSensor] = nTsensBaseX2 + nTsensPoint ;
            anTsensMFactor[uSensor] = Tsensi_InterpolateCalcSlope(TSENS_FACTOR, anTsensX1[uSensor], nTsensY1, anTsensX2[uSensor], nTsensY2);
            abTsensCalValid[uSensor] = TRUE;
            break;

         case HAL_TSENS_20_ONE_POINT:
            nTsensPoint = (int32)HAL_tsens_GetPointX1(bUseRedundant, uSensor);
            anTsensX1[uSensor] = nTsensBaseX1 + nTsensPoint;
            anTsensMFactor[uSensor] = pSensor->nM_default;
            abTsensCalValid[uSensor] = TRUE;
            break;

         case HAL_TSENS_20_TWO_POINT:
            nTsensPoint = (int32)HAL_tsens_GetPointX1(bUseRedundant, uSensor);
            anTsensX1[uSensor] = nTsensBaseX1 + nTsensPoint;
            anTsensMFactor[uSensor] = nTsensMFactor;
            abTsensCalValid[uSensor] = TRUE;
            break;

         default:
            anTsensX1[uSensor] = pSensor->nX1_default;
            anTsensMFactor[uSensor] = pSensor->nM_default;
            abTsensCalValid[uSensor] = TRUE;
            break;
      }
   }
}

/* ============================================================================
**
**  Tsensi_ConvertToDegC
**
**  Description:
**    Take an ADC code and convert to temperature in Celsius
**
**  Parameters:
**   uSensor
**   uAdcCode
**   pnTempDegC
**
**  Return: TsensResultType
**
**  Dependencies: TsensConversionInit must be initialized
**
** ========================================================================= */
TsensResultType Tsensi_ConvertToDegC(uint32 uSensor, uint32 uAdcCode, int32 *pnTempDegC)
{
   *pnTempDegC = Tsensi_InterpolateForY(anTsensMFactor[uSensor],
                                        TSENS_FACTOR,
                                        anTsensX1[uSensor],
                                        nTsensY1,
                                        (int32)uAdcCode);

   if (abTsensCalValid[uSensor] == TRUE)
   {
      return TSENS_SUCCESS;
   }
   else
   {
      return TSENS_ERROR_INVALID_CALIBRATION;
   }
}

/* ============================================================================
**
**  Tsensi_ConvertFromDegC
**
**  Description:
**    Take temperature in degrees C and convert to a Tsens code value
**
**  Parameters:
**   uSensor
**   nTempDegC
**   puAdcCode
**
**  Return: TsensResultType
**
**  Dependencies: TsensConversionInit must be initialized
**
** ========================================================================= */
TsensResultType Tsensi_ConvertFromDegC(uint32 uSensor, int32 nTempDegC, uint32 *puAdcCode)
{
   int32 nAdcCode;

   nAdcCode = Tsensi_InterpolateForX(anTsensMFactor[uSensor],
                                     TSENS_FACTOR,
                                     anTsensX1[uSensor],
                                     nTsensY1,
                                     nTempDegC);

   if (nAdcCode < 0)
   {
      *puAdcCode = 0;
   }
   else if ((uint32)nAdcCode > uTsensMaxAdcCode)
   {
      *puAdcCode = uTsensMaxAdcCode;
   }
   else
   {
      *puAdcCode = (uint32)nAdcCode;
   }

   if (abTsensCalValid[uSensor] == TRUE)
   {
      return TSENS_SUCCESS;
   }
   else
   {
      return TSENS_ERROR_INVALID_CALIBRATION;
   }
}

