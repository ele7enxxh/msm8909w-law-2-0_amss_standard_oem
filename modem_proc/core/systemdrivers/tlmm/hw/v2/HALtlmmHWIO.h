#ifndef __HALTLMMHWIO_H__
#define __HALTLMMHWIO_H__
/*
===========================================================================
*/
/**
  @file HALtlmmHWIO.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    TLMM_CSR

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY RESERVED[TLMM_CSR] DUMMY[TLMM_CSR] 
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

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/tlmm/hw/v2/HALtlmmHWIO.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: TLMM_CSR
 *--------------------------------------------------------------------------*/

#define TLMM_CSR_REG_BASE                                                                   (TLMM_BASE      + 0x00000000)

#define HWIO_TLMM_GPIO_CFGn_ADDR(base,n)                                                    ((base) + 0x00000000 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_CFGn_RMSK                                                                 0x7ff
#define HWIO_TLMM_GPIO_CFGn_MAXn                                                                   121
#define HWIO_TLMM_GPIO_CFGn_INI(base,n)        \
        in_dword_masked(HWIO_TLMM_GPIO_CFGn_ADDR(base,n), HWIO_TLMM_GPIO_CFGn_RMSK)
#define HWIO_TLMM_GPIO_CFGn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_CFGn_ADDR(base,n), mask)
#define HWIO_TLMM_GPIO_CFGn_OUTI(base,n,val)    \
        out_dword(HWIO_TLMM_GPIO_CFGn_ADDR(base,n),val)
#define HWIO_TLMM_GPIO_CFGn_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFGn_ADDR(base,n),mask,val,HWIO_TLMM_GPIO_CFGn_INI(base,n))
#define HWIO_TLMM_GPIO_CFGn_GPIO_HIHYS_EN_BMSK                                                   0x400
#define HWIO_TLMM_GPIO_CFGn_GPIO_HIHYS_EN_SHFT                                                     0xa
#define HWIO_TLMM_GPIO_CFGn_GPIO_OE_BMSK                                                         0x200
#define HWIO_TLMM_GPIO_CFGn_GPIO_OE_SHFT                                                           0x9
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_BMSK                                                    0x1c0
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_SHFT                                                      0x6
#define HWIO_TLMM_GPIO_CFGn_FUNC_SEL_BMSK                                                         0x3c
#define HWIO_TLMM_GPIO_CFGn_FUNC_SEL_SHFT                                                          0x2
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_BMSK                                                         0x3
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_SHFT                                                         0x0

#define HWIO_TLMM_GPIO_IN_OUTn_ADDR(base,n)                                                 ((base) + 0x00000004 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_IN_OUTn_RMSK                                                                0x3
#define HWIO_TLMM_GPIO_IN_OUTn_MAXn                                                                121
#define HWIO_TLMM_GPIO_IN_OUTn_INI(base,n)        \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUTn_ADDR(base,n), HWIO_TLMM_GPIO_IN_OUTn_RMSK)
#define HWIO_TLMM_GPIO_IN_OUTn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUTn_ADDR(base,n), mask)
#define HWIO_TLMM_GPIO_IN_OUTn_OUTI(base,n,val)    \
        out_dword(HWIO_TLMM_GPIO_IN_OUTn_ADDR(base,n),val)
#define HWIO_TLMM_GPIO_IN_OUTn_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUTn_ADDR(base,n),mask,val,HWIO_TLMM_GPIO_IN_OUTn_INI(base,n))
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_OUT_BMSK                                                       0x2
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_OUT_SHFT                                                       0x1
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_IN_BMSK                                                        0x1
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_IN_SHFT                                                        0x0

#define HWIO_TLMM_GPIO_INTR_CFGn_ADDR(base,n)                                               ((base) + 0x00000008 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_INTR_CFGn_RMSK                                                            0x1ff
#define HWIO_TLMM_GPIO_INTR_CFGn_MAXn                                                              121
#define HWIO_TLMM_GPIO_INTR_CFGn_INI(base,n)        \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(base,n), HWIO_TLMM_GPIO_INTR_CFGn_RMSK)
#define HWIO_TLMM_GPIO_INTR_CFGn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(base,n), mask)
#define HWIO_TLMM_GPIO_INTR_CFGn_OUTI(base,n,val)    \
        out_dword(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(base,n),val)
#define HWIO_TLMM_GPIO_INTR_CFGn_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(base,n),mask,val,HWIO_TLMM_GPIO_INTR_CFGn_INI(base,n))
#define HWIO_TLMM_GPIO_INTR_CFGn_DIR_CONN_EN_BMSK                                                0x100
#define HWIO_TLMM_GPIO_INTR_CFGn_DIR_CONN_EN_SHFT                                                  0x8
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_BMSK                                                 0xe0
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_SHFT                                                  0x5
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_RAW_STATUS_EN_BMSK                                          0x10
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_RAW_STATUS_EN_SHFT                                           0x4
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_BMSK                                                0xc
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_SHFT                                                0x2
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_POL_CTL_BMSK                                                 0x2
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_POL_CTL_SHFT                                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_ENABLE_BMSK                                                  0x1
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_ENABLE_SHFT                                                  0x0

#define HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(base,n)                                            ((base) + 0x0000000c + 0x1000 * (n))
#define HWIO_TLMM_GPIO_INTR_STATUSn_RMSK                                                           0x1
#define HWIO_TLMM_GPIO_INTR_STATUSn_MAXn                                                           121
#define HWIO_TLMM_GPIO_INTR_STATUSn_INI(base,n)        \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(base,n), HWIO_TLMM_GPIO_INTR_STATUSn_RMSK)
#define HWIO_TLMM_GPIO_INTR_STATUSn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(base,n), mask)
#define HWIO_TLMM_GPIO_INTR_STATUSn_OUTI(base,n,val)    \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(base,n),val)
#define HWIO_TLMM_GPIO_INTR_STATUSn_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(base,n),mask,val,HWIO_TLMM_GPIO_INTR_STATUSn_INI(base,n))
#define HWIO_TLMM_GPIO_INTR_STATUSn_INTR_STATUS_BMSK                                               0x1
#define HWIO_TLMM_GPIO_INTR_STATUSn_INTR_STATUS_SHFT                                               0x0

#define HWIO_TLMM_CLK_GATE_EN_ADDR(x)                                                       ((x) + 0x00100000)
#define HWIO_TLMM_CLK_GATE_EN_RMSK                                                                 0x7
#define HWIO_TLMM_CLK_GATE_EN_IN(x)      \
        in_dword_masked(HWIO_TLMM_CLK_GATE_EN_ADDR(x), HWIO_TLMM_CLK_GATE_EN_RMSK)
#define HWIO_TLMM_CLK_GATE_EN_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_CLK_GATE_EN_ADDR(x), m)
#define HWIO_TLMM_CLK_GATE_EN_OUT(x, v)      \
        out_dword(HWIO_TLMM_CLK_GATE_EN_ADDR(x),v)
#define HWIO_TLMM_CLK_GATE_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_CLK_GATE_EN_ADDR(x),m,v,HWIO_TLMM_CLK_GATE_EN_IN(x))
#define HWIO_TLMM_CLK_GATE_EN_AHB_HCLK_EN_BMSK                                                     0x4
#define HWIO_TLMM_CLK_GATE_EN_AHB_HCLK_EN_SHFT                                                     0x2
#define HWIO_TLMM_CLK_GATE_EN_SUMMARY_INTR_EN_BMSK                                                 0x2
#define HWIO_TLMM_CLK_GATE_EN_SUMMARY_INTR_EN_SHFT                                                 0x1
#define HWIO_TLMM_CLK_GATE_EN_CRIF_READ_EN_BMSK                                                    0x1
#define HWIO_TLMM_CLK_GATE_EN_CRIF_READ_EN_SHFT                                                    0x0

#define HWIO_TLMM_IE_CTRL_DISABLE_ADDR(x)                                                   ((x) + 0x00100004)
#define HWIO_TLMM_IE_CTRL_DISABLE_RMSK                                                             0x1
#define HWIO_TLMM_IE_CTRL_DISABLE_IN(x)      \
        in_dword_masked(HWIO_TLMM_IE_CTRL_DISABLE_ADDR(x), HWIO_TLMM_IE_CTRL_DISABLE_RMSK)
#define HWIO_TLMM_IE_CTRL_DISABLE_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_IE_CTRL_DISABLE_ADDR(x), m)
#define HWIO_TLMM_IE_CTRL_DISABLE_OUT(x, v)      \
        out_dword(HWIO_TLMM_IE_CTRL_DISABLE_ADDR(x),v)
#define HWIO_TLMM_IE_CTRL_DISABLE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_IE_CTRL_DISABLE_ADDR(x),m,v,HWIO_TLMM_IE_CTRL_DISABLE_IN(x))
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_BMSK                                             0x1
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_SHFT                                             0x0

#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR(x)                                               ((x) + 0x00100008)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_RMSK                                                  0xffffffff
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_IN(x)      \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR(x), HWIO_TLMM_MPM_WAKEUP_INT_EN_0_RMSK)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR(x), m)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_OUT(x, v)      \
        out_dword(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR(x),v)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR(x),m,v,HWIO_TLMM_MPM_WAKEUP_INT_EN_0_IN(x))
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_115_BMSK                                         0x80000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_115_SHFT                                               0x1f
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_114_BMSK                                         0x40000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_114_SHFT                                               0x1e
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_113_BMSK                                         0x20000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_113_SHFT                                               0x1d
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_112_BMSK                                         0x10000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_112_SHFT                                               0x1c
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_111_BMSK                                          0x8000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_111_SHFT                                               0x1b
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_110_BMSK                                          0x4000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_110_SHFT                                               0x1a
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_109_BMSK                                          0x2000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_109_SHFT                                               0x19
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_108_BMSK                                          0x1000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_108_SHFT                                               0x18
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_107_BMSK                                           0x800000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_107_SHFT                                               0x17
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_98_BMSK                                            0x400000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_98_SHFT                                                0x16
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_97_BMSK                                            0x200000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_97_SHFT                                                0x15
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_69_BMSK                                            0x100000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_69_SHFT                                                0x14
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_62_BMSK                                             0x80000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_62_SHFT                                                0x13
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_54_BMSK                                             0x40000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_54_SHFT                                                0x12
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_52_BMSK                                             0x20000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_52_SHFT                                                0x11
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_51_BMSK                                             0x10000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_51_SHFT                                                0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_50_BMSK                                              0x8000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_50_SHFT                                                 0xf
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_49_BMSK                                              0x4000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_49_SHFT                                                 0xe
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_38_BMSK                                              0x2000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_38_SHFT                                                 0xd
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_37_BMSK                                              0x1000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_37_SHFT                                                 0xc
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_36_BMSK                                               0x800
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_36_SHFT                                                 0xb
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_35_BMSK                                               0x400
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_35_SHFT                                                 0xa
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_34_BMSK                                               0x200
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_34_SHFT                                                 0x9
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_31_BMSK                                               0x100
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_31_SHFT                                                 0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_28_BMSK                                                0x80
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_28_SHFT                                                 0x7
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_25_BMSK                                                0x40
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_25_SHFT                                                 0x6
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_21_BMSK                                                0x20
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_21_SHFT                                                 0x5
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_20_BMSK                                                0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_20_SHFT                                                 0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_13_BMSK                                                 0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_13_SHFT                                                 0x3
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_12_BMSK                                                 0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_12_SHFT                                                 0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_5_BMSK                                                  0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_5_SHFT                                                  0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_1_BMSK                                                  0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_1_SHFT                                                  0x0

#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR(x)                                               ((x) + 0x0010000c)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_RMSK                                                      0x7fff
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_IN(x)      \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR(x), HWIO_TLMM_MPM_WAKEUP_INT_EN_1_RMSK)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR(x), m)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_OUT(x, v)      \
        out_dword(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR(x),v)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR(x),m,v,HWIO_TLMM_MPM_WAKEUP_INT_EN_1_IN(x))
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_53_BMSK                                              0x4000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_53_SHFT                                                 0xe
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_65_BMSK                                              0x2000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_65_SHFT                                                 0xd
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_68_BMSK                                              0x1000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_68_SHFT                                                 0xc
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_66_BMSK                                               0x800
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_66_SHFT                                                 0xb
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_3_BMSK                                           0x400
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_3_SHFT                                             0xa
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_1_BMSK                                           0x200
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_1_SHFT                                             0x9
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC1_DATA_3_BMSK                                           0x100
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC1_DATA_3_SHFT                                             0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC1_DATA_1_BMSK                                            0x80
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC1_DATA_1_SHFT                                             0x7
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SRST_N_BMSK                                                 0x40
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SRST_N_SHFT                                                  0x6
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_121_BMSK                                               0x20
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_121_SHFT                                                0x5
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_120_BMSK                                               0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_120_SHFT                                                0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_9_BMSK                                                  0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_9_SHFT                                                  0x3
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_118_BMSK                                                0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_118_SHFT                                                0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_117_BMSK                                                0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_117_SHFT                                                0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_11_BMSK                                                 0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_11_SHFT                                                 0x0

#define HWIO_TLMM_INT_JTAG_CTL_ADDR(x)                                                      ((x) + 0x00110000)
#define HWIO_TLMM_INT_JTAG_CTL_RMSK                                                                0xf
#define HWIO_TLMM_INT_JTAG_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_INT_JTAG_CTL_ADDR(x), HWIO_TLMM_INT_JTAG_CTL_RMSK)
#define HWIO_TLMM_INT_JTAG_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_INT_JTAG_CTL_ADDR(x), m)
#define HWIO_TLMM_INT_JTAG_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_INT_JTAG_CTL_ADDR(x),v)
#define HWIO_TLMM_INT_JTAG_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_INT_JTAG_CTL_ADDR(x),m,v,HWIO_TLMM_INT_JTAG_CTL_IN(x))
#define HWIO_TLMM_INT_JTAG_CTL_APSS_TAP_ENA_BMSK                                                   0x8
#define HWIO_TLMM_INT_JTAG_CTL_APSS_TAP_ENA_SHFT                                                   0x3
#define HWIO_TLMM_INT_JTAG_CTL_QDSS_TAP_ENA_BMSK                                                   0x4
#define HWIO_TLMM_INT_JTAG_CTL_QDSS_TAP_ENA_SHFT                                                   0x2
#define HWIO_TLMM_INT_JTAG_CTL_ACC_TAP_ENA_BMSK                                                    0x2
#define HWIO_TLMM_INT_JTAG_CTL_ACC_TAP_ENA_SHFT                                                    0x1
#define HWIO_TLMM_INT_JTAG_CTL_MSM_TAP_ENA_BMSK                                                    0x1
#define HWIO_TLMM_INT_JTAG_CTL_MSM_TAP_ENA_SHFT                                                    0x0

#define HWIO_TLMM_ETM_MODE_ADDR(x)                                                          ((x) + 0x00110004)
#define HWIO_TLMM_ETM_MODE_RMSK                                                                    0x3
#define HWIO_TLMM_ETM_MODE_IN(x)      \
        in_dword_masked(HWIO_TLMM_ETM_MODE_ADDR(x), HWIO_TLMM_ETM_MODE_RMSK)
#define HWIO_TLMM_ETM_MODE_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_ETM_MODE_ADDR(x), m)
#define HWIO_TLMM_ETM_MODE_OUT(x, v)      \
        out_dword(HWIO_TLMM_ETM_MODE_ADDR(x),v)
#define HWIO_TLMM_ETM_MODE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_ETM_MODE_ADDR(x),m,v,HWIO_TLMM_ETM_MODE_IN(x))
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC2_BMSK                                                    0x3
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC2_SHFT                                                    0x0

#define HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR(x)                                                   ((x) + 0x00110008)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_RMSK                                                             0x3
#define HWIO_TLMM_DBG_BUS_OUT_SEL_IN(x)      \
        in_dword_masked(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR(x), HWIO_TLMM_DBG_BUS_OUT_SEL_RMSK)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR(x), m)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_OUT(x, v)      \
        out_dword(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR(x),v)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR(x),m,v,HWIO_TLMM_DBG_BUS_OUT_SEL_IN(x))
#define HWIO_TLMM_DBG_BUS_OUT_SEL_COPY_SEL_BMSK                                                    0x3
#define HWIO_TLMM_DBG_BUS_OUT_SEL_COPY_SEL_SHFT                                                    0x0

#define HWIO_TLMM_CHIP_MODE_ADDR(x)                                                         ((x) + 0x0011000c)
#define HWIO_TLMM_CHIP_MODE_RMSK                                                                   0x3
#define HWIO_TLMM_CHIP_MODE_IN(x)      \
        in_dword_masked(HWIO_TLMM_CHIP_MODE_ADDR(x), HWIO_TLMM_CHIP_MODE_RMSK)
#define HWIO_TLMM_CHIP_MODE_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_CHIP_MODE_ADDR(x), m)
#define HWIO_TLMM_CHIP_MODE_MODE1_PIN_BMSK                                                         0x2
#define HWIO_TLMM_CHIP_MODE_MODE1_PIN_SHFT                                                         0x1
#define HWIO_TLMM_CHIP_MODE_MODE0_PIN_BMSK                                                         0x1
#define HWIO_TLMM_CHIP_MODE_MODE0_PIN_SHFT                                                         0x0

#define HWIO_TLMM_SPARE_ADDR(x)                                                             ((x) + 0x0010e000)
#define HWIO_TLMM_SPARE_RMSK                                                                0xffffffff
#define HWIO_TLMM_SPARE_IN(x)      \
        in_dword_masked(HWIO_TLMM_SPARE_ADDR(x), HWIO_TLMM_SPARE_RMSK)
#define HWIO_TLMM_SPARE_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_SPARE_ADDR(x), m)
#define HWIO_TLMM_SPARE_OUT(x, v)      \
        out_dword(HWIO_TLMM_SPARE_ADDR(x),v)
#define HWIO_TLMM_SPARE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_SPARE_ADDR(x),m,v,HWIO_TLMM_SPARE_IN(x))
#define HWIO_TLMM_SPARE_SPARE_BMSK                                                          0xffffff00
#define HWIO_TLMM_SPARE_SPARE_SHFT                                                                 0x8
#define HWIO_TLMM_SPARE_MISC_BMSK                                                                 0xff
#define HWIO_TLMM_SPARE_MISC_SHFT                                                                  0x0

#define HWIO_SPARE1_ADDR(x)                                                                 ((x) + 0x0011001c)
#define HWIO_SPARE1_RMSK                                                                    0xffffffff
#define HWIO_SPARE1_IN(x)      \
        in_dword_masked(HWIO_SPARE1_ADDR(x), HWIO_SPARE1_RMSK)
#define HWIO_SPARE1_INM(x, m)      \
        in_dword_masked(HWIO_SPARE1_ADDR(x), m)
#define HWIO_SPARE1_OUT(x, v)      \
        out_dword(HWIO_SPARE1_ADDR(x),v)
#define HWIO_SPARE1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPARE1_ADDR(x),m,v,HWIO_SPARE1_IN(x))
#define HWIO_SPARE1_MISC1_BMSK                                                              0xffffffff
#define HWIO_SPARE1_MISC1_SHFT                                                                     0x0

#define HWIO_SPARE2_ADDR(x)                                                                 ((x) + 0x00110020)
#define HWIO_SPARE2_RMSK                                                                    0xffffffff
#define HWIO_SPARE2_IN(x)      \
        in_dword_masked(HWIO_SPARE2_ADDR(x), HWIO_SPARE2_RMSK)
#define HWIO_SPARE2_INM(x, m)      \
        in_dword_masked(HWIO_SPARE2_ADDR(x), m)
#define HWIO_SPARE2_OUT(x, v)      \
        out_dword(HWIO_SPARE2_ADDR(x),v)
#define HWIO_SPARE2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPARE2_ADDR(x),m,v,HWIO_SPARE2_IN(x))
#define HWIO_SPARE2_MISC2_BMSK                                                              0xffffffff
#define HWIO_SPARE2_MISC2_SHFT                                                                     0x0

#define HWIO_TLMM_HW_REVISION_NUMBER_ADDR(x)                                                ((x) + 0x00110010)
#define HWIO_TLMM_HW_REVISION_NUMBER_RMSK                                                   0xffffffff
#define HWIO_TLMM_HW_REVISION_NUMBER_IN(x)      \
        in_dword_masked(HWIO_TLMM_HW_REVISION_NUMBER_ADDR(x), HWIO_TLMM_HW_REVISION_NUMBER_RMSK)
#define HWIO_TLMM_HW_REVISION_NUMBER_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_HW_REVISION_NUMBER_ADDR(x), m)
#define HWIO_TLMM_HW_REVISION_NUMBER_VERSION_ID_BMSK                                        0xf0000000
#define HWIO_TLMM_HW_REVISION_NUMBER_VERSION_ID_SHFT                                              0x1c
#define HWIO_TLMM_HW_REVISION_NUMBER_PARTNUM_BMSK                                            0xffff000
#define HWIO_TLMM_HW_REVISION_NUMBER_PARTNUM_SHFT                                                  0xc
#define HWIO_TLMM_HW_REVISION_NUMBER_QUALCOMM_MFG_ID_BMSK                                        0xffe
#define HWIO_TLMM_HW_REVISION_NUMBER_QUALCOMM_MFG_ID_SHFT                                          0x1
#define HWIO_TLMM_HW_REVISION_NUMBER_START_BIT_BMSK                                                0x1
#define HWIO_TLMM_HW_REVISION_NUMBER_START_BIT_SHFT                                                0x0

#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR(x)                                             ((x) + 0x00110014)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_RMSK                                                       0x1
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_IN(x)      \
        in_dword_masked(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR(x), HWIO_TLMM_PERIPH_CHAR_TEST_MODE_RMSK)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR(x), m)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_OUT(x, v)      \
        out_dword(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR(x),v)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR(x),m,v,HWIO_TLMM_PERIPH_CHAR_TEST_MODE_IN(x))
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_CHAR_TEST_MODE_BMSK                                        0x1
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_CHAR_TEST_MODE_SHFT                                        0x0

#define HWIO_TLMM_PBL_DEBUG_ADDR(x)                                                         ((x) + 0x00110018)
#define HWIO_TLMM_PBL_DEBUG_RMSK                                                                   0x1
#define HWIO_TLMM_PBL_DEBUG_IN(x)      \
        in_dword_masked(HWIO_TLMM_PBL_DEBUG_ADDR(x), HWIO_TLMM_PBL_DEBUG_RMSK)
#define HWIO_TLMM_PBL_DEBUG_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_PBL_DEBUG_ADDR(x), m)
#define HWIO_TLMM_PBL_DEBUG_OUT(x, v)      \
        out_dword(HWIO_TLMM_PBL_DEBUG_ADDR(x),v)
#define HWIO_TLMM_PBL_DEBUG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_PBL_DEBUG_ADDR(x),m,v,HWIO_TLMM_PBL_DEBUG_IN(x))
#define HWIO_TLMM_PBL_DEBUG_PBL_DEBUG_BMSK                                                         0x1
#define HWIO_TLMM_PBL_DEBUG_PBL_DEBUG_SHFT                                                         0x0

#define HWIO_TLMM_SLIMBUS_CTL_ADDR(x)                                                       ((x) + 0x001a8000)
#define HWIO_TLMM_SLIMBUS_CTL_RMSK                                                                 0x7
#define HWIO_TLMM_SLIMBUS_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_SLIMBUS_CTL_ADDR(x), HWIO_TLMM_SLIMBUS_CTL_RMSK)
#define HWIO_TLMM_SLIMBUS_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_SLIMBUS_CTL_ADDR(x), m)
#define HWIO_TLMM_SLIMBUS_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_SLIMBUS_CTL_ADDR(x),v)
#define HWIO_TLMM_SLIMBUS_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_SLIMBUS_CTL_ADDR(x),m,v,HWIO_TLMM_SLIMBUS_CTL_IN(x))
#define HWIO_TLMM_SLIMBUS_CTL_SEN_EN_BMSK                                                          0x4
#define HWIO_TLMM_SLIMBUS_CTL_SEN_EN_SHFT                                                          0x2
#define HWIO_TLMM_SLIMBUS_CTL_SR_CTL_EN_BMSK                                                       0x3
#define HWIO_TLMM_SLIMBUS_CTL_SR_CTL_EN_SHFT                                                       0x0

#define HWIO_TLMM_RFFE_CTL_ADDR(x)                                                          ((x) + 0x00108000)
#define HWIO_TLMM_RFFE_CTL_RMSK                                                             0x3fffffff
#define HWIO_TLMM_RFFE_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_RFFE_CTL_ADDR(x), HWIO_TLMM_RFFE_CTL_RMSK)
#define HWIO_TLMM_RFFE_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_RFFE_CTL_ADDR(x), m)
#define HWIO_TLMM_RFFE_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_RFFE_CTL_ADDR(x),v)
#define HWIO_TLMM_RFFE_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_RFFE_CTL_ADDR(x),m,v,HWIO_TLMM_RFFE_CTL_IN(x))
#define HWIO_TLMM_RFFE_CTL_RFFE5_DATA_SEN_EN_BMSK                                           0x20000000
#define HWIO_TLMM_RFFE_CTL_RFFE5_DATA_SEN_EN_SHFT                                                 0x1d
#define HWIO_TLMM_RFFE_CTL_RFFE5_DATA_SR_CTL_EN_BMSK                                        0x18000000
#define HWIO_TLMM_RFFE_CTL_RFFE5_DATA_SR_CTL_EN_SHFT                                              0x1b
#define HWIO_TLMM_RFFE_CTL_RFFE5_CLK_SEN_EN_BMSK                                             0x4000000
#define HWIO_TLMM_RFFE_CTL_RFFE5_CLK_SEN_EN_SHFT                                                  0x1a
#define HWIO_TLMM_RFFE_CTL_RFFE5_CLK_SR_CTL_EN_BMSK                                          0x3000000
#define HWIO_TLMM_RFFE_CTL_RFFE5_CLK_SR_CTL_EN_SHFT                                               0x18
#define HWIO_TLMM_RFFE_CTL_RFFE4_DATA_SEN_EN_BMSK                                             0x800000
#define HWIO_TLMM_RFFE_CTL_RFFE4_DATA_SEN_EN_SHFT                                                 0x17
#define HWIO_TLMM_RFFE_CTL_RFFE4_DATA_SR_CTL_EN_BMSK                                          0x600000
#define HWIO_TLMM_RFFE_CTL_RFFE4_DATA_SR_CTL_EN_SHFT                                              0x15
#define HWIO_TLMM_RFFE_CTL_RFFE4_CLK_SEN_EN_BMSK                                              0x100000
#define HWIO_TLMM_RFFE_CTL_RFFE4_CLK_SEN_EN_SHFT                                                  0x14
#define HWIO_TLMM_RFFE_CTL_RFFE4_CLK_SR_CTL_EN_BMSK                                            0xc0000
#define HWIO_TLMM_RFFE_CTL_RFFE4_CLK_SR_CTL_EN_SHFT                                               0x12
#define HWIO_TLMM_RFFE_CTL_RFFE3_DATA_SEN_EN_BMSK                                              0x20000
#define HWIO_TLMM_RFFE_CTL_RFFE3_DATA_SEN_EN_SHFT                                                 0x11
#define HWIO_TLMM_RFFE_CTL_RFFE3_DATA_SR_CTL_EN_BMSK                                           0x18000
#define HWIO_TLMM_RFFE_CTL_RFFE3_DATA_SR_CTL_EN_SHFT                                               0xf
#define HWIO_TLMM_RFFE_CTL_RFFE3_CLK_SEN_EN_BMSK                                                0x4000
#define HWIO_TLMM_RFFE_CTL_RFFE3_CLK_SEN_EN_SHFT                                                   0xe
#define HWIO_TLMM_RFFE_CTL_RFFE3_CLK_SR_CTL_EN_BMSK                                             0x3000
#define HWIO_TLMM_RFFE_CTL_RFFE3_CLK_SR_CTL_EN_SHFT                                                0xc
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SEN_EN_BMSK                                                0x800
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SEN_EN_SHFT                                                  0xb
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SR_CTL_EN_BMSK                                             0x600
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SR_CTL_EN_SHFT                                               0x9
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SEN_EN_BMSK                                                 0x100
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SEN_EN_SHFT                                                   0x8
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SR_CTL_EN_BMSK                                               0xc0
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SR_CTL_EN_SHFT                                                0x6
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SEN_EN_BMSK                                                 0x20
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SEN_EN_SHFT                                                  0x5
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SR_CTL_EN_BMSK                                              0x18
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SR_CTL_EN_SHFT                                               0x3
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SEN_EN_BMSK                                                   0x4
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SEN_EN_SHFT                                                   0x2
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SR_CTL_EN_BMSK                                                0x3
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SR_CTL_EN_SHFT                                                0x0

#define HWIO_TLMM_RESOUT_HDRV_CTL_ADDR(x)                                                   ((x) + 0x0010d000)
#define HWIO_TLMM_RESOUT_HDRV_CTL_RMSK                                                             0x7
#define HWIO_TLMM_RESOUT_HDRV_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR(x), HWIO_TLMM_RESOUT_HDRV_CTL_RMSK)
#define HWIO_TLMM_RESOUT_HDRV_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR(x), m)
#define HWIO_TLMM_RESOUT_HDRV_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR(x),v)
#define HWIO_TLMM_RESOUT_HDRV_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR(x),m,v,HWIO_TLMM_RESOUT_HDRV_CTL_IN(x))
#define HWIO_TLMM_RESOUT_HDRV_CTL_RESOUT_N_HDRV_BMSK                                               0x7
#define HWIO_TLMM_RESOUT_HDRV_CTL_RESOUT_N_HDRV_SHFT                                               0x0

#define HWIO_TLMM_JTAG_HDRV_CTL_ADDR(x)                                                     ((x) + 0x0010c000)
#define HWIO_TLMM_JTAG_HDRV_CTL_RMSK                                                             0x3ff
#define HWIO_TLMM_JTAG_HDRV_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_JTAG_HDRV_CTL_ADDR(x), HWIO_TLMM_JTAG_HDRV_CTL_RMSK)
#define HWIO_TLMM_JTAG_HDRV_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_JTAG_HDRV_CTL_ADDR(x), m)
#define HWIO_TLMM_JTAG_HDRV_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_JTAG_HDRV_CTL_ADDR(x),v)
#define HWIO_TLMM_JTAG_HDRV_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_JTAG_HDRV_CTL_ADDR(x),m,v,HWIO_TLMM_JTAG_HDRV_CTL_IN(x))
#define HWIO_TLMM_JTAG_HDRV_CTL_TDI_PULL_BMSK                                                    0x300
#define HWIO_TLMM_JTAG_HDRV_CTL_TDI_PULL_SHFT                                                      0x8
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_PULL_BMSK                                                     0xc0
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_PULL_SHFT                                                      0x6
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_HDRV_BMSK                                                     0x38
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_HDRV_SHFT                                                      0x3
#define HWIO_TLMM_JTAG_HDRV_CTL_TDO_HDRV_BMSK                                                      0x7
#define HWIO_TLMM_JTAG_HDRV_CTL_TDO_HDRV_SHFT                                                      0x0

#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR(x)                                                ((x) + 0x0010b000)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_RMSK                                                     0x3fffff
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR(x), HWIO_TLMM_PMIC_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR(x), m)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR(x),v)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR(x),m,v,HWIO_TLMM_PMIC_HDRV_PULL_CTL_IN(x))
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_SR_CTL_EN_BMSK                            0x300000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_SR_CTL_EN_SHFT                                0x14
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_SR_CTL_EN_BMSK                              0xc0000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_SR_CTL_EN_SHFT                                 0x12
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_SEN_EN_BMSK                                0x20000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_SEN_EN_SHFT                                   0x11
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_SEN_EN_BMSK                                 0x10000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_SEN_EN_SHFT                                    0x10
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

#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR(x)                                                ((x) + 0x0010a000)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_RMSK                                                       0x7fff
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR(x), HWIO_TLMM_SDC1_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR(x), m)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR(x),v)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR(x),m,v,HWIO_TLMM_SDC1_HDRV_PULL_CTL_IN(x))
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

#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR(x)                                                ((x) + 0x00109000)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_RMSK                                                       0xffff
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR(x), HWIO_TLMM_SDC2_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR(x), m)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR(x),v)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR(x),m,v,HWIO_TLMM_SDC2_HDRV_PULL_CTL_IN(x))
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_PULL_BMSK                                         0xc000
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_PULL_SHFT                                            0xe
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_HYS_CTL_BMSK                                          0x2000
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_HYS_CTL_SHFT                                             0xd
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

#define HWIO_TLMM_MODE_PULL_CTL_ADDR(x)                                                     ((x) + 0x00107000)
#define HWIO_TLMM_MODE_PULL_CTL_RMSK                                                               0xf
#define HWIO_TLMM_MODE_PULL_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_MODE_PULL_CTL_ADDR(x), HWIO_TLMM_MODE_PULL_CTL_RMSK)
#define HWIO_TLMM_MODE_PULL_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_MODE_PULL_CTL_ADDR(x), m)
#define HWIO_TLMM_MODE_PULL_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_MODE_PULL_CTL_ADDR(x),v)
#define HWIO_TLMM_MODE_PULL_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_MODE_PULL_CTL_ADDR(x),m,v,HWIO_TLMM_MODE_PULL_CTL_IN(x))
#define HWIO_TLMM_MODE_PULL_CTL_MODE_1_PULL_BMSK                                                   0xc
#define HWIO_TLMM_MODE_PULL_CTL_MODE_1_PULL_SHFT                                                   0x2
#define HWIO_TLMM_MODE_PULL_CTL_MODE_0_PULL_BMSK                                                   0x3
#define HWIO_TLMM_MODE_PULL_CTL_MODE_0_PULL_SHFT                                                   0x0

#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_ADDR(x)                                     ((x) + 0x0019c000)
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_RMSK                                        0xffffffff
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_ADDR(x), HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_RMSK)
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_ADDR(x), m)
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_ADDR(x),v)
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_ADDR(x),m,v,HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_IN(x))
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_QDSD_HDRV_PULL_DEBUG_GPIO_REG_BMSK          0xffffffff
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_QDSD_HDRV_PULL_DEBUG_GPIO_REG_SHFT                 0x0

#define HWIO_TLMM_QDSD_BOOT_CTL_ADDR(x)                                                     ((x) + 0x0019d000)
#define HWIO_TLMM_QDSD_BOOT_CTL_RMSK                                                            0xffff
#define HWIO_TLMM_QDSD_BOOT_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_QDSD_BOOT_CTL_ADDR(x), HWIO_TLMM_QDSD_BOOT_CTL_RMSK)
#define HWIO_TLMM_QDSD_BOOT_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_QDSD_BOOT_CTL_ADDR(x), m)
#define HWIO_TLMM_QDSD_BOOT_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_QDSD_BOOT_CTL_ADDR(x),v)
#define HWIO_TLMM_QDSD_BOOT_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_BOOT_CTL_ADDR(x),m,v,HWIO_TLMM_QDSD_BOOT_CTL_IN(x))
#define HWIO_TLMM_QDSD_BOOT_CTL_QDSD_BOOT_REG_BMSK                                              0xffff
#define HWIO_TLMM_QDSD_BOOT_CTL_QDSD_BOOT_REG_SHFT                                                 0x0

#define HWIO_TLMM_QDSD_CONFIG_CTL_ADDR(x)                                                   ((x) + 0x0019e000)
#define HWIO_TLMM_QDSD_CONFIG_CTL_RMSK                                                      0xffffffff
#define HWIO_TLMM_QDSD_CONFIG_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_QDSD_CONFIG_CTL_ADDR(x), HWIO_TLMM_QDSD_CONFIG_CTL_RMSK)
#define HWIO_TLMM_QDSD_CONFIG_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_QDSD_CONFIG_CTL_ADDR(x), m)
#define HWIO_TLMM_QDSD_CONFIG_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_QDSD_CONFIG_CTL_ADDR(x),v)
#define HWIO_TLMM_QDSD_CONFIG_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_CONFIG_CTL_ADDR(x),m,v,HWIO_TLMM_QDSD_CONFIG_CTL_IN(x))
#define HWIO_TLMM_QDSD_CONFIG_CTL_QDSD_CONFIG_REG_BMSK                                      0xffffffff
#define HWIO_TLMM_QDSD_CONFIG_CTL_QDSD_CONFIG_REG_SHFT                                             0x0

#define HWIO_TLMM_QDSD_STATUS_CTL_ADDR(x)                                                   ((x) + 0x0019f000)
#define HWIO_TLMM_QDSD_STATUS_CTL_RMSK                                                      0xffffffff
#define HWIO_TLMM_QDSD_STATUS_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_QDSD_STATUS_CTL_ADDR(x), HWIO_TLMM_QDSD_STATUS_CTL_RMSK)
#define HWIO_TLMM_QDSD_STATUS_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_QDSD_STATUS_CTL_ADDR(x), m)
#define HWIO_TLMM_QDSD_STATUS_CTL_QDSD_STATUS_REG_BMSK                                      0xffffffff
#define HWIO_TLMM_QDSD_STATUS_CTL_QDSD_STATUS_REG_SHFT                                             0x0

#define HWIO_TLMM_QDSD_HYS_CTL_ADDR(x)                                                      ((x) + 0x001a7000)
#define HWIO_TLMM_QDSD_HYS_CTL_RMSK                                                                0x1
#define HWIO_TLMM_QDSD_HYS_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_QDSD_HYS_CTL_ADDR(x), HWIO_TLMM_QDSD_HYS_CTL_RMSK)
#define HWIO_TLMM_QDSD_HYS_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_QDSD_HYS_CTL_ADDR(x), m)
#define HWIO_TLMM_QDSD_HYS_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_QDSD_HYS_CTL_ADDR(x),v)
#define HWIO_TLMM_QDSD_HYS_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_HYS_CTL_ADDR(x),m,v,HWIO_TLMM_QDSD_HYS_CTL_IN(x))
#define HWIO_TLMM_QDSD_HYS_CTL_QDSD_HYS_CTL_BMSK                                                   0x1
#define HWIO_TLMM_QDSD_HYS_CTL_QDSD_HYS_CTL_SHFT                                                   0x0

#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_ADDR(x)                                          ((x) + 0x001a0000)
#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_RMSK                                             0x3fffffff
#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_ADDR(x), HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_ADDR(x), m)
#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_QDSD_DEBUG_HDRV_PULL_REG_BMSK                    0x3fffffff
#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_QDSD_DEBUG_HDRV_PULL_REG_SHFT                           0x0

#define HWIO_TLMM_QDSD_GPIO_CTL_ADDR(x)                                                     ((x) + 0x001a1000)
#define HWIO_TLMM_QDSD_GPIO_CTL_RMSK                                                            0xffff
#define HWIO_TLMM_QDSD_GPIO_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_QDSD_GPIO_CTL_ADDR(x), HWIO_TLMM_QDSD_GPIO_CTL_RMSK)
#define HWIO_TLMM_QDSD_GPIO_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_QDSD_GPIO_CTL_ADDR(x), m)
#define HWIO_TLMM_QDSD_GPIO_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_QDSD_GPIO_CTL_ADDR(x),v)
#define HWIO_TLMM_QDSD_GPIO_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_GPIO_CTL_ADDR(x),m,v,HWIO_TLMM_QDSD_GPIO_CTL_IN(x))
#define HWIO_TLMM_QDSD_GPIO_CTL_QDSD_GPIO_REG_BMSK                                              0xffff
#define HWIO_TLMM_QDSD_GPIO_CTL_QDSD_GPIO_REG_SHFT                                                 0x0

#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_ADDR(x)                                              ((x) + 0x001a2000)
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_RMSK                                                     0xffff
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_QDSD_INTR_ENABLE_CTL_ADDR(x), HWIO_TLMM_QDSD_INTR_ENABLE_CTL_RMSK)
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_QDSD_INTR_ENABLE_CTL_ADDR(x), m)
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_QDSD_INTR_ENABLE_CTL_ADDR(x),v)
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_INTR_ENABLE_CTL_ADDR(x),m,v,HWIO_TLMM_QDSD_INTR_ENABLE_CTL_IN(x))
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_QDSD_INTR_ENABLE_REG_BMSK                                0xffff
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_QDSD_INTR_ENABLE_REG_SHFT                                   0x0

#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_ADDR(x)                                               ((x) + 0x001a3000)
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_RMSK                                                      0xffff
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_QDSD_INTR_CLEAR_CTL_ADDR(x), HWIO_TLMM_QDSD_INTR_CLEAR_CTL_RMSK)
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_QDSD_INTR_CLEAR_CTL_ADDR(x), m)
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_QDSD_INTR_CLEAR_CTL_ADDR(x),v)
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_INTR_CLEAR_CTL_ADDR(x),m,v,HWIO_TLMM_QDSD_INTR_CLEAR_CTL_IN(x))
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_QDSD_INTR_CLEAR_REG_BMSK                                  0xffff
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_QDSD_INTR_CLEAR_REG_SHFT                                     0x0

#define HWIO_TLMM_QDSD_SPARE1_CTL_ADDR(x)                                                   ((x) + 0x001a5000)
#define HWIO_TLMM_QDSD_SPARE1_CTL_RMSK                                                      0xffffffff
#define HWIO_TLMM_QDSD_SPARE1_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_QDSD_SPARE1_CTL_ADDR(x), HWIO_TLMM_QDSD_SPARE1_CTL_RMSK)
#define HWIO_TLMM_QDSD_SPARE1_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_QDSD_SPARE1_CTL_ADDR(x), m)
#define HWIO_TLMM_QDSD_SPARE1_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_QDSD_SPARE1_CTL_ADDR(x),v)
#define HWIO_TLMM_QDSD_SPARE1_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_SPARE1_CTL_ADDR(x),m,v,HWIO_TLMM_QDSD_SPARE1_CTL_IN(x))
#define HWIO_TLMM_QDSD_SPARE1_CTL_QDSD_SPARE1_REG_BMSK                                      0xffffffff
#define HWIO_TLMM_QDSD_SPARE1_CTL_QDSD_SPARE1_REG_SHFT                                             0x0

#define HWIO_TLMM_QDSD_SPARE2_CTL_ADDR(x)                                                   ((x) + 0x001a6000)
#define HWIO_TLMM_QDSD_SPARE2_CTL_RMSK                                                      0xffffffff
#define HWIO_TLMM_QDSD_SPARE2_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_QDSD_SPARE2_CTL_ADDR(x), HWIO_TLMM_QDSD_SPARE2_CTL_RMSK)
#define HWIO_TLMM_QDSD_SPARE2_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_QDSD_SPARE2_CTL_ADDR(x), m)
#define HWIO_TLMM_QDSD_SPARE2_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_QDSD_SPARE2_CTL_ADDR(x),v)
#define HWIO_TLMM_QDSD_SPARE2_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_SPARE2_CTL_ADDR(x),m,v,HWIO_TLMM_QDSD_SPARE2_CTL_IN(x))
#define HWIO_TLMM_QDSD_SPARE2_CTL_QDSD_SPARE2_REG_BMSK                                      0xffffffff
#define HWIO_TLMM_QDSD_SPARE2_CTL_QDSD_SPARE2_REG_SHFT                                             0x0

#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_ADDR(x)                                            ((x) + 0x001a4000)
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_RMSK                                               0xffffffff
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_ADDR(x), HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_RMSK)
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_ADDR(x), m)
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_ADDR(x),v)
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_ADDR(x),m,v,HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_IN(x))
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_QDSD_TIMEOUT_VALUE_REG_BMSK                        0xffffffff
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_QDSD_TIMEOUT_VALUE_REG_SHFT                               0x0

#define HWIO_TLMM_GPIO_HS_I2C_CTL_ADDR(x)                                                   ((x) + 0x0010f000)
#define HWIO_TLMM_GPIO_HS_I2C_CTL_RMSK                                                             0x3
#define HWIO_TLMM_GPIO_HS_I2C_CTL_IN(x)      \
        in_dword_masked(HWIO_TLMM_GPIO_HS_I2C_CTL_ADDR(x), HWIO_TLMM_GPIO_HS_I2C_CTL_RMSK)
#define HWIO_TLMM_GPIO_HS_I2C_CTL_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_GPIO_HS_I2C_CTL_ADDR(x), m)
#define HWIO_TLMM_GPIO_HS_I2C_CTL_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_HS_I2C_CTL_ADDR(x),v)
#define HWIO_TLMM_GPIO_HS_I2C_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_HS_I2C_CTL_ADDR(x),m,v,HWIO_TLMM_GPIO_HS_I2C_CTL_IN(x))
#define HWIO_TLMM_GPIO_HS_I2C_CTL_MODE_BMSK                                                        0x3
#define HWIO_TLMM_GPIO_HS_I2C_CTL_MODE_SHFT                                                        0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(base,n)                                      ((base) + 0x00106000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_RMSK                                                   0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_MAXn                                                       1
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_INI(base,n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(base,n), HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(base,n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_OUTI(base,n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(base,n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(base,n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_INI(base,n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_POLARITY_BMSK                                          0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_POLARITY_SHFT                                            0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_GPIO_SEL_BMSK                                           0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_GPIO_SEL_SHFT                                            0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(base,n)                                   ((base) + 0x00105000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_RMSK                                                0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_MAXn                                                    9
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INI(base,n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(base,n), HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(base,n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_OUTI(base,n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(base,n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(base,n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INI(base,n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_POLARITY_BMSK                                       0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_POLARITY_SHFT                                         0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_GPIO_SEL_BMSK                                        0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_GPIO_SEL_SHFT                                         0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(base,n)                                   ((base) + 0x00104000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_RMSK                                                0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_MAXn                                                    5
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INI(base,n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(base,n), HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(base,n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_OUTI(base,n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(base,n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(base,n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INI(base,n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_POLARITY_BMSK                                       0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_POLARITY_SHFT                                         0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_GPIO_SEL_BMSK                                        0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_GPIO_SEL_SHFT                                         0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(base,n)                                       ((base) + 0x00103000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_RMSK                                                    0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_MAXn                                                        0
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INI(base,n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(base,n), HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(base,n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_OUTI(base,n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(base,n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(base,n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INI(base,n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_POLARITY_BMSK                                           0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_POLARITY_SHFT                                             0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_GPIO_SEL_BMSK                                            0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_GPIO_SEL_SHFT                                             0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(base,n)                                      ((base) + 0x00102000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_RMSK                                                   0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_MAXn                                                       7
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_INI(base,n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(base,n), HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(base,n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_OUTI(base,n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(base,n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(base,n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_INI(base,n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_POLARITY_BMSK                                          0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_POLARITY_SHFT                                            0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_GPIO_SEL_BMSK                                           0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_GPIO_SEL_SHFT                                            0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(base,n)                                       ((base) + 0x00101000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_RMSK                                                    0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_MAXn                                                        1
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INI(base,n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(base,n), HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(base,n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_OUTI(base,n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(base,n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(base,n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INI(base,n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_POLARITY_BMSK                                           0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_POLARITY_SHFT                                             0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_GPIO_SEL_BMSK                                            0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_GPIO_SEL_SHFT                                             0x0

#define HWIO_TLMM_GPIO_OUT_0_ADDR(x)                                                        ((x) + 0x00200000)
#define HWIO_TLMM_GPIO_OUT_0_RMSK                                                           0xffffffff
#define HWIO_TLMM_GPIO_OUT_0_IN(x)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_0_ADDR(x), HWIO_TLMM_GPIO_OUT_0_RMSK)
#define HWIO_TLMM_GPIO_OUT_0_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_0_ADDR(x), m)
#define HWIO_TLMM_GPIO_OUT_0_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_0_ADDR(x),v)
#define HWIO_TLMM_GPIO_OUT_0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_0_ADDR(x),m,v,HWIO_TLMM_GPIO_OUT_0_IN(x))
#define HWIO_TLMM_GPIO_OUT_0_GPIO_OUT_BMSK                                                  0xffffffff
#define HWIO_TLMM_GPIO_OUT_0_GPIO_OUT_SHFT                                                         0x0

#define HWIO_TLMM_GPIO_OUT_1_ADDR(x)                                                        ((x) + 0x00200004)
#define HWIO_TLMM_GPIO_OUT_1_RMSK                                                           0xffffffff
#define HWIO_TLMM_GPIO_OUT_1_IN(x)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_1_ADDR(x), HWIO_TLMM_GPIO_OUT_1_RMSK)
#define HWIO_TLMM_GPIO_OUT_1_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_1_ADDR(x), m)
#define HWIO_TLMM_GPIO_OUT_1_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_1_ADDR(x),v)
#define HWIO_TLMM_GPIO_OUT_1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_1_ADDR(x),m,v,HWIO_TLMM_GPIO_OUT_1_IN(x))
#define HWIO_TLMM_GPIO_OUT_1_GPIO_OUT_BMSK                                                  0xffffffff
#define HWIO_TLMM_GPIO_OUT_1_GPIO_OUT_SHFT                                                         0x0

#define HWIO_TLMM_GPIO_OUT_2_ADDR(x)                                                        ((x) + 0x00200008)
#define HWIO_TLMM_GPIO_OUT_2_RMSK                                                           0xffffffff
#define HWIO_TLMM_GPIO_OUT_2_IN(x)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_2_ADDR(x), HWIO_TLMM_GPIO_OUT_2_RMSK)
#define HWIO_TLMM_GPIO_OUT_2_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_2_ADDR(x), m)
#define HWIO_TLMM_GPIO_OUT_2_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_2_ADDR(x),v)
#define HWIO_TLMM_GPIO_OUT_2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_2_ADDR(x),m,v,HWIO_TLMM_GPIO_OUT_2_IN(x))
#define HWIO_TLMM_GPIO_OUT_2_GPIO_OUT_BMSK                                                  0xffffffff
#define HWIO_TLMM_GPIO_OUT_2_GPIO_OUT_SHFT                                                         0x0

#define HWIO_TLMM_GPIO_OUT_3_ADDR(x)                                                        ((x) + 0x0020000c)
#define HWIO_TLMM_GPIO_OUT_3_RMSK                                                            0x3ffffff
#define HWIO_TLMM_GPIO_OUT_3_IN(x)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_3_ADDR(x), HWIO_TLMM_GPIO_OUT_3_RMSK)
#define HWIO_TLMM_GPIO_OUT_3_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_3_ADDR(x), m)
#define HWIO_TLMM_GPIO_OUT_3_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_3_ADDR(x),v)
#define HWIO_TLMM_GPIO_OUT_3_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_3_ADDR(x),m,v,HWIO_TLMM_GPIO_OUT_3_IN(x))
#define HWIO_TLMM_GPIO_OUT_3_GPIO_OUT_BMSK                                                   0x3ffffff
#define HWIO_TLMM_GPIO_OUT_3_GPIO_OUT_SHFT                                                         0x0

#define HWIO_TLMM_GPIO_OUT_CLR_0_ADDR(x)                                                    ((x) + 0x00200020)
#define HWIO_TLMM_GPIO_OUT_CLR_0_RMSK                                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_0_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_0_ADDR(x),v)
#define HWIO_TLMM_GPIO_OUT_CLR_0_GPIO_OUT_CLR_BMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_0_GPIO_OUT_CLR_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_CLR_1_ADDR(x)                                                    ((x) + 0x00200024)
#define HWIO_TLMM_GPIO_OUT_CLR_1_RMSK                                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_1_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_1_ADDR(x),v)
#define HWIO_TLMM_GPIO_OUT_CLR_1_GPIO_OUT_CLR_BMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_1_GPIO_OUT_CLR_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_CLR_2_ADDR(x)                                                    ((x) + 0x00200028)
#define HWIO_TLMM_GPIO_OUT_CLR_2_RMSK                                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_2_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_2_ADDR(x),v)
#define HWIO_TLMM_GPIO_OUT_CLR_2_GPIO_OUT_CLR_BMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_2_GPIO_OUT_CLR_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_CLR_3_ADDR(x)                                                    ((x) + 0x0020002c)
#define HWIO_TLMM_GPIO_OUT_CLR_3_RMSK                                                        0x3ffffff
#define HWIO_TLMM_GPIO_OUT_CLR_3_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_3_ADDR(x),v)
#define HWIO_TLMM_GPIO_OUT_CLR_3_GPIO_OUT_CLR_BMSK                                           0x3ffffff
#define HWIO_TLMM_GPIO_OUT_CLR_3_GPIO_OUT_CLR_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_SET_0_ADDR(x)                                                    ((x) + 0x00200040)
#define HWIO_TLMM_GPIO_OUT_SET_0_RMSK                                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_0_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_0_ADDR(x),v)
#define HWIO_TLMM_GPIO_OUT_SET_0_GPIO_OUT_SET_BMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_0_GPIO_OUT_SET_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_SET_1_ADDR(x)                                                    ((x) + 0x00200044)
#define HWIO_TLMM_GPIO_OUT_SET_1_RMSK                                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_1_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_1_ADDR(x),v)
#define HWIO_TLMM_GPIO_OUT_SET_1_GPIO_OUT_SET_BMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_1_GPIO_OUT_SET_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_SET_2_ADDR(x)                                                    ((x) + 0x00200048)
#define HWIO_TLMM_GPIO_OUT_SET_2_RMSK                                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_2_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_2_ADDR(x),v)
#define HWIO_TLMM_GPIO_OUT_SET_2_GPIO_OUT_SET_BMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_2_GPIO_OUT_SET_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_SET_3_ADDR(x)                                                    ((x) + 0x0020004c)
#define HWIO_TLMM_GPIO_OUT_SET_3_RMSK                                                        0x3ffffff
#define HWIO_TLMM_GPIO_OUT_SET_3_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_3_ADDR(x),v)
#define HWIO_TLMM_GPIO_OUT_SET_3_GPIO_OUT_SET_BMSK                                           0x3ffffff
#define HWIO_TLMM_GPIO_OUT_SET_3_GPIO_OUT_SET_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_IN_0_ADDR(x)                                                         ((x) + 0x00200060)
#define HWIO_TLMM_GPIO_IN_0_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_IN_0_IN(x)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_0_ADDR(x), HWIO_TLMM_GPIO_IN_0_RMSK)
#define HWIO_TLMM_GPIO_IN_0_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_0_ADDR(x), m)
#define HWIO_TLMM_GPIO_IN_0_GPIO_IN_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_IN_0_GPIO_IN_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_IN_1_ADDR(x)                                                         ((x) + 0x00200064)
#define HWIO_TLMM_GPIO_IN_1_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_IN_1_IN(x)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_1_ADDR(x), HWIO_TLMM_GPIO_IN_1_RMSK)
#define HWIO_TLMM_GPIO_IN_1_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_1_ADDR(x), m)
#define HWIO_TLMM_GPIO_IN_1_GPIO_IN_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_IN_1_GPIO_IN_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_IN_2_ADDR(x)                                                         ((x) + 0x00200068)
#define HWIO_TLMM_GPIO_IN_2_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_IN_2_IN(x)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_2_ADDR(x), HWIO_TLMM_GPIO_IN_2_RMSK)
#define HWIO_TLMM_GPIO_IN_2_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_2_ADDR(x), m)
#define HWIO_TLMM_GPIO_IN_2_GPIO_IN_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_IN_2_GPIO_IN_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_IN_3_ADDR(x)                                                         ((x) + 0x0020006c)
#define HWIO_TLMM_GPIO_IN_3_RMSK                                                             0x3ffffff
#define HWIO_TLMM_GPIO_IN_3_IN(x)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_3_ADDR(x), HWIO_TLMM_GPIO_IN_3_RMSK)
#define HWIO_TLMM_GPIO_IN_3_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_3_ADDR(x), m)
#define HWIO_TLMM_GPIO_IN_3_GPIO_IN_BMSK                                                     0x3ffffff
#define HWIO_TLMM_GPIO_IN_3_GPIO_IN_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_OE_0_ADDR(x)                                                         ((x) + 0x00200080)
#define HWIO_TLMM_GPIO_OE_0_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_0_IN(x)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_0_ADDR(x), HWIO_TLMM_GPIO_OE_0_RMSK)
#define HWIO_TLMM_GPIO_OE_0_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_0_ADDR(x), m)
#define HWIO_TLMM_GPIO_OE_0_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OE_0_ADDR(x),v)
#define HWIO_TLMM_GPIO_OE_0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_0_ADDR(x),m,v,HWIO_TLMM_GPIO_OE_0_IN(x))
#define HWIO_TLMM_GPIO_OE_0_GPIO_OE_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_OE_0_GPIO_OE_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_OE_1_ADDR(x)                                                         ((x) + 0x00200084)
#define HWIO_TLMM_GPIO_OE_1_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_1_IN(x)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_1_ADDR(x), HWIO_TLMM_GPIO_OE_1_RMSK)
#define HWIO_TLMM_GPIO_OE_1_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_1_ADDR(x), m)
#define HWIO_TLMM_GPIO_OE_1_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OE_1_ADDR(x),v)
#define HWIO_TLMM_GPIO_OE_1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_1_ADDR(x),m,v,HWIO_TLMM_GPIO_OE_1_IN(x))
#define HWIO_TLMM_GPIO_OE_1_GPIO_OE_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_OE_1_GPIO_OE_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_OE_2_ADDR(x)                                                         ((x) + 0x00200088)
#define HWIO_TLMM_GPIO_OE_2_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_2_IN(x)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_2_ADDR(x), HWIO_TLMM_GPIO_OE_2_RMSK)
#define HWIO_TLMM_GPIO_OE_2_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_2_ADDR(x), m)
#define HWIO_TLMM_GPIO_OE_2_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OE_2_ADDR(x),v)
#define HWIO_TLMM_GPIO_OE_2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_2_ADDR(x),m,v,HWIO_TLMM_GPIO_OE_2_IN(x))
#define HWIO_TLMM_GPIO_OE_2_GPIO_OE_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_OE_2_GPIO_OE_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_OE_3_ADDR(x)                                                         ((x) + 0x0020008c)
#define HWIO_TLMM_GPIO_OE_3_RMSK                                                             0x3ffffff
#define HWIO_TLMM_GPIO_OE_3_IN(x)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_3_ADDR(x), HWIO_TLMM_GPIO_OE_3_RMSK)
#define HWIO_TLMM_GPIO_OE_3_INM(x, m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_3_ADDR(x), m)
#define HWIO_TLMM_GPIO_OE_3_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OE_3_ADDR(x),v)
#define HWIO_TLMM_GPIO_OE_3_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_3_ADDR(x),m,v,HWIO_TLMM_GPIO_OE_3_IN(x))
#define HWIO_TLMM_GPIO_OE_3_GPIO_OE_BMSK                                                     0x3ffffff
#define HWIO_TLMM_GPIO_OE_3_GPIO_OE_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_OE_CLR_0_ADDR(x)                                                     ((x) + 0x002000a0)
#define HWIO_TLMM_GPIO_OE_CLR_0_RMSK                                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_0_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_0_ADDR(x),v)
#define HWIO_TLMM_GPIO_OE_CLR_0_GPIO_OE_CLR_BMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_0_GPIO_OE_CLR_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_CLR_1_ADDR(x)                                                     ((x) + 0x002000a4)
#define HWIO_TLMM_GPIO_OE_CLR_1_RMSK                                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_1_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_1_ADDR(x),v)
#define HWIO_TLMM_GPIO_OE_CLR_1_GPIO_OE_CLR_BMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_1_GPIO_OE_CLR_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_CLR_2_ADDR(x)                                                     ((x) + 0x002000a8)
#define HWIO_TLMM_GPIO_OE_CLR_2_RMSK                                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_2_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_2_ADDR(x),v)
#define HWIO_TLMM_GPIO_OE_CLR_2_GPIO_OE_CLR_BMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_2_GPIO_OE_CLR_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_CLR_3_ADDR(x)                                                     ((x) + 0x002000ac)
#define HWIO_TLMM_GPIO_OE_CLR_3_RMSK                                                         0x3ffffff
#define HWIO_TLMM_GPIO_OE_CLR_3_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_3_ADDR(x),v)
#define HWIO_TLMM_GPIO_OE_CLR_3_GPIO_OE_CLR_BMSK                                             0x3ffffff
#define HWIO_TLMM_GPIO_OE_CLR_3_GPIO_OE_CLR_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_SET_0_ADDR(x)                                                     ((x) + 0x002000c0)
#define HWIO_TLMM_GPIO_OE_SET_0_RMSK                                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_0_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_0_ADDR(x),v)
#define HWIO_TLMM_GPIO_OE_SET_0_GPIO_OE_SET_BMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_0_GPIO_OE_SET_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_SET_1_ADDR(x)                                                     ((x) + 0x002000c4)
#define HWIO_TLMM_GPIO_OE_SET_1_RMSK                                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_1_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_1_ADDR(x),v)
#define HWIO_TLMM_GPIO_OE_SET_1_GPIO_OE_SET_BMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_1_GPIO_OE_SET_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_SET_2_ADDR(x)                                                     ((x) + 0x002000c8)
#define HWIO_TLMM_GPIO_OE_SET_2_RMSK                                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_2_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_2_ADDR(x),v)
#define HWIO_TLMM_GPIO_OE_SET_2_GPIO_OE_SET_BMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_2_GPIO_OE_SET_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_SET_3_ADDR(x)                                                     ((x) + 0x002000cc)
#define HWIO_TLMM_GPIO_OE_SET_3_RMSK                                                         0x3ffffff
#define HWIO_TLMM_GPIO_OE_SET_3_OUT(x, v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_3_ADDR(x),v)
#define HWIO_TLMM_GPIO_OE_SET_3_GPIO_OE_SET_BMSK                                             0x3ffffff
#define HWIO_TLMM_GPIO_OE_SET_3_GPIO_OE_SET_SHFT                                                   0x0


#endif /* __HALTLMMHWIO_H__ */
