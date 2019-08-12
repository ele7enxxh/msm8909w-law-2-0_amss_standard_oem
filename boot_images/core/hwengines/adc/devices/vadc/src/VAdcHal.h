#ifndef VADCHAL_H
#define VADCHAL_H
/*============================================================================
  @file VAdcHal.h

  Function and data structure declarations for VADC HAL


                Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/* $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/adc/devices/vadc/src/VAdcHal.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "com_dtypes.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define VADC_HAL_NUM_REGISTERS_TO_DUMP 35

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef struct
{
   uint8 uDigitalMajor;
   uint8 uDigitalMinor;
   uint8 uAnalogMajor;
   uint8 uAnalogMinor;
   uint8 uType;
   uint8 uSubType;
} VAdcRevisionInfoType;

typedef enum
{
   VADC_OP_MODE_NORMAL = 0,
   VADC_OP_MODE_SEQUENCER,
   VADC_OP_MODE_INTERVAL
} VAdcOpModeType;

typedef enum
{
   VADC_INTERVAL_MODE_SINGLE = 0,
   VADC_INTERVAL_MODE_CONTINUOUS
} VAdcIntervalModeType;

typedef enum
{
   VADC_CONVERSION_STATUS_INVALID = 0,
   VADC_CONVERSION_STATUS_COMPLETE,
   VADC_CONVERSION_STATUS_PENDING,
   VADC_CONVERSION_STATUS_WAITING
} VAdcConversionStatusType;

typedef enum
{
   VADC_SEQUENCER_STATUS_IDLE = 0,
   VADC_SEQUENCER_STATUS_WAITING_FOR_TRIGGER,
   VADC_SEQUENCER_STATUS_WAITING_FOR_HOLDOFF,
   VADC_SEQUENCER_STATUS_WAITING_FOR_EOC
} VAdcSequencerStatusType;

typedef struct
{
   VAdcOpModeType eOpMode;
   VAdcIntervalModeType eIntervalMode;
   VAdcConversionStatusType eConversionStatus;
   VAdcSequencerStatusType eSequencerStatus;
   boolean bSequencerFifoNotEmptyErrorOccurred;
   boolean bSequencerTimeoutErrorOccurred;
} VAdcStatusType;

typedef enum
{
   VADC_DISABLE = 0,
   VADC_ENABLE
} VAdcEnableType;

typedef enum
{
   VADC_DECIMATION_RATIO_512 = 0,
   VADC_DECIMATION_RATIO_1024,
   VADC_DECIMATION_RATIO_2048,
   VADC_DECIMATION_RATIO_4096
} VAdcDecimationRatioType;

typedef enum
{
   VADC_CLOCK_SELECT_2P4_MHZ = 0,
   VADC_CLOCK_SELECT_4P8_MHZ
   /*
    * Note: Although register values exist for the 9.6 MHZ and 19.2 MHZ clock configurations,
    * these clock modes are not supported by the VADC HW.
    */
} VAdcClockSelectType;

typedef uint32 VAdcAmuxChannelSelectType;

typedef enum
{
   VADC_SETTLING_DELAY_0_US = 0,
   VADC_SETTLING_DELAY_100_US,
   VADC_SETTLING_DELAY_200_US,
   VADC_SETTLING_DELAY_300_US,
   VADC_SETTLING_DELAY_400_US,
   VADC_SETTLING_DELAY_500_US,
   VADC_SETTLING_DELAY_600_US,
   VADC_SETTLING_DELAY_700_US,
   VADC_SETTLING_DELAY_800_US,
   VADC_SETTLING_DELAY_900_US,
   VADC_SETTLING_DELAY_1_MS,
   VADC_SETTLING_DELAY_2_MS,
   VADC_SETTLING_DELAY_4_MS,
   VADC_SETTLING_DELAY_6_MS,
   VADC_SETTLING_DELAY_8_MS,
   VADC_SETTLING_DELAY_10_MS
} VAdcSettlingDelay;

typedef uint32 VAdcConversionCodeType;

typedef enum
{
   VADC_SEQUENCER_HOLDOFF_25_US = 0,
   VADC_SEQUENCER_HOLDOFF_50_US,
   VADC_SEQUENCER_HOLDOFF_75_US,
   VADC_SEQUENCER_HOLDOFF_100_US,
   VADC_SEQUENCER_HOLDOFF_125_US,
   VADC_SEQUENCER_HOLDOFF_150_US,
   VADC_SEQUENCER_HOLDOFF_175_US,
   VADC_SEQUENCER_HOLDOFF_200_US,
   VADC_SEQUENCER_HOLDOFF_225_US,
   VADC_SEQUENCER_HOLDOFF_250_US,
   VADC_SEQUENCER_HOLDOFF_275_US,
   VADC_SEQUENCER_HOLDOFF_300_US,
   VADC_SEQUENCER_HOLDOFF_325_US,
   VADC_SEQUENCER_HOLDOFF_350_US,
   VADC_SEQUENCER_HOLDOFF_375_US,
   VADC_SEQUENCER_HOLDOFF_400_US,
} VAdcSequencerHoldoffType;

typedef enum
{
   VADC_SEQUENCER_TIMEOUT_TIME_0_MS = 0,
   VADC_SEQUENCER_TIMEOUT_TIME_1_MS,
   VADC_SEQUENCER_TIMEOUT_TIME_2_MS,
   VADC_SEQUENCER_TIMEOUT_TIME_3_MS,
   VADC_SEQUENCER_TIMEOUT_TIME_4_MS,
   VADC_SEQUENCER_TIMEOUT_TIME_5_MS,
   VADC_SEQUENCER_TIMEOUT_TIME_6_MS,
   VADC_SEQUENCER_TIMEOUT_TIME_7_MS,
   VADC_SEQUENCER_TIMEOUT_TIME_8_MS,
   VADC_SEQUENCER_TIMEOUT_TIME_9_MS,
   VADC_SEQUENCER_TIMEOUT_TIME_10_MS,
   VADC_SEQUENCER_TIMEOUT_TIME_11_MS,
   VADC_SEQUENCER_TIMEOUT_TIME_12_MS,
   VADC_SEQUENCER_TIMEOUT_TIME_13_MS,
   VADC_SEQUENCER_TIMEOUT_TIME_14_MS,
   VADC_SEQUENCER_TIMEOUT_TIME_15_MS
} VAdcSequencerTimeoutTimeType;

typedef enum
{
   VADC_TRIGGER_CONDITION_FALLING_EDGE = 0,
   VADC_TRIGGER_CONDITION_RISING_EDGE
} VAdcTriggerConditionType;

typedef enum
{
   VADC_TRIGGER_INPUT_TRIG_1 = 0,
   VADC_TRIGGER_INPUT_TRIG_2,
   VADC_TRIGGER_INPUT_TRIG_3,
   VADC_TRIGGER_INPUT_TRIG_4
} VAdcTriggerInputType;

typedef enum
{
   VADC_FAST_AVERAGE_1_SAMPLE = 0,
   VADC_FAST_AVERAGE_2_SAMPLES,
   VADC_FAST_AVERAGE_4_SAMPLES,
   VADC_FAST_AVERAGE_8_SAMPLES,
   VADC_FAST_AVERAGE_16_SAMPLES,
   VADC_FAST_AVERAGE_32_SAMPLES,
   VADC_FAST_AVERAGE_64_SAMPLES,
   VADC_FAST_AVERAGE_128_SAMPLES,
   VADC_FAST_AVERAGE_256_SAMPLES,
   VADC_FAST_AVERAGE_512_SAMPLES,
   VADC_FAST_AVERAGE_NONE = 0xFFFF
} VAdcFastAverageModeType;

typedef struct
{
   VAdcAmuxChannelSelectType uChannel;
   VAdcDecimationRatioType eDecimationRatio;
   VAdcClockSelectType eClockSelect;
   VAdcSettlingDelay eSettlingDelay;
   VAdcFastAverageModeType eFastAverageMode;
} VAdcConversionParametersType;

typedef struct
{
   VAdcSequencerHoldoffType eHoldoff;
   VAdcSequencerTimeoutTimeType eTimeoutTime;
   VAdcTriggerConditionType eTriggerCondition;
   VAdcTriggerInputType eTriggerInput;
} VAdcSequencerParametersType;

typedef enum
{
   VADC_INTERRUPT_CONFIG_NONE = 0,
   VADC_INTERRUPT_CONFIG_LEVEL_HIGH,
   VADC_INTERRUPT_CONFIG_LEVEL_LOW,
   VADC_INTERRUPT_CONFIG_RISING_EDGE,
   VADC_INTERRUPT_CONFIG_FALLING_EDGE,
   VADC_INTERRUPT_CONFIG_RISING_OR_FALLING_EDGE,
} VAdcInterruptConfigType;

typedef struct
{
   VAdcInterruptConfigType eEocInterruptConfig;
   VAdcInterruptConfigType eFifoNotEmptyInterruptConfig;
   VAdcInterruptConfigType eSequencerTimeoutInterruptConfig;
   VAdcInterruptConfigType eHighThresholdInterruptConfig;
   VAdcInterruptConfigType eLowThresholdInterruptConfig;
} VAdcInterruptsConfigType;

typedef enum
{
   VADC_INTERRUPT_MASK_BIT_EOC = 0x1,
   VADC_INTERRUPT_MASK_BIT_FIFO_NOT_EMPTY = 0x2,
   VADC_INTERRUPT_MASK_BIT_SEQUENCER_TIMEOUT = 0x4,
   VADC_INTERRUPT_MASK_BIT_HIGH_THRESHOLD = 0x8,
   VADC_INTERRUPT_MASK_BIT_LOW_THRESHOLD = 0x10,
} VAdcInterruptMaskBitsType;

typedef uint32 VAdcInterruptMaskType;

typedef uint32 VAdcInterruptMid;

typedef struct
{
   uint8 u8Offset;
   uint8 u8Val;
} VAdcHalRegType;

typedef struct
{
   VAdcHalRegType aVAdcReg[VADC_HAL_NUM_REGISTERS_TO_DUMP];
} VAdcHalRegDumpType;

typedef uint32 VAdcFabIDType;

typedef enum
{
   VADC_HAL_SUCCESS = 0,
   VADC_HAL_ERROR = 1
} VAdcHalResultType;

typedef struct
{
   void *pCtxt;
   VAdcHalResultType(* pfnWriteBytes)(void *pCtxt, uint32 uRegisterAddress, unsigned char* pucData, uint32 uDataLen);
   VAdcHalResultType(* pfnReadBytes)(void *pCtxt, uint32 uRegisterAddress, unsigned char* pucData, uint32 uDataLen, uint32* puTotalBytesRead);
} VAdcHalInterfaceType;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
VAdcHalResultType VAdcHalGetRevisionInfo(VAdcHalInterfaceType *piVAdcHal, VAdcRevisionInfoType *pRevisionInfo);
VAdcHalResultType VAdcHalGetStatus(VAdcHalInterfaceType *piVAdcHal, VAdcStatusType *pStatus);
VAdcHalResultType VAdcHalConfigInterrupts(VAdcHalInterfaceType *piVAdcHal, VAdcInterruptsConfigType *pConfigs);
VAdcHalResultType VAdcHalGetPendingInterrupts(VAdcHalInterfaceType *piVAdcHal, VAdcInterruptMaskType *puIntrMask);
VAdcHalResultType VAdcHalClearInterrupts(VAdcHalInterfaceType *piVAdcHal, VAdcInterruptMaskType uIntrMask);
VAdcHalResultType VAdcHalEnableInterrupts(VAdcHalInterfaceType *piVAdcHal, VAdcInterruptMaskType uIntrMask);
VAdcHalResultType VAdcHalSetInterruptMid(VAdcHalInterfaceType *piVAdcHal, VAdcInterruptMid uIntrMid);
VAdcHalResultType VAdcHalSetOpMode(VAdcHalInterfaceType *piVAdcHal, VAdcOpModeType eOpMode);
VAdcHalResultType VAdcHalSetEnable(VAdcHalInterfaceType *piVAdcHal, VAdcEnableType eEnable);
VAdcHalResultType VAdcHalSetConversionParameters(VAdcHalInterfaceType *piVAdcHal, VAdcConversionParametersType *pParams);
VAdcHalResultType VAdcHalSetSequencerParameters(VAdcHalInterfaceType *piVAdcHal, VAdcSequencerParametersType *pParams);
VAdcHalResultType VAdcHalRequestConversion(VAdcHalInterfaceType *piVAdcHal);
VAdcHalResultType VAdcHalGetConversionCode(VAdcHalInterfaceType *piVAdcHal, VAdcConversionCodeType *puCode);
VAdcHalResultType VAdcHalDumpRegisters(VAdcHalInterfaceType *piVAdcHal, VAdcHalRegDumpType *pVAdcRegDump);
VAdcHalResultType VAdcHalStoreFabID(VAdcHalInterfaceType *piVAdcHal, VAdcFabIDType uFabID);
VAdcHalResultType VAdcHalGetFabID(VAdcHalInterfaceType *piVAdcHal, VAdcFabIDType *puFabID);

#endif /* #ifndef VADCHAL_H */

