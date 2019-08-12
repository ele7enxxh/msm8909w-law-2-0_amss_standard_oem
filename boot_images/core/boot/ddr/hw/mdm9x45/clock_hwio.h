#ifndef __CLOCKS__
#define __CLOCKS__
/*
===========================================================================
*/
/**
  @file clocks
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    GCC_CLK_CTL_REG

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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

  ===========================================================================

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/mdm9x45/clock_hwio.h#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $

  ===========================================================================
*/
/*----------------------------------------------------------------------------
 * MODULE: GCC_CLK_CTL_REG
 *--------------------------------------------------------------------------*/
#define CLK_CTL_BASE                                                0x01800000
#define GCC_CLK_CTL_REG_REG_BASE                                                                (CLK_CTL_BASE      + 0x00000000)

#define HWIO_GCC_GPLL0_MODE_ADDR                                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00021000)
#define HWIO_GCC_GPLL0_MODE_RMSK                                                                  0x3fff0f
#define HWIO_GCC_GPLL0_MODE_IN          \
        in_dword_masked(HWIO_GCC_GPLL0_MODE_ADDR, HWIO_GCC_GPLL0_MODE_RMSK)
#define HWIO_GCC_GPLL0_MODE_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL0_MODE_ADDR, m)
#define HWIO_GCC_GPLL0_MODE_OUT(v)      \
        out_dword(HWIO_GCC_GPLL0_MODE_ADDR,v)
#define HWIO_GCC_GPLL0_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL0_MODE_ADDR,m,v,HWIO_GCC_GPLL0_MODE_IN)
#define HWIO_GCC_GPLL0_MODE_PLL_VOTE_FSM_RESET_BMSK                                               0x200000
#define HWIO_GCC_GPLL0_MODE_PLL_VOTE_FSM_RESET_SHFT                                                   0x15
#define HWIO_GCC_GPLL0_MODE_PLL_VOTE_FSM_ENA_BMSK                                                 0x100000
#define HWIO_GCC_GPLL0_MODE_PLL_VOTE_FSM_ENA_SHFT                                                     0x14
#define HWIO_GCC_GPLL0_MODE_PLL_BIAS_COUNT_BMSK                                                    0xfc000
#define HWIO_GCC_GPLL0_MODE_PLL_BIAS_COUNT_SHFT                                                        0xe
#define HWIO_GCC_GPLL0_MODE_PLL_LOCK_COUNT_BMSK                                                     0x3f00
#define HWIO_GCC_GPLL0_MODE_PLL_LOCK_COUNT_SHFT                                                        0x8
#define HWIO_GCC_GPLL0_MODE_PLL_PLLTEST_BMSK                                                           0x8
#define HWIO_GCC_GPLL0_MODE_PLL_PLLTEST_SHFT                                                           0x3
#define HWIO_GCC_GPLL0_MODE_PLL_RESET_N_BMSK                                                           0x4
#define HWIO_GCC_GPLL0_MODE_PLL_RESET_N_SHFT                                                           0x2
#define HWIO_GCC_GPLL0_MODE_PLL_BYPASSNL_BMSK                                                          0x2
#define HWIO_GCC_GPLL0_MODE_PLL_BYPASSNL_SHFT                                                          0x1
#define HWIO_GCC_GPLL0_MODE_PLL_OUTCTRL_BMSK                                                           0x1
#define HWIO_GCC_GPLL0_MODE_PLL_OUTCTRL_SHFT                                                           0x0

#define HWIO_GCC_GPLL0_L_VAL_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00021004)
#define HWIO_GCC_GPLL0_L_VAL_RMSK                                                                     0xff
#define HWIO_GCC_GPLL0_L_VAL_IN          \
        in_dword_masked(HWIO_GCC_GPLL0_L_VAL_ADDR, HWIO_GCC_GPLL0_L_VAL_RMSK)
#define HWIO_GCC_GPLL0_L_VAL_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL0_L_VAL_ADDR, m)
#define HWIO_GCC_GPLL0_L_VAL_OUT(v)      \
        out_dword(HWIO_GCC_GPLL0_L_VAL_ADDR,v)
#define HWIO_GCC_GPLL0_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL0_L_VAL_ADDR,m,v,HWIO_GCC_GPLL0_L_VAL_IN)
#define HWIO_GCC_GPLL0_L_VAL_PLL_L_BMSK                                                               0xff
#define HWIO_GCC_GPLL0_L_VAL_PLL_L_SHFT                                                                0x0

#define HWIO_GCC_GPLL0_M_VAL_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00021008)
#define HWIO_GCC_GPLL0_M_VAL_RMSK                                                                  0x7ffff
#define HWIO_GCC_GPLL0_M_VAL_IN          \
        in_dword_masked(HWIO_GCC_GPLL0_M_VAL_ADDR, HWIO_GCC_GPLL0_M_VAL_RMSK)
#define HWIO_GCC_GPLL0_M_VAL_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL0_M_VAL_ADDR, m)
#define HWIO_GCC_GPLL0_M_VAL_OUT(v)      \
        out_dword(HWIO_GCC_GPLL0_M_VAL_ADDR,v)
#define HWIO_GCC_GPLL0_M_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL0_M_VAL_ADDR,m,v,HWIO_GCC_GPLL0_M_VAL_IN)
#define HWIO_GCC_GPLL0_M_VAL_PLL_M_BMSK                                                            0x7ffff
#define HWIO_GCC_GPLL0_M_VAL_PLL_M_SHFT                                                                0x0

#define HWIO_GCC_GPLL0_N_VAL_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0002100c)
#define HWIO_GCC_GPLL0_N_VAL_RMSK                                                                  0x7ffff
#define HWIO_GCC_GPLL0_N_VAL_IN          \
        in_dword_masked(HWIO_GCC_GPLL0_N_VAL_ADDR, HWIO_GCC_GPLL0_N_VAL_RMSK)
#define HWIO_GCC_GPLL0_N_VAL_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL0_N_VAL_ADDR, m)
#define HWIO_GCC_GPLL0_N_VAL_OUT(v)      \
        out_dword(HWIO_GCC_GPLL0_N_VAL_ADDR,v)
#define HWIO_GCC_GPLL0_N_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL0_N_VAL_ADDR,m,v,HWIO_GCC_GPLL0_N_VAL_IN)
#define HWIO_GCC_GPLL0_N_VAL_PLL_N_BMSK                                                            0x7ffff
#define HWIO_GCC_GPLL0_N_VAL_PLL_N_SHFT                                                                0x0

#define HWIO_GCC_GPLL0_USER_CTL_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00021010)
#define HWIO_GCC_GPLL0_USER_CTL_RMSK                                                            0xffffffff
#define HWIO_GCC_GPLL0_USER_CTL_IN          \
        in_dword_masked(HWIO_GCC_GPLL0_USER_CTL_ADDR, HWIO_GCC_GPLL0_USER_CTL_RMSK)
#define HWIO_GCC_GPLL0_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL0_USER_CTL_ADDR, m)
#define HWIO_GCC_GPLL0_USER_CTL_OUT(v)      \
        out_dword(HWIO_GCC_GPLL0_USER_CTL_ADDR,v)
#define HWIO_GCC_GPLL0_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL0_USER_CTL_ADDR,m,v,HWIO_GCC_GPLL0_USER_CTL_IN)
#define HWIO_GCC_GPLL0_USER_CTL_RESERVE_BITS31_25_BMSK                                          0xfe000000
#define HWIO_GCC_GPLL0_USER_CTL_RESERVE_BITS31_25_SHFT                                                0x19
#define HWIO_GCC_GPLL0_USER_CTL_MN_EN_BMSK                                                       0x1000000
#define HWIO_GCC_GPLL0_USER_CTL_MN_EN_SHFT                                                            0x18
#define HWIO_GCC_GPLL0_USER_CTL_RESERVE_BITS23_21_BMSK                                            0xe00000
#define HWIO_GCC_GPLL0_USER_CTL_RESERVE_BITS23_21_SHFT                                                0x15
#define HWIO_GCC_GPLL0_USER_CTL_VCO_SEL_BMSK                                                      0x100000
#define HWIO_GCC_GPLL0_USER_CTL_VCO_SEL_SHFT                                                          0x14
#define HWIO_GCC_GPLL0_USER_CTL_RESERVE_BITS19_13_BMSK                                             0xfe000
#define HWIO_GCC_GPLL0_USER_CTL_RESERVE_BITS19_13_SHFT                                                 0xd
#define HWIO_GCC_GPLL0_USER_CTL_PRE_DIV_RATIO_BMSK                                                  0x1000
#define HWIO_GCC_GPLL0_USER_CTL_PRE_DIV_RATIO_SHFT                                                     0xc
#define HWIO_GCC_GPLL0_USER_CTL_RESERVE_BITS11_10_BMSK                                               0xc00
#define HWIO_GCC_GPLL0_USER_CTL_RESERVE_BITS11_10_SHFT                                                 0xa
#define HWIO_GCC_GPLL0_USER_CTL_POST_DIV_RATIO_BMSK                                                  0x300
#define HWIO_GCC_GPLL0_USER_CTL_POST_DIV_RATIO_SHFT                                                    0x8
#define HWIO_GCC_GPLL0_USER_CTL_OUTPUT_INV_BMSK                                                       0x80
#define HWIO_GCC_GPLL0_USER_CTL_OUTPUT_INV_SHFT                                                        0x7
#define HWIO_GCC_GPLL0_USER_CTL_RESERVE_BITS6_5_BMSK                                                  0x60
#define HWIO_GCC_GPLL0_USER_CTL_RESERVE_BITS6_5_SHFT                                                   0x5
#define HWIO_GCC_GPLL0_USER_CTL_PLLOUT_LV_TEST_BMSK                                                   0x10
#define HWIO_GCC_GPLL0_USER_CTL_PLLOUT_LV_TEST_SHFT                                                    0x4
#define HWIO_GCC_GPLL0_USER_CTL_PLLOUT_LV_EARLY_BMSK                                                   0x8
#define HWIO_GCC_GPLL0_USER_CTL_PLLOUT_LV_EARLY_SHFT                                                   0x3
#define HWIO_GCC_GPLL0_USER_CTL_PLLOUT_LV_BIST_BMSK                                                    0x4
#define HWIO_GCC_GPLL0_USER_CTL_PLLOUT_LV_BIST_SHFT                                                    0x2
#define HWIO_GCC_GPLL0_USER_CTL_PLLOUT_LV_AUX_BMSK                                                     0x2
#define HWIO_GCC_GPLL0_USER_CTL_PLLOUT_LV_AUX_SHFT                                                     0x1
#define HWIO_GCC_GPLL0_USER_CTL_PLLOUT_LV_MAIN_BMSK                                                    0x1
#define HWIO_GCC_GPLL0_USER_CTL_PLLOUT_LV_MAIN_SHFT                                                    0x0

#define HWIO_GCC_GPLL0_CONFIG_CTL_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00021014)
#define HWIO_GCC_GPLL0_CONFIG_CTL_RMSK                                                          0xffffffff
#define HWIO_GCC_GPLL0_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_GCC_GPLL0_CONFIG_CTL_ADDR, HWIO_GCC_GPLL0_CONFIG_CTL_RMSK)
#define HWIO_GCC_GPLL0_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL0_CONFIG_CTL_ADDR, m)
#define HWIO_GCC_GPLL0_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_GCC_GPLL0_CONFIG_CTL_ADDR,v)
#define HWIO_GCC_GPLL0_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL0_CONFIG_CTL_ADDR,m,v,HWIO_GCC_GPLL0_CONFIG_CTL_IN)
#define HWIO_GCC_GPLL0_CONFIG_CTL_RESERVE_BITS31_30_BMSK                                        0xc0000000
#define HWIO_GCC_GPLL0_CONFIG_CTL_RESERVE_BITS31_30_SHFT                                              0x1e
#define HWIO_GCC_GPLL0_CONFIG_CTL_FORCE_PFD_UP_BMSK                                             0x20000000
#define HWIO_GCC_GPLL0_CONFIG_CTL_FORCE_PFD_UP_SHFT                                                   0x1d
#define HWIO_GCC_GPLL0_CONFIG_CTL_FORCE_PFD_DOWN_BMSK                                           0x10000000
#define HWIO_GCC_GPLL0_CONFIG_CTL_FORCE_PFD_DOWN_SHFT                                                 0x1c
#define HWIO_GCC_GPLL0_CONFIG_CTL_NMOSC_FREQ_CTRL_BMSK                                           0xc000000
#define HWIO_GCC_GPLL0_CONFIG_CTL_NMOSC_FREQ_CTRL_SHFT                                                0x1a
#define HWIO_GCC_GPLL0_CONFIG_CTL_PFD_DZSEL_BMSK                                                 0x3000000
#define HWIO_GCC_GPLL0_CONFIG_CTL_PFD_DZSEL_SHFT                                                      0x18
#define HWIO_GCC_GPLL0_CONFIG_CTL_NMOSC_EN_BMSK                                                   0x800000
#define HWIO_GCC_GPLL0_CONFIG_CTL_NMOSC_EN_SHFT                                                       0x17
#define HWIO_GCC_GPLL0_CONFIG_CTL_RESERVE_BIT22_BMSK                                              0x400000
#define HWIO_GCC_GPLL0_CONFIG_CTL_RESERVE_BIT22_SHFT                                                  0x16
#define HWIO_GCC_GPLL0_CONFIG_CTL_ICP_DIV_BMSK                                                    0x300000
#define HWIO_GCC_GPLL0_CONFIG_CTL_ICP_DIV_SHFT                                                        0x14
#define HWIO_GCC_GPLL0_CONFIG_CTL_IREG_DIV_BMSK                                                    0xc0000
#define HWIO_GCC_GPLL0_CONFIG_CTL_IREG_DIV_SHFT                                                       0x12
#define HWIO_GCC_GPLL0_CONFIG_CTL_CUSEL_BMSK                                                       0x30000
#define HWIO_GCC_GPLL0_CONFIG_CTL_CUSEL_SHFT                                                          0x10
#define HWIO_GCC_GPLL0_CONFIG_CTL_REF_MODE_BMSK                                                     0x8000
#define HWIO_GCC_GPLL0_CONFIG_CTL_REF_MODE_SHFT                                                        0xf
#define HWIO_GCC_GPLL0_CONFIG_CTL_RESERVE_BIT14_BMSK                                                0x4000
#define HWIO_GCC_GPLL0_CONFIG_CTL_RESERVE_BIT14_SHFT                                                   0xe
#define HWIO_GCC_GPLL0_CONFIG_CTL_CFG_LOCKDET_BMSK                                                  0x3000
#define HWIO_GCC_GPLL0_CONFIG_CTL_CFG_LOCKDET_SHFT                                                     0xc
#define HWIO_GCC_GPLL0_CONFIG_CTL_FORCE_ISEED_BMSK                                                   0x800
#define HWIO_GCC_GPLL0_CONFIG_CTL_FORCE_ISEED_SHFT                                                     0xb
#define HWIO_GCC_GPLL0_CONFIG_CTL_RESERVE_BITS10_0_BMSK                                              0x7ff
#define HWIO_GCC_GPLL0_CONFIG_CTL_RESERVE_BITS10_0_SHFT                                                0x0

#define HWIO_GCC_GPLL0_TEST_CTL_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00021018)
#define HWIO_GCC_GPLL0_TEST_CTL_RMSK                                                            0xffffffff
#define HWIO_GCC_GPLL0_TEST_CTL_IN          \
        in_dword_masked(HWIO_GCC_GPLL0_TEST_CTL_ADDR, HWIO_GCC_GPLL0_TEST_CTL_RMSK)
#define HWIO_GCC_GPLL0_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL0_TEST_CTL_ADDR, m)
#define HWIO_GCC_GPLL0_TEST_CTL_OUT(v)      \
        out_dword(HWIO_GCC_GPLL0_TEST_CTL_ADDR,v)
#define HWIO_GCC_GPLL0_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL0_TEST_CTL_ADDR,m,v,HWIO_GCC_GPLL0_TEST_CTL_IN)
#define HWIO_GCC_GPLL0_TEST_CTL_RESERVE_BITS31_15_BMSK                                          0xffff8000
#define HWIO_GCC_GPLL0_TEST_CTL_RESERVE_BITS31_15_SHFT                                                 0xf
#define HWIO_GCC_GPLL0_TEST_CTL_PLLOUT_LV_TEST_SEL_BMSK                                             0x4000
#define HWIO_GCC_GPLL0_TEST_CTL_PLLOUT_LV_TEST_SEL_SHFT                                                0xe
#define HWIO_GCC_GPLL0_TEST_CTL_RESERVE_BITS13_10_BMSK                                              0x3c00
#define HWIO_GCC_GPLL0_TEST_CTL_RESERVE_BITS13_10_SHFT                                                 0xa
#define HWIO_GCC_GPLL0_TEST_CTL_ICP_TST_EN_BMSK                                                      0x200
#define HWIO_GCC_GPLL0_TEST_CTL_ICP_TST_EN_SHFT                                                        0x9
#define HWIO_GCC_GPLL0_TEST_CTL_ICP_EXT_SEL_BMSK                                                     0x100
#define HWIO_GCC_GPLL0_TEST_CTL_ICP_EXT_SEL_SHFT                                                       0x8
#define HWIO_GCC_GPLL0_TEST_CTL_DTEST_SEL_BMSK                                                        0x80
#define HWIO_GCC_GPLL0_TEST_CTL_DTEST_SEL_SHFT                                                         0x7
#define HWIO_GCC_GPLL0_TEST_CTL_BYP_TESTAMP_BMSK                                                      0x40
#define HWIO_GCC_GPLL0_TEST_CTL_BYP_TESTAMP_SHFT                                                       0x6
#define HWIO_GCC_GPLL0_TEST_CTL_ATEST1_SEL_BMSK                                                       0x30
#define HWIO_GCC_GPLL0_TEST_CTL_ATEST1_SEL_SHFT                                                        0x4
#define HWIO_GCC_GPLL0_TEST_CTL_ATEST0_SEL_BMSK                                                        0xc
#define HWIO_GCC_GPLL0_TEST_CTL_ATEST0_SEL_SHFT                                                        0x2
#define HWIO_GCC_GPLL0_TEST_CTL_ATEST1_EN_BMSK                                                         0x2
#define HWIO_GCC_GPLL0_TEST_CTL_ATEST1_EN_SHFT                                                         0x1
#define HWIO_GCC_GPLL0_TEST_CTL_ATEST0_EN_BMSK                                                         0x1
#define HWIO_GCC_GPLL0_TEST_CTL_ATEST0_EN_SHFT                                                         0x0

#define HWIO_GCC_GPLL0_STATUS_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0002101c)
#define HWIO_GCC_GPLL0_STATUS_RMSK                                                                 0x3ffff
#define HWIO_GCC_GPLL0_STATUS_IN          \
        in_dword_masked(HWIO_GCC_GPLL0_STATUS_ADDR, HWIO_GCC_GPLL0_STATUS_RMSK)
#define HWIO_GCC_GPLL0_STATUS_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL0_STATUS_ADDR, m)
#define HWIO_GCC_GPLL0_STATUS_PLL_ACTIVE_FLAG_BMSK                                                 0x20000
#define HWIO_GCC_GPLL0_STATUS_PLL_ACTIVE_FLAG_SHFT                                                    0x11
#define HWIO_GCC_GPLL0_STATUS_PLL_LOCK_DET_BMSK                                                    0x10000
#define HWIO_GCC_GPLL0_STATUS_PLL_LOCK_DET_SHFT                                                       0x10
#define HWIO_GCC_GPLL0_STATUS_PLL_D_BMSK                                                            0xffff
#define HWIO_GCC_GPLL0_STATUS_PLL_D_SHFT                                                               0x0

#define HWIO_GCC_GPLL1_MODE_ADDR                                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00020000)
#define HWIO_GCC_GPLL1_MODE_RMSK                                                                  0x3fff0f
#define HWIO_GCC_GPLL1_MODE_IN          \
        in_dword_masked(HWIO_GCC_GPLL1_MODE_ADDR, HWIO_GCC_GPLL1_MODE_RMSK)
#define HWIO_GCC_GPLL1_MODE_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL1_MODE_ADDR, m)
#define HWIO_GCC_GPLL1_MODE_OUT(v)      \
        out_dword(HWIO_GCC_GPLL1_MODE_ADDR,v)
#define HWIO_GCC_GPLL1_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL1_MODE_ADDR,m,v,HWIO_GCC_GPLL1_MODE_IN)
#define HWIO_GCC_GPLL1_MODE_PLL_VOTE_FSM_RESET_BMSK                                               0x200000
#define HWIO_GCC_GPLL1_MODE_PLL_VOTE_FSM_RESET_SHFT                                                   0x15
#define HWIO_GCC_GPLL1_MODE_PLL_VOTE_FSM_ENA_BMSK                                                 0x100000
#define HWIO_GCC_GPLL1_MODE_PLL_VOTE_FSM_ENA_SHFT                                                     0x14
#define HWIO_GCC_GPLL1_MODE_PLL_BIAS_COUNT_BMSK                                                    0xfc000
#define HWIO_GCC_GPLL1_MODE_PLL_BIAS_COUNT_SHFT                                                        0xe
#define HWIO_GCC_GPLL1_MODE_PLL_LOCK_COUNT_BMSK                                                     0x3f00
#define HWIO_GCC_GPLL1_MODE_PLL_LOCK_COUNT_SHFT                                                        0x8
#define HWIO_GCC_GPLL1_MODE_PLL_PLLTEST_BMSK                                                           0x8
#define HWIO_GCC_GPLL1_MODE_PLL_PLLTEST_SHFT                                                           0x3
#define HWIO_GCC_GPLL1_MODE_PLL_RESET_N_BMSK                                                           0x4
#define HWIO_GCC_GPLL1_MODE_PLL_RESET_N_SHFT                                                           0x2
#define HWIO_GCC_GPLL1_MODE_PLL_BYPASSNL_BMSK                                                          0x2
#define HWIO_GCC_GPLL1_MODE_PLL_BYPASSNL_SHFT                                                          0x1
#define HWIO_GCC_GPLL1_MODE_PLL_OUTCTRL_BMSK                                                           0x1
#define HWIO_GCC_GPLL1_MODE_PLL_OUTCTRL_SHFT                                                           0x0

#define HWIO_GCC_GPLL1_L_VAL_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00020004)
#define HWIO_GCC_GPLL1_L_VAL_RMSK                                                                     0xff
#define HWIO_GCC_GPLL1_L_VAL_IN          \
        in_dword_masked(HWIO_GCC_GPLL1_L_VAL_ADDR, HWIO_GCC_GPLL1_L_VAL_RMSK)
#define HWIO_GCC_GPLL1_L_VAL_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL1_L_VAL_ADDR, m)
#define HWIO_GCC_GPLL1_L_VAL_OUT(v)      \
        out_dword(HWIO_GCC_GPLL1_L_VAL_ADDR,v)
#define HWIO_GCC_GPLL1_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL1_L_VAL_ADDR,m,v,HWIO_GCC_GPLL1_L_VAL_IN)
#define HWIO_GCC_GPLL1_L_VAL_PLL_L_BMSK                                                               0xff
#define HWIO_GCC_GPLL1_L_VAL_PLL_L_SHFT                                                                0x0

#define HWIO_GCC_GPLL1_M_VAL_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00020008)
#define HWIO_GCC_GPLL1_M_VAL_RMSK                                                                  0x7ffff
#define HWIO_GCC_GPLL1_M_VAL_IN          \
        in_dword_masked(HWIO_GCC_GPLL1_M_VAL_ADDR, HWIO_GCC_GPLL1_M_VAL_RMSK)
#define HWIO_GCC_GPLL1_M_VAL_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL1_M_VAL_ADDR, m)
#define HWIO_GCC_GPLL1_M_VAL_OUT(v)      \
        out_dword(HWIO_GCC_GPLL1_M_VAL_ADDR,v)
#define HWIO_GCC_GPLL1_M_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL1_M_VAL_ADDR,m,v,HWIO_GCC_GPLL1_M_VAL_IN)
#define HWIO_GCC_GPLL1_M_VAL_PLL_M_BMSK                                                            0x7ffff
#define HWIO_GCC_GPLL1_M_VAL_PLL_M_SHFT                                                                0x0

#define HWIO_GCC_GPLL1_N_VAL_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0002000c)
#define HWIO_GCC_GPLL1_N_VAL_RMSK                                                                  0x7ffff
#define HWIO_GCC_GPLL1_N_VAL_IN          \
        in_dword_masked(HWIO_GCC_GPLL1_N_VAL_ADDR, HWIO_GCC_GPLL1_N_VAL_RMSK)
#define HWIO_GCC_GPLL1_N_VAL_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL1_N_VAL_ADDR, m)
#define HWIO_GCC_GPLL1_N_VAL_OUT(v)      \
        out_dword(HWIO_GCC_GPLL1_N_VAL_ADDR,v)
#define HWIO_GCC_GPLL1_N_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL1_N_VAL_ADDR,m,v,HWIO_GCC_GPLL1_N_VAL_IN)
#define HWIO_GCC_GPLL1_N_VAL_PLL_N_BMSK                                                            0x7ffff
#define HWIO_GCC_GPLL1_N_VAL_PLL_N_SHFT                                                                0x0

#define HWIO_GCC_GPLL1_USER_CTL_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00020010)
#define HWIO_GCC_GPLL1_USER_CTL_RMSK                                                            0xffffffff
#define HWIO_GCC_GPLL1_USER_CTL_IN          \
        in_dword_masked(HWIO_GCC_GPLL1_USER_CTL_ADDR, HWIO_GCC_GPLL1_USER_CTL_RMSK)
#define HWIO_GCC_GPLL1_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL1_USER_CTL_ADDR, m)
#define HWIO_GCC_GPLL1_USER_CTL_OUT(v)      \
        out_dword(HWIO_GCC_GPLL1_USER_CTL_ADDR,v)
#define HWIO_GCC_GPLL1_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL1_USER_CTL_ADDR,m,v,HWIO_GCC_GPLL1_USER_CTL_IN)
#define HWIO_GCC_GPLL1_USER_CTL_RESERVE_BITS31_25_BMSK                                          0xfe000000
#define HWIO_GCC_GPLL1_USER_CTL_RESERVE_BITS31_25_SHFT                                                0x19
#define HWIO_GCC_GPLL1_USER_CTL_MN_EN_BMSK                                                       0x1000000
#define HWIO_GCC_GPLL1_USER_CTL_MN_EN_SHFT                                                            0x18
#define HWIO_GCC_GPLL1_USER_CTL_RESERVE_BITS23_21_BMSK                                            0xe00000
#define HWIO_GCC_GPLL1_USER_CTL_RESERVE_BITS23_21_SHFT                                                0x15
#define HWIO_GCC_GPLL1_USER_CTL_VCO_SEL_BMSK                                                      0x100000
#define HWIO_GCC_GPLL1_USER_CTL_VCO_SEL_SHFT                                                          0x14
#define HWIO_GCC_GPLL1_USER_CTL_RESERVE_BITS19_13_BMSK                                             0xfe000
#define HWIO_GCC_GPLL1_USER_CTL_RESERVE_BITS19_13_SHFT                                                 0xd
#define HWIO_GCC_GPLL1_USER_CTL_PRE_DIV_RATIO_BMSK                                                  0x1000
#define HWIO_GCC_GPLL1_USER_CTL_PRE_DIV_RATIO_SHFT                                                     0xc
#define HWIO_GCC_GPLL1_USER_CTL_RESERVE_BITS11_10_BMSK                                               0xc00
#define HWIO_GCC_GPLL1_USER_CTL_RESERVE_BITS11_10_SHFT                                                 0xa
#define HWIO_GCC_GPLL1_USER_CTL_POST_DIV_RATIO_BMSK                                                  0x300
#define HWIO_GCC_GPLL1_USER_CTL_POST_DIV_RATIO_SHFT                                                    0x8
#define HWIO_GCC_GPLL1_USER_CTL_OUTPUT_INV_BMSK                                                       0x80
#define HWIO_GCC_GPLL1_USER_CTL_OUTPUT_INV_SHFT                                                        0x7
#define HWIO_GCC_GPLL1_USER_CTL_RESERVE_BITS6_5_BMSK                                                  0x60
#define HWIO_GCC_GPLL1_USER_CTL_RESERVE_BITS6_5_SHFT                                                   0x5
#define HWIO_GCC_GPLL1_USER_CTL_PLLOUT_LV_TEST_BMSK                                                   0x10
#define HWIO_GCC_GPLL1_USER_CTL_PLLOUT_LV_TEST_SHFT                                                    0x4
#define HWIO_GCC_GPLL1_USER_CTL_PLLOUT_LV_EARLY_BMSK                                                   0x8
#define HWIO_GCC_GPLL1_USER_CTL_PLLOUT_LV_EARLY_SHFT                                                   0x3
#define HWIO_GCC_GPLL1_USER_CTL_PLLOUT_LV_BIST_BMSK                                                    0x4
#define HWIO_GCC_GPLL1_USER_CTL_PLLOUT_LV_BIST_SHFT                                                    0x2
#define HWIO_GCC_GPLL1_USER_CTL_PLLOUT_LV_AUX_BMSK                                                     0x2
#define HWIO_GCC_GPLL1_USER_CTL_PLLOUT_LV_AUX_SHFT                                                     0x1
#define HWIO_GCC_GPLL1_USER_CTL_PLLOUT_LV_MAIN_BMSK                                                    0x1
#define HWIO_GCC_GPLL1_USER_CTL_PLLOUT_LV_MAIN_SHFT                                                    0x0

#define HWIO_GCC_GPLL1_CONFIG_CTL_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00020014)
#define HWIO_GCC_GPLL1_CONFIG_CTL_RMSK                                                          0xffffffff
#define HWIO_GCC_GPLL1_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_GCC_GPLL1_CONFIG_CTL_ADDR, HWIO_GCC_GPLL1_CONFIG_CTL_RMSK)
#define HWIO_GCC_GPLL1_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL1_CONFIG_CTL_ADDR, m)
#define HWIO_GCC_GPLL1_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_GCC_GPLL1_CONFIG_CTL_ADDR,v)
#define HWIO_GCC_GPLL1_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL1_CONFIG_CTL_ADDR,m,v,HWIO_GCC_GPLL1_CONFIG_CTL_IN)
#define HWIO_GCC_GPLL1_CONFIG_CTL_RESERVE_BITS31_30_BMSK                                        0xc0000000
#define HWIO_GCC_GPLL1_CONFIG_CTL_RESERVE_BITS31_30_SHFT                                              0x1e
#define HWIO_GCC_GPLL1_CONFIG_CTL_FORCE_PFD_UP_BMSK                                             0x20000000
#define HWIO_GCC_GPLL1_CONFIG_CTL_FORCE_PFD_UP_SHFT                                                   0x1d
#define HWIO_GCC_GPLL1_CONFIG_CTL_FORCE_PFD_DOWN_BMSK                                           0x10000000
#define HWIO_GCC_GPLL1_CONFIG_CTL_FORCE_PFD_DOWN_SHFT                                                 0x1c
#define HWIO_GCC_GPLL1_CONFIG_CTL_NMOSC_FREQ_CTRL_BMSK                                           0xc000000
#define HWIO_GCC_GPLL1_CONFIG_CTL_NMOSC_FREQ_CTRL_SHFT                                                0x1a
#define HWIO_GCC_GPLL1_CONFIG_CTL_PFD_DZSEL_BMSK                                                 0x3000000
#define HWIO_GCC_GPLL1_CONFIG_CTL_PFD_DZSEL_SHFT                                                      0x18
#define HWIO_GCC_GPLL1_CONFIG_CTL_NMOSC_EN_BMSK                                                   0x800000
#define HWIO_GCC_GPLL1_CONFIG_CTL_NMOSC_EN_SHFT                                                       0x17
#define HWIO_GCC_GPLL1_CONFIG_CTL_RESERVE_BIT22_BMSK                                              0x400000
#define HWIO_GCC_GPLL1_CONFIG_CTL_RESERVE_BIT22_SHFT                                                  0x16
#define HWIO_GCC_GPLL1_CONFIG_CTL_ICP_DIV_BMSK                                                    0x300000
#define HWIO_GCC_GPLL1_CONFIG_CTL_ICP_DIV_SHFT                                                        0x14
#define HWIO_GCC_GPLL1_CONFIG_CTL_IREG_DIV_BMSK                                                    0xc0000
#define HWIO_GCC_GPLL1_CONFIG_CTL_IREG_DIV_SHFT                                                       0x12
#define HWIO_GCC_GPLL1_CONFIG_CTL_CUSEL_BMSK                                                       0x30000
#define HWIO_GCC_GPLL1_CONFIG_CTL_CUSEL_SHFT                                                          0x10
#define HWIO_GCC_GPLL1_CONFIG_CTL_REF_MODE_BMSK                                                     0x8000
#define HWIO_GCC_GPLL1_CONFIG_CTL_REF_MODE_SHFT                                                        0xf
#define HWIO_GCC_GPLL1_CONFIG_CTL_RESERVE_BIT14_BMSK                                                0x4000
#define HWIO_GCC_GPLL1_CONFIG_CTL_RESERVE_BIT14_SHFT                                                   0xe
#define HWIO_GCC_GPLL1_CONFIG_CTL_CFG_LOCKDET_BMSK                                                  0x3000
#define HWIO_GCC_GPLL1_CONFIG_CTL_CFG_LOCKDET_SHFT                                                     0xc
#define HWIO_GCC_GPLL1_CONFIG_CTL_FORCE_ISEED_BMSK                                                   0x800
#define HWIO_GCC_GPLL1_CONFIG_CTL_FORCE_ISEED_SHFT                                                     0xb
#define HWIO_GCC_GPLL1_CONFIG_CTL_RESERVE_BITS10_0_BMSK                                              0x7ff
#define HWIO_GCC_GPLL1_CONFIG_CTL_RESERVE_BITS10_0_SHFT                                                0x0

#define HWIO_GCC_GPLL1_TEST_CTL_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00020018)
#define HWIO_GCC_GPLL1_TEST_CTL_RMSK                                                            0xffffffff
#define HWIO_GCC_GPLL1_TEST_CTL_IN          \
        in_dword_masked(HWIO_GCC_GPLL1_TEST_CTL_ADDR, HWIO_GCC_GPLL1_TEST_CTL_RMSK)
#define HWIO_GCC_GPLL1_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL1_TEST_CTL_ADDR, m)
#define HWIO_GCC_GPLL1_TEST_CTL_OUT(v)      \
        out_dword(HWIO_GCC_GPLL1_TEST_CTL_ADDR,v)
#define HWIO_GCC_GPLL1_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL1_TEST_CTL_ADDR,m,v,HWIO_GCC_GPLL1_TEST_CTL_IN)
#define HWIO_GCC_GPLL1_TEST_CTL_RESERVE_BITS31_15_BMSK                                          0xffff8000
#define HWIO_GCC_GPLL1_TEST_CTL_RESERVE_BITS31_15_SHFT                                                 0xf
#define HWIO_GCC_GPLL1_TEST_CTL_PLLOUT_LV_TEST_SEL_BMSK                                             0x4000
#define HWIO_GCC_GPLL1_TEST_CTL_PLLOUT_LV_TEST_SEL_SHFT                                                0xe
#define HWIO_GCC_GPLL1_TEST_CTL_RESERVE_BITS13_10_BMSK                                              0x3c00
#define HWIO_GCC_GPLL1_TEST_CTL_RESERVE_BITS13_10_SHFT                                                 0xa
#define HWIO_GCC_GPLL1_TEST_CTL_ICP_TST_EN_BMSK                                                      0x200
#define HWIO_GCC_GPLL1_TEST_CTL_ICP_TST_EN_SHFT                                                        0x9
#define HWIO_GCC_GPLL1_TEST_CTL_ICP_EXT_SEL_BMSK                                                     0x100
#define HWIO_GCC_GPLL1_TEST_CTL_ICP_EXT_SEL_SHFT                                                       0x8
#define HWIO_GCC_GPLL1_TEST_CTL_DTEST_SEL_BMSK                                                        0x80
#define HWIO_GCC_GPLL1_TEST_CTL_DTEST_SEL_SHFT                                                         0x7
#define HWIO_GCC_GPLL1_TEST_CTL_BYP_TESTAMP_BMSK                                                      0x40
#define HWIO_GCC_GPLL1_TEST_CTL_BYP_TESTAMP_SHFT                                                       0x6
#define HWIO_GCC_GPLL1_TEST_CTL_ATEST1_SEL_BMSK                                                       0x30
#define HWIO_GCC_GPLL1_TEST_CTL_ATEST1_SEL_SHFT                                                        0x4
#define HWIO_GCC_GPLL1_TEST_CTL_ATEST0_SEL_BMSK                                                        0xc
#define HWIO_GCC_GPLL1_TEST_CTL_ATEST0_SEL_SHFT                                                        0x2
#define HWIO_GCC_GPLL1_TEST_CTL_ATEST1_EN_BMSK                                                         0x2
#define HWIO_GCC_GPLL1_TEST_CTL_ATEST1_EN_SHFT                                                         0x1
#define HWIO_GCC_GPLL1_TEST_CTL_ATEST0_EN_BMSK                                                         0x1
#define HWIO_GCC_GPLL1_TEST_CTL_ATEST0_EN_SHFT                                                         0x0

#define HWIO_GCC_GPLL1_STATUS_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0002001c)
#define HWIO_GCC_GPLL1_STATUS_RMSK                                                                 0x3ffff
#define HWIO_GCC_GPLL1_STATUS_IN          \
        in_dword_masked(HWIO_GCC_GPLL1_STATUS_ADDR, HWIO_GCC_GPLL1_STATUS_RMSK)
#define HWIO_GCC_GPLL1_STATUS_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL1_STATUS_ADDR, m)
#define HWIO_GCC_GPLL1_STATUS_PLL_ACTIVE_FLAG_BMSK                                                 0x20000
#define HWIO_GCC_GPLL1_STATUS_PLL_ACTIVE_FLAG_SHFT                                                    0x11
#define HWIO_GCC_GPLL1_STATUS_PLL_LOCK_DET_BMSK                                                    0x10000
#define HWIO_GCC_GPLL1_STATUS_PLL_LOCK_DET_SHFT                                                       0x10
#define HWIO_GCC_GPLL1_STATUS_PLL_D_BMSK                                                            0xffff
#define HWIO_GCC_GPLL1_STATUS_PLL_D_SHFT                                                               0x0

#define HWIO_GCC_GPLL2_MODE_ADDR                                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x0004a000)
#define HWIO_GCC_GPLL2_MODE_RMSK                                                                  0x3fff0f
#define HWIO_GCC_GPLL2_MODE_IN          \
        in_dword_masked(HWIO_GCC_GPLL2_MODE_ADDR, HWIO_GCC_GPLL2_MODE_RMSK)
#define HWIO_GCC_GPLL2_MODE_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL2_MODE_ADDR, m)
#define HWIO_GCC_GPLL2_MODE_OUT(v)      \
        out_dword(HWIO_GCC_GPLL2_MODE_ADDR,v)
#define HWIO_GCC_GPLL2_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL2_MODE_ADDR,m,v,HWIO_GCC_GPLL2_MODE_IN)
#define HWIO_GCC_GPLL2_MODE_PLL_VOTE_FSM_RESET_BMSK                                               0x200000
#define HWIO_GCC_GPLL2_MODE_PLL_VOTE_FSM_RESET_SHFT                                                   0x15
#define HWIO_GCC_GPLL2_MODE_PLL_VOTE_FSM_ENA_BMSK                                                 0x100000
#define HWIO_GCC_GPLL2_MODE_PLL_VOTE_FSM_ENA_SHFT                                                     0x14
#define HWIO_GCC_GPLL2_MODE_PLL_BIAS_COUNT_BMSK                                                    0xfc000
#define HWIO_GCC_GPLL2_MODE_PLL_BIAS_COUNT_SHFT                                                        0xe
#define HWIO_GCC_GPLL2_MODE_PLL_LOCK_COUNT_BMSK                                                     0x3f00
#define HWIO_GCC_GPLL2_MODE_PLL_LOCK_COUNT_SHFT                                                        0x8
#define HWIO_GCC_GPLL2_MODE_PLL_PLLTEST_BMSK                                                           0x8
#define HWIO_GCC_GPLL2_MODE_PLL_PLLTEST_SHFT                                                           0x3
#define HWIO_GCC_GPLL2_MODE_PLL_RESET_N_BMSK                                                           0x4
#define HWIO_GCC_GPLL2_MODE_PLL_RESET_N_SHFT                                                           0x2
#define HWIO_GCC_GPLL2_MODE_PLL_BYPASSNL_BMSK                                                          0x2
#define HWIO_GCC_GPLL2_MODE_PLL_BYPASSNL_SHFT                                                          0x1
#define HWIO_GCC_GPLL2_MODE_PLL_OUTCTRL_BMSK                                                           0x1
#define HWIO_GCC_GPLL2_MODE_PLL_OUTCTRL_SHFT                                                           0x0

#define HWIO_GCC_GPLL2_L_VAL_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0004a004)
#define HWIO_GCC_GPLL2_L_VAL_RMSK                                                                     0xff
#define HWIO_GCC_GPLL2_L_VAL_IN          \
        in_dword_masked(HWIO_GCC_GPLL2_L_VAL_ADDR, HWIO_GCC_GPLL2_L_VAL_RMSK)
#define HWIO_GCC_GPLL2_L_VAL_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL2_L_VAL_ADDR, m)
#define HWIO_GCC_GPLL2_L_VAL_OUT(v)      \
        out_dword(HWIO_GCC_GPLL2_L_VAL_ADDR,v)
#define HWIO_GCC_GPLL2_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL2_L_VAL_ADDR,m,v,HWIO_GCC_GPLL2_L_VAL_IN)
#define HWIO_GCC_GPLL2_L_VAL_PLL_L_BMSK                                                               0xff
#define HWIO_GCC_GPLL2_L_VAL_PLL_L_SHFT                                                                0x0

#define HWIO_GCC_GPLL2_M_VAL_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0004a008)
#define HWIO_GCC_GPLL2_M_VAL_RMSK                                                                  0x7ffff
#define HWIO_GCC_GPLL2_M_VAL_IN          \
        in_dword_masked(HWIO_GCC_GPLL2_M_VAL_ADDR, HWIO_GCC_GPLL2_M_VAL_RMSK)
#define HWIO_GCC_GPLL2_M_VAL_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL2_M_VAL_ADDR, m)
#define HWIO_GCC_GPLL2_M_VAL_OUT(v)      \
        out_dword(HWIO_GCC_GPLL2_M_VAL_ADDR,v)
#define HWIO_GCC_GPLL2_M_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL2_M_VAL_ADDR,m,v,HWIO_GCC_GPLL2_M_VAL_IN)
#define HWIO_GCC_GPLL2_M_VAL_PLL_M_BMSK                                                            0x7ffff
#define HWIO_GCC_GPLL2_M_VAL_PLL_M_SHFT                                                                0x0

#define HWIO_GCC_GPLL2_N_VAL_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0004a00c)
#define HWIO_GCC_GPLL2_N_VAL_RMSK                                                                  0x7ffff
#define HWIO_GCC_GPLL2_N_VAL_IN          \
        in_dword_masked(HWIO_GCC_GPLL2_N_VAL_ADDR, HWIO_GCC_GPLL2_N_VAL_RMSK)
#define HWIO_GCC_GPLL2_N_VAL_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL2_N_VAL_ADDR, m)
#define HWIO_GCC_GPLL2_N_VAL_OUT(v)      \
        out_dword(HWIO_GCC_GPLL2_N_VAL_ADDR,v)
#define HWIO_GCC_GPLL2_N_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL2_N_VAL_ADDR,m,v,HWIO_GCC_GPLL2_N_VAL_IN)
#define HWIO_GCC_GPLL2_N_VAL_PLL_N_BMSK                                                            0x7ffff
#define HWIO_GCC_GPLL2_N_VAL_PLL_N_SHFT                                                                0x0

#define HWIO_GCC_GPLL2_USER_CTL_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0004a010)
#define HWIO_GCC_GPLL2_USER_CTL_RMSK                                                            0xffffffff
#define HWIO_GCC_GPLL2_USER_CTL_IN          \
        in_dword_masked(HWIO_GCC_GPLL2_USER_CTL_ADDR, HWIO_GCC_GPLL2_USER_CTL_RMSK)
#define HWIO_GCC_GPLL2_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL2_USER_CTL_ADDR, m)
#define HWIO_GCC_GPLL2_USER_CTL_OUT(v)      \
        out_dword(HWIO_GCC_GPLL2_USER_CTL_ADDR,v)
#define HWIO_GCC_GPLL2_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL2_USER_CTL_ADDR,m,v,HWIO_GCC_GPLL2_USER_CTL_IN)
#define HWIO_GCC_GPLL2_USER_CTL_RESERVE_BITS31_25_BMSK                                          0xfe000000
#define HWIO_GCC_GPLL2_USER_CTL_RESERVE_BITS31_25_SHFT                                                0x19
#define HWIO_GCC_GPLL2_USER_CTL_MN_EN_BMSK                                                       0x1000000
#define HWIO_GCC_GPLL2_USER_CTL_MN_EN_SHFT                                                            0x18
#define HWIO_GCC_GPLL2_USER_CTL_RESERVE_BITS23_21_BMSK                                            0xe00000
#define HWIO_GCC_GPLL2_USER_CTL_RESERVE_BITS23_21_SHFT                                                0x15
#define HWIO_GCC_GPLL2_USER_CTL_VCO_SEL_BMSK                                                      0x100000
#define HWIO_GCC_GPLL2_USER_CTL_VCO_SEL_SHFT                                                          0x14
#define HWIO_GCC_GPLL2_USER_CTL_RESERVE_BITS19_13_BMSK                                             0xfe000
#define HWIO_GCC_GPLL2_USER_CTL_RESERVE_BITS19_13_SHFT                                                 0xd
#define HWIO_GCC_GPLL2_USER_CTL_PRE_DIV_RATIO_BMSK                                                  0x1000
#define HWIO_GCC_GPLL2_USER_CTL_PRE_DIV_RATIO_SHFT                                                     0xc
#define HWIO_GCC_GPLL2_USER_CTL_RESERVE_BITS11_10_BMSK                                               0xc00
#define HWIO_GCC_GPLL2_USER_CTL_RESERVE_BITS11_10_SHFT                                                 0xa
#define HWIO_GCC_GPLL2_USER_CTL_POST_DIV_RATIO_BMSK                                                  0x300
#define HWIO_GCC_GPLL2_USER_CTL_POST_DIV_RATIO_SHFT                                                    0x8
#define HWIO_GCC_GPLL2_USER_CTL_OUTPUT_INV_BMSK                                                       0x80
#define HWIO_GCC_GPLL2_USER_CTL_OUTPUT_INV_SHFT                                                        0x7
#define HWIO_GCC_GPLL2_USER_CTL_RESERVE_BITS6_5_BMSK                                                  0x60
#define HWIO_GCC_GPLL2_USER_CTL_RESERVE_BITS6_5_SHFT                                                   0x5
#define HWIO_GCC_GPLL2_USER_CTL_PLLOUT_LV_TEST_BMSK                                                   0x10
#define HWIO_GCC_GPLL2_USER_CTL_PLLOUT_LV_TEST_SHFT                                                    0x4
#define HWIO_GCC_GPLL2_USER_CTL_PLLOUT_LV_EARLY_BMSK                                                   0x8
#define HWIO_GCC_GPLL2_USER_CTL_PLLOUT_LV_EARLY_SHFT                                                   0x3
#define HWIO_GCC_GPLL2_USER_CTL_PLLOUT_LV_BIST_BMSK                                                    0x4
#define HWIO_GCC_GPLL2_USER_CTL_PLLOUT_LV_BIST_SHFT                                                    0x2
#define HWIO_GCC_GPLL2_USER_CTL_PLLOUT_LV_AUX_BMSK                                                     0x2
#define HWIO_GCC_GPLL2_USER_CTL_PLLOUT_LV_AUX_SHFT                                                     0x1
#define HWIO_GCC_GPLL2_USER_CTL_PLLOUT_LV_MAIN_BMSK                                                    0x1
#define HWIO_GCC_GPLL2_USER_CTL_PLLOUT_LV_MAIN_SHFT                                                    0x0

#define HWIO_GCC_GPLL2_CONFIG_CTL_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x0004a014)
#define HWIO_GCC_GPLL2_CONFIG_CTL_RMSK                                                          0xffffffff
#define HWIO_GCC_GPLL2_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_GCC_GPLL2_CONFIG_CTL_ADDR, HWIO_GCC_GPLL2_CONFIG_CTL_RMSK)
#define HWIO_GCC_GPLL2_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL2_CONFIG_CTL_ADDR, m)
#define HWIO_GCC_GPLL2_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_GCC_GPLL2_CONFIG_CTL_ADDR,v)
#define HWIO_GCC_GPLL2_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL2_CONFIG_CTL_ADDR,m,v,HWIO_GCC_GPLL2_CONFIG_CTL_IN)
#define HWIO_GCC_GPLL2_CONFIG_CTL_RESERVE_BITS31_30_BMSK                                        0xc0000000
#define HWIO_GCC_GPLL2_CONFIG_CTL_RESERVE_BITS31_30_SHFT                                              0x1e
#define HWIO_GCC_GPLL2_CONFIG_CTL_FORCE_PFD_UP_BMSK                                             0x20000000
#define HWIO_GCC_GPLL2_CONFIG_CTL_FORCE_PFD_UP_SHFT                                                   0x1d
#define HWIO_GCC_GPLL2_CONFIG_CTL_FORCE_PFD_DOWN_BMSK                                           0x10000000
#define HWIO_GCC_GPLL2_CONFIG_CTL_FORCE_PFD_DOWN_SHFT                                                 0x1c
#define HWIO_GCC_GPLL2_CONFIG_CTL_NMOSC_FREQ_CTRL_BMSK                                           0xc000000
#define HWIO_GCC_GPLL2_CONFIG_CTL_NMOSC_FREQ_CTRL_SHFT                                                0x1a
#define HWIO_GCC_GPLL2_CONFIG_CTL_PFD_DZSEL_BMSK                                                 0x3000000
#define HWIO_GCC_GPLL2_CONFIG_CTL_PFD_DZSEL_SHFT                                                      0x18
#define HWIO_GCC_GPLL2_CONFIG_CTL_NMOSC_EN_BMSK                                                   0x800000
#define HWIO_GCC_GPLL2_CONFIG_CTL_NMOSC_EN_SHFT                                                       0x17
#define HWIO_GCC_GPLL2_CONFIG_CTL_RESERVE_BIT22_BMSK                                              0x400000
#define HWIO_GCC_GPLL2_CONFIG_CTL_RESERVE_BIT22_SHFT                                                  0x16
#define HWIO_GCC_GPLL2_CONFIG_CTL_ICP_DIV_BMSK                                                    0x300000
#define HWIO_GCC_GPLL2_CONFIG_CTL_ICP_DIV_SHFT                                                        0x14
#define HWIO_GCC_GPLL2_CONFIG_CTL_IREG_DIV_BMSK                                                    0xc0000
#define HWIO_GCC_GPLL2_CONFIG_CTL_IREG_DIV_SHFT                                                       0x12
#define HWIO_GCC_GPLL2_CONFIG_CTL_CUSEL_BMSK                                                       0x30000
#define HWIO_GCC_GPLL2_CONFIG_CTL_CUSEL_SHFT                                                          0x10
#define HWIO_GCC_GPLL2_CONFIG_CTL_REF_MODE_BMSK                                                     0x8000
#define HWIO_GCC_GPLL2_CONFIG_CTL_REF_MODE_SHFT                                                        0xf
#define HWIO_GCC_GPLL2_CONFIG_CTL_RESERVE_BIT14_BMSK                                                0x4000
#define HWIO_GCC_GPLL2_CONFIG_CTL_RESERVE_BIT14_SHFT                                                   0xe
#define HWIO_GCC_GPLL2_CONFIG_CTL_CFG_LOCKDET_BMSK                                                  0x3000
#define HWIO_GCC_GPLL2_CONFIG_CTL_CFG_LOCKDET_SHFT                                                     0xc
#define HWIO_GCC_GPLL2_CONFIG_CTL_FORCE_ISEED_BMSK                                                   0x800
#define HWIO_GCC_GPLL2_CONFIG_CTL_FORCE_ISEED_SHFT                                                     0xb
#define HWIO_GCC_GPLL2_CONFIG_CTL_RESERVE_BITS10_0_BMSK                                              0x7ff
#define HWIO_GCC_GPLL2_CONFIG_CTL_RESERVE_BITS10_0_SHFT                                                0x0

#define HWIO_GCC_GPLL2_TEST_CTL_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0004a018)
#define HWIO_GCC_GPLL2_TEST_CTL_RMSK                                                            0xffffffff
#define HWIO_GCC_GPLL2_TEST_CTL_IN          \
        in_dword_masked(HWIO_GCC_GPLL2_TEST_CTL_ADDR, HWIO_GCC_GPLL2_TEST_CTL_RMSK)
#define HWIO_GCC_GPLL2_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL2_TEST_CTL_ADDR, m)
#define HWIO_GCC_GPLL2_TEST_CTL_OUT(v)      \
        out_dword(HWIO_GCC_GPLL2_TEST_CTL_ADDR,v)
#define HWIO_GCC_GPLL2_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL2_TEST_CTL_ADDR,m,v,HWIO_GCC_GPLL2_TEST_CTL_IN)
#define HWIO_GCC_GPLL2_TEST_CTL_RESERVE_BITS31_15_BMSK                                          0xffff8000
#define HWIO_GCC_GPLL2_TEST_CTL_RESERVE_BITS31_15_SHFT                                                 0xf
#define HWIO_GCC_GPLL2_TEST_CTL_PLLOUT_LV_TEST_SEL_BMSK                                             0x4000
#define HWIO_GCC_GPLL2_TEST_CTL_PLLOUT_LV_TEST_SEL_SHFT                                                0xe
#define HWIO_GCC_GPLL2_TEST_CTL_RESERVE_BITS13_10_BMSK                                              0x3c00
#define HWIO_GCC_GPLL2_TEST_CTL_RESERVE_BITS13_10_SHFT                                                 0xa
#define HWIO_GCC_GPLL2_TEST_CTL_ICP_TST_EN_BMSK                                                      0x200
#define HWIO_GCC_GPLL2_TEST_CTL_ICP_TST_EN_SHFT                                                        0x9
#define HWIO_GCC_GPLL2_TEST_CTL_ICP_EXT_SEL_BMSK                                                     0x100
#define HWIO_GCC_GPLL2_TEST_CTL_ICP_EXT_SEL_SHFT                                                       0x8
#define HWIO_GCC_GPLL2_TEST_CTL_DTEST_SEL_BMSK                                                        0x80
#define HWIO_GCC_GPLL2_TEST_CTL_DTEST_SEL_SHFT                                                         0x7
#define HWIO_GCC_GPLL2_TEST_CTL_BYP_TESTAMP_BMSK                                                      0x40
#define HWIO_GCC_GPLL2_TEST_CTL_BYP_TESTAMP_SHFT                                                       0x6
#define HWIO_GCC_GPLL2_TEST_CTL_ATEST1_SEL_BMSK                                                       0x30
#define HWIO_GCC_GPLL2_TEST_CTL_ATEST1_SEL_SHFT                                                        0x4
#define HWIO_GCC_GPLL2_TEST_CTL_ATEST0_SEL_BMSK                                                        0xc
#define HWIO_GCC_GPLL2_TEST_CTL_ATEST0_SEL_SHFT                                                        0x2
#define HWIO_GCC_GPLL2_TEST_CTL_ATEST1_EN_BMSK                                                         0x2
#define HWIO_GCC_GPLL2_TEST_CTL_ATEST1_EN_SHFT                                                         0x1
#define HWIO_GCC_GPLL2_TEST_CTL_ATEST0_EN_BMSK                                                         0x1
#define HWIO_GCC_GPLL2_TEST_CTL_ATEST0_EN_SHFT                                                         0x0

#define HWIO_GCC_GPLL2_STATUS_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0004a01c)
#define HWIO_GCC_GPLL2_STATUS_RMSK                                                                 0x3ffff
#define HWIO_GCC_GPLL2_STATUS_IN          \
        in_dword_masked(HWIO_GCC_GPLL2_STATUS_ADDR, HWIO_GCC_GPLL2_STATUS_RMSK)
#define HWIO_GCC_GPLL2_STATUS_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL2_STATUS_ADDR, m)
#define HWIO_GCC_GPLL2_STATUS_PLL_ACTIVE_FLAG_BMSK                                                 0x20000
#define HWIO_GCC_GPLL2_STATUS_PLL_ACTIVE_FLAG_SHFT                                                    0x11
#define HWIO_GCC_GPLL2_STATUS_PLL_LOCK_DET_BMSK                                                    0x10000
#define HWIO_GCC_GPLL2_STATUS_PLL_LOCK_DET_SHFT                                                       0x10
#define HWIO_GCC_GPLL2_STATUS_PLL_D_BMSK                                                            0xffff
#define HWIO_GCC_GPLL2_STATUS_PLL_D_SHFT                                                               0x0

#define HWIO_GCC_BIMC_PLL_MODE_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00023000)
#define HWIO_GCC_BIMC_PLL_MODE_RMSK                                                               0x3fff0f
#define HWIO_GCC_BIMC_PLL_MODE_IN          \
        in_dword_masked(HWIO_GCC_BIMC_PLL_MODE_ADDR, HWIO_GCC_BIMC_PLL_MODE_RMSK)
#define HWIO_GCC_BIMC_PLL_MODE_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_PLL_MODE_ADDR, m)
#define HWIO_GCC_BIMC_PLL_MODE_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_PLL_MODE_ADDR,v)
#define HWIO_GCC_BIMC_PLL_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_PLL_MODE_ADDR,m,v,HWIO_GCC_BIMC_PLL_MODE_IN)
#define HWIO_GCC_BIMC_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                                            0x200000
#define HWIO_GCC_BIMC_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                                                0x15
#define HWIO_GCC_BIMC_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                                              0x100000
#define HWIO_GCC_BIMC_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                                                  0x14
#define HWIO_GCC_BIMC_PLL_MODE_PLL_BIAS_COUNT_BMSK                                                 0xfc000
#define HWIO_GCC_BIMC_PLL_MODE_PLL_BIAS_COUNT_SHFT                                                     0xe
#define HWIO_GCC_BIMC_PLL_MODE_PLL_LOCK_COUNT_BMSK                                                  0x3f00
#define HWIO_GCC_BIMC_PLL_MODE_PLL_LOCK_COUNT_SHFT                                                     0x8
#define HWIO_GCC_BIMC_PLL_MODE_PLL_PLLTEST_BMSK                                                        0x8
#define HWIO_GCC_BIMC_PLL_MODE_PLL_PLLTEST_SHFT                                                        0x3
#define HWIO_GCC_BIMC_PLL_MODE_PLL_RESET_N_BMSK                                                        0x4
#define HWIO_GCC_BIMC_PLL_MODE_PLL_RESET_N_SHFT                                                        0x2
#define HWIO_GCC_BIMC_PLL_MODE_PLL_BYPASSNL_BMSK                                                       0x2
#define HWIO_GCC_BIMC_PLL_MODE_PLL_BYPASSNL_SHFT                                                       0x1
#define HWIO_GCC_BIMC_PLL_MODE_PLL_OUTCTRL_BMSK                                                        0x1
#define HWIO_GCC_BIMC_PLL_MODE_PLL_OUTCTRL_SHFT                                                        0x0

#define HWIO_GCC_BIMC_PLL_L_VAL_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00023004)
#define HWIO_GCC_BIMC_PLL_L_VAL_RMSK                                                                  0xff
#define HWIO_GCC_BIMC_PLL_L_VAL_IN          \
        in_dword_masked(HWIO_GCC_BIMC_PLL_L_VAL_ADDR, HWIO_GCC_BIMC_PLL_L_VAL_RMSK)
#define HWIO_GCC_BIMC_PLL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_PLL_L_VAL_ADDR, m)
#define HWIO_GCC_BIMC_PLL_L_VAL_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_PLL_L_VAL_ADDR,v)
#define HWIO_GCC_BIMC_PLL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_PLL_L_VAL_ADDR,m,v,HWIO_GCC_BIMC_PLL_L_VAL_IN)
#define HWIO_GCC_BIMC_PLL_L_VAL_PLL_L_BMSK                                                            0xff
#define HWIO_GCC_BIMC_PLL_L_VAL_PLL_L_SHFT                                                             0x0

#define HWIO_GCC_BIMC_PLL_M_VAL_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00023008)
#define HWIO_GCC_BIMC_PLL_M_VAL_RMSK                                                               0x7ffff
#define HWIO_GCC_BIMC_PLL_M_VAL_IN          \
        in_dword_masked(HWIO_GCC_BIMC_PLL_M_VAL_ADDR, HWIO_GCC_BIMC_PLL_M_VAL_RMSK)
#define HWIO_GCC_BIMC_PLL_M_VAL_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_PLL_M_VAL_ADDR, m)
#define HWIO_GCC_BIMC_PLL_M_VAL_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_PLL_M_VAL_ADDR,v)
#define HWIO_GCC_BIMC_PLL_M_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_PLL_M_VAL_ADDR,m,v,HWIO_GCC_BIMC_PLL_M_VAL_IN)
#define HWIO_GCC_BIMC_PLL_M_VAL_PLL_M_BMSK                                                         0x7ffff
#define HWIO_GCC_BIMC_PLL_M_VAL_PLL_M_SHFT                                                             0x0

#define HWIO_GCC_BIMC_PLL_N_VAL_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0002300c)
#define HWIO_GCC_BIMC_PLL_N_VAL_RMSK                                                               0x7ffff
#define HWIO_GCC_BIMC_PLL_N_VAL_IN          \
        in_dword_masked(HWIO_GCC_BIMC_PLL_N_VAL_ADDR, HWIO_GCC_BIMC_PLL_N_VAL_RMSK)
#define HWIO_GCC_BIMC_PLL_N_VAL_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_PLL_N_VAL_ADDR, m)
#define HWIO_GCC_BIMC_PLL_N_VAL_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_PLL_N_VAL_ADDR,v)
#define HWIO_GCC_BIMC_PLL_N_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_PLL_N_VAL_ADDR,m,v,HWIO_GCC_BIMC_PLL_N_VAL_IN)
#define HWIO_GCC_BIMC_PLL_N_VAL_PLL_N_BMSK                                                         0x7ffff
#define HWIO_GCC_BIMC_PLL_N_VAL_PLL_N_SHFT                                                             0x0

#define HWIO_GCC_BIMC_PLL_USER_CTL_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00023010)
#define HWIO_GCC_BIMC_PLL_USER_CTL_RMSK                                                         0xffffffff
#define HWIO_GCC_BIMC_PLL_USER_CTL_IN          \
        in_dword_masked(HWIO_GCC_BIMC_PLL_USER_CTL_ADDR, HWIO_GCC_BIMC_PLL_USER_CTL_RMSK)
#define HWIO_GCC_BIMC_PLL_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_PLL_USER_CTL_ADDR, m)
#define HWIO_GCC_BIMC_PLL_USER_CTL_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_PLL_USER_CTL_ADDR,v)
#define HWIO_GCC_BIMC_PLL_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_PLL_USER_CTL_ADDR,m,v,HWIO_GCC_BIMC_PLL_USER_CTL_IN)
#define HWIO_GCC_BIMC_PLL_USER_CTL_RESERVE_BITS31_25_BMSK                                       0xfe000000
#define HWIO_GCC_BIMC_PLL_USER_CTL_RESERVE_BITS31_25_SHFT                                             0x19
#define HWIO_GCC_BIMC_PLL_USER_CTL_MN_EN_BMSK                                                    0x1000000
#define HWIO_GCC_BIMC_PLL_USER_CTL_MN_EN_SHFT                                                         0x18
#define HWIO_GCC_BIMC_PLL_USER_CTL_RESERVE_BITS23_21_BMSK                                         0xe00000
#define HWIO_GCC_BIMC_PLL_USER_CTL_RESERVE_BITS23_21_SHFT                                             0x15
#define HWIO_GCC_BIMC_PLL_USER_CTL_VCO_SEL_BMSK                                                   0x100000
#define HWIO_GCC_BIMC_PLL_USER_CTL_VCO_SEL_SHFT                                                       0x14
#define HWIO_GCC_BIMC_PLL_USER_CTL_RESERVE_BITS19_13_BMSK                                          0xfe000
#define HWIO_GCC_BIMC_PLL_USER_CTL_RESERVE_BITS19_13_SHFT                                              0xd
#define HWIO_GCC_BIMC_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                                               0x1000
#define HWIO_GCC_BIMC_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                                                  0xc
#define HWIO_GCC_BIMC_PLL_USER_CTL_RESERVE_BITS11_10_BMSK                                            0xc00
#define HWIO_GCC_BIMC_PLL_USER_CTL_RESERVE_BITS11_10_SHFT                                              0xa
#define HWIO_GCC_BIMC_PLL_USER_CTL_POST_DIV_RATIO_BMSK                                               0x300
#define HWIO_GCC_BIMC_PLL_USER_CTL_POST_DIV_RATIO_SHFT                                                 0x8
#define HWIO_GCC_BIMC_PLL_USER_CTL_OUTPUT_INV_BMSK                                                    0x80
#define HWIO_GCC_BIMC_PLL_USER_CTL_OUTPUT_INV_SHFT                                                     0x7
#define HWIO_GCC_BIMC_PLL_USER_CTL_RESERVE_BITS6_5_BMSK                                               0x60
#define HWIO_GCC_BIMC_PLL_USER_CTL_RESERVE_BITS6_5_SHFT                                                0x5
#define HWIO_GCC_BIMC_PLL_USER_CTL_PLLOUT_LV_TEST_BMSK                                                0x10
#define HWIO_GCC_BIMC_PLL_USER_CTL_PLLOUT_LV_TEST_SHFT                                                 0x4
#define HWIO_GCC_BIMC_PLL_USER_CTL_PLLOUT_LV_EARLY_BMSK                                                0x8
#define HWIO_GCC_BIMC_PLL_USER_CTL_PLLOUT_LV_EARLY_SHFT                                                0x3
#define HWIO_GCC_BIMC_PLL_USER_CTL_PLLOUT_LV_BIST_BMSK                                                 0x4
#define HWIO_GCC_BIMC_PLL_USER_CTL_PLLOUT_LV_BIST_SHFT                                                 0x2
#define HWIO_GCC_BIMC_PLL_USER_CTL_PLLOUT_LV_AUX_BMSK                                                  0x2
#define HWIO_GCC_BIMC_PLL_USER_CTL_PLLOUT_LV_AUX_SHFT                                                  0x1
#define HWIO_GCC_BIMC_PLL_USER_CTL_PLLOUT_LV_MAIN_BMSK                                                 0x1
#define HWIO_GCC_BIMC_PLL_USER_CTL_PLLOUT_LV_MAIN_SHFT                                                 0x0

#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x00023014)
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_RMSK                                                       0xffffffff
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_GCC_BIMC_PLL_CONFIG_CTL_ADDR, HWIO_GCC_BIMC_PLL_CONFIG_CTL_RMSK)
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_PLL_CONFIG_CTL_ADDR, m)
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_PLL_CONFIG_CTL_ADDR,v)
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_PLL_CONFIG_CTL_ADDR,m,v,HWIO_GCC_BIMC_PLL_CONFIG_CTL_IN)
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_RESERVE_BITS31_30_BMSK                                     0xc0000000
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_RESERVE_BITS31_30_SHFT                                           0x1e
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_FORCE_PFD_UP_BMSK                                          0x20000000
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_FORCE_PFD_UP_SHFT                                                0x1d
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_FORCE_PFD_DOWN_BMSK                                        0x10000000
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_FORCE_PFD_DOWN_SHFT                                              0x1c
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_NMOSC_FREQ_CTRL_BMSK                                        0xc000000
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_NMOSC_FREQ_CTRL_SHFT                                             0x1a
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_PFD_DZSEL_BMSK                                              0x3000000
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_PFD_DZSEL_SHFT                                                   0x18
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_NMOSC_EN_BMSK                                                0x800000
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_NMOSC_EN_SHFT                                                    0x17
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_RESERVE_BIT22_BMSK                                           0x400000
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_RESERVE_BIT22_SHFT                                               0x16
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_ICP_DIV_BMSK                                                 0x300000
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_ICP_DIV_SHFT                                                     0x14
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_IREG_DIV_BMSK                                                 0xc0000
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_IREG_DIV_SHFT                                                    0x12
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_CUSEL_BMSK                                                    0x30000
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_CUSEL_SHFT                                                       0x10
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_REF_MODE_BMSK                                                  0x8000
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_REF_MODE_SHFT                                                     0xf
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_RESERVE_BIT14_BMSK                                             0x4000
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_RESERVE_BIT14_SHFT                                                0xe
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_CFG_LOCKDET_BMSK                                               0x3000
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_CFG_LOCKDET_SHFT                                                  0xc
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_FORCE_ISEED_BMSK                                                0x800
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_FORCE_ISEED_SHFT                                                  0xb
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_RESERVE_BITS10_0_BMSK                                           0x7ff
#define HWIO_GCC_BIMC_PLL_CONFIG_CTL_RESERVE_BITS10_0_SHFT                                             0x0

#define HWIO_GCC_BIMC_PLL_TEST_CTL_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00023018)
#define HWIO_GCC_BIMC_PLL_TEST_CTL_RMSK                                                         0xffffffff
#define HWIO_GCC_BIMC_PLL_TEST_CTL_IN          \
        in_dword_masked(HWIO_GCC_BIMC_PLL_TEST_CTL_ADDR, HWIO_GCC_BIMC_PLL_TEST_CTL_RMSK)
#define HWIO_GCC_BIMC_PLL_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_PLL_TEST_CTL_ADDR, m)
#define HWIO_GCC_BIMC_PLL_TEST_CTL_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_PLL_TEST_CTL_ADDR,v)
#define HWIO_GCC_BIMC_PLL_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_PLL_TEST_CTL_ADDR,m,v,HWIO_GCC_BIMC_PLL_TEST_CTL_IN)
#define HWIO_GCC_BIMC_PLL_TEST_CTL_RESERVE_BITS31_15_BMSK                                       0xffff8000
#define HWIO_GCC_BIMC_PLL_TEST_CTL_RESERVE_BITS31_15_SHFT                                              0xf
#define HWIO_GCC_BIMC_PLL_TEST_CTL_PLLOUT_LV_TEST_SEL_BMSK                                          0x4000
#define HWIO_GCC_BIMC_PLL_TEST_CTL_PLLOUT_LV_TEST_SEL_SHFT                                             0xe
#define HWIO_GCC_BIMC_PLL_TEST_CTL_RESERVE_BITS13_10_BMSK                                           0x3c00
#define HWIO_GCC_BIMC_PLL_TEST_CTL_RESERVE_BITS13_10_SHFT                                              0xa
#define HWIO_GCC_BIMC_PLL_TEST_CTL_ICP_TST_EN_BMSK                                                   0x200
#define HWIO_GCC_BIMC_PLL_TEST_CTL_ICP_TST_EN_SHFT                                                     0x9
#define HWIO_GCC_BIMC_PLL_TEST_CTL_ICP_EXT_SEL_BMSK                                                  0x100
#define HWIO_GCC_BIMC_PLL_TEST_CTL_ICP_EXT_SEL_SHFT                                                    0x8
#define HWIO_GCC_BIMC_PLL_TEST_CTL_DTEST_SEL_BMSK                                                     0x80
#define HWIO_GCC_BIMC_PLL_TEST_CTL_DTEST_SEL_SHFT                                                      0x7
#define HWIO_GCC_BIMC_PLL_TEST_CTL_BYP_TESTAMP_BMSK                                                   0x40
#define HWIO_GCC_BIMC_PLL_TEST_CTL_BYP_TESTAMP_SHFT                                                    0x6
#define HWIO_GCC_BIMC_PLL_TEST_CTL_ATEST1_SEL_BMSK                                                    0x30
#define HWIO_GCC_BIMC_PLL_TEST_CTL_ATEST1_SEL_SHFT                                                     0x4
#define HWIO_GCC_BIMC_PLL_TEST_CTL_ATEST0_SEL_BMSK                                                     0xc
#define HWIO_GCC_BIMC_PLL_TEST_CTL_ATEST0_SEL_SHFT                                                     0x2
#define HWIO_GCC_BIMC_PLL_TEST_CTL_ATEST1_EN_BMSK                                                      0x2
#define HWIO_GCC_BIMC_PLL_TEST_CTL_ATEST1_EN_SHFT                                                      0x1
#define HWIO_GCC_BIMC_PLL_TEST_CTL_ATEST0_EN_BMSK                                                      0x1
#define HWIO_GCC_BIMC_PLL_TEST_CTL_ATEST0_EN_SHFT                                                      0x0

#define HWIO_GCC_BIMC_PLL_STATUS_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0002301c)
#define HWIO_GCC_BIMC_PLL_STATUS_RMSK                                                              0x3ffff
#define HWIO_GCC_BIMC_PLL_STATUS_IN          \
        in_dword_masked(HWIO_GCC_BIMC_PLL_STATUS_ADDR, HWIO_GCC_BIMC_PLL_STATUS_RMSK)
#define HWIO_GCC_BIMC_PLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_PLL_STATUS_ADDR, m)
#define HWIO_GCC_BIMC_PLL_STATUS_PLL_ACTIVE_FLAG_BMSK                                              0x20000
#define HWIO_GCC_BIMC_PLL_STATUS_PLL_ACTIVE_FLAG_SHFT                                                 0x11
#define HWIO_GCC_BIMC_PLL_STATUS_PLL_LOCK_DET_BMSK                                                 0x10000
#define HWIO_GCC_BIMC_PLL_STATUS_PLL_LOCK_DET_SHFT                                                    0x10
#define HWIO_GCC_BIMC_PLL_STATUS_PLL_D_BMSK                                                         0xffff
#define HWIO_GCC_BIMC_PLL_STATUS_PLL_D_SHFT                                                            0x0

#define HWIO_GCC_SYSTEM_NOC_BCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00026000)
#define HWIO_GCC_SYSTEM_NOC_BCR_RMSK                                                                   0x1
#define HWIO_GCC_SYSTEM_NOC_BCR_IN          \
        in_dword_masked(HWIO_GCC_SYSTEM_NOC_BCR_ADDR, HWIO_GCC_SYSTEM_NOC_BCR_RMSK)
#define HWIO_GCC_SYSTEM_NOC_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SYSTEM_NOC_BCR_ADDR, m)
#define HWIO_GCC_SYSTEM_NOC_BCR_OUT(v)      \
        out_dword(HWIO_GCC_SYSTEM_NOC_BCR_ADDR,v)
#define HWIO_GCC_SYSTEM_NOC_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SYSTEM_NOC_BCR_ADDR,m,v,HWIO_GCC_SYSTEM_NOC_BCR_IN)
#define HWIO_GCC_SYSTEM_NOC_BCR_BLK_ARES_BMSK                                                          0x1
#define HWIO_GCC_SYSTEM_NOC_BCR_BLK_ARES_SHFT                                                          0x0

#define HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_ADDR                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00026004)
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_RMSK                                                 0x80000013
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_ADDR, HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_RMSK)
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_ADDR, m)
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_ADDR,v)
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_ADDR,m,v,HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_IN)
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_ROOT_OFF_BMSK                                        0x80000000
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_ROOT_OFF_SHFT                                              0x1f
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                        0x10
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                         0x4
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_ROOT_EN_BMSK                                                0x2
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_ROOT_EN_SHFT                                                0x1
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_UPDATE_BMSK                                                 0x1
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CMD_RCGR_UPDATE_SHFT                                                 0x0

#define HWIO_GCC_SYSTEM_NOC_BFDCD_CFG_RCGR_ADDR                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00026008)
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CFG_RCGR_RMSK                                                      0x71f
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_SYSTEM_NOC_BFDCD_CFG_RCGR_ADDR, HWIO_GCC_SYSTEM_NOC_BFDCD_CFG_RCGR_RMSK)
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_SYSTEM_NOC_BFDCD_CFG_RCGR_ADDR, m)
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_SYSTEM_NOC_BFDCD_CFG_RCGR_ADDR,v)
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SYSTEM_NOC_BFDCD_CFG_RCGR_ADDR,m,v,HWIO_GCC_SYSTEM_NOC_BFDCD_CFG_RCGR_IN)
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CFG_RCGR_SRC_SEL_BMSK                                              0x700
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CFG_RCGR_SRC_SEL_SHFT                                                0x8
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CFG_RCGR_SRC_DIV_BMSK                                               0x1f
#define HWIO_GCC_SYSTEM_NOC_BFDCD_CFG_RCGR_SRC_DIV_SHFT                                                0x0

#define HWIO_GCC_SNOC_QOSGEN_EXTREF_CTL_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x0002601c)
#define HWIO_GCC_SNOC_QOSGEN_EXTREF_CTL_RMSK                                                       0x30001
#define HWIO_GCC_SNOC_QOSGEN_EXTREF_CTL_IN          \
        in_dword_masked(HWIO_GCC_SNOC_QOSGEN_EXTREF_CTL_ADDR, HWIO_GCC_SNOC_QOSGEN_EXTREF_CTL_RMSK)
#define HWIO_GCC_SNOC_QOSGEN_EXTREF_CTL_INM(m)      \
        in_dword_masked(HWIO_GCC_SNOC_QOSGEN_EXTREF_CTL_ADDR, m)
#define HWIO_GCC_SNOC_QOSGEN_EXTREF_CTL_OUT(v)      \
        out_dword(HWIO_GCC_SNOC_QOSGEN_EXTREF_CTL_ADDR,v)
#define HWIO_GCC_SNOC_QOSGEN_EXTREF_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SNOC_QOSGEN_EXTREF_CTL_ADDR,m,v,HWIO_GCC_SNOC_QOSGEN_EXTREF_CTL_IN)
#define HWIO_GCC_SNOC_QOSGEN_EXTREF_CTL_DIVIDE_BMSK                                                0x30000
#define HWIO_GCC_SNOC_QOSGEN_EXTREF_CTL_DIVIDE_SHFT                                                   0x10
#define HWIO_GCC_SNOC_QOSGEN_EXTREF_CTL_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_SNOC_QOSGEN_EXTREF_CTL_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00027000)
#define HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_RMSK                                                      0x80000013
#define HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_ADDR, HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_RMSK)
#define HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_ADDR, m)
#define HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_ADDR,v)
#define HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_ADDR,m,v,HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_IN)
#define HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_ROOT_OFF_BMSK                                             0x80000000
#define HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_ROOT_OFF_SHFT                                                   0x1f
#define HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                             0x10
#define HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                              0x4
#define HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_ROOT_EN_BMSK                                                     0x2
#define HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_ROOT_EN_SHFT                                                     0x1
#define HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_UPDATE_BMSK                                                      0x1
#define HWIO_GCC_PCNOC_BFDCD_CMD_RCGR_UPDATE_SHFT                                                      0x0

#define HWIO_GCC_PCNOC_BFDCD_CFG_RCGR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00027004)
#define HWIO_GCC_PCNOC_BFDCD_CFG_RCGR_RMSK                                                           0x71f
#define HWIO_GCC_PCNOC_BFDCD_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BFDCD_CFG_RCGR_ADDR, HWIO_GCC_PCNOC_BFDCD_CFG_RCGR_RMSK)
#define HWIO_GCC_PCNOC_BFDCD_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BFDCD_CFG_RCGR_ADDR, m)
#define HWIO_GCC_PCNOC_BFDCD_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BFDCD_CFG_RCGR_ADDR,v)
#define HWIO_GCC_PCNOC_BFDCD_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BFDCD_CFG_RCGR_ADDR,m,v,HWIO_GCC_PCNOC_BFDCD_CFG_RCGR_IN)
#define HWIO_GCC_PCNOC_BFDCD_CFG_RCGR_SRC_SEL_BMSK                                                   0x700
#define HWIO_GCC_PCNOC_BFDCD_CFG_RCGR_SRC_SEL_SHFT                                                     0x8
#define HWIO_GCC_PCNOC_BFDCD_CFG_RCGR_SRC_DIV_BMSK                                                    0x1f
#define HWIO_GCC_PCNOC_BFDCD_CFG_RCGR_SRC_DIV_SHFT                                                     0x0

#define HWIO_GCC_SYS_NOC_AXI_CBCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00026020)
#define HWIO_GCC_SYS_NOC_AXI_CBCR_RMSK                                                          0x80000001
#define HWIO_GCC_SYS_NOC_AXI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SYS_NOC_AXI_CBCR_ADDR, HWIO_GCC_SYS_NOC_AXI_CBCR_RMSK)
#define HWIO_GCC_SYS_NOC_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SYS_NOC_AXI_CBCR_ADDR, m)
#define HWIO_GCC_SYS_NOC_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SYS_NOC_AXI_CBCR_ADDR,v)
#define HWIO_GCC_SYS_NOC_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SYS_NOC_AXI_CBCR_ADDR,m,v,HWIO_GCC_SYS_NOC_AXI_CBCR_IN)
#define HWIO_GCC_SYS_NOC_AXI_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_SYS_NOC_AXI_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_GCC_SYS_NOC_AXI_CBCR_CLK_ENABLE_BMSK                                                      0x1
#define HWIO_GCC_SYS_NOC_AXI_CBCR_CLK_ENABLE_SHFT                                                      0x0

#define HWIO_GCC_SYS_NOC_QDSS_STM_AXI_CBCR_ADDR                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00026024)
#define HWIO_GCC_SYS_NOC_QDSS_STM_AXI_CBCR_RMSK                                                 0x80000001
#define HWIO_GCC_SYS_NOC_QDSS_STM_AXI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SYS_NOC_QDSS_STM_AXI_CBCR_ADDR, HWIO_GCC_SYS_NOC_QDSS_STM_AXI_CBCR_RMSK)
#define HWIO_GCC_SYS_NOC_QDSS_STM_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SYS_NOC_QDSS_STM_AXI_CBCR_ADDR, m)
#define HWIO_GCC_SYS_NOC_QDSS_STM_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SYS_NOC_QDSS_STM_AXI_CBCR_ADDR,v)
#define HWIO_GCC_SYS_NOC_QDSS_STM_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SYS_NOC_QDSS_STM_AXI_CBCR_ADDR,m,v,HWIO_GCC_SYS_NOC_QDSS_STM_AXI_CBCR_IN)
#define HWIO_GCC_SYS_NOC_QDSS_STM_AXI_CBCR_CLK_OFF_BMSK                                         0x80000000
#define HWIO_GCC_SYS_NOC_QDSS_STM_AXI_CBCR_CLK_OFF_SHFT                                               0x1f
#define HWIO_GCC_SYS_NOC_QDSS_STM_AXI_CBCR_CLK_ENABLE_BMSK                                             0x1
#define HWIO_GCC_SYS_NOC_QDSS_STM_AXI_CBCR_CLK_ENABLE_SHFT                                             0x0

#define HWIO_GCC_SYS_NOC_APSS_AHB_CBCR_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00026028)
#define HWIO_GCC_SYS_NOC_APSS_AHB_CBCR_RMSK                                                     0x80000000
#define HWIO_GCC_SYS_NOC_APSS_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SYS_NOC_APSS_AHB_CBCR_ADDR, HWIO_GCC_SYS_NOC_APSS_AHB_CBCR_RMSK)
#define HWIO_GCC_SYS_NOC_APSS_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SYS_NOC_APSS_AHB_CBCR_ADDR, m)
#define HWIO_GCC_SYS_NOC_APSS_AHB_CBCR_CLK_OFF_BMSK                                             0x80000000
#define HWIO_GCC_SYS_NOC_APSS_AHB_CBCR_CLK_OFF_SHFT                                                   0x1f

#define HWIO_GCC_SNOC_PCNOC_AHB_CBCR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x0002602c)
#define HWIO_GCC_SNOC_PCNOC_AHB_CBCR_RMSK                                                       0x80000001
#define HWIO_GCC_SNOC_PCNOC_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SNOC_PCNOC_AHB_CBCR_ADDR, HWIO_GCC_SNOC_PCNOC_AHB_CBCR_RMSK)
#define HWIO_GCC_SNOC_PCNOC_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SNOC_PCNOC_AHB_CBCR_ADDR, m)
#define HWIO_GCC_SNOC_PCNOC_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SNOC_PCNOC_AHB_CBCR_ADDR,v)
#define HWIO_GCC_SNOC_PCNOC_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SNOC_PCNOC_AHB_CBCR_ADDR,m,v,HWIO_GCC_SNOC_PCNOC_AHB_CBCR_IN)
#define HWIO_GCC_SNOC_PCNOC_AHB_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_GCC_SNOC_PCNOC_AHB_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_GCC_SNOC_PCNOC_AHB_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_GCC_SNOC_PCNOC_AHB_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_GCC_SYS_NOC_AT_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00026030)
#define HWIO_GCC_SYS_NOC_AT_CBCR_RMSK                                                           0x80000001
#define HWIO_GCC_SYS_NOC_AT_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SYS_NOC_AT_CBCR_ADDR, HWIO_GCC_SYS_NOC_AT_CBCR_RMSK)
#define HWIO_GCC_SYS_NOC_AT_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SYS_NOC_AT_CBCR_ADDR, m)
#define HWIO_GCC_SYS_NOC_AT_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SYS_NOC_AT_CBCR_ADDR,v)
#define HWIO_GCC_SYS_NOC_AT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SYS_NOC_AT_CBCR_ADDR,m,v,HWIO_GCC_SYS_NOC_AT_CBCR_IN)
#define HWIO_GCC_SYS_NOC_AT_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_SYS_NOC_AT_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_SYS_NOC_AT_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_SYS_NOC_AT_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_PCNOC_BCR_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00027018)
#define HWIO_GCC_PCNOC_BCR_RMSK                                                                        0x1
#define HWIO_GCC_PCNOC_BCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BCR_ADDR, HWIO_GCC_PCNOC_BCR_RMSK)
#define HWIO_GCC_PCNOC_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BCR_ADDR, m)
#define HWIO_GCC_PCNOC_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BCR_ADDR,v)
#define HWIO_GCC_PCNOC_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BCR_ADDR,m,v,HWIO_GCC_PCNOC_BCR_IN)
#define HWIO_GCC_PCNOC_BCR_BLK_ARES_BMSK                                                               0x1
#define HWIO_GCC_PCNOC_BCR_BLK_ARES_SHFT                                                               0x0

#define HWIO_GCC_PCNOC_AHB_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0002701c)
#define HWIO_GCC_PCNOC_AHB_CBCR_RMSK                                                            0x80000001
#define HWIO_GCC_PCNOC_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_AHB_CBCR_ADDR, HWIO_GCC_PCNOC_AHB_CBCR_RMSK)
#define HWIO_GCC_PCNOC_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_AHB_CBCR_ADDR, m)
#define HWIO_GCC_PCNOC_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_AHB_CBCR_ADDR,v)
#define HWIO_GCC_PCNOC_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_AHB_CBCR_ADDR,m,v,HWIO_GCC_PCNOC_AHB_CBCR_IN)
#define HWIO_GCC_PCNOC_AHB_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_PCNOC_AHB_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_PCNOC_AHB_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_PCNOC_AHB_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_PCNOC_DDR_CFG_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00027020)
#define HWIO_GCC_PCNOC_DDR_CFG_CBCR_RMSK                                                        0x80000001
#define HWIO_GCC_PCNOC_DDR_CFG_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_DDR_CFG_CBCR_ADDR, HWIO_GCC_PCNOC_DDR_CFG_CBCR_RMSK)
#define HWIO_GCC_PCNOC_DDR_CFG_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_DDR_CFG_CBCR_ADDR, m)
#define HWIO_GCC_PCNOC_DDR_CFG_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_DDR_CFG_CBCR_ADDR,v)
#define HWIO_GCC_PCNOC_DDR_CFG_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_DDR_CFG_CBCR_ADDR,m,v,HWIO_GCC_PCNOC_DDR_CFG_CBCR_IN)
#define HWIO_GCC_PCNOC_DDR_CFG_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_PCNOC_DDR_CFG_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_PCNOC_DDR_CFG_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_PCNOC_DDR_CFG_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_PCNOC_RPM_AHB_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00027024)
#define HWIO_GCC_PCNOC_RPM_AHB_CBCR_RMSK                                                        0x80000001
#define HWIO_GCC_PCNOC_RPM_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_RPM_AHB_CBCR_ADDR, HWIO_GCC_PCNOC_RPM_AHB_CBCR_RMSK)
#define HWIO_GCC_PCNOC_RPM_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_RPM_AHB_CBCR_ADDR, m)
#define HWIO_GCC_PCNOC_RPM_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_RPM_AHB_CBCR_ADDR,v)
#define HWIO_GCC_PCNOC_RPM_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_RPM_AHB_CBCR_ADDR,m,v,HWIO_GCC_PCNOC_RPM_AHB_CBCR_IN)
#define HWIO_GCC_PCNOC_RPM_AHB_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_PCNOC_RPM_AHB_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_PCNOC_RPM_AHB_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_PCNOC_RPM_AHB_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_PCNOC_AT_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00027028)
#define HWIO_GCC_PCNOC_AT_CBCR_RMSK                                                             0x80000001
#define HWIO_GCC_PCNOC_AT_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_AT_CBCR_ADDR, HWIO_GCC_PCNOC_AT_CBCR_RMSK)
#define HWIO_GCC_PCNOC_AT_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_AT_CBCR_ADDR, m)
#define HWIO_GCC_PCNOC_AT_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_AT_CBCR_ADDR,v)
#define HWIO_GCC_PCNOC_AT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_AT_CBCR_ADDR,m,v,HWIO_GCC_PCNOC_AT_CBCR_IN)
#define HWIO_GCC_PCNOC_AT_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_PCNOC_AT_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_PCNOC_AT_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_GCC_PCNOC_AT_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00017000)
#define HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_RMSK                                                     0xf0008001
#define HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_ADDR, HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_RMSK)
#define HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_ADDR, m)
#define HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_ADDR,v)
#define HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_ADDR,m,v,HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_IN)
#define HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_CLK_OFF_BMSK                                             0x80000000
#define HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_CLK_OFF_SHFT                                                   0x1f
#define HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                            0x70000000
#define HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                  0x1c
#define HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                    0x8000
#define HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                       0xf
#define HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_GCC_NOC_CONF_XPU_AHB_CBCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_GCC_IMEM_BCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0000e000)
#define HWIO_GCC_IMEM_BCR_RMSK                                                                         0x1
#define HWIO_GCC_IMEM_BCR_IN          \
        in_dword_masked(HWIO_GCC_IMEM_BCR_ADDR, HWIO_GCC_IMEM_BCR_RMSK)
#define HWIO_GCC_IMEM_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_IMEM_BCR_ADDR, m)
#define HWIO_GCC_IMEM_BCR_OUT(v)      \
        out_dword(HWIO_GCC_IMEM_BCR_ADDR,v)
#define HWIO_GCC_IMEM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_IMEM_BCR_ADDR,m,v,HWIO_GCC_IMEM_BCR_IN)
#define HWIO_GCC_IMEM_BCR_BLK_ARES_BMSK                                                                0x1
#define HWIO_GCC_IMEM_BCR_BLK_ARES_SHFT                                                                0x0

#define HWIO_GCC_IMEM_AXI_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0000e004)
#define HWIO_GCC_IMEM_AXI_CBCR_RMSK                                                             0xf000fff0
#define HWIO_GCC_IMEM_AXI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_IMEM_AXI_CBCR_ADDR, HWIO_GCC_IMEM_AXI_CBCR_RMSK)
#define HWIO_GCC_IMEM_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_IMEM_AXI_CBCR_ADDR, m)
#define HWIO_GCC_IMEM_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_IMEM_AXI_CBCR_ADDR,v)
#define HWIO_GCC_IMEM_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_IMEM_AXI_CBCR_ADDR,m,v,HWIO_GCC_IMEM_AXI_CBCR_IN)
#define HWIO_GCC_IMEM_AXI_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_IMEM_AXI_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_IMEM_AXI_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                    0x70000000
#define HWIO_GCC_IMEM_AXI_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                          0x1c
#define HWIO_GCC_IMEM_AXI_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                            0x8000
#define HWIO_GCC_IMEM_AXI_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                               0xf
#define HWIO_GCC_IMEM_AXI_CBCR_FORCE_MEM_CORE_ON_BMSK                                               0x4000
#define HWIO_GCC_IMEM_AXI_CBCR_FORCE_MEM_CORE_ON_SHFT                                                  0xe
#define HWIO_GCC_IMEM_AXI_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                             0x2000
#define HWIO_GCC_IMEM_AXI_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                0xd
#define HWIO_GCC_IMEM_AXI_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                            0x1000
#define HWIO_GCC_IMEM_AXI_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                               0xc
#define HWIO_GCC_IMEM_AXI_CBCR_WAKEUP_BMSK                                                           0xf00
#define HWIO_GCC_IMEM_AXI_CBCR_WAKEUP_SHFT                                                             0x8
#define HWIO_GCC_IMEM_AXI_CBCR_SLEEP_BMSK                                                             0xf0
#define HWIO_GCC_IMEM_AXI_CBCR_SLEEP_SHFT                                                              0x4

#define HWIO_GCC_IMEM_CFG_AHB_CBCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0000e008)
#define HWIO_GCC_IMEM_CFG_AHB_CBCR_RMSK                                                         0xf0008001
#define HWIO_GCC_IMEM_CFG_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_IMEM_CFG_AHB_CBCR_ADDR, HWIO_GCC_IMEM_CFG_AHB_CBCR_RMSK)
#define HWIO_GCC_IMEM_CFG_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_IMEM_CFG_AHB_CBCR_ADDR, m)
#define HWIO_GCC_IMEM_CFG_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_IMEM_CFG_AHB_CBCR_ADDR,v)
#define HWIO_GCC_IMEM_CFG_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_IMEM_CFG_AHB_CBCR_ADDR,m,v,HWIO_GCC_IMEM_CFG_AHB_CBCR_IN)
#define HWIO_GCC_IMEM_CFG_AHB_CBCR_CLK_OFF_BMSK                                                 0x80000000
#define HWIO_GCC_IMEM_CFG_AHB_CBCR_CLK_OFF_SHFT                                                       0x1f
#define HWIO_GCC_IMEM_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                0x70000000
#define HWIO_GCC_IMEM_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                      0x1c
#define HWIO_GCC_IMEM_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                        0x8000
#define HWIO_GCC_IMEM_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                           0xf
#define HWIO_GCC_IMEM_CFG_AHB_CBCR_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_GCC_IMEM_CFG_AHB_CBCR_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_GCC_MMSS_BCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0004b000)
#define HWIO_GCC_MMSS_BCR_RMSK                                                                         0x3
#define HWIO_GCC_MMSS_BCR_IN          \
        in_dword_masked(HWIO_GCC_MMSS_BCR_ADDR, HWIO_GCC_MMSS_BCR_RMSK)
#define HWIO_GCC_MMSS_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMSS_BCR_ADDR, m)
#define HWIO_GCC_MMSS_BCR_OUT(v)      \
        out_dword(HWIO_GCC_MMSS_BCR_ADDR,v)
#define HWIO_GCC_MMSS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMSS_BCR_ADDR,m,v,HWIO_GCC_MMSS_BCR_IN)
#define HWIO_GCC_MMSS_BCR_GMEM_AXI_ARES_BMSK                                                           0x2
#define HWIO_GCC_MMSS_BCR_GMEM_AXI_ARES_SHFT                                                           0x1
#define HWIO_GCC_MMSS_BCR_BLK_ARES_BMSK                                                                0x1
#define HWIO_GCC_MMSS_BCR_BLK_ARES_SHFT                                                                0x0

#define HWIO_GCC_GMEM_SYS_NOC_AXI_CBCR_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x0005903c)
#define HWIO_GCC_GMEM_SYS_NOC_AXI_CBCR_RMSK                                                     0xf0008000
#define HWIO_GCC_GMEM_SYS_NOC_AXI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_GMEM_SYS_NOC_AXI_CBCR_ADDR, HWIO_GCC_GMEM_SYS_NOC_AXI_CBCR_RMSK)
#define HWIO_GCC_GMEM_SYS_NOC_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_GMEM_SYS_NOC_AXI_CBCR_ADDR, m)
#define HWIO_GCC_GMEM_SYS_NOC_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_GMEM_SYS_NOC_AXI_CBCR_ADDR,v)
#define HWIO_GCC_GMEM_SYS_NOC_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GMEM_SYS_NOC_AXI_CBCR_ADDR,m,v,HWIO_GCC_GMEM_SYS_NOC_AXI_CBCR_IN)
#define HWIO_GCC_GMEM_SYS_NOC_AXI_CBCR_CLK_OFF_BMSK                                             0x80000000
#define HWIO_GCC_GMEM_SYS_NOC_AXI_CBCR_CLK_OFF_SHFT                                                   0x1f
#define HWIO_GCC_GMEM_SYS_NOC_AXI_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                            0x70000000
#define HWIO_GCC_GMEM_SYS_NOC_AXI_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                  0x1c
#define HWIO_GCC_GMEM_SYS_NOC_AXI_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                    0x8000
#define HWIO_GCC_GMEM_SYS_NOC_AXI_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                       0xf

#define HWIO_GCC_BIMC_GFX_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00031024)
#define HWIO_GCC_BIMC_GFX_CBCR_RMSK                                                             0x80007ff1
#define HWIO_GCC_BIMC_GFX_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BIMC_GFX_CBCR_ADDR, HWIO_GCC_BIMC_GFX_CBCR_RMSK)
#define HWIO_GCC_BIMC_GFX_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_GFX_CBCR_ADDR, m)
#define HWIO_GCC_BIMC_GFX_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_GFX_CBCR_ADDR,v)
#define HWIO_GCC_BIMC_GFX_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_GFX_CBCR_ADDR,m,v,HWIO_GCC_BIMC_GFX_CBCR_IN)
#define HWIO_GCC_BIMC_GFX_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_BIMC_GFX_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_BIMC_GFX_CBCR_FORCE_MEM_CORE_ON_BMSK                                               0x4000
#define HWIO_GCC_BIMC_GFX_CBCR_FORCE_MEM_CORE_ON_SHFT                                                  0xe
#define HWIO_GCC_BIMC_GFX_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                             0x2000
#define HWIO_GCC_BIMC_GFX_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                0xd
#define HWIO_GCC_BIMC_GFX_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                            0x1000
#define HWIO_GCC_BIMC_GFX_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                               0xc
#define HWIO_GCC_BIMC_GFX_CBCR_WAKEUP_BMSK                                                           0xf00
#define HWIO_GCC_BIMC_GFX_CBCR_WAKEUP_SHFT                                                             0x8
#define HWIO_GCC_BIMC_GFX_CBCR_SLEEP_BMSK                                                             0xf0
#define HWIO_GCC_BIMC_GFX_CBCR_SLEEP_SHFT                                                              0x4
#define HWIO_GCC_BIMC_GFX_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_GCC_BIMC_GFX_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_GCC_MSS_CFG_AHB_CBCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00049000)
#define HWIO_GCC_MSS_CFG_AHB_CBCR_RMSK                                                          0xf0008001
#define HWIO_GCC_MSS_CFG_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MSS_CFG_AHB_CBCR_ADDR, HWIO_GCC_MSS_CFG_AHB_CBCR_RMSK)
#define HWIO_GCC_MSS_CFG_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_CFG_AHB_CBCR_ADDR, m)
#define HWIO_GCC_MSS_CFG_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MSS_CFG_AHB_CBCR_ADDR,v)
#define HWIO_GCC_MSS_CFG_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_CFG_AHB_CBCR_ADDR,m,v,HWIO_GCC_MSS_CFG_AHB_CBCR_IN)
#define HWIO_GCC_MSS_CFG_AHB_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_MSS_CFG_AHB_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_GCC_MSS_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                 0x70000000
#define HWIO_GCC_MSS_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                       0x1c
#define HWIO_GCC_MSS_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                         0x8000
#define HWIO_GCC_MSS_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                            0xf
#define HWIO_GCC_MSS_CFG_AHB_CBCR_CLK_ENABLE_BMSK                                                      0x1
#define HWIO_GCC_MSS_CFG_AHB_CBCR_CLK_ENABLE_SHFT                                                      0x0

#define HWIO_GCC_MSS_Q6_BIMC_AXI_CBCR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00049004)
#define HWIO_GCC_MSS_Q6_BIMC_AXI_CBCR_RMSK                                                      0x80000003
#define HWIO_GCC_MSS_Q6_BIMC_AXI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MSS_Q6_BIMC_AXI_CBCR_ADDR, HWIO_GCC_MSS_Q6_BIMC_AXI_CBCR_RMSK)
#define HWIO_GCC_MSS_Q6_BIMC_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_Q6_BIMC_AXI_CBCR_ADDR, m)
#define HWIO_GCC_MSS_Q6_BIMC_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MSS_Q6_BIMC_AXI_CBCR_ADDR,v)
#define HWIO_GCC_MSS_Q6_BIMC_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_Q6_BIMC_AXI_CBCR_ADDR,m,v,HWIO_GCC_MSS_Q6_BIMC_AXI_CBCR_IN)
#define HWIO_GCC_MSS_Q6_BIMC_AXI_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_GCC_MSS_Q6_BIMC_AXI_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_GCC_MSS_Q6_BIMC_AXI_CBCR_HW_CTL_BMSK                                                      0x2
#define HWIO_GCC_MSS_Q6_BIMC_AXI_CBCR_HW_CTL_SHFT                                                      0x1
#define HWIO_GCC_MSS_Q6_BIMC_AXI_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_GCC_MSS_Q6_BIMC_AXI_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_GCC_RPM_CFG_XPU_CBCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00017004)
#define HWIO_GCC_RPM_CFG_XPU_CBCR_RMSK                                                          0xf0008001
#define HWIO_GCC_RPM_CFG_XPU_CBCR_IN          \
        in_dword_masked(HWIO_GCC_RPM_CFG_XPU_CBCR_ADDR, HWIO_GCC_RPM_CFG_XPU_CBCR_RMSK)
#define HWIO_GCC_RPM_CFG_XPU_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPM_CFG_XPU_CBCR_ADDR, m)
#define HWIO_GCC_RPM_CFG_XPU_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_RPM_CFG_XPU_CBCR_ADDR,v)
#define HWIO_GCC_RPM_CFG_XPU_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPM_CFG_XPU_CBCR_ADDR,m,v,HWIO_GCC_RPM_CFG_XPU_CBCR_IN)
#define HWIO_GCC_RPM_CFG_XPU_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_RPM_CFG_XPU_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_GCC_RPM_CFG_XPU_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                 0x70000000
#define HWIO_GCC_RPM_CFG_XPU_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                       0x1c
#define HWIO_GCC_RPM_CFG_XPU_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                         0x8000
#define HWIO_GCC_RPM_CFG_XPU_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                            0xf
#define HWIO_GCC_RPM_CFG_XPU_CBCR_CLK_ENABLE_BMSK                                                      0x1
#define HWIO_GCC_RPM_CFG_XPU_CBCR_CLK_ENABLE_SHFT                                                      0x0

#define HWIO_GCC_QDSS_BCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00029000)
#define HWIO_GCC_QDSS_BCR_RMSK                                                                         0x1
#define HWIO_GCC_QDSS_BCR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_BCR_ADDR, HWIO_GCC_QDSS_BCR_RMSK)
#define HWIO_GCC_QDSS_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_BCR_ADDR, m)
#define HWIO_GCC_QDSS_BCR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_BCR_ADDR,v)
#define HWIO_GCC_QDSS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_BCR_ADDR,m,v,HWIO_GCC_QDSS_BCR_IN)
#define HWIO_GCC_QDSS_BCR_BLK_ARES_BMSK                                                                0x1
#define HWIO_GCC_QDSS_BCR_BLK_ARES_SHFT                                                                0x0

#define HWIO_GCC_QDSS_DAP_AHB_CBCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00029004)
#define HWIO_GCC_QDSS_DAP_AHB_CBCR_RMSK                                                         0x80000001
#define HWIO_GCC_QDSS_DAP_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_DAP_AHB_CBCR_ADDR, HWIO_GCC_QDSS_DAP_AHB_CBCR_RMSK)
#define HWIO_GCC_QDSS_DAP_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_DAP_AHB_CBCR_ADDR, m)
#define HWIO_GCC_QDSS_DAP_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_DAP_AHB_CBCR_ADDR,v)
#define HWIO_GCC_QDSS_DAP_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_DAP_AHB_CBCR_ADDR,m,v,HWIO_GCC_QDSS_DAP_AHB_CBCR_IN)
#define HWIO_GCC_QDSS_DAP_AHB_CBCR_CLK_OFF_BMSK                                                 0x80000000
#define HWIO_GCC_QDSS_DAP_AHB_CBCR_CLK_OFF_SHFT                                                       0x1f
#define HWIO_GCC_QDSS_DAP_AHB_CBCR_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_GCC_QDSS_DAP_AHB_CBCR_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_GCC_QDSS_CFG_AHB_CBCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00029008)
#define HWIO_GCC_QDSS_CFG_AHB_CBCR_RMSK                                                         0xf0008001
#define HWIO_GCC_QDSS_CFG_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_CFG_AHB_CBCR_ADDR, HWIO_GCC_QDSS_CFG_AHB_CBCR_RMSK)
#define HWIO_GCC_QDSS_CFG_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_CFG_AHB_CBCR_ADDR, m)
#define HWIO_GCC_QDSS_CFG_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_CFG_AHB_CBCR_ADDR,v)
#define HWIO_GCC_QDSS_CFG_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_CFG_AHB_CBCR_ADDR,m,v,HWIO_GCC_QDSS_CFG_AHB_CBCR_IN)
#define HWIO_GCC_QDSS_CFG_AHB_CBCR_CLK_OFF_BMSK                                                 0x80000000
#define HWIO_GCC_QDSS_CFG_AHB_CBCR_CLK_OFF_SHFT                                                       0x1f
#define HWIO_GCC_QDSS_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                0x70000000
#define HWIO_GCC_QDSS_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                      0x1c
#define HWIO_GCC_QDSS_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                        0x8000
#define HWIO_GCC_QDSS_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                           0xf
#define HWIO_GCC_QDSS_CFG_AHB_CBCR_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_GCC_QDSS_CFG_AHB_CBCR_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_GCC_QDSS_AT_CMD_RCGR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x0002900c)
#define HWIO_GCC_QDSS_AT_CMD_RCGR_RMSK                                                          0x80000013
#define HWIO_GCC_QDSS_AT_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_AT_CMD_RCGR_ADDR, HWIO_GCC_QDSS_AT_CMD_RCGR_RMSK)
#define HWIO_GCC_QDSS_AT_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_AT_CMD_RCGR_ADDR, m)
#define HWIO_GCC_QDSS_AT_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_AT_CMD_RCGR_ADDR,v)
#define HWIO_GCC_QDSS_AT_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_AT_CMD_RCGR_ADDR,m,v,HWIO_GCC_QDSS_AT_CMD_RCGR_IN)
#define HWIO_GCC_QDSS_AT_CMD_RCGR_ROOT_OFF_BMSK                                                 0x80000000
#define HWIO_GCC_QDSS_AT_CMD_RCGR_ROOT_OFF_SHFT                                                       0x1f
#define HWIO_GCC_QDSS_AT_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                 0x10
#define HWIO_GCC_QDSS_AT_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                  0x4
#define HWIO_GCC_QDSS_AT_CMD_RCGR_ROOT_EN_BMSK                                                         0x2
#define HWIO_GCC_QDSS_AT_CMD_RCGR_ROOT_EN_SHFT                                                         0x1
#define HWIO_GCC_QDSS_AT_CMD_RCGR_UPDATE_BMSK                                                          0x1
#define HWIO_GCC_QDSS_AT_CMD_RCGR_UPDATE_SHFT                                                          0x0

#define HWIO_GCC_QDSS_AT_CFG_RCGR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00029010)
#define HWIO_GCC_QDSS_AT_CFG_RCGR_RMSK                                                               0x71f
#define HWIO_GCC_QDSS_AT_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_AT_CFG_RCGR_ADDR, HWIO_GCC_QDSS_AT_CFG_RCGR_RMSK)
#define HWIO_GCC_QDSS_AT_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_AT_CFG_RCGR_ADDR, m)
#define HWIO_GCC_QDSS_AT_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_AT_CFG_RCGR_ADDR,v)
#define HWIO_GCC_QDSS_AT_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_AT_CFG_RCGR_ADDR,m,v,HWIO_GCC_QDSS_AT_CFG_RCGR_IN)
#define HWIO_GCC_QDSS_AT_CFG_RCGR_SRC_SEL_BMSK                                                       0x700
#define HWIO_GCC_QDSS_AT_CFG_RCGR_SRC_SEL_SHFT                                                         0x8
#define HWIO_GCC_QDSS_AT_CFG_RCGR_SRC_DIV_BMSK                                                        0x1f
#define HWIO_GCC_QDSS_AT_CFG_RCGR_SRC_DIV_SHFT                                                         0x0

#define HWIO_GCC_QDSS_AT_CBCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00029024)
#define HWIO_GCC_QDSS_AT_CBCR_RMSK                                                              0x80007ff1
#define HWIO_GCC_QDSS_AT_CBCR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_AT_CBCR_ADDR, HWIO_GCC_QDSS_AT_CBCR_RMSK)
#define HWIO_GCC_QDSS_AT_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_AT_CBCR_ADDR, m)
#define HWIO_GCC_QDSS_AT_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_AT_CBCR_ADDR,v)
#define HWIO_GCC_QDSS_AT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_AT_CBCR_ADDR,m,v,HWIO_GCC_QDSS_AT_CBCR_IN)
#define HWIO_GCC_QDSS_AT_CBCR_CLK_OFF_BMSK                                                      0x80000000
#define HWIO_GCC_QDSS_AT_CBCR_CLK_OFF_SHFT                                                            0x1f
#define HWIO_GCC_QDSS_AT_CBCR_FORCE_MEM_CORE_ON_BMSK                                                0x4000
#define HWIO_GCC_QDSS_AT_CBCR_FORCE_MEM_CORE_ON_SHFT                                                   0xe
#define HWIO_GCC_QDSS_AT_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                              0x2000
#define HWIO_GCC_QDSS_AT_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                 0xd
#define HWIO_GCC_QDSS_AT_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                             0x1000
#define HWIO_GCC_QDSS_AT_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                                0xc
#define HWIO_GCC_QDSS_AT_CBCR_WAKEUP_BMSK                                                            0xf00
#define HWIO_GCC_QDSS_AT_CBCR_WAKEUP_SHFT                                                              0x8
#define HWIO_GCC_QDSS_AT_CBCR_SLEEP_BMSK                                                              0xf0
#define HWIO_GCC_QDSS_AT_CBCR_SLEEP_SHFT                                                               0x4
#define HWIO_GCC_QDSS_AT_CBCR_CLK_ENABLE_BMSK                                                          0x1
#define HWIO_GCC_QDSS_AT_CBCR_CLK_ENABLE_SHFT                                                          0x0

#define HWIO_GCC_QDSS_ETR_USB_CBCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00029028)
#define HWIO_GCC_QDSS_ETR_USB_CBCR_RMSK                                                         0x80000001
#define HWIO_GCC_QDSS_ETR_USB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_ETR_USB_CBCR_ADDR, HWIO_GCC_QDSS_ETR_USB_CBCR_RMSK)
#define HWIO_GCC_QDSS_ETR_USB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_ETR_USB_CBCR_ADDR, m)
#define HWIO_GCC_QDSS_ETR_USB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_ETR_USB_CBCR_ADDR,v)
#define HWIO_GCC_QDSS_ETR_USB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_ETR_USB_CBCR_ADDR,m,v,HWIO_GCC_QDSS_ETR_USB_CBCR_IN)
#define HWIO_GCC_QDSS_ETR_USB_CBCR_CLK_OFF_BMSK                                                 0x80000000
#define HWIO_GCC_QDSS_ETR_USB_CBCR_CLK_OFF_SHFT                                                       0x1f
#define HWIO_GCC_QDSS_ETR_USB_CBCR_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_GCC_QDSS_ETR_USB_CBCR_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_GCC_QDSS_STM_CMD_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0002902c)
#define HWIO_GCC_QDSS_STM_CMD_RCGR_RMSK                                                         0x80000013
#define HWIO_GCC_QDSS_STM_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_STM_CMD_RCGR_ADDR, HWIO_GCC_QDSS_STM_CMD_RCGR_RMSK)
#define HWIO_GCC_QDSS_STM_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_STM_CMD_RCGR_ADDR, m)
#define HWIO_GCC_QDSS_STM_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_STM_CMD_RCGR_ADDR,v)
#define HWIO_GCC_QDSS_STM_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_STM_CMD_RCGR_ADDR,m,v,HWIO_GCC_QDSS_STM_CMD_RCGR_IN)
#define HWIO_GCC_QDSS_STM_CMD_RCGR_ROOT_OFF_BMSK                                                0x80000000
#define HWIO_GCC_QDSS_STM_CMD_RCGR_ROOT_OFF_SHFT                                                      0x1f
#define HWIO_GCC_QDSS_STM_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                0x10
#define HWIO_GCC_QDSS_STM_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                 0x4
#define HWIO_GCC_QDSS_STM_CMD_RCGR_ROOT_EN_BMSK                                                        0x2
#define HWIO_GCC_QDSS_STM_CMD_RCGR_ROOT_EN_SHFT                                                        0x1
#define HWIO_GCC_QDSS_STM_CMD_RCGR_UPDATE_BMSK                                                         0x1
#define HWIO_GCC_QDSS_STM_CMD_RCGR_UPDATE_SHFT                                                         0x0

#define HWIO_GCC_QDSS_STM_CFG_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00029030)
#define HWIO_GCC_QDSS_STM_CFG_RCGR_RMSK                                                              0x71f
#define HWIO_GCC_QDSS_STM_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_STM_CFG_RCGR_ADDR, HWIO_GCC_QDSS_STM_CFG_RCGR_RMSK)
#define HWIO_GCC_QDSS_STM_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_STM_CFG_RCGR_ADDR, m)
#define HWIO_GCC_QDSS_STM_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_STM_CFG_RCGR_ADDR,v)
#define HWIO_GCC_QDSS_STM_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_STM_CFG_RCGR_ADDR,m,v,HWIO_GCC_QDSS_STM_CFG_RCGR_IN)
#define HWIO_GCC_QDSS_STM_CFG_RCGR_SRC_SEL_BMSK                                                      0x700
#define HWIO_GCC_QDSS_STM_CFG_RCGR_SRC_SEL_SHFT                                                        0x8
#define HWIO_GCC_QDSS_STM_CFG_RCGR_SRC_DIV_BMSK                                                       0x1f
#define HWIO_GCC_QDSS_STM_CFG_RCGR_SRC_DIV_SHFT                                                        0x0

#define HWIO_GCC_QDSS_STM_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00029044)
#define HWIO_GCC_QDSS_STM_CBCR_RMSK                                                             0xf0008001
#define HWIO_GCC_QDSS_STM_CBCR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_STM_CBCR_ADDR, HWIO_GCC_QDSS_STM_CBCR_RMSK)
#define HWIO_GCC_QDSS_STM_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_STM_CBCR_ADDR, m)
#define HWIO_GCC_QDSS_STM_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_STM_CBCR_ADDR,v)
#define HWIO_GCC_QDSS_STM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_STM_CBCR_ADDR,m,v,HWIO_GCC_QDSS_STM_CBCR_IN)
#define HWIO_GCC_QDSS_STM_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_QDSS_STM_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_QDSS_STM_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                    0x70000000
#define HWIO_GCC_QDSS_STM_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                          0x1c
#define HWIO_GCC_QDSS_STM_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                            0x8000
#define HWIO_GCC_QDSS_STM_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                               0xf
#define HWIO_GCC_QDSS_STM_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_GCC_QDSS_STM_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00029048)
#define HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_RMSK                                                  0x80000013
#define HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_ADDR, HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_RMSK)
#define HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_ADDR, m)
#define HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_ADDR,v)
#define HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_ADDR,m,v,HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_IN)
#define HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_ROOT_OFF_BMSK                                         0x80000000
#define HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_ROOT_OFF_SHFT                                               0x1f
#define HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                         0x10
#define HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                          0x4
#define HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_ROOT_EN_BMSK                                                 0x2
#define HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_ROOT_EN_SHFT                                                 0x1
#define HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_UPDATE_BMSK                                                  0x1
#define HWIO_GCC_QDSS_TRACECLKIN_CMD_RCGR_UPDATE_SHFT                                                  0x0

#define HWIO_GCC_QDSS_TRACECLKIN_CFG_RCGR_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0002904c)
#define HWIO_GCC_QDSS_TRACECLKIN_CFG_RCGR_RMSK                                                       0x71f
#define HWIO_GCC_QDSS_TRACECLKIN_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_TRACECLKIN_CFG_RCGR_ADDR, HWIO_GCC_QDSS_TRACECLKIN_CFG_RCGR_RMSK)
#define HWIO_GCC_QDSS_TRACECLKIN_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_TRACECLKIN_CFG_RCGR_ADDR, m)
#define HWIO_GCC_QDSS_TRACECLKIN_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_TRACECLKIN_CFG_RCGR_ADDR,v)
#define HWIO_GCC_QDSS_TRACECLKIN_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_TRACECLKIN_CFG_RCGR_ADDR,m,v,HWIO_GCC_QDSS_TRACECLKIN_CFG_RCGR_IN)
#define HWIO_GCC_QDSS_TRACECLKIN_CFG_RCGR_SRC_SEL_BMSK                                               0x700
#define HWIO_GCC_QDSS_TRACECLKIN_CFG_RCGR_SRC_SEL_SHFT                                                 0x8
#define HWIO_GCC_QDSS_TRACECLKIN_CFG_RCGR_SRC_DIV_BMSK                                                0x1f
#define HWIO_GCC_QDSS_TRACECLKIN_CFG_RCGR_SRC_DIV_SHFT                                                 0x0

#define HWIO_GCC_QDSS_TRACECLKIN_CBCR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00029060)
#define HWIO_GCC_QDSS_TRACECLKIN_CBCR_RMSK                                                      0x80000001
#define HWIO_GCC_QDSS_TRACECLKIN_CBCR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_TRACECLKIN_CBCR_ADDR, HWIO_GCC_QDSS_TRACECLKIN_CBCR_RMSK)
#define HWIO_GCC_QDSS_TRACECLKIN_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_TRACECLKIN_CBCR_ADDR, m)
#define HWIO_GCC_QDSS_TRACECLKIN_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_TRACECLKIN_CBCR_ADDR,v)
#define HWIO_GCC_QDSS_TRACECLKIN_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_TRACECLKIN_CBCR_ADDR,m,v,HWIO_GCC_QDSS_TRACECLKIN_CBCR_IN)
#define HWIO_GCC_QDSS_TRACECLKIN_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_GCC_QDSS_TRACECLKIN_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_GCC_QDSS_TRACECLKIN_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_GCC_QDSS_TRACECLKIN_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_GCC_QDSS_TSCTR_CMD_RCGR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x00029064)
#define HWIO_GCC_QDSS_TSCTR_CMD_RCGR_RMSK                                                       0x80000013
#define HWIO_GCC_QDSS_TSCTR_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_TSCTR_CMD_RCGR_ADDR, HWIO_GCC_QDSS_TSCTR_CMD_RCGR_RMSK)
#define HWIO_GCC_QDSS_TSCTR_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_TSCTR_CMD_RCGR_ADDR, m)
#define HWIO_GCC_QDSS_TSCTR_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_TSCTR_CMD_RCGR_ADDR,v)
#define HWIO_GCC_QDSS_TSCTR_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_TSCTR_CMD_RCGR_ADDR,m,v,HWIO_GCC_QDSS_TSCTR_CMD_RCGR_IN)
#define HWIO_GCC_QDSS_TSCTR_CMD_RCGR_ROOT_OFF_BMSK                                              0x80000000
#define HWIO_GCC_QDSS_TSCTR_CMD_RCGR_ROOT_OFF_SHFT                                                    0x1f
#define HWIO_GCC_QDSS_TSCTR_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                              0x10
#define HWIO_GCC_QDSS_TSCTR_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                               0x4
#define HWIO_GCC_QDSS_TSCTR_CMD_RCGR_ROOT_EN_BMSK                                                      0x2
#define HWIO_GCC_QDSS_TSCTR_CMD_RCGR_ROOT_EN_SHFT                                                      0x1
#define HWIO_GCC_QDSS_TSCTR_CMD_RCGR_UPDATE_BMSK                                                       0x1
#define HWIO_GCC_QDSS_TSCTR_CMD_RCGR_UPDATE_SHFT                                                       0x0

#define HWIO_GCC_QDSS_TSCTR_CFG_RCGR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x00029068)
#define HWIO_GCC_QDSS_TSCTR_CFG_RCGR_RMSK                                                            0x71f
#define HWIO_GCC_QDSS_TSCTR_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_TSCTR_CFG_RCGR_ADDR, HWIO_GCC_QDSS_TSCTR_CFG_RCGR_RMSK)
#define HWIO_GCC_QDSS_TSCTR_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_TSCTR_CFG_RCGR_ADDR, m)
#define HWIO_GCC_QDSS_TSCTR_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_TSCTR_CFG_RCGR_ADDR,v)
#define HWIO_GCC_QDSS_TSCTR_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_TSCTR_CFG_RCGR_ADDR,m,v,HWIO_GCC_QDSS_TSCTR_CFG_RCGR_IN)
#define HWIO_GCC_QDSS_TSCTR_CFG_RCGR_SRC_SEL_BMSK                                                    0x700
#define HWIO_GCC_QDSS_TSCTR_CFG_RCGR_SRC_SEL_SHFT                                                      0x8
#define HWIO_GCC_QDSS_TSCTR_CFG_RCGR_SRC_DIV_BMSK                                                     0x1f
#define HWIO_GCC_QDSS_TSCTR_CFG_RCGR_SRC_DIV_SHFT                                                      0x0

#define HWIO_GCC_QDSS_TSCTR_DIV2_CBCR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x0002907c)
#define HWIO_GCC_QDSS_TSCTR_DIV2_CBCR_RMSK                                                      0x80000001
#define HWIO_GCC_QDSS_TSCTR_DIV2_CBCR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_TSCTR_DIV2_CBCR_ADDR, HWIO_GCC_QDSS_TSCTR_DIV2_CBCR_RMSK)
#define HWIO_GCC_QDSS_TSCTR_DIV2_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_TSCTR_DIV2_CBCR_ADDR, m)
#define HWIO_GCC_QDSS_TSCTR_DIV2_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_TSCTR_DIV2_CBCR_ADDR,v)
#define HWIO_GCC_QDSS_TSCTR_DIV2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_TSCTR_DIV2_CBCR_ADDR,m,v,HWIO_GCC_QDSS_TSCTR_DIV2_CBCR_IN)
#define HWIO_GCC_QDSS_TSCTR_DIV2_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_GCC_QDSS_TSCTR_DIV2_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_GCC_QDSS_TSCTR_DIV2_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_GCC_QDSS_TSCTR_DIV2_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_GCC_QDSS_TSCTR_DIV3_CBCR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00029080)
#define HWIO_GCC_QDSS_TSCTR_DIV3_CBCR_RMSK                                                      0x80000001
#define HWIO_GCC_QDSS_TSCTR_DIV3_CBCR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_TSCTR_DIV3_CBCR_ADDR, HWIO_GCC_QDSS_TSCTR_DIV3_CBCR_RMSK)
#define HWIO_GCC_QDSS_TSCTR_DIV3_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_TSCTR_DIV3_CBCR_ADDR, m)
#define HWIO_GCC_QDSS_TSCTR_DIV3_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_TSCTR_DIV3_CBCR_ADDR,v)
#define HWIO_GCC_QDSS_TSCTR_DIV3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_TSCTR_DIV3_CBCR_ADDR,m,v,HWIO_GCC_QDSS_TSCTR_DIV3_CBCR_IN)
#define HWIO_GCC_QDSS_TSCTR_DIV3_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_GCC_QDSS_TSCTR_DIV3_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_GCC_QDSS_TSCTR_DIV3_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_GCC_QDSS_TSCTR_DIV3_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_GCC_QDSS_DAP_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00029084)
#define HWIO_GCC_QDSS_DAP_CBCR_RMSK                                                             0x80000001
#define HWIO_GCC_QDSS_DAP_CBCR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_DAP_CBCR_ADDR, HWIO_GCC_QDSS_DAP_CBCR_RMSK)
#define HWIO_GCC_QDSS_DAP_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_DAP_CBCR_ADDR, m)
#define HWIO_GCC_QDSS_DAP_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_DAP_CBCR_ADDR,v)
#define HWIO_GCC_QDSS_DAP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_DAP_CBCR_ADDR,m,v,HWIO_GCC_QDSS_DAP_CBCR_IN)
#define HWIO_GCC_QDSS_DAP_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_QDSS_DAP_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_QDSS_DAP_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_GCC_QDSS_DAP_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_GCC_QDSS_TSCTR_DIV4_CBCR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00029088)
#define HWIO_GCC_QDSS_TSCTR_DIV4_CBCR_RMSK                                                      0x80000001
#define HWIO_GCC_QDSS_TSCTR_DIV4_CBCR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_TSCTR_DIV4_CBCR_ADDR, HWIO_GCC_QDSS_TSCTR_DIV4_CBCR_RMSK)
#define HWIO_GCC_QDSS_TSCTR_DIV4_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_TSCTR_DIV4_CBCR_ADDR, m)
#define HWIO_GCC_QDSS_TSCTR_DIV4_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_TSCTR_DIV4_CBCR_ADDR,v)
#define HWIO_GCC_QDSS_TSCTR_DIV4_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_TSCTR_DIV4_CBCR_ADDR,m,v,HWIO_GCC_QDSS_TSCTR_DIV4_CBCR_IN)
#define HWIO_GCC_QDSS_TSCTR_DIV4_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_GCC_QDSS_TSCTR_DIV4_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_GCC_QDSS_TSCTR_DIV4_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_GCC_QDSS_TSCTR_DIV4_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_GCC_QDSS_TSCTR_DIV8_CBCR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x0002908c)
#define HWIO_GCC_QDSS_TSCTR_DIV8_CBCR_RMSK                                                      0x80000001
#define HWIO_GCC_QDSS_TSCTR_DIV8_CBCR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_TSCTR_DIV8_CBCR_ADDR, HWIO_GCC_QDSS_TSCTR_DIV8_CBCR_RMSK)
#define HWIO_GCC_QDSS_TSCTR_DIV8_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_TSCTR_DIV8_CBCR_ADDR, m)
#define HWIO_GCC_QDSS_TSCTR_DIV8_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_TSCTR_DIV8_CBCR_ADDR,v)
#define HWIO_GCC_QDSS_TSCTR_DIV8_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_TSCTR_DIV8_CBCR_ADDR,m,v,HWIO_GCC_QDSS_TSCTR_DIV8_CBCR_IN)
#define HWIO_GCC_QDSS_TSCTR_DIV8_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_GCC_QDSS_TSCTR_DIV8_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_GCC_QDSS_TSCTR_DIV8_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_GCC_QDSS_TSCTR_DIV8_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_GCC_QDSS_TSCTR_DIV16_CBCR_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00029090)
#define HWIO_GCC_QDSS_TSCTR_DIV16_CBCR_RMSK                                                     0x80000001
#define HWIO_GCC_QDSS_TSCTR_DIV16_CBCR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_TSCTR_DIV16_CBCR_ADDR, HWIO_GCC_QDSS_TSCTR_DIV16_CBCR_RMSK)
#define HWIO_GCC_QDSS_TSCTR_DIV16_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_TSCTR_DIV16_CBCR_ADDR, m)
#define HWIO_GCC_QDSS_TSCTR_DIV16_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_TSCTR_DIV16_CBCR_ADDR,v)
#define HWIO_GCC_QDSS_TSCTR_DIV16_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_TSCTR_DIV16_CBCR_ADDR,m,v,HWIO_GCC_QDSS_TSCTR_DIV16_CBCR_IN)
#define HWIO_GCC_QDSS_TSCTR_DIV16_CBCR_CLK_OFF_BMSK                                             0x80000000
#define HWIO_GCC_QDSS_TSCTR_DIV16_CBCR_CLK_OFF_SHFT                                                   0x1f
#define HWIO_GCC_QDSS_TSCTR_DIV16_CBCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_GCC_QDSS_TSCTR_DIV16_CBCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_GCC_GCC_SLEEP_CMD_RCGR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00030000)
#define HWIO_GCC_GCC_SLEEP_CMD_RCGR_RMSK                                                        0x80000002
#define HWIO_GCC_GCC_SLEEP_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_GCC_SLEEP_CMD_RCGR_ADDR, HWIO_GCC_GCC_SLEEP_CMD_RCGR_RMSK)
#define HWIO_GCC_GCC_SLEEP_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_GCC_SLEEP_CMD_RCGR_ADDR, m)
#define HWIO_GCC_GCC_SLEEP_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_GCC_SLEEP_CMD_RCGR_ADDR,v)
#define HWIO_GCC_GCC_SLEEP_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GCC_SLEEP_CMD_RCGR_ADDR,m,v,HWIO_GCC_GCC_SLEEP_CMD_RCGR_IN)
#define HWIO_GCC_GCC_SLEEP_CMD_RCGR_ROOT_OFF_BMSK                                               0x80000000
#define HWIO_GCC_GCC_SLEEP_CMD_RCGR_ROOT_OFF_SHFT                                                     0x1f
#define HWIO_GCC_GCC_SLEEP_CMD_RCGR_ROOT_EN_BMSK                                                       0x2
#define HWIO_GCC_GCC_SLEEP_CMD_RCGR_ROOT_EN_SHFT                                                       0x1

#define HWIO_GCC_USB_HS_BCR_ADDR                                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00041000)
#define HWIO_GCC_USB_HS_BCR_RMSK                                                                       0x1
#define HWIO_GCC_USB_HS_BCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_BCR_ADDR, HWIO_GCC_USB_HS_BCR_RMSK)
#define HWIO_GCC_USB_HS_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_BCR_ADDR, m)
#define HWIO_GCC_USB_HS_BCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_BCR_ADDR,v)
#define HWIO_GCC_USB_HS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_BCR_ADDR,m,v,HWIO_GCC_USB_HS_BCR_IN)
#define HWIO_GCC_USB_HS_BCR_BLK_ARES_BMSK                                                              0x1
#define HWIO_GCC_USB_HS_BCR_BLK_ARES_SHFT                                                              0x0

#define HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00041004)
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_RMSK                                                        0x80007ff1
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR, HWIO_GCC_USB_HS_SYSTEM_CBCR_RMSK)
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR, m)
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR,v)
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR,m,v,HWIO_GCC_USB_HS_SYSTEM_CBCR_IN)
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_FORCE_MEM_CORE_ON_BMSK                                          0x4000
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_FORCE_MEM_CORE_ON_SHFT                                             0xe
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                        0x2000
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                           0xd
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                       0x1000
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                          0xc
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_WAKEUP_BMSK                                                      0xf00
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_WAKEUP_SHFT                                                        0x8
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_SLEEP_BMSK                                                        0xf0
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_SLEEP_SHFT                                                         0x4
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_USB_HS_AHB_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00041008)
#define HWIO_GCC_USB_HS_AHB_CBCR_RMSK                                                           0xf000fff1
#define HWIO_GCC_USB_HS_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_AHB_CBCR_ADDR, HWIO_GCC_USB_HS_AHB_CBCR_RMSK)
#define HWIO_GCC_USB_HS_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_AHB_CBCR_ADDR, m)
#define HWIO_GCC_USB_HS_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_AHB_CBCR_ADDR,v)
#define HWIO_GCC_USB_HS_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_AHB_CBCR_ADDR,m,v,HWIO_GCC_USB_HS_AHB_CBCR_IN)
#define HWIO_GCC_USB_HS_AHB_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_USB_HS_AHB_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_USB_HS_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                  0x70000000
#define HWIO_GCC_USB_HS_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                        0x1c
#define HWIO_GCC_USB_HS_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                          0x8000
#define HWIO_GCC_USB_HS_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                             0xf
#define HWIO_GCC_USB_HS_AHB_CBCR_FORCE_MEM_CORE_ON_BMSK                                             0x4000
#define HWIO_GCC_USB_HS_AHB_CBCR_FORCE_MEM_CORE_ON_SHFT                                                0xe
#define HWIO_GCC_USB_HS_AHB_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                           0x2000
#define HWIO_GCC_USB_HS_AHB_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                              0xd
#define HWIO_GCC_USB_HS_AHB_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                          0x1000
#define HWIO_GCC_USB_HS_AHB_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                             0xc
#define HWIO_GCC_USB_HS_AHB_CBCR_WAKEUP_BMSK                                                         0xf00
#define HWIO_GCC_USB_HS_AHB_CBCR_WAKEUP_SHFT                                                           0x8
#define HWIO_GCC_USB_HS_AHB_CBCR_SLEEP_BMSK                                                           0xf0
#define HWIO_GCC_USB_HS_AHB_CBCR_SLEEP_SHFT                                                            0x4
#define HWIO_GCC_USB_HS_AHB_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_USB_HS_AHB_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_ADDR                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0004100c)
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_RMSK                                             0x80000001
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_ADDR, HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_RMSK)
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_ADDR, m)
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_ADDR,v)
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_ADDR,m,v,HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_IN)
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_CLK_OFF_BMSK                                     0x80000000
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_CLK_OFF_SHFT                                           0x1f
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_CLK_ENABLE_BMSK                                         0x1
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_CLK_ENABLE_SHFT                                         0x0

#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00041010)
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_RMSK                                                    0x80000013
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ADDR, HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_RMSK)
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ADDR, m)
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ADDR,v)
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ADDR,m,v,HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_IN)
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ROOT_OFF_BMSK                                           0x80000000
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ROOT_OFF_SHFT                                                 0x1f
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                           0x10
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                            0x4
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ROOT_EN_BMSK                                                   0x2
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ROOT_EN_SHFT                                                   0x1
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_UPDATE_BMSK                                                    0x1
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_UPDATE_SHFT                                                    0x0

#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00041014)
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_RMSK                                                         0x71f
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_ADDR, HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_RMSK)
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_ADDR, m)
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_ADDR,v)
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_ADDR,m,v,HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_IN)
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_SRC_SEL_BMSK                                                 0x700
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_SRC_SEL_SHFT                                                   0x8
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_SRC_DIV_BMSK                                                  0x1f
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_SRC_DIV_SHFT                                                   0x0

#define HWIO_GCC_USB2A_PHY_BCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00041028)
#define HWIO_GCC_USB2A_PHY_BCR_RMSK                                                                    0x1
#define HWIO_GCC_USB2A_PHY_BCR_IN          \
        in_dword_masked(HWIO_GCC_USB2A_PHY_BCR_ADDR, HWIO_GCC_USB2A_PHY_BCR_RMSK)
#define HWIO_GCC_USB2A_PHY_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB2A_PHY_BCR_ADDR, m)
#define HWIO_GCC_USB2A_PHY_BCR_OUT(v)      \
        out_dword(HWIO_GCC_USB2A_PHY_BCR_ADDR,v)
#define HWIO_GCC_USB2A_PHY_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB2A_PHY_BCR_ADDR,m,v,HWIO_GCC_USB2A_PHY_BCR_IN)
#define HWIO_GCC_USB2A_PHY_BCR_BLK_ARES_BMSK                                                           0x1
#define HWIO_GCC_USB2A_PHY_BCR_BLK_ARES_SHFT                                                           0x0

#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x0004102c)
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_RMSK                                                      0x80000001
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB2A_PHY_SLEEP_CBCR_ADDR, HWIO_GCC_USB2A_PHY_SLEEP_CBCR_RMSK)
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB2A_PHY_SLEEP_CBCR_ADDR, m)
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB2A_PHY_SLEEP_CBCR_ADDR,v)
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB2A_PHY_SLEEP_CBCR_ADDR,m,v,HWIO_GCC_USB2A_PHY_SLEEP_CBCR_IN)
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_GCC_SDCC1_BCR_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00042000)
#define HWIO_GCC_SDCC1_BCR_RMSK                                                                        0x1
#define HWIO_GCC_SDCC1_BCR_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_BCR_ADDR, HWIO_GCC_SDCC1_BCR_RMSK)
#define HWIO_GCC_SDCC1_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_BCR_ADDR, m)
#define HWIO_GCC_SDCC1_BCR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_BCR_ADDR,v)
#define HWIO_GCC_SDCC1_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_BCR_ADDR,m,v,HWIO_GCC_SDCC1_BCR_IN)
#define HWIO_GCC_SDCC1_BCR_BLK_ARES_BMSK                                                               0x1
#define HWIO_GCC_SDCC1_BCR_BLK_ARES_SHFT                                                               0x0

#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x00042004)
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_RMSK                                                       0x800000f3
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_CMD_RCGR_ADDR, HWIO_GCC_SDCC1_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_SDCC1_APPS_CMD_RCGR_IN)
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_ROOT_OFF_BMSK                                              0x80000000
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_ROOT_OFF_SHFT                                                    0x1f
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_DIRTY_D_BMSK                                                     0x80
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_DIRTY_D_SHFT                                                      0x7
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_DIRTY_M_BMSK                                                     0x40
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_DIRTY_M_SHFT                                                      0x6
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_DIRTY_N_BMSK                                                     0x20
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_DIRTY_N_SHFT                                                      0x5
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                              0x10
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                               0x4
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_ROOT_EN_BMSK                                                      0x2
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_ROOT_EN_SHFT                                                      0x1
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_UPDATE_BMSK                                                       0x1
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_UPDATE_SHFT                                                       0x0

#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x00042008)
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_RMSK                                                           0x371f
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_CFG_RCGR_ADDR, HWIO_GCC_SDCC1_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_SDCC1_APPS_CFG_RCGR_IN)
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_MODE_BMSK                                                      0x3000
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_MODE_SHFT                                                         0xc
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_SRC_SEL_BMSK                                                    0x700
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_SRC_SEL_SHFT                                                      0x8
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_SRC_DIV_BMSK                                                     0x1f
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_SRC_DIV_SHFT                                                      0x0

#define HWIO_GCC_SDCC1_APPS_M_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0004200c)
#define HWIO_GCC_SDCC1_APPS_M_RMSK                                                                    0xff
#define HWIO_GCC_SDCC1_APPS_M_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_M_ADDR, HWIO_GCC_SDCC1_APPS_M_RMSK)
#define HWIO_GCC_SDCC1_APPS_M_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_M_ADDR, m)
#define HWIO_GCC_SDCC1_APPS_M_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_APPS_M_ADDR,v)
#define HWIO_GCC_SDCC1_APPS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_APPS_M_ADDR,m,v,HWIO_GCC_SDCC1_APPS_M_IN)
#define HWIO_GCC_SDCC1_APPS_M_M_BMSK                                                                  0xff
#define HWIO_GCC_SDCC1_APPS_M_M_SHFT                                                                   0x0

#define HWIO_GCC_SDCC1_APPS_N_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00042010)
#define HWIO_GCC_SDCC1_APPS_N_RMSK                                                                    0xff
#define HWIO_GCC_SDCC1_APPS_N_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_N_ADDR, HWIO_GCC_SDCC1_APPS_N_RMSK)
#define HWIO_GCC_SDCC1_APPS_N_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_N_ADDR, m)
#define HWIO_GCC_SDCC1_APPS_N_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_APPS_N_ADDR,v)
#define HWIO_GCC_SDCC1_APPS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_APPS_N_ADDR,m,v,HWIO_GCC_SDCC1_APPS_N_IN)
#define HWIO_GCC_SDCC1_APPS_N_NOT_N_MINUS_M_BMSK                                                      0xff
#define HWIO_GCC_SDCC1_APPS_N_NOT_N_MINUS_M_SHFT                                                       0x0

#define HWIO_GCC_SDCC1_APPS_D_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00042014)
#define HWIO_GCC_SDCC1_APPS_D_RMSK                                                                    0xff
#define HWIO_GCC_SDCC1_APPS_D_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_D_ADDR, HWIO_GCC_SDCC1_APPS_D_RMSK)
#define HWIO_GCC_SDCC1_APPS_D_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_D_ADDR, m)
#define HWIO_GCC_SDCC1_APPS_D_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_APPS_D_ADDR,v)
#define HWIO_GCC_SDCC1_APPS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_APPS_D_ADDR,m,v,HWIO_GCC_SDCC1_APPS_D_IN)
#define HWIO_GCC_SDCC1_APPS_D_NOT_2D_BMSK                                                             0xff
#define HWIO_GCC_SDCC1_APPS_D_NOT_2D_SHFT                                                              0x0

#define HWIO_GCC_SDCC1_APPS_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00042018)
#define HWIO_GCC_SDCC1_APPS_CBCR_RMSK                                                           0x80007ff1
#define HWIO_GCC_SDCC1_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_CBCR_ADDR, HWIO_GCC_SDCC1_APPS_CBCR_RMSK)
#define HWIO_GCC_SDCC1_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_CBCR_ADDR, m)
#define HWIO_GCC_SDCC1_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_APPS_CBCR_ADDR,v)
#define HWIO_GCC_SDCC1_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_APPS_CBCR_ADDR,m,v,HWIO_GCC_SDCC1_APPS_CBCR_IN)
#define HWIO_GCC_SDCC1_APPS_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_SDCC1_APPS_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_SDCC1_APPS_CBCR_FORCE_MEM_CORE_ON_BMSK                                             0x4000
#define HWIO_GCC_SDCC1_APPS_CBCR_FORCE_MEM_CORE_ON_SHFT                                                0xe
#define HWIO_GCC_SDCC1_APPS_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                           0x2000
#define HWIO_GCC_SDCC1_APPS_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                              0xd
#define HWIO_GCC_SDCC1_APPS_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                          0x1000
#define HWIO_GCC_SDCC1_APPS_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                             0xc
#define HWIO_GCC_SDCC1_APPS_CBCR_WAKEUP_BMSK                                                         0xf00
#define HWIO_GCC_SDCC1_APPS_CBCR_WAKEUP_SHFT                                                           0x8
#define HWIO_GCC_SDCC1_APPS_CBCR_SLEEP_BMSK                                                           0xf0
#define HWIO_GCC_SDCC1_APPS_CBCR_SLEEP_SHFT                                                            0x4
#define HWIO_GCC_SDCC1_APPS_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_SDCC1_APPS_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_SDCC1_AHB_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0004201c)
#define HWIO_GCC_SDCC1_AHB_CBCR_RMSK                                                            0xf000fff1
#define HWIO_GCC_SDCC1_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_AHB_CBCR_ADDR, HWIO_GCC_SDCC1_AHB_CBCR_RMSK)
#define HWIO_GCC_SDCC1_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_AHB_CBCR_ADDR, m)
#define HWIO_GCC_SDCC1_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_AHB_CBCR_ADDR,v)
#define HWIO_GCC_SDCC1_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_AHB_CBCR_ADDR,m,v,HWIO_GCC_SDCC1_AHB_CBCR_IN)
#define HWIO_GCC_SDCC1_AHB_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_SDCC1_AHB_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_SDCC1_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                   0x70000000
#define HWIO_GCC_SDCC1_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                         0x1c
#define HWIO_GCC_SDCC1_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                           0x8000
#define HWIO_GCC_SDCC1_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                              0xf
#define HWIO_GCC_SDCC1_AHB_CBCR_FORCE_MEM_CORE_ON_BMSK                                              0x4000
#define HWIO_GCC_SDCC1_AHB_CBCR_FORCE_MEM_CORE_ON_SHFT                                                 0xe
#define HWIO_GCC_SDCC1_AHB_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                            0x2000
#define HWIO_GCC_SDCC1_AHB_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                               0xd
#define HWIO_GCC_SDCC1_AHB_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                           0x1000
#define HWIO_GCC_SDCC1_AHB_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                              0xc
#define HWIO_GCC_SDCC1_AHB_CBCR_WAKEUP_BMSK                                                          0xf00
#define HWIO_GCC_SDCC1_AHB_CBCR_WAKEUP_SHFT                                                            0x8
#define HWIO_GCC_SDCC1_AHB_CBCR_SLEEP_BMSK                                                            0xf0
#define HWIO_GCC_SDCC1_AHB_CBCR_SLEEP_SHFT                                                             0x4
#define HWIO_GCC_SDCC1_AHB_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_SDCC1_AHB_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_SDCC1_MISC_ADDR                                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00042020)
#define HWIO_GCC_SDCC1_MISC_RMSK                                                                       0x1
#define HWIO_GCC_SDCC1_MISC_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_MISC_ADDR, HWIO_GCC_SDCC1_MISC_RMSK)
#define HWIO_GCC_SDCC1_MISC_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_MISC_ADDR, m)
#define HWIO_GCC_SDCC1_MISC_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_MISC_ADDR,v)
#define HWIO_GCC_SDCC1_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_MISC_ADDR,m,v,HWIO_GCC_SDCC1_MISC_IN)
#define HWIO_GCC_SDCC1_MISC_CDC_ARES_BMSK                                                              0x1
#define HWIO_GCC_SDCC1_MISC_CDC_ARES_SHFT                                                              0x0

#define HWIO_GCC_SDCC2_BCR_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00043000)
#define HWIO_GCC_SDCC2_BCR_RMSK                                                                        0x1
#define HWIO_GCC_SDCC2_BCR_IN          \
        in_dword_masked(HWIO_GCC_SDCC2_BCR_ADDR, HWIO_GCC_SDCC2_BCR_RMSK)
#define HWIO_GCC_SDCC2_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC2_BCR_ADDR, m)
#define HWIO_GCC_SDCC2_BCR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC2_BCR_ADDR,v)
#define HWIO_GCC_SDCC2_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC2_BCR_ADDR,m,v,HWIO_GCC_SDCC2_BCR_IN)
#define HWIO_GCC_SDCC2_BCR_BLK_ARES_BMSK                                                               0x1
#define HWIO_GCC_SDCC2_BCR_BLK_ARES_SHFT                                                               0x0

#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x00043004)
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_RMSK                                                       0x800000f3
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_CMD_RCGR_ADDR, HWIO_GCC_SDCC2_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC2_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC2_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_SDCC2_APPS_CMD_RCGR_IN)
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_ROOT_OFF_BMSK                                              0x80000000
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_ROOT_OFF_SHFT                                                    0x1f
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_DIRTY_D_BMSK                                                     0x80
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_DIRTY_D_SHFT                                                      0x7
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_DIRTY_M_BMSK                                                     0x40
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_DIRTY_M_SHFT                                                      0x6
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_DIRTY_N_BMSK                                                     0x20
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_DIRTY_N_SHFT                                                      0x5
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                              0x10
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                               0x4
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_ROOT_EN_BMSK                                                      0x2
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_ROOT_EN_SHFT                                                      0x1
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_UPDATE_BMSK                                                       0x1
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_UPDATE_SHFT                                                       0x0

#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x00043008)
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_RMSK                                                           0x371f
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_CFG_RCGR_ADDR, HWIO_GCC_SDCC2_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC2_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC2_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_SDCC2_APPS_CFG_RCGR_IN)
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_MODE_BMSK                                                      0x3000
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_MODE_SHFT                                                         0xc
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_SRC_SEL_BMSK                                                    0x700
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_SRC_SEL_SHFT                                                      0x8
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_SRC_DIV_BMSK                                                     0x1f
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_SRC_DIV_SHFT                                                      0x0

#define HWIO_GCC_SDCC2_APPS_M_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0004300c)
#define HWIO_GCC_SDCC2_APPS_M_RMSK                                                                    0xff
#define HWIO_GCC_SDCC2_APPS_M_IN          \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_M_ADDR, HWIO_GCC_SDCC2_APPS_M_RMSK)
#define HWIO_GCC_SDCC2_APPS_M_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_M_ADDR, m)
#define HWIO_GCC_SDCC2_APPS_M_OUT(v)      \
        out_dword(HWIO_GCC_SDCC2_APPS_M_ADDR,v)
#define HWIO_GCC_SDCC2_APPS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC2_APPS_M_ADDR,m,v,HWIO_GCC_SDCC2_APPS_M_IN)
#define HWIO_GCC_SDCC2_APPS_M_M_BMSK                                                                  0xff
#define HWIO_GCC_SDCC2_APPS_M_M_SHFT                                                                   0x0

#define HWIO_GCC_SDCC2_APPS_N_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00043010)
#define HWIO_GCC_SDCC2_APPS_N_RMSK                                                                    0xff
#define HWIO_GCC_SDCC2_APPS_N_IN          \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_N_ADDR, HWIO_GCC_SDCC2_APPS_N_RMSK)
#define HWIO_GCC_SDCC2_APPS_N_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_N_ADDR, m)
#define HWIO_GCC_SDCC2_APPS_N_OUT(v)      \
        out_dword(HWIO_GCC_SDCC2_APPS_N_ADDR,v)
#define HWIO_GCC_SDCC2_APPS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC2_APPS_N_ADDR,m,v,HWIO_GCC_SDCC2_APPS_N_IN)
#define HWIO_GCC_SDCC2_APPS_N_NOT_N_MINUS_M_BMSK                                                      0xff
#define HWIO_GCC_SDCC2_APPS_N_NOT_N_MINUS_M_SHFT                                                       0x0

#define HWIO_GCC_SDCC2_APPS_D_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00043014)
#define HWIO_GCC_SDCC2_APPS_D_RMSK                                                                    0xff
#define HWIO_GCC_SDCC2_APPS_D_IN          \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_D_ADDR, HWIO_GCC_SDCC2_APPS_D_RMSK)
#define HWIO_GCC_SDCC2_APPS_D_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_D_ADDR, m)
#define HWIO_GCC_SDCC2_APPS_D_OUT(v)      \
        out_dword(HWIO_GCC_SDCC2_APPS_D_ADDR,v)
#define HWIO_GCC_SDCC2_APPS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC2_APPS_D_ADDR,m,v,HWIO_GCC_SDCC2_APPS_D_IN)
#define HWIO_GCC_SDCC2_APPS_D_NOT_2D_BMSK                                                             0xff
#define HWIO_GCC_SDCC2_APPS_D_NOT_2D_SHFT                                                              0x0

#define HWIO_GCC_SDCC2_APPS_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00043018)
#define HWIO_GCC_SDCC2_APPS_CBCR_RMSK                                                           0x80007ff1
#define HWIO_GCC_SDCC2_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_CBCR_ADDR, HWIO_GCC_SDCC2_APPS_CBCR_RMSK)
#define HWIO_GCC_SDCC2_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_CBCR_ADDR, m)
#define HWIO_GCC_SDCC2_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC2_APPS_CBCR_ADDR,v)
#define HWIO_GCC_SDCC2_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC2_APPS_CBCR_ADDR,m,v,HWIO_GCC_SDCC2_APPS_CBCR_IN)
#define HWIO_GCC_SDCC2_APPS_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_SDCC2_APPS_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_SDCC2_APPS_CBCR_FORCE_MEM_CORE_ON_BMSK                                             0x4000
#define HWIO_GCC_SDCC2_APPS_CBCR_FORCE_MEM_CORE_ON_SHFT                                                0xe
#define HWIO_GCC_SDCC2_APPS_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                           0x2000
#define HWIO_GCC_SDCC2_APPS_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                              0xd
#define HWIO_GCC_SDCC2_APPS_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                          0x1000
#define HWIO_GCC_SDCC2_APPS_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                             0xc
#define HWIO_GCC_SDCC2_APPS_CBCR_WAKEUP_BMSK                                                         0xf00
#define HWIO_GCC_SDCC2_APPS_CBCR_WAKEUP_SHFT                                                           0x8
#define HWIO_GCC_SDCC2_APPS_CBCR_SLEEP_BMSK                                                           0xf0
#define HWIO_GCC_SDCC2_APPS_CBCR_SLEEP_SHFT                                                            0x4
#define HWIO_GCC_SDCC2_APPS_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_SDCC2_APPS_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_SDCC2_AHB_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0004301c)
#define HWIO_GCC_SDCC2_AHB_CBCR_RMSK                                                            0xf000fff1
#define HWIO_GCC_SDCC2_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SDCC2_AHB_CBCR_ADDR, HWIO_GCC_SDCC2_AHB_CBCR_RMSK)
#define HWIO_GCC_SDCC2_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC2_AHB_CBCR_ADDR, m)
#define HWIO_GCC_SDCC2_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC2_AHB_CBCR_ADDR,v)
#define HWIO_GCC_SDCC2_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC2_AHB_CBCR_ADDR,m,v,HWIO_GCC_SDCC2_AHB_CBCR_IN)
#define HWIO_GCC_SDCC2_AHB_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_SDCC2_AHB_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_SDCC2_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                   0x70000000
#define HWIO_GCC_SDCC2_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                         0x1c
#define HWIO_GCC_SDCC2_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                           0x8000
#define HWIO_GCC_SDCC2_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                              0xf
#define HWIO_GCC_SDCC2_AHB_CBCR_FORCE_MEM_CORE_ON_BMSK                                              0x4000
#define HWIO_GCC_SDCC2_AHB_CBCR_FORCE_MEM_CORE_ON_SHFT                                                 0xe
#define HWIO_GCC_SDCC2_AHB_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                            0x2000
#define HWIO_GCC_SDCC2_AHB_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                               0xd
#define HWIO_GCC_SDCC2_AHB_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                           0x1000
#define HWIO_GCC_SDCC2_AHB_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                              0xc
#define HWIO_GCC_SDCC2_AHB_CBCR_WAKEUP_BMSK                                                          0xf00
#define HWIO_GCC_SDCC2_AHB_CBCR_WAKEUP_SHFT                                                            0x8
#define HWIO_GCC_SDCC2_AHB_CBCR_SLEEP_BMSK                                                            0xf0
#define HWIO_GCC_SDCC2_AHB_CBCR_SLEEP_SHFT                                                             0x4
#define HWIO_GCC_SDCC2_AHB_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_SDCC2_AHB_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_BLSP1_BCR_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00001000)
#define HWIO_GCC_BLSP1_BCR_RMSK                                                                        0x1
#define HWIO_GCC_BLSP1_BCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_BCR_ADDR, HWIO_GCC_BLSP1_BCR_RMSK)
#define HWIO_GCC_BLSP1_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_BCR_ADDR, m)
#define HWIO_GCC_BLSP1_BCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_BCR_ADDR,v)
#define HWIO_GCC_BLSP1_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_BCR_ADDR,m,v,HWIO_GCC_BLSP1_BCR_IN)
#define HWIO_GCC_BLSP1_BCR_BLK_ARES_BMSK                                                               0x1
#define HWIO_GCC_BLSP1_BCR_BLK_ARES_SHFT                                                               0x0

#define HWIO_GCC_BLSP1_SLEEP_CBCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00001004)
#define HWIO_GCC_BLSP1_SLEEP_CBCR_RMSK                                                          0x80000000
#define HWIO_GCC_BLSP1_SLEEP_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_SLEEP_CBCR_ADDR, HWIO_GCC_BLSP1_SLEEP_CBCR_RMSK)
#define HWIO_GCC_BLSP1_SLEEP_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_SLEEP_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_SLEEP_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_BLSP1_SLEEP_CBCR_CLK_OFF_SHFT                                                        0x1f

#define HWIO_GCC_BLSP1_AHB_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00001008)
#define HWIO_GCC_BLSP1_AHB_CBCR_RMSK                                                            0xf000fff0
#define HWIO_GCC_BLSP1_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_AHB_CBCR_ADDR, HWIO_GCC_BLSP1_AHB_CBCR_RMSK)
#define HWIO_GCC_BLSP1_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_AHB_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_AHB_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_AHB_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_AHB_CBCR_IN)
#define HWIO_GCC_BLSP1_AHB_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_BLSP1_AHB_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_BLSP1_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                   0x70000000
#define HWIO_GCC_BLSP1_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                         0x1c
#define HWIO_GCC_BLSP1_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                           0x8000
#define HWIO_GCC_BLSP1_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                              0xf
#define HWIO_GCC_BLSP1_AHB_CBCR_FORCE_MEM_CORE_ON_BMSK                                              0x4000
#define HWIO_GCC_BLSP1_AHB_CBCR_FORCE_MEM_CORE_ON_SHFT                                                 0xe
#define HWIO_GCC_BLSP1_AHB_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                            0x2000
#define HWIO_GCC_BLSP1_AHB_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                               0xd
#define HWIO_GCC_BLSP1_AHB_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                           0x1000
#define HWIO_GCC_BLSP1_AHB_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                              0xc
#define HWIO_GCC_BLSP1_AHB_CBCR_WAKEUP_BMSK                                                          0xf00
#define HWIO_GCC_BLSP1_AHB_CBCR_WAKEUP_SHFT                                                            0x8
#define HWIO_GCC_BLSP1_AHB_CBCR_SLEEP_BMSK                                                            0xf0
#define HWIO_GCC_BLSP1_AHB_CBCR_SLEEP_SHFT                                                             0x4

#define HWIO_GCC_BLSP1_QUP1_BCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00002000)
#define HWIO_GCC_BLSP1_QUP1_BCR_RMSK                                                                   0x1
#define HWIO_GCC_BLSP1_QUP1_BCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_BCR_ADDR, HWIO_GCC_BLSP1_QUP1_BCR_RMSK)
#define HWIO_GCC_BLSP1_QUP1_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_BCR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP1_BCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP1_BCR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP1_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP1_BCR_ADDR,m,v,HWIO_GCC_BLSP1_QUP1_BCR_IN)
#define HWIO_GCC_BLSP1_QUP1_BCR_BLK_ARES_BMSK                                                          0x1
#define HWIO_GCC_BLSP1_QUP1_BCR_BLK_ARES_SHFT                                                          0x0

#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CBCR_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00002004)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CBCR_RMSK                                                  0x80000001
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_SPI_APPS_CBCR_ADDR, HWIO_GCC_BLSP1_QUP1_SPI_APPS_CBCR_RMSK)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_SPI_APPS_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP1_SPI_APPS_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP1_SPI_APPS_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_QUP1_SPI_APPS_CBCR_IN)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CBCR_CLK_OFF_BMSK                                          0x80000000
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CBCR_CLK_OFF_SHFT                                                0x1f
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CBCR_CLK_ENABLE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CBCR_CLK_ENABLE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CBCR_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00002008)
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CBCR_RMSK                                                  0x80000001
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_I2C_APPS_CBCR_ADDR, HWIO_GCC_BLSP1_QUP1_I2C_APPS_CBCR_RMSK)
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_I2C_APPS_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP1_I2C_APPS_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP1_I2C_APPS_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_QUP1_I2C_APPS_CBCR_IN)
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CBCR_CLK_OFF_BMSK                                          0x80000000
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CBCR_CLK_OFF_SHFT                                                0x1f
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CBCR_CLK_ENABLE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CBCR_CLK_ENABLE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0000200c)
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_RMSK                                              0x80000013
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_ADDR, HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_ROOT_OFF_BMSK                                     0x80000000
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_ROOT_OFF_SHFT                                           0x1f
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                     0x10
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                      0x4
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_ROOT_EN_BMSK                                             0x2
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_ROOT_EN_SHFT                                             0x1
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_UPDATE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR_UPDATE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00002010)
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR_RMSK                                                   0x71f
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR_ADDR, HWIO_GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR_SRC_SEL_BMSK                                           0x700
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR_SRC_SEL_SHFT                                             0x8
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR_SRC_DIV_BMSK                                            0x1f
#define HWIO_GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR_SRC_DIV_SHFT                                             0x0

#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00003000)
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_RMSK                                              0x80000013
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_ADDR, HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_ROOT_OFF_BMSK                                     0x80000000
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_ROOT_OFF_SHFT                                           0x1f
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                     0x10
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                      0x4
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_ROOT_EN_BMSK                                             0x2
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_ROOT_EN_SHFT                                             0x1
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_UPDATE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR_UPDATE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CFG_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00003004)
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CFG_RCGR_RMSK                                                   0x71f
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_I2C_APPS_CFG_RCGR_ADDR, HWIO_GCC_BLSP1_QUP2_I2C_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_I2C_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP2_I2C_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP2_I2C_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP2_I2C_APPS_CFG_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CFG_RCGR_SRC_SEL_BMSK                                           0x700
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CFG_RCGR_SRC_SEL_SHFT                                             0x8
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CFG_RCGR_SRC_DIV_BMSK                                            0x1f
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CFG_RCGR_SRC_DIV_SHFT                                             0x0

#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00004000)
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_RMSK                                              0x80000013
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_ADDR, HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_ROOT_OFF_BMSK                                     0x80000000
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_ROOT_OFF_SHFT                                           0x1f
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                     0x10
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                      0x4
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_ROOT_EN_BMSK                                             0x2
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_ROOT_EN_SHFT                                             0x1
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_UPDATE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR_UPDATE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CFG_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00004004)
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CFG_RCGR_RMSK                                                   0x71f
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_I2C_APPS_CFG_RCGR_ADDR, HWIO_GCC_BLSP1_QUP3_I2C_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_I2C_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP3_I2C_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP3_I2C_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP3_I2C_APPS_CFG_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CFG_RCGR_SRC_SEL_BMSK                                           0x700
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CFG_RCGR_SRC_SEL_SHFT                                             0x8
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CFG_RCGR_SRC_DIV_BMSK                                            0x1f
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CFG_RCGR_SRC_DIV_SHFT                                             0x0

#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00005000)
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_RMSK                                              0x80000013
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_ADDR, HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_ROOT_OFF_BMSK                                     0x80000000
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_ROOT_OFF_SHFT                                           0x1f
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                     0x10
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                      0x4
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_ROOT_EN_BMSK                                             0x2
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_ROOT_EN_SHFT                                             0x1
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_UPDATE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR_UPDATE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CFG_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00005004)
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CFG_RCGR_RMSK                                                   0x71f
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_I2C_APPS_CFG_RCGR_ADDR, HWIO_GCC_BLSP1_QUP4_I2C_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_I2C_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP4_I2C_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP4_I2C_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP4_I2C_APPS_CFG_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CFG_RCGR_SRC_SEL_BMSK                                           0x700
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CFG_RCGR_SRC_SEL_SHFT                                             0x8
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CFG_RCGR_SRC_DIV_BMSK                                            0x1f
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CFG_RCGR_SRC_DIV_SHFT                                             0x0

#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00006000)
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_RMSK                                              0x80000013
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_ADDR, HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_ROOT_OFF_BMSK                                     0x80000000
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_ROOT_OFF_SHFT                                           0x1f
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                     0x10
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                      0x4
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_ROOT_EN_BMSK                                             0x2
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_ROOT_EN_SHFT                                             0x1
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_UPDATE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR_UPDATE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CFG_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00006004)
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CFG_RCGR_RMSK                                                   0x71f
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_I2C_APPS_CFG_RCGR_ADDR, HWIO_GCC_BLSP1_QUP5_I2C_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_I2C_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP5_I2C_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP5_I2C_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP5_I2C_APPS_CFG_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CFG_RCGR_SRC_SEL_BMSK                                           0x700
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CFG_RCGR_SRC_SEL_SHFT                                             0x8
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CFG_RCGR_SRC_DIV_BMSK                                            0x1f
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CFG_RCGR_SRC_DIV_SHFT                                             0x0

#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00007000)
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_RMSK                                              0x80000013
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_ADDR, HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_ROOT_OFF_BMSK                                     0x80000000
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_ROOT_OFF_SHFT                                           0x1f
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                     0x10
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                      0x4
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_ROOT_EN_BMSK                                             0x2
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_ROOT_EN_SHFT                                             0x1
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_UPDATE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR_UPDATE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CFG_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00007004)
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CFG_RCGR_RMSK                                                   0x71f
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_I2C_APPS_CFG_RCGR_ADDR, HWIO_GCC_BLSP1_QUP6_I2C_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_I2C_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP6_I2C_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP6_I2C_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP6_I2C_APPS_CFG_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CFG_RCGR_SRC_SEL_BMSK                                           0x700
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CFG_RCGR_SRC_SEL_SHFT                                             0x8
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CFG_RCGR_SRC_DIV_BMSK                                            0x1f
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CFG_RCGR_SRC_DIV_SHFT                                             0x0

#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00002024)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_RMSK                                              0x800000f3
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_ADDR, HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_ROOT_OFF_BMSK                                     0x80000000
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_ROOT_OFF_SHFT                                           0x1f
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_DIRTY_D_BMSK                                            0x80
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_DIRTY_D_SHFT                                             0x7
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_DIRTY_M_BMSK                                            0x40
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_DIRTY_M_SHFT                                             0x6
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_DIRTY_N_BMSK                                            0x20
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_DIRTY_N_SHFT                                             0x5
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                     0x10
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                      0x4
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_ROOT_EN_BMSK                                             0x2
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_ROOT_EN_SHFT                                             0x1
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_UPDATE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CMD_RCGR_UPDATE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CFG_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00002028)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CFG_RCGR_RMSK                                                  0x371f
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_SPI_APPS_CFG_RCGR_ADDR, HWIO_GCC_BLSP1_QUP1_SPI_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_SPI_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP1_SPI_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP1_SPI_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP1_SPI_APPS_CFG_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CFG_RCGR_MODE_BMSK                                             0x3000
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CFG_RCGR_MODE_SHFT                                                0xc
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CFG_RCGR_SRC_SEL_BMSK                                           0x700
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CFG_RCGR_SRC_SEL_SHFT                                             0x8
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CFG_RCGR_SRC_DIV_BMSK                                            0x1f
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_CFG_RCGR_SRC_DIV_SHFT                                             0x0

#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_M_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x0000202c)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_M_RMSK                                                           0xff
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_M_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_SPI_APPS_M_ADDR, HWIO_GCC_BLSP1_QUP1_SPI_APPS_M_RMSK)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_M_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_SPI_APPS_M_ADDR, m)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_M_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP1_SPI_APPS_M_ADDR,v)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP1_SPI_APPS_M_ADDR,m,v,HWIO_GCC_BLSP1_QUP1_SPI_APPS_M_IN)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_M_M_BMSK                                                         0xff
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_M_M_SHFT                                                          0x0

#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_N_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00002030)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_N_RMSK                                                           0xff
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_N_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_SPI_APPS_N_ADDR, HWIO_GCC_BLSP1_QUP1_SPI_APPS_N_RMSK)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_N_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_SPI_APPS_N_ADDR, m)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_N_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP1_SPI_APPS_N_ADDR,v)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP1_SPI_APPS_N_ADDR,m,v,HWIO_GCC_BLSP1_QUP1_SPI_APPS_N_IN)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_N_NOT_N_MINUS_M_BMSK                                             0xff
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_N_NOT_N_MINUS_M_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_D_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00002034)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_D_RMSK                                                           0xff
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_D_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_SPI_APPS_D_ADDR, HWIO_GCC_BLSP1_QUP1_SPI_APPS_D_RMSK)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_D_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP1_SPI_APPS_D_ADDR, m)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_D_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP1_SPI_APPS_D_ADDR,v)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP1_SPI_APPS_D_ADDR,m,v,HWIO_GCC_BLSP1_QUP1_SPI_APPS_D_IN)
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_D_NOT_2D_BMSK                                                    0xff
#define HWIO_GCC_BLSP1_QUP1_SPI_APPS_D_NOT_2D_SHFT                                                     0x0

#define HWIO_GCC_BLSP1_UART1_BCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00002038)
#define HWIO_GCC_BLSP1_UART1_BCR_RMSK                                                                  0x1
#define HWIO_GCC_BLSP1_UART1_BCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_BCR_ADDR, HWIO_GCC_BLSP1_UART1_BCR_RMSK)
#define HWIO_GCC_BLSP1_UART1_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_BCR_ADDR, m)
#define HWIO_GCC_BLSP1_UART1_BCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART1_BCR_ADDR,v)
#define HWIO_GCC_BLSP1_UART1_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART1_BCR_ADDR,m,v,HWIO_GCC_BLSP1_UART1_BCR_IN)
#define HWIO_GCC_BLSP1_UART1_BCR_BLK_ARES_BMSK                                                         0x1
#define HWIO_GCC_BLSP1_UART1_BCR_BLK_ARES_SHFT                                                         0x0

#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x0000203c)
#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_RMSK                                                     0x80000001
#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_CBCR_ADDR, HWIO_GCC_BLSP1_UART1_APPS_CBCR_RMSK)
#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART1_APPS_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART1_APPS_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_UART1_APPS_CBCR_IN)
#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_CLK_OFF_BMSK                                             0x80000000
#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_CLK_OFF_SHFT                                                   0x1f
#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00002040)
#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_RMSK                                                      0x80000001
#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_SIM_CBCR_ADDR, HWIO_GCC_BLSP1_UART1_SIM_CBCR_RMSK)
#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_SIM_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART1_SIM_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART1_SIM_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_UART1_SIM_CBCR_IN)
#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_ADDR                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00002044)
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_RMSK                                                 0x800000f3
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_ADDR, HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_IN)
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_ROOT_OFF_BMSK                                        0x80000000
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_ROOT_OFF_SHFT                                              0x1f
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_DIRTY_D_BMSK                                               0x80
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_DIRTY_D_SHFT                                                0x7
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_DIRTY_M_BMSK                                               0x40
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_DIRTY_M_SHFT                                                0x6
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_DIRTY_N_BMSK                                               0x20
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_DIRTY_N_SHFT                                                0x5
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                        0x10
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                         0x4
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_ROOT_EN_BMSK                                                0x2
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_ROOT_EN_SHFT                                                0x1
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_UPDATE_BMSK                                                 0x1
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_UPDATE_SHFT                                                 0x0

#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_ADDR                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00002048)
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_RMSK                                                     0x371f
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_ADDR, HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_IN)
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_MODE_BMSK                                                0x3000
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_MODE_SHFT                                                   0xc
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_SRC_SEL_BMSK                                              0x700
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_SRC_SEL_SHFT                                                0x8
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_SRC_DIV_BMSK                                               0x1f
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_SRC_DIV_SHFT                                                0x0

#define HWIO_GCC_BLSP1_UART1_APPS_M_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0000204c)
#define HWIO_GCC_BLSP1_UART1_APPS_M_RMSK                                                            0xffff
#define HWIO_GCC_BLSP1_UART1_APPS_M_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_M_ADDR, HWIO_GCC_BLSP1_UART1_APPS_M_RMSK)
#define HWIO_GCC_BLSP1_UART1_APPS_M_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_M_ADDR, m)
#define HWIO_GCC_BLSP1_UART1_APPS_M_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART1_APPS_M_ADDR,v)
#define HWIO_GCC_BLSP1_UART1_APPS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART1_APPS_M_ADDR,m,v,HWIO_GCC_BLSP1_UART1_APPS_M_IN)
#define HWIO_GCC_BLSP1_UART1_APPS_M_M_BMSK                                                          0xffff
#define HWIO_GCC_BLSP1_UART1_APPS_M_M_SHFT                                                             0x0

#define HWIO_GCC_BLSP1_UART1_APPS_N_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00002050)
#define HWIO_GCC_BLSP1_UART1_APPS_N_RMSK                                                            0xffff
#define HWIO_GCC_BLSP1_UART1_APPS_N_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_N_ADDR, HWIO_GCC_BLSP1_UART1_APPS_N_RMSK)
#define HWIO_GCC_BLSP1_UART1_APPS_N_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_N_ADDR, m)
#define HWIO_GCC_BLSP1_UART1_APPS_N_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART1_APPS_N_ADDR,v)
#define HWIO_GCC_BLSP1_UART1_APPS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART1_APPS_N_ADDR,m,v,HWIO_GCC_BLSP1_UART1_APPS_N_IN)
#define HWIO_GCC_BLSP1_UART1_APPS_N_NOT_N_MINUS_M_BMSK                                              0xffff
#define HWIO_GCC_BLSP1_UART1_APPS_N_NOT_N_MINUS_M_SHFT                                                 0x0

#define HWIO_GCC_BLSP1_UART1_APPS_D_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00002054)
#define HWIO_GCC_BLSP1_UART1_APPS_D_RMSK                                                            0xffff
#define HWIO_GCC_BLSP1_UART1_APPS_D_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_D_ADDR, HWIO_GCC_BLSP1_UART1_APPS_D_RMSK)
#define HWIO_GCC_BLSP1_UART1_APPS_D_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_D_ADDR, m)
#define HWIO_GCC_BLSP1_UART1_APPS_D_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART1_APPS_D_ADDR,v)
#define HWIO_GCC_BLSP1_UART1_APPS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART1_APPS_D_ADDR,m,v,HWIO_GCC_BLSP1_UART1_APPS_D_IN)
#define HWIO_GCC_BLSP1_UART1_APPS_D_NOT_2D_BMSK                                                     0xffff
#define HWIO_GCC_BLSP1_UART1_APPS_D_NOT_2D_SHFT                                                        0x0

#define HWIO_GCC_BLSP1_QUP2_BCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00003008)
#define HWIO_GCC_BLSP1_QUP2_BCR_RMSK                                                                   0x1
#define HWIO_GCC_BLSP1_QUP2_BCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_BCR_ADDR, HWIO_GCC_BLSP1_QUP2_BCR_RMSK)
#define HWIO_GCC_BLSP1_QUP2_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_BCR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP2_BCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP2_BCR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP2_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP2_BCR_ADDR,m,v,HWIO_GCC_BLSP1_QUP2_BCR_IN)
#define HWIO_GCC_BLSP1_QUP2_BCR_BLK_ARES_BMSK                                                          0x1
#define HWIO_GCC_BLSP1_QUP2_BCR_BLK_ARES_SHFT                                                          0x0

#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CBCR_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0000300c)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CBCR_RMSK                                                  0x80000001
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_SPI_APPS_CBCR_ADDR, HWIO_GCC_BLSP1_QUP2_SPI_APPS_CBCR_RMSK)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_SPI_APPS_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP2_SPI_APPS_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP2_SPI_APPS_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_QUP2_SPI_APPS_CBCR_IN)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CBCR_CLK_OFF_BMSK                                          0x80000000
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CBCR_CLK_OFF_SHFT                                                0x1f
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CBCR_CLK_ENABLE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CBCR_CLK_ENABLE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CBCR_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00003010)
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CBCR_RMSK                                                  0x80000001
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_I2C_APPS_CBCR_ADDR, HWIO_GCC_BLSP1_QUP2_I2C_APPS_CBCR_RMSK)
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_I2C_APPS_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP2_I2C_APPS_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP2_I2C_APPS_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_QUP2_I2C_APPS_CBCR_IN)
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CBCR_CLK_OFF_BMSK                                          0x80000000
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CBCR_CLK_OFF_SHFT                                                0x1f
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CBCR_CLK_ENABLE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP2_I2C_APPS_CBCR_CLK_ENABLE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00003014)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_RMSK                                              0x800000f3
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_ADDR, HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_ROOT_OFF_BMSK                                     0x80000000
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_ROOT_OFF_SHFT                                           0x1f
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_DIRTY_D_BMSK                                            0x80
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_DIRTY_D_SHFT                                             0x7
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_DIRTY_M_BMSK                                            0x40
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_DIRTY_M_SHFT                                             0x6
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_DIRTY_N_BMSK                                            0x20
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_DIRTY_N_SHFT                                             0x5
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                     0x10
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                      0x4
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_ROOT_EN_BMSK                                             0x2
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_ROOT_EN_SHFT                                             0x1
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_UPDATE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CMD_RCGR_UPDATE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CFG_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00003018)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CFG_RCGR_RMSK                                                  0x371f
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_SPI_APPS_CFG_RCGR_ADDR, HWIO_GCC_BLSP1_QUP2_SPI_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_SPI_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP2_SPI_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP2_SPI_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP2_SPI_APPS_CFG_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CFG_RCGR_MODE_BMSK                                             0x3000
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CFG_RCGR_MODE_SHFT                                                0xc
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CFG_RCGR_SRC_SEL_BMSK                                           0x700
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CFG_RCGR_SRC_SEL_SHFT                                             0x8
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CFG_RCGR_SRC_DIV_BMSK                                            0x1f
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_CFG_RCGR_SRC_DIV_SHFT                                             0x0

#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_M_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x0000301c)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_M_RMSK                                                           0xff
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_M_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_SPI_APPS_M_ADDR, HWIO_GCC_BLSP1_QUP2_SPI_APPS_M_RMSK)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_M_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_SPI_APPS_M_ADDR, m)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_M_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP2_SPI_APPS_M_ADDR,v)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP2_SPI_APPS_M_ADDR,m,v,HWIO_GCC_BLSP1_QUP2_SPI_APPS_M_IN)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_M_M_BMSK                                                         0xff
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_M_M_SHFT                                                          0x0

#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_N_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00003020)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_N_RMSK                                                           0xff
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_N_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_SPI_APPS_N_ADDR, HWIO_GCC_BLSP1_QUP2_SPI_APPS_N_RMSK)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_N_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_SPI_APPS_N_ADDR, m)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_N_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP2_SPI_APPS_N_ADDR,v)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP2_SPI_APPS_N_ADDR,m,v,HWIO_GCC_BLSP1_QUP2_SPI_APPS_N_IN)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_N_NOT_N_MINUS_M_BMSK                                             0xff
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_N_NOT_N_MINUS_M_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_D_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00003024)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_D_RMSK                                                           0xff
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_D_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_SPI_APPS_D_ADDR, HWIO_GCC_BLSP1_QUP2_SPI_APPS_D_RMSK)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_D_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP2_SPI_APPS_D_ADDR, m)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_D_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP2_SPI_APPS_D_ADDR,v)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP2_SPI_APPS_D_ADDR,m,v,HWIO_GCC_BLSP1_QUP2_SPI_APPS_D_IN)
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_D_NOT_2D_BMSK                                                    0xff
#define HWIO_GCC_BLSP1_QUP2_SPI_APPS_D_NOT_2D_SHFT                                                     0x0

#define HWIO_GCC_BLSP1_UART2_BCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00003028)
#define HWIO_GCC_BLSP1_UART2_BCR_RMSK                                                                  0x1
#define HWIO_GCC_BLSP1_UART2_BCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_BCR_ADDR, HWIO_GCC_BLSP1_UART2_BCR_RMSK)
#define HWIO_GCC_BLSP1_UART2_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_BCR_ADDR, m)
#define HWIO_GCC_BLSP1_UART2_BCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART2_BCR_ADDR,v)
#define HWIO_GCC_BLSP1_UART2_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART2_BCR_ADDR,m,v,HWIO_GCC_BLSP1_UART2_BCR_IN)
#define HWIO_GCC_BLSP1_UART2_BCR_BLK_ARES_BMSK                                                         0x1
#define HWIO_GCC_BLSP1_UART2_BCR_BLK_ARES_SHFT                                                         0x0

#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x0000302c)
#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_RMSK                                                     0x80000001
#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_CBCR_ADDR, HWIO_GCC_BLSP1_UART2_APPS_CBCR_RMSK)
#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART2_APPS_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART2_APPS_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_UART2_APPS_CBCR_IN)
#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_CLK_OFF_BMSK                                             0x80000000
#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_CLK_OFF_SHFT                                                   0x1f
#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00003030)
#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_RMSK                                                      0x80000001
#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_SIM_CBCR_ADDR, HWIO_GCC_BLSP1_UART2_SIM_CBCR_RMSK)
#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_SIM_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART2_SIM_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART2_SIM_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_UART2_SIM_CBCR_IN)
#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_ADDR                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00003034)
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_RMSK                                                 0x800000f3
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_ADDR, HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_IN)
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_ROOT_OFF_BMSK                                        0x80000000
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_ROOT_OFF_SHFT                                              0x1f
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_DIRTY_D_BMSK                                               0x80
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_DIRTY_D_SHFT                                                0x7
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_DIRTY_M_BMSK                                               0x40
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_DIRTY_M_SHFT                                                0x6
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_DIRTY_N_BMSK                                               0x20
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_DIRTY_N_SHFT                                                0x5
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                        0x10
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                         0x4
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_ROOT_EN_BMSK                                                0x2
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_ROOT_EN_SHFT                                                0x1
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_UPDATE_BMSK                                                 0x1
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_UPDATE_SHFT                                                 0x0

#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_ADDR                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00003038)
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_RMSK                                                     0x371f
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_ADDR, HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_IN)
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_MODE_BMSK                                                0x3000
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_MODE_SHFT                                                   0xc
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_SRC_SEL_BMSK                                              0x700
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_SRC_SEL_SHFT                                                0x8
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_SRC_DIV_BMSK                                               0x1f
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_SRC_DIV_SHFT                                                0x0

#define HWIO_GCC_BLSP1_UART2_APPS_M_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0000303c)
#define HWIO_GCC_BLSP1_UART2_APPS_M_RMSK                                                            0xffff
#define HWIO_GCC_BLSP1_UART2_APPS_M_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_M_ADDR, HWIO_GCC_BLSP1_UART2_APPS_M_RMSK)
#define HWIO_GCC_BLSP1_UART2_APPS_M_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_M_ADDR, m)
#define HWIO_GCC_BLSP1_UART2_APPS_M_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART2_APPS_M_ADDR,v)
#define HWIO_GCC_BLSP1_UART2_APPS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART2_APPS_M_ADDR,m,v,HWIO_GCC_BLSP1_UART2_APPS_M_IN)
#define HWIO_GCC_BLSP1_UART2_APPS_M_M_BMSK                                                          0xffff
#define HWIO_GCC_BLSP1_UART2_APPS_M_M_SHFT                                                             0x0

#define HWIO_GCC_BLSP1_UART2_APPS_N_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00003040)
#define HWIO_GCC_BLSP1_UART2_APPS_N_RMSK                                                            0xffff
#define HWIO_GCC_BLSP1_UART2_APPS_N_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_N_ADDR, HWIO_GCC_BLSP1_UART2_APPS_N_RMSK)
#define HWIO_GCC_BLSP1_UART2_APPS_N_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_N_ADDR, m)
#define HWIO_GCC_BLSP1_UART2_APPS_N_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART2_APPS_N_ADDR,v)
#define HWIO_GCC_BLSP1_UART2_APPS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART2_APPS_N_ADDR,m,v,HWIO_GCC_BLSP1_UART2_APPS_N_IN)
#define HWIO_GCC_BLSP1_UART2_APPS_N_NOT_N_MINUS_M_BMSK                                              0xffff
#define HWIO_GCC_BLSP1_UART2_APPS_N_NOT_N_MINUS_M_SHFT                                                 0x0

#define HWIO_GCC_BLSP1_UART2_APPS_D_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00003044)
#define HWIO_GCC_BLSP1_UART2_APPS_D_RMSK                                                            0xffff
#define HWIO_GCC_BLSP1_UART2_APPS_D_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_D_ADDR, HWIO_GCC_BLSP1_UART2_APPS_D_RMSK)
#define HWIO_GCC_BLSP1_UART2_APPS_D_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_D_ADDR, m)
#define HWIO_GCC_BLSP1_UART2_APPS_D_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART2_APPS_D_ADDR,v)
#define HWIO_GCC_BLSP1_UART2_APPS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART2_APPS_D_ADDR,m,v,HWIO_GCC_BLSP1_UART2_APPS_D_IN)
#define HWIO_GCC_BLSP1_UART2_APPS_D_NOT_2D_BMSK                                                     0xffff
#define HWIO_GCC_BLSP1_UART2_APPS_D_NOT_2D_SHFT                                                        0x0

#define HWIO_GCC_BLSP1_QUP3_BCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00004018)
#define HWIO_GCC_BLSP1_QUP3_BCR_RMSK                                                                   0x1
#define HWIO_GCC_BLSP1_QUP3_BCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_BCR_ADDR, HWIO_GCC_BLSP1_QUP3_BCR_RMSK)
#define HWIO_GCC_BLSP1_QUP3_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_BCR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP3_BCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP3_BCR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP3_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP3_BCR_ADDR,m,v,HWIO_GCC_BLSP1_QUP3_BCR_IN)
#define HWIO_GCC_BLSP1_QUP3_BCR_BLK_ARES_BMSK                                                          0x1
#define HWIO_GCC_BLSP1_QUP3_BCR_BLK_ARES_SHFT                                                          0x0

#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CBCR_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0000401c)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CBCR_RMSK                                                  0x80000001
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_SPI_APPS_CBCR_ADDR, HWIO_GCC_BLSP1_QUP3_SPI_APPS_CBCR_RMSK)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_SPI_APPS_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP3_SPI_APPS_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP3_SPI_APPS_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_QUP3_SPI_APPS_CBCR_IN)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CBCR_CLK_OFF_BMSK                                          0x80000000
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CBCR_CLK_OFF_SHFT                                                0x1f
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CBCR_CLK_ENABLE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CBCR_CLK_ENABLE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CBCR_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00004020)
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CBCR_RMSK                                                  0x80000001
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_I2C_APPS_CBCR_ADDR, HWIO_GCC_BLSP1_QUP3_I2C_APPS_CBCR_RMSK)
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_I2C_APPS_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP3_I2C_APPS_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP3_I2C_APPS_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_QUP3_I2C_APPS_CBCR_IN)
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CBCR_CLK_OFF_BMSK                                          0x80000000
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CBCR_CLK_OFF_SHFT                                                0x1f
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CBCR_CLK_ENABLE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP3_I2C_APPS_CBCR_CLK_ENABLE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00004024)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_RMSK                                              0x800000f3
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_ADDR, HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_ROOT_OFF_BMSK                                     0x80000000
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_ROOT_OFF_SHFT                                           0x1f
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_DIRTY_D_BMSK                                            0x80
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_DIRTY_D_SHFT                                             0x7
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_DIRTY_M_BMSK                                            0x40
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_DIRTY_M_SHFT                                             0x6
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_DIRTY_N_BMSK                                            0x20
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_DIRTY_N_SHFT                                             0x5
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                     0x10
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                      0x4
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_ROOT_EN_BMSK                                             0x2
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_ROOT_EN_SHFT                                             0x1
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_UPDATE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CMD_RCGR_UPDATE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CFG_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00004028)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CFG_RCGR_RMSK                                                  0x371f
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_SPI_APPS_CFG_RCGR_ADDR, HWIO_GCC_BLSP1_QUP3_SPI_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_SPI_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP3_SPI_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP3_SPI_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP3_SPI_APPS_CFG_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CFG_RCGR_MODE_BMSK                                             0x3000
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CFG_RCGR_MODE_SHFT                                                0xc
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CFG_RCGR_SRC_SEL_BMSK                                           0x700
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CFG_RCGR_SRC_SEL_SHFT                                             0x8
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CFG_RCGR_SRC_DIV_BMSK                                            0x1f
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_CFG_RCGR_SRC_DIV_SHFT                                             0x0

#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_M_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x0000402c)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_M_RMSK                                                           0xff
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_M_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_SPI_APPS_M_ADDR, HWIO_GCC_BLSP1_QUP3_SPI_APPS_M_RMSK)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_M_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_SPI_APPS_M_ADDR, m)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_M_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP3_SPI_APPS_M_ADDR,v)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP3_SPI_APPS_M_ADDR,m,v,HWIO_GCC_BLSP1_QUP3_SPI_APPS_M_IN)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_M_M_BMSK                                                         0xff
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_M_M_SHFT                                                          0x0

#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_N_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00004030)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_N_RMSK                                                           0xff
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_N_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_SPI_APPS_N_ADDR, HWIO_GCC_BLSP1_QUP3_SPI_APPS_N_RMSK)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_N_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_SPI_APPS_N_ADDR, m)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_N_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP3_SPI_APPS_N_ADDR,v)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP3_SPI_APPS_N_ADDR,m,v,HWIO_GCC_BLSP1_QUP3_SPI_APPS_N_IN)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_N_NOT_N_MINUS_M_BMSK                                             0xff
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_N_NOT_N_MINUS_M_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_D_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00004034)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_D_RMSK                                                           0xff
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_D_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_SPI_APPS_D_ADDR, HWIO_GCC_BLSP1_QUP3_SPI_APPS_D_RMSK)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_D_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP3_SPI_APPS_D_ADDR, m)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_D_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP3_SPI_APPS_D_ADDR,v)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP3_SPI_APPS_D_ADDR,m,v,HWIO_GCC_BLSP1_QUP3_SPI_APPS_D_IN)
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_D_NOT_2D_BMSK                                                    0xff
#define HWIO_GCC_BLSP1_QUP3_SPI_APPS_D_NOT_2D_SHFT                                                     0x0

#define HWIO_GCC_BLSP1_QUP4_BCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00005018)
#define HWIO_GCC_BLSP1_QUP4_BCR_RMSK                                                                   0x1
#define HWIO_GCC_BLSP1_QUP4_BCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_BCR_ADDR, HWIO_GCC_BLSP1_QUP4_BCR_RMSK)
#define HWIO_GCC_BLSP1_QUP4_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_BCR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP4_BCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP4_BCR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP4_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP4_BCR_ADDR,m,v,HWIO_GCC_BLSP1_QUP4_BCR_IN)
#define HWIO_GCC_BLSP1_QUP4_BCR_BLK_ARES_BMSK                                                          0x1
#define HWIO_GCC_BLSP1_QUP4_BCR_BLK_ARES_SHFT                                                          0x0

#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CBCR_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0000501c)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CBCR_RMSK                                                  0x80000001
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_SPI_APPS_CBCR_ADDR, HWIO_GCC_BLSP1_QUP4_SPI_APPS_CBCR_RMSK)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_SPI_APPS_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP4_SPI_APPS_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP4_SPI_APPS_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_QUP4_SPI_APPS_CBCR_IN)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CBCR_CLK_OFF_BMSK                                          0x80000000
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CBCR_CLK_OFF_SHFT                                                0x1f
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CBCR_CLK_ENABLE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CBCR_CLK_ENABLE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CBCR_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00005020)
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CBCR_RMSK                                                  0x80000001
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_I2C_APPS_CBCR_ADDR, HWIO_GCC_BLSP1_QUP4_I2C_APPS_CBCR_RMSK)
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_I2C_APPS_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP4_I2C_APPS_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP4_I2C_APPS_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_QUP4_I2C_APPS_CBCR_IN)
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CBCR_CLK_OFF_BMSK                                          0x80000000
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CBCR_CLK_OFF_SHFT                                                0x1f
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CBCR_CLK_ENABLE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP4_I2C_APPS_CBCR_CLK_ENABLE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00005024)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_RMSK                                              0x800000f3
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_ADDR, HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_ROOT_OFF_BMSK                                     0x80000000
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_ROOT_OFF_SHFT                                           0x1f
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_DIRTY_D_BMSK                                            0x80
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_DIRTY_D_SHFT                                             0x7
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_DIRTY_M_BMSK                                            0x40
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_DIRTY_M_SHFT                                             0x6
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_DIRTY_N_BMSK                                            0x20
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_DIRTY_N_SHFT                                             0x5
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                     0x10
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                      0x4
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_ROOT_EN_BMSK                                             0x2
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_ROOT_EN_SHFT                                             0x1
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_UPDATE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CMD_RCGR_UPDATE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CFG_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00005028)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CFG_RCGR_RMSK                                                  0x371f
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_SPI_APPS_CFG_RCGR_ADDR, HWIO_GCC_BLSP1_QUP4_SPI_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_SPI_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP4_SPI_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP4_SPI_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP4_SPI_APPS_CFG_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CFG_RCGR_MODE_BMSK                                             0x3000
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CFG_RCGR_MODE_SHFT                                                0xc
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CFG_RCGR_SRC_SEL_BMSK                                           0x700
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CFG_RCGR_SRC_SEL_SHFT                                             0x8
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CFG_RCGR_SRC_DIV_BMSK                                            0x1f
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_CFG_RCGR_SRC_DIV_SHFT                                             0x0

#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_M_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x0000502c)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_M_RMSK                                                           0xff
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_M_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_SPI_APPS_M_ADDR, HWIO_GCC_BLSP1_QUP4_SPI_APPS_M_RMSK)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_M_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_SPI_APPS_M_ADDR, m)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_M_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP4_SPI_APPS_M_ADDR,v)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP4_SPI_APPS_M_ADDR,m,v,HWIO_GCC_BLSP1_QUP4_SPI_APPS_M_IN)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_M_M_BMSK                                                         0xff
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_M_M_SHFT                                                          0x0

#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_N_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00005030)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_N_RMSK                                                           0xff
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_N_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_SPI_APPS_N_ADDR, HWIO_GCC_BLSP1_QUP4_SPI_APPS_N_RMSK)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_N_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_SPI_APPS_N_ADDR, m)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_N_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP4_SPI_APPS_N_ADDR,v)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP4_SPI_APPS_N_ADDR,m,v,HWIO_GCC_BLSP1_QUP4_SPI_APPS_N_IN)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_N_NOT_N_MINUS_M_BMSK                                             0xff
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_N_NOT_N_MINUS_M_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_D_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00005034)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_D_RMSK                                                           0xff
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_D_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_SPI_APPS_D_ADDR, HWIO_GCC_BLSP1_QUP4_SPI_APPS_D_RMSK)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_D_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP4_SPI_APPS_D_ADDR, m)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_D_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP4_SPI_APPS_D_ADDR,v)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP4_SPI_APPS_D_ADDR,m,v,HWIO_GCC_BLSP1_QUP4_SPI_APPS_D_IN)
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_D_NOT_2D_BMSK                                                    0xff
#define HWIO_GCC_BLSP1_QUP4_SPI_APPS_D_NOT_2D_SHFT                                                     0x0

#define HWIO_GCC_BLSP1_QUP5_BCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00006018)
#define HWIO_GCC_BLSP1_QUP5_BCR_RMSK                                                                   0x1
#define HWIO_GCC_BLSP1_QUP5_BCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_BCR_ADDR, HWIO_GCC_BLSP1_QUP5_BCR_RMSK)
#define HWIO_GCC_BLSP1_QUP5_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_BCR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP5_BCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP5_BCR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP5_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP5_BCR_ADDR,m,v,HWIO_GCC_BLSP1_QUP5_BCR_IN)
#define HWIO_GCC_BLSP1_QUP5_BCR_BLK_ARES_BMSK                                                          0x1
#define HWIO_GCC_BLSP1_QUP5_BCR_BLK_ARES_SHFT                                                          0x0

#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CBCR_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0000601c)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CBCR_RMSK                                                  0x80000001
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_SPI_APPS_CBCR_ADDR, HWIO_GCC_BLSP1_QUP5_SPI_APPS_CBCR_RMSK)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_SPI_APPS_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP5_SPI_APPS_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP5_SPI_APPS_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_QUP5_SPI_APPS_CBCR_IN)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CBCR_CLK_OFF_BMSK                                          0x80000000
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CBCR_CLK_OFF_SHFT                                                0x1f
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CBCR_CLK_ENABLE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CBCR_CLK_ENABLE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CBCR_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00006020)
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CBCR_RMSK                                                  0x80000001
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_I2C_APPS_CBCR_ADDR, HWIO_GCC_BLSP1_QUP5_I2C_APPS_CBCR_RMSK)
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_I2C_APPS_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP5_I2C_APPS_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP5_I2C_APPS_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_QUP5_I2C_APPS_CBCR_IN)
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CBCR_CLK_OFF_BMSK                                          0x80000000
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CBCR_CLK_OFF_SHFT                                                0x1f
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CBCR_CLK_ENABLE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP5_I2C_APPS_CBCR_CLK_ENABLE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00006024)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_RMSK                                              0x800000f3
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_ADDR, HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_ROOT_OFF_BMSK                                     0x80000000
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_ROOT_OFF_SHFT                                           0x1f
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_DIRTY_D_BMSK                                            0x80
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_DIRTY_D_SHFT                                             0x7
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_DIRTY_M_BMSK                                            0x40
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_DIRTY_M_SHFT                                             0x6
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_DIRTY_N_BMSK                                            0x20
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_DIRTY_N_SHFT                                             0x5
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                     0x10
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                      0x4
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_ROOT_EN_BMSK                                             0x2
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_ROOT_EN_SHFT                                             0x1
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_UPDATE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CMD_RCGR_UPDATE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CFG_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00006028)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CFG_RCGR_RMSK                                                  0x371f
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_SPI_APPS_CFG_RCGR_ADDR, HWIO_GCC_BLSP1_QUP5_SPI_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_SPI_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP5_SPI_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP5_SPI_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP5_SPI_APPS_CFG_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CFG_RCGR_MODE_BMSK                                             0x3000
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CFG_RCGR_MODE_SHFT                                                0xc
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CFG_RCGR_SRC_SEL_BMSK                                           0x700
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CFG_RCGR_SRC_SEL_SHFT                                             0x8
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CFG_RCGR_SRC_DIV_BMSK                                            0x1f
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_CFG_RCGR_SRC_DIV_SHFT                                             0x0

#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_M_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x0000602c)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_M_RMSK                                                           0xff
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_M_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_SPI_APPS_M_ADDR, HWIO_GCC_BLSP1_QUP5_SPI_APPS_M_RMSK)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_M_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_SPI_APPS_M_ADDR, m)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_M_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP5_SPI_APPS_M_ADDR,v)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP5_SPI_APPS_M_ADDR,m,v,HWIO_GCC_BLSP1_QUP5_SPI_APPS_M_IN)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_M_M_BMSK                                                         0xff
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_M_M_SHFT                                                          0x0

#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_N_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00006030)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_N_RMSK                                                           0xff
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_N_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_SPI_APPS_N_ADDR, HWIO_GCC_BLSP1_QUP5_SPI_APPS_N_RMSK)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_N_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_SPI_APPS_N_ADDR, m)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_N_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP5_SPI_APPS_N_ADDR,v)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP5_SPI_APPS_N_ADDR,m,v,HWIO_GCC_BLSP1_QUP5_SPI_APPS_N_IN)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_N_NOT_N_MINUS_M_BMSK                                             0xff
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_N_NOT_N_MINUS_M_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_D_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00006034)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_D_RMSK                                                           0xff
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_D_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_SPI_APPS_D_ADDR, HWIO_GCC_BLSP1_QUP5_SPI_APPS_D_RMSK)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_D_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP5_SPI_APPS_D_ADDR, m)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_D_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP5_SPI_APPS_D_ADDR,v)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP5_SPI_APPS_D_ADDR,m,v,HWIO_GCC_BLSP1_QUP5_SPI_APPS_D_IN)
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_D_NOT_2D_BMSK                                                    0xff
#define HWIO_GCC_BLSP1_QUP5_SPI_APPS_D_NOT_2D_SHFT                                                     0x0

#define HWIO_GCC_BLSP1_QUP6_BCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00007018)
#define HWIO_GCC_BLSP1_QUP6_BCR_RMSK                                                                   0x1
#define HWIO_GCC_BLSP1_QUP6_BCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_BCR_ADDR, HWIO_GCC_BLSP1_QUP6_BCR_RMSK)
#define HWIO_GCC_BLSP1_QUP6_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_BCR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP6_BCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP6_BCR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP6_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP6_BCR_ADDR,m,v,HWIO_GCC_BLSP1_QUP6_BCR_IN)
#define HWIO_GCC_BLSP1_QUP6_BCR_BLK_ARES_BMSK                                                          0x1
#define HWIO_GCC_BLSP1_QUP6_BCR_BLK_ARES_SHFT                                                          0x0

#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CBCR_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0000701c)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CBCR_RMSK                                                  0x80000001
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_SPI_APPS_CBCR_ADDR, HWIO_GCC_BLSP1_QUP6_SPI_APPS_CBCR_RMSK)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_SPI_APPS_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP6_SPI_APPS_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP6_SPI_APPS_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_QUP6_SPI_APPS_CBCR_IN)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CBCR_CLK_OFF_BMSK                                          0x80000000
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CBCR_CLK_OFF_SHFT                                                0x1f
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CBCR_CLK_ENABLE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CBCR_CLK_ENABLE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CBCR_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00007020)
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CBCR_RMSK                                                  0x80000001
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_I2C_APPS_CBCR_ADDR, HWIO_GCC_BLSP1_QUP6_I2C_APPS_CBCR_RMSK)
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_I2C_APPS_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP6_I2C_APPS_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP6_I2C_APPS_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_QUP6_I2C_APPS_CBCR_IN)
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CBCR_CLK_OFF_BMSK                                          0x80000000
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CBCR_CLK_OFF_SHFT                                                0x1f
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CBCR_CLK_ENABLE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP6_I2C_APPS_CBCR_CLK_ENABLE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00007024)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_RMSK                                              0x800000f3
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_ADDR, HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_ROOT_OFF_BMSK                                     0x80000000
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_ROOT_OFF_SHFT                                           0x1f
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_DIRTY_D_BMSK                                            0x80
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_DIRTY_D_SHFT                                             0x7
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_DIRTY_M_BMSK                                            0x40
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_DIRTY_M_SHFT                                             0x6
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_DIRTY_N_BMSK                                            0x20
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_DIRTY_N_SHFT                                             0x5
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                     0x10
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                      0x4
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_ROOT_EN_BMSK                                             0x2
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_ROOT_EN_SHFT                                             0x1
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_UPDATE_BMSK                                              0x1
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CMD_RCGR_UPDATE_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CFG_RCGR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00007028)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CFG_RCGR_RMSK                                                  0x371f
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_SPI_APPS_CFG_RCGR_ADDR, HWIO_GCC_BLSP1_QUP6_SPI_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_SPI_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP6_SPI_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP6_SPI_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_QUP6_SPI_APPS_CFG_RCGR_IN)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CFG_RCGR_MODE_BMSK                                             0x3000
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CFG_RCGR_MODE_SHFT                                                0xc
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CFG_RCGR_SRC_SEL_BMSK                                           0x700
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CFG_RCGR_SRC_SEL_SHFT                                             0x8
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CFG_RCGR_SRC_DIV_BMSK                                            0x1f
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_CFG_RCGR_SRC_DIV_SHFT                                             0x0

#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_M_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x0000702c)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_M_RMSK                                                           0xff
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_M_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_SPI_APPS_M_ADDR, HWIO_GCC_BLSP1_QUP6_SPI_APPS_M_RMSK)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_M_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_SPI_APPS_M_ADDR, m)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_M_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP6_SPI_APPS_M_ADDR,v)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP6_SPI_APPS_M_ADDR,m,v,HWIO_GCC_BLSP1_QUP6_SPI_APPS_M_IN)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_M_M_BMSK                                                         0xff
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_M_M_SHFT                                                          0x0

#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_N_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00007030)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_N_RMSK                                                           0xff
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_N_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_SPI_APPS_N_ADDR, HWIO_GCC_BLSP1_QUP6_SPI_APPS_N_RMSK)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_N_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_SPI_APPS_N_ADDR, m)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_N_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP6_SPI_APPS_N_ADDR,v)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP6_SPI_APPS_N_ADDR,m,v,HWIO_GCC_BLSP1_QUP6_SPI_APPS_N_IN)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_N_NOT_N_MINUS_M_BMSK                                             0xff
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_N_NOT_N_MINUS_M_SHFT                                              0x0

#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_D_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00007034)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_D_RMSK                                                           0xff
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_D_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_SPI_APPS_D_ADDR, HWIO_GCC_BLSP1_QUP6_SPI_APPS_D_RMSK)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_D_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_QUP6_SPI_APPS_D_ADDR, m)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_D_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_QUP6_SPI_APPS_D_ADDR,v)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_QUP6_SPI_APPS_D_ADDR,m,v,HWIO_GCC_BLSP1_QUP6_SPI_APPS_D_IN)
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_D_NOT_2D_BMSK                                                    0xff
#define HWIO_GCC_BLSP1_QUP6_SPI_APPS_D_NOT_2D_SHFT                                                     0x0

#define HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x0000100c)
#define HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_RMSK                                                    0x80000013
#define HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_ADDR, HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_RMSK)
#define HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_ADDR,m,v,HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_IN)
#define HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_ROOT_OFF_BMSK                                           0x80000000
#define HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_ROOT_OFF_SHFT                                                 0x1f
#define HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                           0x10
#define HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                            0x4
#define HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_ROOT_EN_BMSK                                                   0x2
#define HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_ROOT_EN_SHFT                                                   0x1
#define HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_UPDATE_BMSK                                                    0x1
#define HWIO_GCC_BLSP_UART_SIM_CMD_RCGR_UPDATE_SHFT                                                    0x0

#define HWIO_GCC_BLSP_UART_SIM_CFG_RCGR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00001010)
#define HWIO_GCC_BLSP_UART_SIM_CFG_RCGR_RMSK                                                          0x1f
#define HWIO_GCC_BLSP_UART_SIM_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP_UART_SIM_CFG_RCGR_ADDR, HWIO_GCC_BLSP_UART_SIM_CFG_RCGR_RMSK)
#define HWIO_GCC_BLSP_UART_SIM_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP_UART_SIM_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BLSP_UART_SIM_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP_UART_SIM_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BLSP_UART_SIM_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP_UART_SIM_CFG_RCGR_ADDR,m,v,HWIO_GCC_BLSP_UART_SIM_CFG_RCGR_IN)
#define HWIO_GCC_BLSP_UART_SIM_CFG_RCGR_SRC_DIV_BMSK                                                  0x1f
#define HWIO_GCC_BLSP_UART_SIM_CFG_RCGR_SRC_DIV_SHFT                                                   0x0

#define HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00017008)
#define HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_RMSK                                                     0xf0008001
#define HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_ADDR, HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_RMSK)
#define HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_ADDR, m)
#define HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_ADDR,v)
#define HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_ADDR,m,v,HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_IN)
#define HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_CLK_OFF_BMSK                                             0x80000000
#define HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_CLK_OFF_SHFT                                                   0x1f
#define HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                            0x70000000
#define HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                  0x1c
#define HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                    0x8000
#define HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                       0xf
#define HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_GCC_PRNG_XPU_CFG_AHB_CBCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_GCC_PDM_BCR_ADDR                                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x00044000)
#define HWIO_GCC_PDM_BCR_RMSK                                                                          0x1
#define HWIO_GCC_PDM_BCR_IN          \
        in_dword_masked(HWIO_GCC_PDM_BCR_ADDR, HWIO_GCC_PDM_BCR_RMSK)
#define HWIO_GCC_PDM_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PDM_BCR_ADDR, m)
#define HWIO_GCC_PDM_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PDM_BCR_ADDR,v)
#define HWIO_GCC_PDM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PDM_BCR_ADDR,m,v,HWIO_GCC_PDM_BCR_IN)
#define HWIO_GCC_PDM_BCR_BLK_ARES_BMSK                                                                 0x1
#define HWIO_GCC_PDM_BCR_BLK_ARES_SHFT                                                                 0x0

#define HWIO_GCC_PDM_AHB_CBCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00044004)
#define HWIO_GCC_PDM_AHB_CBCR_RMSK                                                              0xf0008001
#define HWIO_GCC_PDM_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PDM_AHB_CBCR_ADDR, HWIO_GCC_PDM_AHB_CBCR_RMSK)
#define HWIO_GCC_PDM_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PDM_AHB_CBCR_ADDR, m)
#define HWIO_GCC_PDM_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PDM_AHB_CBCR_ADDR,v)
#define HWIO_GCC_PDM_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PDM_AHB_CBCR_ADDR,m,v,HWIO_GCC_PDM_AHB_CBCR_IN)
#define HWIO_GCC_PDM_AHB_CBCR_CLK_OFF_BMSK                                                      0x80000000
#define HWIO_GCC_PDM_AHB_CBCR_CLK_OFF_SHFT                                                            0x1f
#define HWIO_GCC_PDM_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                     0x70000000
#define HWIO_GCC_PDM_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                           0x1c
#define HWIO_GCC_PDM_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                             0x8000
#define HWIO_GCC_PDM_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                                0xf
#define HWIO_GCC_PDM_AHB_CBCR_CLK_ENABLE_BMSK                                                          0x1
#define HWIO_GCC_PDM_AHB_CBCR_CLK_ENABLE_SHFT                                                          0x0

#define HWIO_GCC_PDM_XO4_CBCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00044008)
#define HWIO_GCC_PDM_XO4_CBCR_RMSK                                                              0x80030001
#define HWIO_GCC_PDM_XO4_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PDM_XO4_CBCR_ADDR, HWIO_GCC_PDM_XO4_CBCR_RMSK)
#define HWIO_GCC_PDM_XO4_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PDM_XO4_CBCR_ADDR, m)
#define HWIO_GCC_PDM_XO4_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PDM_XO4_CBCR_ADDR,v)
#define HWIO_GCC_PDM_XO4_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PDM_XO4_CBCR_ADDR,m,v,HWIO_GCC_PDM_XO4_CBCR_IN)
#define HWIO_GCC_PDM_XO4_CBCR_CLK_OFF_BMSK                                                      0x80000000
#define HWIO_GCC_PDM_XO4_CBCR_CLK_OFF_SHFT                                                            0x1f
#define HWIO_GCC_PDM_XO4_CBCR_CLK_DIV_BMSK                                                         0x30000
#define HWIO_GCC_PDM_XO4_CBCR_CLK_DIV_SHFT                                                            0x10
#define HWIO_GCC_PDM_XO4_CBCR_CLK_ENABLE_BMSK                                                          0x1
#define HWIO_GCC_PDM_XO4_CBCR_CLK_ENABLE_SHFT                                                          0x0

#define HWIO_GCC_PDM2_CBCR_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x0004400c)
#define HWIO_GCC_PDM2_CBCR_RMSK                                                                 0x80000001
#define HWIO_GCC_PDM2_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PDM2_CBCR_ADDR, HWIO_GCC_PDM2_CBCR_RMSK)
#define HWIO_GCC_PDM2_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PDM2_CBCR_ADDR, m)
#define HWIO_GCC_PDM2_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PDM2_CBCR_ADDR,v)
#define HWIO_GCC_PDM2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PDM2_CBCR_ADDR,m,v,HWIO_GCC_PDM2_CBCR_IN)
#define HWIO_GCC_PDM2_CBCR_CLK_OFF_BMSK                                                         0x80000000
#define HWIO_GCC_PDM2_CBCR_CLK_OFF_SHFT                                                               0x1f
#define HWIO_GCC_PDM2_CBCR_CLK_ENABLE_BMSK                                                             0x1
#define HWIO_GCC_PDM2_CBCR_CLK_ENABLE_SHFT                                                             0x0

#define HWIO_GCC_PDM2_CMD_RCGR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00044010)
#define HWIO_GCC_PDM2_CMD_RCGR_RMSK                                                             0x80000013
#define HWIO_GCC_PDM2_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_PDM2_CMD_RCGR_ADDR, HWIO_GCC_PDM2_CMD_RCGR_RMSK)
#define HWIO_GCC_PDM2_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_PDM2_CMD_RCGR_ADDR, m)
#define HWIO_GCC_PDM2_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_PDM2_CMD_RCGR_ADDR,v)
#define HWIO_GCC_PDM2_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PDM2_CMD_RCGR_ADDR,m,v,HWIO_GCC_PDM2_CMD_RCGR_IN)
#define HWIO_GCC_PDM2_CMD_RCGR_ROOT_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_PDM2_CMD_RCGR_ROOT_OFF_SHFT                                                          0x1f
#define HWIO_GCC_PDM2_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                    0x10
#define HWIO_GCC_PDM2_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                     0x4
#define HWIO_GCC_PDM2_CMD_RCGR_ROOT_EN_BMSK                                                            0x2
#define HWIO_GCC_PDM2_CMD_RCGR_ROOT_EN_SHFT                                                            0x1
#define HWIO_GCC_PDM2_CMD_RCGR_UPDATE_BMSK                                                             0x1
#define HWIO_GCC_PDM2_CMD_RCGR_UPDATE_SHFT                                                             0x0

#define HWIO_GCC_PDM2_CFG_RCGR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00044014)
#define HWIO_GCC_PDM2_CFG_RCGR_RMSK                                                                  0x71f
#define HWIO_GCC_PDM2_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_PDM2_CFG_RCGR_ADDR, HWIO_GCC_PDM2_CFG_RCGR_RMSK)
#define HWIO_GCC_PDM2_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_PDM2_CFG_RCGR_ADDR, m)
#define HWIO_GCC_PDM2_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_PDM2_CFG_RCGR_ADDR,v)
#define HWIO_GCC_PDM2_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PDM2_CFG_RCGR_ADDR,m,v,HWIO_GCC_PDM2_CFG_RCGR_IN)
#define HWIO_GCC_PDM2_CFG_RCGR_SRC_SEL_BMSK                                                          0x700
#define HWIO_GCC_PDM2_CFG_RCGR_SRC_SEL_SHFT                                                            0x8
#define HWIO_GCC_PDM2_CFG_RCGR_SRC_DIV_BMSK                                                           0x1f
#define HWIO_GCC_PDM2_CFG_RCGR_SRC_DIV_SHFT                                                            0x0

#define HWIO_GCC_PRNG_BCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00013000)
#define HWIO_GCC_PRNG_BCR_RMSK                                                                         0x1
#define HWIO_GCC_PRNG_BCR_IN          \
        in_dword_masked(HWIO_GCC_PRNG_BCR_ADDR, HWIO_GCC_PRNG_BCR_RMSK)
#define HWIO_GCC_PRNG_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PRNG_BCR_ADDR, m)
#define HWIO_GCC_PRNG_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PRNG_BCR_ADDR,v)
#define HWIO_GCC_PRNG_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PRNG_BCR_ADDR,m,v,HWIO_GCC_PRNG_BCR_IN)
#define HWIO_GCC_PRNG_BCR_BLK_ARES_BMSK                                                                0x1
#define HWIO_GCC_PRNG_BCR_BLK_ARES_SHFT                                                                0x0

#define HWIO_GCC_PRNG_AHB_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00013004)
#define HWIO_GCC_PRNG_AHB_CBCR_RMSK                                                             0xf0008000
#define HWIO_GCC_PRNG_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PRNG_AHB_CBCR_ADDR, HWIO_GCC_PRNG_AHB_CBCR_RMSK)
#define HWIO_GCC_PRNG_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PRNG_AHB_CBCR_ADDR, m)
#define HWIO_GCC_PRNG_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PRNG_AHB_CBCR_ADDR,v)
#define HWIO_GCC_PRNG_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PRNG_AHB_CBCR_ADDR,m,v,HWIO_GCC_PRNG_AHB_CBCR_IN)
#define HWIO_GCC_PRNG_AHB_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_PRNG_AHB_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_PRNG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                    0x70000000
#define HWIO_GCC_PRNG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                          0x1c
#define HWIO_GCC_PRNG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                            0x8000
#define HWIO_GCC_PRNG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                               0xf

#define HWIO_GCC_TCSR_BCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00028000)
#define HWIO_GCC_TCSR_BCR_RMSK                                                                         0x1
#define HWIO_GCC_TCSR_BCR_IN          \
        in_dword_masked(HWIO_GCC_TCSR_BCR_ADDR, HWIO_GCC_TCSR_BCR_RMSK)
#define HWIO_GCC_TCSR_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_TCSR_BCR_ADDR, m)
#define HWIO_GCC_TCSR_BCR_OUT(v)      \
        out_dword(HWIO_GCC_TCSR_BCR_ADDR,v)
#define HWIO_GCC_TCSR_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_TCSR_BCR_ADDR,m,v,HWIO_GCC_TCSR_BCR_IN)
#define HWIO_GCC_TCSR_BCR_BLK_ARES_BMSK                                                                0x1
#define HWIO_GCC_TCSR_BCR_BLK_ARES_SHFT                                                                0x0

#define HWIO_GCC_TCSR_AHB_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00028004)
#define HWIO_GCC_TCSR_AHB_CBCR_RMSK                                                             0xf0008001
#define HWIO_GCC_TCSR_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_TCSR_AHB_CBCR_ADDR, HWIO_GCC_TCSR_AHB_CBCR_RMSK)
#define HWIO_GCC_TCSR_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_TCSR_AHB_CBCR_ADDR, m)
#define HWIO_GCC_TCSR_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_TCSR_AHB_CBCR_ADDR,v)
#define HWIO_GCC_TCSR_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_TCSR_AHB_CBCR_ADDR,m,v,HWIO_GCC_TCSR_AHB_CBCR_IN)
#define HWIO_GCC_TCSR_AHB_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_TCSR_AHB_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_TCSR_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                    0x70000000
#define HWIO_GCC_TCSR_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                          0x1c
#define HWIO_GCC_TCSR_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                            0x8000
#define HWIO_GCC_TCSR_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                               0xf
#define HWIO_GCC_TCSR_AHB_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_GCC_TCSR_AHB_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_GCC_BOOT_ROM_BCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00013008)
#define HWIO_GCC_BOOT_ROM_BCR_RMSK                                                                     0x1
#define HWIO_GCC_BOOT_ROM_BCR_IN          \
        in_dword_masked(HWIO_GCC_BOOT_ROM_BCR_ADDR, HWIO_GCC_BOOT_ROM_BCR_RMSK)
#define HWIO_GCC_BOOT_ROM_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BOOT_ROM_BCR_ADDR, m)
#define HWIO_GCC_BOOT_ROM_BCR_OUT(v)      \
        out_dword(HWIO_GCC_BOOT_ROM_BCR_ADDR,v)
#define HWIO_GCC_BOOT_ROM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BOOT_ROM_BCR_ADDR,m,v,HWIO_GCC_BOOT_ROM_BCR_IN)
#define HWIO_GCC_BOOT_ROM_BCR_BLK_ARES_BMSK                                                            0x1
#define HWIO_GCC_BOOT_ROM_BCR_BLK_ARES_SHFT                                                            0x0

#define HWIO_GCC_BOOT_ROM_AHB_CBCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0001300c)
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_RMSK                                                         0xf000fff0
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BOOT_ROM_AHB_CBCR_ADDR, HWIO_GCC_BOOT_ROM_AHB_CBCR_RMSK)
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BOOT_ROM_AHB_CBCR_ADDR, m)
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BOOT_ROM_AHB_CBCR_ADDR,v)
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BOOT_ROM_AHB_CBCR_ADDR,m,v,HWIO_GCC_BOOT_ROM_AHB_CBCR_IN)
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_CLK_OFF_BMSK                                                 0x80000000
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_CLK_OFF_SHFT                                                       0x1f
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                0x70000000
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                      0x1c
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                        0x8000
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                           0xf
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_FORCE_MEM_CORE_ON_BMSK                                           0x4000
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_FORCE_MEM_CORE_ON_SHFT                                              0xe
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                         0x2000
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                            0xd
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                        0x1000
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                           0xc
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_WAKEUP_BMSK                                                       0xf00
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_WAKEUP_SHFT                                                         0x8
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_SLEEP_BMSK                                                         0xf0
#define HWIO_GCC_BOOT_ROM_AHB_CBCR_SLEEP_SHFT                                                          0x4

#define HWIO_GCC_MSG_RAM_BCR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0002b000)
#define HWIO_GCC_MSG_RAM_BCR_RMSK                                                                      0x1
#define HWIO_GCC_MSG_RAM_BCR_IN          \
        in_dword_masked(HWIO_GCC_MSG_RAM_BCR_ADDR, HWIO_GCC_MSG_RAM_BCR_RMSK)
#define HWIO_GCC_MSG_RAM_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MSG_RAM_BCR_ADDR, m)
#define HWIO_GCC_MSG_RAM_BCR_OUT(v)      \
        out_dword(HWIO_GCC_MSG_RAM_BCR_ADDR,v)
#define HWIO_GCC_MSG_RAM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSG_RAM_BCR_ADDR,m,v,HWIO_GCC_MSG_RAM_BCR_IN)
#define HWIO_GCC_MSG_RAM_BCR_BLK_ARES_BMSK                                                             0x1
#define HWIO_GCC_MSG_RAM_BCR_BLK_ARES_SHFT                                                             0x0

#define HWIO_GCC_MSG_RAM_AHB_CBCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x0002b004)
#define HWIO_GCC_MSG_RAM_AHB_CBCR_RMSK                                                          0xf000fff0
#define HWIO_GCC_MSG_RAM_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MSG_RAM_AHB_CBCR_ADDR, HWIO_GCC_MSG_RAM_AHB_CBCR_RMSK)
#define HWIO_GCC_MSG_RAM_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MSG_RAM_AHB_CBCR_ADDR, m)
#define HWIO_GCC_MSG_RAM_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MSG_RAM_AHB_CBCR_ADDR,v)
#define HWIO_GCC_MSG_RAM_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSG_RAM_AHB_CBCR_ADDR,m,v,HWIO_GCC_MSG_RAM_AHB_CBCR_IN)
#define HWIO_GCC_MSG_RAM_AHB_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_MSG_RAM_AHB_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_GCC_MSG_RAM_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                 0x70000000
#define HWIO_GCC_MSG_RAM_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                       0x1c
#define HWIO_GCC_MSG_RAM_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                         0x8000
#define HWIO_GCC_MSG_RAM_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                            0xf
#define HWIO_GCC_MSG_RAM_AHB_CBCR_FORCE_MEM_CORE_ON_BMSK                                            0x4000
#define HWIO_GCC_MSG_RAM_AHB_CBCR_FORCE_MEM_CORE_ON_SHFT                                               0xe
#define HWIO_GCC_MSG_RAM_AHB_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                          0x2000
#define HWIO_GCC_MSG_RAM_AHB_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                             0xd
#define HWIO_GCC_MSG_RAM_AHB_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                         0x1000
#define HWIO_GCC_MSG_RAM_AHB_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                            0xc
#define HWIO_GCC_MSG_RAM_AHB_CBCR_WAKEUP_BMSK                                                        0xf00
#define HWIO_GCC_MSG_RAM_AHB_CBCR_WAKEUP_SHFT                                                          0x8
#define HWIO_GCC_MSG_RAM_AHB_CBCR_SLEEP_BMSK                                                          0xf0
#define HWIO_GCC_MSG_RAM_AHB_CBCR_SLEEP_SHFT                                                           0x4

#define HWIO_GCC_TLMM_BCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00034000)
#define HWIO_GCC_TLMM_BCR_RMSK                                                                         0x1
#define HWIO_GCC_TLMM_BCR_IN          \
        in_dword_masked(HWIO_GCC_TLMM_BCR_ADDR, HWIO_GCC_TLMM_BCR_RMSK)
#define HWIO_GCC_TLMM_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_TLMM_BCR_ADDR, m)
#define HWIO_GCC_TLMM_BCR_OUT(v)      \
        out_dword(HWIO_GCC_TLMM_BCR_ADDR,v)
#define HWIO_GCC_TLMM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_TLMM_BCR_ADDR,m,v,HWIO_GCC_TLMM_BCR_IN)
#define HWIO_GCC_TLMM_BCR_BLK_ARES_BMSK                                                                0x1
#define HWIO_GCC_TLMM_BCR_BLK_ARES_SHFT                                                                0x0

#define HWIO_GCC_TLMM_AHB_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00034004)
#define HWIO_GCC_TLMM_AHB_CBCR_RMSK                                                             0xf0008000
#define HWIO_GCC_TLMM_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_TLMM_AHB_CBCR_ADDR, HWIO_GCC_TLMM_AHB_CBCR_RMSK)
#define HWIO_GCC_TLMM_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_TLMM_AHB_CBCR_ADDR, m)
#define HWIO_GCC_TLMM_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_TLMM_AHB_CBCR_ADDR,v)
#define HWIO_GCC_TLMM_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_TLMM_AHB_CBCR_ADDR,m,v,HWIO_GCC_TLMM_AHB_CBCR_IN)
#define HWIO_GCC_TLMM_AHB_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_TLMM_AHB_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_TLMM_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                    0x70000000
#define HWIO_GCC_TLMM_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                          0x1c
#define HWIO_GCC_TLMM_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                            0x8000
#define HWIO_GCC_TLMM_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                               0xf

#define HWIO_GCC_TLMM_CBCR_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00034008)
#define HWIO_GCC_TLMM_CBCR_RMSK                                                                 0x80000000
#define HWIO_GCC_TLMM_CBCR_IN          \
        in_dword_masked(HWIO_GCC_TLMM_CBCR_ADDR, HWIO_GCC_TLMM_CBCR_RMSK)
#define HWIO_GCC_TLMM_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_TLMM_CBCR_ADDR, m)
#define HWIO_GCC_TLMM_CBCR_CLK_OFF_BMSK                                                         0x80000000
#define HWIO_GCC_TLMM_CBCR_CLK_OFF_SHFT                                                               0x1f

#define HWIO_GCC_MPM_BCR_ADDR                                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0002c000)
#define HWIO_GCC_MPM_BCR_RMSK                                                                          0x1
#define HWIO_GCC_MPM_BCR_IN          \
        in_dword_masked(HWIO_GCC_MPM_BCR_ADDR, HWIO_GCC_MPM_BCR_RMSK)
#define HWIO_GCC_MPM_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MPM_BCR_ADDR, m)
#define HWIO_GCC_MPM_BCR_OUT(v)      \
        out_dword(HWIO_GCC_MPM_BCR_ADDR,v)
#define HWIO_GCC_MPM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MPM_BCR_ADDR,m,v,HWIO_GCC_MPM_BCR_IN)
#define HWIO_GCC_MPM_BCR_BLK_ARES_BMSK                                                                 0x1
#define HWIO_GCC_MPM_BCR_BLK_ARES_SHFT                                                                 0x0

#define HWIO_GCC_MPM_MISC_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0002c004)
#define HWIO_GCC_MPM_MISC_RMSK                                                                         0x7
#define HWIO_GCC_MPM_MISC_IN          \
        in_dword_masked(HWIO_GCC_MPM_MISC_ADDR, HWIO_GCC_MPM_MISC_RMSK)
#define HWIO_GCC_MPM_MISC_INM(m)      \
        in_dword_masked(HWIO_GCC_MPM_MISC_ADDR, m)
#define HWIO_GCC_MPM_MISC_OUT(v)      \
        out_dword(HWIO_GCC_MPM_MISC_ADDR,v)
#define HWIO_GCC_MPM_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MPM_MISC_ADDR,m,v,HWIO_GCC_MPM_MISC_IN)
#define HWIO_GCC_MPM_MISC_MPM_NON_AHB_RESET_BMSK                                                       0x4
#define HWIO_GCC_MPM_MISC_MPM_NON_AHB_RESET_SHFT                                                       0x2
#define HWIO_GCC_MPM_MISC_MPM_AHB_RESET_BMSK                                                           0x2
#define HWIO_GCC_MPM_MISC_MPM_AHB_RESET_SHFT                                                           0x1
#define HWIO_GCC_MPM_MISC_MPM_REF_CLK_EN_BMSK                                                          0x1
#define HWIO_GCC_MPM_MISC_MPM_REF_CLK_EN_SHFT                                                          0x0

#define HWIO_GCC_MPM_AHB_CBCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0002c008)
#define HWIO_GCC_MPM_AHB_CBCR_RMSK                                                              0xf0008000
#define HWIO_GCC_MPM_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MPM_AHB_CBCR_ADDR, HWIO_GCC_MPM_AHB_CBCR_RMSK)
#define HWIO_GCC_MPM_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MPM_AHB_CBCR_ADDR, m)
#define HWIO_GCC_MPM_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MPM_AHB_CBCR_ADDR,v)
#define HWIO_GCC_MPM_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MPM_AHB_CBCR_ADDR,m,v,HWIO_GCC_MPM_AHB_CBCR_IN)
#define HWIO_GCC_MPM_AHB_CBCR_CLK_OFF_BMSK                                                      0x80000000
#define HWIO_GCC_MPM_AHB_CBCR_CLK_OFF_SHFT                                                            0x1f
#define HWIO_GCC_MPM_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                     0x70000000
#define HWIO_GCC_MPM_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                           0x1c
#define HWIO_GCC_MPM_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                             0x8000
#define HWIO_GCC_MPM_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                                0xf

#define HWIO_GCC_RPM_PROC_HCLK_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0002d000)
#define HWIO_GCC_RPM_PROC_HCLK_CBCR_RMSK                                                        0x80000000
#define HWIO_GCC_RPM_PROC_HCLK_CBCR_IN          \
        in_dword_masked(HWIO_GCC_RPM_PROC_HCLK_CBCR_ADDR, HWIO_GCC_RPM_PROC_HCLK_CBCR_RMSK)
#define HWIO_GCC_RPM_PROC_HCLK_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPM_PROC_HCLK_CBCR_ADDR, m)
#define HWIO_GCC_RPM_PROC_HCLK_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_RPM_PROC_HCLK_CBCR_CLK_OFF_SHFT                                                      0x1f

#define HWIO_GCC_RPM_BUS_AHB_CBCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x0002d004)
#define HWIO_GCC_RPM_BUS_AHB_CBCR_RMSK                                                          0xf000fff0
#define HWIO_GCC_RPM_BUS_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_RPM_BUS_AHB_CBCR_ADDR, HWIO_GCC_RPM_BUS_AHB_CBCR_RMSK)
#define HWIO_GCC_RPM_BUS_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPM_BUS_AHB_CBCR_ADDR, m)
#define HWIO_GCC_RPM_BUS_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_RPM_BUS_AHB_CBCR_ADDR,v)
#define HWIO_GCC_RPM_BUS_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPM_BUS_AHB_CBCR_ADDR,m,v,HWIO_GCC_RPM_BUS_AHB_CBCR_IN)
#define HWIO_GCC_RPM_BUS_AHB_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_RPM_BUS_AHB_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_GCC_RPM_BUS_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                 0x70000000
#define HWIO_GCC_RPM_BUS_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                       0x1c
#define HWIO_GCC_RPM_BUS_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                         0x8000
#define HWIO_GCC_RPM_BUS_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                            0xf
#define HWIO_GCC_RPM_BUS_AHB_CBCR_FORCE_MEM_CORE_ON_BMSK                                            0x4000
#define HWIO_GCC_RPM_BUS_AHB_CBCR_FORCE_MEM_CORE_ON_SHFT                                               0xe
#define HWIO_GCC_RPM_BUS_AHB_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                          0x2000
#define HWIO_GCC_RPM_BUS_AHB_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                             0xd
#define HWIO_GCC_RPM_BUS_AHB_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                         0x1000
#define HWIO_GCC_RPM_BUS_AHB_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                            0xc
#define HWIO_GCC_RPM_BUS_AHB_CBCR_WAKEUP_BMSK                                                        0xf00
#define HWIO_GCC_RPM_BUS_AHB_CBCR_WAKEUP_SHFT                                                          0x8
#define HWIO_GCC_RPM_BUS_AHB_CBCR_SLEEP_BMSK                                                          0xf0
#define HWIO_GCC_RPM_BUS_AHB_CBCR_SLEEP_SHFT                                                           0x4

#define HWIO_GCC_RPM_SLEEP_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0002d008)
#define HWIO_GCC_RPM_SLEEP_CBCR_RMSK                                                            0x80000001
#define HWIO_GCC_RPM_SLEEP_CBCR_IN          \
        in_dword_masked(HWIO_GCC_RPM_SLEEP_CBCR_ADDR, HWIO_GCC_RPM_SLEEP_CBCR_RMSK)
#define HWIO_GCC_RPM_SLEEP_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPM_SLEEP_CBCR_ADDR, m)
#define HWIO_GCC_RPM_SLEEP_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_RPM_SLEEP_CBCR_ADDR,v)
#define HWIO_GCC_RPM_SLEEP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPM_SLEEP_CBCR_ADDR,m,v,HWIO_GCC_RPM_SLEEP_CBCR_IN)
#define HWIO_GCC_RPM_SLEEP_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_RPM_SLEEP_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_RPM_SLEEP_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_RPM_SLEEP_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_RPM_TIMER_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0002d00c)
#define HWIO_GCC_RPM_TIMER_CBCR_RMSK                                                            0x80000003
#define HWIO_GCC_RPM_TIMER_CBCR_IN          \
        in_dword_masked(HWIO_GCC_RPM_TIMER_CBCR_ADDR, HWIO_GCC_RPM_TIMER_CBCR_RMSK)
#define HWIO_GCC_RPM_TIMER_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPM_TIMER_CBCR_ADDR, m)
#define HWIO_GCC_RPM_TIMER_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_RPM_TIMER_CBCR_ADDR,v)
#define HWIO_GCC_RPM_TIMER_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPM_TIMER_CBCR_ADDR,m,v,HWIO_GCC_RPM_TIMER_CBCR_IN)
#define HWIO_GCC_RPM_TIMER_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_RPM_TIMER_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_RPM_TIMER_CBCR_HW_CTL_BMSK                                                            0x2
#define HWIO_GCC_RPM_TIMER_CBCR_HW_CTL_SHFT                                                            0x1
#define HWIO_GCC_RPM_TIMER_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_RPM_TIMER_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_RPM_CMD_RCGR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0002d010)
#define HWIO_GCC_RPM_CMD_RCGR_RMSK                                                              0x80000013
#define HWIO_GCC_RPM_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_RPM_CMD_RCGR_ADDR, HWIO_GCC_RPM_CMD_RCGR_RMSK)
#define HWIO_GCC_RPM_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPM_CMD_RCGR_ADDR, m)
#define HWIO_GCC_RPM_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_RPM_CMD_RCGR_ADDR,v)
#define HWIO_GCC_RPM_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPM_CMD_RCGR_ADDR,m,v,HWIO_GCC_RPM_CMD_RCGR_IN)
#define HWIO_GCC_RPM_CMD_RCGR_ROOT_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_RPM_CMD_RCGR_ROOT_OFF_SHFT                                                           0x1f
#define HWIO_GCC_RPM_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                     0x10
#define HWIO_GCC_RPM_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                      0x4
#define HWIO_GCC_RPM_CMD_RCGR_ROOT_EN_BMSK                                                             0x2
#define HWIO_GCC_RPM_CMD_RCGR_ROOT_EN_SHFT                                                             0x1
#define HWIO_GCC_RPM_CMD_RCGR_UPDATE_BMSK                                                              0x1
#define HWIO_GCC_RPM_CMD_RCGR_UPDATE_SHFT                                                              0x0

#define HWIO_GCC_RPM_CFG_RCGR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0002d014)
#define HWIO_GCC_RPM_CFG_RCGR_RMSK                                                                   0x71f
#define HWIO_GCC_RPM_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_RPM_CFG_RCGR_ADDR, HWIO_GCC_RPM_CFG_RCGR_RMSK)
#define HWIO_GCC_RPM_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPM_CFG_RCGR_ADDR, m)
#define HWIO_GCC_RPM_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_RPM_CFG_RCGR_ADDR,v)
#define HWIO_GCC_RPM_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPM_CFG_RCGR_ADDR,m,v,HWIO_GCC_RPM_CFG_RCGR_IN)
#define HWIO_GCC_RPM_CFG_RCGR_SRC_SEL_BMSK                                                           0x700
#define HWIO_GCC_RPM_CFG_RCGR_SRC_SEL_SHFT                                                             0x8
#define HWIO_GCC_RPM_CFG_RCGR_SRC_DIV_BMSK                                                            0x1f
#define HWIO_GCC_RPM_CFG_RCGR_SRC_DIV_SHFT                                                             0x0

#define HWIO_GCC_RPM_MISC_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0002d028)
#define HWIO_GCC_RPM_MISC_RMSK                                                                        0xf1
#define HWIO_GCC_RPM_MISC_IN          \
        in_dword_masked(HWIO_GCC_RPM_MISC_ADDR, HWIO_GCC_RPM_MISC_RMSK)
#define HWIO_GCC_RPM_MISC_INM(m)      \
        in_dword_masked(HWIO_GCC_RPM_MISC_ADDR, m)
#define HWIO_GCC_RPM_MISC_OUT(v)      \
        out_dword(HWIO_GCC_RPM_MISC_ADDR,v)
#define HWIO_GCC_RPM_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPM_MISC_ADDR,m,v,HWIO_GCC_RPM_MISC_IN)
#define HWIO_GCC_RPM_MISC_RPM_CLK_AUTO_SCALE_DIV_BMSK                                                 0xf0
#define HWIO_GCC_RPM_MISC_RPM_CLK_AUTO_SCALE_DIV_SHFT                                                  0x4
#define HWIO_GCC_RPM_MISC_RPM_CLK_AUTO_SCALE_DIS_BMSK                                                  0x1
#define HWIO_GCC_RPM_MISC_RPM_CLK_AUTO_SCALE_DIS_SHFT                                                  0x0

#define HWIO_GCC_SEC_CTRL_BCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0001a000)
#define HWIO_GCC_SEC_CTRL_BCR_RMSK                                                                     0x1
#define HWIO_GCC_SEC_CTRL_BCR_IN          \
        in_dword_masked(HWIO_GCC_SEC_CTRL_BCR_ADDR, HWIO_GCC_SEC_CTRL_BCR_RMSK)
#define HWIO_GCC_SEC_CTRL_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SEC_CTRL_BCR_ADDR, m)
#define HWIO_GCC_SEC_CTRL_BCR_OUT(v)      \
        out_dword(HWIO_GCC_SEC_CTRL_BCR_ADDR,v)
#define HWIO_GCC_SEC_CTRL_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SEC_CTRL_BCR_ADDR,m,v,HWIO_GCC_SEC_CTRL_BCR_IN)
#define HWIO_GCC_SEC_CTRL_BCR_BLK_ARES_BMSK                                                            0x1
#define HWIO_GCC_SEC_CTRL_BCR_BLK_ARES_SHFT                                                            0x0

#define HWIO_GCC_ACC_CMD_RCGR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0001a004)
#define HWIO_GCC_ACC_CMD_RCGR_RMSK                                                              0x80000013
#define HWIO_GCC_ACC_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_ACC_CMD_RCGR_ADDR, HWIO_GCC_ACC_CMD_RCGR_RMSK)
#define HWIO_GCC_ACC_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_ACC_CMD_RCGR_ADDR, m)
#define HWIO_GCC_ACC_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_ACC_CMD_RCGR_ADDR,v)
#define HWIO_GCC_ACC_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ACC_CMD_RCGR_ADDR,m,v,HWIO_GCC_ACC_CMD_RCGR_IN)
#define HWIO_GCC_ACC_CMD_RCGR_ROOT_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_ACC_CMD_RCGR_ROOT_OFF_SHFT                                                           0x1f
#define HWIO_GCC_ACC_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                     0x10
#define HWIO_GCC_ACC_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                      0x4
#define HWIO_GCC_ACC_CMD_RCGR_ROOT_EN_BMSK                                                             0x2
#define HWIO_GCC_ACC_CMD_RCGR_ROOT_EN_SHFT                                                             0x1
#define HWIO_GCC_ACC_CMD_RCGR_UPDATE_BMSK                                                              0x1
#define HWIO_GCC_ACC_CMD_RCGR_UPDATE_SHFT                                                              0x0

#define HWIO_GCC_ACC_CFG_RCGR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0001a008)
#define HWIO_GCC_ACC_CFG_RCGR_RMSK                                                                   0x71f
#define HWIO_GCC_ACC_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_ACC_CFG_RCGR_ADDR, HWIO_GCC_ACC_CFG_RCGR_RMSK)
#define HWIO_GCC_ACC_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_ACC_CFG_RCGR_ADDR, m)
#define HWIO_GCC_ACC_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_ACC_CFG_RCGR_ADDR,v)
#define HWIO_GCC_ACC_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ACC_CFG_RCGR_ADDR,m,v,HWIO_GCC_ACC_CFG_RCGR_IN)
#define HWIO_GCC_ACC_CFG_RCGR_SRC_SEL_BMSK                                                           0x700
#define HWIO_GCC_ACC_CFG_RCGR_SRC_SEL_SHFT                                                             0x8
#define HWIO_GCC_ACC_CFG_RCGR_SRC_DIV_BMSK                                                            0x1f
#define HWIO_GCC_ACC_CFG_RCGR_SRC_DIV_SHFT                                                             0x0

#define HWIO_GCC_ACC_MISC_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0001a01c)
#define HWIO_GCC_ACC_MISC_RMSK                                                                         0x1
#define HWIO_GCC_ACC_MISC_IN          \
        in_dword_masked(HWIO_GCC_ACC_MISC_ADDR, HWIO_GCC_ACC_MISC_RMSK)
#define HWIO_GCC_ACC_MISC_INM(m)      \
        in_dword_masked(HWIO_GCC_ACC_MISC_ADDR, m)
#define HWIO_GCC_ACC_MISC_OUT(v)      \
        out_dword(HWIO_GCC_ACC_MISC_ADDR,v)
#define HWIO_GCC_ACC_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ACC_MISC_ADDR,m,v,HWIO_GCC_ACC_MISC_IN)
#define HWIO_GCC_ACC_MISC_JTAG_ACC_SRC_SEL_EN_BMSK                                                     0x1
#define HWIO_GCC_ACC_MISC_JTAG_ACC_SRC_SEL_EN_SHFT                                                     0x0

#define HWIO_GCC_SEC_CTRL_ACC_CBCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0001a020)
#define HWIO_GCC_SEC_CTRL_ACC_CBCR_RMSK                                                         0x80007ff1
#define HWIO_GCC_SEC_CTRL_ACC_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SEC_CTRL_ACC_CBCR_ADDR, HWIO_GCC_SEC_CTRL_ACC_CBCR_RMSK)
#define HWIO_GCC_SEC_CTRL_ACC_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SEC_CTRL_ACC_CBCR_ADDR, m)
#define HWIO_GCC_SEC_CTRL_ACC_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SEC_CTRL_ACC_CBCR_ADDR,v)
#define HWIO_GCC_SEC_CTRL_ACC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SEC_CTRL_ACC_CBCR_ADDR,m,v,HWIO_GCC_SEC_CTRL_ACC_CBCR_IN)
#define HWIO_GCC_SEC_CTRL_ACC_CBCR_CLK_OFF_BMSK                                                 0x80000000
#define HWIO_GCC_SEC_CTRL_ACC_CBCR_CLK_OFF_SHFT                                                       0x1f
#define HWIO_GCC_SEC_CTRL_ACC_CBCR_FORCE_MEM_CORE_ON_BMSK                                           0x4000
#define HWIO_GCC_SEC_CTRL_ACC_CBCR_FORCE_MEM_CORE_ON_SHFT                                              0xe
#define HWIO_GCC_SEC_CTRL_ACC_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                         0x2000
#define HWIO_GCC_SEC_CTRL_ACC_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                            0xd
#define HWIO_GCC_SEC_CTRL_ACC_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                        0x1000
#define HWIO_GCC_SEC_CTRL_ACC_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                           0xc
#define HWIO_GCC_SEC_CTRL_ACC_CBCR_WAKEUP_BMSK                                                       0xf00
#define HWIO_GCC_SEC_CTRL_ACC_CBCR_WAKEUP_SHFT                                                         0x8
#define HWIO_GCC_SEC_CTRL_ACC_CBCR_SLEEP_BMSK                                                         0xf0
#define HWIO_GCC_SEC_CTRL_ACC_CBCR_SLEEP_SHFT                                                          0x4
#define HWIO_GCC_SEC_CTRL_ACC_CBCR_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_GCC_SEC_CTRL_ACC_CBCR_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_GCC_SEC_CTRL_AHB_CBCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0001a024)
#define HWIO_GCC_SEC_CTRL_AHB_CBCR_RMSK                                                         0xf0008001
#define HWIO_GCC_SEC_CTRL_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SEC_CTRL_AHB_CBCR_ADDR, HWIO_GCC_SEC_CTRL_AHB_CBCR_RMSK)
#define HWIO_GCC_SEC_CTRL_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SEC_CTRL_AHB_CBCR_ADDR, m)
#define HWIO_GCC_SEC_CTRL_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SEC_CTRL_AHB_CBCR_ADDR,v)
#define HWIO_GCC_SEC_CTRL_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SEC_CTRL_AHB_CBCR_ADDR,m,v,HWIO_GCC_SEC_CTRL_AHB_CBCR_IN)
#define HWIO_GCC_SEC_CTRL_AHB_CBCR_CLK_OFF_BMSK                                                 0x80000000
#define HWIO_GCC_SEC_CTRL_AHB_CBCR_CLK_OFF_SHFT                                                       0x1f
#define HWIO_GCC_SEC_CTRL_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                0x70000000
#define HWIO_GCC_SEC_CTRL_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                      0x1c
#define HWIO_GCC_SEC_CTRL_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                        0x8000
#define HWIO_GCC_SEC_CTRL_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                           0xf
#define HWIO_GCC_SEC_CTRL_AHB_CBCR_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_GCC_SEC_CTRL_AHB_CBCR_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_GCC_SEC_CTRL_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0001a028)
#define HWIO_GCC_SEC_CTRL_CBCR_RMSK                                                             0x80007ff1
#define HWIO_GCC_SEC_CTRL_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SEC_CTRL_CBCR_ADDR, HWIO_GCC_SEC_CTRL_CBCR_RMSK)
#define HWIO_GCC_SEC_CTRL_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SEC_CTRL_CBCR_ADDR, m)
#define HWIO_GCC_SEC_CTRL_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SEC_CTRL_CBCR_ADDR,v)
#define HWIO_GCC_SEC_CTRL_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SEC_CTRL_CBCR_ADDR,m,v,HWIO_GCC_SEC_CTRL_CBCR_IN)
#define HWIO_GCC_SEC_CTRL_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_SEC_CTRL_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_SEC_CTRL_CBCR_FORCE_MEM_CORE_ON_BMSK                                               0x4000
#define HWIO_GCC_SEC_CTRL_CBCR_FORCE_MEM_CORE_ON_SHFT                                                  0xe
#define HWIO_GCC_SEC_CTRL_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                             0x2000
#define HWIO_GCC_SEC_CTRL_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                0xd
#define HWIO_GCC_SEC_CTRL_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                            0x1000
#define HWIO_GCC_SEC_CTRL_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                               0xc
#define HWIO_GCC_SEC_CTRL_CBCR_WAKEUP_BMSK                                                           0xf00
#define HWIO_GCC_SEC_CTRL_CBCR_WAKEUP_SHFT                                                             0x8
#define HWIO_GCC_SEC_CTRL_CBCR_SLEEP_BMSK                                                             0xf0
#define HWIO_GCC_SEC_CTRL_CBCR_SLEEP_SHFT                                                              0x4
#define HWIO_GCC_SEC_CTRL_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_GCC_SEC_CTRL_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_GCC_SEC_CTRL_SENSE_CBCR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x0001a02c)
#define HWIO_GCC_SEC_CTRL_SENSE_CBCR_RMSK                                                       0x80000001
#define HWIO_GCC_SEC_CTRL_SENSE_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SEC_CTRL_SENSE_CBCR_ADDR, HWIO_GCC_SEC_CTRL_SENSE_CBCR_RMSK)
#define HWIO_GCC_SEC_CTRL_SENSE_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SEC_CTRL_SENSE_CBCR_ADDR, m)
#define HWIO_GCC_SEC_CTRL_SENSE_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SEC_CTRL_SENSE_CBCR_ADDR,v)
#define HWIO_GCC_SEC_CTRL_SENSE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SEC_CTRL_SENSE_CBCR_ADDR,m,v,HWIO_GCC_SEC_CTRL_SENSE_CBCR_IN)
#define HWIO_GCC_SEC_CTRL_SENSE_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_GCC_SEC_CTRL_SENSE_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_GCC_SEC_CTRL_SENSE_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_GCC_SEC_CTRL_SENSE_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_GCC_SEC_CTRL_BOOT_ROM_PATCH_CBCR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0001a030)
#define HWIO_GCC_SEC_CTRL_BOOT_ROM_PATCH_CBCR_RMSK                                              0x80000001
#define HWIO_GCC_SEC_CTRL_BOOT_ROM_PATCH_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SEC_CTRL_BOOT_ROM_PATCH_CBCR_ADDR, HWIO_GCC_SEC_CTRL_BOOT_ROM_PATCH_CBCR_RMSK)
#define HWIO_GCC_SEC_CTRL_BOOT_ROM_PATCH_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SEC_CTRL_BOOT_ROM_PATCH_CBCR_ADDR, m)
#define HWIO_GCC_SEC_CTRL_BOOT_ROM_PATCH_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SEC_CTRL_BOOT_ROM_PATCH_CBCR_ADDR,v)
#define HWIO_GCC_SEC_CTRL_BOOT_ROM_PATCH_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SEC_CTRL_BOOT_ROM_PATCH_CBCR_ADDR,m,v,HWIO_GCC_SEC_CTRL_BOOT_ROM_PATCH_CBCR_IN)
#define HWIO_GCC_SEC_CTRL_BOOT_ROM_PATCH_CBCR_CLK_OFF_BMSK                                      0x80000000
#define HWIO_GCC_SEC_CTRL_BOOT_ROM_PATCH_CBCR_CLK_OFF_SHFT                                            0x1f
#define HWIO_GCC_SEC_CTRL_BOOT_ROM_PATCH_CBCR_CLK_ENABLE_BMSK                                          0x1
#define HWIO_GCC_SEC_CTRL_BOOT_ROM_PATCH_CBCR_CLK_ENABLE_SHFT                                          0x0

#define HWIO_GCC_SEC_CTRL_CMD_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0001a034)
#define HWIO_GCC_SEC_CTRL_CMD_RCGR_RMSK                                                         0x80000013
#define HWIO_GCC_SEC_CTRL_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_SEC_CTRL_CMD_RCGR_ADDR, HWIO_GCC_SEC_CTRL_CMD_RCGR_RMSK)
#define HWIO_GCC_SEC_CTRL_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_SEC_CTRL_CMD_RCGR_ADDR, m)
#define HWIO_GCC_SEC_CTRL_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_SEC_CTRL_CMD_RCGR_ADDR,v)
#define HWIO_GCC_SEC_CTRL_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SEC_CTRL_CMD_RCGR_ADDR,m,v,HWIO_GCC_SEC_CTRL_CMD_RCGR_IN)
#define HWIO_GCC_SEC_CTRL_CMD_RCGR_ROOT_OFF_BMSK                                                0x80000000
#define HWIO_GCC_SEC_CTRL_CMD_RCGR_ROOT_OFF_SHFT                                                      0x1f
#define HWIO_GCC_SEC_CTRL_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                0x10
#define HWIO_GCC_SEC_CTRL_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                 0x4
#define HWIO_GCC_SEC_CTRL_CMD_RCGR_ROOT_EN_BMSK                                                        0x2
#define HWIO_GCC_SEC_CTRL_CMD_RCGR_ROOT_EN_SHFT                                                        0x1
#define HWIO_GCC_SEC_CTRL_CMD_RCGR_UPDATE_BMSK                                                         0x1
#define HWIO_GCC_SEC_CTRL_CMD_RCGR_UPDATE_SHFT                                                         0x0

#define HWIO_GCC_SEC_CTRL_CFG_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0001a038)
#define HWIO_GCC_SEC_CTRL_CFG_RCGR_RMSK                                                              0x71f
#define HWIO_GCC_SEC_CTRL_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_SEC_CTRL_CFG_RCGR_ADDR, HWIO_GCC_SEC_CTRL_CFG_RCGR_RMSK)
#define HWIO_GCC_SEC_CTRL_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_SEC_CTRL_CFG_RCGR_ADDR, m)
#define HWIO_GCC_SEC_CTRL_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_SEC_CTRL_CFG_RCGR_ADDR,v)
#define HWIO_GCC_SEC_CTRL_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SEC_CTRL_CFG_RCGR_ADDR,m,v,HWIO_GCC_SEC_CTRL_CFG_RCGR_IN)
#define HWIO_GCC_SEC_CTRL_CFG_RCGR_SRC_SEL_BMSK                                                      0x700
#define HWIO_GCC_SEC_CTRL_CFG_RCGR_SRC_SEL_SHFT                                                        0x8
#define HWIO_GCC_SEC_CTRL_CFG_RCGR_SRC_DIV_BMSK                                                       0x1f
#define HWIO_GCC_SEC_CTRL_CFG_RCGR_SRC_DIV_SHFT                                                        0x0

#define HWIO_GCC_SPMI_BCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0002e000)
#define HWIO_GCC_SPMI_BCR_RMSK                                                                         0x1
#define HWIO_GCC_SPMI_BCR_IN          \
        in_dword_masked(HWIO_GCC_SPMI_BCR_ADDR, HWIO_GCC_SPMI_BCR_RMSK)
#define HWIO_GCC_SPMI_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPMI_BCR_ADDR, m)
#define HWIO_GCC_SPMI_BCR_OUT(v)      \
        out_dword(HWIO_GCC_SPMI_BCR_ADDR,v)
#define HWIO_GCC_SPMI_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPMI_BCR_ADDR,m,v,HWIO_GCC_SPMI_BCR_IN)
#define HWIO_GCC_SPMI_BCR_BLK_ARES_BMSK                                                                0x1
#define HWIO_GCC_SPMI_BCR_BLK_ARES_SHFT                                                                0x0

#define HWIO_GCC_SPMI_SER_CMD_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0002e004)
#define HWIO_GCC_SPMI_SER_CMD_RCGR_RMSK                                                         0x80000013
#define HWIO_GCC_SPMI_SER_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_SPMI_SER_CMD_RCGR_ADDR, HWIO_GCC_SPMI_SER_CMD_RCGR_RMSK)
#define HWIO_GCC_SPMI_SER_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPMI_SER_CMD_RCGR_ADDR, m)
#define HWIO_GCC_SPMI_SER_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_SPMI_SER_CMD_RCGR_ADDR,v)
#define HWIO_GCC_SPMI_SER_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPMI_SER_CMD_RCGR_ADDR,m,v,HWIO_GCC_SPMI_SER_CMD_RCGR_IN)
#define HWIO_GCC_SPMI_SER_CMD_RCGR_ROOT_OFF_BMSK                                                0x80000000
#define HWIO_GCC_SPMI_SER_CMD_RCGR_ROOT_OFF_SHFT                                                      0x1f
#define HWIO_GCC_SPMI_SER_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                0x10
#define HWIO_GCC_SPMI_SER_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                 0x4
#define HWIO_GCC_SPMI_SER_CMD_RCGR_ROOT_EN_BMSK                                                        0x2
#define HWIO_GCC_SPMI_SER_CMD_RCGR_ROOT_EN_SHFT                                                        0x1
#define HWIO_GCC_SPMI_SER_CMD_RCGR_UPDATE_BMSK                                                         0x1
#define HWIO_GCC_SPMI_SER_CMD_RCGR_UPDATE_SHFT                                                         0x0

#define HWIO_GCC_SPMI_SER_CFG_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0002e008)
#define HWIO_GCC_SPMI_SER_CFG_RCGR_RMSK                                                              0x71f
#define HWIO_GCC_SPMI_SER_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_SPMI_SER_CFG_RCGR_ADDR, HWIO_GCC_SPMI_SER_CFG_RCGR_RMSK)
#define HWIO_GCC_SPMI_SER_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPMI_SER_CFG_RCGR_ADDR, m)
#define HWIO_GCC_SPMI_SER_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_SPMI_SER_CFG_RCGR_ADDR,v)
#define HWIO_GCC_SPMI_SER_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPMI_SER_CFG_RCGR_ADDR,m,v,HWIO_GCC_SPMI_SER_CFG_RCGR_IN)
#define HWIO_GCC_SPMI_SER_CFG_RCGR_SRC_SEL_BMSK                                                      0x700
#define HWIO_GCC_SPMI_SER_CFG_RCGR_SRC_SEL_SHFT                                                        0x8
#define HWIO_GCC_SPMI_SER_CFG_RCGR_SRC_DIV_BMSK                                                       0x1f
#define HWIO_GCC_SPMI_SER_CFG_RCGR_SRC_DIV_SHFT                                                        0x0

#define HWIO_GCC_SPMI_SER_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0002e01c)
#define HWIO_GCC_SPMI_SER_CBCR_RMSK                                                             0x80007ff1
#define HWIO_GCC_SPMI_SER_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SPMI_SER_CBCR_ADDR, HWIO_GCC_SPMI_SER_CBCR_RMSK)
#define HWIO_GCC_SPMI_SER_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPMI_SER_CBCR_ADDR, m)
#define HWIO_GCC_SPMI_SER_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SPMI_SER_CBCR_ADDR,v)
#define HWIO_GCC_SPMI_SER_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPMI_SER_CBCR_ADDR,m,v,HWIO_GCC_SPMI_SER_CBCR_IN)
#define HWIO_GCC_SPMI_SER_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_SPMI_SER_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_SPMI_SER_CBCR_FORCE_MEM_CORE_ON_BMSK                                               0x4000
#define HWIO_GCC_SPMI_SER_CBCR_FORCE_MEM_CORE_ON_SHFT                                                  0xe
#define HWIO_GCC_SPMI_SER_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                             0x2000
#define HWIO_GCC_SPMI_SER_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                0xd
#define HWIO_GCC_SPMI_SER_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                            0x1000
#define HWIO_GCC_SPMI_SER_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                               0xc
#define HWIO_GCC_SPMI_SER_CBCR_WAKEUP_BMSK                                                           0xf00
#define HWIO_GCC_SPMI_SER_CBCR_WAKEUP_SHFT                                                             0x8
#define HWIO_GCC_SPMI_SER_CBCR_SLEEP_BMSK                                                             0xf0
#define HWIO_GCC_SPMI_SER_CBCR_SLEEP_SHFT                                                              0x4
#define HWIO_GCC_SPMI_SER_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_GCC_SPMI_SER_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_GCC_SPMI_PCNOC_AHB_CBCR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x0002e020)
#define HWIO_GCC_SPMI_PCNOC_AHB_CBCR_RMSK                                                       0xf0008000
#define HWIO_GCC_SPMI_PCNOC_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SPMI_PCNOC_AHB_CBCR_ADDR, HWIO_GCC_SPMI_PCNOC_AHB_CBCR_RMSK)
#define HWIO_GCC_SPMI_PCNOC_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPMI_PCNOC_AHB_CBCR_ADDR, m)
#define HWIO_GCC_SPMI_PCNOC_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SPMI_PCNOC_AHB_CBCR_ADDR,v)
#define HWIO_GCC_SPMI_PCNOC_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPMI_PCNOC_AHB_CBCR_ADDR,m,v,HWIO_GCC_SPMI_PCNOC_AHB_CBCR_IN)
#define HWIO_GCC_SPMI_PCNOC_AHB_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_GCC_SPMI_PCNOC_AHB_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_GCC_SPMI_PCNOC_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                              0x70000000
#define HWIO_GCC_SPMI_PCNOC_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                    0x1c
#define HWIO_GCC_SPMI_PCNOC_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                      0x8000
#define HWIO_GCC_SPMI_PCNOC_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                         0xf

#define HWIO_GCC_SPMI_AHB_CMD_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0002e024)
#define HWIO_GCC_SPMI_AHB_CMD_RCGR_RMSK                                                         0x80000013
#define HWIO_GCC_SPMI_AHB_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_SPMI_AHB_CMD_RCGR_ADDR, HWIO_GCC_SPMI_AHB_CMD_RCGR_RMSK)
#define HWIO_GCC_SPMI_AHB_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPMI_AHB_CMD_RCGR_ADDR, m)
#define HWIO_GCC_SPMI_AHB_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_SPMI_AHB_CMD_RCGR_ADDR,v)
#define HWIO_GCC_SPMI_AHB_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPMI_AHB_CMD_RCGR_ADDR,m,v,HWIO_GCC_SPMI_AHB_CMD_RCGR_IN)
#define HWIO_GCC_SPMI_AHB_CMD_RCGR_ROOT_OFF_BMSK                                                0x80000000
#define HWIO_GCC_SPMI_AHB_CMD_RCGR_ROOT_OFF_SHFT                                                      0x1f
#define HWIO_GCC_SPMI_AHB_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                0x10
#define HWIO_GCC_SPMI_AHB_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                 0x4
#define HWIO_GCC_SPMI_AHB_CMD_RCGR_ROOT_EN_BMSK                                                        0x2
#define HWIO_GCC_SPMI_AHB_CMD_RCGR_ROOT_EN_SHFT                                                        0x1
#define HWIO_GCC_SPMI_AHB_CMD_RCGR_UPDATE_BMSK                                                         0x1
#define HWIO_GCC_SPMI_AHB_CMD_RCGR_UPDATE_SHFT                                                         0x0

#define HWIO_GCC_SPMI_AHB_CFG_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0002e028)
#define HWIO_GCC_SPMI_AHB_CFG_RCGR_RMSK                                                              0x71f
#define HWIO_GCC_SPMI_AHB_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_SPMI_AHB_CFG_RCGR_ADDR, HWIO_GCC_SPMI_AHB_CFG_RCGR_RMSK)
#define HWIO_GCC_SPMI_AHB_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPMI_AHB_CFG_RCGR_ADDR, m)
#define HWIO_GCC_SPMI_AHB_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_SPMI_AHB_CFG_RCGR_ADDR,v)
#define HWIO_GCC_SPMI_AHB_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPMI_AHB_CFG_RCGR_ADDR,m,v,HWIO_GCC_SPMI_AHB_CFG_RCGR_IN)
#define HWIO_GCC_SPMI_AHB_CFG_RCGR_SRC_SEL_BMSK                                                      0x700
#define HWIO_GCC_SPMI_AHB_CFG_RCGR_SRC_SEL_SHFT                                                        0x8
#define HWIO_GCC_SPMI_AHB_CFG_RCGR_SRC_DIV_BMSK                                                       0x1f
#define HWIO_GCC_SPMI_AHB_CFG_RCGR_SRC_DIV_SHFT                                                        0x0

#define HWIO_GCC_SPMI_AHB_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0002e03c)
#define HWIO_GCC_SPMI_AHB_CBCR_RMSK                                                             0x80007ff1
#define HWIO_GCC_SPMI_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SPMI_AHB_CBCR_ADDR, HWIO_GCC_SPMI_AHB_CBCR_RMSK)
#define HWIO_GCC_SPMI_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPMI_AHB_CBCR_ADDR, m)
#define HWIO_GCC_SPMI_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SPMI_AHB_CBCR_ADDR,v)
#define HWIO_GCC_SPMI_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPMI_AHB_CBCR_ADDR,m,v,HWIO_GCC_SPMI_AHB_CBCR_IN)
#define HWIO_GCC_SPMI_AHB_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_SPMI_AHB_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_SPMI_AHB_CBCR_FORCE_MEM_CORE_ON_BMSK                                               0x4000
#define HWIO_GCC_SPMI_AHB_CBCR_FORCE_MEM_CORE_ON_SHFT                                                  0xe
#define HWIO_GCC_SPMI_AHB_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                             0x2000
#define HWIO_GCC_SPMI_AHB_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                0xd
#define HWIO_GCC_SPMI_AHB_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                            0x1000
#define HWIO_GCC_SPMI_AHB_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                               0xc
#define HWIO_GCC_SPMI_AHB_CBCR_WAKEUP_BMSK                                                           0xf00
#define HWIO_GCC_SPMI_AHB_CBCR_WAKEUP_SHFT                                                             0x8
#define HWIO_GCC_SPMI_AHB_CBCR_SLEEP_BMSK                                                             0xf0
#define HWIO_GCC_SPMI_AHB_CBCR_SLEEP_SHFT                                                              0x4
#define HWIO_GCC_SPMI_AHB_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_GCC_SPMI_AHB_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_GCC_SPDM_BCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0002f000)
#define HWIO_GCC_SPDM_BCR_RMSK                                                                         0x1
#define HWIO_GCC_SPDM_BCR_IN          \
        in_dword_masked(HWIO_GCC_SPDM_BCR_ADDR, HWIO_GCC_SPDM_BCR_RMSK)
#define HWIO_GCC_SPDM_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPDM_BCR_ADDR, m)
#define HWIO_GCC_SPDM_BCR_OUT(v)      \
        out_dword(HWIO_GCC_SPDM_BCR_ADDR,v)
#define HWIO_GCC_SPDM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPDM_BCR_ADDR,m,v,HWIO_GCC_SPDM_BCR_IN)
#define HWIO_GCC_SPDM_BCR_BLK_ARES_BMSK                                                                0x1
#define HWIO_GCC_SPDM_BCR_BLK_ARES_SHFT                                                                0x0

#define HWIO_GCC_SPDM_CFG_AHB_CBCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0002f004)
#define HWIO_GCC_SPDM_CFG_AHB_CBCR_RMSK                                                         0xf0008001
#define HWIO_GCC_SPDM_CFG_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SPDM_CFG_AHB_CBCR_ADDR, HWIO_GCC_SPDM_CFG_AHB_CBCR_RMSK)
#define HWIO_GCC_SPDM_CFG_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPDM_CFG_AHB_CBCR_ADDR, m)
#define HWIO_GCC_SPDM_CFG_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SPDM_CFG_AHB_CBCR_ADDR,v)
#define HWIO_GCC_SPDM_CFG_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPDM_CFG_AHB_CBCR_ADDR,m,v,HWIO_GCC_SPDM_CFG_AHB_CBCR_IN)
#define HWIO_GCC_SPDM_CFG_AHB_CBCR_CLK_OFF_BMSK                                                 0x80000000
#define HWIO_GCC_SPDM_CFG_AHB_CBCR_CLK_OFF_SHFT                                                       0x1f
#define HWIO_GCC_SPDM_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                0x70000000
#define HWIO_GCC_SPDM_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                      0x1c
#define HWIO_GCC_SPDM_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                        0x8000
#define HWIO_GCC_SPDM_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                           0xf
#define HWIO_GCC_SPDM_CFG_AHB_CBCR_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_GCC_SPDM_CFG_AHB_CBCR_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_GCC_SPDM_MSTR_AHB_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0002f008)
#define HWIO_GCC_SPDM_MSTR_AHB_CBCR_RMSK                                                        0x80000001
#define HWIO_GCC_SPDM_MSTR_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SPDM_MSTR_AHB_CBCR_ADDR, HWIO_GCC_SPDM_MSTR_AHB_CBCR_RMSK)
#define HWIO_GCC_SPDM_MSTR_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPDM_MSTR_AHB_CBCR_ADDR, m)
#define HWIO_GCC_SPDM_MSTR_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SPDM_MSTR_AHB_CBCR_ADDR,v)
#define HWIO_GCC_SPDM_MSTR_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPDM_MSTR_AHB_CBCR_ADDR,m,v,HWIO_GCC_SPDM_MSTR_AHB_CBCR_IN)
#define HWIO_GCC_SPDM_MSTR_AHB_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_SPDM_MSTR_AHB_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_SPDM_MSTR_AHB_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_SPDM_MSTR_AHB_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_SPDM_FF_CBCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0002f00c)
#define HWIO_GCC_SPDM_FF_CBCR_RMSK                                                              0x80000001
#define HWIO_GCC_SPDM_FF_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SPDM_FF_CBCR_ADDR, HWIO_GCC_SPDM_FF_CBCR_RMSK)
#define HWIO_GCC_SPDM_FF_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPDM_FF_CBCR_ADDR, m)
#define HWIO_GCC_SPDM_FF_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SPDM_FF_CBCR_ADDR,v)
#define HWIO_GCC_SPDM_FF_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPDM_FF_CBCR_ADDR,m,v,HWIO_GCC_SPDM_FF_CBCR_IN)
#define HWIO_GCC_SPDM_FF_CBCR_CLK_OFF_BMSK                                                      0x80000000
#define HWIO_GCC_SPDM_FF_CBCR_CLK_OFF_SHFT                                                            0x1f
#define HWIO_GCC_SPDM_FF_CBCR_CLK_ENABLE_BMSK                                                          0x1
#define HWIO_GCC_SPDM_FF_CBCR_CLK_ENABLE_SHFT                                                          0x0

#define HWIO_GCC_SPDM_BIMC_CY_CBCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0002f010)
#define HWIO_GCC_SPDM_BIMC_CY_CBCR_RMSK                                                         0x80000001
#define HWIO_GCC_SPDM_BIMC_CY_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SPDM_BIMC_CY_CBCR_ADDR, HWIO_GCC_SPDM_BIMC_CY_CBCR_RMSK)
#define HWIO_GCC_SPDM_BIMC_CY_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPDM_BIMC_CY_CBCR_ADDR, m)
#define HWIO_GCC_SPDM_BIMC_CY_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SPDM_BIMC_CY_CBCR_ADDR,v)
#define HWIO_GCC_SPDM_BIMC_CY_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPDM_BIMC_CY_CBCR_ADDR,m,v,HWIO_GCC_SPDM_BIMC_CY_CBCR_IN)
#define HWIO_GCC_SPDM_BIMC_CY_CBCR_CLK_OFF_BMSK                                                 0x80000000
#define HWIO_GCC_SPDM_BIMC_CY_CBCR_CLK_OFF_SHFT                                                       0x1f
#define HWIO_GCC_SPDM_BIMC_CY_CBCR_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_GCC_SPDM_BIMC_CY_CBCR_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_GCC_SPDM_SNOC_CY_CBCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0002f014)
#define HWIO_GCC_SPDM_SNOC_CY_CBCR_RMSK                                                         0x80000001
#define HWIO_GCC_SPDM_SNOC_CY_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SPDM_SNOC_CY_CBCR_ADDR, HWIO_GCC_SPDM_SNOC_CY_CBCR_RMSK)
#define HWIO_GCC_SPDM_SNOC_CY_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPDM_SNOC_CY_CBCR_ADDR, m)
#define HWIO_GCC_SPDM_SNOC_CY_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SPDM_SNOC_CY_CBCR_ADDR,v)
#define HWIO_GCC_SPDM_SNOC_CY_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPDM_SNOC_CY_CBCR_ADDR,m,v,HWIO_GCC_SPDM_SNOC_CY_CBCR_IN)
#define HWIO_GCC_SPDM_SNOC_CY_CBCR_CLK_OFF_BMSK                                                 0x80000000
#define HWIO_GCC_SPDM_SNOC_CY_CBCR_CLK_OFF_SHFT                                                       0x1f
#define HWIO_GCC_SPDM_SNOC_CY_CBCR_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_GCC_SPDM_SNOC_CY_CBCR_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_GCC_SPDM_DEBUG_CY_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0002f018)
#define HWIO_GCC_SPDM_DEBUG_CY_CBCR_RMSK                                                        0x80000001
#define HWIO_GCC_SPDM_DEBUG_CY_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SPDM_DEBUG_CY_CBCR_ADDR, HWIO_GCC_SPDM_DEBUG_CY_CBCR_RMSK)
#define HWIO_GCC_SPDM_DEBUG_CY_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPDM_DEBUG_CY_CBCR_ADDR, m)
#define HWIO_GCC_SPDM_DEBUG_CY_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SPDM_DEBUG_CY_CBCR_ADDR,v)
#define HWIO_GCC_SPDM_DEBUG_CY_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPDM_DEBUG_CY_CBCR_ADDR,m,v,HWIO_GCC_SPDM_DEBUG_CY_CBCR_IN)
#define HWIO_GCC_SPDM_DEBUG_CY_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_SPDM_DEBUG_CY_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_SPDM_DEBUG_CY_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_SPDM_DEBUG_CY_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_SPDM_PCNOC_CY_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0002f01c)
#define HWIO_GCC_SPDM_PCNOC_CY_CBCR_RMSK                                                        0x80000001
#define HWIO_GCC_SPDM_PCNOC_CY_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SPDM_PCNOC_CY_CBCR_ADDR, HWIO_GCC_SPDM_PCNOC_CY_CBCR_RMSK)
#define HWIO_GCC_SPDM_PCNOC_CY_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPDM_PCNOC_CY_CBCR_ADDR, m)
#define HWIO_GCC_SPDM_PCNOC_CY_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SPDM_PCNOC_CY_CBCR_ADDR,v)
#define HWIO_GCC_SPDM_PCNOC_CY_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPDM_PCNOC_CY_CBCR_ADDR,m,v,HWIO_GCC_SPDM_PCNOC_CY_CBCR_IN)
#define HWIO_GCC_SPDM_PCNOC_CY_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_SPDM_PCNOC_CY_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_SPDM_PCNOC_CY_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_SPDM_PCNOC_CY_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_SPDM_RPM_CY_CBCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x0002f020)
#define HWIO_GCC_SPDM_RPM_CY_CBCR_RMSK                                                          0x80000001
#define HWIO_GCC_SPDM_RPM_CY_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SPDM_RPM_CY_CBCR_ADDR, HWIO_GCC_SPDM_RPM_CY_CBCR_RMSK)
#define HWIO_GCC_SPDM_RPM_CY_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPDM_RPM_CY_CBCR_ADDR, m)
#define HWIO_GCC_SPDM_RPM_CY_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SPDM_RPM_CY_CBCR_ADDR,v)
#define HWIO_GCC_SPDM_RPM_CY_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPDM_RPM_CY_CBCR_ADDR,m,v,HWIO_GCC_SPDM_RPM_CY_CBCR_IN)
#define HWIO_GCC_SPDM_RPM_CY_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_SPDM_RPM_CY_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_GCC_SPDM_RPM_CY_CBCR_CLK_ENABLE_BMSK                                                      0x1
#define HWIO_GCC_SPDM_RPM_CY_CBCR_CLK_ENABLE_SHFT                                                      0x0

#define HWIO_GCC_CRYPTO_BCR_ADDR                                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00016000)
#define HWIO_GCC_CRYPTO_BCR_RMSK                                                                       0x1
#define HWIO_GCC_CRYPTO_BCR_IN          \
        in_dword_masked(HWIO_GCC_CRYPTO_BCR_ADDR, HWIO_GCC_CRYPTO_BCR_RMSK)
#define HWIO_GCC_CRYPTO_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CRYPTO_BCR_ADDR, m)
#define HWIO_GCC_CRYPTO_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CRYPTO_BCR_ADDR,v)
#define HWIO_GCC_CRYPTO_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CRYPTO_BCR_ADDR,m,v,HWIO_GCC_CRYPTO_BCR_IN)
#define HWIO_GCC_CRYPTO_BCR_BLK_ARES_BMSK                                                              0x1
#define HWIO_GCC_CRYPTO_BCR_BLK_ARES_SHFT                                                              0x0

#define HWIO_GCC_CRYPTO_CMD_RCGR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00016004)
#define HWIO_GCC_CRYPTO_CMD_RCGR_RMSK                                                           0x80000013
#define HWIO_GCC_CRYPTO_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_CRYPTO_CMD_RCGR_ADDR, HWIO_GCC_CRYPTO_CMD_RCGR_RMSK)
#define HWIO_GCC_CRYPTO_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_CRYPTO_CMD_RCGR_ADDR, m)
#define HWIO_GCC_CRYPTO_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_CRYPTO_CMD_RCGR_ADDR,v)
#define HWIO_GCC_CRYPTO_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CRYPTO_CMD_RCGR_ADDR,m,v,HWIO_GCC_CRYPTO_CMD_RCGR_IN)
#define HWIO_GCC_CRYPTO_CMD_RCGR_ROOT_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_CRYPTO_CMD_RCGR_ROOT_OFF_SHFT                                                        0x1f
#define HWIO_GCC_CRYPTO_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                  0x10
#define HWIO_GCC_CRYPTO_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                   0x4
#define HWIO_GCC_CRYPTO_CMD_RCGR_ROOT_EN_BMSK                                                          0x2
#define HWIO_GCC_CRYPTO_CMD_RCGR_ROOT_EN_SHFT                                                          0x1
#define HWIO_GCC_CRYPTO_CMD_RCGR_UPDATE_BMSK                                                           0x1
#define HWIO_GCC_CRYPTO_CMD_RCGR_UPDATE_SHFT                                                           0x0

#define HWIO_GCC_CRYPTO_CFG_RCGR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00016008)
#define HWIO_GCC_CRYPTO_CFG_RCGR_RMSK                                                                0x71f
#define HWIO_GCC_CRYPTO_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_CRYPTO_CFG_RCGR_ADDR, HWIO_GCC_CRYPTO_CFG_RCGR_RMSK)
#define HWIO_GCC_CRYPTO_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_CRYPTO_CFG_RCGR_ADDR, m)
#define HWIO_GCC_CRYPTO_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_CRYPTO_CFG_RCGR_ADDR,v)
#define HWIO_GCC_CRYPTO_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CRYPTO_CFG_RCGR_ADDR,m,v,HWIO_GCC_CRYPTO_CFG_RCGR_IN)
#define HWIO_GCC_CRYPTO_CFG_RCGR_SRC_SEL_BMSK                                                        0x700
#define HWIO_GCC_CRYPTO_CFG_RCGR_SRC_SEL_SHFT                                                          0x8
#define HWIO_GCC_CRYPTO_CFG_RCGR_SRC_DIV_BMSK                                                         0x1f
#define HWIO_GCC_CRYPTO_CFG_RCGR_SRC_DIV_SHFT                                                          0x0

#define HWIO_GCC_CRYPTO_CBCR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0001601c)
#define HWIO_GCC_CRYPTO_CBCR_RMSK                                                               0x80007ff0
#define HWIO_GCC_CRYPTO_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CRYPTO_CBCR_ADDR, HWIO_GCC_CRYPTO_CBCR_RMSK)
#define HWIO_GCC_CRYPTO_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CRYPTO_CBCR_ADDR, m)
#define HWIO_GCC_CRYPTO_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CRYPTO_CBCR_ADDR,v)
#define HWIO_GCC_CRYPTO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CRYPTO_CBCR_ADDR,m,v,HWIO_GCC_CRYPTO_CBCR_IN)
#define HWIO_GCC_CRYPTO_CBCR_CLK_OFF_BMSK                                                       0x80000000
#define HWIO_GCC_CRYPTO_CBCR_CLK_OFF_SHFT                                                             0x1f
#define HWIO_GCC_CRYPTO_CBCR_FORCE_MEM_CORE_ON_BMSK                                                 0x4000
#define HWIO_GCC_CRYPTO_CBCR_FORCE_MEM_CORE_ON_SHFT                                                    0xe
#define HWIO_GCC_CRYPTO_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                               0x2000
#define HWIO_GCC_CRYPTO_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                  0xd
#define HWIO_GCC_CRYPTO_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                              0x1000
#define HWIO_GCC_CRYPTO_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                                 0xc
#define HWIO_GCC_CRYPTO_CBCR_WAKEUP_BMSK                                                             0xf00
#define HWIO_GCC_CRYPTO_CBCR_WAKEUP_SHFT                                                               0x8
#define HWIO_GCC_CRYPTO_CBCR_SLEEP_BMSK                                                               0xf0
#define HWIO_GCC_CRYPTO_CBCR_SLEEP_SHFT                                                                0x4

#define HWIO_GCC_CRYPTO_AXI_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00016020)
#define HWIO_GCC_CRYPTO_AXI_CBCR_RMSK                                                           0x80000000
#define HWIO_GCC_CRYPTO_AXI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CRYPTO_AXI_CBCR_ADDR, HWIO_GCC_CRYPTO_AXI_CBCR_RMSK)
#define HWIO_GCC_CRYPTO_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CRYPTO_AXI_CBCR_ADDR, m)
#define HWIO_GCC_CRYPTO_AXI_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_CRYPTO_AXI_CBCR_CLK_OFF_SHFT                                                         0x1f

#define HWIO_GCC_CRYPTO_AHB_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00016024)
#define HWIO_GCC_CRYPTO_AHB_CBCR_RMSK                                                           0xf0008000
#define HWIO_GCC_CRYPTO_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CRYPTO_AHB_CBCR_ADDR, HWIO_GCC_CRYPTO_AHB_CBCR_RMSK)
#define HWIO_GCC_CRYPTO_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CRYPTO_AHB_CBCR_ADDR, m)
#define HWIO_GCC_CRYPTO_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CRYPTO_AHB_CBCR_ADDR,v)
#define HWIO_GCC_CRYPTO_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CRYPTO_AHB_CBCR_ADDR,m,v,HWIO_GCC_CRYPTO_AHB_CBCR_IN)
#define HWIO_GCC_CRYPTO_AHB_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_CRYPTO_AHB_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_CRYPTO_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                  0x70000000
#define HWIO_GCC_CRYPTO_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                        0x1c
#define HWIO_GCC_CRYPTO_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                          0x8000
#define HWIO_GCC_CRYPTO_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                             0xf

#define HWIO_GCC_GCC_AHB_CBCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00030014)
#define HWIO_GCC_GCC_AHB_CBCR_RMSK                                                              0x80000001
#define HWIO_GCC_GCC_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_GCC_AHB_CBCR_ADDR, HWIO_GCC_GCC_AHB_CBCR_RMSK)
#define HWIO_GCC_GCC_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_GCC_AHB_CBCR_ADDR, m)
#define HWIO_GCC_GCC_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_GCC_AHB_CBCR_ADDR,v)
#define HWIO_GCC_GCC_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GCC_AHB_CBCR_ADDR,m,v,HWIO_GCC_GCC_AHB_CBCR_IN)
#define HWIO_GCC_GCC_AHB_CBCR_CLK_OFF_BMSK                                                      0x80000000
#define HWIO_GCC_GCC_AHB_CBCR_CLK_OFF_SHFT                                                            0x1f
#define HWIO_GCC_GCC_AHB_CBCR_CLK_ENABLE_BMSK                                                          0x1
#define HWIO_GCC_GCC_AHB_CBCR_CLK_ENABLE_SHFT                                                          0x0

#define HWIO_GCC_GCC_XO_CMD_RCGR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00030018)
#define HWIO_GCC_GCC_XO_CMD_RCGR_RMSK                                                           0x80000002
#define HWIO_GCC_GCC_XO_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_GCC_XO_CMD_RCGR_ADDR, HWIO_GCC_GCC_XO_CMD_RCGR_RMSK)
#define HWIO_GCC_GCC_XO_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_GCC_XO_CMD_RCGR_ADDR, m)
#define HWIO_GCC_GCC_XO_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_GCC_XO_CMD_RCGR_ADDR,v)
#define HWIO_GCC_GCC_XO_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GCC_XO_CMD_RCGR_ADDR,m,v,HWIO_GCC_GCC_XO_CMD_RCGR_IN)
#define HWIO_GCC_GCC_XO_CMD_RCGR_ROOT_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_GCC_XO_CMD_RCGR_ROOT_OFF_SHFT                                                        0x1f
#define HWIO_GCC_GCC_XO_CMD_RCGR_ROOT_EN_BMSK                                                          0x2
#define HWIO_GCC_GCC_XO_CMD_RCGR_ROOT_EN_SHFT                                                          0x1

#define HWIO_GCC_GCC_XO_CBCR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00030030)
#define HWIO_GCC_GCC_XO_CBCR_RMSK                                                               0x80000001
#define HWIO_GCC_GCC_XO_CBCR_IN          \
        in_dword_masked(HWIO_GCC_GCC_XO_CBCR_ADDR, HWIO_GCC_GCC_XO_CBCR_RMSK)
#define HWIO_GCC_GCC_XO_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_GCC_XO_CBCR_ADDR, m)
#define HWIO_GCC_GCC_XO_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_GCC_XO_CBCR_ADDR,v)
#define HWIO_GCC_GCC_XO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GCC_XO_CBCR_ADDR,m,v,HWIO_GCC_GCC_XO_CBCR_IN)
#define HWIO_GCC_GCC_XO_CBCR_CLK_OFF_BMSK                                                       0x80000000
#define HWIO_GCC_GCC_XO_CBCR_CLK_OFF_SHFT                                                             0x1f
#define HWIO_GCC_GCC_XO_CBCR_CLK_ENABLE_BMSK                                                           0x1
#define HWIO_GCC_GCC_XO_CBCR_CLK_ENABLE_SHFT                                                           0x0

#define HWIO_GCC_GCC_XO_DIV4_CBCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00030034)
#define HWIO_GCC_GCC_XO_DIV4_CBCR_RMSK                                                          0x80000001
#define HWIO_GCC_GCC_XO_DIV4_CBCR_IN          \
        in_dword_masked(HWIO_GCC_GCC_XO_DIV4_CBCR_ADDR, HWIO_GCC_GCC_XO_DIV4_CBCR_RMSK)
#define HWIO_GCC_GCC_XO_DIV4_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_GCC_XO_DIV4_CBCR_ADDR, m)
#define HWIO_GCC_GCC_XO_DIV4_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_GCC_XO_DIV4_CBCR_ADDR,v)
#define HWIO_GCC_GCC_XO_DIV4_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GCC_XO_DIV4_CBCR_ADDR,m,v,HWIO_GCC_GCC_XO_DIV4_CBCR_IN)
#define HWIO_GCC_GCC_XO_DIV4_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_GCC_XO_DIV4_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_GCC_GCC_XO_DIV4_CBCR_CLK_ENABLE_BMSK                                                      0x1
#define HWIO_GCC_GCC_XO_DIV4_CBCR_CLK_ENABLE_SHFT                                                      0x0

#define HWIO_GCC_GCC_IM_SLEEP_CBCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00030038)
#define HWIO_GCC_GCC_IM_SLEEP_CBCR_RMSK                                                         0x80000001
#define HWIO_GCC_GCC_IM_SLEEP_CBCR_IN          \
        in_dword_masked(HWIO_GCC_GCC_IM_SLEEP_CBCR_ADDR, HWIO_GCC_GCC_IM_SLEEP_CBCR_RMSK)
#define HWIO_GCC_GCC_IM_SLEEP_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_GCC_IM_SLEEP_CBCR_ADDR, m)
#define HWIO_GCC_GCC_IM_SLEEP_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_GCC_IM_SLEEP_CBCR_ADDR,v)
#define HWIO_GCC_GCC_IM_SLEEP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GCC_IM_SLEEP_CBCR_ADDR,m,v,HWIO_GCC_GCC_IM_SLEEP_CBCR_IN)
#define HWIO_GCC_GCC_IM_SLEEP_CBCR_CLK_OFF_BMSK                                                 0x80000000
#define HWIO_GCC_GCC_IM_SLEEP_CBCR_CLK_OFF_SHFT                                                       0x1f
#define HWIO_GCC_GCC_IM_SLEEP_CBCR_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_GCC_GCC_IM_SLEEP_CBCR_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_GCC_BIMC_BCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00031000)
#define HWIO_GCC_BIMC_BCR_RMSK                                                                         0x1
#define HWIO_GCC_BIMC_BCR_IN          \
        in_dword_masked(HWIO_GCC_BIMC_BCR_ADDR, HWIO_GCC_BIMC_BCR_RMSK)
#define HWIO_GCC_BIMC_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_BCR_ADDR, m)
#define HWIO_GCC_BIMC_BCR_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_BCR_ADDR,v)
#define HWIO_GCC_BIMC_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_BCR_ADDR,m,v,HWIO_GCC_BIMC_BCR_IN)
#define HWIO_GCC_BIMC_BCR_BLK_ARES_BMSK                                                                0x1
#define HWIO_GCC_BIMC_BCR_BLK_ARES_SHFT                                                                0x0

#define HWIO_GCC_BIMC_GDSCR_ADDR                                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00031004)
#define HWIO_GCC_BIMC_GDSCR_RMSK                                                                0xf8ffffff
#define HWIO_GCC_BIMC_GDSCR_IN          \
        in_dword_masked(HWIO_GCC_BIMC_GDSCR_ADDR, HWIO_GCC_BIMC_GDSCR_RMSK)
#define HWIO_GCC_BIMC_GDSCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_GDSCR_ADDR, m)
#define HWIO_GCC_BIMC_GDSCR_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_GDSCR_ADDR,v)
#define HWIO_GCC_BIMC_GDSCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_GDSCR_ADDR,m,v,HWIO_GCC_BIMC_GDSCR_IN)
#define HWIO_GCC_BIMC_GDSCR_PWR_ON_BMSK                                                         0x80000000
#define HWIO_GCC_BIMC_GDSCR_PWR_ON_SHFT                                                               0x1f
#define HWIO_GCC_BIMC_GDSCR_GDSC_STATE_BMSK                                                     0x78000000
#define HWIO_GCC_BIMC_GDSCR_GDSC_STATE_SHFT                                                           0x1b
#define HWIO_GCC_BIMC_GDSCR_EN_REST_WAIT_BMSK                                                     0xf00000
#define HWIO_GCC_BIMC_GDSCR_EN_REST_WAIT_SHFT                                                         0x14
#define HWIO_GCC_BIMC_GDSCR_EN_FEW_WAIT_BMSK                                                       0xf0000
#define HWIO_GCC_BIMC_GDSCR_EN_FEW_WAIT_SHFT                                                          0x10
#define HWIO_GCC_BIMC_GDSCR_CLK_DIS_WAIT_BMSK                                                       0xf000
#define HWIO_GCC_BIMC_GDSCR_CLK_DIS_WAIT_SHFT                                                          0xc
#define HWIO_GCC_BIMC_GDSCR_RETAIN_FF_ENABLE_BMSK                                                    0x800
#define HWIO_GCC_BIMC_GDSCR_RETAIN_FF_ENABLE_SHFT                                                      0xb
#define HWIO_GCC_BIMC_GDSCR_RESTORE_BMSK                                                             0x400
#define HWIO_GCC_BIMC_GDSCR_RESTORE_SHFT                                                               0xa
#define HWIO_GCC_BIMC_GDSCR_SAVE_BMSK                                                                0x200
#define HWIO_GCC_BIMC_GDSCR_SAVE_SHFT                                                                  0x9
#define HWIO_GCC_BIMC_GDSCR_RETAIN_BMSK                                                              0x100
#define HWIO_GCC_BIMC_GDSCR_RETAIN_SHFT                                                                0x8
#define HWIO_GCC_BIMC_GDSCR_EN_REST_BMSK                                                              0x80
#define HWIO_GCC_BIMC_GDSCR_EN_REST_SHFT                                                               0x7
#define HWIO_GCC_BIMC_GDSCR_EN_FEW_BMSK                                                               0x40
#define HWIO_GCC_BIMC_GDSCR_EN_FEW_SHFT                                                                0x6
#define HWIO_GCC_BIMC_GDSCR_CLAMP_IO_BMSK                                                             0x20
#define HWIO_GCC_BIMC_GDSCR_CLAMP_IO_SHFT                                                              0x5
#define HWIO_GCC_BIMC_GDSCR_CLK_DISABLE_BMSK                                                          0x10
#define HWIO_GCC_BIMC_GDSCR_CLK_DISABLE_SHFT                                                           0x4
#define HWIO_GCC_BIMC_GDSCR_PD_ARES_BMSK                                                               0x8
#define HWIO_GCC_BIMC_GDSCR_PD_ARES_SHFT                                                               0x3
#define HWIO_GCC_BIMC_GDSCR_SW_OVERRIDE_BMSK                                                           0x4
#define HWIO_GCC_BIMC_GDSCR_SW_OVERRIDE_SHFT                                                           0x2
#define HWIO_GCC_BIMC_GDSCR_HW_CONTROL_BMSK                                                            0x2
#define HWIO_GCC_BIMC_GDSCR_HW_CONTROL_SHFT                                                            0x1
#define HWIO_GCC_BIMC_GDSCR_SW_COLLAPSE_BMSK                                                           0x1
#define HWIO_GCC_BIMC_GDSCR_SW_COLLAPSE_SHFT                                                           0x0

#define HWIO_GCC_BIMC_DDR_XO_CMD_RCGR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00032000)
#define HWIO_GCC_BIMC_DDR_XO_CMD_RCGR_RMSK                                                      0x80000002
#define HWIO_GCC_BIMC_DDR_XO_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BIMC_DDR_XO_CMD_RCGR_ADDR, HWIO_GCC_BIMC_DDR_XO_CMD_RCGR_RMSK)
#define HWIO_GCC_BIMC_DDR_XO_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_DDR_XO_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BIMC_DDR_XO_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_DDR_XO_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BIMC_DDR_XO_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_DDR_XO_CMD_RCGR_ADDR,m,v,HWIO_GCC_BIMC_DDR_XO_CMD_RCGR_IN)
#define HWIO_GCC_BIMC_DDR_XO_CMD_RCGR_ROOT_OFF_BMSK                                             0x80000000
#define HWIO_GCC_BIMC_DDR_XO_CMD_RCGR_ROOT_OFF_SHFT                                                   0x1f
#define HWIO_GCC_BIMC_DDR_XO_CMD_RCGR_ROOT_EN_BMSK                                                     0x2
#define HWIO_GCC_BIMC_DDR_XO_CMD_RCGR_ROOT_EN_SHFT                                                     0x1

#define HWIO_GCC_BIMC_XO_CBCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00031008)
#define HWIO_GCC_BIMC_XO_CBCR_RMSK                                                              0x80000001
#define HWIO_GCC_BIMC_XO_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BIMC_XO_CBCR_ADDR, HWIO_GCC_BIMC_XO_CBCR_RMSK)
#define HWIO_GCC_BIMC_XO_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_XO_CBCR_ADDR, m)
#define HWIO_GCC_BIMC_XO_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_XO_CBCR_ADDR,v)
#define HWIO_GCC_BIMC_XO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_XO_CBCR_ADDR,m,v,HWIO_GCC_BIMC_XO_CBCR_IN)
#define HWIO_GCC_BIMC_XO_CBCR_CLK_OFF_BMSK                                                      0x80000000
#define HWIO_GCC_BIMC_XO_CBCR_CLK_OFF_SHFT                                                            0x1f
#define HWIO_GCC_BIMC_XO_CBCR_CLK_ENABLE_BMSK                                                          0x1
#define HWIO_GCC_BIMC_XO_CBCR_CLK_ENABLE_SHFT                                                          0x0

#define HWIO_GCC_BIMC_CFG_AHB_CBCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0003100c)
#define HWIO_GCC_BIMC_CFG_AHB_CBCR_RMSK                                                         0xf0008001
#define HWIO_GCC_BIMC_CFG_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BIMC_CFG_AHB_CBCR_ADDR, HWIO_GCC_BIMC_CFG_AHB_CBCR_RMSK)
#define HWIO_GCC_BIMC_CFG_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_CFG_AHB_CBCR_ADDR, m)
#define HWIO_GCC_BIMC_CFG_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_CFG_AHB_CBCR_ADDR,v)
#define HWIO_GCC_BIMC_CFG_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_CFG_AHB_CBCR_ADDR,m,v,HWIO_GCC_BIMC_CFG_AHB_CBCR_IN)
#define HWIO_GCC_BIMC_CFG_AHB_CBCR_CLK_OFF_BMSK                                                 0x80000000
#define HWIO_GCC_BIMC_CFG_AHB_CBCR_CLK_OFF_SHFT                                                       0x1f
#define HWIO_GCC_BIMC_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                0x70000000
#define HWIO_GCC_BIMC_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                      0x1c
#define HWIO_GCC_BIMC_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                        0x8000
#define HWIO_GCC_BIMC_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                           0xf
#define HWIO_GCC_BIMC_CFG_AHB_CBCR_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_GCC_BIMC_CFG_AHB_CBCR_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_GCC_BIMC_SLEEP_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00031010)
#define HWIO_GCC_BIMC_SLEEP_CBCR_RMSK                                                           0x80000001
#define HWIO_GCC_BIMC_SLEEP_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BIMC_SLEEP_CBCR_ADDR, HWIO_GCC_BIMC_SLEEP_CBCR_RMSK)
#define HWIO_GCC_BIMC_SLEEP_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_SLEEP_CBCR_ADDR, m)
#define HWIO_GCC_BIMC_SLEEP_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_SLEEP_CBCR_ADDR,v)
#define HWIO_GCC_BIMC_SLEEP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_SLEEP_CBCR_ADDR,m,v,HWIO_GCC_BIMC_SLEEP_CBCR_IN)
#define HWIO_GCC_BIMC_SLEEP_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_BIMC_SLEEP_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_BIMC_SLEEP_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_BIMC_SLEEP_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00031014)
#define HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_RMSK                                                      0xf0008001
#define HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_ADDR, HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_RMSK)
#define HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_ADDR, m)
#define HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_ADDR,v)
#define HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_ADDR,m,v,HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_IN)
#define HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                             0x70000000
#define HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                   0x1c
#define HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                     0x8000
#define HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                        0xf
#define HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_GCC_BIMC_DDR_CMD_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00032004)
#define HWIO_GCC_BIMC_DDR_CMD_RCGR_RMSK                                                         0x80000013
#define HWIO_GCC_BIMC_DDR_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BIMC_DDR_CMD_RCGR_ADDR, HWIO_GCC_BIMC_DDR_CMD_RCGR_RMSK)
#define HWIO_GCC_BIMC_DDR_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_DDR_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BIMC_DDR_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_DDR_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BIMC_DDR_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_DDR_CMD_RCGR_ADDR,m,v,HWIO_GCC_BIMC_DDR_CMD_RCGR_IN)
#define HWIO_GCC_BIMC_DDR_CMD_RCGR_ROOT_OFF_BMSK                                                0x80000000
#define HWIO_GCC_BIMC_DDR_CMD_RCGR_ROOT_OFF_SHFT                                                      0x1f
#define HWIO_GCC_BIMC_DDR_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                0x10
#define HWIO_GCC_BIMC_DDR_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                 0x4
#define HWIO_GCC_BIMC_DDR_CMD_RCGR_ROOT_EN_BMSK                                                        0x2
#define HWIO_GCC_BIMC_DDR_CMD_RCGR_ROOT_EN_SHFT                                                        0x1
#define HWIO_GCC_BIMC_DDR_CMD_RCGR_UPDATE_BMSK                                                         0x1
#define HWIO_GCC_BIMC_DDR_CMD_RCGR_UPDATE_SHFT                                                         0x0

#define HWIO_GCC_BIMC_DDR_CFG_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00032008)
#define HWIO_GCC_BIMC_DDR_CFG_RCGR_RMSK                                                              0x71f
#define HWIO_GCC_BIMC_DDR_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BIMC_DDR_CFG_RCGR_ADDR, HWIO_GCC_BIMC_DDR_CFG_RCGR_RMSK)
#define HWIO_GCC_BIMC_DDR_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_DDR_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BIMC_DDR_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_DDR_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BIMC_DDR_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_DDR_CFG_RCGR_ADDR,m,v,HWIO_GCC_BIMC_DDR_CFG_RCGR_IN)
#define HWIO_GCC_BIMC_DDR_CFG_RCGR_SRC_SEL_BMSK                                                      0x700
#define HWIO_GCC_BIMC_DDR_CFG_RCGR_SRC_SEL_SHFT                                                        0x8
#define HWIO_GCC_BIMC_DDR_CFG_RCGR_SRC_DIV_BMSK                                                       0x1f
#define HWIO_GCC_BIMC_DDR_CFG_RCGR_SRC_DIV_SHFT                                                        0x0

#define HWIO_GCC_BIMC_MISC_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00031018)
#define HWIO_GCC_BIMC_MISC_RMSK                                                                   0x3f0f00
#define HWIO_GCC_BIMC_MISC_IN          \
        in_dword_masked(HWIO_GCC_BIMC_MISC_ADDR, HWIO_GCC_BIMC_MISC_RMSK)
#define HWIO_GCC_BIMC_MISC_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_MISC_ADDR, m)
#define HWIO_GCC_BIMC_MISC_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_MISC_ADDR,v)
#define HWIO_GCC_BIMC_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_MISC_ADDR,m,v,HWIO_GCC_BIMC_MISC_IN)
#define HWIO_GCC_BIMC_MISC_BIMC_DDR_FRQSW_FSM_STATUS_BMSK                                         0x3f0000
#define HWIO_GCC_BIMC_MISC_BIMC_DDR_FRQSW_FSM_STATUS_SHFT                                             0x10
#define HWIO_GCC_BIMC_MISC_BIMC_FSM_CLK_GATE_DIS_BMSK                                                0x800
#define HWIO_GCC_BIMC_MISC_BIMC_FSM_CLK_GATE_DIS_SHFT                                                  0xb
#define HWIO_GCC_BIMC_MISC_BIMC_DDR_LEGACY_2X_MODE_EN_BMSK                                           0x400
#define HWIO_GCC_BIMC_MISC_BIMC_DDR_LEGACY_2X_MODE_EN_SHFT                                             0xa
#define HWIO_GCC_BIMC_MISC_BIMC_FSM_DIS_DDR_UPDATE_BMSK                                              0x200
#define HWIO_GCC_BIMC_MISC_BIMC_FSM_DIS_DDR_UPDATE_SHFT                                                0x9
#define HWIO_GCC_BIMC_MISC_BIMC_FRQSW_FSM_DIS_BMSK                                                   0x100
#define HWIO_GCC_BIMC_MISC_BIMC_FRQSW_FSM_DIS_SHFT                                                     0x8

#define HWIO_GCC_BIMC_CBCR_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x0003101c)
#define HWIO_GCC_BIMC_CBCR_RMSK                                                                 0x80000001
#define HWIO_GCC_BIMC_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BIMC_CBCR_ADDR, HWIO_GCC_BIMC_CBCR_RMSK)
#define HWIO_GCC_BIMC_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_CBCR_ADDR, m)
#define HWIO_GCC_BIMC_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_CBCR_ADDR,v)
#define HWIO_GCC_BIMC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_CBCR_ADDR,m,v,HWIO_GCC_BIMC_CBCR_IN)
#define HWIO_GCC_BIMC_CBCR_CLK_OFF_BMSK                                                         0x80000000
#define HWIO_GCC_BIMC_CBCR_CLK_OFF_SHFT                                                               0x1f
#define HWIO_GCC_BIMC_CBCR_CLK_ENABLE_BMSK                                                             0x1
#define HWIO_GCC_BIMC_CBCR_CLK_ENABLE_SHFT                                                             0x0

#define HWIO_GCC_BIMC_APSS_AXI_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00031020)
#define HWIO_GCC_BIMC_APSS_AXI_CBCR_RMSK                                                        0x80000000
#define HWIO_GCC_BIMC_APSS_AXI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BIMC_APSS_AXI_CBCR_ADDR, HWIO_GCC_BIMC_APSS_AXI_CBCR_RMSK)
#define HWIO_GCC_BIMC_APSS_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_APSS_AXI_CBCR_ADDR, m)
#define HWIO_GCC_BIMC_APSS_AXI_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_BIMC_APSS_AXI_CBCR_CLK_OFF_SHFT                                                      0x1f

#define HWIO_GCC_DDR_DIM_CFG_CBCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x0003201c)
#define HWIO_GCC_DDR_DIM_CFG_CBCR_RMSK                                                          0xf0008001
#define HWIO_GCC_DDR_DIM_CFG_CBCR_IN          \
        in_dword_masked(HWIO_GCC_DDR_DIM_CFG_CBCR_ADDR, HWIO_GCC_DDR_DIM_CFG_CBCR_RMSK)
#define HWIO_GCC_DDR_DIM_CFG_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_DDR_DIM_CFG_CBCR_ADDR, m)
#define HWIO_GCC_DDR_DIM_CFG_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_DDR_DIM_CFG_CBCR_ADDR,v)
#define HWIO_GCC_DDR_DIM_CFG_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_DDR_DIM_CFG_CBCR_ADDR,m,v,HWIO_GCC_DDR_DIM_CFG_CBCR_IN)
#define HWIO_GCC_DDR_DIM_CFG_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_DDR_DIM_CFG_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_GCC_DDR_DIM_CFG_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                 0x70000000
#define HWIO_GCC_DDR_DIM_CFG_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                       0x1c
#define HWIO_GCC_DDR_DIM_CFG_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                         0x8000
#define HWIO_GCC_DDR_DIM_CFG_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                            0xf
#define HWIO_GCC_DDR_DIM_CFG_CBCR_CLK_ENABLE_BMSK                                                      0x1
#define HWIO_GCC_DDR_DIM_CFG_CBCR_CLK_ENABLE_SHFT                                                      0x0

#define HWIO_GCC_DDR_DIM_SLEEP_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00032020)
#define HWIO_GCC_DDR_DIM_SLEEP_CBCR_RMSK                                                        0x80000001
#define HWIO_GCC_DDR_DIM_SLEEP_CBCR_IN          \
        in_dword_masked(HWIO_GCC_DDR_DIM_SLEEP_CBCR_ADDR, HWIO_GCC_DDR_DIM_SLEEP_CBCR_RMSK)
#define HWIO_GCC_DDR_DIM_SLEEP_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_DDR_DIM_SLEEP_CBCR_ADDR, m)
#define HWIO_GCC_DDR_DIM_SLEEP_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_DDR_DIM_SLEEP_CBCR_ADDR,v)
#define HWIO_GCC_DDR_DIM_SLEEP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_DDR_DIM_SLEEP_CBCR_ADDR,m,v,HWIO_GCC_DDR_DIM_SLEEP_CBCR_IN)
#define HWIO_GCC_DDR_DIM_SLEEP_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_DDR_DIM_SLEEP_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_DDR_DIM_SLEEP_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_DDR_DIM_SLEEP_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_APSS_TCU_CMD_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0001207c)
#define HWIO_GCC_APSS_TCU_CMD_RCGR_RMSK                                                         0x80000013
#define HWIO_GCC_APSS_TCU_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_APSS_TCU_CMD_RCGR_ADDR, HWIO_GCC_APSS_TCU_CMD_RCGR_RMSK)
#define HWIO_GCC_APSS_TCU_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_APSS_TCU_CMD_RCGR_ADDR, m)
#define HWIO_GCC_APSS_TCU_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_APSS_TCU_CMD_RCGR_ADDR,v)
#define HWIO_GCC_APSS_TCU_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APSS_TCU_CMD_RCGR_ADDR,m,v,HWIO_GCC_APSS_TCU_CMD_RCGR_IN)
#define HWIO_GCC_APSS_TCU_CMD_RCGR_ROOT_OFF_BMSK                                                0x80000000
#define HWIO_GCC_APSS_TCU_CMD_RCGR_ROOT_OFF_SHFT                                                      0x1f
#define HWIO_GCC_APSS_TCU_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                0x10
#define HWIO_GCC_APSS_TCU_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                 0x4
#define HWIO_GCC_APSS_TCU_CMD_RCGR_ROOT_EN_BMSK                                                        0x2
#define HWIO_GCC_APSS_TCU_CMD_RCGR_ROOT_EN_SHFT                                                        0x1
#define HWIO_GCC_APSS_TCU_CMD_RCGR_UPDATE_BMSK                                                         0x1
#define HWIO_GCC_APSS_TCU_CMD_RCGR_UPDATE_SHFT                                                         0x0

#define HWIO_GCC_APSS_TCU_CFG_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00012080)
#define HWIO_GCC_APSS_TCU_CFG_RCGR_RMSK                                                              0x71f
#define HWIO_GCC_APSS_TCU_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_APSS_TCU_CFG_RCGR_ADDR, HWIO_GCC_APSS_TCU_CFG_RCGR_RMSK)
#define HWIO_GCC_APSS_TCU_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_APSS_TCU_CFG_RCGR_ADDR, m)
#define HWIO_GCC_APSS_TCU_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_APSS_TCU_CFG_RCGR_ADDR,v)
#define HWIO_GCC_APSS_TCU_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APSS_TCU_CFG_RCGR_ADDR,m,v,HWIO_GCC_APSS_TCU_CFG_RCGR_IN)
#define HWIO_GCC_APSS_TCU_CFG_RCGR_SRC_SEL_BMSK                                                      0x700
#define HWIO_GCC_APSS_TCU_CFG_RCGR_SRC_SEL_SHFT                                                        0x8
#define HWIO_GCC_APSS_TCU_CFG_RCGR_SRC_DIV_BMSK                                                       0x1f
#define HWIO_GCC_APSS_TCU_CFG_RCGR_SRC_DIV_SHFT                                                        0x0

#define HWIO_GCC_BIMC_GPU_CMD_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00031028)
#define HWIO_GCC_BIMC_GPU_CMD_RCGR_RMSK                                                         0x80000013
#define HWIO_GCC_BIMC_GPU_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BIMC_GPU_CMD_RCGR_ADDR, HWIO_GCC_BIMC_GPU_CMD_RCGR_RMSK)
#define HWIO_GCC_BIMC_GPU_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_GPU_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BIMC_GPU_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_GPU_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BIMC_GPU_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_GPU_CMD_RCGR_ADDR,m,v,HWIO_GCC_BIMC_GPU_CMD_RCGR_IN)
#define HWIO_GCC_BIMC_GPU_CMD_RCGR_ROOT_OFF_BMSK                                                0x80000000
#define HWIO_GCC_BIMC_GPU_CMD_RCGR_ROOT_OFF_SHFT                                                      0x1f
#define HWIO_GCC_BIMC_GPU_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                0x10
#define HWIO_GCC_BIMC_GPU_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                 0x4
#define HWIO_GCC_BIMC_GPU_CMD_RCGR_ROOT_EN_BMSK                                                        0x2
#define HWIO_GCC_BIMC_GPU_CMD_RCGR_ROOT_EN_SHFT                                                        0x1
#define HWIO_GCC_BIMC_GPU_CMD_RCGR_UPDATE_BMSK                                                         0x1
#define HWIO_GCC_BIMC_GPU_CMD_RCGR_UPDATE_SHFT                                                         0x0

#define HWIO_GCC_BIMC_GPU_CFG_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0003102c)
#define HWIO_GCC_BIMC_GPU_CFG_RCGR_RMSK                                                              0x71f
#define HWIO_GCC_BIMC_GPU_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BIMC_GPU_CFG_RCGR_ADDR, HWIO_GCC_BIMC_GPU_CFG_RCGR_RMSK)
#define HWIO_GCC_BIMC_GPU_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_GPU_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BIMC_GPU_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_GPU_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BIMC_GPU_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_GPU_CFG_RCGR_ADDR,m,v,HWIO_GCC_BIMC_GPU_CFG_RCGR_IN)
#define HWIO_GCC_BIMC_GPU_CFG_RCGR_SRC_SEL_BMSK                                                      0x700
#define HWIO_GCC_BIMC_GPU_CFG_RCGR_SRC_SEL_SHFT                                                        0x8
#define HWIO_GCC_BIMC_GPU_CFG_RCGR_SRC_DIV_BMSK                                                       0x1f
#define HWIO_GCC_BIMC_GPU_CFG_RCGR_SRC_DIV_SHFT                                                        0x0

#define HWIO_GCC_BIMC_GPU_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00031040)
#define HWIO_GCC_BIMC_GPU_CBCR_RMSK                                                             0x80000001
#define HWIO_GCC_BIMC_GPU_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BIMC_GPU_CBCR_ADDR, HWIO_GCC_BIMC_GPU_CBCR_RMSK)
#define HWIO_GCC_BIMC_GPU_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_GPU_CBCR_ADDR, m)
#define HWIO_GCC_BIMC_GPU_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BIMC_GPU_CBCR_ADDR,v)
#define HWIO_GCC_BIMC_GPU_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BIMC_GPU_CBCR_ADDR,m,v,HWIO_GCC_BIMC_GPU_CBCR_IN)
#define HWIO_GCC_BIMC_GPU_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_BIMC_GPU_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_BIMC_GPU_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_GCC_BIMC_GPU_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_GCC_BIMC_TCU_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00031044)
#define HWIO_GCC_BIMC_TCU_CBCR_RMSK                                                             0x80000000
#define HWIO_GCC_BIMC_TCU_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BIMC_TCU_CBCR_ADDR, HWIO_GCC_BIMC_TCU_CBCR_RMSK)
#define HWIO_GCC_BIMC_TCU_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BIMC_TCU_CBCR_ADDR, m)
#define HWIO_GCC_BIMC_TCU_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_BIMC_TCU_CBCR_CLK_OFF_SHFT                                                           0x1f

#define HWIO_GCC_ULTAUDIO_PCNOC_MPORT_CBCR_ADDR                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c000)
#define HWIO_GCC_ULTAUDIO_PCNOC_MPORT_CBCR_RMSK                                                 0x80000003
#define HWIO_GCC_ULTAUDIO_PCNOC_MPORT_CBCR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_PCNOC_MPORT_CBCR_ADDR, HWIO_GCC_ULTAUDIO_PCNOC_MPORT_CBCR_RMSK)
#define HWIO_GCC_ULTAUDIO_PCNOC_MPORT_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_PCNOC_MPORT_CBCR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_PCNOC_MPORT_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_PCNOC_MPORT_CBCR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_PCNOC_MPORT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_PCNOC_MPORT_CBCR_ADDR,m,v,HWIO_GCC_ULTAUDIO_PCNOC_MPORT_CBCR_IN)
#define HWIO_GCC_ULTAUDIO_PCNOC_MPORT_CBCR_CLK_OFF_BMSK                                         0x80000000
#define HWIO_GCC_ULTAUDIO_PCNOC_MPORT_CBCR_CLK_OFF_SHFT                                               0x1f
#define HWIO_GCC_ULTAUDIO_PCNOC_MPORT_CBCR_HW_CTL_BMSK                                                 0x2
#define HWIO_GCC_ULTAUDIO_PCNOC_MPORT_CBCR_HW_CTL_SHFT                                                 0x1
#define HWIO_GCC_ULTAUDIO_PCNOC_MPORT_CBCR_CLK_ENABLE_BMSK                                             0x1
#define HWIO_GCC_ULTAUDIO_PCNOC_MPORT_CBCR_CLK_ENABLE_SHFT                                             0x0

#define HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c004)
#define HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_RMSK                                                  0xf0008001
#define HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_ADDR, HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_RMSK)
#define HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_ADDR,m,v,HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_IN)
#define HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_CLK_OFF_BMSK                                          0x80000000
#define HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_CLK_OFF_SHFT                                                0x1f
#define HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                         0x70000000
#define HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                               0x1c
#define HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                 0x8000
#define HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                    0xf
#define HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_CLK_ENABLE_BMSK                                              0x1
#define HWIO_GCC_ULTAUDIO_PCNOC_SWAY_CBCR_CLK_ENABLE_SHFT                                              0x0

#define HWIO_GCC_ULT_AUDIO_BCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c0b4)
#define HWIO_GCC_ULT_AUDIO_BCR_RMSK                                                                    0x1
#define HWIO_GCC_ULT_AUDIO_BCR_IN          \
        in_dword_masked(HWIO_GCC_ULT_AUDIO_BCR_ADDR, HWIO_GCC_ULT_AUDIO_BCR_RMSK)
#define HWIO_GCC_ULT_AUDIO_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULT_AUDIO_BCR_ADDR, m)
#define HWIO_GCC_ULT_AUDIO_BCR_OUT(v)      \
        out_dword(HWIO_GCC_ULT_AUDIO_BCR_ADDR,v)
#define HWIO_GCC_ULT_AUDIO_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULT_AUDIO_BCR_ADDR,m,v,HWIO_GCC_ULT_AUDIO_BCR_IN)
#define HWIO_GCC_ULT_AUDIO_BCR_BLK_ARES_BMSK                                                           0x1
#define HWIO_GCC_ULT_AUDIO_BCR_BLK_ARES_SHFT                                                           0x0

#define HWIO_GCC_APSS_AHB_CMD_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00046000)
#define HWIO_GCC_APSS_AHB_CMD_RCGR_RMSK                                                         0x80000013
#define HWIO_GCC_APSS_AHB_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_APSS_AHB_CMD_RCGR_ADDR, HWIO_GCC_APSS_AHB_CMD_RCGR_RMSK)
#define HWIO_GCC_APSS_AHB_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_APSS_AHB_CMD_RCGR_ADDR, m)
#define HWIO_GCC_APSS_AHB_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_APSS_AHB_CMD_RCGR_ADDR,v)
#define HWIO_GCC_APSS_AHB_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APSS_AHB_CMD_RCGR_ADDR,m,v,HWIO_GCC_APSS_AHB_CMD_RCGR_IN)
#define HWIO_GCC_APSS_AHB_CMD_RCGR_ROOT_OFF_BMSK                                                0x80000000
#define HWIO_GCC_APSS_AHB_CMD_RCGR_ROOT_OFF_SHFT                                                      0x1f
#define HWIO_GCC_APSS_AHB_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                0x10
#define HWIO_GCC_APSS_AHB_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                 0x4
#define HWIO_GCC_APSS_AHB_CMD_RCGR_ROOT_EN_BMSK                                                        0x2
#define HWIO_GCC_APSS_AHB_CMD_RCGR_ROOT_EN_SHFT                                                        0x1
#define HWIO_GCC_APSS_AHB_CMD_RCGR_UPDATE_BMSK                                                         0x1
#define HWIO_GCC_APSS_AHB_CMD_RCGR_UPDATE_SHFT                                                         0x0

#define HWIO_GCC_APSS_AHB_CFG_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00046004)
#define HWIO_GCC_APSS_AHB_CFG_RCGR_RMSK                                                              0x71f
#define HWIO_GCC_APSS_AHB_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_APSS_AHB_CFG_RCGR_ADDR, HWIO_GCC_APSS_AHB_CFG_RCGR_RMSK)
#define HWIO_GCC_APSS_AHB_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_APSS_AHB_CFG_RCGR_ADDR, m)
#define HWIO_GCC_APSS_AHB_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_APSS_AHB_CFG_RCGR_ADDR,v)
#define HWIO_GCC_APSS_AHB_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APSS_AHB_CFG_RCGR_ADDR,m,v,HWIO_GCC_APSS_AHB_CFG_RCGR_IN)
#define HWIO_GCC_APSS_AHB_CFG_RCGR_SRC_SEL_BMSK                                                      0x700
#define HWIO_GCC_APSS_AHB_CFG_RCGR_SRC_SEL_SHFT                                                        0x8
#define HWIO_GCC_APSS_AHB_CFG_RCGR_SRC_DIV_BMSK                                                       0x1f
#define HWIO_GCC_APSS_AHB_CFG_RCGR_SRC_DIV_SHFT                                                        0x0

#define HWIO_GCC_APSS_AHB_MISC_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00046018)
#define HWIO_GCC_APSS_AHB_MISC_RMSK                                                                   0xf1
#define HWIO_GCC_APSS_AHB_MISC_IN          \
        in_dword_masked(HWIO_GCC_APSS_AHB_MISC_ADDR, HWIO_GCC_APSS_AHB_MISC_RMSK)
#define HWIO_GCC_APSS_AHB_MISC_INM(m)      \
        in_dword_masked(HWIO_GCC_APSS_AHB_MISC_ADDR, m)
#define HWIO_GCC_APSS_AHB_MISC_OUT(v)      \
        out_dword(HWIO_GCC_APSS_AHB_MISC_ADDR,v)
#define HWIO_GCC_APSS_AHB_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APSS_AHB_MISC_ADDR,m,v,HWIO_GCC_APSS_AHB_MISC_IN)
#define HWIO_GCC_APSS_AHB_MISC_APSS_AHB_CLK_AUTO_SCALE_DIV_BMSK                                       0xf0
#define HWIO_GCC_APSS_AHB_MISC_APSS_AHB_CLK_AUTO_SCALE_DIV_SHFT                                        0x4
#define HWIO_GCC_APSS_AHB_MISC_APSS_AHB_CLK_AUTO_SCALE_DIS_BMSK                                        0x1
#define HWIO_GCC_APSS_AHB_MISC_APSS_AHB_CLK_AUTO_SCALE_DIS_SHFT                                        0x0

#define HWIO_GCC_APSS_AHB_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0004601c)
#define HWIO_GCC_APSS_AHB_CBCR_RMSK                                                             0xf0008000
#define HWIO_GCC_APSS_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_APSS_AHB_CBCR_ADDR, HWIO_GCC_APSS_AHB_CBCR_RMSK)
#define HWIO_GCC_APSS_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_APSS_AHB_CBCR_ADDR, m)
#define HWIO_GCC_APSS_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_APSS_AHB_CBCR_ADDR,v)
#define HWIO_GCC_APSS_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APSS_AHB_CBCR_ADDR,m,v,HWIO_GCC_APSS_AHB_CBCR_IN)
#define HWIO_GCC_APSS_AHB_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_APSS_AHB_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_APSS_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                    0x70000000
#define HWIO_GCC_APSS_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                          0x1c
#define HWIO_GCC_APSS_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                            0x8000
#define HWIO_GCC_APSS_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                               0xf

#define HWIO_GCC_APSS_AXI_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00046020)
#define HWIO_GCC_APSS_AXI_CBCR_RMSK                                                             0x80000000
#define HWIO_GCC_APSS_AXI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_APSS_AXI_CBCR_ADDR, HWIO_GCC_APSS_AXI_CBCR_RMSK)
#define HWIO_GCC_APSS_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_APSS_AXI_CBCR_ADDR, m)
#define HWIO_GCC_APSS_AXI_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_APSS_AXI_CBCR_CLK_OFF_SHFT                                                           0x1f

#define HWIO_GCC_SNOC_BUS_TIMEOUT0_BCR_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00047000)
#define HWIO_GCC_SNOC_BUS_TIMEOUT0_BCR_RMSK                                                            0x1
#define HWIO_GCC_SNOC_BUS_TIMEOUT0_BCR_IN          \
        in_dword_masked(HWIO_GCC_SNOC_BUS_TIMEOUT0_BCR_ADDR, HWIO_GCC_SNOC_BUS_TIMEOUT0_BCR_RMSK)
#define HWIO_GCC_SNOC_BUS_TIMEOUT0_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SNOC_BUS_TIMEOUT0_BCR_ADDR, m)
#define HWIO_GCC_SNOC_BUS_TIMEOUT0_BCR_OUT(v)      \
        out_dword(HWIO_GCC_SNOC_BUS_TIMEOUT0_BCR_ADDR,v)
#define HWIO_GCC_SNOC_BUS_TIMEOUT0_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SNOC_BUS_TIMEOUT0_BCR_ADDR,m,v,HWIO_GCC_SNOC_BUS_TIMEOUT0_BCR_IN)
#define HWIO_GCC_SNOC_BUS_TIMEOUT0_BCR_BLK_ARES_BMSK                                                   0x1
#define HWIO_GCC_SNOC_BUS_TIMEOUT0_BCR_BLK_ARES_SHFT                                                   0x0

#define HWIO_GCC_SNOC_BUS_TIMEOUT0_AHB_CBCR_ADDR                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00047004)
#define HWIO_GCC_SNOC_BUS_TIMEOUT0_AHB_CBCR_RMSK                                                0x80000001
#define HWIO_GCC_SNOC_BUS_TIMEOUT0_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SNOC_BUS_TIMEOUT0_AHB_CBCR_ADDR, HWIO_GCC_SNOC_BUS_TIMEOUT0_AHB_CBCR_RMSK)
#define HWIO_GCC_SNOC_BUS_TIMEOUT0_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SNOC_BUS_TIMEOUT0_AHB_CBCR_ADDR, m)
#define HWIO_GCC_SNOC_BUS_TIMEOUT0_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SNOC_BUS_TIMEOUT0_AHB_CBCR_ADDR,v)
#define HWIO_GCC_SNOC_BUS_TIMEOUT0_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SNOC_BUS_TIMEOUT0_AHB_CBCR_ADDR,m,v,HWIO_GCC_SNOC_BUS_TIMEOUT0_AHB_CBCR_IN)
#define HWIO_GCC_SNOC_BUS_TIMEOUT0_AHB_CBCR_CLK_OFF_BMSK                                        0x80000000
#define HWIO_GCC_SNOC_BUS_TIMEOUT0_AHB_CBCR_CLK_OFF_SHFT                                              0x1f
#define HWIO_GCC_SNOC_BUS_TIMEOUT0_AHB_CBCR_CLK_ENABLE_BMSK                                            0x1
#define HWIO_GCC_SNOC_BUS_TIMEOUT0_AHB_CBCR_CLK_ENABLE_SHFT                                            0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT0_BCR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00048000)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT0_BCR_RMSK                                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT0_BCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT0_BCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT0_BCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT0_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT0_BCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT0_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT0_BCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT0_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT0_BCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT0_BCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT0_BCR_BLK_ARES_BMSK                                                  0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT0_BCR_BLK_ARES_SHFT                                                  0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT0_AHB_CBCR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00048004)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT0_AHB_CBCR_RMSK                                               0x80000001
#define HWIO_GCC_PCNOC_BUS_TIMEOUT0_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT0_AHB_CBCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT0_AHB_CBCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT0_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT0_AHB_CBCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT0_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT0_AHB_CBCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT0_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT0_AHB_CBCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT0_AHB_CBCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT0_AHB_CBCR_CLK_OFF_BMSK                                       0x80000000
#define HWIO_GCC_PCNOC_BUS_TIMEOUT0_AHB_CBCR_CLK_OFF_SHFT                                             0x1f
#define HWIO_GCC_PCNOC_BUS_TIMEOUT0_AHB_CBCR_CLK_ENABLE_BMSK                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT0_AHB_CBCR_CLK_ENABLE_SHFT                                           0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT1_BCR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00048008)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT1_BCR_RMSK                                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT1_BCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT1_BCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT1_BCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT1_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT1_BCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT1_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT1_BCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT1_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT1_BCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT1_BCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT1_BCR_BLK_ARES_BMSK                                                  0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT1_BCR_BLK_ARES_SHFT                                                  0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT1_AHB_CBCR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0004800c)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT1_AHB_CBCR_RMSK                                               0x80000001
#define HWIO_GCC_PCNOC_BUS_TIMEOUT1_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT1_AHB_CBCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT1_AHB_CBCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT1_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT1_AHB_CBCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT1_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT1_AHB_CBCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT1_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT1_AHB_CBCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT1_AHB_CBCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT1_AHB_CBCR_CLK_OFF_BMSK                                       0x80000000
#define HWIO_GCC_PCNOC_BUS_TIMEOUT1_AHB_CBCR_CLK_OFF_SHFT                                             0x1f
#define HWIO_GCC_PCNOC_BUS_TIMEOUT1_AHB_CBCR_CLK_ENABLE_BMSK                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT1_AHB_CBCR_CLK_ENABLE_SHFT                                           0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT2_BCR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00048010)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT2_BCR_RMSK                                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT2_BCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT2_BCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT2_BCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT2_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT2_BCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT2_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT2_BCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT2_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT2_BCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT2_BCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT2_BCR_BLK_ARES_BMSK                                                  0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT2_BCR_BLK_ARES_SHFT                                                  0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT2_AHB_CBCR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00048014)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT2_AHB_CBCR_RMSK                                               0x80000001
#define HWIO_GCC_PCNOC_BUS_TIMEOUT2_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT2_AHB_CBCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT2_AHB_CBCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT2_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT2_AHB_CBCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT2_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT2_AHB_CBCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT2_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT2_AHB_CBCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT2_AHB_CBCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT2_AHB_CBCR_CLK_OFF_BMSK                                       0x80000000
#define HWIO_GCC_PCNOC_BUS_TIMEOUT2_AHB_CBCR_CLK_OFF_SHFT                                             0x1f
#define HWIO_GCC_PCNOC_BUS_TIMEOUT2_AHB_CBCR_CLK_ENABLE_BMSK                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT2_AHB_CBCR_CLK_ENABLE_SHFT                                           0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT3_BCR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00048018)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT3_BCR_RMSK                                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT3_BCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT3_BCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT3_BCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT3_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT3_BCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT3_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT3_BCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT3_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT3_BCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT3_BCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT3_BCR_BLK_ARES_BMSK                                                  0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT3_BCR_BLK_ARES_SHFT                                                  0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT3_AHB_CBCR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0004801c)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT3_AHB_CBCR_RMSK                                               0x80000001
#define HWIO_GCC_PCNOC_BUS_TIMEOUT3_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT3_AHB_CBCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT3_AHB_CBCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT3_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT3_AHB_CBCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT3_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT3_AHB_CBCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT3_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT3_AHB_CBCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT3_AHB_CBCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT3_AHB_CBCR_CLK_OFF_BMSK                                       0x80000000
#define HWIO_GCC_PCNOC_BUS_TIMEOUT3_AHB_CBCR_CLK_OFF_SHFT                                             0x1f
#define HWIO_GCC_PCNOC_BUS_TIMEOUT3_AHB_CBCR_CLK_ENABLE_BMSK                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT3_AHB_CBCR_CLK_ENABLE_SHFT                                           0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT4_BCR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00048020)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT4_BCR_RMSK                                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT4_BCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT4_BCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT4_BCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT4_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT4_BCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT4_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT4_BCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT4_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT4_BCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT4_BCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT4_BCR_BLK_ARES_BMSK                                                  0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT4_BCR_BLK_ARES_SHFT                                                  0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT4_AHB_CBCR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00048024)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT4_AHB_CBCR_RMSK                                               0x80000001
#define HWIO_GCC_PCNOC_BUS_TIMEOUT4_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT4_AHB_CBCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT4_AHB_CBCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT4_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT4_AHB_CBCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT4_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT4_AHB_CBCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT4_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT4_AHB_CBCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT4_AHB_CBCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT4_AHB_CBCR_CLK_OFF_BMSK                                       0x80000000
#define HWIO_GCC_PCNOC_BUS_TIMEOUT4_AHB_CBCR_CLK_OFF_SHFT                                             0x1f
#define HWIO_GCC_PCNOC_BUS_TIMEOUT4_AHB_CBCR_CLK_ENABLE_BMSK                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT4_AHB_CBCR_CLK_ENABLE_SHFT                                           0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT5_BCR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00048028)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT5_BCR_RMSK                                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT5_BCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT5_BCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT5_BCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT5_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT5_BCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT5_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT5_BCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT5_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT5_BCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT5_BCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT5_BCR_BLK_ARES_BMSK                                                  0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT5_BCR_BLK_ARES_SHFT                                                  0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT5_AHB_CBCR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0004802c)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT5_AHB_CBCR_RMSK                                               0x80000001
#define HWIO_GCC_PCNOC_BUS_TIMEOUT5_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT5_AHB_CBCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT5_AHB_CBCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT5_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT5_AHB_CBCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT5_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT5_AHB_CBCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT5_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT5_AHB_CBCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT5_AHB_CBCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT5_AHB_CBCR_CLK_OFF_BMSK                                       0x80000000
#define HWIO_GCC_PCNOC_BUS_TIMEOUT5_AHB_CBCR_CLK_OFF_SHFT                                             0x1f
#define HWIO_GCC_PCNOC_BUS_TIMEOUT5_AHB_CBCR_CLK_ENABLE_BMSK                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT5_AHB_CBCR_CLK_ENABLE_SHFT                                           0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT6_BCR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00048030)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT6_BCR_RMSK                                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT6_BCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT6_BCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT6_BCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT6_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT6_BCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT6_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT6_BCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT6_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT6_BCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT6_BCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT6_BCR_BLK_ARES_BMSK                                                  0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT6_BCR_BLK_ARES_SHFT                                                  0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT6_AHB_CBCR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00048034)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT6_AHB_CBCR_RMSK                                               0x80000001
#define HWIO_GCC_PCNOC_BUS_TIMEOUT6_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT6_AHB_CBCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT6_AHB_CBCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT6_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT6_AHB_CBCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT6_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT6_AHB_CBCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT6_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT6_AHB_CBCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT6_AHB_CBCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT6_AHB_CBCR_CLK_OFF_BMSK                                       0x80000000
#define HWIO_GCC_PCNOC_BUS_TIMEOUT6_AHB_CBCR_CLK_OFF_SHFT                                             0x1f
#define HWIO_GCC_PCNOC_BUS_TIMEOUT6_AHB_CBCR_CLK_ENABLE_BMSK                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT6_AHB_CBCR_CLK_ENABLE_SHFT                                           0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT7_BCR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00048038)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT7_BCR_RMSK                                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT7_BCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT7_BCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT7_BCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT7_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT7_BCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT7_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT7_BCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT7_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT7_BCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT7_BCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT7_BCR_BLK_ARES_BMSK                                                  0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT7_BCR_BLK_ARES_SHFT                                                  0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT7_AHB_CBCR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0004803c)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT7_AHB_CBCR_RMSK                                               0x80000001
#define HWIO_GCC_PCNOC_BUS_TIMEOUT7_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT7_AHB_CBCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT7_AHB_CBCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT7_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT7_AHB_CBCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT7_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT7_AHB_CBCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT7_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT7_AHB_CBCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT7_AHB_CBCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT7_AHB_CBCR_CLK_OFF_BMSK                                       0x80000000
#define HWIO_GCC_PCNOC_BUS_TIMEOUT7_AHB_CBCR_CLK_OFF_SHFT                                             0x1f
#define HWIO_GCC_PCNOC_BUS_TIMEOUT7_AHB_CBCR_CLK_ENABLE_BMSK                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT7_AHB_CBCR_CLK_ENABLE_SHFT                                           0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT8_BCR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00048040)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT8_BCR_RMSK                                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT8_BCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT8_BCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT8_BCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT8_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT8_BCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT8_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT8_BCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT8_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT8_BCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT8_BCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT8_BCR_BLK_ARES_BMSK                                                  0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT8_BCR_BLK_ARES_SHFT                                                  0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT8_AHB_CBCR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00048044)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT8_AHB_CBCR_RMSK                                               0x80000001
#define HWIO_GCC_PCNOC_BUS_TIMEOUT8_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT8_AHB_CBCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT8_AHB_CBCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT8_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT8_AHB_CBCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT8_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT8_AHB_CBCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT8_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT8_AHB_CBCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT8_AHB_CBCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT8_AHB_CBCR_CLK_OFF_BMSK                                       0x80000000
#define HWIO_GCC_PCNOC_BUS_TIMEOUT8_AHB_CBCR_CLK_OFF_SHFT                                             0x1f
#define HWIO_GCC_PCNOC_BUS_TIMEOUT8_AHB_CBCR_CLK_ENABLE_BMSK                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT8_AHB_CBCR_CLK_ENABLE_SHFT                                           0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT9_BCR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00048048)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT9_BCR_RMSK                                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT9_BCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT9_BCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT9_BCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT9_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT9_BCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT9_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT9_BCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT9_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT9_BCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT9_BCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT9_BCR_BLK_ARES_BMSK                                                  0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT9_BCR_BLK_ARES_SHFT                                                  0x0

#define HWIO_GCC_PCNOC_BUS_TIMEOUT9_AHB_CBCR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0004804c)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT9_AHB_CBCR_RMSK                                               0x80000001
#define HWIO_GCC_PCNOC_BUS_TIMEOUT9_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT9_AHB_CBCR_ADDR, HWIO_GCC_PCNOC_BUS_TIMEOUT9_AHB_CBCR_RMSK)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT9_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_BUS_TIMEOUT9_AHB_CBCR_ADDR, m)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT9_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_BUS_TIMEOUT9_AHB_CBCR_ADDR,v)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT9_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_BUS_TIMEOUT9_AHB_CBCR_ADDR,m,v,HWIO_GCC_PCNOC_BUS_TIMEOUT9_AHB_CBCR_IN)
#define HWIO_GCC_PCNOC_BUS_TIMEOUT9_AHB_CBCR_CLK_OFF_BMSK                                       0x80000000
#define HWIO_GCC_PCNOC_BUS_TIMEOUT9_AHB_CBCR_CLK_OFF_SHFT                                             0x1f
#define HWIO_GCC_PCNOC_BUS_TIMEOUT9_AHB_CBCR_CLK_ENABLE_BMSK                                           0x1
#define HWIO_GCC_PCNOC_BUS_TIMEOUT9_AHB_CBCR_CLK_ENABLE_SHFT                                           0x0

#define HWIO_GCC_DEHR_BCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0001f000)
#define HWIO_GCC_DEHR_BCR_RMSK                                                                         0x1
#define HWIO_GCC_DEHR_BCR_IN          \
        in_dword_masked(HWIO_GCC_DEHR_BCR_ADDR, HWIO_GCC_DEHR_BCR_RMSK)
#define HWIO_GCC_DEHR_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_DEHR_BCR_ADDR, m)
#define HWIO_GCC_DEHR_BCR_OUT(v)      \
        out_dword(HWIO_GCC_DEHR_BCR_ADDR,v)
#define HWIO_GCC_DEHR_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_DEHR_BCR_ADDR,m,v,HWIO_GCC_DEHR_BCR_IN)
#define HWIO_GCC_DEHR_BCR_BLK_ARES_BMSK                                                                0x1
#define HWIO_GCC_DEHR_BCR_BLK_ARES_SHFT                                                                0x0

#define HWIO_GCC_DEHR_CBCR_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x0001f004)
#define HWIO_GCC_DEHR_CBCR_RMSK                                                                 0xf000fff1
#define HWIO_GCC_DEHR_CBCR_IN          \
        in_dword_masked(HWIO_GCC_DEHR_CBCR_ADDR, HWIO_GCC_DEHR_CBCR_RMSK)
#define HWIO_GCC_DEHR_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_DEHR_CBCR_ADDR, m)
#define HWIO_GCC_DEHR_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_DEHR_CBCR_ADDR,v)
#define HWIO_GCC_DEHR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_DEHR_CBCR_ADDR,m,v,HWIO_GCC_DEHR_CBCR_IN)
#define HWIO_GCC_DEHR_CBCR_CLK_OFF_BMSK                                                         0x80000000
#define HWIO_GCC_DEHR_CBCR_CLK_OFF_SHFT                                                               0x1f
#define HWIO_GCC_DEHR_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                        0x70000000
#define HWIO_GCC_DEHR_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                              0x1c
#define HWIO_GCC_DEHR_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                                0x8000
#define HWIO_GCC_DEHR_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                                   0xf
#define HWIO_GCC_DEHR_CBCR_FORCE_MEM_CORE_ON_BMSK                                                   0x4000
#define HWIO_GCC_DEHR_CBCR_FORCE_MEM_CORE_ON_SHFT                                                      0xe
#define HWIO_GCC_DEHR_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                                 0x2000
#define HWIO_GCC_DEHR_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                    0xd
#define HWIO_GCC_DEHR_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                                0x1000
#define HWIO_GCC_DEHR_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                                   0xc
#define HWIO_GCC_DEHR_CBCR_WAKEUP_BMSK                                                               0xf00
#define HWIO_GCC_DEHR_CBCR_WAKEUP_SHFT                                                                 0x8
#define HWIO_GCC_DEHR_CBCR_SLEEP_BMSK                                                                 0xf0
#define HWIO_GCC_DEHR_CBCR_SLEEP_SHFT                                                                  0x4
#define HWIO_GCC_DEHR_CBCR_CLK_ENABLE_BMSK                                                             0x1
#define HWIO_GCC_DEHR_CBCR_CLK_ENABLE_SHFT                                                             0x0

#define HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x0001700c)
#define HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_RMSK                                                    0xf0008001
#define HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_ADDR, HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_RMSK)
#define HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_ADDR, m)
#define HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_ADDR,v)
#define HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_ADDR,m,v,HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_IN)
#define HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_CLK_OFF_BMSK                                            0x80000000
#define HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_CLK_OFF_SHFT                                                  0x1f
#define HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                           0x70000000
#define HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                 0x1c
#define HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                   0x8000
#define HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                      0xf
#define HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_CLK_ENABLE_BMSK                                                0x1
#define HWIO_GCC_PCNOC_MPU_CFG_AHB_CBCR_CLK_ENABLE_SHFT                                                0x0

#define HWIO_GCC_GFX_TBU_CBCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00012010)
#define HWIO_GCC_GFX_TBU_CBCR_RMSK                                                              0x80007ff0
#define HWIO_GCC_GFX_TBU_CBCR_IN          \
        in_dword_masked(HWIO_GCC_GFX_TBU_CBCR_ADDR, HWIO_GCC_GFX_TBU_CBCR_RMSK)
#define HWIO_GCC_GFX_TBU_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_GFX_TBU_CBCR_ADDR, m)
#define HWIO_GCC_GFX_TBU_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_GFX_TBU_CBCR_ADDR,v)
#define HWIO_GCC_GFX_TBU_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GFX_TBU_CBCR_ADDR,m,v,HWIO_GCC_GFX_TBU_CBCR_IN)
#define HWIO_GCC_GFX_TBU_CBCR_CLK_OFF_BMSK                                                      0x80000000
#define HWIO_GCC_GFX_TBU_CBCR_CLK_OFF_SHFT                                                            0x1f
#define HWIO_GCC_GFX_TBU_CBCR_FORCE_MEM_CORE_ON_BMSK                                                0x4000
#define HWIO_GCC_GFX_TBU_CBCR_FORCE_MEM_CORE_ON_SHFT                                                   0xe
#define HWIO_GCC_GFX_TBU_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                              0x2000
#define HWIO_GCC_GFX_TBU_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                 0xd
#define HWIO_GCC_GFX_TBU_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                             0x1000
#define HWIO_GCC_GFX_TBU_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                                0xc
#define HWIO_GCC_GFX_TBU_CBCR_WAKEUP_BMSK                                                            0xf00
#define HWIO_GCC_GFX_TBU_CBCR_WAKEUP_SHFT                                                              0x8
#define HWIO_GCC_GFX_TBU_CBCR_SLEEP_BMSK                                                              0xf0
#define HWIO_GCC_GFX_TBU_CBCR_SLEEP_SHFT                                                               0x4

#define HWIO_GCC_VENUS_TBU_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00012014)
#define HWIO_GCC_VENUS_TBU_CBCR_RMSK                                                            0x80007ff0
#define HWIO_GCC_VENUS_TBU_CBCR_IN          \
        in_dword_masked(HWIO_GCC_VENUS_TBU_CBCR_ADDR, HWIO_GCC_VENUS_TBU_CBCR_RMSK)
#define HWIO_GCC_VENUS_TBU_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_VENUS_TBU_CBCR_ADDR, m)
#define HWIO_GCC_VENUS_TBU_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_VENUS_TBU_CBCR_ADDR,v)
#define HWIO_GCC_VENUS_TBU_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_VENUS_TBU_CBCR_ADDR,m,v,HWIO_GCC_VENUS_TBU_CBCR_IN)
#define HWIO_GCC_VENUS_TBU_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_VENUS_TBU_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_VENUS_TBU_CBCR_FORCE_MEM_CORE_ON_BMSK                                              0x4000
#define HWIO_GCC_VENUS_TBU_CBCR_FORCE_MEM_CORE_ON_SHFT                                                 0xe
#define HWIO_GCC_VENUS_TBU_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                            0x2000
#define HWIO_GCC_VENUS_TBU_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                               0xd
#define HWIO_GCC_VENUS_TBU_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                           0x1000
#define HWIO_GCC_VENUS_TBU_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                              0xc
#define HWIO_GCC_VENUS_TBU_CBCR_WAKEUP_BMSK                                                          0xf00
#define HWIO_GCC_VENUS_TBU_CBCR_WAKEUP_SHFT                                                            0x8
#define HWIO_GCC_VENUS_TBU_CBCR_SLEEP_BMSK                                                            0xf0
#define HWIO_GCC_VENUS_TBU_CBCR_SLEEP_SHFT                                                             0x4

#define HWIO_GCC_APSS_TCU_ASYNC_CBCR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x00012098)
#define HWIO_GCC_APSS_TCU_ASYNC_CBCR_RMSK                                                       0x80007ff0
#define HWIO_GCC_APSS_TCU_ASYNC_CBCR_IN          \
        in_dword_masked(HWIO_GCC_APSS_TCU_ASYNC_CBCR_ADDR, HWIO_GCC_APSS_TCU_ASYNC_CBCR_RMSK)
#define HWIO_GCC_APSS_TCU_ASYNC_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_APSS_TCU_ASYNC_CBCR_ADDR, m)
#define HWIO_GCC_APSS_TCU_ASYNC_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_APSS_TCU_ASYNC_CBCR_ADDR,v)
#define HWIO_GCC_APSS_TCU_ASYNC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APSS_TCU_ASYNC_CBCR_ADDR,m,v,HWIO_GCC_APSS_TCU_ASYNC_CBCR_IN)
#define HWIO_GCC_APSS_TCU_ASYNC_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_GCC_APSS_TCU_ASYNC_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_GCC_APSS_TCU_ASYNC_CBCR_FORCE_MEM_CORE_ON_BMSK                                         0x4000
#define HWIO_GCC_APSS_TCU_ASYNC_CBCR_FORCE_MEM_CORE_ON_SHFT                                            0xe
#define HWIO_GCC_APSS_TCU_ASYNC_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                       0x2000
#define HWIO_GCC_APSS_TCU_ASYNC_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                          0xd
#define HWIO_GCC_APSS_TCU_ASYNC_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                      0x1000
#define HWIO_GCC_APSS_TCU_ASYNC_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                         0xc
#define HWIO_GCC_APSS_TCU_ASYNC_CBCR_WAKEUP_BMSK                                                     0xf00
#define HWIO_GCC_APSS_TCU_ASYNC_CBCR_WAKEUP_SHFT                                                       0x8
#define HWIO_GCC_APSS_TCU_ASYNC_CBCR_SLEEP_BMSK                                                       0xf0
#define HWIO_GCC_APSS_TCU_ASYNC_CBCR_SLEEP_SHFT                                                        0x4

#define HWIO_GCC_APSS_TCU_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00012018)
#define HWIO_GCC_APSS_TCU_CBCR_RMSK                                                             0x80007ff0
#define HWIO_GCC_APSS_TCU_CBCR_IN          \
        in_dword_masked(HWIO_GCC_APSS_TCU_CBCR_ADDR, HWIO_GCC_APSS_TCU_CBCR_RMSK)
#define HWIO_GCC_APSS_TCU_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_APSS_TCU_CBCR_ADDR, m)
#define HWIO_GCC_APSS_TCU_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_APSS_TCU_CBCR_ADDR,v)
#define HWIO_GCC_APSS_TCU_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APSS_TCU_CBCR_ADDR,m,v,HWIO_GCC_APSS_TCU_CBCR_IN)
#define HWIO_GCC_APSS_TCU_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_APSS_TCU_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_APSS_TCU_CBCR_FORCE_MEM_CORE_ON_BMSK                                               0x4000
#define HWIO_GCC_APSS_TCU_CBCR_FORCE_MEM_CORE_ON_SHFT                                                  0xe
#define HWIO_GCC_APSS_TCU_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                             0x2000
#define HWIO_GCC_APSS_TCU_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                0xd
#define HWIO_GCC_APSS_TCU_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                            0x1000
#define HWIO_GCC_APSS_TCU_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                               0xc
#define HWIO_GCC_APSS_TCU_CBCR_WAKEUP_BMSK                                                           0xf00
#define HWIO_GCC_APSS_TCU_CBCR_WAKEUP_SHFT                                                             0x8
#define HWIO_GCC_APSS_TCU_CBCR_SLEEP_BMSK                                                             0xf0
#define HWIO_GCC_APSS_TCU_CBCR_SLEEP_SHFT                                                              0x4

#define HWIO_GCC_MDP_TBU_CBCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0001201c)
#define HWIO_GCC_MDP_TBU_CBCR_RMSK                                                              0x80007ff0
#define HWIO_GCC_MDP_TBU_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MDP_TBU_CBCR_ADDR, HWIO_GCC_MDP_TBU_CBCR_RMSK)
#define HWIO_GCC_MDP_TBU_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MDP_TBU_CBCR_ADDR, m)
#define HWIO_GCC_MDP_TBU_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MDP_TBU_CBCR_ADDR,v)
#define HWIO_GCC_MDP_TBU_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MDP_TBU_CBCR_ADDR,m,v,HWIO_GCC_MDP_TBU_CBCR_IN)
#define HWIO_GCC_MDP_TBU_CBCR_CLK_OFF_BMSK                                                      0x80000000
#define HWIO_GCC_MDP_TBU_CBCR_CLK_OFF_SHFT                                                            0x1f
#define HWIO_GCC_MDP_TBU_CBCR_FORCE_MEM_CORE_ON_BMSK                                                0x4000
#define HWIO_GCC_MDP_TBU_CBCR_FORCE_MEM_CORE_ON_SHFT                                                   0xe
#define HWIO_GCC_MDP_TBU_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                              0x2000
#define HWIO_GCC_MDP_TBU_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                 0xd
#define HWIO_GCC_MDP_TBU_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                             0x1000
#define HWIO_GCC_MDP_TBU_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                                0xc
#define HWIO_GCC_MDP_TBU_CBCR_WAKEUP_BMSK                                                            0xf00
#define HWIO_GCC_MDP_TBU_CBCR_WAKEUP_SHFT                                                              0x8
#define HWIO_GCC_MDP_TBU_CBCR_SLEEP_BMSK                                                              0xf0
#define HWIO_GCC_MDP_TBU_CBCR_SLEEP_SHFT                                                               0x4

#define HWIO_GCC_GFX_TCU_CBCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00012020)
#define HWIO_GCC_GFX_TCU_CBCR_RMSK                                                              0x80007ff0
#define HWIO_GCC_GFX_TCU_CBCR_IN          \
        in_dword_masked(HWIO_GCC_GFX_TCU_CBCR_ADDR, HWIO_GCC_GFX_TCU_CBCR_RMSK)
#define HWIO_GCC_GFX_TCU_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_GFX_TCU_CBCR_ADDR, m)
#define HWIO_GCC_GFX_TCU_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_GFX_TCU_CBCR_ADDR,v)
#define HWIO_GCC_GFX_TCU_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GFX_TCU_CBCR_ADDR,m,v,HWIO_GCC_GFX_TCU_CBCR_IN)
#define HWIO_GCC_GFX_TCU_CBCR_CLK_OFF_BMSK                                                      0x80000000
#define HWIO_GCC_GFX_TCU_CBCR_CLK_OFF_SHFT                                                            0x1f
#define HWIO_GCC_GFX_TCU_CBCR_FORCE_MEM_CORE_ON_BMSK                                                0x4000
#define HWIO_GCC_GFX_TCU_CBCR_FORCE_MEM_CORE_ON_SHFT                                                   0xe
#define HWIO_GCC_GFX_TCU_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                              0x2000
#define HWIO_GCC_GFX_TCU_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                 0xd
#define HWIO_GCC_GFX_TCU_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                             0x1000
#define HWIO_GCC_GFX_TCU_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                                0xc
#define HWIO_GCC_GFX_TCU_CBCR_WAKEUP_BMSK                                                            0xf00
#define HWIO_GCC_GFX_TCU_CBCR_WAKEUP_SHFT                                                              0x8
#define HWIO_GCC_GFX_TCU_CBCR_SLEEP_BMSK                                                              0xf0
#define HWIO_GCC_GFX_TCU_CBCR_SLEEP_SHFT                                                               0x4

#define HWIO_GCC_MSS_TBU_AXI_CBCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00012024)
#define HWIO_GCC_MSS_TBU_AXI_CBCR_RMSK                                                          0x80007ff0
#define HWIO_GCC_MSS_TBU_AXI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MSS_TBU_AXI_CBCR_ADDR, HWIO_GCC_MSS_TBU_AXI_CBCR_RMSK)
#define HWIO_GCC_MSS_TBU_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_TBU_AXI_CBCR_ADDR, m)
#define HWIO_GCC_MSS_TBU_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MSS_TBU_AXI_CBCR_ADDR,v)
#define HWIO_GCC_MSS_TBU_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_TBU_AXI_CBCR_ADDR,m,v,HWIO_GCC_MSS_TBU_AXI_CBCR_IN)
#define HWIO_GCC_MSS_TBU_AXI_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_MSS_TBU_AXI_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_GCC_MSS_TBU_AXI_CBCR_FORCE_MEM_CORE_ON_BMSK                                            0x4000
#define HWIO_GCC_MSS_TBU_AXI_CBCR_FORCE_MEM_CORE_ON_SHFT                                               0xe
#define HWIO_GCC_MSS_TBU_AXI_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                          0x2000
#define HWIO_GCC_MSS_TBU_AXI_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                             0xd
#define HWIO_GCC_MSS_TBU_AXI_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                         0x1000
#define HWIO_GCC_MSS_TBU_AXI_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                            0xc
#define HWIO_GCC_MSS_TBU_AXI_CBCR_WAKEUP_BMSK                                                        0xf00
#define HWIO_GCC_MSS_TBU_AXI_CBCR_WAKEUP_SHFT                                                          0x8
#define HWIO_GCC_MSS_TBU_AXI_CBCR_SLEEP_BMSK                                                          0xf0
#define HWIO_GCC_MSS_TBU_AXI_CBCR_SLEEP_SHFT                                                           0x4

#define HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00012028)
#define HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_RMSK                                                      0x80007ff0
#define HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_ADDR, HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_RMSK)
#define HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_ADDR, m)
#define HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_ADDR,v)
#define HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_ADDR,m,v,HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_IN)
#define HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_FORCE_MEM_CORE_ON_BMSK                                        0x4000
#define HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_FORCE_MEM_CORE_ON_SHFT                                           0xe
#define HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                      0x2000
#define HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                         0xd
#define HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                     0x1000
#define HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                        0xc
#define HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_WAKEUP_BMSK                                                    0xf00
#define HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_WAKEUP_SHFT                                                      0x8
#define HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_SLEEP_BMSK                                                      0xf0
#define HWIO_GCC_MSS_TBU_GSS_AXI_CBCR_SLEEP_SHFT                                                       0x4

#define HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x0001202c)
#define HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_RMSK                                                       0x80007ff0
#define HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_ADDR, HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_RMSK)
#define HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_ADDR, m)
#define HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_ADDR,v)
#define HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_ADDR,m,v,HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_IN)
#define HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_FORCE_MEM_CORE_ON_BMSK                                         0x4000
#define HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_FORCE_MEM_CORE_ON_SHFT                                            0xe
#define HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                       0x2000
#define HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                          0xd
#define HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                      0x1000
#define HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                         0xc
#define HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_WAKEUP_BMSK                                                     0xf00
#define HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_WAKEUP_SHFT                                                       0x8
#define HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_SLEEP_BMSK                                                       0xf0
#define HWIO_GCC_MSS_TBU_Q6_AXI_CBCR_SLEEP_SHFT                                                        0x4

#define HWIO_GCC_PCNOC_TBU_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00012030)
#define HWIO_GCC_PCNOC_TBU_CBCR_RMSK                                                            0x80007ff0
#define HWIO_GCC_PCNOC_TBU_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_TBU_CBCR_ADDR, HWIO_GCC_PCNOC_TBU_CBCR_RMSK)
#define HWIO_GCC_PCNOC_TBU_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_TBU_CBCR_ADDR, m)
#define HWIO_GCC_PCNOC_TBU_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_TBU_CBCR_ADDR,v)
#define HWIO_GCC_PCNOC_TBU_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_TBU_CBCR_ADDR,m,v,HWIO_GCC_PCNOC_TBU_CBCR_IN)
#define HWIO_GCC_PCNOC_TBU_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_PCNOC_TBU_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_PCNOC_TBU_CBCR_FORCE_MEM_CORE_ON_BMSK                                              0x4000
#define HWIO_GCC_PCNOC_TBU_CBCR_FORCE_MEM_CORE_ON_SHFT                                                 0xe
#define HWIO_GCC_PCNOC_TBU_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                            0x2000
#define HWIO_GCC_PCNOC_TBU_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                               0xd
#define HWIO_GCC_PCNOC_TBU_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                           0x1000
#define HWIO_GCC_PCNOC_TBU_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                              0xc
#define HWIO_GCC_PCNOC_TBU_CBCR_WAKEUP_BMSK                                                          0xf00
#define HWIO_GCC_PCNOC_TBU_CBCR_WAKEUP_SHFT                                                            0x8
#define HWIO_GCC_PCNOC_TBU_CBCR_SLEEP_BMSK                                                            0xf0
#define HWIO_GCC_PCNOC_TBU_CBCR_SLEEP_SHFT                                                             0x4

#define HWIO_GCC_PRONTO_TBU_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00012048)
#define HWIO_GCC_PRONTO_TBU_CBCR_RMSK                                                           0x80000000
#define HWIO_GCC_PRONTO_TBU_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PRONTO_TBU_CBCR_ADDR, HWIO_GCC_PRONTO_TBU_CBCR_RMSK)
#define HWIO_GCC_PRONTO_TBU_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PRONTO_TBU_CBCR_ADDR, m)
#define HWIO_GCC_PRONTO_TBU_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_PRONTO_TBU_CBCR_CLK_OFF_SHFT                                                         0x1f

#define HWIO_GCC_JPEG_TBU_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00012034)
#define HWIO_GCC_JPEG_TBU_CBCR_RMSK                                                             0x80007ff0
#define HWIO_GCC_JPEG_TBU_CBCR_IN          \
        in_dword_masked(HWIO_GCC_JPEG_TBU_CBCR_ADDR, HWIO_GCC_JPEG_TBU_CBCR_RMSK)
#define HWIO_GCC_JPEG_TBU_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_JPEG_TBU_CBCR_ADDR, m)
#define HWIO_GCC_JPEG_TBU_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_JPEG_TBU_CBCR_ADDR,v)
#define HWIO_GCC_JPEG_TBU_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_JPEG_TBU_CBCR_ADDR,m,v,HWIO_GCC_JPEG_TBU_CBCR_IN)
#define HWIO_GCC_JPEG_TBU_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_JPEG_TBU_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_JPEG_TBU_CBCR_FORCE_MEM_CORE_ON_BMSK                                               0x4000
#define HWIO_GCC_JPEG_TBU_CBCR_FORCE_MEM_CORE_ON_SHFT                                                  0xe
#define HWIO_GCC_JPEG_TBU_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                             0x2000
#define HWIO_GCC_JPEG_TBU_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                0xd
#define HWIO_GCC_JPEG_TBU_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                            0x1000
#define HWIO_GCC_JPEG_TBU_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                               0xc
#define HWIO_GCC_JPEG_TBU_CBCR_WAKEUP_BMSK                                                           0xf00
#define HWIO_GCC_JPEG_TBU_CBCR_WAKEUP_SHFT                                                             0x8
#define HWIO_GCC_JPEG_TBU_CBCR_SLEEP_BMSK                                                             0xf0
#define HWIO_GCC_JPEG_TBU_CBCR_SLEEP_SHFT                                                              0x4

#define HWIO_GCC_SMMU_CFG_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00012038)
#define HWIO_GCC_SMMU_CFG_CBCR_RMSK                                                             0xf0008000
#define HWIO_GCC_SMMU_CFG_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SMMU_CFG_CBCR_ADDR, HWIO_GCC_SMMU_CFG_CBCR_RMSK)
#define HWIO_GCC_SMMU_CFG_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SMMU_CFG_CBCR_ADDR, m)
#define HWIO_GCC_SMMU_CFG_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SMMU_CFG_CBCR_ADDR,v)
#define HWIO_GCC_SMMU_CFG_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SMMU_CFG_CBCR_ADDR,m,v,HWIO_GCC_SMMU_CFG_CBCR_IN)
#define HWIO_GCC_SMMU_CFG_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_SMMU_CFG_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_SMMU_CFG_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                    0x70000000
#define HWIO_GCC_SMMU_CFG_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                          0x1c
#define HWIO_GCC_SMMU_CFG_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                            0x8000
#define HWIO_GCC_SMMU_CFG_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                               0xf

#define HWIO_GCC_VFE_TBU_CBCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0001203c)
#define HWIO_GCC_VFE_TBU_CBCR_RMSK                                                              0x80007ff0
#define HWIO_GCC_VFE_TBU_CBCR_IN          \
        in_dword_masked(HWIO_GCC_VFE_TBU_CBCR_ADDR, HWIO_GCC_VFE_TBU_CBCR_RMSK)
#define HWIO_GCC_VFE_TBU_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_VFE_TBU_CBCR_ADDR, m)
#define HWIO_GCC_VFE_TBU_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_VFE_TBU_CBCR_ADDR,v)
#define HWIO_GCC_VFE_TBU_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_VFE_TBU_CBCR_ADDR,m,v,HWIO_GCC_VFE_TBU_CBCR_IN)
#define HWIO_GCC_VFE_TBU_CBCR_CLK_OFF_BMSK                                                      0x80000000
#define HWIO_GCC_VFE_TBU_CBCR_CLK_OFF_SHFT                                                            0x1f
#define HWIO_GCC_VFE_TBU_CBCR_FORCE_MEM_CORE_ON_BMSK                                                0x4000
#define HWIO_GCC_VFE_TBU_CBCR_FORCE_MEM_CORE_ON_SHFT                                                   0xe
#define HWIO_GCC_VFE_TBU_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                              0x2000
#define HWIO_GCC_VFE_TBU_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                 0xd
#define HWIO_GCC_VFE_TBU_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                             0x1000
#define HWIO_GCC_VFE_TBU_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                                0xc
#define HWIO_GCC_VFE_TBU_CBCR_WAKEUP_BMSK                                                            0xf00
#define HWIO_GCC_VFE_TBU_CBCR_WAKEUP_SHFT                                                              0x8
#define HWIO_GCC_VFE_TBU_CBCR_SLEEP_BMSK                                                              0xf0
#define HWIO_GCC_VFE_TBU_CBCR_SLEEP_SHFT                                                               0x4

#define HWIO_GCC_SMMU_XPU_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00017010)
#define HWIO_GCC_SMMU_XPU_CBCR_RMSK                                                             0xf0008001
#define HWIO_GCC_SMMU_XPU_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SMMU_XPU_CBCR_ADDR, HWIO_GCC_SMMU_XPU_CBCR_RMSK)
#define HWIO_GCC_SMMU_XPU_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SMMU_XPU_CBCR_ADDR, m)
#define HWIO_GCC_SMMU_XPU_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SMMU_XPU_CBCR_ADDR,v)
#define HWIO_GCC_SMMU_XPU_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SMMU_XPU_CBCR_ADDR,m,v,HWIO_GCC_SMMU_XPU_CBCR_IN)
#define HWIO_GCC_SMMU_XPU_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_SMMU_XPU_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_SMMU_XPU_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                    0x70000000
#define HWIO_GCC_SMMU_XPU_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                          0x1c
#define HWIO_GCC_SMMU_XPU_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                            0x8000
#define HWIO_GCC_SMMU_XPU_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                               0xf
#define HWIO_GCC_SMMU_XPU_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_GCC_SMMU_XPU_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_GCC_GTCU_AHB_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00012044)
#define HWIO_GCC_GTCU_AHB_CBCR_RMSK                                                             0x80000000
#define HWIO_GCC_GTCU_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_GTCU_AHB_CBCR_ADDR, HWIO_GCC_GTCU_AHB_CBCR_RMSK)
#define HWIO_GCC_GTCU_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_GTCU_AHB_CBCR_ADDR, m)
#define HWIO_GCC_GTCU_AHB_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_GTCU_AHB_CBCR_CLK_OFF_SHFT                                                           0x1f

#define HWIO_GCC_GTCU_AHB_BRIDGE_CBCR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00012094)
#define HWIO_GCC_GTCU_AHB_BRIDGE_CBCR_RMSK                                                      0x80000000
#define HWIO_GCC_GTCU_AHB_BRIDGE_CBCR_IN          \
        in_dword_masked(HWIO_GCC_GTCU_AHB_BRIDGE_CBCR_ADDR, HWIO_GCC_GTCU_AHB_BRIDGE_CBCR_RMSK)
#define HWIO_GCC_GTCU_AHB_BRIDGE_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_GTCU_AHB_BRIDGE_CBCR_ADDR, m)
#define HWIO_GCC_GTCU_AHB_BRIDGE_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_GCC_GTCU_AHB_BRIDGE_CBCR_CLK_OFF_SHFT                                                    0x1f

#define HWIO_GCC_SMMU_CATS_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0007c004)
#define HWIO_GCC_SMMU_CATS_CBCR_RMSK                                                            0x80000001
#define HWIO_GCC_SMMU_CATS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SMMU_CATS_CBCR_ADDR, HWIO_GCC_SMMU_CATS_CBCR_RMSK)
#define HWIO_GCC_SMMU_CATS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SMMU_CATS_CBCR_ADDR, m)
#define HWIO_GCC_SMMU_CATS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SMMU_CATS_CBCR_ADDR,v)
#define HWIO_GCC_SMMU_CATS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SMMU_CATS_CBCR_ADDR,m,v,HWIO_GCC_SMMU_CATS_CBCR_IN)
#define HWIO_GCC_SMMU_CATS_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_SMMU_CATS_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_SMMU_CATS_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_SMMU_CATS_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_SMMU_CATS_BCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0007c000)
#define HWIO_GCC_SMMU_CATS_BCR_RMSK                                                                    0x1
#define HWIO_GCC_SMMU_CATS_BCR_IN          \
        in_dword_masked(HWIO_GCC_SMMU_CATS_BCR_ADDR, HWIO_GCC_SMMU_CATS_BCR_RMSK)
#define HWIO_GCC_SMMU_CATS_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SMMU_CATS_BCR_ADDR, m)
#define HWIO_GCC_SMMU_CATS_BCR_OUT(v)      \
        out_dword(HWIO_GCC_SMMU_CATS_BCR_ADDR,v)
#define HWIO_GCC_SMMU_CATS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SMMU_CATS_BCR_ADDR,m,v,HWIO_GCC_SMMU_CATS_BCR_IN)
#define HWIO_GCC_SMMU_CATS_BCR_BLK_ARES_BMSK                                                           0x1
#define HWIO_GCC_SMMU_CATS_BCR_BLK_ARES_SHFT                                                           0x0

#define HWIO_GCC_SMMU_BCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00012000)
#define HWIO_GCC_SMMU_BCR_RMSK                                                                         0x1
#define HWIO_GCC_SMMU_BCR_IN          \
        in_dword_masked(HWIO_GCC_SMMU_BCR_ADDR, HWIO_GCC_SMMU_BCR_RMSK)
#define HWIO_GCC_SMMU_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SMMU_BCR_ADDR, m)
#define HWIO_GCC_SMMU_BCR_OUT(v)      \
        out_dword(HWIO_GCC_SMMU_BCR_ADDR,v)
#define HWIO_GCC_SMMU_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SMMU_BCR_ADDR,m,v,HWIO_GCC_SMMU_BCR_IN)
#define HWIO_GCC_SMMU_BCR_BLK_ARES_BMSK                                                                0x1
#define HWIO_GCC_SMMU_BCR_BLK_ARES_SHFT                                                                0x0

#define HWIO_GCC_APSS_TCU_BCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00012050)
#define HWIO_GCC_APSS_TCU_BCR_RMSK                                                                     0x1
#define HWIO_GCC_APSS_TCU_BCR_IN          \
        in_dword_masked(HWIO_GCC_APSS_TCU_BCR_ADDR, HWIO_GCC_APSS_TCU_BCR_RMSK)
#define HWIO_GCC_APSS_TCU_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_APSS_TCU_BCR_ADDR, m)
#define HWIO_GCC_APSS_TCU_BCR_OUT(v)      \
        out_dword(HWIO_GCC_APSS_TCU_BCR_ADDR,v)
#define HWIO_GCC_APSS_TCU_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APSS_TCU_BCR_ADDR,m,v,HWIO_GCC_APSS_TCU_BCR_IN)
#define HWIO_GCC_APSS_TCU_BCR_BLK_ARES_BMSK                                                            0x1
#define HWIO_GCC_APSS_TCU_BCR_BLK_ARES_SHFT                                                            0x0

#define HWIO_GCC_MDP_TBU_BCR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00062000)
#define HWIO_GCC_MDP_TBU_BCR_RMSK                                                                      0x1
#define HWIO_GCC_MDP_TBU_BCR_IN          \
        in_dword_masked(HWIO_GCC_MDP_TBU_BCR_ADDR, HWIO_GCC_MDP_TBU_BCR_RMSK)
#define HWIO_GCC_MDP_TBU_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MDP_TBU_BCR_ADDR, m)
#define HWIO_GCC_MDP_TBU_BCR_OUT(v)      \
        out_dword(HWIO_GCC_MDP_TBU_BCR_ADDR,v)
#define HWIO_GCC_MDP_TBU_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MDP_TBU_BCR_ADDR,m,v,HWIO_GCC_MDP_TBU_BCR_IN)
#define HWIO_GCC_MDP_TBU_BCR_BLK_ARES_BMSK                                                             0x1
#define HWIO_GCC_MDP_TBU_BCR_BLK_ARES_SHFT                                                             0x0

#define HWIO_GCC_GFX_TBU_BCR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00063000)
#define HWIO_GCC_GFX_TBU_BCR_RMSK                                                                      0x1
#define HWIO_GCC_GFX_TBU_BCR_IN          \
        in_dword_masked(HWIO_GCC_GFX_TBU_BCR_ADDR, HWIO_GCC_GFX_TBU_BCR_RMSK)
#define HWIO_GCC_GFX_TBU_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_GFX_TBU_BCR_ADDR, m)
#define HWIO_GCC_GFX_TBU_BCR_OUT(v)      \
        out_dword(HWIO_GCC_GFX_TBU_BCR_ADDR,v)
#define HWIO_GCC_GFX_TBU_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GFX_TBU_BCR_ADDR,m,v,HWIO_GCC_GFX_TBU_BCR_IN)
#define HWIO_GCC_GFX_TBU_BCR_BLK_ARES_BMSK                                                             0x1
#define HWIO_GCC_GFX_TBU_BCR_BLK_ARES_SHFT                                                             0x0

#define HWIO_GCC_GFX_TCU_BCR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00064000)
#define HWIO_GCC_GFX_TCU_BCR_RMSK                                                                      0x1
#define HWIO_GCC_GFX_TCU_BCR_IN          \
        in_dword_masked(HWIO_GCC_GFX_TCU_BCR_ADDR, HWIO_GCC_GFX_TCU_BCR_RMSK)
#define HWIO_GCC_GFX_TCU_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_GFX_TCU_BCR_ADDR, m)
#define HWIO_GCC_GFX_TCU_BCR_OUT(v)      \
        out_dword(HWIO_GCC_GFX_TCU_BCR_ADDR,v)
#define HWIO_GCC_GFX_TCU_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GFX_TCU_BCR_ADDR,m,v,HWIO_GCC_GFX_TCU_BCR_IN)
#define HWIO_GCC_GFX_TCU_BCR_BLK_ARES_BMSK                                                             0x1
#define HWIO_GCC_GFX_TCU_BCR_BLK_ARES_SHFT                                                             0x0

#define HWIO_GCC_MSS_TBU_AXI_BCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00065000)
#define HWIO_GCC_MSS_TBU_AXI_BCR_RMSK                                                                  0x1
#define HWIO_GCC_MSS_TBU_AXI_BCR_IN          \
        in_dword_masked(HWIO_GCC_MSS_TBU_AXI_BCR_ADDR, HWIO_GCC_MSS_TBU_AXI_BCR_RMSK)
#define HWIO_GCC_MSS_TBU_AXI_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_TBU_AXI_BCR_ADDR, m)
#define HWIO_GCC_MSS_TBU_AXI_BCR_OUT(v)      \
        out_dword(HWIO_GCC_MSS_TBU_AXI_BCR_ADDR,v)
#define HWIO_GCC_MSS_TBU_AXI_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_TBU_AXI_BCR_ADDR,m,v,HWIO_GCC_MSS_TBU_AXI_BCR_IN)
#define HWIO_GCC_MSS_TBU_AXI_BCR_BLK_ARES_BMSK                                                         0x1
#define HWIO_GCC_MSS_TBU_AXI_BCR_BLK_ARES_SHFT                                                         0x0

#define HWIO_GCC_MSS_TBU_Q6_AXI_BCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00067000)
#define HWIO_GCC_MSS_TBU_Q6_AXI_BCR_RMSK                                                               0x1
#define HWIO_GCC_MSS_TBU_Q6_AXI_BCR_IN          \
        in_dword_masked(HWIO_GCC_MSS_TBU_Q6_AXI_BCR_ADDR, HWIO_GCC_MSS_TBU_Q6_AXI_BCR_RMSK)
#define HWIO_GCC_MSS_TBU_Q6_AXI_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_TBU_Q6_AXI_BCR_ADDR, m)
#define HWIO_GCC_MSS_TBU_Q6_AXI_BCR_OUT(v)      \
        out_dword(HWIO_GCC_MSS_TBU_Q6_AXI_BCR_ADDR,v)
#define HWIO_GCC_MSS_TBU_Q6_AXI_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_TBU_Q6_AXI_BCR_ADDR,m,v,HWIO_GCC_MSS_TBU_Q6_AXI_BCR_IN)
#define HWIO_GCC_MSS_TBU_Q6_AXI_BCR_BLK_ARES_BMSK                                                      0x1
#define HWIO_GCC_MSS_TBU_Q6_AXI_BCR_BLK_ARES_SHFT                                                      0x0

#define HWIO_GCC_MSS_TBU_GSS_AXI_BCR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x00066000)
#define HWIO_GCC_MSS_TBU_GSS_AXI_BCR_RMSK                                                              0x1
#define HWIO_GCC_MSS_TBU_GSS_AXI_BCR_IN          \
        in_dword_masked(HWIO_GCC_MSS_TBU_GSS_AXI_BCR_ADDR, HWIO_GCC_MSS_TBU_GSS_AXI_BCR_RMSK)
#define HWIO_GCC_MSS_TBU_GSS_AXI_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_TBU_GSS_AXI_BCR_ADDR, m)
#define HWIO_GCC_MSS_TBU_GSS_AXI_BCR_OUT(v)      \
        out_dword(HWIO_GCC_MSS_TBU_GSS_AXI_BCR_ADDR,v)
#define HWIO_GCC_MSS_TBU_GSS_AXI_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_TBU_GSS_AXI_BCR_ADDR,m,v,HWIO_GCC_MSS_TBU_GSS_AXI_BCR_IN)
#define HWIO_GCC_MSS_TBU_GSS_AXI_BCR_BLK_ARES_BMSK                                                     0x1
#define HWIO_GCC_MSS_TBU_GSS_AXI_BCR_BLK_ARES_SHFT                                                     0x0

#define HWIO_GCC_GTCU_AHB_BCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00068000)
#define HWIO_GCC_GTCU_AHB_BCR_RMSK                                                                     0x1
#define HWIO_GCC_GTCU_AHB_BCR_IN          \
        in_dword_masked(HWIO_GCC_GTCU_AHB_BCR_ADDR, HWIO_GCC_GTCU_AHB_BCR_RMSK)
#define HWIO_GCC_GTCU_AHB_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_GTCU_AHB_BCR_ADDR, m)
#define HWIO_GCC_GTCU_AHB_BCR_OUT(v)      \
        out_dword(HWIO_GCC_GTCU_AHB_BCR_ADDR,v)
#define HWIO_GCC_GTCU_AHB_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GTCU_AHB_BCR_ADDR,m,v,HWIO_GCC_GTCU_AHB_BCR_IN)
#define HWIO_GCC_GTCU_AHB_BCR_BLK_ARES_BMSK                                                            0x1
#define HWIO_GCC_GTCU_AHB_BCR_BLK_ARES_SHFT                                                            0x0

#define HWIO_GCC_SMMU_XPU_BCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00012054)
#define HWIO_GCC_SMMU_XPU_BCR_RMSK                                                                     0x1
#define HWIO_GCC_SMMU_XPU_BCR_IN          \
        in_dword_masked(HWIO_GCC_SMMU_XPU_BCR_ADDR, HWIO_GCC_SMMU_XPU_BCR_RMSK)
#define HWIO_GCC_SMMU_XPU_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SMMU_XPU_BCR_ADDR, m)
#define HWIO_GCC_SMMU_XPU_BCR_OUT(v)      \
        out_dword(HWIO_GCC_SMMU_XPU_BCR_ADDR,v)
#define HWIO_GCC_SMMU_XPU_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SMMU_XPU_BCR_ADDR,m,v,HWIO_GCC_SMMU_XPU_BCR_IN)
#define HWIO_GCC_SMMU_XPU_BCR_BLK_ARES_BMSK                                                            0x1
#define HWIO_GCC_SMMU_XPU_BCR_BLK_ARES_SHFT                                                            0x0

#define HWIO_GCC_SMMU_CFG_BCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00069000)
#define HWIO_GCC_SMMU_CFG_BCR_RMSK                                                                     0x1
#define HWIO_GCC_SMMU_CFG_BCR_IN          \
        in_dword_masked(HWIO_GCC_SMMU_CFG_BCR_ADDR, HWIO_GCC_SMMU_CFG_BCR_RMSK)
#define HWIO_GCC_SMMU_CFG_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SMMU_CFG_BCR_ADDR, m)
#define HWIO_GCC_SMMU_CFG_BCR_OUT(v)      \
        out_dword(HWIO_GCC_SMMU_CFG_BCR_ADDR,v)
#define HWIO_GCC_SMMU_CFG_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SMMU_CFG_BCR_ADDR,m,v,HWIO_GCC_SMMU_CFG_BCR_IN)
#define HWIO_GCC_SMMU_CFG_BCR_BLK_ARES_BMSK                                                            0x1
#define HWIO_GCC_SMMU_CFG_BCR_BLK_ARES_SHFT                                                            0x0

#define HWIO_GCC_VFE_TBU_BCR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0006a000)
#define HWIO_GCC_VFE_TBU_BCR_RMSK                                                                      0x1
#define HWIO_GCC_VFE_TBU_BCR_IN          \
        in_dword_masked(HWIO_GCC_VFE_TBU_BCR_ADDR, HWIO_GCC_VFE_TBU_BCR_RMSK)
#define HWIO_GCC_VFE_TBU_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_VFE_TBU_BCR_ADDR, m)
#define HWIO_GCC_VFE_TBU_BCR_OUT(v)      \
        out_dword(HWIO_GCC_VFE_TBU_BCR_ADDR,v)
#define HWIO_GCC_VFE_TBU_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_VFE_TBU_BCR_ADDR,m,v,HWIO_GCC_VFE_TBU_BCR_IN)
#define HWIO_GCC_VFE_TBU_BCR_BLK_ARES_BMSK                                                             0x1
#define HWIO_GCC_VFE_TBU_BCR_BLK_ARES_SHFT                                                             0x0

#define HWIO_GCC_VENUS_TBU_BCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0006b000)
#define HWIO_GCC_VENUS_TBU_BCR_RMSK                                                                    0x1
#define HWIO_GCC_VENUS_TBU_BCR_IN          \
        in_dword_masked(HWIO_GCC_VENUS_TBU_BCR_ADDR, HWIO_GCC_VENUS_TBU_BCR_RMSK)
#define HWIO_GCC_VENUS_TBU_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_VENUS_TBU_BCR_ADDR, m)
#define HWIO_GCC_VENUS_TBU_BCR_OUT(v)      \
        out_dword(HWIO_GCC_VENUS_TBU_BCR_ADDR,v)
#define HWIO_GCC_VENUS_TBU_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_VENUS_TBU_BCR_ADDR,m,v,HWIO_GCC_VENUS_TBU_BCR_IN)
#define HWIO_GCC_VENUS_TBU_BCR_BLK_ARES_BMSK                                                           0x1
#define HWIO_GCC_VENUS_TBU_BCR_BLK_ARES_SHFT                                                           0x0

#define HWIO_GCC_JPEG_TBU_BCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0006c000)
#define HWIO_GCC_JPEG_TBU_BCR_RMSK                                                                     0x1
#define HWIO_GCC_JPEG_TBU_BCR_IN          \
        in_dword_masked(HWIO_GCC_JPEG_TBU_BCR_ADDR, HWIO_GCC_JPEG_TBU_BCR_RMSK)
#define HWIO_GCC_JPEG_TBU_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_JPEG_TBU_BCR_ADDR, m)
#define HWIO_GCC_JPEG_TBU_BCR_OUT(v)      \
        out_dword(HWIO_GCC_JPEG_TBU_BCR_ADDR,v)
#define HWIO_GCC_JPEG_TBU_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_JPEG_TBU_BCR_ADDR,m,v,HWIO_GCC_JPEG_TBU_BCR_IN)
#define HWIO_GCC_JPEG_TBU_BCR_BLK_ARES_BMSK                                                            0x1
#define HWIO_GCC_JPEG_TBU_BCR_BLK_ARES_SHFT                                                            0x0

#define HWIO_GCC_PRONTO_TBU_BCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0006d000)
#define HWIO_GCC_PRONTO_TBU_BCR_RMSK                                                                   0x1
#define HWIO_GCC_PRONTO_TBU_BCR_IN          \
        in_dword_masked(HWIO_GCC_PRONTO_TBU_BCR_ADDR, HWIO_GCC_PRONTO_TBU_BCR_RMSK)
#define HWIO_GCC_PRONTO_TBU_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PRONTO_TBU_BCR_ADDR, m)
#define HWIO_GCC_PRONTO_TBU_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PRONTO_TBU_BCR_ADDR,v)
#define HWIO_GCC_PRONTO_TBU_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PRONTO_TBU_BCR_ADDR,m,v,HWIO_GCC_PRONTO_TBU_BCR_IN)
#define HWIO_GCC_PRONTO_TBU_BCR_BLK_ARES_BMSK                                                          0x1
#define HWIO_GCC_PRONTO_TBU_BCR_BLK_ARES_SHFT                                                          0x0

#define HWIO_GCC_PCNOC_TBU_BCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00012058)
#define HWIO_GCC_PCNOC_TBU_BCR_RMSK                                                                    0x1
#define HWIO_GCC_PCNOC_TBU_BCR_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_TBU_BCR_ADDR, HWIO_GCC_PCNOC_TBU_BCR_RMSK)
#define HWIO_GCC_PCNOC_TBU_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_TBU_BCR_ADDR, m)
#define HWIO_GCC_PCNOC_TBU_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_TBU_BCR_ADDR,v)
#define HWIO_GCC_PCNOC_TBU_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_TBU_BCR_ADDR,m,v,HWIO_GCC_PCNOC_TBU_BCR_IN)
#define HWIO_GCC_PCNOC_TBU_BCR_BLK_ARES_BMSK                                                           0x1
#define HWIO_GCC_PCNOC_TBU_BCR_BLK_ARES_SHFT                                                           0x0

#define HWIO_GCC_RBCPR_BCR_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00033000)
#define HWIO_GCC_RBCPR_BCR_RMSK                                                                        0x1
#define HWIO_GCC_RBCPR_BCR_IN          \
        in_dword_masked(HWIO_GCC_RBCPR_BCR_ADDR, HWIO_GCC_RBCPR_BCR_RMSK)
#define HWIO_GCC_RBCPR_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_RBCPR_BCR_ADDR, m)
#define HWIO_GCC_RBCPR_BCR_OUT(v)      \
        out_dword(HWIO_GCC_RBCPR_BCR_ADDR,v)
#define HWIO_GCC_RBCPR_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RBCPR_BCR_ADDR,m,v,HWIO_GCC_RBCPR_BCR_IN)
#define HWIO_GCC_RBCPR_BCR_BLK_ARES_BMSK                                                               0x1
#define HWIO_GCC_RBCPR_BCR_BLK_ARES_SHFT                                                               0x0

#define HWIO_GCC_RBCPR_CBCR_ADDR                                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00033004)
#define HWIO_GCC_RBCPR_CBCR_RMSK                                                                0x80000001
#define HWIO_GCC_RBCPR_CBCR_IN          \
        in_dword_masked(HWIO_GCC_RBCPR_CBCR_ADDR, HWIO_GCC_RBCPR_CBCR_RMSK)
#define HWIO_GCC_RBCPR_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_RBCPR_CBCR_ADDR, m)
#define HWIO_GCC_RBCPR_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_RBCPR_CBCR_ADDR,v)
#define HWIO_GCC_RBCPR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RBCPR_CBCR_ADDR,m,v,HWIO_GCC_RBCPR_CBCR_IN)
#define HWIO_GCC_RBCPR_CBCR_CLK_OFF_BMSK                                                        0x80000000
#define HWIO_GCC_RBCPR_CBCR_CLK_OFF_SHFT                                                              0x1f
#define HWIO_GCC_RBCPR_CBCR_CLK_ENABLE_BMSK                                                            0x1
#define HWIO_GCC_RBCPR_CBCR_CLK_ENABLE_SHFT                                                            0x0

#define HWIO_GCC_RBCPR_AHB_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00033008)
#define HWIO_GCC_RBCPR_AHB_CBCR_RMSK                                                            0xf0008001
#define HWIO_GCC_RBCPR_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_RBCPR_AHB_CBCR_ADDR, HWIO_GCC_RBCPR_AHB_CBCR_RMSK)
#define HWIO_GCC_RBCPR_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_RBCPR_AHB_CBCR_ADDR, m)
#define HWIO_GCC_RBCPR_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_RBCPR_AHB_CBCR_ADDR,v)
#define HWIO_GCC_RBCPR_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RBCPR_AHB_CBCR_ADDR,m,v,HWIO_GCC_RBCPR_AHB_CBCR_IN)
#define HWIO_GCC_RBCPR_AHB_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_RBCPR_AHB_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_RBCPR_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                   0x70000000
#define HWIO_GCC_RBCPR_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                         0x1c
#define HWIO_GCC_RBCPR_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                           0x8000
#define HWIO_GCC_RBCPR_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                              0xf
#define HWIO_GCC_RBCPR_AHB_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_RBCPR_AHB_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_RBCPR_CMD_RCGR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0003300c)
#define HWIO_GCC_RBCPR_CMD_RCGR_RMSK                                                            0x80000013
#define HWIO_GCC_RBCPR_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_RBCPR_CMD_RCGR_ADDR, HWIO_GCC_RBCPR_CMD_RCGR_RMSK)
#define HWIO_GCC_RBCPR_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_RBCPR_CMD_RCGR_ADDR, m)
#define HWIO_GCC_RBCPR_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_RBCPR_CMD_RCGR_ADDR,v)
#define HWIO_GCC_RBCPR_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RBCPR_CMD_RCGR_ADDR,m,v,HWIO_GCC_RBCPR_CMD_RCGR_IN)
#define HWIO_GCC_RBCPR_CMD_RCGR_ROOT_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_RBCPR_CMD_RCGR_ROOT_OFF_SHFT                                                         0x1f
#define HWIO_GCC_RBCPR_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                   0x10
#define HWIO_GCC_RBCPR_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                    0x4
#define HWIO_GCC_RBCPR_CMD_RCGR_ROOT_EN_BMSK                                                           0x2
#define HWIO_GCC_RBCPR_CMD_RCGR_ROOT_EN_SHFT                                                           0x1
#define HWIO_GCC_RBCPR_CMD_RCGR_UPDATE_BMSK                                                            0x1
#define HWIO_GCC_RBCPR_CMD_RCGR_UPDATE_SHFT                                                            0x0

#define HWIO_GCC_RBCPR_CFG_RCGR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00033010)
#define HWIO_GCC_RBCPR_CFG_RCGR_RMSK                                                                 0x71f
#define HWIO_GCC_RBCPR_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_RBCPR_CFG_RCGR_ADDR, HWIO_GCC_RBCPR_CFG_RCGR_RMSK)
#define HWIO_GCC_RBCPR_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_RBCPR_CFG_RCGR_ADDR, m)
#define HWIO_GCC_RBCPR_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_RBCPR_CFG_RCGR_ADDR,v)
#define HWIO_GCC_RBCPR_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RBCPR_CFG_RCGR_ADDR,m,v,HWIO_GCC_RBCPR_CFG_RCGR_IN)
#define HWIO_GCC_RBCPR_CFG_RCGR_SRC_SEL_BMSK                                                         0x700
#define HWIO_GCC_RBCPR_CFG_RCGR_SRC_SEL_SHFT                                                           0x8
#define HWIO_GCC_RBCPR_CFG_RCGR_SRC_DIV_BMSK                                                          0x1f
#define HWIO_GCC_RBCPR_CFG_RCGR_SRC_DIV_SHFT                                                           0x0

#define HWIO_GCC_RPM_GPLL_ENA_VOTE_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00036000)
#define HWIO_GCC_RPM_GPLL_ENA_VOTE_RMSK                                                                0xf
#define HWIO_GCC_RPM_GPLL_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPM_GPLL_ENA_VOTE_ADDR, HWIO_GCC_RPM_GPLL_ENA_VOTE_RMSK)
#define HWIO_GCC_RPM_GPLL_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPM_GPLL_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPM_GPLL_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPM_GPLL_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPM_GPLL_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPM_GPLL_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPM_GPLL_ENA_VOTE_IN)
#define HWIO_GCC_RPM_GPLL_ENA_VOTE_BIMC_PLL_BMSK                                                       0x8
#define HWIO_GCC_RPM_GPLL_ENA_VOTE_BIMC_PLL_SHFT                                                       0x3
#define HWIO_GCC_RPM_GPLL_ENA_VOTE_GPLL2_BMSK                                                          0x4
#define HWIO_GCC_RPM_GPLL_ENA_VOTE_GPLL2_SHFT                                                          0x2
#define HWIO_GCC_RPM_GPLL_ENA_VOTE_GPLL1_BMSK                                                          0x2
#define HWIO_GCC_RPM_GPLL_ENA_VOTE_GPLL1_SHFT                                                          0x1
#define HWIO_GCC_RPM_GPLL_ENA_VOTE_GPLL0_BMSK                                                          0x1
#define HWIO_GCC_RPM_GPLL_ENA_VOTE_GPLL0_SHFT                                                          0x0

#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_ADDR                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00036004)
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_RMSK                                                    0x3ffff
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_ADDR, HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_RMSK)
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_IN)
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_IMEM_AXI_CLK_ENA_BMSK                                   0x20000
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_IMEM_AXI_CLK_ENA_SHFT                                      0x11
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_ENA_BMSK                           0x10000
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_ENA_SHFT                              0x10
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_BIMC_APSS_AXI_CLK_ENA_BMSK                               0x8000
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_BIMC_APSS_AXI_CLK_ENA_SHFT                                  0xf
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_APSS_AHB_CLK_ENA_BMSK                                    0x4000
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_APSS_AHB_CLK_ENA_SHFT                                       0xe
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_APSS_AXI_CLK_ENA_BMSK                                    0x2000
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_APSS_AXI_CLK_ENA_SHFT                                       0xd
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_MPM_AHB_CLK_ENA_BMSK                                     0x1000
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_MPM_AHB_CLK_ENA_SHFT                                        0xc
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_ENA_BMSK                             0x800
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_ENA_SHFT                               0xb
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_BLSP1_AHB_CLK_ENA_BMSK                                    0x400
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_BLSP1_AHB_CLK_ENA_SHFT                                      0xa
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_BLSP1_SLEEP_CLK_ENA_BMSK                                  0x200
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_BLSP1_SLEEP_CLK_ENA_SHFT                                    0x9
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_PRNG_AHB_CLK_ENA_BMSK                                     0x100
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_PRNG_AHB_CLK_ENA_SHFT                                       0x8
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_BOOT_ROM_AHB_CLK_ENA_BMSK                                  0x80
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_BOOT_ROM_AHB_CLK_ENA_SHFT                                   0x7
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_MSG_RAM_AHB_CLK_ENA_BMSK                                   0x40
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_MSG_RAM_AHB_CLK_ENA_SHFT                                    0x6
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_TLMM_AHB_CLK_ENA_BMSK                                      0x20
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_TLMM_AHB_CLK_ENA_SHFT                                       0x5
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_TLMM_CLK_ENA_BMSK                                          0x10
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_TLMM_CLK_ENA_SHFT                                           0x4
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_SPMI_PCNOC_AHB_CLK_ENA_BMSK                                 0x8
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_SPMI_PCNOC_AHB_CLK_ENA_SHFT                                 0x3
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_CRYPTO_CLK_ENA_BMSK                                         0x4
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_CRYPTO_CLK_ENA_SHFT                                         0x2
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AXI_CLK_ENA_BMSK                                     0x2
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AXI_CLK_ENA_SHFT                                     0x1
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AHB_CLK_ENA_BMSK                                     0x1
#define HWIO_GCC_RPM_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AHB_CLK_ENA_SHFT                                     0x0

#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0003600c)
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_RMSK                                                0x3fff
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR, HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_RMSK)
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_IN)
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_GTCU_AHB_CLK_ENA_BMSK                               0x2000
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_GTCU_AHB_CLK_ENA_SHFT                                  0xd
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_SMMU_CFG_CLK_ENA_BMSK                               0x1000
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_SMMU_CFG_CLK_ENA_SHFT                                  0xc
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_PCNOC_TBU_CLK_ENA_BMSK                               0x800
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_PCNOC_TBU_CLK_ENA_SHFT                                 0xb
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_JPEG_TBU_CLK_ENA_BMSK                                0x400
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_JPEG_TBU_CLK_ENA_SHFT                                  0xa
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_VFE_TBU_CLK_ENA_BMSK                                 0x200
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_VFE_TBU_CLK_ENA_SHFT                                   0x9
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_Q6_AXI_CLK_ENA_BMSK                          0x100
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_Q6_AXI_CLK_ENA_SHFT                            0x8
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_GSS_AXI_CLK_ENA_BMSK                          0x80
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_GSS_AXI_CLK_ENA_SHFT                           0x7
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_AXI_CLK_ENA_BMSK                              0x40
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_AXI_CLK_ENA_SHFT                               0x6
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_VENUS_TBU_CLK_ENA_BMSK                                0x20
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_VENUS_TBU_CLK_ENA_SHFT                                 0x5
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_MDP_TBU_CLK_ENA_BMSK                                  0x10
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_MDP_TBU_CLK_ENA_SHFT                                   0x4
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_GFX_TBU_CLK_ENA_BMSK                                   0x8
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_GFX_TBU_CLK_ENA_SHFT                                   0x3
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_GFX_TCU_CLK_ENA_BMSK                                   0x4
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_GFX_TCU_CLK_ENA_SHFT                                   0x2
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_APSS_TCU_CLK_ENA_BMSK                                  0x2
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_APSS_TCU_CLK_ENA_SHFT                                  0x1
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_PRONTO_TBU_CLK_ENA_BMSK                                0x1
#define HWIO_GCC_RPM_SMMU_CLOCK_BRANCH_ENA_VOTE_PRONTO_TBU_CLK_ENA_SHFT                                0x0

#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00036008)
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_RMSK                                                     0x3ffff
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_ADDR, HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_RMSK)
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_IN)
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_IMEM_AXI_CLK_SLEEP_ENA_BMSK                              0x20000
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_IMEM_AXI_CLK_SLEEP_ENA_SHFT                                 0x11
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_SLEEP_ENA_BMSK                      0x10000
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_SLEEP_ENA_SHFT                         0x10
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_BIMC_APSS_AXI_CLK_SLEEP_ENA_BMSK                          0x8000
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_BIMC_APSS_AXI_CLK_SLEEP_ENA_SHFT                             0xf
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_APSS_AHB_CLK_SLEEP_ENA_BMSK                               0x4000
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_APSS_AHB_CLK_SLEEP_ENA_SHFT                                  0xe
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_APSS_AXI_CLK_SLEEP_ENA_BMSK                               0x2000
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_APSS_AXI_CLK_SLEEP_ENA_SHFT                                  0xd
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_MPM_AHB_CLK_SLEEP_ENA_BMSK                                0x1000
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_MPM_AHB_CLK_SLEEP_ENA_SHFT                                   0xc
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_SLEEP_ENA_BMSK                        0x800
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_SLEEP_ENA_SHFT                          0xb
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_BLSP1_AHB_CLK_SLEEP_ENA_BMSK                               0x400
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_BLSP1_AHB_CLK_SLEEP_ENA_SHFT                                 0xa
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_BLSP1_SLEEP_CLK_SLEEP_ENA_BMSK                             0x200
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_BLSP1_SLEEP_CLK_SLEEP_ENA_SHFT                               0x9
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_PRNG_AHB_CLK_SLEEP_ENA_BMSK                                0x100
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_PRNG_AHB_CLK_SLEEP_ENA_SHFT                                  0x8
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_BOOT_ROM_AHB_CLK_SLEEP_ENA_BMSK                             0x80
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_BOOT_ROM_AHB_CLK_SLEEP_ENA_SHFT                              0x7
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_MSG_RAM_AHB_CLK_SLEEP_ENA_BMSK                              0x40
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_MSG_RAM_AHB_CLK_SLEEP_ENA_SHFT                               0x6
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_TLMM_AHB_CLK_SLEEP_ENA_BMSK                                 0x20
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_TLMM_AHB_CLK_SLEEP_ENA_SHFT                                  0x5
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_TLMM_CLK_SLEEP_ENA_BMSK                                     0x10
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_TLMM_CLK_SLEEP_ENA_SHFT                                      0x4
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_SPMI_PCNOC_AHB_CLK_SLEEP_ENA_BMSK                            0x8
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_SPMI_PCNOC_AHB_CLK_SLEEP_ENA_SHFT                            0x3
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_CRYPTO_CLK_SLEEP_ENA_BMSK                                    0x4
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_CRYPTO_CLK_SLEEP_ENA_SHFT                                    0x2
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AXI_CLK_SLEEP_ENA_BMSK                                0x2
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AXI_CLK_SLEEP_ENA_SHFT                                0x1
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AHB_CLK_SLEEP_ENA_BMSK                                0x1
#define HWIO_GCC_RPM_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AHB_CLK_SLEEP_ENA_SHFT                                0x0

#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00036010)
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_RMSK                                                 0x3fff
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR, HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_RMSK)
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_IN)
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_GTCU_AHB_CLK_SLEEP_ENA_BMSK                          0x2000
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_GTCU_AHB_CLK_SLEEP_ENA_SHFT                             0xd
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_SMMU_CFG_CLK_SLEEP_ENA_BMSK                          0x1000
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_SMMU_CFG_CLK_SLEEP_ENA_SHFT                             0xc
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_PCNOC_TBU_CLK_SLEEP_ENA_BMSK                          0x800
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_PCNOC_TBU_CLK_SLEEP_ENA_SHFT                            0xb
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_JPEG_TBU_CLK_SLEEP_ENA_BMSK                           0x400
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_JPEG_TBU_CLK_SLEEP_ENA_SHFT                             0xa
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_VFE_TBU_CLK_SLEEP_ENA_BMSK                            0x200
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_VFE_TBU_CLK_SLEEP_ENA_SHFT                              0x9
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_Q6_AXI_CLK_SLEEP_ENA_BMSK                     0x100
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_Q6_AXI_CLK_SLEEP_ENA_SHFT                       0x8
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_GSS_AXI_CLK_SLEEP_ENA_BMSK                     0x80
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_GSS_AXI_CLK_SLEEP_ENA_SHFT                      0x7
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_AXI_CLK_SLEEP_ENA_BMSK                         0x40
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_AXI_CLK_SLEEP_ENA_SHFT                          0x6
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_VENUS_TBU_CLK_SLEEP_ENA_BMSK                           0x20
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_VENUS_TBU_CLK_SLEEP_ENA_SHFT                            0x5
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_MDP_TBU_CLK_SLEEP_ENA_BMSK                             0x10
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_MDP_TBU_CLK_SLEEP_ENA_SHFT                              0x4
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_GFX_TBU_CLK_SLEEP_ENA_BMSK                              0x8
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_GFX_TBU_CLK_SLEEP_ENA_SHFT                              0x3
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_GFX_TCU_CLK_SLEEP_ENA_BMSK                              0x4
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_GFX_TCU_CLK_SLEEP_ENA_SHFT                              0x2
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_APSS_TCU_CLK_SLEEP_ENA_BMSK                             0x2
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_APSS_TCU_CLK_SLEEP_ENA_SHFT                             0x1
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_PRONTO_TBU_CLK_SLEEP_ENA_BMSK                           0x1
#define HWIO_GCC_RPM_SMMU_CLOCK_SLEEP_ENA_VOTE_PRONTO_TBU_CLK_SLEEP_ENA_SHFT                           0x0

#define HWIO_GCC_APCS_GPLL_ENA_VOTE_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00045000)
#define HWIO_GCC_APCS_GPLL_ENA_VOTE_RMSK                                                               0xf
#define HWIO_GCC_APCS_GPLL_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_APCS_GPLL_ENA_VOTE_ADDR, HWIO_GCC_APCS_GPLL_ENA_VOTE_RMSK)
#define HWIO_GCC_APCS_GPLL_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_APCS_GPLL_ENA_VOTE_ADDR, m)
#define HWIO_GCC_APCS_GPLL_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_APCS_GPLL_ENA_VOTE_ADDR,v)
#define HWIO_GCC_APCS_GPLL_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APCS_GPLL_ENA_VOTE_ADDR,m,v,HWIO_GCC_APCS_GPLL_ENA_VOTE_IN)
#define HWIO_GCC_APCS_GPLL_ENA_VOTE_BIMC_PLL_BMSK                                                      0x8
#define HWIO_GCC_APCS_GPLL_ENA_VOTE_BIMC_PLL_SHFT                                                      0x3
#define HWIO_GCC_APCS_GPLL_ENA_VOTE_GPLL2_BMSK                                                         0x4
#define HWIO_GCC_APCS_GPLL_ENA_VOTE_GPLL2_SHFT                                                         0x2
#define HWIO_GCC_APCS_GPLL_ENA_VOTE_GPLL1_BMSK                                                         0x2
#define HWIO_GCC_APCS_GPLL_ENA_VOTE_GPLL1_SHFT                                                         0x1
#define HWIO_GCC_APCS_GPLL_ENA_VOTE_GPLL0_BMSK                                                         0x1
#define HWIO_GCC_APCS_GPLL_ENA_VOTE_GPLL0_SHFT                                                         0x0

#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_ADDR                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00045004)
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_RMSK                                                   0x3ffff
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_ADDR, HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_RMSK)
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_ADDR, m)
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_ADDR,v)
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_ADDR,m,v,HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_IN)
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_IMEM_AXI_CLK_ENA_BMSK                                  0x20000
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_IMEM_AXI_CLK_ENA_SHFT                                     0x11
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_ENA_BMSK                          0x10000
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_ENA_SHFT                             0x10
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_BIMC_APSS_AXI_CLK_ENA_BMSK                              0x8000
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_BIMC_APSS_AXI_CLK_ENA_SHFT                                 0xf
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_APSS_AHB_CLK_ENA_BMSK                                   0x4000
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_APSS_AHB_CLK_ENA_SHFT                                      0xe
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_APSS_AXI_CLK_ENA_BMSK                                   0x2000
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_APSS_AXI_CLK_ENA_SHFT                                      0xd
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_MPM_AHB_CLK_ENA_BMSK                                    0x1000
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_MPM_AHB_CLK_ENA_SHFT                                       0xc
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_ENA_BMSK                            0x800
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_ENA_SHFT                              0xb
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_BLSP1_AHB_CLK_ENA_BMSK                                   0x400
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_BLSP1_AHB_CLK_ENA_SHFT                                     0xa
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_BLSP1_SLEEP_CLK_ENA_BMSK                                 0x200
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_BLSP1_SLEEP_CLK_ENA_SHFT                                   0x9
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_PRNG_AHB_CLK_ENA_BMSK                                    0x100
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_PRNG_AHB_CLK_ENA_SHFT                                      0x8
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_BOOT_ROM_AHB_CLK_ENA_BMSK                                 0x80
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_BOOT_ROM_AHB_CLK_ENA_SHFT                                  0x7
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_MSG_RAM_AHB_CLK_ENA_BMSK                                  0x40
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_MSG_RAM_AHB_CLK_ENA_SHFT                                   0x6
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_TLMM_AHB_CLK_ENA_BMSK                                     0x20
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_TLMM_AHB_CLK_ENA_SHFT                                      0x5
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_TLMM_CLK_ENA_BMSK                                         0x10
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_TLMM_CLK_ENA_SHFT                                          0x4
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_SPMI_PCNOC_AHB_CLK_ENA_BMSK                                0x8
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_SPMI_PCNOC_AHB_CLK_ENA_SHFT                                0x3
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_CRYPTO_CLK_ENA_BMSK                                        0x4
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_CRYPTO_CLK_ENA_SHFT                                        0x2
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AXI_CLK_ENA_BMSK                                    0x2
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AXI_CLK_ENA_SHFT                                    0x1
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AHB_CLK_ENA_BMSK                                    0x1
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AHB_CLK_ENA_SHFT                                    0x0

#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0004500c)
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_RMSK                                               0x3fff
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR, HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_RMSK)
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR, m)
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR,v)
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR,m,v,HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_IN)
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_GTCU_AHB_CLK_ENA_BMSK                              0x2000
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_GTCU_AHB_CLK_ENA_SHFT                                 0xd
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_SMMU_CFG_CLK_ENA_BMSK                              0x1000
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_SMMU_CFG_CLK_ENA_SHFT                                 0xc
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_PCNOC_TBU_CLK_ENA_BMSK                              0x800
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_PCNOC_TBU_CLK_ENA_SHFT                                0xb
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_JPEG_TBU_CLK_ENA_BMSK                               0x400
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_JPEG_TBU_CLK_ENA_SHFT                                 0xa
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_VFE_TBU_CLK_ENA_BMSK                                0x200
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_VFE_TBU_CLK_ENA_SHFT                                  0x9
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_Q6_AXI_CLK_ENA_BMSK                         0x100
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_Q6_AXI_CLK_ENA_SHFT                           0x8
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_GSS_AXI_CLK_ENA_BMSK                         0x80
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_GSS_AXI_CLK_ENA_SHFT                          0x7
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_AXI_CLK_ENA_BMSK                             0x40
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_AXI_CLK_ENA_SHFT                              0x6
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_VENUS_TBU_CLK_ENA_BMSK                               0x20
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_VENUS_TBU_CLK_ENA_SHFT                                0x5
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_MDP_TBU_CLK_ENA_BMSK                                 0x10
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_MDP_TBU_CLK_ENA_SHFT                                  0x4
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_GFX_TBU_CLK_ENA_BMSK                                  0x8
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_GFX_TBU_CLK_ENA_SHFT                                  0x3
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_GFX_TCU_CLK_ENA_BMSK                                  0x4
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_GFX_TCU_CLK_ENA_SHFT                                  0x2
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_APSS_TCU_CLK_ENA_BMSK                                 0x2
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_APSS_TCU_CLK_ENA_SHFT                                 0x1
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_PRONTO_TBU_CLK_ENA_BMSK                               0x1
#define HWIO_GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE_PRONTO_TBU_CLK_ENA_SHFT                               0x0

#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_ADDR                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00045008)
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_RMSK                                                    0x3ffff
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_ADDR, HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_RMSK)
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_ADDR, m)
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_ADDR,v)
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_ADDR,m,v,HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_IN)
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_IMEM_AXI_CLK_SLEEP_ENA_BMSK                             0x20000
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_IMEM_AXI_CLK_SLEEP_ENA_SHFT                                0x11
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_SLEEP_ENA_BMSK                     0x10000
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_SLEEP_ENA_SHFT                        0x10
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_BIMC_APSS_AXI_CLK_SLEEP_ENA_BMSK                         0x8000
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_BIMC_APSS_AXI_CLK_SLEEP_ENA_SHFT                            0xf
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_APSS_AHB_CLK_SLEEP_ENA_BMSK                              0x4000
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_APSS_AHB_CLK_SLEEP_ENA_SHFT                                 0xe
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_APSS_AXI_CLK_SLEEP_ENA_BMSK                              0x2000
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_APSS_AXI_CLK_SLEEP_ENA_SHFT                                 0xd
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_MPM_AHB_CLK_SLEEP_ENA_BMSK                               0x1000
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_MPM_AHB_CLK_SLEEP_ENA_SHFT                                  0xc
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_SLEEP_ENA_BMSK                       0x800
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_SLEEP_ENA_SHFT                         0xb
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_BLSP1_AHB_CLK_SLEEP_ENA_BMSK                              0x400
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_BLSP1_AHB_CLK_SLEEP_ENA_SHFT                                0xa
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_BLSP1_SLEEP_CLK_SLEEP_ENA_BMSK                            0x200
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_BLSP1_SLEEP_CLK_SLEEP_ENA_SHFT                              0x9
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_PRNG_AHB_CLK_SLEEP_ENA_BMSK                               0x100
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_PRNG_AHB_CLK_SLEEP_ENA_SHFT                                 0x8
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_BOOT_ROM_AHB_CLK_SLEEP_ENA_BMSK                            0x80
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_BOOT_ROM_AHB_CLK_SLEEP_ENA_SHFT                             0x7
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_MSG_RAM_AHB_CLK_SLEEP_ENA_BMSK                             0x40
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_MSG_RAM_AHB_CLK_SLEEP_ENA_SHFT                              0x6
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_TLMM_AHB_CLK_SLEEP_ENA_BMSK                                0x20
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_TLMM_AHB_CLK_SLEEP_ENA_SHFT                                 0x5
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_TLMM_CLK_SLEEP_ENA_BMSK                                    0x10
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_TLMM_CLK_SLEEP_ENA_SHFT                                     0x4
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_SPMI_PCNOC_AHB_CLK_SLEEP_ENA_BMSK                           0x8
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_SPMI_PCNOC_AHB_CLK_SLEEP_ENA_SHFT                           0x3
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_CRYPTO_CLK_SLEEP_ENA_BMSK                                   0x4
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_CRYPTO_CLK_SLEEP_ENA_SHFT                                   0x2
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AXI_CLK_SLEEP_ENA_BMSK                               0x2
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AXI_CLK_SLEEP_ENA_SHFT                               0x1
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AHB_CLK_SLEEP_ENA_BMSK                               0x1
#define HWIO_GCC_APCS_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AHB_CLK_SLEEP_ENA_SHFT                               0x0

#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00045010)
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_RMSK                                                0x3fff
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR, HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_RMSK)
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR, m)
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR,v)
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR,m,v,HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_IN)
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_GTCU_AHB_CLK_SLEEP_ENA_BMSK                         0x2000
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_GTCU_AHB_CLK_SLEEP_ENA_SHFT                            0xd
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_SMMU_CFG_CLK_SLEEP_ENA_BMSK                         0x1000
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_SMMU_CFG_CLK_SLEEP_ENA_SHFT                            0xc
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_PCNOC_TBU_CLK_SLEEP_ENA_BMSK                         0x800
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_PCNOC_TBU_CLK_SLEEP_ENA_SHFT                           0xb
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_JPEG_TBU_CLK_SLEEP_ENA_BMSK                          0x400
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_JPEG_TBU_CLK_SLEEP_ENA_SHFT                            0xa
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_VFE_TBU_CLK_SLEEP_ENA_BMSK                           0x200
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_VFE_TBU_CLK_SLEEP_ENA_SHFT                             0x9
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_Q6_AXI_CLK_SLEEP_ENA_BMSK                    0x100
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_Q6_AXI_CLK_SLEEP_ENA_SHFT                      0x8
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_GSS_AXI_CLK_SLEEP_ENA_BMSK                    0x80
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_GSS_AXI_CLK_SLEEP_ENA_SHFT                     0x7
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_AXI_CLK_SLEEP_ENA_BMSK                        0x40
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_AXI_CLK_SLEEP_ENA_SHFT                         0x6
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_VENUS_TBU_CLK_SLEEP_ENA_BMSK                          0x20
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_VENUS_TBU_CLK_SLEEP_ENA_SHFT                           0x5
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_MDP_TBU_CLK_SLEEP_ENA_BMSK                            0x10
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_MDP_TBU_CLK_SLEEP_ENA_SHFT                             0x4
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_GFX_TBU_CLK_SLEEP_ENA_BMSK                             0x8
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_GFX_TBU_CLK_SLEEP_ENA_SHFT                             0x3
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_GFX_TCU_CLK_SLEEP_ENA_BMSK                             0x4
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_GFX_TCU_CLK_SLEEP_ENA_SHFT                             0x2
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_APSS_TCU_CLK_SLEEP_ENA_BMSK                            0x2
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_APSS_TCU_CLK_SLEEP_ENA_SHFT                            0x1
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_PRONTO_TBU_CLK_SLEEP_ENA_BMSK                          0x1
#define HWIO_GCC_APCS_SMMU_CLOCK_SLEEP_ENA_VOTE_PRONTO_TBU_CLK_SLEEP_ENA_SHFT                          0x0

#define HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00013010)
#define HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_RMSK                                                            0xf
#define HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_ADDR, HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_RMSK)
#define HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_ADDR, m)
#define HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_ADDR,v)
#define HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_ADDR,m,v,HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_IN)
#define HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_BIMC_PLL_BMSK                                                   0x8
#define HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_BIMC_PLL_SHFT                                                   0x3
#define HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_GPLL2_BMSK                                                      0x4
#define HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_GPLL2_SHFT                                                      0x2
#define HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_GPLL1_BMSK                                                      0x2
#define HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_GPLL1_SHFT                                                      0x1
#define HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_GPLL0_BMSK                                                      0x1
#define HWIO_GCC_APCS_TZ_GPLL_ENA_VOTE_GPLL0_SHFT                                                      0x0

#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_ADDR                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00013014)
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_RMSK                                                0x3ffff
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_ADDR, HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_RMSK)
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_ADDR, m)
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_ADDR,v)
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_ADDR,m,v,HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_IN)
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_IMEM_AXI_CLK_ENA_BMSK                               0x20000
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_IMEM_AXI_CLK_ENA_SHFT                                  0x11
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_ENA_BMSK                       0x10000
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_ENA_SHFT                          0x10
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_BIMC_APSS_AXI_CLK_ENA_BMSK                           0x8000
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_BIMC_APSS_AXI_CLK_ENA_SHFT                              0xf
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_APSS_AHB_CLK_ENA_BMSK                                0x4000
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_APSS_AHB_CLK_ENA_SHFT                                   0xe
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_APSS_AXI_CLK_ENA_BMSK                                0x2000
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_APSS_AXI_CLK_ENA_SHFT                                   0xd
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_MPM_AHB_CLK_ENA_BMSK                                 0x1000
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_MPM_AHB_CLK_ENA_SHFT                                    0xc
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_ENA_BMSK                         0x800
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_ENA_SHFT                           0xb
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_BLSP1_AHB_CLK_ENA_BMSK                                0x400
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_BLSP1_AHB_CLK_ENA_SHFT                                  0xa
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_BLSP1_SLEEP_CLK_ENA_BMSK                              0x200
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_BLSP1_SLEEP_CLK_ENA_SHFT                                0x9
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_PRNG_AHB_CLK_ENA_BMSK                                 0x100
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_PRNG_AHB_CLK_ENA_SHFT                                   0x8
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_BOOT_ROM_AHB_CLK_ENA_BMSK                              0x80
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_BOOT_ROM_AHB_CLK_ENA_SHFT                               0x7
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_MSG_RAM_AHB_CLK_ENA_BMSK                               0x40
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_MSG_RAM_AHB_CLK_ENA_SHFT                                0x6
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_TLMM_AHB_CLK_ENA_BMSK                                  0x20
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_TLMM_AHB_CLK_ENA_SHFT                                   0x5
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_TLMM_CLK_ENA_BMSK                                      0x10
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_TLMM_CLK_ENA_SHFT                                       0x4
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_SPMI_PCNOC_AHB_CLK_ENA_BMSK                             0x8
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_SPMI_PCNOC_AHB_CLK_ENA_SHFT                             0x3
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_CRYPTO_CLK_ENA_BMSK                                     0x4
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_CRYPTO_CLK_ENA_SHFT                                     0x2
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AXI_CLK_ENA_BMSK                                 0x2
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AXI_CLK_ENA_SHFT                                 0x1
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AHB_CLK_ENA_BMSK                                 0x1
#define HWIO_GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AHB_CLK_ENA_SHFT                                 0x0

#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00013020)
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_RMSK                                            0x3fff
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR, HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_RMSK)
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR, m)
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR,v)
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR,m,v,HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_IN)
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_GTCU_AHB_CLK_ENA_BMSK                           0x2000
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_GTCU_AHB_CLK_ENA_SHFT                              0xd
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_SMMU_CFG_CLK_ENA_BMSK                           0x1000
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_SMMU_CFG_CLK_ENA_SHFT                              0xc
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_PCNOC_TBU_CLK_ENA_BMSK                           0x800
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_PCNOC_TBU_CLK_ENA_SHFT                             0xb
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_JPEG_TBU_CLK_ENA_BMSK                            0x400
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_JPEG_TBU_CLK_ENA_SHFT                              0xa
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_VFE_TBU_CLK_ENA_BMSK                             0x200
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_VFE_TBU_CLK_ENA_SHFT                               0x9
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_Q6_AXI_CLK_ENA_BMSK                      0x100
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_Q6_AXI_CLK_ENA_SHFT                        0x8
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_GSS_AXI_CLK_ENA_BMSK                      0x80
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_GSS_AXI_CLK_ENA_SHFT                       0x7
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_AXI_CLK_ENA_BMSK                          0x40
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_AXI_CLK_ENA_SHFT                           0x6
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_VENUS_TBU_CLK_ENA_BMSK                            0x20
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_VENUS_TBU_CLK_ENA_SHFT                             0x5
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_MDP_TBU_CLK_ENA_BMSK                              0x10
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_MDP_TBU_CLK_ENA_SHFT                               0x4
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_GFX_TBU_CLK_ENA_BMSK                               0x8
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_GFX_TBU_CLK_ENA_SHFT                               0x3
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_GFX_TCU_CLK_ENA_BMSK                               0x4
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_GFX_TCU_CLK_ENA_SHFT                               0x2
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_APSS_TCU_CLK_ENA_BMSK                              0x2
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_APSS_TCU_CLK_ENA_SHFT                              0x1
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_PRONTO_TBU_CLK_ENA_BMSK                            0x1
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE_PRONTO_TBU_CLK_ENA_SHFT                            0x0

#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00013018)
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_RMSK                                                 0x3ffff
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_ADDR, HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_RMSK)
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_ADDR, m)
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_ADDR,v)
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_ADDR,m,v,HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_IN)
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_IMEM_AXI_CLK_SLEEP_ENA_BMSK                          0x20000
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_IMEM_AXI_CLK_SLEEP_ENA_SHFT                             0x11
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_SLEEP_ENA_BMSK                  0x10000
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_SLEEP_ENA_SHFT                     0x10
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_BIMC_APSS_AXI_CLK_SLEEP_ENA_BMSK                      0x8000
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_BIMC_APSS_AXI_CLK_SLEEP_ENA_SHFT                         0xf
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_APSS_AHB_CLK_SLEEP_ENA_BMSK                           0x4000
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_APSS_AHB_CLK_SLEEP_ENA_SHFT                              0xe
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_APSS_AXI_CLK_SLEEP_ENA_BMSK                           0x2000
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_APSS_AXI_CLK_SLEEP_ENA_SHFT                              0xd
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_MPM_AHB_CLK_SLEEP_ENA_BMSK                            0x1000
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_MPM_AHB_CLK_SLEEP_ENA_SHFT                               0xc
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_SLEEP_ENA_BMSK                    0x800
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_SLEEP_ENA_SHFT                      0xb
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_BLSP1_AHB_CLK_SLEEP_ENA_BMSK                           0x400
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_BLSP1_AHB_CLK_SLEEP_ENA_SHFT                             0xa
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_BLSP1_SLEEP_CLK_SLEEP_ENA_BMSK                         0x200
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_BLSP1_SLEEP_CLK_SLEEP_ENA_SHFT                           0x9
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_PRNG_AHB_CLK_SLEEP_ENA_BMSK                            0x100
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_PRNG_AHB_CLK_SLEEP_ENA_SHFT                              0x8
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_BOOT_ROM_AHB_CLK_SLEEP_ENA_BMSK                         0x80
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_BOOT_ROM_AHB_CLK_SLEEP_ENA_SHFT                          0x7
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_MSG_RAM_AHB_CLK_SLEEP_ENA_BMSK                          0x40
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_MSG_RAM_AHB_CLK_SLEEP_ENA_SHFT                           0x6
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_TLMM_AHB_CLK_SLEEP_ENA_BMSK                             0x20
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_TLMM_AHB_CLK_SLEEP_ENA_SHFT                              0x5
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_TLMM_CLK_SLEEP_ENA_BMSK                                 0x10
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_TLMM_CLK_SLEEP_ENA_SHFT                                  0x4
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_SPMI_PCNOC_AHB_CLK_SLEEP_ENA_BMSK                        0x8
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_SPMI_PCNOC_AHB_CLK_SLEEP_ENA_SHFT                        0x3
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_CRYPTO_CLK_SLEEP_ENA_BMSK                                0x4
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_CRYPTO_CLK_SLEEP_ENA_SHFT                                0x2
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AXI_CLK_SLEEP_ENA_BMSK                            0x2
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AXI_CLK_SLEEP_ENA_SHFT                            0x1
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AHB_CLK_SLEEP_ENA_BMSK                            0x1
#define HWIO_GCC_APCS_TZ_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AHB_CLK_SLEEP_ENA_SHFT                            0x0

#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00013024)
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_RMSK                                             0x3fff
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR, HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_RMSK)
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR, m)
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR,v)
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR,m,v,HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_IN)
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_GTCU_AHB_CLK_SLEEP_ENA_BMSK                      0x2000
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_GTCU_AHB_CLK_SLEEP_ENA_SHFT                         0xd
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_SMMU_CFG_CLK_SLEEP_ENA_BMSK                      0x1000
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_SMMU_CFG_CLK_SLEEP_ENA_SHFT                         0xc
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_PCNOC_TBU_CLK_SLEEP_ENA_BMSK                      0x800
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_PCNOC_TBU_CLK_SLEEP_ENA_SHFT                        0xb
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_JPEG_TBU_CLK_SLEEP_ENA_BMSK                       0x400
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_JPEG_TBU_CLK_SLEEP_ENA_SHFT                         0xa
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_VFE_TBU_CLK_SLEEP_ENA_BMSK                        0x200
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_VFE_TBU_CLK_SLEEP_ENA_SHFT                          0x9
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_Q6_AXI_CLK_SLEEP_ENA_BMSK                 0x100
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_Q6_AXI_CLK_SLEEP_ENA_SHFT                   0x8
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_GSS_AXI_CLK_SLEEP_ENA_BMSK                 0x80
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_GSS_AXI_CLK_SLEEP_ENA_SHFT                  0x7
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_AXI_CLK_SLEEP_ENA_BMSK                     0x40
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_AXI_CLK_SLEEP_ENA_SHFT                      0x6
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_VENUS_TBU_CLK_SLEEP_ENA_BMSK                       0x20
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_VENUS_TBU_CLK_SLEEP_ENA_SHFT                        0x5
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_MDP_TBU_CLK_SLEEP_ENA_BMSK                         0x10
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_MDP_TBU_CLK_SLEEP_ENA_SHFT                          0x4
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_GFX_TBU_CLK_SLEEP_ENA_BMSK                          0x8
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_GFX_TBU_CLK_SLEEP_ENA_SHFT                          0x3
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_GFX_TCU_CLK_SLEEP_ENA_BMSK                          0x4
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_GFX_TCU_CLK_SLEEP_ENA_SHFT                          0x2
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_APSS_TCU_CLK_SLEEP_ENA_BMSK                         0x2
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_APSS_TCU_CLK_SLEEP_ENA_SHFT                         0x1
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_PRONTO_TBU_CLK_SLEEP_ENA_BMSK                       0x1
#define HWIO_GCC_APCS_TZ_SMMU_CLOCK_SLEEP_ENA_VOTE_PRONTO_TBU_CLK_SLEEP_ENA_SHFT                       0x0

#define HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00061000)
#define HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_RMSK                                                           0xf
#define HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_ADDR, HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_RMSK)
#define HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_ADDR, m)
#define HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_ADDR,v)
#define HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_ADDR,m,v,HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_IN)
#define HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_BIMC_PLL_BMSK                                                  0x8
#define HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_BIMC_PLL_SHFT                                                  0x3
#define HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_GPLL2_BMSK                                                     0x4
#define HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_GPLL2_SHFT                                                     0x2
#define HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_GPLL1_BMSK                                                     0x2
#define HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_GPLL1_SHFT                                                     0x1
#define HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_GPLL0_BMSK                                                     0x1
#define HWIO_GCC_APCS_HYP_GPLL_ENA_VOTE_GPLL0_SHFT                                                     0x0

#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00061004)
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_RMSK                                               0x3ffff
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_ADDR, HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_RMSK)
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_ADDR, m)
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_ADDR,v)
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_ADDR,m,v,HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_IN)
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_IMEM_AXI_CLK_ENA_BMSK                              0x20000
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_IMEM_AXI_CLK_ENA_SHFT                                 0x11
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_ENA_BMSK                      0x10000
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_ENA_SHFT                         0x10
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_BIMC_APSS_AXI_CLK_ENA_BMSK                          0x8000
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_BIMC_APSS_AXI_CLK_ENA_SHFT                             0xf
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_APSS_AHB_CLK_ENA_BMSK                               0x4000
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_APSS_AHB_CLK_ENA_SHFT                                  0xe
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_APSS_AXI_CLK_ENA_BMSK                               0x2000
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_APSS_AXI_CLK_ENA_SHFT                                  0xd
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_MPM_AHB_CLK_ENA_BMSK                                0x1000
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_MPM_AHB_CLK_ENA_SHFT                                   0xc
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_ENA_BMSK                        0x800
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_ENA_SHFT                          0xb
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_BLSP1_AHB_CLK_ENA_BMSK                               0x400
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_BLSP1_AHB_CLK_ENA_SHFT                                 0xa
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_BLSP1_SLEEP_CLK_ENA_BMSK                             0x200
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_BLSP1_SLEEP_CLK_ENA_SHFT                               0x9
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_PRNG_AHB_CLK_ENA_BMSK                                0x100
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_PRNG_AHB_CLK_ENA_SHFT                                  0x8
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_BOOT_ROM_AHB_CLK_ENA_BMSK                             0x80
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_BOOT_ROM_AHB_CLK_ENA_SHFT                              0x7
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_MSG_RAM_AHB_CLK_ENA_BMSK                              0x40
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_MSG_RAM_AHB_CLK_ENA_SHFT                               0x6
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_TLMM_AHB_CLK_ENA_BMSK                                 0x20
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_TLMM_AHB_CLK_ENA_SHFT                                  0x5
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_TLMM_CLK_ENA_BMSK                                     0x10
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_TLMM_CLK_ENA_SHFT                                      0x4
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_SPMI_PCNOC_AHB_CLK_ENA_BMSK                            0x8
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_SPMI_PCNOC_AHB_CLK_ENA_SHFT                            0x3
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_CRYPTO_CLK_ENA_BMSK                                    0x4
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_CRYPTO_CLK_ENA_SHFT                                    0x2
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AXI_CLK_ENA_BMSK                                0x2
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AXI_CLK_ENA_SHFT                                0x1
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AHB_CLK_ENA_BMSK                                0x1
#define HWIO_GCC_APCS_HYP_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AHB_CLK_ENA_SHFT                                0x0

#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x0006100c)
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_RMSK                                           0x3fff
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR, HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_RMSK)
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR, m)
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR,v)
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_ADDR,m,v,HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_IN)
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_GTCU_AHB_CLK_ENA_BMSK                          0x2000
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_GTCU_AHB_CLK_ENA_SHFT                             0xd
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_SMMU_CFG_CLK_ENA_BMSK                          0x1000
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_SMMU_CFG_CLK_ENA_SHFT                             0xc
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_PCNOC_TBU_CLK_ENA_BMSK                          0x800
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_PCNOC_TBU_CLK_ENA_SHFT                            0xb
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_JPEG_TBU_CLK_ENA_BMSK                           0x400
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_JPEG_TBU_CLK_ENA_SHFT                             0xa
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_VFE_TBU_CLK_ENA_BMSK                            0x200
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_VFE_TBU_CLK_ENA_SHFT                              0x9
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_Q6_AXI_CLK_ENA_BMSK                     0x100
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_Q6_AXI_CLK_ENA_SHFT                       0x8
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_GSS_AXI_CLK_ENA_BMSK                     0x80
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_GSS_AXI_CLK_ENA_SHFT                      0x7
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_AXI_CLK_ENA_BMSK                         0x40
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_MSS_TBU_AXI_CLK_ENA_SHFT                          0x6
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_VENUS_TBU_CLK_ENA_BMSK                           0x20
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_VENUS_TBU_CLK_ENA_SHFT                            0x5
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_MDP_TBU_CLK_ENA_BMSK                             0x10
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_MDP_TBU_CLK_ENA_SHFT                              0x4
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_GFX_TBU_CLK_ENA_BMSK                              0x8
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_GFX_TBU_CLK_ENA_SHFT                              0x3
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_GFX_TCU_CLK_ENA_BMSK                              0x4
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_GFX_TCU_CLK_ENA_SHFT                              0x2
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_APSS_TCU_CLK_ENA_BMSK                             0x2
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_APSS_TCU_CLK_ENA_SHFT                             0x1
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_PRONTO_TBU_CLK_ENA_BMSK                           0x1
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_BRANCH_ENA_VOTE_PRONTO_TBU_CLK_ENA_SHFT                           0x0

#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_ADDR                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00061008)
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_RMSK                                                0x3ffff
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_ADDR, HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_RMSK)
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_ADDR, m)
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_ADDR,v)
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_ADDR,m,v,HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_IN)
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_IMEM_AXI_CLK_SLEEP_ENA_BMSK                         0x20000
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_IMEM_AXI_CLK_SLEEP_ENA_SHFT                            0x11
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_SLEEP_ENA_BMSK                 0x10000
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_SLEEP_ENA_SHFT                    0x10
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_BIMC_APSS_AXI_CLK_SLEEP_ENA_BMSK                     0x8000
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_BIMC_APSS_AXI_CLK_SLEEP_ENA_SHFT                        0xf
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_APSS_AHB_CLK_SLEEP_ENA_BMSK                          0x4000
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_APSS_AHB_CLK_SLEEP_ENA_SHFT                             0xe
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_APSS_AXI_CLK_SLEEP_ENA_BMSK                          0x2000
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_APSS_AXI_CLK_SLEEP_ENA_SHFT                             0xd
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_MPM_AHB_CLK_SLEEP_ENA_BMSK                           0x1000
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_MPM_AHB_CLK_SLEEP_ENA_SHFT                              0xc
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_SLEEP_ENA_BMSK                   0x800
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_SLEEP_ENA_SHFT                     0xb
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_BLSP1_AHB_CLK_SLEEP_ENA_BMSK                          0x400
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_BLSP1_AHB_CLK_SLEEP_ENA_SHFT                            0xa
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_BLSP1_SLEEP_CLK_SLEEP_ENA_BMSK                        0x200
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_BLSP1_SLEEP_CLK_SLEEP_ENA_SHFT                          0x9
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_PRNG_AHB_CLK_SLEEP_ENA_BMSK                           0x100
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_PRNG_AHB_CLK_SLEEP_ENA_SHFT                             0x8
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_BOOT_ROM_AHB_CLK_SLEEP_ENA_BMSK                        0x80
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_BOOT_ROM_AHB_CLK_SLEEP_ENA_SHFT                         0x7
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_MSG_RAM_AHB_CLK_SLEEP_ENA_BMSK                         0x40
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_MSG_RAM_AHB_CLK_SLEEP_ENA_SHFT                          0x6
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_TLMM_AHB_CLK_SLEEP_ENA_BMSK                            0x20
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_TLMM_AHB_CLK_SLEEP_ENA_SHFT                             0x5
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_TLMM_CLK_SLEEP_ENA_BMSK                                0x10
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_TLMM_CLK_SLEEP_ENA_SHFT                                 0x4
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_SPMI_PCNOC_AHB_CLK_SLEEP_ENA_BMSK                       0x8
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_SPMI_PCNOC_AHB_CLK_SLEEP_ENA_SHFT                       0x3
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_CRYPTO_CLK_SLEEP_ENA_BMSK                               0x4
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_CRYPTO_CLK_SLEEP_ENA_SHFT                               0x2
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AXI_CLK_SLEEP_ENA_BMSK                           0x2
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AXI_CLK_SLEEP_ENA_SHFT                           0x1
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AHB_CLK_SLEEP_ENA_BMSK                           0x1
#define HWIO_GCC_APCS_HYP_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AHB_CLK_SLEEP_ENA_SHFT                           0x0

#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00061010)
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_RMSK                                            0x3fff
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR, HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_RMSK)
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR, m)
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR,v)
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_ADDR,m,v,HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_IN)
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_GTCU_AHB_CLK_SLEEP_ENA_BMSK                     0x2000
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_GTCU_AHB_CLK_SLEEP_ENA_SHFT                        0xd
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_SMMU_CFG_CLK_SLEEP_ENA_BMSK                     0x1000
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_SMMU_CFG_CLK_SLEEP_ENA_SHFT                        0xc
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_PCNOC_TBU_CLK_SLEEP_ENA_BMSK                     0x800
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_PCNOC_TBU_CLK_SLEEP_ENA_SHFT                       0xb
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_JPEG_TBU_CLK_SLEEP_ENA_BMSK                      0x400
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_JPEG_TBU_CLK_SLEEP_ENA_SHFT                        0xa
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_VFE_TBU_CLK_SLEEP_ENA_BMSK                       0x200
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_VFE_TBU_CLK_SLEEP_ENA_SHFT                         0x9
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_Q6_AXI_CLK_SLEEP_ENA_BMSK                0x100
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_Q6_AXI_CLK_SLEEP_ENA_SHFT                  0x8
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_GSS_AXI_CLK_SLEEP_ENA_BMSK                0x80
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_GSS_AXI_CLK_SLEEP_ENA_SHFT                 0x7
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_AXI_CLK_SLEEP_ENA_BMSK                    0x40
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_MSS_TBU_AXI_CLK_SLEEP_ENA_SHFT                     0x6
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_VENUS_TBU_CLK_SLEEP_ENA_BMSK                      0x20
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_VENUS_TBU_CLK_SLEEP_ENA_SHFT                       0x5
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_MDP_TBU_CLK_SLEEP_ENA_BMSK                        0x10
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_MDP_TBU_CLK_SLEEP_ENA_SHFT                         0x4
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_GFX_TBU_CLK_SLEEP_ENA_BMSK                         0x8
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_GFX_TBU_CLK_SLEEP_ENA_SHFT                         0x3
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_GFX_TCU_CLK_SLEEP_ENA_BMSK                         0x4
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_GFX_TCU_CLK_SLEEP_ENA_SHFT                         0x2
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_APSS_TCU_CLK_SLEEP_ENA_BMSK                        0x2
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_APSS_TCU_CLK_SLEEP_ENA_SHFT                        0x1
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_PRONTO_TBU_CLK_SLEEP_ENA_BMSK                      0x1
#define HWIO_GCC_APCS_HYP_SMMU_CLOCK_SLEEP_ENA_VOTE_PRONTO_TBU_CLK_SLEEP_ENA_SHFT                      0x0

#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x0001b000)
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_RMSK                                                             0xf
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_ADDR, HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_RMSK)
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_ADDR, m)
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_ADDR,v)
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_ADDR,m,v,HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_IN)
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_BIMC_PLL_BMSK                                                    0x8
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_BIMC_PLL_SHFT                                                    0x3
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_GPLL2_BMSK                                                       0x4
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_GPLL2_SHFT                                                       0x2
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_GPLL1_BMSK                                                       0x2
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_GPLL1_SHFT                                                       0x1
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_GPLL0_BMSK                                                       0x1
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_GPLL0_SHFT                                                       0x0

#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0001b004)
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_RMSK                                                 0x3ffff
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_ADDR, HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_RMSK)
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_ADDR, m)
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_ADDR,v)
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_ADDR,m,v,HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_IN)
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_IMEM_AXI_CLK_ENA_BMSK                                0x20000
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_IMEM_AXI_CLK_ENA_SHFT                                   0x11
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_ENA_BMSK                        0x10000
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_ENA_SHFT                           0x10
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_BIMC_APSS_AXI_CLK_ENA_BMSK                            0x8000
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_BIMC_APSS_AXI_CLK_ENA_SHFT                               0xf
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_APSS_AHB_CLK_ENA_BMSK                                 0x4000
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_APSS_AHB_CLK_ENA_SHFT                                    0xe
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_APSS_AXI_CLK_ENA_BMSK                                 0x2000
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_APSS_AXI_CLK_ENA_SHFT                                    0xd
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_MPM_AHB_CLK_ENA_BMSK                                  0x1000
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_MPM_AHB_CLK_ENA_SHFT                                     0xc
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_ENA_BMSK                          0x800
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_ENA_SHFT                            0xb
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_BLSP1_AHB_CLK_ENA_BMSK                                 0x400
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_BLSP1_AHB_CLK_ENA_SHFT                                   0xa
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_BLSP1_SLEEP_CLK_ENA_BMSK                               0x200
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_BLSP1_SLEEP_CLK_ENA_SHFT                                 0x9
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_PRNG_AHB_CLK_ENA_BMSK                                  0x100
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_PRNG_AHB_CLK_ENA_SHFT                                    0x8
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_BOOT_ROM_AHB_CLK_ENA_BMSK                               0x80
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_BOOT_ROM_AHB_CLK_ENA_SHFT                                0x7
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_MSG_RAM_AHB_CLK_ENA_BMSK                                0x40
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_MSG_RAM_AHB_CLK_ENA_SHFT                                 0x6
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_TLMM_AHB_CLK_ENA_BMSK                                   0x20
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_TLMM_AHB_CLK_ENA_SHFT                                    0x5
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_TLMM_CLK_ENA_BMSK                                       0x10
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_TLMM_CLK_ENA_SHFT                                        0x4
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_SPMI_PCNOC_AHB_CLK_ENA_BMSK                              0x8
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_SPMI_PCNOC_AHB_CLK_ENA_SHFT                              0x3
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_CRYPTO_CLK_ENA_BMSK                                      0x4
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_CRYPTO_CLK_ENA_SHFT                                      0x2
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AXI_CLK_ENA_BMSK                                  0x2
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AXI_CLK_ENA_SHFT                                  0x1
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AHB_CLK_ENA_BMSK                                  0x1
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AHB_CLK_ENA_SHFT                                  0x0

#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0001b008)
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_RMSK                                                  0x3ffff
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_ADDR, HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_RMSK)
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_ADDR, m)
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_ADDR,v)
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_ADDR,m,v,HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_IN)
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_IMEM_AXI_CLK_SLEEP_ENA_BMSK                           0x20000
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_IMEM_AXI_CLK_SLEEP_ENA_SHFT                              0x11
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_SLEEP_ENA_BMSK                   0x10000
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_SLEEP_ENA_SHFT                      0x10
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_BIMC_APSS_AXI_CLK_SLEEP_ENA_BMSK                       0x8000
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_BIMC_APSS_AXI_CLK_SLEEP_ENA_SHFT                          0xf
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_APSS_AHB_CLK_SLEEP_ENA_BMSK                            0x4000
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_APSS_AHB_CLK_SLEEP_ENA_SHFT                               0xe
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_APSS_AXI_CLK_SLEEP_ENA_BMSK                            0x2000
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_APSS_AXI_CLK_SLEEP_ENA_SHFT                               0xd
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_MPM_AHB_CLK_SLEEP_ENA_BMSK                             0x1000
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_MPM_AHB_CLK_SLEEP_ENA_SHFT                                0xc
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_SLEEP_ENA_BMSK                     0x800
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_SLEEP_ENA_SHFT                       0xb
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_BLSP1_AHB_CLK_SLEEP_ENA_BMSK                            0x400
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_BLSP1_AHB_CLK_SLEEP_ENA_SHFT                              0xa
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_BLSP1_SLEEP_CLK_SLEEP_ENA_BMSK                          0x200
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_BLSP1_SLEEP_CLK_SLEEP_ENA_SHFT                            0x9
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_PRNG_AHB_CLK_SLEEP_ENA_BMSK                             0x100
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_PRNG_AHB_CLK_SLEEP_ENA_SHFT                               0x8
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_BOOT_ROM_AHB_CLK_SLEEP_ENA_BMSK                          0x80
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_BOOT_ROM_AHB_CLK_SLEEP_ENA_SHFT                           0x7
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_MSG_RAM_AHB_CLK_SLEEP_ENA_BMSK                           0x40
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_MSG_RAM_AHB_CLK_SLEEP_ENA_SHFT                            0x6
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_TLMM_AHB_CLK_SLEEP_ENA_BMSK                              0x20
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_TLMM_AHB_CLK_SLEEP_ENA_SHFT                               0x5
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_TLMM_CLK_SLEEP_ENA_BMSK                                  0x10
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_TLMM_CLK_SLEEP_ENA_SHFT                                   0x4
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_SPMI_PCNOC_AHB_CLK_SLEEP_ENA_BMSK                         0x8
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_SPMI_PCNOC_AHB_CLK_SLEEP_ENA_SHFT                         0x3
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_CRYPTO_CLK_SLEEP_ENA_BMSK                                 0x4
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_CRYPTO_CLK_SLEEP_ENA_SHFT                                 0x2
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AXI_CLK_SLEEP_ENA_BMSK                             0x2
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AXI_CLK_SLEEP_ENA_SHFT                             0x1
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AHB_CLK_SLEEP_ENA_BMSK                             0x1
#define HWIO_GCC_MSS_Q6_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AHB_CLK_SLEEP_ENA_SHFT                             0x0

#define HWIO_GCC_WCSS_GPLL_ENA_VOTE_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00070000)
#define HWIO_GCC_WCSS_GPLL_ENA_VOTE_RMSK                                                               0xf
#define HWIO_GCC_WCSS_GPLL_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_WCSS_GPLL_ENA_VOTE_ADDR, HWIO_GCC_WCSS_GPLL_ENA_VOTE_RMSK)
#define HWIO_GCC_WCSS_GPLL_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_WCSS_GPLL_ENA_VOTE_ADDR, m)
#define HWIO_GCC_WCSS_GPLL_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_WCSS_GPLL_ENA_VOTE_ADDR,v)
#define HWIO_GCC_WCSS_GPLL_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_WCSS_GPLL_ENA_VOTE_ADDR,m,v,HWIO_GCC_WCSS_GPLL_ENA_VOTE_IN)
#define HWIO_GCC_WCSS_GPLL_ENA_VOTE_BIMC_PLL_BMSK                                                      0x8
#define HWIO_GCC_WCSS_GPLL_ENA_VOTE_BIMC_PLL_SHFT                                                      0x3
#define HWIO_GCC_WCSS_GPLL_ENA_VOTE_GPLL2_BMSK                                                         0x4
#define HWIO_GCC_WCSS_GPLL_ENA_VOTE_GPLL2_SHFT                                                         0x2
#define HWIO_GCC_WCSS_GPLL_ENA_VOTE_GPLL1_BMSK                                                         0x2
#define HWIO_GCC_WCSS_GPLL_ENA_VOTE_GPLL1_SHFT                                                         0x1
#define HWIO_GCC_WCSS_GPLL_ENA_VOTE_GPLL0_BMSK                                                         0x1
#define HWIO_GCC_WCSS_GPLL_ENA_VOTE_GPLL0_SHFT                                                         0x0

#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_ADDR                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00070004)
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_RMSK                                                   0x3ffff
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_ADDR, HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_RMSK)
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_ADDR, m)
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_ADDR,v)
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_ADDR,m,v,HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_IN)
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_IMEM_AXI_CLK_ENA_BMSK                                  0x20000
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_IMEM_AXI_CLK_ENA_SHFT                                     0x11
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_ENA_BMSK                          0x10000
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_ENA_SHFT                             0x10
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_BIMC_APSS_AXI_CLK_ENA_BMSK                              0x8000
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_BIMC_APSS_AXI_CLK_ENA_SHFT                                 0xf
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_APSS_AHB_CLK_ENA_BMSK                                   0x4000
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_APSS_AHB_CLK_ENA_SHFT                                      0xe
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_APSS_AXI_CLK_ENA_BMSK                                   0x2000
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_APSS_AXI_CLK_ENA_SHFT                                      0xd
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_MPM_AHB_CLK_ENA_BMSK                                    0x1000
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_MPM_AHB_CLK_ENA_SHFT                                       0xc
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_ENA_BMSK                            0x800
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_ENA_SHFT                              0xb
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_BLSP1_AHB_CLK_ENA_BMSK                                   0x400
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_BLSP1_AHB_CLK_ENA_SHFT                                     0xa
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_BLSP1_SLEEP_CLK_ENA_BMSK                                 0x200
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_BLSP1_SLEEP_CLK_ENA_SHFT                                   0x9
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_PRNG_AHB_CLK_ENA_BMSK                                    0x100
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_PRNG_AHB_CLK_ENA_SHFT                                      0x8
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_BOOT_ROM_AHB_CLK_ENA_BMSK                                 0x80
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_BOOT_ROM_AHB_CLK_ENA_SHFT                                  0x7
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_MSG_RAM_AHB_CLK_ENA_BMSK                                  0x40
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_MSG_RAM_AHB_CLK_ENA_SHFT                                   0x6
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_TLMM_AHB_CLK_ENA_BMSK                                     0x20
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_TLMM_AHB_CLK_ENA_SHFT                                      0x5
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_TLMM_CLK_ENA_BMSK                                         0x10
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_TLMM_CLK_ENA_SHFT                                          0x4
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_SPMI_PCNOC_AHB_CLK_ENA_BMSK                                0x8
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_SPMI_PCNOC_AHB_CLK_ENA_SHFT                                0x3
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_CRYPTO_CLK_ENA_BMSK                                        0x4
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_CRYPTO_CLK_ENA_SHFT                                        0x2
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AXI_CLK_ENA_BMSK                                    0x2
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AXI_CLK_ENA_SHFT                                    0x1
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AHB_CLK_ENA_BMSK                                    0x1
#define HWIO_GCC_WCSS_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AHB_CLK_ENA_SHFT                                    0x0

#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_ADDR                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00070008)
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_RMSK                                                    0x3ffff
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_ADDR, HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_RMSK)
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_ADDR, m)
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_ADDR,v)
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_ADDR,m,v,HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_IN)
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_IMEM_AXI_CLK_SLEEP_ENA_BMSK                             0x20000
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_IMEM_AXI_CLK_SLEEP_ENA_SHFT                                0x11
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_SLEEP_ENA_BMSK                     0x10000
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_SLEEP_ENA_SHFT                        0x10
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_BIMC_APSS_AXI_CLK_SLEEP_ENA_BMSK                         0x8000
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_BIMC_APSS_AXI_CLK_SLEEP_ENA_SHFT                            0xf
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_APSS_AHB_CLK_SLEEP_ENA_BMSK                              0x4000
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_APSS_AHB_CLK_SLEEP_ENA_SHFT                                 0xe
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_APSS_AXI_CLK_SLEEP_ENA_BMSK                              0x2000
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_APSS_AXI_CLK_SLEEP_ENA_SHFT                                 0xd
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_MPM_AHB_CLK_SLEEP_ENA_BMSK                               0x1000
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_MPM_AHB_CLK_SLEEP_ENA_SHFT                                  0xc
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_SLEEP_ENA_BMSK                       0x800
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_SLEEP_ENA_SHFT                         0xb
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_BLSP1_AHB_CLK_SLEEP_ENA_BMSK                              0x400
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_BLSP1_AHB_CLK_SLEEP_ENA_SHFT                                0xa
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_BLSP1_SLEEP_CLK_SLEEP_ENA_BMSK                            0x200
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_BLSP1_SLEEP_CLK_SLEEP_ENA_SHFT                              0x9
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_PRNG_AHB_CLK_SLEEP_ENA_BMSK                               0x100
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_PRNG_AHB_CLK_SLEEP_ENA_SHFT                                 0x8
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_BOOT_ROM_AHB_CLK_SLEEP_ENA_BMSK                            0x80
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_BOOT_ROM_AHB_CLK_SLEEP_ENA_SHFT                             0x7
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_MSG_RAM_AHB_CLK_SLEEP_ENA_BMSK                             0x40
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_MSG_RAM_AHB_CLK_SLEEP_ENA_SHFT                              0x6
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_TLMM_AHB_CLK_SLEEP_ENA_BMSK                                0x20
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_TLMM_AHB_CLK_SLEEP_ENA_SHFT                                 0x5
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_TLMM_CLK_SLEEP_ENA_BMSK                                    0x10
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_TLMM_CLK_SLEEP_ENA_SHFT                                     0x4
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_SPMI_PCNOC_AHB_CLK_SLEEP_ENA_BMSK                           0x8
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_SPMI_PCNOC_AHB_CLK_SLEEP_ENA_SHFT                           0x3
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_CRYPTO_CLK_SLEEP_ENA_BMSK                                   0x4
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_CRYPTO_CLK_SLEEP_ENA_SHFT                                   0x2
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AXI_CLK_SLEEP_ENA_BMSK                               0x2
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AXI_CLK_SLEEP_ENA_SHFT                               0x1
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AHB_CLK_SLEEP_ENA_BMSK                               0x1
#define HWIO_GCC_WCSS_CLOCK_SLEEP_ENA_VOTE_CRYPTO_AHB_CLK_SLEEP_ENA_SHFT                               0x0

#define HWIO_GCC_SPARE_GPLL_ENA_VOTE_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x00000000)
#define HWIO_GCC_SPARE_GPLL_ENA_VOTE_RMSK                                                              0xf
#define HWIO_GCC_SPARE_GPLL_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_SPARE_GPLL_ENA_VOTE_ADDR, HWIO_GCC_SPARE_GPLL_ENA_VOTE_RMSK)
#define HWIO_GCC_SPARE_GPLL_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_SPARE_GPLL_ENA_VOTE_ADDR, m)
#define HWIO_GCC_SPARE_GPLL_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_SPARE_GPLL_ENA_VOTE_ADDR,v)
#define HWIO_GCC_SPARE_GPLL_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPARE_GPLL_ENA_VOTE_ADDR,m,v,HWIO_GCC_SPARE_GPLL_ENA_VOTE_IN)
#define HWIO_GCC_SPARE_GPLL_ENA_VOTE_BIMC_PLL_BMSK                                                     0x8
#define HWIO_GCC_SPARE_GPLL_ENA_VOTE_BIMC_PLL_SHFT                                                     0x3
#define HWIO_GCC_SPARE_GPLL_ENA_VOTE_GPLL2_BMSK                                                        0x4
#define HWIO_GCC_SPARE_GPLL_ENA_VOTE_GPLL2_SHFT                                                        0x2
#define HWIO_GCC_SPARE_GPLL_ENA_VOTE_GPLL1_BMSK                                                        0x2
#define HWIO_GCC_SPARE_GPLL_ENA_VOTE_GPLL1_SHFT                                                        0x1
#define HWIO_GCC_SPARE_GPLL_ENA_VOTE_GPLL0_BMSK                                                        0x1
#define HWIO_GCC_SPARE_GPLL_ENA_VOTE_GPLL0_SHFT                                                        0x0

#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00000004)
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_RMSK                                                  0x3ffff
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_ADDR, HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_RMSK)
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_ADDR, m)
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_ADDR,v)
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_ADDR,m,v,HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_IN)
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_IMEM_AXI_CLK_ENA_BMSK                                 0x20000
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_IMEM_AXI_CLK_ENA_SHFT                                    0x11
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_ENA_BMSK                         0x10000
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_SYS_NOC_APSS_AHB_CLK_ENA_SHFT                            0x10
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_BIMC_APSS_AXI_CLK_ENA_BMSK                             0x8000
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_BIMC_APSS_AXI_CLK_ENA_SHFT                                0xf
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_APSS_AHB_CLK_ENA_BMSK                                  0x4000
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_APSS_AHB_CLK_ENA_SHFT                                     0xe
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_APSS_AXI_CLK_ENA_BMSK                                  0x2000
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_APSS_AXI_CLK_ENA_SHFT                                     0xd
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_MPM_AHB_CLK_ENA_BMSK                                   0x1000
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_MPM_AHB_CLK_ENA_SHFT                                      0xc
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_ENA_BMSK                           0x800
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_GMEM_SYS_NOC_AXI_CLK_ENA_SHFT                             0xb
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_BLSP1_AHB_CLK_ENA_BMSK                                  0x400
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_BLSP1_AHB_CLK_ENA_SHFT                                    0xa
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_BLSP1_SLEEP_CLK_ENA_BMSK                                0x200
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_BLSP1_SLEEP_CLK_ENA_SHFT                                  0x9
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_PRNG_AHB_CLK_ENA_BMSK                                   0x100
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_PRNG_AHB_CLK_ENA_SHFT                                     0x8
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_BOOT_ROM_AHB_CLK_ENA_BMSK                                0x80
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_BOOT_ROM_AHB_CLK_ENA_SHFT                                 0x7
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_MSG_RAM_AHB_CLK_ENA_BMSK                                 0x40
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_MSG_RAM_AHB_CLK_ENA_SHFT                                  0x6
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_TLMM_AHB_CLK_ENA_BMSK                                    0x20
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_TLMM_AHB_CLK_ENA_SHFT                                     0x5
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_TLMM_CLK_ENA_BMSK                                        0x10
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_TLMM_CLK_ENA_SHFT                                         0x4
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_SPMI_PCNOC_AHB_CLK_ENA_BMSK                               0x8
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_SPMI_PCNOC_AHB_CLK_ENA_SHFT                               0x3
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_CRYPTO_CLK_ENA_BMSK                                       0x4
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_CRYPTO_CLK_ENA_SHFT                                       0x2
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AXI_CLK_ENA_BMSK                                   0x2
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AXI_CLK_ENA_SHFT                                   0x1
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AHB_CLK_ENA_BMSK                                   0x1
#define HWIO_GCC_SPARE_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AHB_CLK_ENA_SHFT                                   0x0

#define HWIO_GCC_MSS_RESTART_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00010000)
#define HWIO_GCC_MSS_RESTART_RMSK                                                                      0x1
#define HWIO_GCC_MSS_RESTART_IN          \
        in_dword_masked(HWIO_GCC_MSS_RESTART_ADDR, HWIO_GCC_MSS_RESTART_RMSK)
#define HWIO_GCC_MSS_RESTART_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_RESTART_ADDR, m)
#define HWIO_GCC_MSS_RESTART_OUT(v)      \
        out_dword(HWIO_GCC_MSS_RESTART_ADDR,v)
#define HWIO_GCC_MSS_RESTART_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_RESTART_ADDR,m,v,HWIO_GCC_MSS_RESTART_IN)
#define HWIO_GCC_MSS_RESTART_MSS_RESTART_BMSK                                                          0x1
#define HWIO_GCC_MSS_RESTART_MSS_RESTART_SHFT                                                          0x0

#define HWIO_GCC_VENUS_RESTART_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00010004)
#define HWIO_GCC_VENUS_RESTART_RMSK                                                                    0x1
#define HWIO_GCC_VENUS_RESTART_IN          \
        in_dword_masked(HWIO_GCC_VENUS_RESTART_ADDR, HWIO_GCC_VENUS_RESTART_RMSK)
#define HWIO_GCC_VENUS_RESTART_INM(m)      \
        in_dword_masked(HWIO_GCC_VENUS_RESTART_ADDR, m)
#define HWIO_GCC_VENUS_RESTART_OUT(v)      \
        out_dword(HWIO_GCC_VENUS_RESTART_ADDR,v)
#define HWIO_GCC_VENUS_RESTART_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_VENUS_RESTART_ADDR,m,v,HWIO_GCC_VENUS_RESTART_IN)
#define HWIO_GCC_VENUS_RESTART_VENUS_RESTART_BMSK                                                      0x1
#define HWIO_GCC_VENUS_RESTART_VENUS_RESTART_SHFT                                                      0x0

#define HWIO_GCC_WCSS_RESTART_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00011000)
#define HWIO_GCC_WCSS_RESTART_RMSK                                                                     0x1
#define HWIO_GCC_WCSS_RESTART_IN          \
        in_dword_masked(HWIO_GCC_WCSS_RESTART_ADDR, HWIO_GCC_WCSS_RESTART_RMSK)
#define HWIO_GCC_WCSS_RESTART_INM(m)      \
        in_dword_masked(HWIO_GCC_WCSS_RESTART_ADDR, m)
#define HWIO_GCC_WCSS_RESTART_OUT(v)      \
        out_dword(HWIO_GCC_WCSS_RESTART_ADDR,v)
#define HWIO_GCC_WCSS_RESTART_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_WCSS_RESTART_ADDR,m,v,HWIO_GCC_WCSS_RESTART_IN)
#define HWIO_GCC_WCSS_RESTART_WCSS_RESTART_BMSK                                                        0x1
#define HWIO_GCC_WCSS_RESTART_WCSS_RESTART_SHFT                                                        0x0

#define HWIO_GCC_RESET_DEBUG_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00014000)
#define HWIO_GCC_RESET_DEBUG_RMSK                                                                 0xffffff
#define HWIO_GCC_RESET_DEBUG_IN          \
        in_dword_masked(HWIO_GCC_RESET_DEBUG_ADDR, HWIO_GCC_RESET_DEBUG_RMSK)
#define HWIO_GCC_RESET_DEBUG_INM(m)      \
        in_dword_masked(HWIO_GCC_RESET_DEBUG_ADDR, m)
#define HWIO_GCC_RESET_DEBUG_OUT(v)      \
        out_dword(HWIO_GCC_RESET_DEBUG_ADDR,v)
#define HWIO_GCC_RESET_DEBUG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RESET_DEBUG_ADDR,m,v,HWIO_GCC_RESET_DEBUG_IN)
#define HWIO_GCC_RESET_DEBUG_MSFT_DBG_RQST_BMSK                                                   0x800000
#define HWIO_GCC_RESET_DEBUG_MSFT_DBG_RQST_SHFT                                                       0x17
#define HWIO_GCC_RESET_DEBUG_BLOCK_RESIN_BMSK                                                     0x400000
#define HWIO_GCC_RESET_DEBUG_BLOCK_RESIN_SHFT                                                         0x16
#define HWIO_GCC_RESET_DEBUG_RESET_ACCESS_FIRST_PASS_BMSK                                         0x200000
#define HWIO_GCC_RESET_DEBUG_RESET_ACCESS_FIRST_PASS_SHFT                                             0x15
#define HWIO_GCC_RESET_DEBUG_RESET_DEBUG_FIRST_PASS_BMSK                                          0x100000
#define HWIO_GCC_RESET_DEBUG_RESET_DEBUG_FIRST_PASS_SHFT                                              0x14
#define HWIO_GCC_RESET_DEBUG_MSM_TSENSE_RESET_DEBUG_EN_BMSK                                        0x80000
#define HWIO_GCC_RESET_DEBUG_MSM_TSENSE_RESET_DEBUG_EN_SHFT                                           0x13
#define HWIO_GCC_RESET_DEBUG_PMIC_ABNORMAL_RESET_DEBUG_EN_BMSK                                     0x40000
#define HWIO_GCC_RESET_DEBUG_PMIC_ABNORMAL_RESET_DEBUG_EN_SHFT                                        0x12
#define HWIO_GCC_RESET_DEBUG_SECURE_WDOG_DEBUG_EN_BMSK                                             0x20000
#define HWIO_GCC_RESET_DEBUG_SECURE_WDOG_DEBUG_EN_SHFT                                                0x11
#define HWIO_GCC_RESET_DEBUG_PROC_HALT_EN_BMSK                                                     0x10000
#define HWIO_GCC_RESET_DEBUG_PROC_HALT_EN_SHFT                                                        0x10
#define HWIO_GCC_RESET_DEBUG_SRST_V1_MODE_BMSK                                                      0x8000
#define HWIO_GCC_RESET_DEBUG_SRST_V1_MODE_SHFT                                                         0xf
#define HWIO_GCC_RESET_DEBUG_PRE_ARES_DEBUG_TIMER_VAL_BMSK                                          0x7fff
#define HWIO_GCC_RESET_DEBUG_PRE_ARES_DEBUG_TIMER_VAL_SHFT                                             0x0

#define HWIO_GCC_FLUSH_ETR_DEBUG_TIMER_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00015000)
#define HWIO_GCC_FLUSH_ETR_DEBUG_TIMER_RMSK                                                         0xffff
#define HWIO_GCC_FLUSH_ETR_DEBUG_TIMER_IN          \
        in_dword_masked(HWIO_GCC_FLUSH_ETR_DEBUG_TIMER_ADDR, HWIO_GCC_FLUSH_ETR_DEBUG_TIMER_RMSK)
#define HWIO_GCC_FLUSH_ETR_DEBUG_TIMER_INM(m)      \
        in_dword_masked(HWIO_GCC_FLUSH_ETR_DEBUG_TIMER_ADDR, m)
#define HWIO_GCC_FLUSH_ETR_DEBUG_TIMER_OUT(v)      \
        out_dword(HWIO_GCC_FLUSH_ETR_DEBUG_TIMER_ADDR,v)
#define HWIO_GCC_FLUSH_ETR_DEBUG_TIMER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_FLUSH_ETR_DEBUG_TIMER_ADDR,m,v,HWIO_GCC_FLUSH_ETR_DEBUG_TIMER_IN)
#define HWIO_GCC_FLUSH_ETR_DEBUG_TIMER_FLUSH_ETR_DEBUG_TIMER_VAL_BMSK                               0xffff
#define HWIO_GCC_FLUSH_ETR_DEBUG_TIMER_FLUSH_ETR_DEBUG_TIMER_VAL_SHFT                                  0x0

#define HWIO_GCC_STOP_CAPTURE_DEBUG_TIMER_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00015004)
#define HWIO_GCC_STOP_CAPTURE_DEBUG_TIMER_RMSK                                                  0xffffffff
#define HWIO_GCC_STOP_CAPTURE_DEBUG_TIMER_IN          \
        in_dword_masked(HWIO_GCC_STOP_CAPTURE_DEBUG_TIMER_ADDR, HWIO_GCC_STOP_CAPTURE_DEBUG_TIMER_RMSK)
#define HWIO_GCC_STOP_CAPTURE_DEBUG_TIMER_INM(m)      \
        in_dword_masked(HWIO_GCC_STOP_CAPTURE_DEBUG_TIMER_ADDR, m)
#define HWIO_GCC_STOP_CAPTURE_DEBUG_TIMER_OUT(v)      \
        out_dword(HWIO_GCC_STOP_CAPTURE_DEBUG_TIMER_ADDR,v)
#define HWIO_GCC_STOP_CAPTURE_DEBUG_TIMER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_STOP_CAPTURE_DEBUG_TIMER_ADDR,m,v,HWIO_GCC_STOP_CAPTURE_DEBUG_TIMER_IN)
#define HWIO_GCC_STOP_CAPTURE_DEBUG_TIMER_RESERVE_BITS31_16_BMSK                                0xffff0000
#define HWIO_GCC_STOP_CAPTURE_DEBUG_TIMER_RESERVE_BITS31_16_SHFT                                      0x10
#define HWIO_GCC_STOP_CAPTURE_DEBUG_TIMER_STOP_CAPTURE_DEBUG_TIMER_VAL_BMSK                         0xffff
#define HWIO_GCC_STOP_CAPTURE_DEBUG_TIMER_STOP_CAPTURE_DEBUG_TIMER_VAL_SHFT                            0x0

#define HWIO_GCC_RESET_STATUS_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00015008)
#define HWIO_GCC_RESET_STATUS_RMSK                                                                    0x3f
#define HWIO_GCC_RESET_STATUS_IN          \
        in_dword_masked(HWIO_GCC_RESET_STATUS_ADDR, HWIO_GCC_RESET_STATUS_RMSK)
#define HWIO_GCC_RESET_STATUS_INM(m)      \
        in_dword_masked(HWIO_GCC_RESET_STATUS_ADDR, m)
#define HWIO_GCC_RESET_STATUS_OUT(v)      \
        out_dword(HWIO_GCC_RESET_STATUS_ADDR,v)
#define HWIO_GCC_RESET_STATUS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RESET_STATUS_ADDR,m,v,HWIO_GCC_RESET_STATUS_IN)
#define HWIO_GCC_RESET_STATUS_SECURE_WDOG_EXPIRE_STATUS_BMSK                                          0x20
#define HWIO_GCC_RESET_STATUS_SECURE_WDOG_EXPIRE_STATUS_SHFT                                           0x5
#define HWIO_GCC_RESET_STATUS_PMIC_ABNORMAL_RESIN_STATUS_BMSK                                         0x10
#define HWIO_GCC_RESET_STATUS_PMIC_ABNORMAL_RESIN_STATUS_SHFT                                          0x4
#define HWIO_GCC_RESET_STATUS_TSENSE_RESET_STATUS_BMSK                                                 0x8
#define HWIO_GCC_RESET_STATUS_TSENSE_RESET_STATUS_SHFT                                                 0x3
#define HWIO_GCC_RESET_STATUS_SRST_STATUS_BMSK                                                         0x4
#define HWIO_GCC_RESET_STATUS_SRST_STATUS_SHFT                                                         0x2
#define HWIO_GCC_RESET_STATUS_DEBUG_RESET_STATUS_BMSK                                                  0x3
#define HWIO_GCC_RESET_STATUS_DEBUG_RESET_STATUS_SHFT                                                  0x0

#define HWIO_GCC_SW_SRST_ADDR                                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0001500c)
#define HWIO_GCC_SW_SRST_RMSK                                                                          0x1
#define HWIO_GCC_SW_SRST_IN          \
        in_dword_masked(HWIO_GCC_SW_SRST_ADDR, HWIO_GCC_SW_SRST_RMSK)
#define HWIO_GCC_SW_SRST_INM(m)      \
        in_dword_masked(HWIO_GCC_SW_SRST_ADDR, m)
#define HWIO_GCC_SW_SRST_OUT(v)      \
        out_dword(HWIO_GCC_SW_SRST_ADDR,v)
#define HWIO_GCC_SW_SRST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SW_SRST_ADDR,m,v,HWIO_GCC_SW_SRST_IN)
#define HWIO_GCC_SW_SRST_SW_SRST_BMSK                                                                  0x1
#define HWIO_GCC_SW_SRST_SW_SRST_SHFT                                                                  0x0

#define HWIO_GCC_PROC_HALT_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x0001301c)
#define HWIO_GCC_PROC_HALT_RMSK                                                                        0x1
#define HWIO_GCC_PROC_HALT_IN          \
        in_dword_masked(HWIO_GCC_PROC_HALT_ADDR, HWIO_GCC_PROC_HALT_RMSK)
#define HWIO_GCC_PROC_HALT_INM(m)      \
        in_dword_masked(HWIO_GCC_PROC_HALT_ADDR, m)
#define HWIO_GCC_PROC_HALT_OUT(v)      \
        out_dword(HWIO_GCC_PROC_HALT_ADDR,v)
#define HWIO_GCC_PROC_HALT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PROC_HALT_ADDR,m,v,HWIO_GCC_PROC_HALT_IN)
#define HWIO_GCC_PROC_HALT_PROC_HALT_BMSK                                                              0x1
#define HWIO_GCC_PROC_HALT_PROC_HALT_SHFT                                                              0x0

#define HWIO_GCC_GCC_DEBUG_CLK_CTL_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00074000)
#define HWIO_GCC_GCC_DEBUG_CLK_CTL_RMSK                                                            0x1f1ff
#define HWIO_GCC_GCC_DEBUG_CLK_CTL_IN          \
        in_dword_masked(HWIO_GCC_GCC_DEBUG_CLK_CTL_ADDR, HWIO_GCC_GCC_DEBUG_CLK_CTL_RMSK)
#define HWIO_GCC_GCC_DEBUG_CLK_CTL_INM(m)      \
        in_dword_masked(HWIO_GCC_GCC_DEBUG_CLK_CTL_ADDR, m)
#define HWIO_GCC_GCC_DEBUG_CLK_CTL_OUT(v)      \
        out_dword(HWIO_GCC_GCC_DEBUG_CLK_CTL_ADDR,v)
#define HWIO_GCC_GCC_DEBUG_CLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GCC_DEBUG_CLK_CTL_ADDR,m,v,HWIO_GCC_GCC_DEBUG_CLK_CTL_IN)
#define HWIO_GCC_GCC_DEBUG_CLK_CTL_CLK_ENABLE_BMSK                                                 0x10000
#define HWIO_GCC_GCC_DEBUG_CLK_CTL_CLK_ENABLE_SHFT                                                    0x10
#define HWIO_GCC_GCC_DEBUG_CLK_CTL_POST_DIV_BMSK                                                    0xf000
#define HWIO_GCC_GCC_DEBUG_CLK_CTL_POST_DIV_SHFT                                                       0xc
#define HWIO_GCC_GCC_DEBUG_CLK_CTL_MUX_SEL_BMSK                                                      0x1ff
#define HWIO_GCC_GCC_DEBUG_CLK_CTL_MUX_SEL_SHFT                                                        0x0

#define HWIO_GCC_CLOCK_FRQ_MEASURE_CTL_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00074004)
#define HWIO_GCC_CLOCK_FRQ_MEASURE_CTL_RMSK                                                       0x1fffff
#define HWIO_GCC_CLOCK_FRQ_MEASURE_CTL_IN          \
        in_dword_masked(HWIO_GCC_CLOCK_FRQ_MEASURE_CTL_ADDR, HWIO_GCC_CLOCK_FRQ_MEASURE_CTL_RMSK)
#define HWIO_GCC_CLOCK_FRQ_MEASURE_CTL_INM(m)      \
        in_dword_masked(HWIO_GCC_CLOCK_FRQ_MEASURE_CTL_ADDR, m)
#define HWIO_GCC_CLOCK_FRQ_MEASURE_CTL_OUT(v)      \
        out_dword(HWIO_GCC_CLOCK_FRQ_MEASURE_CTL_ADDR,v)
#define HWIO_GCC_CLOCK_FRQ_MEASURE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CLOCK_FRQ_MEASURE_CTL_ADDR,m,v,HWIO_GCC_CLOCK_FRQ_MEASURE_CTL_IN)
#define HWIO_GCC_CLOCK_FRQ_MEASURE_CTL_CNT_EN_BMSK                                                0x100000
#define HWIO_GCC_CLOCK_FRQ_MEASURE_CTL_CNT_EN_SHFT                                                    0x14
#define HWIO_GCC_CLOCK_FRQ_MEASURE_CTL_XO_DIV4_TERM_CNT_BMSK                                       0xfffff
#define HWIO_GCC_CLOCK_FRQ_MEASURE_CTL_XO_DIV4_TERM_CNT_SHFT                                           0x0

#define HWIO_GCC_CLOCK_FRQ_MEASURE_STATUS_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00074008)
#define HWIO_GCC_CLOCK_FRQ_MEASURE_STATUS_RMSK                                                   0x3ffffff
#define HWIO_GCC_CLOCK_FRQ_MEASURE_STATUS_IN          \
        in_dword_masked(HWIO_GCC_CLOCK_FRQ_MEASURE_STATUS_ADDR, HWIO_GCC_CLOCK_FRQ_MEASURE_STATUS_RMSK)
#define HWIO_GCC_CLOCK_FRQ_MEASURE_STATUS_INM(m)      \
        in_dword_masked(HWIO_GCC_CLOCK_FRQ_MEASURE_STATUS_ADDR, m)
#define HWIO_GCC_CLOCK_FRQ_MEASURE_STATUS_XO_DIV4_CNT_DONE_BMSK                                  0x2000000
#define HWIO_GCC_CLOCK_FRQ_MEASURE_STATUS_XO_DIV4_CNT_DONE_SHFT                                       0x19
#define HWIO_GCC_CLOCK_FRQ_MEASURE_STATUS_MEASURE_CNT_BMSK                                       0x1ffffff
#define HWIO_GCC_CLOCK_FRQ_MEASURE_STATUS_MEASURE_CNT_SHFT                                             0x0

#define HWIO_GCC_PLLTEST_PAD_CFG_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0007400c)
#define HWIO_GCC_PLLTEST_PAD_CFG_RMSK                                                              0xfbfbf
#define HWIO_GCC_PLLTEST_PAD_CFG_IN          \
        in_dword_masked(HWIO_GCC_PLLTEST_PAD_CFG_ADDR, HWIO_GCC_PLLTEST_PAD_CFG_RMSK)
#define HWIO_GCC_PLLTEST_PAD_CFG_INM(m)      \
        in_dword_masked(HWIO_GCC_PLLTEST_PAD_CFG_ADDR, m)
#define HWIO_GCC_PLLTEST_PAD_CFG_OUT(v)      \
        out_dword(HWIO_GCC_PLLTEST_PAD_CFG_ADDR,v)
#define HWIO_GCC_PLLTEST_PAD_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PLLTEST_PAD_CFG_ADDR,m,v,HWIO_GCC_PLLTEST_PAD_CFG_IN)
#define HWIO_GCC_PLLTEST_PAD_CFG_PLL_INPUT_N_BMSK                                                  0xc0000
#define HWIO_GCC_PLLTEST_PAD_CFG_PLL_INPUT_N_SHFT                                                     0x12
#define HWIO_GCC_PLLTEST_PAD_CFG_PLL_INPUT_P_BMSK                                                  0x30000
#define HWIO_GCC_PLLTEST_PAD_CFG_PLL_INPUT_P_SHFT                                                     0x10
#define HWIO_GCC_PLLTEST_PAD_CFG_DIVIDER_EN_BMSK                                                    0x8000
#define HWIO_GCC_PLLTEST_PAD_CFG_DIVIDER_EN_SHFT                                                       0xf
#define HWIO_GCC_PLLTEST_PAD_CFG_RT_EN_BMSK                                                         0x2000
#define HWIO_GCC_PLLTEST_PAD_CFG_RT_EN_SHFT                                                            0xd
#define HWIO_GCC_PLLTEST_PAD_CFG_CORE_OE_BMSK                                                       0x1000
#define HWIO_GCC_PLLTEST_PAD_CFG_CORE_OE_SHFT                                                          0xc
#define HWIO_GCC_PLLTEST_PAD_CFG_CORE_IE_N_BMSK                                                      0x800
#define HWIO_GCC_PLLTEST_PAD_CFG_CORE_IE_N_SHFT                                                        0xb
#define HWIO_GCC_PLLTEST_PAD_CFG_CORE_IE_P_BMSK                                                      0x400
#define HWIO_GCC_PLLTEST_PAD_CFG_CORE_IE_P_SHFT                                                        0xa
#define HWIO_GCC_PLLTEST_PAD_CFG_CORE_DRIVE_BMSK                                                     0x380
#define HWIO_GCC_PLLTEST_PAD_CFG_CORE_DRIVE_SHFT                                                       0x7
#define HWIO_GCC_PLLTEST_PAD_CFG_OUT_SEL_BMSK                                                         0x3f
#define HWIO_GCC_PLLTEST_PAD_CFG_OUT_SEL_SHFT                                                          0x0

#define HWIO_GCC_JITTER_PROBE_CFG_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00074010)
#define HWIO_GCC_JITTER_PROBE_CFG_RMSK                                                               0x1ff
#define HWIO_GCC_JITTER_PROBE_CFG_IN          \
        in_dword_masked(HWIO_GCC_JITTER_PROBE_CFG_ADDR, HWIO_GCC_JITTER_PROBE_CFG_RMSK)
#define HWIO_GCC_JITTER_PROBE_CFG_INM(m)      \
        in_dword_masked(HWIO_GCC_JITTER_PROBE_CFG_ADDR, m)
#define HWIO_GCC_JITTER_PROBE_CFG_OUT(v)      \
        out_dword(HWIO_GCC_JITTER_PROBE_CFG_ADDR,v)
#define HWIO_GCC_JITTER_PROBE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_JITTER_PROBE_CFG_ADDR,m,v,HWIO_GCC_JITTER_PROBE_CFG_IN)
#define HWIO_GCC_JITTER_PROBE_CFG_JITTER_PROBE_EN_BMSK                                               0x100
#define HWIO_GCC_JITTER_PROBE_CFG_JITTER_PROBE_EN_SHFT                                                 0x8
#define HWIO_GCC_JITTER_PROBE_CFG_INIT_COUNTER_BMSK                                                   0xff
#define HWIO_GCC_JITTER_PROBE_CFG_INIT_COUNTER_SHFT                                                    0x0

#define HWIO_GCC_JITTER_PROBE_VAL_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00074014)
#define HWIO_GCC_JITTER_PROBE_VAL_RMSK                                                                0xff
#define HWIO_GCC_JITTER_PROBE_VAL_IN          \
        in_dword_masked(HWIO_GCC_JITTER_PROBE_VAL_ADDR, HWIO_GCC_JITTER_PROBE_VAL_RMSK)
#define HWIO_GCC_JITTER_PROBE_VAL_INM(m)      \
        in_dword_masked(HWIO_GCC_JITTER_PROBE_VAL_ADDR, m)
#define HWIO_GCC_JITTER_PROBE_VAL_COUNT_VALUE_BMSK                                                    0xff
#define HWIO_GCC_JITTER_PROBE_VAL_COUNT_VALUE_SHFT                                                     0x0

#define HWIO_GCC_GP1_CBCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00008000)
#define HWIO_GCC_GP1_CBCR_RMSK                                                                  0x80000001
#define HWIO_GCC_GP1_CBCR_IN          \
        in_dword_masked(HWIO_GCC_GP1_CBCR_ADDR, HWIO_GCC_GP1_CBCR_RMSK)
#define HWIO_GCC_GP1_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_GP1_CBCR_ADDR, m)
#define HWIO_GCC_GP1_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_GP1_CBCR_ADDR,v)
#define HWIO_GCC_GP1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GP1_CBCR_ADDR,m,v,HWIO_GCC_GP1_CBCR_IN)
#define HWIO_GCC_GP1_CBCR_CLK_OFF_BMSK                                                          0x80000000
#define HWIO_GCC_GP1_CBCR_CLK_OFF_SHFT                                                                0x1f
#define HWIO_GCC_GP1_CBCR_CLK_ENABLE_BMSK                                                              0x1
#define HWIO_GCC_GP1_CBCR_CLK_ENABLE_SHFT                                                              0x0

#define HWIO_GCC_GP1_CMD_RCGR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00008004)
#define HWIO_GCC_GP1_CMD_RCGR_RMSK                                                              0x800000f3
#define HWIO_GCC_GP1_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_GP1_CMD_RCGR_ADDR, HWIO_GCC_GP1_CMD_RCGR_RMSK)
#define HWIO_GCC_GP1_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_GP1_CMD_RCGR_ADDR, m)
#define HWIO_GCC_GP1_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_GP1_CMD_RCGR_ADDR,v)
#define HWIO_GCC_GP1_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GP1_CMD_RCGR_ADDR,m,v,HWIO_GCC_GP1_CMD_RCGR_IN)
#define HWIO_GCC_GP1_CMD_RCGR_ROOT_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_GP1_CMD_RCGR_ROOT_OFF_SHFT                                                           0x1f
#define HWIO_GCC_GP1_CMD_RCGR_DIRTY_D_BMSK                                                            0x80
#define HWIO_GCC_GP1_CMD_RCGR_DIRTY_D_SHFT                                                             0x7
#define HWIO_GCC_GP1_CMD_RCGR_DIRTY_M_BMSK                                                            0x40
#define HWIO_GCC_GP1_CMD_RCGR_DIRTY_M_SHFT                                                             0x6
#define HWIO_GCC_GP1_CMD_RCGR_DIRTY_N_BMSK                                                            0x20
#define HWIO_GCC_GP1_CMD_RCGR_DIRTY_N_SHFT                                                             0x5
#define HWIO_GCC_GP1_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                     0x10
#define HWIO_GCC_GP1_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                      0x4
#define HWIO_GCC_GP1_CMD_RCGR_ROOT_EN_BMSK                                                             0x2
#define HWIO_GCC_GP1_CMD_RCGR_ROOT_EN_SHFT                                                             0x1
#define HWIO_GCC_GP1_CMD_RCGR_UPDATE_BMSK                                                              0x1
#define HWIO_GCC_GP1_CMD_RCGR_UPDATE_SHFT                                                              0x0

#define HWIO_GCC_GP1_CFG_RCGR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00008008)
#define HWIO_GCC_GP1_CFG_RCGR_RMSK                                                                  0x371f
#define HWIO_GCC_GP1_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_GP1_CFG_RCGR_ADDR, HWIO_GCC_GP1_CFG_RCGR_RMSK)
#define HWIO_GCC_GP1_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_GP1_CFG_RCGR_ADDR, m)
#define HWIO_GCC_GP1_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_GP1_CFG_RCGR_ADDR,v)
#define HWIO_GCC_GP1_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GP1_CFG_RCGR_ADDR,m,v,HWIO_GCC_GP1_CFG_RCGR_IN)
#define HWIO_GCC_GP1_CFG_RCGR_MODE_BMSK                                                             0x3000
#define HWIO_GCC_GP1_CFG_RCGR_MODE_SHFT                                                                0xc
#define HWIO_GCC_GP1_CFG_RCGR_SRC_SEL_BMSK                                                           0x700
#define HWIO_GCC_GP1_CFG_RCGR_SRC_SEL_SHFT                                                             0x8
#define HWIO_GCC_GP1_CFG_RCGR_SRC_DIV_BMSK                                                            0x1f
#define HWIO_GCC_GP1_CFG_RCGR_SRC_DIV_SHFT                                                             0x0

#define HWIO_GCC_GP1_M_ADDR                                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x0000800c)
#define HWIO_GCC_GP1_M_RMSK                                                                           0xff
#define HWIO_GCC_GP1_M_IN          \
        in_dword_masked(HWIO_GCC_GP1_M_ADDR, HWIO_GCC_GP1_M_RMSK)
#define HWIO_GCC_GP1_M_INM(m)      \
        in_dword_masked(HWIO_GCC_GP1_M_ADDR, m)
#define HWIO_GCC_GP1_M_OUT(v)      \
        out_dword(HWIO_GCC_GP1_M_ADDR,v)
#define HWIO_GCC_GP1_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GP1_M_ADDR,m,v,HWIO_GCC_GP1_M_IN)
#define HWIO_GCC_GP1_M_M_BMSK                                                                         0xff
#define HWIO_GCC_GP1_M_M_SHFT                                                                          0x0

#define HWIO_GCC_GP1_N_ADDR                                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00008010)
#define HWIO_GCC_GP1_N_RMSK                                                                           0xff
#define HWIO_GCC_GP1_N_IN          \
        in_dword_masked(HWIO_GCC_GP1_N_ADDR, HWIO_GCC_GP1_N_RMSK)
#define HWIO_GCC_GP1_N_INM(m)      \
        in_dword_masked(HWIO_GCC_GP1_N_ADDR, m)
#define HWIO_GCC_GP1_N_OUT(v)      \
        out_dword(HWIO_GCC_GP1_N_ADDR,v)
#define HWIO_GCC_GP1_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GP1_N_ADDR,m,v,HWIO_GCC_GP1_N_IN)
#define HWIO_GCC_GP1_N_NOT_N_MINUS_M_BMSK                                                             0xff
#define HWIO_GCC_GP1_N_NOT_N_MINUS_M_SHFT                                                              0x0

#define HWIO_GCC_GP1_D_ADDR                                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00008014)
#define HWIO_GCC_GP1_D_RMSK                                                                           0xff
#define HWIO_GCC_GP1_D_IN          \
        in_dword_masked(HWIO_GCC_GP1_D_ADDR, HWIO_GCC_GP1_D_RMSK)
#define HWIO_GCC_GP1_D_INM(m)      \
        in_dword_masked(HWIO_GCC_GP1_D_ADDR, m)
#define HWIO_GCC_GP1_D_OUT(v)      \
        out_dword(HWIO_GCC_GP1_D_ADDR,v)
#define HWIO_GCC_GP1_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GP1_D_ADDR,m,v,HWIO_GCC_GP1_D_IN)
#define HWIO_GCC_GP1_D_NOT_2D_BMSK                                                                    0xff
#define HWIO_GCC_GP1_D_NOT_2D_SHFT                                                                     0x0

#define HWIO_GCC_GP2_CBCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00009000)
#define HWIO_GCC_GP2_CBCR_RMSK                                                                  0x80000001
#define HWIO_GCC_GP2_CBCR_IN          \
        in_dword_masked(HWIO_GCC_GP2_CBCR_ADDR, HWIO_GCC_GP2_CBCR_RMSK)
#define HWIO_GCC_GP2_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_GP2_CBCR_ADDR, m)
#define HWIO_GCC_GP2_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_GP2_CBCR_ADDR,v)
#define HWIO_GCC_GP2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GP2_CBCR_ADDR,m,v,HWIO_GCC_GP2_CBCR_IN)
#define HWIO_GCC_GP2_CBCR_CLK_OFF_BMSK                                                          0x80000000
#define HWIO_GCC_GP2_CBCR_CLK_OFF_SHFT                                                                0x1f
#define HWIO_GCC_GP2_CBCR_CLK_ENABLE_BMSK                                                              0x1
#define HWIO_GCC_GP2_CBCR_CLK_ENABLE_SHFT                                                              0x0

#define HWIO_GCC_GP2_CMD_RCGR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00009004)
#define HWIO_GCC_GP2_CMD_RCGR_RMSK                                                              0x800000f3
#define HWIO_GCC_GP2_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_GP2_CMD_RCGR_ADDR, HWIO_GCC_GP2_CMD_RCGR_RMSK)
#define HWIO_GCC_GP2_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_GP2_CMD_RCGR_ADDR, m)
#define HWIO_GCC_GP2_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_GP2_CMD_RCGR_ADDR,v)
#define HWIO_GCC_GP2_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GP2_CMD_RCGR_ADDR,m,v,HWIO_GCC_GP2_CMD_RCGR_IN)
#define HWIO_GCC_GP2_CMD_RCGR_ROOT_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_GP2_CMD_RCGR_ROOT_OFF_SHFT                                                           0x1f
#define HWIO_GCC_GP2_CMD_RCGR_DIRTY_D_BMSK                                                            0x80
#define HWIO_GCC_GP2_CMD_RCGR_DIRTY_D_SHFT                                                             0x7
#define HWIO_GCC_GP2_CMD_RCGR_DIRTY_M_BMSK                                                            0x40
#define HWIO_GCC_GP2_CMD_RCGR_DIRTY_M_SHFT                                                             0x6
#define HWIO_GCC_GP2_CMD_RCGR_DIRTY_N_BMSK                                                            0x20
#define HWIO_GCC_GP2_CMD_RCGR_DIRTY_N_SHFT                                                             0x5
#define HWIO_GCC_GP2_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                     0x10
#define HWIO_GCC_GP2_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                      0x4
#define HWIO_GCC_GP2_CMD_RCGR_ROOT_EN_BMSK                                                             0x2
#define HWIO_GCC_GP2_CMD_RCGR_ROOT_EN_SHFT                                                             0x1
#define HWIO_GCC_GP2_CMD_RCGR_UPDATE_BMSK                                                              0x1
#define HWIO_GCC_GP2_CMD_RCGR_UPDATE_SHFT                                                              0x0

#define HWIO_GCC_GP2_CFG_RCGR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00009008)
#define HWIO_GCC_GP2_CFG_RCGR_RMSK                                                                  0x371f
#define HWIO_GCC_GP2_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_GP2_CFG_RCGR_ADDR, HWIO_GCC_GP2_CFG_RCGR_RMSK)
#define HWIO_GCC_GP2_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_GP2_CFG_RCGR_ADDR, m)
#define HWIO_GCC_GP2_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_GP2_CFG_RCGR_ADDR,v)
#define HWIO_GCC_GP2_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GP2_CFG_RCGR_ADDR,m,v,HWIO_GCC_GP2_CFG_RCGR_IN)
#define HWIO_GCC_GP2_CFG_RCGR_MODE_BMSK                                                             0x3000
#define HWIO_GCC_GP2_CFG_RCGR_MODE_SHFT                                                                0xc
#define HWIO_GCC_GP2_CFG_RCGR_SRC_SEL_BMSK                                                           0x700
#define HWIO_GCC_GP2_CFG_RCGR_SRC_SEL_SHFT                                                             0x8
#define HWIO_GCC_GP2_CFG_RCGR_SRC_DIV_BMSK                                                            0x1f
#define HWIO_GCC_GP2_CFG_RCGR_SRC_DIV_SHFT                                                             0x0

#define HWIO_GCC_GP2_M_ADDR                                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x0000900c)
#define HWIO_GCC_GP2_M_RMSK                                                                           0xff
#define HWIO_GCC_GP2_M_IN          \
        in_dword_masked(HWIO_GCC_GP2_M_ADDR, HWIO_GCC_GP2_M_RMSK)
#define HWIO_GCC_GP2_M_INM(m)      \
        in_dword_masked(HWIO_GCC_GP2_M_ADDR, m)
#define HWIO_GCC_GP2_M_OUT(v)      \
        out_dword(HWIO_GCC_GP2_M_ADDR,v)
#define HWIO_GCC_GP2_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GP2_M_ADDR,m,v,HWIO_GCC_GP2_M_IN)
#define HWIO_GCC_GP2_M_M_BMSK                                                                         0xff
#define HWIO_GCC_GP2_M_M_SHFT                                                                          0x0

#define HWIO_GCC_GP2_N_ADDR                                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00009010)
#define HWIO_GCC_GP2_N_RMSK                                                                           0xff
#define HWIO_GCC_GP2_N_IN          \
        in_dword_masked(HWIO_GCC_GP2_N_ADDR, HWIO_GCC_GP2_N_RMSK)
#define HWIO_GCC_GP2_N_INM(m)      \
        in_dword_masked(HWIO_GCC_GP2_N_ADDR, m)
#define HWIO_GCC_GP2_N_OUT(v)      \
        out_dword(HWIO_GCC_GP2_N_ADDR,v)
#define HWIO_GCC_GP2_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GP2_N_ADDR,m,v,HWIO_GCC_GP2_N_IN)
#define HWIO_GCC_GP2_N_NOT_N_MINUS_M_BMSK                                                             0xff
#define HWIO_GCC_GP2_N_NOT_N_MINUS_M_SHFT                                                              0x0

#define HWIO_GCC_GP2_D_ADDR                                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00009014)
#define HWIO_GCC_GP2_D_RMSK                                                                           0xff
#define HWIO_GCC_GP2_D_IN          \
        in_dword_masked(HWIO_GCC_GP2_D_ADDR, HWIO_GCC_GP2_D_RMSK)
#define HWIO_GCC_GP2_D_INM(m)      \
        in_dword_masked(HWIO_GCC_GP2_D_ADDR, m)
#define HWIO_GCC_GP2_D_OUT(v)      \
        out_dword(HWIO_GCC_GP2_D_ADDR,v)
#define HWIO_GCC_GP2_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GP2_D_ADDR,m,v,HWIO_GCC_GP2_D_IN)
#define HWIO_GCC_GP2_D_NOT_2D_BMSK                                                                    0xff
#define HWIO_GCC_GP2_D_NOT_2D_SHFT                                                                     0x0

#define HWIO_GCC_GP3_CBCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0000a000)
#define HWIO_GCC_GP3_CBCR_RMSK                                                                  0x80000001
#define HWIO_GCC_GP3_CBCR_IN          \
        in_dword_masked(HWIO_GCC_GP3_CBCR_ADDR, HWIO_GCC_GP3_CBCR_RMSK)
#define HWIO_GCC_GP3_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_GP3_CBCR_ADDR, m)
#define HWIO_GCC_GP3_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_GP3_CBCR_ADDR,v)
#define HWIO_GCC_GP3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GP3_CBCR_ADDR,m,v,HWIO_GCC_GP3_CBCR_IN)
#define HWIO_GCC_GP3_CBCR_CLK_OFF_BMSK                                                          0x80000000
#define HWIO_GCC_GP3_CBCR_CLK_OFF_SHFT                                                                0x1f
#define HWIO_GCC_GP3_CBCR_CLK_ENABLE_BMSK                                                              0x1
#define HWIO_GCC_GP3_CBCR_CLK_ENABLE_SHFT                                                              0x0

#define HWIO_GCC_GP3_CMD_RCGR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0000a004)
#define HWIO_GCC_GP3_CMD_RCGR_RMSK                                                              0x800000f3
#define HWIO_GCC_GP3_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_GP3_CMD_RCGR_ADDR, HWIO_GCC_GP3_CMD_RCGR_RMSK)
#define HWIO_GCC_GP3_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_GP3_CMD_RCGR_ADDR, m)
#define HWIO_GCC_GP3_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_GP3_CMD_RCGR_ADDR,v)
#define HWIO_GCC_GP3_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GP3_CMD_RCGR_ADDR,m,v,HWIO_GCC_GP3_CMD_RCGR_IN)
#define HWIO_GCC_GP3_CMD_RCGR_ROOT_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_GP3_CMD_RCGR_ROOT_OFF_SHFT                                                           0x1f
#define HWIO_GCC_GP3_CMD_RCGR_DIRTY_D_BMSK                                                            0x80
#define HWIO_GCC_GP3_CMD_RCGR_DIRTY_D_SHFT                                                             0x7
#define HWIO_GCC_GP3_CMD_RCGR_DIRTY_M_BMSK                                                            0x40
#define HWIO_GCC_GP3_CMD_RCGR_DIRTY_M_SHFT                                                             0x6
#define HWIO_GCC_GP3_CMD_RCGR_DIRTY_N_BMSK                                                            0x20
#define HWIO_GCC_GP3_CMD_RCGR_DIRTY_N_SHFT                                                             0x5
#define HWIO_GCC_GP3_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                     0x10
#define HWIO_GCC_GP3_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                      0x4
#define HWIO_GCC_GP3_CMD_RCGR_ROOT_EN_BMSK                                                             0x2
#define HWIO_GCC_GP3_CMD_RCGR_ROOT_EN_SHFT                                                             0x1
#define HWIO_GCC_GP3_CMD_RCGR_UPDATE_BMSK                                                              0x1
#define HWIO_GCC_GP3_CMD_RCGR_UPDATE_SHFT                                                              0x0

#define HWIO_GCC_GP3_CFG_RCGR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0000a008)
#define HWIO_GCC_GP3_CFG_RCGR_RMSK                                                                  0x371f
#define HWIO_GCC_GP3_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_GP3_CFG_RCGR_ADDR, HWIO_GCC_GP3_CFG_RCGR_RMSK)
#define HWIO_GCC_GP3_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_GP3_CFG_RCGR_ADDR, m)
#define HWIO_GCC_GP3_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_GP3_CFG_RCGR_ADDR,v)
#define HWIO_GCC_GP3_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GP3_CFG_RCGR_ADDR,m,v,HWIO_GCC_GP3_CFG_RCGR_IN)
#define HWIO_GCC_GP3_CFG_RCGR_MODE_BMSK                                                             0x3000
#define HWIO_GCC_GP3_CFG_RCGR_MODE_SHFT                                                                0xc
#define HWIO_GCC_GP3_CFG_RCGR_SRC_SEL_BMSK                                                           0x700
#define HWIO_GCC_GP3_CFG_RCGR_SRC_SEL_SHFT                                                             0x8
#define HWIO_GCC_GP3_CFG_RCGR_SRC_DIV_BMSK                                                            0x1f
#define HWIO_GCC_GP3_CFG_RCGR_SRC_DIV_SHFT                                                             0x0

#define HWIO_GCC_GP3_M_ADDR                                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x0000a00c)
#define HWIO_GCC_GP3_M_RMSK                                                                           0xff
#define HWIO_GCC_GP3_M_IN          \
        in_dword_masked(HWIO_GCC_GP3_M_ADDR, HWIO_GCC_GP3_M_RMSK)
#define HWIO_GCC_GP3_M_INM(m)      \
        in_dword_masked(HWIO_GCC_GP3_M_ADDR, m)
#define HWIO_GCC_GP3_M_OUT(v)      \
        out_dword(HWIO_GCC_GP3_M_ADDR,v)
#define HWIO_GCC_GP3_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GP3_M_ADDR,m,v,HWIO_GCC_GP3_M_IN)
#define HWIO_GCC_GP3_M_M_BMSK                                                                         0xff
#define HWIO_GCC_GP3_M_M_SHFT                                                                          0x0

#define HWIO_GCC_GP3_N_ADDR                                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x0000a010)
#define HWIO_GCC_GP3_N_RMSK                                                                           0xff
#define HWIO_GCC_GP3_N_IN          \
        in_dword_masked(HWIO_GCC_GP3_N_ADDR, HWIO_GCC_GP3_N_RMSK)
#define HWIO_GCC_GP3_N_INM(m)      \
        in_dword_masked(HWIO_GCC_GP3_N_ADDR, m)
#define HWIO_GCC_GP3_N_OUT(v)      \
        out_dword(HWIO_GCC_GP3_N_ADDR,v)
#define HWIO_GCC_GP3_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GP3_N_ADDR,m,v,HWIO_GCC_GP3_N_IN)
#define HWIO_GCC_GP3_N_NOT_N_MINUS_M_BMSK                                                             0xff
#define HWIO_GCC_GP3_N_NOT_N_MINUS_M_SHFT                                                              0x0

#define HWIO_GCC_GP3_D_ADDR                                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x0000a014)
#define HWIO_GCC_GP3_D_RMSK                                                                           0xff
#define HWIO_GCC_GP3_D_IN          \
        in_dword_masked(HWIO_GCC_GP3_D_ADDR, HWIO_GCC_GP3_D_RMSK)
#define HWIO_GCC_GP3_D_INM(m)      \
        in_dword_masked(HWIO_GCC_GP3_D_ADDR, m)
#define HWIO_GCC_GP3_D_OUT(v)      \
        out_dword(HWIO_GCC_GP3_D_ADDR,v)
#define HWIO_GCC_GP3_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GP3_D_ADDR,m,v,HWIO_GCC_GP3_D_IN)
#define HWIO_GCC_GP3_D_NOT_2D_BMSK                                                                    0xff
#define HWIO_GCC_GP3_D_NOT_2D_SHFT                                                                     0x0

#define HWIO_GCC_APSS_MISC_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00060000)
#define HWIO_GCC_APSS_MISC_RMSK                                                                        0x7
#define HWIO_GCC_APSS_MISC_IN          \
        in_dword_masked(HWIO_GCC_APSS_MISC_ADDR, HWIO_GCC_APSS_MISC_RMSK)
#define HWIO_GCC_APSS_MISC_INM(m)      \
        in_dword_masked(HWIO_GCC_APSS_MISC_ADDR, m)
#define HWIO_GCC_APSS_MISC_OUT(v)      \
        out_dword(HWIO_GCC_APSS_MISC_ADDR,v)
#define HWIO_GCC_APSS_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APSS_MISC_ADDR,m,v,HWIO_GCC_APSS_MISC_IN)
#define HWIO_GCC_APSS_MISC_AUX2_CLK_ENABLE_BMSK                                                        0x4
#define HWIO_GCC_APSS_MISC_AUX2_CLK_ENABLE_SHFT                                                        0x2
#define HWIO_GCC_APSS_MISC_AUX1_CLK_ENABLE_BMSK                                                        0x2
#define HWIO_GCC_APSS_MISC_AUX1_CLK_ENABLE_SHFT                                                        0x1
#define HWIO_GCC_APSS_MISC_RPM_RESET_REMOVAL_BMSK                                                      0x1
#define HWIO_GCC_APSS_MISC_RPM_RESET_REMOVAL_SHFT                                                      0x0

#define HWIO_GCC_TIC_MODE_APSS_BOOT_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0007f000)
#define HWIO_GCC_TIC_MODE_APSS_BOOT_RMSK                                                               0x1
#define HWIO_GCC_TIC_MODE_APSS_BOOT_IN          \
        in_dword_masked(HWIO_GCC_TIC_MODE_APSS_BOOT_ADDR, HWIO_GCC_TIC_MODE_APSS_BOOT_RMSK)
#define HWIO_GCC_TIC_MODE_APSS_BOOT_INM(m)      \
        in_dword_masked(HWIO_GCC_TIC_MODE_APSS_BOOT_ADDR, m)
#define HWIO_GCC_TIC_MODE_APSS_BOOT_OUT(v)      \
        out_dword(HWIO_GCC_TIC_MODE_APSS_BOOT_ADDR,v)
#define HWIO_GCC_TIC_MODE_APSS_BOOT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_TIC_MODE_APSS_BOOT_ADDR,m,v,HWIO_GCC_TIC_MODE_APSS_BOOT_IN)
#define HWIO_GCC_TIC_MODE_APSS_BOOT_APSS_BOOT_IN_TIC_MODE_BMSK                                         0x1
#define HWIO_GCC_TIC_MODE_APSS_BOOT_APSS_BOOT_IN_TIC_MODE_SHFT                                         0x0

#define HWIO_GCC_USB_BOOT_CLOCK_CTL_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00040000)
#define HWIO_GCC_USB_BOOT_CLOCK_CTL_RMSK                                                               0x1
#define HWIO_GCC_USB_BOOT_CLOCK_CTL_IN          \
        in_dword_masked(HWIO_GCC_USB_BOOT_CLOCK_CTL_ADDR, HWIO_GCC_USB_BOOT_CLOCK_CTL_RMSK)
#define HWIO_GCC_USB_BOOT_CLOCK_CTL_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_BOOT_CLOCK_CTL_ADDR, m)
#define HWIO_GCC_USB_BOOT_CLOCK_CTL_OUT(v)      \
        out_dword(HWIO_GCC_USB_BOOT_CLOCK_CTL_ADDR,v)
#define HWIO_GCC_USB_BOOT_CLOCK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_BOOT_CLOCK_CTL_ADDR,m,v,HWIO_GCC_USB_BOOT_CLOCK_CTL_IN)
#define HWIO_GCC_USB_BOOT_CLOCK_CTL_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_USB_BOOT_CLOCK_CTL_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_RAW_SLEEP_CLK_CTRL_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00035000)
#define HWIO_GCC_RAW_SLEEP_CLK_CTRL_RMSK                                                               0x1
#define HWIO_GCC_RAW_SLEEP_CLK_CTRL_IN          \
        in_dword_masked(HWIO_GCC_RAW_SLEEP_CLK_CTRL_ADDR, HWIO_GCC_RAW_SLEEP_CLK_CTRL_RMSK)
#define HWIO_GCC_RAW_SLEEP_CLK_CTRL_INM(m)      \
        in_dword_masked(HWIO_GCC_RAW_SLEEP_CLK_CTRL_ADDR, m)
#define HWIO_GCC_RAW_SLEEP_CLK_CTRL_OUT(v)      \
        out_dword(HWIO_GCC_RAW_SLEEP_CLK_CTRL_ADDR,v)
#define HWIO_GCC_RAW_SLEEP_CLK_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RAW_SLEEP_CLK_CTRL_ADDR,m,v,HWIO_GCC_RAW_SLEEP_CLK_CTRL_IN)
#define HWIO_GCC_RAW_SLEEP_CLK_CTRL_GATING_DISABLE_BMSK                                                0x1
#define HWIO_GCC_RAW_SLEEP_CLK_CTRL_GATING_DISABLE_SHFT                                                0x0

#define HWIO_GCC_GCC_SPARE0_REG_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00000008)
#define HWIO_GCC_GCC_SPARE0_REG_RMSK                                                            0xffffffff
#define HWIO_GCC_GCC_SPARE0_REG_IN          \
        in_dword_masked(HWIO_GCC_GCC_SPARE0_REG_ADDR, HWIO_GCC_GCC_SPARE0_REG_RMSK)
#define HWIO_GCC_GCC_SPARE0_REG_INM(m)      \
        in_dword_masked(HWIO_GCC_GCC_SPARE0_REG_ADDR, m)
#define HWIO_GCC_GCC_SPARE0_REG_OUT(v)      \
        out_dword(HWIO_GCC_GCC_SPARE0_REG_ADDR,v)
#define HWIO_GCC_GCC_SPARE0_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GCC_SPARE0_REG_ADDR,m,v,HWIO_GCC_GCC_SPARE0_REG_IN)
#define HWIO_GCC_GCC_SPARE0_REG_SPARE_BITS_BMSK                                                 0xffffffff
#define HWIO_GCC_GCC_SPARE0_REG_SPARE_BITS_SHFT                                                        0x0

#define HWIO_GCC_GCC_SPARE1_REG_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0000000c)
#define HWIO_GCC_GCC_SPARE1_REG_RMSK                                                            0xffffffff
#define HWIO_GCC_GCC_SPARE1_REG_IN          \
        in_dword_masked(HWIO_GCC_GCC_SPARE1_REG_ADDR, HWIO_GCC_GCC_SPARE1_REG_RMSK)
#define HWIO_GCC_GCC_SPARE1_REG_INM(m)      \
        in_dword_masked(HWIO_GCC_GCC_SPARE1_REG_ADDR, m)
#define HWIO_GCC_GCC_SPARE1_REG_OUT(v)      \
        out_dword(HWIO_GCC_GCC_SPARE1_REG_ADDR,v)
#define HWIO_GCC_GCC_SPARE1_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GCC_SPARE1_REG_ADDR,m,v,HWIO_GCC_GCC_SPARE1_REG_IN)
#define HWIO_GCC_GCC_SPARE1_REG_SPARE_BITS_BMSK                                                 0xffffffff
#define HWIO_GCC_GCC_SPARE1_REG_SPARE_BITS_SHFT                                                        0x0

#define HWIO_GCC_GCC_SPARE2_REG_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0007e000)
#define HWIO_GCC_GCC_SPARE2_REG_RMSK                                                            0xffffffff
#define HWIO_GCC_GCC_SPARE2_REG_IN          \
        in_dword_masked(HWIO_GCC_GCC_SPARE2_REG_ADDR, HWIO_GCC_GCC_SPARE2_REG_RMSK)
#define HWIO_GCC_GCC_SPARE2_REG_INM(m)      \
        in_dword_masked(HWIO_GCC_GCC_SPARE2_REG_ADDR, m)
#define HWIO_GCC_GCC_SPARE2_REG_OUT(v)      \
        out_dword(HWIO_GCC_GCC_SPARE2_REG_ADDR,v)
#define HWIO_GCC_GCC_SPARE2_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GCC_SPARE2_REG_ADDR,m,v,HWIO_GCC_GCC_SPARE2_REG_IN)
#define HWIO_GCC_GCC_SPARE2_REG_SPARE_BITS_BMSK                                                 0xffffffff
#define HWIO_GCC_GCC_SPARE2_REG_SPARE_BITS_SHFT                                                        0x0

#define HWIO_GCC_GCC_SPARE3_REG_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0007e004)
#define HWIO_GCC_GCC_SPARE3_REG_RMSK                                                            0xffffffff
#define HWIO_GCC_GCC_SPARE3_REG_IN          \
        in_dword_masked(HWIO_GCC_GCC_SPARE3_REG_ADDR, HWIO_GCC_GCC_SPARE3_REG_RMSK)
#define HWIO_GCC_GCC_SPARE3_REG_INM(m)      \
        in_dword_masked(HWIO_GCC_GCC_SPARE3_REG_ADDR, m)
#define HWIO_GCC_GCC_SPARE3_REG_OUT(v)      \
        out_dword(HWIO_GCC_GCC_SPARE3_REG_ADDR,v)
#define HWIO_GCC_GCC_SPARE3_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GCC_SPARE3_REG_ADDR,m,v,HWIO_GCC_GCC_SPARE3_REG_IN)
#define HWIO_GCC_GCC_SPARE3_REG_SPARE_BITS_BMSK                                                 0xffffffff
#define HWIO_GCC_GCC_SPARE3_REG_SPARE_BITS_SHFT                                                        0x0

#define HWIO_GCC_DCD_BCR_ADDR                                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0002a000)
#define HWIO_GCC_DCD_BCR_RMSK                                                                          0x1
#define HWIO_GCC_DCD_BCR_IN          \
        in_dword_masked(HWIO_GCC_DCD_BCR_ADDR, HWIO_GCC_DCD_BCR_RMSK)
#define HWIO_GCC_DCD_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_DCD_BCR_ADDR, m)
#define HWIO_GCC_DCD_BCR_OUT(v)      \
        out_dword(HWIO_GCC_DCD_BCR_ADDR,v)
#define HWIO_GCC_DCD_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_DCD_BCR_ADDR,m,v,HWIO_GCC_DCD_BCR_IN)
#define HWIO_GCC_DCD_BCR_BLK_ARES_BMSK                                                                 0x1
#define HWIO_GCC_DCD_BCR_BLK_ARES_SHFT                                                                 0x0

#define HWIO_GCC_DCD_XO_CBCR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0002a004)
#define HWIO_GCC_DCD_XO_CBCR_RMSK                                                               0x80000001
#define HWIO_GCC_DCD_XO_CBCR_IN          \
        in_dword_masked(HWIO_GCC_DCD_XO_CBCR_ADDR, HWIO_GCC_DCD_XO_CBCR_RMSK)
#define HWIO_GCC_DCD_XO_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_DCD_XO_CBCR_ADDR, m)
#define HWIO_GCC_DCD_XO_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_DCD_XO_CBCR_ADDR,v)
#define HWIO_GCC_DCD_XO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_DCD_XO_CBCR_ADDR,m,v,HWIO_GCC_DCD_XO_CBCR_IN)
#define HWIO_GCC_DCD_XO_CBCR_CLK_OFF_BMSK                                                       0x80000000
#define HWIO_GCC_DCD_XO_CBCR_CLK_OFF_SHFT                                                             0x1f
#define HWIO_GCC_DCD_XO_CBCR_CLK_ENABLE_BMSK                                                           0x1
#define HWIO_GCC_DCD_XO_CBCR_CLK_ENABLE_SHFT                                                           0x0

#define HWIO_GCC_SNOC_DCD_CONFIG_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0002a008)
#define HWIO_GCC_SNOC_DCD_CONFIG_RMSK                                                           0x80007fff
#define HWIO_GCC_SNOC_DCD_CONFIG_IN          \
        in_dword_masked(HWIO_GCC_SNOC_DCD_CONFIG_ADDR, HWIO_GCC_SNOC_DCD_CONFIG_RMSK)
#define HWIO_GCC_SNOC_DCD_CONFIG_INM(m)      \
        in_dword_masked(HWIO_GCC_SNOC_DCD_CONFIG_ADDR, m)
#define HWIO_GCC_SNOC_DCD_CONFIG_OUT(v)      \
        out_dword(HWIO_GCC_SNOC_DCD_CONFIG_ADDR,v)
#define HWIO_GCC_SNOC_DCD_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SNOC_DCD_CONFIG_ADDR,m,v,HWIO_GCC_SNOC_DCD_CONFIG_IN)
#define HWIO_GCC_SNOC_DCD_CONFIG_DCD_ENABLE_BMSK                                                0x80000000
#define HWIO_GCC_SNOC_DCD_CONFIG_DCD_ENABLE_SHFT                                                      0x1f
#define HWIO_GCC_SNOC_DCD_CONFIG_ALLOWED_DIV_BMSK                                                   0x7fff
#define HWIO_GCC_SNOC_DCD_CONFIG_ALLOWED_DIV_SHFT                                                      0x0

#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_ADDR                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0002a00c)
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_RMSK                                                   0xffffffff
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_IN          \
        in_dword_masked(HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_ADDR, HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_RMSK)
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_INM(m)      \
        in_dword_masked(HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_ADDR, m)
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_OUT(v)      \
        out_dword(HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_ADDR,v)
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_ADDR,m,v,HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_IN)
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_FIRST_CNT_BMSK                                         0xfffff000
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_FIRST_CNT_SHFT                                                0xc
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_NEXT_CNT_BMSK                                               0xfff
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_NEXT_CNT_SHFT                                                 0x0

#define HWIO_GCC_PCNOC_DCD_CONFIG_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x0002a010)
#define HWIO_GCC_PCNOC_DCD_CONFIG_RMSK                                                          0x80007fff
#define HWIO_GCC_PCNOC_DCD_CONFIG_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_DCD_CONFIG_ADDR, HWIO_GCC_PCNOC_DCD_CONFIG_RMSK)
#define HWIO_GCC_PCNOC_DCD_CONFIG_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_DCD_CONFIG_ADDR, m)
#define HWIO_GCC_PCNOC_DCD_CONFIG_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_DCD_CONFIG_ADDR,v)
#define HWIO_GCC_PCNOC_DCD_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_DCD_CONFIG_ADDR,m,v,HWIO_GCC_PCNOC_DCD_CONFIG_IN)
#define HWIO_GCC_PCNOC_DCD_CONFIG_DCD_ENABLE_BMSK                                               0x80000000
#define HWIO_GCC_PCNOC_DCD_CONFIG_DCD_ENABLE_SHFT                                                     0x1f
#define HWIO_GCC_PCNOC_DCD_CONFIG_ALLOWED_DIV_BMSK                                                  0x7fff
#define HWIO_GCC_PCNOC_DCD_CONFIG_ALLOWED_DIV_SHFT                                                     0x0

#define HWIO_GCC_PCNOC_DCD_HYSTERESIS_CNT_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0002a014)
#define HWIO_GCC_PCNOC_DCD_HYSTERESIS_CNT_RMSK                                                  0xffffffff
#define HWIO_GCC_PCNOC_DCD_HYSTERESIS_CNT_IN          \
        in_dword_masked(HWIO_GCC_PCNOC_DCD_HYSTERESIS_CNT_ADDR, HWIO_GCC_PCNOC_DCD_HYSTERESIS_CNT_RMSK)
#define HWIO_GCC_PCNOC_DCD_HYSTERESIS_CNT_INM(m)      \
        in_dword_masked(HWIO_GCC_PCNOC_DCD_HYSTERESIS_CNT_ADDR, m)
#define HWIO_GCC_PCNOC_DCD_HYSTERESIS_CNT_OUT(v)      \
        out_dword(HWIO_GCC_PCNOC_DCD_HYSTERESIS_CNT_ADDR,v)
#define HWIO_GCC_PCNOC_DCD_HYSTERESIS_CNT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCNOC_DCD_HYSTERESIS_CNT_ADDR,m,v,HWIO_GCC_PCNOC_DCD_HYSTERESIS_CNT_IN)
#define HWIO_GCC_PCNOC_DCD_HYSTERESIS_CNT_FIRST_CNT_BMSK                                        0xfffff000
#define HWIO_GCC_PCNOC_DCD_HYSTERESIS_CNT_FIRST_CNT_SHFT                                               0xc
#define HWIO_GCC_PCNOC_DCD_HYSTERESIS_CNT_NEXT_CNT_BMSK                                              0xfff
#define HWIO_GCC_PCNOC_DCD_HYSTERESIS_CNT_NEXT_CNT_SHFT                                                0x0

#define HWIO_GCC_MM_SPDM_BCR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0002f024)
#define HWIO_GCC_MM_SPDM_BCR_RMSK                                                                      0x1
#define HWIO_GCC_MM_SPDM_BCR_IN          \
        in_dword_masked(HWIO_GCC_MM_SPDM_BCR_ADDR, HWIO_GCC_MM_SPDM_BCR_RMSK)
#define HWIO_GCC_MM_SPDM_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MM_SPDM_BCR_ADDR, m)
#define HWIO_GCC_MM_SPDM_BCR_OUT(v)      \
        out_dword(HWIO_GCC_MM_SPDM_BCR_ADDR,v)
#define HWIO_GCC_MM_SPDM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MM_SPDM_BCR_ADDR,m,v,HWIO_GCC_MM_SPDM_BCR_IN)
#define HWIO_GCC_MM_SPDM_BCR_BLK_ARES_BMSK                                                             0x1
#define HWIO_GCC_MM_SPDM_BCR_BLK_ARES_SHFT                                                             0x0

#define HWIO_GCC_SPDM_JPEG0_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0002f028)
#define HWIO_GCC_SPDM_JPEG0_CBCR_RMSK                                                           0x80000001
#define HWIO_GCC_SPDM_JPEG0_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SPDM_JPEG0_CBCR_ADDR, HWIO_GCC_SPDM_JPEG0_CBCR_RMSK)
#define HWIO_GCC_SPDM_JPEG0_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPDM_JPEG0_CBCR_ADDR, m)
#define HWIO_GCC_SPDM_JPEG0_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SPDM_JPEG0_CBCR_ADDR,v)
#define HWIO_GCC_SPDM_JPEG0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPDM_JPEG0_CBCR_ADDR,m,v,HWIO_GCC_SPDM_JPEG0_CBCR_IN)
#define HWIO_GCC_SPDM_JPEG0_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_SPDM_JPEG0_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_SPDM_JPEG0_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_SPDM_JPEG0_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_SPDM_MDP_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0002f02c)
#define HWIO_GCC_SPDM_MDP_CBCR_RMSK                                                             0x80000001
#define HWIO_GCC_SPDM_MDP_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SPDM_MDP_CBCR_ADDR, HWIO_GCC_SPDM_MDP_CBCR_RMSK)
#define HWIO_GCC_SPDM_MDP_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPDM_MDP_CBCR_ADDR, m)
#define HWIO_GCC_SPDM_MDP_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SPDM_MDP_CBCR_ADDR,v)
#define HWIO_GCC_SPDM_MDP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPDM_MDP_CBCR_ADDR,m,v,HWIO_GCC_SPDM_MDP_CBCR_IN)
#define HWIO_GCC_SPDM_MDP_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_SPDM_MDP_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_SPDM_MDP_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_GCC_SPDM_MDP_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_GCC_SPDM_VCODEC0_CBCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0002f034)
#define HWIO_GCC_SPDM_VCODEC0_CBCR_RMSK                                                         0x80000001
#define HWIO_GCC_SPDM_VCODEC0_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SPDM_VCODEC0_CBCR_ADDR, HWIO_GCC_SPDM_VCODEC0_CBCR_RMSK)
#define HWIO_GCC_SPDM_VCODEC0_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPDM_VCODEC0_CBCR_ADDR, m)
#define HWIO_GCC_SPDM_VCODEC0_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SPDM_VCODEC0_CBCR_ADDR,v)
#define HWIO_GCC_SPDM_VCODEC0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPDM_VCODEC0_CBCR_ADDR,m,v,HWIO_GCC_SPDM_VCODEC0_CBCR_IN)
#define HWIO_GCC_SPDM_VCODEC0_CBCR_CLK_OFF_BMSK                                                 0x80000000
#define HWIO_GCC_SPDM_VCODEC0_CBCR_CLK_OFF_SHFT                                                       0x1f
#define HWIO_GCC_SPDM_VCODEC0_CBCR_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_GCC_SPDM_VCODEC0_CBCR_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_GCC_SPDM_VFE0_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0002f038)
#define HWIO_GCC_SPDM_VFE0_CBCR_RMSK                                                            0x80000001
#define HWIO_GCC_SPDM_VFE0_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SPDM_VFE0_CBCR_ADDR, HWIO_GCC_SPDM_VFE0_CBCR_RMSK)
#define HWIO_GCC_SPDM_VFE0_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPDM_VFE0_CBCR_ADDR, m)
#define HWIO_GCC_SPDM_VFE0_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SPDM_VFE0_CBCR_ADDR,v)
#define HWIO_GCC_SPDM_VFE0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPDM_VFE0_CBCR_ADDR,m,v,HWIO_GCC_SPDM_VFE0_CBCR_IN)
#define HWIO_GCC_SPDM_VFE0_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_SPDM_VFE0_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_SPDM_VFE0_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_SPDM_VFE0_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_SPDM_GFX3D_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0002f03c)
#define HWIO_GCC_SPDM_GFX3D_CBCR_RMSK                                                           0x80000001
#define HWIO_GCC_SPDM_GFX3D_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SPDM_GFX3D_CBCR_ADDR, HWIO_GCC_SPDM_GFX3D_CBCR_RMSK)
#define HWIO_GCC_SPDM_GFX3D_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPDM_GFX3D_CBCR_ADDR, m)
#define HWIO_GCC_SPDM_GFX3D_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SPDM_GFX3D_CBCR_ADDR,v)
#define HWIO_GCC_SPDM_GFX3D_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPDM_GFX3D_CBCR_ADDR,m,v,HWIO_GCC_SPDM_GFX3D_CBCR_IN)
#define HWIO_GCC_SPDM_GFX3D_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_SPDM_GFX3D_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_SPDM_GFX3D_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_SPDM_GFX3D_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_SPDM_AHB_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0002f040)
#define HWIO_GCC_SPDM_AHB_CBCR_RMSK                                                             0x80000001
#define HWIO_GCC_SPDM_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SPDM_AHB_CBCR_ADDR, HWIO_GCC_SPDM_AHB_CBCR_RMSK)
#define HWIO_GCC_SPDM_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPDM_AHB_CBCR_ADDR, m)
#define HWIO_GCC_SPDM_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SPDM_AHB_CBCR_ADDR,v)
#define HWIO_GCC_SPDM_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPDM_AHB_CBCR_ADDR,m,v,HWIO_GCC_SPDM_AHB_CBCR_IN)
#define HWIO_GCC_SPDM_AHB_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_SPDM_AHB_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_SPDM_AHB_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_GCC_SPDM_AHB_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_GCC_SPDM_PCLK0_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0002f044)
#define HWIO_GCC_SPDM_PCLK0_CBCR_RMSK                                                           0x80000001
#define HWIO_GCC_SPDM_PCLK0_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SPDM_PCLK0_CBCR_ADDR, HWIO_GCC_SPDM_PCLK0_CBCR_RMSK)
#define HWIO_GCC_SPDM_PCLK0_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPDM_PCLK0_CBCR_ADDR, m)
#define HWIO_GCC_SPDM_PCLK0_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SPDM_PCLK0_CBCR_ADDR,v)
#define HWIO_GCC_SPDM_PCLK0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPDM_PCLK0_CBCR_ADDR,m,v,HWIO_GCC_SPDM_PCLK0_CBCR_IN)
#define HWIO_GCC_SPDM_PCLK0_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_SPDM_PCLK0_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_SPDM_PCLK0_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_SPDM_PCLK0_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_SPDM_CSI0_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0002f048)
#define HWIO_GCC_SPDM_CSI0_CBCR_RMSK                                                            0x80000001
#define HWIO_GCC_SPDM_CSI0_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SPDM_CSI0_CBCR_ADDR, HWIO_GCC_SPDM_CSI0_CBCR_RMSK)
#define HWIO_GCC_SPDM_CSI0_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SPDM_CSI0_CBCR_ADDR, m)
#define HWIO_GCC_SPDM_CSI0_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SPDM_CSI0_CBCR_ADDR,v)
#define HWIO_GCC_SPDM_CSI0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SPDM_CSI0_CBCR_ADDR,m,v,HWIO_GCC_SPDM_CSI0_CBCR_IN)
#define HWIO_GCC_SPDM_CSI0_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_SPDM_CSI0_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_SPDM_CSI0_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_SPDM_CSI0_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_VCODEC0_CMD_RCGR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x0004c000)
#define HWIO_GCC_VCODEC0_CMD_RCGR_RMSK                                                          0x800000f3
#define HWIO_GCC_VCODEC0_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_VCODEC0_CMD_RCGR_ADDR, HWIO_GCC_VCODEC0_CMD_RCGR_RMSK)
#define HWIO_GCC_VCODEC0_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_VCODEC0_CMD_RCGR_ADDR, m)
#define HWIO_GCC_VCODEC0_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_VCODEC0_CMD_RCGR_ADDR,v)
#define HWIO_GCC_VCODEC0_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_VCODEC0_CMD_RCGR_ADDR,m,v,HWIO_GCC_VCODEC0_CMD_RCGR_IN)
#define HWIO_GCC_VCODEC0_CMD_RCGR_ROOT_OFF_BMSK                                                 0x80000000
#define HWIO_GCC_VCODEC0_CMD_RCGR_ROOT_OFF_SHFT                                                       0x1f
#define HWIO_GCC_VCODEC0_CMD_RCGR_DIRTY_D_BMSK                                                        0x80
#define HWIO_GCC_VCODEC0_CMD_RCGR_DIRTY_D_SHFT                                                         0x7
#define HWIO_GCC_VCODEC0_CMD_RCGR_DIRTY_N_BMSK                                                        0x40
#define HWIO_GCC_VCODEC0_CMD_RCGR_DIRTY_N_SHFT                                                         0x6
#define HWIO_GCC_VCODEC0_CMD_RCGR_DIRTY_M_BMSK                                                        0x20
#define HWIO_GCC_VCODEC0_CMD_RCGR_DIRTY_M_SHFT                                                         0x5
#define HWIO_GCC_VCODEC0_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                 0x10
#define HWIO_GCC_VCODEC0_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                  0x4
#define HWIO_GCC_VCODEC0_CMD_RCGR_ROOT_EN_BMSK                                                         0x2
#define HWIO_GCC_VCODEC0_CMD_RCGR_ROOT_EN_SHFT                                                         0x1
#define HWIO_GCC_VCODEC0_CMD_RCGR_UPDATE_BMSK                                                          0x1
#define HWIO_GCC_VCODEC0_CMD_RCGR_UPDATE_SHFT                                                          0x0

#define HWIO_GCC_VCODEC0_CFG_RCGR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x0004c004)
#define HWIO_GCC_VCODEC0_CFG_RCGR_RMSK                                                              0x371f
#define HWIO_GCC_VCODEC0_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_VCODEC0_CFG_RCGR_ADDR, HWIO_GCC_VCODEC0_CFG_RCGR_RMSK)
#define HWIO_GCC_VCODEC0_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_VCODEC0_CFG_RCGR_ADDR, m)
#define HWIO_GCC_VCODEC0_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_VCODEC0_CFG_RCGR_ADDR,v)
#define HWIO_GCC_VCODEC0_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_VCODEC0_CFG_RCGR_ADDR,m,v,HWIO_GCC_VCODEC0_CFG_RCGR_IN)
#define HWIO_GCC_VCODEC0_CFG_RCGR_MODE_BMSK                                                         0x3000
#define HWIO_GCC_VCODEC0_CFG_RCGR_MODE_SHFT                                                            0xc
#define HWIO_GCC_VCODEC0_CFG_RCGR_SRC_SEL_BMSK                                                       0x700
#define HWIO_GCC_VCODEC0_CFG_RCGR_SRC_SEL_SHFT                                                         0x8
#define HWIO_GCC_VCODEC0_CFG_RCGR_SRC_DIV_BMSK                                                        0x1f
#define HWIO_GCC_VCODEC0_CFG_RCGR_SRC_DIV_SHFT                                                         0x0

#define HWIO_GCC_VCODEC0_M_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x0004c008)
#define HWIO_GCC_VCODEC0_M_RMSK                                                                       0xff
#define HWIO_GCC_VCODEC0_M_IN          \
        in_dword_masked(HWIO_GCC_VCODEC0_M_ADDR, HWIO_GCC_VCODEC0_M_RMSK)
#define HWIO_GCC_VCODEC0_M_INM(m)      \
        in_dword_masked(HWIO_GCC_VCODEC0_M_ADDR, m)
#define HWIO_GCC_VCODEC0_M_OUT(v)      \
        out_dword(HWIO_GCC_VCODEC0_M_ADDR,v)
#define HWIO_GCC_VCODEC0_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_VCODEC0_M_ADDR,m,v,HWIO_GCC_VCODEC0_M_IN)
#define HWIO_GCC_VCODEC0_M_M_BMSK                                                                     0xff
#define HWIO_GCC_VCODEC0_M_M_SHFT                                                                      0x0

#define HWIO_GCC_VCODEC0_N_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x0004c00c)
#define HWIO_GCC_VCODEC0_N_RMSK                                                                       0xff
#define HWIO_GCC_VCODEC0_N_IN          \
        in_dword_masked(HWIO_GCC_VCODEC0_N_ADDR, HWIO_GCC_VCODEC0_N_RMSK)
#define HWIO_GCC_VCODEC0_N_INM(m)      \
        in_dword_masked(HWIO_GCC_VCODEC0_N_ADDR, m)
#define HWIO_GCC_VCODEC0_N_OUT(v)      \
        out_dword(HWIO_GCC_VCODEC0_N_ADDR,v)
#define HWIO_GCC_VCODEC0_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_VCODEC0_N_ADDR,m,v,HWIO_GCC_VCODEC0_N_IN)
#define HWIO_GCC_VCODEC0_N_NOT_N_MINUS_M_BMSK                                                         0xff
#define HWIO_GCC_VCODEC0_N_NOT_N_MINUS_M_SHFT                                                          0x0

#define HWIO_GCC_VCODEC0_D_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x0004c010)
#define HWIO_GCC_VCODEC0_D_RMSK                                                                       0xff
#define HWIO_GCC_VCODEC0_D_IN          \
        in_dword_masked(HWIO_GCC_VCODEC0_D_ADDR, HWIO_GCC_VCODEC0_D_RMSK)
#define HWIO_GCC_VCODEC0_D_INM(m)      \
        in_dword_masked(HWIO_GCC_VCODEC0_D_ADDR, m)
#define HWIO_GCC_VCODEC0_D_OUT(v)      \
        out_dword(HWIO_GCC_VCODEC0_D_ADDR,v)
#define HWIO_GCC_VCODEC0_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_VCODEC0_D_ADDR,m,v,HWIO_GCC_VCODEC0_D_IN)
#define HWIO_GCC_VCODEC0_D_NOT_2D_BMSK                                                                0xff
#define HWIO_GCC_VCODEC0_D_NOT_2D_SHFT                                                                 0x0

#define HWIO_GCC_VENUS0_BCR_ADDR                                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x0004c014)
#define HWIO_GCC_VENUS0_BCR_RMSK                                                                       0x1
#define HWIO_GCC_VENUS0_BCR_IN          \
        in_dword_masked(HWIO_GCC_VENUS0_BCR_ADDR, HWIO_GCC_VENUS0_BCR_RMSK)
#define HWIO_GCC_VENUS0_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_VENUS0_BCR_ADDR, m)
#define HWIO_GCC_VENUS0_BCR_OUT(v)      \
        out_dword(HWIO_GCC_VENUS0_BCR_ADDR,v)
#define HWIO_GCC_VENUS0_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_VENUS0_BCR_ADDR,m,v,HWIO_GCC_VENUS0_BCR_IN)
#define HWIO_GCC_VENUS0_BCR_BLK_ARES_BMSK                                                              0x1
#define HWIO_GCC_VENUS0_BCR_BLK_ARES_SHFT                                                              0x0

#define HWIO_GCC_VENUS0_GDSCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0004c018)
#define HWIO_GCC_VENUS0_GDSCR_RMSK                                                              0xf8fff7ff
#define HWIO_GCC_VENUS0_GDSCR_IN          \
        in_dword_masked(HWIO_GCC_VENUS0_GDSCR_ADDR, HWIO_GCC_VENUS0_GDSCR_RMSK)
#define HWIO_GCC_VENUS0_GDSCR_INM(m)      \
        in_dword_masked(HWIO_GCC_VENUS0_GDSCR_ADDR, m)
#define HWIO_GCC_VENUS0_GDSCR_OUT(v)      \
        out_dword(HWIO_GCC_VENUS0_GDSCR_ADDR,v)
#define HWIO_GCC_VENUS0_GDSCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_VENUS0_GDSCR_ADDR,m,v,HWIO_GCC_VENUS0_GDSCR_IN)
#define HWIO_GCC_VENUS0_GDSCR_PWR_ON_BMSK                                                       0x80000000
#define HWIO_GCC_VENUS0_GDSCR_PWR_ON_SHFT                                                             0x1f
#define HWIO_GCC_VENUS0_GDSCR_GDSC_STATE_BMSK                                                   0x78000000
#define HWIO_GCC_VENUS0_GDSCR_GDSC_STATE_SHFT                                                         0x1b
#define HWIO_GCC_VENUS0_GDSCR_EN_REST_WAIT_BMSK                                                   0xf00000
#define HWIO_GCC_VENUS0_GDSCR_EN_REST_WAIT_SHFT                                                       0x14
#define HWIO_GCC_VENUS0_GDSCR_EN_FEW_WAIT_BMSK                                                     0xf0000
#define HWIO_GCC_VENUS0_GDSCR_EN_FEW_WAIT_SHFT                                                        0x10
#define HWIO_GCC_VENUS0_GDSCR_CLK_DIS_WAIT_BMSK                                                     0xf000
#define HWIO_GCC_VENUS0_GDSCR_CLK_DIS_WAIT_SHFT                                                        0xc
#define HWIO_GCC_VENUS0_GDSCR_RESTORE_BMSK                                                           0x400
#define HWIO_GCC_VENUS0_GDSCR_RESTORE_SHFT                                                             0xa
#define HWIO_GCC_VENUS0_GDSCR_SAVE_BMSK                                                              0x200
#define HWIO_GCC_VENUS0_GDSCR_SAVE_SHFT                                                                0x9
#define HWIO_GCC_VENUS0_GDSCR_RETAIN_BMSK                                                            0x100
#define HWIO_GCC_VENUS0_GDSCR_RETAIN_SHFT                                                              0x8
#define HWIO_GCC_VENUS0_GDSCR_EN_REST_BMSK                                                            0x80
#define HWIO_GCC_VENUS0_GDSCR_EN_REST_SHFT                                                             0x7
#define HWIO_GCC_VENUS0_GDSCR_EN_FEW_BMSK                                                             0x40
#define HWIO_GCC_VENUS0_GDSCR_EN_FEW_SHFT                                                              0x6
#define HWIO_GCC_VENUS0_GDSCR_CLAMP_IO_BMSK                                                           0x20
#define HWIO_GCC_VENUS0_GDSCR_CLAMP_IO_SHFT                                                            0x5
#define HWIO_GCC_VENUS0_GDSCR_CLK_DISABLE_BMSK                                                        0x10
#define HWIO_GCC_VENUS0_GDSCR_CLK_DISABLE_SHFT                                                         0x4
#define HWIO_GCC_VENUS0_GDSCR_PD_ARES_BMSK                                                             0x8
#define HWIO_GCC_VENUS0_GDSCR_PD_ARES_SHFT                                                             0x3
#define HWIO_GCC_VENUS0_GDSCR_SW_OVERRIDE_BMSK                                                         0x4
#define HWIO_GCC_VENUS0_GDSCR_SW_OVERRIDE_SHFT                                                         0x2
#define HWIO_GCC_VENUS0_GDSCR_HW_CONTROL_BMSK                                                          0x2
#define HWIO_GCC_VENUS0_GDSCR_HW_CONTROL_SHFT                                                          0x1
#define HWIO_GCC_VENUS0_GDSCR_SW_COLLAPSE_BMSK                                                         0x1
#define HWIO_GCC_VENUS0_GDSCR_SW_COLLAPSE_SHFT                                                         0x0

#define HWIO_GCC_VENUS0_VCODEC0_CBCR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x0004c01c)
#define HWIO_GCC_VENUS0_VCODEC0_CBCR_RMSK                                                       0x80007ff1
#define HWIO_GCC_VENUS0_VCODEC0_CBCR_IN          \
        in_dword_masked(HWIO_GCC_VENUS0_VCODEC0_CBCR_ADDR, HWIO_GCC_VENUS0_VCODEC0_CBCR_RMSK)
#define HWIO_GCC_VENUS0_VCODEC0_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_VENUS0_VCODEC0_CBCR_ADDR, m)
#define HWIO_GCC_VENUS0_VCODEC0_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_VENUS0_VCODEC0_CBCR_ADDR,v)
#define HWIO_GCC_VENUS0_VCODEC0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_VENUS0_VCODEC0_CBCR_ADDR,m,v,HWIO_GCC_VENUS0_VCODEC0_CBCR_IN)
#define HWIO_GCC_VENUS0_VCODEC0_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_GCC_VENUS0_VCODEC0_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_GCC_VENUS0_VCODEC0_CBCR_FORCE_MEM_CORE_ON_BMSK                                         0x4000
#define HWIO_GCC_VENUS0_VCODEC0_CBCR_FORCE_MEM_CORE_ON_SHFT                                            0xe
#define HWIO_GCC_VENUS0_VCODEC0_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                       0x2000
#define HWIO_GCC_VENUS0_VCODEC0_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                          0xd
#define HWIO_GCC_VENUS0_VCODEC0_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                      0x1000
#define HWIO_GCC_VENUS0_VCODEC0_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                         0xc
#define HWIO_GCC_VENUS0_VCODEC0_CBCR_WAKEUP_BMSK                                                     0xf00
#define HWIO_GCC_VENUS0_VCODEC0_CBCR_WAKEUP_SHFT                                                       0x8
#define HWIO_GCC_VENUS0_VCODEC0_CBCR_SLEEP_BMSK                                                       0xf0
#define HWIO_GCC_VENUS0_VCODEC0_CBCR_SLEEP_SHFT                                                        0x4
#define HWIO_GCC_VENUS0_VCODEC0_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_GCC_VENUS0_VCODEC0_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_GCC_VENUS0_AHB_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0004c020)
#define HWIO_GCC_VENUS0_AHB_CBCR_RMSK                                                           0xf0008001
#define HWIO_GCC_VENUS0_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_VENUS0_AHB_CBCR_ADDR, HWIO_GCC_VENUS0_AHB_CBCR_RMSK)
#define HWIO_GCC_VENUS0_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_VENUS0_AHB_CBCR_ADDR, m)
#define HWIO_GCC_VENUS0_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_VENUS0_AHB_CBCR_ADDR,v)
#define HWIO_GCC_VENUS0_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_VENUS0_AHB_CBCR_ADDR,m,v,HWIO_GCC_VENUS0_AHB_CBCR_IN)
#define HWIO_GCC_VENUS0_AHB_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_VENUS0_AHB_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_VENUS0_AHB_CBCR_NOC_HANDSHAKE_FSM_STATE_BMSK                                   0x70000000
#define HWIO_GCC_VENUS0_AHB_CBCR_NOC_HANDSHAKE_FSM_STATE_SHFT                                         0x1c
#define HWIO_GCC_VENUS0_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                          0x8000
#define HWIO_GCC_VENUS0_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                             0xf
#define HWIO_GCC_VENUS0_AHB_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_VENUS0_AHB_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_VENUS0_AXI_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0004c024)
#define HWIO_GCC_VENUS0_AXI_CBCR_RMSK                                                           0x80007ff1
#define HWIO_GCC_VENUS0_AXI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_VENUS0_AXI_CBCR_ADDR, HWIO_GCC_VENUS0_AXI_CBCR_RMSK)
#define HWIO_GCC_VENUS0_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_VENUS0_AXI_CBCR_ADDR, m)
#define HWIO_GCC_VENUS0_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_VENUS0_AXI_CBCR_ADDR,v)
#define HWIO_GCC_VENUS0_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_VENUS0_AXI_CBCR_ADDR,m,v,HWIO_GCC_VENUS0_AXI_CBCR_IN)
#define HWIO_GCC_VENUS0_AXI_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_VENUS0_AXI_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_VENUS0_AXI_CBCR_FORCE_MEM_CORE_ON_BMSK                                             0x4000
#define HWIO_GCC_VENUS0_AXI_CBCR_FORCE_MEM_CORE_ON_SHFT                                                0xe
#define HWIO_GCC_VENUS0_AXI_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                           0x2000
#define HWIO_GCC_VENUS0_AXI_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                              0xd
#define HWIO_GCC_VENUS0_AXI_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                          0x1000
#define HWIO_GCC_VENUS0_AXI_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                             0xc
#define HWIO_GCC_VENUS0_AXI_CBCR_WAKEUP_BMSK                                                         0xf00
#define HWIO_GCC_VENUS0_AXI_CBCR_WAKEUP_SHFT                                                           0x8
#define HWIO_GCC_VENUS0_AXI_CBCR_SLEEP_BMSK                                                           0xf0
#define HWIO_GCC_VENUS0_AXI_CBCR_SLEEP_SHFT                                                            0x4
#define HWIO_GCC_VENUS0_AXI_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_VENUS0_AXI_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_PCLK0_CMD_RCGR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d000)
#define HWIO_GCC_PCLK0_CMD_RCGR_RMSK                                                            0x800000f3
#define HWIO_GCC_PCLK0_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_PCLK0_CMD_RCGR_ADDR, HWIO_GCC_PCLK0_CMD_RCGR_RMSK)
#define HWIO_GCC_PCLK0_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCLK0_CMD_RCGR_ADDR, m)
#define HWIO_GCC_PCLK0_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_PCLK0_CMD_RCGR_ADDR,v)
#define HWIO_GCC_PCLK0_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCLK0_CMD_RCGR_ADDR,m,v,HWIO_GCC_PCLK0_CMD_RCGR_IN)
#define HWIO_GCC_PCLK0_CMD_RCGR_ROOT_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_PCLK0_CMD_RCGR_ROOT_OFF_SHFT                                                         0x1f
#define HWIO_GCC_PCLK0_CMD_RCGR_DIRTY_D_BMSK                                                          0x80
#define HWIO_GCC_PCLK0_CMD_RCGR_DIRTY_D_SHFT                                                           0x7
#define HWIO_GCC_PCLK0_CMD_RCGR_DIRTY_N_BMSK                                                          0x40
#define HWIO_GCC_PCLK0_CMD_RCGR_DIRTY_N_SHFT                                                           0x6
#define HWIO_GCC_PCLK0_CMD_RCGR_DIRTY_M_BMSK                                                          0x20
#define HWIO_GCC_PCLK0_CMD_RCGR_DIRTY_M_SHFT                                                           0x5
#define HWIO_GCC_PCLK0_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                   0x10
#define HWIO_GCC_PCLK0_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                    0x4
#define HWIO_GCC_PCLK0_CMD_RCGR_ROOT_EN_BMSK                                                           0x2
#define HWIO_GCC_PCLK0_CMD_RCGR_ROOT_EN_SHFT                                                           0x1
#define HWIO_GCC_PCLK0_CMD_RCGR_UPDATE_BMSK                                                            0x1
#define HWIO_GCC_PCLK0_CMD_RCGR_UPDATE_SHFT                                                            0x0

#define HWIO_GCC_PCLK0_CFG_RCGR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d004)
#define HWIO_GCC_PCLK0_CFG_RCGR_RMSK                                                                0x371f
#define HWIO_GCC_PCLK0_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_PCLK0_CFG_RCGR_ADDR, HWIO_GCC_PCLK0_CFG_RCGR_RMSK)
#define HWIO_GCC_PCLK0_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCLK0_CFG_RCGR_ADDR, m)
#define HWIO_GCC_PCLK0_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_PCLK0_CFG_RCGR_ADDR,v)
#define HWIO_GCC_PCLK0_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCLK0_CFG_RCGR_ADDR,m,v,HWIO_GCC_PCLK0_CFG_RCGR_IN)
#define HWIO_GCC_PCLK0_CFG_RCGR_MODE_BMSK                                                           0x3000
#define HWIO_GCC_PCLK0_CFG_RCGR_MODE_SHFT                                                              0xc
#define HWIO_GCC_PCLK0_CFG_RCGR_SRC_SEL_BMSK                                                         0x700
#define HWIO_GCC_PCLK0_CFG_RCGR_SRC_SEL_SHFT                                                           0x8
#define HWIO_GCC_PCLK0_CFG_RCGR_SRC_DIV_BMSK                                                          0x1f
#define HWIO_GCC_PCLK0_CFG_RCGR_SRC_DIV_SHFT                                                           0x0

#define HWIO_GCC_PCLK0_M_ADDR                                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d008)
#define HWIO_GCC_PCLK0_M_RMSK                                                                         0xff
#define HWIO_GCC_PCLK0_M_IN          \
        in_dword_masked(HWIO_GCC_PCLK0_M_ADDR, HWIO_GCC_PCLK0_M_RMSK)
#define HWIO_GCC_PCLK0_M_INM(m)      \
        in_dword_masked(HWIO_GCC_PCLK0_M_ADDR, m)
#define HWIO_GCC_PCLK0_M_OUT(v)      \
        out_dword(HWIO_GCC_PCLK0_M_ADDR,v)
#define HWIO_GCC_PCLK0_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCLK0_M_ADDR,m,v,HWIO_GCC_PCLK0_M_IN)
#define HWIO_GCC_PCLK0_M_M_BMSK                                                                       0xff
#define HWIO_GCC_PCLK0_M_M_SHFT                                                                        0x0

#define HWIO_GCC_PCLK0_N_ADDR                                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d00c)
#define HWIO_GCC_PCLK0_N_RMSK                                                                         0xff
#define HWIO_GCC_PCLK0_N_IN          \
        in_dword_masked(HWIO_GCC_PCLK0_N_ADDR, HWIO_GCC_PCLK0_N_RMSK)
#define HWIO_GCC_PCLK0_N_INM(m)      \
        in_dword_masked(HWIO_GCC_PCLK0_N_ADDR, m)
#define HWIO_GCC_PCLK0_N_OUT(v)      \
        out_dword(HWIO_GCC_PCLK0_N_ADDR,v)
#define HWIO_GCC_PCLK0_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCLK0_N_ADDR,m,v,HWIO_GCC_PCLK0_N_IN)
#define HWIO_GCC_PCLK0_N_NOT_N_MINUS_M_BMSK                                                           0xff
#define HWIO_GCC_PCLK0_N_NOT_N_MINUS_M_SHFT                                                            0x0

#define HWIO_GCC_PCLK0_D_ADDR                                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d010)
#define HWIO_GCC_PCLK0_D_RMSK                                                                         0xff
#define HWIO_GCC_PCLK0_D_IN          \
        in_dword_masked(HWIO_GCC_PCLK0_D_ADDR, HWIO_GCC_PCLK0_D_RMSK)
#define HWIO_GCC_PCLK0_D_INM(m)      \
        in_dword_masked(HWIO_GCC_PCLK0_D_ADDR, m)
#define HWIO_GCC_PCLK0_D_OUT(v)      \
        out_dword(HWIO_GCC_PCLK0_D_ADDR,v)
#define HWIO_GCC_PCLK0_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCLK0_D_ADDR,m,v,HWIO_GCC_PCLK0_D_IN)
#define HWIO_GCC_PCLK0_D_NOT_2D_BMSK                                                                  0xff
#define HWIO_GCC_PCLK0_D_NOT_2D_SHFT                                                                   0x0

#define HWIO_GCC_MDP_CMD_RCGR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d014)
#define HWIO_GCC_MDP_CMD_RCGR_RMSK                                                              0x80000013
#define HWIO_GCC_MDP_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_MDP_CMD_RCGR_ADDR, HWIO_GCC_MDP_CMD_RCGR_RMSK)
#define HWIO_GCC_MDP_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_MDP_CMD_RCGR_ADDR, m)
#define HWIO_GCC_MDP_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_MDP_CMD_RCGR_ADDR,v)
#define HWIO_GCC_MDP_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MDP_CMD_RCGR_ADDR,m,v,HWIO_GCC_MDP_CMD_RCGR_IN)
#define HWIO_GCC_MDP_CMD_RCGR_ROOT_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_MDP_CMD_RCGR_ROOT_OFF_SHFT                                                           0x1f
#define HWIO_GCC_MDP_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                     0x10
#define HWIO_GCC_MDP_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                      0x4
#define HWIO_GCC_MDP_CMD_RCGR_ROOT_EN_BMSK                                                             0x2
#define HWIO_GCC_MDP_CMD_RCGR_ROOT_EN_SHFT                                                             0x1
#define HWIO_GCC_MDP_CMD_RCGR_UPDATE_BMSK                                                              0x1
#define HWIO_GCC_MDP_CMD_RCGR_UPDATE_SHFT                                                              0x0

#define HWIO_GCC_MDP_CFG_RCGR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d018)
#define HWIO_GCC_MDP_CFG_RCGR_RMSK                                                                   0x71f
#define HWIO_GCC_MDP_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_MDP_CFG_RCGR_ADDR, HWIO_GCC_MDP_CFG_RCGR_RMSK)
#define HWIO_GCC_MDP_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_MDP_CFG_RCGR_ADDR, m)
#define HWIO_GCC_MDP_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_MDP_CFG_RCGR_ADDR,v)
#define HWIO_GCC_MDP_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MDP_CFG_RCGR_ADDR,m,v,HWIO_GCC_MDP_CFG_RCGR_IN)
#define HWIO_GCC_MDP_CFG_RCGR_SRC_SEL_BMSK                                                           0x700
#define HWIO_GCC_MDP_CFG_RCGR_SRC_SEL_SHFT                                                             0x8
#define HWIO_GCC_MDP_CFG_RCGR_SRC_DIV_BMSK                                                            0x1f
#define HWIO_GCC_MDP_CFG_RCGR_SRC_DIV_SHFT                                                             0x0

#define HWIO_GCC_VSYNC_CMD_RCGR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d02c)
#define HWIO_GCC_VSYNC_CMD_RCGR_RMSK                                                            0x80000013
#define HWIO_GCC_VSYNC_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_VSYNC_CMD_RCGR_ADDR, HWIO_GCC_VSYNC_CMD_RCGR_RMSK)
#define HWIO_GCC_VSYNC_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_VSYNC_CMD_RCGR_ADDR, m)
#define HWIO_GCC_VSYNC_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_VSYNC_CMD_RCGR_ADDR,v)
#define HWIO_GCC_VSYNC_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_VSYNC_CMD_RCGR_ADDR,m,v,HWIO_GCC_VSYNC_CMD_RCGR_IN)
#define HWIO_GCC_VSYNC_CMD_RCGR_ROOT_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_VSYNC_CMD_RCGR_ROOT_OFF_SHFT                                                         0x1f
#define HWIO_GCC_VSYNC_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                   0x10
#define HWIO_GCC_VSYNC_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                    0x4
#define HWIO_GCC_VSYNC_CMD_RCGR_ROOT_EN_BMSK                                                           0x2
#define HWIO_GCC_VSYNC_CMD_RCGR_ROOT_EN_SHFT                                                           0x1
#define HWIO_GCC_VSYNC_CMD_RCGR_UPDATE_BMSK                                                            0x1
#define HWIO_GCC_VSYNC_CMD_RCGR_UPDATE_SHFT                                                            0x0

#define HWIO_GCC_VSYNC_CFG_RCGR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d030)
#define HWIO_GCC_VSYNC_CFG_RCGR_RMSK                                                                 0x71f
#define HWIO_GCC_VSYNC_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_VSYNC_CFG_RCGR_ADDR, HWIO_GCC_VSYNC_CFG_RCGR_RMSK)
#define HWIO_GCC_VSYNC_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_VSYNC_CFG_RCGR_ADDR, m)
#define HWIO_GCC_VSYNC_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_VSYNC_CFG_RCGR_ADDR,v)
#define HWIO_GCC_VSYNC_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_VSYNC_CFG_RCGR_ADDR,m,v,HWIO_GCC_VSYNC_CFG_RCGR_IN)
#define HWIO_GCC_VSYNC_CFG_RCGR_SRC_SEL_BMSK                                                         0x700
#define HWIO_GCC_VSYNC_CFG_RCGR_SRC_SEL_SHFT                                                           0x8
#define HWIO_GCC_VSYNC_CFG_RCGR_SRC_DIV_BMSK                                                          0x1f
#define HWIO_GCC_VSYNC_CFG_RCGR_SRC_DIV_SHFT                                                           0x0

#define HWIO_GCC_BYTE0_CMD_RCGR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d044)
#define HWIO_GCC_BYTE0_CMD_RCGR_RMSK                                                            0x80000013
#define HWIO_GCC_BYTE0_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BYTE0_CMD_RCGR_ADDR, HWIO_GCC_BYTE0_CMD_RCGR_RMSK)
#define HWIO_GCC_BYTE0_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BYTE0_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BYTE0_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BYTE0_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BYTE0_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BYTE0_CMD_RCGR_ADDR,m,v,HWIO_GCC_BYTE0_CMD_RCGR_IN)
#define HWIO_GCC_BYTE0_CMD_RCGR_ROOT_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_BYTE0_CMD_RCGR_ROOT_OFF_SHFT                                                         0x1f
#define HWIO_GCC_BYTE0_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                   0x10
#define HWIO_GCC_BYTE0_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                    0x4
#define HWIO_GCC_BYTE0_CMD_RCGR_ROOT_EN_BMSK                                                           0x2
#define HWIO_GCC_BYTE0_CMD_RCGR_ROOT_EN_SHFT                                                           0x1
#define HWIO_GCC_BYTE0_CMD_RCGR_UPDATE_BMSK                                                            0x1
#define HWIO_GCC_BYTE0_CMD_RCGR_UPDATE_SHFT                                                            0x0

#define HWIO_GCC_BYTE0_CFG_RCGR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d048)
#define HWIO_GCC_BYTE0_CFG_RCGR_RMSK                                                                 0x71f
#define HWIO_GCC_BYTE0_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BYTE0_CFG_RCGR_ADDR, HWIO_GCC_BYTE0_CFG_RCGR_RMSK)
#define HWIO_GCC_BYTE0_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BYTE0_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BYTE0_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BYTE0_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BYTE0_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BYTE0_CFG_RCGR_ADDR,m,v,HWIO_GCC_BYTE0_CFG_RCGR_IN)
#define HWIO_GCC_BYTE0_CFG_RCGR_SRC_SEL_BMSK                                                         0x700
#define HWIO_GCC_BYTE0_CFG_RCGR_SRC_SEL_SHFT                                                           0x8
#define HWIO_GCC_BYTE0_CFG_RCGR_SRC_DIV_BMSK                                                          0x1f
#define HWIO_GCC_BYTE0_CFG_RCGR_SRC_DIV_SHFT                                                           0x0

#define HWIO_GCC_ESC0_CMD_RCGR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d05c)
#define HWIO_GCC_ESC0_CMD_RCGR_RMSK                                                             0x80000013
#define HWIO_GCC_ESC0_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_ESC0_CMD_RCGR_ADDR, HWIO_GCC_ESC0_CMD_RCGR_RMSK)
#define HWIO_GCC_ESC0_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_ESC0_CMD_RCGR_ADDR, m)
#define HWIO_GCC_ESC0_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_ESC0_CMD_RCGR_ADDR,v)
#define HWIO_GCC_ESC0_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ESC0_CMD_RCGR_ADDR,m,v,HWIO_GCC_ESC0_CMD_RCGR_IN)
#define HWIO_GCC_ESC0_CMD_RCGR_ROOT_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_ESC0_CMD_RCGR_ROOT_OFF_SHFT                                                          0x1f
#define HWIO_GCC_ESC0_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                    0x10
#define HWIO_GCC_ESC0_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                     0x4
#define HWIO_GCC_ESC0_CMD_RCGR_ROOT_EN_BMSK                                                            0x2
#define HWIO_GCC_ESC0_CMD_RCGR_ROOT_EN_SHFT                                                            0x1
#define HWIO_GCC_ESC0_CMD_RCGR_UPDATE_BMSK                                                             0x1
#define HWIO_GCC_ESC0_CMD_RCGR_UPDATE_SHFT                                                             0x0

#define HWIO_GCC_ESC0_CFG_RCGR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d060)
#define HWIO_GCC_ESC0_CFG_RCGR_RMSK                                                                  0x71f
#define HWIO_GCC_ESC0_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_ESC0_CFG_RCGR_ADDR, HWIO_GCC_ESC0_CFG_RCGR_RMSK)
#define HWIO_GCC_ESC0_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_ESC0_CFG_RCGR_ADDR, m)
#define HWIO_GCC_ESC0_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_ESC0_CFG_RCGR_ADDR,v)
#define HWIO_GCC_ESC0_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ESC0_CFG_RCGR_ADDR,m,v,HWIO_GCC_ESC0_CFG_RCGR_IN)
#define HWIO_GCC_ESC0_CFG_RCGR_SRC_SEL_BMSK                                                          0x700
#define HWIO_GCC_ESC0_CFG_RCGR_SRC_SEL_SHFT                                                            0x8
#define HWIO_GCC_ESC0_CFG_RCGR_SRC_DIV_BMSK                                                           0x1f
#define HWIO_GCC_ESC0_CFG_RCGR_SRC_DIV_SHFT                                                            0x0

#define HWIO_GCC_MDSS_BCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d074)
#define HWIO_GCC_MDSS_BCR_RMSK                                                                         0x1
#define HWIO_GCC_MDSS_BCR_IN          \
        in_dword_masked(HWIO_GCC_MDSS_BCR_ADDR, HWIO_GCC_MDSS_BCR_RMSK)
#define HWIO_GCC_MDSS_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MDSS_BCR_ADDR, m)
#define HWIO_GCC_MDSS_BCR_OUT(v)      \
        out_dword(HWIO_GCC_MDSS_BCR_ADDR,v)
#define HWIO_GCC_MDSS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MDSS_BCR_ADDR,m,v,HWIO_GCC_MDSS_BCR_IN)
#define HWIO_GCC_MDSS_BCR_BLK_ARES_BMSK                                                                0x1
#define HWIO_GCC_MDSS_BCR_BLK_ARES_SHFT                                                                0x0

#define HWIO_GCC_MDSS_GDSCR_ADDR                                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d078)
#define HWIO_GCC_MDSS_GDSCR_RMSK                                                                0xf8fff7ff
#define HWIO_GCC_MDSS_GDSCR_IN          \
        in_dword_masked(HWIO_GCC_MDSS_GDSCR_ADDR, HWIO_GCC_MDSS_GDSCR_RMSK)
#define HWIO_GCC_MDSS_GDSCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MDSS_GDSCR_ADDR, m)
#define HWIO_GCC_MDSS_GDSCR_OUT(v)      \
        out_dword(HWIO_GCC_MDSS_GDSCR_ADDR,v)
#define HWIO_GCC_MDSS_GDSCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MDSS_GDSCR_ADDR,m,v,HWIO_GCC_MDSS_GDSCR_IN)
#define HWIO_GCC_MDSS_GDSCR_PWR_ON_BMSK                                                         0x80000000
#define HWIO_GCC_MDSS_GDSCR_PWR_ON_SHFT                                                               0x1f
#define HWIO_GCC_MDSS_GDSCR_GDSC_STATE_BMSK                                                     0x78000000
#define HWIO_GCC_MDSS_GDSCR_GDSC_STATE_SHFT                                                           0x1b
#define HWIO_GCC_MDSS_GDSCR_EN_REST_WAIT_BMSK                                                     0xf00000
#define HWIO_GCC_MDSS_GDSCR_EN_REST_WAIT_SHFT                                                         0x14
#define HWIO_GCC_MDSS_GDSCR_EN_FEW_WAIT_BMSK                                                       0xf0000
#define HWIO_GCC_MDSS_GDSCR_EN_FEW_WAIT_SHFT                                                          0x10
#define HWIO_GCC_MDSS_GDSCR_CLK_DIS_WAIT_BMSK                                                       0xf000
#define HWIO_GCC_MDSS_GDSCR_CLK_DIS_WAIT_SHFT                                                          0xc
#define HWIO_GCC_MDSS_GDSCR_RESTORE_BMSK                                                             0x400
#define HWIO_GCC_MDSS_GDSCR_RESTORE_SHFT                                                               0xa
#define HWIO_GCC_MDSS_GDSCR_SAVE_BMSK                                                                0x200
#define HWIO_GCC_MDSS_GDSCR_SAVE_SHFT                                                                  0x9
#define HWIO_GCC_MDSS_GDSCR_RETAIN_BMSK                                                              0x100
#define HWIO_GCC_MDSS_GDSCR_RETAIN_SHFT                                                                0x8
#define HWIO_GCC_MDSS_GDSCR_EN_REST_BMSK                                                              0x80
#define HWIO_GCC_MDSS_GDSCR_EN_REST_SHFT                                                               0x7
#define HWIO_GCC_MDSS_GDSCR_EN_FEW_BMSK                                                               0x40
#define HWIO_GCC_MDSS_GDSCR_EN_FEW_SHFT                                                                0x6
#define HWIO_GCC_MDSS_GDSCR_CLAMP_IO_BMSK                                                             0x20
#define HWIO_GCC_MDSS_GDSCR_CLAMP_IO_SHFT                                                              0x5
#define HWIO_GCC_MDSS_GDSCR_CLK_DISABLE_BMSK                                                          0x10
#define HWIO_GCC_MDSS_GDSCR_CLK_DISABLE_SHFT                                                           0x4
#define HWIO_GCC_MDSS_GDSCR_PD_ARES_BMSK                                                               0x8
#define HWIO_GCC_MDSS_GDSCR_PD_ARES_SHFT                                                               0x3
#define HWIO_GCC_MDSS_GDSCR_SW_OVERRIDE_BMSK                                                           0x4
#define HWIO_GCC_MDSS_GDSCR_SW_OVERRIDE_SHFT                                                           0x2
#define HWIO_GCC_MDSS_GDSCR_HW_CONTROL_BMSK                                                            0x2
#define HWIO_GCC_MDSS_GDSCR_HW_CONTROL_SHFT                                                            0x1
#define HWIO_GCC_MDSS_GDSCR_SW_COLLAPSE_BMSK                                                           0x1
#define HWIO_GCC_MDSS_GDSCR_SW_COLLAPSE_SHFT                                                           0x0

#define HWIO_GCC_MDSS_AHB_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d07c)
#define HWIO_GCC_MDSS_AHB_CBCR_RMSK                                                             0xf0008001
#define HWIO_GCC_MDSS_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MDSS_AHB_CBCR_ADDR, HWIO_GCC_MDSS_AHB_CBCR_RMSK)
#define HWIO_GCC_MDSS_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MDSS_AHB_CBCR_ADDR, m)
#define HWIO_GCC_MDSS_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MDSS_AHB_CBCR_ADDR,v)
#define HWIO_GCC_MDSS_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MDSS_AHB_CBCR_ADDR,m,v,HWIO_GCC_MDSS_AHB_CBCR_IN)
#define HWIO_GCC_MDSS_AHB_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_MDSS_AHB_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_MDSS_AHB_CBCR_NOC_HANDSHAKE_FSM_STATE_BMSK                                     0x70000000
#define HWIO_GCC_MDSS_AHB_CBCR_NOC_HANDSHAKE_FSM_STATE_SHFT                                           0x1c
#define HWIO_GCC_MDSS_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                            0x8000
#define HWIO_GCC_MDSS_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                               0xf
#define HWIO_GCC_MDSS_AHB_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_GCC_MDSS_AHB_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_GCC_MDSS_AXI_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d080)
#define HWIO_GCC_MDSS_AXI_CBCR_RMSK                                                             0x80007ff1
#define HWIO_GCC_MDSS_AXI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MDSS_AXI_CBCR_ADDR, HWIO_GCC_MDSS_AXI_CBCR_RMSK)
#define HWIO_GCC_MDSS_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MDSS_AXI_CBCR_ADDR, m)
#define HWIO_GCC_MDSS_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MDSS_AXI_CBCR_ADDR,v)
#define HWIO_GCC_MDSS_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MDSS_AXI_CBCR_ADDR,m,v,HWIO_GCC_MDSS_AXI_CBCR_IN)
#define HWIO_GCC_MDSS_AXI_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_MDSS_AXI_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_MDSS_AXI_CBCR_FORCE_MEM_CORE_ON_BMSK                                               0x4000
#define HWIO_GCC_MDSS_AXI_CBCR_FORCE_MEM_CORE_ON_SHFT                                                  0xe
#define HWIO_GCC_MDSS_AXI_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                             0x2000
#define HWIO_GCC_MDSS_AXI_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                0xd
#define HWIO_GCC_MDSS_AXI_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                            0x1000
#define HWIO_GCC_MDSS_AXI_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                               0xc
#define HWIO_GCC_MDSS_AXI_CBCR_WAKEUP_BMSK                                                           0xf00
#define HWIO_GCC_MDSS_AXI_CBCR_WAKEUP_SHFT                                                             0x8
#define HWIO_GCC_MDSS_AXI_CBCR_SLEEP_BMSK                                                             0xf0
#define HWIO_GCC_MDSS_AXI_CBCR_SLEEP_SHFT                                                              0x4
#define HWIO_GCC_MDSS_AXI_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_GCC_MDSS_AXI_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_GCC_MDSS_PCLK0_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d084)
#define HWIO_GCC_MDSS_PCLK0_CBCR_RMSK                                                           0x80000001
#define HWIO_GCC_MDSS_PCLK0_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MDSS_PCLK0_CBCR_ADDR, HWIO_GCC_MDSS_PCLK0_CBCR_RMSK)
#define HWIO_GCC_MDSS_PCLK0_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MDSS_PCLK0_CBCR_ADDR, m)
#define HWIO_GCC_MDSS_PCLK0_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MDSS_PCLK0_CBCR_ADDR,v)
#define HWIO_GCC_MDSS_PCLK0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MDSS_PCLK0_CBCR_ADDR,m,v,HWIO_GCC_MDSS_PCLK0_CBCR_IN)
#define HWIO_GCC_MDSS_PCLK0_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_MDSS_PCLK0_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_MDSS_PCLK0_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_MDSS_PCLK0_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_MDSS_MDP_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d088)
#define HWIO_GCC_MDSS_MDP_CBCR_RMSK                                                             0x80007ff1
#define HWIO_GCC_MDSS_MDP_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MDSS_MDP_CBCR_ADDR, HWIO_GCC_MDSS_MDP_CBCR_RMSK)
#define HWIO_GCC_MDSS_MDP_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MDSS_MDP_CBCR_ADDR, m)
#define HWIO_GCC_MDSS_MDP_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MDSS_MDP_CBCR_ADDR,v)
#define HWIO_GCC_MDSS_MDP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MDSS_MDP_CBCR_ADDR,m,v,HWIO_GCC_MDSS_MDP_CBCR_IN)
#define HWIO_GCC_MDSS_MDP_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_MDSS_MDP_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_MDSS_MDP_CBCR_FORCE_MEM_CORE_ON_BMSK                                               0x4000
#define HWIO_GCC_MDSS_MDP_CBCR_FORCE_MEM_CORE_ON_SHFT                                                  0xe
#define HWIO_GCC_MDSS_MDP_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                             0x2000
#define HWIO_GCC_MDSS_MDP_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                0xd
#define HWIO_GCC_MDSS_MDP_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                            0x1000
#define HWIO_GCC_MDSS_MDP_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                               0xc
#define HWIO_GCC_MDSS_MDP_CBCR_WAKEUP_BMSK                                                           0xf00
#define HWIO_GCC_MDSS_MDP_CBCR_WAKEUP_SHFT                                                             0x8
#define HWIO_GCC_MDSS_MDP_CBCR_SLEEP_BMSK                                                             0xf0
#define HWIO_GCC_MDSS_MDP_CBCR_SLEEP_SHFT                                                              0x4
#define HWIO_GCC_MDSS_MDP_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_GCC_MDSS_MDP_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_GCC_MDSS_VSYNC_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d090)
#define HWIO_GCC_MDSS_VSYNC_CBCR_RMSK                                                           0x80000001
#define HWIO_GCC_MDSS_VSYNC_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MDSS_VSYNC_CBCR_ADDR, HWIO_GCC_MDSS_VSYNC_CBCR_RMSK)
#define HWIO_GCC_MDSS_VSYNC_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MDSS_VSYNC_CBCR_ADDR, m)
#define HWIO_GCC_MDSS_VSYNC_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MDSS_VSYNC_CBCR_ADDR,v)
#define HWIO_GCC_MDSS_VSYNC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MDSS_VSYNC_CBCR_ADDR,m,v,HWIO_GCC_MDSS_VSYNC_CBCR_IN)
#define HWIO_GCC_MDSS_VSYNC_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_MDSS_VSYNC_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_MDSS_VSYNC_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_MDSS_VSYNC_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_MDSS_BYTE0_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d094)
#define HWIO_GCC_MDSS_BYTE0_CBCR_RMSK                                                           0x80000001
#define HWIO_GCC_MDSS_BYTE0_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MDSS_BYTE0_CBCR_ADDR, HWIO_GCC_MDSS_BYTE0_CBCR_RMSK)
#define HWIO_GCC_MDSS_BYTE0_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MDSS_BYTE0_CBCR_ADDR, m)
#define HWIO_GCC_MDSS_BYTE0_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MDSS_BYTE0_CBCR_ADDR,v)
#define HWIO_GCC_MDSS_BYTE0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MDSS_BYTE0_CBCR_ADDR,m,v,HWIO_GCC_MDSS_BYTE0_CBCR_IN)
#define HWIO_GCC_MDSS_BYTE0_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_MDSS_BYTE0_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_MDSS_BYTE0_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_MDSS_BYTE0_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_MDSS_ESC0_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0004d098)
#define HWIO_GCC_MDSS_ESC0_CBCR_RMSK                                                            0x80000001
#define HWIO_GCC_MDSS_ESC0_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MDSS_ESC0_CBCR_ADDR, HWIO_GCC_MDSS_ESC0_CBCR_RMSK)
#define HWIO_GCC_MDSS_ESC0_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MDSS_ESC0_CBCR_ADDR, m)
#define HWIO_GCC_MDSS_ESC0_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MDSS_ESC0_CBCR_ADDR,v)
#define HWIO_GCC_MDSS_ESC0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MDSS_ESC0_CBCR_ADDR,m,v,HWIO_GCC_MDSS_ESC0_CBCR_IN)
#define HWIO_GCC_MDSS_ESC0_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_MDSS_ESC0_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_MDSS_ESC0_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_MDSS_ESC0_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x0004e000)
#define HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_RMSK                                                     0x80000013
#define HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_ADDR, HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_RMSK)
#define HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_ADDR, m)
#define HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_ADDR,v)
#define HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_ADDR,m,v,HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_IN)
#define HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_ROOT_OFF_BMSK                                            0x80000000
#define HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_ROOT_OFF_SHFT                                                  0x1f
#define HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                            0x10
#define HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                             0x4
#define HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_ROOT_EN_BMSK                                                    0x2
#define HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_ROOT_EN_SHFT                                                    0x1
#define HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_UPDATE_BMSK                                                     0x1
#define HWIO_GCC_CSI0PHYTIMER_CMD_RCGR_UPDATE_SHFT                                                     0x0

#define HWIO_GCC_CSI0PHYTIMER_CFG_RCGR_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x0004e004)
#define HWIO_GCC_CSI0PHYTIMER_CFG_RCGR_RMSK                                                          0x71f
#define HWIO_GCC_CSI0PHYTIMER_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_CSI0PHYTIMER_CFG_RCGR_ADDR, HWIO_GCC_CSI0PHYTIMER_CFG_RCGR_RMSK)
#define HWIO_GCC_CSI0PHYTIMER_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_CSI0PHYTIMER_CFG_RCGR_ADDR, m)
#define HWIO_GCC_CSI0PHYTIMER_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_CSI0PHYTIMER_CFG_RCGR_ADDR,v)
#define HWIO_GCC_CSI0PHYTIMER_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CSI0PHYTIMER_CFG_RCGR_ADDR,m,v,HWIO_GCC_CSI0PHYTIMER_CFG_RCGR_IN)
#define HWIO_GCC_CSI0PHYTIMER_CFG_RCGR_SRC_SEL_BMSK                                                  0x700
#define HWIO_GCC_CSI0PHYTIMER_CFG_RCGR_SRC_SEL_SHFT                                                    0x8
#define HWIO_GCC_CSI0PHYTIMER_CFG_RCGR_SRC_DIV_BMSK                                                   0x1f
#define HWIO_GCC_CSI0PHYTIMER_CFG_RCGR_SRC_DIV_SHFT                                                    0x0

#define HWIO_GCC_CAMSS_PHY0_BCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0004e018)
#define HWIO_GCC_CAMSS_PHY0_BCR_RMSK                                                                   0x1
#define HWIO_GCC_CAMSS_PHY0_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_PHY0_BCR_ADDR, HWIO_GCC_CAMSS_PHY0_BCR_RMSK)
#define HWIO_GCC_CAMSS_PHY0_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_PHY0_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_PHY0_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_PHY0_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_PHY0_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_PHY0_BCR_ADDR,m,v,HWIO_GCC_CAMSS_PHY0_BCR_IN)
#define HWIO_GCC_CAMSS_PHY0_BCR_BLK_ARES_BMSK                                                          0x1
#define HWIO_GCC_CAMSS_PHY0_BCR_BLK_ARES_SHFT                                                          0x0

#define HWIO_GCC_CAMSS_PHY0_CSI0PHYTIMER_CBCR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0004e01c)
#define HWIO_GCC_CAMSS_PHY0_CSI0PHYTIMER_CBCR_RMSK                                              0x80000001
#define HWIO_GCC_CAMSS_PHY0_CSI0PHYTIMER_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_PHY0_CSI0PHYTIMER_CBCR_ADDR, HWIO_GCC_CAMSS_PHY0_CSI0PHYTIMER_CBCR_RMSK)
#define HWIO_GCC_CAMSS_PHY0_CSI0PHYTIMER_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_PHY0_CSI0PHYTIMER_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_PHY0_CSI0PHYTIMER_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_PHY0_CSI0PHYTIMER_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_PHY0_CSI0PHYTIMER_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_PHY0_CSI0PHYTIMER_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_PHY0_CSI0PHYTIMER_CBCR_IN)
#define HWIO_GCC_CAMSS_PHY0_CSI0PHYTIMER_CBCR_CLK_OFF_BMSK                                      0x80000000
#define HWIO_GCC_CAMSS_PHY0_CSI0PHYTIMER_CBCR_CLK_OFF_SHFT                                            0x1f
#define HWIO_GCC_CAMSS_PHY0_CSI0PHYTIMER_CBCR_CLK_ENABLE_BMSK                                          0x1
#define HWIO_GCC_CAMSS_PHY0_CSI0PHYTIMER_CBCR_CLK_ENABLE_SHFT                                          0x0

#define HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x0004f000)
#define HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_RMSK                                                     0x80000013
#define HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_ADDR, HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_RMSK)
#define HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_ADDR, m)
#define HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_ADDR,v)
#define HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_ADDR,m,v,HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_IN)
#define HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_ROOT_OFF_BMSK                                            0x80000000
#define HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_ROOT_OFF_SHFT                                                  0x1f
#define HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                            0x10
#define HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                             0x4
#define HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_ROOT_EN_BMSK                                                    0x2
#define HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_ROOT_EN_SHFT                                                    0x1
#define HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_UPDATE_BMSK                                                     0x1
#define HWIO_GCC_CSI1PHYTIMER_CMD_RCGR_UPDATE_SHFT                                                     0x0

#define HWIO_GCC_CSI1PHYTIMER_CFG_RCGR_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x0004f004)
#define HWIO_GCC_CSI1PHYTIMER_CFG_RCGR_RMSK                                                          0x71f
#define HWIO_GCC_CSI1PHYTIMER_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_CSI1PHYTIMER_CFG_RCGR_ADDR, HWIO_GCC_CSI1PHYTIMER_CFG_RCGR_RMSK)
#define HWIO_GCC_CSI1PHYTIMER_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_CSI1PHYTIMER_CFG_RCGR_ADDR, m)
#define HWIO_GCC_CSI1PHYTIMER_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_CSI1PHYTIMER_CFG_RCGR_ADDR,v)
#define HWIO_GCC_CSI1PHYTIMER_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CSI1PHYTIMER_CFG_RCGR_ADDR,m,v,HWIO_GCC_CSI1PHYTIMER_CFG_RCGR_IN)
#define HWIO_GCC_CSI1PHYTIMER_CFG_RCGR_SRC_SEL_BMSK                                                  0x700
#define HWIO_GCC_CSI1PHYTIMER_CFG_RCGR_SRC_SEL_SHFT                                                    0x8
#define HWIO_GCC_CSI1PHYTIMER_CFG_RCGR_SRC_DIV_BMSK                                                   0x1f
#define HWIO_GCC_CSI1PHYTIMER_CFG_RCGR_SRC_DIV_SHFT                                                    0x0

#define HWIO_GCC_CAMSS_PHY1_BCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0004f018)
#define HWIO_GCC_CAMSS_PHY1_BCR_RMSK                                                                   0x1
#define HWIO_GCC_CAMSS_PHY1_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_PHY1_BCR_ADDR, HWIO_GCC_CAMSS_PHY1_BCR_RMSK)
#define HWIO_GCC_CAMSS_PHY1_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_PHY1_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_PHY1_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_PHY1_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_PHY1_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_PHY1_BCR_ADDR,m,v,HWIO_GCC_CAMSS_PHY1_BCR_IN)
#define HWIO_GCC_CAMSS_PHY1_BCR_BLK_ARES_BMSK                                                          0x1
#define HWIO_GCC_CAMSS_PHY1_BCR_BLK_ARES_SHFT                                                          0x0

#define HWIO_GCC_CAMSS_PHY1_CSI1PHYTIMER_CBCR_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0004f01c)
#define HWIO_GCC_CAMSS_PHY1_CSI1PHYTIMER_CBCR_RMSK                                              0x80000001
#define HWIO_GCC_CAMSS_PHY1_CSI1PHYTIMER_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_PHY1_CSI1PHYTIMER_CBCR_ADDR, HWIO_GCC_CAMSS_PHY1_CSI1PHYTIMER_CBCR_RMSK)
#define HWIO_GCC_CAMSS_PHY1_CSI1PHYTIMER_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_PHY1_CSI1PHYTIMER_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_PHY1_CSI1PHYTIMER_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_PHY1_CSI1PHYTIMER_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_PHY1_CSI1PHYTIMER_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_PHY1_CSI1PHYTIMER_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_PHY1_CSI1PHYTIMER_CBCR_IN)
#define HWIO_GCC_CAMSS_PHY1_CSI1PHYTIMER_CBCR_CLK_OFF_BMSK                                      0x80000000
#define HWIO_GCC_CAMSS_PHY1_CSI1PHYTIMER_CBCR_CLK_OFF_SHFT                                            0x1f
#define HWIO_GCC_CAMSS_PHY1_CSI1PHYTIMER_CBCR_CLK_ENABLE_BMSK                                          0x1
#define HWIO_GCC_CAMSS_PHY1_CSI1PHYTIMER_CBCR_CLK_ENABLE_SHFT                                          0x0

#define HWIO_GCC_CSI0_CMD_RCGR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0004e020)
#define HWIO_GCC_CSI0_CMD_RCGR_RMSK                                                             0x80000013
#define HWIO_GCC_CSI0_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_CSI0_CMD_RCGR_ADDR, HWIO_GCC_CSI0_CMD_RCGR_RMSK)
#define HWIO_GCC_CSI0_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_CSI0_CMD_RCGR_ADDR, m)
#define HWIO_GCC_CSI0_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_CSI0_CMD_RCGR_ADDR,v)
#define HWIO_GCC_CSI0_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CSI0_CMD_RCGR_ADDR,m,v,HWIO_GCC_CSI0_CMD_RCGR_IN)
#define HWIO_GCC_CSI0_CMD_RCGR_ROOT_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_CSI0_CMD_RCGR_ROOT_OFF_SHFT                                                          0x1f
#define HWIO_GCC_CSI0_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                    0x10
#define HWIO_GCC_CSI0_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                     0x4
#define HWIO_GCC_CSI0_CMD_RCGR_ROOT_EN_BMSK                                                            0x2
#define HWIO_GCC_CSI0_CMD_RCGR_ROOT_EN_SHFT                                                            0x1
#define HWIO_GCC_CSI0_CMD_RCGR_UPDATE_BMSK                                                             0x1
#define HWIO_GCC_CSI0_CMD_RCGR_UPDATE_SHFT                                                             0x0

#define HWIO_GCC_CSI0_CFG_RCGR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0004e024)
#define HWIO_GCC_CSI0_CFG_RCGR_RMSK                                                                  0x71f
#define HWIO_GCC_CSI0_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_CSI0_CFG_RCGR_ADDR, HWIO_GCC_CSI0_CFG_RCGR_RMSK)
#define HWIO_GCC_CSI0_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_CSI0_CFG_RCGR_ADDR, m)
#define HWIO_GCC_CSI0_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_CSI0_CFG_RCGR_ADDR,v)
#define HWIO_GCC_CSI0_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CSI0_CFG_RCGR_ADDR,m,v,HWIO_GCC_CSI0_CFG_RCGR_IN)
#define HWIO_GCC_CSI0_CFG_RCGR_SRC_SEL_BMSK                                                          0x700
#define HWIO_GCC_CSI0_CFG_RCGR_SRC_SEL_SHFT                                                            0x8
#define HWIO_GCC_CSI0_CFG_RCGR_SRC_DIV_BMSK                                                           0x1f
#define HWIO_GCC_CSI0_CFG_RCGR_SRC_DIV_SHFT                                                            0x0

#define HWIO_GCC_CAMSS_CSI0_BCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0004e038)
#define HWIO_GCC_CAMSS_CSI0_BCR_RMSK                                                                   0x1
#define HWIO_GCC_CAMSS_CSI0_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI0_BCR_ADDR, HWIO_GCC_CAMSS_CSI0_BCR_RMSK)
#define HWIO_GCC_CAMSS_CSI0_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI0_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI0_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI0_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI0_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI0_BCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI0_BCR_IN)
#define HWIO_GCC_CAMSS_CSI0_BCR_BLK_ARES_BMSK                                                          0x1
#define HWIO_GCC_CAMSS_CSI0_BCR_BLK_ARES_SHFT                                                          0x0

#define HWIO_GCC_CAMSS_CSI0_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0004e03c)
#define HWIO_GCC_CAMSS_CSI0_CBCR_RMSK                                                           0x80000001
#define HWIO_GCC_CAMSS_CSI0_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI0_CBCR_ADDR, HWIO_GCC_CAMSS_CSI0_CBCR_RMSK)
#define HWIO_GCC_CAMSS_CSI0_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI0_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI0_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI0_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI0_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI0_CBCR_IN)
#define HWIO_GCC_CAMSS_CSI0_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_CAMSS_CSI0_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_CAMSS_CSI0_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_CAMSS_CSI0_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_CAMSS_CSI0_AHB_CBCR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x0004e040)
#define HWIO_GCC_CAMSS_CSI0_AHB_CBCR_RMSK                                                       0x80000001
#define HWIO_GCC_CAMSS_CSI0_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI0_AHB_CBCR_ADDR, HWIO_GCC_CAMSS_CSI0_AHB_CBCR_RMSK)
#define HWIO_GCC_CAMSS_CSI0_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI0_AHB_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI0_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI0_AHB_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI0_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI0_AHB_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI0_AHB_CBCR_IN)
#define HWIO_GCC_CAMSS_CSI0_AHB_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_GCC_CAMSS_CSI0_AHB_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_GCC_CAMSS_CSI0_AHB_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_GCC_CAMSS_CSI0_AHB_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_GCC_CAMSS_CSI0PHY_BCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0004e044)
#define HWIO_GCC_CAMSS_CSI0PHY_BCR_RMSK                                                                0x1
#define HWIO_GCC_CAMSS_CSI0PHY_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI0PHY_BCR_ADDR, HWIO_GCC_CAMSS_CSI0PHY_BCR_RMSK)
#define HWIO_GCC_CAMSS_CSI0PHY_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI0PHY_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI0PHY_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI0PHY_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI0PHY_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI0PHY_BCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI0PHY_BCR_IN)
#define HWIO_GCC_CAMSS_CSI0PHY_BCR_BLK_ARES_BMSK                                                       0x1
#define HWIO_GCC_CAMSS_CSI0PHY_BCR_BLK_ARES_SHFT                                                       0x0

#define HWIO_GCC_CAMSS_CSI0PHY_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0004e048)
#define HWIO_GCC_CAMSS_CSI0PHY_CBCR_RMSK                                                        0x80000001
#define HWIO_GCC_CAMSS_CSI0PHY_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI0PHY_CBCR_ADDR, HWIO_GCC_CAMSS_CSI0PHY_CBCR_RMSK)
#define HWIO_GCC_CAMSS_CSI0PHY_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI0PHY_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI0PHY_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI0PHY_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI0PHY_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI0PHY_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI0PHY_CBCR_IN)
#define HWIO_GCC_CAMSS_CSI0PHY_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_CAMSS_CSI0PHY_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_CAMSS_CSI0PHY_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_CAMSS_CSI0PHY_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_CAMSS_CSI0RDI_BCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0004e04c)
#define HWIO_GCC_CAMSS_CSI0RDI_BCR_RMSK                                                                0x1
#define HWIO_GCC_CAMSS_CSI0RDI_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI0RDI_BCR_ADDR, HWIO_GCC_CAMSS_CSI0RDI_BCR_RMSK)
#define HWIO_GCC_CAMSS_CSI0RDI_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI0RDI_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI0RDI_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI0RDI_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI0RDI_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI0RDI_BCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI0RDI_BCR_IN)
#define HWIO_GCC_CAMSS_CSI0RDI_BCR_BLK_ARES_BMSK                                                       0x1
#define HWIO_GCC_CAMSS_CSI0RDI_BCR_BLK_ARES_SHFT                                                       0x0

#define HWIO_GCC_CAMSS_CSI0RDI_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0004e050)
#define HWIO_GCC_CAMSS_CSI0RDI_CBCR_RMSK                                                        0x80000001
#define HWIO_GCC_CAMSS_CSI0RDI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI0RDI_CBCR_ADDR, HWIO_GCC_CAMSS_CSI0RDI_CBCR_RMSK)
#define HWIO_GCC_CAMSS_CSI0RDI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI0RDI_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI0RDI_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI0RDI_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI0RDI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI0RDI_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI0RDI_CBCR_IN)
#define HWIO_GCC_CAMSS_CSI0RDI_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_CAMSS_CSI0RDI_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_CAMSS_CSI0RDI_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_CAMSS_CSI0RDI_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_CAMSS_CSI0PIX_BCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0004e054)
#define HWIO_GCC_CAMSS_CSI0PIX_BCR_RMSK                                                                0x1
#define HWIO_GCC_CAMSS_CSI0PIX_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI0PIX_BCR_ADDR, HWIO_GCC_CAMSS_CSI0PIX_BCR_RMSK)
#define HWIO_GCC_CAMSS_CSI0PIX_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI0PIX_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI0PIX_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI0PIX_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI0PIX_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI0PIX_BCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI0PIX_BCR_IN)
#define HWIO_GCC_CAMSS_CSI0PIX_BCR_BLK_ARES_BMSK                                                       0x1
#define HWIO_GCC_CAMSS_CSI0PIX_BCR_BLK_ARES_SHFT                                                       0x0

#define HWIO_GCC_CAMSS_CSI0PIX_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0004e058)
#define HWIO_GCC_CAMSS_CSI0PIX_CBCR_RMSK                                                        0x80000001
#define HWIO_GCC_CAMSS_CSI0PIX_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI0PIX_CBCR_ADDR, HWIO_GCC_CAMSS_CSI0PIX_CBCR_RMSK)
#define HWIO_GCC_CAMSS_CSI0PIX_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI0PIX_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI0PIX_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI0PIX_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI0PIX_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI0PIX_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI0PIX_CBCR_IN)
#define HWIO_GCC_CAMSS_CSI0PIX_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_CAMSS_CSI0PIX_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_CAMSS_CSI0PIX_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_CAMSS_CSI0PIX_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_CSI1_CMD_RCGR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0004f020)
#define HWIO_GCC_CSI1_CMD_RCGR_RMSK                                                             0x80000013
#define HWIO_GCC_CSI1_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_CSI1_CMD_RCGR_ADDR, HWIO_GCC_CSI1_CMD_RCGR_RMSK)
#define HWIO_GCC_CSI1_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_CSI1_CMD_RCGR_ADDR, m)
#define HWIO_GCC_CSI1_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_CSI1_CMD_RCGR_ADDR,v)
#define HWIO_GCC_CSI1_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CSI1_CMD_RCGR_ADDR,m,v,HWIO_GCC_CSI1_CMD_RCGR_IN)
#define HWIO_GCC_CSI1_CMD_RCGR_ROOT_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_CSI1_CMD_RCGR_ROOT_OFF_SHFT                                                          0x1f
#define HWIO_GCC_CSI1_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                    0x10
#define HWIO_GCC_CSI1_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                     0x4
#define HWIO_GCC_CSI1_CMD_RCGR_ROOT_EN_BMSK                                                            0x2
#define HWIO_GCC_CSI1_CMD_RCGR_ROOT_EN_SHFT                                                            0x1
#define HWIO_GCC_CSI1_CMD_RCGR_UPDATE_BMSK                                                             0x1
#define HWIO_GCC_CSI1_CMD_RCGR_UPDATE_SHFT                                                             0x0

#define HWIO_GCC_CSI1_CFG_RCGR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0004f024)
#define HWIO_GCC_CSI1_CFG_RCGR_RMSK                                                                  0x71f
#define HWIO_GCC_CSI1_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_CSI1_CFG_RCGR_ADDR, HWIO_GCC_CSI1_CFG_RCGR_RMSK)
#define HWIO_GCC_CSI1_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_CSI1_CFG_RCGR_ADDR, m)
#define HWIO_GCC_CSI1_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_CSI1_CFG_RCGR_ADDR,v)
#define HWIO_GCC_CSI1_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CSI1_CFG_RCGR_ADDR,m,v,HWIO_GCC_CSI1_CFG_RCGR_IN)
#define HWIO_GCC_CSI1_CFG_RCGR_SRC_SEL_BMSK                                                          0x700
#define HWIO_GCC_CSI1_CFG_RCGR_SRC_SEL_SHFT                                                            0x8
#define HWIO_GCC_CSI1_CFG_RCGR_SRC_DIV_BMSK                                                           0x1f
#define HWIO_GCC_CSI1_CFG_RCGR_SRC_DIV_SHFT                                                            0x0

#define HWIO_GCC_CAMSS_CSI1_BCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0004f038)
#define HWIO_GCC_CAMSS_CSI1_BCR_RMSK                                                                   0x1
#define HWIO_GCC_CAMSS_CSI1_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI1_BCR_ADDR, HWIO_GCC_CAMSS_CSI1_BCR_RMSK)
#define HWIO_GCC_CAMSS_CSI1_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI1_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI1_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI1_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI1_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI1_BCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI1_BCR_IN)
#define HWIO_GCC_CAMSS_CSI1_BCR_BLK_ARES_BMSK                                                          0x1
#define HWIO_GCC_CAMSS_CSI1_BCR_BLK_ARES_SHFT                                                          0x0

#define HWIO_GCC_CAMSS_CSI1_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0004f03c)
#define HWIO_GCC_CAMSS_CSI1_CBCR_RMSK                                                           0x80000001
#define HWIO_GCC_CAMSS_CSI1_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI1_CBCR_ADDR, HWIO_GCC_CAMSS_CSI1_CBCR_RMSK)
#define HWIO_GCC_CAMSS_CSI1_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI1_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI1_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI1_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI1_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI1_CBCR_IN)
#define HWIO_GCC_CAMSS_CSI1_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_CAMSS_CSI1_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_CAMSS_CSI1_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_CAMSS_CSI1_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_CAMSS_CSI1_AHB_CBCR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x0004f040)
#define HWIO_GCC_CAMSS_CSI1_AHB_CBCR_RMSK                                                       0x80000001
#define HWIO_GCC_CAMSS_CSI1_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI1_AHB_CBCR_ADDR, HWIO_GCC_CAMSS_CSI1_AHB_CBCR_RMSK)
#define HWIO_GCC_CAMSS_CSI1_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI1_AHB_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI1_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI1_AHB_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI1_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI1_AHB_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI1_AHB_CBCR_IN)
#define HWIO_GCC_CAMSS_CSI1_AHB_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_GCC_CAMSS_CSI1_AHB_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_GCC_CAMSS_CSI1_AHB_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_GCC_CAMSS_CSI1_AHB_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_GCC_CAMSS_CSI1PHY_BCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0004f044)
#define HWIO_GCC_CAMSS_CSI1PHY_BCR_RMSK                                                                0x1
#define HWIO_GCC_CAMSS_CSI1PHY_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI1PHY_BCR_ADDR, HWIO_GCC_CAMSS_CSI1PHY_BCR_RMSK)
#define HWIO_GCC_CAMSS_CSI1PHY_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI1PHY_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI1PHY_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI1PHY_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI1PHY_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI1PHY_BCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI1PHY_BCR_IN)
#define HWIO_GCC_CAMSS_CSI1PHY_BCR_BLK_ARES_BMSK                                                       0x1
#define HWIO_GCC_CAMSS_CSI1PHY_BCR_BLK_ARES_SHFT                                                       0x0

#define HWIO_GCC_CAMSS_CSI1PHY_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0004f048)
#define HWIO_GCC_CAMSS_CSI1PHY_CBCR_RMSK                                                        0x80000001
#define HWIO_GCC_CAMSS_CSI1PHY_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI1PHY_CBCR_ADDR, HWIO_GCC_CAMSS_CSI1PHY_CBCR_RMSK)
#define HWIO_GCC_CAMSS_CSI1PHY_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI1PHY_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI1PHY_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI1PHY_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI1PHY_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI1PHY_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI1PHY_CBCR_IN)
#define HWIO_GCC_CAMSS_CSI1PHY_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_CAMSS_CSI1PHY_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_CAMSS_CSI1PHY_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_CAMSS_CSI1PHY_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_CAMSS_CSI1RDI_BCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0004f04c)
#define HWIO_GCC_CAMSS_CSI1RDI_BCR_RMSK                                                                0x1
#define HWIO_GCC_CAMSS_CSI1RDI_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI1RDI_BCR_ADDR, HWIO_GCC_CAMSS_CSI1RDI_BCR_RMSK)
#define HWIO_GCC_CAMSS_CSI1RDI_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI1RDI_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI1RDI_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI1RDI_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI1RDI_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI1RDI_BCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI1RDI_BCR_IN)
#define HWIO_GCC_CAMSS_CSI1RDI_BCR_BLK_ARES_BMSK                                                       0x1
#define HWIO_GCC_CAMSS_CSI1RDI_BCR_BLK_ARES_SHFT                                                       0x0

#define HWIO_GCC_CAMSS_CSI1RDI_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0004f050)
#define HWIO_GCC_CAMSS_CSI1RDI_CBCR_RMSK                                                        0x80000001
#define HWIO_GCC_CAMSS_CSI1RDI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI1RDI_CBCR_ADDR, HWIO_GCC_CAMSS_CSI1RDI_CBCR_RMSK)
#define HWIO_GCC_CAMSS_CSI1RDI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI1RDI_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI1RDI_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI1RDI_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI1RDI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI1RDI_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI1RDI_CBCR_IN)
#define HWIO_GCC_CAMSS_CSI1RDI_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_CAMSS_CSI1RDI_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_CAMSS_CSI1RDI_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_CAMSS_CSI1RDI_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_CAMSS_CSI1PIX_BCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0004f054)
#define HWIO_GCC_CAMSS_CSI1PIX_BCR_RMSK                                                                0x1
#define HWIO_GCC_CAMSS_CSI1PIX_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI1PIX_BCR_ADDR, HWIO_GCC_CAMSS_CSI1PIX_BCR_RMSK)
#define HWIO_GCC_CAMSS_CSI1PIX_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI1PIX_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI1PIX_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI1PIX_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI1PIX_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI1PIX_BCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI1PIX_BCR_IN)
#define HWIO_GCC_CAMSS_CSI1PIX_BCR_BLK_ARES_BMSK                                                       0x1
#define HWIO_GCC_CAMSS_CSI1PIX_BCR_BLK_ARES_SHFT                                                       0x0

#define HWIO_GCC_CAMSS_CSI1PIX_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0004f058)
#define HWIO_GCC_CAMSS_CSI1PIX_CBCR_RMSK                                                        0x80000001
#define HWIO_GCC_CAMSS_CSI1PIX_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI1PIX_CBCR_ADDR, HWIO_GCC_CAMSS_CSI1PIX_CBCR_RMSK)
#define HWIO_GCC_CAMSS_CSI1PIX_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI1PIX_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI1PIX_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI1PIX_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI1PIX_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI1PIX_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI1PIX_CBCR_IN)
#define HWIO_GCC_CAMSS_CSI1PIX_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_CAMSS_CSI1PIX_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_CAMSS_CSI1PIX_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_CAMSS_CSI1PIX_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_CAMSS_ISPIF_BCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00050000)
#define HWIO_GCC_CAMSS_ISPIF_BCR_RMSK                                                                  0x1
#define HWIO_GCC_CAMSS_ISPIF_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_ISPIF_BCR_ADDR, HWIO_GCC_CAMSS_ISPIF_BCR_RMSK)
#define HWIO_GCC_CAMSS_ISPIF_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_ISPIF_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_ISPIF_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_ISPIF_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_ISPIF_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_ISPIF_BCR_ADDR,m,v,HWIO_GCC_CAMSS_ISPIF_BCR_IN)
#define HWIO_GCC_CAMSS_ISPIF_BCR_BLK_ARES_BMSK                                                         0x1
#define HWIO_GCC_CAMSS_ISPIF_BCR_BLK_ARES_SHFT                                                         0x0

#define HWIO_GCC_CAMSS_ISPIF_AHB_CBCR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00050004)
#define HWIO_GCC_CAMSS_ISPIF_AHB_CBCR_RMSK                                                      0x80000001
#define HWIO_GCC_CAMSS_ISPIF_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_ISPIF_AHB_CBCR_ADDR, HWIO_GCC_CAMSS_ISPIF_AHB_CBCR_RMSK)
#define HWIO_GCC_CAMSS_ISPIF_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_ISPIF_AHB_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_ISPIF_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_ISPIF_AHB_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_ISPIF_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_ISPIF_AHB_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_ISPIF_AHB_CBCR_IN)
#define HWIO_GCC_CAMSS_ISPIF_AHB_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_GCC_CAMSS_ISPIF_AHB_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_GCC_CAMSS_ISPIF_AHB_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_GCC_CAMSS_ISPIF_AHB_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_GCC_CCI_CMD_RCGR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00051000)
#define HWIO_GCC_CCI_CMD_RCGR_RMSK                                                              0x800000f3
#define HWIO_GCC_CCI_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_CCI_CMD_RCGR_ADDR, HWIO_GCC_CCI_CMD_RCGR_RMSK)
#define HWIO_GCC_CCI_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_CCI_CMD_RCGR_ADDR, m)
#define HWIO_GCC_CCI_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_CCI_CMD_RCGR_ADDR,v)
#define HWIO_GCC_CCI_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CCI_CMD_RCGR_ADDR,m,v,HWIO_GCC_CCI_CMD_RCGR_IN)
#define HWIO_GCC_CCI_CMD_RCGR_ROOT_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_CCI_CMD_RCGR_ROOT_OFF_SHFT                                                           0x1f
#define HWIO_GCC_CCI_CMD_RCGR_DIRTY_D_BMSK                                                            0x80
#define HWIO_GCC_CCI_CMD_RCGR_DIRTY_D_SHFT                                                             0x7
#define HWIO_GCC_CCI_CMD_RCGR_DIRTY_N_BMSK                                                            0x40
#define HWIO_GCC_CCI_CMD_RCGR_DIRTY_N_SHFT                                                             0x6
#define HWIO_GCC_CCI_CMD_RCGR_DIRTY_M_BMSK                                                            0x20
#define HWIO_GCC_CCI_CMD_RCGR_DIRTY_M_SHFT                                                             0x5
#define HWIO_GCC_CCI_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                     0x10
#define HWIO_GCC_CCI_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                      0x4
#define HWIO_GCC_CCI_CMD_RCGR_ROOT_EN_BMSK                                                             0x2
#define HWIO_GCC_CCI_CMD_RCGR_ROOT_EN_SHFT                                                             0x1
#define HWIO_GCC_CCI_CMD_RCGR_UPDATE_BMSK                                                              0x1
#define HWIO_GCC_CCI_CMD_RCGR_UPDATE_SHFT                                                              0x0

#define HWIO_GCC_CCI_CFG_RCGR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00051004)
#define HWIO_GCC_CCI_CFG_RCGR_RMSK                                                                  0x371f
#define HWIO_GCC_CCI_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_CCI_CFG_RCGR_ADDR, HWIO_GCC_CCI_CFG_RCGR_RMSK)
#define HWIO_GCC_CCI_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_CCI_CFG_RCGR_ADDR, m)
#define HWIO_GCC_CCI_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_CCI_CFG_RCGR_ADDR,v)
#define HWIO_GCC_CCI_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CCI_CFG_RCGR_ADDR,m,v,HWIO_GCC_CCI_CFG_RCGR_IN)
#define HWIO_GCC_CCI_CFG_RCGR_MODE_BMSK                                                             0x3000
#define HWIO_GCC_CCI_CFG_RCGR_MODE_SHFT                                                                0xc
#define HWIO_GCC_CCI_CFG_RCGR_SRC_SEL_BMSK                                                           0x700
#define HWIO_GCC_CCI_CFG_RCGR_SRC_SEL_SHFT                                                             0x8
#define HWIO_GCC_CCI_CFG_RCGR_SRC_DIV_BMSK                                                            0x1f
#define HWIO_GCC_CCI_CFG_RCGR_SRC_DIV_SHFT                                                             0x0

#define HWIO_GCC_CCI_M_ADDR                                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00051008)
#define HWIO_GCC_CCI_M_RMSK                                                                           0xff
#define HWIO_GCC_CCI_M_IN          \
        in_dword_masked(HWIO_GCC_CCI_M_ADDR, HWIO_GCC_CCI_M_RMSK)
#define HWIO_GCC_CCI_M_INM(m)      \
        in_dword_masked(HWIO_GCC_CCI_M_ADDR, m)
#define HWIO_GCC_CCI_M_OUT(v)      \
        out_dword(HWIO_GCC_CCI_M_ADDR,v)
#define HWIO_GCC_CCI_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CCI_M_ADDR,m,v,HWIO_GCC_CCI_M_IN)
#define HWIO_GCC_CCI_M_M_BMSK                                                                         0xff
#define HWIO_GCC_CCI_M_M_SHFT                                                                          0x0

#define HWIO_GCC_CCI_N_ADDR                                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x0005100c)
#define HWIO_GCC_CCI_N_RMSK                                                                           0xff
#define HWIO_GCC_CCI_N_IN          \
        in_dword_masked(HWIO_GCC_CCI_N_ADDR, HWIO_GCC_CCI_N_RMSK)
#define HWIO_GCC_CCI_N_INM(m)      \
        in_dword_masked(HWIO_GCC_CCI_N_ADDR, m)
#define HWIO_GCC_CCI_N_OUT(v)      \
        out_dword(HWIO_GCC_CCI_N_ADDR,v)
#define HWIO_GCC_CCI_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CCI_N_ADDR,m,v,HWIO_GCC_CCI_N_IN)
#define HWIO_GCC_CCI_N_NOT_N_MINUS_M_BMSK                                                             0xff
#define HWIO_GCC_CCI_N_NOT_N_MINUS_M_SHFT                                                              0x0

#define HWIO_GCC_CCI_D_ADDR                                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00051010)
#define HWIO_GCC_CCI_D_RMSK                                                                           0xff
#define HWIO_GCC_CCI_D_IN          \
        in_dword_masked(HWIO_GCC_CCI_D_ADDR, HWIO_GCC_CCI_D_RMSK)
#define HWIO_GCC_CCI_D_INM(m)      \
        in_dword_masked(HWIO_GCC_CCI_D_ADDR, m)
#define HWIO_GCC_CCI_D_OUT(v)      \
        out_dword(HWIO_GCC_CCI_D_ADDR,v)
#define HWIO_GCC_CCI_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CCI_D_ADDR,m,v,HWIO_GCC_CCI_D_IN)
#define HWIO_GCC_CCI_D_NOT_2D_BMSK                                                                    0xff
#define HWIO_GCC_CCI_D_NOT_2D_SHFT                                                                     0x0

#define HWIO_GCC_CAMSS_CCI_BCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00051014)
#define HWIO_GCC_CAMSS_CCI_BCR_RMSK                                                                    0x1
#define HWIO_GCC_CAMSS_CCI_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CCI_BCR_ADDR, HWIO_GCC_CAMSS_CCI_BCR_RMSK)
#define HWIO_GCC_CAMSS_CCI_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CCI_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_CCI_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CCI_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_CCI_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CCI_BCR_ADDR,m,v,HWIO_GCC_CAMSS_CCI_BCR_IN)
#define HWIO_GCC_CAMSS_CCI_BCR_BLK_ARES_BMSK                                                           0x1
#define HWIO_GCC_CAMSS_CCI_BCR_BLK_ARES_SHFT                                                           0x0

#define HWIO_GCC_CAMSS_CCI_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00051018)
#define HWIO_GCC_CAMSS_CCI_CBCR_RMSK                                                            0x80000001
#define HWIO_GCC_CAMSS_CCI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CCI_CBCR_ADDR, HWIO_GCC_CAMSS_CCI_CBCR_RMSK)
#define HWIO_GCC_CAMSS_CCI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CCI_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_CCI_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CCI_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_CCI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CCI_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_CCI_CBCR_IN)
#define HWIO_GCC_CAMSS_CCI_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_CAMSS_CCI_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_CAMSS_CCI_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_CAMSS_CCI_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_CAMSS_CCI_AHB_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0005101c)
#define HWIO_GCC_CAMSS_CCI_AHB_CBCR_RMSK                                                        0x80000001
#define HWIO_GCC_CAMSS_CCI_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CCI_AHB_CBCR_ADDR, HWIO_GCC_CAMSS_CCI_AHB_CBCR_RMSK)
#define HWIO_GCC_CAMSS_CCI_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CCI_AHB_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_CCI_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CCI_AHB_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_CCI_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CCI_AHB_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_CCI_AHB_CBCR_IN)
#define HWIO_GCC_CAMSS_CCI_AHB_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_CAMSS_CCI_AHB_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_CAMSS_CCI_AHB_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_CAMSS_CCI_AHB_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_MCLK0_CMD_RCGR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00052000)
#define HWIO_GCC_MCLK0_CMD_RCGR_RMSK                                                            0x800000f3
#define HWIO_GCC_MCLK0_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_MCLK0_CMD_RCGR_ADDR, HWIO_GCC_MCLK0_CMD_RCGR_RMSK)
#define HWIO_GCC_MCLK0_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_MCLK0_CMD_RCGR_ADDR, m)
#define HWIO_GCC_MCLK0_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_MCLK0_CMD_RCGR_ADDR,v)
#define HWIO_GCC_MCLK0_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MCLK0_CMD_RCGR_ADDR,m,v,HWIO_GCC_MCLK0_CMD_RCGR_IN)
#define HWIO_GCC_MCLK0_CMD_RCGR_ROOT_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_MCLK0_CMD_RCGR_ROOT_OFF_SHFT                                                         0x1f
#define HWIO_GCC_MCLK0_CMD_RCGR_DIRTY_D_BMSK                                                          0x80
#define HWIO_GCC_MCLK0_CMD_RCGR_DIRTY_D_SHFT                                                           0x7
#define HWIO_GCC_MCLK0_CMD_RCGR_DIRTY_N_BMSK                                                          0x40
#define HWIO_GCC_MCLK0_CMD_RCGR_DIRTY_N_SHFT                                                           0x6
#define HWIO_GCC_MCLK0_CMD_RCGR_DIRTY_M_BMSK                                                          0x20
#define HWIO_GCC_MCLK0_CMD_RCGR_DIRTY_M_SHFT                                                           0x5
#define HWIO_GCC_MCLK0_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                   0x10
#define HWIO_GCC_MCLK0_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                    0x4
#define HWIO_GCC_MCLK0_CMD_RCGR_ROOT_EN_BMSK                                                           0x2
#define HWIO_GCC_MCLK0_CMD_RCGR_ROOT_EN_SHFT                                                           0x1
#define HWIO_GCC_MCLK0_CMD_RCGR_UPDATE_BMSK                                                            0x1
#define HWIO_GCC_MCLK0_CMD_RCGR_UPDATE_SHFT                                                            0x0

#define HWIO_GCC_MCLK0_CFG_RCGR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00052004)
#define HWIO_GCC_MCLK0_CFG_RCGR_RMSK                                                                0x371f
#define HWIO_GCC_MCLK0_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_MCLK0_CFG_RCGR_ADDR, HWIO_GCC_MCLK0_CFG_RCGR_RMSK)
#define HWIO_GCC_MCLK0_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_MCLK0_CFG_RCGR_ADDR, m)
#define HWIO_GCC_MCLK0_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_MCLK0_CFG_RCGR_ADDR,v)
#define HWIO_GCC_MCLK0_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MCLK0_CFG_RCGR_ADDR,m,v,HWIO_GCC_MCLK0_CFG_RCGR_IN)
#define HWIO_GCC_MCLK0_CFG_RCGR_MODE_BMSK                                                           0x3000
#define HWIO_GCC_MCLK0_CFG_RCGR_MODE_SHFT                                                              0xc
#define HWIO_GCC_MCLK0_CFG_RCGR_SRC_SEL_BMSK                                                         0x700
#define HWIO_GCC_MCLK0_CFG_RCGR_SRC_SEL_SHFT                                                           0x8
#define HWIO_GCC_MCLK0_CFG_RCGR_SRC_DIV_BMSK                                                          0x1f
#define HWIO_GCC_MCLK0_CFG_RCGR_SRC_DIV_SHFT                                                           0x0

#define HWIO_GCC_MCLK0_M_ADDR                                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x00052008)
#define HWIO_GCC_MCLK0_M_RMSK                                                                         0xff
#define HWIO_GCC_MCLK0_M_IN          \
        in_dword_masked(HWIO_GCC_MCLK0_M_ADDR, HWIO_GCC_MCLK0_M_RMSK)
#define HWIO_GCC_MCLK0_M_INM(m)      \
        in_dword_masked(HWIO_GCC_MCLK0_M_ADDR, m)
#define HWIO_GCC_MCLK0_M_OUT(v)      \
        out_dword(HWIO_GCC_MCLK0_M_ADDR,v)
#define HWIO_GCC_MCLK0_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MCLK0_M_ADDR,m,v,HWIO_GCC_MCLK0_M_IN)
#define HWIO_GCC_MCLK0_M_M_BMSK                                                                       0xff
#define HWIO_GCC_MCLK0_M_M_SHFT                                                                        0x0

#define HWIO_GCC_MCLK0_N_ADDR                                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0005200c)
#define HWIO_GCC_MCLK0_N_RMSK                                                                         0xff
#define HWIO_GCC_MCLK0_N_IN          \
        in_dword_masked(HWIO_GCC_MCLK0_N_ADDR, HWIO_GCC_MCLK0_N_RMSK)
#define HWIO_GCC_MCLK0_N_INM(m)      \
        in_dword_masked(HWIO_GCC_MCLK0_N_ADDR, m)
#define HWIO_GCC_MCLK0_N_OUT(v)      \
        out_dword(HWIO_GCC_MCLK0_N_ADDR,v)
#define HWIO_GCC_MCLK0_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MCLK0_N_ADDR,m,v,HWIO_GCC_MCLK0_N_IN)
#define HWIO_GCC_MCLK0_N_NOT_N_MINUS_M_BMSK                                                           0xff
#define HWIO_GCC_MCLK0_N_NOT_N_MINUS_M_SHFT                                                            0x0

#define HWIO_GCC_MCLK0_D_ADDR                                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x00052010)
#define HWIO_GCC_MCLK0_D_RMSK                                                                         0xff
#define HWIO_GCC_MCLK0_D_IN          \
        in_dword_masked(HWIO_GCC_MCLK0_D_ADDR, HWIO_GCC_MCLK0_D_RMSK)
#define HWIO_GCC_MCLK0_D_INM(m)      \
        in_dword_masked(HWIO_GCC_MCLK0_D_ADDR, m)
#define HWIO_GCC_MCLK0_D_OUT(v)      \
        out_dword(HWIO_GCC_MCLK0_D_ADDR,v)
#define HWIO_GCC_MCLK0_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MCLK0_D_ADDR,m,v,HWIO_GCC_MCLK0_D_IN)
#define HWIO_GCC_MCLK0_D_NOT_2D_BMSK                                                                  0xff
#define HWIO_GCC_MCLK0_D_NOT_2D_SHFT                                                                   0x0

#define HWIO_GCC_CAMSS_MCLK0_BCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00052014)
#define HWIO_GCC_CAMSS_MCLK0_BCR_RMSK                                                                  0x1
#define HWIO_GCC_CAMSS_MCLK0_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_MCLK0_BCR_ADDR, HWIO_GCC_CAMSS_MCLK0_BCR_RMSK)
#define HWIO_GCC_CAMSS_MCLK0_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_MCLK0_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_MCLK0_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_MCLK0_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_MCLK0_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_MCLK0_BCR_ADDR,m,v,HWIO_GCC_CAMSS_MCLK0_BCR_IN)
#define HWIO_GCC_CAMSS_MCLK0_BCR_BLK_ARES_BMSK                                                         0x1
#define HWIO_GCC_CAMSS_MCLK0_BCR_BLK_ARES_SHFT                                                         0x0

#define HWIO_GCC_CAMSS_MCLK0_CBCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00052018)
#define HWIO_GCC_CAMSS_MCLK0_CBCR_RMSK                                                          0x80000001
#define HWIO_GCC_CAMSS_MCLK0_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_MCLK0_CBCR_ADDR, HWIO_GCC_CAMSS_MCLK0_CBCR_RMSK)
#define HWIO_GCC_CAMSS_MCLK0_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_MCLK0_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_MCLK0_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_MCLK0_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_MCLK0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_MCLK0_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_MCLK0_CBCR_IN)
#define HWIO_GCC_CAMSS_MCLK0_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_CAMSS_MCLK0_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_GCC_CAMSS_MCLK0_CBCR_CLK_ENABLE_BMSK                                                      0x1
#define HWIO_GCC_CAMSS_MCLK0_CBCR_CLK_ENABLE_SHFT                                                      0x0

#define HWIO_GCC_MCLK1_CMD_RCGR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00053000)
#define HWIO_GCC_MCLK1_CMD_RCGR_RMSK                                                            0x800000f3
#define HWIO_GCC_MCLK1_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_MCLK1_CMD_RCGR_ADDR, HWIO_GCC_MCLK1_CMD_RCGR_RMSK)
#define HWIO_GCC_MCLK1_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_MCLK1_CMD_RCGR_ADDR, m)
#define HWIO_GCC_MCLK1_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_MCLK1_CMD_RCGR_ADDR,v)
#define HWIO_GCC_MCLK1_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MCLK1_CMD_RCGR_ADDR,m,v,HWIO_GCC_MCLK1_CMD_RCGR_IN)
#define HWIO_GCC_MCLK1_CMD_RCGR_ROOT_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_MCLK1_CMD_RCGR_ROOT_OFF_SHFT                                                         0x1f
#define HWIO_GCC_MCLK1_CMD_RCGR_DIRTY_D_BMSK                                                          0x80
#define HWIO_GCC_MCLK1_CMD_RCGR_DIRTY_D_SHFT                                                           0x7
#define HWIO_GCC_MCLK1_CMD_RCGR_DIRTY_N_BMSK                                                          0x40
#define HWIO_GCC_MCLK1_CMD_RCGR_DIRTY_N_SHFT                                                           0x6
#define HWIO_GCC_MCLK1_CMD_RCGR_DIRTY_M_BMSK                                                          0x20
#define HWIO_GCC_MCLK1_CMD_RCGR_DIRTY_M_SHFT                                                           0x5
#define HWIO_GCC_MCLK1_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                   0x10
#define HWIO_GCC_MCLK1_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                    0x4
#define HWIO_GCC_MCLK1_CMD_RCGR_ROOT_EN_BMSK                                                           0x2
#define HWIO_GCC_MCLK1_CMD_RCGR_ROOT_EN_SHFT                                                           0x1
#define HWIO_GCC_MCLK1_CMD_RCGR_UPDATE_BMSK                                                            0x1
#define HWIO_GCC_MCLK1_CMD_RCGR_UPDATE_SHFT                                                            0x0

#define HWIO_GCC_MCLK1_CFG_RCGR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00053004)
#define HWIO_GCC_MCLK1_CFG_RCGR_RMSK                                                                0x371f
#define HWIO_GCC_MCLK1_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_MCLK1_CFG_RCGR_ADDR, HWIO_GCC_MCLK1_CFG_RCGR_RMSK)
#define HWIO_GCC_MCLK1_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_MCLK1_CFG_RCGR_ADDR, m)
#define HWIO_GCC_MCLK1_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_MCLK1_CFG_RCGR_ADDR,v)
#define HWIO_GCC_MCLK1_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MCLK1_CFG_RCGR_ADDR,m,v,HWIO_GCC_MCLK1_CFG_RCGR_IN)
#define HWIO_GCC_MCLK1_CFG_RCGR_MODE_BMSK                                                           0x3000
#define HWIO_GCC_MCLK1_CFG_RCGR_MODE_SHFT                                                              0xc
#define HWIO_GCC_MCLK1_CFG_RCGR_SRC_SEL_BMSK                                                         0x700
#define HWIO_GCC_MCLK1_CFG_RCGR_SRC_SEL_SHFT                                                           0x8
#define HWIO_GCC_MCLK1_CFG_RCGR_SRC_DIV_BMSK                                                          0x1f
#define HWIO_GCC_MCLK1_CFG_RCGR_SRC_DIV_SHFT                                                           0x0

#define HWIO_GCC_MCLK1_M_ADDR                                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x00053008)
#define HWIO_GCC_MCLK1_M_RMSK                                                                         0xff
#define HWIO_GCC_MCLK1_M_IN          \
        in_dword_masked(HWIO_GCC_MCLK1_M_ADDR, HWIO_GCC_MCLK1_M_RMSK)
#define HWIO_GCC_MCLK1_M_INM(m)      \
        in_dword_masked(HWIO_GCC_MCLK1_M_ADDR, m)
#define HWIO_GCC_MCLK1_M_OUT(v)      \
        out_dword(HWIO_GCC_MCLK1_M_ADDR,v)
#define HWIO_GCC_MCLK1_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MCLK1_M_ADDR,m,v,HWIO_GCC_MCLK1_M_IN)
#define HWIO_GCC_MCLK1_M_M_BMSK                                                                       0xff
#define HWIO_GCC_MCLK1_M_M_SHFT                                                                        0x0

#define HWIO_GCC_MCLK1_N_ADDR                                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0005300c)
#define HWIO_GCC_MCLK1_N_RMSK                                                                         0xff
#define HWIO_GCC_MCLK1_N_IN          \
        in_dword_masked(HWIO_GCC_MCLK1_N_ADDR, HWIO_GCC_MCLK1_N_RMSK)
#define HWIO_GCC_MCLK1_N_INM(m)      \
        in_dword_masked(HWIO_GCC_MCLK1_N_ADDR, m)
#define HWIO_GCC_MCLK1_N_OUT(v)      \
        out_dword(HWIO_GCC_MCLK1_N_ADDR,v)
#define HWIO_GCC_MCLK1_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MCLK1_N_ADDR,m,v,HWIO_GCC_MCLK1_N_IN)
#define HWIO_GCC_MCLK1_N_NOT_N_MINUS_M_BMSK                                                           0xff
#define HWIO_GCC_MCLK1_N_NOT_N_MINUS_M_SHFT                                                            0x0

#define HWIO_GCC_MCLK1_D_ADDR                                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x00053010)
#define HWIO_GCC_MCLK1_D_RMSK                                                                         0xff
#define HWIO_GCC_MCLK1_D_IN          \
        in_dword_masked(HWIO_GCC_MCLK1_D_ADDR, HWIO_GCC_MCLK1_D_RMSK)
#define HWIO_GCC_MCLK1_D_INM(m)      \
        in_dword_masked(HWIO_GCC_MCLK1_D_ADDR, m)
#define HWIO_GCC_MCLK1_D_OUT(v)      \
        out_dword(HWIO_GCC_MCLK1_D_ADDR,v)
#define HWIO_GCC_MCLK1_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MCLK1_D_ADDR,m,v,HWIO_GCC_MCLK1_D_IN)
#define HWIO_GCC_MCLK1_D_NOT_2D_BMSK                                                                  0xff
#define HWIO_GCC_MCLK1_D_NOT_2D_SHFT                                                                   0x0

#define HWIO_GCC_CAMSS_MCLK1_BCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00053014)
#define HWIO_GCC_CAMSS_MCLK1_BCR_RMSK                                                                  0x1
#define HWIO_GCC_CAMSS_MCLK1_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_MCLK1_BCR_ADDR, HWIO_GCC_CAMSS_MCLK1_BCR_RMSK)
#define HWIO_GCC_CAMSS_MCLK1_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_MCLK1_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_MCLK1_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_MCLK1_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_MCLK1_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_MCLK1_BCR_ADDR,m,v,HWIO_GCC_CAMSS_MCLK1_BCR_IN)
#define HWIO_GCC_CAMSS_MCLK1_BCR_BLK_ARES_BMSK                                                         0x1
#define HWIO_GCC_CAMSS_MCLK1_BCR_BLK_ARES_SHFT                                                         0x0

#define HWIO_GCC_CAMSS_MCLK1_CBCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00053018)
#define HWIO_GCC_CAMSS_MCLK1_CBCR_RMSK                                                          0x80000001
#define HWIO_GCC_CAMSS_MCLK1_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_MCLK1_CBCR_ADDR, HWIO_GCC_CAMSS_MCLK1_CBCR_RMSK)
#define HWIO_GCC_CAMSS_MCLK1_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_MCLK1_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_MCLK1_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_MCLK1_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_MCLK1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_MCLK1_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_MCLK1_CBCR_IN)
#define HWIO_GCC_CAMSS_MCLK1_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_CAMSS_MCLK1_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_GCC_CAMSS_MCLK1_CBCR_CLK_ENABLE_BMSK                                                      0x1
#define HWIO_GCC_CAMSS_MCLK1_CBCR_CLK_ENABLE_SHFT                                                      0x0

#define HWIO_GCC_MM_GP0_CMD_RCGR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00054000)
#define HWIO_GCC_MM_GP0_CMD_RCGR_RMSK                                                           0x800000f3
#define HWIO_GCC_MM_GP0_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_MM_GP0_CMD_RCGR_ADDR, HWIO_GCC_MM_GP0_CMD_RCGR_RMSK)
#define HWIO_GCC_MM_GP0_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_MM_GP0_CMD_RCGR_ADDR, m)
#define HWIO_GCC_MM_GP0_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_MM_GP0_CMD_RCGR_ADDR,v)
#define HWIO_GCC_MM_GP0_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MM_GP0_CMD_RCGR_ADDR,m,v,HWIO_GCC_MM_GP0_CMD_RCGR_IN)
#define HWIO_GCC_MM_GP0_CMD_RCGR_ROOT_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_MM_GP0_CMD_RCGR_ROOT_OFF_SHFT                                                        0x1f
#define HWIO_GCC_MM_GP0_CMD_RCGR_DIRTY_D_BMSK                                                         0x80
#define HWIO_GCC_MM_GP0_CMD_RCGR_DIRTY_D_SHFT                                                          0x7
#define HWIO_GCC_MM_GP0_CMD_RCGR_DIRTY_N_BMSK                                                         0x40
#define HWIO_GCC_MM_GP0_CMD_RCGR_DIRTY_N_SHFT                                                          0x6
#define HWIO_GCC_MM_GP0_CMD_RCGR_DIRTY_M_BMSK                                                         0x20
#define HWIO_GCC_MM_GP0_CMD_RCGR_DIRTY_M_SHFT                                                          0x5
#define HWIO_GCC_MM_GP0_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                  0x10
#define HWIO_GCC_MM_GP0_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                   0x4
#define HWIO_GCC_MM_GP0_CMD_RCGR_ROOT_EN_BMSK                                                          0x2
#define HWIO_GCC_MM_GP0_CMD_RCGR_ROOT_EN_SHFT                                                          0x1
#define HWIO_GCC_MM_GP0_CMD_RCGR_UPDATE_BMSK                                                           0x1
#define HWIO_GCC_MM_GP0_CMD_RCGR_UPDATE_SHFT                                                           0x0

#define HWIO_GCC_MM_GP0_CFG_RCGR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00054004)
#define HWIO_GCC_MM_GP0_CFG_RCGR_RMSK                                                               0x371f
#define HWIO_GCC_MM_GP0_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_MM_GP0_CFG_RCGR_ADDR, HWIO_GCC_MM_GP0_CFG_RCGR_RMSK)
#define HWIO_GCC_MM_GP0_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_MM_GP0_CFG_RCGR_ADDR, m)
#define HWIO_GCC_MM_GP0_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_MM_GP0_CFG_RCGR_ADDR,v)
#define HWIO_GCC_MM_GP0_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MM_GP0_CFG_RCGR_ADDR,m,v,HWIO_GCC_MM_GP0_CFG_RCGR_IN)
#define HWIO_GCC_MM_GP0_CFG_RCGR_MODE_BMSK                                                          0x3000
#define HWIO_GCC_MM_GP0_CFG_RCGR_MODE_SHFT                                                             0xc
#define HWIO_GCC_MM_GP0_CFG_RCGR_SRC_SEL_BMSK                                                        0x700
#define HWIO_GCC_MM_GP0_CFG_RCGR_SRC_SEL_SHFT                                                          0x8
#define HWIO_GCC_MM_GP0_CFG_RCGR_SRC_DIV_BMSK                                                         0x1f
#define HWIO_GCC_MM_GP0_CFG_RCGR_SRC_DIV_SHFT                                                          0x0

#define HWIO_GCC_MM_GP0_M_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00054008)
#define HWIO_GCC_MM_GP0_M_RMSK                                                                        0xff
#define HWIO_GCC_MM_GP0_M_IN          \
        in_dword_masked(HWIO_GCC_MM_GP0_M_ADDR, HWIO_GCC_MM_GP0_M_RMSK)
#define HWIO_GCC_MM_GP0_M_INM(m)      \
        in_dword_masked(HWIO_GCC_MM_GP0_M_ADDR, m)
#define HWIO_GCC_MM_GP0_M_OUT(v)      \
        out_dword(HWIO_GCC_MM_GP0_M_ADDR,v)
#define HWIO_GCC_MM_GP0_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MM_GP0_M_ADDR,m,v,HWIO_GCC_MM_GP0_M_IN)
#define HWIO_GCC_MM_GP0_M_M_BMSK                                                                      0xff
#define HWIO_GCC_MM_GP0_M_M_SHFT                                                                       0x0

#define HWIO_GCC_MM_GP0_N_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0005400c)
#define HWIO_GCC_MM_GP0_N_RMSK                                                                        0xff
#define HWIO_GCC_MM_GP0_N_IN          \
        in_dword_masked(HWIO_GCC_MM_GP0_N_ADDR, HWIO_GCC_MM_GP0_N_RMSK)
#define HWIO_GCC_MM_GP0_N_INM(m)      \
        in_dword_masked(HWIO_GCC_MM_GP0_N_ADDR, m)
#define HWIO_GCC_MM_GP0_N_OUT(v)      \
        out_dword(HWIO_GCC_MM_GP0_N_ADDR,v)
#define HWIO_GCC_MM_GP0_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MM_GP0_N_ADDR,m,v,HWIO_GCC_MM_GP0_N_IN)
#define HWIO_GCC_MM_GP0_N_NOT_N_MINUS_M_BMSK                                                          0xff
#define HWIO_GCC_MM_GP0_N_NOT_N_MINUS_M_SHFT                                                           0x0

#define HWIO_GCC_MM_GP0_D_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00054010)
#define HWIO_GCC_MM_GP0_D_RMSK                                                                        0xff
#define HWIO_GCC_MM_GP0_D_IN          \
        in_dword_masked(HWIO_GCC_MM_GP0_D_ADDR, HWIO_GCC_MM_GP0_D_RMSK)
#define HWIO_GCC_MM_GP0_D_INM(m)      \
        in_dword_masked(HWIO_GCC_MM_GP0_D_ADDR, m)
#define HWIO_GCC_MM_GP0_D_OUT(v)      \
        out_dword(HWIO_GCC_MM_GP0_D_ADDR,v)
#define HWIO_GCC_MM_GP0_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MM_GP0_D_ADDR,m,v,HWIO_GCC_MM_GP0_D_IN)
#define HWIO_GCC_MM_GP0_D_NOT_2D_BMSK                                                                 0xff
#define HWIO_GCC_MM_GP0_D_NOT_2D_SHFT                                                                  0x0

#define HWIO_GCC_CAMSS_GP0_BCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00054014)
#define HWIO_GCC_CAMSS_GP0_BCR_RMSK                                                                    0x1
#define HWIO_GCC_CAMSS_GP0_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_GP0_BCR_ADDR, HWIO_GCC_CAMSS_GP0_BCR_RMSK)
#define HWIO_GCC_CAMSS_GP0_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_GP0_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_GP0_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_GP0_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_GP0_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_GP0_BCR_ADDR,m,v,HWIO_GCC_CAMSS_GP0_BCR_IN)
#define HWIO_GCC_CAMSS_GP0_BCR_BLK_ARES_BMSK                                                           0x1
#define HWIO_GCC_CAMSS_GP0_BCR_BLK_ARES_SHFT                                                           0x0

#define HWIO_GCC_CAMSS_GP0_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00054018)
#define HWIO_GCC_CAMSS_GP0_CBCR_RMSK                                                            0x80000001
#define HWIO_GCC_CAMSS_GP0_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_GP0_CBCR_ADDR, HWIO_GCC_CAMSS_GP0_CBCR_RMSK)
#define HWIO_GCC_CAMSS_GP0_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_GP0_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_GP0_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_GP0_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_GP0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_GP0_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_GP0_CBCR_IN)
#define HWIO_GCC_CAMSS_GP0_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_CAMSS_GP0_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_CAMSS_GP0_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_CAMSS_GP0_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_MM_GP1_CMD_RCGR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00055000)
#define HWIO_GCC_MM_GP1_CMD_RCGR_RMSK                                                           0x800000f3
#define HWIO_GCC_MM_GP1_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_MM_GP1_CMD_RCGR_ADDR, HWIO_GCC_MM_GP1_CMD_RCGR_RMSK)
#define HWIO_GCC_MM_GP1_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_MM_GP1_CMD_RCGR_ADDR, m)
#define HWIO_GCC_MM_GP1_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_MM_GP1_CMD_RCGR_ADDR,v)
#define HWIO_GCC_MM_GP1_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MM_GP1_CMD_RCGR_ADDR,m,v,HWIO_GCC_MM_GP1_CMD_RCGR_IN)
#define HWIO_GCC_MM_GP1_CMD_RCGR_ROOT_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_MM_GP1_CMD_RCGR_ROOT_OFF_SHFT                                                        0x1f
#define HWIO_GCC_MM_GP1_CMD_RCGR_DIRTY_D_BMSK                                                         0x80
#define HWIO_GCC_MM_GP1_CMD_RCGR_DIRTY_D_SHFT                                                          0x7
#define HWIO_GCC_MM_GP1_CMD_RCGR_DIRTY_N_BMSK                                                         0x40
#define HWIO_GCC_MM_GP1_CMD_RCGR_DIRTY_N_SHFT                                                          0x6
#define HWIO_GCC_MM_GP1_CMD_RCGR_DIRTY_M_BMSK                                                         0x20
#define HWIO_GCC_MM_GP1_CMD_RCGR_DIRTY_M_SHFT                                                          0x5
#define HWIO_GCC_MM_GP1_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                  0x10
#define HWIO_GCC_MM_GP1_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                   0x4
#define HWIO_GCC_MM_GP1_CMD_RCGR_ROOT_EN_BMSK                                                          0x2
#define HWIO_GCC_MM_GP1_CMD_RCGR_ROOT_EN_SHFT                                                          0x1
#define HWIO_GCC_MM_GP1_CMD_RCGR_UPDATE_BMSK                                                           0x1
#define HWIO_GCC_MM_GP1_CMD_RCGR_UPDATE_SHFT                                                           0x0

#define HWIO_GCC_MM_GP1_CFG_RCGR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00055004)
#define HWIO_GCC_MM_GP1_CFG_RCGR_RMSK                                                               0x371f
#define HWIO_GCC_MM_GP1_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_MM_GP1_CFG_RCGR_ADDR, HWIO_GCC_MM_GP1_CFG_RCGR_RMSK)
#define HWIO_GCC_MM_GP1_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_MM_GP1_CFG_RCGR_ADDR, m)
#define HWIO_GCC_MM_GP1_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_MM_GP1_CFG_RCGR_ADDR,v)
#define HWIO_GCC_MM_GP1_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MM_GP1_CFG_RCGR_ADDR,m,v,HWIO_GCC_MM_GP1_CFG_RCGR_IN)
#define HWIO_GCC_MM_GP1_CFG_RCGR_MODE_BMSK                                                          0x3000
#define HWIO_GCC_MM_GP1_CFG_RCGR_MODE_SHFT                                                             0xc
#define HWIO_GCC_MM_GP1_CFG_RCGR_SRC_SEL_BMSK                                                        0x700
#define HWIO_GCC_MM_GP1_CFG_RCGR_SRC_SEL_SHFT                                                          0x8
#define HWIO_GCC_MM_GP1_CFG_RCGR_SRC_DIV_BMSK                                                         0x1f
#define HWIO_GCC_MM_GP1_CFG_RCGR_SRC_DIV_SHFT                                                          0x0

#define HWIO_GCC_MM_GP1_M_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00055008)
#define HWIO_GCC_MM_GP1_M_RMSK                                                                        0xff
#define HWIO_GCC_MM_GP1_M_IN          \
        in_dword_masked(HWIO_GCC_MM_GP1_M_ADDR, HWIO_GCC_MM_GP1_M_RMSK)
#define HWIO_GCC_MM_GP1_M_INM(m)      \
        in_dword_masked(HWIO_GCC_MM_GP1_M_ADDR, m)
#define HWIO_GCC_MM_GP1_M_OUT(v)      \
        out_dword(HWIO_GCC_MM_GP1_M_ADDR,v)
#define HWIO_GCC_MM_GP1_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MM_GP1_M_ADDR,m,v,HWIO_GCC_MM_GP1_M_IN)
#define HWIO_GCC_MM_GP1_M_M_BMSK                                                                      0xff
#define HWIO_GCC_MM_GP1_M_M_SHFT                                                                       0x0

#define HWIO_GCC_MM_GP1_N_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0005500c)
#define HWIO_GCC_MM_GP1_N_RMSK                                                                        0xff
#define HWIO_GCC_MM_GP1_N_IN          \
        in_dword_masked(HWIO_GCC_MM_GP1_N_ADDR, HWIO_GCC_MM_GP1_N_RMSK)
#define HWIO_GCC_MM_GP1_N_INM(m)      \
        in_dword_masked(HWIO_GCC_MM_GP1_N_ADDR, m)
#define HWIO_GCC_MM_GP1_N_OUT(v)      \
        out_dword(HWIO_GCC_MM_GP1_N_ADDR,v)
#define HWIO_GCC_MM_GP1_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MM_GP1_N_ADDR,m,v,HWIO_GCC_MM_GP1_N_IN)
#define HWIO_GCC_MM_GP1_N_NOT_N_MINUS_M_BMSK                                                          0xff
#define HWIO_GCC_MM_GP1_N_NOT_N_MINUS_M_SHFT                                                           0x0

#define HWIO_GCC_MM_GP1_D_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00055010)
#define HWIO_GCC_MM_GP1_D_RMSK                                                                        0xff
#define HWIO_GCC_MM_GP1_D_IN          \
        in_dword_masked(HWIO_GCC_MM_GP1_D_ADDR, HWIO_GCC_MM_GP1_D_RMSK)
#define HWIO_GCC_MM_GP1_D_INM(m)      \
        in_dword_masked(HWIO_GCC_MM_GP1_D_ADDR, m)
#define HWIO_GCC_MM_GP1_D_OUT(v)      \
        out_dword(HWIO_GCC_MM_GP1_D_ADDR,v)
#define HWIO_GCC_MM_GP1_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MM_GP1_D_ADDR,m,v,HWIO_GCC_MM_GP1_D_IN)
#define HWIO_GCC_MM_GP1_D_NOT_2D_BMSK                                                                 0xff
#define HWIO_GCC_MM_GP1_D_NOT_2D_SHFT                                                                  0x0

#define HWIO_GCC_CAMSS_GP1_BCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00055014)
#define HWIO_GCC_CAMSS_GP1_BCR_RMSK                                                                    0x1
#define HWIO_GCC_CAMSS_GP1_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_GP1_BCR_ADDR, HWIO_GCC_CAMSS_GP1_BCR_RMSK)
#define HWIO_GCC_CAMSS_GP1_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_GP1_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_GP1_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_GP1_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_GP1_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_GP1_BCR_ADDR,m,v,HWIO_GCC_CAMSS_GP1_BCR_IN)
#define HWIO_GCC_CAMSS_GP1_BCR_BLK_ARES_BMSK                                                           0x1
#define HWIO_GCC_CAMSS_GP1_BCR_BLK_ARES_SHFT                                                           0x0

#define HWIO_GCC_CAMSS_GP1_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00055018)
#define HWIO_GCC_CAMSS_GP1_CBCR_RMSK                                                            0x80000001
#define HWIO_GCC_CAMSS_GP1_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_GP1_CBCR_ADDR, HWIO_GCC_CAMSS_GP1_CBCR_RMSK)
#define HWIO_GCC_CAMSS_GP1_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_GP1_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_GP1_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_GP1_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_GP1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_GP1_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_GP1_CBCR_IN)
#define HWIO_GCC_CAMSS_GP1_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_CAMSS_GP1_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_CAMSS_GP1_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_CAMSS_GP1_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_CAMSS_TOP_BCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00056000)
#define HWIO_GCC_CAMSS_TOP_BCR_RMSK                                                                    0x1
#define HWIO_GCC_CAMSS_TOP_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_TOP_BCR_ADDR, HWIO_GCC_CAMSS_TOP_BCR_RMSK)
#define HWIO_GCC_CAMSS_TOP_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_TOP_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_TOP_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_TOP_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_TOP_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_TOP_BCR_ADDR,m,v,HWIO_GCC_CAMSS_TOP_BCR_IN)
#define HWIO_GCC_CAMSS_TOP_BCR_BLK_ARES_BMSK                                                           0x1
#define HWIO_GCC_CAMSS_TOP_BCR_BLK_ARES_SHFT                                                           0x0

#define HWIO_GCC_CAMSS_AHB_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0005a014)
#define HWIO_GCC_CAMSS_AHB_CBCR_RMSK                                                            0x80000001
#define HWIO_GCC_CAMSS_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_AHB_CBCR_ADDR, HWIO_GCC_CAMSS_AHB_CBCR_RMSK)
#define HWIO_GCC_CAMSS_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_AHB_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_AHB_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_AHB_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_AHB_CBCR_IN)
#define HWIO_GCC_CAMSS_AHB_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_CAMSS_AHB_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_CAMSS_AHB_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_CAMSS_AHB_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_CAMSS_AHB_BCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0005a018)
#define HWIO_GCC_CAMSS_AHB_BCR_RMSK                                                                    0x1
#define HWIO_GCC_CAMSS_AHB_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_AHB_BCR_ADDR, HWIO_GCC_CAMSS_AHB_BCR_RMSK)
#define HWIO_GCC_CAMSS_AHB_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_AHB_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_AHB_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_AHB_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_AHB_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_AHB_BCR_ADDR,m,v,HWIO_GCC_CAMSS_AHB_BCR_IN)
#define HWIO_GCC_CAMSS_AHB_BCR_BLK_ARES_BMSK                                                           0x1
#define HWIO_GCC_CAMSS_AHB_BCR_BLK_ARES_SHFT                                                           0x0

#define HWIO_GCC_CAMSS_TOP_AHB_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00056004)
#define HWIO_GCC_CAMSS_TOP_AHB_CBCR_RMSK                                                        0xf0008001
#define HWIO_GCC_CAMSS_TOP_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_TOP_AHB_CBCR_ADDR, HWIO_GCC_CAMSS_TOP_AHB_CBCR_RMSK)
#define HWIO_GCC_CAMSS_TOP_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_TOP_AHB_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_TOP_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_TOP_AHB_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_TOP_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_TOP_AHB_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_TOP_AHB_CBCR_IN)
#define HWIO_GCC_CAMSS_TOP_AHB_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_CAMSS_TOP_AHB_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_CAMSS_TOP_AHB_CBCR_NOC_HANDSHAKE_FSM_STATE_BMSK                                0x70000000
#define HWIO_GCC_CAMSS_TOP_AHB_CBCR_NOC_HANDSHAKE_FSM_STATE_SHFT                                      0x1c
#define HWIO_GCC_CAMSS_TOP_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                       0x8000
#define HWIO_GCC_CAMSS_TOP_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                          0xf
#define HWIO_GCC_CAMSS_TOP_AHB_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_CAMSS_TOP_AHB_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_CAMSS_MICRO_BCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00056008)
#define HWIO_GCC_CAMSS_MICRO_BCR_RMSK                                                                  0x1
#define HWIO_GCC_CAMSS_MICRO_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_MICRO_BCR_ADDR, HWIO_GCC_CAMSS_MICRO_BCR_RMSK)
#define HWIO_GCC_CAMSS_MICRO_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_MICRO_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_MICRO_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_MICRO_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_MICRO_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_MICRO_BCR_ADDR,m,v,HWIO_GCC_CAMSS_MICRO_BCR_IN)
#define HWIO_GCC_CAMSS_MICRO_BCR_BLK_ARES_BMSK                                                         0x1
#define HWIO_GCC_CAMSS_MICRO_BCR_BLK_ARES_SHFT                                                         0x0

#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x0005600c)
#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_RMSK                                                      0x80007ff1
#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_MICRO_AHB_CBCR_ADDR, HWIO_GCC_CAMSS_MICRO_AHB_CBCR_RMSK)
#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_MICRO_AHB_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_MICRO_AHB_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_MICRO_AHB_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_MICRO_AHB_CBCR_IN)
#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_FORCE_MEM_CORE_ON_BMSK                                        0x4000
#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_FORCE_MEM_CORE_ON_SHFT                                           0xe
#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                      0x2000
#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                         0xd
#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                     0x1000
#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                        0xc
#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_WAKEUP_BMSK                                                    0xf00
#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_WAKEUP_SHFT                                                      0x8
#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_SLEEP_BMSK                                                      0xf0
#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_SLEEP_SHFT                                                       0x4
#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_GCC_CAMSS_MICRO_AHB_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_GCC_JPEG0_CMD_RCGR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00057000)
#define HWIO_GCC_JPEG0_CMD_RCGR_RMSK                                                            0x80000013
#define HWIO_GCC_JPEG0_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_JPEG0_CMD_RCGR_ADDR, HWIO_GCC_JPEG0_CMD_RCGR_RMSK)
#define HWIO_GCC_JPEG0_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_JPEG0_CMD_RCGR_ADDR, m)
#define HWIO_GCC_JPEG0_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_JPEG0_CMD_RCGR_ADDR,v)
#define HWIO_GCC_JPEG0_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_JPEG0_CMD_RCGR_ADDR,m,v,HWIO_GCC_JPEG0_CMD_RCGR_IN)
#define HWIO_GCC_JPEG0_CMD_RCGR_ROOT_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_JPEG0_CMD_RCGR_ROOT_OFF_SHFT                                                         0x1f
#define HWIO_GCC_JPEG0_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                   0x10
#define HWIO_GCC_JPEG0_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                    0x4
#define HWIO_GCC_JPEG0_CMD_RCGR_ROOT_EN_BMSK                                                           0x2
#define HWIO_GCC_JPEG0_CMD_RCGR_ROOT_EN_SHFT                                                           0x1
#define HWIO_GCC_JPEG0_CMD_RCGR_UPDATE_BMSK                                                            0x1
#define HWIO_GCC_JPEG0_CMD_RCGR_UPDATE_SHFT                                                            0x0

#define HWIO_GCC_JPEG0_CFG_RCGR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00057004)
#define HWIO_GCC_JPEG0_CFG_RCGR_RMSK                                                                 0x71f
#define HWIO_GCC_JPEG0_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_JPEG0_CFG_RCGR_ADDR, HWIO_GCC_JPEG0_CFG_RCGR_RMSK)
#define HWIO_GCC_JPEG0_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_JPEG0_CFG_RCGR_ADDR, m)
#define HWIO_GCC_JPEG0_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_JPEG0_CFG_RCGR_ADDR,v)
#define HWIO_GCC_JPEG0_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_JPEG0_CFG_RCGR_ADDR,m,v,HWIO_GCC_JPEG0_CFG_RCGR_IN)
#define HWIO_GCC_JPEG0_CFG_RCGR_SRC_SEL_BMSK                                                         0x700
#define HWIO_GCC_JPEG0_CFG_RCGR_SRC_SEL_SHFT                                                           0x8
#define HWIO_GCC_JPEG0_CFG_RCGR_SRC_DIV_BMSK                                                          0x1f
#define HWIO_GCC_JPEG0_CFG_RCGR_SRC_DIV_SHFT                                                           0x0

#define HWIO_GCC_CAMSS_JPEG_BCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00057018)
#define HWIO_GCC_CAMSS_JPEG_BCR_RMSK                                                                   0x1
#define HWIO_GCC_CAMSS_JPEG_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_JPEG_BCR_ADDR, HWIO_GCC_CAMSS_JPEG_BCR_RMSK)
#define HWIO_GCC_CAMSS_JPEG_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_JPEG_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_JPEG_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_JPEG_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_JPEG_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_JPEG_BCR_ADDR,m,v,HWIO_GCC_CAMSS_JPEG_BCR_IN)
#define HWIO_GCC_CAMSS_JPEG_BCR_BLK_ARES_BMSK                                                          0x1
#define HWIO_GCC_CAMSS_JPEG_BCR_BLK_ARES_SHFT                                                          0x0

#define HWIO_GCC_CAMSS_JPEG_GDSCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x0005701c)
#define HWIO_GCC_CAMSS_JPEG_GDSCR_RMSK                                                          0xf8fff7ff
#define HWIO_GCC_CAMSS_JPEG_GDSCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_JPEG_GDSCR_ADDR, HWIO_GCC_CAMSS_JPEG_GDSCR_RMSK)
#define HWIO_GCC_CAMSS_JPEG_GDSCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_JPEG_GDSCR_ADDR, m)
#define HWIO_GCC_CAMSS_JPEG_GDSCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_JPEG_GDSCR_ADDR,v)
#define HWIO_GCC_CAMSS_JPEG_GDSCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_JPEG_GDSCR_ADDR,m,v,HWIO_GCC_CAMSS_JPEG_GDSCR_IN)
#define HWIO_GCC_CAMSS_JPEG_GDSCR_PWR_ON_BMSK                                                   0x80000000
#define HWIO_GCC_CAMSS_JPEG_GDSCR_PWR_ON_SHFT                                                         0x1f
#define HWIO_GCC_CAMSS_JPEG_GDSCR_GDSC_STATE_BMSK                                               0x78000000
#define HWIO_GCC_CAMSS_JPEG_GDSCR_GDSC_STATE_SHFT                                                     0x1b
#define HWIO_GCC_CAMSS_JPEG_GDSCR_EN_REST_WAIT_BMSK                                               0xf00000
#define HWIO_GCC_CAMSS_JPEG_GDSCR_EN_REST_WAIT_SHFT                                                   0x14
#define HWIO_GCC_CAMSS_JPEG_GDSCR_EN_FEW_WAIT_BMSK                                                 0xf0000
#define HWIO_GCC_CAMSS_JPEG_GDSCR_EN_FEW_WAIT_SHFT                                                    0x10
#define HWIO_GCC_CAMSS_JPEG_GDSCR_CLK_DIS_WAIT_BMSK                                                 0xf000
#define HWIO_GCC_CAMSS_JPEG_GDSCR_CLK_DIS_WAIT_SHFT                                                    0xc
#define HWIO_GCC_CAMSS_JPEG_GDSCR_RESTORE_BMSK                                                       0x400
#define HWIO_GCC_CAMSS_JPEG_GDSCR_RESTORE_SHFT                                                         0xa
#define HWIO_GCC_CAMSS_JPEG_GDSCR_SAVE_BMSK                                                          0x200
#define HWIO_GCC_CAMSS_JPEG_GDSCR_SAVE_SHFT                                                            0x9
#define HWIO_GCC_CAMSS_JPEG_GDSCR_RETAIN_BMSK                                                        0x100
#define HWIO_GCC_CAMSS_JPEG_GDSCR_RETAIN_SHFT                                                          0x8
#define HWIO_GCC_CAMSS_JPEG_GDSCR_EN_REST_BMSK                                                        0x80
#define HWIO_GCC_CAMSS_JPEG_GDSCR_EN_REST_SHFT                                                         0x7
#define HWIO_GCC_CAMSS_JPEG_GDSCR_EN_FEW_BMSK                                                         0x40
#define HWIO_GCC_CAMSS_JPEG_GDSCR_EN_FEW_SHFT                                                          0x6
#define HWIO_GCC_CAMSS_JPEG_GDSCR_CLAMP_IO_BMSK                                                       0x20
#define HWIO_GCC_CAMSS_JPEG_GDSCR_CLAMP_IO_SHFT                                                        0x5
#define HWIO_GCC_CAMSS_JPEG_GDSCR_CLK_DISABLE_BMSK                                                    0x10
#define HWIO_GCC_CAMSS_JPEG_GDSCR_CLK_DISABLE_SHFT                                                     0x4
#define HWIO_GCC_CAMSS_JPEG_GDSCR_PD_ARES_BMSK                                                         0x8
#define HWIO_GCC_CAMSS_JPEG_GDSCR_PD_ARES_SHFT                                                         0x3
#define HWIO_GCC_CAMSS_JPEG_GDSCR_SW_OVERRIDE_BMSK                                                     0x4
#define HWIO_GCC_CAMSS_JPEG_GDSCR_SW_OVERRIDE_SHFT                                                     0x2
#define HWIO_GCC_CAMSS_JPEG_GDSCR_HW_CONTROL_BMSK                                                      0x2
#define HWIO_GCC_CAMSS_JPEG_GDSCR_HW_CONTROL_SHFT                                                      0x1
#define HWIO_GCC_CAMSS_JPEG_GDSCR_SW_COLLAPSE_BMSK                                                     0x1
#define HWIO_GCC_CAMSS_JPEG_GDSCR_SW_COLLAPSE_SHFT                                                     0x0

#define HWIO_GCC_CAMSS_JPEG0_CBCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00057020)
#define HWIO_GCC_CAMSS_JPEG0_CBCR_RMSK                                                          0x80007ff1
#define HWIO_GCC_CAMSS_JPEG0_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_JPEG0_CBCR_ADDR, HWIO_GCC_CAMSS_JPEG0_CBCR_RMSK)
#define HWIO_GCC_CAMSS_JPEG0_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_JPEG0_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_JPEG0_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_JPEG0_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_JPEG0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_JPEG0_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_JPEG0_CBCR_IN)
#define HWIO_GCC_CAMSS_JPEG0_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_CAMSS_JPEG0_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_GCC_CAMSS_JPEG0_CBCR_FORCE_MEM_CORE_ON_BMSK                                            0x4000
#define HWIO_GCC_CAMSS_JPEG0_CBCR_FORCE_MEM_CORE_ON_SHFT                                               0xe
#define HWIO_GCC_CAMSS_JPEG0_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                          0x2000
#define HWIO_GCC_CAMSS_JPEG0_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                             0xd
#define HWIO_GCC_CAMSS_JPEG0_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                         0x1000
#define HWIO_GCC_CAMSS_JPEG0_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                            0xc
#define HWIO_GCC_CAMSS_JPEG0_CBCR_WAKEUP_BMSK                                                        0xf00
#define HWIO_GCC_CAMSS_JPEG0_CBCR_WAKEUP_SHFT                                                          0x8
#define HWIO_GCC_CAMSS_JPEG0_CBCR_SLEEP_BMSK                                                          0xf0
#define HWIO_GCC_CAMSS_JPEG0_CBCR_SLEEP_SHFT                                                           0x4
#define HWIO_GCC_CAMSS_JPEG0_CBCR_CLK_ENABLE_BMSK                                                      0x1
#define HWIO_GCC_CAMSS_JPEG0_CBCR_CLK_ENABLE_SHFT                                                      0x0

#define HWIO_GCC_CAMSS_JPEG_AHB_CBCR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x00057024)
#define HWIO_GCC_CAMSS_JPEG_AHB_CBCR_RMSK                                                       0x80000001
#define HWIO_GCC_CAMSS_JPEG_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_JPEG_AHB_CBCR_ADDR, HWIO_GCC_CAMSS_JPEG_AHB_CBCR_RMSK)
#define HWIO_GCC_CAMSS_JPEG_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_JPEG_AHB_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_JPEG_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_JPEG_AHB_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_JPEG_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_JPEG_AHB_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_JPEG_AHB_CBCR_IN)
#define HWIO_GCC_CAMSS_JPEG_AHB_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_GCC_CAMSS_JPEG_AHB_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_GCC_CAMSS_JPEG_AHB_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_GCC_CAMSS_JPEG_AHB_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x00057028)
#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_RMSK                                                       0x80007ff1
#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_JPEG_AXI_CBCR_ADDR, HWIO_GCC_CAMSS_JPEG_AXI_CBCR_RMSK)
#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_JPEG_AXI_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_JPEG_AXI_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_JPEG_AXI_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_JPEG_AXI_CBCR_IN)
#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_FORCE_MEM_CORE_ON_BMSK                                         0x4000
#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_FORCE_MEM_CORE_ON_SHFT                                            0xe
#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                       0x2000
#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                          0xd
#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                      0x1000
#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                         0xc
#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_WAKEUP_BMSK                                                     0xf00
#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_WAKEUP_SHFT                                                       0x8
#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_SLEEP_BMSK                                                       0xf0
#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_SLEEP_SHFT                                                        0x4
#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_GCC_CAMSS_JPEG_AXI_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_GCC_VFE0_CMD_RCGR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00058000)
#define HWIO_GCC_VFE0_CMD_RCGR_RMSK                                                             0x80000013
#define HWIO_GCC_VFE0_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_VFE0_CMD_RCGR_ADDR, HWIO_GCC_VFE0_CMD_RCGR_RMSK)
#define HWIO_GCC_VFE0_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_VFE0_CMD_RCGR_ADDR, m)
#define HWIO_GCC_VFE0_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_VFE0_CMD_RCGR_ADDR,v)
#define HWIO_GCC_VFE0_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_VFE0_CMD_RCGR_ADDR,m,v,HWIO_GCC_VFE0_CMD_RCGR_IN)
#define HWIO_GCC_VFE0_CMD_RCGR_ROOT_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_VFE0_CMD_RCGR_ROOT_OFF_SHFT                                                          0x1f
#define HWIO_GCC_VFE0_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                    0x10
#define HWIO_GCC_VFE0_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                     0x4
#define HWIO_GCC_VFE0_CMD_RCGR_ROOT_EN_BMSK                                                            0x2
#define HWIO_GCC_VFE0_CMD_RCGR_ROOT_EN_SHFT                                                            0x1
#define HWIO_GCC_VFE0_CMD_RCGR_UPDATE_BMSK                                                             0x1
#define HWIO_GCC_VFE0_CMD_RCGR_UPDATE_SHFT                                                             0x0

#define HWIO_GCC_VFE0_CFG_RCGR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00058004)
#define HWIO_GCC_VFE0_CFG_RCGR_RMSK                                                                  0x71f
#define HWIO_GCC_VFE0_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_VFE0_CFG_RCGR_ADDR, HWIO_GCC_VFE0_CFG_RCGR_RMSK)
#define HWIO_GCC_VFE0_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_VFE0_CFG_RCGR_ADDR, m)
#define HWIO_GCC_VFE0_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_VFE0_CFG_RCGR_ADDR,v)
#define HWIO_GCC_VFE0_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_VFE0_CFG_RCGR_ADDR,m,v,HWIO_GCC_VFE0_CFG_RCGR_IN)
#define HWIO_GCC_VFE0_CFG_RCGR_SRC_SEL_BMSK                                                          0x700
#define HWIO_GCC_VFE0_CFG_RCGR_SRC_SEL_SHFT                                                            0x8
#define HWIO_GCC_VFE0_CFG_RCGR_SRC_DIV_BMSK                                                           0x1f
#define HWIO_GCC_VFE0_CFG_RCGR_SRC_DIV_SHFT                                                            0x0

#define HWIO_GCC_CPP_CMD_RCGR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00058018)
#define HWIO_GCC_CPP_CMD_RCGR_RMSK                                                              0x80000013
#define HWIO_GCC_CPP_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_CPP_CMD_RCGR_ADDR, HWIO_GCC_CPP_CMD_RCGR_RMSK)
#define HWIO_GCC_CPP_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_CPP_CMD_RCGR_ADDR, m)
#define HWIO_GCC_CPP_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_CPP_CMD_RCGR_ADDR,v)
#define HWIO_GCC_CPP_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CPP_CMD_RCGR_ADDR,m,v,HWIO_GCC_CPP_CMD_RCGR_IN)
#define HWIO_GCC_CPP_CMD_RCGR_ROOT_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_CPP_CMD_RCGR_ROOT_OFF_SHFT                                                           0x1f
#define HWIO_GCC_CPP_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                     0x10
#define HWIO_GCC_CPP_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                      0x4
#define HWIO_GCC_CPP_CMD_RCGR_ROOT_EN_BMSK                                                             0x2
#define HWIO_GCC_CPP_CMD_RCGR_ROOT_EN_SHFT                                                             0x1
#define HWIO_GCC_CPP_CMD_RCGR_UPDATE_BMSK                                                              0x1
#define HWIO_GCC_CPP_CMD_RCGR_UPDATE_SHFT                                                              0x0

#define HWIO_GCC_CPP_CFG_RCGR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0005801c)
#define HWIO_GCC_CPP_CFG_RCGR_RMSK                                                                   0x71f
#define HWIO_GCC_CPP_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_CPP_CFG_RCGR_ADDR, HWIO_GCC_CPP_CFG_RCGR_RMSK)
#define HWIO_GCC_CPP_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_CPP_CFG_RCGR_ADDR, m)
#define HWIO_GCC_CPP_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_CPP_CFG_RCGR_ADDR,v)
#define HWIO_GCC_CPP_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CPP_CFG_RCGR_ADDR,m,v,HWIO_GCC_CPP_CFG_RCGR_IN)
#define HWIO_GCC_CPP_CFG_RCGR_SRC_SEL_BMSK                                                           0x700
#define HWIO_GCC_CPP_CFG_RCGR_SRC_SEL_SHFT                                                             0x8
#define HWIO_GCC_CPP_CFG_RCGR_SRC_DIV_BMSK                                                            0x1f
#define HWIO_GCC_CPP_CFG_RCGR_SRC_DIV_SHFT                                                             0x0

#define HWIO_GCC_CAMSS_VFE_BCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00058030)
#define HWIO_GCC_CAMSS_VFE_BCR_RMSK                                                                    0x1
#define HWIO_GCC_CAMSS_VFE_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_VFE_BCR_ADDR, HWIO_GCC_CAMSS_VFE_BCR_RMSK)
#define HWIO_GCC_CAMSS_VFE_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_VFE_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_VFE_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_VFE_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_VFE_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_VFE_BCR_ADDR,m,v,HWIO_GCC_CAMSS_VFE_BCR_IN)
#define HWIO_GCC_CAMSS_VFE_BCR_BLK_ARES_BMSK                                                           0x1
#define HWIO_GCC_CAMSS_VFE_BCR_BLK_ARES_SHFT                                                           0x0

#define HWIO_GCC_CAMSS_VFE_GDSCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00058034)
#define HWIO_GCC_CAMSS_VFE_GDSCR_RMSK                                                           0xf8fff7ff
#define HWIO_GCC_CAMSS_VFE_GDSCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_VFE_GDSCR_ADDR, HWIO_GCC_CAMSS_VFE_GDSCR_RMSK)
#define HWIO_GCC_CAMSS_VFE_GDSCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_VFE_GDSCR_ADDR, m)
#define HWIO_GCC_CAMSS_VFE_GDSCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_VFE_GDSCR_ADDR,v)
#define HWIO_GCC_CAMSS_VFE_GDSCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_VFE_GDSCR_ADDR,m,v,HWIO_GCC_CAMSS_VFE_GDSCR_IN)
#define HWIO_GCC_CAMSS_VFE_GDSCR_PWR_ON_BMSK                                                    0x80000000
#define HWIO_GCC_CAMSS_VFE_GDSCR_PWR_ON_SHFT                                                          0x1f
#define HWIO_GCC_CAMSS_VFE_GDSCR_GDSC_STATE_BMSK                                                0x78000000
#define HWIO_GCC_CAMSS_VFE_GDSCR_GDSC_STATE_SHFT                                                      0x1b
#define HWIO_GCC_CAMSS_VFE_GDSCR_EN_REST_WAIT_BMSK                                                0xf00000
#define HWIO_GCC_CAMSS_VFE_GDSCR_EN_REST_WAIT_SHFT                                                    0x14
#define HWIO_GCC_CAMSS_VFE_GDSCR_EN_FEW_WAIT_BMSK                                                  0xf0000
#define HWIO_GCC_CAMSS_VFE_GDSCR_EN_FEW_WAIT_SHFT                                                     0x10
#define HWIO_GCC_CAMSS_VFE_GDSCR_CLK_DIS_WAIT_BMSK                                                  0xf000
#define HWIO_GCC_CAMSS_VFE_GDSCR_CLK_DIS_WAIT_SHFT                                                     0xc
#define HWIO_GCC_CAMSS_VFE_GDSCR_RESTORE_BMSK                                                        0x400
#define HWIO_GCC_CAMSS_VFE_GDSCR_RESTORE_SHFT                                                          0xa
#define HWIO_GCC_CAMSS_VFE_GDSCR_SAVE_BMSK                                                           0x200
#define HWIO_GCC_CAMSS_VFE_GDSCR_SAVE_SHFT                                                             0x9
#define HWIO_GCC_CAMSS_VFE_GDSCR_RETAIN_BMSK                                                         0x100
#define HWIO_GCC_CAMSS_VFE_GDSCR_RETAIN_SHFT                                                           0x8
#define HWIO_GCC_CAMSS_VFE_GDSCR_EN_REST_BMSK                                                         0x80
#define HWIO_GCC_CAMSS_VFE_GDSCR_EN_REST_SHFT                                                          0x7
#define HWIO_GCC_CAMSS_VFE_GDSCR_EN_FEW_BMSK                                                          0x40
#define HWIO_GCC_CAMSS_VFE_GDSCR_EN_FEW_SHFT                                                           0x6
#define HWIO_GCC_CAMSS_VFE_GDSCR_CLAMP_IO_BMSK                                                        0x20
#define HWIO_GCC_CAMSS_VFE_GDSCR_CLAMP_IO_SHFT                                                         0x5
#define HWIO_GCC_CAMSS_VFE_GDSCR_CLK_DISABLE_BMSK                                                     0x10
#define HWIO_GCC_CAMSS_VFE_GDSCR_CLK_DISABLE_SHFT                                                      0x4
#define HWIO_GCC_CAMSS_VFE_GDSCR_PD_ARES_BMSK                                                          0x8
#define HWIO_GCC_CAMSS_VFE_GDSCR_PD_ARES_SHFT                                                          0x3
#define HWIO_GCC_CAMSS_VFE_GDSCR_SW_OVERRIDE_BMSK                                                      0x4
#define HWIO_GCC_CAMSS_VFE_GDSCR_SW_OVERRIDE_SHFT                                                      0x2
#define HWIO_GCC_CAMSS_VFE_GDSCR_HW_CONTROL_BMSK                                                       0x2
#define HWIO_GCC_CAMSS_VFE_GDSCR_HW_CONTROL_SHFT                                                       0x1
#define HWIO_GCC_CAMSS_VFE_GDSCR_SW_COLLAPSE_BMSK                                                      0x1
#define HWIO_GCC_CAMSS_VFE_GDSCR_SW_COLLAPSE_SHFT                                                      0x0

#define HWIO_GCC_CAMSS_VFE0_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00058038)
#define HWIO_GCC_CAMSS_VFE0_CBCR_RMSK                                                           0x80007ff1
#define HWIO_GCC_CAMSS_VFE0_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_VFE0_CBCR_ADDR, HWIO_GCC_CAMSS_VFE0_CBCR_RMSK)
#define HWIO_GCC_CAMSS_VFE0_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_VFE0_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_VFE0_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_VFE0_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_VFE0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_VFE0_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_VFE0_CBCR_IN)
#define HWIO_GCC_CAMSS_VFE0_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_CAMSS_VFE0_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_CAMSS_VFE0_CBCR_FORCE_MEM_CORE_ON_BMSK                                             0x4000
#define HWIO_GCC_CAMSS_VFE0_CBCR_FORCE_MEM_CORE_ON_SHFT                                                0xe
#define HWIO_GCC_CAMSS_VFE0_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                           0x2000
#define HWIO_GCC_CAMSS_VFE0_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                              0xd
#define HWIO_GCC_CAMSS_VFE0_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                          0x1000
#define HWIO_GCC_CAMSS_VFE0_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                             0xc
#define HWIO_GCC_CAMSS_VFE0_CBCR_WAKEUP_BMSK                                                         0xf00
#define HWIO_GCC_CAMSS_VFE0_CBCR_WAKEUP_SHFT                                                           0x8
#define HWIO_GCC_CAMSS_VFE0_CBCR_SLEEP_BMSK                                                           0xf0
#define HWIO_GCC_CAMSS_VFE0_CBCR_SLEEP_SHFT                                                            0x4
#define HWIO_GCC_CAMSS_VFE0_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_CAMSS_VFE0_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_CAMSS_CPP_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0005803c)
#define HWIO_GCC_CAMSS_CPP_CBCR_RMSK                                                            0x80007ff1
#define HWIO_GCC_CAMSS_CPP_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CPP_CBCR_ADDR, HWIO_GCC_CAMSS_CPP_CBCR_RMSK)
#define HWIO_GCC_CAMSS_CPP_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CPP_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_CPP_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CPP_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_CPP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CPP_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_CPP_CBCR_IN)
#define HWIO_GCC_CAMSS_CPP_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_CAMSS_CPP_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_CAMSS_CPP_CBCR_FORCE_MEM_CORE_ON_BMSK                                              0x4000
#define HWIO_GCC_CAMSS_CPP_CBCR_FORCE_MEM_CORE_ON_SHFT                                                 0xe
#define HWIO_GCC_CAMSS_CPP_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                            0x2000
#define HWIO_GCC_CAMSS_CPP_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                               0xd
#define HWIO_GCC_CAMSS_CPP_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                           0x1000
#define HWIO_GCC_CAMSS_CPP_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                              0xc
#define HWIO_GCC_CAMSS_CPP_CBCR_WAKEUP_BMSK                                                          0xf00
#define HWIO_GCC_CAMSS_CPP_CBCR_WAKEUP_SHFT                                                            0x8
#define HWIO_GCC_CAMSS_CPP_CBCR_SLEEP_BMSK                                                            0xf0
#define HWIO_GCC_CAMSS_CPP_CBCR_SLEEP_SHFT                                                             0x4
#define HWIO_GCC_CAMSS_CPP_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_CAMSS_CPP_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_CAMSS_CPP_AHB_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00058040)
#define HWIO_GCC_CAMSS_CPP_AHB_CBCR_RMSK                                                        0x80000001
#define HWIO_GCC_CAMSS_CPP_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CPP_AHB_CBCR_ADDR, HWIO_GCC_CAMSS_CPP_AHB_CBCR_RMSK)
#define HWIO_GCC_CAMSS_CPP_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CPP_AHB_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_CPP_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CPP_AHB_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_CPP_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CPP_AHB_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_CPP_AHB_CBCR_IN)
#define HWIO_GCC_CAMSS_CPP_AHB_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_CAMSS_CPP_AHB_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_CAMSS_CPP_AHB_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_CAMSS_CPP_AHB_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_CAMSS_VFE_AHB_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00058044)
#define HWIO_GCC_CAMSS_VFE_AHB_CBCR_RMSK                                                        0x80000001
#define HWIO_GCC_CAMSS_VFE_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_VFE_AHB_CBCR_ADDR, HWIO_GCC_CAMSS_VFE_AHB_CBCR_RMSK)
#define HWIO_GCC_CAMSS_VFE_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_VFE_AHB_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_VFE_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_VFE_AHB_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_VFE_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_VFE_AHB_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_VFE_AHB_CBCR_IN)
#define HWIO_GCC_CAMSS_VFE_AHB_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_CAMSS_VFE_AHB_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_CAMSS_VFE_AHB_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_CAMSS_VFE_AHB_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00058048)
#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_RMSK                                                        0x80007ff1
#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_VFE_AXI_CBCR_ADDR, HWIO_GCC_CAMSS_VFE_AXI_CBCR_RMSK)
#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_VFE_AXI_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_VFE_AXI_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_VFE_AXI_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_VFE_AXI_CBCR_IN)
#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_FORCE_MEM_CORE_ON_BMSK                                          0x4000
#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_FORCE_MEM_CORE_ON_SHFT                                             0xe
#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                        0x2000
#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                           0xd
#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                       0x1000
#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                          0xc
#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_WAKEUP_BMSK                                                      0xf00
#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_WAKEUP_SHFT                                                        0x8
#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_SLEEP_BMSK                                                        0xf0
#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_SLEEP_SHFT                                                         0x4
#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_CAMSS_VFE_AXI_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_CAMSS_CSI_VFE0_BCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0005804c)
#define HWIO_GCC_CAMSS_CSI_VFE0_BCR_RMSK                                                               0x1
#define HWIO_GCC_CAMSS_CSI_VFE0_BCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI_VFE0_BCR_ADDR, HWIO_GCC_CAMSS_CSI_VFE0_BCR_RMSK)
#define HWIO_GCC_CAMSS_CSI_VFE0_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI_VFE0_BCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI_VFE0_BCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI_VFE0_BCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI_VFE0_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI_VFE0_BCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI_VFE0_BCR_IN)
#define HWIO_GCC_CAMSS_CSI_VFE0_BCR_BLK_ARES_BMSK                                                      0x1
#define HWIO_GCC_CAMSS_CSI_VFE0_BCR_BLK_ARES_SHFT                                                      0x0

#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x00058050)
#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_RMSK                                                       0x80007ff1
#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_CSI_VFE0_CBCR_ADDR, HWIO_GCC_CAMSS_CSI_VFE0_CBCR_RMSK)
#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_CSI_VFE0_CBCR_ADDR, m)
#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_CSI_VFE0_CBCR_ADDR,v)
#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_CSI_VFE0_CBCR_ADDR,m,v,HWIO_GCC_CAMSS_CSI_VFE0_CBCR_IN)
#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_FORCE_MEM_CORE_ON_BMSK                                         0x4000
#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_FORCE_MEM_CORE_ON_SHFT                                            0xe
#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                       0x2000
#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                          0xd
#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                      0x1000
#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                         0xc
#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_WAKEUP_BMSK                                                     0xf00
#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_WAKEUP_SHFT                                                       0x8
#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_SLEEP_BMSK                                                       0xf0
#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_SLEEP_SHFT                                                        0x4
#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_GCC_CAMSS_CSI_VFE0_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_GCC_GFX3D_CMD_RCGR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00059000)
#define HWIO_GCC_GFX3D_CMD_RCGR_RMSK                                                            0x80000013
#define HWIO_GCC_GFX3D_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_GFX3D_CMD_RCGR_ADDR, HWIO_GCC_GFX3D_CMD_RCGR_RMSK)
#define HWIO_GCC_GFX3D_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_GFX3D_CMD_RCGR_ADDR, m)
#define HWIO_GCC_GFX3D_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_GFX3D_CMD_RCGR_ADDR,v)
#define HWIO_GCC_GFX3D_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GFX3D_CMD_RCGR_ADDR,m,v,HWIO_GCC_GFX3D_CMD_RCGR_IN)
#define HWIO_GCC_GFX3D_CMD_RCGR_ROOT_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_GFX3D_CMD_RCGR_ROOT_OFF_SHFT                                                         0x1f
#define HWIO_GCC_GFX3D_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                   0x10
#define HWIO_GCC_GFX3D_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                    0x4
#define HWIO_GCC_GFX3D_CMD_RCGR_ROOT_EN_BMSK                                                           0x2
#define HWIO_GCC_GFX3D_CMD_RCGR_ROOT_EN_SHFT                                                           0x1
#define HWIO_GCC_GFX3D_CMD_RCGR_UPDATE_BMSK                                                            0x1
#define HWIO_GCC_GFX3D_CMD_RCGR_UPDATE_SHFT                                                            0x0

#define HWIO_GCC_GFX3D_CFG_RCGR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00059004)
#define HWIO_GCC_GFX3D_CFG_RCGR_RMSK                                                                 0x71f
#define HWIO_GCC_GFX3D_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_GFX3D_CFG_RCGR_ADDR, HWIO_GCC_GFX3D_CFG_RCGR_RMSK)
#define HWIO_GCC_GFX3D_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_GFX3D_CFG_RCGR_ADDR, m)
#define HWIO_GCC_GFX3D_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_GFX3D_CFG_RCGR_ADDR,v)
#define HWIO_GCC_GFX3D_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GFX3D_CFG_RCGR_ADDR,m,v,HWIO_GCC_GFX3D_CFG_RCGR_IN)
#define HWIO_GCC_GFX3D_CFG_RCGR_SRC_SEL_BMSK                                                         0x700
#define HWIO_GCC_GFX3D_CFG_RCGR_SRC_SEL_SHFT                                                           0x8
#define HWIO_GCC_GFX3D_CFG_RCGR_SRC_DIV_BMSK                                                          0x1f
#define HWIO_GCC_GFX3D_CFG_RCGR_SRC_DIV_SHFT                                                           0x0

#define HWIO_GCC_OXILI_BCR_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00059018)
#define HWIO_GCC_OXILI_BCR_RMSK                                                                        0x1
#define HWIO_GCC_OXILI_BCR_IN          \
        in_dword_masked(HWIO_GCC_OXILI_BCR_ADDR, HWIO_GCC_OXILI_BCR_RMSK)
#define HWIO_GCC_OXILI_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_OXILI_BCR_ADDR, m)
#define HWIO_GCC_OXILI_BCR_OUT(v)      \
        out_dword(HWIO_GCC_OXILI_BCR_ADDR,v)
#define HWIO_GCC_OXILI_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_OXILI_BCR_ADDR,m,v,HWIO_GCC_OXILI_BCR_IN)
#define HWIO_GCC_OXILI_BCR_BLK_ARES_BMSK                                                               0x1
#define HWIO_GCC_OXILI_BCR_BLK_ARES_SHFT                                                               0x0

#define HWIO_GCC_OXILI_GDSCR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0005901c)
#define HWIO_GCC_OXILI_GDSCR_RMSK                                                               0xf8fff7ff
#define HWIO_GCC_OXILI_GDSCR_IN          \
        in_dword_masked(HWIO_GCC_OXILI_GDSCR_ADDR, HWIO_GCC_OXILI_GDSCR_RMSK)
#define HWIO_GCC_OXILI_GDSCR_INM(m)      \
        in_dword_masked(HWIO_GCC_OXILI_GDSCR_ADDR, m)
#define HWIO_GCC_OXILI_GDSCR_OUT(v)      \
        out_dword(HWIO_GCC_OXILI_GDSCR_ADDR,v)
#define HWIO_GCC_OXILI_GDSCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_OXILI_GDSCR_ADDR,m,v,HWIO_GCC_OXILI_GDSCR_IN)
#define HWIO_GCC_OXILI_GDSCR_PWR_ON_BMSK                                                        0x80000000
#define HWIO_GCC_OXILI_GDSCR_PWR_ON_SHFT                                                              0x1f
#define HWIO_GCC_OXILI_GDSCR_GDSC_STATE_BMSK                                                    0x78000000
#define HWIO_GCC_OXILI_GDSCR_GDSC_STATE_SHFT                                                          0x1b
#define HWIO_GCC_OXILI_GDSCR_EN_REST_WAIT_BMSK                                                    0xf00000
#define HWIO_GCC_OXILI_GDSCR_EN_REST_WAIT_SHFT                                                        0x14
#define HWIO_GCC_OXILI_GDSCR_EN_FEW_WAIT_BMSK                                                      0xf0000
#define HWIO_GCC_OXILI_GDSCR_EN_FEW_WAIT_SHFT                                                         0x10
#define HWIO_GCC_OXILI_GDSCR_CLK_DIS_WAIT_BMSK                                                      0xf000
#define HWIO_GCC_OXILI_GDSCR_CLK_DIS_WAIT_SHFT                                                         0xc
#define HWIO_GCC_OXILI_GDSCR_RESTORE_BMSK                                                            0x400
#define HWIO_GCC_OXILI_GDSCR_RESTORE_SHFT                                                              0xa
#define HWIO_GCC_OXILI_GDSCR_SAVE_BMSK                                                               0x200
#define HWIO_GCC_OXILI_GDSCR_SAVE_SHFT                                                                 0x9
#define HWIO_GCC_OXILI_GDSCR_RETAIN_BMSK                                                             0x100
#define HWIO_GCC_OXILI_GDSCR_RETAIN_SHFT                                                               0x8
#define HWIO_GCC_OXILI_GDSCR_EN_REST_BMSK                                                             0x80
#define HWIO_GCC_OXILI_GDSCR_EN_REST_SHFT                                                              0x7
#define HWIO_GCC_OXILI_GDSCR_EN_FEW_BMSK                                                              0x40
#define HWIO_GCC_OXILI_GDSCR_EN_FEW_SHFT                                                               0x6
#define HWIO_GCC_OXILI_GDSCR_CLAMP_IO_BMSK                                                            0x20
#define HWIO_GCC_OXILI_GDSCR_CLAMP_IO_SHFT                                                             0x5
#define HWIO_GCC_OXILI_GDSCR_CLK_DISABLE_BMSK                                                         0x10
#define HWIO_GCC_OXILI_GDSCR_CLK_DISABLE_SHFT                                                          0x4
#define HWIO_GCC_OXILI_GDSCR_PD_ARES_BMSK                                                              0x8
#define HWIO_GCC_OXILI_GDSCR_PD_ARES_SHFT                                                              0x3
#define HWIO_GCC_OXILI_GDSCR_SW_OVERRIDE_BMSK                                                          0x4
#define HWIO_GCC_OXILI_GDSCR_SW_OVERRIDE_SHFT                                                          0x2
#define HWIO_GCC_OXILI_GDSCR_HW_CONTROL_BMSK                                                           0x2
#define HWIO_GCC_OXILI_GDSCR_HW_CONTROL_SHFT                                                           0x1
#define HWIO_GCC_OXILI_GDSCR_SW_COLLAPSE_BMSK                                                          0x1
#define HWIO_GCC_OXILI_GDSCR_SW_COLLAPSE_SHFT                                                          0x0

#define HWIO_GCC_OXILI_GFX3D_CBCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00059020)
#define HWIO_GCC_OXILI_GFX3D_CBCR_RMSK                                                          0x80007ff1
#define HWIO_GCC_OXILI_GFX3D_CBCR_IN          \
        in_dword_masked(HWIO_GCC_OXILI_GFX3D_CBCR_ADDR, HWIO_GCC_OXILI_GFX3D_CBCR_RMSK)
#define HWIO_GCC_OXILI_GFX3D_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_OXILI_GFX3D_CBCR_ADDR, m)
#define HWIO_GCC_OXILI_GFX3D_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_OXILI_GFX3D_CBCR_ADDR,v)
#define HWIO_GCC_OXILI_GFX3D_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_OXILI_GFX3D_CBCR_ADDR,m,v,HWIO_GCC_OXILI_GFX3D_CBCR_IN)
#define HWIO_GCC_OXILI_GFX3D_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_OXILI_GFX3D_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_GCC_OXILI_GFX3D_CBCR_FORCE_MEM_CORE_ON_BMSK                                            0x4000
#define HWIO_GCC_OXILI_GFX3D_CBCR_FORCE_MEM_CORE_ON_SHFT                                               0xe
#define HWIO_GCC_OXILI_GFX3D_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                          0x2000
#define HWIO_GCC_OXILI_GFX3D_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                             0xd
#define HWIO_GCC_OXILI_GFX3D_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                         0x1000
#define HWIO_GCC_OXILI_GFX3D_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                            0xc
#define HWIO_GCC_OXILI_GFX3D_CBCR_WAKEUP_BMSK                                                        0xf00
#define HWIO_GCC_OXILI_GFX3D_CBCR_WAKEUP_SHFT                                                          0x8
#define HWIO_GCC_OXILI_GFX3D_CBCR_SLEEP_BMSK                                                          0xf0
#define HWIO_GCC_OXILI_GFX3D_CBCR_SLEEP_SHFT                                                           0x4
#define HWIO_GCC_OXILI_GFX3D_CBCR_CLK_ENABLE_BMSK                                                      0x1
#define HWIO_GCC_OXILI_GFX3D_CBCR_CLK_ENABLE_SHFT                                                      0x0

#define HWIO_GCC_OXILI_GMEM_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00059024)
#define HWIO_GCC_OXILI_GMEM_CBCR_RMSK                                                           0x80007ff1
#define HWIO_GCC_OXILI_GMEM_CBCR_IN          \
        in_dword_masked(HWIO_GCC_OXILI_GMEM_CBCR_ADDR, HWIO_GCC_OXILI_GMEM_CBCR_RMSK)
#define HWIO_GCC_OXILI_GMEM_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_OXILI_GMEM_CBCR_ADDR, m)
#define HWIO_GCC_OXILI_GMEM_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_OXILI_GMEM_CBCR_ADDR,v)
#define HWIO_GCC_OXILI_GMEM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_OXILI_GMEM_CBCR_ADDR,m,v,HWIO_GCC_OXILI_GMEM_CBCR_IN)
#define HWIO_GCC_OXILI_GMEM_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_OXILI_GMEM_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_OXILI_GMEM_CBCR_FORCE_MEM_CORE_ON_BMSK                                             0x4000
#define HWIO_GCC_OXILI_GMEM_CBCR_FORCE_MEM_CORE_ON_SHFT                                                0xe
#define HWIO_GCC_OXILI_GMEM_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                           0x2000
#define HWIO_GCC_OXILI_GMEM_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                              0xd
#define HWIO_GCC_OXILI_GMEM_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                          0x1000
#define HWIO_GCC_OXILI_GMEM_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                             0xc
#define HWIO_GCC_OXILI_GMEM_CBCR_WAKEUP_BMSK                                                         0xf00
#define HWIO_GCC_OXILI_GMEM_CBCR_WAKEUP_SHFT                                                           0x8
#define HWIO_GCC_OXILI_GMEM_CBCR_SLEEP_BMSK                                                           0xf0
#define HWIO_GCC_OXILI_GMEM_CBCR_SLEEP_SHFT                                                            0x4
#define HWIO_GCC_OXILI_GMEM_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_OXILI_GMEM_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_OXILI_AHB_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00059028)
#define HWIO_GCC_OXILI_AHB_CBCR_RMSK                                                            0xf0008001
#define HWIO_GCC_OXILI_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_OXILI_AHB_CBCR_ADDR, HWIO_GCC_OXILI_AHB_CBCR_RMSK)
#define HWIO_GCC_OXILI_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_OXILI_AHB_CBCR_ADDR, m)
#define HWIO_GCC_OXILI_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_OXILI_AHB_CBCR_ADDR,v)
#define HWIO_GCC_OXILI_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_OXILI_AHB_CBCR_ADDR,m,v,HWIO_GCC_OXILI_AHB_CBCR_IN)
#define HWIO_GCC_OXILI_AHB_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_OXILI_AHB_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_OXILI_AHB_CBCR_NOC_HANDSHAKE_FSM_STATE_BMSK                                    0x70000000
#define HWIO_GCC_OXILI_AHB_CBCR_NOC_HANDSHAKE_FSM_STATE_SHFT                                          0x1c
#define HWIO_GCC_OXILI_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                           0x8000
#define HWIO_GCC_OXILI_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                              0xf
#define HWIO_GCC_OXILI_AHB_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_OXILI_AHB_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_GMEM_BCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0005902c)
#define HWIO_GCC_GMEM_BCR_RMSK                                                                         0x1
#define HWIO_GCC_GMEM_BCR_IN          \
        in_dword_masked(HWIO_GCC_GMEM_BCR_ADDR, HWIO_GCC_GMEM_BCR_RMSK)
#define HWIO_GCC_GMEM_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_GMEM_BCR_ADDR, m)
#define HWIO_GCC_GMEM_BCR_OUT(v)      \
        out_dword(HWIO_GCC_GMEM_BCR_ADDR,v)
#define HWIO_GCC_GMEM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GMEM_BCR_ADDR,m,v,HWIO_GCC_GMEM_BCR_IN)
#define HWIO_GCC_GMEM_BCR_BLK_ARES_BMSK                                                                0x1
#define HWIO_GCC_GMEM_BCR_BLK_ARES_SHFT                                                                0x0

#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0005a000)
#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_RMSK                                                        0x800000f3
#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_AHB_CMD_RCGR_ADDR, HWIO_GCC_CAMSS_AHB_CMD_RCGR_RMSK)
#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_AHB_CMD_RCGR_ADDR, m)
#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_AHB_CMD_RCGR_ADDR,v)
#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_AHB_CMD_RCGR_ADDR,m,v,HWIO_GCC_CAMSS_AHB_CMD_RCGR_IN)
#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_ROOT_OFF_BMSK                                               0x80000000
#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_ROOT_OFF_SHFT                                                     0x1f
#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_DIRTY_D_BMSK                                                      0x80
#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_DIRTY_D_SHFT                                                       0x7
#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_DIRTY_N_BMSK                                                      0x40
#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_DIRTY_N_SHFT                                                       0x6
#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_DIRTY_M_BMSK                                                      0x20
#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_DIRTY_M_SHFT                                                       0x5
#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                               0x10
#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                0x4
#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_ROOT_EN_BMSK                                                       0x2
#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_ROOT_EN_SHFT                                                       0x1
#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_UPDATE_BMSK                                                        0x1
#define HWIO_GCC_CAMSS_AHB_CMD_RCGR_UPDATE_SHFT                                                        0x0

#define HWIO_GCC_CAMSS_AHB_CFG_RCGR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0005a004)
#define HWIO_GCC_CAMSS_AHB_CFG_RCGR_RMSK                                                            0x371f
#define HWIO_GCC_CAMSS_AHB_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_AHB_CFG_RCGR_ADDR, HWIO_GCC_CAMSS_AHB_CFG_RCGR_RMSK)
#define HWIO_GCC_CAMSS_AHB_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_AHB_CFG_RCGR_ADDR, m)
#define HWIO_GCC_CAMSS_AHB_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_AHB_CFG_RCGR_ADDR,v)
#define HWIO_GCC_CAMSS_AHB_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_AHB_CFG_RCGR_ADDR,m,v,HWIO_GCC_CAMSS_AHB_CFG_RCGR_IN)
#define HWIO_GCC_CAMSS_AHB_CFG_RCGR_MODE_BMSK                                                       0x3000
#define HWIO_GCC_CAMSS_AHB_CFG_RCGR_MODE_SHFT                                                          0xc
#define HWIO_GCC_CAMSS_AHB_CFG_RCGR_SRC_SEL_BMSK                                                     0x700
#define HWIO_GCC_CAMSS_AHB_CFG_RCGR_SRC_SEL_SHFT                                                       0x8
#define HWIO_GCC_CAMSS_AHB_CFG_RCGR_SRC_DIV_BMSK                                                      0x1f
#define HWIO_GCC_CAMSS_AHB_CFG_RCGR_SRC_DIV_SHFT                                                       0x0

#define HWIO_GCC_CAMSS_AHB_M_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0005a008)
#define HWIO_GCC_CAMSS_AHB_M_RMSK                                                                     0xff
#define HWIO_GCC_CAMSS_AHB_M_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_AHB_M_ADDR, HWIO_GCC_CAMSS_AHB_M_RMSK)
#define HWIO_GCC_CAMSS_AHB_M_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_AHB_M_ADDR, m)
#define HWIO_GCC_CAMSS_AHB_M_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_AHB_M_ADDR,v)
#define HWIO_GCC_CAMSS_AHB_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_AHB_M_ADDR,m,v,HWIO_GCC_CAMSS_AHB_M_IN)
#define HWIO_GCC_CAMSS_AHB_M_M_BMSK                                                                   0xff
#define HWIO_GCC_CAMSS_AHB_M_M_SHFT                                                                    0x0

#define HWIO_GCC_CAMSS_AHB_N_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0005a00c)
#define HWIO_GCC_CAMSS_AHB_N_RMSK                                                                     0xff
#define HWIO_GCC_CAMSS_AHB_N_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_AHB_N_ADDR, HWIO_GCC_CAMSS_AHB_N_RMSK)
#define HWIO_GCC_CAMSS_AHB_N_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_AHB_N_ADDR, m)
#define HWIO_GCC_CAMSS_AHB_N_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_AHB_N_ADDR,v)
#define HWIO_GCC_CAMSS_AHB_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_AHB_N_ADDR,m,v,HWIO_GCC_CAMSS_AHB_N_IN)
#define HWIO_GCC_CAMSS_AHB_N_NOT_N_MINUS_M_BMSK                                                       0xff
#define HWIO_GCC_CAMSS_AHB_N_NOT_N_MINUS_M_SHFT                                                        0x0

#define HWIO_GCC_CAMSS_AHB_D_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0005a010)
#define HWIO_GCC_CAMSS_AHB_D_RMSK                                                                     0xff
#define HWIO_GCC_CAMSS_AHB_D_IN          \
        in_dword_masked(HWIO_GCC_CAMSS_AHB_D_ADDR, HWIO_GCC_CAMSS_AHB_D_RMSK)
#define HWIO_GCC_CAMSS_AHB_D_INM(m)      \
        in_dword_masked(HWIO_GCC_CAMSS_AHB_D_ADDR, m)
#define HWIO_GCC_CAMSS_AHB_D_OUT(v)      \
        out_dword(HWIO_GCC_CAMSS_AHB_D_ADDR,v)
#define HWIO_GCC_CAMSS_AHB_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CAMSS_AHB_D_ADDR,m,v,HWIO_GCC_CAMSS_AHB_D_IN)
#define HWIO_GCC_CAMSS_AHB_D_NOT_2D_BMSK                                                              0xff
#define HWIO_GCC_CAMSS_AHB_D_NOT_2D_SHFT                                                               0x0

#define HWIO_GCC_GX_DOMAIN_MISC_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0005b00c)
#define HWIO_GCC_GX_DOMAIN_MISC_RMSK                                                                0x1111
#define HWIO_GCC_GX_DOMAIN_MISC_IN          \
        in_dword_masked(HWIO_GCC_GX_DOMAIN_MISC_ADDR, HWIO_GCC_GX_DOMAIN_MISC_RMSK)
#define HWIO_GCC_GX_DOMAIN_MISC_INM(m)      \
        in_dword_masked(HWIO_GCC_GX_DOMAIN_MISC_ADDR, m)
#define HWIO_GCC_GX_DOMAIN_MISC_OUT(v)      \
        out_dword(HWIO_GCC_GX_DOMAIN_MISC_ADDR,v)
#define HWIO_GCC_GX_DOMAIN_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GX_DOMAIN_MISC_ADDR,m,v,HWIO_GCC_GX_DOMAIN_MISC_IN)
#define HWIO_GCC_GX_DOMAIN_MISC_GMEM_FIFO_HALT_BMSK                                                 0x1000
#define HWIO_GCC_GX_DOMAIN_MISC_GMEM_FIFO_HALT_SHFT                                                    0xc
#define HWIO_GCC_GX_DOMAIN_MISC_OXILI_FIFO_HALT_BMSK                                                 0x100
#define HWIO_GCC_GX_DOMAIN_MISC_OXILI_FIFO_HALT_SHFT                                                   0x8
#define HWIO_GCC_GX_DOMAIN_MISC_OXILI_GMEM_RESET_BMSK                                                 0x10
#define HWIO_GCC_GX_DOMAIN_MISC_OXILI_GMEM_RESET_SHFT                                                  0x4
#define HWIO_GCC_GX_DOMAIN_MISC_OXILI_GMEM_CLAMP_IO_BMSK                                               0x1
#define HWIO_GCC_GX_DOMAIN_MISC_OXILI_GMEM_CLAMP_IO_SHFT                                               0x0

#define HWIO_GCC_AUDIO_CORE_BCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c008)
#define HWIO_GCC_AUDIO_CORE_BCR_RMSK                                                            0x80000007
#define HWIO_GCC_AUDIO_CORE_BCR_IN          \
        in_dword_masked(HWIO_GCC_AUDIO_CORE_BCR_ADDR, HWIO_GCC_AUDIO_CORE_BCR_RMSK)
#define HWIO_GCC_AUDIO_CORE_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_AUDIO_CORE_BCR_ADDR, m)
#define HWIO_GCC_AUDIO_CORE_BCR_OUT(v)      \
        out_dword(HWIO_GCC_AUDIO_CORE_BCR_ADDR,v)
#define HWIO_GCC_AUDIO_CORE_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_AUDIO_CORE_BCR_ADDR,m,v,HWIO_GCC_AUDIO_CORE_BCR_IN)
#define HWIO_GCC_AUDIO_CORE_BCR_DFD_STATUS_BMSK                                                 0x80000000
#define HWIO_GCC_AUDIO_CORE_BCR_DFD_STATUS_SHFT                                                       0x1f
#define HWIO_GCC_AUDIO_CORE_BCR_FORCE_RESET_BMSK                                                       0x4
#define HWIO_GCC_AUDIO_CORE_BCR_FORCE_RESET_SHFT                                                       0x2
#define HWIO_GCC_AUDIO_CORE_BCR_DFD_EN_BMSK                                                            0x2
#define HWIO_GCC_AUDIO_CORE_BCR_DFD_EN_SHFT                                                            0x1
#define HWIO_GCC_AUDIO_CORE_BCR_BLK_ARES_BMSK                                                          0x1
#define HWIO_GCC_AUDIO_CORE_BCR_BLK_ARES_SHFT                                                          0x0

#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_ADDR                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c054)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_RMSK                                           0x800000f3
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_ADDR, HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_RMSK)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_ADDR,m,v,HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_IN)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_ROOT_OFF_BMSK                                  0x80000000
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_ROOT_OFF_SHFT                                        0x1f
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_DIRTY_D_BMSK                                         0x80
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_DIRTY_D_SHFT                                          0x7
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_DIRTY_N_BMSK                                         0x40
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_DIRTY_N_SHFT                                          0x6
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_DIRTY_M_BMSK                                         0x20
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_DIRTY_M_SHFT                                          0x5
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                  0x10
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                   0x4
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_ROOT_EN_BMSK                                          0x2
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_ROOT_EN_SHFT                                          0x1
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_UPDATE_BMSK                                           0x1
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR_UPDATE_SHFT                                           0x0

#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CFG_RCGR_ADDR                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c058)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CFG_RCGR_RMSK                                               0x371f
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CFG_RCGR_ADDR, HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CFG_RCGR_RMSK)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CFG_RCGR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CFG_RCGR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CFG_RCGR_ADDR,m,v,HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CFG_RCGR_IN)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CFG_RCGR_MODE_BMSK                                          0x3000
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CFG_RCGR_MODE_SHFT                                             0xc
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CFG_RCGR_SRC_SEL_BMSK                                        0x700
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CFG_RCGR_SRC_SEL_SHFT                                          0x8
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CFG_RCGR_SRC_DIV_BMSK                                         0x1f
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CFG_RCGR_SRC_DIV_SHFT                                          0x0

#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_M_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c05c)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_M_RMSK                                                        0xff
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_M_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_M_ADDR, HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_M_RMSK)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_M_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_M_ADDR, m)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_M_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_M_ADDR,v)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_M_ADDR,m,v,HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_M_IN)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_M_M_BMSK                                                      0xff
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_M_M_SHFT                                                       0x0

#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_N_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c060)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_N_RMSK                                                        0xff
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_N_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_N_ADDR, HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_N_RMSK)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_N_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_N_ADDR, m)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_N_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_N_ADDR,v)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_N_ADDR,m,v,HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_N_IN)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_N_NOT_N_MINUS_M_BMSK                                          0xff
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_N_NOT_N_MINUS_M_SHFT                                           0x0

#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_D_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c064)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_D_RMSK                                                        0xff
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_D_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_D_ADDR, HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_D_RMSK)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_D_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_D_ADDR, m)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_D_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_D_ADDR,v)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_D_ADDR,m,v,HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_D_IN)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_D_NOT_2D_BMSK                                                 0xff
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_D_NOT_2D_SHFT                                                  0x0

#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CBCR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c068)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CBCR_RMSK                                               0x80000003
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CBCR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CBCR_ADDR, HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CBCR_RMSK)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CBCR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CBCR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CBCR_ADDR,m,v,HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CBCR_IN)
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CBCR_CLK_OFF_BMSK                                       0x80000000
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CBCR_CLK_OFF_SHFT                                             0x1f
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CBCR_HW_CTL_BMSK                                               0x2
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CBCR_HW_CTL_SHFT                                               0x1
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CBCR_CLK_ENABLE_BMSK                                           0x1
#define HWIO_GCC_ULTAUDIO_LPAIF_PRI_I2S_CBCR_CLK_ENABLE_SHFT                                           0x0

#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_ADDR                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c06c)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_RMSK                                           0x800000f3
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_ADDR, HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_RMSK)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_ADDR,m,v,HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_IN)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_ROOT_OFF_BMSK                                  0x80000000
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_ROOT_OFF_SHFT                                        0x1f
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_DIRTY_D_BMSK                                         0x80
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_DIRTY_D_SHFT                                          0x7
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_DIRTY_N_BMSK                                         0x40
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_DIRTY_N_SHFT                                          0x6
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_DIRTY_M_BMSK                                         0x20
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_DIRTY_M_SHFT                                          0x5
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                  0x10
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                   0x4
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_ROOT_EN_BMSK                                          0x2
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_ROOT_EN_SHFT                                          0x1
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_UPDATE_BMSK                                           0x1
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CMD_RCGR_UPDATE_SHFT                                           0x0

#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CFG_RCGR_ADDR                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c070)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CFG_RCGR_RMSK                                               0x371f
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CFG_RCGR_ADDR, HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CFG_RCGR_RMSK)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CFG_RCGR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CFG_RCGR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CFG_RCGR_ADDR,m,v,HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CFG_RCGR_IN)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CFG_RCGR_MODE_BMSK                                          0x3000
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CFG_RCGR_MODE_SHFT                                             0xc
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CFG_RCGR_SRC_SEL_BMSK                                        0x700
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CFG_RCGR_SRC_SEL_SHFT                                          0x8
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CFG_RCGR_SRC_DIV_BMSK                                         0x1f
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CFG_RCGR_SRC_DIV_SHFT                                          0x0

#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_M_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c074)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_M_RMSK                                                        0xff
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_M_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_M_ADDR, HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_M_RMSK)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_M_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_M_ADDR, m)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_M_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_M_ADDR,v)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_M_ADDR,m,v,HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_M_IN)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_M_M_BMSK                                                      0xff
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_M_M_SHFT                                                       0x0

#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_N_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c078)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_N_RMSK                                                        0xff
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_N_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_N_ADDR, HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_N_RMSK)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_N_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_N_ADDR, m)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_N_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_N_ADDR,v)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_N_ADDR,m,v,HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_N_IN)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_N_NOT_N_MINUS_M_BMSK                                          0xff
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_N_NOT_N_MINUS_M_SHFT                                           0x0

#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_D_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c07c)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_D_RMSK                                                        0xff
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_D_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_D_ADDR, HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_D_RMSK)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_D_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_D_ADDR, m)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_D_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_D_ADDR,v)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_D_ADDR,m,v,HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_D_IN)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_D_NOT_2D_BMSK                                                 0xff
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_D_NOT_2D_SHFT                                                  0x0

#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CBCR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c080)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CBCR_RMSK                                               0x80000003
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CBCR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CBCR_ADDR, HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CBCR_RMSK)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CBCR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CBCR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CBCR_ADDR,m,v,HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CBCR_IN)
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CBCR_CLK_OFF_BMSK                                       0x80000000
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CBCR_CLK_OFF_SHFT                                             0x1f
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CBCR_HW_CTL_BMSK                                               0x2
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CBCR_HW_CTL_SHFT                                               0x1
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CBCR_CLK_ENABLE_BMSK                                           0x1
#define HWIO_GCC_ULTAUDIO_LPAIF_SEC_I2S_CBCR_CLK_ENABLE_SHFT                                           0x0

#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c034)
#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_RMSK                                                      0x800000f3
#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_ADDR, HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_RMSK)
#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_ADDR,m,v,HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_IN)
#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_ROOT_OFF_BMSK                                             0x80000000
#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_ROOT_OFF_SHFT                                                   0x1f
#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_DIRTY_D_BMSK                                                    0x80
#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_DIRTY_D_SHFT                                                     0x7
#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_DIRTY_N_BMSK                                                    0x40
#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_DIRTY_N_SHFT                                                     0x6
#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_DIRTY_M_BMSK                                                    0x20
#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_DIRTY_M_SHFT                                                     0x5
#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                             0x10
#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                              0x4
#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_ROOT_EN_BMSK                                                     0x2
#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_ROOT_EN_SHFT                                                     0x1
#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_UPDATE_BMSK                                                      0x1
#define HWIO_GCC_ULTAUDIO_XO_CMD_RCGR_UPDATE_SHFT                                                      0x0

#define HWIO_GCC_ULTAUDIO_XO_CFG_RCGR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c038)
#define HWIO_GCC_ULTAUDIO_XO_CFG_RCGR_RMSK                                                          0x371f
#define HWIO_GCC_ULTAUDIO_XO_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_XO_CFG_RCGR_ADDR, HWIO_GCC_ULTAUDIO_XO_CFG_RCGR_RMSK)
#define HWIO_GCC_ULTAUDIO_XO_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_XO_CFG_RCGR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_XO_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_XO_CFG_RCGR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_XO_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_XO_CFG_RCGR_ADDR,m,v,HWIO_GCC_ULTAUDIO_XO_CFG_RCGR_IN)
#define HWIO_GCC_ULTAUDIO_XO_CFG_RCGR_MODE_BMSK                                                     0x3000
#define HWIO_GCC_ULTAUDIO_XO_CFG_RCGR_MODE_SHFT                                                        0xc
#define HWIO_GCC_ULTAUDIO_XO_CFG_RCGR_SRC_SEL_BMSK                                                   0x700
#define HWIO_GCC_ULTAUDIO_XO_CFG_RCGR_SRC_SEL_SHFT                                                     0x8
#define HWIO_GCC_ULTAUDIO_XO_CFG_RCGR_SRC_DIV_BMSK                                                    0x1f
#define HWIO_GCC_ULTAUDIO_XO_CFG_RCGR_SRC_DIV_SHFT                                                     0x0

#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c010)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_RMSK                                               0x800000f3
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_ADDR, HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_RMSK)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_ADDR,m,v,HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_IN)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_ROOT_OFF_BMSK                                      0x80000000
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_ROOT_OFF_SHFT                                            0x1f
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_DIRTY_D_BMSK                                             0x80
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_DIRTY_D_SHFT                                              0x7
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_DIRTY_N_BMSK                                             0x40
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_DIRTY_N_SHFT                                              0x6
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_DIRTY_M_BMSK                                             0x20
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_DIRTY_M_SHFT                                              0x5
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                      0x10
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                       0x4
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_ROOT_EN_BMSK                                              0x2
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_ROOT_EN_SHFT                                              0x1
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_UPDATE_BMSK                                               0x1
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CMD_RCGR_UPDATE_SHFT                                               0x0

#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CFG_RCGR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c014)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CFG_RCGR_RMSK                                                   0x371f
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_AHBFABRIC_CFG_RCGR_ADDR, HWIO_GCC_ULTAUDIO_AHBFABRIC_CFG_RCGR_RMSK)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_AHBFABRIC_CFG_RCGR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_AHBFABRIC_CFG_RCGR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_AHBFABRIC_CFG_RCGR_ADDR,m,v,HWIO_GCC_ULTAUDIO_AHBFABRIC_CFG_RCGR_IN)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CFG_RCGR_MODE_BMSK                                              0x3000
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CFG_RCGR_MODE_SHFT                                                 0xc
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CFG_RCGR_SRC_SEL_BMSK                                            0x700
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CFG_RCGR_SRC_SEL_SHFT                                              0x8
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CFG_RCGR_SRC_DIV_BMSK                                             0x1f
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_CFG_RCGR_SRC_DIV_SHFT                                              0x0

#define HWIO_GCC_ULTAUDIO_AHBFABRIC_M_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c018)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_M_RMSK                                                            0xff
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_M_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_AHBFABRIC_M_ADDR, HWIO_GCC_ULTAUDIO_AHBFABRIC_M_RMSK)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_M_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_AHBFABRIC_M_ADDR, m)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_M_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_AHBFABRIC_M_ADDR,v)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_AHBFABRIC_M_ADDR,m,v,HWIO_GCC_ULTAUDIO_AHBFABRIC_M_IN)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_M_M_BMSK                                                          0xff
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_M_M_SHFT                                                           0x0

#define HWIO_GCC_ULTAUDIO_AHBFABRIC_N_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c01c)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_N_RMSK                                                            0xff
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_N_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_AHBFABRIC_N_ADDR, HWIO_GCC_ULTAUDIO_AHBFABRIC_N_RMSK)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_N_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_AHBFABRIC_N_ADDR, m)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_N_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_AHBFABRIC_N_ADDR,v)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_AHBFABRIC_N_ADDR,m,v,HWIO_GCC_ULTAUDIO_AHBFABRIC_N_IN)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_N_NOT_N_MINUS_M_BMSK                                              0xff
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_N_NOT_N_MINUS_M_SHFT                                               0x0

#define HWIO_GCC_ULTAUDIO_AHBFABRIC_D_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c020)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_D_RMSK                                                            0xff
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_D_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_AHBFABRIC_D_ADDR, HWIO_GCC_ULTAUDIO_AHBFABRIC_D_RMSK)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_D_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_AHBFABRIC_D_ADDR, m)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_D_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_AHBFABRIC_D_ADDR,v)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_AHBFABRIC_D_ADDR,m,v,HWIO_GCC_ULTAUDIO_AHBFABRIC_D_IN)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_D_NOT_2D_BMSK                                                     0xff
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_D_NOT_2D_SHFT                                                      0x0

#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_ADDR                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c024)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_RMSK                                      0x80007ff3
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_ADDR, HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_RMSK)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_ADDR,m,v,HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_IN)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_CLK_OFF_BMSK                              0x80000000
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_CLK_OFF_SHFT                                    0x1f
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_FORCE_MEM_CORE_ON_BMSK                        0x4000
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_FORCE_MEM_CORE_ON_SHFT                           0xe
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_FORCE_MEM_PERIPH_ON_BMSK                      0x2000
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_FORCE_MEM_PERIPH_ON_SHFT                         0xd
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                     0x1000
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                        0xc
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_WAKEUP_BMSK                                    0xf00
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_WAKEUP_SHFT                                      0x8
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_SLEEP_BMSK                                      0xf0
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_SLEEP_SHFT                                       0x4
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_HW_CTL_BMSK                                      0x2
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_HW_CTL_SHFT                                      0x1
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_CLK_ENABLE_BMSK                                  0x1
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_CLK_ENABLE_SHFT                                  0x0

#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR_ADDR                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c028)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR_RMSK                                          0x80000003
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR_ADDR, HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR_RMSK)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR_ADDR,m,v,HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR_IN)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR_CLK_OFF_BMSK                                  0x80000000
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR_CLK_OFF_SHFT                                        0x1f
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR_HW_CTL_BMSK                                          0x2
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR_HW_CTL_SHFT                                          0x1
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR_CLK_ENABLE_BMSK                                      0x1
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR_CLK_ENABLE_SHFT                                      0x0

#define HWIO_GCC_ULTAUDIO_AVSYNC_XO_CBCR_ADDR                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c04c)
#define HWIO_GCC_ULTAUDIO_AVSYNC_XO_CBCR_RMSK                                                   0x80000003
#define HWIO_GCC_ULTAUDIO_AVSYNC_XO_CBCR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_AVSYNC_XO_CBCR_ADDR, HWIO_GCC_ULTAUDIO_AVSYNC_XO_CBCR_RMSK)
#define HWIO_GCC_ULTAUDIO_AVSYNC_XO_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_AVSYNC_XO_CBCR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_AVSYNC_XO_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_AVSYNC_XO_CBCR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_AVSYNC_XO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_AVSYNC_XO_CBCR_ADDR,m,v,HWIO_GCC_ULTAUDIO_AVSYNC_XO_CBCR_IN)
#define HWIO_GCC_ULTAUDIO_AVSYNC_XO_CBCR_CLK_OFF_BMSK                                           0x80000000
#define HWIO_GCC_ULTAUDIO_AVSYNC_XO_CBCR_CLK_OFF_SHFT                                                 0x1f
#define HWIO_GCC_ULTAUDIO_AVSYNC_XO_CBCR_HW_CTL_BMSK                                                   0x2
#define HWIO_GCC_ULTAUDIO_AVSYNC_XO_CBCR_HW_CTL_SHFT                                                   0x1
#define HWIO_GCC_ULTAUDIO_AVSYNC_XO_CBCR_CLK_ENABLE_BMSK                                               0x1
#define HWIO_GCC_ULTAUDIO_AVSYNC_XO_CBCR_CLK_ENABLE_SHFT                                               0x0

#define HWIO_GCC_ULTAUDIO_STC_XO_CBCR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c050)
#define HWIO_GCC_ULTAUDIO_STC_XO_CBCR_RMSK                                                      0x80000003
#define HWIO_GCC_ULTAUDIO_STC_XO_CBCR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_STC_XO_CBCR_ADDR, HWIO_GCC_ULTAUDIO_STC_XO_CBCR_RMSK)
#define HWIO_GCC_ULTAUDIO_STC_XO_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_STC_XO_CBCR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_STC_XO_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_STC_XO_CBCR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_STC_XO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_STC_XO_CBCR_ADDR,m,v,HWIO_GCC_ULTAUDIO_STC_XO_CBCR_IN)
#define HWIO_GCC_ULTAUDIO_STC_XO_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_GCC_ULTAUDIO_STC_XO_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_GCC_ULTAUDIO_STC_XO_CBCR_HW_CTL_BMSK                                                      0x2
#define HWIO_GCC_ULTAUDIO_STC_XO_CBCR_HW_CTL_SHFT                                                      0x1
#define HWIO_GCC_ULTAUDIO_STC_XO_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_GCC_ULTAUDIO_STC_XO_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR_ADDR                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c030)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR_RMSK                                      0x80000003
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR_ADDR, HWIO_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR_RMSK)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR_ADDR,m,v,HWIO_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR_IN)
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR_CLK_OFF_BMSK                              0x80000000
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR_CLK_OFF_SHFT                                    0x1f
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR_HW_CTL_BMSK                                      0x2
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR_HW_CTL_SHFT                                      0x1
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR_CLK_ENABLE_BMSK                                  0x1
#define HWIO_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR_CLK_ENABLE_SHFT                                  0x0

#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_ADDR                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c09c)
#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_RMSK                                                   0x800000f3
#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_ADDR, HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_RMSK)
#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_ADDR, m)
#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_ADDR,v)
#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_ADDR,m,v,HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_IN)
#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_ROOT_OFF_BMSK                                          0x80000000
#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_ROOT_OFF_SHFT                                                0x1f
#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_DIRTY_D_BMSK                                                 0x80
#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_DIRTY_D_SHFT                                                  0x7
#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_DIRTY_N_BMSK                                                 0x40
#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_DIRTY_N_SHFT                                                  0x6
#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_DIRTY_M_BMSK                                                 0x20
#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_DIRTY_M_SHFT                                                  0x5
#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                          0x10
#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                           0x4
#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_ROOT_EN_BMSK                                                  0x2
#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_ROOT_EN_SHFT                                                  0x1
#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_UPDATE_BMSK                                                   0x1
#define HWIO_GCC_CODEC_DIGCODEC_CMD_RCGR_UPDATE_SHFT                                                   0x0

#define HWIO_GCC_CODEC_DIGCODEC_CFG_RCGR_ADDR                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c0a0)
#define HWIO_GCC_CODEC_DIGCODEC_CFG_RCGR_RMSK                                                       0x371f
#define HWIO_GCC_CODEC_DIGCODEC_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_CODEC_DIGCODEC_CFG_RCGR_ADDR, HWIO_GCC_CODEC_DIGCODEC_CFG_RCGR_RMSK)
#define HWIO_GCC_CODEC_DIGCODEC_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_CODEC_DIGCODEC_CFG_RCGR_ADDR, m)
#define HWIO_GCC_CODEC_DIGCODEC_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_CODEC_DIGCODEC_CFG_RCGR_ADDR,v)
#define HWIO_GCC_CODEC_DIGCODEC_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CODEC_DIGCODEC_CFG_RCGR_ADDR,m,v,HWIO_GCC_CODEC_DIGCODEC_CFG_RCGR_IN)
#define HWIO_GCC_CODEC_DIGCODEC_CFG_RCGR_MODE_BMSK                                                  0x3000
#define HWIO_GCC_CODEC_DIGCODEC_CFG_RCGR_MODE_SHFT                                                     0xc
#define HWIO_GCC_CODEC_DIGCODEC_CFG_RCGR_SRC_SEL_BMSK                                                0x700
#define HWIO_GCC_CODEC_DIGCODEC_CFG_RCGR_SRC_SEL_SHFT                                                  0x8
#define HWIO_GCC_CODEC_DIGCODEC_CFG_RCGR_SRC_DIV_BMSK                                                 0x1f
#define HWIO_GCC_CODEC_DIGCODEC_CFG_RCGR_SRC_DIV_SHFT                                                  0x0

#define HWIO_GCC_CODEC_DIGCODEC_M_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c0a4)
#define HWIO_GCC_CODEC_DIGCODEC_M_RMSK                                                                0xff
#define HWIO_GCC_CODEC_DIGCODEC_M_IN          \
        in_dword_masked(HWIO_GCC_CODEC_DIGCODEC_M_ADDR, HWIO_GCC_CODEC_DIGCODEC_M_RMSK)
#define HWIO_GCC_CODEC_DIGCODEC_M_INM(m)      \
        in_dword_masked(HWIO_GCC_CODEC_DIGCODEC_M_ADDR, m)
#define HWIO_GCC_CODEC_DIGCODEC_M_OUT(v)      \
        out_dword(HWIO_GCC_CODEC_DIGCODEC_M_ADDR,v)
#define HWIO_GCC_CODEC_DIGCODEC_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CODEC_DIGCODEC_M_ADDR,m,v,HWIO_GCC_CODEC_DIGCODEC_M_IN)
#define HWIO_GCC_CODEC_DIGCODEC_M_M_BMSK                                                              0xff
#define HWIO_GCC_CODEC_DIGCODEC_M_M_SHFT                                                               0x0

#define HWIO_GCC_CODEC_DIGCODEC_N_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c0a8)
#define HWIO_GCC_CODEC_DIGCODEC_N_RMSK                                                                0xff
#define HWIO_GCC_CODEC_DIGCODEC_N_IN          \
        in_dword_masked(HWIO_GCC_CODEC_DIGCODEC_N_ADDR, HWIO_GCC_CODEC_DIGCODEC_N_RMSK)
#define HWIO_GCC_CODEC_DIGCODEC_N_INM(m)      \
        in_dword_masked(HWIO_GCC_CODEC_DIGCODEC_N_ADDR, m)
#define HWIO_GCC_CODEC_DIGCODEC_N_OUT(v)      \
        out_dword(HWIO_GCC_CODEC_DIGCODEC_N_ADDR,v)
#define HWIO_GCC_CODEC_DIGCODEC_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CODEC_DIGCODEC_N_ADDR,m,v,HWIO_GCC_CODEC_DIGCODEC_N_IN)
#define HWIO_GCC_CODEC_DIGCODEC_N_NOT_N_MINUS_M_BMSK                                                  0xff
#define HWIO_GCC_CODEC_DIGCODEC_N_NOT_N_MINUS_M_SHFT                                                   0x0

#define HWIO_GCC_CODEC_DIGCODEC_D_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c0ac)
#define HWIO_GCC_CODEC_DIGCODEC_D_RMSK                                                                0xff
#define HWIO_GCC_CODEC_DIGCODEC_D_IN          \
        in_dword_masked(HWIO_GCC_CODEC_DIGCODEC_D_ADDR, HWIO_GCC_CODEC_DIGCODEC_D_RMSK)
#define HWIO_GCC_CODEC_DIGCODEC_D_INM(m)      \
        in_dword_masked(HWIO_GCC_CODEC_DIGCODEC_D_ADDR, m)
#define HWIO_GCC_CODEC_DIGCODEC_D_OUT(v)      \
        out_dword(HWIO_GCC_CODEC_DIGCODEC_D_ADDR,v)
#define HWIO_GCC_CODEC_DIGCODEC_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CODEC_DIGCODEC_D_ADDR,m,v,HWIO_GCC_CODEC_DIGCODEC_D_IN)
#define HWIO_GCC_CODEC_DIGCODEC_D_NOT_2D_BMSK                                                         0xff
#define HWIO_GCC_CODEC_DIGCODEC_D_NOT_2D_SHFT                                                          0x0

#define HWIO_GCC_CODEC_DIGCODEC_CBCR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c0b0)
#define HWIO_GCC_CODEC_DIGCODEC_CBCR_RMSK                                                       0x80000003
#define HWIO_GCC_CODEC_DIGCODEC_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CODEC_DIGCODEC_CBCR_ADDR, HWIO_GCC_CODEC_DIGCODEC_CBCR_RMSK)
#define HWIO_GCC_CODEC_DIGCODEC_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CODEC_DIGCODEC_CBCR_ADDR, m)
#define HWIO_GCC_CODEC_DIGCODEC_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CODEC_DIGCODEC_CBCR_ADDR,v)
#define HWIO_GCC_CODEC_DIGCODEC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CODEC_DIGCODEC_CBCR_ADDR,m,v,HWIO_GCC_CODEC_DIGCODEC_CBCR_IN)
#define HWIO_GCC_CODEC_DIGCODEC_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_GCC_CODEC_DIGCODEC_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_GCC_CODEC_DIGCODEC_CBCR_HW_CTL_BMSK                                                       0x2
#define HWIO_GCC_CODEC_DIGCODEC_CBCR_HW_CTL_SHFT                                                       0x1
#define HWIO_GCC_CODEC_DIGCODEC_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_GCC_CODEC_DIGCODEC_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_ADDR                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c084)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_RMSK                                           0x800000f3
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_ADDR, HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_RMSK)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_ADDR,m,v,HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_IN)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_ROOT_OFF_BMSK                                  0x80000000
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_ROOT_OFF_SHFT                                        0x1f
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_DIRTY_D_BMSK                                         0x80
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_DIRTY_D_SHFT                                          0x7
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_DIRTY_N_BMSK                                         0x40
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_DIRTY_N_SHFT                                          0x6
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_DIRTY_M_BMSK                                         0x20
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_DIRTY_M_SHFT                                          0x5
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                  0x10
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                   0x4
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_ROOT_EN_BMSK                                          0x2
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_ROOT_EN_SHFT                                          0x1
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_UPDATE_BMSK                                           0x1
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CMD_RCGR_UPDATE_SHFT                                           0x0

#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CFG_RCGR_ADDR                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c088)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CFG_RCGR_RMSK                                               0x371f
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CFG_RCGR_ADDR, HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CFG_RCGR_RMSK)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CFG_RCGR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CFG_RCGR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CFG_RCGR_ADDR,m,v,HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CFG_RCGR_IN)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CFG_RCGR_MODE_BMSK                                          0x3000
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CFG_RCGR_MODE_SHFT                                             0xc
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CFG_RCGR_SRC_SEL_BMSK                                        0x700
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CFG_RCGR_SRC_SEL_SHFT                                          0x8
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CFG_RCGR_SRC_DIV_BMSK                                         0x1f
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CFG_RCGR_SRC_DIV_SHFT                                          0x0

#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_M_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c08c)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_M_RMSK                                                        0xff
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_M_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_M_ADDR, HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_M_RMSK)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_M_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_M_ADDR, m)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_M_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_M_ADDR,v)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_M_ADDR,m,v,HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_M_IN)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_M_M_BMSK                                                      0xff
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_M_M_SHFT                                                       0x0

#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_N_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c090)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_N_RMSK                                                        0xff
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_N_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_N_ADDR, HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_N_RMSK)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_N_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_N_ADDR, m)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_N_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_N_ADDR,v)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_N_ADDR,m,v,HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_N_IN)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_N_NOT_N_MINUS_M_BMSK                                          0xff
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_N_NOT_N_MINUS_M_SHFT                                           0x0

#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_D_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c094)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_D_RMSK                                                        0xff
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_D_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_D_ADDR, HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_D_RMSK)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_D_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_D_ADDR, m)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_D_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_D_ADDR,v)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_D_ADDR,m,v,HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_D_IN)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_D_NOT_2D_BMSK                                                 0xff
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_D_NOT_2D_SHFT                                                  0x0

#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CBCR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0001c098)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CBCR_RMSK                                               0x80000003
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CBCR_IN          \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CBCR_ADDR, HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CBCR_RMSK)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CBCR_ADDR, m)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CBCR_ADDR,v)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CBCR_ADDR,m,v,HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CBCR_IN)
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CBCR_CLK_OFF_BMSK                                       0x80000000
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CBCR_CLK_OFF_SHFT                                             0x1f
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CBCR_HW_CTL_BMSK                                               0x2
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CBCR_HW_CTL_SHFT                                               0x1
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CBCR_CLK_ENABLE_BMSK                                           0x1
#define HWIO_GCC_ULTAUDIO_LPAIF_AUX_I2S_CBCR_CLK_ENABLE_SHFT                                           0x0


#endif /* __CLOCKS__ */
