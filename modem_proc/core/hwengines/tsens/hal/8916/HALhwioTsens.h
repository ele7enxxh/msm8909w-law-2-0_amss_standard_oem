#ifndef __HAL_HWIO_TSENS_H__
#define __HAL_HWIO_TSENS_H__
/*============================================================================
  @file HALhwioTsens.h

  Implementation of the TSENS HAL for 8916 - HWIO info was auto-generated

                Copyright (c) 2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/tsens/hal/8916/HALhwioTsens.h#1 $ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * MODULE: MPM2_TSENS
 *--------------------------------------------------------------------------*/

#define MPM2_TSENS_REG_BASE                                                                  (gpuMpm2MpmBase      + 0x00008000)

#define HWIO_MPM2_TSENS_CTRL_ADDR                                                            (MPM2_TSENS_REG_BASE      + 0x00000000)
#define HWIO_MPM2_TSENS_CTRL_RMSK                                                             0xffc3fff
#define HWIO_MPM2_TSENS_CTRL_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_CTRL_ADDR, HWIO_MPM2_TSENS_CTRL_RMSK)
#define HWIO_MPM2_TSENS_CTRL_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_CTRL_ADDR, m)
#define HWIO_MPM2_TSENS_CTRL_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_CTRL_ADDR,v)
#define HWIO_MPM2_TSENS_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_CTRL_ADDR,m,v,HWIO_MPM2_TSENS_CTRL_IN)
#define HWIO_MPM2_TSENS_CTRL_TSENS_CLAMP_BMSK                                                 0x8000000
#define HWIO_MPM2_TSENS_CTRL_TSENS_CLAMP_SHFT                                                      0x1b
#define HWIO_MPM2_TSENS_CTRL_TSENS_CLAMP_UNCLAMPED_FVAL                                             0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_CLAMP_CLAMPED_FVAL                                               0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_BYPASS_EN_BMSK                                             0x4000000
#define HWIO_MPM2_TSENS_CTRL_TSENS_BYPASS_EN_SHFT                                                  0x1a
#define HWIO_MPM2_TSENS_CTRL_TSENS_BYPASS_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_BYPASS_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_MEASURE_PERIOD_BMSK                                              0x3fc0000
#define HWIO_MPM2_TSENS_CTRL_MEASURE_PERIOD_SHFT                                                   0x12
#define HWIO_MPM2_TSENS_CTRL_SENSOR10_EN_BMSK                                                    0x2000
#define HWIO_MPM2_TSENS_CTRL_SENSOR10_EN_SHFT                                                       0xd
#define HWIO_MPM2_TSENS_CTRL_SENSOR10_EN_DISABLED_FVAL                                              0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR10_EN_ENABLED_FVAL                                               0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR9_EN_BMSK                                                     0x1000
#define HWIO_MPM2_TSENS_CTRL_SENSOR9_EN_SHFT                                                        0xc
#define HWIO_MPM2_TSENS_CTRL_SENSOR9_EN_DISABLED_FVAL                                               0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR9_EN_ENABLED_FVAL                                                0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR8_EN_BMSK                                                      0x800
#define HWIO_MPM2_TSENS_CTRL_SENSOR8_EN_SHFT                                                        0xb
#define HWIO_MPM2_TSENS_CTRL_SENSOR8_EN_DISABLED_FVAL                                               0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR8_EN_ENABLED_FVAL                                                0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR7_EN_BMSK                                                      0x400
#define HWIO_MPM2_TSENS_CTRL_SENSOR7_EN_SHFT                                                        0xa
#define HWIO_MPM2_TSENS_CTRL_SENSOR7_EN_DISABLED_FVAL                                               0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR7_EN_ENABLED_FVAL                                                0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR6_EN_BMSK                                                      0x200
#define HWIO_MPM2_TSENS_CTRL_SENSOR6_EN_SHFT                                                        0x9
#define HWIO_MPM2_TSENS_CTRL_SENSOR6_EN_DISABLED_FVAL                                               0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR6_EN_ENABLED_FVAL                                                0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR5_EN_BMSK                                                      0x100
#define HWIO_MPM2_TSENS_CTRL_SENSOR5_EN_SHFT                                                        0x8
#define HWIO_MPM2_TSENS_CTRL_SENSOR5_EN_DISABLED_FVAL                                               0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR5_EN_ENABLED_FVAL                                                0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR4_EN_BMSK                                                       0x80
#define HWIO_MPM2_TSENS_CTRL_SENSOR4_EN_SHFT                                                        0x7
#define HWIO_MPM2_TSENS_CTRL_SENSOR4_EN_DISABLED_FVAL                                               0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR4_EN_ENABLED_FVAL                                                0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR3_EN_BMSK                                                       0x40
#define HWIO_MPM2_TSENS_CTRL_SENSOR3_EN_SHFT                                                        0x6
#define HWIO_MPM2_TSENS_CTRL_SENSOR3_EN_DISABLED_FVAL                                               0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR3_EN_ENABLED_FVAL                                                0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR2_EN_BMSK                                                       0x20
#define HWIO_MPM2_TSENS_CTRL_SENSOR2_EN_SHFT                                                        0x5
#define HWIO_MPM2_TSENS_CTRL_SENSOR2_EN_DISABLED_FVAL                                               0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR2_EN_ENABLED_FVAL                                                0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR1_EN_BMSK                                                       0x10
#define HWIO_MPM2_TSENS_CTRL_SENSOR1_EN_SHFT                                                        0x4
#define HWIO_MPM2_TSENS_CTRL_SENSOR1_EN_DISABLED_FVAL                                               0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR1_EN_ENABLED_FVAL                                                0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR0_EN_BMSK                                                        0x8
#define HWIO_MPM2_TSENS_CTRL_SENSOR0_EN_SHFT                                                        0x3
#define HWIO_MPM2_TSENS_CTRL_SENSOR0_EN_DISABLED_FVAL                                               0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR0_EN_ENABLED_FVAL                                                0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_ADC_CLK_SEL_BMSK                                                 0x4
#define HWIO_MPM2_TSENS_CTRL_TSENS_ADC_CLK_SEL_SHFT                                                 0x2
#define HWIO_MPM2_TSENS_CTRL_TSENS_ADC_CLK_SEL_INTERNAL_OSCILLATOR_FVAL                             0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_ADC_CLK_SEL_EXTERNAL_CLOCK_SOURCE_FVAL                           0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_SW_RST_BMSK                                                      0x2
#define HWIO_MPM2_TSENS_CTRL_TSENS_SW_RST_SHFT                                                      0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_SW_RST_RESET_DEASSERTED_FVAL                                     0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_SW_RST_RESET_ASSERTED_FVAL                                       0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_EN_BMSK                                                          0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_EN_SHFT                                                          0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_EN_DISABLED_FVAL                                                 0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_EN_ENABLED_FVAL                                                  0x1

#define HWIO_MPM2_TSENS_TEST_CTRL_ADDR                                                       (MPM2_TSENS_REG_BASE      + 0x00000004)
#define HWIO_MPM2_TSENS_TEST_CTRL_RMSK                                                             0x1f
#define HWIO_MPM2_TSENS_TEST_CTRL_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_TEST_CTRL_ADDR, HWIO_MPM2_TSENS_TEST_CTRL_RMSK)
#define HWIO_MPM2_TSENS_TEST_CTRL_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_TEST_CTRL_ADDR, m)
#define HWIO_MPM2_TSENS_TEST_CTRL_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_TEST_CTRL_ADDR,v)
#define HWIO_MPM2_TSENS_TEST_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_TEST_CTRL_ADDR,m,v,HWIO_MPM2_TSENS_TEST_CTRL_IN)
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_BMSK                                              0x1e
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_SHFT                                               0x1
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_VREF_OF_SENSOR_0_FVAL                              0x0
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_IPTAT_OF_SENSOR_0_FVAL                             0x1
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_ICTAT_OF_SENSOR_0_FVAL                             0x2
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_ISENSE_OF_SENSOR_0_FVAL                            0x3
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_ISENSE_OF_SENSOR_1_FVAL                            0x4
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_ISENSE_OF_SENSOR_2_FVAL                            0x5
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_ISENSE_OF_SENSOR_3_FVAL                            0x6
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_ISENSE_OF_SENSOR_4_FVAL                            0x7
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_ISENSE_OF_SENSOR_5_FVAL                            0x8
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_ISENSE_OF_SENSOR_6_FVAL                            0x9
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_ISENSE_OF_SENSOR_7_FVAL                            0xa
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_ISENSE_OF_SENSOR_8_FVAL                            0xb
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_ISENSE_OF_SENSOR_9_FVAL                            0xc
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_ISENSE_OF_SENSOR_10_FVAL                           0xd
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_VBE_OF_SENSOR_0_FVAL                               0xe
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_VR_OF_SENSOR_0_FVAL                                0xf
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_EN_BMSK                                                0x1
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_EN_SHFT                                                0x0
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_EN_TEST_DISABLED_FVAL                                  0x0
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_EN_TEST_ENABLED_FVAL                                   0x1

#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_ADDR(n)                                       (MPM2_TSENS_REG_BASE      + 0x00000008 + 0x4 * (n))
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_RMSK                                            0x3fffff
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAXn                                                  10
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_ADDR(n), HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_RMSK)
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_ADDR(n), mask)
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_OUTI(n,val)    \
        out_dword(HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_ADDR(n),val)
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_ADDR(n),mask,val,HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_INI(n))
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAX_STATUS_MASK_BMSK                            0x200000
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAX_STATUS_MASK_SHFT                                0x15
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAX_STATUS_MASK_NORMAL_OPERATION_FVAL                0x0
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAX_STATUS_MASK_MASK_OFF_MAX_STATUS_FVAL             0x1
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MIN_STATUS_MASK_BMSK                            0x100000
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MIN_STATUS_MASK_SHFT                                0x14
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MIN_STATUS_MASK_NORMAL_OPERATION_FVAL                0x0
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MIN_STATUS_MASK_MASK_OFF_MIN_STATUS_FVAL             0x1
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAX_THRESHOLD_BMSK                               0xffc00
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAX_THRESHOLD_SHFT                                   0xa
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MIN_THRESHOLD_BMSK                                 0x3ff
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MIN_THRESHOLD_SHFT                                   0x0

#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_ADDR                                                   (MPM2_TSENS_REG_BASE      + 0x00000034)
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_RMSK                                                   0x1fffffff
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_GLOBAL_CONFIG_ADDR, HWIO_MPM2_TSENS_GLOBAL_CONFIG_RMSK)
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_GLOBAL_CONFIG_ADDR, m)
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_GLOBAL_CONFIG_ADDR,v)
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_GLOBAL_CONFIG_ADDR,m,v,HWIO_MPM2_TSENS_GLOBAL_CONFIG_IN)
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_REMOTE_DISABLE_PTAT_OPAMP_CHOPPING_BMSK          0x10000000
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_REMOTE_DISABLE_PTAT_OPAMP_CHOPPING_SHFT                0x1c
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_REMOTE_VBE_BMSK                                   0x8000000
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_REMOTE_VBE_SHFT                                        0x1b
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_REMOTE_VBE_VBE_MISSION_HIGH_FVAL                        0x0
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_REMOTE_VBE_VBE_TEST_LOW_FVAL                            0x1
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_REMOTE_CHOPPING_FREQ_BMSK                         0x6000000
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_REMOTE_CHOPPING_FREQ_SHFT                              0x19
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_REMOTE_DEM_FREQ_BMSK                              0x1800000
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_REMOTE_DEM_FREQ_SHFT                                   0x17
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_S0_VBE_BIT0_BMSK                              0x400000
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_S0_VBE_BIT0_SHFT                                  0x16
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_UNDEFINED_BMSK                                0x380000
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_UNDEFINED_SHFT                                    0x13
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_S0_VBE_BIT1_BMSK                               0x40000
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_S0_VBE_BIT1_SHFT                                  0x12
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_TURBO_MODE_EN_BMSK                             0x20000
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_TURBO_MODE_EN_SHFT                                0x11
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_TURBO_MODE_EN_DISABLED_FVAL                        0x0
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_TURBO_MODE_EN_ENABLED_FVAL                         0x1
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_QUANTIZER_OUT_EN_BMSK                          0x10000
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_QUANTIZER_OUT_EN_SHFT                             0x10
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_QUANTIZER_OUT_EN_DISABLED_FVAL                     0x0
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_QUANTIZER_OUT_EN_ENABLED_FVAL                      0x1
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_CLK_DEM3_EN_BMSK                                0x8000
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_CLK_DEM3_EN_SHFT                                   0xf
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_CLK_DEM3_EN_DISABLED_FVAL                          0x0
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_CLK_DEM3_EN_ENABLED_FVAL                           0x1
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_CLK_DEM2_EN_BMSK                                0x4000
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_CLK_DEM2_EN_SHFT                                   0xe
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_CLK_DEM2_EN_DISABLED_FVAL                          0x0
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_CLK_DEM2_EN_ENABLED_FVAL                           0x1
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_CLK_DEM1_EN_BMSK                                0x2000
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_CLK_DEM1_EN_SHFT                                   0xd
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_CLK_DEM1_EN_DISABLED_FVAL                          0x0
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_CLK_DEM1_EN_ENABLED_FVAL                           0x1
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_DEM_SEL_BMSK                                    0x1800
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_DEM_SEL_SHFT                                       0xb
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_SENSE_MODE_PTAT_BMSK                             0x400
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_SENSE_MODE_PTAT_SHFT                               0xa
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_CHOP_SEL_BMSK                                    0x300
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_CHOP_SEL_SHFT                                      0x8
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_OSC_CLK_DEBUG_EN_BMSK                             0x80
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_OSC_CLK_DEBUG_EN_SHFT                              0x7
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_OSC_CLK_DEBUG_EN_DISABLED_FVAL                     0x0
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_OSC_CLK_DEBUG_EN_ENABLED_FVAL                      0x1
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_CTAT_CHOPPING_EN_BMSK                             0x40
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_CTAT_CHOPPING_EN_SHFT                              0x6
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_CTAT_CHOPPING_EN_DISABLED_FVAL                     0x0
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_CTAT_CHOPPING_EN_ENABLED_FVAL                      0x1
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_PTAT_CHOPPING_EN_BMSK                             0x20
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_PTAT_CHOPPING_EN_SHFT                              0x5
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_PTAT_CHOPPING_EN_DISABLED_FVAL                     0x0
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CTRL_PTAT_CHOPPING_EN_ENABLED_FVAL                      0x1
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CFG_OSC_FREQ_BMSK                                      0x18
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CFG_OSC_FREQ_SHFT                                       0x3
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CFG_IIN_SLOPE_BMSK                                      0x7
#define HWIO_MPM2_TSENS_GLOBAL_CONFIG_TSENS_CFG_IIN_SLOPE_SHFT                                      0x0

#define HWIO_MPM2_TSENS_S0_MAIN_CONFIG_ADDR                                                  (MPM2_TSENS_REG_BASE      + 0x00000038)
#define HWIO_MPM2_TSENS_S0_MAIN_CONFIG_RMSK                                                       0x3ff
#define HWIO_MPM2_TSENS_S0_MAIN_CONFIG_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_S0_MAIN_CONFIG_ADDR, HWIO_MPM2_TSENS_S0_MAIN_CONFIG_RMSK)
#define HWIO_MPM2_TSENS_S0_MAIN_CONFIG_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_S0_MAIN_CONFIG_ADDR, m)
#define HWIO_MPM2_TSENS_S0_MAIN_CONFIG_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_S0_MAIN_CONFIG_ADDR,v)
#define HWIO_MPM2_TSENS_S0_MAIN_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_S0_MAIN_CONFIG_ADDR,m,v,HWIO_MPM2_TSENS_S0_MAIN_CONFIG_IN)
#define HWIO_MPM2_TSENS_S0_MAIN_CONFIG_TSENS_CFG_IREF_GAIN_BMSK                                   0x380
#define HWIO_MPM2_TSENS_S0_MAIN_CONFIG_TSENS_CFG_IREF_GAIN_SHFT                                     0x7
#define HWIO_MPM2_TSENS_S0_MAIN_CONFIG_TSENS_CTRL_CTAT_WEIGHT_BMSK                                 0x78
#define HWIO_MPM2_TSENS_S0_MAIN_CONFIG_TSENS_CTRL_CTAT_WEIGHT_SHFT                                  0x3
#define HWIO_MPM2_TSENS_S0_MAIN_CONFIG_TSENS_CTRL_IREF_SUBTRACTION_WEIGHT_BMSK                      0x7
#define HWIO_MPM2_TSENS_S0_MAIN_CONFIG_TSENS_CTRL_IREF_SUBTRACTION_WEIGHT_SHFT                      0x0

#define HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_ADDR(n)                                             (MPM2_TSENS_REG_BASE      + 0x0000003c + 0x4 * (n))
#define HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_RMSK                                                    0x1fff
#define HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_MAXn                                                        10
#define HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_ADDR(n), HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_RMSK)
#define HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_ADDR(n), mask)
#define HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_OUTI(n,val)    \
        out_dword(HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_ADDR(n),val)
#define HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_ADDR(n),mask,val,HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_INI(n))
#define HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_TSENS_REMOTE_ISENSE_WEIGHT_BMSK                         0x1c00
#define HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_TSENS_REMOTE_ISENSE_WEIGHT_SHFT                            0xa
#define HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_TSENS_CFG_IREF_GAIN_BMSK                                 0x380
#define HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_TSENS_CFG_IREF_GAIN_SHFT                                   0x7
#define HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_TSENS_CTRL_CTAT_WEIGHT_BMSK                               0x78
#define HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_TSENS_CTRL_CTAT_WEIGHT_SHFT                                0x3
#define HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_TSENS_CTRL_IREF_SUBTRACTION_WEIGHT_BMSK                    0x7
#define HWIO_MPM2_TSENS_Sn_REMOTE_CONFIG_TSENS_CTRL_IREF_SUBTRACTION_WEIGHT_SHFT                    0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_TSENS_TM
 *--------------------------------------------------------------------------*/

#define MPM2_TSENS_TM_REG_BASE                                                    (gpuMpm2MpmBase      + 0x00009000)

#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_ADDR                           (MPM2_TSENS_TM_REG_BASE      + 0x00000000)
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_RMSK                                  0x1
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_ADDR, HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_RMSK)
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_ADDR, m)
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_ADDR,v)
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_ADDR,m,v,HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_IN)
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_INTERRUPT_EN_BMSK                     0x1
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_INTERRUPT_EN_SHFT                     0x0
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_INTERRUPT_EN_DISABLED_FVAL            0x0
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_INTERRUPT_EN_ENABLED_FVAL             0x1

#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_ADDR(n)                        (MPM2_TSENS_TM_REG_BASE      + 0x00000004 + 0x4 * (n))
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_RMSK                             0x3fffff
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_MAXn                                   10
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_ADDR(n), HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_RMSK)
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_ADDR(n), mask)
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_OUTI(n,val)    \
        out_dword(HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_ADDR(n),val)
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_ADDR(n),mask,val,HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_INI(n))
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_UPPER_STATUS_CLR_BMSK            0x200000
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_UPPER_STATUS_CLR_SHFT                0x15
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_UPPER_STATUS_CLR_NORMAL_OPERATION_FVAL        0x0
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_UPPER_STATUS_CLR_CLEAR_UPPER_STATUS_FVAL        0x1
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_LOWER_STATUS_CLR_BMSK            0x100000
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_LOWER_STATUS_CLR_SHFT                0x14
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_LOWER_STATUS_CLR_NORMAL_OPERATION_FVAL        0x0
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_LOWER_STATUS_CLR_CLEAR_LOWER_STATUS_FVAL        0x1
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_UPPER_THRESHOLD_BMSK              0xffc00
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_UPPER_THRESHOLD_SHFT                  0xa
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_LOWER_THRESHOLD_BMSK                0x3ff
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_LOWER_THRESHOLD_SHFT                  0x0

#define HWIO_MPM2_TSENS_Sn_STATUS_ADDR(n)                                         (MPM2_TSENS_TM_REG_BASE      + 0x00000030 + 0x4 * (n))
#define HWIO_MPM2_TSENS_Sn_STATUS_RMSK                                                0x3fff
#define HWIO_MPM2_TSENS_Sn_STATUS_MAXn                                                    10
#define HWIO_MPM2_TSENS_Sn_STATUS_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_STATUS_ADDR(n), HWIO_MPM2_TSENS_Sn_STATUS_RMSK)
#define HWIO_MPM2_TSENS_Sn_STATUS_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_STATUS_ADDR(n), mask)
#define HWIO_MPM2_TSENS_Sn_STATUS_MAX_STATUS_BMSK                                     0x2000
#define HWIO_MPM2_TSENS_Sn_STATUS_MAX_STATUS_SHFT                                        0xd
#define HWIO_MPM2_TSENS_Sn_STATUS_MAX_STATUS_MAX_THRESHOLD_NOT_VIOLATED_FVAL             0x0
#define HWIO_MPM2_TSENS_Sn_STATUS_MAX_STATUS_MAX_THRESHOLD_VIOLATED_FVAL                 0x1
#define HWIO_MPM2_TSENS_Sn_STATUS_UPPER_STATUS_BMSK                                   0x1000
#define HWIO_MPM2_TSENS_Sn_STATUS_UPPER_STATUS_SHFT                                      0xc
#define HWIO_MPM2_TSENS_Sn_STATUS_UPPER_STATUS_UPPER_THRESHOLD_NOT_VIOLATED_FVAL         0x0
#define HWIO_MPM2_TSENS_Sn_STATUS_UPPER_STATUS_UPPER_THRESHOLD_VIOLATED_FVAL             0x1
#define HWIO_MPM2_TSENS_Sn_STATUS_LOWER_STATUS_BMSK                                    0x800
#define HWIO_MPM2_TSENS_Sn_STATUS_LOWER_STATUS_SHFT                                      0xb
#define HWIO_MPM2_TSENS_Sn_STATUS_LOWER_STATUS_LOWER_THRESHOLD_NOT_VIOLATED_FVAL         0x0
#define HWIO_MPM2_TSENS_Sn_STATUS_LOWER_STATUS_LOWER_THRESHOLD_VIOLATED_FVAL             0x1
#define HWIO_MPM2_TSENS_Sn_STATUS_MIN_STATUS_BMSK                                      0x400
#define HWIO_MPM2_TSENS_Sn_STATUS_MIN_STATUS_SHFT                                        0xa
#define HWIO_MPM2_TSENS_Sn_STATUS_MIN_STATUS_MIN_THRESHOLD_NOT_VIOLATED_FVAL             0x0
#define HWIO_MPM2_TSENS_Sn_STATUS_MIN_STATUS_MIN_THRESHOLD_VIOLATED_FVAL                 0x1
#define HWIO_MPM2_TSENS_Sn_STATUS_LAST_TEMP_BMSK                                       0x3ff
#define HWIO_MPM2_TSENS_Sn_STATUS_LAST_TEMP_SHFT                                         0x0

#define HWIO_MPM2_TSENS_TRDY_ADDR                                                 (MPM2_TSENS_TM_REG_BASE      + 0x0000005c)
#define HWIO_MPM2_TSENS_TRDY_RMSK                                                        0x7
#define HWIO_MPM2_TSENS_TRDY_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_TRDY_ADDR, HWIO_MPM2_TSENS_TRDY_RMSK)
#define HWIO_MPM2_TSENS_TRDY_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_TRDY_ADDR, m)
#define HWIO_MPM2_TSENS_TRDY_OSC_CLK_OFF_BMSK                                            0x4
#define HWIO_MPM2_TSENS_TRDY_OSC_CLK_OFF_SHFT                                            0x2
#define HWIO_MPM2_TSENS_TRDY_OSC_CLK_OFF_CLK_IS_ON_FVAL                                  0x0
#define HWIO_MPM2_TSENS_TRDY_OSC_CLK_OFF_CLK_IS_OFF_FVAL                                 0x1
#define HWIO_MPM2_TSENS_TRDY_SLP_CLK_OFF_BMSK                                            0x2
#define HWIO_MPM2_TSENS_TRDY_SLP_CLK_OFF_SHFT                                            0x1
#define HWIO_MPM2_TSENS_TRDY_SLP_CLK_OFF_CLK_IS_ON_FVAL                                  0x0
#define HWIO_MPM2_TSENS_TRDY_SLP_CLK_OFF_CLK_IS_OFF_FVAL                                 0x1
#define HWIO_MPM2_TSENS_TRDY_TRDY_BMSK                                                   0x1
#define HWIO_MPM2_TSENS_TRDY_TRDY_SHFT                                                   0x0
#define HWIO_MPM2_TSENS_TRDY_TRDY_TEMPERATURE_MEASUREMENT_IN_PROGRESS_FVAL               0x0
#define HWIO_MPM2_TSENS_TRDY_TRDY_TEMPERATURE_READING_IS_READY_FVAL                      0x1

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                        (gpuSecCtrlBase      + 0x00000000)

#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040d0)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS2_POINT1_BMSK                                    0xF8000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS2_POINT1_SHFT                                    27
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS1_POINT2_BMSK                                    0x07C00000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS1_POINT2_SHFT                                    22
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS1_POINT1_BMSK                                    0x003E0000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS1_POINT1_SHFT                                    17
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS0_POINT2_BMSK                                    0x0001F000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS0_POINT2_SHFT                                    12
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS0_POINT1_BMSK                                    0x00000F80
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS0_POINT1_SHFT                                    7
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS_BASE0_BMSK                                      0x0000007F
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS_BASE0_SHFT                                      0

#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE    + 0x000040D4)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS_BASE1_BMSK                                      0xFE000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS_BASE1_SHFT                                      25
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS4_POINT2_BMSK                                    0x01F00000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS4_POINT2_SHFT                                    20
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS4_POINT1_BMSK                                    0x000F8000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS4_POINT1_SHFT                                    15
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS3_POINT2_BMSK                                    0x00007C00
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS3_POINT2_SHFT                                    10
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS3_POINT1_BMSK                                    0x000003E0
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS3_POINT1_SHFT                                    5
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS2_POINT2_BMSK                                    0x0000001F
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS2_POINT2_SHFT                                    0

#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE   + 0x000040EC)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TSENS_CAL_SEL_BMSK                                    0xE0000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TSENS_CAL_SEL_SHFT                                    29
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_SPARE3_BMSK                                           0x1FF80000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_SPARE3_SHFT                                           19
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TXDAC1_STEP1_CORR_BMSK                                0x00078000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TXDAC1_STEP1_CORR_SHFT                                15
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TXDAC1_STEP0_CORR_BMSK                                0x00007800
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TXDAC1_STEP0_CORR_SHFT                                11
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TXDAC1_RANGE_CORR_BMSK                                0x00000400
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TXDAC1_RANGE_CORR_SHFT                                10
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TXDAC1_AVEG_CORR_BMSK                                 0x00000200
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TXDAC1_AVEG_CORR_SHFT                                 9
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TXDAC1_RPOLY_CAL_BMSK                                 0x000001FE
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TXDAC1_RPOLY_CAL_SHFT                                 1
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TXDAC0_STEP1_CORR_3_BMSK                              0x00000001
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TXDAC0_STEP1_CORR_3_SHFT                              0

#endif /* __HAL_HWIO_TSENS_H__ */
