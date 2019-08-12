/*============================================================================
  FILE:         HALTsens.c

  OVERVIEW:     Implementation of the TSENS HAL for 9607.

  DEPENDENCIES: None

                Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-07-17  SA   Updated for 9x07 (Lykan).
  2014-08-04  SA   Updated for 8909.
  2014-07-09  jjo  GetSensorPrevTemp update.
  2014-01-21  PR   cleaned up code for better readability and Maintainability.
  2013-11-25  PR   Ported to Bear.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "HALhwio.h"
#include "HALhwioTsens.h"
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
uint8 *gpuMpm2MpmBase;
uint8 *gpuSecCtrlBase;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Get temperature measurement
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_GetSensorPrevTemp
**
**  Description:
**    Gets the adc result from the previous output.
**
**  Parameters:
**    uSensor - which of the sensors
**    puCode  - TSENS ADC code
**
**  Return: true if reading is valid, else false
**
**  Dependencies: adc result must be generated already
**
** ========================================================================= */
boolean HAL_tsens_GetSensorPrevTemp(uint32 uSensor, uint32 *puCode)
{
   *puCode = HWIO_INFI(MPM2_TSENS_Sn_STATUS, uSensor, LAST_TEMP);
   return TRUE;
}
/* ============================================================================
**
**  HAL_tsens_TempMeasurementIsComplete
**
**  Description:
**    Finds out if the adc result of a given sensor is ready to read.
**
**  Parameters:
**    None
**
**  Return: whether temperature measurement has been completed w/ the associated
**          adc result in the register as a boolean (T/F).
**
**  Dependencies: tsens is enabled
**
** ========================================================================= */
boolean HAL_tsens_TempMeasurementIsComplete(void)
{
   uint32 uTempReady;

   uTempReady = HWIO_INF(MPM2_TSENS_TRDY, TRDY);

   if (uTempReady == 0)
   {
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}

/*----------------------------------------------------------------------------
 * Interrupts
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_EnableUpperLowerInterrupt
**
**  Description:
**    Main enable for upper / lower thresholds
**
**  Parameters:
**    None
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_EnableUpperLowerInterrupt(void)
{
   HWIO_OUTV(MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL, INTERRUPT_EN, ENABLED);
}

/* ============================================================================
**
**  HAL_tsens_DisableUpperLowerInterrupt
**
**  Description:
**    Main disable for upper / lower thresholds
**
**  Parameters:
**    None
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_DisableUpperLowerInterrupt(void)
{
   HWIO_OUTV(MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL, INTERRUPT_EN, DISABLED);
}
/* ============================================================================
**
**  HAL_tsens_EnableInterrupt
**
**  Description:
**    Enable tsens for a given threshold level
**
**  Parameters:
**    eThresholdLevel
**    uSensor
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_EnableInterrupt(HAL_tsens_ThresholdLevel eThresholdLevel, uint32 uSensor)
{
   switch(eThresholdLevel)
   {
      case HAL_TSENS_MIN_LIMIT_TH:
         HWIO_OUTVI(MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL, uSensor, MIN_STATUS_MASK, NORMAL_OPERATION);
         break;
      case HAL_TSENS_LOWER_LIMIT_TH:
         HWIO_OUTVI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, LOWER_STATUS_CLR, NORMAL_OPERATION);
         break;
      case HAL_TSENS_UPPER_LIMIT_TH:
         HWIO_OUTVI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, UPPER_STATUS_CLR, NORMAL_OPERATION);
         break;
      case HAL_TSENS_MAX_LIMIT_TH:
         HWIO_OUTVI(MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL, uSensor, MAX_STATUS_MASK, NORMAL_OPERATION);
         break;
      default:
         break;
   }
}

/* ============================================================================
**
**  HAL_tsens_DisableInterrupt
**
**  Description:
**    Disable tsens for a given threshold level
**
**  Parameters:
**    eThresholdLevel
**    uSensor
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_DisableInterrupt(HAL_tsens_ThresholdLevel eThresholdLevel, uint32 uSensor)
{
   switch(eThresholdLevel)
   {
      case HAL_TSENS_MIN_LIMIT_TH:
         HWIO_OUTVI(MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL, uSensor, MIN_STATUS_MASK, MASK_OFF_MIN_STATUS);
         break;
      case HAL_TSENS_LOWER_LIMIT_TH:
         HWIO_OUTVI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, LOWER_STATUS_CLR, CLEAR_LOWER_STATUS);
         break;
      case HAL_TSENS_UPPER_LIMIT_TH:
         HWIO_OUTVI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, UPPER_STATUS_CLR, CLEAR_UPPER_STATUS);
         break;
      case HAL_TSENS_MAX_LIMIT_TH:
         HWIO_OUTVI(MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL, uSensor, MAX_STATUS_MASK, MASK_OFF_MAX_STATUS);
         break;
      default:
         break;
   }
}

/* ============================================================================
**
**  HAL_tsens_ClearInterrupt
**
**  Description:
**    Clears tsens interrupt for a given threshold level
**
**  Parameters:
**    eThresholdLevel
**    uSensor
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_ClearInterrupt(HAL_tsens_ThresholdLevel eThresholdLevel, uint32 uSensor)
{
   uint32 uStatusClr;

   switch(eThresholdLevel)
   {
      case HAL_TSENS_MIN_LIMIT_TH:
         break;
      case HAL_TSENS_LOWER_LIMIT_TH:
         uStatusClr = HWIO_INFI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, LOWER_STATUS_CLR);
         if (uStatusClr == 1)
         {
            // Clear bit already set - do nothing
            return;
         }

         HWIO_OUTVI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, LOWER_STATUS_CLR, CLEAR_LOWER_STATUS);
         HWIO_OUTVI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, LOWER_STATUS_CLR, NORMAL_OPERATION);

         break;
      case HAL_TSENS_UPPER_LIMIT_TH:
         uStatusClr = HWIO_INFI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, UPPER_STATUS_CLR);
         if (uStatusClr == 1)
         {
            // Clear bit already set - do nothing
            return;
         }

         HWIO_OUTVI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, UPPER_STATUS_CLR, CLEAR_UPPER_STATUS);
         HWIO_OUTVI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, UPPER_STATUS_CLR, NORMAL_OPERATION);

         break;
      case HAL_TSENS_MAX_LIMIT_TH:
         break;
      default:
         break;
   }
}

/*----------------------------------------------------------------------------
 * Thresholds
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_CurrentlyBeyondThreshold
**
**  Description:
**    Finds out if a given threshold has crossed
**
**  Parameters:
**    eThresholdLevel
**    uSensor
**
**  Return: T/F
**
**  Dependencies: None
**
** ========================================================================= */
boolean HAL_tsens_CurrentlyBeyondThreshold(HAL_tsens_ThresholdLevel eThresholdLevel, uint32 uSensor)
{
   uint32 uThreshSet = 0;

   switch(eThresholdLevel)
   {
      case HAL_TSENS_MIN_LIMIT_TH:
         uThreshSet = HWIO_INFI(MPM2_TSENS_Sn_STATUS, uSensor, MIN_STATUS);
         break;
      case HAL_TSENS_LOWER_LIMIT_TH:
         uThreshSet = HWIO_INFI(MPM2_TSENS_Sn_STATUS, uSensor, LOWER_STATUS);
         break;
      case HAL_TSENS_UPPER_LIMIT_TH:
         uThreshSet = HWIO_INFI(MPM2_TSENS_Sn_STATUS, uSensor, UPPER_STATUS);
         break;
      case HAL_TSENS_MAX_LIMIT_TH:
         uThreshSet = HWIO_INFI(MPM2_TSENS_Sn_STATUS, uSensor, MAX_STATUS);
         break;
      default:
         break;
   }

   if (uThreshSet == 0)
   {
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}

/* ============================================================================
**
**  HAL_tsens_SetThreshold
**
**  Description:
**    Sets a given threshold
**
**  Parameters:
**    eThresholdLevel
**    uSensor
**    uThresholdCode - Value for that threshold
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_SetThreshold(HAL_tsens_ThresholdLevel eThresholdLevel, uint32 uSensor, uint32 uThresholdCode)
{
   switch(eThresholdLevel)
   {
      case HAL_TSENS_MIN_LIMIT_TH:
         HWIO_OUTFI(MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL, uSensor, MIN_THRESHOLD, uThresholdCode);
         break;
      case HAL_TSENS_LOWER_LIMIT_TH:
         HWIO_OUTFI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, LOWER_THRESHOLD, uThresholdCode);
         break;
      case HAL_TSENS_UPPER_LIMIT_TH:
         HWIO_OUTFI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, UPPER_THRESHOLD, uThresholdCode);
         break;
      case HAL_TSENS_MAX_LIMIT_TH:
         HWIO_OUTFI(MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL, uSensor, MAX_THRESHOLD, uThresholdCode);
         break;
      default:
         break;
   }
}

/* ============================================================================
**
**  HAL_tsens_GetThreshold
**
**  Description:
**    Sets a given threshold
**
**  Parameters:
**    eThresholdLevel
**    uSensor
**
**  Return: Value for that threshold
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetThreshold(HAL_tsens_ThresholdLevel eThresholdLevel, uint32 uSensor)
{
   uint32 uAdcCode = 0;

   switch(eThresholdLevel)
   {
      case HAL_TSENS_MIN_LIMIT_TH:
         uAdcCode = HWIO_INFI(MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL, uSensor, MIN_THRESHOLD);
         break;
      case HAL_TSENS_LOWER_LIMIT_TH:
         uAdcCode = HWIO_INFI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, LOWER_THRESHOLD);
         break;
      case HAL_TSENS_UPPER_LIMIT_TH:
         uAdcCode = HWIO_INFI(MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL, uSensor, UPPER_THRESHOLD);
         break;
      case HAL_TSENS_MAX_LIMIT_TH:
         uAdcCode = HWIO_INFI(MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL, uSensor, MAX_THRESHOLD);
         break;
      default:
         break;
   }

   return uAdcCode;
}

/*----------------------------------------------------------------------------
 * Measurement period
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_SetPeriod
**
**  Description:
**    Sets the re-sampling rate
**
**  Parameters:
**    0x00 measures temp once
**    0x01 62.5 ms
**    0xFF 63.5625 s
**    There is a 0.25 sec increment between 0x01 - 0xFF
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_SetPeriod(uint32 uTime)
{
   HWIO_OUTF(MPM2_TSENS_CTRL, MEASURE_PERIOD, uTime);
}

/* ============================================================================
**
**  HAL_tsens_GetPeriod
**
**  Description:
**    Gets the re-sampling rate
**
**  Parameters:
**    None
**
**  Return:
**    0x00 measures temp once
**    0x01 62.5 ms
**    0xFF 63.5625 s
**    There is a 0.25 sec increment between 0x01 - 0xFF
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetPeriod(void)
{
   return HWIO_INF(MPM2_TSENS_CTRL, MEASURE_PERIOD);
}

/* ============================================================================
**
**  HAL_tsens_GetLongestPeriod
**
**  Description:
**    Provides the longest measurement period
**
**  Parameters:
**    None
**
**  Return: Highest value supported
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetLongestPeriod(void)
{
   return HWIO_FMSK(MPM2_TSENS_CTRL, MEASURE_PERIOD) >> HWIO_SHFT(MPM2_TSENS_CTRL, MEASURE_PERIOD);
}

/*----------------------------------------------------------------------------
 * Sensor enable / disable
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_SetSensorsEnabled
**
**  Description:
**    Enable a set of sensors based on bitmask settings
**
**  Parameters:
**    uSensorMask - bit mask settings
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_SetSensorsEnabled(uint32 uSensorMask)
{
   uint32 uReg;

   uReg = HWIO_IN(MPM2_TSENS_CTRL);

   uReg &= ~(HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR0_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR1_EN) |
             HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR2_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR3_EN) |
             HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR4_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR5_EN) |
             HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR6_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR7_EN) |
             HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR8_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR9_EN) |
             HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR10_EN));

   uReg |= uSensorMask << HWIO_SHFT(MPM2_TSENS_CTRL, SENSOR0_EN) &
           (HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR0_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR1_EN) |
            HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR2_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR3_EN) |
            HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR4_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR5_EN) |
            HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR6_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR7_EN) |
            HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR8_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR9_EN) |
            HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR10_EN));

   HWIO_OUT(MPM2_TSENS_CTRL , uReg);
}

/* ============================================================================
**
**  HAL_tsens_GetEnabledSensors
**
**  Description:
**    Gets a bit mask of all enabled TSENS sensors.
**
**  Parameters:
**    None
**
**  Return: bit mask settings of enabled sensors;
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetEnabledSensors(void)
{
   uint32 uSensorMask;

   uSensorMask = HWIO_IN(MPM2_TSENS_CTRL);

   uSensorMask &= (HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR0_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR1_EN) |
                   HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR2_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR3_EN) |
                   HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR4_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR5_EN) |
                   HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR6_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR7_EN) |
                   HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR8_EN) | HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR9_EN) |
                   HWIO_FMSK(MPM2_TSENS_CTRL, SENSOR10_EN));

   uSensorMask = uSensorMask >> HWIO_SHFT(MPM2_TSENS_CTRL, SENSOR0_EN);

   return uSensorMask;
}

/*----------------------------------------------------------------------------
 * Main enable / disable
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_SetState
**
**  Description:
**    Enable or disable tsens and returned the previous state
**
**  Parameters:
**    eTsensState - HAL_TSENS_ENABLE/HAL_TSENS_DISABLE
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_SetState(HAL_tsens_State eTsensState)
{
   switch(eTsensState)
   {
      case HAL_TSENS_ENABLE:
         HWIO_OUTV(MPM2_TSENS_CTRL, TSENS_EN, ENABLED);
         break;
      case HAL_TSENS_DISABLE:
         HWIO_OUTV(MPM2_TSENS_CTRL, TSENS_EN, DISABLED);
         break;
      default:
         break;
   }
}

/*----------------------------------------------------------------------------
 * Software reset
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_Reset
**
**  Description:
**    Resets TSENS.
**
**  Parameters:
**    None
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_Reset(void)
{
   HWIO_OUTV(MPM2_TSENS_CTRL, TSENS_SW_RST, RESET_ASSERTED);

   HWIO_OUTV(MPM2_TSENS_CTRL, TSENS_SW_RST, RESET_DEASSERTED);
}

/*----------------------------------------------------------------------------
 * Config
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_Init
**
**  Description:
**    Initialization of tsens.
**
**  Parameters:
**    uVal
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_Init(uint32 uVal)
{
   HWIO_OUTM(MPM2_TSENS_GLOBAL_CONFIG, HWIO_RMSK(MPM2_TSENS_GLOBAL_CONFIG), uVal);
}

/* ============================================================================
**
**  HAL_tsens_ConfigSensor
**
**  Description:
**    Initialization of tsens.
**
**  Parameters:
**    uSensor
**    uVal
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_ConfigSensor(uint32 uSensor, uint32 uVal)
{
   if (uSensor == 0)
   {
      HWIO_OUTM(MPM2_TSENS_S0_MAIN_CONFIG, HWIO_RMSK(MPM2_TSENS_S0_MAIN_CONFIG), uVal);
   }
   else
   {
      HWIO_OUTMI(MPM2_TSENS_Sn_REMOTE_CONFIG, uSensor, HWIO_RMSK(MPM2_TSENS_Sn_REMOTE_CONFIG), uVal);
   }
}

/* ============================================================================
**
**  HAL_tsens_SetTurboMode
**
**  Description:
**    Enables / disabled turbo mode (10-bit ADC v 8-bit ADC).
**
**  Parameters:
**    bEnabled
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_SetTurboMode(boolean bEnabled)
{
   if (bEnabled == TRUE)
   {
      HWIO_OUTV(MPM2_TSENS_GLOBAL_CONFIG, TSENS_CTRL_TURBO_MODE_EN, ENABLED);
   }
   else
   {
      HWIO_OUTV(MPM2_TSENS_GLOBAL_CONFIG, TSENS_CTRL_TURBO_MODE_EN, DISABLED);
   }
}

/* ============================================================================
**
**  HAL_tsens_SelectADCClkSrc
**
**  Description:
**    Sets a given threshold
**
**  Parameters:
**    eADCClkSrc - internal clk or external clk.
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_SelectADCClkSrc(HAL_tsens_ADCClkSrc eADCClkSrc)
{
   switch(eADCClkSrc)
   {
      case HAL_TSENS_INTERNAL:
         HWIO_OUTV(MPM2_TSENS_CTRL, TSENS_ADC_CLK_SEL, INTERNAL_OSCILLATOR);
         break;
      case HAL_TSENS_EXTERNAL:
         HWIO_OUTV(MPM2_TSENS_CTRL, TSENS_ADC_CLK_SEL, EXTERNAL_CLOCK_SOURCE);
         break;
      default:
         break;
   }
}

/* ============================================================================
**
**  HAL_tsens_GetMaxCode
**
**  Description:
**    Provides the max ADC code
**
**  Parameters:
**    None
**
**  Return: adc result
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetMaxCode(boolean bTurboModeEnabled)
{
   if (bTurboModeEnabled == TRUE)
   {
      return 0x3FF;
   }
   else
   {
      return 0xFF;
   }
}


/*----------------------------------------------------------------------------
 * Char data
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_UseRedundant
**
**  Description:
**    Determines whether or not to use the redundant fuses
**
**  Parameters:
**    None
**
**  Return: TRUE: use redundant char data
**
**  Dependencies: None
**
** ========================================================================= */
boolean HAL_tsens_UseRedundant(void)
{
   return FALSE;
}

/* ============================================================================
**
**  HAL_tsens_CalSelect
**
**  Description:
**    Determines whether there is 1) no cal data 2) single point cal data or
**    3) two point cal data
**
**  Parameters:
**    bUseRedundant
**
**  Return: HAL_tsens_Calibration (not calibrated, one point, or two point)
**
**  Dependencies: None
**
** ========================================================================= */
HAL_tsens_Calibration HAL_tsens_CalSelect(boolean bUseRedundant)
{
   return (HAL_tsens_Calibration) HWIO_INF(QFPROM_CORR_CALIB_ROW5_LSB, TSENS_CALIB);
}

/* ============================================================================
**
**  HAL_tsens_GetBaseX1
**
**  Description:
**    Gets the Tsens base for cal point 1
**
**  Parameters:
**    bUseRedundant
**
**  Return: adc code
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetBaseX1(boolean bUseRedundant)
{
   return ((HWIO_INF(QFPROM_CORR_CALIB_ROW4_LSB, TSENS_BASE) << 2) & 0x3FC);/* since TSENS_BASE0 is 8 Bits*/
}

/* ============================================================================
**
**  HAL_tsens_GetBaseX2
**
**  Description:
**    Gets the Tsens base for cal point 2
**
**  Parameters:
**    bUseRedundant
**
**  Return: adc code
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetBaseX2(boolean bUseRedundant)
{
   return ((HWIO_INF(QFPROM_CORR_CALIB_ROW5_LSB, TSENS_BASE1) << 2) & 0x3FC); /* since TSENS_BASE1 is 8 Bits*/
}

/* ============================================================================
**
**  HAL_tsens_GetPointX1
**
**  Description:
**    Gets Tsens uSensor point 1
**
**  Parameters:
**    bUseRedundant
**    uint32 uSensor
**
**  Return: adc code
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetPointX1(boolean bUseRedundant, uint32 uSensor)
{
   if (bUseRedundant == FALSE)
   {
      switch (uSensor)
      {
         case 0:
            return ((HWIO_INF(QFPROM_CORR_CALIB_ROW4_LSB, TSENS0_POINT1) << 2) & 0xFC);/*offset is 6 bit value*/
         case 1:
            return ((HWIO_INF(QFPROM_CORR_CALIB_ROW4_LSB, TSENS1_POINT1) << 2) & 0xFC);
         case 2:
            return ((HWIO_INF(QFPROM_CORR_CALIB_ROW4_MSB, TSENS2_POINT1) << 2) & 0xFC);
         case 3:
            return ((HWIO_INF(QFPROM_CORR_CALIB_ROW4_MSB, TSENS3_POINT1) << 2) & 0xFC);
         case 4:
            return ((HWIO_INF(QFPROM_CORR_CALIB_ROW5_LSB, TSENS4_POINT1) << 2) & 0xFC);
         default:
            break;
      }
   }

   return 0; /* return 0 for sensor>TS4*/
}

/* ============================================================================
**
**  HAL_tsens_GetPointX2
**
**  Description:
**    Gets Tsens uSensor point 2
**
**  Parameters:
**    bUseRedundant
**    uint32 uSensor
**
**  Return: adc code
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetPointX2(boolean bUseRedundant, uint32 uSensor)
{
   if (bUseRedundant == FALSE)
   {
      switch (uSensor)
      {
         case 0:
            return ((HWIO_INF(QFPROM_CORR_CALIB_ROW4_LSB, TSENS0_POINT2) << 2) & 0xFC); /*offset is 6 bit value*/ 
         case 1:
            return ((HWIO_INF(QFPROM_CORR_CALIB_ROW4_LSB, TSENS1_POINT2) << 2) & 0xFC);
         case 2:
            return ((HWIO_INF(QFPROM_CORR_CALIB_ROW4_MSB, TSENS2_POINT2) << 2) & 0xFC);
         case 3:
            return ((HWIO_INF(QFPROM_CORR_CALIB_ROW4_MSB, TSENS3_POINT2) << 2) & 0xFC);
         case 4:
            return ((HWIO_INF(QFPROM_CORR_CALIB_ROW5_LSB, TSENS4_POINT2) << 2) & 0xFC);
         default:          
            break;
      }
   }

   return 0; /* return 0 for sensor>TS4 */
}
