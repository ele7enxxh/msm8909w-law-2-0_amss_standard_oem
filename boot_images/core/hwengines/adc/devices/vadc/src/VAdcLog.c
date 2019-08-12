/*============================================================================
  FILE:         VAdcLog.c

  OVERVIEW:     Logging for the VAdc

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

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/adc/devices/vadc/src/VAdcLog.c#1 $$DateTime: 2015/09/01 00:30:35 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-07-07  jjo  Use DDITimetick.
  2013-06-19  jjo  Support for multiple VADCs.
  2012-07-09  jjo  Initial version.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "VAdcLog.h"

#ifndef VADC_LOG_DISABLED

#include "DALFramework.h"
#include "DDITimetick.h"

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
static uint64 VAdc_GetTimetick(const VAdcDebugInfoType *pVAdcDebugInfo)
{
   DALResult result;
   DalTimetickTime64Type u64Tick;

   result = DalTimetick_GetTimetick64(pVAdcDebugInfo->phTimetickDev, &u64Tick);
   if (result != DAL_SUCCESS)
   {
      return 0;
   }

   return (uint64)u64Tick;
}

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
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uTimestamp = VAdc_GetTimetick(pVAdcDebugInfo);
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].eEvent = VADC_LOG_EVENT_CONVERSION_REQUEST;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uChannelIdx = uChannelIdx;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].pszErrorMsg = "";

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
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uTimestamp = VAdc_GetTimetick(pVAdcDebugInfo);
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].eEvent = VADC_LOG_EVENT_RECALIBRATION_REQUEST;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uChannelIdx = uChannelIdx;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].pszErrorMsg = "";

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
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uTimestamp = VAdc_GetTimetick(pVAdcDebugInfo);
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].eEvent = VADC_LOG_EVENT_EOC;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uChannelIdx = 0xFFFFFFFF;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].pszErrorMsg = "";

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

   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uTimestamp = VAdc_GetTimetick(pVAdcDebugInfo);
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].eEvent = VADC_LOG_EVENT_RESULT;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uChannelIdx = 0xFFFFFFFF;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].pszErrorMsg = "";

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
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uTimestamp = VAdc_GetTimetick(pVAdcDebugInfo);
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].eEvent = VADC_LOG_EVENT_ERROR;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uChannelIdx = 0xFFFFFFFF;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].pszErrorMsg = pszErrorMsg;

   if (bFatalError == TRUE)
   {
      DALSYS_LogEvent(DeviceId,
                      DALSYS_LOGEVENT_FATAL_ERROR,
                      pszErrorMsg);
   }
   else
   {
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
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uTimestamp = VAdc_GetTimetick(pVAdcDebugInfo);
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].eEvent = VADC_LOG_EVENT_INTERRUPT;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].uChannelIdx = 0xFFFFFFFF;
   pVAdcDebugInfo->aLog[pVAdcDebugInfo->uCurIdx].pszErrorMsg = "";

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

   pVAdcRegDump = &pVAdcDebugInfo->aRegDump[pVAdcDebugInfo->uRegDumpCurIdx];

   (void)VAdcHalDumpRegisters(piVAdcHal, pVAdcRegDump);

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

  RETURN VALUE    DAL_SUCCESS or an error

  SIDE EFFECTS    None

======================================================================*/
DALResult VAdc_LogInit(VAdcDebugInfoType *pVAdcDebugInfo)
{
   DALResult result;
   pVAdcDebugInfo->uCurIdx = 0;
   pVAdcDebugInfo->uRegDumpCurIdx = 0;

   result = DalTimetick_Attach("SystemTimer", &pVAdcDebugInfo->phTimetickDev);
   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(DALDEVICEID_XOADC,
                      DALSYS_LOGEVENT_FATAL_ERROR,
                      "VADC: Failed to attach to Timetick device");
      return result;
   }

   result = DalTimetick_InitTimetick64(pVAdcDebugInfo->phTimetickDev);
   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(DALDEVICEID_XOADC,
                      DALSYS_LOGEVENT_FATAL_ERROR,
                      "VADC: Failed to init Timetick device");
      DAL_DeviceDetach(pVAdcDebugInfo->phTimetickDev);
      return result;
   }

   return DAL_SUCCESS;
}

#endif /* !VADC_LOG_DISABLED */

