/*============================================================================
  FILE:         DALVAdc.c

  OVERVIEW:     Implementation of a Physical ADC device DAL for the
                PMIC VADC peripheral.
                
  DEPENDENCIES: None
 
                Copyright (c) 2009-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/adc/devices/vadc/src/DalVAdc.c#1 $$DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-10-03  PR   support for calibration before the conversion(8916 WA for MPP2 current sink issue).
  2014-01-31  PR   Added RUMI platform check in VAdc_DeviceInit.
  2013-08-23  jjo  Dump registers if polling times out.
  2013-08-06  jjo  Name work loop.
  2013-06-27  jjo  Need to account for slave ID when registering SPMI int.
  2013-01-10  jjo  PM3.0 workaround for changing sub-type.
  2012-11-21  jjo  Fixed calibration to use matching config.
  2012-09-25  jjo  Added MPP support.
  2012-09-17  jjo  Increased number of conversion periods before timeout.
  2012-09-05  jjo  New DAL interrupt API.
  2012-08-20  jjo  Now checks for a minimum dig / ana major / minor revision.
  2012-07-31  jjo  Now checks the ADC type.
  2012-07-09  jjo  ADC conversions are now performed entirely in a workloop.
  2012-06-11  jjo  VADC support.
  2012-04-20  jdt  Support for VADC peripheral.
  2012-03-05  gps  Fixed scaling bug in ScaleResult3
  2012-02-09  gps  Added Qxdm logging
  2011-10-26  gps  Requests using sequencer now actually make request
  2010-10-19  jjo  Resolved integral promotion problem by using a cast in
                   VAdc_ScaleResult
  2010-05-25  jdt  Initial revision based on DALVAdc.c#10

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DalVAdc.h"
#include "DALDeviceId.h"
#include "VAdcDiag.h"
#include "DDIPlatformInfo.h"
#include "pm_version.h"
#include "pm_mpps.h"
#include "msg.h"

#ifdef VADC_UNIT_TEST
#include "VAdcSim.h"
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define VADC_DEFAULT_STACK_SIZE 0x2000

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

static DALResult VAdc_ReadAdcChannel(VAdcDevCtxt *pDevCtxt, uint32 uChannel, AdcDeviceResultType *pAdcDeviceResult);
static DALResult VAdc_ReadAdcWithCalibrationRatiometric(VAdcDevCtxt *pDevCtxt, const VAdcChannelConfigType *pChConfig, VAdcStatusType *convStatus, uint32 *uCode);
static DALResult VAdc_ReadAdcWithCalibrationAbsolute(VAdcDevCtxt *pDevCtxt, const VAdcChannelConfigType *pChConfig, VAdcStatusType *convStatus, uint32 *uCode);

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
VAdcReclibStatistics gVAdcAbsRecalibStatistics={0};
VAdcAbsoluteRecalibData gVAdcAbsoluteRecalibData={0};
VAdcReclibStatistics gVAdcRatioRecalibStatistics={0};

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
/* 
 * Work loop priority - this is a global so it can be easily changed
 * in the debugger 
 */ 
static uint32 gVAdcWorkloopPriority = 0;
volatile DALBOOL bIsPM8916=FALSE;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
/*======================================================================

  FUNCTION        VAdc_ReadBytes

  DESCRIPTION     This function reads from the VAdc peripheral

  DEPENDENCIES    Need to call DalDeviceOpen for SPMI first

  PARAMETERS
      pCtxt            [in]  pointer to the HAL interface context
      uOffset          [in]  the starting address
      pucData          [out] the bytes read
      uDataLen         [in]  the number of bytes to read
      puTotalBytesRead [out] the number of bytes read

  RETURN VALUE    VADC_HAL_SUCCESS or an error code

  SIDE EFFECTS    None

======================================================================*/
static VAdcHalResultType VAdc_ReadBytes(void *pCtxt, 
                                        uint32 uOffset, 
                                        unsigned char *pucData, 
                                        uint32 uDataLen, 
                                        uint32 *puTotalBytesRead)
{
   VAdcHalInterfaceCtxtType *pVAdcHalInterfaceCtxt;
   VAdcDevCtxt *pDevCtxt;
   uint32 uRegisterAddress;
#ifndef VADC_UNIT_TEST
   DALResult result;
#endif

   if (pCtxt == NULL)
   {
      return VADC_HAL_ERROR;
   }

   pVAdcHalInterfaceCtxt = (VAdcHalInterfaceCtxtType *)pCtxt;
   pDevCtxt = pVAdcHalInterfaceCtxt->pDevCtxt;

   uRegisterAddress = uOffset & 0xFF;
   uRegisterAddress |= (pDevCtxt->pBsp->uPeripheralID << 8) & 0xFF00;

#ifndef VADC_UNIT_TEST
   result = DalSpmi_ReadLong(pDevCtxt->phSpmiDev, 
                             pDevCtxt->pBsp->uSlaveId,
                             pDevCtxt->pBsp->eAccessPriority,
                             uRegisterAddress,  
                             pucData,  
                             uDataLen,  
                             puTotalBytesRead);

   if (result != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - SPMI read failed",
                    TRUE);
      return VADC_HAL_ERROR;
   }

#else
   (void)VAdcSim_ReadLong(uOffset, pucData, uDataLen, puTotalBytesRead);
#endif

   return VADC_HAL_SUCCESS;
}

/*======================================================================

  FUNCTION        VAdc_WriteBytes

  DESCRIPTION     This function writes to the VAdc peripheral

  DEPENDENCIES    Need to call DalDeviceOpen for SPMI first

  PARAMETERS
      pCtxt            [in] pointer to the HAL interface context
      uOffset          [in] the starting address
      pucData          [in] the bytes to write
      uDataLen         [in] the number of bytes to write

  RETURN VALUE    VADC_HAL_SUCCESS or an error code

  SIDE EFFECTS    None

======================================================================*/
static VAdcHalResultType VAdc_WriteBytes(void *pCtxt, 
                                         uint32 uOffset, 
                                         unsigned char* pucData, 
                                         uint32 uDataLen)
{
   VAdcHalInterfaceCtxtType *pVAdcHalInterfaceCtxt;
   VAdcDevCtxt *pDevCtxt;
   uint32 uRegisterAddress;
#ifndef VADC_UNIT_TEST
   DALResult result;
#endif

   if (pCtxt == NULL)
   {
      return VADC_HAL_ERROR;
   }

   pVAdcHalInterfaceCtxt = (VAdcHalInterfaceCtxtType *)pCtxt;
   pDevCtxt = pVAdcHalInterfaceCtxt->pDevCtxt;

   uRegisterAddress = uOffset & 0xFF;
   uRegisterAddress |= (pDevCtxt->pBsp->uPeripheralID << 8) & 0xFF00;

#ifndef VADC_UNIT_TEST
   result = DalSpmi_WriteLong(pDevCtxt->phSpmiDev, 
                              pDevCtxt->pBsp->uSlaveId,
                              pDevCtxt->pBsp->eAccessPriority,
                              uRegisterAddress,  
                              pucData,  
                              uDataLen);

   if (result != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - SPMI write failed",
                    TRUE);
      return VADC_HAL_ERROR;
   }
#else
   (void)VAdcSim_WriteLong(uOffset, pucData, uDataLen);
#endif

   return VADC_HAL_SUCCESS;
}

/*======================================================================

  FUNCTION        VAdc_ScaleResult

  DESCRIPTION     This function scales a raw ADC result into
                  a 16-bit value. It uses the calibration values
                  nVrefN and nVrefP as the min and max values of
                  the scale.

                  Raw results less than or equal to vVrefN are 
                  scaled to 0 and results greater than or equal to
                  nVrefP are scaled to 0xFFFF.
                  
           
  DEPENDENCIES  	None

  PARAMETERS
      uRawResult          [in] the raw result input to scale
      uVrefN              [in] the zero reference
      uVrefP              [in] the max scale reference
      puScaledResult      [out] the scaled output value

  RETURN VALUE    ADC_DEVICE_RESULT_VALID if successful

  SIDE EFFECTS    None

======================================================================*/
AdcDeviceResultStatusType VAdc_ScaleResult(uint32 uRawResult,
                                           uint32 uVrefN,
                                           uint32 uVrefP,
                                           uint32 *puScaledResult)
{
   uint16 uRange16, uScaledResult16;
   uint16 uVrefN16 = (uint16)uVrefN;
   uint16 uVrefP16 = (uint16)uVrefP;
   uint16 uRawResult16 = (uint16)uRawResult;
   uint16 uDenom16;
   AdcDeviceResultStatusType ret = ADC_DEVICE_RESULT_VALID;
   
   if (
       /* Checks for: |-----N-----P--x--| or |--x--N-----P-----| */
       ((uVrefP16 > uVrefN16) && ((uRawResult16 > uVrefP16) || (uRawResult16 < uVrefN16))) ||
       /* Checks for: |-----P--x--N-----| (P can be < N on certain PMICs) */
       ((uVrefP16 < uVrefN16) && (uRawResult16 > uVrefP16) && (uRawResult16 < uVrefN16))
     )
   {
      uRange16 = uVrefN16 - uVrefP16;
      if ((uint16)(uVrefN16-uRawResult16) < (uRange16 / 2))
      {
         /* underflow */
         uScaledResult16 = 0;
      }
      else
      {
         /* overflow */
         uScaledResult16 = 0xFFFF;
      }
   }
   else
   {
      uDenom16 = uVrefP16 - uVrefN16;

      if (uDenom16 > 0)
      {
   
         uScaledResult16 = ((uint32)((uRawResult16 - uVrefN16) * 0xFFFF)) / uDenom16;
      }
      else
      {
         uScaledResult16 = 0;
         ret = ADC_DEVICE_RESULT_INVALID;
      }
   }

   *puScaledResult = uScaledResult16;

   return ret;
}

/*======================================================================

  FUNCTION        VAdc_ScaleResultAbsolute

  DESCRIPTION     This function takes a reading that was scaled using
                  VAdcScaleResult1 and applies an additional 2-point
                  calibration using nVref1 and nVref2.

  DEPENDENCIES    None

  PARAMETERS
      uRawResult          [in] the raw result input to scale
      uVref1Percent       [in] scaled reading of the Vref1 AMUX channel
      uVref2Percent       [in] scaled reading of the Vref2 AMUX channel
      puResultMicrovolts  [out] the scaled output value in microvolts

  RETURN VALUE    ADC_DEVICE_RESULT_VALID if successful

  SIDE EFFECTS    None

======================================================================*/
AdcDeviceResultStatusType VAdc_ScaleResultAbsolute(uint32 uPercent,
                                                   uint32 uVref1Percent,
                                                   uint32 uVref2Percent,
                                                   uint32 uVref1_mv,
                                                   uint32 uVref2_mv,
                                                   uint32 *puResultMicrovolts)
{
   int32 m, b, nTempVal;

   if (uVref1Percent >= uVref2Percent)
   {
      /* error */
      return ADC_DEVICE_RESULT_INVALID;
   }

   m = uVref1_mv * 100000 / (uVref2Percent - uVref1Percent);
   b = uVref2_mv * 100000 - m * uVref2Percent;
   nTempVal = (m * (int32)uPercent + b) / 100;

   if (nTempVal < 0)
   {
      nTempVal = 0;
   }

   *puResultMicrovolts = nTempVal;

   return ADC_DEVICE_RESULT_VALID;
}

/*======================================================================

  FUNCTION        VAdc_ProcessConversionResult

  DESCRIPTION     This function is called at the end of conversion to process
                  the result. The raw result is converted into a scaled 16-bit
                  result and then and the scaled result is converted into
                  physical units in millivolts. Further scaling may be
                  performed by the scaling function defined in the BSP.

  DEPENDENCIES    None

  PARAMETERS
      pDevCtxt   [in]  pointer to the current device context
      uChannel   [in]  current channel index
      pChannel   [in]  current channel config
      pCalibData [in]  channel's calib data
      uCode      [in]  raw ADC code
      pResult    [out] ADC result

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
static void VAdc_ProcessConversionResult(VAdcDevCtxt *pDevCtxt,
                                         uint32 uChannel,
                                         const VAdcChannelConfigType *pChannel,
                                         const VAdcCalibDataType *pCalibData,
                                         uint32 uCode,
                                         AdcDeviceResultType *pResult)
{
   uint32 uScaled = 0;

   pResult->uCode = uCode;

   /* Compute a scaled and calibrated 16-bit value */
   pResult->eStatus = VAdc_ScaleResult(pResult->uCode, 
                                       pCalibData->uVrefN,
                                       pCalibData->uVrefP, 
                                       &uScaled);

   pResult->uChannelIdx = uChannel;
   pResult->uPercent = uScaled;

   switch(pChannel->eCalMethod)
   {
      case VADC_CAL_METHOD_RATIOMETRIC:
         /* Ratiometric: use only VrefN and VrefP */
         pResult->uMicrovolts = (uScaled * ((pDevCtxt->pBsp->uVrefP_mv * 100000) / 0xFFFF)) / 100;
         break;

      case VADC_CAL_METHOD_ABSOLUTE:
         /* Absolute: use VrefN and VrefP and Vref1 and Vref2 */
         pResult->eStatus = VAdc_ScaleResultAbsolute(pResult->uPercent,
                                                     pCalibData->uVref1Percent,
                                                     pCalibData->uVref2Percent,
                                                     pDevCtxt->pBsp->uVref1_mv,
                                                     pDevCtxt->pBsp->uVref2_mv,
                                                     &pResult->uMicrovolts);
         break;

      default:
         break;
   }

   /*
    * Since PMIC AMUX channels have a prescalar gain applied to the input,
    * we need to scale input by inverse of the channel prescalar gain to
    * obtain the actual input voltage.
    */
   if (pChannel->scalingFactor.uNumerator > 0)
   {
      pResult->uMicrovolts *= pChannel->scalingFactor.uDenominator; 
      pResult->uMicrovolts /= pChannel->scalingFactor.uNumerator;
   }
   else
   {
      pResult->eStatus = ADC_DEVICE_RESULT_INVALID;
   }

   /* By default, scale the physical result in units of millivolts */
   pResult->nPhysical = pResult->uMicrovolts / 1000;

   /* Call the BSP scaling function (if present) */
   switch (pChannel->eScalingMethod)
   {
      case VADC_SCALE_TO_MILLIVOLTS:
         /* by default, nPhysical is in millivolts */
         break;
      case VADC_SCALE_PMIC_SENSOR_TO_MILLIDEGREES:
         pResult->eStatus = VAdcScalePmicTherm(pResult);
         break;
      case VADC_SCALE_INTERPOLATE_FROM_RT_R25:
         pResult->eStatus = VAdcScaleTdkNTCGTherm(pResult,
                                                  pChannel->pInterpolationTable,
                                                  pChannel->uInterpolationTableLength);
         break;
      case VADC_SCALE_INTERPOLATE_FROM_MILLIVOLTS:
         pResult->eStatus = AdcMapLinearInt32toInt32(pChannel->pInterpolationTable,
                                                     pChannel->uInterpolationTableLength,
                                                     pResult->nPhysical,
                                                     &pResult->nPhysical);
         break;
      default:
         /* By default, nPhysical is in millivolts */
         break;
   }

   return;
}

/*======================================================================

  FUNCTION        VAdc_StartConversionInternal

  DESCRIPTION     This function starts the ADC conversion

  DEPENDENCIES    None

  PARAMETERS
      pDevCtxt          [in] pointer to the VADC device context
      pConfig           [in] the VADC configuration to use
      pChConfig         [in] which VADC mux input to use

  RETURN VALUE    DAL_SUCCESS or an error code

  SIDE EFFECTS    DAL_SUCCESS if conversion was started successfully
                  Otherwise, returns a DAL error code.

======================================================================*/
static DALResult VAdc_StartConversionInternal(VAdcDevCtxt *pDevCtxt,
                                              const VAdcConfigType *pConfig,
                                              const VAdcChannelConfigType *pChConfig)
{
   DALResult ret;
   VAdcHalResultType result;
   VAdcConversionParametersType conversionParams;
   VAdcSequencerParametersType sequencerParams;

   /*
    * Initialize conversion parameter variables.
    */
   conversionParams.uChannel = pChConfig->uAdcHardwareChannel;
   conversionParams.eSettlingDelay = pChConfig->eSettlingDelay;
   conversionParams.eClockSelect = pConfig->eClockSelect;
   conversionParams.eDecimationRatio = pConfig->eDecimationRatio;
   conversionParams.eFastAverageMode = pChConfig->eFastAverageMode;

   /*
    * Configure the conversion parameters.
    */
   result = VAdcHalSetConversionParameters(&pDevCtxt->iVAdcHalInterface, &conversionParams);
   if (result != VADC_HAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   /*
    * Check if this channel will use the conversion sequencer. 
    */
   if (pChConfig->bUseSequencer == TRUE)
   {
      sequencerParams = pDevCtxt->pBsp->paSequencerParams[pChConfig->uSequencerIdx];

      result = VAdcHalSetSequencerParameters(&pDevCtxt->iVAdcHalInterface, &sequencerParams);
      if (result != VADC_HAL_SUCCESS)
      {
         return DAL_ERROR;
      }

      result = VAdcHalSetOpMode(&pDevCtxt->iVAdcHalInterface, VADC_OP_MODE_SEQUENCER);
      if (result != VADC_HAL_SUCCESS)
      {
         return DAL_ERROR;
      }
   } 
   else
   {
      result = VAdcHalSetOpMode(&pDevCtxt->iVAdcHalInterface, VADC_OP_MODE_NORMAL);
      if (result != VADC_HAL_SUCCESS)
      {
         return DAL_ERROR;
      }
   }

   /*
    * Need to reset the EOC events and claim ownership prior to starting the conversion
    */
   /* EOC signal */
   ret = DALSYS_EventCtrl(pDevCtxt->ahEocEvent[0], DALSYS_EVENT_CTRL_RESET);
   if (ret != DAL_SUCCESS)
   {
      return ret;
   }

   /* EOC timeout */
   ret = DALSYS_EventCtrl(pDevCtxt->ahEocEvent[1], DALSYS_EVENT_CTRL_RESET);
   if (ret != DAL_SUCCESS)
   {
      return ret;
   }

   /*
    * Request an ADC conversion by asserting REQ field of the arbiter control
    * register.
    */
   result = VAdcHalRequestConversion(&pDevCtxt->iVAdcHalInterface);
   if (result != VADC_HAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   return DAL_SUCCESS;
}

/*======================================================================

  FUNCTION        VAdc_WaitForEoc

  DESCRIPTION
      This function waits for the EOC and gets the conversion status.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt    [in]
      pConfig     [in]
      pConvStatus [out]

  RETURN VALUE
      DAL_SUCCESS or an error

  SIDE EFFECTS
      Be careful not to make the wait time too small or EOC may
      time out too early.
                  
======================================================================*/
static DALResult VAdc_WaitForEoc(VAdcDevCtxt *pDevCtxt, const VAdcConfigType *pConfig, VAdcStatusType *pConvStatus)
{
   uint32 uConversionTime_us;
   uint32 uMaxWaitTime_us;
   VAdcHalResultType status;
   DALResult result;
   uint32 uEventIdx;
   uint32 uNumPeriodsWaited;

   /* Calculate the wait time */
   uConversionTime_us = pConfig->uConversionTime_us;

   if (pDevCtxt->pBsp->bUsesInterrupts == TRUE)
   {
      uMaxWaitTime_us = uConversionTime_us * pDevCtxt->pBsp->uNumPeriodsTimeout;

      /* Wait for the EOC signal */
      result = DALSYS_EventMultipleWait(pDevCtxt->ahEocEvent, 
                                        2, 
                                        uMaxWaitTime_us, 
                                        &uEventIdx);

      if (result != DAL_SUCCESS && result != DAL_ERROR_TIMEOUT)
      {
         return result;
      }
   
      if (result == DAL_ERROR_TIMEOUT)
      {
         /* Log missed interrupt */
         VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - missed interrupt",
                       FALSE);

         VAdc_LogDumpRegisters(&pDevCtxt->vAdcDebug, &pDevCtxt->iVAdcHalInterface);
      }

      /* Check for EOC */
      status = VAdcHalGetStatus(&pDevCtxt->iVAdcHalInterface, pConvStatus);
      if (status != VADC_HAL_SUCCESS)
      {
         return DAL_ERROR;
      }
   
      if (pConvStatus->eConversionStatus == VADC_CONVERSION_STATUS_COMPLETE)
      {
         VAdc_LogEOC(&pDevCtxt->vAdcDebug);

         return DAL_SUCCESS;
      }
   }
   else
   {
      for (uNumPeriodsWaited = 0; uNumPeriodsWaited < pDevCtxt->pBsp->uNumPeriodsTimeout; uNumPeriodsWaited++)
      {
         /* Wait for the EOC timeout */
         result = DALSYS_EventMultipleWait(pDevCtxt->ahEocEvent, 
                                           2, 
                                           uConversionTime_us, 
                                           &uEventIdx);
   
         if (result != DAL_SUCCESS && result != DAL_ERROR_TIMEOUT)
         {
            return result;
         }
   
         /* Check for EOC */
         status = VAdcHalGetStatus(&pDevCtxt->iVAdcHalInterface, pConvStatus);
         if (status != VADC_HAL_SUCCESS)
         {
            return DAL_ERROR;
         }
      
         if (pConvStatus->eConversionStatus == VADC_CONVERSION_STATUS_COMPLETE)
         {
            VAdc_LogEOC(&pDevCtxt->vAdcDebug);

            return DAL_SUCCESS;
         }
      }
   }

   /* Conversion did not complete */
   VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - conversion timed out",
                 FALSE);

   VAdc_LogDumpRegisters(&pDevCtxt->vAdcDebug, &pDevCtxt->iVAdcHalInterface);

   return DAL_ERROR;
}

/*======================================================================

  FUNCTION        VAdc_ReadAdcCode

  DESCRIPTION
      This function reads the ADC result.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt [in]
      puCode   [out]

  RETURN VALUE
      DAL_SUCCESS or an error

  SIDE EFFECTS
      None
                  
======================================================================*/
static DALResult VAdc_ReadAdcCode(VAdcDevCtxt *pDevCtxt, uint32 *puCode)
{
   VAdcHalResultType result;
   VAdcConversionCodeType uCode;

   /*
    * Read the data from the VADC
    */
   result = VAdcHalGetConversionCode(&pDevCtxt->iVAdcHalInterface, &uCode);
   if (result != VADC_HAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   *puCode = uCode;

   return DAL_SUCCESS;
}

/*======================================================================

  FUNCTION        VAdc_ReadAdc

  DESCRIPTION
      This function reads an ADC channel.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt    [in]
      pChConfig   [in]
      uConfigIdx  [in]
      pConvStatus [out]
      puCode      [out]

  RETURN VALUE
      DAL_SUCCESS or an error

  SIDE EFFECTS
      None
                  
======================================================================*/
static DALResult VAdc_ReadAdc(VAdcDevCtxt *pDevCtxt, const VAdcChannelConfigType *pChConfig, uint32 uConfigIdx, VAdcStatusType *pConvStatus, uint32 *puCode)
{
   DALResult result;
   const VAdcConfigType *pConfig = &pDevCtxt->pBsp->paConfigs[uConfigIdx];

   /* start the conversion */
   result = VAdc_StartConversionInternal(pDevCtxt,
                                         pConfig,
                                         pChConfig);
   if (result != DAL_SUCCESS)
   {
      return result;
   }

   /* wait for the conversion to finish */
   result = VAdc_WaitForEoc(pDevCtxt, pConfig, pConvStatus);
   if (result != DAL_SUCCESS)
   {
      return result;
   }

   /* read the ADC code */
   result = VAdc_ReadAdcCode(pDevCtxt, puCode);
   if (result != DAL_SUCCESS)
   {
      return result;
   }

#ifdef VADC_DUMP_REGISTERS_AFTER_READS
   VAdc_LogDumpRegisters(&pDevCtxt->vAdcDebug, &pDevCtxt->iVAdcHalInterface);
#endif

   return DAL_SUCCESS;
}

/*======================================================================

  FUNCTION        VAdc_CalibrateInternal

  DESCRIPTION
      This function calibrates the ADC.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt    [in]

  RETURN VALUE
      DAL_SUCCESS or an error

  SIDE EFFECTS
      None
                  
======================================================================*/
static DALResult VAdc_CalibrateInternal(VAdcDevCtxt *pDevCtxt)
{
   DALResult result;
   AdcDeviceResultStatusType eStatus;
   uint32 uConfig;
   VAdcCalibDataType *paCalibData;
   uint32 uCode;
   VAdcStatusType convStatus;
   const VAdcChannelConfigType *pChannel;
   uint32 uNumConfigs = pDevCtxt->pBsp->uNumConfigs;
   DALBOOL bIncludeAbsReferences;

   if (pDevCtxt->bCalibrated == FALSE)
   {
      /* Only calibrate ref 1 & 2 one time */
      bIncludeAbsReferences = TRUE;
   }
   else
   {
      bIncludeAbsReferences = FALSE;
   }

   for (uConfig = 0; uConfig < uNumConfigs; uConfig++)
   {
      paCalibData = &pDevCtxt->paCalibData[uConfig];
      
      /* VADC_CALIB_STATUS_MEASURING_VREFP */
      pChannel = &pDevCtxt->pBsp->paCalChannels[0];
      result = VAdc_ReadAdc(pDevCtxt, pChannel, uConfig, &convStatus, &uCode);
      if (result != DAL_SUCCESS)
      {
         goto error;
      }

      paCalibData->uVrefP = uCode;

      /* VADC_CALIB_STATUS_MEASURING_VREFN */
      pChannel = &pDevCtxt->pBsp->paCalChannels[1];
      result = VAdc_ReadAdc(pDevCtxt, pChannel, uConfig, &convStatus, &uCode);
      if (result != DAL_SUCCESS)
      {
         goto error;
      }

      paCalibData->uVrefN = uCode;

      if (bIncludeAbsReferences == TRUE)
      {
         /* VADC_CALIB_STATUS_MEASURING_VREF1 */
         pChannel = &pDevCtxt->pBsp->paCalChannels[2];
         result = VAdc_ReadAdc(pDevCtxt, pChannel, uConfig, &convStatus, &uCode);
         if (result != DAL_SUCCESS)
         {
            goto error;
         }
   
         paCalibData->uVref1 = uCode;
   
         /* VADC_CALIB_STATUS_MEASURING_VREF2 */
         pChannel = &pDevCtxt->pBsp->paCalChannels[3];
         result = VAdc_ReadAdc(pDevCtxt, pChannel, uConfig, &convStatus, &uCode);
         if (result != DAL_SUCCESS)
         {
            goto error;
         }
   
         paCalibData->uVref2 = uCode;
      }
   
      eStatus = VAdc_ScaleResult(paCalibData->uVref1, 
                                 paCalibData->uVrefN,
                                 paCalibData->uVrefP, 
                                 &paCalibData->uVref1Percent);

      if (eStatus != ADC_DEVICE_RESULT_VALID) 
      {
         result = DAL_ERROR;
         goto error;
      }
   
      eStatus = VAdc_ScaleResult(paCalibData->uVref2, 
                                 paCalibData->uVrefN,
                                 paCalibData->uVrefP, 
                                 &paCalibData->uVref2Percent);
   
      if (eStatus != ADC_DEVICE_RESULT_VALID) 
      {
         result = DAL_ERROR;
         goto error;
      }
   }

   pDevCtxt->bCalibrated = TRUE;

   return DAL_SUCCESS;

error:
   return result;
}

/*======================================================================

  FUNCTION        VAdc_ReadAdcWithCalibrationAbsolute

  DESCRIPTION
      This function does the absolute calibration before adc conversion.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt    [in]

  RETURN VALUE
      DAL_SUCCESS or an error

  SIDE EFFECTS
      None
                  
======================================================================*/
static DALResult VAdc_ReadAdcWithCalibrationAbsolute(VAdcDevCtxt *pDevCtxt, const VAdcChannelConfigType *pChConfig, VAdcStatusType *convStatus, uint32 *uCode)
{
   DALResult result;
   AdcDeviceResultStatusType eStatus;
   uint32 uConfig,uCurrenCalib=0;
   VAdcCalibDataType *paCalibData;
   uint32 uCalibCode,uPreviousCode,uAdcCodeDeviation;
   VAdcStatusType calibconvStatus;
   const VAdcChannelConfigType *pChannel;
   uConfig=pChConfig->uConfigIdx;
   gVAdcAbsoluteRecalibData.uConfig=uConfig;
   paCalibData = &pDevCtxt->paCalibData[uConfig];
   do
   {   
      /* VADC_CALIB_STATUS_MEASURING_VREF2 */
      pChannel = &pDevCtxt->pBsp->paCalChannels[3];
      result = VAdc_ReadAdc(pDevCtxt, pChannel, uConfig, &calibconvStatus, &uCalibCode);
      if (result != DAL_SUCCESS)
      {
         goto error;
      }
   
      paCalibData->uVref2 = uCalibCode;
      gVAdcAbsoluteRecalibData.sRecalibData[uCurrenCalib].uVref2CalibCode=uCalibCode;  
      uPreviousCode=uCalibCode;
  
      /* VADC_CALIB_STATUS_MEASURING_VREFP */
      pChannel = &pDevCtxt->pBsp->paCalChannels[0];
      result = VAdc_ReadAdc(pDevCtxt, pChannel, uConfig, &calibconvStatus, &uCalibCode);
      if (result != DAL_SUCCESS)
      {
         goto error;
      }

      paCalibData->uVrefP = uCalibCode;
      gVAdcAbsoluteRecalibData.sRecalibData[uCurrenCalib].uVrefPCalibCode=uCalibCode;

      /* VADC_CALIB_STATUS_MEASURING_VREFN */
      pChannel = &pDevCtxt->pBsp->paCalChannels[1];
      result = VAdc_ReadAdc(pDevCtxt, pChannel, uConfig, &calibconvStatus, &uCalibCode);
      if (result != DAL_SUCCESS)
      {
         goto error;
      }

      paCalibData->uVrefN = uCalibCode;
      gVAdcAbsoluteRecalibData.sRecalibData[uCurrenCalib].uVrefNCalibCode=uCalibCode;

      /* VADC_CALIB_STATUS_MEASURING_VREF1 */
      pChannel = &pDevCtxt->pBsp->paCalChannels[2];
      result = VAdc_ReadAdc(pDevCtxt, pChannel, uConfig, &calibconvStatus, &uCalibCode);
      if (result != DAL_SUCCESS)
      {
         goto error;
      }
   
      paCalibData->uVref1 = uCalibCode;
      gVAdcAbsoluteRecalibData.sRecalibData[uCurrenCalib].uVref1CalibCode=uCalibCode;
   
   
   
     /* Read the ADC */
     result = VAdc_ReadAdc(pDevCtxt, 
                         pChConfig, 
                         pChConfig->uConfigIdx,
                         convStatus, 
                         uCode);

     if (result != DAL_SUCCESS)
     {
        goto error;
     }   

      /* VADC_CALIB_STATUS_MEASURING_VREF2 */
      pChannel = &pDevCtxt->pBsp->paCalChannels[3];
      result = VAdc_ReadAdc(pDevCtxt, pChannel, uConfig, &calibconvStatus, &uCalibCode);
      if (result != DAL_SUCCESS)
      {
         goto error;
      }
      gVAdcAbsoluteRecalibData.sRecalibData[uCurrenCalib].uVref2CalibCode_2=uCalibCode;

      if(uPreviousCode>uCalibCode)
      uAdcCodeDeviation=uPreviousCode-uCalibCode;
      else
      uAdcCodeDeviation=uCalibCode-uPreviousCode; 
      
      uCurrenCalib++;
      MSG_8(MSG_SSID_ADC,  MSG_LEGACY_HIGH, 
		  "VADC Abs:  VrefP: %u, VrefN: %u, Vref1: %u, Vref2: %u, CalibCode: %u, currentCalib: %u, chnNum: %u, chnCode: %u", paCalibData->uVrefP, paCalibData->uVrefN, paCalibData->uVref1, paCalibData->uVref2, uCalibCode,  uCurrenCalib, pChConfig->uAdcHardwareChannel, *uCode);
   }while((uCurrenCalib<pChConfig->uMaxRecalibrations)&&(uAdcCodeDeviation > VADC_MAX_ABS_ADCCODES) );
   
   if(uCurrenCalib<pChConfig->uMaxRecalibrations)
   {
      gVAdcAbsRecalibStatistics.sVAdcRecalib[gVAdcAbsRecalibStatistics.uIndex].bIsTimedout=FALSE;
   }
   else
   {
      gVAdcAbsRecalibStatistics.sVAdcRecalib[gVAdcAbsRecalibStatistics.uIndex].bIsTimedout=TRUE;
   }

   gVAdcAbsRecalibStatistics.sVAdcRecalib[gVAdcAbsRecalibStatistics.uIndex].dwRecalibCount=uCurrenCalib;   
   gVAdcAbsRecalibStatistics.uIndex++;
   if(gVAdcAbsRecalibStatistics.uIndex==VADC_MAX_STATISTICS)
      gVAdcAbsRecalibStatistics.uIndex=0;
   

   gVAdcAbsoluteRecalibData.uIndex=uCurrenCalib;
   
      eStatus = VAdc_ScaleResult(paCalibData->uVref1, 
                                 paCalibData->uVrefN,
                                 paCalibData->uVrefP, 
                                 &paCalibData->uVref1Percent);

      if (eStatus != ADC_DEVICE_RESULT_VALID) 
      {
         result = DAL_ERROR;
         goto error;
      }
   
      eStatus = VAdc_ScaleResult(paCalibData->uVref2, 
                                 paCalibData->uVrefN,
                                 paCalibData->uVrefP, 
                                 &paCalibData->uVref2Percent);
   
      if (eStatus != ADC_DEVICE_RESULT_VALID) 
      {
         result = DAL_ERROR;
         goto error;
      }

   pDevCtxt->bCalibrated = FALSE; /* This is to force the recalibration once after MPPs back as non current sinks */
   return DAL_SUCCESS;

error:
   return result;
}
/*======================================================================

  FUNCTION        VAdc_ReadAdcWithCalibrationRatiometric

  DESCRIPTION
      This function does the absolute calibration before adc conversion.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt    [in]

  RETURN VALUE
      DAL_SUCCESS or an error

  SIDE EFFECTS
      None
                  
======================================================================*/
static DALResult VAdc_ReadAdcWithCalibrationRatiometric(VAdcDevCtxt *pDevCtxt, const VAdcChannelConfigType *pChConfig, VAdcStatusType *convStatus, uint32 *uCode)
{
   DALResult result;
   AdcDeviceResultStatusType eStatus;
   uint32 uConfig,uCurrenCalib=0;
   VAdcCalibDataType *paCalibData;
   uint32 uCalibCode,uPreviousCode,uAdcCodeDeviation;
   VAdcStatusType calibconvStatus;
   const VAdcChannelConfigType *pChannel;
   DALBOOL bIncludeAbsReferences;

   uConfig=pChConfig->uConfigIdx;
   paCalibData = &pDevCtxt->paCalibData[uConfig];

   bIncludeAbsReferences = TRUE;

   do
   {   
      /* VADC_CALIB_STATUS_MEASURING_VREFP */
      pChannel = &pDevCtxt->pBsp->paCalChannels[0];
      result = VAdc_ReadAdc(pDevCtxt, pChannel, uConfig, &calibconvStatus, &uCalibCode);
      if (result != DAL_SUCCESS)
      {
         goto error;
      }

      paCalibData->uVrefP = uCalibCode;
      uPreviousCode=uCalibCode;

      /* VADC_CALIB_STATUS_MEASURING_VREFN */
      pChannel = &pDevCtxt->pBsp->paCalChannels[1];
      result = VAdc_ReadAdc(pDevCtxt, pChannel, uConfig, &calibconvStatus, &uCalibCode);
      if (result != DAL_SUCCESS)
      {
         goto error;
      }

      paCalibData->uVrefN = uCalibCode;
      if (bIncludeAbsReferences == TRUE)
      {
         /* VADC_CALIB_STATUS_MEASURING_VREF1 */
         pChannel = &pDevCtxt->pBsp->paCalChannels[2];
         result = VAdc_ReadAdc(pDevCtxt, pChannel, uConfig, &calibconvStatus, &uCalibCode);
         if (result != DAL_SUCCESS)
         {
           goto error;
         }
   
         paCalibData->uVref1 = uCalibCode;

         /* VADC_CALIB_STATUS_MEASURING_VREF2 */
         pChannel = &pDevCtxt->pBsp->paCalChannels[3];
         result = VAdc_ReadAdc(pDevCtxt, pChannel, uConfig, &calibconvStatus, &uCalibCode);
         if (result != DAL_SUCCESS)
         {
            goto error;
         }
   
         paCalibData->uVref2 = uCalibCode;
      }
     /* Read the ADC */
     result = VAdc_ReadAdc(pDevCtxt, 
                         pChConfig, 
                         pChConfig->uConfigIdx,
                         convStatus, 
                         uCode);

     if (result != DAL_SUCCESS)
     {
        goto error;
     }   

      /* VADC_CALIB_STATUS_MEASURING_VREFP */
      pChannel = &pDevCtxt->pBsp->paCalChannels[0];
      result = VAdc_ReadAdc(pDevCtxt, pChannel, uConfig, &calibconvStatus, &uCalibCode);
      if (result != DAL_SUCCESS)
      {
         goto error;
      }
      if(uPreviousCode>uCalibCode)
      uAdcCodeDeviation=uPreviousCode-uCalibCode;
      else
      uAdcCodeDeviation=uCalibCode-uPreviousCode; 
      
      uCurrenCalib++;
      MSG_7(MSG_SSID_ADC,  MSG_LEGACY_HIGH, 
		  "VADC Rat:  VrefP: %u, VrefN: %u, Vref1: %u, Vref2: %u, CalibCode: %u, currentCalib: %u, chnCode: %u", paCalibData->uVrefP, paCalibData->uVrefN, paCalibData->uVref1, paCalibData->uVref2, uCalibCode,  uCurrenCalib, *uCode);
   }while((uCurrenCalib<pChConfig->uMaxRecalibrations)&&(uAdcCodeDeviation > VADC_MAX_RM_ADCCODES));

   if(uCurrenCalib<pChConfig->uMaxRecalibrations)
   {
      gVAdcRatioRecalibStatistics.sVAdcRecalib[gVAdcRatioRecalibStatistics.uIndex].bIsTimedout=FALSE;
   }
   else
   {
      gVAdcRatioRecalibStatistics.sVAdcRecalib[gVAdcRatioRecalibStatistics.uIndex].bIsTimedout=TRUE;
   }
   gVAdcRatioRecalibStatistics.sVAdcRecalib[gVAdcRatioRecalibStatistics.uIndex].dwRecalibCount=uCurrenCalib;  

   gVAdcRatioRecalibStatistics.uIndex++;
   if(gVAdcRatioRecalibStatistics.uIndex==VADC_MAX_STATISTICS)
      gVAdcRatioRecalibStatistics.uIndex=0;
   
	  eStatus = VAdc_ScaleResult(paCalibData->uVref1, 
                                 paCalibData->uVrefN,
                                 paCalibData->uVrefP, 
                                 &paCalibData->uVref1Percent);

      if (eStatus != ADC_DEVICE_RESULT_VALID) 
      {
         result = DAL_ERROR;
         goto error;
      }
   
      eStatus = VAdc_ScaleResult(paCalibData->uVref2, 
                                 paCalibData->uVrefN,
                                 paCalibData->uVrefP, 
                                 &paCalibData->uVref2Percent);
   
      if (eStatus != ADC_DEVICE_RESULT_VALID) 
      {
         result = DAL_ERROR;
         goto error;
      }
   pDevCtxt->bCalibrated = FALSE;  /* This is to force the recalibration once after MPPs back as non current sinks */
   return DAL_SUCCESS;

error:
   return result;
}
/*======================================================================

  FUNCTION        VAdc_ReadAdcChannel

  DESCRIPTION
      This function reads an ADC channel and returns the scaled result
      and status of the read. It is not meant for reading calibration
      channels.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt         [in]
      uChannel         [in]
      pAdcDeviceResult [out]

  RETURN VALUE
      DAL_SUCCESS or an error

  SIDE EFFECTS
      None
                  
======================================================================*/
static DALResult VAdc_ReadAdcChannel(VAdcDevCtxt *pDevCtxt, uint32 uChannel, AdcDeviceResultType *pAdcDeviceResult)
{
   DALResult result;
   VAdcStatusType convStatus;
   uint32 uCode;
   const VAdcChannelConfigType *pChConfig = &pDevCtxt->pBsp->paChannels[uChannel];
   uint32 uConfigIdx = pChConfig->uConfigIdx;
   const VAdcCalibDataType *pCalibData = &pDevCtxt->paCalibData[uConfigIdx];
   VAdcDebugCalDataType debugCalData;
   DALBOOL bRecalibrationRequired=TRUE;

   pDevCtxt->uCurrentChannelIdx = uChannel;

   if (bIsPM8916)
   {
      bRecalibrationRequired=TRUE;
   }
   else
   {
      bRecalibrationRequired = FALSE;
   }

   if((pChConfig->bRecalibrate == FALSE) || ((pChConfig->bRecalibrate == TRUE)&&(bRecalibrationRequired==FALSE)))
   {
   /* Make sure the VAdc has been calibrated */
   if (pDevCtxt->bCalibrated == FALSE)
   {
      result = VAdc_CalibrateInternal(pDevCtxt);
      if (result != DAL_SUCCESS)
      {
         goto error;
      }
   }

   /* Read the ADC */
   result = VAdc_ReadAdc(pDevCtxt, 
                         pChConfig, 
                         pChConfig->uConfigIdx,
                         &convStatus, 
                         &uCode);

   if (result != DAL_SUCCESS)
   {
      goto error;
   }
   }
   else
   {
      MSG(MSG_SSID_ADC, 
         MSG_LEGACY_HIGH, 
         "VADC: Recalibrating");
      if(pChConfig->eCalMethod == VADC_CAL_METHOD_ABSOLUTE)
      {
         result = VAdc_ReadAdcWithCalibrationAbsolute(pDevCtxt, pChConfig, &convStatus, &uCode);
         if (result != DAL_SUCCESS)
         {
            goto error;
         }
      }
      else //if(pChConfig->eCalMethod == VADC_CAL_METHOD_RATIOMETRIC)
      {
         result = VAdc_ReadAdcWithCalibrationRatiometric(pDevCtxt, pChConfig, &convStatus, &uCode);
         if (result != DAL_SUCCESS)
         {
            goto error;
         }
      }
   }

   /* Scale the result */
   VAdc_ProcessConversionResult(pDevCtxt,
                                uChannel,
                                pChConfig,
                                pCalibData,
                                uCode,
                                pAdcDeviceResult);

   if (pChConfig->bUseSequencer == TRUE && pAdcDeviceResult->eStatus == ADC_DEVICE_RESULT_VALID)
   {
      if (convStatus.bSequencerTimeoutErrorOccurred == TRUE)
      {
         pAdcDeviceResult->eStatus = ADC_DEVICE_RESULT_TIMEOUT;
      }
      else if (convStatus.bSequencerFifoNotEmptyErrorOccurred == TRUE)
      {
         pAdcDeviceResult->eStatus = ADC_DEVICE_RESULT_FIFO_NOT_EMPTY;
      }
   }

   debugCalData.uVrefP = pCalibData->uVrefP;
   debugCalData.uVrefN = pCalibData->uVrefN;
   debugCalData.uVref1 = pCalibData->uVref1;
   debugCalData.uVref2 = pCalibData->uVref2;
   debugCalData.uVref1Percent = pCalibData->uVref1Percent;
   debugCalData.uVref2Percent = pCalibData->uVref2Percent;

   VAdc_LogConversionResult(&pDevCtxt->vAdcDebug, pAdcDeviceResult, &debugCalData);

   return DAL_SUCCESS;

error:
   pAdcDeviceResult->eStatus = ADC_DEVICE_RESULT_INVALID;

   return DAL_SUCCESS;
}

/*======================================================================

  FUNCTION        VAdc_PerformOperationInWorkLoop

  DESCRIPTION
      This function performs the ADC conversion / calibration in the
      context of a workloop.

  DEPENDENCIES    None

  PARAMETERS
      hEvent     [in]
      pInDevCtxt [in]

  RETURN VALUE    DAL_SUCCESS or an error

  SIDE EFFECTS    Completes the client notification
                  
======================================================================*/
static DALResult VAdc_PerformOperationInWorkLoop(DALSYSEventHandle hEvent, void *pInDevCtxt)
{
   VAdcDevCtxt *pDevCtxt = (VAdcDevCtxt *)pInDevCtxt;
   AdcDeviceResultType adcDeviceResult;
   VAdcRequestModeType eCurrentRequestMode;
   uint32 uCurrentChannelIdx;
   DALResult result;
   void *pPayload;
   uint32 uPayloadSize;
   DALSYSEventHandle hClientEvent;
   uint32 uEventParam;
   const VAdcConfigType *pConfig;
   const VAdcCalibDataType *pCalibData;
   const VAdcChannelConfigType *pChConfig;

   hClientEvent = pDevCtxt->hClientEvent;
   uEventParam = pDevCtxt->uEventParam;

   eCurrentRequestMode = pDevCtxt->eCurrentRequestMode;
   uCurrentChannelIdx = pDevCtxt->uCurrentChannelIdx;

   switch (eCurrentRequestMode)
   {
      case VADC_REQUEST_MODE_READ:
         adcDeviceResult.uChannelIdx = pDevCtxt->uCurrentChannelIdx;

         result = VAdc_ReadAdcChannel(pDevCtxt, 
                                      uCurrentChannelIdx,
                                      &adcDeviceResult);
         if (result != DAL_SUCCESS) 
         {
            adcDeviceResult.eStatus = ADC_DEVICE_RESULT_INVALID;
         }

         pPayload = (void *)&adcDeviceResult;
         uPayloadSize = sizeof(adcDeviceResult);

         break;
      case VADC_REQUEST_MODE_RECALIBRATE:
         result = VAdc_CalibrateInternal(pDevCtxt);
         /*
         if (result != DAL_SUCCESS)
         {
            // If a payload is ever used then mark the result invalid here
         } 
         */ 

         pPayload = NULL;
         uPayloadSize = 0;

         // TODO: add a result type for recalibration if needed

         break;
      case VADC_REQUEST_MODE_CALIBRATE:
         result = VAdc_CalibrateInternal(pDevCtxt);
         /*
         if (result != DAL_SUCCESS)
         {
            // If a payload is ever used then mark the result invalid here
         } 
         */ 

         pPayload = NULL;
         uPayloadSize = 0;

         break;
      default:
         return DAL_SUCCESS;
   }

   if (eCurrentRequestMode == VADC_REQUEST_MODE_READ)
   {
      
      pChConfig = &pDevCtxt->pBsp->paChannels[pDevCtxt->uCurrentChannelIdx];
      pConfig = &pDevCtxt->pBsp->paConfigs[pChConfig->uConfigIdx];
      pCalibData = &pDevCtxt->paCalibData[pChConfig->uConfigIdx];

      vadc_diag_send_client_log(&adcDeviceResult, 
                                (uint32)pConfig->eDecimationRatio,
                                (uint32)pConfig->eClockSelect, 
                                pCalibData);
   }

   /* Processing is complete */
   pDevCtxt->eCurrentRequestMode = VADC_REQUEST_MODE_IDLE;

   DALSYS_SyncEnter(pDevCtxt->hSync);
   pDevCtxt->guardedDeviceStatus = ADC_DEVICE_CHANNEL_READY;
   DALSYS_SyncLeave(pDevCtxt->hSync);

   /* Notify the Adc driver that the request is complete */
   result = DALSYS_EventCtrlEx(hClientEvent,
                               DALSYS_EVENT_CTRL_TRIGGER,
                               uEventParam,
                               pPayload,
                               uPayloadSize);
   if (result != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to trigger client notification",
                    TRUE);
   }

   return DAL_SUCCESS;
}

/*======================================================================

  FUNCTION        VAdc_InterruptCb

  DESCRIPTION
      This function is the callback to process the conversion complete 
      interrupt from the arbiter. It sets the EOC signal.

  DEPENDENCIES    None

  PARAMETERS
      pArg            [in]
      uIntrStatusMask [in]

  RETURN VALUE    None

  SIDE EFFECTS    None
                  
======================================================================*/
#ifndef VADC_UNIT_TEST
void * VAdc_InterruptCb(void * pArg, uint32 uIntrStatusMask)
{
   DALResult result;
   VAdcHalResultType status;
   VAdcDevCtxt *pDevCtxt = (VAdcDevCtxt *)pArg;

   VAdc_LogInterrupt(&pDevCtxt->vAdcDebug);

   /* Clear all pending interrupts */
   status = VAdcHalClearInterrupts(&pDevCtxt->iVAdcHalInterface, VADC_INTERRUPT_MASK_BIT_EOC);
   if (status != VADC_HAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to clear EOC interrupt",
                    FALSE);
   }
/* As per SPMI team DalSpmi_InterruptDone no longer required and deprectated in SPMI HAL Interface 2.0 used in Bear*/
/*   result = DalSpmi_InterruptDone(pDevCtxt->phSpmiDev,
                                  pDevCtxt->pBsp->uPeripheralID,
                                  (uint32)VADC_INTERRUPT_MASK_BIT_EOC);
   if (result != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to mark SPMI interrupt done",
                    FALSE);
   }
*/
   /* Set the EOC signal */
   result = DALSYS_EventCtrlEx(pDevCtxt->ahEocEvent[0], 
                               DALSYS_EVENT_CTRL_TRIGGER,
                               0, 
                               NULL, 
                               0);
   if (result != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to set EOC signal",
                    FALSE);
   }

   return NULL;
}
#endif

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * The following functions are for DALDriver specific functionality
 * -------------------------------------------------------------------------*/
DALResult 
VAdc_DriverInit(VAdcDrvCtxt *pCtxt)
{
   return DAL_SUCCESS;
}

DALResult 
VAdc_DriverDeInit(VAdcDrvCtxt *pCtxt)
{
   return DAL_SUCCESS;
}

/*----------------------------------------------------------------------------
 * The following functions are declared in DalDevice Interface.
 * -------------------------------------------------------------------------*/
DALResult
VAdc_DeviceInit(VAdcClientCtxt *pCtxt)
{
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;
   VAdcInterruptsConfigType interruptConfig;
   DALSYSPropertyVar propVar;
   VAdcHalResultType result;
   DALResult ret=DAL_ERROR;
   uint32 uMode;
   uint32 uChannelIdx;
   uint32 uPerphId;
   const VAdcChannelConfigType *pChConfig;
   pm_err_flag_type pmResult;

   if (DalPlatformInfo_Platform() == DALPLATFORMINFO_TYPE_RUMI)
   {
     return ret;
   }

   /* Check if this is PM8916 to apply the MPP current sink WAR on MSM8909+PM8916 */
   if (pm_get_pmic_model(0) == PMIC_IS_PM8916)
   {
      bIsPM8916 = TRUE;
   }

   /* Initialize VADC context */
   pDevCtxt->bHardwareSupported = TRUE;
   pDevCtxt->hSync = NULL;
   pDevCtxt->paCalibData = NULL;
   pDevCtxt->hAdcOpWorkLoop = NULL;
   pDevCtxt->hAdcOpEvent = NULL;
   pDevCtxt->ahEocEvent[0] = NULL;
   pDevCtxt->ahEocEvent[1] = NULL;
   pDevCtxt->bCalibrated = FALSE;
   pDevCtxt->pBsp = NULL;

   VAdc_LogInit(&pDevCtxt->vAdcDebug);

   pDevCtxt->hNPACpuLatency = npa_create_sync_client("/core/cpu/latency", "VADC", NPA_CLIENT_REQUIRED);

   /* Read the DAL properties - hProp populated in device attach */
   ret = DALSYS_GetPropertyValue(pDevCtxt->hProp, "VADC_BSP", 0, &propVar); 

   if (ret == DAL_SUCCESS)
   {
      pDevCtxt->pBsp = (VAdcBspType *)propVar.Val.pStruct;
   }
   else
   {   
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to get BSP",
                    TRUE);
      return ret;
   }

#ifndef VADC_UNIT_TEST
   /* Attach to to the SPMI driver */
   ret = DAL_SpmiDeviceAttach("DALDEVICEID_SPMI_DEVICE", &pDevCtxt->phSpmiDev);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to attach to SPMI",
                    TRUE);
      return ret;
   }

   uMode = DAL_OPEN_SHARED | DAL_OPEN_READ | DAL_OPEN_WRITE;
   ret = DalDevice_Open(pDevCtxt->phSpmiDev, uMode);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to open SPMI",
                    TRUE);
      return ret;
   }
#endif

   /* Initialize the HAL interface */
   pDevCtxt->VAdcHalInterfaceCtxt.pDevCtxt = pDevCtxt;
   pDevCtxt->iVAdcHalInterface.pCtxt = (void *)&pDevCtxt->VAdcHalInterfaceCtxt;
   pDevCtxt->iVAdcHalInterface.pfnWriteBytes = VAdc_WriteBytes;
   pDevCtxt->iVAdcHalInterface.pfnReadBytes = VAdc_ReadBytes;

   /* Initialize synchronization object */
   ret = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                           &pDevCtxt->hSync,
                           &pDevCtxt->syncObject);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to create the sync object",
                    TRUE);
      goto errorCloseSpmi;
   }
   
   /* Allocate and initialize internal variables */
   ret = DALSYS_Malloc(sizeof(VAdcCalibDataType) * pDevCtxt->pBsp->uNumConfigs,
                       (void **)&pDevCtxt->paCalibData);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to allocate memory for calibration data",
                    TRUE);
      goto errorCloseSpmi;
   }

   // Log the revision & peripheral type
   result = VAdcHalGetRevisionInfo(&pDevCtxt->iVAdcHalInterface, &pDevCtxt->revisionInfo);
   if (result != VADC_HAL_SUCCESS)
   {
      ret = DAL_ERROR;
      goto errorCloseSpmi;
   }

   // Sanity check the peripheral type
   if (pDevCtxt->pBsp->uPerphType != pDevCtxt->revisionInfo.uType)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - Invalid VADC peripheral type", 
                    TRUE);
      pDevCtxt->bHardwareSupported = FALSE;
   }

   // Check the digital version information
   if (pDevCtxt->revisionInfo.uDigitalMajor < pDevCtxt->pBsp->uMinDigMajor)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - Invalid HW - dig major", 
                    TRUE);
      pDevCtxt->bHardwareSupported = FALSE;
   }
   else if (pDevCtxt->revisionInfo.uDigitalMajor == pDevCtxt->pBsp->uMinDigMajor)
   {
      if (pDevCtxt->revisionInfo.uDigitalMinor < pDevCtxt->pBsp->uMinDigMinor)
      {
         VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - Invalid HW - dig minor", 
                       TRUE);
         pDevCtxt->bHardwareSupported = FALSE;
      }
   }

   // Check the analog version information
   if (pDevCtxt->revisionInfo.uAnalogMajor < pDevCtxt->pBsp->uMinAnaMajor)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - Invalid HW - ana major", 
                    TRUE);
      pDevCtxt->bHardwareSupported = FALSE;
   }
   else if (pDevCtxt->revisionInfo.uAnalogMajor == pDevCtxt->pBsp->uMinAnaMajor)
   {
      if (pDevCtxt->revisionInfo.uAnalogMinor < pDevCtxt->pBsp->uMinAnaMinor)
      {
         VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - Invalid HW - ana minor", 
                       TRUE);
         pDevCtxt->bHardwareSupported = FALSE;
      }
   }

   if (pDevCtxt->bHardwareSupported == FALSE)
   {
      ret = DAL_SUCCESS;
      goto errorCloseSpmi;
   }

   if (pDevCtxt->pBsp->bUsesInterrupts)
   {
#ifndef VADC_UNIT_TEST
      /* Register for the ADC peripheral ISR */
      uPerphId = ((pDevCtxt->pBsp->uSlaveId << 8) & 0xF00) | (pDevCtxt->pBsp->uPeripheralID & 0xFF);

      ret = DalSpmi_RegisterIsr(pDevCtxt->phSpmiDev, 
                                uPerphId,
                                (uint32)VADC_INTERRUPT_MASK_BIT_EOC,
                                VAdc_InterruptCb, 
                                pDevCtxt);
      if (ret != DAL_SUCCESS)
      {
         VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to register ISR",
                    TRUE);
         goto errorCloseSpmi;
      }
#endif

      /* Configure peripheral interrupts */
      interruptConfig.eEocInterruptConfig = VADC_INTERRUPT_CONFIG_RISING_EDGE;
      interruptConfig.eFifoNotEmptyInterruptConfig = VADC_INTERRUPT_CONFIG_NONE;
      interruptConfig.eSequencerTimeoutInterruptConfig = VADC_INTERRUPT_CONFIG_NONE;
      interruptConfig.eHighThresholdInterruptConfig = VADC_INTERRUPT_CONFIG_NONE;
      interruptConfig.eLowThresholdInterruptConfig = VADC_INTERRUPT_CONFIG_NONE;

      result = VAdcHalConfigInterrupts(&pDevCtxt->iVAdcHalInterface, &interruptConfig);
      if (result != VADC_HAL_SUCCESS)
      {
         ret = DAL_ERROR;
         goto errorCloseSpmi;
      }

      result = VAdcHalSetInterruptMid(&pDevCtxt->iVAdcHalInterface, 
                                      (VAdcInterruptMid)pDevCtxt->pBsp->uMasterID);
      if (result != VADC_HAL_SUCCESS)
      {
         ret = DAL_ERROR;
         goto errorCloseSpmi;
      }

      /* Clear all pending interrupts */
      result = VAdcHalClearInterrupts(&pDevCtxt->iVAdcHalInterface, VADC_INTERRUPT_MASK_BIT_EOC);
      if (result != VADC_HAL_SUCCESS)
      {
         ret = DAL_ERROR;
         goto errorCloseSpmi;
      }
      
      result = VAdcHalEnableInterrupts(&pDevCtxt->iVAdcHalInterface, VADC_INTERRUPT_MASK_BIT_EOC);
      if (result != VADC_HAL_SUCCESS)
      {
         ret = DAL_ERROR;
         goto errorCloseSpmi;
      }
   }

   /* Create the work-loop */
   ret = DALSYS_RegisterWorkLoopEx("VADC_WORK_LOOP",
                                   VADC_DEFAULT_STACK_SIZE,
                                   gVAdcWorkloopPriority, 
                                   10,
                                   &(pDevCtxt->hAdcOpWorkLoop),
                                   NULL);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to create workloop",
                    TRUE);
      goto errorCloseSpmi;
   }

   /* Create a workloop event for performing ADC functions */
   ret = DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT,
                            &(pDevCtxt->hAdcOpEvent), 
                            NULL);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to create workloop event",
                    TRUE);
      goto errorCloseSpmi;
   }

   /* Add the event to the workloop */
   ret = DALSYS_AddEventToWorkLoop(pDevCtxt->hAdcOpWorkLoop,
                                   VAdc_PerformOperationInWorkLoop, 
                                   pDevCtxt,
                                   pDevCtxt->hAdcOpEvent, 
                                   NULL);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to add event to workloop",
                    TRUE);
      goto errorCloseSpmi;
   }

   /* Configure the EOC event & timeout event */
   ret = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT,
                            &pDevCtxt->ahEocEvent[0], 
                            NULL);
   if (ret != DAL_SUCCESS)
   {
       VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to create the EOC event",
                    TRUE);
      goto errorCloseSpmi;
   }

   ret = DALSYS_EventCreate(DALSYS_EVENT_ATTR_TIMEOUT_EVENT,
                            &pDevCtxt->ahEocEvent[1], 
                            NULL);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to create the EOC timeout event",
                    TRUE);

      goto errorCloseSpmi;
   }

#ifndef VADC_UNIT_TEST
   ret = DalDevice_Close(pDevCtxt->phSpmiDev);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - warning - could not close SPMI in init",
                    FALSE);
   }

   /* Map static MPPs */
   for (uChannelIdx = 0; uChannelIdx < pDevCtxt->pBsp->uNumChannels; uChannelIdx++)
   {
      pChConfig = &pDevCtxt->pBsp->paChannels[uChannelIdx];

      if (pChConfig->eMppConfig == VADC_CHANNEL_MPP_CONFIG_STATIC)
      {
         if (pChConfig->eMpp == PM_MPP_INVALID || 
             pChConfig->eChSelect == PM_MPP__AIN__CH_INVALID)
         {
            VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - invalid MPP configuration",
                          FALSE);
            return DAL_ERROR;
         }

         pmResult = pm_dev_mpp_config_analog_input(pDevCtxt->pBsp->uPmicDevice,
                                                   pChConfig->eMpp,
                                                   pChConfig->eChSelect);
         if (pmResult != PM_ERR_FLAG__SUCCESS)
         {
            VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to configure static MPP",
                          FALSE);
            return DAL_ERROR;
         }
      }
   }
#endif

   /* Ready for conversions */
   DALSYS_SyncEnter(pDevCtxt->hSync);
   pDevCtxt->guardedDeviceStatus = ADC_DEVICE_CHANNEL_READY;
   DALSYS_SyncLeave(pDevCtxt->hSync);

   pDevCtxt->eCurrentRequestMode = VADC_REQUEST_MODE_IDLE;

   return DAL_SUCCESS;

errorCloseSpmi:
#ifndef VADC_UNIT_TEST
   ret = DalDevice_Close(pDevCtxt->phSpmiDev);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - warning - could not close SPMI in init",
                    FALSE);
   }
#endif
   return ret;
}

DALResult 
VAdc_DeviceDeInit(VAdcClientCtxt *pCtxt)
{
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;
   DALResult result;
   DALResult retResult = DAL_SUCCESS;

#ifndef VADC_UNIT_TEST
   if (pDevCtxt->phSpmiDev != NULL)
   {
      if (pDevCtxt->pBsp->bUsesInterrupts)
      {
         result = DalSpmi_UnregisterIsr(pDevCtxt->phSpmiDev, 
                                        pDevCtxt->pBsp->uPeripheralID,
                                        (uint32)VADC_INTERRUPT_MASK_BIT_EOC);
         if (result != DAL_SUCCESS)
         {
            retResult = DAL_ERROR;
         }
      }

      result = DAL_DeviceDetach(pDevCtxt->phSpmiDev);
      if (result != DAL_SUCCESS)
      {
         retResult = DAL_ERROR;
      }
      pDevCtxt->phSpmiDev = NULL;
   }
#endif

   if (pDevCtxt->ahEocEvent[0] != NULL)
   {
      result = DALSYS_DestroyObject(&pDevCtxt->ahEocEvent[0]);
      if (result != DAL_SUCCESS)
      {
         retResult = DAL_ERROR;
      }
      pDevCtxt->ahEocEvent[0] = NULL;
   }

   if (pDevCtxt->ahEocEvent[1] != NULL)
   {
      result = DALSYS_DestroyObject(&pDevCtxt->ahEocEvent[1]);
      if (result != DAL_SUCCESS)
      {
         retResult = DAL_ERROR;
      }
      pDevCtxt->ahEocEvent[1] = NULL;
   }

   if (pDevCtxt->paCalibData != NULL)
   {
      result = DALSYS_Free(pDevCtxt->paCalibData);
      if (result != DAL_SUCCESS)
      {
         retResult = DAL_ERROR;
      }
      pDevCtxt->paCalibData = NULL;
   }

   if (pDevCtxt->hSync != NULL)
   {
      result = DALSYS_DestroyObject(pDevCtxt->hSync);
      if (result != DAL_SUCCESS)
      {
         retResult = DAL_ERROR;
      }
      pDevCtxt->hSync = NULL;
   }

   if (pDevCtxt->hAdcOpEvent != NULL)
   {
      if (pDevCtxt->hAdcOpWorkLoop != NULL)
      {
         result = DALSYS_DeleteEventFromWorkLoop(pDevCtxt->hAdcOpWorkLoop,
                                                 pDevCtxt->hAdcOpEvent);
         if (result != DAL_SUCCESS)
         {
            retResult = DAL_ERROR;
         }
      }

      result = DALSYS_DestroyObject(pDevCtxt->hAdcOpEvent);
      if (result != DAL_SUCCESS)
      {
         retResult = DAL_ERROR;
      }
      pDevCtxt->hAdcOpEvent = NULL;
   }

   return retResult;
}

DALResult 
VAdc_PowerEvent(VAdcClientCtxt *pCtxt, 
                DalPowerCmd PowerCmd, 
                DalPowerDomain PowerDomain)
{
    return DAL_SUCCESS;
}

DALResult 
VAdc_Open(VAdcClientCtxt *pCtxt, uint32 dwaccessMode)
{
   VAdcHalResultType result;
   DALResult ret;
   uint32 uMode;
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;

   if (pDevCtxt->bHardwareSupported == FALSE)
   {
      return DAL_SUCCESS;
   }

   if (pDevCtxt->hNPACpuLatency != NULL)
   {
      npa_issue_required_request(pDevCtxt->hNPACpuLatency, 1);
   }

#ifndef VADC_UNIT_TEST
   uMode = DAL_OPEN_SHARED | DAL_OPEN_READ | DAL_OPEN_WRITE;
   ret = DalDevice_Open(pDevCtxt->phSpmiDev, uMode);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to open SPMI",
                    TRUE);
      return ret;
   }
#endif

   result = VAdcHalSetEnable(&pDevCtxt->iVAdcHalInterface, VADC_ENABLE);
   if (result != VADC_HAL_SUCCESS)
   {
      return DAL_ERROR;
   }
   
   return DAL_SUCCESS;
}

DALResult 
VAdc_Close(VAdcClientCtxt *pCtxt)
{
   VAdcHalResultType result;
   DALResult ret = DAL_SUCCESS;
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;

   if (pDevCtxt->bHardwareSupported == FALSE)
   {
      return DAL_SUCCESS;
   }

   result = VAdcHalSetEnable(&pDevCtxt->iVAdcHalInterface, VADC_DISABLE);
   if (result != VADC_HAL_SUCCESS)
   {
      ret = DAL_ERROR;
   }

#ifndef VADC_UNIT_TEST
   ret = DalDevice_Close(pDevCtxt->phSpmiDev);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - warning - could not close SPMI",
                    FALSE);
   }
#endif

   if (pDevCtxt->hNPACpuLatency != NULL)
   {
      npa_complete_request(pDevCtxt->hNPACpuLatency);
   }

   return ret;
}

DALResult 
VAdc_Info(VAdcClientCtxt *pCtxt, DalDeviceInfo *pdeviceInfo, uint32 dwSize)
{
   return DAL_SUCCESS;
}

/*----------------------------------------------------------------------------
 * The following functions are extended in DalAdcDevice Interface. 
 *  
 * These functions are documented in DDIAdcDevice.h
 * -------------------------------------------------------------------------*/
DALResult 
VAdc_GetDeviceProperties(VAdcClientCtxt *pCtxt,
                         AdcDeviceDevicePropertiesType *pAdcDeviceProp)
{
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;

   pAdcDeviceProp->uNumChannels = pDevCtxt->pBsp->uNumChannels;

   return DAL_SUCCESS;
}

DALResult 
VAdc_GetChannel(VAdcClientCtxt *pCtxt, 
                const char *pszChannelName, 
                uint32 *puChannelIdx)
{
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;
   uint32 uNumChannels;
   uint32 uChannel;
   const char *pszCurrentChannelName;

   if (pDevCtxt->bHardwareSupported == FALSE)
   {
      return DAL_ERROR;
   }

   uNumChannels = pDevCtxt->pBsp->uNumChannels;

   for (uChannel = 0; uChannel < uNumChannels; uChannel++)
   {
      pszCurrentChannelName = pDevCtxt->pBsp->paChannels[uChannel].pName;

      if (strcmp(pszChannelName, pszCurrentChannelName) == 0)
      {
         *puChannelIdx = uChannel;
         return DAL_SUCCESS;
      }
   }
   
   return DAL_ERROR;
}

DALResult 
VAdc_GetChannelStatus(VAdcClientCtxt *pCtxt,
                      uint32 uChannelIdx,
                      AdcDeviceChannelStatusType *pDeviceStatus) 
{
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;

   DALSYS_SyncEnter(pDevCtxt->hSync);
   *pDeviceStatus = pDevCtxt->guardedDeviceStatus;
   DALSYS_SyncLeave(pDevCtxt->hSync);

   return DAL_SUCCESS;
}

DALResult 
VAdc_Calibrate(VAdcClientCtxt *pCtxt,
               const DALSYSEventHandle hEvent,
               uint32 uEventParam) 
{
   DALResult ret;
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;

   if (pDevCtxt->bHardwareSupported == FALSE)
   {
      return DAL_SUCCESS;
   }

   DALSYS_SyncEnter(pDevCtxt->hSync);
   if (pDevCtxt->guardedDeviceStatus != ADC_DEVICE_CHANNEL_READY)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc recalibration requested while device busy",
                    FALSE);

      DALSYS_SyncLeave(pDevCtxt->hSync);

      return DAL_ERROR;
   }

   pDevCtxt->guardedDeviceStatus = ADC_DEVICE_CHANNEL_CALIBRATING;
   DALSYS_SyncLeave(pDevCtxt->hSync);

   /* Fill in context info about the current request */  
   pDevCtxt->hClientEvent = hEvent;
   pDevCtxt->uEventParam = uEventParam;
   pDevCtxt->eCurrentRequestMode = VADC_REQUEST_MODE_CALIBRATE;

   /* Trigger the workloop to perform the recalibration */
   ret = DALSYS_EventCtrlEx(pDevCtxt->hAdcOpEvent, 
                            DALSYS_EVENT_CTRL_TRIGGER,
                            0, 
                            NULL, 
                            0);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to trigger workloop",
                    TRUE);
      goto error;
   }
   
   return DAL_SUCCESS;

error:
   pDevCtxt->eCurrentRequestMode = VADC_REQUEST_MODE_IDLE;

   DALSYS_SyncEnter(pDevCtxt->hSync);
   pDevCtxt->guardedDeviceStatus = ADC_DEVICE_CHANNEL_READY;
   DALSYS_SyncLeave(pDevCtxt->hSync);

   return ret;
}

DALResult 
VAdc_RecalibrateChannel(VAdcClientCtxt *pCtxt,
                        uint32 uChannelIdx,
                        const DALSYSEventHandle hEvent,
                        uint32 uEventParam) 
{
   DALResult ret;
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;

   if (pDevCtxt->bHardwareSupported == FALSE)
   {
      return DAL_ERROR;
   }

   if (uChannelIdx >= pDevCtxt->pBsp->uNumChannels)
   {
      return DAL_ERROR;
   }

   DALSYS_SyncEnter(pDevCtxt->hSync);
   if (pDevCtxt->guardedDeviceStatus != ADC_DEVICE_CHANNEL_READY)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc recalibration requested while device busy",
                    FALSE);

      DALSYS_SyncLeave(pDevCtxt->hSync);

      return DAL_ERROR;
   }

   pDevCtxt->guardedDeviceStatus = ADC_DEVICE_CHANNEL_CALIBRATING;
   DALSYS_SyncLeave(pDevCtxt->hSync);

   /* Fill in context info about the current request */  
   pDevCtxt->hClientEvent = hEvent;
   pDevCtxt->uEventParam = uEventParam;
   pDevCtxt->uCurrentChannelIdx = uChannelIdx;
   pDevCtxt->eCurrentRequestMode = VADC_REQUEST_MODE_RECALIBRATE;

   VAdc_LogRecalibrationRequest(&pDevCtxt->vAdcDebug, uChannelIdx);

   /* Trigger the workloop to perform the recalibration */
   ret = DALSYS_EventCtrlEx(pDevCtxt->hAdcOpEvent, 
                            DALSYS_EVENT_CTRL_TRIGGER,
                            0, 
                            NULL, 
                            0);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to trigger workloop",
                    TRUE);
      goto error;
   }
   
   return DAL_SUCCESS;

error:
   pDevCtxt->eCurrentRequestMode = VADC_REQUEST_MODE_IDLE;

   DALSYS_SyncEnter(pDevCtxt->hSync);
   pDevCtxt->guardedDeviceStatus = ADC_DEVICE_CHANNEL_READY;
   DALSYS_SyncLeave(pDevCtxt->hSync);

   return ret;
}

DALResult 
VAdc_StartConversion(VAdcClientCtxt *pCtxt,
                     uint32 uChannelIdx,
                     const DALSYSEventHandle hEvent,
                     uint32 uEventParam) 
{
   DALResult ret;
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;

   if (pDevCtxt->bHardwareSupported == FALSE)
   {
      return DAL_ERROR;
   }

   if (uChannelIdx >= pDevCtxt->pBsp->uNumChannels)
   {
      return DAL_ERROR;
   }

   DALSYS_SyncEnter(pDevCtxt->hSync);
   if (pDevCtxt->guardedDeviceStatus != ADC_DEVICE_CHANNEL_READY)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc conversion requested while device busy",
                    FALSE);

      DALSYS_SyncLeave(pDevCtxt->hSync);

      return DAL_ERROR;
   }

   pDevCtxt->guardedDeviceStatus = ADC_DEVICE_CHANNEL_BUSY;
   DALSYS_SyncLeave(pDevCtxt->hSync);

   /* Fill in context info about the current request */  
   pDevCtxt->hClientEvent = hEvent;
   pDevCtxt->uEventParam = uEventParam;
   pDevCtxt->uCurrentChannelIdx = uChannelIdx;
   pDevCtxt->eCurrentRequestMode = VADC_REQUEST_MODE_READ;

   VAdc_LogConversionRequest(&pDevCtxt->vAdcDebug, uChannelIdx);

   /* Trigger the workloop to perform the read */
   ret = DALSYS_EventCtrlEx(pDevCtxt->hAdcOpEvent, 
                            DALSYS_EVENT_CTRL_TRIGGER,
                            0, 
                            NULL, 
                            0);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to trigger workloop",
                    TRUE);
      goto error;
   }
   
   return DAL_SUCCESS;

error:
   pDevCtxt->eCurrentRequestMode = VADC_REQUEST_MODE_IDLE;

   DALSYS_SyncEnter(pDevCtxt->hSync);
   pDevCtxt->guardedDeviceStatus = ADC_DEVICE_CHANNEL_READY;
   DALSYS_SyncLeave(pDevCtxt->hSync);

   return ret;
}

