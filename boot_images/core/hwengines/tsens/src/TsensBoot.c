/*============================================================================
  FILE:         TsensBoot.c

  OVERVIEW:     Implementation of TSENS in the boot

  DEPENDENCIES: None

                Copyright (c) 2012-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.


  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/tsens/src/TsensBoot.c#1 $$DateTime: 2015/09/01 00:30:35 $$Author: pwbldsvc $


  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-07-09  jjo  Add support for 20 nm TSENS.
  2013-01-15  jjo  Fixed timeout to account for all sensors being enabled.
  2012-11-14  jjo  Added critical thresholds.
  2012-09-13  jjo  Only check the ready bit on the first read.
  2012-07-30  jjo  Now using a C file for properties.
  2012-05-23  jjo  Added DAL properties.
  2012-05-20  jjo  Ported to 8974.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "TsensBoot.h"
#include "HALtsens.h"
#include "busywait.h"
#include "DALSys.h"
#include "DALStdDef.h"
#include "TsensiConversion.h"
#include "TsensBootBsp.h"

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
int32 gnTsensLastTemp = 0x7FFFFFFF;
uint32 guTsensLastSensor = 0;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static const TsensBootBspType *pTsensBootBsp = NULL;
static boolean bTsensInitialized = FALSE;   /* Flag to determine if Tsens is initialized */
static boolean bFirstReadDone = FALSE;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  Tsens_Init
**
**  Description:
**    Initializes TSENS in the boot.
**
**  Parameters:
**    None
**
**  Return:
**    TsensResultType
**
**  Dependencies:
**    None
**
** ========================================================================= */
TsensResultType Tsens_Init(void)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hTsensBootProperties);
   DALSYSPropertyVar propertyVar;
   DALResult status;
   TsensResultType result;
   const TsensBootSensorType *pSensor;
   uint32 uSensor;
   uint32 uCode;
   uint32 uSensorEnableMask;

   if (bTsensInitialized == TRUE)
   {
      return TSENS_SUCCESS;
   }

   /* Get the properties */
   status = DALSYS_GetDALPropertyHandleStr("QTsens", hTsensBootProperties);
   if (status != DAL_SUCCESS)
   {
      return TSENS_ERROR;
   }

   status = DALSYS_GetPropertyValue(hTsensBootProperties,
                                    "TSENS_BOOT_BSP",
                                    0,
                                    &propertyVar);
   if (status != DAL_SUCCESS)
   {
      return TSENS_ERROR;
   }

   pTsensBootBsp = (TsensBootBspType *)propertyVar.Val.pStruct;

   uSensorEnableMask = 0;
   for (uSensor = 0; uSensor < pTsensBootBsp->uNumSensors; uSensor++)
   {
      uSensorEnableMask |= 1 << uSensor;
   }

   /* Initialize calibration */
   Tsensi_CalibrationInit(pTsensBootBsp->paSensors,
                          pTsensBootBsp->uNumSensors,
                          pTsensBootBsp->nY1,
                          pTsensBootBsp->nY2);

   /* Configure & start Tsens */
   HAL_tsens_SetState(HAL_TSENS_DISABLE);
   HAL_tsens_Reset();
   HAL_tsens_Init(pTsensBootBsp->uGlobalConfig);

   /* Configure the sensor & set the critical thresholds */
   for (uSensor = 0; uSensor < pTsensBootBsp->uNumSensors; uSensor++)
   {
      pSensor = &pTsensBootBsp->paSensors[uSensor];

      /* Sensor configuration */
      HAL_tsens_ConfigSensor(uSensor, pSensor->uTsensConfig);

      /* Critical threshold configuration */
      if (pSensor->nCriticalMin != TSENS_THRESHOLD_DISABLED)
      {
         result = Tsensi_ConvertFromDegC(uSensor, pSensor->nCriticalMin, &uCode);
         if (result != TSENS_SUCCESS)
         {
            return result;
         }

         HAL_tsens_SetThreshold(HAL_TSENS_MIN_LIMIT_TH, uSensor, uCode);
         HAL_tsens_EnableInterrupt(HAL_TSENS_MIN_LIMIT_TH, uSensor);
      }
      else
      {
         HAL_tsens_DisableInterrupt(HAL_TSENS_MIN_LIMIT_TH, uSensor);
      }

      if (pSensor->nCriticalMax != TSENS_THRESHOLD_DISABLED)
      {
         result = Tsensi_ConvertFromDegC(uSensor, pSensor->nCriticalMax, &uCode);
         if (result != TSENS_SUCCESS)
         {
            return result;
         }

         HAL_tsens_SetThreshold(HAL_TSENS_MAX_LIMIT_TH, uSensor, uCode);
         HAL_tsens_EnableInterrupt(HAL_TSENS_MAX_LIMIT_TH, uSensor);
      }
      else
      {
         HAL_tsens_DisableInterrupt(HAL_TSENS_MAX_LIMIT_TH, uSensor);
      }
   }

   HAL_tsens_SetPeriod(pTsensBootBsp->uPeriod);
   HAL_tsens_SelectADCClkSrc(HAL_TSENS_INTERNAL);
   HAL_tsens_SetSensorsEnabled(uSensorEnableMask);
   HAL_tsens_SetState(HAL_TSENS_ENABLE);

   bTsensInitialized = TRUE;
   bFirstReadDone = FALSE;

   return TSENS_SUCCESS;
}

/* ============================================================================
**
**  Tsens_GetTemp
**
**  Description:
**    Gets the current temperature of the sensor.
**
**  Parameters:
**    uSensor [in]: sensor index
**    pnDegC [out]: sensor temperature in degrees C
**
**  Return:
**    TsensResultType
**
**  Dependencies:
**    Tsens_Init must be called once prior to calling this function.
**
** ========================================================================= */
TsensResultType Tsens_GetTemp(uint32 uSensor, int32 *pnDegC)
{
   DALBOOL bMeasComplete = FALSE;
   uint32 uTimerCnt;
   uint32 uAdcCode = 0;
   TsensResultType result;
   uint32 uWaitTime_us;
   uint32 uAdcCodeTry1;
   uint32 uAdcCodeTry2;
   uint32 uAdcCodeTry3;
   boolean bValid;

   if (pnDegC == NULL)
   {
      return TSENS_ERROR;
   }

   if (bTsensInitialized != TRUE)
   {
      return TSENS_ERROR_NOT_INITIALIZED;
   }

   if (uSensor >= pTsensBootBsp->uNumSensors)
   {
      return TSENS_ERROR;
   }

   /* Need to delay until TSENS performs the first reads */
   if (bFirstReadDone == FALSE)
   {
      uWaitTime_us = pTsensBootBsp->uSensorConvTime_us * pTsensBootBsp->uNumSensors;

      for (uTimerCnt = 0; uTimerCnt < pTsensBootBsp->uNumGetTempRetries; uTimerCnt++)
      {
         if (HAL_tsens_TempMeasurementIsComplete() == TRUE)
         {
            bMeasComplete = TRUE;
            break;
         }
         DALSYS_BusyWait(uWaitTime_us);
      }

      if (bMeasComplete == FALSE)
      {
         if (HAL_tsens_TempMeasurementIsComplete() == FALSE)
         {
            return TSENS_ERROR_TIMEOUT;
         }
      }

      bFirstReadDone = TRUE;
   }

   /* Read the ADC code */
   bValid = HAL_tsens_GetSensorPrevTemp(uSensor, &uAdcCodeTry1);
   if (bValid == TRUE)
   {
      uAdcCode = uAdcCodeTry1;
   }
   else
   {
      bValid = HAL_tsens_GetSensorPrevTemp(uSensor, &uAdcCodeTry2);
      if (bValid == TRUE)
      {
         uAdcCode = uAdcCodeTry2;
      }
      else
      {
         bValid = HAL_tsens_GetSensorPrevTemp(uSensor, &uAdcCodeTry3);
         if (bValid == TRUE)
         {
            uAdcCode = uAdcCodeTry3;
         }
         else if (uAdcCodeTry1 == uAdcCodeTry2)
         {
            uAdcCode = uAdcCodeTry1;
         }
         else if (uAdcCodeTry2 == uAdcCodeTry3)
         {
            uAdcCode = uAdcCodeTry2;
         }
         else
         {
            uAdcCode = uAdcCodeTry1;
         }
      }
   }

   /* Scale the ADC code to physical units */
   result = Tsensi_ConvertToDegC(uSensor, uAdcCode, pnDegC);

   if (result == TSENS_SUCCESS)
   {
      gnTsensLastTemp = *pnDegC;
      guTsensLastSensor = uSensor;
   }

   return result;
}

/* ============================================================================
**
**  Tsens_GetNumSensors
**
**  Description:
**    Gets the number of sensors. Note that the sensor index is zero-based, i.e.
**    uSensor = {0 to *puNumSensors - 1}
**
**  Parameters:
**    puNumSensors [out]: number of sensors
**
**  Return:
**    TsensResultType
**
**  Dependencies:
**    Tsens_Init must be called once prior to calling this function.
**
** ========================================================================= */
TsensResultType Tsens_GetNumSensors(uint32 *puNumSensors)
{
   if (puNumSensors == NULL)
   {
      return TSENS_ERROR;
   }

   if (bTsensInitialized != TRUE)
   {
      return TSENS_ERROR_NOT_INITIALIZED;
   }

   *puNumSensors = pTsensBootBsp->uNumSensors;

   return TSENS_SUCCESS;
}

