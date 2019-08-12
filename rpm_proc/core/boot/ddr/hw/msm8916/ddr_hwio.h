
#ifndef __DDR_HWIO_H__
#define __DDR_HWIO_H__
/*
===========================================================================
*/
/**
  @file ddr_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    BIMC_MISC
    BIMC_S_DDR0_DPE
    BIMC_S_DDR0_SCMO
    BIMC_S_DDR0_SHKE
    DIM_C00_DDRPHY_CA
    DIM_D00_DDRPHY_DQ
    DIM_D01_DDRPHY_DQ
    DIM_D02_DDRPHY_DQ
    DIM_D03_DDRPHY_DQ
    EBI1_AHB2PHY_BROADCAST_SWMAN
    EBI1_AHB2PHY_SWMAN

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
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

  $Header: //components/rel/rpm.bf/2.1.1/core/boot/ddr/hw/msm8916/ddr_hwio.h#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $

  ===========================================================================
*/
#include "HALhwio.h"

/*----------------------------------------------------------------------------
 * Module base addresses
 *--------------------------------------------------------------------------*/
#define BIMC_BASE              0x60400000
#define EBI1_PHY_CFG_BASE      0x60480000

/*----------------------------------------------------------------------------
 * MODULE: BIMC_MISC
 *--------------------------------------------------------------------------*/

#define BIMC_MISC_REG_BASE                                                          (BIMC_BASE      + 0x00000000)

#define HWIO_BIMC_HW_VERSION_ADDR                                                   (BIMC_MISC_REG_BASE      + 0x00000000)
#define HWIO_BIMC_HW_VERSION_RMSK                                                   0xffffffff
#define HWIO_BIMC_HW_VERSION_IN          \
        in_dword_masked(HWIO_BIMC_HW_VERSION_ADDR, HWIO_BIMC_HW_VERSION_RMSK)
#define HWIO_BIMC_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_BIMC_HW_VERSION_ADDR, m)
#define HWIO_BIMC_HW_VERSION_MAJOR_BMSK                                             0xf0000000
#define HWIO_BIMC_HW_VERSION_MAJOR_SHFT                                                   0x1c
#define HWIO_BIMC_HW_VERSION_MINOR_BMSK                                              0xfff0000
#define HWIO_BIMC_HW_VERSION_MINOR_SHFT                                                   0x10
#define HWIO_BIMC_HW_VERSION_STEP_BMSK                                                  0xffff
#define HWIO_BIMC_HW_VERSION_STEP_SHFT                                                     0x0

#define HWIO_BIMC_CORE_INFO_ADDR                                                    (BIMC_MISC_REG_BASE      + 0x00000004)
#define HWIO_BIMC_CORE_INFO_RMSK                                                    0xffffffff
#define HWIO_BIMC_CORE_INFO_IN          \
        in_dword_masked(HWIO_BIMC_CORE_INFO_ADDR, HWIO_BIMC_CORE_INFO_RMSK)
#define HWIO_BIMC_CORE_INFO_INM(m)      \
        in_dword_masked(HWIO_BIMC_CORE_INFO_ADDR, m)
#define HWIO_BIMC_CORE_INFO_ID_BMSK                                                 0xffffffff
#define HWIO_BIMC_CORE_INFO_ID_SHFT                                                        0x0

#define HWIO_BIMC_HW_INFO_ADDR                                                      (BIMC_MISC_REG_BASE      + 0x00000008)
#define HWIO_BIMC_HW_INFO_RMSK                                                      0xffffffff
#define HWIO_BIMC_HW_INFO_IN          \
        in_dword_masked(HWIO_BIMC_HW_INFO_ADDR, HWIO_BIMC_HW_INFO_RMSK)
#define HWIO_BIMC_HW_INFO_INM(m)      \
        in_dword_masked(HWIO_BIMC_HW_INFO_ADDR, m)
#define HWIO_BIMC_HW_INFO_MAJOR_BMSK                                                0xff000000
#define HWIO_BIMC_HW_INFO_MAJOR_SHFT                                                      0x18
#define HWIO_BIMC_HW_INFO_BRANCH_BMSK                                                 0xff0000
#define HWIO_BIMC_HW_INFO_BRANCH_SHFT                                                     0x10
#define HWIO_BIMC_HW_INFO_MINOR_BMSK                                                    0xff00
#define HWIO_BIMC_HW_INFO_MINOR_SHFT                                                       0x8
#define HWIO_BIMC_HW_INFO_ECO_BMSK                                                        0xff
#define HWIO_BIMC_HW_INFO_ECO_SHFT                                                         0x0

#define HWIO_BIMC_CORE_CLK_SEL_ADDR                                                 (BIMC_MISC_REG_BASE      + 0x00000020)
#define HWIO_BIMC_CORE_CLK_SEL_RMSK                                                        0x3
#define HWIO_BIMC_CORE_CLK_SEL_IN          \
        in_dword_masked(HWIO_BIMC_CORE_CLK_SEL_ADDR, HWIO_BIMC_CORE_CLK_SEL_RMSK)
#define HWIO_BIMC_CORE_CLK_SEL_INM(m)      \
        in_dword_masked(HWIO_BIMC_CORE_CLK_SEL_ADDR, m)
#define HWIO_BIMC_CORE_CLK_SEL_OUT(v)      \
        out_dword(HWIO_BIMC_CORE_CLK_SEL_ADDR,v)
#define HWIO_BIMC_CORE_CLK_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_CORE_CLK_SEL_ADDR,m,v,HWIO_BIMC_CORE_CLK_SEL_IN)
#define HWIO_BIMC_CORE_CLK_SEL_CLK_2X_MODE_BMSK                                            0x2
#define HWIO_BIMC_CORE_CLK_SEL_CLK_2X_MODE_SHFT                                            0x1
#define HWIO_BIMC_CORE_CLK_SEL_CLK_2X_MODE_CTRL_EN_BMSK                                    0x1
#define HWIO_BIMC_CORE_CLK_SEL_CLK_2X_MODE_CTRL_EN_SHFT                                    0x0

#define HWIO_BIMC_CORE_CLK_PERIOD_ADDR                                              (BIMC_MISC_REG_BASE      + 0x00000024)
#define HWIO_BIMC_CORE_CLK_PERIOD_RMSK                                              0x1001ffff
#define HWIO_BIMC_CORE_CLK_PERIOD_IN          \
        in_dword_masked(HWIO_BIMC_CORE_CLK_PERIOD_ADDR, HWIO_BIMC_CORE_CLK_PERIOD_RMSK)
#define HWIO_BIMC_CORE_CLK_PERIOD_INM(m)      \
        in_dword_masked(HWIO_BIMC_CORE_CLK_PERIOD_ADDR, m)
#define HWIO_BIMC_CORE_CLK_PERIOD_OUT(v)      \
        out_dword(HWIO_BIMC_CORE_CLK_PERIOD_ADDR,v)
#define HWIO_BIMC_CORE_CLK_PERIOD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_CORE_CLK_PERIOD_ADDR,m,v,HWIO_BIMC_CORE_CLK_PERIOD_IN)
#define HWIO_BIMC_CORE_CLK_PERIOD_CLK_PERIOD_RESOLUTION_BMSK                        0x10000000
#define HWIO_BIMC_CORE_CLK_PERIOD_CLK_PERIOD_RESOLUTION_SHFT                              0x1c
#define HWIO_BIMC_CORE_CLK_PERIOD_PERIOD_BMSK                                          0x1ffff
#define HWIO_BIMC_CORE_CLK_PERIOD_PERIOD_SHFT                                              0x0

#define HWIO_BIMC_SCMOn_LOCAL_CGC_THRESHOLD_ADDR(n)                                 (BIMC_MISC_REG_BASE      + 0x00000100 + 0x4 * (n))
#define HWIO_BIMC_SCMOn_LOCAL_CGC_THRESHOLD_RMSK                                    0x100000ff
#define HWIO_BIMC_SCMOn_LOCAL_CGC_THRESHOLD_MAXn                                             0
#define HWIO_BIMC_SCMOn_LOCAL_CGC_THRESHOLD_INI(n)        \
        in_dword_masked(HWIO_BIMC_SCMOn_LOCAL_CGC_THRESHOLD_ADDR(n), HWIO_BIMC_SCMOn_LOCAL_CGC_THRESHOLD_RMSK)
#define HWIO_BIMC_SCMOn_LOCAL_CGC_THRESHOLD_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_SCMOn_LOCAL_CGC_THRESHOLD_ADDR(n), mask)
#define HWIO_BIMC_SCMOn_LOCAL_CGC_THRESHOLD_OUTI(n,val)    \
        out_dword(HWIO_BIMC_SCMOn_LOCAL_CGC_THRESHOLD_ADDR(n),val)
#define HWIO_BIMC_SCMOn_LOCAL_CGC_THRESHOLD_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_SCMOn_LOCAL_CGC_THRESHOLD_ADDR(n),mask,val,HWIO_BIMC_SCMOn_LOCAL_CGC_THRESHOLD_INI(n))
#define HWIO_BIMC_SCMOn_LOCAL_CGC_THRESHOLD_THRESHOLD_ENA_BMSK                      0x10000000
#define HWIO_BIMC_SCMOn_LOCAL_CGC_THRESHOLD_THRESHOLD_ENA_SHFT                            0x1c
#define HWIO_BIMC_SCMOn_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_BMSK                            0xff
#define HWIO_BIMC_SCMOn_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_SHFT                             0x0

#define HWIO_BIMC_DPEn_LOCAL_CGC_THRESHOLD_ADDR(n)                                  (BIMC_MISC_REG_BASE      + 0x00000140 + 0x4 * (n))
#define HWIO_BIMC_DPEn_LOCAL_CGC_THRESHOLD_RMSK                                     0x100000ff
#define HWIO_BIMC_DPEn_LOCAL_CGC_THRESHOLD_MAXn                                              0
#define HWIO_BIMC_DPEn_LOCAL_CGC_THRESHOLD_INI(n)        \
        in_dword_masked(HWIO_BIMC_DPEn_LOCAL_CGC_THRESHOLD_ADDR(n), HWIO_BIMC_DPEn_LOCAL_CGC_THRESHOLD_RMSK)
#define HWIO_BIMC_DPEn_LOCAL_CGC_THRESHOLD_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_DPEn_LOCAL_CGC_THRESHOLD_ADDR(n), mask)
#define HWIO_BIMC_DPEn_LOCAL_CGC_THRESHOLD_OUTI(n,val)    \
        out_dword(HWIO_BIMC_DPEn_LOCAL_CGC_THRESHOLD_ADDR(n),val)
#define HWIO_BIMC_DPEn_LOCAL_CGC_THRESHOLD_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_DPEn_LOCAL_CGC_THRESHOLD_ADDR(n),mask,val,HWIO_BIMC_DPEn_LOCAL_CGC_THRESHOLD_INI(n))
#define HWIO_BIMC_DPEn_LOCAL_CGC_THRESHOLD_THRESHOLD_ENA_BMSK                       0x10000000
#define HWIO_BIMC_DPEn_LOCAL_CGC_THRESHOLD_THRESHOLD_ENA_SHFT                             0x1c
#define HWIO_BIMC_DPEn_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_BMSK                             0xff
#define HWIO_BIMC_DPEn_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_SHFT                              0x0

#define HWIO_BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_ADDR                        (BIMC_MISC_REG_BASE      + 0x00000180)
#define HWIO_BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_RMSK                        0x100000ff
#define HWIO_BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_IN          \
        in_dword_masked(HWIO_BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_ADDR, HWIO_BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_RMSK)
#define HWIO_BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_INM(m)      \
        in_dword_masked(HWIO_BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_ADDR, m)
#define HWIO_BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_OUT(v)      \
        out_dword(HWIO_BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_ADDR,v)
#define HWIO_BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_ADDR,m,v,HWIO_BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_IN)
#define HWIO_BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_THRESHOLD_ENA_BMSK          0x10000000
#define HWIO_BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_THRESHOLD_ENA_SHFT                0x1c
#define HWIO_BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_BMSK                0xff
#define HWIO_BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_SHFT                 0x0

#define HWIO_BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_ADDR(n)                            (BIMC_MISC_REG_BASE      + 0x000001c0 + 0x4 * (n))
#define HWIO_BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_RMSK                               0x100000ff
#define HWIO_BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_MAXn                                        6
#define HWIO_BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_INI(n)        \
        in_dword_masked(HWIO_BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_ADDR(n), HWIO_BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_RMSK)
#define HWIO_BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_ADDR(n), mask)
#define HWIO_BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_OUTI(n,val)    \
        out_dword(HWIO_BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_ADDR(n),val)
#define HWIO_BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_ADDR(n),mask,val,HWIO_BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_INI(n))
#define HWIO_BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_THRESHOLD_ENA_BMSK                 0x10000000
#define HWIO_BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_THRESHOLD_ENA_SHFT                       0x1c
#define HWIO_BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_BMSK                       0xff
#define HWIO_BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_SHFT                        0x0

#define HWIO_BIMC_SWAY_LOCAL_CGC_THRESHOLD_ADDR                                     (BIMC_MISC_REG_BASE      + 0x00000200)
#define HWIO_BIMC_SWAY_LOCAL_CGC_THRESHOLD_RMSK                                     0x100000ff
#define HWIO_BIMC_SWAY_LOCAL_CGC_THRESHOLD_IN          \
        in_dword_masked(HWIO_BIMC_SWAY_LOCAL_CGC_THRESHOLD_ADDR, HWIO_BIMC_SWAY_LOCAL_CGC_THRESHOLD_RMSK)
#define HWIO_BIMC_SWAY_LOCAL_CGC_THRESHOLD_INM(m)      \
        in_dword_masked(HWIO_BIMC_SWAY_LOCAL_CGC_THRESHOLD_ADDR, m)
#define HWIO_BIMC_SWAY_LOCAL_CGC_THRESHOLD_OUT(v)      \
        out_dword(HWIO_BIMC_SWAY_LOCAL_CGC_THRESHOLD_ADDR,v)
#define HWIO_BIMC_SWAY_LOCAL_CGC_THRESHOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_SWAY_LOCAL_CGC_THRESHOLD_ADDR,m,v,HWIO_BIMC_SWAY_LOCAL_CGC_THRESHOLD_IN)
#define HWIO_BIMC_SWAY_LOCAL_CGC_THRESHOLD_THRESHOLD_ENA_BMSK                       0x10000000
#define HWIO_BIMC_SWAY_LOCAL_CGC_THRESHOLD_THRESHOLD_ENA_SHFT                             0x1c
#define HWIO_BIMC_SWAY_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_BMSK                             0xff
#define HWIO_BIMC_SWAY_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_SHFT                              0x0

#define HWIO_BIMC_SCMOn_CGC_CFG_ADDR(n)                                             (BIMC_MISC_REG_BASE      + 0x00000240 + 0x4 * (n))
#define HWIO_BIMC_SCMOn_CGC_CFG_RMSK                                                0x110000ff
#define HWIO_BIMC_SCMOn_CGC_CFG_MAXn                                                         0
#define HWIO_BIMC_SCMOn_CGC_CFG_INI(n)        \
        in_dword_masked(HWIO_BIMC_SCMOn_CGC_CFG_ADDR(n), HWIO_BIMC_SCMOn_CGC_CFG_RMSK)
#define HWIO_BIMC_SCMOn_CGC_CFG_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_SCMOn_CGC_CFG_ADDR(n), mask)
#define HWIO_BIMC_SCMOn_CGC_CFG_OUTI(n,val)    \
        out_dword(HWIO_BIMC_SCMOn_CGC_CFG_ADDR(n),val)
#define HWIO_BIMC_SCMOn_CGC_CFG_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_SCMOn_CGC_CFG_ADDR(n),mask,val,HWIO_BIMC_SCMOn_CGC_CFG_INI(n))
#define HWIO_BIMC_SCMOn_CGC_CFG_CLK_DIS_BMSK                                        0x10000000
#define HWIO_BIMC_SCMOn_CGC_CFG_CLK_DIS_SHFT                                              0x1c
#define HWIO_BIMC_SCMOn_CGC_CFG_THRESHOLD_ENA_BMSK                                   0x1000000
#define HWIO_BIMC_SCMOn_CGC_CFG_THRESHOLD_ENA_SHFT                                        0x18
#define HWIO_BIMC_SCMOn_CGC_CFG_THRESHOLD_VAL_BMSK                                        0xff
#define HWIO_BIMC_SCMOn_CGC_CFG_THRESHOLD_VAL_SHFT                                         0x0

#define HWIO_BIMC_DPEn_CGC_CFG_ADDR(n)                                              (BIMC_MISC_REG_BASE      + 0x00000280 + 0x4 * (n))
#define HWIO_BIMC_DPEn_CGC_CFG_RMSK                                                 0x110000ff
#define HWIO_BIMC_DPEn_CGC_CFG_MAXn                                                          0
#define HWIO_BIMC_DPEn_CGC_CFG_INI(n)        \
        in_dword_masked(HWIO_BIMC_DPEn_CGC_CFG_ADDR(n), HWIO_BIMC_DPEn_CGC_CFG_RMSK)
#define HWIO_BIMC_DPEn_CGC_CFG_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_DPEn_CGC_CFG_ADDR(n), mask)
#define HWIO_BIMC_DPEn_CGC_CFG_OUTI(n,val)    \
        out_dword(HWIO_BIMC_DPEn_CGC_CFG_ADDR(n),val)
#define HWIO_BIMC_DPEn_CGC_CFG_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_DPEn_CGC_CFG_ADDR(n),mask,val,HWIO_BIMC_DPEn_CGC_CFG_INI(n))
#define HWIO_BIMC_DPEn_CGC_CFG_CLK_DIS_BMSK                                         0x10000000
#define HWIO_BIMC_DPEn_CGC_CFG_CLK_DIS_SHFT                                               0x1c
#define HWIO_BIMC_DPEn_CGC_CFG_THRESHOLD_ENA_BMSK                                    0x1000000
#define HWIO_BIMC_DPEn_CGC_CFG_THRESHOLD_ENA_SHFT                                         0x18
#define HWIO_BIMC_DPEn_CGC_CFG_THRESHOLD_VAL_BMSK                                         0xff
#define HWIO_BIMC_DPEn_CGC_CFG_THRESHOLD_VAL_SHFT                                          0x0

#define HWIO_BIMC_CALIBRATION_CFG_ADDR                                              (BIMC_MISC_REG_BASE      + 0x00000400)
#define HWIO_BIMC_CALIBRATION_CFG_RMSK                                              0x10ffffff
#define HWIO_BIMC_CALIBRATION_CFG_IN          \
        in_dword_masked(HWIO_BIMC_CALIBRATION_CFG_ADDR, HWIO_BIMC_CALIBRATION_CFG_RMSK)
#define HWIO_BIMC_CALIBRATION_CFG_INM(m)      \
        in_dword_masked(HWIO_BIMC_CALIBRATION_CFG_ADDR, m)
#define HWIO_BIMC_CALIBRATION_CFG_OUT(v)      \
        out_dword(HWIO_BIMC_CALIBRATION_CFG_ADDR,v)
#define HWIO_BIMC_CALIBRATION_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_CALIBRATION_CFG_ADDR,m,v,HWIO_BIMC_CALIBRATION_CFG_IN)
#define HWIO_BIMC_CALIBRATION_CFG_RAPID_CAL_ENA_BMSK                                0x10000000
#define HWIO_BIMC_CALIBRATION_CFG_RAPID_CAL_ENA_SHFT                                      0x1c
#define HWIO_BIMC_CALIBRATION_CFG_COUNT_VAL_BMSK                                      0xffffff
#define HWIO_BIMC_CALIBRATION_CFG_COUNT_VAL_SHFT                                           0x0

#define HWIO_BIMC_DFIn_LP_CFG_ADDR(n)                                               (BIMC_MISC_REG_BASE      + 0x00000600 + 0x4 * (n))
#define HWIO_BIMC_DFIn_LP_CFG_RMSK                                                      0xf11f
#define HWIO_BIMC_DFIn_LP_CFG_MAXn                                                           0
#define HWIO_BIMC_DFIn_LP_CFG_INI(n)        \
        in_dword_masked(HWIO_BIMC_DFIn_LP_CFG_ADDR(n), HWIO_BIMC_DFIn_LP_CFG_RMSK)
#define HWIO_BIMC_DFIn_LP_CFG_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_DFIn_LP_CFG_ADDR(n), mask)
#define HWIO_BIMC_DFIn_LP_CFG_OUTI(n,val)    \
        out_dword(HWIO_BIMC_DFIn_LP_CFG_ADDR(n),val)
#define HWIO_BIMC_DFIn_LP_CFG_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_DFIn_LP_CFG_ADDR(n),mask,val,HWIO_BIMC_DFIn_LP_CFG_INI(n))
#define HWIO_BIMC_DFIn_LP_CFG_RFU_BMSK                                                  0xf000
#define HWIO_BIMC_DFIn_LP_CFG_RFU_SHFT                                                     0xc
#define HWIO_BIMC_DFIn_LP_CFG_LP_ACK_BMSK                                                0x100
#define HWIO_BIMC_DFIn_LP_CFG_LP_ACK_SHFT                                                  0x8
#define HWIO_BIMC_DFIn_LP_CFG_LP_REQ_BMSK                                                 0x10
#define HWIO_BIMC_DFIn_LP_CFG_LP_REQ_SHFT                                                  0x4
#define HWIO_BIMC_DFIn_LP_CFG_LP_WAKEUP_BMSK                                               0xf
#define HWIO_BIMC_DFIn_LP_CFG_LP_WAKEUP_SHFT                                               0x0

#define HWIO_BIMC_HFSC_CFG_ADDR                                                     (BIMC_MISC_REG_BASE      + 0x00000800)
#define HWIO_BIMC_HFSC_CFG_RMSK                                                            0x1
#define HWIO_BIMC_HFSC_CFG_IN          \
        in_dword_masked(HWIO_BIMC_HFSC_CFG_ADDR, HWIO_BIMC_HFSC_CFG_RMSK)
#define HWIO_BIMC_HFSC_CFG_INM(m)      \
        in_dword_masked(HWIO_BIMC_HFSC_CFG_ADDR, m)
#define HWIO_BIMC_HFSC_CFG_OUT(v)      \
        out_dword(HWIO_BIMC_HFSC_CFG_ADDR,v)
#define HWIO_BIMC_HFSC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_HFSC_CFG_ADDR,m,v,HWIO_BIMC_HFSC_CFG_IN)
#define HWIO_BIMC_HFSC_CFG_DISABLE_BMSK                                                    0x1
#define HWIO_BIMC_HFSC_CFG_DISABLE_SHFT                                                    0x0

#define HWIO_BIMC_HFSC_STATUS_ADDR                                                  (BIMC_MISC_REG_BASE      + 0x0000080c)
#define HWIO_BIMC_HFSC_STATUS_RMSK                                                         0x1
#define HWIO_BIMC_HFSC_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_HFSC_STATUS_ADDR, HWIO_BIMC_HFSC_STATUS_RMSK)
#define HWIO_BIMC_HFSC_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_HFSC_STATUS_ADDR, m)
#define HWIO_BIMC_HFSC_STATUS_BUSY_BMSK                                                    0x1
#define HWIO_BIMC_HFSC_STATUS_BUSY_SHFT                                                    0x0

#define HWIO_BIMC_HFSC_STAGE1_CTRL_ADDR                                             (BIMC_MISC_REG_BASE      + 0x00000840)
#define HWIO_BIMC_HFSC_STAGE1_CTRL_RMSK                                                   0x11
#define HWIO_BIMC_HFSC_STAGE1_CTRL_IN          \
        in_dword_masked(HWIO_BIMC_HFSC_STAGE1_CTRL_ADDR, HWIO_BIMC_HFSC_STAGE1_CTRL_RMSK)
#define HWIO_BIMC_HFSC_STAGE1_CTRL_INM(m)      \
        in_dword_masked(HWIO_BIMC_HFSC_STAGE1_CTRL_ADDR, m)
#define HWIO_BIMC_HFSC_STAGE1_CTRL_OUT(v)      \
        out_dword(HWIO_BIMC_HFSC_STAGE1_CTRL_ADDR,v)
#define HWIO_BIMC_HFSC_STAGE1_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_HFSC_STAGE1_CTRL_ADDR,m,v,HWIO_BIMC_HFSC_STAGE1_CTRL_IN)
#define HWIO_BIMC_HFSC_STAGE1_CTRL_ACK_BMSK                                               0x10
#define HWIO_BIMC_HFSC_STAGE1_CTRL_ACK_SHFT                                                0x4
#define HWIO_BIMC_HFSC_STAGE1_CTRL_REQ_BMSK                                                0x1
#define HWIO_BIMC_HFSC_STAGE1_CTRL_REQ_SHFT                                                0x0

#define HWIO_BIMC_HFSC_STAGE2_CTRL_ADDR                                             (BIMC_MISC_REG_BASE      + 0x00000860)
#define HWIO_BIMC_HFSC_STAGE2_CTRL_RMSK                                                   0x11
#define HWIO_BIMC_HFSC_STAGE2_CTRL_IN          \
        in_dword_masked(HWIO_BIMC_HFSC_STAGE2_CTRL_ADDR, HWIO_BIMC_HFSC_STAGE2_CTRL_RMSK)
#define HWIO_BIMC_HFSC_STAGE2_CTRL_INM(m)      \
        in_dword_masked(HWIO_BIMC_HFSC_STAGE2_CTRL_ADDR, m)
#define HWIO_BIMC_HFSC_STAGE2_CTRL_OUT(v)      \
        out_dword(HWIO_BIMC_HFSC_STAGE2_CTRL_ADDR,v)
#define HWIO_BIMC_HFSC_STAGE2_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_HFSC_STAGE2_CTRL_ADDR,m,v,HWIO_BIMC_HFSC_STAGE2_CTRL_IN)
#define HWIO_BIMC_HFSC_STAGE2_CTRL_ACK_BMSK                                               0x10
#define HWIO_BIMC_HFSC_STAGE2_CTRL_ACK_SHFT                                                0x4
#define HWIO_BIMC_HFSC_STAGE2_CTRL_REQ_BMSK                                                0x1
#define HWIO_BIMC_HFSC_STAGE2_CTRL_REQ_SHFT                                                0x0

/*----------------------------------------------------------------------------
 * MODULE: BIMC_S_DDR0_DPE
 *--------------------------------------------------------------------------*/

#define BIMC_S_DDR0_DPE_REG_BASE                                                      (BIMC_BASE      + 0x0004c000)

#define HWIO_BIMC_S_DDR0_DPE_HW_INFO_ADDR                                             (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000000)
#define HWIO_BIMC_S_DDR0_DPE_HW_INFO_RMSK                                             0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_HW_INFO_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_HW_INFO_ADDR, HWIO_BIMC_S_DDR0_DPE_HW_INFO_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_HW_INFO_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_HW_INFO_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_HW_INFO_MAJOR_REVISION_BMSK                              0xff000000
#define HWIO_BIMC_S_DDR0_DPE_HW_INFO_MAJOR_REVISION_SHFT                                    0x18
#define HWIO_BIMC_S_DDR0_DPE_HW_INFO_BRANCH_REVISION_BMSK                               0xff0000
#define HWIO_BIMC_S_DDR0_DPE_HW_INFO_BRANCH_REVISION_SHFT                                   0x10
#define HWIO_BIMC_S_DDR0_DPE_HW_INFO_MINOR_REVISION_BMSK                                  0xff00
#define HWIO_BIMC_S_DDR0_DPE_HW_INFO_MINOR_REVISION_SHFT                                     0x8
#define HWIO_BIMC_S_DDR0_DPE_HW_INFO_ECO_REVISION_BMSK                                      0xff
#define HWIO_BIMC_S_DDR0_DPE_HW_INFO_ECO_REVISION_SHFT                                       0x0

#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_ADDR                                            (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000010)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_RMSK                                            0x31130f11
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CONFIG_0_ADDR, HWIO_BIMC_S_DDR0_DPE_CONFIG_0_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CONFIG_0_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_CONFIG_0_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_CONFIG_0_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_CONFIG_0_IN)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_CA_BUS_OE_CTRL_BMSK                             0x30000000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_CA_BUS_OE_CTRL_SHFT                                   0x1c
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_RD_BEAT_NDX_OOO_BMSK                             0x1000000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_RD_BEAT_NDX_OOO_SHFT                                  0x18
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_DDR_WR_MASK_POLARITY_BMSK                         0x100000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_DDR_WR_MASK_POLARITY_SHFT                             0x14
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_DEVICE_TYPE_BMSK                                   0x30000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_DEVICE_TYPE_SHFT                                      0x10
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_DEVICE_CFG_RANK1_BMSK                                0xc00
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_DEVICE_CFG_RANK1_SHFT                                  0xa
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_DEVICE_CFG_RANK0_BMSK                                0x300
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_DEVICE_CFG_RANK0_SHFT                                  0x8
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_SHARED_CLK_BMSK                                       0x10
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_SHARED_CLK_SHFT                                        0x4
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_DDR_CMD_BMSK                                           0x1
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_0_DDR_CMD_SHFT                                           0x0

#define HWIO_BIMC_S_DDR0_DPE_CONFIG_1_ADDR                                            (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000014)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_1_RMSK                                            0xff111f1f
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CONFIG_1_ADDR, HWIO_BIMC_S_DDR0_DPE_CONFIG_1_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CONFIG_1_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_1_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_CONFIG_1_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_CONFIG_1_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_CONFIG_1_IN)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_1_PERIOD_BUS_THRESHOLD_POWER_MODE_BMSK            0xff000000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_1_PERIOD_BUS_THRESHOLD_POWER_MODE_SHFT                  0x18
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_1_PAD_POWER_MODE_CTRL_BMSK                          0x100000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_1_PAD_POWER_MODE_CTRL_SHFT                              0x14
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_1_PAD_POWER_MODE_BMSK                                0x10000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_1_PAD_POWER_MODE_SHFT                                   0x10
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_1_NUM_BANKS_RANK1_BMSK                                0x1f00
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_1_NUM_BANKS_RANK1_SHFT                                   0x8
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_1_NUM_BANKS_RANK0_BMSK                                  0x1f
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_1_NUM_BANKS_RANK0_SHFT                                   0x0

#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_ADDR                                            (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000018)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_RMSK                                            0x31f11f33
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CONFIG_2_ADDR, HWIO_BIMC_S_DDR0_DPE_CONFIG_2_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CONFIG_2_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_CONFIG_2_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_CONFIG_2_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_CONFIG_2_IN)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_TIMING_MODE_BMSK                                0x30000000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_TIMING_MODE_SHFT                                      0x1c
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_DDR_BURST_LEN_BMSK                               0x1f00000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_DDR_BURST_LEN_SHFT                                    0x14
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_WR_DQS_EARLY_BMSK                                  0x10000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_WR_DQS_EARLY_SHFT                                     0x10
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_DDR_CK_ON_DBG_BMSK                                  0x1000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_DDR_CK_ON_DBG_SHFT                                     0xc
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_MRR_BURST_LEN_BMSK                                   0xf00
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_MRR_BURST_LEN_SHFT                                     0x8
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_BURST_RD_START_BMSK                                   0x30
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_BURST_RD_START_SHFT                                    0x4
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_BURST_WR_START_BMSK                                    0x3
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_2_BURST_WR_START_SHFT                                    0x0

#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_ADDR                                            (BIMC_S_DDR0_DPE_REG_BASE      + 0x0000001c)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_RMSK                                            0x171f7113
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CONFIG_3_ADDR, HWIO_BIMC_S_DDR0_DPE_CONFIG_3_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CONFIG_3_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_CONFIG_3_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_CONFIG_3_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_CONFIG_3_IN)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_DIST_LOAD_CTRL_BMSK                             0x10000000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_DIST_LOAD_CTRL_SHFT                                   0x1c
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_REF_DEFICIT_WTRMRK_BMSK                          0x7000000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_REF_DEFICIT_WTRMRK_SHFT                               0x18
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_TREFBW_XO_DIV_6_CYCLES_BMSK                       0x1f0000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_TREFBW_XO_DIV_6_CYCLES_SHFT                           0x10
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_MAX_REFRESHES_TREFBW_BMSK                           0x7000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_MAX_REFRESHES_TREFBW_SHFT                              0xc
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_DRAIN_CQ_ON_SHKE_CMD_BMSK                            0x100
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_DRAIN_CQ_ON_SHKE_CMD_SHFT                              0x8
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_LOAD_TIMING_ON_FREQ_CHNG_BMSK                         0x10
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_LOAD_TIMING_ON_FREQ_CHNG_SHFT                          0x4
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_HW_FREQ_SW_SR_ONLY_BMSK                                0x2
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_HW_FREQ_SW_SR_ONLY_SHFT                                0x1
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_HW_FREQ_SWITCH_BMSK                                    0x1
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_3_HW_FREQ_SWITCH_SHFT                                    0x0

#define HWIO_BIMC_S_DDR0_DPE_CONFIG_4_ADDR                                            (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000020)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_4_RMSK                                                  0x11
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_4_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_CONFIG_4_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_4_RECALC_PS_PARAMS_BMSK                                 0x10
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_4_RECALC_PS_PARAMS_SHFT                                  0x4
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_4_LOAD_ALL_CONFIG_BMSK                                   0x1
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_4_LOAD_ALL_CONFIG_SHFT                                   0x0

#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_ADDR                                            (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000024)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_RMSK                                            0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CONFIG_5_ADDR, HWIO_BIMC_S_DDR0_DPE_CONFIG_5_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CONFIG_5_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_CONFIG_5_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_CONFIG_5_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_CONFIG_5_IN)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_ODTOFF_MIN_IN_PS_BMSK                           0x80000000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_ODTOFF_MIN_IN_PS_SHFT                                 0x1f
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_ODTOFF_MAX_IN_PS_BMSK                           0x40000000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_ODTOFF_MAX_IN_PS_SHFT                                 0x1e
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_ODTON_MIN_IN_PS_BMSK                            0x20000000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_ODTON_MIN_IN_PS_SHFT                                  0x1d
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_ODTON_MAX_IN_PS_BMSK                            0x10000000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_ODTON_MAX_IN_PS_SHFT                                  0x1c
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TRASMIN_IN_PS_BMSK                               0x8000000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TRASMIN_IN_PS_SHFT                                    0x1b
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TWR_IN_PS_BMSK                                   0x4000000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TWR_IN_PS_SHFT                                        0x1a
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TRCD_IN_PS_BMSK                                  0x2000000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TRCD_IN_PS_SHFT                                       0x19
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TWTR_IN_PS_BMSK                                  0x1000000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TWTR_IN_PS_SHFT                                       0x18
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TRRD_IN_PS_BMSK                                   0x800000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TRRD_IN_PS_SHFT                                       0x17
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TRFCAB_IN_PS_BMSK                                 0x400000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TRFCAB_IN_PS_SHFT                                     0x16
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TRFCPB_IN_PS_BMSK                                 0x200000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TRFCPB_IN_PS_SHFT                                     0x15
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TRTP_IN_PS_BMSK                                   0x100000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TRTP_IN_PS_SHFT                                       0x14
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TRPAB_IN_PS_BMSK                                   0x80000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TRPAB_IN_PS_SHFT                                      0x13
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TRPPB_IN_PS_BMSK                                   0x40000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TRPPB_IN_PS_SHFT                                      0x12
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TFAW_IN_PS_BMSK                                    0x20000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TFAW_IN_PS_SHFT                                       0x11
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TCKE_IN_PS_BMSK                                    0x10000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TCKE_IN_PS_SHFT                                       0x10
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TZQCS_IN_PS_BMSK                                    0x8000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TZQCS_IN_PS_SHFT                                       0xf
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TZQCL_IN_PS_BMSK                                    0x4000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TZQCL_IN_PS_SHFT                                       0xe
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TMOD_IN_PS_BMSK                                     0x2000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TMOD_IN_PS_SHFT                                        0xd
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_MIN_SR_DURATION_IN_PS_BMSK                          0x1000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_MIN_SR_DURATION_IN_PS_SHFT                             0xc
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TXSRD_IN_PS_BMSK                                     0x800
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TXSRD_IN_PS_SHFT                                       0xb
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TXSNR_IN_PS_BMSK                                     0x400
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TXSNR_IN_PS_SHFT                                       0xa
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TXPNR_ACT_PWR_DN_IN_PS_BMSK                          0x200
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TXPNR_ACT_PWR_DN_IN_PS_SHFT                            0x9
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TXPR_ACT_PWR_DN_IN_PS_BMSK                           0x100
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TXPR_ACT_PWR_DN_IN_PS_SHFT                             0x8
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TXPNR_PCHG_PWR_DN_IN_PS_BMSK                          0x80
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TXPNR_PCHG_PWR_DN_IN_PS_SHFT                           0x7
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TXPR_PCHG_PWR_DN_IN_PS_BMSK                           0x40
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_TXPR_PCHG_PWR_DN_IN_PS_SHFT                            0x6
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_CLK_AFTER_SR_ENTRY_IN_PS_BMSK                         0x20
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_CLK_AFTER_SR_ENTRY_IN_PS_SHFT                          0x5
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_CLK_BEFORE_SR_EXIT_IN_PS_BMSK                         0x10
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_CLK_BEFORE_SR_EXIT_IN_PS_SHFT                          0x4
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_IE_WINDOW_START_IN_PS_BMSK                             0x8
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_IE_WINDOW_START_IN_PS_SHFT                             0x3
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_IE_WINDOW_END_IN_PS_BMSK                               0x4
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_IE_WINDOW_END_IN_PS_SHFT                               0x2
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_RD_CAPTURE_START_IN_PS_BMSK                            0x2
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_RD_CAPTURE_START_IN_PS_SHFT                            0x1
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_RD_CAPTURE_END_IN_PS_BMSK                              0x1
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_5_RD_CAPTURE_END_IN_PS_SHFT                              0x0

#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_ADDR                                            (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000028)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_RMSK                                                0x13f7
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CONFIG_6_ADDR, HWIO_BIMC_S_DDR0_DPE_CONFIG_6_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CONFIG_6_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_CONFIG_6_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_CONFIG_6_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_CONFIG_6_IN)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_DRAM_RD_BURST_ORDER_BMSK                            0x1000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_DRAM_RD_BURST_ORDER_SHFT                               0xc
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_RANK1_BLK_ACT_BMSK                                   0x200
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_RANK1_BLK_ACT_SHFT                                     0x9
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_RANK0_BLK_ACT_BMSK                                   0x100
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_RANK0_BLK_ACT_SHFT                                     0x8
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_IOSTAGE_ODT_DEBUG_MODE_BMSK                           0x80
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_IOSTAGE_ODT_DEBUG_MODE_SHFT                            0x7
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_IOSTAGE_WR_DEBUG_MODE_BMSK                            0x40
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_IOSTAGE_WR_DEBUG_MODE_SHFT                             0x6
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_IOSTAGE_RD_DEBUG_MODE_BMSK                            0x20
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_IOSTAGE_RD_DEBUG_MODE_SHFT                             0x5
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_IOSTAGE_CA_DEBUG_MODE_BMSK                            0x10
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_IOSTAGE_CA_DEBUG_MODE_SHFT                             0x4
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_CDC_WR_DEBUG_MODE_BMSK                                 0x4
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_CDC_WR_DEBUG_MODE_SHFT                                 0x2
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_CDC_RD_DEBUG_MODE_BMSK                                 0x2
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_CDC_RD_DEBUG_MODE_SHFT                                 0x1
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_CDC_CA_DEBUG_MODE_BMSK                                 0x1
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_6_CDC_CA_DEBUG_MODE_SHFT                                 0x0

#define HWIO_BIMC_S_DDR0_DPE_CONFIG_7_ADDR                                            (BIMC_S_DDR0_DPE_REG_BASE      + 0x0000002c)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_7_RMSK                                                  0xff
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_7_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CONFIG_7_ADDR, HWIO_BIMC_S_DDR0_DPE_CONFIG_7_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_7_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CONFIG_7_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_7_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_CONFIG_7_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_CONFIG_7_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_CONFIG_7_IN)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_7_MREG_DQ3_BYTE_MAP_BMSK                                0xc0
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_7_MREG_DQ3_BYTE_MAP_SHFT                                 0x6
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_7_MREG_DQ2_BYTE_MAP_BMSK                                0x30
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_7_MREG_DQ2_BYTE_MAP_SHFT                                 0x4
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_7_MREG_DQ1_BYTE_MAP_BMSK                                 0xc
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_7_MREG_DQ1_BYTE_MAP_SHFT                                 0x2
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_7_MREG_DQ0_BYTE_MAP_BMSK                                 0x3
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_7_MREG_DQ0_BYTE_MAP_SHFT                                 0x0

#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_ADDR                                       (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000038)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_RMSK                                       0x77777777
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_ADDR, HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_IN)
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_DQ7_BYTE_MAP_BMSK                          0x70000000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_DQ7_BYTE_MAP_SHFT                                0x1c
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_DQ6_BYTE_MAP_BMSK                           0x7000000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_DQ6_BYTE_MAP_SHFT                                0x18
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_DQ5_BYTE_MAP_BMSK                            0x700000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_DQ5_BYTE_MAP_SHFT                                0x14
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_DQ4_BYTE_MAP_BMSK                             0x70000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_DQ4_BYTE_MAP_SHFT                                0x10
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_DQ3_BYTE_MAP_BMSK                              0x7000
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_DQ3_BYTE_MAP_SHFT                                 0xc
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_DQ2_BYTE_MAP_BMSK                               0x700
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_DQ2_BYTE_MAP_SHFT                                 0x8
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_DQ1_BYTE_MAP_BMSK                                0x70
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_DQ1_BYTE_MAP_SHFT                                 0x4
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_DQ0_BYTE_MAP_BMSK                                 0x7
#define HWIO_BIMC_S_DDR0_DPE_CONFIG_DQ_MAP_DQ0_BYTE_MAP_SHFT                                 0x0

#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_ADDR                                        (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000040)
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_RMSK                                             0x377
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_ADDR, HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_IN)
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_RANK1_FORCE_ODT_BMSK                             0x200
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_RANK1_FORCE_ODT_SHFT                               0x9
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_RANK0_FORCE_ODT_BMSK                             0x100
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_RANK0_FORCE_ODT_SHFT                               0x8
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_RANK1_ODT_ON_WR_RANK1_BMSK                        0x40
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_RANK1_ODT_ON_WR_RANK1_SHFT                         0x6
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_RANK1_ODT_ON_WR_RANK0_BMSK                        0x20
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_RANK1_ODT_ON_WR_RANK0_SHFT                         0x5
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_RANK1_ODT_ON_RD_RANK0_BMSK                        0x10
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_RANK1_ODT_ON_RD_RANK0_SHFT                         0x4
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_RANK0_ODT_ON_WR_RANK1_BMSK                         0x4
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_RANK0_ODT_ON_WR_RANK1_SHFT                         0x2
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_RANK0_ODT_ON_WR_RANK0_BMSK                         0x2
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_RANK0_ODT_ON_WR_RANK0_SHFT                         0x1
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_RANK0_ODT_ON_RD_RANK1_BMSK                         0x1
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_0_RANK0_ODT_ON_RD_RANK1_SHFT                         0x0

#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_ADDR                                        (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000044)
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_RMSK                                        0x71770311
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_ADDR, HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_IN)
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_ODTH8_BMSK                                  0x70000000
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_ODTH8_SHFT                                        0x1c
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_SYNC_ODT_DLY_MODE_BMSK                       0x1000000
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_SYNC_ODT_DLY_MODE_SHFT                            0x18
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_SYNC_ODT_ON_DLY_BMSK                          0x700000
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_SYNC_ODT_ON_DLY_SHFT                              0x14
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_SYNC_ODT_OFF_DLY_BMSK                          0x70000
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_SYNC_ODT_OFF_DLY_SHFT                             0x10
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_ODT_OFF_BEFORE_RD_BMSK                           0x300
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_ODT_OFF_BEFORE_RD_SHFT                             0x8
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_ODT_ON_MIN_POS_BMSK                               0x10
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_ODT_ON_MIN_POS_SHFT                                0x4
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_ODT_OFF_MIN_POS_BMSK                               0x1
#define HWIO_BIMC_S_DDR0_DPE_ODT_CONFIG_1_ODT_OFF_MIN_POS_SHFT                               0x0

#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_PRE_CS_ADDR                                     (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000050)
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_PRE_CS_RMSK                                      0x3ff03ff
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_PRE_CS_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_PRE_CS_ADDR, HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_PRE_CS_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_PRE_CS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_PRE_CS_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_PRE_CS_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_PRE_CS_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_PRE_CS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_PRE_CS_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_PRE_CS_IN)
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_PRE_CS_CA_BUS_2_BMSK                             0x3ff0000
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_PRE_CS_CA_BUS_2_SHFT                                  0x10
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_PRE_CS_CA_BUS_1_BMSK                                 0x3ff
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_PRE_CS_CA_BUS_1_SHFT                                   0x0

#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_CS_ADDR                                         (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000054)
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_CS_RMSK                                          0x3ff03ff
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_CS_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_CS_ADDR, HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_CS_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_CS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_CS_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_CS_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_CS_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_CS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_CS_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_CS_IN)
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_CS_CA_BUS_4_BMSK                                 0x3ff0000
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_CS_CA_BUS_4_SHFT                                      0x10
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_CS_CA_BUS_3_BMSK                                     0x3ff
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_CS_CA_BUS_3_SHFT                                       0x0

#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_POST_CS_ADDR                                    (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000058)
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_POST_CS_RMSK                                     0x3ff03ff
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_POST_CS_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_POST_CS_ADDR, HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_POST_CS_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_POST_CS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_POST_CS_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_POST_CS_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_POST_CS_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_POST_CS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_POST_CS_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_POST_CS_IN)
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_POST_CS_CA_BUS_6_BMSK                            0x3ff0000
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_POST_CS_CA_BUS_6_SHFT                                 0x10
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_POST_CS_CA_BUS_5_BMSK                                0x3ff
#define HWIO_BIMC_S_DDR0_DPE_CA_TRAIN_POST_CS_CA_BUS_5_SHFT                                  0x0

#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_ADDR                                            (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000060)
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RMSK                                            0xff1000ff
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_ADDR, HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_IN)
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE7_BMSK                      0x80000000
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE7_SHFT                            0x1f
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE6_BMSK                      0x40000000
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE6_SHFT                            0x1e
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE5_BMSK                      0x20000000
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE5_SHFT                            0x1d
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE4_BMSK                      0x10000000
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE4_SHFT                            0x1c
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE3_BMSK                       0x8000000
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE3_SHFT                            0x1b
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE2_BMSK                       0x4000000
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE2_SHFT                            0x1a
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE1_BMSK                       0x2000000
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE1_SHFT                            0x19
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE0_BMSK                       0x1000000
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE0_SHFT                            0x18
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RCW_ENABLE_BMSK                                   0x100000
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_RCW_ENABLE_SHFT                                       0x14
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_EXP_PATTERN_BMSK                                      0xff
#define HWIO_BIMC_S_DDR0_DPE_RCW_CTRL_EXP_PATTERN_SHFT                                       0x0

#define HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_ADDR                                          (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000068)
#define HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_RMSK                                          0x1111fff1
#define HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_ADDR, HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_IN)
#define HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_PWR_DN_MODE_BMSK                              0x10000000
#define HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_PWR_DN_MODE_SHFT                                    0x1c
#define HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_PWR_DN_EN_BMSK                                 0x1000000
#define HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_PWR_DN_EN_SHFT                                      0x18
#define HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_CLK_STOP_PWR_DN_EN_BMSK                         0x100000
#define HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_CLK_STOP_PWR_DN_EN_SHFT                             0x14
#define HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_CLK_ON_EN_BMSK                                   0x10000
#define HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_CLK_ON_EN_SHFT                                      0x10
#define HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_CLK_ON_IDLE_CNT_BMSK                              0xfff0
#define HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_CLK_ON_IDLE_CNT_SHFT                                 0x4
#define HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_PCHG_PD_DLL_BMSK                                     0x1
#define HWIO_BIMC_S_DDR0_DPE_PWR_CTRL_0_PCHG_PD_DLL_SHFT                                     0x0

#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_ADDR                                          (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000078)
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_RMSK                                            0x111111
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_ADDR, HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_IN)
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_BC_OTF_EN_BMSK                                  0x100000
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_BC_OTF_EN_SHFT                                      0x14
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_RD_INT_BST_BMSK                                  0x10000
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_RD_INT_BST_SHFT                                     0x10
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_WR_INT_BST_BMSK                                   0x1000
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_WR_INT_BST_SHFT                                      0xc
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_RD_INT_RD_BMSK                                     0x100
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_RD_INT_RD_SHFT                                       0x8
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_WR_INT_WR_BMSK                                      0x10
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_WR_INT_WR_SHFT                                       0x4
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_SPLIT_LONG_WR_BMSK                                   0x1
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_0_SPLIT_LONG_WR_SHFT                                   0x0

#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_1_ADDR                                          (BIMC_S_DDR0_DPE_REG_BASE      + 0x0000007c)
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_1_RMSK                                           0x3ff1fff
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_1_ADDR, HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_1_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_1_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_1_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_1_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_1_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_1_IN)
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_1_DDR_EFF_OPT_TIMER_BMSK                         0x3ff0000
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_1_DDR_EFF_OPT_TIMER_SHFT                              0x10
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_1_PG_MGMT_BMSK                                      0x1000
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_1_PG_MGMT_SHFT                                         0xc
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_1_PG_OPEN_TIMER_BMSK                                 0xfff
#define HWIO_BIMC_S_DDR0_DPE_OPT_CTRL_1_PG_OPEN_TIMER_SHFT                                   0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_ADDR                                       (BIMC_S_DDR0_DPE_REG_BASE      + 0x0000008c)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RMSK                                        0x3333333
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_1_SR_BMSK                              0x2000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_1_SR_SHFT                                   0x19
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_0_SR_BMSK                              0x1000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_0_SR_SHFT                                   0x18
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_1_DPD_BMSK                              0x200000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_1_DPD_SHFT                                  0x15
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_0_DPD_BMSK                              0x100000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_0_DPD_SHFT                                  0x14
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_1_DERATE_BMSK                            0x20000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_1_DERATE_SHFT                               0x11
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_0_DERATE_BMSK                            0x10000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_0_DERATE_SHFT                               0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_1_TEMP_OOR_BMSK                           0x2000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_1_TEMP_OOR_SHFT                              0xd
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_0_TEMP_OOR_BMSK                           0x1000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_0_TEMP_OOR_SHFT                              0xc
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_1_CSPD_BMSK                                0x200
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_1_CSPD_SHFT                                  0x9
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_0_CSPD_BMSK                                0x100
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_0_CSPD_SHFT                                  0x8
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_1_PWR_DN_BMSK                               0x20
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_1_PWR_DN_SHFT                                0x5
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_0_PWR_DN_BMSK                               0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_0_PWR_DN_SHFT                                0x4
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_1_BUSY_BMSK                                  0x2
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_1_BUSY_SHFT                                  0x1
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_0_BUSY_BMSK                                  0x1
#define HWIO_BIMC_S_DDR0_DPE_DRAM_STATUS_0_RANK_0_BUSY_SHFT                                  0x0

#define HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_0_ADDR                                       (BIMC_S_DDR0_DPE_REG_BASE      + 0x0000009c)
#define HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_0_RMSK                                           0xffff
#define HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_0_ADDR, HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_0_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_0_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_0_RD_FIFO_BYTE_EMPTY_BMSK                        0xffff
#define HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_0_RD_FIFO_BYTE_EMPTY_SHFT                           0x0

#define HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_1_ADDR                                       (BIMC_S_DDR0_DPE_REG_BASE      + 0x000000a0)
#define HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_1_RMSK                                              0x1
#define HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_1_ADDR, HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_1_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_1_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_1_CYC_CALC_VALID_BMSK                               0x1
#define HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_1_CYC_CALC_VALID_SHFT                               0x0

#define HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_2_ADDR                                       (BIMC_S_DDR0_DPE_REG_BASE      + 0x000000a4)
#define HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_2_RMSK                                              0x1
#define HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_2_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_2_ADDR, HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_2_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_2_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_2_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_2_SHADOW_REGS_STALE_BMSK                            0x1
#define HWIO_BIMC_S_DDR0_DPE_MEMC_STATUS_2_SHADOW_REGS_STALE_SHFT                            0x0

#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_ENABLE_ADDR                                    (BIMC_S_DDR0_DPE_REG_BASE      + 0x000000b4)
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_ENABLE_RMSK                                          0x77
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_ENABLE_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_INTERRUPT_ENABLE_ADDR, HWIO_BIMC_S_DDR0_DPE_INTERRUPT_ENABLE_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_ENABLE_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_INTERRUPT_ENABLE_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_ENABLE_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_INTERRUPT_ENABLE_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_INTERRUPT_ENABLE_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_INTERRUPT_ENABLE_IN)
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_ENABLE_LOW_TEMP_THRESHOLD_BMSK                       0x70
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_ENABLE_LOW_TEMP_THRESHOLD_SHFT                        0x4
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_ENABLE_HIGH_TEMP_THRESHOLD_BMSK                       0x7
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_ENABLE_HIGH_TEMP_THRESHOLD_SHFT                       0x0

#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_STATUS_ADDR                                    (BIMC_S_DDR0_DPE_REG_BASE      + 0x000000b8)
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_STATUS_RMSK                                      0x77000f
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_INTERRUPT_STATUS_ADDR, HWIO_BIMC_S_DDR0_DPE_INTERRUPT_STATUS_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_INTERRUPT_STATUS_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_STATUS_RANK1_TEMP_INFO_BMSK                      0x700000
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_STATUS_RANK1_TEMP_INFO_SHFT                          0x14
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_STATUS_RANK0_TEMP_INFO_BMSK                       0x70000
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_STATUS_RANK0_TEMP_INFO_SHFT                          0x10
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_STATUS_RANK1_LOW_TEMP_BMSK                            0x8
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_STATUS_RANK1_LOW_TEMP_SHFT                            0x3
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_STATUS_RANK1_HIGH_TEMP_BMSK                           0x4
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_STATUS_RANK1_HIGH_TEMP_SHFT                           0x2
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_STATUS_RANK0_LOW_TEMP_BMSK                            0x2
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_STATUS_RANK0_LOW_TEMP_SHFT                            0x1
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_STATUS_RANK0_HIGH_TEMP_BMSK                           0x1
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_STATUS_RANK0_HIGH_TEMP_SHFT                           0x0

#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_CLR_ADDR                                       (BIMC_S_DDR0_DPE_REG_BASE      + 0x000000bc)
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_CLR_RMSK                                              0xf
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_CLR_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_INTERRUPT_CLR_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_CLR_RANK1_LOW_TEMP_BMSK                               0x8
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_CLR_RANK1_LOW_TEMP_SHFT                               0x3
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_CLR_RANK1_HIGH_TEMP_BMSK                              0x4
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_CLR_RANK1_HIGH_TEMP_SHFT                              0x2
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_CLR_RANK0_LOW_TEMP_BMSK                               0x2
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_CLR_RANK0_LOW_TEMP_SHFT                               0x1
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_CLR_RANK0_HIGH_TEMP_BMSK                              0x1
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_CLR_RANK0_HIGH_TEMP_SHFT                              0x0

#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_SET_ADDR                                       (BIMC_S_DDR0_DPE_REG_BASE      + 0x000000c0)
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_SET_RMSK                                              0xf
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_SET_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_INTERRUPT_SET_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_SET_RANK1_LOW_TEMP_BMSK                               0x8
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_SET_RANK1_LOW_TEMP_SHFT                               0x3
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_SET_RANK1_HIGH_TEMP_BMSK                              0x4
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_SET_RANK1_HIGH_TEMP_SHFT                              0x2
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_SET_RANK0_LOW_TEMP_BMSK                               0x2
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_SET_RANK0_LOW_TEMP_SHFT                               0x1
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_SET_RANK0_HIGH_TEMP_BMSK                              0x1
#define HWIO_BIMC_S_DDR0_DPE_INTERRUPT_SET_RANK0_HIGH_TEMP_SHFT                              0x0

#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_ADDR                                      (BIMC_S_DDR0_DPE_REG_BASE      + 0x000000d0)
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_RMSK                                      0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_ADDR, HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_IN)
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_CYCLES_PRI0_BMSK                          0xff000000
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_CYCLES_PRI0_SHFT                                0x18
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_CYCLES_PRI1_BMSK                            0xff0000
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_CYCLES_PRI1_SHFT                                0x10
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_CYCLES_PRI2_BMSK                              0xff00
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_CYCLES_PRI2_SHFT                                 0x8
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_CYCLES_PRI3_BMSK                                0xff
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_RD_CYCLES_PRI3_SHFT                                 0x0

#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_ADDR                                      (BIMC_S_DDR0_DPE_REG_BASE      + 0x000000d4)
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_RMSK                                      0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_ADDR, HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_IN)
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_CYCLES_PRI0_BMSK                          0xff000000
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_CYCLES_PRI0_SHFT                                0x18
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_CYCLES_PRI1_BMSK                            0xff0000
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_CYCLES_PRI1_SHFT                                0x10
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_CYCLES_PRI2_BMSK                              0xff00
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_CYCLES_PRI2_SHFT                                 0x8
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_CYCLES_PRI3_BMSK                                0xff
#define HWIO_BIMC_S_DDR0_DPE_ELEVATE_PRI_WR_CYCLES_PRI3_SHFT                                 0x0

#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_ADDR                                             (BIMC_S_DDR0_DPE_REG_BASE      + 0x000000e4)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_RMSK                                             0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_TIMER_0_ADDR, HWIO_BIMC_S_DDR0_DPE_TIMER_0_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_TIMER_0_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_TIMER_0_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_TIMER_0_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_TIMER_0_IN)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_WTR_DIFF_RANK_BMSK                               0xf0000000
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_WTR_DIFF_RANK_SHFT                                     0x1c
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_RTW_DIFF_RANK_BMSK                                0xf000000
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_RTW_DIFF_RANK_SHFT                                     0x18
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_RTR_DIFF_RANK_BMSK                                 0xf00000
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_RTR_DIFF_RANK_SHFT                                     0x14
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_WTW_DIFF_RANK_BMSK                                  0xf0000
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_WTW_DIFF_RANK_SHFT                                     0x10
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_RTW_SAME_RANK_ADD_DLY_BMSK                           0xf000
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_RTW_SAME_RANK_ADD_DLY_SHFT                              0xc
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_WTR_SAME_RANK_ADD_DLY_BMSK                            0xf00
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_WTR_SAME_RANK_ADD_DLY_SHFT                              0x8
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_RTR_SAME_RANK_ADD_DLY_BMSK                             0xf0
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_RTR_SAME_RANK_ADD_DLY_SHFT                              0x4
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_WTW_SAME_RANK_ADD_DLY_BMSK                              0xf
#define HWIO_BIMC_S_DDR0_DPE_TIMER_0_WTW_SAME_RANK_ADD_DLY_SHFT                              0x0

#define HWIO_BIMC_S_DDR0_DPE_TIMER_1_ADDR                                             (BIMC_S_DDR0_DPE_REG_BASE      + 0x000000e8)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_1_RMSK                                                 0x31ff
#define HWIO_BIMC_S_DDR0_DPE_TIMER_1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_TIMER_1_ADDR, HWIO_BIMC_S_DDR0_DPE_TIMER_1_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_TIMER_1_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_1_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_TIMER_1_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_TIMER_1_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_TIMER_1_IN)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_1_DERATE_OFFSET_BMSK                                   0x3000
#define HWIO_BIMC_S_DDR0_DPE_TIMER_1_DERATE_OFFSET_SHFT                                      0xc
#define HWIO_BIMC_S_DDR0_DPE_TIMER_1_TDQSS_BMSK                                            0x100
#define HWIO_BIMC_S_DDR0_DPE_TIMER_1_TDQSS_SHFT                                              0x8
#define HWIO_BIMC_S_DDR0_DPE_TIMER_1_IDLE_PWR_DN_CNT_BMSK                                   0xff
#define HWIO_BIMC_S_DDR0_DPE_TIMER_1_IDLE_PWR_DN_CNT_SHFT                                    0x0

#define HWIO_BIMC_S_DDR0_DPE_TIMER_2_ADDR                                             (BIMC_S_DDR0_DPE_REG_BASE      + 0x000000ec)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_2_RMSK                                                0x1fff7
#define HWIO_BIMC_S_DDR0_DPE_TIMER_2_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_TIMER_2_ADDR, HWIO_BIMC_S_DDR0_DPE_TIMER_2_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_2_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_TIMER_2_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_2_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_TIMER_2_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_TIMER_2_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_TIMER_2_IN)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_2_TRASMAX_XO_DIV_6_CYCLES_BMSK                        0x1ff00
#define HWIO_BIMC_S_DDR0_DPE_TIMER_2_TRASMAX_XO_DIV_6_CYCLES_SHFT                            0x8
#define HWIO_BIMC_S_DDR0_DPE_TIMER_2_TMRW_BMSK                                              0xf0
#define HWIO_BIMC_S_DDR0_DPE_TIMER_2_TMRW_SHFT                                               0x4
#define HWIO_BIMC_S_DDR0_DPE_TIMER_2_TMRR_BMSK                                               0x7
#define HWIO_BIMC_S_DDR0_DPE_TIMER_2_TMRR_SHFT                                               0x0

#define HWIO_BIMC_S_DDR0_DPE_TIMER_3_ADDR                                             (BIMC_S_DDR0_DPE_REG_BASE      + 0x000000f0)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_3_RMSK                                                  0xf37
#define HWIO_BIMC_S_DDR0_DPE_TIMER_3_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_TIMER_3_ADDR, HWIO_BIMC_S_DDR0_DPE_TIMER_3_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_3_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_TIMER_3_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_3_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_TIMER_3_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_TIMER_3_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_TIMER_3_IN)
#define HWIO_BIMC_S_DDR0_DPE_TIMER_3_BANK_AVAIL_OFFSET_BMSK                                0xf00
#define HWIO_BIMC_S_DDR0_DPE_TIMER_3_BANK_AVAIL_OFFSET_SHFT                                  0x8
#define HWIO_BIMC_S_DDR0_DPE_TIMER_3_CLK_CYC_BEFORE_PD_EXIT_BMSK                            0x30
#define HWIO_BIMC_S_DDR0_DPE_TIMER_3_CLK_CYC_BEFORE_PD_EXIT_SHFT                             0x4
#define HWIO_BIMC_S_DDR0_DPE_TIMER_3_CLK_CYC_AFTER_PD_ENTRY_BMSK                             0x7
#define HWIO_BIMC_S_DDR0_DPE_TIMER_3_CLK_CYC_AFTER_PD_ENTRY_SHFT                             0x0

#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_0_ADDR                            (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000114)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_0_RMSK                             0xff0ffff
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_0_ADDR, HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_0_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_0_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_0_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_0_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_0_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_0_IN)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_0_W_ADDR_BMSK                      0xff00000
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_0_W_ADDR_SHFT                           0x14
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_0_W_DATA_BMSK                         0xffff
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_0_W_DATA_SHFT                            0x0

#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_1_ADDR                            (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000118)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_1_RMSK                             0xff0ffff
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_1_ADDR, HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_1_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_1_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_1_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_1_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_1_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_1_IN)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_1_W_ADDR_BMSK                      0xff00000
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_1_W_ADDR_SHFT                           0x14
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_1_W_DATA_BMSK                         0xffff
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_1_W_DATA_SHFT                            0x0

#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_2_ADDR                            (BIMC_S_DDR0_DPE_REG_BASE      + 0x0000011c)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_2_RMSK                             0xff0ffff
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_2_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_2_ADDR, HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_2_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_2_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_2_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_2_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_2_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_2_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_2_IN)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_2_W_ADDR_BMSK                      0xff00000
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_2_W_ADDR_SHFT                           0x14
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_2_W_DATA_BMSK                         0xffff
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_2_W_DATA_SHFT                            0x0

#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_3_ADDR                            (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000120)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_3_RMSK                             0xff0ffff
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_3_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_3_ADDR, HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_3_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_3_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_3_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_3_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_3_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_3_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_3_IN)
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_3_W_ADDR_BMSK                      0xff00000
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_3_W_ADDR_SHFT                           0x14
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_3_W_DATA_BMSK                         0xffff
#define HWIO_BIMC_S_DDR0_DPE_MRW_BEFORE_FREQ_SWITCH_3_W_DATA_SHFT                            0x0

#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_0_ADDR                             (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000124)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_0_RMSK                              0xff0ffff
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_0_ADDR, HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_0_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_0_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_0_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_0_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_0_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_0_IN)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_0_W_ADDR_BMSK                       0xff00000
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_0_W_ADDR_SHFT                            0x14
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_0_W_DATA_BMSK                          0xffff
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_0_W_DATA_SHFT                             0x0

#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_1_ADDR                             (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000128)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_1_RMSK                              0xff0ffff
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_1_ADDR, HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_1_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_1_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_1_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_1_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_1_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_1_IN)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_1_W_ADDR_BMSK                       0xff00000
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_1_W_ADDR_SHFT                            0x14
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_1_W_DATA_BMSK                          0xffff
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_1_W_DATA_SHFT                             0x0

#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_2_ADDR                             (BIMC_S_DDR0_DPE_REG_BASE      + 0x0000012c)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_2_RMSK                              0xff0ffff
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_2_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_2_ADDR, HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_2_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_2_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_2_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_2_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_2_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_2_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_2_IN)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_2_W_ADDR_BMSK                       0xff00000
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_2_W_ADDR_SHFT                            0x14
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_2_W_DATA_BMSK                          0xffff
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_2_W_DATA_SHFT                             0x0

#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_3_ADDR                             (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000130)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_3_RMSK                              0xff0ffff
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_3_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_3_ADDR, HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_3_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_3_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_3_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_3_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_3_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_3_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_3_IN)
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_3_W_ADDR_BMSK                       0xff00000
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_3_W_ADDR_SHFT                            0x14
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_3_W_DATA_BMSK                          0xffff
#define HWIO_BIMC_S_DDR0_DPE_MRW_AFTER_FREQ_SWITCH_3_W_DATA_SHFT                             0x0

#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_ADDR                                     (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000134)
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_RMSK                                        0xf000f
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_ADDR, HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_IN)
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_BEFORE_3_BMSK                               0x80000
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_BEFORE_3_SHFT                                  0x13
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_BEFORE_2_BMSK                               0x40000
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_BEFORE_2_SHFT                                  0x12
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_BEFORE_1_BMSK                               0x20000
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_BEFORE_1_SHFT                                  0x11
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_BEFORE_0_BMSK                               0x10000
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_BEFORE_0_SHFT                                  0x10
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_AFTER_3_BMSK                                    0x8
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_AFTER_3_SHFT                                    0x3
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_AFTER_2_BMSK                                    0x4
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_AFTER_2_SHFT                                    0x2
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_AFTER_1_BMSK                                    0x2
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_AFTER_1_SHFT                                    0x1
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_AFTER_0_BMSK                                    0x1
#define HWIO_BIMC_S_DDR0_DPE_MRW_FREQ_SWITCH_AFTER_0_SHFT                                    0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_ADDR                                       (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000144)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_RMSK                                           0xf3ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_TRASMIN_MIN_CYC_BMSK                           0xf000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_TRASMIN_MIN_CYC_SHFT                              0xc
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_TRASMIN_BMSK                                    0x3ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_TRASMIN_SHFT                                      0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_ADDR                                       (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000148)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_RMSK                                       0xf0fff1ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_TWR_MIN_CYC_BMSK                           0xf0000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_TWR_MIN_CYC_SHFT                                 0x1c
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_TWR_BMSK                                     0xff0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_TWR_SHFT                                         0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_TRCD_MIN_CYC_BMSK                              0xf000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_TRCD_MIN_CYC_SHFT                                 0xc
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_TRCD_BMSK                                       0x1ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_TRCD_SHFT                                         0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_ADDR                                       (BIMC_S_DDR0_DPE_REG_BASE      + 0x0000014c)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_RMSK                                       0xf07ff0ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_TWTR_MIN_CYC_BMSK                          0xf0000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_TWTR_MIN_CYC_SHFT                                0x1c
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_TWTR_BMSK                                    0x7f0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_TWTR_SHFT                                        0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_TRRD_MIN_CYC_BMSK                              0xf000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_TRRD_MIN_CYC_SHFT                                 0xc
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_TRRD_BMSK                                        0xff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_TRRD_SHFT                                         0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_ADDR                                       (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000150)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_RMSK                                       0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_TRFCAB_MIN_CYC_BMSK                        0xf0000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_TRFCAB_MIN_CYC_SHFT                              0x1c
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_TRFCAB_BMSK                                 0xfff0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_TRFCAB_SHFT                                      0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_TRFCPB_MIN_CYC_BMSK                            0xf000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_TRFCPB_MIN_CYC_SHFT                               0xc
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_TRFCPB_BMSK                                     0xfff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_TRFCPB_SHFT                                       0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_ADDR                                       (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000154)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_RMSK                                           0xf07f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_TRTP_MIN_CYC_BMSK                              0xf000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_TRTP_MIN_CYC_SHFT                                 0xc
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_TRTP_BMSK                                        0x7f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_TRTP_SHFT                                         0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_ADDR                                       (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000158)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_RMSK                                       0xf1fff1ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_TRPAB_MIN_CYC_BMSK                         0xf0000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_TRPAB_MIN_CYC_SHFT                               0x1c
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_TRPAB_BMSK                                  0x1ff0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_TRPAB_SHFT                                       0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_TRPPB_MIN_CYC_BMSK                             0xf000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_TRPPB_MIN_CYC_SHFT                                0xc
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_TRPPB_BMSK                                      0x1ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_TRPPB_SHFT                                        0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_ADDR                                       (BIMC_S_DDR0_DPE_REG_BASE      + 0x0000015c)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_RMSK                                       0xf3fff0ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_TFAW_MIN_CYC_BMSK                          0xf0000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_TFAW_MIN_CYC_SHFT                                0x1c
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_TFAW_BMSK                                   0x3ff0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_TFAW_SHFT                                        0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_TCKE_MIN_CYC_BMSK                              0xf000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_TCKE_MIN_CYC_SHFT                                 0xc
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_TCKE_BMSK                                        0xff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_TCKE_SHFT                                         0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_ADDR                                       (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000160)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_RMSK                                         0x7f07ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_TZQCS_MIN_CYC_BMSK                           0x7f0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_TZQCS_MIN_CYC_SHFT                               0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_TZQCS_BMSK                                      0x7ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_TZQCS_SHFT                                        0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_ADDR                                       (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000164)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_RMSK                                           0xf7ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_TMOD_MIN_CYC_BMSK                              0xf000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_TMOD_MIN_CYC_SHFT                                 0xc
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_TMOD_BMSK                                       0x7ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_TMOD_SHFT                                         0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_ADDR                                       (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000168)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_RMSK                                           0xf0ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_MIN_SR_DURATION_MIN_CYC_BMSK                   0xf000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_MIN_SR_DURATION_MIN_CYC_SHFT                      0xc
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_MIN_SR_DURATION_BMSK                             0xff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_MIN_SR_DURATION_SHFT                              0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_ADDR                                      (BIMC_S_DDR0_DPE_REG_BASE      + 0x0000016c)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_RMSK                                      0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_TXSRD_MIN_CYC_BMSK                        0xf0000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_TXSRD_MIN_CYC_SHFT                              0x1c
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_TXSRD_BMSK                                 0xfff0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_TXSRD_SHFT                                      0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_TXSNR_MIN_CYC_BMSK                            0xf000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_TXSNR_MIN_CYC_SHFT                               0xc
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_TXSNR_BMSK                                     0xfff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_TXSNR_SHFT                                       0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_ADDR                                      (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000170)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_RMSK                                      0xf0fff1ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_TXPNR_ACT_PWR_DN_MIN_CYC_BMSK             0xf0000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_TXPNR_ACT_PWR_DN_MIN_CYC_SHFT                   0x1c
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_TXPNR_ACT_PWR_DN_BMSK                       0xff0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_TXPNR_ACT_PWR_DN_SHFT                           0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_TXPR_ACT_PWR_DN_MIN_CYC_BMSK                  0xf000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_TXPR_ACT_PWR_DN_MIN_CYC_SHFT                     0xc
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_TXPR_ACT_PWR_DN_BMSK                           0x1ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_TXPR_ACT_PWR_DN_SHFT                             0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_ADDR                                      (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000174)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_RMSK                                      0xf0fff1ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_TXPNR_PCHG_PWR_DN_MIN_CYC_BMSK            0xf0000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_TXPNR_PCHG_PWR_DN_MIN_CYC_SHFT                  0x1c
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_TXPNR_PCHG_PWR_DN_BMSK                      0xff0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_TXPNR_PCHG_PWR_DN_SHFT                          0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_TXPR_PCHG_PWR_DN_MIN_CYC_BMSK                 0xf000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_TXPR_PCHG_PWR_DN_MIN_CYC_SHFT                    0xc
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_TXPR_PCHG_PWR_DN_BMSK                          0x1ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_TXPR_PCHG_PWR_DN_SHFT                            0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_ADDR                                      (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000178)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_RMSK                                      0xf07ff7ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_CLK_AFTER_SR_ENTRY_MIN_CYC_BMSK           0xf0000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_CLK_AFTER_SR_ENTRY_MIN_CYC_SHFT                 0x1c
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_CLK_AFTER_SR_ENTRY_BMSK                     0x7f0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_CLK_AFTER_SR_ENTRY_SHFT                         0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_CLK_BEFORE_SR_EXIT_MIN_CYC_BMSK               0xf000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_CLK_BEFORE_SR_EXIT_MIN_CYC_SHFT                  0xc
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_CLK_BEFORE_SR_EXIT_BMSK                        0x7ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_CLK_BEFORE_SR_EXIT_SHFT                          0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_ADDR                                      (BIMC_S_DDR0_DPE_REG_BASE      + 0x0000017c)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_RMSK                                      0xf3fff0ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_IE_WINDOW_START_ADD_CYCLES_BMSK           0xf0000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_IE_WINDOW_START_ADD_CYCLES_SHFT                 0x1c
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_IE_WINDOW_START_SUB_CYCLES_BMSK            0x3000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_IE_WINDOW_START_SUB_CYCLES_SHFT                 0x18
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_IE_WINDOW_START_BMSK                        0xff0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_IE_WINDOW_START_SHFT                            0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_IE_WINDOW_END_ADD_CYCLES_BMSK                 0xf000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_IE_WINDOW_END_ADD_CYCLES_SHFT                    0xc
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_IE_WINDOW_END_BMSK                              0xff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_IE_WINDOW_END_SHFT                               0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_ADDR                                      (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000180)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_RMSK                                      0xf0fff0ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_RD_CAPTURE_START_ADD_CYCLES_BMSK          0xf0000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_RD_CAPTURE_START_ADD_CYCLES_SHFT                0x1c
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_RD_CAPTURE_START_BMSK                       0xff0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_RD_CAPTURE_START_SHFT                           0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_RD_CAPTURE_END_ADD_CYCLES_BMSK                0xf000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_RD_CAPTURE_END_ADD_CYCLES_SHFT                   0xc
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_RD_CAPTURE_END_BMSK                             0xff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_RD_CAPTURE_END_SHFT                              0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ADDR                                      (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000184)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_RMSK                                      0x331f3f3f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ODTON_MAX_ADD_CYCLES_BMSK                 0x30000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ODTON_MAX_ADD_CYCLES_SHFT                       0x1c
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ODTOFF_MIN_SUB_CYCLES_BMSK                 0x3000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ODTOFF_MIN_SUB_CYCLES_SHFT                      0x18
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_PAD_MODE_BMSK                               0x100000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_PAD_MODE_SHFT                                   0x14
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_TCCD_BMSK                                    0xf0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_TCCD_SHFT                                       0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_RD_LATENCY_BMSK                               0x3f00
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_RD_LATENCY_SHFT                                  0x8
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_WR_LATENCY_BMSK                                 0x3f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_WR_LATENCY_SHFT                                  0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_ADDR                                      (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000188)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_RMSK                                       0x3ff0fff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_TZQCL_MIN_CYC_BMSK                         0x3ff0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_TZQCL_MIN_CYC_SHFT                              0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_TZQCL_BMSK                                     0xfff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_TZQCL_SHFT                                       0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ADDR                                      (BIMC_S_DDR0_DPE_REG_BASE      + 0x0000018c)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_RMSK                                      0x1f1f1f1f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_RCW_START_DLY_BYTE7_BMSK                  0x1f000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_RCW_START_DLY_BYTE7_SHFT                        0x18
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_RCW_START_DLY_BYTE6_BMSK                    0x1f0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_RCW_START_DLY_BYTE6_SHFT                        0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_RCW_START_DLY_BYTE5_BMSK                      0x1f00
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_RCW_START_DLY_BYTE5_SHFT                         0x8
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_RCW_START_DLY_BYTE4_BMSK                        0x1f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_RCW_START_DLY_BYTE4_SHFT                         0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ADDR                                      (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000190)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_RMSK                                      0x1f1f1f1f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_RCW_START_DLY_BYTE3_BMSK                  0x1f000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_RCW_START_DLY_BYTE3_SHFT                        0x18
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_RCW_START_DLY_BYTE2_BMSK                    0x1f0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_RCW_START_DLY_BYTE2_SHFT                        0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_RCW_START_DLY_BYTE1_BMSK                      0x1f00
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_RCW_START_DLY_BYTE1_SHFT                         0x8
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_RCW_START_DLY_BYTE0_BMSK                        0x1f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_RCW_START_DLY_BYTE0_SHFT                         0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ADDR                                      (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000194)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_RMSK                                      0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_RCW_FINE_DLY_BYTE7_BMSK                   0xff000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_RCW_FINE_DLY_BYTE7_SHFT                         0x18
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_RCW_FINE_DLY_BYTE6_BMSK                     0xff0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_RCW_FINE_DLY_BYTE6_SHFT                         0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_RCW_FINE_DLY_BYTE5_BMSK                       0xff00
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_RCW_FINE_DLY_BYTE5_SHFT                          0x8
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_RCW_FINE_DLY_BYTE4_BMSK                         0xff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_RCW_FINE_DLY_BYTE4_SHFT                          0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ADDR                                      (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000198)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_RMSK                                      0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_RCW_FINE_DLY_BYTE3_BMSK                   0xff000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_RCW_FINE_DLY_BYTE3_SHFT                         0x18
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_RCW_FINE_DLY_BYTE2_BMSK                     0xff0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_RCW_FINE_DLY_BYTE2_SHFT                         0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_RCW_FINE_DLY_BYTE1_BMSK                       0xff00
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_RCW_FINE_DLY_BYTE1_SHFT                          0x8
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_RCW_FINE_DLY_BYTE0_BMSK                         0xff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_RCW_FINE_DLY_BYTE0_SHFT                          0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ADDR                                      (BIMC_S_DDR0_DPE_REG_BASE      + 0x0000019c)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_RMSK                                      0x33333333
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_RCW_COARSE_DLY_BYTE7_BMSK                 0x30000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_RCW_COARSE_DLY_BYTE7_SHFT                       0x1c
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_RCW_COARSE_DLY_BYTE6_BMSK                  0x3000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_RCW_COARSE_DLY_BYTE6_SHFT                       0x18
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_RCW_COARSE_DLY_BYTE5_BMSK                   0x300000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_RCW_COARSE_DLY_BYTE5_SHFT                       0x14
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_RCW_COARSE_DLY_BYTE4_BMSK                    0x30000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_RCW_COARSE_DLY_BYTE4_SHFT                       0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_RCW_COARSE_DLY_BYTE3_BMSK                     0x3000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_RCW_COARSE_DLY_BYTE3_SHFT                        0xc
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_RCW_COARSE_DLY_BYTE2_BMSK                      0x300
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_RCW_COARSE_DLY_BYTE2_SHFT                        0x8
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_RCW_COARSE_DLY_BYTE1_BMSK                       0x30
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_RCW_COARSE_DLY_BYTE1_SHFT                        0x4
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_RCW_COARSE_DLY_BYTE0_BMSK                        0x3
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_RCW_COARSE_DLY_BYTE0_SHFT                        0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ADDR                                      (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001a0)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_RMSK                                      0x3f1f3f1f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_IN)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ODTON_MAX_BMSK                            0x3f000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ODTON_MAX_SHFT                                  0x18
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ODTON_MIN_BMSK                              0x1f0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ODTON_MIN_SHFT                                  0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ODTOFF_MAX_BMSK                               0x3f00
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ODTOFF_MAX_SHFT                                  0x8
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ODTOFF_MIN_BMSK                                 0x1f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ODTOFF_MIN_SHFT                                  0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_ACT_ADDR                                   (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001a4)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_ACT_RMSK                                         0x7f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_ACT_TRASMIN_BMSK                                 0x7f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_0_ACT_TRASMIN_SHFT                                  0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_ACT_ADDR                                   (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001a8)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_ACT_RMSK                                     0x1f003f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_ACT_TWR_BMSK                                 0x1f0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_ACT_TWR_SHFT                                     0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_ACT_TRCD_BMSK                                    0x3f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_1_ACT_TRCD_SHFT                                     0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_ACT_ADDR                                   (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001ac)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_ACT_RMSK                                      0xf001f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_ACT_TWTR_BMSK                                 0xf0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_ACT_TWTR_SHFT                                    0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_ACT_TRRD_BMSK                                    0x1f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_2_ACT_TRRD_SHFT                                     0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_ACT_ADDR                                   (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001b0)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_ACT_RMSK                                    0x1ff01ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_ACT_TRFCAB_BMSK                             0x1ff0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_ACT_TRFCAB_SHFT                                  0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_ACT_TRFCPB_BMSK                                 0x1ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_3_ACT_TRFCPB_SHFT                                   0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_ACT_ADDR                                   (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001b4)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_ACT_RMSK                                          0xf
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_ACT_TRTP_BMSK                                     0xf
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_4_ACT_TRTP_SHFT                                     0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_ACT_ADDR                                   (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001b8)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_ACT_RMSK                                     0x3f003f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_ACT_TRPAB_BMSK                               0x3f0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_ACT_TRPAB_SHFT                                   0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_ACT_TRPPB_BMSK                                   0x3f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_5_ACT_TRPPB_SHFT                                    0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_ACT_ADDR                                   (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001bc)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_ACT_RMSK                                     0x7f001f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_ACT_TFAW_BMSK                                0x7f0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_ACT_TFAW_SHFT                                    0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_ACT_TCKE_BMSK                                    0x1f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_6_ACT_TCKE_SHFT                                     0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_ACT_ADDR                                   (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001c0)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_ACT_RMSK                                         0xff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_ACT_TZQCS_BMSK                                   0xff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_7_ACT_TZQCS_SHFT                                    0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_ACT_ADDR                                   (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001c4)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_ACT_RMSK                                         0xff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_ACT_TMOD_BMSK                                    0xff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_8_ACT_TMOD_SHFT                                     0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_ACT_ADDR                                   (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001c8)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_ACT_RMSK                                         0x1f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_ACT_MIN_SR_DURATION_BMSK                         0x1f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_9_ACT_MIN_SR_DURATION_SHFT                          0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_ACT_ADDR                                  (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001cc)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_ACT_RMSK                                   0x3ff01ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_ACT_TXSRD_BMSK                             0x3ff0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_ACT_TXSRD_SHFT                                  0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_ACT_TXSNR_BMSK                                 0x1ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_10_ACT_TXSNR_SHFT                                   0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_ACT_ADDR                                  (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001d0)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_ACT_RMSK                                    0x1f003f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_ACT_TXPNR_ACT_PWR_DN_BMSK                   0x1f0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_ACT_TXPNR_ACT_PWR_DN_SHFT                       0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_ACT_TXPR_ACT_PWR_DN_BMSK                        0x3f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_11_ACT_TXPR_ACT_PWR_DN_SHFT                         0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_ACT_ADDR                                  (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001d4)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_ACT_RMSK                                    0x1f003f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_ACT_TXPNR_PCHG_PWR_DN_BMSK                  0x1f0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_ACT_TXPNR_PCHG_PWR_DN_SHFT                      0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_ACT_TXPR_PCHG_PWR_DN_BMSK                       0x3f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_12_ACT_TXPR_PCHG_PWR_DN_SHFT                        0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_ACT_ADDR                                  (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001d8)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_ACT_RMSK                                    0x1f00ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_ACT_CLK_AFTER_SR_ENTRY_BMSK                 0x1f0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_ACT_CLK_AFTER_SR_ENTRY_SHFT                     0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_ACT_CLK_BEFORE_SR_EXIT_BMSK                     0xff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_13_ACT_CLK_BEFORE_SR_EXIT_SHFT                      0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_ACT_ADDR                                  (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001dc)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_ACT_RMSK                                    0x3f003f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_ACT_IE_WINDOW_START_BMSK                    0x3f0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_ACT_IE_WINDOW_START_SHFT                        0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_ACT_IE_WINDOW_END_BMSK                          0x3f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_14_ACT_IE_WINDOW_END_SHFT                           0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_ACT_ADDR                                  (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001e0)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_ACT_RMSK                                    0x3f003f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_ACT_RD_CAPTURE_START_BMSK                   0x3f0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_ACT_RD_CAPTURE_START_SHFT                       0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_ACT_RD_CAPTURE_END_BMSK                         0x3f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_15_ACT_RD_CAPTURE_END_SHFT                          0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ACT_ADDR                                  (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001e4)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ACT_RMSK                                    0x1f3f3f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ACT_PAD_MODE_BMSK                           0x100000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ACT_PAD_MODE_SHFT                               0x14
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ACT_TCCD_BMSK                                0xf0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ACT_TCCD_SHFT                                   0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ACT_RD_LATENCY_BMSK                           0x3f00
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ACT_RD_LATENCY_SHFT                              0x8
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ACT_WR_LATENCY_BMSK                             0x3f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_16_ACT_WR_LATENCY_SHFT                              0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_ACT_ADDR                                  (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001e8)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_ACT_RMSK                                       0x3ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_ACT_TZQCL_BMSK                                 0x3ff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_17_ACT_TZQCL_SHFT                                   0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ACT_ADDR                                  (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001ec)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ACT_RMSK                                  0x1f1f1f1f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ACT_RCW_START_DLY_BYTE7_BMSK              0x1f000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ACT_RCW_START_DLY_BYTE7_SHFT                    0x18
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ACT_RCW_START_DLY_BYTE6_BMSK                0x1f0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ACT_RCW_START_DLY_BYTE6_SHFT                    0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ACT_RCW_START_DLY_BYTE5_BMSK                  0x1f00
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ACT_RCW_START_DLY_BYTE5_SHFT                     0x8
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ACT_RCW_START_DLY_BYTE4_BMSK                    0x1f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_18_ACT_RCW_START_DLY_BYTE4_SHFT                     0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ACT_ADDR                                  (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001f0)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ACT_RMSK                                  0x1f1f1f1f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ACT_RCW_START_DLY_BYTE3_BMSK              0x1f000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ACT_RCW_START_DLY_BYTE3_SHFT                    0x18
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ACT_RCW_START_DLY_BYTE2_BMSK                0x1f0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ACT_RCW_START_DLY_BYTE2_SHFT                    0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ACT_RCW_START_DLY_BYTE1_BMSK                  0x1f00
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ACT_RCW_START_DLY_BYTE1_SHFT                     0x8
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ACT_RCW_START_DLY_BYTE0_BMSK                    0x1f
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_19_ACT_RCW_START_DLY_BYTE0_SHFT                     0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ACT_ADDR                                  (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001f4)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ACT_RMSK                                  0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ACT_RCW_FINE_DLY_BYTE7_BMSK               0xff000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ACT_RCW_FINE_DLY_BYTE7_SHFT                     0x18
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ACT_RCW_FINE_DLY_BYTE6_BMSK                 0xff0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ACT_RCW_FINE_DLY_BYTE6_SHFT                     0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ACT_RCW_FINE_DLY_BYTE5_BMSK                   0xff00
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ACT_RCW_FINE_DLY_BYTE5_SHFT                      0x8
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ACT_RCW_FINE_DLY_BYTE4_BMSK                     0xff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_20_ACT_RCW_FINE_DLY_BYTE4_SHFT                      0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ACT_ADDR                                  (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001f8)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ACT_RMSK                                  0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ACT_RCW_FINE_DLY_BYTE3_BMSK               0xff000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ACT_RCW_FINE_DLY_BYTE3_SHFT                     0x18
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ACT_RCW_FINE_DLY_BYTE2_BMSK                 0xff0000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ACT_RCW_FINE_DLY_BYTE2_SHFT                     0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ACT_RCW_FINE_DLY_BYTE1_BMSK                   0xff00
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ACT_RCW_FINE_DLY_BYTE1_SHFT                      0x8
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ACT_RCW_FINE_DLY_BYTE0_BMSK                     0xff
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_21_ACT_RCW_FINE_DLY_BYTE0_SHFT                      0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_ADDR                                  (BIMC_S_DDR0_DPE_REG_BASE      + 0x000001fc)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_RMSK                                  0x33333333
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_RCW_COARSE_DLY_BYTE7_BMSK             0x30000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_RCW_COARSE_DLY_BYTE7_SHFT                   0x1c
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_RCW_COARSE_DLY_BYTE6_BMSK              0x3000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_RCW_COARSE_DLY_BYTE6_SHFT                   0x18
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_RCW_COARSE_DLY_BYTE5_BMSK               0x300000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_RCW_COARSE_DLY_BYTE5_SHFT                   0x14
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_RCW_COARSE_DLY_BYTE4_BMSK                0x30000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_RCW_COARSE_DLY_BYTE4_SHFT                   0x10
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_RCW_COARSE_DLY_BYTE3_BMSK                 0x3000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_RCW_COARSE_DLY_BYTE3_SHFT                    0xc
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_RCW_COARSE_DLY_BYTE2_BMSK                  0x300
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_RCW_COARSE_DLY_BYTE2_SHFT                    0x8
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_RCW_COARSE_DLY_BYTE1_BMSK                   0x30
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_RCW_COARSE_DLY_BYTE1_SHFT                    0x4
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_RCW_COARSE_DLY_BYTE0_BMSK                    0x3
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_22_ACT_RCW_COARSE_DLY_BYTE0_SHFT                    0x0

#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ACT_ADDR                                  (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000200)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ACT_RMSK                                  0xf3730000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ACT_ODTON_MAX_BMSK                        0xf0000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ACT_ODTON_MAX_SHFT                              0x1c
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ACT_ODTON_MIN_BMSK                         0x3000000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ACT_ODTON_MIN_SHFT                              0x18
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ACT_ODTOFF_MAX_BMSK                         0x700000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ACT_ODTOFF_MAX_SHFT                             0x14
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ACT_ODTOFF_MIN_BMSK                          0x30000
#define HWIO_BIMC_S_DDR0_DPE_DRAM_TIMING_23_ACT_ODTOFF_MIN_SHFT                             0x10

#define HWIO_BIMC_S_DDR0_DPE_TESTBUS_CTRL_ADDR                                        (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000204)
#define HWIO_BIMC_S_DDR0_DPE_TESTBUS_CTRL_RMSK                                             0x1f1
#define HWIO_BIMC_S_DDR0_DPE_TESTBUS_CTRL_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_TESTBUS_CTRL_ADDR, HWIO_BIMC_S_DDR0_DPE_TESTBUS_CTRL_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_TESTBUS_CTRL_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_TESTBUS_CTRL_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_TESTBUS_CTRL_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_TESTBUS_CTRL_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_TESTBUS_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_TESTBUS_CTRL_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_TESTBUS_CTRL_IN)
#define HWIO_BIMC_S_DDR0_DPE_TESTBUS_CTRL_TESTBUS_SEL_BMSK                                 0x1f0
#define HWIO_BIMC_S_DDR0_DPE_TESTBUS_CTRL_TESTBUS_SEL_SHFT                                   0x4
#define HWIO_BIMC_S_DDR0_DPE_TESTBUS_CTRL_TESTBUS_EN_BMSK                                    0x1
#define HWIO_BIMC_S_DDR0_DPE_TESTBUS_CTRL_TESTBUS_EN_SHFT                                    0x0

#define HWIO_BIMC_S_DDR0_DPE_SPARE_REG_ADDR                                           (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000214)
#define HWIO_BIMC_S_DDR0_DPE_SPARE_REG_RMSK                                           0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_SPARE_REG_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_SPARE_REG_ADDR, HWIO_BIMC_S_DDR0_DPE_SPARE_REG_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_SPARE_REG_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_SPARE_REG_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_SPARE_REG_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_SPARE_REG_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_SPARE_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_SPARE_REG_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_SPARE_REG_IN)
#define HWIO_BIMC_S_DDR0_DPE_SPARE_REG_SPARE_REGS_BMSK                                0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_SPARE_REG_SPARE_REGS_SHFT                                       0x0

#define HWIO_BIMC_S_DDR0_DPE_SPARE_REG_ACT_ADDR                                       (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000224)
#define HWIO_BIMC_S_DDR0_DPE_SPARE_REG_ACT_RMSK                                       0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_SPARE_REG_ACT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_SPARE_REG_ACT_ADDR, HWIO_BIMC_S_DDR0_DPE_SPARE_REG_ACT_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_SPARE_REG_ACT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_SPARE_REG_ACT_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_SPARE_REG_ACT_SPARE_REGS_ACT_BMSK                        0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_SPARE_REG_ACT_SPARE_REGS_ACT_SHFT                               0x0

#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK1_U_ADDR                                 (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000240)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK1_U_RMSK                                 0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK1_U_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK1_U_ADDR, HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK1_U_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK1_U_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK1_U_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK1_U_DQCAL_RDATA_BMSK                     0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK1_U_DQCAL_RDATA_SHFT                            0x0

#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK1_L_ADDR                                 (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000244)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK1_L_RMSK                                 0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK1_L_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK1_L_ADDR, HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK1_L_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK1_L_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK1_L_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK1_L_DQCAL_RDATA_BMSK                     0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK1_L_DQCAL_RDATA_SHFT                            0x0

#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK0_U_ADDR                                 (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000248)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK0_U_RMSK                                 0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK0_U_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK0_U_ADDR, HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK0_U_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK0_U_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK0_U_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK0_U_DQCAL_RDATA_BMSK                     0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK0_U_DQCAL_RDATA_SHFT                            0x0

#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK0_L_ADDR                                 (BIMC_S_DDR0_DPE_REG_BASE      + 0x0000024c)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK0_L_RMSK                                 0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK0_L_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK0_L_ADDR, HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK0_L_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK0_L_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK0_L_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK0_L_DQCAL_RDATA_BMSK                     0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_RDATA_RANK0_L_DQCAL_RDATA_SHFT                            0x0

#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_ADDR                                 (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000260)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_RMSK                                       0xff
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_ADDR, HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_DQCAL_COMPARE_BYTE7_BMSK                   0x80
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_DQCAL_COMPARE_BYTE7_SHFT                    0x7
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_DQCAL_COMPARE_BYTE6_BMSK                   0x40
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_DQCAL_COMPARE_BYTE6_SHFT                    0x6
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_DQCAL_COMPARE_BYTE5_BMSK                   0x20
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_DQCAL_COMPARE_BYTE5_SHFT                    0x5
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_DQCAL_COMPARE_BYTE4_BMSK                   0x10
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_DQCAL_COMPARE_BYTE4_SHFT                    0x4
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_DQCAL_COMPARE_BYTE3_BMSK                    0x8
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_DQCAL_COMPARE_BYTE3_SHFT                    0x3
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_DQCAL_COMPARE_BYTE2_BMSK                    0x4
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_DQCAL_COMPARE_BYTE2_SHFT                    0x2
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_DQCAL_COMPARE_BYTE1_BMSK                    0x2
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_DQCAL_COMPARE_BYTE1_SHFT                    0x1
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_DQCAL_COMPARE_BYTE0_BMSK                    0x1
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK1_DQCAL_COMPARE_BYTE0_SHFT                    0x0

#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_ADDR                                 (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000264)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_RMSK                                 0x77777777
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_ADDR, HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_DQS_PE_BYTE7_BMSK                    0x70000000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_DQS_PE_BYTE7_SHFT                          0x1c
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_DQS_NE_BYTE7_BMSK                     0x7000000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_DQS_NE_BYTE7_SHFT                          0x18
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_DQS_PE_BYTE6_BMSK                      0x700000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_DQS_PE_BYTE6_SHFT                          0x14
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_DQS_NE_BYTE6_BMSK                       0x70000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_DQS_NE_BYTE6_SHFT                          0x10
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_DQS_PE_BYTE5_BMSK                        0x7000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_DQS_PE_BYTE5_SHFT                           0xc
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_DQS_NE_BYTE5_BMSK                         0x700
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_DQS_NE_BYTE5_SHFT                           0x8
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_DQS_PE_BYTE4_BMSK                          0x70
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_DQS_PE_BYTE4_SHFT                           0x4
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_DQS_NE_BYTE4_BMSK                           0x7
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK1_DQS_NE_BYTE4_SHFT                           0x0

#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_ADDR                                 (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000268)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_RMSK                                 0x77777777
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_ADDR, HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_DQS_PE_BYTE3_BMSK                    0x70000000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_DQS_PE_BYTE3_SHFT                          0x1c
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_DQS_NE_BYTE3_BMSK                     0x7000000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_DQS_NE_BYTE3_SHFT                          0x18
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_DQS_PE_BYTE2_BMSK                      0x700000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_DQS_PE_BYTE2_SHFT                          0x14
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_DQS_NE_BYTE2_BMSK                       0x70000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_DQS_NE_BYTE2_SHFT                          0x10
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_DQS_PE_BYTE1_BMSK                        0x7000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_DQS_PE_BYTE1_SHFT                           0xc
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_DQS_NE_BYTE1_BMSK                         0x700
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_DQS_NE_BYTE1_SHFT                           0x8
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_DQS_PE_BYTE0_BMSK                          0x70
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_DQS_PE_BYTE0_SHFT                           0x4
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_DQS_NE_BYTE0_BMSK                           0x7
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK1_DQS_NE_BYTE0_SHFT                           0x0

#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_ADDR                                 (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000280)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_RMSK                                       0xff
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_ADDR, HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_DQCAL_COMPARE_BYTE7_BMSK                   0x80
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_DQCAL_COMPARE_BYTE7_SHFT                    0x7
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_DQCAL_COMPARE_BYTE6_BMSK                   0x40
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_DQCAL_COMPARE_BYTE6_SHFT                    0x6
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_DQCAL_COMPARE_BYTE5_BMSK                   0x20
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_DQCAL_COMPARE_BYTE5_SHFT                    0x5
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_DQCAL_COMPARE_BYTE4_BMSK                   0x10
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_DQCAL_COMPARE_BYTE4_SHFT                    0x4
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_DQCAL_COMPARE_BYTE3_BMSK                    0x8
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_DQCAL_COMPARE_BYTE3_SHFT                    0x3
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_DQCAL_COMPARE_BYTE2_BMSK                    0x4
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_DQCAL_COMPARE_BYTE2_SHFT                    0x2
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_DQCAL_COMPARE_BYTE1_BMSK                    0x2
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_DQCAL_COMPARE_BYTE1_SHFT                    0x1
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_DQCAL_COMPARE_BYTE0_BMSK                    0x1
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS0_RANK0_DQCAL_COMPARE_BYTE0_SHFT                    0x0

#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_ADDR                                 (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000284)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_RMSK                                 0x77777777
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_ADDR, HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_DQS_PE_BYTE7_BMSK                    0x70000000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_DQS_PE_BYTE7_SHFT                          0x1c
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_DQS_NE_BYTE7_BMSK                     0x7000000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_DQS_NE_BYTE7_SHFT                          0x18
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_DQS_PE_BYTE6_BMSK                      0x700000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_DQS_PE_BYTE6_SHFT                          0x14
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_DQS_NE_BYTE6_BMSK                       0x70000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_DQS_NE_BYTE6_SHFT                          0x10
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_DQS_PE_BYTE5_BMSK                        0x7000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_DQS_PE_BYTE5_SHFT                           0xc
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_DQS_NE_BYTE5_BMSK                         0x700
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_DQS_NE_BYTE5_SHFT                           0x8
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_DQS_PE_BYTE4_BMSK                          0x70
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_DQS_PE_BYTE4_SHFT                           0x4
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_DQS_NE_BYTE4_BMSK                           0x7
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS1_RANK0_DQS_NE_BYTE4_SHFT                           0x0

#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_ADDR                                 (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000288)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_RMSK                                 0x77777777
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_ADDR, HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_DQS_PE_BYTE3_BMSK                    0x70000000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_DQS_PE_BYTE3_SHFT                          0x1c
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_DQS_NE_BYTE3_BMSK                     0x7000000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_DQS_NE_BYTE3_SHFT                          0x18
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_DQS_PE_BYTE2_BMSK                      0x700000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_DQS_PE_BYTE2_SHFT                          0x14
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_DQS_NE_BYTE2_BMSK                       0x70000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_DQS_NE_BYTE2_SHFT                          0x10
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_DQS_PE_BYTE1_BMSK                        0x7000
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_DQS_PE_BYTE1_SHFT                           0xc
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_DQS_NE_BYTE1_BMSK                         0x700
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_DQS_NE_BYTE1_SHFT                           0x8
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_DQS_PE_BYTE0_BMSK                          0x70
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_DQS_PE_BYTE0_SHFT                           0x4
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_DQS_NE_BYTE0_BMSK                           0x7
#define HWIO_BIMC_S_DDR0_DPE_DQCAL_STATUS2_RANK0_DQS_NE_BYTE0_SHFT                           0x0

#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_ADDR                                            (BIMC_S_DDR0_DPE_REG_BASE      + 0x000002fc)
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_RMSK                                               0x1ffff
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_ADDR, HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_IN)
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_BANKTIMERS_BMSK                                    0x10000
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_BANKTIMERS_SHFT                                       0x10
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_COL_TIMERS_BMSK                                     0x8000
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_COL_TIMERS_SHFT                                        0xf
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_CQ_BMSK                                             0x4000
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_CQ_SHFT                                                0xe
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_CSR_ACCESS_BMSK                                     0x2000
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_CSR_ACCESS_SHFT                                        0xd
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_LOAD_TIMING_BMSK                                    0x1000
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_LOAD_TIMING_SHFT                                       0xc
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_MEMCAS_CMD_BMSK                                      0x800
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_MEMCAS_CMD_SHFT                                        0xb
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_NDX_FIFO_BMSK                                        0x400
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_NDX_FIFO_SHFT                                          0xa
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_OPT_BMSK                                             0x200
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_OPT_SHFT                                               0x9
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_PAGE_IDLE_TIMER_BMSK                                 0x100
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_PAGE_IDLE_TIMER_SHFT                                   0x8
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_PWR_MGMT_BMSK                                         0x80
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_PWR_MGMT_SHFT                                          0x7
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_RANKTIMERS_BMSK                                       0x40
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_RANKTIMERS_SHFT                                        0x6
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_RDATA_BMSK                                            0x20
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_RDATA_SHFT                                             0x5
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_RECALC_TCYC_BMSK                                      0x10
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_RECALC_TCYC_SHFT                                       0x4
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_SHKECS_BMSK                                            0x8
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_SHKECS_SHFT                                            0x3
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_WR_BUFFER_BMSK                                         0x4
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_WR_BUFFER_SHFT                                         0x2
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_WR_CTRL_BMSK                                           0x2
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_WR_CTRL_SHFT                                           0x1
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_WRDATA_MGMT_BMSK                                       0x1
#define HWIO_BIMC_S_DDR0_DPE_CGC_CTRL_WRDATA_MGMT_SHFT                                       0x0

#define HWIO_BIMC_S_DDR0_DPE_PMON_EN_ADDR                                             (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000300)
#define HWIO_BIMC_S_DDR0_DPE_PMON_EN_RMSK                                                    0x3
#define HWIO_BIMC_S_DDR0_DPE_PMON_EN_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_EN_ADDR, HWIO_BIMC_S_DDR0_DPE_PMON_EN_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_PMON_EN_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_EN_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_PMON_EN_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_PMON_EN_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_PMON_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_PMON_EN_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_PMON_EN_IN)
#define HWIO_BIMC_S_DDR0_DPE_PMON_EN_CLK_REQ_BMSK                                            0x2
#define HWIO_BIMC_S_DDR0_DPE_PMON_EN_CLK_REQ_SHFT                                            0x1
#define HWIO_BIMC_S_DDR0_DPE_PMON_EN_ENABLE_BMSK                                             0x1
#define HWIO_BIMC_S_DDR0_DPE_PMON_EN_ENABLE_SHFT                                             0x0

#define HWIO_BIMC_S_DDR0_DPE_PMON_CFG_ADDR                                            (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000304)
#define HWIO_BIMC_S_DDR0_DPE_PMON_CFG_RMSK                                                   0xb
#define HWIO_BIMC_S_DDR0_DPE_PMON_CFG_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_CFG_ADDR, HWIO_BIMC_S_DDR0_DPE_PMON_CFG_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_PMON_CFG_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_CFG_ADDR, m)
#define HWIO_BIMC_S_DDR0_DPE_PMON_CFG_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_DPE_PMON_CFG_ADDR,v)
#define HWIO_BIMC_S_DDR0_DPE_PMON_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_PMON_CFG_ADDR,m,v,HWIO_BIMC_S_DDR0_DPE_PMON_CFG_IN)
#define HWIO_BIMC_S_DDR0_DPE_PMON_CFG_HK_BUSY_BMSK                                           0x8
#define HWIO_BIMC_S_DDR0_DPE_PMON_CFG_HK_BUSY_SHFT                                           0x3
#define HWIO_BIMC_S_DDR0_DPE_PMON_CFG_OVERHEAD_BMSK                                          0x3
#define HWIO_BIMC_S_DDR0_DPE_PMON_CFG_OVERHEAD_SHFT                                          0x0

#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_QUEUED_n_ADDR(n)                                (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000310 + 0x4 * (n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_QUEUED_n_RMSK                                    0xf700300
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_QUEUED_n_MAXn                                            3
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_QUEUED_n_INI(n)        \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_CMD_QUEUED_n_ADDR(n), HWIO_BIMC_S_DDR0_DPE_PMON_CMD_QUEUED_n_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_QUEUED_n_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_CMD_QUEUED_n_ADDR(n), mask)
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_QUEUED_n_OUTI(n,val)    \
        out_dword(HWIO_BIMC_S_DDR0_DPE_PMON_CMD_QUEUED_n_ADDR(n),val)
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_QUEUED_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_PMON_CMD_QUEUED_n_ADDR(n),mask,val,HWIO_BIMC_S_DDR0_DPE_PMON_CMD_QUEUED_n_INI(n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_QUEUED_n_QC_CNT_BMSK                             0xf000000
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_QUEUED_n_QC_CNT_SHFT                                  0x18
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_QUEUED_n_QC_CMP_BMSK                              0x700000
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_QUEUED_n_QC_CMP_SHFT                                  0x14
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_QUEUED_n_QC_FLTR_BMSK                                0x300
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_QUEUED_n_QC_FLTR_SHFT                                  0x8

#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_ADDR(n)                                  (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000320 + 0x4 * (n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_RMSK                                      0xf731373
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_MAXn                                              3
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_INI(n)        \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_ADDR(n), HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_ADDR(n), mask)
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_OUTI(n,val)    \
        out_dword(HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_ADDR(n),val)
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_ADDR(n),mask,val,HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_INI(n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_QC_CNT_BMSK                               0xf000000
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_QC_CNT_SHFT                                    0x18
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_QC_CMP_BMSK                                0x700000
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_QC_CMP_SHFT                                    0x14
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_QC_AGE_BMSK                                 0x30000
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_QC_AGE_SHFT                                    0x10
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_QC_SAME_ADDR_BMSK                            0x1000
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_QC_SAME_ADDR_SHFT                               0xc
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_QC_FLTR_BMSK                                  0x300
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_QC_FLTR_SHFT                                    0x8
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_XC_PG_ACC_BMSK                                 0x70
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_XC_PG_ACC_SHFT                                  0x4
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_XC_FLTR_BMSK                                    0x3
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_EXEC_n_XC_FLTR_SHFT                                    0x0

#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_ADDR(n)                                  (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000330 + 0x4 * (n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_RMSK                                        0x3fdff
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_MAXn                                              3
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_INI(n)        \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_ADDR(n), HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_ADDR(n), mask)
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_OUTI(n,val)    \
        out_dword(HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_ADDR(n),val)
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_ADDR(n),mask,val,HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_INI(n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_TYPE_BMSK                                   0x30000
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_TYPE_SHFT                                      0x10
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_ADDR_FLTR_BMSK                               0x8000
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_ADDR_FLTR_SHFT                                  0xf
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_ADDR_FLTR_POL_BMSK                           0x4000
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_ADDR_FLTR_POL_SHFT                              0xe
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_ADDR_FLTR_SEL_BMSK                           0x3000
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_ADDR_FLTR_SEL_SHFT                              0xc
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_MID_FLTR_BMSK                                 0x800
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_MID_FLTR_SHFT                                   0xb
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_MID_FLTR_POL_BMSK                             0x400
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_MID_FLTR_POL_SHFT                               0xa
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_MID_FLTR_SEL_BMSK                             0x100
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_MID_FLTR_SEL_SHFT                               0x8
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_LAT_LIM_BMSK                                   0x80
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_LAT_LIM_SHFT                                    0x7
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_LAT_LIM_POL_BMSK                               0x40
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_LAT_LIM_POL_SHFT                                0x6
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_LAT_LIM_SEL_BMSK                               0x30
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_LAT_LIM_SEL_SHFT                                0x4
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_PRI_FLTR_BMSK                                   0x8
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_PRI_FLTR_SHFT                                   0x3
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_PRI_FLTR_POL_BMSK                               0x4
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_PRI_FLTR_POL_SHFT                               0x2
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_PRI_FLTR_SEL_BMSK                               0x3
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMD_FLTR_n_PRI_FLTR_SEL_SHFT                               0x0

#define HWIO_BIMC_S_DDR0_DPE_PMON_ADDR_FLTR_n_ADDR(n)                                 (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000340 + 0x4 * (n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_ADDR_FLTR_n_RMSK                                        0x30ff
#define HWIO_BIMC_S_DDR0_DPE_PMON_ADDR_FLTR_n_MAXn                                             3
#define HWIO_BIMC_S_DDR0_DPE_PMON_ADDR_FLTR_n_INI(n)        \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_ADDR_FLTR_n_ADDR(n), HWIO_BIMC_S_DDR0_DPE_PMON_ADDR_FLTR_n_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_PMON_ADDR_FLTR_n_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_ADDR_FLTR_n_ADDR(n), mask)
#define HWIO_BIMC_S_DDR0_DPE_PMON_ADDR_FLTR_n_OUTI(n,val)    \
        out_dword(HWIO_BIMC_S_DDR0_DPE_PMON_ADDR_FLTR_n_ADDR(n),val)
#define HWIO_BIMC_S_DDR0_DPE_PMON_ADDR_FLTR_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_PMON_ADDR_FLTR_n_ADDR(n),mask,val,HWIO_BIMC_S_DDR0_DPE_PMON_ADDR_FLTR_n_INI(n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_ADDR_FLTR_n_RANK_BMSK                                   0x3000
#define HWIO_BIMC_S_DDR0_DPE_PMON_ADDR_FLTR_n_RANK_SHFT                                      0xc
#define HWIO_BIMC_S_DDR0_DPE_PMON_ADDR_FLTR_n_BANK_BMSK                                     0xff
#define HWIO_BIMC_S_DDR0_DPE_PMON_ADDR_FLTR_n_BANK_SHFT                                      0x0

#define HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_ADDR(n)                                  (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000350 + 0x4 * (n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_RMSK                                     0xffffffff
#define HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_MAXn                                              1
#define HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_INI(n)        \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_ADDR(n), HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_ADDR(n), mask)
#define HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_OUTI(n,val)    \
        out_dword(HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_ADDR(n),val)
#define HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_ADDR(n),mask,val,HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_INI(n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_BID_MASK_BMSK                            0xe0000000
#define HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_BID_MASK_SHFT                                  0x1d
#define HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_PID_MASK_BMSK                            0x1f000000
#define HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_PID_MASK_SHFT                                  0x18
#define HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_MID_MASK_BMSK                              0xff0000
#define HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_MID_MASK_SHFT                                  0x10
#define HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_BID_MATCH_BMSK                               0xe000
#define HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_BID_MATCH_SHFT                                  0xd
#define HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_PID_MATCH_BMSK                               0x1f00
#define HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_PID_MATCH_SHFT                                  0x8
#define HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_MID_MATCH_BMSK                                 0xff
#define HWIO_BIMC_S_DDR0_DPE_PMON_MID_FLTR_n_MID_MATCH_SHFT                                  0x0

#define HWIO_BIMC_S_DDR0_DPE_PMON_LAT_LIM_n_ADDR(n)                                   (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000360 + 0x4 * (n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_LAT_LIM_n_RMSK                                          0xffff
#define HWIO_BIMC_S_DDR0_DPE_PMON_LAT_LIM_n_MAXn                                               3
#define HWIO_BIMC_S_DDR0_DPE_PMON_LAT_LIM_n_INI(n)        \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_LAT_LIM_n_ADDR(n), HWIO_BIMC_S_DDR0_DPE_PMON_LAT_LIM_n_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_PMON_LAT_LIM_n_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_LAT_LIM_n_ADDR(n), mask)
#define HWIO_BIMC_S_DDR0_DPE_PMON_LAT_LIM_n_OUTI(n,val)    \
        out_dword(HWIO_BIMC_S_DDR0_DPE_PMON_LAT_LIM_n_ADDR(n),val)
#define HWIO_BIMC_S_DDR0_DPE_PMON_LAT_LIM_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_PMON_LAT_LIM_n_ADDR(n),mask,val,HWIO_BIMC_S_DDR0_DPE_PMON_LAT_LIM_n_INI(n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_LAT_LIM_n_CYCLES_BMSK                                   0xffff
#define HWIO_BIMC_S_DDR0_DPE_PMON_LAT_LIM_n_CYCLES_SHFT                                      0x0

#define HWIO_BIMC_S_DDR0_DPE_PMON_PRI_FLTR_n_ADDR(n)                                  (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000370 + 0x4 * (n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_PRI_FLTR_n_RMSK                                         0x31ff
#define HWIO_BIMC_S_DDR0_DPE_PMON_PRI_FLTR_n_MAXn                                              3
#define HWIO_BIMC_S_DDR0_DPE_PMON_PRI_FLTR_n_INI(n)        \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_PRI_FLTR_n_ADDR(n), HWIO_BIMC_S_DDR0_DPE_PMON_PRI_FLTR_n_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_PMON_PRI_FLTR_n_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_PRI_FLTR_n_ADDR(n), mask)
#define HWIO_BIMC_S_DDR0_DPE_PMON_PRI_FLTR_n_OUTI(n,val)    \
        out_dword(HWIO_BIMC_S_DDR0_DPE_PMON_PRI_FLTR_n_ADDR(n),val)
#define HWIO_BIMC_S_DDR0_DPE_PMON_PRI_FLTR_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_PMON_PRI_FLTR_n_ADDR(n),mask,val,HWIO_BIMC_S_DDR0_DPE_PMON_PRI_FLTR_n_INI(n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_PRI_FLTR_n_PRI_CMP_BMSK                                 0x3000
#define HWIO_BIMC_S_DDR0_DPE_PMON_PRI_FLTR_n_PRI_CMP_SHFT                                    0xc
#define HWIO_BIMC_S_DDR0_DPE_PMON_PRI_FLTR_n_PRI_CUR_BMSK                                  0x1f0
#define HWIO_BIMC_S_DDR0_DPE_PMON_PRI_FLTR_n_PRI_CUR_SHFT                                    0x4
#define HWIO_BIMC_S_DDR0_DPE_PMON_PRI_FLTR_n_PRI_ORIG_BMSK                                   0xf
#define HWIO_BIMC_S_DDR0_DPE_PMON_PRI_FLTR_n_PRI_ORIG_SHFT                                   0x0

#define HWIO_BIMC_S_DDR0_DPE_PMON_CMDQ_n_ADDR(n)                                      (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000380 + 0x4 * (n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMDQ_n_RMSK                                               0xf7
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMDQ_n_MAXn                                                  1
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMDQ_n_INI(n)        \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_CMDQ_n_ADDR(n), HWIO_BIMC_S_DDR0_DPE_PMON_CMDQ_n_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMDQ_n_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_CMDQ_n_ADDR(n), mask)
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMDQ_n_OUTI(n,val)    \
        out_dword(HWIO_BIMC_S_DDR0_DPE_PMON_CMDQ_n_ADDR(n),val)
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMDQ_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_PMON_CMDQ_n_ADDR(n),mask,val,HWIO_BIMC_S_DDR0_DPE_PMON_CMDQ_n_INI(n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMDQ_n_CNT_BMSK                                           0xf0
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMDQ_n_CNT_SHFT                                            0x4
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMDQ_n_CMP_BMSK                                            0x7
#define HWIO_BIMC_S_DDR0_DPE_PMON_CMDQ_n_CMP_SHFT                                            0x0

#define HWIO_BIMC_S_DDR0_DPE_PMON_WRBUF_n_ADDR(n)                                     (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000388 + 0x4 * (n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_WRBUF_n_RMSK                                             0x3f7
#define HWIO_BIMC_S_DDR0_DPE_PMON_WRBUF_n_MAXn                                                 1
#define HWIO_BIMC_S_DDR0_DPE_PMON_WRBUF_n_INI(n)        \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_WRBUF_n_ADDR(n), HWIO_BIMC_S_DDR0_DPE_PMON_WRBUF_n_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_PMON_WRBUF_n_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_WRBUF_n_ADDR(n), mask)
#define HWIO_BIMC_S_DDR0_DPE_PMON_WRBUF_n_OUTI(n,val)    \
        out_dword(HWIO_BIMC_S_DDR0_DPE_PMON_WRBUF_n_ADDR(n),val)
#define HWIO_BIMC_S_DDR0_DPE_PMON_WRBUF_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_PMON_WRBUF_n_ADDR(n),mask,val,HWIO_BIMC_S_DDR0_DPE_PMON_WRBUF_n_INI(n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_WRBUF_n_CNT_BMSK                                         0x3f0
#define HWIO_BIMC_S_DDR0_DPE_PMON_WRBUF_n_CNT_SHFT                                           0x4
#define HWIO_BIMC_S_DDR0_DPE_PMON_WRBUF_n_CMP_BMSK                                           0x7
#define HWIO_BIMC_S_DDR0_DPE_PMON_WRBUF_n_CMP_SHFT                                           0x0

#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_ADDR(n)                                   (BIMC_S_DDR0_DPE_REG_BASE      + 0x00000390 + 0x4 * (n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_RMSK                                      0xfd07ff1f
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_MAXn                                               3
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_INI(n)        \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_ADDR(n), HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_ADDR(n), mask)
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_OUTI(n,val)    \
        out_dword(HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_ADDR(n),val)
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_ADDR(n),mask,val,HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_INI(n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_ADDR_FLTR_BMSK                            0x80000000
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_ADDR_FLTR_SHFT                                  0x1f
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_ADDR_FLTR_POL_BMSK                        0x40000000
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_ADDR_FLTR_POL_SHFT                              0x1e
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_ADDR_FLTR_SEL_BMSK                        0x30000000
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_ADDR_FLTR_SEL_SHFT                              0x1c
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_MID_FLTR_BMSK                              0x8000000
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_MID_FLTR_SHFT                                   0x1b
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_MID_FLTR_POL_BMSK                          0x4000000
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_MID_FLTR_POL_SHFT                               0x1a
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_MID_FLTR_SEL_BMSK                          0x1000000
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_MID_FLTR_SEL_SHFT                               0x18
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_CMD_BMSK                                     0x7ff00
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_CMD_SHFT                                         0x8
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_CMD_MOD_BMSK                                    0x1f
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_CMD_n_CMD_MOD_SHFT                                     0x0

#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_ADDR(n)                                  (BIMC_S_DDR0_DPE_REG_BASE      + 0x000003a0 + 0x4 * (n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_RMSK                                           0xdf
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_MAXn                                              1
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_INI(n)        \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_ADDR(n), HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_ADDR(n), mask)
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_OUTI(n,val)    \
        out_dword(HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_ADDR(n),val)
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_ADDR(n),mask,val,HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_INI(n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_MID_FLTR_BMSK                                  0x80
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_MID_FLTR_SHFT                                   0x7
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_MID_FLTR_POL_BMSK                              0x40
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_MID_FLTR_POL_SHFT                               0x6
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_MID_FLTR_SEL_BMSK                              0x10
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_MID_FLTR_SEL_SHFT                               0x4
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_TYPE_BMSK                                       0xf
#define HWIO_BIMC_S_DDR0_DPE_PMON_DDR_DATA_n_TYPE_SHFT                                       0x0

#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_ADDR(n)                                (BIMC_S_DDR0_DPE_REG_BASE      + 0x000003b0 + 0x4 * (n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_RMSK                                   0x31ff7fef
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_MAXn                                            3
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_INI(n)        \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_ADDR(n), HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_ADDR(n), mask)
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_OUTI(n,val)    \
        out_dword(HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_ADDR(n),val)
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_ADDR(n),mask,val,HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_INI(n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_RNK_BMSK                               0x30000000
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_RNK_SHFT                                     0x1c
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_RNK_MATCH_BMSK                          0x1000000
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_RNK_MATCH_SHFT                               0x18
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_ACTUAL_POL_BMSK                          0x800000
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_ACTUAL_POL_SHFT                              0x17
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_ACTUAL_BMSK                              0x7f0000
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_ACTUAL_SHFT                                  0x10
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_ALLOWED_BMSK                               0x7000
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_ALLOWED_SHFT                                  0xc
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_CMD_MATCH_BMSK                              0x800
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_CMD_MATCH_SHFT                                0xb
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_CMD_BMSK                                    0x700
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_CMD_SHFT                                      0x8
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_BNK_CMP_BMSK                                 0xe0
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_BNK_CMP_SHFT                                  0x5
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_BNK_CNT_BMSK                                  0xf
#define HWIO_BIMC_S_DDR0_DPE_PMON_RANK_STATE_n_BNK_CNT_SHFT                                  0x0

#define HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_ADDR(n)                                      (BIMC_S_DDR0_DPE_REG_BASE      + 0x000003c0 + 0x4 * (n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_RMSK                                          0x3ff031f
#define HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_MAXn                                                  7
#define HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_INI(n)        \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_ADDR(n), HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_RMSK)
#define HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_ADDR(n), mask)
#define HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_OUTI(n,val)    \
        out_dword(HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_ADDR(n),val)
#define HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_ADDR(n),mask,val,HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_INI(n))
#define HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_RNK_BMSK                                      0x3000000
#define HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_RNK_SHFT                                           0x18
#define HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_REF_RATE_BMSK                                  0xff0000
#define HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_REF_RATE_SHFT                                      0x10
#define HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_CMD_FLTR_BMSK                                     0x300
#define HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_CMD_FLTR_SHFT                                       0x8
#define HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_CMD_BMSK                                           0x1f
#define HWIO_BIMC_S_DDR0_DPE_PMON_SHKE_n_CMD_SHFT                                            0x0

/*----------------------------------------------------------------------------
 * MODULE: BIMC_S_DDR0_SCMO
 *--------------------------------------------------------------------------*/

#define BIMC_S_DDR0_SCMO_REG_BASE                                                    (BIMC_BASE      + 0x00048000)

#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_ADDR                                    (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000000)
#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_RMSK                                      0xffffff
#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_ADDR, HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_INSTANCE_BMSK                             0xff0000
#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_INSTANCE_SHFT                                 0x10
#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_SUB_TYPE_BMSK                               0xff00
#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_SUB_TYPE_SHFT                                  0x8
#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_TYPE_BMSK                                     0xff
#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_TYPE_SHFT                                      0x0

#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_ADDR                                           (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000010)
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_RMSK                                           0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_HW_INFO_ADDR, HWIO_BIMC_S_DDR0_SCMO_HW_INFO_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_HW_INFO_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_MAJOR_BMSK                                     0xff000000
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_MAJOR_SHFT                                           0x18
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_BRANCH_BMSK                                      0xff0000
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_BRANCH_SHFT                                          0x10
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_MINOR_BMSK                                         0xff00
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_MINOR_SHFT                                            0x8
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_ECO_BMSK                                             0xff
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_ECO_SHFT                                              0x0

#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_ADDR                              (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000020)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_RMSK                              0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_ADDR, HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_DATA_WIDTH_BMSK                   0xffff0000
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_DATA_WIDTH_SHFT                         0x10
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_TID_WIDTH_BMSK                        0xff00
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_TID_WIDTH_SHFT                           0x8
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_MID_WIDTH_BMSK                          0xff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_MID_WIDTH_SHFT                           0x0

#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_1_ADDR                              (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000030)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_1_RMSK                              0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_1_ADDR, HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_1_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_1_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_1_MPORT_CONNECTIVITY_BMSK           0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_1_MPORT_CONNECTIVITY_SHFT                  0x0

#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_ADDR                              (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000040)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_RMSK                                0xff00ff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_ADDR, HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_NUM_GLOBAL_MONS_BMSK                0xff0000
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_NUM_GLOBAL_MONS_SHFT                    0x10
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_VMID_WIDTH_BMSK                         0xff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_VMID_WIDTH_SHFT                          0x0

#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_ADDR                              (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000050)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_RMSK                              0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_ADDR, HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_RCH0_CTRL_DEPTH_BMSK              0xff000000
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_RCH0_CTRL_DEPTH_SHFT                    0x18
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_RCH0_DEPTH_BMSK                     0xff0000
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_RCH0_DEPTH_SHFT                         0x10
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_BCH_DEPTH_BMSK                        0xff00
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_BCH_DEPTH_SHFT                           0x8
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_WCH_DEPTH_BMSK                          0xff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_WCH_DEPTH_SHFT                           0x0

#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_ADDR                              (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000060)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_RMSK                                  0xffff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_ADDR, HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_RCH1_CTRL_DEPTH_BMSK                  0xff00
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_RCH1_CTRL_DEPTH_SHFT                     0x8
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_RCH1_DEPTH_BMSK                         0xff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_RCH1_DEPTH_SHFT                          0x0

#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_ADDR                              (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000070)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_RMSK                                  0xffff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_ADDR, HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_DPE_CQ_DEPTH_BMSK                     0xff00
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_DPE_CQ_DEPTH_SHFT                        0x8
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_DDR_BUS_WIDTH_BMSK                      0xff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_DDR_BUS_WIDTH_SHFT                       0x0

#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_ADDR                              (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000080)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_RMSK                                  0x1111
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_ADDR, HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_WBUFC_PIPE_BMSK                       0x1000
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_WBUFC_PIPE_SHFT                          0xc
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_RDOPT_PIPE_BMSK                        0x100
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_RDOPT_PIPE_SHFT                          0x8
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_ACHAN_INTF_PIPE_BMSK                    0x10
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_ACHAN_INTF_PIPE_SHFT                     0x4
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_ADDR_DECODE_HT_BMSK                      0x1
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_ADDR_DECODE_HT_SHFT                      0x0

#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_ADDR                                  (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000100)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_RMSK                                         0x1
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_ADDR, HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_ERR_OCCURRED_BMSK                            0x1
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_ERR_OCCURRED_SHFT                            0x0

#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_CLEAR_ADDR                                   (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000108)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_CLEAR_RMSK                                          0x1
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_CLEAR_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_CLEAR_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_CLEAR_IRQ_CLR_BMSK                                  0x1
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_CLEAR_IRQ_CLR_SHFT                                  0x0

#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_ADDR                                  (BIMC_S_DDR0_SCMO_REG_BASE      + 0x0000010c)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_RMSK                                         0x1
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_ADDR, HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_IN)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_IRQ_EN_BMSK                                  0x1
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_IRQ_EN_SHFT                                  0x0

#define HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_ADDR                                         (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000120)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_RMSK                                         0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_ADDR, HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_ERR_ADDR_L_BMSK                              0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_ERR_ADDR_L_SHFT                                     0x0

#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ADDR                                    (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000128)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_RMSK                                    0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ADDR, HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ERR_ATID_BMSK                           0xffff0000
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ERR_ATID_SHFT                                 0x10
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ERR_AMID_BMSK                               0xffff
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ERR_AMID_SHFT                                  0x0

#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ADDR                                    (BIMC_S_DDR0_SCMO_REG_BASE      + 0x0000012c)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_RMSK                                     0x10ff117
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ADDR, HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_CODE_BMSK                            0x1000000
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_CODE_SHFT                                 0x18
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_ALEN_BMSK                              0xf0000
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_ALEN_SHFT                                 0x10
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_ASIZE_BMSK                              0xe000
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_ASIZE_SHFT                                 0xd
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_ABURST_BMSK                             0x1000
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_ABURST_SHFT                                0xc
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_AEXCLUSIVE_BMSK                          0x100
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_AEXCLUSIVE_SHFT                            0x8
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_APROTNS_BMSK                              0x10
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_APROTNS_SHFT                               0x4
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_AOOORD_BMSK                                0x4
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_AOOORD_SHFT                                0x2
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_AOOOWR_BMSK                                0x2
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_AOOOWR_SHFT                                0x1
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_AWRITE_BMSK                                0x1
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_AWRITE_SHFT                                0x0

#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_ADDR                                    (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000130)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_RMSK                                          0x1f
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_ADDR, HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_ERR_AVMID_BMSK                                0x1f
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_ERR_AVMID_SHFT                                 0x0

#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_ADDR                                        (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000200)
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_RMSK                                        0xfff11111
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_ADDR, HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_IN)
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_RFU_31_21_BMSK                              0xffe00000
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_RFU_31_21_SHFT                                    0x15
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_CFG_WR_CORE_CG_EN_BMSK                        0x100000
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_CFG_WR_CORE_CG_EN_SHFT                            0x14
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_PEN_CMD_CG_EN_BMSK                             0x10000
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_PEN_CMD_CG_EN_SHFT                                0x10
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_RCH_CG_EN_BMSK                                  0x1000
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_RCH_CG_EN_SHFT                                     0xc
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_FLUSH_CG_EN_BMSK                                 0x100
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_FLUSH_CG_EN_SHFT                                   0x8
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_WCH_CG_EN_BMSK                                    0x10
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_WCH_CG_EN_SHFT                                     0x4
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_ACH_CG_EN_BMSK                                     0x1
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_ACH_CG_EN_SHFT                                     0x0

#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_ADDR                                (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000400)
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_RMSK                                     0x3ff
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_ADDR, HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_IN)
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_RANK_INTERLEAVE_BMSK                     0x300
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_RANK_INTERLEAVE_SHFT                       0x8
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_RFU_7_5_BMSK                              0xe0
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_RFU_7_5_SHFT                               0x5
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_INTERLEAVE_CS1_BMSK                       0x10
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_INTERLEAVE_CS1_SHFT                        0x4
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_RFU_3_1_BMSK                               0xe
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_RFU_3_1_SHFT                               0x1
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_INTERLEAVE_CS0_BMSK                        0x1
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_INTERLEAVE_CS0_SHFT                        0x0

#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_ADDR(n)                                  (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000410 + 0x4 * (n))
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_RMSK                                         0xffff
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_MAXn                                              1
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_INI(n)        \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_ADDR(n), HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_ADDR(n), mask)
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_OUTI(n,val)    \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_ADDR(n),val)
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_ADDR(n),mask,val,HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_INI(n))
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_RFU_15_10_BMSK                               0xfc00
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_RFU_15_10_SHFT                                  0xa
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_ADDR_BASE_BMSK                                0x3fc
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_ADDR_BASE_SHFT                                  0x2
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_RFU_1_0_BMSK                                    0x3
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_RFU_1_0_SHFT                                    0x0

#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_ADDR(n)                                   (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000420 + 0x4 * (n))
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RMSK                                          0xffff
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_MAXn                                               1
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_INI(n)        \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_ADDR(n), HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_ADDR(n), mask)
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_OUTI(n,val)    \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_ADDR(n),val)
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_ADDR(n),mask,val,HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_INI(n))
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RANK_EN_BMSK                                  0x8000
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RANK_EN_SHFT                                     0xf
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RFU_14_13_BMSK                                0x6000
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RFU_14_13_SHFT                                   0xd
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_ADDR_MODE_BMSK                                0x1000
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_ADDR_MODE_SHFT                                   0xc
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RFU_11_9_BMSK                                  0xe00
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RFU_11_9_SHFT                                    0x9
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_BANK_SIZE_BMSK                                 0x100
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_BANK_SIZE_SHFT                                   0x8
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RFU_7_6_BMSK                                    0xc0
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RFU_7_6_SHFT                                     0x6
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_ROW_SIZE_BMSK                                   0x30
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_ROW_SIZE_SHFT                                    0x4
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RFU_3_2_BMSK                                     0xc
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RFU_3_2_SHFT                                     0x2
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_COL_SIZE_BMSK                                    0x3
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_COL_SIZE_SHFT                                    0x0

#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_ADDR(n)                                  (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000430 + 0x4 * (n))
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_RMSK                                         0xffff
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_MAXn                                              1
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_INI(n)        \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_ADDR(n), HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_ADDR(n), mask)
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_OUTI(n,val)    \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_ADDR(n),val)
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_ADDR(n),mask,val,HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_INI(n))
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_RFU_15_10_BMSK                               0xfc00
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_RFU_15_10_SHFT                                  0xa
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_ADDR_MASK_BMSK                                0x3fc
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_ADDR_MASK_SHFT                                  0x2
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_RFU_1_0_BMSK                                    0x3
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_RFU_1_0_SHFT                                    0x0

#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_ADDR                                        (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000450)
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_RMSK                                            0xff31
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_ADDR, HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_GLOBAL_MONS_IN_USE_BMSK                         0xff00
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_GLOBAL_MONS_IN_USE_SHFT                            0x8
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_RANK_IDLE_BMSK                                    0x30
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_RANK_IDLE_SHFT                                     0x4
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_SLAVE_IDLE_BMSK                                    0x1
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_SLAVE_IDLE_SHFT                                    0x0

#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_ADDR                                    (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000460)
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_RMSK                                          0x1f
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_ADDR, HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_IN)
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_EXCMD_ADDR_GRAN_BMSK                          0x10
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_EXCMD_ADDR_GRAN_SHFT                           0x4
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_RFU_3_2_BMSK                                   0xc
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_RFU_3_2_SHFT                                   0x2
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_CLR_EXMON_EN_BMSK                              0x2
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_CLR_EXMON_EN_SHFT                              0x1
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_EXRD_RESP_BMSK                                 0x1
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_EXRD_RESP_SHFT                                 0x0

#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_ADDR                                       (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000500)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_RMSK                                            0xf1f
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_ADDR, HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_IN)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_RFU_11_10_BMSK                                  0xc00
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_RFU_11_10_SHFT                                    0xa
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_CMD_ORDERING_BMSK                               0x300
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_CMD_ORDERING_SHFT                                 0x8
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_HP_CMD_AREQPRIO_MAP_BMSK                         0x10
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_HP_CMD_AREQPRIO_MAP_SHFT                          0x4
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_RFU_3_BMSK                                        0x8
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_RFU_3_SHFT                                        0x3
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_HP_CMD_Q_DEPTH_BMSK                               0x7
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_HP_CMD_Q_DEPTH_SHFT                               0x0

#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_ADDR                                    (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000520)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_RMSK                                          0xff
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_ADDR, HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_HP_CMD_BUF_ENTRIES_IN_USE_BMSK                0xf0
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_HP_CMD_BUF_ENTRIES_IN_USE_SHFT                 0x4
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_LP_CMD_BUF_ENTRIES_IN_USE_BMSK                 0xf
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_LP_CMD_BUF_ENTRIES_IN_USE_SHFT                 0x0

#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_ADDR                                        (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000540)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_RMSK                                        0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_ADDR, HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_IN)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_DYNAMIC_RCH_SELECT_BMSK                     0xffff0000
#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_DYNAMIC_RCH_SELECT_SHFT                           0x10
#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_RCH_PORTS_BMSK                                  0xffff
#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_RCH_PORTS_SHFT                                     0x0

#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_ADDR                                      (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000544)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RMSK                                      0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_ADDR, HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_IN)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH1_FIFO_BKPR_HI_TH_BMSK                 0xff000000
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH1_FIFO_BKPR_HI_TH_SHFT                       0x18
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH1_FIFO_BKPR_LO_TH_BMSK                   0xff0000
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH1_FIFO_BKPR_LO_TH_SHFT                       0x10
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH0_FIFO_BKPR_HI_TH_BMSK                     0xff00
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH0_FIFO_BKPR_HI_TH_SHFT                        0x8
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH0_FIFO_BKPR_LO_TH_BMSK                       0xff
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH0_FIFO_BKPR_LO_TH_SHFT                        0x0

#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_ADDR                                        (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000560)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RMSK                                           0x33333
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_ADDR, HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_PRQ_FIFO_FULL_BMSK                             0x20000
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_PRQ_FIFO_FULL_SHFT                                0x11
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_PRQ_FIFO_EMPTY_BMSK                            0x10000
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_PRQ_FIFO_EMPTY_SHFT                               0x10
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH1_QUAL_FIFO_FULL_BMSK                        0x2000
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH1_QUAL_FIFO_FULL_SHFT                           0xd
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH1_QUAL_FIFO_EMPTY_BMSK                       0x1000
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH1_QUAL_FIFO_EMPTY_SHFT                          0xc
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH1_DATA_FIFO_FULL_BMSK                         0x200
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH1_DATA_FIFO_FULL_SHFT                           0x9
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH1_DATA_FIFO_EMPTY_BMSK                        0x100
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH1_DATA_FIFO_EMPTY_SHFT                          0x8
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH0_QUAL_FIFO_FULL_BMSK                          0x20
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH0_QUAL_FIFO_FULL_SHFT                           0x5
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH0_QUAL_FIFO_EMPTY_BMSK                         0x10
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH0_QUAL_FIFO_EMPTY_SHFT                          0x4
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH0_DATA_FIFO_FULL_BMSK                           0x2
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH0_DATA_FIFO_FULL_SHFT                           0x1
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH0_DATA_FIFO_EMPTY_BMSK                          0x1
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH0_DATA_FIFO_EMPTY_SHFT                          0x0

#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_ADDR                                       (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000580)
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_RMSK                                             0xff
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_ADDR, HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_IN)
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_RFU_7_5_BMSK                                     0xe0
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_RFU_7_5_SHFT                                      0x5
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_WRITE_BLOCK_READ_BMSK                            0x10
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_WRITE_BLOCK_READ_SHFT                             0x4
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_RFU_3_1_BMSK                                      0xe
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_RFU_3_1_SHFT                                      0x1
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_COALESCE_EN_BMSK                                  0x1
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_COALESCE_EN_SHFT                                  0x0

#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_ADDR                                        (BIMC_S_DDR0_SCMO_REG_BASE      + 0x000005a0)
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_RMSK                                             0x333
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_ADDR, HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_BRESP_FIFO_FULL_BMSK                             0x200
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_BRESP_FIFO_FULL_SHFT                               0x9
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_BRESP_FIFO_EMPTY_BMSK                            0x100
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_BRESP_FIFO_EMPTY_SHFT                              0x8
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_WDATA_FIFO_FULL_BMSK                              0x20
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_WDATA_FIFO_FULL_SHFT                               0x5
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_WDATA_FIFO_EMPTY_BMSK                             0x10
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_WDATA_FIFO_EMPTY_SHFT                              0x4
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_WBUF_FULL_BMSK                                     0x2
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_WBUF_FULL_SHFT                                     0x1
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_WBUF_EMPTY_BMSK                                    0x1
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_WBUF_EMPTY_SHFT                                    0x0

#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_ADDR                                         (BIMC_S_DDR0_SCMO_REG_BASE      + 0x000005c0)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RMSK                                         0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_ADDR, HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_IN)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_BL16_ALIGN_FLUSH_EN_BMSK                     0x80000000
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_BL16_ALIGN_FLUSH_EN_SHFT                           0x1f
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RFU_30_29_BMSK                               0x60000000
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RFU_30_29_SHFT                                     0x1d
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_FLUSH_IN_ORDER_BMSK                          0x10000000
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_FLUSH_IN_ORDER_SHFT                                0x1c
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RFU_27_26_BMSK                                0xc000000
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RFU_27_26_SHFT                                     0x1a
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_FLUSH_IDLE_DELAY_BMSK                         0x3ff0000
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_FLUSH_IDLE_DELAY_SHFT                              0x10
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RFU_15_12_BMSK                                   0xf000
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RFU_15_12_SHFT                                      0xc
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_FLUSH_UPPER_LIMIT_BMSK                            0xf00
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_FLUSH_UPPER_LIMIT_SHFT                              0x8
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RFU_7_4_BMSK                                       0xf0
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RFU_7_4_SHFT                                        0x4
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_FLUSH_LOWER_LIMIT_BMSK                              0xf
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_FLUSH_LOWER_LIMIT_SHFT                              0x0

#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_ADDR                                         (BIMC_S_DDR0_SCMO_REG_BASE      + 0x000005c4)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_RMSK                                                0xf
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_ADDR, HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_IN)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_RFU_3_2_BMSK                                        0xc
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_RFU_3_2_SHFT                                        0x2
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_FLUSH_ALL_BUF_BMSK                                  0x3
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_FLUSH_ALL_BUF_SHFT                                  0x0

#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_ADDR                                      (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000700)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RMSK                                        0xffffff
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_ADDR, HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_IN)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_23_21_BMSK                              0xe00000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_23_21_SHFT                                  0x15
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_IGNORE_BANK_UNAVL_BMSK                      0x100000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_IGNORE_BANK_UNAVL_SHFT                          0x14
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_19_18_BMSK                               0xc0000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_19_18_SHFT                                  0x12
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_MASK_CMDOUT_PRI_BMSK                         0x30000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_MASK_CMDOUT_PRI_SHFT                            0x10
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_15_13_BMSK                                0xe000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_15_13_SHFT                                   0xd
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_DPE_CMD_REORDERING_BMSK                       0x1000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_DPE_CMD_REORDERING_SHFT                          0xc
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_11_9_BMSK                                  0xe00
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_11_9_SHFT                                    0x9
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_WR_OPT_EN_BMSK                                 0x100
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_WR_OPT_EN_SHFT                                   0x8
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_7_5_BMSK                                    0xe0
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_7_5_SHFT                                     0x5
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RD_OPT_EN_BMSK                                  0x10
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RD_OPT_EN_SHFT                                   0x4
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_3_1_BMSK                                     0xe
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_3_1_SHFT                                     0x1
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_PAGE_MGMT_POLICY_BMSK                            0x1
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_PAGE_MGMT_POLICY_SHFT                            0x0

#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_ADDR                                      (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000704)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RMSK                                      0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_ADDR, HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_IN)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RFU_31_29_BMSK                            0xe0000000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RFU_31_29_SHFT                                  0x1d
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_HSTP_CMD_TIMEOUT_BMSK                     0x1f000000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_HSTP_CMD_TIMEOUT_SHFT                           0x18
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RFU_23_21_BMSK                              0xe00000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RFU_23_21_SHFT                                  0x15
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_HP_CMD_TIMEOUT_BMSK                         0x1f0000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_HP_CMD_TIMEOUT_SHFT                             0x10
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RFU_15_13_BMSK                                0xe000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RFU_15_13_SHFT                                   0xd
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_MP_CMD_TIMEOUT_BMSK                           0x1f00
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_MP_CMD_TIMEOUT_SHFT                              0x8
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RFU_7_5_BMSK                                    0xe0
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RFU_7_5_SHFT                                     0x5
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_LP_CMD_TIMEOUT_BMSK                             0x1f
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_LP_CMD_TIMEOUT_SHFT                              0x0

#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_ADDR                                      (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000708)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RMSK                                          0xffff
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_ADDR, HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_IN)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RFU_15_12_BMSK                                0xf000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RFU_15_12_SHFT                                   0xc
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RWOPT_WR_CMD_TIMEOUT_BMSK                      0xf00
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RWOPT_WR_CMD_TIMEOUT_SHFT                        0x8
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RFU_7_4_BMSK                                    0xf0
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RFU_7_4_SHFT                                     0x4
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RWOPT_RD_CMD_TIMEOUT_BMSK                        0xf
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RWOPT_RD_CMD_TIMEOUT_SHFT                        0x0

#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_ADDR                                      (BIMC_S_DDR0_SCMO_REG_BASE      + 0x0000070c)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_RMSK                                      0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_ADDR, HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_IN)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_RFU_31_21_BMSK                            0xffe00000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_RFU_31_21_SHFT                                  0x15
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_WROPT_CMD_TIMEOUT_BMSK                      0x1f0000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_WROPT_CMD_TIMEOUT_SHFT                          0x10
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_RFU_15_5_BMSK                                 0xffe0
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_RFU_15_5_SHFT                                    0x5
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_FLUSH_CMD_TIMEOUT_BMSK                          0x1f
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_FLUSH_CMD_TIMEOUT_SHFT                           0x0

#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_ADDR                                      (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000710)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_RMSK                                            0xff
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_ADDR, HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_IN)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_RFU_7_5_BMSK                                    0xe0
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_RFU_7_5_SHFT                                     0x5
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_MASK_WRQ_FULL_BMSK                              0x10
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_MASK_WRQ_FULL_SHFT                               0x4
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_RFU_3_1_BMSK                                     0xe
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_RFU_3_1_SHFT                                     0x1
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_MASK_RDQ_FULL_BMSK                               0x1
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_MASK_RDQ_FULL_SHFT                               0x0

#define HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_ADDR                                        (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000a40)
#define HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_RMSK                                               0x7
#define HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_ADDR, HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_FREQ_SET_POINT_REG_IN_USE_BMSK                     0x7
#define HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_FREQ_SET_POINT_REG_IN_USE_SHFT                     0x0

/*----------------------------------------------------------------------------
 * MODULE: BIMC_S_DDR0_SHKE
 *--------------------------------------------------------------------------*/

#define BIMC_S_DDR0_SHKE_REG_BASE                                                              (BIMC_BASE      + 0x0004d000)

#define HWIO_BIMC_S_DDR0_SHKE_HW_INFO_ADDR                                                     (BIMC_S_DDR0_SHKE_REG_BASE      + 0x00000000)
#define HWIO_BIMC_S_DDR0_SHKE_HW_INFO_RMSK                                                     0xffffffff
#define HWIO_BIMC_S_DDR0_SHKE_HW_INFO_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_HW_INFO_ADDR, HWIO_BIMC_S_DDR0_SHKE_HW_INFO_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_HW_INFO_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_HW_INFO_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_HW_INFO_MAJOR_REVISION_BMSK                                      0xff000000
#define HWIO_BIMC_S_DDR0_SHKE_HW_INFO_MAJOR_REVISION_SHFT                                            0x18
#define HWIO_BIMC_S_DDR0_SHKE_HW_INFO_BRANCH_REVISION_BMSK                                       0xff0000
#define HWIO_BIMC_S_DDR0_SHKE_HW_INFO_BRANCH_REVISION_SHFT                                           0x10
#define HWIO_BIMC_S_DDR0_SHKE_HW_INFO_MINOR_REVISION_BMSK                                          0xff00
#define HWIO_BIMC_S_DDR0_SHKE_HW_INFO_MINOR_REVISION_SHFT                                             0x8
#define HWIO_BIMC_S_DDR0_SHKE_HW_INFO_ECO_REVISION_BMSK                                              0xff
#define HWIO_BIMC_S_DDR0_SHKE_HW_INFO_ECO_REVISION_SHFT                                               0x0

#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_ADDR                                                      (BIMC_S_DDR0_SHKE_REG_BASE      + 0x00000004)
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_RMSK                                                      0x333f8001
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_CONFIG_ADDR, HWIO_BIMC_S_DDR0_SHKE_CONFIG_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_CONFIG_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SHKE_CONFIG_ADDR,v)
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SHKE_CONFIG_ADDR,m,v,HWIO_BIMC_S_DDR0_SHKE_CONFIG_IN)
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_RANK1_EN_BMSK                                             0x20000000
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_RANK1_EN_SHFT                                                   0x1d
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_RANK0_EN_BMSK                                             0x10000000
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_RANK0_EN_SHFT                                                   0x1c
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_RANK1_INITCOMPLETE_BMSK                                    0x2000000
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_RANK1_INITCOMPLETE_SHFT                                         0x19
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_RANK0_INITCOMPLETE_BMSK                                    0x1000000
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_RANK0_INITCOMPLETE_SHFT                                         0x18
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_NUM_BANKS_BMSK                                              0x300000
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_NUM_BANKS_SHFT                                                  0x14
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_IOCAL_UPDATE_PULSE_WIDTH_BMSK                                0xf0000
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_IOCAL_UPDATE_PULSE_WIDTH_SHFT                                   0x10
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_IOCAL_UPDATE_PULSE_WIDTH_MULT_BMSK                            0x8000
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_IOCAL_UPDATE_PULSE_WIDTH_MULT_SHFT                               0xf
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_CLK_ON_EN_BMSK                                                   0x1
#define HWIO_BIMC_S_DDR0_SHKE_CONFIG_CLK_ON_EN_SHFT                                                   0x0

#define HWIO_BIMC_S_DDR0_SHKE_CGC_CNTL_ADDR                                                    (BIMC_S_DDR0_SHKE_REG_BASE      + 0x00000010)
#define HWIO_BIMC_S_DDR0_SHKE_CGC_CNTL_RMSK                                                           0x1
#define HWIO_BIMC_S_DDR0_SHKE_CGC_CNTL_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_CGC_CNTL_ADDR, HWIO_BIMC_S_DDR0_SHKE_CGC_CNTL_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_CGC_CNTL_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_CGC_CNTL_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_CGC_CNTL_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SHKE_CGC_CNTL_ADDR,v)
#define HWIO_BIMC_S_DDR0_SHKE_CGC_CNTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SHKE_CGC_CNTL_ADDR,m,v,HWIO_BIMC_S_DDR0_SHKE_CGC_CNTL_IN)
#define HWIO_BIMC_S_DDR0_SHKE_CGC_CNTL_CSR_ACCESS_BMSK                                                0x1
#define HWIO_BIMC_S_DDR0_SHKE_CGC_CNTL_CSR_ACCESS_SHFT                                                0x0

#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_ADDR                                               (BIMC_S_DDR0_SHKE_REG_BASE      + 0x00000020)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_RMSK                                               0x3ffffbf0
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_ADDR, HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_ADDR,v)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_ADDR,m,v,HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_IN)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_RANK_SEL_BMSK                                      0x30000000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_RANK_SEL_SHFT                                            0x1c
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_CK_ON_BMSK                                          0x8000000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_CK_ON_SHFT                                               0x1b
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_CKE_ON_BMSK                                         0x4000000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_CKE_ON_SHFT                                              0x1a
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_RESETN_ON_BMSK                                      0x2000000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_RESETN_ON_SHFT                                           0x19
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_CK_OFF_BMSK                                         0x1000000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_CK_OFF_SHFT                                              0x18
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_CKE_OFF_BMSK                                         0x800000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_CKE_OFF_SHFT                                             0x17
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_RESETN_OFF_BMSK                                      0x400000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_RESETN_OFF_SHFT                                          0x16
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_ENTER_DEEP_PD_BMSK                                   0x200000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_ENTER_DEEP_PD_SHFT                                       0x15
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_EXIT_DEEP_PD_BMSK                                    0x100000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_EXIT_DEEP_PD_SHFT                                        0x14
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_ZQCAL_SHORT_BMSK                                      0x80000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_ZQCAL_SHORT_SHFT                                         0x13
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_ZQCAL_LONG_BMSK                                       0x40000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_ZQCAL_LONG_SHFT                                          0x12
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_AUTO_REFRESH_BMSK                                     0x20000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_AUTO_REFRESH_SHFT                                        0x11
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_PRECHARGE_ALL_BMSK                                    0x10000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_PRECHARGE_ALL_SHFT                                       0x10
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_ENTER_SELF_REFRESH_IDLE_BMSK                           0x8000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_ENTER_SELF_REFRESH_IDLE_SHFT                              0xf
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_ENTER_SELF_REFRESH_ASAP_BMSK                           0x4000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_ENTER_SELF_REFRESH_ASAP_SHFT                              0xe
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_EXIT_SELF_REFRESH_BMSK                                 0x2000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_EXIT_SELF_REFRESH_SHFT                                    0xd
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_MODE_REGISTER_READ_BMSK                                0x1000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_MODE_REGISTER_READ_SHFT                                   0xc
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_MODE_REGISTER_WRITE_BMSK                                0x800
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_MODE_REGISTER_WRITE_SHFT                                  0xb
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_DRAM_DEBUG_CMD_BMSK                                     0x200
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_DRAM_DEBUG_CMD_SHFT                                       0x9
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_STALL_CMDS_BMSK                                         0x100
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_STALL_CMDS_SHFT                                           0x8
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_RESUME_CMDS_BMSK                                         0x80
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_RESUME_CMDS_SHFT                                          0x7
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_EXTND_MODE_REGISTER_WRITE_BMSK                           0x40
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_EXTND_MODE_REGISTER_WRITE_SHFT                            0x6
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_CA_TRAIN_PATTERN_CMD_BMSK                                0x20
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_CA_TRAIN_PATTERN_CMD_SHFT                                 0x5
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_RD_DQCAL_BMSK                                            0x10
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_0_RD_DQCAL_SHFT                                             0x4

#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_1_ADDR                                               (BIMC_S_DDR0_SHKE_REG_BASE      + 0x00000024)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_1_RMSK                                                  0x1ffff
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_1_ADDR, HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_1_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_1_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_1_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_1_ADDR,v)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_1_ADDR,m,v,HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_1_IN)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK                                0x10000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT                                   0x10
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK                        0xffff
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT                           0x0

#define HWIO_BIMC_S_DDR0_SHKE_MREG_ADDR_WDATA_CNTL_ADDR                                        (BIMC_S_DDR0_SHKE_REG_BASE      + 0x00000030)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_ADDR_WDATA_CNTL_RMSK                                         0xff0ffff
#define HWIO_BIMC_S_DDR0_SHKE_MREG_ADDR_WDATA_CNTL_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_MREG_ADDR_WDATA_CNTL_ADDR, HWIO_BIMC_S_DDR0_SHKE_MREG_ADDR_WDATA_CNTL_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_ADDR_WDATA_CNTL_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_MREG_ADDR_WDATA_CNTL_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_ADDR_WDATA_CNTL_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SHKE_MREG_ADDR_WDATA_CNTL_ADDR,v)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_ADDR_WDATA_CNTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SHKE_MREG_ADDR_WDATA_CNTL_ADDR,m,v,HWIO_BIMC_S_DDR0_SHKE_MREG_ADDR_WDATA_CNTL_IN)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_ADDR_WDATA_CNTL_MREG_ADDR_BMSK                               0xff00000
#define HWIO_BIMC_S_DDR0_SHKE_MREG_ADDR_WDATA_CNTL_MREG_ADDR_SHFT                                    0x14
#define HWIO_BIMC_S_DDR0_SHKE_MREG_ADDR_WDATA_CNTL_MREG_WDATA_BMSK                                 0xffff
#define HWIO_BIMC_S_DDR0_SHKE_MREG_ADDR_WDATA_CNTL_MREG_WDATA_SHFT                                    0x0

#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK1_U_ADDR                                          (BIMC_S_DDR0_SHKE_REG_BASE      + 0x00000050)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK1_U_RMSK                                          0xffffffff
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK1_U_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK1_U_ADDR, HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK1_U_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK1_U_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK1_U_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK1_U_MREG_RDATA_BMSK                               0xffffffff
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK1_U_MREG_RDATA_SHFT                                      0x0

#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK1_L_ADDR                                          (BIMC_S_DDR0_SHKE_REG_BASE      + 0x00000054)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK1_L_RMSK                                          0xffffffff
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK1_L_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK1_L_ADDR, HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK1_L_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK1_L_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK1_L_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK1_L_MREG_RDATA_BMSK                               0xffffffff
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK1_L_MREG_RDATA_SHFT                                      0x0

#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK0_U_ADDR                                          (BIMC_S_DDR0_SHKE_REG_BASE      + 0x00000060)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK0_U_RMSK                                          0xffffffff
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK0_U_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK0_U_ADDR, HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK0_U_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK0_U_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK0_U_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK0_U_MREG_RDATA_BMSK                               0xffffffff
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK0_U_MREG_RDATA_SHFT                                      0x0

#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK0_L_ADDR                                          (BIMC_S_DDR0_SHKE_REG_BASE      + 0x00000064)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK0_L_RMSK                                          0xffffffff
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK0_L_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK0_L_ADDR, HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK0_L_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK0_L_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK0_L_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK0_L_MREG_RDATA_BMSK                               0xffffffff
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_RANK0_L_MREG_RDATA_SHFT                                      0x0

#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_STATUS_ADDR                                           (BIMC_S_DDR0_SHKE_REG_BASE      + 0x00000070)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_STATUS_RMSK                                                  0x1
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_STATUS_ADDR, HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_STATUS_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_STATUS_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_STATUS_MREG_TIMEOUT_BMSK                                     0x1
#define HWIO_BIMC_S_DDR0_SHKE_MREG_RDATA_STATUS_MREG_TIMEOUT_SHFT                                     0x0

#define HWIO_BIMC_S_DDR0_SHKE_DRAM_STATUS_ADDR                                                 (BIMC_S_DDR0_SHKE_REG_BASE      + 0x00000074)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_STATUS_RMSK                                                        0x7
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_DRAM_STATUS_ADDR, HWIO_BIMC_S_DDR0_SHKE_DRAM_STATUS_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_DRAM_STATUS_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_STATUS_WDOG_SELF_RFSH_BMSK                                         0x4
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_STATUS_WDOG_SELF_RFSH_SHFT                                         0x2
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_STATUS_SW_SELF_RFSH_BMSK                                           0x2
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_STATUS_SW_SELF_RFSH_SHFT                                           0x1
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_STATUS_STALL_BMSK                                                  0x1
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_STATUS_STALL_SHFT                                                  0x0

#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_0_ADDR                                            (BIMC_S_DDR0_SHKE_REG_BASE      + 0x00000080)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_0_RMSK                                                0xffff
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_0_ADDR, HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_0_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_0_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_0_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_0_ADDR,v)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_0_ADDR,m,v,HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_0_IN)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_0_DDR_CA_EVEN_BMSK                                    0xffff
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_0_DDR_CA_EVEN_SHFT                                       0x0

#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_1_ADDR                                            (BIMC_S_DDR0_SHKE_REG_BASE      + 0x00000084)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_1_RMSK                                                0xffff
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_1_ADDR, HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_1_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_1_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_1_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_1_ADDR,v)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_1_ADDR,m,v,HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_1_IN)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_1_DDR_CA_ODD_BMSK                                     0xffff
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_1_DDR_CA_ODD_SHFT                                        0x0

#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_ADDR                                            (BIMC_S_DDR0_SHKE_REG_BASE      + 0x00000088)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_RMSK                                            0x3ff00000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_ADDR, HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_ADDR,v)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_ADDR,m,v,HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_IN)
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_DDR_CS_N_BMSK                                   0x30000000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_DDR_CS_N_SHFT                                         0x1c
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_DDR_RAS_N_BMSK                                   0x8000000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_DDR_RAS_N_SHFT                                        0x1b
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_DDR_CAS_N_BMSK                                   0x4000000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_DDR_CAS_N_SHFT                                        0x1a
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_DDR_WE_N_BMSK                                    0x2000000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_DDR_WE_N_SHFT                                         0x19
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_DDR_CKE_BMSK                                     0x1000000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_DDR_CKE_SHFT                                          0x18
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_DDR_BA_BMSK                                       0xf00000
#define HWIO_BIMC_S_DDR0_SHKE_DRAM_DEBUG_CMD_2_DDR_BA_SHFT                                           0x14

#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_ADDR                                                (BIMC_S_DDR0_SHKE_REG_BASE      + 0x000000a0)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RMSK                                                 0xff3ffff
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_ADDR, HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_ADDR,v)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_ADDR,m,v,HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_IN)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_MRR_ADDR_BMSK                                        0xff00000
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_MRR_ADDR_SHFT                                             0x14
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_MRR_RANK_SEL_BMSK                                      0x30000
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_MRR_RANK_SEL_SHFT                                         0x10
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_MRR_INTERVAL_BMSK                                       0xffff
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_MRR_INTERVAL_SHFT                                          0x0

#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK1_U_ADDR                                  (BIMC_S_DDR0_SHKE_REG_BASE      + 0x000000b0)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK1_U_RMSK                                  0xffffffff
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK1_U_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK1_U_ADDR, HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK1_U_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK1_U_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK1_U_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK1_U_MREG_DATA_BMSK                        0xffffffff
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK1_U_MREG_DATA_SHFT                               0x0

#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK1_L_ADDR                                  (BIMC_S_DDR0_SHKE_REG_BASE      + 0x000000b4)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK1_L_RMSK                                  0xffffffff
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK1_L_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK1_L_ADDR, HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK1_L_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK1_L_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK1_L_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK1_L_MREG_DATA_BMSK                        0xffffffff
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK1_L_MREG_DATA_SHFT                               0x0

#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK0_U_ADDR                                  (BIMC_S_DDR0_SHKE_REG_BASE      + 0x000000b8)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK0_U_RMSK                                  0xffffffff
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK0_U_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK0_U_ADDR, HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK0_U_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK0_U_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK0_U_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK0_U_MREG_DATA_BMSK                        0xffffffff
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK0_U_MREG_DATA_SHFT                               0x0

#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK0_L_ADDR                                  (BIMC_S_DDR0_SHKE_REG_BASE      + 0x000000bc)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK0_L_RMSK                                  0xffffffff
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK0_L_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK0_L_ADDR, HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK0_L_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK0_L_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK0_L_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK0_L_MREG_DATA_BMSK                        0xffffffff
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA_RANK0_L_MREG_DATA_SHFT                               0x0

#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_ZQCAL_ADDR                                              (BIMC_S_DDR0_SHKE_REG_BASE      + 0x000000d0)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_ZQCAL_RMSK                                              0x3000ffff
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_ZQCAL_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_PERIODIC_ZQCAL_ADDR, HWIO_BIMC_S_DDR0_SHKE_PERIODIC_ZQCAL_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_ZQCAL_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_PERIODIC_ZQCAL_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_ZQCAL_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SHKE_PERIODIC_ZQCAL_ADDR,v)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_ZQCAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SHKE_PERIODIC_ZQCAL_ADDR,m,v,HWIO_BIMC_S_DDR0_SHKE_PERIODIC_ZQCAL_IN)
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_ZQCAL_RANK_SEL_BMSK                                     0x30000000
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_ZQCAL_RANK_SEL_SHFT                                           0x1c
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_ZQCAL_ZQCAL_INTERVAL_BMSK                                   0xffff
#define HWIO_BIMC_S_DDR0_SHKE_PERIODIC_ZQCAL_ZQCAL_INTERVAL_SHFT                                      0x0

#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_ADDR                                           (BIMC_S_DDR0_SHKE_REG_BASE      + 0x000000f0)
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_RMSK                                           0x33c103ff
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_ADDR, HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_ADDR,v)
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_ADDR,m,v,HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_IN)
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_AUTO_RFSH_ENABLE_RANK1_BMSK                    0x20000000
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_AUTO_RFSH_ENABLE_RANK1_SHFT                          0x1d
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_AUTO_RFSH_ENABLE_RANK0_BMSK                    0x10000000
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_AUTO_RFSH_ENABLE_RANK0_SHFT                          0x1c
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_AUTO_REFRESH_MODE_BMSK                          0x3000000
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_AUTO_REFRESH_MODE_SHFT                               0x18
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_AUTO_SWITCH_ALL_BANK_MODE_BMSK                   0xc00000
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_AUTO_SWITCH_ALL_BANK_MODE_SHFT                       0x16
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_DISABLE_AUTO_REFRESH_TEMP_UPDATE_BMSK             0x10000
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_DISABLE_AUTO_REFRESH_TEMP_UPDATE_SHFT                0x10
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_TREFI_BMSK                                          0x3ff
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_TREFI_SHFT                                            0x0

#define HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_ADDR                                           (BIMC_S_DDR0_SHKE_REG_BASE      + 0x000000f4)
#define HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_RMSK                                           0x30fff100
#define HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_ADDR, HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_ADDR,v)
#define HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_ADDR,m,v,HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_IN)
#define HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_HW_SELF_RFSH_ENABLE_RANK1_BMSK                 0x20000000
#define HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_HW_SELF_RFSH_ENABLE_RANK1_SHFT                       0x1d
#define HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_HW_SELF_RFSH_ENABLE_RANK0_BMSK                 0x10000000
#define HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_HW_SELF_RFSH_ENABLE_RANK0_SHFT                       0x1c
#define HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_HW_SELF_RFSH_IDLE_TIMER_BMSK                     0xfff000
#define HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_HW_SELF_RFSH_IDLE_TIMER_SHFT                          0xc
#define HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_CONCURRENT_SELF_RFSH_EN_BMSK                        0x100
#define HWIO_BIMC_S_DDR0_SHKE_SELF_REFRESH_CNTL_CONCURRENT_SELF_RFSH_EN_SHFT                          0x8

#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_1_ADDR                                         (BIMC_S_DDR0_SHKE_REG_BASE      + 0x00000100)
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_1_RMSK                                              0x3ff
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_1_ADDR, HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_1_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_1_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_1_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_1_ADDR,v)
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_1_ADDR,m,v,HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_1_IN)
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_1_TREFI_BMSK                                        0x3ff
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_1_TREFI_SHFT                                          0x0

#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_2_ADDR                                         (BIMC_S_DDR0_SHKE_REG_BASE      + 0x00000104)
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_2_RMSK                                         0xffff0001
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_2_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_2_ADDR, HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_2_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_2_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_2_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_2_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_2_ADDR,v)
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_2_ADDR,m,v,HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_2_IN)
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_2_RFSH_UPDATE_TIMER_BMSK                       0xffff0000
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_2_RFSH_UPDATE_TIMER_SHFT                             0x10
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_2_DLY_RFSH_CONDN_BMSK                                 0x1
#define HWIO_BIMC_S_DDR0_SHKE_AUTO_REFRESH_CNTL_2_DLY_RFSH_CONDN_SHFT                                 0x0

#define HWIO_BIMC_S_DDR0_SHKE_TESTBUS_CNTL_ADDR                                                (BIMC_S_DDR0_SHKE_REG_BASE      + 0x00000114)
#define HWIO_BIMC_S_DDR0_SHKE_TESTBUS_CNTL_RMSK                                                      0xf1
#define HWIO_BIMC_S_DDR0_SHKE_TESTBUS_CNTL_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_TESTBUS_CNTL_ADDR, HWIO_BIMC_S_DDR0_SHKE_TESTBUS_CNTL_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_TESTBUS_CNTL_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_TESTBUS_CNTL_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_TESTBUS_CNTL_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SHKE_TESTBUS_CNTL_ADDR,v)
#define HWIO_BIMC_S_DDR0_SHKE_TESTBUS_CNTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SHKE_TESTBUS_CNTL_ADDR,m,v,HWIO_BIMC_S_DDR0_SHKE_TESTBUS_CNTL_IN)
#define HWIO_BIMC_S_DDR0_SHKE_TESTBUS_CNTL_TESTBUS_SEL_BMSK                                          0xf0
#define HWIO_BIMC_S_DDR0_SHKE_TESTBUS_CNTL_TESTBUS_SEL_SHFT                                           0x4
#define HWIO_BIMC_S_DDR0_SHKE_TESTBUS_CNTL_TESTBUS_EN_BMSK                                            0x1
#define HWIO_BIMC_S_DDR0_SHKE_TESTBUS_CNTL_TESTBUS_EN_SHFT                                            0x0

#define HWIO_BIMC_S_DDR0_SHKE_SPARE_REG_ADDR                                                   (BIMC_S_DDR0_SHKE_REG_BASE      + 0x00000130)
#define HWIO_BIMC_S_DDR0_SHKE_SPARE_REG_RMSK                                                   0xffffffff
#define HWIO_BIMC_S_DDR0_SHKE_SPARE_REG_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_SPARE_REG_ADDR, HWIO_BIMC_S_DDR0_SHKE_SPARE_REG_RMSK)
#define HWIO_BIMC_S_DDR0_SHKE_SPARE_REG_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SHKE_SPARE_REG_ADDR, m)
#define HWIO_BIMC_S_DDR0_SHKE_SPARE_REG_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SHKE_SPARE_REG_ADDR,v)
#define HWIO_BIMC_S_DDR0_SHKE_SPARE_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SHKE_SPARE_REG_ADDR,m,v,HWIO_BIMC_S_DDR0_SHKE_SPARE_REG_IN)
#define HWIO_BIMC_S_DDR0_SHKE_SPARE_REG_SPARE_BITS_BMSK                                        0xffffffff
#define HWIO_BIMC_S_DDR0_SHKE_SPARE_REG_SPARE_BITS_SHFT                                               0x0

/*----------------------------------------------------------------------------
 * MODULE: DIM_C00_DDRPHY_CA
 *--------------------------------------------------------------------------*/

#define DIM_C00_DDRPHY_CA_REG_BASE                                                (EBI1_PHY_CFG_BASE      + 0x00000000)

#define HWIO_DIM_C00_DIM_CA_TOP_CFG_ADDR                                          (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000000)
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_RMSK                                           0x1f13131
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_TOP_CFG_ADDR, HWIO_DIM_C00_DIM_CA_TOP_CFG_RMSK)
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_TOP_CFG_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_TOP_CFG_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_TOP_CFG_ADDR,m,v,HWIO_DIM_C00_DIM_CA_TOP_CFG_IN)
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_CA_LB_MODE_SEL_BMSK                            0x1000000
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_CA_LB_MODE_SEL_SHFT                                 0x18
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_CA_LB_MODE_EN_BMSK                              0x800000
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_CA_LB_MODE_EN_SHFT                                  0x17
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_MISR_EN_OVRD_BMSK                               0x400000
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_MISR_EN_OVRD_SHFT                                   0x16
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_MISR_DATA_CLK_SEL_BMSK                          0x200000
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_MISR_DATA_CLK_SEL_SHFT                              0x15
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_IOCAL_CTLR_SEL_BMSK                             0x100000
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_IOCAL_CTLR_SEL_SHFT                                 0x14
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_SDR_MODE_EN_BMSK                                 0x10000
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_SDR_MODE_EN_SHFT                                    0x10
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_DEBUG_BUS_SEL_BMSK                                0x2000
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_DEBUG_BUS_SEL_SHFT                                   0xd
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_DEBUG_BUS_EN_BMSK                                 0x1000
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_DEBUG_BUS_EN_SHFT                                    0xc
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_CDC_TEST_EN_BMSK                                   0x100
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_CDC_TEST_EN_SHFT                                     0x8
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_WR_PIPE_EXTEND1_BMSK                                0x20
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_WR_PIPE_EXTEND1_SHFT                                 0x5
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_WR_PIPE_EXTEND0_BMSK                                0x10
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_WR_PIPE_EXTEND0_SHFT                                 0x4
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_WR_CLK_SEL_BMSK                                      0x1
#define HWIO_DIM_C00_DIM_CA_TOP_CFG_WR_CLK_SEL_SHFT                                      0x0

#define HWIO_DIM_C00_DIM_CA_HW_INFO_ADDR                                          (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000004)
#define HWIO_DIM_C00_DIM_CA_HW_INFO_RMSK                                             0xfffff
#define HWIO_DIM_C00_DIM_CA_HW_INFO_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_HW_INFO_ADDR, HWIO_DIM_C00_DIM_CA_HW_INFO_RMSK)
#define HWIO_DIM_C00_DIM_CA_HW_INFO_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_HW_INFO_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_HW_INFO_CORE_ID_BMSK                                     0xf0000
#define HWIO_DIM_C00_DIM_CA_HW_INFO_CORE_ID_SHFT                                        0x10
#define HWIO_DIM_C00_DIM_CA_HW_INFO_MAJOR_REV_BMSK                                    0xf000
#define HWIO_DIM_C00_DIM_CA_HW_INFO_MAJOR_REV_SHFT                                       0xc
#define HWIO_DIM_C00_DIM_CA_HW_INFO_BRANCH_REV_BMSK                                    0xf00
#define HWIO_DIM_C00_DIM_CA_HW_INFO_BRANCH_REV_SHFT                                      0x8
#define HWIO_DIM_C00_DIM_CA_HW_INFO_MINOR_REV_BMSK                                      0xff
#define HWIO_DIM_C00_DIM_CA_HW_INFO_MINOR_REV_SHFT                                       0x0

#define HWIO_DIM_C00_DIM_CA_HW_VERSION_ADDR                                       (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000008)
#define HWIO_DIM_C00_DIM_CA_HW_VERSION_RMSK                                       0xffffffff
#define HWIO_DIM_C00_DIM_CA_HW_VERSION_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_HW_VERSION_ADDR, HWIO_DIM_C00_DIM_CA_HW_VERSION_RMSK)
#define HWIO_DIM_C00_DIM_CA_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_HW_VERSION_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_HW_VERSION_MAJOR_BMSK                                 0xf0000000
#define HWIO_DIM_C00_DIM_CA_HW_VERSION_MAJOR_SHFT                                       0x1c
#define HWIO_DIM_C00_DIM_CA_HW_VERSION_MINOR_BMSK                                  0xfff0000
#define HWIO_DIM_C00_DIM_CA_HW_VERSION_MINOR_SHFT                                       0x10
#define HWIO_DIM_C00_DIM_CA_HW_VERSION_STEP_BMSK                                      0xffff
#define HWIO_DIM_C00_DIM_CA_HW_VERSION_STEP_SHFT                                         0x0

#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_ADDR                                         (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000010)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_RMSK                                         0xff3333f7
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_PAD_CFG0_ADDR, HWIO_DIM_C00_DIM_CA_PAD_CFG0_RMSK)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_PAD_CFG0_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_PAD_CFG0_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_PAD_CFG0_ADDR,m,v,HWIO_DIM_C00_DIM_CA_PAD_CFG0_IN)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_DDR_MODE1_BMSK                            0x80000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_DDR_MODE1_SHFT                                  0x1f
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_DDR_MODE0_BMSK                            0x40000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_DDR_MODE0_SHFT                                  0x1e
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_LV_MODE_BMSK                              0x20000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_LV_MODE_SHFT                                    0x1d
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_ODT_ENA_BMSK                              0x10000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_ODT_ENA_SHFT                                    0x1c
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_ODT_BMSK                                   0xc000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_ODT_SHFT                                        0x1a
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_PULL_BMSK                                  0x3000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_PULL_SHFT                                       0x18
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_NSLEW_BMSK                                  0x300000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_NSLEW_SHFT                                      0x14
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_PSLEW_BMSK                                   0x30000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_PSLEW_SHFT                                      0x10
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_NRXDEL_BMSK                                   0x3000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_NRXDEL_SHFT                                      0xc
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_PRXDEL_BMSK                                    0x300
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_PRXDEL_SHFT                                      0x8
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_VM_SHIFT_ENA_BMSK                               0x80
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_VM_SHIFT_ENA_SHFT                                0x7
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_ROUT_BMSK                                       0x70
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_ROUT_SHFT                                        0x4
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_DCC_BMSK                                         0x7
#define HWIO_DIM_C00_DIM_CA_PAD_CFG0_CA_DCC_SHFT                                         0x0

#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_ADDR                                         (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000014)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_RMSK                                         0xff3337f7
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_PAD_CFG1_ADDR, HWIO_DIM_C00_DIM_CA_PAD_CFG1_RMSK)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_PAD_CFG1_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_PAD_CFG1_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_PAD_CFG1_ADDR,m,v,HWIO_DIM_C00_DIM_CA_PAD_CFG1_IN)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_DDR_MODE1_BMSK                            0x80000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_DDR_MODE1_SHFT                                  0x1f
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_DDR_MODE0_BMSK                            0x40000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_DDR_MODE0_SHFT                                  0x1e
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_LV_MODE_BMSK                              0x20000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_LV_MODE_SHFT                                    0x1d
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_CMFB_ENA_BMSK                             0x10000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_CMFB_ENA_SHFT                                   0x1c
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_ODT_ENA1_BMSK                              0x8000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_ODT_ENA1_SHFT                                   0x1b
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_ODT_ENA_BMSK                               0x4000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_ODT_ENA_SHFT                                    0x1a
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_ODT_BMSK                                   0x3000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_ODT_SHFT                                        0x18
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_NSLEW_BMSK                                  0x300000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_NSLEW_SHFT                                      0x14
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_PSLEW_BMSK                                   0x30000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_PSLEW_SHFT                                      0x10
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_CUR_MODE1_BMSK                                0x2000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_CUR_MODE1_SHFT                                   0xd
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_CUR_MODE0_BMSK                                0x1000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_CUR_MODE0_SHFT                                   0xc
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_I_DRV_BMSK                                     0x700
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_I_DRV_SHFT                                       0x8
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_VM_SHIFT_ENA_BMSK                               0x80
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_VM_SHIFT_ENA_SHFT                                0x7
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_ROUT_BMSK                                       0x70
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_ROUT_SHFT                                        0x4
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_DCC_BMSK                                         0x7
#define HWIO_DIM_C00_DIM_CA_PAD_CFG1_CK_DCC_SHFT                                         0x0

#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_ADDR                                         (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000018)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_RMSK                                         0x3f3f3f3f
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_PAD_CFG2_ADDR, HWIO_DIM_C00_DIM_CA_PAD_CFG2_RMSK)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_PAD_CFG2_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_PAD_CFG2_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_PAD_CFG2_ADDR,m,v,HWIO_DIM_C00_DIM_CA_PAD_CFG2_IN)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_VREF_LDO_ENA_BMSK                            0x20000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_VREF_LDO_ENA_SHFT                                  0x1d
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_VREF_PASSGATE_ENA_BMSK                       0x10000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_VREF_PASSGATE_ENA_SHFT                             0x1c
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_VREF_SP_OUT_BMSK                              0xf000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_VREF_SP_OUT_SHFT                                   0x18
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_VREF_SP_IN_BMSK                                0x3c0000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_VREF_SP_IN_SHFT                                    0x12
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_VREF_BYPASS_ENA_BMSK                            0x20000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_VREF_BYPASS_ENA_SHFT                               0x11
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_VREF_RDIV_ENA_BMSK                              0x10000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_VREF_RDIV_ENA_SHFT                                 0x10
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_VREF_PULLDN_SPEED_CNTL_BMSK                      0x3f00
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_VREF_PULLDN_SPEED_CNTL_SHFT                         0x8
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_VREF_LEVEL_CNTL_BMSK                               0x3f
#define HWIO_DIM_C00_DIM_CA_PAD_CFG2_VREF_LEVEL_CNTL_SHFT                                0x0

#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_ADDR                                         (DIM_C00_DDRPHY_CA_REG_BASE      + 0x0000001c)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_RMSK                                         0xf3ffffff
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_PAD_CFG3_ADDR, HWIO_DIM_C00_DIM_CA_PAD_CFG3_RMSK)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_PAD_CFG3_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_PAD_CFG3_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_PAD_CFG3_ADDR,m,v,HWIO_DIM_C00_DIM_CA_PAD_CFG3_IN)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_CS_N_IE_BMSK                                 0xc0000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_CS_N_IE_SHFT                                       0x1e
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_CS_N_OE_BMSK                                 0x30000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_CS_N_OE_SHFT                                       0x1c
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_CK_IE_BMSK                                    0x2000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_CK_IE_SHFT                                         0x19
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_CK_OE_BMSK                                    0x1000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_CK_OE_SHFT                                         0x18
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_CKE_IE_BMSK                                    0xc00000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_CKE_IE_SHFT                                        0x16
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_CKE_OE_BMSK                                    0x300000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_CKE_OE_SHFT                                        0x14
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_CA_IE_BMSK                                      0xffc00
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_CA_IE_SHFT                                          0xa
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_CA_OE_BMSK                                        0x3ff
#define HWIO_DIM_C00_DIM_CA_PAD_CFG3_CA_OE_SHFT                                          0x0

#define HWIO_DIM_C00_DIM_CA_PAD_CFG4_ADDR                                         (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000020)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG4_RMSK                                         0xf0ffffff
#define HWIO_DIM_C00_DIM_CA_PAD_CFG4_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_PAD_CFG4_ADDR, HWIO_DIM_C00_DIM_CA_PAD_CFG4_RMSK)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG4_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_PAD_CFG4_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG4_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_PAD_CFG4_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_PAD_CFG4_ADDR,m,v,HWIO_DIM_C00_DIM_CA_PAD_CFG4_IN)
#define HWIO_DIM_C00_DIM_CA_PAD_CFG4_CS_N_OE_DYN_ENA_BMSK                         0xc0000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG4_CS_N_OE_DYN_ENA_SHFT                               0x1e
#define HWIO_DIM_C00_DIM_CA_PAD_CFG4_CS_N_OE_DYN_BMSK                             0x30000000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG4_CS_N_OE_DYN_SHFT                                   0x1c
#define HWIO_DIM_C00_DIM_CA_PAD_CFG4_CKE_OE_DYN_ENA_BMSK                            0xc00000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG4_CKE_OE_DYN_ENA_SHFT                                0x16
#define HWIO_DIM_C00_DIM_CA_PAD_CFG4_CKE_OE_DYN_BMSK                                0x300000
#define HWIO_DIM_C00_DIM_CA_PAD_CFG4_CKE_OE_DYN_SHFT                                    0x14
#define HWIO_DIM_C00_DIM_CA_PAD_CFG4_CA_OE_DYN_ENA_BMSK                              0xffc00
#define HWIO_DIM_C00_DIM_CA_PAD_CFG4_CA_OE_DYN_ENA_SHFT                                  0xa
#define HWIO_DIM_C00_DIM_CA_PAD_CFG4_CA_OE_DYN_BMSK                                    0x3ff
#define HWIO_DIM_C00_DIM_CA_PAD_CFG4_CA_OE_DYN_SHFT                                      0x0

#define HWIO_DIM_C00_DDRPHY_CA_MISR_CFG_ADDR                                      (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000024)
#define HWIO_DIM_C00_DDRPHY_CA_MISR_CFG_RMSK                                      0xffffffff
#define HWIO_DIM_C00_DDRPHY_CA_MISR_CFG_IN          \
        in_dword_masked(HWIO_DIM_C00_DDRPHY_CA_MISR_CFG_ADDR, HWIO_DIM_C00_DDRPHY_CA_MISR_CFG_RMSK)
#define HWIO_DIM_C00_DDRPHY_CA_MISR_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DDRPHY_CA_MISR_CFG_ADDR, m)
#define HWIO_DIM_C00_DDRPHY_CA_MISR_CFG_OUT(v)      \
        out_dword(HWIO_DIM_C00_DDRPHY_CA_MISR_CFG_ADDR,v)
#define HWIO_DIM_C00_DDRPHY_CA_MISR_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DDRPHY_CA_MISR_CFG_ADDR,m,v,HWIO_DIM_C00_DDRPHY_CA_MISR_CFG_IN)
#define HWIO_DIM_C00_DDRPHY_CA_MISR_CFG_SEED_VALUE_BMSK                           0xffffffff
#define HWIO_DIM_C00_DDRPHY_CA_MISR_CFG_SEED_VALUE_SHFT                                  0x0

#define HWIO_DIM_C00_DDRPHY_CA_MISR_EVEN_STATUS_ADDR                              (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000028)
#define HWIO_DIM_C00_DDRPHY_CA_MISR_EVEN_STATUS_RMSK                              0xffffffff
#define HWIO_DIM_C00_DDRPHY_CA_MISR_EVEN_STATUS_IN          \
        in_dword_masked(HWIO_DIM_C00_DDRPHY_CA_MISR_EVEN_STATUS_ADDR, HWIO_DIM_C00_DDRPHY_CA_MISR_EVEN_STATUS_RMSK)
#define HWIO_DIM_C00_DDRPHY_CA_MISR_EVEN_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DDRPHY_CA_MISR_EVEN_STATUS_ADDR, m)
#define HWIO_DIM_C00_DDRPHY_CA_MISR_EVEN_STATUS_SIGNATURE_BMSK                    0xffffffff
#define HWIO_DIM_C00_DDRPHY_CA_MISR_EVEN_STATUS_SIGNATURE_SHFT                           0x0

#define HWIO_DIM_C00_DDRPHY_CA_MISR_ODD_STATUS_ADDR                               (DIM_C00_DDRPHY_CA_REG_BASE      + 0x0000002c)
#define HWIO_DIM_C00_DDRPHY_CA_MISR_ODD_STATUS_RMSK                               0xffffffff
#define HWIO_DIM_C00_DDRPHY_CA_MISR_ODD_STATUS_IN          \
        in_dword_masked(HWIO_DIM_C00_DDRPHY_CA_MISR_ODD_STATUS_ADDR, HWIO_DIM_C00_DDRPHY_CA_MISR_ODD_STATUS_RMSK)
#define HWIO_DIM_C00_DDRPHY_CA_MISR_ODD_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DDRPHY_CA_MISR_ODD_STATUS_ADDR, m)
#define HWIO_DIM_C00_DDRPHY_CA_MISR_ODD_STATUS_SIGNATURE_BMSK                     0xffffffff
#define HWIO_DIM_C00_DDRPHY_CA_MISR_ODD_STATUS_SIGNATURE_SHFT                            0x0

#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_ADDR                                    (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000030)
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_RMSK                                     0x3ff37ff
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_ADDR, HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_RMSK)
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_ADDR,m,v,HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_IN)
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_ERROR_CODE_UPDATE_EN_BMSK                0x2000000
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_ERROR_CODE_UPDATE_EN_SHFT                     0x19
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_TRACKING_CAL_ENA_BMSK                    0x1000000
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_TRACKING_CAL_ENA_SHFT                         0x18
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_OSC_COUNT_ERR_TOLERANCE_BMSK              0xf00000
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_OSC_COUNT_ERR_TOLERANCE_SHFT                  0x14
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_TRACK_CALIB_MODE_BMSK                      0x80000
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_TRACK_CALIB_MODE_SHFT                         0x13
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_FULL_DELAY_BMSK                            0x40000
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_FULL_DELAY_SHFT                               0x12
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_HW_AUTOCAL_ENA_BMSK                        0x20000
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_HW_AUTOCAL_ENA_SHFT                           0x11
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_SW_TRIG_FULL_CALIB_BMSK                    0x10000
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_SW_TRIG_FULL_CALIB_SHFT                       0x10
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_OSC_PRE_DIV_BMSK                            0x3000
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_OSC_PRE_DIV_SHFT                               0xc
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_TMUX_CHAR_BMSK                               0x7ff
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG0_TMUX_CHAR_SHFT                                 0x0

#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_ADDR                                    (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000034)
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_RMSK                                     0x7f77777
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_ADDR, HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_RMSK)
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_ADDR,m,v,HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_IN)
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_OSC_COUNT_DELAY_BMSK                     0x7000000
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_OSC_COUNT_DELAY_SHFT                          0x18
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_STANDBY_DELAY_BMSK                        0xf00000
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_STANDBY_DELAY_SHFT                            0x14
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_DEL_MODE_DELAY_BMSK                        0x70000
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_DEL_MODE_DELAY_SHFT                           0x10
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_OSC_MODE_DELAY_BMSK                         0x7000
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_OSC_MODE_DELAY_SHFT                            0xc
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_DECODER_DELAY_BMSK                           0x700
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_DECODER_DELAY_SHFT                             0x8
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_DIVIDER_DELAY_BMSK                            0x70
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_DIVIDER_DELAY_SHFT                             0x4
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_MULTIPLIER_DELAY_BMSK                          0x7
#define HWIO_DIM_C00_DIM_CA_CDC_CTLR_CFG1_MULTIPLIER_DELAY_SHFT                          0x0

#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_ADDR                               (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000038)
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_RMSK                                0x1f1ffff
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_ADDR, HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_RMSK)
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_ADDR,m,v,HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_IN)
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_INVALID_TIMER_ENA_BMSK              0x1000000
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_INVALID_TIMER_ENA_SHFT                   0x18
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_INVALID_TIMER_VAL_BMSK               0xf00000
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_INVALID_TIMER_VAL_SHFT                   0x14
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_TIMER_ENA_BMSK                        0x10000
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_TIMER_ENA_SHFT                           0x10
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_TIMER_VAL_BMSK                         0xffff
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG0_TIMER_VAL_SHFT                            0x0

#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG1_ADDR                               (DIM_C00_DDRPHY_CA_REG_BASE      + 0x0000003c)
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG1_RMSK                                   0x13ff
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG1_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG1_ADDR, HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG1_RMSK)
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG1_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG1_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG1_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG1_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG1_ADDR,m,v,HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG1_IN)
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG1_FF_TIMER_ENA_BMSK                      0x1000
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG1_FF_TIMER_ENA_SHFT                         0xc
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG1_FF_TIMER_VAL_BMSK                       0x3ff
#define HWIO_DIM_C00_DIM_CA_CDC_CAL_TIMER_CFG1_FF_TIMER_VAL_SHFT                         0x0

#define HWIO_DIM_C00_DIM_CA_CDC_REFCOUNT_CFG_ADDR                                 (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000040)
#define HWIO_DIM_C00_DIM_CA_CDC_REFCOUNT_CFG_RMSK                                 0xffff3f3f
#define HWIO_DIM_C00_DIM_CA_CDC_REFCOUNT_CFG_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_REFCOUNT_CFG_ADDR, HWIO_DIM_C00_DIM_CA_CDC_REFCOUNT_CFG_RMSK)
#define HWIO_DIM_C00_DIM_CA_CDC_REFCOUNT_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_REFCOUNT_CFG_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CDC_REFCOUNT_CFG_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_CDC_REFCOUNT_CFG_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_CDC_REFCOUNT_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_CDC_REFCOUNT_CFG_ADDR,m,v,HWIO_DIM_C00_DIM_CA_CDC_REFCOUNT_CFG_IN)
#define HWIO_DIM_C00_DIM_CA_CDC_REFCOUNT_CFG_TREF_BMSK                            0xffff0000
#define HWIO_DIM_C00_DIM_CA_CDC_REFCOUNT_CFG_TREF_SHFT                                  0x10
#define HWIO_DIM_C00_DIM_CA_CDC_REFCOUNT_CFG_CCAL_REF_COUNT_BMSK                      0x3f00
#define HWIO_DIM_C00_DIM_CA_CDC_REFCOUNT_CFG_CCAL_REF_COUNT_SHFT                         0x8
#define HWIO_DIM_C00_DIM_CA_CDC_REFCOUNT_CFG_FCAL_REF_COUNT_BMSK                        0x3f
#define HWIO_DIM_C00_DIM_CA_CDC_REFCOUNT_CFG_FCAL_REF_COUNT_SHFT                         0x0

#define HWIO_DIM_C00_DIM_CA_CDC_COARSE_CAL_CFG_ADDR                               (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000044)
#define HWIO_DIM_C00_DIM_CA_CDC_COARSE_CAL_CFG_RMSK                                   0x1f3f
#define HWIO_DIM_C00_DIM_CA_CDC_COARSE_CAL_CFG_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_COARSE_CAL_CFG_ADDR, HWIO_DIM_C00_DIM_CA_CDC_COARSE_CAL_CFG_RMSK)
#define HWIO_DIM_C00_DIM_CA_CDC_COARSE_CAL_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_COARSE_CAL_CFG_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CDC_COARSE_CAL_CFG_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_CDC_COARSE_CAL_CFG_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_CDC_COARSE_CAL_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_CDC_COARSE_CAL_CFG_ADDR,m,v,HWIO_DIM_C00_DIM_CA_CDC_COARSE_CAL_CFG_IN)
#define HWIO_DIM_C00_DIM_CA_CDC_COARSE_CAL_CFG_CCAL_SUBUNIT_CAPS_BMSK                 0x1f00
#define HWIO_DIM_C00_DIM_CA_CDC_COARSE_CAL_CFG_CCAL_SUBUNIT_CAPS_SHFT                    0x8
#define HWIO_DIM_C00_DIM_CA_CDC_COARSE_CAL_CFG_CCAL_UNITSTEPS_BMSK                      0x3f
#define HWIO_DIM_C00_DIM_CA_CDC_COARSE_CAL_CFG_CCAL_UNITSTEPS_SHFT                       0x0

#define HWIO_DIM_C00_DIM_CA_CDC_RSVD_CFG_ADDR                                     (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000048)
#define HWIO_DIM_C00_DIM_CA_CDC_RSVD_CFG_RMSK                                         0xffff
#define HWIO_DIM_C00_DIM_CA_CDC_RSVD_CFG_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_RSVD_CFG_ADDR, HWIO_DIM_C00_DIM_CA_CDC_RSVD_CFG_RMSK)
#define HWIO_DIM_C00_DIM_CA_CDC_RSVD_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_RSVD_CFG_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CDC_RSVD_CFG_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_CDC_RSVD_CFG_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_CDC_RSVD_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_CDC_RSVD_CFG_ADDR,m,v,HWIO_DIM_C00_DIM_CA_CDC_RSVD_CFG_IN)
#define HWIO_DIM_C00_DIM_CA_CDC_RSVD_CFG_TEMP_FIELD_BMSK                              0xffff
#define HWIO_DIM_C00_DIM_CA_CDC_RSVD_CFG_TEMP_FIELD_SHFT                                 0x0

#define HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_ADDR                                   (DIM_C00_DDRPHY_CA_REG_BASE      + 0x0000004c)
#define HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_RMSK                                     0x19f1bf
#define HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_ADDR, HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_RMSK)
#define HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_ADDR,m,v,HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_IN)
#define HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_SUBUNIT_OFFSET_MODE_BMSK                 0x100000
#define HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_SUBUNIT_OFFSET_MODE_SHFT                     0x14
#define HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SIGN_BMSK                  0x80000
#define HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SIGN_SHFT                     0x13
#define HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_SUBUNIT_OFFSET_BMSK                       0x1f000
#define HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SHFT                           0xc
#define HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_UNIT_OFFSET_MODE_BMSK                       0x100
#define HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_UNIT_OFFSET_MODE_SHFT                         0x8
#define HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_UNIT_OFFSET_SIGN_BMSK                        0x80
#define HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_UNIT_OFFSET_SIGN_SHFT                         0x7
#define HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_UNIT_OFFSET_BMSK                             0x3f
#define HWIO_DIM_C00_DIM_CA_CDC_OFFSET_CFG_UNIT_OFFSET_SHFT                              0x0

#define HWIO_DIM_C00_DIM_CA_CDC_DELAY_CFG_ADDR                                    (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000050)
#define HWIO_DIM_C00_DIM_CA_CDC_DELAY_CFG_RMSK                                    0x1fff0fff
#define HWIO_DIM_C00_DIM_CA_CDC_DELAY_CFG_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_DELAY_CFG_ADDR, HWIO_DIM_C00_DIM_CA_CDC_DELAY_CFG_RMSK)
#define HWIO_DIM_C00_DIM_CA_CDC_DELAY_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_DELAY_CFG_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CDC_DELAY_CFG_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_CDC_DELAY_CFG_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_CDC_DELAY_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_CDC_DELAY_CFG_ADDR,m,v,HWIO_DIM_C00_DIM_CA_CDC_DELAY_CFG_IN)
#define HWIO_DIM_C00_DIM_CA_CDC_DELAY_CFG_TARGET_COUNT_ENA_BMSK                   0x10000000
#define HWIO_DIM_C00_DIM_CA_CDC_DELAY_CFG_TARGET_COUNT_ENA_SHFT                         0x1c
#define HWIO_DIM_C00_DIM_CA_CDC_DELAY_CFG_TARGET_COUNT_BMSK                        0xfff0000
#define HWIO_DIM_C00_DIM_CA_CDC_DELAY_CFG_TARGET_COUNT_SHFT                             0x10
#define HWIO_DIM_C00_DIM_CA_CDC_DELAY_CFG_DELAY_VAL_BMSK                               0xfff
#define HWIO_DIM_C00_DIM_CA_CDC_DELAY_CFG_DELAY_VAL_SHFT                                 0x0

#define HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_ADDR                                  (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000054)
#define HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_RMSK                                         0xf
#define HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_ADDR, HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_RMSK)
#define HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_ADDR,m,v,HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_IN)
#define HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_SW_DEL_MODE_BMSK                             0x8
#define HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_SW_DEL_MODE_SHFT                             0x3
#define HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_SW_OSC_MODE_BMSK                             0x4
#define HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_SW_OSC_MODE_SHFT                             0x2
#define HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_SW_DA_SEL_BMSK                               0x2
#define HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_SW_DA_SEL_SHFT                               0x1
#define HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_SW_LOAD_BMSK                                 0x1
#define HWIO_DIM_C00_DIM_CA_CDC_SW_MODE_CFG_SW_LOAD_SHFT                                 0x0

#define HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_ADDR                                     (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000058)
#define HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_RMSK                                            0xf
#define HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_ADDR, HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_RMSK)
#define HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_ADDR,m,v,HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_IN)
#define HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_CDC_SEL_DDA_TEST_BMSK                           0x8
#define HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_CDC_SEL_DDA_TEST_SHFT                           0x3
#define HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_CDC_OSC_TEST_EN_BMSK                            0x4
#define HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_CDC_OSC_TEST_EN_SHFT                            0x2
#define HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_CDC_TEST_EN_BMSK                                0x2
#define HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_CDC_TEST_EN_SHFT                                0x1
#define HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_CDC_OUT_ONTEST2_BMSK                            0x1
#define HWIO_DIM_C00_DIM_CA_CDC_TEST_CFG_CDC_OUT_ONTEST2_SHFT                            0x0

#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_ADDR                                  (DIM_C00_DDRPHY_CA_REG_BASE      + 0x0000005c)
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_RMSK                                     0x10f0f
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_ADDR, HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_RMSK)
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_ADDR,m,v,HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_IN)
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_SW_REF_GATE_BMSK                         0x10000
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_SW_REF_GATE_SHFT                            0x10
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_SW_OVRD_DA1_OSC_EN_BMSK                    0x800
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_SW_OVRD_DA1_OSC_EN_SHFT                      0xb
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_SW_OVRD_DA1_IN_EN_BMSK                     0x400
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_SW_OVRD_DA1_IN_EN_SHFT                       0xa
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_SW_OVRD_DA0_OSC_EN_BMSK                    0x200
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_SW_OVRD_DA0_OSC_EN_SHFT                      0x9
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_SW_OVRD_DA0_IN_EN_BMSK                     0x100
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_SW_OVRD_DA0_IN_EN_SHFT                       0x8
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_SW_OVRD_CDC_COUNTER_RST_BMSK                 0x8
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_SW_OVRD_CDC_COUNTER_RST_SHFT                 0x3
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_SW_OVRD_LOAD_DA_SEL_BMSK                     0x4
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_SW_OVRD_LOAD_DA_SEL_SHFT                     0x2
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_SW_OVRD_ACTV_DA_SEL_BMSK                     0x2
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_SW_OVRD_ACTV_DA_SEL_SHFT                     0x1
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_SW_OVRD_ENA_BMSK                             0x1
#define HWIO_DIM_C00_DIM_CA_CDC_SW_OVRD_CFG_SW_OVRD_ENA_SHFT                             0x0

#define HWIO_DIM_C00_DIM_CA_CDC_STATUS0_ADDR                                      (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000070)
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS0_RMSK                                       0x7ffffff
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS0_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_STATUS0_ADDR, HWIO_DIM_C00_DIM_CA_CDC_STATUS0_RMSK)
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS0_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_STATUS0_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS0_CDC_ERROR_CODE_BMSK                        0x7000000
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS0_CDC_ERROR_CODE_SHFT                             0x18
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS0_SW_REF_GATE_COUNT_BMSK                      0xff0000
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS0_SW_REF_GATE_COUNT_SHFT                          0x10
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS0_OSC_COUNT_BMSK                                0xfff0
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS0_OSC_COUNT_SHFT                                   0x4
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS0_CURR_SEL_DA_BMSK                                 0x8
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS0_CURR_SEL_DA_SHFT                                 0x3
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS0_CTLR_SM_IDLE_BMSK                                0x4
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS0_CTLR_SM_IDLE_SHFT                                0x2
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS0_OSC_DONE_BMSK                                    0x2
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS0_OSC_DONE_SHFT                                    0x1
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS0_CALIBRATION_DONE_BMSK                            0x1
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS0_CALIBRATION_DONE_SHFT                            0x0

#define HWIO_DIM_C00_DIM_CA_CDC_STATUS1_ADDR                                      (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000074)
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS1_RMSK                                       0xfff0fff
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS1_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_STATUS1_ADDR, HWIO_DIM_C00_DIM_CA_CDC_STATUS1_RMSK)
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS1_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_STATUS1_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS1_CURR_DELAY_VALUE_BMSK                      0xfff0000
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS1_CURR_DELAY_VALUE_SHFT                           0x10
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS1_CURR_TMUX_DELAY_BMSK                           0xfff
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS1_CURR_TMUX_DELAY_SHFT                             0x0

#define HWIO_DIM_C00_DIM_CA_CDC_STATUS2_ADDR                                      (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000078)
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS2_RMSK                                      0x1f3f1f3f
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS2_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_STATUS2_ADDR, HWIO_DIM_C00_DIM_CA_CDC_STATUS2_RMSK)
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS2_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_STATUS2_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS2_DA1_SUBUNITS_BMSK                         0x1f000000
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS2_DA1_SUBUNITS_SHFT                               0x18
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS2_DA1_UNITSTEPS_BMSK                          0x3f0000
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS2_DA1_UNITSTEPS_SHFT                              0x10
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS2_DA0_SUBUNITS_BMSK                             0x1f00
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS2_DA0_SUBUNITS_SHFT                                0x8
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS2_DA0_UNITSTEPS_BMSK                              0x3f
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS2_DA0_UNITSTEPS_SHFT                               0x0

#define HWIO_DIM_C00_DIM_CA_CDC_STATUS3_ADDR                                      (DIM_C00_DDRPHY_CA_REG_BASE      + 0x0000007c)
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS3_RMSK                                      0xffffffff
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS3_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_STATUS3_ADDR, HWIO_DIM_C00_DIM_CA_CDC_STATUS3_RMSK)
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS3_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CDC_STATUS3_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS3_NUM_OF_OSC_ITER_BMSK                      0xff000000
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS3_NUM_OF_OSC_ITER_SHFT                            0x18
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS3_COUNT_ERROR_BMSK                            0xfff000
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS3_COUNT_ERROR_SHFT                                 0xc
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS3_CURR_TARGET_COUNT_BMSK                         0xfff
#define HWIO_DIM_C00_DIM_CA_CDC_STATUS3_CURR_TARGET_COUNT_SHFT                           0x0

#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_ADDR                                     (DIM_C00_DDRPHY_CA_REG_BASE      + 0x000000e0)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_RMSK                                     0xf31f377f
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_ADDR, HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_RMSK)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_ADDR,m,v,HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_IN)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_CAL_NOW_BMSK                             0x80000000
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_CAL_NOW_SHFT                                   0x1f
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_IO_CAL_AUTO_BMSK                         0x40000000
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_IO_CAL_AUTO_SHFT                               0x1e
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_IO_CAL_FF_TIMER_EN_BMSK                  0x20000000
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_IO_CAL_FF_TIMER_EN_SHFT                        0x1d
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_IO_CAL_BANDGAP_DYN_CTRL_BMSK             0x10000000
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_IO_CAL_BANDGAP_DYN_CTRL_SHFT                   0x1c
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_SW_FFCLK_ON_BMSK                          0x2000000
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_SW_FFCLK_ON_SHFT                               0x19
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_LV_MODE_BMSK                              0x1000000
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_LV_MODE_SHFT                                   0x18
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_MARGIN_LOAD_BMSK                           0x1f0000
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_MARGIN_LOAD_SHFT                               0x10
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_IMP_SEL_BMSK                                 0x3000
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_IMP_SEL_SHFT                                    0xc
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_PN_SEL_CA_BMSK                                0x400
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_PN_SEL_CA_SHFT                                  0xa
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_PN_SEL_DATA_BMSK                              0x200
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_PN_SEL_DATA_SHFT                                0x9
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_CAL_USE_LAST_BMSK                             0x100
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_CAL_USE_LAST_SHFT                               0x8
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_SAMPLE_POINT_BMSK                              0x70
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_SAMPLE_POINT_SHFT                               0x4
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_DDR_MODE1_BMSK                                  0x8
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_DDR_MODE1_SHFT                                  0x3
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_DDR_MODE0_BMSK                                  0x4
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_DDR_MODE0_SHFT                                  0x2
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_BANDGAP_ENA1_BMSK                               0x2
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_BANDGAP_ENA1_SHFT                               0x1
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_BANDGAP_ENA0_BMSK                               0x1
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CFG_BANDGAP_ENA0_SHFT                               0x0

#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_PNCNT_CFG_ADDR                               (DIM_C00_DDRPHY_CA_REG_BASE      + 0x000000e4)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_PNCNT_CFG_RMSK                                   0x1f1f
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_PNCNT_CFG_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_IOC_CTLR_PNCNT_CFG_ADDR, HWIO_DIM_C00_DIM_CA_IOC_CTLR_PNCNT_CFG_RMSK)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_PNCNT_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_IOC_CTLR_PNCNT_CFG_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_PNCNT_CFG_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_IOC_CTLR_PNCNT_CFG_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_PNCNT_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_IOC_CTLR_PNCNT_CFG_ADDR,m,v,HWIO_DIM_C00_DIM_CA_IOC_CTLR_PNCNT_CFG_IN)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_PNCNT_CFG_NCNT_INIT_CSR_BMSK                     0x1f00
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_PNCNT_CFG_NCNT_INIT_CSR_SHFT                        0x8
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_PNCNT_CFG_PCNT_INIT_CSR_BMSK                       0x1f
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_PNCNT_CFG_PCNT_INIT_CSR_SHFT                        0x0

#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_CFG_ADDR                               (DIM_C00_DDRPHY_CA_REG_BASE      + 0x000000e8)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_CFG_RMSK                               0xffffffff
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_CFG_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_CFG_ADDR, HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_CFG_RMSK)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_CFG_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_CFG_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_CFG_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_CFG_ADDR,m,v,HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_CFG_IN)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_CFG_TIMER_PERIOD_BMSK                  0xffff0000
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_CFG_TIMER_PERIOD_SHFT                        0x10
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_CFG_FF_TIMER_PERIOD_BMSK                   0xffff
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_CFG_FF_TIMER_PERIOD_SHFT                      0x0

#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_STATUS_ADDR                            (DIM_C00_DDRPHY_CA_REG_BASE      + 0x000000ec)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_STATUS_RMSK                                0xffff
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_STATUS_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_STATUS_ADDR, HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_STATUS_RMSK)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_STATUS_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_STATUS_TIMER_STATUS_BMSK                   0xffff
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_TIMER_STATUS_TIMER_STATUS_SHFT                      0x0

#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_ADDR                                (DIM_C00_DDRPHY_CA_REG_BASE      + 0x000000f0)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_RMSK                                   0x19f9f
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_ADDR, HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_RMSK)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_ADDR,m,v,HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_IN)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_SM_BYP_ENA_BMSK                        0x10000
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_SM_BYP_ENA_SHFT                           0x10
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_SM_BYP_N_ENA_BMSK                       0x8000
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_SM_BYP_N_ENA_SHFT                          0xf
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_SM_BYP_NCNT_BMSK                        0x1f00
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_SM_BYP_NCNT_SHFT                           0x8
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_SM_BYP_P_ENA_BMSK                         0x80
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_SM_BYP_P_ENA_SHFT                          0x7
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_SM_BYP_PCNT_BMSK                          0x1f
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_CHAR_CFG_SM_BYP_PCNT_SHFT                           0x0

#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_STATUS_ADDR                                  (DIM_C00_DDRPHY_CA_REG_BASE      + 0x000000f4)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_STATUS_RMSK                                  0x80071f1f
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_STATUS_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_IOC_CTLR_STATUS_ADDR, HWIO_DIM_C00_DIM_CA_IOC_CTLR_STATUS_RMSK)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_IOC_CTLR_STATUS_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_STATUS_INIT_IOCAL_DONE_BMSK                  0x80000000
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_STATUS_INIT_IOCAL_DONE_SHFT                        0x1f
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_STATUS_IOCAL_DONE_D_BMSK                        0x40000
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_STATUS_IOCAL_DONE_D_SHFT                           0x12
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_STATUS_IOCAL_BUSY_BMSK                          0x20000
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_STATUS_IOCAL_BUSY_SHFT                             0x11
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_STATUS_SYNC_COMP_BMSK                           0x10000
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_STATUS_SYNC_COMP_SHFT                              0x10
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_STATUS_NCNT_HOLD_BMSK                            0x1f00
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_STATUS_NCNT_HOLD_SHFT                               0x8
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_STATUS_PCNT_HOLD_BMSK                              0x1f
#define HWIO_DIM_C00_DIM_CA_IOC_CTLR_STATUS_PCNT_HOLD_SHFT                               0x0

#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_ADDR                                   (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000100)
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_RMSK                                   0x9f1f9f9f
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_ADDR, HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_RMSK)
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_ADDR,m,v,HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_IN)
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_BMSK                  0x80000000
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_SHFT                        0x1f
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_NCNT_SW_VAL_BMSK                       0x1f000000
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_NCNT_SW_VAL_SHFT                             0x18
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_PCNT_SW_VAL_BMSK                         0x1f0000
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_PCNT_SW_VAL_SHFT                             0x10
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_NCNT_OFFSET_SIGN_BMSK                      0x8000
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_NCNT_OFFSET_SIGN_SHFT                         0xf
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_NCNT_SW_OFFSET_BMSK                        0x1f00
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_NCNT_SW_OFFSET_SHFT                           0x8
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_PCNT_OFFSET_SIGN_BMSK                        0x80
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_PCNT_OFFSET_SIGN_SHFT                         0x7
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_PCNT_SW_OFFSET_BMSK                          0x1f
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_CFG_PCNT_SW_OFFSET_SHFT                           0x0

#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_STATUS_ADDR                                (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000104)
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_STATUS_RMSK                                    0x1f1f
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_STATUS_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_STATUS_ADDR, HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_STATUS_RMSK)
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_STATUS_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_STATUS_PAD_NCNT_BMSK                           0x1f00
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_STATUS_PAD_NCNT_SHFT                              0x8
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_STATUS_PAD_PCNT_BMSK                             0x1f
#define HWIO_DIM_C00_DIM_CA_CA_IOC_SLV_STATUS_PAD_PCNT_SHFT                              0x0

#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_ADDR                                   (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000110)
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_RMSK                                   0x9f1f9f9f
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_ADDR, HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_RMSK)
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_OUT(v)      \
        out_dword(HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_ADDR,v)
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_ADDR,m,v,HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_IN)
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_BMSK                  0x80000000
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_SHFT                        0x1f
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_NCNT_SW_VAL_BMSK                       0x1f000000
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_NCNT_SW_VAL_SHFT                             0x18
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_PCNT_SW_VAL_BMSK                         0x1f0000
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_PCNT_SW_VAL_SHFT                             0x10
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_NCNT_OFFSET_SIGN_BMSK                      0x8000
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_NCNT_OFFSET_SIGN_SHFT                         0xf
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_NCNT_SW_OFFSET_BMSK                        0x1f00
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_NCNT_SW_OFFSET_SHFT                           0x8
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_PCNT_OFFSET_SIGN_BMSK                        0x80
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_PCNT_OFFSET_SIGN_SHFT                         0x7
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_PCNT_SW_OFFSET_BMSK                          0x1f
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_CFG_PCNT_SW_OFFSET_SHFT                           0x0

#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_STATUS_ADDR                                (DIM_C00_DDRPHY_CA_REG_BASE      + 0x00000114)
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_STATUS_RMSK                                    0x1f1f
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_STATUS_IN          \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_STATUS_ADDR, HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_STATUS_RMSK)
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_STATUS_ADDR, m)
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_STATUS_PAD_NCNT_BMSK                           0x1f00
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_STATUS_PAD_NCNT_SHFT                              0x8
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_STATUS_PAD_PCNT_BMSK                             0x1f
#define HWIO_DIM_C00_DIM_CA_CK_IOC_SLV_STATUS_PAD_PCNT_SHFT                              0x0

/*----------------------------------------------------------------------------
 * MODULE: DIM_D00_DDRPHY_DQ
 *--------------------------------------------------------------------------*/

#define DIM_D00_DDRPHY_DQ_REG_BASE                                                   (EBI1_PHY_CFG_BASE      + 0x00000800)

#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_ADDR                                             (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000000)
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_RMSK                                             0x7f013131
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_TOP_CFG_ADDR, HWIO_DIM_D00_DIM_DQ_TOP_CFG_RMSK)
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_TOP_CFG_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_TOP_CFG_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_TOP_CFG_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_TOP_CFG_IN)
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_DQ_LB_MODE_SEL_BMSK                              0x40000000
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_DQ_LB_MODE_SEL_SHFT                                    0x1e
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_DQ_LB_MODE_EN_BMSK                               0x20000000
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_DQ_LB_MODE_EN_SHFT                                     0x1d
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_MISR_EN_OVRD_BMSK                                0x10000000
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_MISR_EN_OVRD_SHFT                                      0x1c
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_MISR_DATA_CLK_SEL_BMSK                            0x8000000
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_MISR_DATA_CLK_SEL_SHFT                                 0x1b
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_CDC_LDO_EN_BMSK                                   0x4000000
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_CDC_LDO_EN_SHFT                                        0x1a
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_CDC_SWITCH_RC_EN_BMSK                             0x2000000
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_CDC_SWITCH_RC_EN_SHFT                                  0x19
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_CDC_SWITCH_BYPASS_OFF_BMSK                        0x1000000
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_CDC_SWITCH_BYPASS_OFF_SHFT                             0x18
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_RCW_EN_BMSK                                         0x10000
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_RCW_EN_SHFT                                            0x10
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_DEBUG_BUS_SEL_BMSK                                   0x2000
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_DEBUG_BUS_SEL_SHFT                                      0xd
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_DEBUG_BUS_EN_BMSK                                    0x1000
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_DEBUG_BUS_EN_SHFT                                       0xc
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_CDC_TEST_EN_BMSK                                      0x100
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_CDC_TEST_EN_SHFT                                        0x8
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_WR_PIPE_EXTEND1_BMSK                                   0x20
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_WR_PIPE_EXTEND1_SHFT                                    0x5
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_WR_PIPE_EXTEND0_BMSK                                   0x10
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_WR_PIPE_EXTEND0_SHFT                                    0x4
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_WR_CLK_SEL_BMSK                                         0x1
#define HWIO_DIM_D00_DIM_DQ_TOP_CFG_WR_CLK_SEL_SHFT                                         0x0

#define HWIO_DIM_D00_DIM_DQ_HW_INFO_ADDR                                             (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000004)
#define HWIO_DIM_D00_DIM_DQ_HW_INFO_RMSK                                                0xfffff
#define HWIO_DIM_D00_DIM_DQ_HW_INFO_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_HW_INFO_ADDR, HWIO_DIM_D00_DIM_DQ_HW_INFO_RMSK)
#define HWIO_DIM_D00_DIM_DQ_HW_INFO_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_HW_INFO_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_HW_INFO_CORE_ID_BMSK                                        0xf0000
#define HWIO_DIM_D00_DIM_DQ_HW_INFO_CORE_ID_SHFT                                           0x10
#define HWIO_DIM_D00_DIM_DQ_HW_INFO_MAJOR_REV_BMSK                                       0xf000
#define HWIO_DIM_D00_DIM_DQ_HW_INFO_MAJOR_REV_SHFT                                          0xc
#define HWIO_DIM_D00_DIM_DQ_HW_INFO_BRANCH_REV_BMSK                                       0xf00
#define HWIO_DIM_D00_DIM_DQ_HW_INFO_BRANCH_REV_SHFT                                         0x8
#define HWIO_DIM_D00_DIM_DQ_HW_INFO_MINOR_REV_BMSK                                         0xff
#define HWIO_DIM_D00_DIM_DQ_HW_INFO_MINOR_REV_SHFT                                          0x0

#define HWIO_DIM_D00_DIM_DQ_HW_VERSION_ADDR                                          (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000008)
#define HWIO_DIM_D00_DIM_DQ_HW_VERSION_RMSK                                          0xffffffff
#define HWIO_DIM_D00_DIM_DQ_HW_VERSION_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_HW_VERSION_ADDR, HWIO_DIM_D00_DIM_DQ_HW_VERSION_RMSK)
#define HWIO_DIM_D00_DIM_DQ_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_HW_VERSION_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_HW_VERSION_MAJOR_BMSK                                    0xf0000000
#define HWIO_DIM_D00_DIM_DQ_HW_VERSION_MAJOR_SHFT                                          0x1c
#define HWIO_DIM_D00_DIM_DQ_HW_VERSION_MINOR_BMSK                                     0xfff0000
#define HWIO_DIM_D00_DIM_DQ_HW_VERSION_MINOR_SHFT                                          0x10
#define HWIO_DIM_D00_DIM_DQ_HW_VERSION_STEP_BMSK                                         0xffff
#define HWIO_DIM_D00_DIM_DQ_HW_VERSION_STEP_SHFT                                            0x0

#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_ADDR                                            (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000010)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_RMSK                                            0xff3333f7
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_PAD_CFG0_ADDR, HWIO_DIM_D00_DIM_DQ_PAD_CFG0_RMSK)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_PAD_CFG0_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_PAD_CFG0_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_PAD_CFG0_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_PAD_CFG0_IN)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_DDR_MODE1_BMSK                               0x80000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_DDR_MODE1_SHFT                                     0x1f
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_DDR_MODE0_BMSK                               0x40000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_DDR_MODE0_SHFT                                     0x1e
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_LV_MODE_BMSK                                 0x20000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_LV_MODE_SHFT                                       0x1d
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_ODT_ENA_BMSK                                 0x10000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_ODT_ENA_SHFT                                       0x1c
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_ODT_BMSK                                      0xc000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_ODT_SHFT                                           0x1a
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_PULL_B_BMSK                                   0x3000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_PULL_B_SHFT                                        0x18
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_NSLEW_BMSK                                     0x300000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_NSLEW_SHFT                                         0x14
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_PSLEW_BMSK                                      0x30000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_PSLEW_SHFT                                         0x10
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_NRXDEL_BMSK                                      0x3000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_NRXDEL_SHFT                                         0xc
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_PRXDEL_BMSK                                       0x300
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_PRXDEL_SHFT                                         0x8
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_VM_SHIFT_ENA_BMSK                                  0x80
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_VM_SHIFT_ENA_SHFT                                   0x7
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_ROUT_BMSK                                          0x70
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_ROUT_SHFT                                           0x4
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_DCC_BMSK                                            0x7
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG0_DQ_DCC_SHFT                                            0x0

#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_ADDR                                            (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000014)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_RMSK                                            0xff3333f7
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_PAD_CFG1_ADDR, HWIO_DIM_D00_DIM_DQ_PAD_CFG1_RMSK)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_PAD_CFG1_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_PAD_CFG1_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_PAD_CFG1_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_PAD_CFG1_IN)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_DDR_MODE1_BMSK                              0x80000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_DDR_MODE1_SHFT                                    0x1f
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_DDR_MODE0_BMSK                              0x40000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_DDR_MODE0_SHFT                                    0x1e
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_LV_MODE_BMSK                                0x20000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_LV_MODE_SHFT                                      0x1d
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_ODT_ENA_BMSK                                0x10000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_ODT_ENA_SHFT                                      0x1c
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_ODT_BMSK                                     0xc000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_ODT_SHFT                                          0x1a
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_PULL_BMSK                                    0x3000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_PULL_SHFT                                         0x18
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_NSLEW_BMSK                                    0x300000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_NSLEW_SHFT                                        0x14
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_PSLEW_BMSK                                     0x30000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_PSLEW_SHFT                                        0x10
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_NRXDEL_BMSK                                     0x3000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_NRXDEL_SHFT                                        0xc
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_PRXDEL_BMSK                                      0x300
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_PRXDEL_SHFT                                        0x8
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_VM_SHIFT_ENA_BMSK                                 0x80
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_VM_SHIFT_ENA_SHFT                                  0x7
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_ROUT_BMSK                                         0x70
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_ROUT_SHFT                                          0x4
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_DCC_BMSK                                           0x7
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG1_DQS_DCC_SHFT                                           0x0

#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_ADDR                                            (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000018)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_RMSK                                            0x3f3f3f3f
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_PAD_CFG2_ADDR, HWIO_DIM_D00_DIM_DQ_PAD_CFG2_RMSK)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_PAD_CFG2_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_PAD_CFG2_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_PAD_CFG2_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_PAD_CFG2_IN)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_VREF_LDO_ENA_BMSK                               0x20000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_VREF_LDO_ENA_SHFT                                     0x1d
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_VREF_PASSGATE_ENA_BMSK                          0x10000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_VREF_PASSGATE_ENA_SHFT                                0x1c
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_VREF_SP_OUT_BMSK                                 0xf000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_VREF_SP_OUT_SHFT                                      0x18
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_VREF_SP_IN_BMSK                                   0x3c0000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_VREF_SP_IN_SHFT                                       0x12
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_VREF_BYPASS_ENA_BMSK                               0x20000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_VREF_BYPASS_ENA_SHFT                                  0x11
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_VREF_RDIV_ENA_BMSK                                 0x10000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_VREF_RDIV_ENA_SHFT                                    0x10
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_VREF_PULLDN_SPEED_CNTL_BMSK                         0x3f00
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_VREF_PULLDN_SPEED_CNTL_SHFT                            0x8
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_VREF_LEVEL_CNTL_BMSK                                  0x3f
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG2_VREF_LEVEL_CNTL_SHFT                                   0x0

#define HWIO_DIM_D00_DIM_DQ_DQ_SMT_STATUS_ADDR                                       (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x0000001c)
#define HWIO_DIM_D00_DIM_DQ_DQ_SMT_STATUS_RMSK                                             0xff
#define HWIO_DIM_D00_DIM_DQ_DQ_SMT_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_DQ_SMT_STATUS_ADDR, HWIO_DIM_D00_DIM_DQ_DQ_SMT_STATUS_RMSK)
#define HWIO_DIM_D00_DIM_DQ_DQ_SMT_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_DQ_SMT_STATUS_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_DQ_SMT_STATUS_DIM_DQ_DQ_SMT_STATUS_BMSK                        0xff
#define HWIO_DIM_D00_DIM_DQ_DQ_SMT_STATUS_DIM_DQ_DQ_SMT_STATUS_SHFT                         0x0

#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_ADDR                                            (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000020)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_RMSK                                            0x1fffff33
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_PAD_CFG3_ADDR, HWIO_DIM_D00_DIM_DQ_PAD_CFG3_RMSK)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_PAD_CFG3_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_PAD_CFG3_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_PAD_CFG3_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_PAD_CFG3_IN)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DQS_DIFF_MODE_BMSK                              0x10000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DQS_DIFF_MODE_SHFT                                    0x1c
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_RCW_ODT_ENA1_BMSK                                0x8000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_RCW_ODT_ENA1_SHFT                                     0x1b
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_RCW_ODT_ENA0_BMSK                                0x4000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_RCW_ODT_ENA0_SHFT                                     0x1a
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_RCW_ODT_BMSK                                     0x3000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_RCW_ODT_SHFT                                          0x18
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DQ_ODT_ENA1_BMSK                                  0x800000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DQ_ODT_ENA1_SHFT                                      0x17
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DQ_ODT_ENA0_BMSK                                  0x400000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DQ_ODT_ENA0_SHFT                                      0x16
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DQ_ODT_BMSK                                       0x300000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DQ_ODT_SHFT                                           0x14
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DQS_ODT_ENA1_BMSK                                  0x80000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DQS_ODT_ENA1_SHFT                                     0x13
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DQS_ODT_ENA0_BMSK                                  0x40000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DQS_ODT_ENA0_SHFT                                     0x12
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DQS_ODT_BMSK                                       0x30000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DQS_ODT_SHFT                                          0x10
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DQ_IE_OE_BMSK                                       0xff00
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DQ_IE_OE_SHFT                                          0x8
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_RCW_IE_OE_BMSK                                        0x20
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_RCW_IE_OE_SHFT                                         0x5
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DQS_IE_OE_BMSK                                        0x10
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DQS_IE_OE_SHFT                                         0x4
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DM_IE_BMSK                                             0x2
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DM_IE_SHFT                                             0x1
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DM_OE_BMSK                                             0x1
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG3_DM_OE_SHFT                                             0x0

#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_ADDR                                            (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000024)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RMSK                                            0xe33333f7
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_PAD_CFG4_ADDR, HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RMSK)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_PAD_CFG4_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_PAD_CFG4_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_PAD_CFG4_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_PAD_CFG4_IN)
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_DDR_MODE1_BMSK                              0x80000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_DDR_MODE1_SHFT                                    0x1f
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_DDR_MODE0_BMSK                              0x40000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_DDR_MODE0_SHFT                                    0x1e
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_LV_MODE_BMSK                                0x20000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_LV_MODE_SHFT                                      0x1d
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_PULL_B_BMSK                                  0x3000000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_PULL_B_SHFT                                       0x18
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_NSLEW_BMSK                                    0x300000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_NSLEW_SHFT                                        0x14
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_PSLEW_BMSK                                     0x30000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_PSLEW_SHFT                                        0x10
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_NRXDEL_BMSK                                     0x3000
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_NRXDEL_SHFT                                        0xc
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_PRXDEL_BMSK                                      0x300
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_PRXDEL_SHFT                                        0x8
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_VM_SHIFT_ENA_BMSK                                 0x80
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_VM_SHIFT_ENA_SHFT                                  0x7
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_ROUT_BMSK                                         0x70
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_ROUT_SHFT                                          0x4
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_DCC_BMSK                                           0x7
#define HWIO_DIM_D00_DIM_DQ_PAD_CFG4_RCW_DCC_SHFT                                           0x0

#define HWIO_DIM_D00_DDRPHY_DQ_MISR_CFG_ADDR                                         (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x0000000c)
#define HWIO_DIM_D00_DDRPHY_DQ_MISR_CFG_RMSK                                         0xffffffff
#define HWIO_DIM_D00_DDRPHY_DQ_MISR_CFG_IN          \
        in_dword_masked(HWIO_DIM_D00_DDRPHY_DQ_MISR_CFG_ADDR, HWIO_DIM_D00_DDRPHY_DQ_MISR_CFG_RMSK)
#define HWIO_DIM_D00_DDRPHY_DQ_MISR_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DDRPHY_DQ_MISR_CFG_ADDR, m)
#define HWIO_DIM_D00_DDRPHY_DQ_MISR_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D00_DDRPHY_DQ_MISR_CFG_ADDR,v)
#define HWIO_DIM_D00_DDRPHY_DQ_MISR_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DDRPHY_DQ_MISR_CFG_ADDR,m,v,HWIO_DIM_D00_DDRPHY_DQ_MISR_CFG_IN)
#define HWIO_DIM_D00_DDRPHY_DQ_MISR_CFG_SEED_VALUE_BMSK                              0xffffffff
#define HWIO_DIM_D00_DDRPHY_DQ_MISR_CFG_SEED_VALUE_SHFT                                     0x0

#define HWIO_DIM_D00_DDRPHY_DQ_MISR_EVEN_STATUS_ADDR                                 (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000028)
#define HWIO_DIM_D00_DDRPHY_DQ_MISR_EVEN_STATUS_RMSK                                 0xffffffff
#define HWIO_DIM_D00_DDRPHY_DQ_MISR_EVEN_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D00_DDRPHY_DQ_MISR_EVEN_STATUS_ADDR, HWIO_DIM_D00_DDRPHY_DQ_MISR_EVEN_STATUS_RMSK)
#define HWIO_DIM_D00_DDRPHY_DQ_MISR_EVEN_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DDRPHY_DQ_MISR_EVEN_STATUS_ADDR, m)
#define HWIO_DIM_D00_DDRPHY_DQ_MISR_EVEN_STATUS_SIGNATURE_BMSK                       0xffffffff
#define HWIO_DIM_D00_DDRPHY_DQ_MISR_EVEN_STATUS_SIGNATURE_SHFT                              0x0

#define HWIO_DIM_D00_DDRPHY_DQ_MISR_ODD_STATUS_ADDR                                  (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x0000002c)
#define HWIO_DIM_D00_DDRPHY_DQ_MISR_ODD_STATUS_RMSK                                  0xffffffff
#define HWIO_DIM_D00_DDRPHY_DQ_MISR_ODD_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D00_DDRPHY_DQ_MISR_ODD_STATUS_ADDR, HWIO_DIM_D00_DDRPHY_DQ_MISR_ODD_STATUS_RMSK)
#define HWIO_DIM_D00_DDRPHY_DQ_MISR_ODD_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DDRPHY_DQ_MISR_ODD_STATUS_ADDR, m)
#define HWIO_DIM_D00_DDRPHY_DQ_MISR_ODD_STATUS_SIGNATURE_BMSK                        0xffffffff
#define HWIO_DIM_D00_DDRPHY_DQ_MISR_ODD_STATUS_SIGNATURE_SHFT                               0x0

#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_ADDR                                       (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000030)
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_RMSK                                        0x7ff37ff
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_ADDR, HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_RMSK)
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_IN)
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_ERROR_CODE_UPDATE_EN_BMSK                   0x4000000
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_ERROR_CODE_UPDATE_EN_SHFT                        0x1a
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_STAGGER_CAL_ENA_BMSK                        0x2000000
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_STAGGER_CAL_ENA_SHFT                             0x19
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_TRACKING_CAL_ENA_BMSK                       0x1000000
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_TRACKING_CAL_ENA_SHFT                            0x18
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_OSC_COUNT_ERR_TOLERANCE_BMSK                 0xf00000
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_OSC_COUNT_ERR_TOLERANCE_SHFT                     0x14
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_TRACK_CALIB_MODE_BMSK                         0x80000
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_TRACK_CALIB_MODE_SHFT                            0x13
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_FULL_DELAY_BMSK                               0x40000
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_FULL_DELAY_SHFT                                  0x12
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_HW_AUTOCAL_ENA_BMSK                           0x20000
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_HW_AUTOCAL_ENA_SHFT                              0x11
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_SW_TRIG_FULL_CALIB_BMSK                       0x10000
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_SW_TRIG_FULL_CALIB_SHFT                          0x10
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_OSC_PRE_DIV_BMSK                               0x3000
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_OSC_PRE_DIV_SHFT                                  0xc
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_TMUX_CHAR_BMSK                                  0x7ff
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG0_TMUX_CHAR_SHFT                                    0x0

#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_ADDR                                       (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000034)
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_RMSK                                        0x7f77777
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_ADDR, HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_RMSK)
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_IN)
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_OSC_COUNT_DELAY_BMSK                        0x7000000
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_OSC_COUNT_DELAY_SHFT                             0x18
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_STANDBY_DELAY_BMSK                           0xf00000
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_STANDBY_DELAY_SHFT                               0x14
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_DEL_MODE_DELAY_BMSK                           0x70000
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_DEL_MODE_DELAY_SHFT                              0x10
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_OSC_MODE_DELAY_BMSK                            0x7000
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_OSC_MODE_DELAY_SHFT                               0xc
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_DECODER_DELAY_BMSK                              0x700
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_DECODER_DELAY_SHFT                                0x8
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_DIVIDER_DELAY_BMSK                               0x70
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_DIVIDER_DELAY_SHFT                                0x4
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_MULTIPLIER_DELAY_BMSK                             0x7
#define HWIO_DIM_D00_DIM_DQ_CDC_CTLR_CFG1_MULTIPLIER_DELAY_SHFT                             0x0

#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR                                  (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000038)
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_RMSK                                   0x1f1ffff
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR, HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_RMSK)
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_IN)
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_INVALID_TIMER_ENA_BMSK                 0x1000000
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_INVALID_TIMER_ENA_SHFT                      0x18
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_INVALID_TIMER_VAL_BMSK                  0xf00000
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_INVALID_TIMER_VAL_SHFT                      0x14
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_TIMER_ENA_BMSK                           0x10000
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_TIMER_ENA_SHFT                              0x10
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_TIMER_VAL_BMSK                            0xffff
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG0_TIMER_VAL_SHFT                               0x0

#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR                                  (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x0000003c)
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG1_RMSK                                      0x13ff
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG1_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR, HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG1_RMSK)
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG1_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG1_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG1_IN)
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG1_FF_TIMER_ENA_BMSK                         0x1000
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG1_FF_TIMER_ENA_SHFT                            0xc
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG1_FF_TIMER_VAL_BMSK                          0x3ff
#define HWIO_DIM_D00_DIM_DQ_CDC_CAL_TIMER_CFG1_FF_TIMER_VAL_SHFT                            0x0

#define HWIO_DIM_D00_DIM_DQ_CDC_REFCOUNT_CFG_ADDR                                    (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000040)
#define HWIO_DIM_D00_DIM_DQ_CDC_REFCOUNT_CFG_RMSK                                    0xffff3f3f
#define HWIO_DIM_D00_DIM_DQ_CDC_REFCOUNT_CFG_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_CDC_REFCOUNT_CFG_ADDR, HWIO_DIM_D00_DIM_DQ_CDC_REFCOUNT_CFG_RMSK)
#define HWIO_DIM_D00_DIM_DQ_CDC_REFCOUNT_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_CDC_REFCOUNT_CFG_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_CDC_REFCOUNT_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_CDC_REFCOUNT_CFG_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_CDC_REFCOUNT_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_CDC_REFCOUNT_CFG_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_CDC_REFCOUNT_CFG_IN)
#define HWIO_DIM_D00_DIM_DQ_CDC_REFCOUNT_CFG_TREF_BMSK                               0xffff0000
#define HWIO_DIM_D00_DIM_DQ_CDC_REFCOUNT_CFG_TREF_SHFT                                     0x10
#define HWIO_DIM_D00_DIM_DQ_CDC_REFCOUNT_CFG_CCAL_REF_COUNT_BMSK                         0x3f00
#define HWIO_DIM_D00_DIM_DQ_CDC_REFCOUNT_CFG_CCAL_REF_COUNT_SHFT                            0x8
#define HWIO_DIM_D00_DIM_DQ_CDC_REFCOUNT_CFG_FCAL_REF_COUNT_BMSK                           0x3f
#define HWIO_DIM_D00_DIM_DQ_CDC_REFCOUNT_CFG_FCAL_REF_COUNT_SHFT                            0x0

#define HWIO_DIM_D00_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR                                  (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000044)
#define HWIO_DIM_D00_DIM_DQ_CDC_COARSE_CAL_CFG_RMSK                                      0x1f3f
#define HWIO_DIM_D00_DIM_DQ_CDC_COARSE_CAL_CFG_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR, HWIO_DIM_D00_DIM_DQ_CDC_COARSE_CAL_CFG_RMSK)
#define HWIO_DIM_D00_DIM_DQ_CDC_COARSE_CAL_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_CDC_COARSE_CAL_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_CDC_COARSE_CAL_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_CDC_COARSE_CAL_CFG_IN)
#define HWIO_DIM_D00_DIM_DQ_CDC_COARSE_CAL_CFG_CCAL_SUBUNIT_CAPS_BMSK                    0x1f00
#define HWIO_DIM_D00_DIM_DQ_CDC_COARSE_CAL_CFG_CCAL_SUBUNIT_CAPS_SHFT                       0x8
#define HWIO_DIM_D00_DIM_DQ_CDC_COARSE_CAL_CFG_CCAL_UNITSTEPS_BMSK                         0x3f
#define HWIO_DIM_D00_DIM_DQ_CDC_COARSE_CAL_CFG_CCAL_UNITSTEPS_SHFT                          0x0

#define HWIO_DIM_D00_DIM_DQ_CDC_RSVD_CFG_ADDR                                        (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000048)
#define HWIO_DIM_D00_DIM_DQ_CDC_RSVD_CFG_RMSK                                            0xffff
#define HWIO_DIM_D00_DIM_DQ_CDC_RSVD_CFG_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_CDC_RSVD_CFG_ADDR, HWIO_DIM_D00_DIM_DQ_CDC_RSVD_CFG_RMSK)
#define HWIO_DIM_D00_DIM_DQ_CDC_RSVD_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_CDC_RSVD_CFG_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_CDC_RSVD_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_CDC_RSVD_CFG_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_CDC_RSVD_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_CDC_RSVD_CFG_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_CDC_RSVD_CFG_IN)
#define HWIO_DIM_D00_DIM_DQ_CDC_RSVD_CFG_TEMP_FIELD_BMSK                                 0xffff
#define HWIO_DIM_D00_DIM_DQ_CDC_RSVD_CFG_TEMP_FIELD_SHFT                                    0x0

#define HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR                                   (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x0000004c)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_RMSK                                     0x19f1bf
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR, HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_RMSK)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_IN)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_MODE_BMSK                 0x100000
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_MODE_SHFT                     0x14
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SIGN_BMSK                  0x80000
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SIGN_SHFT                     0x13
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_BMSK                       0x1f000
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SHFT                           0xc
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_MODE_BMSK                       0x100
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_MODE_SHFT                         0x8
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_SIGN_BMSK                        0x80
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_SIGN_SHFT                         0x7
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_BMSK                             0x3f
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_SHFT                              0x0

#define HWIO_DIM_D00_DIM_DQ_RD_CDC_DELAY_CFG_ADDR                                    (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000050)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_DELAY_CFG_RMSK                                    0x1fff0fff
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_DELAY_CFG_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_DELAY_CFG_ADDR, HWIO_DIM_D00_DIM_DQ_RD_CDC_DELAY_CFG_RMSK)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_DELAY_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_DELAY_CFG_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_DELAY_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_RD_CDC_DELAY_CFG_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_DELAY_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_RD_CDC_DELAY_CFG_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_RD_CDC_DELAY_CFG_IN)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_DELAY_CFG_TARGET_COUNT_ENA_BMSK                   0x10000000
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_DELAY_CFG_TARGET_COUNT_ENA_SHFT                         0x1c
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_DELAY_CFG_TARGET_COUNT_BMSK                        0xfff0000
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_DELAY_CFG_TARGET_COUNT_SHFT                             0x10
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_DELAY_CFG_DELAY_VAL_BMSK                               0xfff
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_DELAY_CFG_DELAY_VAL_SHFT                                 0x0

#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR                                  (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000054)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_RMSK                                         0xf
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR, HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_RMSK)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_IN)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_DEL_MODE_BMSK                             0x8
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_DEL_MODE_SHFT                             0x3
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_OSC_MODE_BMSK                             0x4
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_OSC_MODE_SHFT                             0x2
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_DA_SEL_BMSK                               0x2
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_DA_SEL_SHFT                               0x1
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_LOAD_BMSK                                 0x1
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_LOAD_SHFT                                 0x0

#define HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_ADDR                                     (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000058)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_RMSK                                            0xf
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_ADDR, HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_RMSK)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_IN)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_CDC_SEL_DDA_TEST_BMSK                           0x8
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_CDC_SEL_DDA_TEST_SHFT                           0x3
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_CDC_OSC_TEST_EN_BMSK                            0x4
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_CDC_OSC_TEST_EN_SHFT                            0x2
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_CDC_TEST_EN_BMSK                                0x2
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_CDC_TEST_EN_SHFT                                0x1
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_CDC_OUT_ONTEST2_BMSK                            0x1
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_TEST_CFG_CDC_OUT_ONTEST2_SHFT                            0x0

#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR                                  (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x0000005c)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_RMSK                                     0x10f0f
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR, HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_RMSK)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_IN)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_REF_GATE_BMSK                         0x10000
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_REF_GATE_SHFT                            0x10
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA1_OSC_EN_BMSK                    0x800
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA1_OSC_EN_SHFT                      0xb
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA1_IN_EN_BMSK                     0x400
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA1_IN_EN_SHFT                       0xa
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA0_OSC_EN_BMSK                    0x200
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA0_OSC_EN_SHFT                      0x9
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA0_IN_EN_BMSK                     0x100
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA0_IN_EN_SHFT                       0x8
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_CDC_COUNTER_RST_BMSK                 0x8
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_CDC_COUNTER_RST_SHFT                 0x3
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_LOAD_DA_SEL_BMSK                     0x4
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_LOAD_DA_SEL_SHFT                     0x2
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_ACTV_DA_SEL_BMSK                     0x2
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_ACTV_DA_SEL_SHFT                     0x1
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_ENA_BMSK                             0x1
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_ENA_SHFT                             0x0

#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR                                (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000060)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_RMSK                                   0x3f7ff
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR, HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_RMSK)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_IN)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAV_DDA_OFFSET_MODE_BMSK              0x20000
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAV_DDA_OFFSET_MODE_SHFT                 0x11
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAV_DDA_OFFSET_SIGN_BMSK              0x10000
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAV_DDA_OFFSET_SIGN_SHFT                 0x10
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAVE_DDA_OFFSET_BMSK                   0xf000
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAVE_DDA_OFFSET_SHFT                      0xc
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAVE_DDA_DELAY_BMSK                     0x7ff
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAVE_DDA_DELAY_SHFT                       0x0

#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_ADDR                                      (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000070)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_RMSK                                       0x7ffffff
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_ADDR, HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_RMSK)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_CDC_ERROR_CODE_BMSK                        0x7000000
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_CDC_ERROR_CODE_SHFT                             0x18
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_SW_REF_GATE_COUNT_BMSK                      0xff0000
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_SW_REF_GATE_COUNT_SHFT                          0x10
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_OSC_COUNT_BMSK                                0xfff0
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_OSC_COUNT_SHFT                                   0x4
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_CURR_SEL_DA_BMSK                                 0x8
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_CURR_SEL_DA_SHFT                                 0x3
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_CTLR_SM_IDLE_BMSK                                0x4
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_CTLR_SM_IDLE_SHFT                                0x2
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_OSC_DONE_BMSK                                    0x2
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_OSC_DONE_SHFT                                    0x1
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_CALIBRATION_DONE_BMSK                            0x1
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS0_CALIBRATION_DONE_SHFT                            0x0

#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS1_ADDR                                      (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000074)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS1_RMSK                                       0xfff0fff
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS1_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS1_ADDR, HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS1_RMSK)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS1_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS1_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS1_CURR_DELAY_VALUE_BMSK                      0xfff0000
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS1_CURR_DELAY_VALUE_SHFT                           0x10
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS1_CURR_TMUX_DELAY_BMSK                           0xfff
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS1_CURR_TMUX_DELAY_SHFT                             0x0

#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS2_ADDR                                      (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000078)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS2_RMSK                                      0x1f3f1f3f
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS2_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS2_ADDR, HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS2_RMSK)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS2_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS2_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS2_DA1_SUBUNITS_BMSK                         0x1f000000
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS2_DA1_SUBUNITS_SHFT                               0x18
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS2_DA1_UNITSTEPS_BMSK                          0x3f0000
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS2_DA1_UNITSTEPS_SHFT                              0x10
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS2_DA0_SUBUNITS_BMSK                             0x1f00
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS2_DA0_SUBUNITS_SHFT                                0x8
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS2_DA0_UNITSTEPS_BMSK                              0x3f
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS2_DA0_UNITSTEPS_SHFT                               0x0

#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS3_ADDR                                      (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x0000007c)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS3_RMSK                                      0xffffffff
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS3_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS3_ADDR, HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS3_RMSK)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS3_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS3_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS3_NUM_OF_OSC_ITER_BMSK                      0xff000000
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS3_NUM_OF_OSC_ITER_SHFT                            0x18
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS3_COUNT_ERROR_BMSK                            0xfff000
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS3_COUNT_ERROR_SHFT                                 0xc
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS3_CURR_TARGET_COUNT_BMSK                         0xfff
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS3_CURR_TARGET_COUNT_SHFT                           0x0

#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS4_ADDR                                      (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000080)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS4_RMSK                                            0xff
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS4_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS4_ADDR, HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS4_RMSK)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS4_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS4_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS4_SLAVE_DDA_DA1_TAPS_BMSK                         0xf0
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS4_SLAVE_DDA_DA1_TAPS_SHFT                          0x4
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS4_SLAVE_DDA_DA0_TAPS_BMSK                          0xf
#define HWIO_DIM_D00_DIM_DQ_RD_CDC_STATUS4_SLAVE_DDA_DA0_TAPS_SHFT                          0x0

#define HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR                                   (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x000000ac)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_RMSK                                     0x19f1bf
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR, HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_RMSK)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_IN)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_MODE_BMSK                 0x100000
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_MODE_SHFT                     0x14
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SIGN_BMSK                  0x80000
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SIGN_SHFT                     0x13
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_BMSK                       0x1f000
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SHFT                           0xc
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_MODE_BMSK                       0x100
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_MODE_SHFT                         0x8
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_SIGN_BMSK                        0x80
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_SIGN_SHFT                         0x7
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_BMSK                             0x3f
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_SHFT                              0x0

#define HWIO_DIM_D00_DIM_DQ_WR_CDC_DELAY_CFG_ADDR                                    (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x000000b0)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_DELAY_CFG_RMSK                                    0x1fff0fff
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_DELAY_CFG_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_WR_CDC_DELAY_CFG_ADDR, HWIO_DIM_D00_DIM_DQ_WR_CDC_DELAY_CFG_RMSK)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_DELAY_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_WR_CDC_DELAY_CFG_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_DELAY_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_WR_CDC_DELAY_CFG_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_DELAY_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_WR_CDC_DELAY_CFG_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_WR_CDC_DELAY_CFG_IN)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_DELAY_CFG_TARGET_COUNT_ENA_BMSK                   0x10000000
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_DELAY_CFG_TARGET_COUNT_ENA_SHFT                         0x1c
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_DELAY_CFG_TARGET_COUNT_BMSK                        0xfff0000
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_DELAY_CFG_TARGET_COUNT_SHFT                             0x10
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_DELAY_CFG_DELAY_VAL_BMSK                               0xfff
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_DELAY_CFG_DELAY_VAL_SHFT                                 0x0

#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR                                  (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x000000b4)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_RMSK                                         0xf
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR, HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_RMSK)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_IN)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_DEL_MODE_BMSK                             0x8
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_DEL_MODE_SHFT                             0x3
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_OSC_MODE_BMSK                             0x4
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_OSC_MODE_SHFT                             0x2
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_DA_SEL_BMSK                               0x2
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_DA_SEL_SHFT                               0x1
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_LOAD_BMSK                                 0x1
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_LOAD_SHFT                                 0x0

#define HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_ADDR                                     (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x000000b8)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_RMSK                                            0xf
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_ADDR, HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_RMSK)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_IN)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_CDC_SEL_DDA_TEST_BMSK                           0x8
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_CDC_SEL_DDA_TEST_SHFT                           0x3
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_CDC_OSC_TEST_EN_BMSK                            0x4
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_CDC_OSC_TEST_EN_SHFT                            0x2
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_CDC_TEST_EN_BMSK                                0x2
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_CDC_TEST_EN_SHFT                                0x1
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_CDC_OUT_ONTEST2_BMSK                            0x1
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_TEST_CFG_CDC_OUT_ONTEST2_SHFT                            0x0

#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR                                  (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x000000bc)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_RMSK                                     0x10f0f
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR, HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_RMSK)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_IN)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_REF_GATE_BMSK                         0x10000
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_REF_GATE_SHFT                            0x10
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA1_OSC_EN_BMSK                    0x800
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA1_OSC_EN_SHFT                      0xb
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA1_IN_EN_BMSK                     0x400
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA1_IN_EN_SHFT                       0xa
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA0_OSC_EN_BMSK                    0x200
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA0_OSC_EN_SHFT                      0x9
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA0_IN_EN_BMSK                     0x100
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA0_IN_EN_SHFT                       0x8
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_CDC_COUNTER_RST_BMSK                 0x8
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_CDC_COUNTER_RST_SHFT                 0x3
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_LOAD_DA_SEL_BMSK                     0x4
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_LOAD_DA_SEL_SHFT                     0x2
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_ACTV_DA_SEL_BMSK                     0x2
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_ACTV_DA_SEL_SHFT                     0x1
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_ENA_BMSK                             0x1
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_ENA_SHFT                             0x0

#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_ADDR                                      (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x000000d0)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_RMSK                                       0x7ffffff
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_ADDR, HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_RMSK)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_CDC_ERROR_CODE_BMSK                        0x7000000
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_CDC_ERROR_CODE_SHFT                             0x18
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_SW_REF_GATE_COUNT_BMSK                      0xff0000
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_SW_REF_GATE_COUNT_SHFT                          0x10
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_OSC_COUNT_BMSK                                0xfff0
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_OSC_COUNT_SHFT                                   0x4
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_CURR_SEL_DA_BMSK                                 0x8
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_CURR_SEL_DA_SHFT                                 0x3
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_CTLR_SM_IDLE_BMSK                                0x4
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_CTLR_SM_IDLE_SHFT                                0x2
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_OSC_DONE_BMSK                                    0x2
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_OSC_DONE_SHFT                                    0x1
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_CALIBRATION_DONE_BMSK                            0x1
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS0_CALIBRATION_DONE_SHFT                            0x0

#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS1_ADDR                                      (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x000000d4)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS1_RMSK                                       0xfff0fff
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS1_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS1_ADDR, HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS1_RMSK)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS1_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS1_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS1_CURR_DELAY_VALUE_BMSK                      0xfff0000
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS1_CURR_DELAY_VALUE_SHFT                           0x10
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS1_CURR_TMUX_DELAY_BMSK                           0xfff
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS1_CURR_TMUX_DELAY_SHFT                             0x0

#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS2_ADDR                                      (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x000000d8)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS2_RMSK                                      0x1f3f1f3f
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS2_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS2_ADDR, HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS2_RMSK)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS2_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS2_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS2_DA1_SUBUNITS_BMSK                         0x1f000000
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS2_DA1_SUBUNITS_SHFT                               0x18
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS2_DA1_UNITSTEPS_BMSK                          0x3f0000
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS2_DA1_UNITSTEPS_SHFT                              0x10
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS2_DA0_SUBUNITS_BMSK                             0x1f00
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS2_DA0_SUBUNITS_SHFT                                0x8
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS2_DA0_UNITSTEPS_BMSK                              0x3f
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS2_DA0_UNITSTEPS_SHFT                               0x0

#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS3_ADDR                                      (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x000000dc)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS3_RMSK                                      0xffffffff
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS3_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS3_ADDR, HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS3_RMSK)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS3_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS3_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS3_NUM_OF_OSC_ITER_BMSK                      0xff000000
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS3_NUM_OF_OSC_ITER_SHFT                            0x18
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS3_COUNT_ERROR_BMSK                            0xfff000
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS3_COUNT_ERROR_SHFT                                 0xc
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS3_CURR_TARGET_COUNT_BMSK                         0xfff
#define HWIO_DIM_D00_DIM_DQ_WR_CDC_STATUS3_CURR_TARGET_COUNT_SHFT                           0x0

#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_ADDR                                      (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000100)
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_RMSK                                      0x9f1f9f9f
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_ADDR, HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_RMSK)
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_IN)
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_BMSK                     0x80000000
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_SHFT                           0x1f
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_SW_VAL_BMSK                          0x1f000000
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_SW_VAL_SHFT                                0x18
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_SW_VAL_BMSK                            0x1f0000
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_SW_VAL_SHFT                                0x10
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_OFFSET_SIGN_BMSK                         0x8000
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_OFFSET_SIGN_SHFT                            0xf
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_SW_OFFSET_BMSK                           0x1f00
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_SW_OFFSET_SHFT                              0x8
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_OFFSET_SIGN_BMSK                           0x80
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_OFFSET_SIGN_SHFT                            0x7
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_SW_OFFSET_BMSK                             0x1f
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_SW_OFFSET_SHFT                              0x0

#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_STATUS_ADDR                                   (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000104)
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_STATUS_RMSK                                       0x1f1f
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_STATUS_ADDR, HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_STATUS_RMSK)
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_STATUS_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_STATUS_PAD_NCNT_BMSK                              0x1f00
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_STATUS_PAD_NCNT_SHFT                                 0x8
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_STATUS_PAD_PCNT_BMSK                                0x1f
#define HWIO_DIM_D00_DIM_DQ_DQ_IOC_SLV_STATUS_PAD_PCNT_SHFT                                 0x0

#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_ADDR                                     (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000110)
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_RMSK                                     0x9f1f9f9f
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_ADDR, HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_RMSK)
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_ADDR,v)
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_ADDR,m,v,HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_IN)
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_BMSK                    0x80000000
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_SHFT                          0x1f
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_SW_VAL_BMSK                         0x1f000000
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_SW_VAL_SHFT                               0x18
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_SW_VAL_BMSK                           0x1f0000
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_SW_VAL_SHFT                               0x10
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_OFFSET_SIGN_BMSK                        0x8000
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_OFFSET_SIGN_SHFT                           0xf
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_SW_OFFSET_BMSK                          0x1f00
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_SW_OFFSET_SHFT                             0x8
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_OFFSET_SIGN_BMSK                          0x80
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_OFFSET_SIGN_SHFT                           0x7
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_SW_OFFSET_BMSK                            0x1f
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_SW_OFFSET_SHFT                             0x0

#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_STATUS_ADDR                                  (DIM_D00_DDRPHY_DQ_REG_BASE      + 0x00000114)
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_STATUS_RMSK                                      0x1f1f
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_STATUS_ADDR, HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_STATUS_RMSK)
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_STATUS_ADDR, m)
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_STATUS_PAD_NCNT_BMSK                             0x1f00
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_STATUS_PAD_NCNT_SHFT                                0x8
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_STATUS_PAD_PCNT_BMSK                               0x1f
#define HWIO_DIM_D00_DIM_DQ_DQS_IOC_SLV_STATUS_PAD_PCNT_SHFT                                0x0

/*----------------------------------------------------------------------------
 * MODULE: DIM_D01_DDRPHY_DQ
 *--------------------------------------------------------------------------*/

#define DIM_D01_DDRPHY_DQ_REG_BASE                                                   (EBI1_PHY_CFG_BASE      + 0x00001000)

#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_ADDR                                             (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000000)
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_RMSK                                             0x7f013131
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_TOP_CFG_ADDR, HWIO_DIM_D01_DIM_DQ_TOP_CFG_RMSK)
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_TOP_CFG_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_TOP_CFG_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_TOP_CFG_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_TOP_CFG_IN)
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_DQ_LB_MODE_SEL_BMSK                              0x40000000
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_DQ_LB_MODE_SEL_SHFT                                    0x1e
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_DQ_LB_MODE_EN_BMSK                               0x20000000
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_DQ_LB_MODE_EN_SHFT                                     0x1d
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_MISR_EN_OVRD_BMSK                                0x10000000
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_MISR_EN_OVRD_SHFT                                      0x1c
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_MISR_DATA_CLK_SEL_BMSK                            0x8000000
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_MISR_DATA_CLK_SEL_SHFT                                 0x1b
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_CDC_LDO_EN_BMSK                                   0x4000000
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_CDC_LDO_EN_SHFT                                        0x1a
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_CDC_SWITCH_RC_EN_BMSK                             0x2000000
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_CDC_SWITCH_RC_EN_SHFT                                  0x19
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_CDC_SWITCH_BYPASS_OFF_BMSK                        0x1000000
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_CDC_SWITCH_BYPASS_OFF_SHFT                             0x18
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_RCW_EN_BMSK                                         0x10000
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_RCW_EN_SHFT                                            0x10
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_DEBUG_BUS_SEL_BMSK                                   0x2000
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_DEBUG_BUS_SEL_SHFT                                      0xd
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_DEBUG_BUS_EN_BMSK                                    0x1000
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_DEBUG_BUS_EN_SHFT                                       0xc
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_CDC_TEST_EN_BMSK                                      0x100
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_CDC_TEST_EN_SHFT                                        0x8
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_WR_PIPE_EXTEND1_BMSK                                   0x20
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_WR_PIPE_EXTEND1_SHFT                                    0x5
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_WR_PIPE_EXTEND0_BMSK                                   0x10
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_WR_PIPE_EXTEND0_SHFT                                    0x4
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_WR_CLK_SEL_BMSK                                         0x1
#define HWIO_DIM_D01_DIM_DQ_TOP_CFG_WR_CLK_SEL_SHFT                                         0x0

#define HWIO_DIM_D01_DIM_DQ_HW_INFO_ADDR                                             (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000004)
#define HWIO_DIM_D01_DIM_DQ_HW_INFO_RMSK                                                0xfffff
#define HWIO_DIM_D01_DIM_DQ_HW_INFO_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_HW_INFO_ADDR, HWIO_DIM_D01_DIM_DQ_HW_INFO_RMSK)
#define HWIO_DIM_D01_DIM_DQ_HW_INFO_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_HW_INFO_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_HW_INFO_CORE_ID_BMSK                                        0xf0000
#define HWIO_DIM_D01_DIM_DQ_HW_INFO_CORE_ID_SHFT                                           0x10
#define HWIO_DIM_D01_DIM_DQ_HW_INFO_MAJOR_REV_BMSK                                       0xf000
#define HWIO_DIM_D01_DIM_DQ_HW_INFO_MAJOR_REV_SHFT                                          0xc
#define HWIO_DIM_D01_DIM_DQ_HW_INFO_BRANCH_REV_BMSK                                       0xf00
#define HWIO_DIM_D01_DIM_DQ_HW_INFO_BRANCH_REV_SHFT                                         0x8
#define HWIO_DIM_D01_DIM_DQ_HW_INFO_MINOR_REV_BMSK                                         0xff
#define HWIO_DIM_D01_DIM_DQ_HW_INFO_MINOR_REV_SHFT                                          0x0

#define HWIO_DIM_D01_DIM_DQ_HW_VERSION_ADDR                                          (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000008)
#define HWIO_DIM_D01_DIM_DQ_HW_VERSION_RMSK                                          0xffffffff
#define HWIO_DIM_D01_DIM_DQ_HW_VERSION_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_HW_VERSION_ADDR, HWIO_DIM_D01_DIM_DQ_HW_VERSION_RMSK)
#define HWIO_DIM_D01_DIM_DQ_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_HW_VERSION_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_HW_VERSION_MAJOR_BMSK                                    0xf0000000
#define HWIO_DIM_D01_DIM_DQ_HW_VERSION_MAJOR_SHFT                                          0x1c
#define HWIO_DIM_D01_DIM_DQ_HW_VERSION_MINOR_BMSK                                     0xfff0000
#define HWIO_DIM_D01_DIM_DQ_HW_VERSION_MINOR_SHFT                                          0x10
#define HWIO_DIM_D01_DIM_DQ_HW_VERSION_STEP_BMSK                                         0xffff
#define HWIO_DIM_D01_DIM_DQ_HW_VERSION_STEP_SHFT                                            0x0

#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_ADDR                                            (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000010)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_RMSK                                            0xff3333f7
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_PAD_CFG0_ADDR, HWIO_DIM_D01_DIM_DQ_PAD_CFG0_RMSK)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_PAD_CFG0_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_PAD_CFG0_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_PAD_CFG0_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_PAD_CFG0_IN)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_DDR_MODE1_BMSK                               0x80000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_DDR_MODE1_SHFT                                     0x1f
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_DDR_MODE0_BMSK                               0x40000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_DDR_MODE0_SHFT                                     0x1e
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_LV_MODE_BMSK                                 0x20000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_LV_MODE_SHFT                                       0x1d
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_ODT_ENA_BMSK                                 0x10000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_ODT_ENA_SHFT                                       0x1c
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_ODT_BMSK                                      0xc000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_ODT_SHFT                                           0x1a
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_PULL_B_BMSK                                   0x3000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_PULL_B_SHFT                                        0x18
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_NSLEW_BMSK                                     0x300000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_NSLEW_SHFT                                         0x14
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_PSLEW_BMSK                                      0x30000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_PSLEW_SHFT                                         0x10
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_NRXDEL_BMSK                                      0x3000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_NRXDEL_SHFT                                         0xc
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_PRXDEL_BMSK                                       0x300
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_PRXDEL_SHFT                                         0x8
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_VM_SHIFT_ENA_BMSK                                  0x80
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_VM_SHIFT_ENA_SHFT                                   0x7
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_ROUT_BMSK                                          0x70
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_ROUT_SHFT                                           0x4
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_DCC_BMSK                                            0x7
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG0_DQ_DCC_SHFT                                            0x0

#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_ADDR                                            (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000014)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_RMSK                                            0xff3333f7
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_PAD_CFG1_ADDR, HWIO_DIM_D01_DIM_DQ_PAD_CFG1_RMSK)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_PAD_CFG1_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_PAD_CFG1_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_PAD_CFG1_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_PAD_CFG1_IN)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_DDR_MODE1_BMSK                              0x80000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_DDR_MODE1_SHFT                                    0x1f
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_DDR_MODE0_BMSK                              0x40000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_DDR_MODE0_SHFT                                    0x1e
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_LV_MODE_BMSK                                0x20000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_LV_MODE_SHFT                                      0x1d
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_ODT_ENA_BMSK                                0x10000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_ODT_ENA_SHFT                                      0x1c
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_ODT_BMSK                                     0xc000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_ODT_SHFT                                          0x1a
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_PULL_BMSK                                    0x3000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_PULL_SHFT                                         0x18
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_NSLEW_BMSK                                    0x300000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_NSLEW_SHFT                                        0x14
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_PSLEW_BMSK                                     0x30000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_PSLEW_SHFT                                        0x10
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_NRXDEL_BMSK                                     0x3000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_NRXDEL_SHFT                                        0xc
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_PRXDEL_BMSK                                      0x300
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_PRXDEL_SHFT                                        0x8
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_VM_SHIFT_ENA_BMSK                                 0x80
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_VM_SHIFT_ENA_SHFT                                  0x7
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_ROUT_BMSK                                         0x70
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_ROUT_SHFT                                          0x4
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_DCC_BMSK                                           0x7
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG1_DQS_DCC_SHFT                                           0x0

#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_ADDR                                            (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000018)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_RMSK                                            0x3f3f3f3f
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_PAD_CFG2_ADDR, HWIO_DIM_D01_DIM_DQ_PAD_CFG2_RMSK)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_PAD_CFG2_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_PAD_CFG2_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_PAD_CFG2_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_PAD_CFG2_IN)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_VREF_LDO_ENA_BMSK                               0x20000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_VREF_LDO_ENA_SHFT                                     0x1d
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_VREF_PASSGATE_ENA_BMSK                          0x10000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_VREF_PASSGATE_ENA_SHFT                                0x1c
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_VREF_SP_OUT_BMSK                                 0xf000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_VREF_SP_OUT_SHFT                                      0x18
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_VREF_SP_IN_BMSK                                   0x3c0000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_VREF_SP_IN_SHFT                                       0x12
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_VREF_BYPASS_ENA_BMSK                               0x20000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_VREF_BYPASS_ENA_SHFT                                  0x11
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_VREF_RDIV_ENA_BMSK                                 0x10000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_VREF_RDIV_ENA_SHFT                                    0x10
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_VREF_PULLDN_SPEED_CNTL_BMSK                         0x3f00
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_VREF_PULLDN_SPEED_CNTL_SHFT                            0x8
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_VREF_LEVEL_CNTL_BMSK                                  0x3f
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG2_VREF_LEVEL_CNTL_SHFT                                   0x0

#define HWIO_DIM_D01_DIM_DQ_DQ_SMT_STATUS_ADDR                                       (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x0000001c)
#define HWIO_DIM_D01_DIM_DQ_DQ_SMT_STATUS_RMSK                                             0xff
#define HWIO_DIM_D01_DIM_DQ_DQ_SMT_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_DQ_SMT_STATUS_ADDR, HWIO_DIM_D01_DIM_DQ_DQ_SMT_STATUS_RMSK)
#define HWIO_DIM_D01_DIM_DQ_DQ_SMT_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_DQ_SMT_STATUS_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_DQ_SMT_STATUS_DIM_DQ_DQ_SMT_STATUS_BMSK                        0xff
#define HWIO_DIM_D01_DIM_DQ_DQ_SMT_STATUS_DIM_DQ_DQ_SMT_STATUS_SHFT                         0x0

#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_ADDR                                            (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000020)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_RMSK                                            0x1fffff33
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_PAD_CFG3_ADDR, HWIO_DIM_D01_DIM_DQ_PAD_CFG3_RMSK)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_PAD_CFG3_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_PAD_CFG3_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_PAD_CFG3_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_PAD_CFG3_IN)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DQS_DIFF_MODE_BMSK                              0x10000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DQS_DIFF_MODE_SHFT                                    0x1c
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_RCW_ODT_ENA1_BMSK                                0x8000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_RCW_ODT_ENA1_SHFT                                     0x1b
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_RCW_ODT_ENA0_BMSK                                0x4000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_RCW_ODT_ENA0_SHFT                                     0x1a
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_RCW_ODT_BMSK                                     0x3000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_RCW_ODT_SHFT                                          0x18
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DQ_ODT_ENA1_BMSK                                  0x800000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DQ_ODT_ENA1_SHFT                                      0x17
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DQ_ODT_ENA0_BMSK                                  0x400000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DQ_ODT_ENA0_SHFT                                      0x16
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DQ_ODT_BMSK                                       0x300000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DQ_ODT_SHFT                                           0x14
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DQS_ODT_ENA1_BMSK                                  0x80000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DQS_ODT_ENA1_SHFT                                     0x13
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DQS_ODT_ENA0_BMSK                                  0x40000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DQS_ODT_ENA0_SHFT                                     0x12
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DQS_ODT_BMSK                                       0x30000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DQS_ODT_SHFT                                          0x10
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DQ_IE_OE_BMSK                                       0xff00
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DQ_IE_OE_SHFT                                          0x8
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_RCW_IE_OE_BMSK                                        0x20
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_RCW_IE_OE_SHFT                                         0x5
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DQS_IE_OE_BMSK                                        0x10
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DQS_IE_OE_SHFT                                         0x4
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DM_IE_BMSK                                             0x2
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DM_IE_SHFT                                             0x1
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DM_OE_BMSK                                             0x1
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG3_DM_OE_SHFT                                             0x0

#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_ADDR                                            (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000024)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RMSK                                            0xe33333f7
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_PAD_CFG4_ADDR, HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RMSK)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_PAD_CFG4_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_PAD_CFG4_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_PAD_CFG4_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_PAD_CFG4_IN)
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_DDR_MODE1_BMSK                              0x80000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_DDR_MODE1_SHFT                                    0x1f
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_DDR_MODE0_BMSK                              0x40000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_DDR_MODE0_SHFT                                    0x1e
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_LV_MODE_BMSK                                0x20000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_LV_MODE_SHFT                                      0x1d
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_PULL_B_BMSK                                  0x3000000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_PULL_B_SHFT                                       0x18
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_NSLEW_BMSK                                    0x300000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_NSLEW_SHFT                                        0x14
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_PSLEW_BMSK                                     0x30000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_PSLEW_SHFT                                        0x10
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_NRXDEL_BMSK                                     0x3000
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_NRXDEL_SHFT                                        0xc
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_PRXDEL_BMSK                                      0x300
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_PRXDEL_SHFT                                        0x8
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_VM_SHIFT_ENA_BMSK                                 0x80
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_VM_SHIFT_ENA_SHFT                                  0x7
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_ROUT_BMSK                                         0x70
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_ROUT_SHFT                                          0x4
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_DCC_BMSK                                           0x7
#define HWIO_DIM_D01_DIM_DQ_PAD_CFG4_RCW_DCC_SHFT                                           0x0

#define HWIO_DIM_D01_DDRPHY_DQ_MISR_CFG_ADDR                                         (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x0000000c)
#define HWIO_DIM_D01_DDRPHY_DQ_MISR_CFG_RMSK                                         0xffffffff
#define HWIO_DIM_D01_DDRPHY_DQ_MISR_CFG_IN          \
        in_dword_masked(HWIO_DIM_D01_DDRPHY_DQ_MISR_CFG_ADDR, HWIO_DIM_D01_DDRPHY_DQ_MISR_CFG_RMSK)
#define HWIO_DIM_D01_DDRPHY_DQ_MISR_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DDRPHY_DQ_MISR_CFG_ADDR, m)
#define HWIO_DIM_D01_DDRPHY_DQ_MISR_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D01_DDRPHY_DQ_MISR_CFG_ADDR,v)
#define HWIO_DIM_D01_DDRPHY_DQ_MISR_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DDRPHY_DQ_MISR_CFG_ADDR,m,v,HWIO_DIM_D01_DDRPHY_DQ_MISR_CFG_IN)
#define HWIO_DIM_D01_DDRPHY_DQ_MISR_CFG_SEED_VALUE_BMSK                              0xffffffff
#define HWIO_DIM_D01_DDRPHY_DQ_MISR_CFG_SEED_VALUE_SHFT                                     0x0

#define HWIO_DIM_D01_DDRPHY_DQ_MISR_EVEN_STATUS_ADDR                                 (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000028)
#define HWIO_DIM_D01_DDRPHY_DQ_MISR_EVEN_STATUS_RMSK                                 0xffffffff
#define HWIO_DIM_D01_DDRPHY_DQ_MISR_EVEN_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D01_DDRPHY_DQ_MISR_EVEN_STATUS_ADDR, HWIO_DIM_D01_DDRPHY_DQ_MISR_EVEN_STATUS_RMSK)
#define HWIO_DIM_D01_DDRPHY_DQ_MISR_EVEN_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DDRPHY_DQ_MISR_EVEN_STATUS_ADDR, m)
#define HWIO_DIM_D01_DDRPHY_DQ_MISR_EVEN_STATUS_SIGNATURE_BMSK                       0xffffffff
#define HWIO_DIM_D01_DDRPHY_DQ_MISR_EVEN_STATUS_SIGNATURE_SHFT                              0x0

#define HWIO_DIM_D01_DDRPHY_DQ_MISR_ODD_STATUS_ADDR                                  (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x0000002c)
#define HWIO_DIM_D01_DDRPHY_DQ_MISR_ODD_STATUS_RMSK                                  0xffffffff
#define HWIO_DIM_D01_DDRPHY_DQ_MISR_ODD_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D01_DDRPHY_DQ_MISR_ODD_STATUS_ADDR, HWIO_DIM_D01_DDRPHY_DQ_MISR_ODD_STATUS_RMSK)
#define HWIO_DIM_D01_DDRPHY_DQ_MISR_ODD_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DDRPHY_DQ_MISR_ODD_STATUS_ADDR, m)
#define HWIO_DIM_D01_DDRPHY_DQ_MISR_ODD_STATUS_SIGNATURE_BMSK                        0xffffffff
#define HWIO_DIM_D01_DDRPHY_DQ_MISR_ODD_STATUS_SIGNATURE_SHFT                               0x0

#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_ADDR                                       (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000030)
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_RMSK                                        0x7ff37ff
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_ADDR, HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_RMSK)
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_IN)
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_ERROR_CODE_UPDATE_EN_BMSK                   0x4000000
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_ERROR_CODE_UPDATE_EN_SHFT                        0x1a
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_STAGGER_CAL_ENA_BMSK                        0x2000000
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_STAGGER_CAL_ENA_SHFT                             0x19
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_TRACKING_CAL_ENA_BMSK                       0x1000000
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_TRACKING_CAL_ENA_SHFT                            0x18
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_OSC_COUNT_ERR_TOLERANCE_BMSK                 0xf00000
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_OSC_COUNT_ERR_TOLERANCE_SHFT                     0x14
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_TRACK_CALIB_MODE_BMSK                         0x80000
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_TRACK_CALIB_MODE_SHFT                            0x13
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_FULL_DELAY_BMSK                               0x40000
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_FULL_DELAY_SHFT                                  0x12
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_HW_AUTOCAL_ENA_BMSK                           0x20000
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_HW_AUTOCAL_ENA_SHFT                              0x11
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_SW_TRIG_FULL_CALIB_BMSK                       0x10000
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_SW_TRIG_FULL_CALIB_SHFT                          0x10
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_OSC_PRE_DIV_BMSK                               0x3000
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_OSC_PRE_DIV_SHFT                                  0xc
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_TMUX_CHAR_BMSK                                  0x7ff
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG0_TMUX_CHAR_SHFT                                    0x0

#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_ADDR                                       (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000034)
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_RMSK                                        0x7f77777
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_ADDR, HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_RMSK)
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_IN)
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_OSC_COUNT_DELAY_BMSK                        0x7000000
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_OSC_COUNT_DELAY_SHFT                             0x18
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_STANDBY_DELAY_BMSK                           0xf00000
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_STANDBY_DELAY_SHFT                               0x14
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_DEL_MODE_DELAY_BMSK                           0x70000
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_DEL_MODE_DELAY_SHFT                              0x10
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_OSC_MODE_DELAY_BMSK                            0x7000
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_OSC_MODE_DELAY_SHFT                               0xc
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_DECODER_DELAY_BMSK                              0x700
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_DECODER_DELAY_SHFT                                0x8
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_DIVIDER_DELAY_BMSK                               0x70
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_DIVIDER_DELAY_SHFT                                0x4
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_MULTIPLIER_DELAY_BMSK                             0x7
#define HWIO_DIM_D01_DIM_DQ_CDC_CTLR_CFG1_MULTIPLIER_DELAY_SHFT                             0x0

#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR                                  (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000038)
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_RMSK                                   0x1f1ffff
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR, HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_RMSK)
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_IN)
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_INVALID_TIMER_ENA_BMSK                 0x1000000
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_INVALID_TIMER_ENA_SHFT                      0x18
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_INVALID_TIMER_VAL_BMSK                  0xf00000
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_INVALID_TIMER_VAL_SHFT                      0x14
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_TIMER_ENA_BMSK                           0x10000
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_TIMER_ENA_SHFT                              0x10
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_TIMER_VAL_BMSK                            0xffff
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG0_TIMER_VAL_SHFT                               0x0

#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR                                  (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x0000003c)
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG1_RMSK                                      0x13ff
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG1_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR, HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG1_RMSK)
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG1_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG1_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG1_IN)
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG1_FF_TIMER_ENA_BMSK                         0x1000
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG1_FF_TIMER_ENA_SHFT                            0xc
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG1_FF_TIMER_VAL_BMSK                          0x3ff
#define HWIO_DIM_D01_DIM_DQ_CDC_CAL_TIMER_CFG1_FF_TIMER_VAL_SHFT                            0x0

#define HWIO_DIM_D01_DIM_DQ_CDC_REFCOUNT_CFG_ADDR                                    (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000040)
#define HWIO_DIM_D01_DIM_DQ_CDC_REFCOUNT_CFG_RMSK                                    0xffff3f3f
#define HWIO_DIM_D01_DIM_DQ_CDC_REFCOUNT_CFG_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_CDC_REFCOUNT_CFG_ADDR, HWIO_DIM_D01_DIM_DQ_CDC_REFCOUNT_CFG_RMSK)
#define HWIO_DIM_D01_DIM_DQ_CDC_REFCOUNT_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_CDC_REFCOUNT_CFG_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_CDC_REFCOUNT_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_CDC_REFCOUNT_CFG_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_CDC_REFCOUNT_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_CDC_REFCOUNT_CFG_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_CDC_REFCOUNT_CFG_IN)
#define HWIO_DIM_D01_DIM_DQ_CDC_REFCOUNT_CFG_TREF_BMSK                               0xffff0000
#define HWIO_DIM_D01_DIM_DQ_CDC_REFCOUNT_CFG_TREF_SHFT                                     0x10
#define HWIO_DIM_D01_DIM_DQ_CDC_REFCOUNT_CFG_CCAL_REF_COUNT_BMSK                         0x3f00
#define HWIO_DIM_D01_DIM_DQ_CDC_REFCOUNT_CFG_CCAL_REF_COUNT_SHFT                            0x8
#define HWIO_DIM_D01_DIM_DQ_CDC_REFCOUNT_CFG_FCAL_REF_COUNT_BMSK                           0x3f
#define HWIO_DIM_D01_DIM_DQ_CDC_REFCOUNT_CFG_FCAL_REF_COUNT_SHFT                            0x0

#define HWIO_DIM_D01_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR                                  (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000044)
#define HWIO_DIM_D01_DIM_DQ_CDC_COARSE_CAL_CFG_RMSK                                      0x1f3f
#define HWIO_DIM_D01_DIM_DQ_CDC_COARSE_CAL_CFG_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR, HWIO_DIM_D01_DIM_DQ_CDC_COARSE_CAL_CFG_RMSK)
#define HWIO_DIM_D01_DIM_DQ_CDC_COARSE_CAL_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_CDC_COARSE_CAL_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_CDC_COARSE_CAL_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_CDC_COARSE_CAL_CFG_IN)
#define HWIO_DIM_D01_DIM_DQ_CDC_COARSE_CAL_CFG_CCAL_SUBUNIT_CAPS_BMSK                    0x1f00
#define HWIO_DIM_D01_DIM_DQ_CDC_COARSE_CAL_CFG_CCAL_SUBUNIT_CAPS_SHFT                       0x8
#define HWIO_DIM_D01_DIM_DQ_CDC_COARSE_CAL_CFG_CCAL_UNITSTEPS_BMSK                         0x3f
#define HWIO_DIM_D01_DIM_DQ_CDC_COARSE_CAL_CFG_CCAL_UNITSTEPS_SHFT                          0x0

#define HWIO_DIM_D01_DIM_DQ_CDC_RSVD_CFG_ADDR                                        (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000048)
#define HWIO_DIM_D01_DIM_DQ_CDC_RSVD_CFG_RMSK                                            0xffff
#define HWIO_DIM_D01_DIM_DQ_CDC_RSVD_CFG_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_CDC_RSVD_CFG_ADDR, HWIO_DIM_D01_DIM_DQ_CDC_RSVD_CFG_RMSK)
#define HWIO_DIM_D01_DIM_DQ_CDC_RSVD_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_CDC_RSVD_CFG_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_CDC_RSVD_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_CDC_RSVD_CFG_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_CDC_RSVD_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_CDC_RSVD_CFG_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_CDC_RSVD_CFG_IN)
#define HWIO_DIM_D01_DIM_DQ_CDC_RSVD_CFG_TEMP_FIELD_BMSK                                 0xffff
#define HWIO_DIM_D01_DIM_DQ_CDC_RSVD_CFG_TEMP_FIELD_SHFT                                    0x0

#define HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR                                   (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x0000004c)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_RMSK                                     0x19f1bf
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR, HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_RMSK)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_IN)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_MODE_BMSK                 0x100000
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_MODE_SHFT                     0x14
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SIGN_BMSK                  0x80000
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SIGN_SHFT                     0x13
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_BMSK                       0x1f000
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SHFT                           0xc
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_MODE_BMSK                       0x100
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_MODE_SHFT                         0x8
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_SIGN_BMSK                        0x80
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_SIGN_SHFT                         0x7
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_BMSK                             0x3f
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_SHFT                              0x0

#define HWIO_DIM_D01_DIM_DQ_RD_CDC_DELAY_CFG_ADDR                                    (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000050)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_DELAY_CFG_RMSK                                    0x1fff0fff
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_DELAY_CFG_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_DELAY_CFG_ADDR, HWIO_DIM_D01_DIM_DQ_RD_CDC_DELAY_CFG_RMSK)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_DELAY_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_DELAY_CFG_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_DELAY_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_RD_CDC_DELAY_CFG_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_DELAY_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_RD_CDC_DELAY_CFG_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_RD_CDC_DELAY_CFG_IN)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_DELAY_CFG_TARGET_COUNT_ENA_BMSK                   0x10000000
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_DELAY_CFG_TARGET_COUNT_ENA_SHFT                         0x1c
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_DELAY_CFG_TARGET_COUNT_BMSK                        0xfff0000
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_DELAY_CFG_TARGET_COUNT_SHFT                             0x10
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_DELAY_CFG_DELAY_VAL_BMSK                               0xfff
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_DELAY_CFG_DELAY_VAL_SHFT                                 0x0

#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR                                  (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000054)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_RMSK                                         0xf
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR, HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_RMSK)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_IN)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_DEL_MODE_BMSK                             0x8
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_DEL_MODE_SHFT                             0x3
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_OSC_MODE_BMSK                             0x4
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_OSC_MODE_SHFT                             0x2
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_DA_SEL_BMSK                               0x2
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_DA_SEL_SHFT                               0x1
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_LOAD_BMSK                                 0x1
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_LOAD_SHFT                                 0x0

#define HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_ADDR                                     (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000058)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_RMSK                                            0xf
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_ADDR, HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_RMSK)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_IN)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_CDC_SEL_DDA_TEST_BMSK                           0x8
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_CDC_SEL_DDA_TEST_SHFT                           0x3
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_CDC_OSC_TEST_EN_BMSK                            0x4
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_CDC_OSC_TEST_EN_SHFT                            0x2
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_CDC_TEST_EN_BMSK                                0x2
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_CDC_TEST_EN_SHFT                                0x1
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_CDC_OUT_ONTEST2_BMSK                            0x1
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_TEST_CFG_CDC_OUT_ONTEST2_SHFT                            0x0

#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR                                  (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x0000005c)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_RMSK                                     0x10f0f
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR, HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_RMSK)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_IN)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_REF_GATE_BMSK                         0x10000
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_REF_GATE_SHFT                            0x10
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA1_OSC_EN_BMSK                    0x800
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA1_OSC_EN_SHFT                      0xb
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA1_IN_EN_BMSK                     0x400
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA1_IN_EN_SHFT                       0xa
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA0_OSC_EN_BMSK                    0x200
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA0_OSC_EN_SHFT                      0x9
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA0_IN_EN_BMSK                     0x100
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA0_IN_EN_SHFT                       0x8
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_CDC_COUNTER_RST_BMSK                 0x8
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_CDC_COUNTER_RST_SHFT                 0x3
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_LOAD_DA_SEL_BMSK                     0x4
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_LOAD_DA_SEL_SHFT                     0x2
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_ACTV_DA_SEL_BMSK                     0x2
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_ACTV_DA_SEL_SHFT                     0x1
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_ENA_BMSK                             0x1
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_ENA_SHFT                             0x0

#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR                                (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000060)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_RMSK                                   0x3f7ff
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR, HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_RMSK)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_IN)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAV_DDA_OFFSET_MODE_BMSK              0x20000
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAV_DDA_OFFSET_MODE_SHFT                 0x11
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAV_DDA_OFFSET_SIGN_BMSK              0x10000
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAV_DDA_OFFSET_SIGN_SHFT                 0x10
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAVE_DDA_OFFSET_BMSK                   0xf000
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAVE_DDA_OFFSET_SHFT                      0xc
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAVE_DDA_DELAY_BMSK                     0x7ff
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAVE_DDA_DELAY_SHFT                       0x0

#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_ADDR                                      (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000070)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_RMSK                                       0x7ffffff
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_ADDR, HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_RMSK)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_CDC_ERROR_CODE_BMSK                        0x7000000
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_CDC_ERROR_CODE_SHFT                             0x18
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_SW_REF_GATE_COUNT_BMSK                      0xff0000
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_SW_REF_GATE_COUNT_SHFT                          0x10
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_OSC_COUNT_BMSK                                0xfff0
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_OSC_COUNT_SHFT                                   0x4
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_CURR_SEL_DA_BMSK                                 0x8
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_CURR_SEL_DA_SHFT                                 0x3
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_CTLR_SM_IDLE_BMSK                                0x4
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_CTLR_SM_IDLE_SHFT                                0x2
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_OSC_DONE_BMSK                                    0x2
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_OSC_DONE_SHFT                                    0x1
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_CALIBRATION_DONE_BMSK                            0x1
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS0_CALIBRATION_DONE_SHFT                            0x0

#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS1_ADDR                                      (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000074)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS1_RMSK                                       0xfff0fff
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS1_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS1_ADDR, HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS1_RMSK)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS1_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS1_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS1_CURR_DELAY_VALUE_BMSK                      0xfff0000
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS1_CURR_DELAY_VALUE_SHFT                           0x10
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS1_CURR_TMUX_DELAY_BMSK                           0xfff
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS1_CURR_TMUX_DELAY_SHFT                             0x0

#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS2_ADDR                                      (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000078)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS2_RMSK                                      0x1f3f1f3f
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS2_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS2_ADDR, HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS2_RMSK)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS2_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS2_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS2_DA1_SUBUNITS_BMSK                         0x1f000000
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS2_DA1_SUBUNITS_SHFT                               0x18
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS2_DA1_UNITSTEPS_BMSK                          0x3f0000
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS2_DA1_UNITSTEPS_SHFT                              0x10
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS2_DA0_SUBUNITS_BMSK                             0x1f00
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS2_DA0_SUBUNITS_SHFT                                0x8
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS2_DA0_UNITSTEPS_BMSK                              0x3f
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS2_DA0_UNITSTEPS_SHFT                               0x0

#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS3_ADDR                                      (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x0000007c)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS3_RMSK                                      0xffffffff
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS3_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS3_ADDR, HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS3_RMSK)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS3_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS3_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS3_NUM_OF_OSC_ITER_BMSK                      0xff000000
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS3_NUM_OF_OSC_ITER_SHFT                            0x18
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS3_COUNT_ERROR_BMSK                            0xfff000
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS3_COUNT_ERROR_SHFT                                 0xc
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS3_CURR_TARGET_COUNT_BMSK                         0xfff
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS3_CURR_TARGET_COUNT_SHFT                           0x0

#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS4_ADDR                                      (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000080)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS4_RMSK                                            0xff
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS4_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS4_ADDR, HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS4_RMSK)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS4_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS4_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS4_SLAVE_DDA_DA1_TAPS_BMSK                         0xf0
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS4_SLAVE_DDA_DA1_TAPS_SHFT                          0x4
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS4_SLAVE_DDA_DA0_TAPS_BMSK                          0xf
#define HWIO_DIM_D01_DIM_DQ_RD_CDC_STATUS4_SLAVE_DDA_DA0_TAPS_SHFT                          0x0

#define HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR                                   (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x000000ac)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_RMSK                                     0x19f1bf
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR, HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_RMSK)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_IN)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_MODE_BMSK                 0x100000
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_MODE_SHFT                     0x14
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SIGN_BMSK                  0x80000
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SIGN_SHFT                     0x13
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_BMSK                       0x1f000
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SHFT                           0xc
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_MODE_BMSK                       0x100
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_MODE_SHFT                         0x8
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_SIGN_BMSK                        0x80
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_SIGN_SHFT                         0x7
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_BMSK                             0x3f
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_SHFT                              0x0

#define HWIO_DIM_D01_DIM_DQ_WR_CDC_DELAY_CFG_ADDR                                    (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x000000b0)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_DELAY_CFG_RMSK                                    0x1fff0fff
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_DELAY_CFG_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_WR_CDC_DELAY_CFG_ADDR, HWIO_DIM_D01_DIM_DQ_WR_CDC_DELAY_CFG_RMSK)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_DELAY_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_WR_CDC_DELAY_CFG_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_DELAY_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_WR_CDC_DELAY_CFG_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_DELAY_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_WR_CDC_DELAY_CFG_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_WR_CDC_DELAY_CFG_IN)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_DELAY_CFG_TARGET_COUNT_ENA_BMSK                   0x10000000
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_DELAY_CFG_TARGET_COUNT_ENA_SHFT                         0x1c
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_DELAY_CFG_TARGET_COUNT_BMSK                        0xfff0000
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_DELAY_CFG_TARGET_COUNT_SHFT                             0x10
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_DELAY_CFG_DELAY_VAL_BMSK                               0xfff
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_DELAY_CFG_DELAY_VAL_SHFT                                 0x0

#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR                                  (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x000000b4)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_RMSK                                         0xf
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR, HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_RMSK)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_IN)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_DEL_MODE_BMSK                             0x8
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_DEL_MODE_SHFT                             0x3
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_OSC_MODE_BMSK                             0x4
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_OSC_MODE_SHFT                             0x2
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_DA_SEL_BMSK                               0x2
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_DA_SEL_SHFT                               0x1
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_LOAD_BMSK                                 0x1
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_LOAD_SHFT                                 0x0

#define HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_ADDR                                     (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x000000b8)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_RMSK                                            0xf
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_ADDR, HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_RMSK)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_IN)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_CDC_SEL_DDA_TEST_BMSK                           0x8
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_CDC_SEL_DDA_TEST_SHFT                           0x3
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_CDC_OSC_TEST_EN_BMSK                            0x4
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_CDC_OSC_TEST_EN_SHFT                            0x2
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_CDC_TEST_EN_BMSK                                0x2
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_CDC_TEST_EN_SHFT                                0x1
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_CDC_OUT_ONTEST2_BMSK                            0x1
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_TEST_CFG_CDC_OUT_ONTEST2_SHFT                            0x0

#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR                                  (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x000000bc)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_RMSK                                     0x10f0f
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR, HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_RMSK)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_IN)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_REF_GATE_BMSK                         0x10000
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_REF_GATE_SHFT                            0x10
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA1_OSC_EN_BMSK                    0x800
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA1_OSC_EN_SHFT                      0xb
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA1_IN_EN_BMSK                     0x400
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA1_IN_EN_SHFT                       0xa
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA0_OSC_EN_BMSK                    0x200
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA0_OSC_EN_SHFT                      0x9
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA0_IN_EN_BMSK                     0x100
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA0_IN_EN_SHFT                       0x8
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_CDC_COUNTER_RST_BMSK                 0x8
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_CDC_COUNTER_RST_SHFT                 0x3
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_LOAD_DA_SEL_BMSK                     0x4
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_LOAD_DA_SEL_SHFT                     0x2
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_ACTV_DA_SEL_BMSK                     0x2
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_ACTV_DA_SEL_SHFT                     0x1
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_ENA_BMSK                             0x1
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_ENA_SHFT                             0x0

#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_ADDR                                      (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x000000d0)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_RMSK                                       0x7ffffff
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_ADDR, HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_RMSK)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_CDC_ERROR_CODE_BMSK                        0x7000000
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_CDC_ERROR_CODE_SHFT                             0x18
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_SW_REF_GATE_COUNT_BMSK                      0xff0000
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_SW_REF_GATE_COUNT_SHFT                          0x10
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_OSC_COUNT_BMSK                                0xfff0
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_OSC_COUNT_SHFT                                   0x4
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_CURR_SEL_DA_BMSK                                 0x8
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_CURR_SEL_DA_SHFT                                 0x3
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_CTLR_SM_IDLE_BMSK                                0x4
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_CTLR_SM_IDLE_SHFT                                0x2
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_OSC_DONE_BMSK                                    0x2
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_OSC_DONE_SHFT                                    0x1
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_CALIBRATION_DONE_BMSK                            0x1
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS0_CALIBRATION_DONE_SHFT                            0x0

#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS1_ADDR                                      (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x000000d4)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS1_RMSK                                       0xfff0fff
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS1_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS1_ADDR, HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS1_RMSK)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS1_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS1_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS1_CURR_DELAY_VALUE_BMSK                      0xfff0000
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS1_CURR_DELAY_VALUE_SHFT                           0x10
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS1_CURR_TMUX_DELAY_BMSK                           0xfff
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS1_CURR_TMUX_DELAY_SHFT                             0x0

#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS2_ADDR                                      (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x000000d8)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS2_RMSK                                      0x1f3f1f3f
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS2_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS2_ADDR, HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS2_RMSK)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS2_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS2_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS2_DA1_SUBUNITS_BMSK                         0x1f000000
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS2_DA1_SUBUNITS_SHFT                               0x18
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS2_DA1_UNITSTEPS_BMSK                          0x3f0000
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS2_DA1_UNITSTEPS_SHFT                              0x10
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS2_DA0_SUBUNITS_BMSK                             0x1f00
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS2_DA0_SUBUNITS_SHFT                                0x8
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS2_DA0_UNITSTEPS_BMSK                              0x3f
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS2_DA0_UNITSTEPS_SHFT                               0x0

#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS3_ADDR                                      (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x000000dc)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS3_RMSK                                      0xffffffff
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS3_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS3_ADDR, HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS3_RMSK)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS3_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS3_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS3_NUM_OF_OSC_ITER_BMSK                      0xff000000
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS3_NUM_OF_OSC_ITER_SHFT                            0x18
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS3_COUNT_ERROR_BMSK                            0xfff000
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS3_COUNT_ERROR_SHFT                                 0xc
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS3_CURR_TARGET_COUNT_BMSK                         0xfff
#define HWIO_DIM_D01_DIM_DQ_WR_CDC_STATUS3_CURR_TARGET_COUNT_SHFT                           0x0

#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_ADDR                                      (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000100)
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_RMSK                                      0x9f1f9f9f
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_ADDR, HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_RMSK)
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_IN)
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_BMSK                     0x80000000
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_SHFT                           0x1f
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_SW_VAL_BMSK                          0x1f000000
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_SW_VAL_SHFT                                0x18
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_SW_VAL_BMSK                            0x1f0000
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_SW_VAL_SHFT                                0x10
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_OFFSET_SIGN_BMSK                         0x8000
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_OFFSET_SIGN_SHFT                            0xf
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_SW_OFFSET_BMSK                           0x1f00
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_SW_OFFSET_SHFT                              0x8
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_OFFSET_SIGN_BMSK                           0x80
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_OFFSET_SIGN_SHFT                            0x7
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_SW_OFFSET_BMSK                             0x1f
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_SW_OFFSET_SHFT                              0x0

#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_STATUS_ADDR                                   (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000104)
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_STATUS_RMSK                                       0x1f1f
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_STATUS_ADDR, HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_STATUS_RMSK)
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_STATUS_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_STATUS_PAD_NCNT_BMSK                              0x1f00
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_STATUS_PAD_NCNT_SHFT                                 0x8
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_STATUS_PAD_PCNT_BMSK                                0x1f
#define HWIO_DIM_D01_DIM_DQ_DQ_IOC_SLV_STATUS_PAD_PCNT_SHFT                                 0x0

#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_ADDR                                     (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000110)
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_RMSK                                     0x9f1f9f9f
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_ADDR, HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_RMSK)
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_ADDR,v)
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_ADDR,m,v,HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_IN)
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_BMSK                    0x80000000
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_SHFT                          0x1f
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_SW_VAL_BMSK                         0x1f000000
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_SW_VAL_SHFT                               0x18
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_SW_VAL_BMSK                           0x1f0000
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_SW_VAL_SHFT                               0x10
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_OFFSET_SIGN_BMSK                        0x8000
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_OFFSET_SIGN_SHFT                           0xf
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_SW_OFFSET_BMSK                          0x1f00
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_SW_OFFSET_SHFT                             0x8
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_OFFSET_SIGN_BMSK                          0x80
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_OFFSET_SIGN_SHFT                           0x7
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_SW_OFFSET_BMSK                            0x1f
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_SW_OFFSET_SHFT                             0x0

#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_STATUS_ADDR                                  (DIM_D01_DDRPHY_DQ_REG_BASE      + 0x00000114)
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_STATUS_RMSK                                      0x1f1f
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_STATUS_ADDR, HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_STATUS_RMSK)
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_STATUS_ADDR, m)
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_STATUS_PAD_NCNT_BMSK                             0x1f00
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_STATUS_PAD_NCNT_SHFT                                0x8
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_STATUS_PAD_PCNT_BMSK                               0x1f
#define HWIO_DIM_D01_DIM_DQ_DQS_IOC_SLV_STATUS_PAD_PCNT_SHFT                                0x0

/*----------------------------------------------------------------------------
 * MODULE: DIM_D02_DDRPHY_DQ
 *--------------------------------------------------------------------------*/

#define DIM_D02_DDRPHY_DQ_REG_BASE                                                   (EBI1_PHY_CFG_BASE      + 0x00001800)

#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_ADDR                                             (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000000)
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_RMSK                                             0x7f013131
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_TOP_CFG_ADDR, HWIO_DIM_D02_DIM_DQ_TOP_CFG_RMSK)
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_TOP_CFG_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_TOP_CFG_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_TOP_CFG_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_TOP_CFG_IN)
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_DQ_LB_MODE_SEL_BMSK                              0x40000000
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_DQ_LB_MODE_SEL_SHFT                                    0x1e
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_DQ_LB_MODE_EN_BMSK                               0x20000000
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_DQ_LB_MODE_EN_SHFT                                     0x1d
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_MISR_EN_OVRD_BMSK                                0x10000000
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_MISR_EN_OVRD_SHFT                                      0x1c
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_MISR_DATA_CLK_SEL_BMSK                            0x8000000
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_MISR_DATA_CLK_SEL_SHFT                                 0x1b
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_CDC_LDO_EN_BMSK                                   0x4000000
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_CDC_LDO_EN_SHFT                                        0x1a
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_CDC_SWITCH_RC_EN_BMSK                             0x2000000
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_CDC_SWITCH_RC_EN_SHFT                                  0x19
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_CDC_SWITCH_BYPASS_OFF_BMSK                        0x1000000
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_CDC_SWITCH_BYPASS_OFF_SHFT                             0x18
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_RCW_EN_BMSK                                         0x10000
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_RCW_EN_SHFT                                            0x10
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_DEBUG_BUS_SEL_BMSK                                   0x2000
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_DEBUG_BUS_SEL_SHFT                                      0xd
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_DEBUG_BUS_EN_BMSK                                    0x1000
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_DEBUG_BUS_EN_SHFT                                       0xc
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_CDC_TEST_EN_BMSK                                      0x100
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_CDC_TEST_EN_SHFT                                        0x8
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_WR_PIPE_EXTEND1_BMSK                                   0x20
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_WR_PIPE_EXTEND1_SHFT                                    0x5
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_WR_PIPE_EXTEND0_BMSK                                   0x10
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_WR_PIPE_EXTEND0_SHFT                                    0x4
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_WR_CLK_SEL_BMSK                                         0x1
#define HWIO_DIM_D02_DIM_DQ_TOP_CFG_WR_CLK_SEL_SHFT                                         0x0

#define HWIO_DIM_D02_DIM_DQ_HW_INFO_ADDR                                             (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000004)
#define HWIO_DIM_D02_DIM_DQ_HW_INFO_RMSK                                                0xfffff
#define HWIO_DIM_D02_DIM_DQ_HW_INFO_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_HW_INFO_ADDR, HWIO_DIM_D02_DIM_DQ_HW_INFO_RMSK)
#define HWIO_DIM_D02_DIM_DQ_HW_INFO_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_HW_INFO_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_HW_INFO_CORE_ID_BMSK                                        0xf0000
#define HWIO_DIM_D02_DIM_DQ_HW_INFO_CORE_ID_SHFT                                           0x10
#define HWIO_DIM_D02_DIM_DQ_HW_INFO_MAJOR_REV_BMSK                                       0xf000
#define HWIO_DIM_D02_DIM_DQ_HW_INFO_MAJOR_REV_SHFT                                          0xc
#define HWIO_DIM_D02_DIM_DQ_HW_INFO_BRANCH_REV_BMSK                                       0xf00
#define HWIO_DIM_D02_DIM_DQ_HW_INFO_BRANCH_REV_SHFT                                         0x8
#define HWIO_DIM_D02_DIM_DQ_HW_INFO_MINOR_REV_BMSK                                         0xff
#define HWIO_DIM_D02_DIM_DQ_HW_INFO_MINOR_REV_SHFT                                          0x0

#define HWIO_DIM_D02_DIM_DQ_HW_VERSION_ADDR                                          (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000008)
#define HWIO_DIM_D02_DIM_DQ_HW_VERSION_RMSK                                          0xffffffff
#define HWIO_DIM_D02_DIM_DQ_HW_VERSION_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_HW_VERSION_ADDR, HWIO_DIM_D02_DIM_DQ_HW_VERSION_RMSK)
#define HWIO_DIM_D02_DIM_DQ_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_HW_VERSION_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_HW_VERSION_MAJOR_BMSK                                    0xf0000000
#define HWIO_DIM_D02_DIM_DQ_HW_VERSION_MAJOR_SHFT                                          0x1c
#define HWIO_DIM_D02_DIM_DQ_HW_VERSION_MINOR_BMSK                                     0xfff0000
#define HWIO_DIM_D02_DIM_DQ_HW_VERSION_MINOR_SHFT                                          0x10
#define HWIO_DIM_D02_DIM_DQ_HW_VERSION_STEP_BMSK                                         0xffff
#define HWIO_DIM_D02_DIM_DQ_HW_VERSION_STEP_SHFT                                            0x0

#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_ADDR                                            (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000010)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_RMSK                                            0xff3333f7
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_PAD_CFG0_ADDR, HWIO_DIM_D02_DIM_DQ_PAD_CFG0_RMSK)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_PAD_CFG0_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_PAD_CFG0_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_PAD_CFG0_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_PAD_CFG0_IN)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_DDR_MODE1_BMSK                               0x80000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_DDR_MODE1_SHFT                                     0x1f
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_DDR_MODE0_BMSK                               0x40000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_DDR_MODE0_SHFT                                     0x1e
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_LV_MODE_BMSK                                 0x20000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_LV_MODE_SHFT                                       0x1d
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_ODT_ENA_BMSK                                 0x10000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_ODT_ENA_SHFT                                       0x1c
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_ODT_BMSK                                      0xc000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_ODT_SHFT                                           0x1a
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_PULL_B_BMSK                                   0x3000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_PULL_B_SHFT                                        0x18
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_NSLEW_BMSK                                     0x300000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_NSLEW_SHFT                                         0x14
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_PSLEW_BMSK                                      0x30000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_PSLEW_SHFT                                         0x10
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_NRXDEL_BMSK                                      0x3000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_NRXDEL_SHFT                                         0xc
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_PRXDEL_BMSK                                       0x300
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_PRXDEL_SHFT                                         0x8
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_VM_SHIFT_ENA_BMSK                                  0x80
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_VM_SHIFT_ENA_SHFT                                   0x7
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_ROUT_BMSK                                          0x70
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_ROUT_SHFT                                           0x4
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_DCC_BMSK                                            0x7
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG0_DQ_DCC_SHFT                                            0x0

#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_ADDR                                            (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000014)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_RMSK                                            0xff3333f7
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_PAD_CFG1_ADDR, HWIO_DIM_D02_DIM_DQ_PAD_CFG1_RMSK)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_PAD_CFG1_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_PAD_CFG1_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_PAD_CFG1_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_PAD_CFG1_IN)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_DDR_MODE1_BMSK                              0x80000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_DDR_MODE1_SHFT                                    0x1f
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_DDR_MODE0_BMSK                              0x40000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_DDR_MODE0_SHFT                                    0x1e
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_LV_MODE_BMSK                                0x20000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_LV_MODE_SHFT                                      0x1d
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_ODT_ENA_BMSK                                0x10000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_ODT_ENA_SHFT                                      0x1c
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_ODT_BMSK                                     0xc000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_ODT_SHFT                                          0x1a
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_PULL_BMSK                                    0x3000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_PULL_SHFT                                         0x18
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_NSLEW_BMSK                                    0x300000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_NSLEW_SHFT                                        0x14
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_PSLEW_BMSK                                     0x30000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_PSLEW_SHFT                                        0x10
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_NRXDEL_BMSK                                     0x3000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_NRXDEL_SHFT                                        0xc
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_PRXDEL_BMSK                                      0x300
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_PRXDEL_SHFT                                        0x8
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_VM_SHIFT_ENA_BMSK                                 0x80
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_VM_SHIFT_ENA_SHFT                                  0x7
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_ROUT_BMSK                                         0x70
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_ROUT_SHFT                                          0x4
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_DCC_BMSK                                           0x7
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG1_DQS_DCC_SHFT                                           0x0

#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_ADDR                                            (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000018)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_RMSK                                            0x3f3f3f3f
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_PAD_CFG2_ADDR, HWIO_DIM_D02_DIM_DQ_PAD_CFG2_RMSK)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_PAD_CFG2_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_PAD_CFG2_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_PAD_CFG2_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_PAD_CFG2_IN)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_VREF_LDO_ENA_BMSK                               0x20000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_VREF_LDO_ENA_SHFT                                     0x1d
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_VREF_PASSGATE_ENA_BMSK                          0x10000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_VREF_PASSGATE_ENA_SHFT                                0x1c
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_VREF_SP_OUT_BMSK                                 0xf000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_VREF_SP_OUT_SHFT                                      0x18
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_VREF_SP_IN_BMSK                                   0x3c0000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_VREF_SP_IN_SHFT                                       0x12
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_VREF_BYPASS_ENA_BMSK                               0x20000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_VREF_BYPASS_ENA_SHFT                                  0x11
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_VREF_RDIV_ENA_BMSK                                 0x10000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_VREF_RDIV_ENA_SHFT                                    0x10
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_VREF_PULLDN_SPEED_CNTL_BMSK                         0x3f00
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_VREF_PULLDN_SPEED_CNTL_SHFT                            0x8
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_VREF_LEVEL_CNTL_BMSK                                  0x3f
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG2_VREF_LEVEL_CNTL_SHFT                                   0x0

#define HWIO_DIM_D02_DIM_DQ_DQ_SMT_STATUS_ADDR                                       (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x0000001c)
#define HWIO_DIM_D02_DIM_DQ_DQ_SMT_STATUS_RMSK                                             0xff
#define HWIO_DIM_D02_DIM_DQ_DQ_SMT_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_DQ_SMT_STATUS_ADDR, HWIO_DIM_D02_DIM_DQ_DQ_SMT_STATUS_RMSK)
#define HWIO_DIM_D02_DIM_DQ_DQ_SMT_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_DQ_SMT_STATUS_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_DQ_SMT_STATUS_DIM_DQ_DQ_SMT_STATUS_BMSK                        0xff
#define HWIO_DIM_D02_DIM_DQ_DQ_SMT_STATUS_DIM_DQ_DQ_SMT_STATUS_SHFT                         0x0

#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_ADDR                                            (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000020)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_RMSK                                            0x1fffff33
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_PAD_CFG3_ADDR, HWIO_DIM_D02_DIM_DQ_PAD_CFG3_RMSK)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_PAD_CFG3_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_PAD_CFG3_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_PAD_CFG3_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_PAD_CFG3_IN)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DQS_DIFF_MODE_BMSK                              0x10000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DQS_DIFF_MODE_SHFT                                    0x1c
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_RCW_ODT_ENA1_BMSK                                0x8000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_RCW_ODT_ENA1_SHFT                                     0x1b
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_RCW_ODT_ENA0_BMSK                                0x4000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_RCW_ODT_ENA0_SHFT                                     0x1a
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_RCW_ODT_BMSK                                     0x3000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_RCW_ODT_SHFT                                          0x18
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DQ_ODT_ENA1_BMSK                                  0x800000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DQ_ODT_ENA1_SHFT                                      0x17
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DQ_ODT_ENA0_BMSK                                  0x400000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DQ_ODT_ENA0_SHFT                                      0x16
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DQ_ODT_BMSK                                       0x300000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DQ_ODT_SHFT                                           0x14
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DQS_ODT_ENA1_BMSK                                  0x80000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DQS_ODT_ENA1_SHFT                                     0x13
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DQS_ODT_ENA0_BMSK                                  0x40000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DQS_ODT_ENA0_SHFT                                     0x12
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DQS_ODT_BMSK                                       0x30000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DQS_ODT_SHFT                                          0x10
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DQ_IE_OE_BMSK                                       0xff00
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DQ_IE_OE_SHFT                                          0x8
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_RCW_IE_OE_BMSK                                        0x20
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_RCW_IE_OE_SHFT                                         0x5
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DQS_IE_OE_BMSK                                        0x10
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DQS_IE_OE_SHFT                                         0x4
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DM_IE_BMSK                                             0x2
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DM_IE_SHFT                                             0x1
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DM_OE_BMSK                                             0x1
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG3_DM_OE_SHFT                                             0x0

#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_ADDR                                            (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000024)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RMSK                                            0xe33333f7
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_PAD_CFG4_ADDR, HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RMSK)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_PAD_CFG4_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_PAD_CFG4_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_PAD_CFG4_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_PAD_CFG4_IN)
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_DDR_MODE1_BMSK                              0x80000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_DDR_MODE1_SHFT                                    0x1f
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_DDR_MODE0_BMSK                              0x40000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_DDR_MODE0_SHFT                                    0x1e
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_LV_MODE_BMSK                                0x20000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_LV_MODE_SHFT                                      0x1d
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_PULL_B_BMSK                                  0x3000000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_PULL_B_SHFT                                       0x18
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_NSLEW_BMSK                                    0x300000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_NSLEW_SHFT                                        0x14
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_PSLEW_BMSK                                     0x30000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_PSLEW_SHFT                                        0x10
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_NRXDEL_BMSK                                     0x3000
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_NRXDEL_SHFT                                        0xc
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_PRXDEL_BMSK                                      0x300
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_PRXDEL_SHFT                                        0x8
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_VM_SHIFT_ENA_BMSK                                 0x80
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_VM_SHIFT_ENA_SHFT                                  0x7
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_ROUT_BMSK                                         0x70
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_ROUT_SHFT                                          0x4
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_DCC_BMSK                                           0x7
#define HWIO_DIM_D02_DIM_DQ_PAD_CFG4_RCW_DCC_SHFT                                           0x0

#define HWIO_DIM_D02_DDRPHY_DQ_MISR_CFG_ADDR                                         (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x0000000c)
#define HWIO_DIM_D02_DDRPHY_DQ_MISR_CFG_RMSK                                         0xffffffff
#define HWIO_DIM_D02_DDRPHY_DQ_MISR_CFG_IN          \
        in_dword_masked(HWIO_DIM_D02_DDRPHY_DQ_MISR_CFG_ADDR, HWIO_DIM_D02_DDRPHY_DQ_MISR_CFG_RMSK)
#define HWIO_DIM_D02_DDRPHY_DQ_MISR_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DDRPHY_DQ_MISR_CFG_ADDR, m)
#define HWIO_DIM_D02_DDRPHY_DQ_MISR_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D02_DDRPHY_DQ_MISR_CFG_ADDR,v)
#define HWIO_DIM_D02_DDRPHY_DQ_MISR_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DDRPHY_DQ_MISR_CFG_ADDR,m,v,HWIO_DIM_D02_DDRPHY_DQ_MISR_CFG_IN)
#define HWIO_DIM_D02_DDRPHY_DQ_MISR_CFG_SEED_VALUE_BMSK                              0xffffffff
#define HWIO_DIM_D02_DDRPHY_DQ_MISR_CFG_SEED_VALUE_SHFT                                     0x0

#define HWIO_DIM_D02_DDRPHY_DQ_MISR_EVEN_STATUS_ADDR                                 (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000028)
#define HWIO_DIM_D02_DDRPHY_DQ_MISR_EVEN_STATUS_RMSK                                 0xffffffff
#define HWIO_DIM_D02_DDRPHY_DQ_MISR_EVEN_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D02_DDRPHY_DQ_MISR_EVEN_STATUS_ADDR, HWIO_DIM_D02_DDRPHY_DQ_MISR_EVEN_STATUS_RMSK)
#define HWIO_DIM_D02_DDRPHY_DQ_MISR_EVEN_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DDRPHY_DQ_MISR_EVEN_STATUS_ADDR, m)
#define HWIO_DIM_D02_DDRPHY_DQ_MISR_EVEN_STATUS_SIGNATURE_BMSK                       0xffffffff
#define HWIO_DIM_D02_DDRPHY_DQ_MISR_EVEN_STATUS_SIGNATURE_SHFT                              0x0

#define HWIO_DIM_D02_DDRPHY_DQ_MISR_ODD_STATUS_ADDR                                  (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x0000002c)
#define HWIO_DIM_D02_DDRPHY_DQ_MISR_ODD_STATUS_RMSK                                  0xffffffff
#define HWIO_DIM_D02_DDRPHY_DQ_MISR_ODD_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D02_DDRPHY_DQ_MISR_ODD_STATUS_ADDR, HWIO_DIM_D02_DDRPHY_DQ_MISR_ODD_STATUS_RMSK)
#define HWIO_DIM_D02_DDRPHY_DQ_MISR_ODD_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DDRPHY_DQ_MISR_ODD_STATUS_ADDR, m)
#define HWIO_DIM_D02_DDRPHY_DQ_MISR_ODD_STATUS_SIGNATURE_BMSK                        0xffffffff
#define HWIO_DIM_D02_DDRPHY_DQ_MISR_ODD_STATUS_SIGNATURE_SHFT                               0x0

#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_ADDR                                       (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000030)
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_RMSK                                        0x7ff37ff
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_ADDR, HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_RMSK)
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_IN)
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_ERROR_CODE_UPDATE_EN_BMSK                   0x4000000
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_ERROR_CODE_UPDATE_EN_SHFT                        0x1a
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_STAGGER_CAL_ENA_BMSK                        0x2000000
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_STAGGER_CAL_ENA_SHFT                             0x19
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_TRACKING_CAL_ENA_BMSK                       0x1000000
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_TRACKING_CAL_ENA_SHFT                            0x18
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_OSC_COUNT_ERR_TOLERANCE_BMSK                 0xf00000
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_OSC_COUNT_ERR_TOLERANCE_SHFT                     0x14
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_TRACK_CALIB_MODE_BMSK                         0x80000
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_TRACK_CALIB_MODE_SHFT                            0x13
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_FULL_DELAY_BMSK                               0x40000
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_FULL_DELAY_SHFT                                  0x12
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_HW_AUTOCAL_ENA_BMSK                           0x20000
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_HW_AUTOCAL_ENA_SHFT                              0x11
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_SW_TRIG_FULL_CALIB_BMSK                       0x10000
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_SW_TRIG_FULL_CALIB_SHFT                          0x10
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_OSC_PRE_DIV_BMSK                               0x3000
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_OSC_PRE_DIV_SHFT                                  0xc
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_TMUX_CHAR_BMSK                                  0x7ff
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG0_TMUX_CHAR_SHFT                                    0x0

#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_ADDR                                       (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000034)
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_RMSK                                        0x7f77777
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_ADDR, HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_RMSK)
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_IN)
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_OSC_COUNT_DELAY_BMSK                        0x7000000
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_OSC_COUNT_DELAY_SHFT                             0x18
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_STANDBY_DELAY_BMSK                           0xf00000
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_STANDBY_DELAY_SHFT                               0x14
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_DEL_MODE_DELAY_BMSK                           0x70000
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_DEL_MODE_DELAY_SHFT                              0x10
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_OSC_MODE_DELAY_BMSK                            0x7000
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_OSC_MODE_DELAY_SHFT                               0xc
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_DECODER_DELAY_BMSK                              0x700
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_DECODER_DELAY_SHFT                                0x8
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_DIVIDER_DELAY_BMSK                               0x70
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_DIVIDER_DELAY_SHFT                                0x4
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_MULTIPLIER_DELAY_BMSK                             0x7
#define HWIO_DIM_D02_DIM_DQ_CDC_CTLR_CFG1_MULTIPLIER_DELAY_SHFT                             0x0

#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR                                  (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000038)
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_RMSK                                   0x1f1ffff
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR, HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_RMSK)
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_IN)
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_INVALID_TIMER_ENA_BMSK                 0x1000000
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_INVALID_TIMER_ENA_SHFT                      0x18
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_INVALID_TIMER_VAL_BMSK                  0xf00000
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_INVALID_TIMER_VAL_SHFT                      0x14
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_TIMER_ENA_BMSK                           0x10000
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_TIMER_ENA_SHFT                              0x10
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_TIMER_VAL_BMSK                            0xffff
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG0_TIMER_VAL_SHFT                               0x0

#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR                                  (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x0000003c)
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG1_RMSK                                      0x13ff
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG1_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR, HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG1_RMSK)
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG1_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG1_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG1_IN)
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG1_FF_TIMER_ENA_BMSK                         0x1000
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG1_FF_TIMER_ENA_SHFT                            0xc
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG1_FF_TIMER_VAL_BMSK                          0x3ff
#define HWIO_DIM_D02_DIM_DQ_CDC_CAL_TIMER_CFG1_FF_TIMER_VAL_SHFT                            0x0

#define HWIO_DIM_D02_DIM_DQ_CDC_REFCOUNT_CFG_ADDR                                    (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000040)
#define HWIO_DIM_D02_DIM_DQ_CDC_REFCOUNT_CFG_RMSK                                    0xffff3f3f
#define HWIO_DIM_D02_DIM_DQ_CDC_REFCOUNT_CFG_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_CDC_REFCOUNT_CFG_ADDR, HWIO_DIM_D02_DIM_DQ_CDC_REFCOUNT_CFG_RMSK)
#define HWIO_DIM_D02_DIM_DQ_CDC_REFCOUNT_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_CDC_REFCOUNT_CFG_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_CDC_REFCOUNT_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_CDC_REFCOUNT_CFG_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_CDC_REFCOUNT_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_CDC_REFCOUNT_CFG_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_CDC_REFCOUNT_CFG_IN)
#define HWIO_DIM_D02_DIM_DQ_CDC_REFCOUNT_CFG_TREF_BMSK                               0xffff0000
#define HWIO_DIM_D02_DIM_DQ_CDC_REFCOUNT_CFG_TREF_SHFT                                     0x10
#define HWIO_DIM_D02_DIM_DQ_CDC_REFCOUNT_CFG_CCAL_REF_COUNT_BMSK                         0x3f00
#define HWIO_DIM_D02_DIM_DQ_CDC_REFCOUNT_CFG_CCAL_REF_COUNT_SHFT                            0x8
#define HWIO_DIM_D02_DIM_DQ_CDC_REFCOUNT_CFG_FCAL_REF_COUNT_BMSK                           0x3f
#define HWIO_DIM_D02_DIM_DQ_CDC_REFCOUNT_CFG_FCAL_REF_COUNT_SHFT                            0x0

#define HWIO_DIM_D02_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR                                  (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000044)
#define HWIO_DIM_D02_DIM_DQ_CDC_COARSE_CAL_CFG_RMSK                                      0x1f3f
#define HWIO_DIM_D02_DIM_DQ_CDC_COARSE_CAL_CFG_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR, HWIO_DIM_D02_DIM_DQ_CDC_COARSE_CAL_CFG_RMSK)
#define HWIO_DIM_D02_DIM_DQ_CDC_COARSE_CAL_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_CDC_COARSE_CAL_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_CDC_COARSE_CAL_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_CDC_COARSE_CAL_CFG_IN)
#define HWIO_DIM_D02_DIM_DQ_CDC_COARSE_CAL_CFG_CCAL_SUBUNIT_CAPS_BMSK                    0x1f00
#define HWIO_DIM_D02_DIM_DQ_CDC_COARSE_CAL_CFG_CCAL_SUBUNIT_CAPS_SHFT                       0x8
#define HWIO_DIM_D02_DIM_DQ_CDC_COARSE_CAL_CFG_CCAL_UNITSTEPS_BMSK                         0x3f
#define HWIO_DIM_D02_DIM_DQ_CDC_COARSE_CAL_CFG_CCAL_UNITSTEPS_SHFT                          0x0

#define HWIO_DIM_D02_DIM_DQ_CDC_RSVD_CFG_ADDR                                        (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000048)
#define HWIO_DIM_D02_DIM_DQ_CDC_RSVD_CFG_RMSK                                            0xffff
#define HWIO_DIM_D02_DIM_DQ_CDC_RSVD_CFG_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_CDC_RSVD_CFG_ADDR, HWIO_DIM_D02_DIM_DQ_CDC_RSVD_CFG_RMSK)
#define HWIO_DIM_D02_DIM_DQ_CDC_RSVD_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_CDC_RSVD_CFG_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_CDC_RSVD_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_CDC_RSVD_CFG_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_CDC_RSVD_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_CDC_RSVD_CFG_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_CDC_RSVD_CFG_IN)
#define HWIO_DIM_D02_DIM_DQ_CDC_RSVD_CFG_TEMP_FIELD_BMSK                                 0xffff
#define HWIO_DIM_D02_DIM_DQ_CDC_RSVD_CFG_TEMP_FIELD_SHFT                                    0x0

#define HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR                                   (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x0000004c)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_RMSK                                     0x19f1bf
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR, HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_RMSK)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_IN)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_MODE_BMSK                 0x100000
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_MODE_SHFT                     0x14
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SIGN_BMSK                  0x80000
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SIGN_SHFT                     0x13
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_BMSK                       0x1f000
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SHFT                           0xc
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_MODE_BMSK                       0x100
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_MODE_SHFT                         0x8
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_SIGN_BMSK                        0x80
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_SIGN_SHFT                         0x7
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_BMSK                             0x3f
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_SHFT                              0x0

#define HWIO_DIM_D02_DIM_DQ_RD_CDC_DELAY_CFG_ADDR                                    (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000050)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_DELAY_CFG_RMSK                                    0x1fff0fff
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_DELAY_CFG_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_DELAY_CFG_ADDR, HWIO_DIM_D02_DIM_DQ_RD_CDC_DELAY_CFG_RMSK)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_DELAY_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_DELAY_CFG_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_DELAY_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_RD_CDC_DELAY_CFG_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_DELAY_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_RD_CDC_DELAY_CFG_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_RD_CDC_DELAY_CFG_IN)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_DELAY_CFG_TARGET_COUNT_ENA_BMSK                   0x10000000
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_DELAY_CFG_TARGET_COUNT_ENA_SHFT                         0x1c
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_DELAY_CFG_TARGET_COUNT_BMSK                        0xfff0000
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_DELAY_CFG_TARGET_COUNT_SHFT                             0x10
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_DELAY_CFG_DELAY_VAL_BMSK                               0xfff
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_DELAY_CFG_DELAY_VAL_SHFT                                 0x0

#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR                                  (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000054)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_RMSK                                         0xf
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR, HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_RMSK)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_IN)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_DEL_MODE_BMSK                             0x8
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_DEL_MODE_SHFT                             0x3
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_OSC_MODE_BMSK                             0x4
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_OSC_MODE_SHFT                             0x2
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_DA_SEL_BMSK                               0x2
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_DA_SEL_SHFT                               0x1
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_LOAD_BMSK                                 0x1
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_LOAD_SHFT                                 0x0

#define HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_ADDR                                     (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000058)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_RMSK                                            0xf
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_ADDR, HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_RMSK)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_IN)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_CDC_SEL_DDA_TEST_BMSK                           0x8
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_CDC_SEL_DDA_TEST_SHFT                           0x3
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_CDC_OSC_TEST_EN_BMSK                            0x4
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_CDC_OSC_TEST_EN_SHFT                            0x2
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_CDC_TEST_EN_BMSK                                0x2
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_CDC_TEST_EN_SHFT                                0x1
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_CDC_OUT_ONTEST2_BMSK                            0x1
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_TEST_CFG_CDC_OUT_ONTEST2_SHFT                            0x0

#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR                                  (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x0000005c)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_RMSK                                     0x10f0f
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR, HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_RMSK)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_IN)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_REF_GATE_BMSK                         0x10000
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_REF_GATE_SHFT                            0x10
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA1_OSC_EN_BMSK                    0x800
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA1_OSC_EN_SHFT                      0xb
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA1_IN_EN_BMSK                     0x400
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA1_IN_EN_SHFT                       0xa
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA0_OSC_EN_BMSK                    0x200
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA0_OSC_EN_SHFT                      0x9
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA0_IN_EN_BMSK                     0x100
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA0_IN_EN_SHFT                       0x8
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_CDC_COUNTER_RST_BMSK                 0x8
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_CDC_COUNTER_RST_SHFT                 0x3
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_LOAD_DA_SEL_BMSK                     0x4
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_LOAD_DA_SEL_SHFT                     0x2
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_ACTV_DA_SEL_BMSK                     0x2
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_ACTV_DA_SEL_SHFT                     0x1
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_ENA_BMSK                             0x1
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_ENA_SHFT                             0x0

#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR                                (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000060)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_RMSK                                   0x3f7ff
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR, HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_RMSK)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_IN)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAV_DDA_OFFSET_MODE_BMSK              0x20000
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAV_DDA_OFFSET_MODE_SHFT                 0x11
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAV_DDA_OFFSET_SIGN_BMSK              0x10000
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAV_DDA_OFFSET_SIGN_SHFT                 0x10
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAVE_DDA_OFFSET_BMSK                   0xf000
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAVE_DDA_OFFSET_SHFT                      0xc
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAVE_DDA_DELAY_BMSK                     0x7ff
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAVE_DDA_DELAY_SHFT                       0x0

#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_ADDR                                      (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000070)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_RMSK                                       0x7ffffff
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_ADDR, HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_RMSK)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_CDC_ERROR_CODE_BMSK                        0x7000000
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_CDC_ERROR_CODE_SHFT                             0x18
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_SW_REF_GATE_COUNT_BMSK                      0xff0000
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_SW_REF_GATE_COUNT_SHFT                          0x10
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_OSC_COUNT_BMSK                                0xfff0
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_OSC_COUNT_SHFT                                   0x4
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_CURR_SEL_DA_BMSK                                 0x8
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_CURR_SEL_DA_SHFT                                 0x3
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_CTLR_SM_IDLE_BMSK                                0x4
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_CTLR_SM_IDLE_SHFT                                0x2
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_OSC_DONE_BMSK                                    0x2
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_OSC_DONE_SHFT                                    0x1
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_CALIBRATION_DONE_BMSK                            0x1
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS0_CALIBRATION_DONE_SHFT                            0x0

#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS1_ADDR                                      (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000074)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS1_RMSK                                       0xfff0fff
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS1_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS1_ADDR, HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS1_RMSK)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS1_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS1_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS1_CURR_DELAY_VALUE_BMSK                      0xfff0000
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS1_CURR_DELAY_VALUE_SHFT                           0x10
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS1_CURR_TMUX_DELAY_BMSK                           0xfff
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS1_CURR_TMUX_DELAY_SHFT                             0x0

#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS2_ADDR                                      (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000078)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS2_RMSK                                      0x1f3f1f3f
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS2_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS2_ADDR, HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS2_RMSK)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS2_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS2_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS2_DA1_SUBUNITS_BMSK                         0x1f000000
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS2_DA1_SUBUNITS_SHFT                               0x18
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS2_DA1_UNITSTEPS_BMSK                          0x3f0000
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS2_DA1_UNITSTEPS_SHFT                              0x10
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS2_DA0_SUBUNITS_BMSK                             0x1f00
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS2_DA0_SUBUNITS_SHFT                                0x8
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS2_DA0_UNITSTEPS_BMSK                              0x3f
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS2_DA0_UNITSTEPS_SHFT                               0x0

#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS3_ADDR                                      (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x0000007c)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS3_RMSK                                      0xffffffff
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS3_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS3_ADDR, HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS3_RMSK)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS3_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS3_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS3_NUM_OF_OSC_ITER_BMSK                      0xff000000
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS3_NUM_OF_OSC_ITER_SHFT                            0x18
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS3_COUNT_ERROR_BMSK                            0xfff000
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS3_COUNT_ERROR_SHFT                                 0xc
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS3_CURR_TARGET_COUNT_BMSK                         0xfff
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS3_CURR_TARGET_COUNT_SHFT                           0x0

#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS4_ADDR                                      (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000080)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS4_RMSK                                            0xff
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS4_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS4_ADDR, HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS4_RMSK)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS4_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS4_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS4_SLAVE_DDA_DA1_TAPS_BMSK                         0xf0
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS4_SLAVE_DDA_DA1_TAPS_SHFT                          0x4
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS4_SLAVE_DDA_DA0_TAPS_BMSK                          0xf
#define HWIO_DIM_D02_DIM_DQ_RD_CDC_STATUS4_SLAVE_DDA_DA0_TAPS_SHFT                          0x0

#define HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR                                   (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x000000ac)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_RMSK                                     0x19f1bf
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR, HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_RMSK)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_IN)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_MODE_BMSK                 0x100000
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_MODE_SHFT                     0x14
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SIGN_BMSK                  0x80000
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SIGN_SHFT                     0x13
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_BMSK                       0x1f000
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SHFT                           0xc
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_MODE_BMSK                       0x100
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_MODE_SHFT                         0x8
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_SIGN_BMSK                        0x80
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_SIGN_SHFT                         0x7
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_BMSK                             0x3f
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_SHFT                              0x0

#define HWIO_DIM_D02_DIM_DQ_WR_CDC_DELAY_CFG_ADDR                                    (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x000000b0)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_DELAY_CFG_RMSK                                    0x1fff0fff
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_DELAY_CFG_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_WR_CDC_DELAY_CFG_ADDR, HWIO_DIM_D02_DIM_DQ_WR_CDC_DELAY_CFG_RMSK)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_DELAY_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_WR_CDC_DELAY_CFG_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_DELAY_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_WR_CDC_DELAY_CFG_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_DELAY_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_WR_CDC_DELAY_CFG_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_WR_CDC_DELAY_CFG_IN)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_DELAY_CFG_TARGET_COUNT_ENA_BMSK                   0x10000000
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_DELAY_CFG_TARGET_COUNT_ENA_SHFT                         0x1c
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_DELAY_CFG_TARGET_COUNT_BMSK                        0xfff0000
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_DELAY_CFG_TARGET_COUNT_SHFT                             0x10
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_DELAY_CFG_DELAY_VAL_BMSK                               0xfff
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_DELAY_CFG_DELAY_VAL_SHFT                                 0x0

#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR                                  (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x000000b4)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_RMSK                                         0xf
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR, HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_RMSK)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_IN)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_DEL_MODE_BMSK                             0x8
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_DEL_MODE_SHFT                             0x3
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_OSC_MODE_BMSK                             0x4
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_OSC_MODE_SHFT                             0x2
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_DA_SEL_BMSK                               0x2
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_DA_SEL_SHFT                               0x1
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_LOAD_BMSK                                 0x1
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_LOAD_SHFT                                 0x0

#define HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_ADDR                                     (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x000000b8)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_RMSK                                            0xf
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_ADDR, HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_RMSK)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_IN)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_CDC_SEL_DDA_TEST_BMSK                           0x8
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_CDC_SEL_DDA_TEST_SHFT                           0x3
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_CDC_OSC_TEST_EN_BMSK                            0x4
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_CDC_OSC_TEST_EN_SHFT                            0x2
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_CDC_TEST_EN_BMSK                                0x2
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_CDC_TEST_EN_SHFT                                0x1
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_CDC_OUT_ONTEST2_BMSK                            0x1
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_TEST_CFG_CDC_OUT_ONTEST2_SHFT                            0x0

#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR                                  (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x000000bc)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_RMSK                                     0x10f0f
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR, HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_RMSK)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_IN)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_REF_GATE_BMSK                         0x10000
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_REF_GATE_SHFT                            0x10
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA1_OSC_EN_BMSK                    0x800
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA1_OSC_EN_SHFT                      0xb
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA1_IN_EN_BMSK                     0x400
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA1_IN_EN_SHFT                       0xa
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA0_OSC_EN_BMSK                    0x200
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA0_OSC_EN_SHFT                      0x9
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA0_IN_EN_BMSK                     0x100
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA0_IN_EN_SHFT                       0x8
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_CDC_COUNTER_RST_BMSK                 0x8
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_CDC_COUNTER_RST_SHFT                 0x3
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_LOAD_DA_SEL_BMSK                     0x4
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_LOAD_DA_SEL_SHFT                     0x2
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_ACTV_DA_SEL_BMSK                     0x2
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_ACTV_DA_SEL_SHFT                     0x1
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_ENA_BMSK                             0x1
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_ENA_SHFT                             0x0

#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_ADDR                                      (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x000000d0)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_RMSK                                       0x7ffffff
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_ADDR, HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_RMSK)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_CDC_ERROR_CODE_BMSK                        0x7000000
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_CDC_ERROR_CODE_SHFT                             0x18
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_SW_REF_GATE_COUNT_BMSK                      0xff0000
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_SW_REF_GATE_COUNT_SHFT                          0x10
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_OSC_COUNT_BMSK                                0xfff0
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_OSC_COUNT_SHFT                                   0x4
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_CURR_SEL_DA_BMSK                                 0x8
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_CURR_SEL_DA_SHFT                                 0x3
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_CTLR_SM_IDLE_BMSK                                0x4
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_CTLR_SM_IDLE_SHFT                                0x2
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_OSC_DONE_BMSK                                    0x2
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_OSC_DONE_SHFT                                    0x1
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_CALIBRATION_DONE_BMSK                            0x1
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS0_CALIBRATION_DONE_SHFT                            0x0

#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS1_ADDR                                      (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x000000d4)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS1_RMSK                                       0xfff0fff
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS1_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS1_ADDR, HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS1_RMSK)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS1_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS1_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS1_CURR_DELAY_VALUE_BMSK                      0xfff0000
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS1_CURR_DELAY_VALUE_SHFT                           0x10
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS1_CURR_TMUX_DELAY_BMSK                           0xfff
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS1_CURR_TMUX_DELAY_SHFT                             0x0

#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS2_ADDR                                      (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x000000d8)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS2_RMSK                                      0x1f3f1f3f
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS2_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS2_ADDR, HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS2_RMSK)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS2_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS2_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS2_DA1_SUBUNITS_BMSK                         0x1f000000
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS2_DA1_SUBUNITS_SHFT                               0x18
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS2_DA1_UNITSTEPS_BMSK                          0x3f0000
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS2_DA1_UNITSTEPS_SHFT                              0x10
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS2_DA0_SUBUNITS_BMSK                             0x1f00
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS2_DA0_SUBUNITS_SHFT                                0x8
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS2_DA0_UNITSTEPS_BMSK                              0x3f
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS2_DA0_UNITSTEPS_SHFT                               0x0

#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS3_ADDR                                      (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x000000dc)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS3_RMSK                                      0xffffffff
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS3_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS3_ADDR, HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS3_RMSK)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS3_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS3_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS3_NUM_OF_OSC_ITER_BMSK                      0xff000000
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS3_NUM_OF_OSC_ITER_SHFT                            0x18
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS3_COUNT_ERROR_BMSK                            0xfff000
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS3_COUNT_ERROR_SHFT                                 0xc
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS3_CURR_TARGET_COUNT_BMSK                         0xfff
#define HWIO_DIM_D02_DIM_DQ_WR_CDC_STATUS3_CURR_TARGET_COUNT_SHFT                           0x0

#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_ADDR                                      (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000100)
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_RMSK                                      0x9f1f9f9f
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_ADDR, HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_RMSK)
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_IN)
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_BMSK                     0x80000000
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_SHFT                           0x1f
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_SW_VAL_BMSK                          0x1f000000
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_SW_VAL_SHFT                                0x18
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_SW_VAL_BMSK                            0x1f0000
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_SW_VAL_SHFT                                0x10
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_OFFSET_SIGN_BMSK                         0x8000
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_OFFSET_SIGN_SHFT                            0xf
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_SW_OFFSET_BMSK                           0x1f00
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_SW_OFFSET_SHFT                              0x8
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_OFFSET_SIGN_BMSK                           0x80
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_OFFSET_SIGN_SHFT                            0x7
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_SW_OFFSET_BMSK                             0x1f
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_SW_OFFSET_SHFT                              0x0

#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_STATUS_ADDR                                   (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000104)
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_STATUS_RMSK                                       0x1f1f
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_STATUS_ADDR, HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_STATUS_RMSK)
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_STATUS_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_STATUS_PAD_NCNT_BMSK                              0x1f00
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_STATUS_PAD_NCNT_SHFT                                 0x8
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_STATUS_PAD_PCNT_BMSK                                0x1f
#define HWIO_DIM_D02_DIM_DQ_DQ_IOC_SLV_STATUS_PAD_PCNT_SHFT                                 0x0

#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_ADDR                                     (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000110)
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_RMSK                                     0x9f1f9f9f
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_ADDR, HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_RMSK)
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_ADDR,v)
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_ADDR,m,v,HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_IN)
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_BMSK                    0x80000000
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_SHFT                          0x1f
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_SW_VAL_BMSK                         0x1f000000
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_SW_VAL_SHFT                               0x18
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_SW_VAL_BMSK                           0x1f0000
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_SW_VAL_SHFT                               0x10
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_OFFSET_SIGN_BMSK                        0x8000
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_OFFSET_SIGN_SHFT                           0xf
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_SW_OFFSET_BMSK                          0x1f00
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_SW_OFFSET_SHFT                             0x8
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_OFFSET_SIGN_BMSK                          0x80
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_OFFSET_SIGN_SHFT                           0x7
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_SW_OFFSET_BMSK                            0x1f
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_SW_OFFSET_SHFT                             0x0

#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_STATUS_ADDR                                  (DIM_D02_DDRPHY_DQ_REG_BASE      + 0x00000114)
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_STATUS_RMSK                                      0x1f1f
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_STATUS_ADDR, HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_STATUS_RMSK)
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_STATUS_ADDR, m)
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_STATUS_PAD_NCNT_BMSK                             0x1f00
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_STATUS_PAD_NCNT_SHFT                                0x8
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_STATUS_PAD_PCNT_BMSK                               0x1f
#define HWIO_DIM_D02_DIM_DQ_DQS_IOC_SLV_STATUS_PAD_PCNT_SHFT                                0x0

/*----------------------------------------------------------------------------
 * MODULE: DIM_D03_DDRPHY_DQ
 *--------------------------------------------------------------------------*/

#define DIM_D03_DDRPHY_DQ_REG_BASE                                                   (EBI1_PHY_CFG_BASE      + 0x00002000)

#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_ADDR                                             (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000000)
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_RMSK                                             0x7f013131
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_TOP_CFG_ADDR, HWIO_DIM_D03_DIM_DQ_TOP_CFG_RMSK)
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_TOP_CFG_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_TOP_CFG_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_TOP_CFG_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_TOP_CFG_IN)
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_DQ_LB_MODE_SEL_BMSK                              0x40000000
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_DQ_LB_MODE_SEL_SHFT                                    0x1e
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_DQ_LB_MODE_EN_BMSK                               0x20000000
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_DQ_LB_MODE_EN_SHFT                                     0x1d
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_MISR_EN_OVRD_BMSK                                0x10000000
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_MISR_EN_OVRD_SHFT                                      0x1c
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_MISR_DATA_CLK_SEL_BMSK                            0x8000000
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_MISR_DATA_CLK_SEL_SHFT                                 0x1b
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_CDC_LDO_EN_BMSK                                   0x4000000
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_CDC_LDO_EN_SHFT                                        0x1a
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_CDC_SWITCH_RC_EN_BMSK                             0x2000000
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_CDC_SWITCH_RC_EN_SHFT                                  0x19
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_CDC_SWITCH_BYPASS_OFF_BMSK                        0x1000000
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_CDC_SWITCH_BYPASS_OFF_SHFT                             0x18
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_RCW_EN_BMSK                                         0x10000
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_RCW_EN_SHFT                                            0x10
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_DEBUG_BUS_SEL_BMSK                                   0x2000
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_DEBUG_BUS_SEL_SHFT                                      0xd
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_DEBUG_BUS_EN_BMSK                                    0x1000
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_DEBUG_BUS_EN_SHFT                                       0xc
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_CDC_TEST_EN_BMSK                                      0x100
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_CDC_TEST_EN_SHFT                                        0x8
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_WR_PIPE_EXTEND1_BMSK                                   0x20
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_WR_PIPE_EXTEND1_SHFT                                    0x5
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_WR_PIPE_EXTEND0_BMSK                                   0x10
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_WR_PIPE_EXTEND0_SHFT                                    0x4
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_WR_CLK_SEL_BMSK                                         0x1
#define HWIO_DIM_D03_DIM_DQ_TOP_CFG_WR_CLK_SEL_SHFT                                         0x0

#define HWIO_DIM_D03_DIM_DQ_HW_INFO_ADDR                                             (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000004)
#define HWIO_DIM_D03_DIM_DQ_HW_INFO_RMSK                                                0xfffff
#define HWIO_DIM_D03_DIM_DQ_HW_INFO_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_HW_INFO_ADDR, HWIO_DIM_D03_DIM_DQ_HW_INFO_RMSK)
#define HWIO_DIM_D03_DIM_DQ_HW_INFO_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_HW_INFO_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_HW_INFO_CORE_ID_BMSK                                        0xf0000
#define HWIO_DIM_D03_DIM_DQ_HW_INFO_CORE_ID_SHFT                                           0x10
#define HWIO_DIM_D03_DIM_DQ_HW_INFO_MAJOR_REV_BMSK                                       0xf000
#define HWIO_DIM_D03_DIM_DQ_HW_INFO_MAJOR_REV_SHFT                                          0xc
#define HWIO_DIM_D03_DIM_DQ_HW_INFO_BRANCH_REV_BMSK                                       0xf00
#define HWIO_DIM_D03_DIM_DQ_HW_INFO_BRANCH_REV_SHFT                                         0x8
#define HWIO_DIM_D03_DIM_DQ_HW_INFO_MINOR_REV_BMSK                                         0xff
#define HWIO_DIM_D03_DIM_DQ_HW_INFO_MINOR_REV_SHFT                                          0x0

#define HWIO_DIM_D03_DIM_DQ_HW_VERSION_ADDR                                          (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000008)
#define HWIO_DIM_D03_DIM_DQ_HW_VERSION_RMSK                                          0xffffffff
#define HWIO_DIM_D03_DIM_DQ_HW_VERSION_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_HW_VERSION_ADDR, HWIO_DIM_D03_DIM_DQ_HW_VERSION_RMSK)
#define HWIO_DIM_D03_DIM_DQ_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_HW_VERSION_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_HW_VERSION_MAJOR_BMSK                                    0xf0000000
#define HWIO_DIM_D03_DIM_DQ_HW_VERSION_MAJOR_SHFT                                          0x1c
#define HWIO_DIM_D03_DIM_DQ_HW_VERSION_MINOR_BMSK                                     0xfff0000
#define HWIO_DIM_D03_DIM_DQ_HW_VERSION_MINOR_SHFT                                          0x10
#define HWIO_DIM_D03_DIM_DQ_HW_VERSION_STEP_BMSK                                         0xffff
#define HWIO_DIM_D03_DIM_DQ_HW_VERSION_STEP_SHFT                                            0x0

#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_ADDR                                            (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000010)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_RMSK                                            0xff3333f7
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_PAD_CFG0_ADDR, HWIO_DIM_D03_DIM_DQ_PAD_CFG0_RMSK)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_PAD_CFG0_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_PAD_CFG0_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_PAD_CFG0_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_PAD_CFG0_IN)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_DDR_MODE1_BMSK                               0x80000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_DDR_MODE1_SHFT                                     0x1f
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_DDR_MODE0_BMSK                               0x40000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_DDR_MODE0_SHFT                                     0x1e
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_LV_MODE_BMSK                                 0x20000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_LV_MODE_SHFT                                       0x1d
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_ODT_ENA_BMSK                                 0x10000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_ODT_ENA_SHFT                                       0x1c
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_ODT_BMSK                                      0xc000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_ODT_SHFT                                           0x1a
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_PULL_B_BMSK                                   0x3000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_PULL_B_SHFT                                        0x18
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_NSLEW_BMSK                                     0x300000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_NSLEW_SHFT                                         0x14
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_PSLEW_BMSK                                      0x30000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_PSLEW_SHFT                                         0x10
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_NRXDEL_BMSK                                      0x3000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_NRXDEL_SHFT                                         0xc
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_PRXDEL_BMSK                                       0x300
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_PRXDEL_SHFT                                         0x8
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_VM_SHIFT_ENA_BMSK                                  0x80
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_VM_SHIFT_ENA_SHFT                                   0x7
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_ROUT_BMSK                                          0x70
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_ROUT_SHFT                                           0x4
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_DCC_BMSK                                            0x7
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG0_DQ_DCC_SHFT                                            0x0

#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_ADDR                                            (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000014)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_RMSK                                            0xff3333f7
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_PAD_CFG1_ADDR, HWIO_DIM_D03_DIM_DQ_PAD_CFG1_RMSK)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_PAD_CFG1_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_PAD_CFG1_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_PAD_CFG1_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_PAD_CFG1_IN)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_DDR_MODE1_BMSK                              0x80000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_DDR_MODE1_SHFT                                    0x1f
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_DDR_MODE0_BMSK                              0x40000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_DDR_MODE0_SHFT                                    0x1e
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_LV_MODE_BMSK                                0x20000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_LV_MODE_SHFT                                      0x1d
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_ODT_ENA_BMSK                                0x10000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_ODT_ENA_SHFT                                      0x1c
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_ODT_BMSK                                     0xc000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_ODT_SHFT                                          0x1a
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_PULL_BMSK                                    0x3000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_PULL_SHFT                                         0x18
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_NSLEW_BMSK                                    0x300000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_NSLEW_SHFT                                        0x14
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_PSLEW_BMSK                                     0x30000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_PSLEW_SHFT                                        0x10
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_NRXDEL_BMSK                                     0x3000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_NRXDEL_SHFT                                        0xc
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_PRXDEL_BMSK                                      0x300
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_PRXDEL_SHFT                                        0x8
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_VM_SHIFT_ENA_BMSK                                 0x80
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_VM_SHIFT_ENA_SHFT                                  0x7
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_ROUT_BMSK                                         0x70
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_ROUT_SHFT                                          0x4
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_DCC_BMSK                                           0x7
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG1_DQS_DCC_SHFT                                           0x0

#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_ADDR                                            (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000018)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_RMSK                                            0x3f3f3f3f
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_PAD_CFG2_ADDR, HWIO_DIM_D03_DIM_DQ_PAD_CFG2_RMSK)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_PAD_CFG2_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_PAD_CFG2_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_PAD_CFG2_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_PAD_CFG2_IN)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_VREF_LDO_ENA_BMSK                               0x20000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_VREF_LDO_ENA_SHFT                                     0x1d
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_VREF_PASSGATE_ENA_BMSK                          0x10000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_VREF_PASSGATE_ENA_SHFT                                0x1c
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_VREF_SP_OUT_BMSK                                 0xf000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_VREF_SP_OUT_SHFT                                      0x18
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_VREF_SP_IN_BMSK                                   0x3c0000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_VREF_SP_IN_SHFT                                       0x12
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_VREF_BYPASS_ENA_BMSK                               0x20000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_VREF_BYPASS_ENA_SHFT                                  0x11
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_VREF_RDIV_ENA_BMSK                                 0x10000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_VREF_RDIV_ENA_SHFT                                    0x10
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_VREF_PULLDN_SPEED_CNTL_BMSK                         0x3f00
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_VREF_PULLDN_SPEED_CNTL_SHFT                            0x8
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_VREF_LEVEL_CNTL_BMSK                                  0x3f
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG2_VREF_LEVEL_CNTL_SHFT                                   0x0

#define HWIO_DIM_D03_DIM_DQ_DQ_SMT_STATUS_ADDR                                       (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x0000001c)
#define HWIO_DIM_D03_DIM_DQ_DQ_SMT_STATUS_RMSK                                             0xff
#define HWIO_DIM_D03_DIM_DQ_DQ_SMT_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_DQ_SMT_STATUS_ADDR, HWIO_DIM_D03_DIM_DQ_DQ_SMT_STATUS_RMSK)
#define HWIO_DIM_D03_DIM_DQ_DQ_SMT_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_DQ_SMT_STATUS_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_DQ_SMT_STATUS_DIM_DQ_DQ_SMT_STATUS_BMSK                        0xff
#define HWIO_DIM_D03_DIM_DQ_DQ_SMT_STATUS_DIM_DQ_DQ_SMT_STATUS_SHFT                         0x0

#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_ADDR                                            (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000020)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_RMSK                                            0x1fffff33
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_PAD_CFG3_ADDR, HWIO_DIM_D03_DIM_DQ_PAD_CFG3_RMSK)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_PAD_CFG3_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_PAD_CFG3_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_PAD_CFG3_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_PAD_CFG3_IN)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DQS_DIFF_MODE_BMSK                              0x10000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DQS_DIFF_MODE_SHFT                                    0x1c
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_RCW_ODT_ENA1_BMSK                                0x8000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_RCW_ODT_ENA1_SHFT                                     0x1b
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_RCW_ODT_ENA0_BMSK                                0x4000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_RCW_ODT_ENA0_SHFT                                     0x1a
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_RCW_ODT_BMSK                                     0x3000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_RCW_ODT_SHFT                                          0x18
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DQ_ODT_ENA1_BMSK                                  0x800000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DQ_ODT_ENA1_SHFT                                      0x17
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DQ_ODT_ENA0_BMSK                                  0x400000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DQ_ODT_ENA0_SHFT                                      0x16
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DQ_ODT_BMSK                                       0x300000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DQ_ODT_SHFT                                           0x14
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DQS_ODT_ENA1_BMSK                                  0x80000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DQS_ODT_ENA1_SHFT                                     0x13
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DQS_ODT_ENA0_BMSK                                  0x40000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DQS_ODT_ENA0_SHFT                                     0x12
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DQS_ODT_BMSK                                       0x30000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DQS_ODT_SHFT                                          0x10
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DQ_IE_OE_BMSK                                       0xff00
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DQ_IE_OE_SHFT                                          0x8
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_RCW_IE_OE_BMSK                                        0x20
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_RCW_IE_OE_SHFT                                         0x5
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DQS_IE_OE_BMSK                                        0x10
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DQS_IE_OE_SHFT                                         0x4
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DM_IE_BMSK                                             0x2
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DM_IE_SHFT                                             0x1
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DM_OE_BMSK                                             0x1
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG3_DM_OE_SHFT                                             0x0

#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_ADDR                                            (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000024)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RMSK                                            0xe33333f7
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_PAD_CFG4_ADDR, HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RMSK)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_PAD_CFG4_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_PAD_CFG4_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_PAD_CFG4_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_PAD_CFG4_IN)
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_DDR_MODE1_BMSK                              0x80000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_DDR_MODE1_SHFT                                    0x1f
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_DDR_MODE0_BMSK                              0x40000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_DDR_MODE0_SHFT                                    0x1e
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_LV_MODE_BMSK                                0x20000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_LV_MODE_SHFT                                      0x1d
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_PULL_B_BMSK                                  0x3000000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_PULL_B_SHFT                                       0x18
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_NSLEW_BMSK                                    0x300000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_NSLEW_SHFT                                        0x14
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_PSLEW_BMSK                                     0x30000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_PSLEW_SHFT                                        0x10
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_NRXDEL_BMSK                                     0x3000
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_NRXDEL_SHFT                                        0xc
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_PRXDEL_BMSK                                      0x300
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_PRXDEL_SHFT                                        0x8
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_VM_SHIFT_ENA_BMSK                                 0x80
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_VM_SHIFT_ENA_SHFT                                  0x7
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_ROUT_BMSK                                         0x70
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_ROUT_SHFT                                          0x4
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_DCC_BMSK                                           0x7
#define HWIO_DIM_D03_DIM_DQ_PAD_CFG4_RCW_DCC_SHFT                                           0x0

#define HWIO_DIM_D03_DDRPHY_DQ_MISR_CFG_ADDR                                         (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x0000000c)
#define HWIO_DIM_D03_DDRPHY_DQ_MISR_CFG_RMSK                                         0xffffffff
#define HWIO_DIM_D03_DDRPHY_DQ_MISR_CFG_IN          \
        in_dword_masked(HWIO_DIM_D03_DDRPHY_DQ_MISR_CFG_ADDR, HWIO_DIM_D03_DDRPHY_DQ_MISR_CFG_RMSK)
#define HWIO_DIM_D03_DDRPHY_DQ_MISR_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DDRPHY_DQ_MISR_CFG_ADDR, m)
#define HWIO_DIM_D03_DDRPHY_DQ_MISR_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D03_DDRPHY_DQ_MISR_CFG_ADDR,v)
#define HWIO_DIM_D03_DDRPHY_DQ_MISR_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DDRPHY_DQ_MISR_CFG_ADDR,m,v,HWIO_DIM_D03_DDRPHY_DQ_MISR_CFG_IN)
#define HWIO_DIM_D03_DDRPHY_DQ_MISR_CFG_SEED_VALUE_BMSK                              0xffffffff
#define HWIO_DIM_D03_DDRPHY_DQ_MISR_CFG_SEED_VALUE_SHFT                                     0x0

#define HWIO_DIM_D03_DDRPHY_DQ_MISR_EVEN_STATUS_ADDR                                 (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000028)
#define HWIO_DIM_D03_DDRPHY_DQ_MISR_EVEN_STATUS_RMSK                                 0xffffffff
#define HWIO_DIM_D03_DDRPHY_DQ_MISR_EVEN_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D03_DDRPHY_DQ_MISR_EVEN_STATUS_ADDR, HWIO_DIM_D03_DDRPHY_DQ_MISR_EVEN_STATUS_RMSK)
#define HWIO_DIM_D03_DDRPHY_DQ_MISR_EVEN_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DDRPHY_DQ_MISR_EVEN_STATUS_ADDR, m)
#define HWIO_DIM_D03_DDRPHY_DQ_MISR_EVEN_STATUS_SIGNATURE_BMSK                       0xffffffff
#define HWIO_DIM_D03_DDRPHY_DQ_MISR_EVEN_STATUS_SIGNATURE_SHFT                              0x0

#define HWIO_DIM_D03_DDRPHY_DQ_MISR_ODD_STATUS_ADDR                                  (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x0000002c)
#define HWIO_DIM_D03_DDRPHY_DQ_MISR_ODD_STATUS_RMSK                                  0xffffffff
#define HWIO_DIM_D03_DDRPHY_DQ_MISR_ODD_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D03_DDRPHY_DQ_MISR_ODD_STATUS_ADDR, HWIO_DIM_D03_DDRPHY_DQ_MISR_ODD_STATUS_RMSK)
#define HWIO_DIM_D03_DDRPHY_DQ_MISR_ODD_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DDRPHY_DQ_MISR_ODD_STATUS_ADDR, m)
#define HWIO_DIM_D03_DDRPHY_DQ_MISR_ODD_STATUS_SIGNATURE_BMSK                        0xffffffff
#define HWIO_DIM_D03_DDRPHY_DQ_MISR_ODD_STATUS_SIGNATURE_SHFT                               0x0

#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_ADDR                                       (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000030)
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_RMSK                                        0x7ff37ff
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_ADDR, HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_RMSK)
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_IN)
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_ERROR_CODE_UPDATE_EN_BMSK                   0x4000000
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_ERROR_CODE_UPDATE_EN_SHFT                        0x1a
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_STAGGER_CAL_ENA_BMSK                        0x2000000
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_STAGGER_CAL_ENA_SHFT                             0x19
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_TRACKING_CAL_ENA_BMSK                       0x1000000
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_TRACKING_CAL_ENA_SHFT                            0x18
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_OSC_COUNT_ERR_TOLERANCE_BMSK                 0xf00000
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_OSC_COUNT_ERR_TOLERANCE_SHFT                     0x14
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_TRACK_CALIB_MODE_BMSK                         0x80000
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_TRACK_CALIB_MODE_SHFT                            0x13
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_FULL_DELAY_BMSK                               0x40000
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_FULL_DELAY_SHFT                                  0x12
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_HW_AUTOCAL_ENA_BMSK                           0x20000
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_HW_AUTOCAL_ENA_SHFT                              0x11
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_SW_TRIG_FULL_CALIB_BMSK                       0x10000
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_SW_TRIG_FULL_CALIB_SHFT                          0x10
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_OSC_PRE_DIV_BMSK                               0x3000
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_OSC_PRE_DIV_SHFT                                  0xc
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_TMUX_CHAR_BMSK                                  0x7ff
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG0_TMUX_CHAR_SHFT                                    0x0

#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_ADDR                                       (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000034)
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_RMSK                                        0x7f77777
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_ADDR, HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_RMSK)
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_IN)
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_OSC_COUNT_DELAY_BMSK                        0x7000000
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_OSC_COUNT_DELAY_SHFT                             0x18
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_STANDBY_DELAY_BMSK                           0xf00000
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_STANDBY_DELAY_SHFT                               0x14
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_DEL_MODE_DELAY_BMSK                           0x70000
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_DEL_MODE_DELAY_SHFT                              0x10
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_OSC_MODE_DELAY_BMSK                            0x7000
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_OSC_MODE_DELAY_SHFT                               0xc
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_DECODER_DELAY_BMSK                              0x700
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_DECODER_DELAY_SHFT                                0x8
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_DIVIDER_DELAY_BMSK                               0x70
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_DIVIDER_DELAY_SHFT                                0x4
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_MULTIPLIER_DELAY_BMSK                             0x7
#define HWIO_DIM_D03_DIM_DQ_CDC_CTLR_CFG1_MULTIPLIER_DELAY_SHFT                             0x0

#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR                                  (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000038)
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_RMSK                                   0x1f1ffff
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR, HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_RMSK)
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_IN)
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_INVALID_TIMER_ENA_BMSK                 0x1000000
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_INVALID_TIMER_ENA_SHFT                      0x18
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_INVALID_TIMER_VAL_BMSK                  0xf00000
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_INVALID_TIMER_VAL_SHFT                      0x14
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_TIMER_ENA_BMSK                           0x10000
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_TIMER_ENA_SHFT                              0x10
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_TIMER_VAL_BMSK                            0xffff
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG0_TIMER_VAL_SHFT                               0x0

#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR                                  (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x0000003c)
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG1_RMSK                                      0x13ff
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG1_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR, HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG1_RMSK)
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG1_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG1_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG1_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG1_IN)
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG1_FF_TIMER_ENA_BMSK                         0x1000
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG1_FF_TIMER_ENA_SHFT                            0xc
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG1_FF_TIMER_VAL_BMSK                          0x3ff
#define HWIO_DIM_D03_DIM_DQ_CDC_CAL_TIMER_CFG1_FF_TIMER_VAL_SHFT                            0x0

#define HWIO_DIM_D03_DIM_DQ_CDC_REFCOUNT_CFG_ADDR                                    (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000040)
#define HWIO_DIM_D03_DIM_DQ_CDC_REFCOUNT_CFG_RMSK                                    0xffff3f3f
#define HWIO_DIM_D03_DIM_DQ_CDC_REFCOUNT_CFG_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_CDC_REFCOUNT_CFG_ADDR, HWIO_DIM_D03_DIM_DQ_CDC_REFCOUNT_CFG_RMSK)
#define HWIO_DIM_D03_DIM_DQ_CDC_REFCOUNT_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_CDC_REFCOUNT_CFG_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_CDC_REFCOUNT_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_CDC_REFCOUNT_CFG_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_CDC_REFCOUNT_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_CDC_REFCOUNT_CFG_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_CDC_REFCOUNT_CFG_IN)
#define HWIO_DIM_D03_DIM_DQ_CDC_REFCOUNT_CFG_TREF_BMSK                               0xffff0000
#define HWIO_DIM_D03_DIM_DQ_CDC_REFCOUNT_CFG_TREF_SHFT                                     0x10
#define HWIO_DIM_D03_DIM_DQ_CDC_REFCOUNT_CFG_CCAL_REF_COUNT_BMSK                         0x3f00
#define HWIO_DIM_D03_DIM_DQ_CDC_REFCOUNT_CFG_CCAL_REF_COUNT_SHFT                            0x8
#define HWIO_DIM_D03_DIM_DQ_CDC_REFCOUNT_CFG_FCAL_REF_COUNT_BMSK                           0x3f
#define HWIO_DIM_D03_DIM_DQ_CDC_REFCOUNT_CFG_FCAL_REF_COUNT_SHFT                            0x0

#define HWIO_DIM_D03_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR                                  (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000044)
#define HWIO_DIM_D03_DIM_DQ_CDC_COARSE_CAL_CFG_RMSK                                      0x1f3f
#define HWIO_DIM_D03_DIM_DQ_CDC_COARSE_CAL_CFG_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR, HWIO_DIM_D03_DIM_DQ_CDC_COARSE_CAL_CFG_RMSK)
#define HWIO_DIM_D03_DIM_DQ_CDC_COARSE_CAL_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_CDC_COARSE_CAL_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_CDC_COARSE_CAL_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_CDC_COARSE_CAL_CFG_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_CDC_COARSE_CAL_CFG_IN)
#define HWIO_DIM_D03_DIM_DQ_CDC_COARSE_CAL_CFG_CCAL_SUBUNIT_CAPS_BMSK                    0x1f00
#define HWIO_DIM_D03_DIM_DQ_CDC_COARSE_CAL_CFG_CCAL_SUBUNIT_CAPS_SHFT                       0x8
#define HWIO_DIM_D03_DIM_DQ_CDC_COARSE_CAL_CFG_CCAL_UNITSTEPS_BMSK                         0x3f
#define HWIO_DIM_D03_DIM_DQ_CDC_COARSE_CAL_CFG_CCAL_UNITSTEPS_SHFT                          0x0

#define HWIO_DIM_D03_DIM_DQ_CDC_RSVD_CFG_ADDR                                        (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000048)
#define HWIO_DIM_D03_DIM_DQ_CDC_RSVD_CFG_RMSK                                            0xffff
#define HWIO_DIM_D03_DIM_DQ_CDC_RSVD_CFG_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_CDC_RSVD_CFG_ADDR, HWIO_DIM_D03_DIM_DQ_CDC_RSVD_CFG_RMSK)
#define HWIO_DIM_D03_DIM_DQ_CDC_RSVD_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_CDC_RSVD_CFG_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_CDC_RSVD_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_CDC_RSVD_CFG_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_CDC_RSVD_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_CDC_RSVD_CFG_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_CDC_RSVD_CFG_IN)
#define HWIO_DIM_D03_DIM_DQ_CDC_RSVD_CFG_TEMP_FIELD_BMSK                                 0xffff
#define HWIO_DIM_D03_DIM_DQ_CDC_RSVD_CFG_TEMP_FIELD_SHFT                                    0x0

#define HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR                                   (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x0000004c)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_RMSK                                     0x19f1bf
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR, HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_RMSK)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_IN)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_MODE_BMSK                 0x100000
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_MODE_SHFT                     0x14
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SIGN_BMSK                  0x80000
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SIGN_SHFT                     0x13
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_BMSK                       0x1f000
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SHFT                           0xc
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_MODE_BMSK                       0x100
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_MODE_SHFT                         0x8
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_SIGN_BMSK                        0x80
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_SIGN_SHFT                         0x7
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_BMSK                             0x3f
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_OFFSET_CFG_UNIT_OFFSET_SHFT                              0x0

#define HWIO_DIM_D03_DIM_DQ_RD_CDC_DELAY_CFG_ADDR                                    (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000050)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_DELAY_CFG_RMSK                                    0x1fff0fff
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_DELAY_CFG_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_DELAY_CFG_ADDR, HWIO_DIM_D03_DIM_DQ_RD_CDC_DELAY_CFG_RMSK)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_DELAY_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_DELAY_CFG_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_DELAY_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_RD_CDC_DELAY_CFG_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_DELAY_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_RD_CDC_DELAY_CFG_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_RD_CDC_DELAY_CFG_IN)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_DELAY_CFG_TARGET_COUNT_ENA_BMSK                   0x10000000
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_DELAY_CFG_TARGET_COUNT_ENA_SHFT                         0x1c
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_DELAY_CFG_TARGET_COUNT_BMSK                        0xfff0000
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_DELAY_CFG_TARGET_COUNT_SHFT                             0x10
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_DELAY_CFG_DELAY_VAL_BMSK                               0xfff
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_DELAY_CFG_DELAY_VAL_SHFT                                 0x0

#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR                                  (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000054)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_RMSK                                         0xf
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR, HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_RMSK)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_IN)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_DEL_MODE_BMSK                             0x8
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_DEL_MODE_SHFT                             0x3
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_OSC_MODE_BMSK                             0x4
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_OSC_MODE_SHFT                             0x2
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_DA_SEL_BMSK                               0x2
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_DA_SEL_SHFT                               0x1
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_LOAD_BMSK                                 0x1
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_MODE_CFG_SW_LOAD_SHFT                                 0x0

#define HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_ADDR                                     (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000058)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_RMSK                                            0xf
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_ADDR, HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_RMSK)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_IN)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_CDC_SEL_DDA_TEST_BMSK                           0x8
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_CDC_SEL_DDA_TEST_SHFT                           0x3
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_CDC_OSC_TEST_EN_BMSK                            0x4
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_CDC_OSC_TEST_EN_SHFT                            0x2
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_CDC_TEST_EN_BMSK                                0x2
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_CDC_TEST_EN_SHFT                                0x1
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_CDC_OUT_ONTEST2_BMSK                            0x1
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_TEST_CFG_CDC_OUT_ONTEST2_SHFT                            0x0

#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR                                  (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x0000005c)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_RMSK                                     0x10f0f
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR, HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_RMSK)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_IN)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_REF_GATE_BMSK                         0x10000
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_REF_GATE_SHFT                            0x10
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA1_OSC_EN_BMSK                    0x800
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA1_OSC_EN_SHFT                      0xb
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA1_IN_EN_BMSK                     0x400
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA1_IN_EN_SHFT                       0xa
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA0_OSC_EN_BMSK                    0x200
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA0_OSC_EN_SHFT                      0x9
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA0_IN_EN_BMSK                     0x100
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_DA0_IN_EN_SHFT                       0x8
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_CDC_COUNTER_RST_BMSK                 0x8
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_CDC_COUNTER_RST_SHFT                 0x3
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_LOAD_DA_SEL_BMSK                     0x4
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_LOAD_DA_SEL_SHFT                     0x2
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_ACTV_DA_SEL_BMSK                     0x2
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_ACTV_DA_SEL_SHFT                     0x1
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_ENA_BMSK                             0x1
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SW_OVRD_CFG_SW_OVRD_ENA_SHFT                             0x0

#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR                                (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000060)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_RMSK                                   0x3f7ff
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR, HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_RMSK)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_IN)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAV_DDA_OFFSET_MODE_BMSK              0x20000
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAV_DDA_OFFSET_MODE_SHFT                 0x11
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAV_DDA_OFFSET_SIGN_BMSK              0x10000
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAV_DDA_OFFSET_SIGN_SHFT                 0x10
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAVE_DDA_OFFSET_BMSK                   0xf000
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAVE_DDA_OFFSET_SHFT                      0xc
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAVE_DDA_DELAY_BMSK                     0x7ff
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_SLAVE_DDA_CFG_SLAVE_DDA_DELAY_SHFT                       0x0

#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_ADDR                                      (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000070)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_RMSK                                       0x7ffffff
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_ADDR, HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_RMSK)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_CDC_ERROR_CODE_BMSK                        0x7000000
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_CDC_ERROR_CODE_SHFT                             0x18
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_SW_REF_GATE_COUNT_BMSK                      0xff0000
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_SW_REF_GATE_COUNT_SHFT                          0x10
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_OSC_COUNT_BMSK                                0xfff0
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_OSC_COUNT_SHFT                                   0x4
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_CURR_SEL_DA_BMSK                                 0x8
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_CURR_SEL_DA_SHFT                                 0x3
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_CTLR_SM_IDLE_BMSK                                0x4
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_CTLR_SM_IDLE_SHFT                                0x2
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_OSC_DONE_BMSK                                    0x2
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_OSC_DONE_SHFT                                    0x1
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_CALIBRATION_DONE_BMSK                            0x1
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS0_CALIBRATION_DONE_SHFT                            0x0

#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS1_ADDR                                      (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000074)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS1_RMSK                                       0xfff0fff
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS1_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS1_ADDR, HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS1_RMSK)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS1_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS1_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS1_CURR_DELAY_VALUE_BMSK                      0xfff0000
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS1_CURR_DELAY_VALUE_SHFT                           0x10
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS1_CURR_TMUX_DELAY_BMSK                           0xfff
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS1_CURR_TMUX_DELAY_SHFT                             0x0

#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS2_ADDR                                      (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000078)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS2_RMSK                                      0x1f3f1f3f
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS2_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS2_ADDR, HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS2_RMSK)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS2_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS2_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS2_DA1_SUBUNITS_BMSK                         0x1f000000
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS2_DA1_SUBUNITS_SHFT                               0x18
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS2_DA1_UNITSTEPS_BMSK                          0x3f0000
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS2_DA1_UNITSTEPS_SHFT                              0x10
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS2_DA0_SUBUNITS_BMSK                             0x1f00
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS2_DA0_SUBUNITS_SHFT                                0x8
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS2_DA0_UNITSTEPS_BMSK                              0x3f
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS2_DA0_UNITSTEPS_SHFT                               0x0

#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS3_ADDR                                      (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x0000007c)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS3_RMSK                                      0xffffffff
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS3_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS3_ADDR, HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS3_RMSK)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS3_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS3_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS3_NUM_OF_OSC_ITER_BMSK                      0xff000000
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS3_NUM_OF_OSC_ITER_SHFT                            0x18
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS3_COUNT_ERROR_BMSK                            0xfff000
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS3_COUNT_ERROR_SHFT                                 0xc
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS3_CURR_TARGET_COUNT_BMSK                         0xfff
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS3_CURR_TARGET_COUNT_SHFT                           0x0

#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS4_ADDR                                      (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000080)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS4_RMSK                                            0xff
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS4_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS4_ADDR, HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS4_RMSK)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS4_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS4_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS4_SLAVE_DDA_DA1_TAPS_BMSK                         0xf0
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS4_SLAVE_DDA_DA1_TAPS_SHFT                          0x4
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS4_SLAVE_DDA_DA0_TAPS_BMSK                          0xf
#define HWIO_DIM_D03_DIM_DQ_RD_CDC_STATUS4_SLAVE_DDA_DA0_TAPS_SHFT                          0x0

#define HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR                                   (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x000000ac)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_RMSK                                     0x19f1bf
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR, HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_RMSK)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_IN)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_MODE_BMSK                 0x100000
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_MODE_SHFT                     0x14
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SIGN_BMSK                  0x80000
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SIGN_SHFT                     0x13
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_BMSK                       0x1f000
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_SUBUNIT_OFFSET_SHFT                           0xc
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_MODE_BMSK                       0x100
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_MODE_SHFT                         0x8
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_SIGN_BMSK                        0x80
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_SIGN_SHFT                         0x7
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_BMSK                             0x3f
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_OFFSET_CFG_UNIT_OFFSET_SHFT                              0x0

#define HWIO_DIM_D03_DIM_DQ_WR_CDC_DELAY_CFG_ADDR                                    (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x000000b0)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_DELAY_CFG_RMSK                                    0x1fff0fff
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_DELAY_CFG_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_WR_CDC_DELAY_CFG_ADDR, HWIO_DIM_D03_DIM_DQ_WR_CDC_DELAY_CFG_RMSK)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_DELAY_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_WR_CDC_DELAY_CFG_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_DELAY_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_WR_CDC_DELAY_CFG_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_DELAY_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_WR_CDC_DELAY_CFG_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_WR_CDC_DELAY_CFG_IN)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_DELAY_CFG_TARGET_COUNT_ENA_BMSK                   0x10000000
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_DELAY_CFG_TARGET_COUNT_ENA_SHFT                         0x1c
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_DELAY_CFG_TARGET_COUNT_BMSK                        0xfff0000
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_DELAY_CFG_TARGET_COUNT_SHFT                             0x10
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_DELAY_CFG_DELAY_VAL_BMSK                               0xfff
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_DELAY_CFG_DELAY_VAL_SHFT                                 0x0

#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR                                  (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x000000b4)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_RMSK                                         0xf
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR, HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_RMSK)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_IN)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_DEL_MODE_BMSK                             0x8
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_DEL_MODE_SHFT                             0x3
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_OSC_MODE_BMSK                             0x4
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_OSC_MODE_SHFT                             0x2
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_DA_SEL_BMSK                               0x2
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_DA_SEL_SHFT                               0x1
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_LOAD_BMSK                                 0x1
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_MODE_CFG_SW_LOAD_SHFT                                 0x0

#define HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_ADDR                                     (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x000000b8)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_RMSK                                            0xf
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_ADDR, HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_RMSK)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_IN)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_CDC_SEL_DDA_TEST_BMSK                           0x8
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_CDC_SEL_DDA_TEST_SHFT                           0x3
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_CDC_OSC_TEST_EN_BMSK                            0x4
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_CDC_OSC_TEST_EN_SHFT                            0x2
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_CDC_TEST_EN_BMSK                                0x2
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_CDC_TEST_EN_SHFT                                0x1
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_CDC_OUT_ONTEST2_BMSK                            0x1
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_TEST_CFG_CDC_OUT_ONTEST2_SHFT                            0x0

#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR                                  (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x000000bc)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_RMSK                                     0x10f0f
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR, HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_RMSK)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_IN)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_REF_GATE_BMSK                         0x10000
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_REF_GATE_SHFT                            0x10
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA1_OSC_EN_BMSK                    0x800
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA1_OSC_EN_SHFT                      0xb
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA1_IN_EN_BMSK                     0x400
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA1_IN_EN_SHFT                       0xa
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA0_OSC_EN_BMSK                    0x200
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA0_OSC_EN_SHFT                      0x9
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA0_IN_EN_BMSK                     0x100
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_DA0_IN_EN_SHFT                       0x8
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_CDC_COUNTER_RST_BMSK                 0x8
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_CDC_COUNTER_RST_SHFT                 0x3
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_LOAD_DA_SEL_BMSK                     0x4
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_LOAD_DA_SEL_SHFT                     0x2
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_ACTV_DA_SEL_BMSK                     0x2
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_ACTV_DA_SEL_SHFT                     0x1
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_ENA_BMSK                             0x1
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_SW_OVRD_CFG_SW_OVRD_ENA_SHFT                             0x0

#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_ADDR                                      (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x000000d0)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_RMSK                                       0x7ffffff
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_ADDR, HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_RMSK)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_CDC_ERROR_CODE_BMSK                        0x7000000
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_CDC_ERROR_CODE_SHFT                             0x18
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_SW_REF_GATE_COUNT_BMSK                      0xff0000
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_SW_REF_GATE_COUNT_SHFT                          0x10
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_OSC_COUNT_BMSK                                0xfff0
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_OSC_COUNT_SHFT                                   0x4
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_CURR_SEL_DA_BMSK                                 0x8
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_CURR_SEL_DA_SHFT                                 0x3
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_CTLR_SM_IDLE_BMSK                                0x4
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_CTLR_SM_IDLE_SHFT                                0x2
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_OSC_DONE_BMSK                                    0x2
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_OSC_DONE_SHFT                                    0x1
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_CALIBRATION_DONE_BMSK                            0x1
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS0_CALIBRATION_DONE_SHFT                            0x0

#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS1_ADDR                                      (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x000000d4)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS1_RMSK                                       0xfff0fff
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS1_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS1_ADDR, HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS1_RMSK)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS1_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS1_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS1_CURR_DELAY_VALUE_BMSK                      0xfff0000
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS1_CURR_DELAY_VALUE_SHFT                           0x10
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS1_CURR_TMUX_DELAY_BMSK                           0xfff
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS1_CURR_TMUX_DELAY_SHFT                             0x0

#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS2_ADDR                                      (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x000000d8)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS2_RMSK                                      0x1f3f1f3f
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS2_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS2_ADDR, HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS2_RMSK)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS2_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS2_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS2_DA1_SUBUNITS_BMSK                         0x1f000000
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS2_DA1_SUBUNITS_SHFT                               0x18
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS2_DA1_UNITSTEPS_BMSK                          0x3f0000
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS2_DA1_UNITSTEPS_SHFT                              0x10
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS2_DA0_SUBUNITS_BMSK                             0x1f00
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS2_DA0_SUBUNITS_SHFT                                0x8
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS2_DA0_UNITSTEPS_BMSK                              0x3f
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS2_DA0_UNITSTEPS_SHFT                               0x0

#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS3_ADDR                                      (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x000000dc)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS3_RMSK                                      0xffffffff
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS3_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS3_ADDR, HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS3_RMSK)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS3_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS3_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS3_NUM_OF_OSC_ITER_BMSK                      0xff000000
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS3_NUM_OF_OSC_ITER_SHFT                            0x18
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS3_COUNT_ERROR_BMSK                            0xfff000
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS3_COUNT_ERROR_SHFT                                 0xc
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS3_CURR_TARGET_COUNT_BMSK                         0xfff
#define HWIO_DIM_D03_DIM_DQ_WR_CDC_STATUS3_CURR_TARGET_COUNT_SHFT                           0x0

#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_ADDR                                      (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000100)
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_RMSK                                      0x9f1f9f9f
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_ADDR, HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_RMSK)
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_IN)
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_BMSK                     0x80000000
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_SHFT                           0x1f
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_SW_VAL_BMSK                          0x1f000000
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_SW_VAL_SHFT                                0x18
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_SW_VAL_BMSK                            0x1f0000
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_SW_VAL_SHFT                                0x10
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_OFFSET_SIGN_BMSK                         0x8000
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_OFFSET_SIGN_SHFT                            0xf
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_SW_OFFSET_BMSK                           0x1f00
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_NCNT_SW_OFFSET_SHFT                              0x8
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_OFFSET_SIGN_BMSK                           0x80
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_OFFSET_SIGN_SHFT                            0x7
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_SW_OFFSET_BMSK                             0x1f
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_CFG_PCNT_SW_OFFSET_SHFT                              0x0

#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_STATUS_ADDR                                   (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000104)
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_STATUS_RMSK                                       0x1f1f
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_STATUS_ADDR, HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_STATUS_RMSK)
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_STATUS_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_STATUS_PAD_NCNT_BMSK                              0x1f00
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_STATUS_PAD_NCNT_SHFT                                 0x8
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_STATUS_PAD_PCNT_BMSK                                0x1f
#define HWIO_DIM_D03_DIM_DQ_DQ_IOC_SLV_STATUS_PAD_PCNT_SHFT                                 0x0

#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_ADDR                                     (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000110)
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_RMSK                                     0x9f1f9f9f
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_ADDR, HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_RMSK)
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_OUT(v)      \
        out_dword(HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_ADDR,v)
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_ADDR,m,v,HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_IN)
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_BMSK                    0x80000000
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_PNCNT_HW_LOAD_EN_SHFT                          0x1f
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_SW_VAL_BMSK                         0x1f000000
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_SW_VAL_SHFT                               0x18
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_SW_VAL_BMSK                           0x1f0000
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_SW_VAL_SHFT                               0x10
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_OFFSET_SIGN_BMSK                        0x8000
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_OFFSET_SIGN_SHFT                           0xf
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_SW_OFFSET_BMSK                          0x1f00
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_NCNT_SW_OFFSET_SHFT                             0x8
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_OFFSET_SIGN_BMSK                          0x80
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_OFFSET_SIGN_SHFT                           0x7
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_SW_OFFSET_BMSK                            0x1f
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_CFG_PCNT_SW_OFFSET_SHFT                             0x0

#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_STATUS_ADDR                                  (DIM_D03_DDRPHY_DQ_REG_BASE      + 0x00000114)
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_STATUS_RMSK                                      0x1f1f
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_STATUS_IN          \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_STATUS_ADDR, HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_STATUS_RMSK)
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_STATUS_INM(m)      \
        in_dword_masked(HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_STATUS_ADDR, m)
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_STATUS_PAD_NCNT_BMSK                             0x1f00
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_STATUS_PAD_NCNT_SHFT                                0x8
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_STATUS_PAD_PCNT_BMSK                               0x1f
#define HWIO_DIM_D03_DIM_DQ_DQS_IOC_SLV_STATUS_PAD_PCNT_SHFT                                0x0

/*----------------------------------------------------------------------------
 * MODULE: EBI1_AHB2PHY_BROADCAST_SWMAN
 *--------------------------------------------------------------------------*/

#define EBI1_AHB2PHY_BROADCAST_SWMAN_REG_BASE                                     (EBI1_PHY_CFG_BASE      + 0x0001f800)

#define HWIO_EBI1_AHB2PHY_BROADCAST_ADDRESS_SPACE_n_ADDR(n)                       (EBI1_AHB2PHY_BROADCAST_SWMAN_REG_BASE      + 0x00000000 + 0x4 * (n))
#define HWIO_EBI1_AHB2PHY_BROADCAST_ADDRESS_SPACE_n_RMSK                          0xffffffff
#define HWIO_EBI1_AHB2PHY_BROADCAST_ADDRESS_SPACE_n_MAXn                                 255
#define HWIO_EBI1_AHB2PHY_BROADCAST_ADDRESS_SPACE_n_INI(n)        \
        in_dword_masked(HWIO_EBI1_AHB2PHY_BROADCAST_ADDRESS_SPACE_n_ADDR(n), HWIO_EBI1_AHB2PHY_BROADCAST_ADDRESS_SPACE_n_RMSK)
#define HWIO_EBI1_AHB2PHY_BROADCAST_ADDRESS_SPACE_n_INMI(n,mask)    \
        in_dword_masked(HWIO_EBI1_AHB2PHY_BROADCAST_ADDRESS_SPACE_n_ADDR(n), mask)
#define HWIO_EBI1_AHB2PHY_BROADCAST_ADDRESS_SPACE_n_OUTI(n,val)    \
        out_dword(HWIO_EBI1_AHB2PHY_BROADCAST_ADDRESS_SPACE_n_ADDR(n),val)
#define HWIO_EBI1_AHB2PHY_BROADCAST_ADDRESS_SPACE_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_EBI1_AHB2PHY_BROADCAST_ADDRESS_SPACE_n_ADDR(n),mask,val,HWIO_EBI1_AHB2PHY_BROADCAST_ADDRESS_SPACE_n_INI(n))
#define HWIO_EBI1_AHB2PHY_BROADCAST_ADDRESS_SPACE_n_BROADCAST_SPACE_BMSK          0xffffffff
#define HWIO_EBI1_AHB2PHY_BROADCAST_ADDRESS_SPACE_n_BROADCAST_SPACE_SHFT                 0x0

/*----------------------------------------------------------------------------
 * MODULE: EBI1_AHB2PHY_SWMAN
 *--------------------------------------------------------------------------*/

#define EBI1_AHB2PHY_SWMAN_REG_BASE                                               (EBI1_PHY_CFG_BASE      + 0x0001f000)

#define HWIO_EBI1_HW_VERSION_ADDR                                                 (EBI1_AHB2PHY_SWMAN_REG_BASE      + 0x00000000)
#define HWIO_EBI1_HW_VERSION_RMSK                                                   0xffffff
#define HWIO_EBI1_HW_VERSION_IN          \
        in_dword_masked(HWIO_EBI1_HW_VERSION_ADDR, HWIO_EBI1_HW_VERSION_RMSK)
#define HWIO_EBI1_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_EBI1_HW_VERSION_ADDR, m)
#define HWIO_EBI1_HW_VERSION_MAJOR_BMSK                                             0xff0000
#define HWIO_EBI1_HW_VERSION_MAJOR_SHFT                                                 0x10
#define HWIO_EBI1_HW_VERSION_MINOR_BMSK                                               0xff00
#define HWIO_EBI1_HW_VERSION_MINOR_SHFT                                                  0x8
#define HWIO_EBI1_HW_VERSION_STEP_BMSK                                                  0xff
#define HWIO_EBI1_HW_VERSION_STEP_SHFT                                                   0x0

#define HWIO_EBI1_HW_INFO_ADDR                                                    (EBI1_AHB2PHY_SWMAN_REG_BASE      + 0x00000004)
#define HWIO_EBI1_HW_INFO_RMSK                                                    0xffffffff
#define HWIO_EBI1_HW_INFO_IN          \
        in_dword_masked(HWIO_EBI1_HW_INFO_ADDR, HWIO_EBI1_HW_INFO_RMSK)
#define HWIO_EBI1_HW_INFO_INM(m)      \
        in_dword_masked(HWIO_EBI1_HW_INFO_ADDR, m)
#define HWIO_EBI1_HW_INFO_HW_MAJOR_BMSK                                           0xff000000
#define HWIO_EBI1_HW_INFO_HW_MAJOR_SHFT                                                 0x18
#define HWIO_EBI1_HW_INFO_HW_BRANCH_BMSK                                            0xff0000
#define HWIO_EBI1_HW_INFO_HW_BRANCH_SHFT                                                0x10
#define HWIO_EBI1_HW_INFO_HW_MINOR_BMSK                                               0xff00
#define HWIO_EBI1_HW_INFO_HW_MINOR_SHFT                                                  0x8
#define HWIO_EBI1_HW_INFO_HW_ECO_BMSK                                                   0xff
#define HWIO_EBI1_HW_INFO_HW_ECO_SHFT                                                    0x0

#define HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_LOWER_ADDR                             (EBI1_AHB2PHY_SWMAN_REG_BASE      + 0x00000008)
#define HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_LOWER_RMSK                             0xffffffff
#define HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_LOWER_IN          \
        in_dword_masked(HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_LOWER_ADDR, HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_LOWER_RMSK)
#define HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_LOWER_INM(m)      \
        in_dword_masked(HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_LOWER_ADDR, m)
#define HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_LOWER_OUT(v)      \
        out_dword(HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_LOWER_ADDR,v)
#define HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_LOWER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_LOWER_ADDR,m,v,HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_LOWER_IN)
#define HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_LOWER_BROADCAST_EN_LOWER_BMSK          0xffffffff
#define HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_LOWER_BROADCAST_EN_LOWER_SHFT                 0x0

#define HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_UPPER_ADDR                             (EBI1_AHB2PHY_SWMAN_REG_BASE      + 0x0000000c)
#define HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_UPPER_RMSK                             0x3fffffff
#define HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_UPPER_IN          \
        in_dword_masked(HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_UPPER_ADDR, HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_UPPER_RMSK)
#define HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_UPPER_INM(m)      \
        in_dword_masked(HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_UPPER_ADDR, m)
#define HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_UPPER_OUT(v)      \
        out_dword(HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_UPPER_ADDR,v)
#define HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_UPPER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_UPPER_ADDR,m,v,HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_UPPER_IN)
#define HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_UPPER_BROADCAST_EN_UPPER_BMSK          0x3fffffff
#define HWIO_EBI1_AHB2PHY_BROADCAST_EN_CFG_UPPER_BROADCAST_EN_UPPER_SHFT                 0x0

#define HWIO_EBI1_AHB2PHY_TOP_CFG_ADDR                                            (EBI1_AHB2PHY_SWMAN_REG_BASE      + 0x00000010)
#define HWIO_EBI1_AHB2PHY_TOP_CFG_RMSK                                                  0x33
#define HWIO_EBI1_AHB2PHY_TOP_CFG_IN          \
        in_dword_masked(HWIO_EBI1_AHB2PHY_TOP_CFG_ADDR, HWIO_EBI1_AHB2PHY_TOP_CFG_RMSK)
#define HWIO_EBI1_AHB2PHY_TOP_CFG_INM(m)      \
        in_dword_masked(HWIO_EBI1_AHB2PHY_TOP_CFG_ADDR, m)
#define HWIO_EBI1_AHB2PHY_TOP_CFG_OUT(v)      \
        out_dword(HWIO_EBI1_AHB2PHY_TOP_CFG_ADDR,v)
#define HWIO_EBI1_AHB2PHY_TOP_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_EBI1_AHB2PHY_TOP_CFG_ADDR,m,v,HWIO_EBI1_AHB2PHY_TOP_CFG_IN)
#define HWIO_EBI1_AHB2PHY_TOP_CFG_WRITE_WAIT_STATES_BMSK                                0x30
#define HWIO_EBI1_AHB2PHY_TOP_CFG_WRITE_WAIT_STATES_SHFT                                 0x4
#define HWIO_EBI1_AHB2PHY_TOP_CFG_READ_WAIT_STATES_BMSK                                  0x3
#define HWIO_EBI1_AHB2PHY_TOP_CFG_READ_WAIT_STATES_SHFT                                  0x0

#define HWIO_EBI1_AHB2PHY_DEBUG_BUS_CFG_ADDR                                      (EBI1_AHB2PHY_SWMAN_REG_BASE      + 0x00000014)
#define HWIO_EBI1_AHB2PHY_DEBUG_BUS_CFG_RMSK                                             0x3
#define HWIO_EBI1_AHB2PHY_DEBUG_BUS_CFG_IN          \
        in_dword_masked(HWIO_EBI1_AHB2PHY_DEBUG_BUS_CFG_ADDR, HWIO_EBI1_AHB2PHY_DEBUG_BUS_CFG_RMSK)
#define HWIO_EBI1_AHB2PHY_DEBUG_BUS_CFG_INM(m)      \
        in_dword_masked(HWIO_EBI1_AHB2PHY_DEBUG_BUS_CFG_ADDR, m)
#define HWIO_EBI1_AHB2PHY_DEBUG_BUS_CFG_OUT(v)      \
        out_dword(HWIO_EBI1_AHB2PHY_DEBUG_BUS_CFG_ADDR,v)
#define HWIO_EBI1_AHB2PHY_DEBUG_BUS_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_EBI1_AHB2PHY_DEBUG_BUS_CFG_ADDR,m,v,HWIO_EBI1_AHB2PHY_DEBUG_BUS_CFG_IN)
#define HWIO_EBI1_AHB2PHY_DEBUG_BUS_CFG_DEBUG_BUS_SEL_BMSK                               0x3
#define HWIO_EBI1_AHB2PHY_DEBUG_BUS_CFG_DEBUG_BUS_SEL_SHFT                               0x0


#endif /* __DDR_HWIO_H__ */
