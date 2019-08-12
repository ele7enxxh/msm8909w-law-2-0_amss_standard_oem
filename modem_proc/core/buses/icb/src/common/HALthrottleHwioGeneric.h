#ifndef __HALTHROTTLEHWIOGENERIC_H__
#define __HALTHROTTLEHWIOGENERIC_H__
/*
===========================================================================
*/
/**
  @file HALthrottleHwioGeneric.h
  @brief Generic hwio for the QIMP THROTTLE block.

*/
/*
  ===========================================================================

  Copyright (c) 2015 QUALCOMM Technologies, Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/common/HALthrottleHwioGeneric.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: THROTTLE
 *--------------------------------------------------------------------------*/
#define THROTTLE_REG_BASE(b)                                       ((uint8_t *)(b))

#define THROTTLE_CNTRL_ADDR(b,n)                                   (THROTTLE_REG_BASE(b)   + 0x00000000 + 0x200 * (n))
#define THROTTLE_CNTRL_RMSK                                               0x1
#define THROTTLE_CNTRL_INI(b,n)     \
        in_dword_masked(THROTTLE_CNTRL_ADDR(b,n), THROTTLE_CNTRL_RMSK)
#define THROTTLE_CNTRL_INMI(b,n,m) \
        in_dword_masked(THROTTLE_CNTRL_ADDR(b,n), m)
#define THROTTLE_CNTRL_OUTI(b,n,v) \
        out_dword(THROTTLE_CNTRL_ADDR(b,n),v)
#define THROTTLE_CNTRL_OUTMI(b,n,m,v) \
        out_dword_masked_ns(THROTTLE_CNTRL_ADDR(b,n),m,v,THROTTLE_CNTRL_INI(b,n))
#define THROTTLE_CNTRL_THROTTLE_EN_BMSK                                   0x1
#define THROTTLE_CNTRL_THROTTLE_EN_SHFT                                   0x0

#define THROTTLE_CGC_CNTRL_ADDR(b,n)                               (THROTTLE_REG_BASE(b)   + 0x00000004 + 0x200 * (n))
#define THROTTLE_CGC_CNTRL_RMSK                                           0x1
#define THROTTLE_CGC_CNTRL_INI(b,n)     \
        in_dword_masked(THROTTLE_CGC_CNTRL_ADDR(b,n), THROTTLE_CGC_CNTRL_RMSK)
#define THROTTLE_CGC_CNTRL_INMI(b,n,m) \
        in_dword_masked(THROTTLE_CGC_CNTRL_ADDR(b,n), m)
#define THROTTLE_CGC_CNTRL_OUTI(b,n,v) \
        out_dword(THROTTLE_CGC_CNTRL_ADDR(b,n),v)
#define THROTTLE_CGC_CNTRL_OUTMI(b,n,m,v) \
        out_dword_masked_ns(THROTTLE_CGC_CNTRL_ADDR(b,n),m,v,THROTTLE_CGC_CNTRL_INI(b,n))
#define THROTTLE_CGC_CNTRL_THROTTLE_CGC_EN_BMSK                           0x1
#define THROTTLE_CGC_CNTRL_THROTTLE_CGC_EN_SHFT                           0x0

#define THROTTLE_RESET_CNTRL_ADDR(b,n)                             (THROTTLE_REG_BASE(b)   + 0x00000008 + 0x200 * (n))
#define THROTTLE_RESET_CNTRL_RMSK                                         0x1
#define THROTTLE_RESET_CNTRL_INI(b,n)     \
        in_dword_masked(THROTTLE_RESET_CNTRL_ADDR(b,n), THROTTLE_RESET_CNTRL_RMSK)
#define THROTTLE_RESET_CNTRL_INMI(b,n,m) \
        in_dword_masked(THROTTLE_RESET_CNTRL_ADDR(b,n), m)
#define THROTTLE_RESET_CNTRL_OUTI(b,n,v) \
        out_dword(THROTTLE_RESET_CNTRL_ADDR(b,n),v)
#define THROTTLE_RESET_CNTRL_OUTMI(b,n,m,v) \
        out_dword_masked_ns(THROTTLE_RESET_CNTRL_ADDR(b,n),m,v,THROTTLE_RESET_CNTRL_INI(b,n))
#define THROTTLE_RESET_CNTRL_CORE_SW_RESET_BMSK                           0x1
#define THROTTLE_RESET_CNTRL_CORE_SW_RESET_SHFT                           0x0

#define THROTTLE_CONFIG_ADDR(b,n)                                  (THROTTLE_REG_BASE(b)   + 0x0000000c + 0x200 * (n))
#define THROTTLE_CONFIG_RMSK                                              0x7
#define THROTTLE_CONFIG_INI(b,n)     \
        in_dword_masked(THROTTLE_CONFIG_ADDR(b,n), THROTTLE_CONFIG_RMSK)
#define THROTTLE_CONFIG_INMI(b,n,m) \
        in_dword_masked(THROTTLE_CONFIG_ADDR(b,n), m)
#define THROTTLE_CONFIG_OUTI(b,n,v) \
        out_dword(THROTTLE_CONFIG_ADDR(b,n),v)
#define THROTTLE_CONFIG_OUTMI(b,n,m,v) \
        out_dword_masked_ns(THROTTLE_CONFIG_ADDR(b,n),m,v,THROTTLE_CONFIG_INI(b,n))
#define THROTTLE_CONFIG_THROTTLE_CNTRL_MAX_SKEW_BMSK                      0x7
#define THROTTLE_CONFIG_THROTTLE_CNTRL_MAX_SKEW_SHFT                      0x0

#define THROTTLE_GRANT_PERIOD_ADDR(b,n)                            (THROTTLE_REG_BASE(b)   + 0x00000010 + 0x200 * (n))
#define THROTTLE_GRANT_PERIOD_RMSK                                     0x3fff
#define THROTTLE_GRANT_PERIOD_INI(b,n)     \
        in_dword_masked(THROTTLE_GRANT_PERIOD_ADDR(b,n), THROTTLE_GRANT_PERIOD_RMSK)
#define THROTTLE_GRANT_PERIOD_INMI(b,n,m) \
        in_dword_masked(THROTTLE_GRANT_PERIOD_ADDR(b,n), m)
#define THROTTLE_GRANT_PERIOD_OUTI(b,n,v) \
        out_dword(THROTTLE_GRANT_PERIOD_ADDR(b,n),v)
#define THROTTLE_GRANT_PERIOD_OUTMI(b,n,m,v) \
        out_dword_masked_ns(THROTTLE_GRANT_PERIOD_ADDR(b,n),m,v,THROTTLE_GRANT_PERIOD_INI(b,n))
#define THROTTLE_GRANT_PERIOD_GRANT_PERIOD_BMSK                        0x3fff
#define THROTTLE_GRANT_PERIOD_GRANT_PERIOD_SHFT                           0x0

#define THROTTLE_GRANT_COUNT_ADDR(b,n)                             (THROTTLE_REG_BASE(b)   + 0x00000014 + 0x200 * (n))
#define THROTTLE_GRANT_COUNT_RMSK                                      0xffff
#define THROTTLE_GRANT_COUNT_INI(b,n)     \
        in_dword_masked(THROTTLE_GRANT_COUNT_ADDR(b,n), THROTTLE_GRANT_COUNT_RMSK)
#define THROTTLE_GRANT_COUNT_INMI(b,n,m) \
        in_dword_masked(THROTTLE_GRANT_COUNT_ADDR(b,n), m)
#define THROTTLE_GRANT_COUNT_OUTI(b,n,v) \
        out_dword(THROTTLE_GRANT_COUNT_ADDR(b,n),v)
#define THROTTLE_GRANT_COUNT_OUTMI(b,n,m,v) \
        out_dword_masked_ns(THROTTLE_GRANT_COUNT_ADDR(b,n),m,v,THROTTLE_GRANT_COUNT_INI(b,n))
#define THROTTLE_GRANT_COUNT_GRANT_COUNT_BMSK                          0xffff
#define THROTTLE_GRANT_COUNT_GRANT_COUNT_SHFT                             0x0

#define THROTTLE_THRESHOLD_i_ADDR(b,n,i)                          (THROTTLE_REG_BASE(b)   + 0x00000018 + 0x200 * (n) + 0x4 * (i))
#define THROTTLE_THRESHOLD_i_RMSK                                 0xffffffff
#define THROTTLE_THRESHOLD_i_INI2(b,n,i)   \
        in_dword_masked(THROTTLE_THRESHOLD_i_ADDR(b,n,i), THROTTLE_THRESHOLD_i_RMSK)
#define THROTTLE_THRESHOLD_i_INMI2(b,n,i,m) \
        in_dword_masked(THROTTLE_THRESHOLD_i_ADDR(b,n,i), m)
#define THROTTLE_THRESHOLD_i_OUTI2(b,n,i,v) \
        out_dword(THROTTLE_THRESHOLD_i_ADDR(b,n,i),v)
#define THROTTLE_THRESHOLD_i_OUTMI2(b,n,i,m,v) \
        out_dword_masked_ns(THROTTLE_THRESHOLD_i_ADDR(b,n,i),m,v,THROTTLE_THRESHOLD_i_INI2(b,n,i))
#define THROTTLE_THRESHOLD_i_THRESHOLD_LEVEL_0_BMSK               0xffff0000
#define THROTTLE_THRESHOLD_i_THRESHOLD_LEVEL_0_SHFT                     0x10
#define THROTTLE_THRESHOLD_i_THRESHOLD_LEVEL_1_BMSK                   0xffff
#define THROTTLE_THRESHOLD_i_THRESHOLD_LEVEL_1_SHFT                      0x0

#define THROTTLE_THRESHOLD_00_ADDR(b,n)                            (THROTTLE_REG_BASE(b)   + 0x00000018 + 0x200 * (n))
#define THROTTLE_THRESHOLD_00_RMSK                                 0xffffffff
#define THROTTLE_THRESHOLD_00_INI(b,n)     \
        in_dword_masked(THROTTLE_THRESHOLD_00_ADDR(b,n), THROTTLE_THRESHOLD_00_RMSK)
#define THROTTLE_THRESHOLD_00_INMI(b,n,m) \
        in_dword_masked(THROTTLE_THRESHOLD_00_ADDR(b,n), m)
#define THROTTLE_THRESHOLD_00_OUTI(b,n,v) \
        out_dword(THROTTLE_THRESHOLD_00_ADDR(b,n),v)
#define THROTTLE_THRESHOLD_00_OUTMI(b,n,m,v) \
        out_dword_masked_ns(THROTTLE_THRESHOLD_00_ADDR(b,n),m,v,THROTTLE_THRESHOLD_00_INI(b,n))
#define THROTTLE_THRESHOLD_00_THRESHOLD_LEVEL_6_BMSK               0xffff0000
#define THROTTLE_THRESHOLD_00_THRESHOLD_LEVEL_6_SHFT                     0x10
#define THROTTLE_THRESHOLD_00_THRESHOLD_LEVEL_7_BMSK                   0xffff
#define THROTTLE_THRESHOLD_00_THRESHOLD_LEVEL_7_SHFT                      0x0

#define THROTTLE_THRESHOLD_01_ADDR(b,n)                            (THROTTLE_REG_BASE(b)   + 0x0000001c + 0x200 * (n))
#define THROTTLE_THRESHOLD_01_RMSK                                 0xffffffff
#define THROTTLE_THRESHOLD_01_INI(b,n)     \
        in_dword_masked(THROTTLE_THRESHOLD_01_ADDR(b,n), THROTTLE_THRESHOLD_01_RMSK)
#define THROTTLE_THRESHOLD_01_INMI(b,n,m) \
        in_dword_masked(THROTTLE_THRESHOLD_01_ADDR(b,n), m)
#define THROTTLE_THRESHOLD_01_OUTI(b,n,v) \
        out_dword(THROTTLE_THRESHOLD_01_ADDR(b,n),v)
#define THROTTLE_THRESHOLD_01_OUTMI(b,n,m,v) \
        out_dword_masked_ns(THROTTLE_THRESHOLD_01_ADDR(b,n),m,v,THROTTLE_THRESHOLD_01_INI(b,n))
#define THROTTLE_THRESHOLD_01_THRESHOLD_LEVEL_4_BMSK               0xffff0000
#define THROTTLE_THRESHOLD_01_THRESHOLD_LEVEL_4_SHFT                     0x10
#define THROTTLE_THRESHOLD_01_THRESHOLD_LEVEL_5_BMSK                   0xffff
#define THROTTLE_THRESHOLD_01_THRESHOLD_LEVEL_5_SHFT                      0x0

#define THROTTLE_THRESHOLD_02_ADDR(b,n)                            (THROTTLE_REG_BASE(b)   + 0x00000020 + 0x200 * (n))
#define THROTTLE_THRESHOLD_02_RMSK                                 0xffffffff
#define THROTTLE_THRESHOLD_02_INI(b,n)     \
        in_dword_masked(THROTTLE_THRESHOLD_02_ADDR(b,n), THROTTLE_THRESHOLD_02_RMSK)
#define THROTTLE_THRESHOLD_02_INMI(b,n,m) \
        in_dword_masked(THROTTLE_THRESHOLD_02_ADDR(b,n), m)
#define THROTTLE_THRESHOLD_02_OUTI(b,n,v) \
        out_dword(THROTTLE_THRESHOLD_02_ADDR(b,n),v)
#define THROTTLE_THRESHOLD_02_OUTMI(b,n,m,v) \
        out_dword_masked_ns(THROTTLE_THRESHOLD_02_ADDR(b,n),m,v,THROTTLE_THRESHOLD_02_INI(b,n))
#define THROTTLE_THRESHOLD_02_THRESHOLD_LEVEL_2_BMSK               0xffff0000
#define THROTTLE_THRESHOLD_02_THRESHOLD_LEVEL_2_SHFT                     0x10
#define THROTTLE_THRESHOLD_02_THRESHOLD_LEVEL_3_BMSK                   0xffff
#define THROTTLE_THRESHOLD_02_THRESHOLD_LEVEL_3_SHFT                      0x0

#define THROTTLE_THRESHOLD_03_ADDR(b,n)                            (THROTTLE_REG_BASE(b)   + 0x00000024 + 0x200 * (n))
#define THROTTLE_THRESHOLD_03_RMSK                                     0xffff
#define THROTTLE_THRESHOLD_03_INI(b,n)     \
        in_dword_masked(THROTTLE_THRESHOLD_03_ADDR(b,n), THROTTLE_THRESHOLD_03_RMSK)
#define THROTTLE_THRESHOLD_03_INMI(b,n,m) \
        in_dword_masked(THROTTLE_THRESHOLD_03_ADDR(b,n), m)
#define THROTTLE_THRESHOLD_03_OUTI(b,n,v) \
        out_dword(THROTTLE_THRESHOLD_03_ADDR(b,n),v)
#define THROTTLE_THRESHOLD_03_OUTMI(b,n,m,v) \
        out_dword_masked_ns(THROTTLE_THRESHOLD_03_ADDR(b,n),m,v,THROTTLE_THRESHOLD_03_INI(b,n))
#define THROTTLE_THRESHOLD_03_THRESHOLD_LEVEL_1_BMSK                   0xffff
#define THROTTLE_THRESHOLD_03_THRESHOLD_LEVEL_1_SHFT                      0x0

#define THROTTLE_PEAK_ACCUM_CREDIT_ADDR(b,n)                       (THROTTLE_REG_BASE(b)   + 0x00000028 + 0x200 * (n))
#define THROTTLE_PEAK_ACCUM_CREDIT_RMSK                               0xfffff
#define THROTTLE_PEAK_ACCUM_CREDIT_INI(b,n)     \
        in_dword_masked(THROTTLE_PEAK_ACCUM_CREDIT_ADDR(b,n), THROTTLE_PEAK_ACCUM_CREDIT_RMSK)
#define THROTTLE_PEAK_ACCUM_CREDIT_INMI(b,n,m) \
        in_dword_masked(THROTTLE_PEAK_ACCUM_CREDIT_ADDR(b,n), m)
#define THROTTLE_PEAK_ACCUM_CREDIT_OUTI(b,n,v) \
        out_dword(THROTTLE_PEAK_ACCUM_CREDIT_ADDR(b,n),v)
#define THROTTLE_PEAK_ACCUM_CREDIT_OUTMI(b,n,m,v) \
        out_dword_masked_ns(THROTTLE_PEAK_ACCUM_CREDIT_ADDR(b,n),m,v,THROTTLE_PEAK_ACCUM_CREDIT_INI(b,n))
#define THROTTLE_PEAK_ACCUM_CREDIT_PEAK_ACCUM_CREDIT_BMSK             0xfffff
#define THROTTLE_PEAK_ACCUM_CREDIT_PEAK_ACCUM_CREDIT_SHFT                 0x0

#define THROTTLE_DEBUG_CNTL_ADDR(b,n)                              (THROTTLE_REG_BASE(b)   + 0x00000030 + 0x200 * (n))
#define THROTTLE_DEBUG_CNTL_RMSK                                        0xf01
#define THROTTLE_DEBUG_CNTL_INI(b,n)     \
        in_dword_masked(THROTTLE_DEBUG_CNTL_ADDR(b,n), THROTTLE_DEBUG_CNTL_RMSK)
#define THROTTLE_DEBUG_CNTL_INMI(b,n,m) \
        in_dword_masked(THROTTLE_DEBUG_CNTL_ADDR(b,n), m)
#define THROTTLE_DEBUG_CNTL_OUTI(b,n,v) \
        out_dword(THROTTLE_DEBUG_CNTL_ADDR(b,n),v)
#define THROTTLE_DEBUG_CNTL_OUTMI(b,n,m,v) \
        out_dword_masked_ns(THROTTLE_DEBUG_CNTL_ADDR(b,n),m,v,THROTTLE_DEBUG_CNTL_INI(b,n))
#define THROTTLE_DEBUG_CNTL_DEBUG_SEL_BMSK                              0xf00
#define THROTTLE_DEBUG_CNTL_DEBUG_SEL_SHFT                                0x8
#define THROTTLE_DEBUG_CNTL_DEBUG_EN_BMSK                                 0x1
#define THROTTLE_DEBUG_CNTL_DEBUG_EN_SHFT                                 0x0

#define THROTTLE_DEBUG_READBACK_ADDR(b,n)                          (THROTTLE_REG_BASE(b)   + 0x00000034 + 0x200 * (n))
#define THROTTLE_DEBUG_READBACK_RMSK                               0xffffffff
#define THROTTLE_DEBUG_READBACK_INI(b,n)     \
        in_dword_masked(THROTTLE_DEBUG_READBACK_ADDR(b,n), THROTTLE_DEBUG_READBACK_RMSK)
#define THROTTLE_DEBUG_READBACK_INMI(b,n,m) \
        in_dword_masked(THROTTLE_DEBUG_READBACK_ADDR(b,n), m)
#define THROTTLE_DEBUG_READBACK_DEBUG_BUS_READBACK_BMSK            0xffffffff
#define THROTTLE_DEBUG_READBACK_DEBUG_BUS_READBACK_SHFT                   0x0

#define THROTTLE_SPARE_REGS_ADDR(b,n)                              (THROTTLE_REG_BASE(b)   + 0x00000040 + 0x200 * (n))
#define THROTTLE_SPARE_REGS_RMSK                                   0xffffffff
#define THROTTLE_SPARE_REGS_INI(b,n)     \
        in_dword_masked(THROTTLE_SPARE_REGS_ADDR(b,n), THROTTLE_SPARE_REGS_RMSK)
#define THROTTLE_SPARE_REGS_INMI(b,n,m) \
        in_dword_masked(THROTTLE_SPARE_REGS_ADDR(b,n), m)
#define THROTTLE_SPARE_REGS_OUTI(b,n,v) \
        out_dword(THROTTLE_SPARE_REGS_ADDR(b,n),v)
#define THROTTLE_SPARE_REGS_OUTMI(b,n,m,v) \
        out_dword_masked_ns(THROTTLE_SPARE_REGS_ADDR(b,n),m,v,THROTTLE_SPARE_REGS_INI(b,n))
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_31_BMSK                  0x80000000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_31_SHFT                        0x1f
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_30_BMSK                  0x40000000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_30_SHFT                        0x1e
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_29_BMSK                  0x20000000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_29_SHFT                        0x1d
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_28_BMSK                  0x10000000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_28_SHFT                        0x1c
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_27_BMSK                   0x8000000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_27_SHFT                        0x1b
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_26_BMSK                   0x4000000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_26_SHFT                        0x1a
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_25_BMSK                   0x2000000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_25_SHFT                        0x19
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_24_BMSK                   0x1000000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_24_SHFT                        0x18
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_23_BMSK                    0x800000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_23_SHFT                        0x17
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_22_BMSK                    0x400000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_22_SHFT                        0x16
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_21_BMSK                    0x200000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_21_SHFT                        0x15
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_20_BMSK                    0x100000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_20_SHFT                        0x14
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_19_BMSK                     0x80000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_19_SHFT                        0x13
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_18_BMSK                     0x40000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_18_SHFT                        0x12
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_17_BMSK                     0x20000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_17_SHFT                        0x11
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_16_BMSK                     0x10000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_16_SHFT                        0x10
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_15_BMSK                      0x8000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_15_SHFT                         0xf
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_14_BMSK                      0x4000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_14_SHFT                         0xe
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_13_BMSK                      0x2000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_13_SHFT                         0xd
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_12_BMSK                      0x1000
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_12_SHFT                         0xc
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_11_BMSK                       0x800
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_11_SHFT                         0xb
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_10_BMSK                       0x400
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_10_SHFT                         0xa
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_9_BMSK                        0x200
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_9_SHFT                          0x9
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_8_BMSK                        0x100
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_8_SHFT                          0x8
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_7_BMSK                         0x80
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_7_SHFT                          0x7
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_6_BMSK                         0x40
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_6_SHFT                          0x6
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_5_BMSK                         0x20
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_5_SHFT                          0x5
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_4_BMSK                         0x10
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_4_SHFT                          0x4
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_3_BMSK                          0x8
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_3_SHFT                          0x3
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_2_BMSK                          0x4
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_2_SHFT                          0x2
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_1_BMSK                          0x2
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_1_SHFT                          0x1
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_0_BMSK                          0x1
#define THROTTLE_SPARE_REGS_SPARE_REG_BIT_0_SHFT                          0x0

#endif /* __HALTHROTTLEHWIOGENERIC_H__ */
