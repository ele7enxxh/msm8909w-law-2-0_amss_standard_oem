#ifndef __HALGPIOINTHWIO_H__
#define __HALGPIOINTHWIO_H__
/*
===========================================================================
*/
/**
  @file HALgpiointHWIO.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    TLMM_CSR

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

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/gpioint/hw/msm8909/HALgpiointHWIO.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

extern uint32 nGPIOIntBaseAddress;
extern uint32 nGPIOIntBaseAddressPhys;

#define TLMM_BASE nGPIOIntBaseAddress
#define TLMM_BASE_PHYS nGPIOIntBaseAddressPhys

/*----------------------------------------------------------------------------
 * MODULE: TLMM_CSR
 *--------------------------------------------------------------------------*/

#define TLMM_CSR_REG_BASE                                                                   (TLMM_BASE      + 0x00000000)
#define TLMM_CSR_REG_BASE_PHYS                                                              (TLMM_BASE_PHYS + 0x00000000)
#define TLMM_CSR_REG_BASE_OFFS                                                              0x00000000

#define HWIO_TLMM_GPIO_CFGn_ADDR(n)                                                         (TLMM_CSR_REG_BASE      + 0x00000000 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_CFGn_PHYS(n)                                                         (TLMM_CSR_REG_BASE_PHYS + 0x00000000 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_CFGn_OFFS(n)                                                         (TLMM_CSR_REG_BASE_OFFS + 0x00000000 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_CFGn_RMSK                                                                 0x7ff
#define HWIO_TLMM_GPIO_CFGn_MAXn                                                                   112
#define HWIO_TLMM_GPIO_CFGn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_CFGn_ADDR(n), HWIO_TLMM_GPIO_CFGn_RMSK)
#define HWIO_TLMM_GPIO_CFGn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_CFGn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_CFGn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_CFGn_ADDR(n),val)
#define HWIO_TLMM_GPIO_CFGn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFGn_ADDR(n),mask,val,HWIO_TLMM_GPIO_CFGn_INI(n))
#define HWIO_TLMM_GPIO_CFGn_GPIO_HIHYS_EN_BMSK                                                   0x400
#define HWIO_TLMM_GPIO_CFGn_GPIO_HIHYS_EN_SHFT                                                     0xa
#define HWIO_TLMM_GPIO_CFGn_GPIO_OE_BMSK                                                         0x200
#define HWIO_TLMM_GPIO_CFGn_GPIO_OE_SHFT                                                           0x9
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_BMSK                                                    0x1c0
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_SHFT                                                      0x6
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_DRV_2_MA_FVAL                                             0x0
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_DRV_4_MA_FVAL                                             0x1
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_DRV_6_MA_FVAL                                             0x2
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_DRV_8_MA_FVAL                                             0x3
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_DRV_10_MA_FVAL                                            0x4
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_DRV_12_MA_FVAL                                            0x5
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_DRV_14_MA_FVAL                                            0x6
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_DRV_16_MA_FVAL                                            0x7
#define HWIO_TLMM_GPIO_CFGn_FUNC_SEL_BMSK                                                         0x3c
#define HWIO_TLMM_GPIO_CFGn_FUNC_SEL_SHFT                                                          0x2
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_BMSK                                                         0x3
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_SHFT                                                         0x0
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_NO_PULL_FVAL                                                 0x0
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_PULL_DOWN_FVAL                                               0x1
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_KEEPER_FVAL                                                  0x2
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_PULL_UP_FVAL                                                 0x3

#define HWIO_TLMM_GPIO_IN_OUTn_ADDR(n)                                                      (TLMM_CSR_REG_BASE      + 0x00000004 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_IN_OUTn_PHYS(n)                                                      (TLMM_CSR_REG_BASE_PHYS + 0x00000004 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_IN_OUTn_OFFS(n)                                                      (TLMM_CSR_REG_BASE_OFFS + 0x00000004 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_IN_OUTn_RMSK                                                                0x3
#define HWIO_TLMM_GPIO_IN_OUTn_MAXn                                                                112
#define HWIO_TLMM_GPIO_IN_OUTn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n), HWIO_TLMM_GPIO_IN_OUTn_RMSK)
#define HWIO_TLMM_GPIO_IN_OUTn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_IN_OUTn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n),val)
#define HWIO_TLMM_GPIO_IN_OUTn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n),mask,val,HWIO_TLMM_GPIO_IN_OUTn_INI(n))
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_OUT_BMSK                                                       0x2
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_OUT_SHFT                                                       0x1
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_IN_BMSK                                                        0x1
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_IN_SHFT                                                        0x0

#define HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n)                                                    (TLMM_CSR_REG_BASE      + 0x00000008 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_INTR_CFGn_PHYS(n)                                                    (TLMM_CSR_REG_BASE_PHYS + 0x00000008 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_INTR_CFGn_OFFS(n)                                                    (TLMM_CSR_REG_BASE_OFFS + 0x00000008 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_INTR_CFGn_RMSK                                                            0x1ff
#define HWIO_TLMM_GPIO_INTR_CFGn_MAXn                                                              112
#define HWIO_TLMM_GPIO_INTR_CFGn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n), HWIO_TLMM_GPIO_INTR_CFGn_RMSK)
#define HWIO_TLMM_GPIO_INTR_CFGn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_INTR_CFGn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n),val)
#define HWIO_TLMM_GPIO_INTR_CFGn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n),mask,val,HWIO_TLMM_GPIO_INTR_CFGn_INI(n))
#define HWIO_TLMM_GPIO_INTR_CFGn_DIR_CONN_EN_BMSK                                                0x100
#define HWIO_TLMM_GPIO_INTR_CFGn_DIR_CONN_EN_SHFT                                                  0x8
#define HWIO_TLMM_GPIO_INTR_CFGn_DIR_CONN_EN_DISABLE_FVAL                                          0x0
#define HWIO_TLMM_GPIO_INTR_CFGn_DIR_CONN_EN_ENABLE_FVAL                                           0x1
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_BMSK                                                 0xe0
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_SHFT                                                  0x5
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_WCSS_FVAL                                             0x0
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_SENSORS_FVAL                                          0x1
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_LPA_DSP_FVAL                                          0x2
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_RPM_FVAL                                              0x3
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_APSS_FVAL                                             0x4
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_MSS_FVAL                                              0x5
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_TZ_FVAL                                               0x6
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_NONE_FVAL                                             0x7
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_RAW_STATUS_EN_BMSK                                          0x10
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_RAW_STATUS_EN_SHFT                                           0x4
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_RAW_STATUS_EN_DISABLE_FVAL                                   0x0
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_RAW_STATUS_EN_ENABLE_FVAL                                    0x1
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_BMSK                                                0xc
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_SHFT                                                0x2
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_LEVEL_FVAL                                          0x0
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_POS_EDGE_FVAL                                       0x1
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_NEG_EDGE_FVAL                                       0x2
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_DUAL_EDGE_FVAL                                      0x3
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_POL_CTL_BMSK                                                 0x2
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_POL_CTL_SHFT                                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_POL_CTL_POLARITY_0_FVAL                                      0x0
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_POL_CTL_POLARITY_1_FVAL                                      0x1
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_ENABLE_BMSK                                                  0x1
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_ENABLE_SHFT                                                  0x0
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_ENABLE_DISABLE_FVAL                                          0x0
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_ENABLE_ENABLE_FVAL                                           0x1

#define HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n)                                                 (TLMM_CSR_REG_BASE      + 0x0000000c + 0x1000 * (n))
#define HWIO_TLMM_GPIO_INTR_STATUSn_PHYS(n)                                                 (TLMM_CSR_REG_BASE_PHYS + 0x0000000c + 0x1000 * (n))
#define HWIO_TLMM_GPIO_INTR_STATUSn_OFFS(n)                                                 (TLMM_CSR_REG_BASE_OFFS + 0x0000000c + 0x1000 * (n))
#define HWIO_TLMM_GPIO_INTR_STATUSn_RMSK                                                           0x1
#define HWIO_TLMM_GPIO_INTR_STATUSn_MAXn                                                           112
#define HWIO_TLMM_GPIO_INTR_STATUSn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n), HWIO_TLMM_GPIO_INTR_STATUSn_RMSK)
#define HWIO_TLMM_GPIO_INTR_STATUSn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_INTR_STATUSn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n),val)
#define HWIO_TLMM_GPIO_INTR_STATUSn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n),mask,val,HWIO_TLMM_GPIO_INTR_STATUSn_INI(n))
#define HWIO_TLMM_GPIO_INTR_STATUSn_INTR_STATUS_BMSK                                               0x1
#define HWIO_TLMM_GPIO_INTR_STATUSn_INTR_STATUS_SHFT                                               0x0

#define HWIO_TLMM_CLK_GATE_EN_ADDR                                                          (TLMM_CSR_REG_BASE      + 0x00100000)
#define HWIO_TLMM_CLK_GATE_EN_PHYS                                                          (TLMM_CSR_REG_BASE_PHYS + 0x00100000)
#define HWIO_TLMM_CLK_GATE_EN_OFFS                                                          (TLMM_CSR_REG_BASE_OFFS + 0x00100000)
#define HWIO_TLMM_CLK_GATE_EN_RMSK                                                                 0x7
#define HWIO_TLMM_CLK_GATE_EN_IN          \
        in_dword_masked(HWIO_TLMM_CLK_GATE_EN_ADDR, HWIO_TLMM_CLK_GATE_EN_RMSK)
#define HWIO_TLMM_CLK_GATE_EN_INM(m)      \
        in_dword_masked(HWIO_TLMM_CLK_GATE_EN_ADDR, m)
#define HWIO_TLMM_CLK_GATE_EN_OUT(v)      \
        out_dword(HWIO_TLMM_CLK_GATE_EN_ADDR,v)
#define HWIO_TLMM_CLK_GATE_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_CLK_GATE_EN_ADDR,m,v,HWIO_TLMM_CLK_GATE_EN_IN)
#define HWIO_TLMM_CLK_GATE_EN_AHB_HCLK_EN_BMSK                                                     0x4
#define HWIO_TLMM_CLK_GATE_EN_AHB_HCLK_EN_SHFT                                                     0x2
#define HWIO_TLMM_CLK_GATE_EN_AHB_HCLK_EN_DISABLE_FVAL                                             0x0
#define HWIO_TLMM_CLK_GATE_EN_AHB_HCLK_EN_ENABLE_FVAL                                              0x1
#define HWIO_TLMM_CLK_GATE_EN_SUMMARY_INTR_EN_BMSK                                                 0x2
#define HWIO_TLMM_CLK_GATE_EN_SUMMARY_INTR_EN_SHFT                                                 0x1
#define HWIO_TLMM_CLK_GATE_EN_SUMMARY_INTR_EN_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_CLK_GATE_EN_SUMMARY_INTR_EN_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_CLK_GATE_EN_CRIF_READ_EN_BMSK                                                    0x1
#define HWIO_TLMM_CLK_GATE_EN_CRIF_READ_EN_SHFT                                                    0x0
#define HWIO_TLMM_CLK_GATE_EN_CRIF_READ_EN_DISABLE_FVAL                                            0x0
#define HWIO_TLMM_CLK_GATE_EN_CRIF_READ_EN_ENABLE_FVAL                                             0x1

#define HWIO_TLMM_IE_CTRL_DISABLE_ADDR                                                      (TLMM_CSR_REG_BASE      + 0x00100004)
#define HWIO_TLMM_IE_CTRL_DISABLE_PHYS                                                      (TLMM_CSR_REG_BASE_PHYS + 0x00100004)
#define HWIO_TLMM_IE_CTRL_DISABLE_OFFS                                                      (TLMM_CSR_REG_BASE_OFFS + 0x00100004)
#define HWIO_TLMM_IE_CTRL_DISABLE_RMSK                                                             0x1
#define HWIO_TLMM_IE_CTRL_DISABLE_IN          \
        in_dword_masked(HWIO_TLMM_IE_CTRL_DISABLE_ADDR, HWIO_TLMM_IE_CTRL_DISABLE_RMSK)
#define HWIO_TLMM_IE_CTRL_DISABLE_INM(m)      \
        in_dword_masked(HWIO_TLMM_IE_CTRL_DISABLE_ADDR, m)
#define HWIO_TLMM_IE_CTRL_DISABLE_OUT(v)      \
        out_dword(HWIO_TLMM_IE_CTRL_DISABLE_ADDR,v)
#define HWIO_TLMM_IE_CTRL_DISABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_IE_CTRL_DISABLE_ADDR,m,v,HWIO_TLMM_IE_CTRL_DISABLE_IN)
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_BMSK                                             0x1
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_SHFT                                             0x0
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_ENABLE_FVAL                                      0x0
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_DISABLE_FVAL                                     0x1

#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR                                                  (TLMM_CSR_REG_BASE      + 0x00100008)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_PHYS                                                  (TLMM_CSR_REG_BASE_PHYS + 0x00100008)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_OFFS                                                  (TLMM_CSR_REG_BASE_OFFS + 0x00100008)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_RMSK                                                  0xffffffff
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_IN          \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR, HWIO_TLMM_MPM_WAKEUP_INT_EN_0_RMSK)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR, m)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_OUT(v)      \
        out_dword(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR,v)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR,m,v,HWIO_TLMM_MPM_WAKEUP_INT_EN_0_IN)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_91_BMSK                                          0x80000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_91_SHFT                                                0x1f
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_91_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_91_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_90_BMSK                                          0x40000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_90_SHFT                                                0x1e
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_90_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_90_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_65_BMSK                                          0x20000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_65_SHFT                                                0x1d
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_65_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_65_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_64_BMSK                                          0x10000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_64_SHFT                                                0x1c
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_64_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_64_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_112_BMSK                                          0x8000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_112_SHFT                                               0x1b
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_112_DISABLE_FVAL                                        0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_112_ENABLE_FVAL                                         0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_111_BMSK                                          0x4000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_111_SHFT                                               0x1a
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_111_DISABLE_FVAL                                        0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_111_ENABLE_FVAL                                         0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_58_BMSK                                           0x2000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_58_SHFT                                                0x19
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_58_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_58_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_20_BMSK                                           0x1000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_20_SHFT                                                0x18
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_20_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_20_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_50_BMSK                                            0x800000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_50_SHFT                                                0x17
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_50_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_50_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_49_BMSK                                            0x400000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_49_SHFT                                                0x16
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_49_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_49_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_48_BMSK                                            0x200000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_48_SHFT                                                0x15
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_48_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_48_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_45_BMSK                                            0x100000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_45_SHFT                                                0x14
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_45_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_45_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_38_BMSK                                             0x80000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_38_SHFT                                                0x13
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_38_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_38_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_37_BMSK                                             0x40000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_37_SHFT                                                0x12
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_37_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_37_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_36_BMSK                                             0x20000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_36_SHFT                                                0x11
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_36_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_36_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_35_BMSK                                             0x10000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_35_SHFT                                                0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_35_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_35_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_34_BMSK                                              0x8000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_34_SHFT                                                 0xf
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_34_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_34_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_31_BMSK                                              0x4000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_31_SHFT                                                 0xe
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_31_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_31_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_28_BMSK                                              0x2000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_28_SHFT                                                 0xd
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_28_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_28_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_25_BMSK                                              0x1000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_25_SHFT                                                 0xc
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_25_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_25_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_44_BMSK                                               0x800
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_44_SHFT                                                 0xb
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_44_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_44_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_21_BMSK                                               0x400
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_21_SHFT                                                 0xa
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_21_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_21_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_46_BMSK                                               0x200
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_46_SHFT                                                 0x9
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_46_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_46_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_13_BMSK                                               0x100
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_13_SHFT                                                 0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_13_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_13_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_12_BMSK                                                0x80
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_12_SHFT                                                 0x7
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_12_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_12_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_11_BMSK                                                0x40
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_11_SHFT                                                 0x6
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_11_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_11_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_43_BMSK                                                0x20
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_43_SHFT                                                 0x5
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_43_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_43_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_5_BMSK                                                 0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_5_SHFT                                                  0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_5_DISABLE_FVAL                                          0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_5_ENABLE_FVAL                                           0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_42_BMSK                                                 0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_42_SHFT                                                 0x3
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_42_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_42_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_41_BMSK                                                 0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_41_SHFT                                                 0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_41_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_41_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_40_BMSK                                                 0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_40_SHFT                                                 0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_40_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_40_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_39_BMSK                                                 0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_39_SHFT                                                 0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_39_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_39_ENABLE_FVAL                                          0x1

#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR                                                  (TLMM_CSR_REG_BASE      + 0x0010000c)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_PHYS                                                  (TLMM_CSR_REG_BASE_PHYS + 0x0010000c)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_OFFS                                                  (TLMM_CSR_REG_BASE_OFFS + 0x0010000c)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_RMSK                                                      0x1fff
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_IN          \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR, HWIO_TLMM_MPM_WAKEUP_INT_EN_1_RMSK)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR, m)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_OUT(v)      \
        out_dword(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR,v)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR,m,v,HWIO_TLMM_MPM_WAKEUP_INT_EN_1_IN)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_107_BMSK                                             0x1000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_107_SHFT                                                0xc
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_107_DISABLE_FVAL                                        0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_107_ENABLE_FVAL                                         0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_105_BMSK                                              0x800
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_105_SHFT                                                0xb
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_105_DISABLE_FVAL                                        0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_105_ENABLE_FVAL                                         0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_3_BMSK                                           0x400
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_3_SHFT                                             0xa
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_3_DISABLE_FVAL                                     0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_3_ENABLE_FVAL                                      0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_1_BMSK                                           0x200
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_1_SHFT                                             0x9
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_1_DISABLE_FVAL                                     0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_1_ENABLE_FVAL                                      0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_110_BMSK                                              0x100
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_110_SHFT                                                0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_110_DISABLE_FVAL                                        0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_110_ENABLE_FVAL                                         0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_98_BMSK                                                0x80
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_98_SHFT                                                 0x7
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_98_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_98_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SRST_N_BMSK                                                 0x40
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SRST_N_SHFT                                                  0x6
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SRST_N_DISABLE_FVAL                                          0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SRST_N_ENABLE_FVAL                                           0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_97_BMSK                                                0x20
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_97_SHFT                                                 0x5
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_97_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_97_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_96_BMSK                                                0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_96_SHFT                                                 0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_96_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_96_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_95_BMSK                                                 0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_95_SHFT                                                 0x3
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_95_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_95_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_94_BMSK                                                 0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_94_SHFT                                                 0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_94_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_94_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_47_BMSK                                                 0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_47_SHFT                                                 0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_47_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_47_ENABLE_FVAL                                          0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_92_BMSK                                                 0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_92_SHFT                                                 0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_92_DISABLE_FVAL                                         0x0
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_92_ENABLE_FVAL                                          0x1

#define HWIO_TLMM_INT_JTAG_CTL_ADDR                                                         (TLMM_CSR_REG_BASE      + 0x00110000)
#define HWIO_TLMM_INT_JTAG_CTL_PHYS                                                         (TLMM_CSR_REG_BASE_PHYS + 0x00110000)
#define HWIO_TLMM_INT_JTAG_CTL_OFFS                                                         (TLMM_CSR_REG_BASE_OFFS + 0x00110000)
#define HWIO_TLMM_INT_JTAG_CTL_RMSK                                                                0xf
#define HWIO_TLMM_INT_JTAG_CTL_IN          \
        in_dword_masked(HWIO_TLMM_INT_JTAG_CTL_ADDR, HWIO_TLMM_INT_JTAG_CTL_RMSK)
#define HWIO_TLMM_INT_JTAG_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_INT_JTAG_CTL_ADDR, m)
#define HWIO_TLMM_INT_JTAG_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_INT_JTAG_CTL_ADDR,v)
#define HWIO_TLMM_INT_JTAG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_INT_JTAG_CTL_ADDR,m,v,HWIO_TLMM_INT_JTAG_CTL_IN)
#define HWIO_TLMM_INT_JTAG_CTL_APSS_TAP_ENA_BMSK                                                   0x8
#define HWIO_TLMM_INT_JTAG_CTL_APSS_TAP_ENA_SHFT                                                   0x3
#define HWIO_TLMM_INT_JTAG_CTL_QDSS_TAP_ENA_BMSK                                                   0x4
#define HWIO_TLMM_INT_JTAG_CTL_QDSS_TAP_ENA_SHFT                                                   0x2
#define HWIO_TLMM_INT_JTAG_CTL_ACC_TAP_ENA_BMSK                                                    0x2
#define HWIO_TLMM_INT_JTAG_CTL_ACC_TAP_ENA_SHFT                                                    0x1
#define HWIO_TLMM_INT_JTAG_CTL_MSM_TAP_ENA_BMSK                                                    0x1
#define HWIO_TLMM_INT_JTAG_CTL_MSM_TAP_ENA_SHFT                                                    0x0

#define HWIO_TLMM_ETM_MODE_ADDR                                                             (TLMM_CSR_REG_BASE      + 0x00110004)
#define HWIO_TLMM_ETM_MODE_PHYS                                                             (TLMM_CSR_REG_BASE_PHYS + 0x00110004)
#define HWIO_TLMM_ETM_MODE_OFFS                                                             (TLMM_CSR_REG_BASE_OFFS + 0x00110004)
#define HWIO_TLMM_ETM_MODE_RMSK                                                                    0x3
#define HWIO_TLMM_ETM_MODE_IN          \
        in_dword_masked(HWIO_TLMM_ETM_MODE_ADDR, HWIO_TLMM_ETM_MODE_RMSK)
#define HWIO_TLMM_ETM_MODE_INM(m)      \
        in_dword_masked(HWIO_TLMM_ETM_MODE_ADDR, m)
#define HWIO_TLMM_ETM_MODE_OUT(v)      \
        out_dword(HWIO_TLMM_ETM_MODE_ADDR,v)
#define HWIO_TLMM_ETM_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_ETM_MODE_ADDR,m,v,HWIO_TLMM_ETM_MODE_IN)
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC2_BMSK                                                    0x3
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC2_SHFT                                                    0x0
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC2_MODE0_FVAL                                              0x0
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC2_MODE1_FVAL                                              0x1
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC2_MODE2_FVAL                                              0x2
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC2_MODE3_FVAL                                              0x3

#define HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR                                                      (TLMM_CSR_REG_BASE      + 0x00110008)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_PHYS                                                      (TLMM_CSR_REG_BASE_PHYS + 0x00110008)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_OFFS                                                      (TLMM_CSR_REG_BASE_OFFS + 0x00110008)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_RMSK                                                             0x7
#define HWIO_TLMM_DBG_BUS_OUT_SEL_IN          \
        in_dword_masked(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR, HWIO_TLMM_DBG_BUS_OUT_SEL_RMSK)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_INM(m)      \
        in_dword_masked(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR, m)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_OUT(v)      \
        out_dword(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR,v)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR,m,v,HWIO_TLMM_DBG_BUS_OUT_SEL_IN)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_QDSS_ETM_BYTE_SHIFT_BMSK                                         0x4
#define HWIO_TLMM_DBG_BUS_OUT_SEL_QDSS_ETM_BYTE_SHIFT_SHFT                                         0x2
#define HWIO_TLMM_DBG_BUS_OUT_SEL_COPY_SEL_BMSK                                                    0x3
#define HWIO_TLMM_DBG_BUS_OUT_SEL_COPY_SEL_SHFT                                                    0x0
#define HWIO_TLMM_DBG_BUS_OUT_SEL_COPY_SEL_COPY_A_FVAL                                             0x0
#define HWIO_TLMM_DBG_BUS_OUT_SEL_COPY_SEL_COPY_B_FVAL                                             0x1
#define HWIO_TLMM_DBG_BUS_OUT_SEL_COPY_SEL_COPY_C_FVAL                                             0x2
#define HWIO_TLMM_DBG_BUS_OUT_SEL_COPY_SEL_COPY_D_FVAL                                             0x3

#define HWIO_TLMM_CHIP_MODE_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x0011000c)
#define HWIO_TLMM_CHIP_MODE_PHYS                                                            (TLMM_CSR_REG_BASE_PHYS + 0x0011000c)
#define HWIO_TLMM_CHIP_MODE_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x0011000c)
#define HWIO_TLMM_CHIP_MODE_RMSK                                                                   0x3
#define HWIO_TLMM_CHIP_MODE_IN          \
        in_dword_masked(HWIO_TLMM_CHIP_MODE_ADDR, HWIO_TLMM_CHIP_MODE_RMSK)
#define HWIO_TLMM_CHIP_MODE_INM(m)      \
        in_dword_masked(HWIO_TLMM_CHIP_MODE_ADDR, m)
#define HWIO_TLMM_CHIP_MODE_MODE1_PIN_BMSK                                                         0x2
#define HWIO_TLMM_CHIP_MODE_MODE1_PIN_SHFT                                                         0x1
#define HWIO_TLMM_CHIP_MODE_MODE0_PIN_BMSK                                                         0x1
#define HWIO_TLMM_CHIP_MODE_MODE0_PIN_SHFT                                                         0x0

#define HWIO_TLMM_SPARE_ADDR                                                                (TLMM_CSR_REG_BASE      + 0x0010e000)
#define HWIO_TLMM_SPARE_PHYS                                                                (TLMM_CSR_REG_BASE_PHYS + 0x0010e000)
#define HWIO_TLMM_SPARE_OFFS                                                                (TLMM_CSR_REG_BASE_OFFS + 0x0010e000)
#define HWIO_TLMM_SPARE_RMSK                                                                0xffffffff
#define HWIO_TLMM_SPARE_IN          \
        in_dword_masked(HWIO_TLMM_SPARE_ADDR, HWIO_TLMM_SPARE_RMSK)
#define HWIO_TLMM_SPARE_INM(m)      \
        in_dword_masked(HWIO_TLMM_SPARE_ADDR, m)
#define HWIO_TLMM_SPARE_OUT(v)      \
        out_dword(HWIO_TLMM_SPARE_ADDR,v)
#define HWIO_TLMM_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SPARE_ADDR,m,v,HWIO_TLMM_SPARE_IN)
#define HWIO_TLMM_SPARE_SPARE_BMSK                                                          0xffffff00
#define HWIO_TLMM_SPARE_SPARE_SHFT                                                                 0x8
#define HWIO_TLMM_SPARE_MISC_BMSK                                                                 0xff
#define HWIO_TLMM_SPARE_MISC_SHFT                                                                  0x0

#define HWIO_SPARE1_ADDR                                                                    (TLMM_CSR_REG_BASE      + 0x0011001c)
#define HWIO_SPARE1_PHYS                                                                    (TLMM_CSR_REG_BASE_PHYS + 0x0011001c)
#define HWIO_SPARE1_OFFS                                                                    (TLMM_CSR_REG_BASE_OFFS + 0x0011001c)
#define HWIO_SPARE1_RMSK                                                                    0xffffffff
#define HWIO_SPARE1_IN          \
        in_dword_masked(HWIO_SPARE1_ADDR, HWIO_SPARE1_RMSK)
#define HWIO_SPARE1_INM(m)      \
        in_dword_masked(HWIO_SPARE1_ADDR, m)
#define HWIO_SPARE1_OUT(v)      \
        out_dword(HWIO_SPARE1_ADDR,v)
#define HWIO_SPARE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPARE1_ADDR,m,v,HWIO_SPARE1_IN)
#define HWIO_SPARE1_MISC1_BMSK                                                              0xffffffff
#define HWIO_SPARE1_MISC1_SHFT                                                                     0x0

#define HWIO_SPARE2_ADDR                                                                    (TLMM_CSR_REG_BASE      + 0x00110020)
#define HWIO_SPARE2_PHYS                                                                    (TLMM_CSR_REG_BASE_PHYS + 0x00110020)
#define HWIO_SPARE2_OFFS                                                                    (TLMM_CSR_REG_BASE_OFFS + 0x00110020)
#define HWIO_SPARE2_RMSK                                                                    0xffffffff
#define HWIO_SPARE2_IN          \
        in_dword_masked(HWIO_SPARE2_ADDR, HWIO_SPARE2_RMSK)
#define HWIO_SPARE2_INM(m)      \
        in_dword_masked(HWIO_SPARE2_ADDR, m)
#define HWIO_SPARE2_OUT(v)      \
        out_dword(HWIO_SPARE2_ADDR,v)
#define HWIO_SPARE2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPARE2_ADDR,m,v,HWIO_SPARE2_IN)
#define HWIO_SPARE2_MISC2_BMSK                                                              0xffffffff
#define HWIO_SPARE2_MISC2_SHFT                                                                     0x0

#define HWIO_TLMM_HW_REVISION_NUMBER_ADDR                                                   (TLMM_CSR_REG_BASE      + 0x00110010)
#define HWIO_TLMM_HW_REVISION_NUMBER_PHYS                                                   (TLMM_CSR_REG_BASE_PHYS + 0x00110010)
#define HWIO_TLMM_HW_REVISION_NUMBER_OFFS                                                   (TLMM_CSR_REG_BASE_OFFS + 0x00110010)
#define HWIO_TLMM_HW_REVISION_NUMBER_RMSK                                                   0xffffffff
#define HWIO_TLMM_HW_REVISION_NUMBER_IN          \
        in_dword_masked(HWIO_TLMM_HW_REVISION_NUMBER_ADDR, HWIO_TLMM_HW_REVISION_NUMBER_RMSK)
#define HWIO_TLMM_HW_REVISION_NUMBER_INM(m)      \
        in_dword_masked(HWIO_TLMM_HW_REVISION_NUMBER_ADDR, m)
#define HWIO_TLMM_HW_REVISION_NUMBER_VERSION_ID_BMSK                                        0xf0000000
#define HWIO_TLMM_HW_REVISION_NUMBER_VERSION_ID_SHFT                                              0x1c
#define HWIO_TLMM_HW_REVISION_NUMBER_VERSION_ID_FIRST_TAPE_OUT_FVAL                                0x0
#define HWIO_TLMM_HW_REVISION_NUMBER_PARTNUM_BMSK                                            0xffff000
#define HWIO_TLMM_HW_REVISION_NUMBER_PARTNUM_SHFT                                                  0xc
#define HWIO_TLMM_HW_REVISION_NUMBER_QUALCOMM_MFG_ID_BMSK                                        0xffe
#define HWIO_TLMM_HW_REVISION_NUMBER_QUALCOMM_MFG_ID_SHFT                                          0x1
#define HWIO_TLMM_HW_REVISION_NUMBER_START_BIT_BMSK                                                0x1
#define HWIO_TLMM_HW_REVISION_NUMBER_START_BIT_SHFT                                                0x0

#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR                                                (TLMM_CSR_REG_BASE      + 0x00110014)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_PHYS                                                (TLMM_CSR_REG_BASE_PHYS + 0x00110014)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_OFFS                                                (TLMM_CSR_REG_BASE_OFFS + 0x00110014)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_RMSK                                                       0x1
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_IN          \
        in_dword_masked(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR, HWIO_TLMM_PERIPH_CHAR_TEST_MODE_RMSK)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_INM(m)      \
        in_dword_masked(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR, m)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_OUT(v)      \
        out_dword(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR,v)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR,m,v,HWIO_TLMM_PERIPH_CHAR_TEST_MODE_IN)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_CHAR_TEST_MODE_BMSK                                        0x1
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_CHAR_TEST_MODE_SHFT                                        0x0
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_CHAR_TEST_MODE_DISABLE_FVAL                                0x0
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_CHAR_TEST_MODE_ENABLE_FVAL                                 0x1

#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR                                                   (TLMM_CSR_REG_BASE      + 0x00111000)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_PHYS                                                   (TLMM_CSR_REG_BASE_PHYS + 0x00111000)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_OFFS                                                   (TLMM_CSR_REG_BASE_OFFS + 0x00111000)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_RMSK                                                   0xffffffff
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_IN          \
        in_dword_masked(HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR, HWIO_TLMM_EBI2_EMMC_GPIO_CFG_RMSK)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_INM(m)      \
        in_dword_masked(HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR, m)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_OUT(v)      \
        out_dword(HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR,v)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR,m,v,HWIO_TLMM_EBI2_EMMC_GPIO_CFG_IN)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_BUSY_HDRV_BMSK                                    0xe0000000
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_BUSY_HDRV_SHFT                                          0x1d
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_BUSY_PULL_BMSK                                    0x18000000
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_BUSY_PULL_SHFT                                          0x1b
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_WE_HDRV_BMSK                                       0x7000000
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_WE_HDRV_SHFT                                            0x18
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_WE_PULL_BMSK                                        0xc00000
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_WE_PULL_SHFT                                            0x16
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_CLE_HDRV_BMSK                                       0x380000
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_CLE_HDRV_SHFT                                           0x13
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_CLE_PULL_BMSK                                        0x60000
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_CLE_PULL_SHFT                                           0x11
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_ALE_HDRV_BMSK                                        0x1c000
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_ALE_HDRV_SHFT                                            0xe
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_ALE_PULL_BMSK                                         0x3000
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_ALE_PULL_SHFT                                            0xc
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_OE_HDRV_BMSK                                           0xe00
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_OE_HDRV_SHFT                                             0x9
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_OE_PULL_BMSK                                           0x180
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_OE_PULL_SHFT                                             0x7
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_CS_HDRV_BMSK                                            0x70
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_CS_HDRV_SHFT                                             0x4
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_CS_PULL_BMSK                                             0xc
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_CS_PULL_SHFT                                             0x2
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EMMC_BOOT_SELECT_BMSK                                         0x2
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EMMC_BOOT_SELECT_SHFT                                         0x1
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_BOOT_SELECT_BMSK                                         0x1
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_BOOT_SELECT_SHFT                                         0x0

#define HWIO_TLMM_PBL_DEBUG_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x00110018)
#define HWIO_TLMM_PBL_DEBUG_PHYS                                                            (TLMM_CSR_REG_BASE_PHYS + 0x00110018)
#define HWIO_TLMM_PBL_DEBUG_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x00110018)
#define HWIO_TLMM_PBL_DEBUG_RMSK                                                                   0x1
#define HWIO_TLMM_PBL_DEBUG_IN          \
        in_dword_masked(HWIO_TLMM_PBL_DEBUG_ADDR, HWIO_TLMM_PBL_DEBUG_RMSK)
#define HWIO_TLMM_PBL_DEBUG_INM(m)      \
        in_dword_masked(HWIO_TLMM_PBL_DEBUG_ADDR, m)
#define HWIO_TLMM_PBL_DEBUG_OUT(v)      \
        out_dword(HWIO_TLMM_PBL_DEBUG_ADDR,v)
#define HWIO_TLMM_PBL_DEBUG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_PBL_DEBUG_ADDR,m,v,HWIO_TLMM_PBL_DEBUG_IN)
#define HWIO_TLMM_PBL_DEBUG_PBL_DEBUG_BMSK                                                         0x1
#define HWIO_TLMM_PBL_DEBUG_PBL_DEBUG_SHFT                                                         0x0

#define HWIO_TLMM_RFFE_CTL_ADDR                                                             (TLMM_CSR_REG_BASE      + 0x00108000)
#define HWIO_TLMM_RFFE_CTL_PHYS                                                             (TLMM_CSR_REG_BASE_PHYS + 0x00108000)
#define HWIO_TLMM_RFFE_CTL_OFFS                                                             (TLMM_CSR_REG_BASE_OFFS + 0x00108000)
#define HWIO_TLMM_RFFE_CTL_RMSK                                                                0xfffff
#define HWIO_TLMM_RFFE_CTL_IN          \
        in_dword_masked(HWIO_TLMM_RFFE_CTL_ADDR, HWIO_TLMM_RFFE_CTL_RMSK)
#define HWIO_TLMM_RFFE_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_RFFE_CTL_ADDR, m)
#define HWIO_TLMM_RFFE_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_RFFE_CTL_ADDR,v)
#define HWIO_TLMM_RFFE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_RFFE_CTL_ADDR,m,v,HWIO_TLMM_RFFE_CTL_IN)
#define HWIO_TLMM_RFFE_CTL_RFFE5_DATA_SR_CTL_EN_BMSK                                           0xc0000
#define HWIO_TLMM_RFFE_CTL_RFFE5_DATA_SR_CTL_EN_SHFT                                              0x12
#define HWIO_TLMM_RFFE_CTL_RFFE5_CLK_SR_CTL_EN_BMSK                                            0x30000
#define HWIO_TLMM_RFFE_CTL_RFFE5_CLK_SR_CTL_EN_SHFT                                               0x10
#define HWIO_TLMM_RFFE_CTL_RFFE4_DATA_SR_CTL_EN_BMSK                                            0xc000
#define HWIO_TLMM_RFFE_CTL_RFFE4_DATA_SR_CTL_EN_SHFT                                               0xe
#define HWIO_TLMM_RFFE_CTL_RFFE4_CLK_SR_CTL_EN_BMSK                                             0x3000
#define HWIO_TLMM_RFFE_CTL_RFFE4_CLK_SR_CTL_EN_SHFT                                                0xc
#define HWIO_TLMM_RFFE_CTL_RFFE3_DATA_SR_CTL_EN_BMSK                                             0xc00
#define HWIO_TLMM_RFFE_CTL_RFFE3_DATA_SR_CTL_EN_SHFT                                               0xa
#define HWIO_TLMM_RFFE_CTL_RFFE3_CLK_SR_CTL_EN_BMSK                                              0x300
#define HWIO_TLMM_RFFE_CTL_RFFE3_CLK_SR_CTL_EN_SHFT                                                0x8
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SR_CTL_EN_BMSK                                              0xc0
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SR_CTL_EN_SHFT                                               0x6
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SR_CTL_EN_BMSK                                               0x30
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SR_CTL_EN_SHFT                                                0x4
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SR_CTL_EN_BMSK                                               0xc
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SR_CTL_EN_SHFT                                               0x2
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SR_CTL_EN_BMSK                                                0x3
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SR_CTL_EN_SHFT                                                0x0

#define HWIO_TLMM_RESOUT_HDRV_CTL_ADDR                                                      (TLMM_CSR_REG_BASE      + 0x0010d000)
#define HWIO_TLMM_RESOUT_HDRV_CTL_PHYS                                                      (TLMM_CSR_REG_BASE_PHYS + 0x0010d000)
#define HWIO_TLMM_RESOUT_HDRV_CTL_OFFS                                                      (TLMM_CSR_REG_BASE_OFFS + 0x0010d000)
#define HWIO_TLMM_RESOUT_HDRV_CTL_RMSK                                                             0x7
#define HWIO_TLMM_RESOUT_HDRV_CTL_IN          \
        in_dword_masked(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR, HWIO_TLMM_RESOUT_HDRV_CTL_RMSK)
#define HWIO_TLMM_RESOUT_HDRV_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR, m)
#define HWIO_TLMM_RESOUT_HDRV_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR,v)
#define HWIO_TLMM_RESOUT_HDRV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR,m,v,HWIO_TLMM_RESOUT_HDRV_CTL_IN)
#define HWIO_TLMM_RESOUT_HDRV_CTL_RESOUT_N_HDRV_BMSK                                               0x7
#define HWIO_TLMM_RESOUT_HDRV_CTL_RESOUT_N_HDRV_SHFT                                               0x0

#define HWIO_TLMM_JTAG_HDRV_CTL_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x0010c000)
#define HWIO_TLMM_JTAG_HDRV_CTL_PHYS                                                        (TLMM_CSR_REG_BASE_PHYS + 0x0010c000)
#define HWIO_TLMM_JTAG_HDRV_CTL_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x0010c000)
#define HWIO_TLMM_JTAG_HDRV_CTL_RMSK                                                             0x3ff
#define HWIO_TLMM_JTAG_HDRV_CTL_IN          \
        in_dword_masked(HWIO_TLMM_JTAG_HDRV_CTL_ADDR, HWIO_TLMM_JTAG_HDRV_CTL_RMSK)
#define HWIO_TLMM_JTAG_HDRV_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_JTAG_HDRV_CTL_ADDR, m)
#define HWIO_TLMM_JTAG_HDRV_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_JTAG_HDRV_CTL_ADDR,v)
#define HWIO_TLMM_JTAG_HDRV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_JTAG_HDRV_CTL_ADDR,m,v,HWIO_TLMM_JTAG_HDRV_CTL_IN)
#define HWIO_TLMM_JTAG_HDRV_CTL_TDI_PULL_BMSK                                                    0x300
#define HWIO_TLMM_JTAG_HDRV_CTL_TDI_PULL_SHFT                                                      0x8
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_PULL_BMSK                                                     0xc0
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_PULL_SHFT                                                      0x6
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_HDRV_BMSK                                                     0x38
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_HDRV_SHFT                                                      0x3
#define HWIO_TLMM_JTAG_HDRV_CTL_TDO_HDRV_BMSK                                                      0x7
#define HWIO_TLMM_JTAG_HDRV_CTL_TDO_HDRV_SHFT                                                      0x0

#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR                                                   (TLMM_CSR_REG_BASE      + 0x0010b000)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PHYS                                                   (TLMM_CSR_REG_BASE_PHYS + 0x0010b000)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_OFFS                                                   (TLMM_CSR_REG_BASE_OFFS + 0x0010b000)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_RMSK                                                      0xfffff
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR, HWIO_TLMM_PMIC_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_PMIC_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_SR_CTL_EN_BMSK                             0xc0000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_SR_CTL_EN_SHFT                                0x12
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_SR_CTL_EN_BMSK                              0x30000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_SR_CTL_EN_SHFT                                 0x10
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_PULL_BMSK                                   0xc000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_PULL_SHFT                                      0xe
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_PULL_BMSK                                    0x3000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_PULL_SHFT                                       0xc
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PSHOLD_HDRV_BMSK                                            0xe00
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PSHOLD_HDRV_SHFT                                              0x9
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_CXO_EN_HDRV_BMSK                                            0x1c0
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_CXO_EN_HDRV_SHFT                                              0x6
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_HDRV_BMSK                                     0x38
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_HDRV_SHFT                                      0x3
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_HDRV_BMSK                                       0x7
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_HDRV_SHFT                                       0x0

#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR                                                   (TLMM_CSR_REG_BASE      + 0x0010a000)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_PHYS                                                   (TLMM_CSR_REG_BASE_PHYS + 0x0010a000)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OFFS                                                   (TLMM_CSR_REG_BASE_OFFS + 0x0010a000)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_RMSK                                                      0xfffff
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR, HWIO_TLMM_SDC1_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_SDC1_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_EBI2_DATA_HDRV_BMSK                                       0xe0000
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_EBI2_DATA_HDRV_SHFT                                          0x11
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_EBI2_DATA_PULL_BMSK                                       0x18000
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_EBI2_DATA_PULL_SHFT                                           0xf
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_PULL_BMSK                                         0x6000
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_PULL_SHFT                                            0xd
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_PULL_BMSK                                         0x1800
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_PULL_SHFT                                            0xb
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_PULL_BMSK                                         0x600
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_PULL_SHFT                                           0x9
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_HDRV_BMSK                                          0x1c0
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_HDRV_SHFT                                            0x6
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_HDRV_BMSK                                           0x38
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_HDRV_SHFT                                            0x3
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_HDRV_BMSK                                           0x7
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_HDRV_SHFT                                           0x0

#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR                                                   (TLMM_CSR_REG_BASE      + 0x00109000)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_PHYS                                                   (TLMM_CSR_REG_BASE_PHYS + 0x00109000)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_OFFS                                                   (TLMM_CSR_REG_BASE_OFFS + 0x00109000)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_RMSK                                                       0xffff
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR, HWIO_TLMM_SDC2_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_SDC2_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_PULL_BMSK                                         0xc000
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_PULL_SHFT                                            0xe
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_HYS_CTL_BMSK                                          0x2000
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_HYS_CTL_SHFT                                             0xd
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_HYS_CTL_DISABLE_FVAL                                     0x0
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_HYS_CTL_ENABLE_FVAL                                      0x1
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_PULL_BMSK                                         0x1800
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_PULL_SHFT                                            0xb
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_PULL_BMSK                                         0x600
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_PULL_SHFT                                           0x9
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_HDRV_BMSK                                          0x1c0
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_HDRV_SHFT                                            0x6
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_HDRV_BMSK                                           0x38
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_HDRV_SHFT                                            0x3
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_HDRV_BMSK                                           0x7
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_HDRV_SHFT                                           0x0

#define HWIO_TLMM_MODE_PULL_CTL_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x00107000)
#define HWIO_TLMM_MODE_PULL_CTL_PHYS                                                        (TLMM_CSR_REG_BASE_PHYS + 0x00107000)
#define HWIO_TLMM_MODE_PULL_CTL_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x00107000)
#define HWIO_TLMM_MODE_PULL_CTL_RMSK                                                               0xf
#define HWIO_TLMM_MODE_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_MODE_PULL_CTL_ADDR, HWIO_TLMM_MODE_PULL_CTL_RMSK)
#define HWIO_TLMM_MODE_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_MODE_PULL_CTL_ADDR, m)
#define HWIO_TLMM_MODE_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_MODE_PULL_CTL_ADDR,v)
#define HWIO_TLMM_MODE_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_MODE_PULL_CTL_ADDR,m,v,HWIO_TLMM_MODE_PULL_CTL_IN)
#define HWIO_TLMM_MODE_PULL_CTL_MODE_1_PULL_BMSK                                                   0xc
#define HWIO_TLMM_MODE_PULL_CTL_MODE_1_PULL_SHFT                                                   0x2
#define HWIO_TLMM_MODE_PULL_CTL_MODE_0_PULL_BMSK                                                   0x3
#define HWIO_TLMM_MODE_PULL_CTL_MODE_0_PULL_SHFT                                                   0x0

#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0019c000)
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_PHYS                                        (TLMM_CSR_REG_BASE_PHYS + 0x0019c000)
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_OFFS                                        (TLMM_CSR_REG_BASE_OFFS + 0x0019c000)
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_RMSK                                        0xffffffff
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_ADDR, HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_RMSK)
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_ADDR,m,v,HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_IN)
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_QDSD_HDRV_PULL_DEBUG_GPIO_REG_BMSK          0xffffffff
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_QDSD_HDRV_PULL_DEBUG_GPIO_REG_SHFT                 0x0

#define HWIO_TLMM_QDSD_BOOT_CTL_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x0019d000)
#define HWIO_TLMM_QDSD_BOOT_CTL_PHYS                                                        (TLMM_CSR_REG_BASE_PHYS + 0x0019d000)
#define HWIO_TLMM_QDSD_BOOT_CTL_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x0019d000)
#define HWIO_TLMM_QDSD_BOOT_CTL_RMSK                                                            0xffff
#define HWIO_TLMM_QDSD_BOOT_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_BOOT_CTL_ADDR, HWIO_TLMM_QDSD_BOOT_CTL_RMSK)
#define HWIO_TLMM_QDSD_BOOT_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_BOOT_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_BOOT_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_BOOT_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_BOOT_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_BOOT_CTL_ADDR,m,v,HWIO_TLMM_QDSD_BOOT_CTL_IN)
#define HWIO_TLMM_QDSD_BOOT_CTL_QDSD_BOOT_REG_BMSK                                              0xffff
#define HWIO_TLMM_QDSD_BOOT_CTL_QDSD_BOOT_REG_SHFT                                                 0x0

#define HWIO_TLMM_QDSD_CONFIG_CTL_ADDR                                                      (TLMM_CSR_REG_BASE      + 0x0019e000)
#define HWIO_TLMM_QDSD_CONFIG_CTL_PHYS                                                      (TLMM_CSR_REG_BASE_PHYS + 0x0019e000)
#define HWIO_TLMM_QDSD_CONFIG_CTL_OFFS                                                      (TLMM_CSR_REG_BASE_OFFS + 0x0019e000)
#define HWIO_TLMM_QDSD_CONFIG_CTL_RMSK                                                      0xffffffff
#define HWIO_TLMM_QDSD_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_CONFIG_CTL_ADDR, HWIO_TLMM_QDSD_CONFIG_CTL_RMSK)
#define HWIO_TLMM_QDSD_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_CONFIG_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_CONFIG_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_CONFIG_CTL_ADDR,m,v,HWIO_TLMM_QDSD_CONFIG_CTL_IN)
#define HWIO_TLMM_QDSD_CONFIG_CTL_QDSD_CONFIG_REG_BMSK                                      0xffffffff
#define HWIO_TLMM_QDSD_CONFIG_CTL_QDSD_CONFIG_REG_SHFT                                             0x0

#define HWIO_TLMM_QDSD_STATUS_CTL_ADDR                                                      (TLMM_CSR_REG_BASE      + 0x0019f000)
#define HWIO_TLMM_QDSD_STATUS_CTL_PHYS                                                      (TLMM_CSR_REG_BASE_PHYS + 0x0019f000)
#define HWIO_TLMM_QDSD_STATUS_CTL_OFFS                                                      (TLMM_CSR_REG_BASE_OFFS + 0x0019f000)
#define HWIO_TLMM_QDSD_STATUS_CTL_RMSK                                                      0xffffffff
#define HWIO_TLMM_QDSD_STATUS_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_STATUS_CTL_ADDR, HWIO_TLMM_QDSD_STATUS_CTL_RMSK)
#define HWIO_TLMM_QDSD_STATUS_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_STATUS_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_STATUS_CTL_QDSD_STATUS_REG_BMSK                                      0xffffffff
#define HWIO_TLMM_QDSD_STATUS_CTL_QDSD_STATUS_REG_SHFT                                             0x0

#define HWIO_TLMM_QDSD_HYS_CTL_ADDR                                                         (TLMM_CSR_REG_BASE      + 0x001a7000)
#define HWIO_TLMM_QDSD_HYS_CTL_PHYS                                                         (TLMM_CSR_REG_BASE_PHYS + 0x001a7000)
#define HWIO_TLMM_QDSD_HYS_CTL_OFFS                                                         (TLMM_CSR_REG_BASE_OFFS + 0x001a7000)
#define HWIO_TLMM_QDSD_HYS_CTL_RMSK                                                                0x1
#define HWIO_TLMM_QDSD_HYS_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_HYS_CTL_ADDR, HWIO_TLMM_QDSD_HYS_CTL_RMSK)
#define HWIO_TLMM_QDSD_HYS_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_HYS_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_HYS_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_HYS_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_HYS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_HYS_CTL_ADDR,m,v,HWIO_TLMM_QDSD_HYS_CTL_IN)
#define HWIO_TLMM_QDSD_HYS_CTL_QDSD_HYS_CTL_BMSK                                                   0x1
#define HWIO_TLMM_QDSD_HYS_CTL_QDSD_HYS_CTL_SHFT                                                   0x0
#define HWIO_TLMM_QDSD_HYS_CTL_QDSD_HYS_CTL_DISABLE_FVAL                                           0x0
#define HWIO_TLMM_QDSD_HYS_CTL_QDSD_HYS_CTL_ENABLE_FVAL                                            0x1

#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_ADDR                                             (TLMM_CSR_REG_BASE      + 0x001a0000)
#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_PHYS                                             (TLMM_CSR_REG_BASE_PHYS + 0x001a0000)
#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_OFFS                                             (TLMM_CSR_REG_BASE_OFFS + 0x001a0000)
#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_RMSK                                             0x3fffffff
#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_ADDR, HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_QDSD_DEBUG_HDRV_PULL_REG_BMSK                    0x3fffffff
#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_QDSD_DEBUG_HDRV_PULL_REG_SHFT                           0x0

#define HWIO_TLMM_QDSD_GPIO_CTL_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x001a1000)
#define HWIO_TLMM_QDSD_GPIO_CTL_PHYS                                                        (TLMM_CSR_REG_BASE_PHYS + 0x001a1000)
#define HWIO_TLMM_QDSD_GPIO_CTL_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x001a1000)
#define HWIO_TLMM_QDSD_GPIO_CTL_RMSK                                                            0xffff
#define HWIO_TLMM_QDSD_GPIO_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_GPIO_CTL_ADDR, HWIO_TLMM_QDSD_GPIO_CTL_RMSK)
#define HWIO_TLMM_QDSD_GPIO_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_GPIO_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_GPIO_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_GPIO_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_GPIO_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_GPIO_CTL_ADDR,m,v,HWIO_TLMM_QDSD_GPIO_CTL_IN)
#define HWIO_TLMM_QDSD_GPIO_CTL_QDSD_GPIO_REG_BMSK                                              0xffff
#define HWIO_TLMM_QDSD_GPIO_CTL_QDSD_GPIO_REG_SHFT                                                 0x0

#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_ADDR                                                 (TLMM_CSR_REG_BASE      + 0x001a2000)
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_PHYS                                                 (TLMM_CSR_REG_BASE_PHYS + 0x001a2000)
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_OFFS                                                 (TLMM_CSR_REG_BASE_OFFS + 0x001a2000)
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_RMSK                                                     0xffff
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_INTR_ENABLE_CTL_ADDR, HWIO_TLMM_QDSD_INTR_ENABLE_CTL_RMSK)
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_INTR_ENABLE_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_INTR_ENABLE_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_INTR_ENABLE_CTL_ADDR,m,v,HWIO_TLMM_QDSD_INTR_ENABLE_CTL_IN)
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_QDSD_INTR_ENABLE_REG_BMSK                                0xffff
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_QDSD_INTR_ENABLE_REG_SHFT                                   0x0

#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_ADDR                                                  (TLMM_CSR_REG_BASE      + 0x001a3000)
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_PHYS                                                  (TLMM_CSR_REG_BASE_PHYS + 0x001a3000)
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_OFFS                                                  (TLMM_CSR_REG_BASE_OFFS + 0x001a3000)
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_RMSK                                                      0xffff
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_INTR_CLEAR_CTL_ADDR, HWIO_TLMM_QDSD_INTR_CLEAR_CTL_RMSK)
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_INTR_CLEAR_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_INTR_CLEAR_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_INTR_CLEAR_CTL_ADDR,m,v,HWIO_TLMM_QDSD_INTR_CLEAR_CTL_IN)
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_QDSD_INTR_CLEAR_REG_BMSK                                  0xffff
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_QDSD_INTR_CLEAR_REG_SHFT                                     0x0

#define HWIO_TLMM_QDSD_SPARE1_CTL_ADDR                                                      (TLMM_CSR_REG_BASE      + 0x001a5000)
#define HWIO_TLMM_QDSD_SPARE1_CTL_PHYS                                                      (TLMM_CSR_REG_BASE_PHYS + 0x001a5000)
#define HWIO_TLMM_QDSD_SPARE1_CTL_OFFS                                                      (TLMM_CSR_REG_BASE_OFFS + 0x001a5000)
#define HWIO_TLMM_QDSD_SPARE1_CTL_RMSK                                                      0xffffffff
#define HWIO_TLMM_QDSD_SPARE1_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_SPARE1_CTL_ADDR, HWIO_TLMM_QDSD_SPARE1_CTL_RMSK)
#define HWIO_TLMM_QDSD_SPARE1_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_SPARE1_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_SPARE1_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_SPARE1_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_SPARE1_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_SPARE1_CTL_ADDR,m,v,HWIO_TLMM_QDSD_SPARE1_CTL_IN)
#define HWIO_TLMM_QDSD_SPARE1_CTL_QDSD_SPARE1_REG_BMSK                                      0xffffffff
#define HWIO_TLMM_QDSD_SPARE1_CTL_QDSD_SPARE1_REG_SHFT                                             0x0

#define HWIO_TLMM_QDSD_SPARE2_CTL_ADDR                                                      (TLMM_CSR_REG_BASE      + 0x001a6000)
#define HWIO_TLMM_QDSD_SPARE2_CTL_PHYS                                                      (TLMM_CSR_REG_BASE_PHYS + 0x001a6000)
#define HWIO_TLMM_QDSD_SPARE2_CTL_OFFS                                                      (TLMM_CSR_REG_BASE_OFFS + 0x001a6000)
#define HWIO_TLMM_QDSD_SPARE2_CTL_RMSK                                                      0xffffffff
#define HWIO_TLMM_QDSD_SPARE2_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_SPARE2_CTL_ADDR, HWIO_TLMM_QDSD_SPARE2_CTL_RMSK)
#define HWIO_TLMM_QDSD_SPARE2_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_SPARE2_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_SPARE2_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_SPARE2_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_SPARE2_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_SPARE2_CTL_ADDR,m,v,HWIO_TLMM_QDSD_SPARE2_CTL_IN)
#define HWIO_TLMM_QDSD_SPARE2_CTL_QDSD_SPARE2_REG_BMSK                                      0xffffffff
#define HWIO_TLMM_QDSD_SPARE2_CTL_QDSD_SPARE2_REG_SHFT                                             0x0

#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_ADDR                                               (TLMM_CSR_REG_BASE      + 0x001a4000)
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_PHYS                                               (TLMM_CSR_REG_BASE_PHYS + 0x001a4000)
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_OFFS                                               (TLMM_CSR_REG_BASE_OFFS + 0x001a4000)
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_RMSK                                               0xffffffff
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_ADDR, HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_RMSK)
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_ADDR,m,v,HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_IN)
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_QDSD_TIMEOUT_VALUE_REG_BMSK                        0xffffffff
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_QDSD_TIMEOUT_VALUE_REG_SHFT                               0x0

#define HWIO_TLMM_GPIO_HS_I2C_CTL_ADDR                                                      (TLMM_CSR_REG_BASE      + 0x0010f000)
#define HWIO_TLMM_GPIO_HS_I2C_CTL_PHYS                                                      (TLMM_CSR_REG_BASE_PHYS + 0x0010f000)
#define HWIO_TLMM_GPIO_HS_I2C_CTL_OFFS                                                      (TLMM_CSR_REG_BASE_OFFS + 0x0010f000)
#define HWIO_TLMM_GPIO_HS_I2C_CTL_RMSK                                                             0x3
#define HWIO_TLMM_GPIO_HS_I2C_CTL_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_HS_I2C_CTL_ADDR, HWIO_TLMM_GPIO_HS_I2C_CTL_RMSK)
#define HWIO_TLMM_GPIO_HS_I2C_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_HS_I2C_CTL_ADDR, m)
#define HWIO_TLMM_GPIO_HS_I2C_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_HS_I2C_CTL_ADDR,v)
#define HWIO_TLMM_GPIO_HS_I2C_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_HS_I2C_CTL_ADDR,m,v,HWIO_TLMM_GPIO_HS_I2C_CTL_IN)
#define HWIO_TLMM_GPIO_HS_I2C_CTL_MODE_BMSK                                                        0x3
#define HWIO_TLMM_GPIO_HS_I2C_CTL_MODE_SHFT                                                        0x0
#define HWIO_TLMM_GPIO_HS_I2C_CTL_MODE_GPIO_MODE_FVAL                                              0x0
#define HWIO_TLMM_GPIO_HS_I2C_CTL_MODE_HS_I2C_MODE_FVAL                                            0x1
#define HWIO_TLMM_GPIO_HS_I2C_CTL_MODE_FS_I2C_MODE_FVAL                                            0x2
#define HWIO_TLMM_GPIO_HS_I2C_CTL_MODE_HS_I2C_HIGH_LOAD_MODE_FVAL                                  0x3

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n)                                           (TLMM_CSR_REG_BASE      + 0x00106000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_PHYS(n)                                           (TLMM_CSR_REG_BASE_PHYS + 0x00106000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_OFFS(n)                                           (TLMM_CSR_REG_BASE_OFFS + 0x00106000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_RMSK                                                   0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_MAXn                                                       1
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_POLARITY_BMSK                                          0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_POLARITY_SHFT                                            0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_POLARITY_POLARITY_0_FVAL                                 0x0
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_POLARITY_POLARITY_1_FVAL                                 0x1
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_GPIO_SEL_BMSK                                           0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_GPIO_SEL_SHFT                                            0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n)                                        (TLMM_CSR_REG_BASE      + 0x00105000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_PHYS(n)                                        (TLMM_CSR_REG_BASE_PHYS + 0x00105000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_OFFS(n)                                        (TLMM_CSR_REG_BASE_OFFS + 0x00105000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_RMSK                                                0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_MAXn                                                    9
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_POLARITY_BMSK                                       0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_POLARITY_SHFT                                         0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_POLARITY_POLARITY_0_FVAL                              0x0
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_POLARITY_POLARITY_1_FVAL                              0x1
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_GPIO_SEL_BMSK                                        0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_GPIO_SEL_SHFT                                         0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n)                                        (TLMM_CSR_REG_BASE      + 0x00104000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_PHYS(n)                                        (TLMM_CSR_REG_BASE_PHYS + 0x00104000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_OFFS(n)                                        (TLMM_CSR_REG_BASE_OFFS + 0x00104000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_RMSK                                                0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_MAXn                                                    5
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_POLARITY_BMSK                                       0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_POLARITY_SHFT                                         0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_POLARITY_POLARITY_0_FVAL                              0x0
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_POLARITY_POLARITY_1_FVAL                              0x1
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_GPIO_SEL_BMSK                                        0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_GPIO_SEL_SHFT                                         0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n)                                            (TLMM_CSR_REG_BASE      + 0x00103000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_PHYS(n)                                            (TLMM_CSR_REG_BASE_PHYS + 0x00103000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_OFFS(n)                                            (TLMM_CSR_REG_BASE_OFFS + 0x00103000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_RMSK                                                    0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_MAXn                                                        0
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_POLARITY_BMSK                                           0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_POLARITY_SHFT                                             0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_POLARITY_POLARITY_0_FVAL                                  0x0
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_POLARITY_POLARITY_1_FVAL                                  0x1
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_GPIO_SEL_BMSK                                            0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_GPIO_SEL_SHFT                                             0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(n)                                           (TLMM_CSR_REG_BASE      + 0x00102000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_PHYS(n)                                           (TLMM_CSR_REG_BASE_PHYS + 0x00102000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_OFFS(n)                                           (TLMM_CSR_REG_BASE_OFFS + 0x00102000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_RMSK                                                   0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_MAXn                                                       7
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_POLARITY_BMSK                                          0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_POLARITY_SHFT                                            0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_POLARITY_POLARITY_0_FVAL                                 0x0
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_POLARITY_POLARITY_1_FVAL                                 0x1
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_GPIO_SEL_BMSK                                           0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_GPIO_SEL_SHFT                                            0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n)                                            (TLMM_CSR_REG_BASE      + 0x00101000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_PHYS(n)                                            (TLMM_CSR_REG_BASE_PHYS + 0x00101000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_OFFS(n)                                            (TLMM_CSR_REG_BASE_OFFS + 0x00101000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_RMSK                                                    0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_MAXn                                                        1
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_POLARITY_BMSK                                           0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_POLARITY_SHFT                                             0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_POLARITY_POLARITY_0_FVAL                                  0x0
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_POLARITY_POLARITY_1_FVAL                                  0x1
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_GPIO_SEL_BMSK                                            0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_GPIO_SEL_SHFT                                             0x0

#define HWIO_TLMM_GPIO_OUT_0_ADDR                                                           (TLMM_CSR_REG_BASE      + 0x00200000)
#define HWIO_TLMM_GPIO_OUT_0_PHYS                                                           (TLMM_CSR_REG_BASE_PHYS + 0x00200000)
#define HWIO_TLMM_GPIO_OUT_0_OFFS                                                           (TLMM_CSR_REG_BASE_OFFS + 0x00200000)
#define HWIO_TLMM_GPIO_OUT_0_RMSK                                                           0xffffffff
#define HWIO_TLMM_GPIO_OUT_0_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_0_ADDR, HWIO_TLMM_GPIO_OUT_0_RMSK)
#define HWIO_TLMM_GPIO_OUT_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_0_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_0_ADDR,m,v,HWIO_TLMM_GPIO_OUT_0_IN)
#define HWIO_TLMM_GPIO_OUT_0_GPIO_OUT_BMSK                                                  0xffffffff
#define HWIO_TLMM_GPIO_OUT_0_GPIO_OUT_SHFT                                                         0x0

#define HWIO_TLMM_GPIO_OUT_1_ADDR                                                           (TLMM_CSR_REG_BASE      + 0x00200004)
#define HWIO_TLMM_GPIO_OUT_1_PHYS                                                           (TLMM_CSR_REG_BASE_PHYS + 0x00200004)
#define HWIO_TLMM_GPIO_OUT_1_OFFS                                                           (TLMM_CSR_REG_BASE_OFFS + 0x00200004)
#define HWIO_TLMM_GPIO_OUT_1_RMSK                                                           0xffffffff
#define HWIO_TLMM_GPIO_OUT_1_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_1_ADDR, HWIO_TLMM_GPIO_OUT_1_RMSK)
#define HWIO_TLMM_GPIO_OUT_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_1_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_1_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_1_ADDR,m,v,HWIO_TLMM_GPIO_OUT_1_IN)
#define HWIO_TLMM_GPIO_OUT_1_GPIO_OUT_BMSK                                                  0xffffffff
#define HWIO_TLMM_GPIO_OUT_1_GPIO_OUT_SHFT                                                         0x0

#define HWIO_TLMM_GPIO_OUT_2_ADDR                                                           (TLMM_CSR_REG_BASE      + 0x00200008)
#define HWIO_TLMM_GPIO_OUT_2_PHYS                                                           (TLMM_CSR_REG_BASE_PHYS + 0x00200008)
#define HWIO_TLMM_GPIO_OUT_2_OFFS                                                           (TLMM_CSR_REG_BASE_OFFS + 0x00200008)
#define HWIO_TLMM_GPIO_OUT_2_RMSK                                                           0xffffffff
#define HWIO_TLMM_GPIO_OUT_2_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_2_ADDR, HWIO_TLMM_GPIO_OUT_2_RMSK)
#define HWIO_TLMM_GPIO_OUT_2_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_2_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_2_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_2_ADDR,m,v,HWIO_TLMM_GPIO_OUT_2_IN)
#define HWIO_TLMM_GPIO_OUT_2_GPIO_OUT_BMSK                                                  0xffffffff
#define HWIO_TLMM_GPIO_OUT_2_GPIO_OUT_SHFT                                                         0x0

#define HWIO_TLMM_GPIO_OUT_3_ADDR                                                           (TLMM_CSR_REG_BASE      + 0x0020000c)
#define HWIO_TLMM_GPIO_OUT_3_PHYS                                                           (TLMM_CSR_REG_BASE_PHYS + 0x0020000c)
#define HWIO_TLMM_GPIO_OUT_3_OFFS                                                           (TLMM_CSR_REG_BASE_OFFS + 0x0020000c)
#define HWIO_TLMM_GPIO_OUT_3_RMSK                                                              0x1ffff
#define HWIO_TLMM_GPIO_OUT_3_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_3_ADDR, HWIO_TLMM_GPIO_OUT_3_RMSK)
#define HWIO_TLMM_GPIO_OUT_3_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_3_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_3_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_3_ADDR,m,v,HWIO_TLMM_GPIO_OUT_3_IN)
#define HWIO_TLMM_GPIO_OUT_3_GPIO_OUT_BMSK                                                     0x1ffff
#define HWIO_TLMM_GPIO_OUT_3_GPIO_OUT_SHFT                                                         0x0

#define HWIO_TLMM_GPIO_OUT_CLR_0_ADDR                                                       (TLMM_CSR_REG_BASE      + 0x00200020)
#define HWIO_TLMM_GPIO_OUT_CLR_0_PHYS                                                       (TLMM_CSR_REG_BASE_PHYS + 0x00200020)
#define HWIO_TLMM_GPIO_OUT_CLR_0_OFFS                                                       (TLMM_CSR_REG_BASE_OFFS + 0x00200020)
#define HWIO_TLMM_GPIO_OUT_CLR_0_RMSK                                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_0_GPIO_OUT_CLR_BMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_0_GPIO_OUT_CLR_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_CLR_1_ADDR                                                       (TLMM_CSR_REG_BASE      + 0x00200024)
#define HWIO_TLMM_GPIO_OUT_CLR_1_PHYS                                                       (TLMM_CSR_REG_BASE_PHYS + 0x00200024)
#define HWIO_TLMM_GPIO_OUT_CLR_1_OFFS                                                       (TLMM_CSR_REG_BASE_OFFS + 0x00200024)
#define HWIO_TLMM_GPIO_OUT_CLR_1_RMSK                                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_1_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_1_GPIO_OUT_CLR_BMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_1_GPIO_OUT_CLR_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_CLR_2_ADDR                                                       (TLMM_CSR_REG_BASE      + 0x00200028)
#define HWIO_TLMM_GPIO_OUT_CLR_2_PHYS                                                       (TLMM_CSR_REG_BASE_PHYS + 0x00200028)
#define HWIO_TLMM_GPIO_OUT_CLR_2_OFFS                                                       (TLMM_CSR_REG_BASE_OFFS + 0x00200028)
#define HWIO_TLMM_GPIO_OUT_CLR_2_RMSK                                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_2_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_2_GPIO_OUT_CLR_BMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_2_GPIO_OUT_CLR_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_CLR_3_ADDR                                                       (TLMM_CSR_REG_BASE      + 0x0020002c)
#define HWIO_TLMM_GPIO_OUT_CLR_3_PHYS                                                       (TLMM_CSR_REG_BASE_PHYS + 0x0020002c)
#define HWIO_TLMM_GPIO_OUT_CLR_3_OFFS                                                       (TLMM_CSR_REG_BASE_OFFS + 0x0020002c)
#define HWIO_TLMM_GPIO_OUT_CLR_3_RMSK                                                          0x1ffff
#define HWIO_TLMM_GPIO_OUT_CLR_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_3_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_3_GPIO_OUT_CLR_BMSK                                             0x1ffff
#define HWIO_TLMM_GPIO_OUT_CLR_3_GPIO_OUT_CLR_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_SET_0_ADDR                                                       (TLMM_CSR_REG_BASE      + 0x00200040)
#define HWIO_TLMM_GPIO_OUT_SET_0_PHYS                                                       (TLMM_CSR_REG_BASE_PHYS + 0x00200040)
#define HWIO_TLMM_GPIO_OUT_SET_0_OFFS                                                       (TLMM_CSR_REG_BASE_OFFS + 0x00200040)
#define HWIO_TLMM_GPIO_OUT_SET_0_RMSK                                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_0_GPIO_OUT_SET_BMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_0_GPIO_OUT_SET_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_SET_1_ADDR                                                       (TLMM_CSR_REG_BASE      + 0x00200044)
#define HWIO_TLMM_GPIO_OUT_SET_1_PHYS                                                       (TLMM_CSR_REG_BASE_PHYS + 0x00200044)
#define HWIO_TLMM_GPIO_OUT_SET_1_OFFS                                                       (TLMM_CSR_REG_BASE_OFFS + 0x00200044)
#define HWIO_TLMM_GPIO_OUT_SET_1_RMSK                                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_1_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_1_GPIO_OUT_SET_BMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_1_GPIO_OUT_SET_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_SET_2_ADDR                                                       (TLMM_CSR_REG_BASE      + 0x00200048)
#define HWIO_TLMM_GPIO_OUT_SET_2_PHYS                                                       (TLMM_CSR_REG_BASE_PHYS + 0x00200048)
#define HWIO_TLMM_GPIO_OUT_SET_2_OFFS                                                       (TLMM_CSR_REG_BASE_OFFS + 0x00200048)
#define HWIO_TLMM_GPIO_OUT_SET_2_RMSK                                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_2_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_2_GPIO_OUT_SET_BMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_2_GPIO_OUT_SET_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_SET_3_ADDR                                                       (TLMM_CSR_REG_BASE      + 0x0020004c)
#define HWIO_TLMM_GPIO_OUT_SET_3_PHYS                                                       (TLMM_CSR_REG_BASE_PHYS + 0x0020004c)
#define HWIO_TLMM_GPIO_OUT_SET_3_OFFS                                                       (TLMM_CSR_REG_BASE_OFFS + 0x0020004c)
#define HWIO_TLMM_GPIO_OUT_SET_3_RMSK                                                          0x1ffff
#define HWIO_TLMM_GPIO_OUT_SET_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_3_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_3_GPIO_OUT_SET_BMSK                                             0x1ffff
#define HWIO_TLMM_GPIO_OUT_SET_3_GPIO_OUT_SET_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_IN_0_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x00200060)
#define HWIO_TLMM_GPIO_IN_0_PHYS                                                            (TLMM_CSR_REG_BASE_PHYS + 0x00200060)
#define HWIO_TLMM_GPIO_IN_0_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x00200060)
#define HWIO_TLMM_GPIO_IN_0_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_IN_0_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_0_ADDR, HWIO_TLMM_GPIO_IN_0_RMSK)
#define HWIO_TLMM_GPIO_IN_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_0_ADDR, m)
#define HWIO_TLMM_GPIO_IN_0_GPIO_IN_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_IN_0_GPIO_IN_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_IN_1_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x00200064)
#define HWIO_TLMM_GPIO_IN_1_PHYS                                                            (TLMM_CSR_REG_BASE_PHYS + 0x00200064)
#define HWIO_TLMM_GPIO_IN_1_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x00200064)
#define HWIO_TLMM_GPIO_IN_1_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_IN_1_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_1_ADDR, HWIO_TLMM_GPIO_IN_1_RMSK)
#define HWIO_TLMM_GPIO_IN_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_1_ADDR, m)
#define HWIO_TLMM_GPIO_IN_1_GPIO_IN_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_IN_1_GPIO_IN_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_IN_2_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x00200068)
#define HWIO_TLMM_GPIO_IN_2_PHYS                                                            (TLMM_CSR_REG_BASE_PHYS + 0x00200068)
#define HWIO_TLMM_GPIO_IN_2_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x00200068)
#define HWIO_TLMM_GPIO_IN_2_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_IN_2_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_2_ADDR, HWIO_TLMM_GPIO_IN_2_RMSK)
#define HWIO_TLMM_GPIO_IN_2_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_2_ADDR, m)
#define HWIO_TLMM_GPIO_IN_2_GPIO_IN_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_IN_2_GPIO_IN_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_IN_3_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x0020006c)
#define HWIO_TLMM_GPIO_IN_3_PHYS                                                            (TLMM_CSR_REG_BASE_PHYS + 0x0020006c)
#define HWIO_TLMM_GPIO_IN_3_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x0020006c)
#define HWIO_TLMM_GPIO_IN_3_RMSK                                                               0x1ffff
#define HWIO_TLMM_GPIO_IN_3_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_3_ADDR, HWIO_TLMM_GPIO_IN_3_RMSK)
#define HWIO_TLMM_GPIO_IN_3_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_3_ADDR, m)
#define HWIO_TLMM_GPIO_IN_3_GPIO_IN_BMSK                                                       0x1ffff
#define HWIO_TLMM_GPIO_IN_3_GPIO_IN_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_OE_0_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x00200080)
#define HWIO_TLMM_GPIO_OE_0_PHYS                                                            (TLMM_CSR_REG_BASE_PHYS + 0x00200080)
#define HWIO_TLMM_GPIO_OE_0_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x00200080)
#define HWIO_TLMM_GPIO_OE_0_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_0_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_0_ADDR, HWIO_TLMM_GPIO_OE_0_RMSK)
#define HWIO_TLMM_GPIO_OE_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_0_ADDR, m)
#define HWIO_TLMM_GPIO_OE_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_0_ADDR,m,v,HWIO_TLMM_GPIO_OE_0_IN)
#define HWIO_TLMM_GPIO_OE_0_GPIO_OE_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_OE_0_GPIO_OE_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_OE_1_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x00200084)
#define HWIO_TLMM_GPIO_OE_1_PHYS                                                            (TLMM_CSR_REG_BASE_PHYS + 0x00200084)
#define HWIO_TLMM_GPIO_OE_1_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x00200084)
#define HWIO_TLMM_GPIO_OE_1_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_1_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_1_ADDR, HWIO_TLMM_GPIO_OE_1_RMSK)
#define HWIO_TLMM_GPIO_OE_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_1_ADDR, m)
#define HWIO_TLMM_GPIO_OE_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_1_ADDR,v)
#define HWIO_TLMM_GPIO_OE_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_1_ADDR,m,v,HWIO_TLMM_GPIO_OE_1_IN)
#define HWIO_TLMM_GPIO_OE_1_GPIO_OE_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_OE_1_GPIO_OE_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_OE_2_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x00200088)
#define HWIO_TLMM_GPIO_OE_2_PHYS                                                            (TLMM_CSR_REG_BASE_PHYS + 0x00200088)
#define HWIO_TLMM_GPIO_OE_2_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x00200088)
#define HWIO_TLMM_GPIO_OE_2_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_2_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_2_ADDR, HWIO_TLMM_GPIO_OE_2_RMSK)
#define HWIO_TLMM_GPIO_OE_2_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_2_ADDR, m)
#define HWIO_TLMM_GPIO_OE_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_2_ADDR,v)
#define HWIO_TLMM_GPIO_OE_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_2_ADDR,m,v,HWIO_TLMM_GPIO_OE_2_IN)
#define HWIO_TLMM_GPIO_OE_2_GPIO_OE_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_OE_2_GPIO_OE_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_OE_3_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x0020008c)
#define HWIO_TLMM_GPIO_OE_3_PHYS                                                            (TLMM_CSR_REG_BASE_PHYS + 0x0020008c)
#define HWIO_TLMM_GPIO_OE_3_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x0020008c)
#define HWIO_TLMM_GPIO_OE_3_RMSK                                                               0x1ffff
#define HWIO_TLMM_GPIO_OE_3_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_3_ADDR, HWIO_TLMM_GPIO_OE_3_RMSK)
#define HWIO_TLMM_GPIO_OE_3_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_3_ADDR, m)
#define HWIO_TLMM_GPIO_OE_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_3_ADDR,v)
#define HWIO_TLMM_GPIO_OE_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_3_ADDR,m,v,HWIO_TLMM_GPIO_OE_3_IN)
#define HWIO_TLMM_GPIO_OE_3_GPIO_OE_BMSK                                                       0x1ffff
#define HWIO_TLMM_GPIO_OE_3_GPIO_OE_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_OE_CLR_0_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x002000a0)
#define HWIO_TLMM_GPIO_OE_CLR_0_PHYS                                                        (TLMM_CSR_REG_BASE_PHYS + 0x002000a0)
#define HWIO_TLMM_GPIO_OE_CLR_0_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x002000a0)
#define HWIO_TLMM_GPIO_OE_CLR_0_RMSK                                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_0_GPIO_OE_CLR_BMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_0_GPIO_OE_CLR_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_CLR_1_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x002000a4)
#define HWIO_TLMM_GPIO_OE_CLR_1_PHYS                                                        (TLMM_CSR_REG_BASE_PHYS + 0x002000a4)
#define HWIO_TLMM_GPIO_OE_CLR_1_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x002000a4)
#define HWIO_TLMM_GPIO_OE_CLR_1_RMSK                                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_1_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_1_GPIO_OE_CLR_BMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_1_GPIO_OE_CLR_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_CLR_2_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x002000a8)
#define HWIO_TLMM_GPIO_OE_CLR_2_PHYS                                                        (TLMM_CSR_REG_BASE_PHYS + 0x002000a8)
#define HWIO_TLMM_GPIO_OE_CLR_2_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x002000a8)
#define HWIO_TLMM_GPIO_OE_CLR_2_RMSK                                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_2_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_2_GPIO_OE_CLR_BMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_2_GPIO_OE_CLR_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_CLR_3_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x002000ac)
#define HWIO_TLMM_GPIO_OE_CLR_3_PHYS                                                        (TLMM_CSR_REG_BASE_PHYS + 0x002000ac)
#define HWIO_TLMM_GPIO_OE_CLR_3_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x002000ac)
#define HWIO_TLMM_GPIO_OE_CLR_3_RMSK                                                           0x1ffff
#define HWIO_TLMM_GPIO_OE_CLR_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_3_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_3_GPIO_OE_CLR_BMSK                                               0x1ffff
#define HWIO_TLMM_GPIO_OE_CLR_3_GPIO_OE_CLR_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_SET_0_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x002000c0)
#define HWIO_TLMM_GPIO_OE_SET_0_PHYS                                                        (TLMM_CSR_REG_BASE_PHYS + 0x002000c0)
#define HWIO_TLMM_GPIO_OE_SET_0_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x002000c0)
#define HWIO_TLMM_GPIO_OE_SET_0_RMSK                                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_0_GPIO_OE_SET_BMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_0_GPIO_OE_SET_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_SET_1_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x002000c4)
#define HWIO_TLMM_GPIO_OE_SET_1_PHYS                                                        (TLMM_CSR_REG_BASE_PHYS + 0x002000c4)
#define HWIO_TLMM_GPIO_OE_SET_1_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x002000c4)
#define HWIO_TLMM_GPIO_OE_SET_1_RMSK                                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_1_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_1_GPIO_OE_SET_BMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_1_GPIO_OE_SET_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_SET_2_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x002000c8)
#define HWIO_TLMM_GPIO_OE_SET_2_PHYS                                                        (TLMM_CSR_REG_BASE_PHYS + 0x002000c8)
#define HWIO_TLMM_GPIO_OE_SET_2_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x002000c8)
#define HWIO_TLMM_GPIO_OE_SET_2_RMSK                                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_2_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_2_GPIO_OE_SET_BMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_2_GPIO_OE_SET_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_SET_3_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x002000cc)
#define HWIO_TLMM_GPIO_OE_SET_3_PHYS                                                        (TLMM_CSR_REG_BASE_PHYS + 0x002000cc)
#define HWIO_TLMM_GPIO_OE_SET_3_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x002000cc)
#define HWIO_TLMM_GPIO_OE_SET_3_RMSK                                                           0x1ffff
#define HWIO_TLMM_GPIO_OE_SET_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_3_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_3_GPIO_OE_SET_BMSK                                               0x1ffff
#define HWIO_TLMM_GPIO_OE_SET_3_GPIO_OE_SET_SHFT                                                   0x0


#endif /* __HALGPIOINTHWIO_H__ */
