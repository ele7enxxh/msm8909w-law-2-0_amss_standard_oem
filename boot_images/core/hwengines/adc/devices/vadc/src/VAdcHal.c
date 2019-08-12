/*============================================================================
  FILE:         VAdcHal.c

  OVERVIEW:     Implementation of device HAL for VADC

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

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/adc/devices/vadc/src/VAdcHal.c#1 $$DateTime: 2015/09/01 00:30:35 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-09-15  jjo  Reduce number of SPMI transactions
  2013-04-17  jjo  Added PMIC fab ID get / store
  2012-08-23  jjo  Added fast average mode
  2012-07-10  jjo  Added register dump feature
  2012-05-16  jjo  Initial revision

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "VAdcHal.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define ARRAY_LENGTH(a) \
   (sizeof(a) / sizeof(a[0]))

#define READ_FIELD(reg_val, reg, field) \
   ((reg_val & reg##_##field##_BMSK) >> reg##_##field##_SHFT)

#define READ_REG(reg_val, reg) \
   (reg_val & reg##_RMSK)

#define WRITE_FIELD(reg_val, reg, field, val) \
   ((reg_val & ~reg##_##field##_BMSK) | ((val << reg##_##field##_SHFT) & reg##_##field##_BMSK))

#define WRITE_REG(reg_val, reg, val) \
   ((reg_val & ~reg##_RMSK) | (val & reg##_RMSK))

#define VADC_REVISION1_ADDR                                                  0
#define VADC_REVISION1_RMSK                                               0xFF
#define VADC_REVISION1_DIG_MINOR_BMSK                                     0xFF
#define VADC_REVISION1_DIG_MINOR_SHFT                                        0

#define VADC_REVISION2_ADDR                                                  1
#define VADC_REVISION2_RMSK                                               0xFF
#define VADC_REVISION1_DIG_MAJOR_BMSK                                     0xFF
#define VADC_REVISION1_DIG_MAJOR_SHFT                                        0

#define VADC_REVISION3_ADDR                                                  2
#define VADC_REVISION3_RMSK                                               0xFF
#define VADC_REVISION1_ANA_MINOR_BMSK                                     0xFF
#define VADC_REVISION1_ANA_MINOR_SHFT                                        0

#define VADC_REVISION4_ADDR                                                  3
#define VADC_REVISION4_RMSK                                               0xFF
#define VADC_REVISION1_ANA_MAJOR_BMSK                                     0xFF
#define VADC_REVISION1_ANA_MAJOR_SHFT                                        0

#define VADC_PERPH_TYPE_ADDR                                                 4
#define VADC_PERPH_TYPE_RMSK                                              0xFF
#define VADC_PERPH_TYPE_TYPE_BMSK                                         0xFF
#define VADC_PERPH_TYPE_TYPE_SHFT                                            0

#define VADC_PERPH_SUBTYPE_ADDR                                              5
#define VADC_PERPH_SUBTYPE_RMSK                                           0xFF
#define VADC_PERPH_SUBTYPE_SUBTYPE_BMSK                                   0xFF
#define VADC_PERPH_SUBTYPE_SUBTYPE_SHFT                                      0

#define VADC_STATUS1_ADDR                                                    8
#define VADC_STATUS1_RMSK                                                 0x1F
#define VADC_STATUS1_OP_MODE_BMSK                                         0x18
#define VADC_STATUS1_OP_MODE_SHFT                                            3
#define VADC_STATUS1_MEAS_INTERVAL_EN_STS_BMSK                            0x04
#define VADC_STATUS1_MEAS_INTERVAL_EN_STS_SHFT                               2
#define VADC_STATUS1_REQ_STS_AND_EOC_BMSK                                 0x03
#define VADC_STATUS1_REQ_STS_AND_EOC_SHFT                                    0

#define VADC_STATUS2_ADDR                                                    9
#define VADC_STATUS2_RMSK                                                 0xFB
#define VADC_STATUS2_CONV_SEQ_STATE_BMSK                                  0xF8
#define VADC_STATUS2_CONV_SEQ_STATE_SHFT                                     3
#define VADC_STATUS2_FIFO_NOT_EMPTY_FLAG_BMSK                             0x02
#define VADC_STATUS2_FIFO_NOT_EMPTY_FLAG_SHFT                                1
#define VADC_STATUS2_CONV_SEQ_TIMEOUT_STS_BMSK                            0x01
#define VADC_STATUS2_CONV_SEQ_TIMEOUT_STS_SHFT                               0

#define VADC_INT_RT_STS_ADDR                                              0x10

#define VADC_INT_SET_TYPE_ADDR                                            0x11
#define VADC_INT_SET_TYPE_RMSK                                            0x1F
#define VADC_INT_SET_TYPE_EOC_BMSK                                        0x01
#define VADC_INT_SET_TYPE_EOC_SHFT                                           0

#define VADC_INT_POLARITY_HIGH_ADDR                                       0x12
#define VADC_INT_POLARITY_HIGH_RMSK                                       0x1F
#define VADC_INT_POLARITY_HIGH_EOC_BMSK                                   0x01
#define VADC_INT_POLARITY_HIGH_EOC_SHFT                                      0

#define VADC_INT_POLARITY_LOW_ADDR                                        0x13
#define VADC_INT_POLARITY_LOW_RMSK                                        0x1F
#define VADC_INT_POLARITY_LOW_EOC_BMSK                                    0x01
#define VADC_INT_POLARITY_LOW_EOC_SHFT                                       0

#define VADC_INT_LATCHED_CLR_ADDR                                         0x14
#define VADC_INT_LATCHED_CLR_RMSK                                         0x1F

#define VADC_INT_EN_SET_ADDR                                              0x15
#define VADC_INT_EN_SET_RMSK                                              0x1F

#define VADC_INT_EN_CLR_ADDR                                              0x16

#define VADC_INT_LATCHED_STS_ADDR                                         0x18

#define VADC_INT_PENDING_STS_ADDR                                         0x19
#define VADC_INT_PENDING_STS_RMSK                                         0x1F

#define VADC_INT_MID_SEL_ADDR                                             0x1A
#define VADC_INT_MID_SEL_RMSK                                             0x03
#define VADC_INT_MID_SEL_INT_MID_SEL_BMSK                                 0x03
#define VADC_INT_MID_SEL_INT_MID_SEL_SHFT                                    0

#define VADC_INT_PRIORITY_ADDR                                            0x1B

#define VADC_MODE_CTL_ADDR                                                0x40
#define VADC_MODE_CTL_RMSK                                                0x1F
#define VADC_MODE_CTL_OP_MODE_BMSK                                        0x18
#define VADC_MODE_CTL_OP_MODE_SHFT                                           3

#define VADC_EN_CTL1_ADDR                                                 0x46
#define VADC_EN_CTL1_RMSK                                                 0x80
#define VADC_EN_CTL1_ADC_EN_BMSK                                          0x80
#define VADC_EN_CTL1_ADC_EN_SHFT                                             7

#define VADC_ADC_CH_SEL_CTL_ADDR                                          0x48
#define VADC_ADC_CH_SEL_CTL_RMSK                                          0xFF
#define VADC_ADC_CH_SEL_CTL_ADC_CH_SEL_BMSK                               0xFF
#define VADC_ADC_CH_SEL_CTL_ADC_CH_SEL_SHFT                                  0

#define VADC_ADC_DIG_PARAM_ADDR                                           0x50
#define VADC_ADC_DIG_PARAM_RMSK                                           0x0F
#define VADC_ADC_DIG_PARAM_DEC_RATIO_SEL_BMSK                             0x0C
#define VADC_ADC_DIG_PARAM_DEC_RATIO_SEL_SHFT                                2
#define VADC_ADC_DIG_PARAM_CLK_SEL_BMSK                                   0x03
#define VADC_ADC_DIG_PARAM_CLK_SEL_SHFT                                      0

#define VADC_HW_SETTLE_DELAY_ADDR                                         0x51
#define VADC_HW_SETTLE_DELAY_RMSK                                         0x0F
#define VADC_HW_SETTLE_DELAY_HW_SETTLE_DELAY_BMSK                         0x0F
#define VADC_HW_SETTLE_DELAY_HW_SETTLE_DELAY_SHFT                            0

#define VADC_CONV_REQ_ADDR                                                0x52
#define VADC_CONV_REQ_REQ_BMSK                                            0x80
#define VADC_CONV_REQ_REQ_SHFT                                               7

#define VADC_CONV_SEQ_CTL_ADDR                                            0x54
#define VADC_CONV_SEQ_CTL_RMSK                                            0xFF
#define VADC_CONV_SEQ_CTL_CONV_SEQ_HOLDOFF_BMSK                           0xF0
#define VADC_CONV_SEQ_CTL_CONV_SEQ_HOLDOFF_SHFT                              4
#define VADC_CONV_SEQ_CTL_CONV_SEQ_TIMEOUT_BMSK                           0x0F
#define VADC_CONV_SEQ_CTL_CONV_SEQ_TIMEOUT_SHFT                              0

#define VADC_CONV_SEQ_TRIG_CTL_ADDR                                       0x55
#define VADC_CONV_SEQ_TRIG_CTL_RMSK                                       0x83
#define VADC_CONV_SEQ_TRIG_CTL_CONV_SEQ_TRIG_COND_BMSK                    0x80
#define VADC_CONV_SEQ_TRIG_CTL_CONV_SEQ_TRIG_COND_SHFT                       7
#define VADC_CONV_SEQ_TRIG_CTL_CONV_SEQ_TRIG_SEL_BMSK                     0x03
#define VADC_CONV_SEQ_TRIG_CTL_CONV_SEQ_TRIG_SEL_SHFT                        0

#define VADC_MEAS_INTERVAL_CTL_ADDR                                       0x57
#define VADC_MEAS_INTERVAL_OP_CTL_ADDR                                    0x59

#define VADC_FAST_AVG_CTL_ADDR                                            0x5A
#define VADC_FAST_AVG_CTL_RMSK                                            0x0F
#define VADC_FAST_AVG_CTL_FAST_AVG_SAMPLES_BMSK                           0x0F
#define VADC_FAST_AVG_CTL_FAST_AVG_SAMPLES_SHFT                              0

#define VADC_FAST_AVG_EN_ADDR                                             0x5B
#define VADC_FAST_AVG_EN_RMSK                                             0x80
#define VADC_FAST_AVG_EN_FAST_AVG_EN_BMSK                                 0x80
#define VADC_FAST_AVG_EN_FAST_AVG_EN_SHFT                                    7

#define VADC_LOW_THR0_ADDR                                                0x5C
#define VADC_LOW_THR0_RMSK                                                0xFF
#define VADC_LOW_THR0_LOW_THR_7_0_BMSK                                    0xFF
#define VADC_LOW_THR0_LOW_THR_7_0_SHFT                                       0

#define VADC_LOW_THR1_ADDR                                                0x5D
#define VADC_LOW_THR1_RMSK                                                0xFF
#define VADC_LOW_THR1_LOW_THR_15_8_BMSK                                   0xFF
#define VADC_LOW_THR1_LOW_THR_15_8_SHFT                                      0

#define VADC_HIGH_THR0_ADDR                                               0x5E
#define VADC_HIGH_THR0_RMSK                                               0xFF
#define VADC_HIGH_THR0_HIGH_THR_7_0_BMSK                                  0xFF
#define VADC_HIGH_THR0_HIGH_THR_7_0_SHFT                                     0

#define VADC_HIGH_THR1_ADDR                                               0x5F
#define VADC_HIGH_THR1_RMSK                                               0xFF
#define VADC_HIGH_THR1_HIGH_THR_15_8_BMSK                                 0xFF
#define VADC_HIGH_THR1_HIGH_THR_15_8_SHFT                                    0

#define VADC_DATA0_ADDR                                                   0x60
#define VADC_DATA0_RMSK                                                   0xFF
#define VADC_DATA0_DATA_7_0_BMSK                                          0xFF
#define VADC_DATA0_DATA_7_0_SHFT                                             0

#define VADC_DATA1_ADDR                                                   0x61
#define VADC_DATA1_RMSK                                                   0xFF
#define VADC_DATA1_DATA_7_0_BMSK                                          0xFF
#define VADC_DATA1_DATA_15_8_SHFT                                            0

#define VADC_SEC_ACCESS_ADDR                                              0xD0
#define VADC_SEC_ACCESS_SEC_UNLOCK_BMSK                                   0xFF
#define VADC_SEC_ACCESS_SEC_UNLOCK_SHFT                                      0

#define VADC_INT_TEST_VAL_ADDR                                            0xE1
#define VADC_INT_TEST_VAL_FAB_ID_BMSK                                     0xFF
#define VADC_INT_TEST_VAL_FAB_ID_SHFT                                        0

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
static const uint8 vAdcHalRegDumpAddresses[VADC_HAL_NUM_REGISTERS_TO_DUMP] = {
   VADC_REVISION1_ADDR,
   VADC_REVISION2_ADDR,
   VADC_REVISION3_ADDR,
   VADC_REVISION4_ADDR,
   VADC_PERPH_TYPE_ADDR,
   VADC_PERPH_SUBTYPE_ADDR,
   VADC_STATUS1_ADDR,
   VADC_STATUS2_ADDR,
   VADC_INT_RT_STS_ADDR,
   VADC_INT_SET_TYPE_ADDR,
   VADC_INT_POLARITY_HIGH_ADDR,
   VADC_INT_POLARITY_LOW_ADDR,
   VADC_INT_EN_SET_ADDR,
   VADC_INT_EN_CLR_ADDR,
   VADC_INT_LATCHED_STS_ADDR,
   VADC_INT_PENDING_STS_ADDR,
   VADC_INT_MID_SEL_ADDR,
   VADC_INT_PRIORITY_ADDR,
   VADC_MODE_CTL_ADDR,
   VADC_EN_CTL1_ADDR,
   VADC_ADC_CH_SEL_CTL_ADDR,
   VADC_ADC_DIG_PARAM_ADDR,
   VADC_HW_SETTLE_DELAY_ADDR,
   VADC_CONV_SEQ_CTL_ADDR,
   VADC_CONV_SEQ_TRIG_CTL_ADDR,
   VADC_MEAS_INTERVAL_CTL_ADDR,
   VADC_MEAS_INTERVAL_OP_CTL_ADDR,
   VADC_FAST_AVG_CTL_ADDR,
   VADC_FAST_AVG_EN_ADDR,
   VADC_LOW_THR0_ADDR,
   VADC_LOW_THR1_ADDR,
   VADC_HIGH_THR0_ADDR,
   VADC_HIGH_THR1_ADDR,
   VADC_DATA0_ADDR,
   VADC_DATA1_ADDR
};

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static VAdcHalResultType VAdcHalUnlockSecReg(VAdcHalInterfaceType *piVAdcHal)
{
   VAdcHalResultType result;
   uint8 aucData[1];
   uint32 uOffset = VADC_SEC_ACCESS_ADDR;

   aucData[0] = 0xA5;

   result = piVAdcHal->pfnWriteBytes(piVAdcHal->pCtxt,
                                     uOffset,
                                     aucData,
                                     ARRAY_LENGTH(aucData));

   return result;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
VAdcHalResultType VAdcHalGetRevisionInfo(VAdcHalInterfaceType *piVAdcHal, VAdcRevisionInfoType *pRevisionInfo)
{
   VAdcHalResultType result;
   uint8 aucData[6];
   uint32 uTotalBytesRead;
   uint32 uOffset = VADC_REVISION1_ADDR;

   result = piVAdcHal->pfnReadBytes(piVAdcHal->pCtxt,
                                    uOffset,
                                    aucData,
                                    ARRAY_LENGTH(aucData),
                                    &uTotalBytesRead);
   if (result != VADC_HAL_SUCCESS)
   {
      return result;
   }

   if (uTotalBytesRead != ARRAY_LENGTH(aucData))
   {
      return VADC_HAL_ERROR;
   }

   pRevisionInfo->uDigitalMinor = aucData[0];
   pRevisionInfo->uDigitalMajor = aucData[1];
   pRevisionInfo->uAnalogMinor = aucData[2];
   pRevisionInfo->uAnalogMajor = aucData[3];
   pRevisionInfo->uType = aucData[4];
   pRevisionInfo->uSubType = aucData[5];

   return result;
}

VAdcHalResultType VAdcHalGetStatus(VAdcHalInterfaceType *piVAdcHal, VAdcStatusType *pStatus)
{
   VAdcHalResultType result;
   uint8 aucData[2];
   uint32 uTotalBytesRead;
   uint32 uOffset = VADC_STATUS1_ADDR;

   result = piVAdcHal->pfnReadBytes(piVAdcHal->pCtxt,
                                    uOffset,
                                    aucData,
                                    ARRAY_LENGTH(aucData),
                                    &uTotalBytesRead);
   if (result != VADC_HAL_SUCCESS)
   {
      return result;
   }

   if (uTotalBytesRead != ARRAY_LENGTH(aucData))
   {
      return VADC_HAL_ERROR;
   }

   pStatus->eOpMode = (VAdcOpModeType)READ_FIELD(aucData[0], VADC_STATUS1, OP_MODE);
   pStatus->eIntervalMode = (VAdcIntervalModeType)READ_FIELD(aucData[0], VADC_STATUS1, MEAS_INTERVAL_EN_STS);
   pStatus->eConversionStatus = (VAdcConversionStatusType)READ_FIELD(aucData[0], VADC_STATUS1, REQ_STS_AND_EOC);

   pStatus->eSequencerStatus = (VAdcSequencerStatusType)READ_FIELD(aucData[1], VADC_STATUS2, CONV_SEQ_STATE);

   if (READ_FIELD(aucData[1], VADC_STATUS2, FIFO_NOT_EMPTY_FLAG) == 0)
   {
      pStatus->bSequencerFifoNotEmptyErrorOccurred = FALSE;
   }
   else
   {
      pStatus->bSequencerFifoNotEmptyErrorOccurred = TRUE;
   }

   if (READ_FIELD(aucData[1], VADC_STATUS2, CONV_SEQ_TIMEOUT_STS) == 0)
   {
      pStatus->bSequencerTimeoutErrorOccurred = FALSE;
   }
   else
   {
      pStatus->bSequencerTimeoutErrorOccurred = TRUE;
   }

   return VADC_HAL_SUCCESS;
}

VAdcHalResultType VAdcHalConfigInterrupts(VAdcHalInterfaceType *piVAdcHal, VAdcInterruptsConfigType *pConfigs)
{
   VAdcHalResultType result;
   uint8 aucData[3];
   uint32 uTotalBytesRead;
   uint32 uOffset = VADC_INT_SET_TYPE_ADDR;

   result = piVAdcHal->pfnReadBytes(piVAdcHal->pCtxt,
                                    uOffset,
                                    aucData,
                                    ARRAY_LENGTH(aucData),
                                    &uTotalBytesRead);
   if (result != VADC_HAL_SUCCESS)
   {
      return result;
   }

   if (uTotalBytesRead != ARRAY_LENGTH(aucData))
   {
      return VADC_HAL_ERROR;
   }

   /* Only supporting the EOC interrupt */
   switch (pConfigs->eEocInterruptConfig)
   {
      case VADC_INTERRUPT_CONFIG_LEVEL_HIGH:
         aucData[0] = WRITE_FIELD(aucData[0], VADC_INT_SET_TYPE, EOC, 0);
         aucData[1] = WRITE_FIELD(aucData[1], VADC_INT_POLARITY_HIGH, EOC, 1);
         aucData[2] = WRITE_FIELD(aucData[2], VADC_INT_POLARITY_LOW, EOC, 0);
         break;
      case VADC_INTERRUPT_CONFIG_LEVEL_LOW:
         aucData[0] = WRITE_FIELD(aucData[0], VADC_INT_SET_TYPE, EOC, 0);
         aucData[1] = WRITE_FIELD(aucData[1], VADC_INT_POLARITY_HIGH, EOC, 0);
         aucData[2] = WRITE_FIELD(aucData[2], VADC_INT_POLARITY_LOW, EOC, 1);
         break;
      case VADC_INTERRUPT_CONFIG_RISING_EDGE:
         aucData[0] = WRITE_FIELD(aucData[0], VADC_INT_SET_TYPE, EOC, 1);
         aucData[1] = WRITE_FIELD(aucData[1], VADC_INT_POLARITY_HIGH, EOC, 1);
         aucData[2] = WRITE_FIELD(aucData[2], VADC_INT_POLARITY_LOW, EOC, 0);
         break;
      case VADC_INTERRUPT_CONFIG_FALLING_EDGE:
         aucData[0] = WRITE_FIELD(aucData[0], VADC_INT_SET_TYPE, EOC, 1);
         aucData[1] = WRITE_FIELD(aucData[1], VADC_INT_POLARITY_HIGH, EOC, 0);
         aucData[2] = WRITE_FIELD(aucData[2], VADC_INT_POLARITY_LOW, EOC, 1);
         break;
      case VADC_INTERRUPT_CONFIG_RISING_OR_FALLING_EDGE:
         aucData[0] = WRITE_FIELD(aucData[0], VADC_INT_SET_TYPE, EOC, 1);
         aucData[1] = WRITE_FIELD(aucData[1], VADC_INT_POLARITY_HIGH, EOC, 1);
         aucData[2] = WRITE_FIELD(aucData[2], VADC_INT_POLARITY_LOW, EOC, 1);
         break;
      default:
         break;
   }

   result = piVAdcHal->pfnWriteBytes(piVAdcHal->pCtxt,
                                     uOffset,
                                     aucData,
                                     ARRAY_LENGTH(aucData));
   if (result != VADC_HAL_SUCCESS)
   {
      return result;
   }

   return VADC_HAL_SUCCESS;
}

VAdcHalResultType VAdcHalGetPendingInterrupts(VAdcHalInterfaceType *piVAdcHal, VAdcInterruptMaskType *puIntrMask)
{
   VAdcHalResultType result;
   uint8 aucData[1];
   uint32 uTotalBytesRead;
   uint32 uOffset = VADC_INT_PENDING_STS_ADDR;

   result = piVAdcHal->pfnReadBytes(piVAdcHal->pCtxt,
                                    uOffset,
                                    aucData,
                                    ARRAY_LENGTH(aucData),
                                    &uTotalBytesRead);
   if (result != VADC_HAL_SUCCESS)
   {
      return result;
   }

   if (uTotalBytesRead != ARRAY_LENGTH(aucData))
   {
      return VADC_HAL_ERROR;
   }

   *puIntrMask = READ_REG(aucData[0], VADC_INT_PENDING_STS);

   return VADC_HAL_SUCCESS;
}

VAdcHalResultType VAdcHalClearInterrupts(VAdcHalInterfaceType *piVAdcHal, VAdcInterruptMaskType uIntrMask)
{
   VAdcHalResultType result;
   uint8 aucData[1];
   uint32 uOffset = VADC_INT_LATCHED_CLR_ADDR;

   aucData[0] = WRITE_REG(0, VADC_INT_LATCHED_CLR, uIntrMask);

   result = piVAdcHal->pfnWriteBytes(piVAdcHal->pCtxt,
                                     uOffset,
                                     aucData,
                                     ARRAY_LENGTH(aucData));
   if (result != VADC_HAL_SUCCESS)
   {
      return result;
   }

   return VADC_HAL_SUCCESS;
}

VAdcHalResultType VAdcHalEnableInterrupts(VAdcHalInterfaceType *piVAdcHal, VAdcInterruptMaskType uIntrMask)
{
   VAdcHalResultType result;
   uint8 aucData[1];
   uint32 uOffset = VADC_INT_EN_SET_ADDR;

   aucData[0] = 0;  /* Other bits are not populated */
   aucData[0] = WRITE_REG(aucData[0], VADC_INT_EN_SET, uIntrMask);

   result = piVAdcHal->pfnWriteBytes(piVAdcHal->pCtxt,
                                     uOffset,
                                     aucData,
                                     ARRAY_LENGTH(aucData));
   if (result != VADC_HAL_SUCCESS)
   {
      return result;
   }

   return VADC_HAL_SUCCESS;
}

VAdcHalResultType VAdcHalSetInterruptMid(VAdcHalInterfaceType *piVAdcHal, VAdcInterruptMid uIntrMid)
{
   VAdcHalResultType result;
   uint8 aucData[1];
   uint32 uTotalBytesRead;
   uint32 uOffset = VADC_INT_MID_SEL_ADDR;

   result = piVAdcHal->pfnReadBytes(piVAdcHal->pCtxt,
                                    uOffset,
                                    aucData,
                                    ARRAY_LENGTH(aucData),
                                    &uTotalBytesRead);
   if (result != VADC_HAL_SUCCESS)
   {
      return result;
   }

   if (uTotalBytesRead != ARRAY_LENGTH(aucData))
   {
      return VADC_HAL_ERROR;
   }

   aucData[0] = WRITE_FIELD(aucData[0], VADC_INT_MID_SEL, INT_MID_SEL, uIntrMid);

   result = piVAdcHal->pfnWriteBytes(piVAdcHal->pCtxt,
                                     uOffset,
                                     aucData,
                                     ARRAY_LENGTH(aucData));
   if (result != VADC_HAL_SUCCESS)
   {
      return result;
   }

   return VADC_HAL_SUCCESS;
}

VAdcHalResultType VAdcHalSetOpMode(VAdcHalInterfaceType *piVAdcHal, VAdcOpModeType eOpMode)
{
   VAdcHalResultType result;
   uint8 aucData[1];
   uint32 uOffset = VADC_MODE_CTL_ADDR;

   aucData[0] = 0x3;  /* AMUX_TRIM_EN = 1; ADC_TRIM_EN = 1 */
   aucData[0] = WRITE_FIELD(aucData[0], VADC_MODE_CTL, OP_MODE, (uint8)eOpMode);

   result = piVAdcHal->pfnWriteBytes(piVAdcHal->pCtxt,
                                     uOffset,
                                     aucData,
                                     ARRAY_LENGTH(aucData));
   if (result != VADC_HAL_SUCCESS)
   {
      return result;
   }

   return VADC_HAL_SUCCESS;
}

VAdcHalResultType VAdcHalSetEnable(VAdcHalInterfaceType *piVAdcHal, VAdcEnableType eEnable)
{
   VAdcHalResultType result;
   uint8 aucData[1];
   uint32 uOffset = VADC_EN_CTL1_ADDR;

   aucData[0] = 0;  /* Only bit 7 exists, no need to read first */
   aucData[0] = WRITE_FIELD(aucData[0], VADC_EN_CTL1, ADC_EN, (uint8)eEnable);

   result = piVAdcHal->pfnWriteBytes(piVAdcHal->pCtxt,
                                     uOffset,
                                     aucData,
                                     ARRAY_LENGTH(aucData));
   if (result != VADC_HAL_SUCCESS)
   {
      return result;
   }

   return VADC_HAL_SUCCESS;
}

VAdcHalResultType VAdcHalSetConversionParameters(VAdcHalInterfaceType *piVAdcHal, VAdcConversionParametersType *pParams)
{
   VAdcHalResultType result;
   uint8 aucData[2];
   uint32 uOffset = VADC_ADC_CH_SEL_CTL_ADDR;

   /* First set the channel index - only write one byte */
   aucData[0] = (uint8)pParams->uChannel;

   result = piVAdcHal->pfnWriteBytes(piVAdcHal->pCtxt,
                                     uOffset,
                                     aucData,
                                     sizeof(aucData[0]));
   if (result != VADC_HAL_SUCCESS)
   {
      return result;
   }

   /* Set the dig params & HW settle delay */
   uOffset = VADC_ADC_DIG_PARAM_ADDR;

   aucData[0] = 0;  /* Other bits are not populated */
   aucData[0] = WRITE_FIELD(aucData[0], VADC_ADC_DIG_PARAM, DEC_RATIO_SEL, (uint8)pParams->eDecimationRatio);
   aucData[0] = WRITE_FIELD(aucData[0], VADC_ADC_DIG_PARAM, CLK_SEL, (uint8)pParams->eClockSelect);

   aucData[1] = 0;  /* Other bits are not populated */
   aucData[1] = WRITE_FIELD(aucData[1], VADC_HW_SETTLE_DELAY, HW_SETTLE_DELAY, (uint8)pParams->eSettlingDelay);

   result = piVAdcHal->pfnWriteBytes(piVAdcHal->pCtxt,
                                     uOffset,
                                     aucData,
                                     ARRAY_LENGTH(aucData));
   if (result != VADC_HAL_SUCCESS)
   {
      return result;
   }

   /* Set fast average mode */
   uOffset = VADC_FAST_AVG_CTL_ADDR;

   aucData[0] = 0;  /* Other bits are not populated */
   aucData[1] = 0;  /* Other bits are not populated */

   if (pParams->eFastAverageMode == VADC_FAST_AVERAGE_NONE)
   {
      aucData[0] = WRITE_FIELD(aucData[0], VADC_FAST_AVG_CTL, FAST_AVG_SAMPLES, 0);
      aucData[1] = WRITE_FIELD(aucData[1], VADC_FAST_AVG_EN, FAST_AVG_EN, 0);
   }
   else
   {
      aucData[0] = WRITE_FIELD(aucData[0], VADC_FAST_AVG_CTL, FAST_AVG_SAMPLES, (uint8)pParams->eFastAverageMode);
      aucData[1] = WRITE_FIELD(aucData[1], VADC_FAST_AVG_EN, FAST_AVG_EN, 1);
   }

   result = piVAdcHal->pfnWriteBytes(piVAdcHal->pCtxt,
                                     uOffset,
                                     aucData,
                                     ARRAY_LENGTH(aucData));
   if (result != VADC_HAL_SUCCESS)
   {
      return result;
   }

   return VADC_HAL_SUCCESS;
}

VAdcHalResultType VAdcHalSetSequencerParameters(VAdcHalInterfaceType *piVAdcHal, VAdcSequencerParametersType *pParams)
{
   VAdcHalResultType result;
   uint8 aucData[2];
   uint32 uOffset = VADC_CONV_SEQ_CTL_ADDR;

   aucData[0] = 0;  /* Other bits are not populated */
   aucData[0] = WRITE_FIELD(aucData[0], VADC_CONV_SEQ_CTL, CONV_SEQ_HOLDOFF, (uint8)pParams->eHoldoff);
   aucData[0] = WRITE_FIELD(aucData[0], VADC_CONV_SEQ_CTL, CONV_SEQ_TIMEOUT, (uint8)pParams->eTimeoutTime);

   aucData[1] = 0;  /* Other bits are not populated */
   aucData[1] = WRITE_FIELD(aucData[1], VADC_CONV_SEQ_TRIG_CTL, CONV_SEQ_TRIG_COND, (uint8)pParams->eTriggerCondition);
   aucData[1] = WRITE_FIELD(aucData[1], VADC_CONV_SEQ_TRIG_CTL, CONV_SEQ_TRIG_SEL, (uint8)pParams->eTriggerInput);

   result = piVAdcHal->pfnWriteBytes(piVAdcHal->pCtxt,
                                     uOffset,
                                     aucData,
                                     ARRAY_LENGTH(aucData));
   if (result != VADC_HAL_SUCCESS)
   {
      return result;
   }

   return VADC_HAL_SUCCESS;
}

VAdcHalResultType VAdcHalRequestConversion(VAdcHalInterfaceType *piVAdcHal)
{
   VAdcHalResultType result;
   uint8 aucData[1];
   uint32 uOffset = VADC_CONV_REQ_ADDR;

   aucData[0] = 0; /* register is write-only so cannot read the other bits - assume they are 0 */
   aucData[0] = WRITE_FIELD(aucData[0], VADC_CONV_REQ, REQ, 1);

   result = piVAdcHal->pfnWriteBytes(piVAdcHal->pCtxt,
                                     uOffset,
                                     aucData,
                                     ARRAY_LENGTH(aucData));
   if (result != VADC_HAL_SUCCESS)
   {
      return result;
   }

   return VADC_HAL_SUCCESS;
}

VAdcHalResultType VAdcHalGetConversionCode(VAdcHalInterfaceType *piVAdcHal, VAdcConversionCodeType *puCode)
{
   VAdcHalResultType result;
   uint8 aucData[2];
   uint32 uTotalBytesRead;
   uint32 uOffset = VADC_DATA0_ADDR;

   result = piVAdcHal->pfnReadBytes(piVAdcHal->pCtxt,
                                    uOffset,
                                    aucData,
                                    ARRAY_LENGTH(aucData),
                                    &uTotalBytesRead);
   if (result != VADC_HAL_SUCCESS)
   {
      return result;
   }

   if (uTotalBytesRead != ARRAY_LENGTH(aucData))
   {
      return VADC_HAL_ERROR;
   }

   *puCode = ((aucData[1] & 0xFF) << 8) | (aucData[0] & 0xFF);

   return VADC_HAL_SUCCESS;
}

VAdcHalResultType VAdcHalDumpRegisters(VAdcHalInterfaceType *piVAdcHal, VAdcHalRegDumpType *pVAdcRegDump)
{
   VAdcHalResultType result;
   uint8 aucData[1];
   uint32 uTotalBytesRead;
   uint32 uOffset;
   uint32 uReg;

   for (uReg = 0; uReg < VADC_HAL_NUM_REGISTERS_TO_DUMP; uReg++)
   {
      uOffset = vAdcHalRegDumpAddresses[uReg];

      result = piVAdcHal->pfnReadBytes(piVAdcHal->pCtxt,
                                       uOffset,
                                       aucData,
                                       ARRAY_LENGTH(aucData),
                                       &uTotalBytesRead);
      if (result != VADC_HAL_SUCCESS)
      {
         return result;
      }

      if (uTotalBytesRead != ARRAY_LENGTH(aucData))
      {
         return VADC_HAL_ERROR;
      }

      pVAdcRegDump->aVAdcReg[uReg].u8Offset = (uint8)uOffset;
      pVAdcRegDump->aVAdcReg[uReg].u8Val = aucData[0];
   }

   return VADC_HAL_SUCCESS;
}

VAdcHalResultType VAdcHalStoreFabID(VAdcHalInterfaceType *piVAdcHal, VAdcFabIDType uFabID)
{
   VAdcHalResultType result;
   uint8 aucData[1];
   uint32 uOffset;

   /* Unlock secure reg */
   result = VAdcHalUnlockSecReg(piVAdcHal);
   if (result != VADC_HAL_SUCCESS)
   {
      return result;
   }

   /* Write fab ID */
   aucData[0] = (uint8)uFabID;
   uOffset = VADC_INT_TEST_VAL_ADDR;
   result = piVAdcHal->pfnWriteBytes(piVAdcHal->pCtxt,
                                     uOffset,
                                     aucData,
                                     ARRAY_LENGTH(aucData));
   return result;
}

VAdcHalResultType VAdcHalGetFabID(VAdcHalInterfaceType *piVAdcHal, VAdcFabIDType *puFabID)
{
   VAdcHalResultType result;
   uint8 aucData[1];
   uint32 uTotalBytesRead;
   uint32 uOffset = VADC_INT_TEST_VAL_ADDR;

   result = piVAdcHal->pfnReadBytes(piVAdcHal->pCtxt,
                                    uOffset,
                                    aucData,
                                    ARRAY_LENGTH(aucData),
                                    &uTotalBytesRead);
   if (result != VADC_HAL_SUCCESS)
   {
      return result;
   }

   if (uTotalBytesRead != ARRAY_LENGTH(aucData))
   {
      return VADC_HAL_ERROR;
   }

   *puFabID = aucData[0];

   return VADC_HAL_SUCCESS;
}

