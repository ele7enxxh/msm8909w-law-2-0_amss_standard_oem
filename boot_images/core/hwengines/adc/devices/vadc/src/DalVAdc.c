/*============================================================================
  FILE:         DALVAdc.c

  OVERVIEW:     Implementation of a Physical ADC device DAL for the
                PMIC VADC peripheral.

  DEPENDENCIES: None

                Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/adc/devices/vadc/src/DalVAdc.c#1 $$DateTime: 2015/09/01 00:30:35 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-06-19  jjo  Support for multiple VADCs.
  2013-04-17  jjo  Store PMIC fab ID.
  2012-11-26  jjo  Fix to use the correct config when calibrating.
  2012-07-31  jjo  Now checks the ADC type
  2012-06-29  jjo  Added SPMI.
  2012-06-28  jjo  Unit test updates.
  2012-06-20  jjo  Ported to UEFI.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DalVAdc.h"
#include "DALDeviceId.h"
#include "pm_version.h"
#include "string.h"

#ifdef VADC_UNIT_TEST
#include "VAdcSim.h"
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define VADC_BUSYWAIT_INCREMENT_US 50

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
/*======================================================================

  FUNCTION        VAdc_ReadBytes

  DESCRIPTION     This function reads from the VAdc peripheral

  DEPENDENCIES    None

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
   DALResult status = DAL_SUCCESS;
   SpmiBus_ResultType result;
   VAdcHalInterfaceCtxtType *pVAdcHalInterfaceCtxt;
   VAdcDevCtxt *pDevCtxt;
   uint32 uRegisterAddress;

   if (pCtxt == NULL)
   {
      return VADC_HAL_ERROR;
   }

   pVAdcHalInterfaceCtxt = (VAdcHalInterfaceCtxtType *)pCtxt;
   pDevCtxt = pVAdcHalInterfaceCtxt->pDevCtxt;

   uRegisterAddress = uOffset & 0xFF;
   uRegisterAddress |= (pDevCtxt->pBsp->uPeripheralID << 8) & 0xFF00;

#ifndef VADC_UNIT_TEST
   result = SpmiBus_ReadLong((uint32) pDevCtxt->pBsp->uSlaveId,
                             pDevCtxt->pBsp->eAccessPriority,
                             (uint32) uRegisterAddress,
                             (uint8 *) pucData,
                             (uint32) uDataLen,
                             (uint32 *) puTotalBytesRead);

   if ((status != DAL_SUCCESS) ||
       (result != SPMI_BUS_SUCCESS))
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

  DEPENDENCIES    None

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
   DALResult status = DAL_SUCCESS;
   SpmiBus_ResultType result;
   VAdcHalInterfaceCtxtType *pVAdcHalInterfaceCtxt;
   VAdcDevCtxt *pDevCtxt;
   uint32 uRegisterAddress;

   if (pCtxt == NULL)
   {
      return VADC_HAL_ERROR;
   }

   pVAdcHalInterfaceCtxt = (VAdcHalInterfaceCtxtType *)pCtxt;
   pDevCtxt = pVAdcHalInterfaceCtxt->pDevCtxt;

   uRegisterAddress = uOffset & 0xFF;
   uRegisterAddress |= (pDevCtxt->pBsp->uPeripheralID << 8) & 0xFF00;

#ifndef VADC_UNIT_TEST
   result = SpmiBus_WriteLong((uint32) pDevCtxt->pBsp->uSlaveId,
                              pDevCtxt->pBsp->eAccessPriority,
                              (uint32) uRegisterAddress,
                              (uint8 *) pucData,
                              (uint32) uDataLen);

   if ((status != DAL_SUCCESS) ||
       (result != SPMI_BUS_SUCCESS))
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

  FUNCTION        VAdc_StoreFabID

  DESCRIPTION
      Gets the fab ID from PMIC and stores it in dev ctxt and peripheral
      registers.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt    [in]

  RETURN VALUE
      DAL_SUCCESS or an error

  SIDE EFFECTS
      None.

======================================================================*/
static DALResult VAdc_StoreFabID(VAdcDevCtxt *pDevCtxt)
{
//TODO: Remove after 8952 PMIC compilation support is enabled.
#ifndef FEATURE_NO_PMIC_SUPPORT
   pm_err_flag_type pm_ret;
   pm_pbs_info_type pbs_info;
   VAdcHalResultType result;

   pm_ret = pm_get_pbs_info((uint8)pDevCtxt->pBsp->uPmicDevice, &pbs_info);
   if (pm_ret != PM_ERR_FLAG__SUCCESS)
   {
      return DAL_ERROR;
   }

   pDevCtxt->uFabID = pbs_info.fab_id;

   result = VAdcHalStoreFabID(&pDevCtxt->iVAdcHalInterface,
                              (VAdcFabIDType)pbs_info.fab_id);
   if (result != VADC_HAL_SUCCESS)
   {
      return DAL_ERROR;
   }
#endif
   return DAL_SUCCESS;
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


  DEPENDENCIES    None

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
   VAdcHalResultType result;
   VAdcConversionParametersType conversionParams;
   VAdcSequencerParametersType sequencerParams;

   /*
    * Initialize variables.
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
   uint32 uTimeWaited_us;
   VAdcHalResultType status;

   uConversionTime_us = pConfig->uConversionTime_us;
   uMaxWaitTime_us = uConversionTime_us * pDevCtxt->pBsp->uNumPeriodsTimeout;
   uTimeWaited_us = 0;

   do
   {

#ifndef VADC_UNIT_TEST
      DALSYS_BusyWait(VADC_BUSYWAIT_INCREMENT_US);
#endif

      uTimeWaited_us += VADC_BUSYWAIT_INCREMENT_US;

      if (uTimeWaited_us >= uConversionTime_us)
      {
         /* check for EOC */
         status = VAdcHalGetStatus(&pDevCtxt->iVAdcHalInterface, pConvStatus);
         if (status != VADC_HAL_SUCCESS)
         {
            return DAL_ERROR;
         }

         if (pConvStatus->eConversionStatus == VADC_CONVERSION_STATUS_COMPLETE)
         {
            return DAL_SUCCESS;
         }
      }

   } while (uTimeWaited_us < uMaxWaitTime_us);

   /* Conversion did not complete */
   VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - conversion timed out",
                 FALSE);
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

   pDevCtxt->uCurrentChannelIdx = uChannel;

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

   VAdc_LogConversionResult(&pDevCtxt->vAdcDebug, pAdcDeviceResult);

   return DAL_SUCCESS;

error:
   pAdcDeviceResult->eStatus = ADC_DEVICE_RESULT_INVALID;

   return DAL_SUCCESS;
}

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
   DALSYSPropertyVar propVar;
   VAdcHalResultType result;
   DALResult ret = DAL_ERROR;
//TODO: Remove after 8952 PMIC compilation support is enabled.
#ifndef FEATURE_NO_PMIC_SUPPORT
   pm_err_flag_type status;
#endif
   uint32 uChannelIdx;
   const VAdcChannelConfigType *pChConfig;
   SpmiBus_ResultType spmiRet;

   VAdc_LogInit(&pDevCtxt->vAdcDebug);

   pDevCtxt->bHardwareSupported = TRUE;

   /* Read the DAL properties - hProp populated in device attach */
   ret = DALSYS_GetPropertyValue(pDevCtxt->hProp, "VADC_BSP", 0, &propVar);

   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to get BSP",
                    TRUE);
      return ret;
   }

   pDevCtxt->pBsp = (VAdcBspType *)propVar.Val.pStruct;

#ifndef VADC_UNIT_TEST
   /* Attach to to the SPMI driver */
   spmiRet = SpmiBus_Init();
   if (spmiRet != SPMI_BUS_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed attaching to SPMI",
                    TRUE);
      return DAL_ERROR;
   }
#endif

   /* Initialize the HAL interface */
   pDevCtxt->VAdcHalInterfaceCtxt.pDevCtxt = pDevCtxt;
   pDevCtxt->iVAdcHalInterface.pCtxt = (void *)&pDevCtxt->VAdcHalInterfaceCtxt;
   pDevCtxt->iVAdcHalInterface.pfnWriteBytes = VAdc_WriteBytes;
   pDevCtxt->iVAdcHalInterface.pfnReadBytes = VAdc_ReadBytes;

   /* allocate and initialize internal variables */
   pDevCtxt->bCalibrated = FALSE;

   ret = DALSYS_Malloc(sizeof(VAdcCalibDataType) * pDevCtxt->pBsp->uNumConfigs,
                       (void **)&pDevCtxt->paCalibData);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to allocate memory for calibration data",
                    TRUE);
      return ret;
   }

   // Get the fab ID
   ret = VAdc_StoreFabID(pDevCtxt);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to get the fab ID",
                    TRUE);
      return ret;
   }

   // Log the revision & peripheral type
   result = VAdcHalGetRevisionInfo(&pDevCtxt->iVAdcHalInterface, &pDevCtxt->revisionInfo);
   if (result != VADC_HAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   // Sanity check the peripheral type & subtype
   if (pDevCtxt->pBsp->uPerphType != pDevCtxt->revisionInfo.uType)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - Invalid VADC peripheral type",
                    TRUE);
      pDevCtxt->bHardwareSupported = FALSE;
   }

   if (pDevCtxt->pBsp->uPerphSubType != pDevCtxt->revisionInfo.uSubType)
   {
      VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - Invalid VADC peripheral subtype",
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
      return DAL_SUCCESS;
   }

#ifndef VADC_UNIT_TEST
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
//TODO: Remove after 8952 PMIC compilation support is enabled.
#ifndef FEATURE_NO_PMIC_SUPPORT
         status = pm_mpp_config_analog_input( pDevCtxt->pBsp->uPmicDevice,
                                              pChConfig->eMpp,
                                              pChConfig->eChSelect);
         if (status != PM_ERR_FLAG__SUCCESS)
         {
            VAdc_LogError(&pDevCtxt->vAdcDebug, pDevCtxt->DevId, "VAdc - failed to configure static MPP",
                          FALSE);
            return DAL_ERROR;
         }
#endif
      }
   }
#endif

   return DAL_SUCCESS;
}

DALResult
VAdc_DeviceDeInit(VAdcClientCtxt *pCtxt)
{
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;
   DALResult result;
   DALResult ret = DAL_SUCCESS;

   /* detach from SPMI */
   (void)SpmiBus_DeInit();

   /* free memory */
   if (pDevCtxt->paCalibData != NULL)
   {
      result = DALSYS_Free(pDevCtxt->paCalibData);
      if (result != DAL_SUCCESS)
      {
         ret = DAL_ERROR;
      }
      pDevCtxt->paCalibData = NULL;
   }

   return ret;
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
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;

   if (pDevCtxt->bHardwareSupported == FALSE)
   {
      return DAL_SUCCESS;
   }

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
VAdc_RecalibrateChannelSync(VAdcClientCtxt *pCtxt,
                            uint32 uChannelIdx,
                            AdcDeviceRecalibrateResultType *pAdcDeviceRecalibrateResult)
{
   DALResult result;
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;
   uint32 uNumChannels;
   uint32 uConfigIdx;
   VAdcCalibDataType *paCalibData;

   if (pDevCtxt->bHardwareSupported == FALSE)
   {
      return DAL_ERROR;
   }

   uNumChannels = pDevCtxt->pBsp->uNumChannels;

   if (uChannelIdx >= uNumChannels)
   {
      pAdcDeviceRecalibrateResult->eStatus = ADC_DEVICE_RESULT_INVALID_CHANNEL;

      return DAL_SUCCESS;
   }

   pDevCtxt->guardedDeviceStatus = ADC_DEVICE_CHANNEL_CALIBRATING;
   pDevCtxt->uCurrentChannelIdx = uChannelIdx;
   pDevCtxt->eCurrentRequestMode = VADC_REQUEST_MODE_RECALIBRATE;

   VAdc_LogRecalibrationRequest(&pDevCtxt->vAdcDebug, uChannelIdx);

   result = VAdc_CalibrateInternal(pDevCtxt);
   if (result != DAL_SUCCESS)
   {
      pDevCtxt->eCurrentRequestMode = VADC_REQUEST_MODE_IDLE;
      pDevCtxt->guardedDeviceStatus = ADC_DEVICE_CHANNEL_READY;

      return result;
   }

   pDevCtxt->eCurrentRequestMode = VADC_REQUEST_MODE_IDLE;
   pDevCtxt->guardedDeviceStatus = ADC_DEVICE_CHANNEL_READY;

   uConfigIdx = pDevCtxt->pBsp->paChannels[uChannelIdx].uConfigIdx;
   paCalibData = &pDevCtxt->paCalibData[uConfigIdx];

   pAdcDeviceRecalibrateResult->eStatus = ADC_DEVICE_RESULT_VALID;
   pAdcDeviceRecalibrateResult->uChannelIdx = uChannelIdx;

   if (pDevCtxt->pBsp->paChannels[uChannelIdx].eCalMethod == VADC_CAL_METHOD_ABSOLUTE)
   {
      pAdcDeviceRecalibrateResult->nPhysical1 = pDevCtxt->pBsp->uVref1_mv;
      pAdcDeviceRecalibrateResult->nPhysical2 = pDevCtxt->pBsp->uVref2_mv;

      pAdcDeviceRecalibrateResult->uCode1 = paCalibData->uVref1;
      pAdcDeviceRecalibrateResult->uCode2 = paCalibData->uVref2;
   }
   else
   {
      pAdcDeviceRecalibrateResult->nPhysical1 = pDevCtxt->pBsp->uVrefN_mv;
      pAdcDeviceRecalibrateResult->nPhysical2 = pDevCtxt->pBsp->uVrefP_mv;

      pAdcDeviceRecalibrateResult->uCode1 = paCalibData->uVrefN;
      pAdcDeviceRecalibrateResult->uCode2 = paCalibData->uVrefP;
   }

   return DAL_SUCCESS;
}

DALResult
VAdc_ReadChannelSync(VAdcClientCtxt *pCtxt,
                     uint32 uChannelIdx,
                     AdcDeviceResultType *pAdcDeviceResult)
{
   DALResult result;
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;
   uint32 uNumChannels;

   if (pDevCtxt->bHardwareSupported == FALSE)
   {
      return DAL_ERROR;
   }

   uNumChannels = pDevCtxt->pBsp->uNumChannels;

   if (uChannelIdx >= uNumChannels)
   {
      pAdcDeviceResult->eStatus = ADC_DEVICE_RESULT_INVALID_CHANNEL;

      return DAL_SUCCESS;
   }

   pDevCtxt->guardedDeviceStatus = ADC_DEVICE_CHANNEL_BUSY;
   pDevCtxt->uCurrentChannelIdx = uChannelIdx;
   pDevCtxt->eCurrentRequestMode = VADC_REQUEST_MODE_READ;

   VAdc_LogConversionRequest(&pDevCtxt->vAdcDebug, uChannelIdx);

   result = VAdc_ReadAdcChannel(pDevCtxt, uChannelIdx, pAdcDeviceResult);
   if (result != DAL_SUCCESS)
   {
      pDevCtxt->eCurrentRequestMode = VADC_REQUEST_MODE_IDLE;
      pDevCtxt->guardedDeviceStatus = ADC_DEVICE_CHANNEL_READY;

      return result;
   }

   pDevCtxt->eCurrentRequestMode = VADC_REQUEST_MODE_IDLE;
   pDevCtxt->guardedDeviceStatus = ADC_DEVICE_CHANNEL_READY;

   return DAL_SUCCESS;
}

/*----------------------------------------------------------------------------
 * The following functions are not supported in boot.
 * -------------------------------------------------------------------------*/
DALResult
VAdc_GetChannelStatus(VAdcClientCtxt *pCtxt,
                      uint32 uChannelIdx,
                      AdcDeviceChannelStatusType *pAdcChannelStatus)
{
   return DAL_ERROR;
}

DALResult
VAdc_Calibrate(VAdcClientCtxt *pCtxt,
               const DALSYSEventHandle hEvent,
               uint32 uEventParam)
{
   return DAL_ERROR;
}

DALResult
VAdc_RecalibrateChannel(VAdcClientCtxt *pCtxt,
                        uint32 uChannelIdx,
                        const DALSYSEventHandle hEvent,
                        uint32 uEventParam)
{
   return DAL_ERROR;
}

DALResult
VAdc_StartConversion(VAdcClientCtxt *pCtxt,
                     uint32 uChannelIdx,
                     const DALSYSEventHandle hEvent,
                     uint32 uEventParam)
{
   return DAL_ERROR;
}

