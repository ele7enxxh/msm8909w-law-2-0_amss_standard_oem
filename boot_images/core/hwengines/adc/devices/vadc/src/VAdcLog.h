#ifndef VADCLOG_H
#define VADCLOG_H
/*============================================================================
  @file VAdcLog.h

  Logging for the VAdc


                Copyright (c) 2012-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/* $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/adc/devices/vadc/src/VAdcLog.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DDIAdcDevice.h"
#include "VAdcHal.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define VADC_LOG_DISABLED

#ifdef VADC_LOG_DISABLED

#define VAdc_LogInit(x)
#define VAdc_LogConversionRequest(x,y)
#define VAdc_LogRecalibrationRequest(x,y)
#define VAdc_LogEOC(x)
#define VAdc_LogConversionResult(x,y)
#define VAdc_LogError(w,x,y,z)
#define VAdc_LogInterrupt(x)
#define VAdc_LogDumpRegisters(x,y)

#endif

#ifndef VADC_LOG_DISABLED

#define VADC_LOG_MAX 30

#ifdef VADC_DUMP_REGISTERS_AFTER_READS
#define VADC_REG_DUMP_MAX 5
#else
#define VADC_REG_DUMP_MAX 1
#endif

#endif

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
#ifndef VADC_LOG_DISABLED

typedef struct
{
   enum
   {
      VADC_LOG_EVENT_CONVERSION_REQUEST = 0,
      VADC_LOG_EVENT_RECALIBRATION_REQUEST,
      VADC_LOG_EVENT_INTERRUPT,
      VADC_LOG_EVENT_EOC,
      VADC_LOG_EVENT_RESULT,
      VADC_LOG_EVENT_ERROR
   } eEvent;
   uint64 uTimestamp;
   uint32 uChannelIdx;
   AdcDeviceResultType result;
   const char *pszErrorMsg;
} VAdcRegDataLogEntryType;

typedef struct
{
   DalDeviceHandle *phTimetickDev;
   VAdcRegDataLogEntryType aLog[VADC_LOG_MAX];
   uint32 uCurIdx;
   VAdcHalRegDumpType aRegDump[VADC_REG_DUMP_MAX];
   uint32 uRegDumpCurIdx;
} VAdcDebugInfoType;

typedef struct
{
   uint32 uVrefP;
   uint32 uVrefN;
   uint32 uVref1;
   uint32 uVref2;
   uint32 uVref1Percent;
   uint32 uVref2Percent;
} VAdcDebugCalDataType;

#else

typedef void *VAdcDebugInfoType;

#endif

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
#ifndef VADC_LOG_DISABLED

DALResult VAdc_LogInit(VAdcDebugInfoType *pVAdcDebugInfo);
void VAdc_LogConversionRequest(VAdcDebugInfoType *pVAdcDebugInfo, uint32 uChannelIdx);
void VAdc_LogRecalibrationRequest(VAdcDebugInfoType *pVAdcDebugInfo, uint32 uChannelIdx);
void VAdc_LogEOC(VAdcDebugInfoType *pVAdcDebugInfo);
void VAdc_LogConversionResult(VAdcDebugInfoType *pVAdcDebugInfo, const AdcDeviceResultType *pResult, const VAdcDebugCalDataType *pCalibData);
void VAdc_LogError(VAdcDebugInfoType *pVAdcDebugInfo, DALDEVICEID DeviceId, const char *pszErrorMsg, DALBOOL bFatalError);
void VAdc_LogInterrupt(VAdcDebugInfoType *pVAdcDebugInfo);
void VAdc_LogDumpRegisters(VAdcDebugInfoType *pVAdcDebugInfo, VAdcHalInterfaceType *piVAdcHal);

#endif

#endif /* #ifndef VADCLOG_H */

