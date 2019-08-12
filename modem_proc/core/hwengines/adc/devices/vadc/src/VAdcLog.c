/*============================================================================
  FILE:         VAdcLog.c

  OVERVIEW:     Logging for the VAdc
                
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

  $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/adc/devices/vadc/src/VAdcLog.c#1 $$DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-06-19  jjo  Support for multiple VADCs.
  2013-02-07  jjo  Added F3 logging.
  2012-07-09  jjo  Initial version.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "VAdcLog.h"

#ifndef VADC_LOG_DISABLED

#include "DALFramework.h"
#include "timetick.h" // TODO: try to log in us instead of ticks
#include "msg.h"

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
/*======================================================================

  FUNCTION        VAdc_LogConversionRequest

  DESCRIPTION     This function logs a conversion request
           
  DEPENDENCIES    None

  PARAMETERS      pVAdcDebugInfo [in] debug structure
                  uChannelIdx    [in] the channel index

  RETURN VALUE    None
                  

  SIDE EFFECTS    None

======================================================================*/
void VAdc_LogConversionRequest(VAdcDebugInfoType *pVAdcDebugInfo, uint32 uChannelIdx)
{
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uTimestamp = timetick_get();
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].eEvent = VADC_LOG_EVENT_CONVERSION_REQUEST;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uChannelIdx = uChannelIdx;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].pszErrorMsg = "";

   MSG_1(MSG_SSID_ADC, MSG_LEGACY_MED, "VADC conv req: index %u", uChannelIdx);

   if (pVAdcDebugInfo->uCurIdx < VADC_LOG_MAX - 1)
   {
      pVAdcDebugInfo->uCurIdx++;
   }
   else
   {
      pVAdcDebugInfo->uCurIdx = 0;
   }
} 

/*======================================================================

  FUNCTION        VAdc_LogRecalibrationRequest

  DESCRIPTION     This function logs a recalibration request
           
  DEPENDENCIES    None

  PARAMETERS      pVAdcDebugInfo [in] debug structure
                  uChannelIdx    [in]

  RETURN VALUE    None
                  
  SIDE EFFECTS    None

======================================================================*/
void VAdc_LogRecalibrationRequest(VAdcDebugInfoType *pVAdcDebugInfo, uint32 uChannelIdx)
{
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uTimestamp = timetick_get();
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].eEvent = VADC_LOG_EVENT_RECALIBRATION_REQUEST;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uChannelIdx = uChannelIdx;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].pszErrorMsg = "";

   MSG_1(MSG_SSID_ADC, MSG_LEGACY_MED, "VADC recalibration req: index %u", uChannelIdx);

   if (pVAdcDebugInfo->uCurIdx < VADC_LOG_MAX - 1)
   {
      pVAdcDebugInfo->uCurIdx++;
   }
   else
   {
      pVAdcDebugInfo->uCurIdx = 0;
   }
}

/*======================================================================

  FUNCTION        VAdc_LogEOC

  DESCRIPTION     This function logs the internal conversion complete callback
           
  DEPENDENCIES    None

  PARAMETERS      pVAdcDebugInfo [in] debug structure

  RETURN VALUE    None
                  
  SIDE EFFECTS    None

======================================================================*/
void VAdc_LogEOC(VAdcDebugInfoType *pVAdcDebugInfo)
{
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uTimestamp = timetick_get();
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].eEvent = VADC_LOG_EVENT_EOC;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uChannelIdx = 0xFFFFFFFF;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].pszErrorMsg = "";

   MSG(MSG_SSID_ADC, MSG_LEGACY_MED, "VADC EOC");

   if (pVAdcDebugInfo->uCurIdx < VADC_LOG_MAX - 1)
   {
      pVAdcDebugInfo->uCurIdx++;
   }
   else
   {
      pVAdcDebugInfo->uCurIdx = 0;
   }
}

/*======================================================================

  FUNCTION        VAdc_LogConversionResult

  DESCRIPTION     This function logs the conversion result
           
  DEPENDENCIES    None

  PARAMETERS      pVAdcDebugInfo [in] debug structure
                  pResult        [in] the AdcDeviceResultType result data
                  pCalibData     [in] calibration data

  RETURN VALUE    None
                  
  SIDE EFFECTS    None

======================================================================*/
void VAdc_LogConversionResult(VAdcDebugInfoType *pVAdcDebugInfo, const AdcDeviceResultType *pResult, const VAdcDebugCalDataType *pCalibData)
{
   DALSYS_memcpy(&pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].result, pResult, sizeof(AdcDeviceResultType));
 
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uTimestamp = timetick_get();
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].eEvent = VADC_LOG_EVENT_RESULT;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uChannelIdx = 0xFFFFFFFF;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].pszErrorMsg = "";

   MSG_6(MSG_SSID_ADC, 
         MSG_LEGACY_HIGH, 
         "VADC conv result: eStatus %u, uChannelIdx %u, nPhysical %u, uPercent %u, uMicrovolts %u, uCode %u", 
         pResult->eStatus,
         pResult->uChannelIdx,
         pResult->nPhysical,
         pResult->uPercent,
         pResult->uMicrovolts,
         pResult->uCode);

   MSG_6(MSG_SSID_ADC, 
         MSG_LEGACY_HIGH, 
         "VADC channel's cal: uVrefP %u, uVrefN %u, uVref1 %u, uVref2 %u, uVref1Percent %u, uVref2Percent %u", 
         pCalibData->uVrefP,
         pCalibData->uVrefN,
         pCalibData->uVref1,
         pCalibData->uVref2,
         pCalibData->uVref1Percent,
         pCalibData->uVref2Percent);
    
   if (pVAdcDebugInfo->uCurIdx < VADC_LOG_MAX - 1)
   {
      pVAdcDebugInfo->uCurIdx++;
   }
   else
   {
      pVAdcDebugInfo->uCurIdx = 0;
   }
}

/*======================================================================

  FUNCTION        VAdc_LogError

  DESCRIPTION     This function logs an error
           
  DEPENDENCIES    None

  PARAMETERS      pVAdcDebugInfo [in] debug structure
                  DeviceId    [in]
                  pszErrorMsg [in]
                  bFatalError [in]

  RETURN VALUE    None
                  
  SIDE EFFECTS    None

======================================================================*/
void VAdc_LogError(VAdcDebugInfoType *pVAdcDebugInfo, DALDEVICEID DeviceId, const char *pszErrorMsg, DALBOOL bFatalError)
{
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uTimestamp = timetick_get();
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].eEvent = VADC_LOG_EVENT_ERROR;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uChannelIdx = 0xFFFFFFFF;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].pszErrorMsg = pszErrorMsg;

   if (bFatalError == TRUE)
   {
      MSG_SPRINTF_1(MSG_SSID_ADC, MSG_LEGACY_FATAL, "%s", pszErrorMsg);

      DALSYS_LogEvent(DeviceId, 
                      DALSYS_LOGEVENT_FATAL_ERROR,
                      pszErrorMsg);
   }
   else
   {
      MSG_SPRINTF_1(MSG_SSID_ADC, MSG_LEGACY_ERROR, "%s", pszErrorMsg);

      DALSYS_LogEvent(DeviceId,
                      DALSYS_LOGEVENT_ERROR,
                      pszErrorMsg);
   }
    
   if (pVAdcDebugInfo->uCurIdx < VADC_LOG_MAX - 1)
   {
      pVAdcDebugInfo->uCurIdx++;
   }
   else
   {
      pVAdcDebugInfo->uCurIdx = 0;
   }
}

/*======================================================================

  FUNCTION        VAdc_LogInterrupt

  DESCRIPTION     This function logs an interrupt
           
  DEPENDENCIES    None

  PARAMETERS      pVAdcDebugInfo [in] debug structure

  RETURN VALUE    None
                  
  SIDE EFFECTS    None

======================================================================*/
void VAdc_LogInterrupt(VAdcDebugInfoType *pVAdcDebugInfo)
{
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uTimestamp = timetick_get();
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].eEvent = VADC_LOG_EVENT_INTERRUPT;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uChannelIdx = 0xFFFFFFFF;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].pszErrorMsg = "";
    
   MSG(MSG_SSID_ADC, MSG_LEGACY_MED, "VADC EOC Interrupt");

   if (pVAdcDebugInfo->uCurIdx < VADC_LOG_MAX - 1)
   {
      pVAdcDebugInfo->uCurIdx++;
   }
   else
   {
      pVAdcDebugInfo->uCurIdx = 0;
   }
}

/*======================================================================

  FUNCTION        VAdc_LogDumpRegisters

  DESCRIPTION     This function dumps the VADC registers
           
  DEPENDENCIES    None

  PARAMETERS      pVAdcDebugInfo [in] debug structure
                  piVAdcHal      [in]

  RETURN VALUE    None
                  
  SIDE EFFECTS    This function causes SPMI transfers to occur

======================================================================*/
void VAdc_LogDumpRegisters(VAdcDebugInfoType *pVAdcDebugInfo, VAdcHalInterfaceType *piVAdcHal)
{
   VAdcHalRegDumpType *pVAdcRegDump;
   uint32 uReg;

   pVAdcRegDump = &pVAdcDebugInfo->aRegDump[pVAdcDebugInfo->uRegDumpCurIdx];

   (void)VAdcHalDumpRegisters(piVAdcHal, pVAdcRegDump);

   MSG(MSG_SSID_ADC, MSG_LEGACY_ERROR, "VADC register dump");

   for (uReg = 0; uReg < VADC_HAL_NUM_REGISTERS_TO_DUMP; uReg++)
   {
      MSG_2(MSG_SSID_ADC, 
            MSG_LEGACY_ERROR, 
            "Offset = %u, Contents = %u",
            pVAdcRegDump->aVAdcReg[uReg].u8Offset,
            pVAdcRegDump->aVAdcReg[uReg].u8Val);
   }

#ifdef VADC_DUMP_REGISTERS_AFTER_READS   
   if (pVAdcDebugInfo->uRegDumpCurIdx < VADC_REG_DUMP_MAX - 1)
   {
      pVAdcDebugInfo->uRegDumpCurIdx++;
   }
   else
   {
      pVAdcDebugInfo->uRegDumpCurIdx = 0;
   }
#endif
}

/*======================================================================

  FUNCTION        VAdc_LogInit

  DESCRIPTION     This function initializes logging
           
  DEPENDENCIES    None

  PARAMETERS      pVAdcDebugInfo [in] debug structure

  RETURN VALUE    None
                  
  SIDE EFFECTS    None
 
======================================================================*/
void VAdc_LogInit(VAdcDebugInfoType *pVAdcDebugInfo)
{
   pVAdcDebugInfo->uCurIdx = 0;
   pVAdcDebugInfo->uRegDumpCurIdx = 0;
}

#endif /* !VADC_LOG_DISABLED */

