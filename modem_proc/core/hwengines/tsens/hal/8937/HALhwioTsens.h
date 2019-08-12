#ifndef HAL_HWIO_TSENS_H
#define HAL_HWIO_TSENS_H
/*
  ===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.
*/
/*============================================================================
  @file HALhwioTsens.h

  Implementation of the TSENS HAL for 8937 - HWIO info was auto-generated

                Copyright (c) 2015 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/


/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * MODULE: MPM2_TSENS
 *--------------------------------------------------------------------------*/

#define MPM2_TSENS_REG_BASE                                                             (gpuMpm2MpmBase      + 0x00008000)
#define MPM2_TSENS_REG_BASE_OFFS                                                        0x00008000

#define HWIO_MPM2_TSENS_HW_VER_ADDR                                                     (MPM2_TSENS_REG_BASE      + 0x00000000)
#define HWIO_MPM2_TSENS_HW_VER_OFFS                                                     (MPM2_TSENS_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM2_TSENS_HW_VER_RMSK                                                     0xffffffff
#define HWIO_MPM2_TSENS_HW_VER_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_HW_VER_ADDR, HWIO_MPM2_TSENS_HW_VER_RMSK)
#define HWIO_MPM2_TSENS_HW_VER_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_HW_VER_ADDR, m)
#define HWIO_MPM2_TSENS_HW_VER_MAJOR_BMSK                                               0xf0000000
#define HWIO_MPM2_TSENS_HW_VER_MAJOR_SHFT                                                     0x1c
#define HWIO_MPM2_TSENS_HW_VER_MINOR_BMSK                                                0xfff0000
#define HWIO_MPM2_TSENS_HW_VER_MINOR_SHFT                                                     0x10
#define HWIO_MPM2_TSENS_HW_VER_STEP_BMSK                                                    0xffff
#define HWIO_MPM2_TSENS_HW_VER_STEP_SHFT                                                       0x0

#define HWIO_MPM2_TSENS_CTRL_ADDR                                                       (MPM2_TSENS_REG_BASE      + 0x00000004)
#define HWIO_MPM2_TSENS_CTRL_OFFS                                                       (MPM2_TSENS_REG_BASE_OFFS + 0x00000004)
#define HWIO_MPM2_TSENS_CTRL_RMSK                                                       0xfff83fff
#define HWIO_MPM2_TSENS_CTRL_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_CTRL_ADDR, HWIO_MPM2_TSENS_CTRL_RMSK)
#define HWIO_MPM2_TSENS_CTRL_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_CTRL_ADDR, m)
#define HWIO_MPM2_TSENS_CTRL_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_CTRL_ADDR,v)
#define HWIO_MPM2_TSENS_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_CTRL_ADDR,m,v,HWIO_MPM2_TSENS_CTRL_IN)
#define HWIO_MPM2_TSENS_CTRL_TSENS_MTC_EN_BMSK                                          0x80000000
#define HWIO_MPM2_TSENS_CTRL_TSENS_MTC_EN_SHFT                                                0x1f
#define HWIO_MPM2_TSENS_CTRL_TSENS_MTC_EN_DISABLED_FVAL                                        0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_MTC_EN_ENABLED_FVAL                                         0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_CPU_HIGH_LOW_INT_EN_BMSK                             0x40000000
#define HWIO_MPM2_TSENS_CTRL_TSENS_CPU_HIGH_LOW_INT_EN_SHFT                                   0x1e
#define HWIO_MPM2_TSENS_CTRL_TSENS_CPU_HIGH_LOW_INT_EN_DISABLED_FVAL                           0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_CPU_HIGH_LOW_INT_EN_ENABLED_FVAL                            0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_AUTO_ADJUST_PERIOD_EN_BMSK                           0x20000000
#define HWIO_MPM2_TSENS_CTRL_TSENS_AUTO_ADJUST_PERIOD_EN_SHFT                                 0x1d
#define HWIO_MPM2_TSENS_CTRL_TSENS_AUTO_ADJUST_PERIOD_EN_DISABLED_FVAL                         0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_AUTO_ADJUST_PERIOD_EN_ENABLED_FVAL                          0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_CLAMP_BMSK                                           0x10000000
#define HWIO_MPM2_TSENS_CTRL_TSENS_CLAMP_SHFT                                                 0x1c
#define HWIO_MPM2_TSENS_CTRL_TSENS_CLAMP_UNCLAMPED_FVAL                                        0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_CLAMP_CLAMPED_FVAL                                          0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_BYPASS_EN_BMSK                                        0x8000000
#define HWIO_MPM2_TSENS_CTRL_TSENS_BYPASS_EN_SHFT                                             0x1b
#define HWIO_MPM2_TSENS_CTRL_TSENS_BYPASS_EN_DISABLED_FVAL                                     0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_BYPASS_EN_ENABLED_FVAL                                      0x1
#define HWIO_MPM2_TSENS_CTRL_MEASURE_PERIOD_BMSK                                         0x7f80000
#define HWIO_MPM2_TSENS_CTRL_MEASURE_PERIOD_SHFT                                              0x13
#define HWIO_MPM2_TSENS_CTRL_SENSOR10_EN_BMSK                                               0x2000
#define HWIO_MPM2_TSENS_CTRL_SENSOR10_EN_SHFT                                                  0xd
#define HWIO_MPM2_TSENS_CTRL_SENSOR10_EN_DISABLED_FVAL                                         0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR10_EN_ENABLED_FVAL                                          0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR9_EN_BMSK                                                0x1000
#define HWIO_MPM2_TSENS_CTRL_SENSOR9_EN_SHFT                                                   0xc
#define HWIO_MPM2_TSENS_CTRL_SENSOR9_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR9_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR8_EN_BMSK                                                 0x800
#define HWIO_MPM2_TSENS_CTRL_SENSOR8_EN_SHFT                                                   0xb
#define HWIO_MPM2_TSENS_CTRL_SENSOR8_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR8_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR7_EN_BMSK                                                 0x400
#define HWIO_MPM2_TSENS_CTRL_SENSOR7_EN_SHFT                                                   0xa
#define HWIO_MPM2_TSENS_CTRL_SENSOR7_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR7_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR6_EN_BMSK                                                 0x200
#define HWIO_MPM2_TSENS_CTRL_SENSOR6_EN_SHFT                                                   0x9
#define HWIO_MPM2_TSENS_CTRL_SENSOR6_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR6_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR5_EN_BMSK                                                 0x100
#define HWIO_MPM2_TSENS_CTRL_SENSOR5_EN_SHFT                                                   0x8
#define HWIO_MPM2_TSENS_CTRL_SENSOR5_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR5_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR4_EN_BMSK                                                  0x80
#define HWIO_MPM2_TSENS_CTRL_SENSOR4_EN_SHFT                                                   0x7
#define HWIO_MPM2_TSENS_CTRL_SENSOR4_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR4_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR3_EN_BMSK                                                  0x40
#define HWIO_MPM2_TSENS_CTRL_SENSOR3_EN_SHFT                                                   0x6
#define HWIO_MPM2_TSENS_CTRL_SENSOR3_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR3_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR2_EN_BMSK                                                  0x20
#define HWIO_MPM2_TSENS_CTRL_SENSOR2_EN_SHFT                                                   0x5
#define HWIO_MPM2_TSENS_CTRL_SENSOR2_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR2_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR1_EN_BMSK                                                  0x10
#define HWIO_MPM2_TSENS_CTRL_SENSOR1_EN_SHFT                                                   0x4
#define HWIO_MPM2_TSENS_CTRL_SENSOR1_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR1_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR0_EN_BMSK                                                   0x8
#define HWIO_MPM2_TSENS_CTRL_SENSOR0_EN_SHFT                                                   0x3
#define HWIO_MPM2_TSENS_CTRL_SENSOR0_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR0_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_ADC_CLK_SEL_BMSK                                            0x4
#define HWIO_MPM2_TSENS_CTRL_TSENS_ADC_CLK_SEL_SHFT                                            0x2
#define HWIO_MPM2_TSENS_CTRL_TSENS_ADC_CLK_SEL_INTERNAL_OSCILLATOR_FVAL                        0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_ADC_CLK_SEL_EXTERNAL_CLOCK_SOURCE_FVAL                      0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_SW_RST_BMSK                                                 0x2
#define HWIO_MPM2_TSENS_CTRL_TSENS_SW_RST_SHFT                                                 0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_SW_RST_RESET_DEASSERTED_FVAL                                0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_SW_RST_RESET_ASSERTED_FVAL                                  0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_EN_BMSK                                                     0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_EN_SHFT                                                     0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_EN_DISABLED_FVAL                                            0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_EN_ENABLED_FVAL                                             0x1

#define HWIO_MPM2_TSENS_TEST_CTRL_ADDR                                                  (MPM2_TSENS_REG_BASE      + 0x00000008)
#define HWIO_MPM2_TSENS_TEST_CTRL_OFFS                                                  (MPM2_TSENS_REG_BASE_OFFS + 0x00000008)
#define HWIO_MPM2_TSENS_TEST_CTRL_RMSK                                                        0x1f
#define HWIO_MPM2_TSENS_TEST_CTRL_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_TEST_CTRL_ADDR, HWIO_MPM2_TSENS_TEST_CTRL_RMSK)
#define HWIO_MPM2_TSENS_TEST_CTRL_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_TEST_CTRL_ADDR, m)
#define HWIO_MPM2_TSENS_TEST_CTRL_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_TEST_CTRL_ADDR,v)
#define HWIO_MPM2_TSENS_TEST_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_TEST_CTRL_ADDR,m,v,HWIO_MPM2_TSENS_TEST_CTRL_IN)
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_BMSK                                         0x1e
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_SHFT                                          0x1
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_EN_BMSK                                           0x1
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_EN_SHFT                                           0x0
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_EN_TEST_DISABLED_FVAL                             0x0
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_EN_TEST_ENABLED_FVAL                              0x1

#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_ADDR(n)                                  (MPM2_TSENS_REG_BASE      + 0x0000000c + 0x4 * (n))
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_OFFS(n)                                  (MPM2_TSENS_REG_BASE_OFFS + 0x0000000c + 0x4 * (n))
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_RMSK                                       0x3fffff
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAXn                                             10
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_ADDR(n), HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_RMSK)
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_ADDR(n), mask)
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_OUTI(n,val)    \
        out_dword(HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_ADDR(n),val)
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_ADDR(n),mask,val,HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_INI(n))
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAX_STATUS_MASK_BMSK                       0x200000
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAX_STATUS_MASK_SHFT                           0x15
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAX_STATUS_MASK_NORMAL_OPERATION_FVAL           0x0
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAX_STATUS_MASK_MASK_OFF_MAX_STATUS_FVAL        0x1
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MIN_STATUS_MASK_BMSK                       0x100000
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MIN_STATUS_MASK_SHFT                           0x14
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MIN_STATUS_MASK_NORMAL_OPERATION_FVAL           0x0
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MIN_STATUS_MASK_MASK_OFF_MIN_STATUS_FVAL        0x1
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAX_THRESHOLD_BMSK                          0xffc00
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAX_THRESHOLD_SHFT                              0xa
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MIN_THRESHOLD_BMSK                            0x3ff
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MIN_THRESHOLD_SHFT                              0x0

#define HWIO_MPM2_TS_CONTROL_ADDR                                                       (MPM2_TSENS_REG_BASE      + 0x0000004c)
#define HWIO_MPM2_TS_CONTROL_OFFS                                                       (MPM2_TSENS_REG_BASE_OFFS + 0x0000004c)
#define HWIO_MPM2_TS_CONTROL_RMSK                                                        0x1ffffff
#define HWIO_MPM2_TS_CONTROL_IN          \
        in_dword_masked(HWIO_MPM2_TS_CONTROL_ADDR, HWIO_MPM2_TS_CONTROL_RMSK)
#define HWIO_MPM2_TS_CONTROL_INM(m)      \
        in_dword_masked(HWIO_MPM2_TS_CONTROL_ADDR, m)
#define HWIO_MPM2_TS_CONTROL_OUT(v)      \
        out_dword(HWIO_MPM2_TS_CONTROL_ADDR,v)
#define HWIO_MPM2_TS_CONTROL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TS_CONTROL_ADDR,m,v,HWIO_MPM2_TS_CONTROL_IN)
#define HWIO_MPM2_TS_CONTROL_TS_CONTROL_BMSK                                             0x1ffffff
#define HWIO_MPM2_TS_CONTROL_TS_CONTROL_SHFT                                                   0x0

#define HWIO_MPM2_TS_CONFIG_ADDR                                                        (MPM2_TSENS_REG_BASE      + 0x00000050)
#define HWIO_MPM2_TS_CONFIG_OFFS                                                        (MPM2_TSENS_REG_BASE_OFFS + 0x00000050)
#define HWIO_MPM2_TS_CONFIG_RMSK                                                              0xff
#define HWIO_MPM2_TS_CONFIG_IN          \
        in_dword_masked(HWIO_MPM2_TS_CONFIG_ADDR, HWIO_MPM2_TS_CONFIG_RMSK)
#define HWIO_MPM2_TS_CONFIG_INM(m)      \
        in_dword_masked(HWIO_MPM2_TS_CONFIG_ADDR, m)
#define HWIO_MPM2_TS_CONFIG_OUT(v)      \
        out_dword(HWIO_MPM2_TS_CONFIG_ADDR,v)
#define HWIO_MPM2_TS_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TS_CONFIG_ADDR,m,v,HWIO_MPM2_TS_CONFIG_IN)
#define HWIO_MPM2_TS_CONFIG_TS_CONFIG_BMSK                                                    0xff
#define HWIO_MPM2_TS_CONFIG_TS_CONFIG_SHFT                                                     0x0

#define HWIO_MPM2_TS_CONFIG_REMOTE_IN_ADDR                                              (MPM2_TSENS_REG_BASE      + 0x00000054)
#define HWIO_MPM2_TS_CONFIG_REMOTE_IN_OFFS                                              (MPM2_TSENS_REG_BASE_OFFS + 0x00000054)
#define HWIO_MPM2_TS_CONFIG_REMOTE_IN_RMSK                                                   0x1ff
#define HWIO_MPM2_TS_CONFIG_REMOTE_IN_IN          \
        in_dword_masked(HWIO_MPM2_TS_CONFIG_REMOTE_IN_ADDR, HWIO_MPM2_TS_CONFIG_REMOTE_IN_RMSK)
#define HWIO_MPM2_TS_CONFIG_REMOTE_IN_INM(m)      \
        in_dword_masked(HWIO_MPM2_TS_CONFIG_REMOTE_IN_ADDR, m)
#define HWIO_MPM2_TS_CONFIG_REMOTE_IN_OUT(v)      \
        out_dword(HWIO_MPM2_TS_CONFIG_REMOTE_IN_ADDR,v)
#define HWIO_MPM2_TS_CONFIG_REMOTE_IN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TS_CONFIG_REMOTE_IN_ADDR,m,v,HWIO_MPM2_TS_CONFIG_REMOTE_IN_IN)
#define HWIO_MPM2_TS_CONFIG_REMOTE_IN_TS_CONFIG_REMOTE_IN_BMSK                               0x1ff
#define HWIO_MPM2_TS_CONFIG_REMOTE_IN_TS_CONFIG_REMOTE_IN_SHFT                                 0x0

#define HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_ADDR                                   (MPM2_TSENS_REG_BASE      + 0x00000058)
#define HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_OFFS                                   (MPM2_TSENS_REG_BASE_OFFS + 0x00000058)
#define HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_RMSK                                         0xff
#define HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_ADDR, HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_RMSK)
#define HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_ADDR, m)
#define HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_ADDR,v)
#define HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_ADDR,m,v,HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_IN)
#define HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_POWERDOWN_MEASURE_PERIOD_BMSK                0xff
#define HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_POWERDOWN_MEASURE_PERIOD_SHFT                 0x0

#define HWIO_MPM2_TSENS_CPU_INDEXES_ADDR                                                (MPM2_TSENS_REG_BASE      + 0x0000005c)
#define HWIO_MPM2_TSENS_CPU_INDEXES_OFFS                                                (MPM2_TSENS_REG_BASE_OFFS + 0x0000005c)
#define HWIO_MPM2_TSENS_CPU_INDEXES_RMSK                                                0xffffffff
#define HWIO_MPM2_TSENS_CPU_INDEXES_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_CPU_INDEXES_ADDR, HWIO_MPM2_TSENS_CPU_INDEXES_RMSK)
#define HWIO_MPM2_TSENS_CPU_INDEXES_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_CPU_INDEXES_ADDR, m)
#define HWIO_MPM2_TSENS_CPU_INDEXES_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_CPU_INDEXES_ADDR,v)
#define HWIO_MPM2_TSENS_CPU_INDEXES_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_CPU_INDEXES_ADDR,m,v,HWIO_MPM2_TSENS_CPU_INDEXES_IN)
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU7_INDEX_BMSK                                     0xf0000000
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU7_INDEX_SHFT                                           0x1c
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU6_INDEX_BMSK                                      0xf000000
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU6_INDEX_SHFT                                           0x18
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU5_INDEX_BMSK                                       0xf00000
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU5_INDEX_SHFT                                           0x14
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU4_INDEX_BMSK                                        0xf0000
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU4_INDEX_SHFT                                           0x10
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU3_INDEX_BMSK                                         0xf000
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU3_INDEX_SHFT                                            0xc
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU2_INDEX_BMSK                                          0xf00
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU2_INDEX_SHFT                                            0x8
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU1_INDEX_BMSK                                           0xf0
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU1_INDEX_SHFT                                            0x4
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU0_INDEX_BMSK                                            0xf
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU0_INDEX_SHFT                                            0x0

#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_ADDR(n)                                          (MPM2_TSENS_REG_BASE      + 0x00000060 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_OFFS(n)                                          (MPM2_TSENS_REG_BASE_OFFS + 0x00000060 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_RMSK                                             0xffffffff
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_MAXn                                                      3
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_ADDR(n), HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_RMSK)
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_ADDR(n), mask)
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_OUTI(n,val)    \
        out_dword(HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_ADDR(n),val)
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_ADDR(n),mask,val,HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_INI(n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_ZONE_MTC_EN_BMSK                                 0x80000000
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_ZONE_MTC_EN_SHFT                                       0x1f
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_PS_COMMAND_TH2_VIOLATED_BMSK                     0x7c000000
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_PS_COMMAND_TH2_VIOLATED_SHFT                           0x1a
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_PS_COMMAND_TH1_VIOLATED_BMSK                      0x3e00000
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_PS_COMMAND_TH1_VIOLATED_SHFT                           0x15
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_PS_COMMAND_COOL_BMSK                               0x1f0000
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_PS_COMMAND_COOL_SHFT                                   0x10
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_RSRVD_BMSK                                           0xf800
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_RSRVD_SHFT                                              0xb
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_SENSOR_MAP_BMSK                                       0x7ff
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_SENSOR_MAP_SHFT                                         0x0

#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_ADDR(n)                                       (MPM2_TSENS_REG_BASE      + 0x00000070 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_OFFS(n)                                       (MPM2_TSENS_REG_BASE_OFFS + 0x00000070 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_RMSK                                             0xfffff
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_MAXn                                                  10
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_ADDR(n), HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_RMSK)
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_ADDR(n), mask)
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_OUTI(n,val)    \
        out_dword(HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_ADDR(n),val)
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_ADDR(n),mask,val,HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_INI(n))
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_TH2_BMSK                                         0xffc00
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_TH2_SHFT                                             0xa
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_TH1_BMSK                                           0x3ff
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_TH1_SHFT                                             0x0

#define HWIO_MPM2_TSENS_MTC_MARGIN_ADDR                                                 (MPM2_TSENS_REG_BASE      + 0x0000009c)
#define HWIO_MPM2_TSENS_MTC_MARGIN_OFFS                                                 (MPM2_TSENS_REG_BASE_OFFS + 0x0000009c)
#define HWIO_MPM2_TSENS_MTC_MARGIN_RMSK                                                      0x3ff
#define HWIO_MPM2_TSENS_MTC_MARGIN_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_MARGIN_ADDR, HWIO_MPM2_TSENS_MTC_MARGIN_RMSK)
#define HWIO_MPM2_TSENS_MTC_MARGIN_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_MARGIN_ADDR, m)
#define HWIO_MPM2_TSENS_MTC_MARGIN_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_MTC_MARGIN_ADDR,v)
#define HWIO_MPM2_TSENS_MTC_MARGIN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_MTC_MARGIN_ADDR,m,v,HWIO_MPM2_TSENS_MTC_MARGIN_IN)
#define HWIO_MPM2_TSENS_MTC_MARGIN_TH2_MARGIN_VALUE_BMSK                                     0x3e0
#define HWIO_MPM2_TSENS_MTC_MARGIN_TH2_MARGIN_VALUE_SHFT                                       0x5
#define HWIO_MPM2_TSENS_MTC_MARGIN_TH1_MARGIN_VALUE_BMSK                                      0x1f
#define HWIO_MPM2_TSENS_MTC_MARGIN_TH1_MARGIN_VALUE_SHFT                                       0x0

#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_ADDR                                             (MPM2_TSENS_REG_BASE      + 0x000000a0)
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_OFFS                                             (MPM2_TSENS_REG_BASE_OFFS + 0x000000a0)
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_RMSK                                                   0x1f
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_ADDR, HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_RMSK)
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_ADDR, m)
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_ADDR,v)
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_ADDR,m,v,HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_IN)
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_DEBUG_BUS_SEL_BMSK                                     0x1e
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_DEBUG_BUS_SEL_SHFT                                      0x1
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_DEBUG_BUS_EN_BMSK                                       0x1
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_DEBUG_BUS_EN_SHFT                                       0x0

#define HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_ADDR                                      (MPM2_TSENS_REG_BASE      + 0x000000a4)
#define HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_OFFS                                      (MPM2_TSENS_REG_BASE_OFFS + 0x000000a4)
#define HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_RMSK                                      0xffffffff
#define HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_ADDR, HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_RMSK)
#define HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_ADDR, m)
#define HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_DEBUG_BUS_VALUE_BMSK                      0xffffffff
#define HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_DEBUG_BUS_VALUE_SHFT                             0x0

#define HWIO_MPM2_TSENS_SROT_SPARE_ADDR                                                 (MPM2_TSENS_REG_BASE      + 0x000000a8)
#define HWIO_MPM2_TSENS_SROT_SPARE_OFFS                                                 (MPM2_TSENS_REG_BASE_OFFS + 0x000000a8)
#define HWIO_MPM2_TSENS_SROT_SPARE_RMSK                                                 0xffffffff
#define HWIO_MPM2_TSENS_SROT_SPARE_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_SROT_SPARE_ADDR, HWIO_MPM2_TSENS_SROT_SPARE_RMSK)
#define HWIO_MPM2_TSENS_SROT_SPARE_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_SROT_SPARE_ADDR, m)
#define HWIO_MPM2_TSENS_SROT_SPARE_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_SROT_SPARE_ADDR,v)
#define HWIO_MPM2_TSENS_SROT_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_SROT_SPARE_ADDR,m,v,HWIO_MPM2_TSENS_SROT_SPARE_IN)
#define HWIO_MPM2_TSENS_SROT_SPARE_VALUE_BMSK                                           0xffffffff
#define HWIO_MPM2_TSENS_SROT_SPARE_VALUE_SHFT                                                  0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_TSENS_TM
 *--------------------------------------------------------------------------*/

#define MPM2_TSENS_TM_REG_BASE                                                                    (gpuMpm2MpmBase      + 0x00009000)
#define MPM2_TSENS_TM_REG_BASE_OFFS                                                               0x00009000

#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_ADDR                                           (MPM2_TSENS_TM_REG_BASE      + 0x00000000)
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_OFFS                                           (MPM2_TSENS_TM_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_RMSK                                                  0x1
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_ADDR, HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_RMSK)
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_ADDR, m)
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_ADDR,v)
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_ADDR,m,v,HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_IN)
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_INTERRUPT_EN_BMSK                                     0x1
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_INTERRUPT_EN_SHFT                                     0x0
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_INTERRUPT_EN_DISABLED_FVAL                            0x0
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_INTERRUPT_EN_ENABLED_FVAL                             0x1

#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_ADDR(n)                                        (MPM2_TSENS_TM_REG_BASE      + 0x00000004 + 0x4 * (n))
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_OFFS(n)                                        (MPM2_TSENS_TM_REG_BASE_OFFS + 0x00000004 + 0x4 * (n))
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_RMSK                                             0x3fffff
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_MAXn                                                   10
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_ADDR(n), HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_RMSK)
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_ADDR(n), mask)
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_OUTI(n,val)    \
        out_dword(HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_ADDR(n),val)
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_ADDR(n),mask,val,HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_INI(n))
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_UPPER_STATUS_CLR_BMSK                            0x200000
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_UPPER_STATUS_CLR_SHFT                                0x15
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_UPPER_STATUS_CLR_NORMAL_OPERATION_FVAL                0x0
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_UPPER_STATUS_CLR_CLEAR_UPPER_STATUS_FVAL              0x1
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_LOWER_STATUS_CLR_BMSK                            0x100000
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_LOWER_STATUS_CLR_SHFT                                0x14
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_LOWER_STATUS_CLR_NORMAL_OPERATION_FVAL                0x0
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_LOWER_STATUS_CLR_CLEAR_LOWER_STATUS_FVAL              0x1
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_UPPER_THRESHOLD_BMSK                              0xffc00
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_UPPER_THRESHOLD_SHFT                                  0xa
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_LOWER_THRESHOLD_BMSK                                0x3ff
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_LOWER_THRESHOLD_SHFT                                  0x0

#define HWIO_MPM2_TSENS_Sn_STATUS_ADDR(n)                                                         (MPM2_TSENS_TM_REG_BASE      + 0x00000044 + 0x4 * (n))
#define HWIO_MPM2_TSENS_Sn_STATUS_OFFS(n)                                                         (MPM2_TSENS_TM_REG_BASE_OFFS + 0x00000044 + 0x4 * (n))
#define HWIO_MPM2_TSENS_Sn_STATUS_RMSK                                                                0x7fff
#define HWIO_MPM2_TSENS_Sn_STATUS_MAXn                                                                    10
#define HWIO_MPM2_TSENS_Sn_STATUS_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_STATUS_ADDR(n), HWIO_MPM2_TSENS_Sn_STATUS_RMSK)
#define HWIO_MPM2_TSENS_Sn_STATUS_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_STATUS_ADDR(n), mask)
#define HWIO_MPM2_TSENS_Sn_STATUS_VALID_BMSK                                                          0x4000
#define HWIO_MPM2_TSENS_Sn_STATUS_VALID_SHFT                                                             0xe
#define HWIO_MPM2_TSENS_Sn_STATUS_MAX_STATUS_BMSK                                                     0x2000
#define HWIO_MPM2_TSENS_Sn_STATUS_MAX_STATUS_SHFT                                                        0xd
#define HWIO_MPM2_TSENS_Sn_STATUS_MAX_STATUS_MAX_THRESHOLD_NOT_VIOLATED_FVAL                             0x0
#define HWIO_MPM2_TSENS_Sn_STATUS_MAX_STATUS_MAX_THRESHOLD_VIOLATED_FVAL                                 0x1
#define HWIO_MPM2_TSENS_Sn_STATUS_UPPER_STATUS_BMSK                                                   0x1000
#define HWIO_MPM2_TSENS_Sn_STATUS_UPPER_STATUS_SHFT                                                      0xc
#define HWIO_MPM2_TSENS_Sn_STATUS_UPPER_STATUS_UPPER_THRESHOLD_NOT_VIOLATED_FVAL                         0x0
#define HWIO_MPM2_TSENS_Sn_STATUS_UPPER_STATUS_UPPER_THRESHOLD_VIOLATED_FVAL                             0x1
#define HWIO_MPM2_TSENS_Sn_STATUS_LOWER_STATUS_BMSK                                                    0x800
#define HWIO_MPM2_TSENS_Sn_STATUS_LOWER_STATUS_SHFT                                                      0xb
#define HWIO_MPM2_TSENS_Sn_STATUS_LOWER_STATUS_LOWER_THRESHOLD_NOT_VIOLATED_FVAL                         0x0
#define HWIO_MPM2_TSENS_Sn_STATUS_LOWER_STATUS_LOWER_THRESHOLD_VIOLATED_FVAL                             0x1
#define HWIO_MPM2_TSENS_Sn_STATUS_MIN_STATUS_BMSK                                                      0x400
#define HWIO_MPM2_TSENS_Sn_STATUS_MIN_STATUS_SHFT                                                        0xa
#define HWIO_MPM2_TSENS_Sn_STATUS_MIN_STATUS_MIN_THRESHOLD_NOT_VIOLATED_FVAL                             0x0
#define HWIO_MPM2_TSENS_Sn_STATUS_MIN_STATUS_MIN_THRESHOLD_VIOLATED_FVAL                                 0x1
#define HWIO_MPM2_TSENS_Sn_STATUS_LAST_TEMP_BMSK                                                       0x3ff
#define HWIO_MPM2_TSENS_Sn_STATUS_LAST_TEMP_SHFT                                                         0x0

#define HWIO_MPM2_TSENS_TRDY_ADDR                                                                 (MPM2_TSENS_TM_REG_BASE      + 0x00000084)
#define HWIO_MPM2_TSENS_TRDY_OFFS                                                                 (MPM2_TSENS_TM_REG_BASE_OFFS + 0x00000084)
#define HWIO_MPM2_TSENS_TRDY_RMSK                                                                        0x7
#define HWIO_MPM2_TSENS_TRDY_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_TRDY_ADDR, HWIO_MPM2_TSENS_TRDY_RMSK)
#define HWIO_MPM2_TSENS_TRDY_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_TRDY_ADDR, m)
#define HWIO_MPM2_TSENS_TRDY_OSC_CLK_OFF_BMSK                                                            0x4
#define HWIO_MPM2_TSENS_TRDY_OSC_CLK_OFF_SHFT                                                            0x2
#define HWIO_MPM2_TSENS_TRDY_OSC_CLK_OFF_CLK_IS_ON_FVAL                                                  0x0
#define HWIO_MPM2_TSENS_TRDY_OSC_CLK_OFF_CLK_IS_OFF_FVAL                                                 0x1
#define HWIO_MPM2_TSENS_TRDY_SLP_CLK_OFF_BMSK                                                            0x2
#define HWIO_MPM2_TSENS_TRDY_SLP_CLK_OFF_SHFT                                                            0x1
#define HWIO_MPM2_TSENS_TRDY_SLP_CLK_OFF_CLK_IS_ON_FVAL                                                  0x0
#define HWIO_MPM2_TSENS_TRDY_SLP_CLK_OFF_CLK_IS_OFF_FVAL                                                 0x1
#define HWIO_MPM2_TSENS_TRDY_TRDY_BMSK                                                                   0x1
#define HWIO_MPM2_TSENS_TRDY_TRDY_SHFT                                                                   0x0
#define HWIO_MPM2_TSENS_TRDY_TRDY_TEMPERATURE_MEASUREMENT_IN_PROGRESS_FVAL                               0x0
#define HWIO_MPM2_TSENS_TRDY_TRDY_TEMPERATURE_READING_IS_READY_FVAL                                      0x1

#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_ADDR                                        (MPM2_TSENS_TM_REG_BASE      + 0x00000088)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_OFFS                                        (MPM2_TSENS_TM_REG_BASE_OFFS + 0x00000088)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_RMSK                                            0xffff
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_ADDR, HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_RMSK)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_ADDR, m)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_CPU_HIGH_INT_BMSK                               0xff00
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_CPU_HIGH_INT_SHFT                                  0x8
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_CPU_LOW_INT_BMSK                                  0xff
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_CPU_LOW_INT_SHFT                                   0x0

#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_ADDR                                      (MPM2_TSENS_TM_REG_BASE      + 0x0000008c)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_OFFS                                      (MPM2_TSENS_TM_REG_BASE_OFFS + 0x0000008c)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_RMSK                                         0x3ffff
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_ADDR, HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_RMSK)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_ADDR, m)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_ADDR,v)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_ADDR,m,v,HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_IN)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_CPU_HIGH_OVERRIDE_EN_BMSK                    0x20000
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_CPU_HIGH_OVERRIDE_EN_SHFT                       0x11
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_CPU_LOW_OVERRIDE_EN_BMSK                     0x10000
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_CPU_LOW_OVERRIDE_EN_SHFT                        0x10
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_CPU_HIGH_INT_OVERRIDE_VALUE_BMSK              0xff00
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_CPU_HIGH_INT_OVERRIDE_VALUE_SHFT                 0x8
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_CPU_LOW_INT_OVERRIDE_VALUE_BMSK                 0xff
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_CPU_LOW_INT_OVERRIDE_VALUE_SHFT                  0x0

#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_ADDR(n)                                      (MPM2_TSENS_TM_REG_BASE      + 0x00000090 + 0x4 * (n))
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_OFFS(n)                                      (MPM2_TSENS_TM_REG_BASE_OFFS + 0x00000090 + 0x4 * (n))
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_RMSK                                           0x3fffff
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_MAXn                                                  7
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_ADDR(n), HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_RMSK)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_ADDR(n), mask)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_OUTI(n,val)    \
        out_dword(HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_ADDR(n),val)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_ADDR(n),mask,val,HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_INI(n))
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_CPU_HIGH_INT_MASK_BMSK                         0x200000
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_CPU_HIGH_INT_MASK_SHFT                             0x15
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_CPU_LOW_INT_MASK_BMSK                          0x100000
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_CPU_LOW_INT_MASK_SHFT                              0x14
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_CPU_HIGH_THRESHOLD_BMSK                         0xffc00
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_CPU_HIGH_THRESHOLD_SHFT                             0xa
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_CPU_LOW_THRESHOLD_BMSK                            0x3ff
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_CPU_LOW_THRESHOLD_SHFT                              0x0

#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_ADDR(n)                                                 (MPM2_TSENS_TM_REG_BASE      + 0x000000c0 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_OFFS(n)                                                 (MPM2_TSENS_TM_REG_BASE_OFFS + 0x000000c0 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_RMSK                                                           0x3
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_MAXn                                                             3
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_ADDR(n), HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_RMSK)
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_ADDR(n), mask)
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_OUTI(n,val)    \
        out_dword(HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_ADDR(n),val)
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_ADDR(n),mask,val,HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_INI(n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_TH2_MTC_IN_EFFECT_BMSK                                         0x2
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_TH2_MTC_IN_EFFECT_SHFT                                         0x1
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_TH1_MTC_IN_EFFECT_BMSK                                         0x1
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_TH1_MTC_IN_EFFECT_SHFT                                         0x0

#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_ADDR(n)                                                     (MPM2_TSENS_TM_REG_BASE      + 0x000000d0 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_OFFS(n)                                                     (MPM2_TSENS_TM_REG_BASE_OFFS + 0x000000d0 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_RMSK                                                        0x7fffffff
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_MAXn                                                                 3
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_ZONEn_LOG_ADDR(n), HWIO_MPM2_TSENS_MTC_ZONEn_LOG_RMSK)
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_ZONEn_LOG_ADDR(n), mask)
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOGS_VALID_BMSK                                  0x40000000
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOGS_VALID_SHFT                                        0x1e
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG5_BMSK                                        0x3e000000
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG5_SHFT                                              0x19
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG4_BMSK                                         0x1f00000
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG4_SHFT                                              0x14
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG3_BMSK                                           0xf8000
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG3_SHFT                                               0xf
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG2_BMSK                                            0x7c00
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG2_SHFT                                               0xa
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG1_BMSK                                             0x3e0
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG1_SHFT                                               0x5
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LATEST_BMSK                                            0x1f
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LATEST_SHFT                                             0x0

#define HWIO_MPM2_TSENS_TM_SPARE_ADDR                                                             (MPM2_TSENS_TM_REG_BASE      + 0x000000e0)
#define HWIO_MPM2_TSENS_TM_SPARE_OFFS                                                             (MPM2_TSENS_TM_REG_BASE_OFFS + 0x000000e0)
#define HWIO_MPM2_TSENS_TM_SPARE_RMSK                                                             0xffffffff
#define HWIO_MPM2_TSENS_TM_SPARE_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_TM_SPARE_ADDR, HWIO_MPM2_TSENS_TM_SPARE_RMSK)
#define HWIO_MPM2_TSENS_TM_SPARE_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_TM_SPARE_ADDR, m)
#define HWIO_MPM2_TSENS_TM_SPARE_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_TM_SPARE_ADDR,v)
#define HWIO_MPM2_TSENS_TM_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_TM_SPARE_ADDR,m,v,HWIO_MPM2_TSENS_TM_SPARE_IN)
#define HWIO_MPM2_TSENS_TM_SPARE_VALUE_BMSK                                                       0xffffffff
#define HWIO_MPM2_TSENS_TM_SPARE_VALUE_SHFT                                                              0x0

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                         (gpuSecCtrlBase      + 0x00000000)
#define SECURITY_CONTROL_CORE_REG_BASE_OFFS                                                    0x00000000

#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041d8)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_OFFS                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041d8)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_IN)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS6_POINT2_BMSK                                   0xfc000000
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS6_POINT2_SHFT                                         0x1a
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS6_POINT1_BMSK                                    0x3f00000
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS6_POINT1_SHFT                                         0x14
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS5_POINT2_BMSK                                      0xfc000
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS5_POINT2_SHFT                                          0xe
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS5_POINT1_BMSK                                       0x3f00
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS5_POINT1_SHFT                                          0x8
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS_BASE0_BMSK                                           0xff
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS_BASE0_SHFT                                            0x0

#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041dc)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_OFFS                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041dc)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS_BASE1_BMSK                                     0xff000000
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS_BASE1_SHFT                                           0x18
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS8_POINT2_BMSK                                     0xfc0000
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS8_POINT2_SHFT                                         0x12
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS8_POINT1_BMSK                                      0x3f000
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS8_POINT1_SHFT                                          0xc
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS7_POINT2_BMSK                                        0xfc0
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS7_POINT2_SHFT                                          0x6
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS7_POINT1_BMSK                                         0x3f
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS7_POINT1_SHFT                                          0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004210)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_OFFS                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004210)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS2_POINT1_4_0_BMSK                                0xf8000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS2_POINT1_4_0_SHFT                                      0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS1_POINT2_BMSK                                     0x7e00000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS1_POINT2_SHFT                                          0x15
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS1_POINT1_BMSK                                      0x1f8000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS1_POINT1_SHFT                                           0xf
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS0_POINT2_BMSK                                        0x7e00
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS0_POINT2_SHFT                                           0x9
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS0_POINT1_BMSK                                         0x1f8
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS0_POINT1_SHFT                                           0x3
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS_CALIB_BMSK                                             0x7
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS_CALIB_SHFT                                             0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004214)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_OFFS                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004214)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK                                                  0x7fffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS4_POINT2_BMSK                                    0x7e000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS4_POINT2_SHFT                                          0x19
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS4_POINT1_BMSK                                     0x1f80000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS4_POINT1_SHFT                                          0x13
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS3_POINT2_BMSK                                       0x7e000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS3_POINT2_SHFT                                           0xd
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS3_POINT1_BMSK                                        0x1f80
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS3_POINT1_SHFT                                           0x7
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS2_POINT2_BMSK                                          0x7e
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS2_POINT2_SHFT                                           0x1
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS2_POINT1_5_BMSK                                         0x1
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS2_POINT1_5_SHFT                                         0x0

#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004230)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_OFFS                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004230)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR2_TARG_VOLT_NOM_0_BMSK                             0xe0000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR2_TARG_VOLT_NOM_0_SHFT                                   0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR2_TARG_VOLT_TURBO_BMSK                             0x1f000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR2_TARG_VOLT_TURBO_SHFT                                   0x18
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_TSENS10_POINT2_BMSK                                     0xfc0000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_TSENS10_POINT2_SHFT                                         0x12
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_TSENS10_POINT1_BMSK                                      0x3f000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_TSENS10_POINT1_SHFT                                          0xc
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_TSENS9_POINT2_BMSK                                         0xfc0
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_TSENS9_POINT2_SHFT                                           0x6
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_TSENS9_POINT1_BMSK                                          0x3f
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_TSENS9_POINT1_SHFT                                           0x0

#endif /* HAL_HWIO_TSENS_H */
