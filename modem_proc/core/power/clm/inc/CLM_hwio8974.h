#ifndef __CLM_HWIO8974_H__
#define __CLM_HWIO8974_H__
/*
===========================================================================
*/
/**
  @file CLM_hwio8974.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    MSS_QDSP6SS_PUB

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/power/clm/inc/CLM_hwio8974.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: MSS_QDSP6SS_PUB
 *--------------------------------------------------------------------------*/

#define MSS_QDSP6SS_PUB_REG_BASE                                      (MSS_TOP_BASE      + 0x00080000)

#define HWIO_MSS_QDSP6SS_VERSION_ADDR                                 (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000000)
#define HWIO_MSS_QDSP6SS_VERSION_RMSK                                 0xffffffff
#define HWIO_MSS_QDSP6SS_VERSION_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_VERSION_ADDR, HWIO_MSS_QDSP6SS_VERSION_RMSK)
#define HWIO_MSS_QDSP6SS_VERSION_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_VERSION_ADDR, m)
#define HWIO_MSS_QDSP6SS_VERSION_MAJOR_BMSK                           0xf0000000
#define HWIO_MSS_QDSP6SS_VERSION_MAJOR_SHFT                                 0x1c
#define HWIO_MSS_QDSP6SS_VERSION_MINOR_BMSK                            0xfff0000
#define HWIO_MSS_QDSP6SS_VERSION_MINOR_SHFT                                 0x10
#define HWIO_MSS_QDSP6SS_VERSION_STEP_BMSK                                0xffff
#define HWIO_MSS_QDSP6SS_VERSION_STEP_SHFT                                   0x0

#define HWIO_MSS_QDSP6SS_RST_EVB_ADDR                                 (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000010)
#define HWIO_MSS_QDSP6SS_RST_EVB_RMSK                                  0xffffff0
#define HWIO_MSS_QDSP6SS_RST_EVB_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_RST_EVB_ADDR, HWIO_MSS_QDSP6SS_RST_EVB_RMSK)
#define HWIO_MSS_QDSP6SS_RST_EVB_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_RST_EVB_ADDR, m)
#define HWIO_MSS_QDSP6SS_RST_EVB_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_RST_EVB_ADDR,v)
#define HWIO_MSS_QDSP6SS_RST_EVB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_RST_EVB_ADDR,m,v,HWIO_MSS_QDSP6SS_RST_EVB_IN)
#define HWIO_MSS_QDSP6SS_RST_EVB_EVB_BMSK                              0xffffff0
#define HWIO_MSS_QDSP6SS_RST_EVB_EVB_SHFT                                    0x4

#define HWIO_MSS_QDSP6SS_RESET_ADDR                                   (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000014)
#define HWIO_MSS_QDSP6SS_RESET_RMSK                                        0x1f7
#define HWIO_MSS_QDSP6SS_RESET_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_RESET_ADDR, HWIO_MSS_QDSP6SS_RESET_RMSK)
#define HWIO_MSS_QDSP6SS_RESET_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_RESET_ADDR, m)
#define HWIO_MSS_QDSP6SS_RESET_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_RESET_ADDR,v)
#define HWIO_MSS_QDSP6SS_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_RESET_ADDR,m,v,HWIO_MSS_QDSP6SS_RESET_IN)
#define HWIO_MSS_QDSP6SS_RESET_AXIS_ARES_BMSK                              0x100
#define HWIO_MSS_QDSP6SS_RESET_AXIS_ARES_SHFT                                0x8
#define HWIO_MSS_QDSP6SS_RESET_ATBM_ARES_BMSK                               0x80
#define HWIO_MSS_QDSP6SS_RESET_ATBM_ARES_SHFT                                0x7
#define HWIO_MSS_QDSP6SS_RESET_AHBM_ARES_BMSK                               0x40
#define HWIO_MSS_QDSP6SS_RESET_AHBM_ARES_SHFT                                0x6
#define HWIO_MSS_QDSP6SS_RESET_AXIM_ARES_BMSK                               0x20
#define HWIO_MSS_QDSP6SS_RESET_AXIM_ARES_SHFT                                0x5
#define HWIO_MSS_QDSP6SS_RESET_ETM_ISDB_ARES_BMSK                           0x10
#define HWIO_MSS_QDSP6SS_RESET_ETM_ISDB_ARES_SHFT                            0x4
#define HWIO_MSS_QDSP6SS_RESET_BUS_ARES_ENA_BMSK                             0x4
#define HWIO_MSS_QDSP6SS_RESET_BUS_ARES_ENA_SHFT                             0x2
#define HWIO_MSS_QDSP6SS_RESET_CORE_ARES_BMSK                                0x2
#define HWIO_MSS_QDSP6SS_RESET_CORE_ARES_SHFT                                0x1
#define HWIO_MSS_QDSP6SS_RESET_STOP_CORE_BMSK                                0x1
#define HWIO_MSS_QDSP6SS_RESET_STOP_CORE_SHFT                                0x0

#define HWIO_MSS_QDSP6SS_DBG_CFG_ADDR                                 (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000018)
#define HWIO_MSS_QDSP6SS_DBG_CFG_RMSK                                 0xffff0003
#define HWIO_MSS_QDSP6SS_DBG_CFG_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_DBG_CFG_ADDR, HWIO_MSS_QDSP6SS_DBG_CFG_RMSK)
#define HWIO_MSS_QDSP6SS_DBG_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_DBG_CFG_ADDR, m)
#define HWIO_MSS_QDSP6SS_DBG_CFG_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_DBG_CFG_ADDR,v)
#define HWIO_MSS_QDSP6SS_DBG_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_DBG_CFG_ADDR,m,v,HWIO_MSS_QDSP6SS_DBG_CFG_IN)
#define HWIO_MSS_QDSP6SS_DBG_CFG_DBG_SW_REG_BMSK                      0xff000000
#define HWIO_MSS_QDSP6SS_DBG_CFG_DBG_SW_REG_SHFT                            0x18
#define HWIO_MSS_QDSP6SS_DBG_CFG_DBG_SPARE_BMSK                         0xff0000
#define HWIO_MSS_QDSP6SS_DBG_CFG_DBG_SPARE_SHFT                             0x10
#define HWIO_MSS_QDSP6SS_DBG_CFG_CTI_TIHS_ENA_BMSK                           0x2
#define HWIO_MSS_QDSP6SS_DBG_CFG_CTI_TIHS_ENA_SHFT                           0x1
#define HWIO_MSS_QDSP6SS_DBG_CFG_DBG_ARES_ENA_BMSK                           0x1
#define HWIO_MSS_QDSP6SS_DBG_CFG_DBG_ARES_ENA_SHFT                           0x0

#define HWIO_MSS_QDSP6SS_GFMUX_CTL_ADDR                               (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000020)
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_RMSK                                    0x30f
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_GFMUX_CTL_ADDR, HWIO_MSS_QDSP6SS_GFMUX_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_GFMUX_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_GFMUX_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_GFMUX_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_GFMUX_CTL_IN)
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_IDLE_CORE_CLK_EN_BMSK                   0x200
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_IDLE_CORE_CLK_EN_SHFT                     0x9
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_SRC_SWITCH_CLK_OVR_BMSK                 0x100
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_SRC_SWITCH_CLK_OVR_SHFT                   0x8
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_CLK_SRC_SEL_BMSK                          0xc
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_CLK_SRC_SEL_SHFT                          0x2
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_CLK_ENA_BMSK                              0x2
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_CLK_ENA_SHFT                              0x1
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_CLK_ARES_BMSK                             0x1
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_CLK_ARES_SHFT                             0x0

#define HWIO_MSS_QDSP6SS_GFMUX_STATUS_ADDR                            (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000024)
#define HWIO_MSS_QDSP6SS_GFMUX_STATUS_RMSK                                  0x1f
#define HWIO_MSS_QDSP6SS_GFMUX_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_GFMUX_STATUS_ADDR, HWIO_MSS_QDSP6SS_GFMUX_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_GFMUX_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_GFMUX_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_GFMUX_STATUS_SEL_STATUS_BMSK                       0x1e
#define HWIO_MSS_QDSP6SS_GFMUX_STATUS_SEL_STATUS_SHFT                        0x1
#define HWIO_MSS_QDSP6SS_GFMUX_STATUS_SWITCH_STATUS_BMSK                     0x1
#define HWIO_MSS_QDSP6SS_GFMUX_STATUS_SWITCH_STATUS_SHFT                     0x0

#define HWIO_MSS_QDSP6SS_PWR_CTL_ADDR                                 (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000030)
#define HWIO_MSS_QDSP6SS_PWR_CTL_RMSK                                  0xfff0007
#define HWIO_MSS_QDSP6SS_PWR_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PWR_CTL_ADDR, HWIO_MSS_QDSP6SS_PWR_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_PWR_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PWR_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_PWR_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PWR_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_PWR_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PWR_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_PWR_CTL_IN)
#define HWIO_MSS_QDSP6SS_PWR_CTL_LDO_BG_PU_BMSK                        0x8000000
#define HWIO_MSS_QDSP6SS_PWR_CTL_LDO_BG_PU_SHFT                             0x1b
#define HWIO_MSS_QDSP6SS_PWR_CTL_LDO_VREF_SEL_BMSK                     0x4000000
#define HWIO_MSS_QDSP6SS_PWR_CTL_LDO_VREF_SEL_SHFT                          0x1a
#define HWIO_MSS_QDSP6SS_PWR_CTL_LDO_PWR_UP_BMSK                       0x3000000
#define HWIO_MSS_QDSP6SS_PWR_CTL_LDO_PWR_UP_SHFT                            0x18
#define HWIO_MSS_QDSP6SS_PWR_CTL_LDO_BYP_BMSK                           0xc00000
#define HWIO_MSS_QDSP6SS_PWR_CTL_LDO_BYP_SHFT                               0x16
#define HWIO_MSS_QDSP6SS_PWR_CTL_BHS_ON_BMSK                            0x200000
#define HWIO_MSS_QDSP6SS_PWR_CTL_BHS_ON_SHFT                                0x15
#define HWIO_MSS_QDSP6SS_PWR_CTL_CLAMP_IO_BMSK                          0x100000
#define HWIO_MSS_QDSP6SS_PWR_CTL_CLAMP_IO_SHFT                              0x14
#define HWIO_MSS_QDSP6SS_PWR_CTL_SLP_RET_N_BMSK                          0x80000
#define HWIO_MSS_QDSP6SS_PWR_CTL_SLP_RET_N_SHFT                             0x13
#define HWIO_MSS_QDSP6SS_PWR_CTL_L2DATA_STBY_N_BMSK                      0x40000
#define HWIO_MSS_QDSP6SS_PWR_CTL_L2DATA_STBY_N_SHFT                         0x12
#define HWIO_MSS_QDSP6SS_PWR_CTL_ETB_SLP_NRET_N_BMSK                     0x20000
#define HWIO_MSS_QDSP6SS_PWR_CTL_ETB_SLP_NRET_N_SHFT                        0x11
#define HWIO_MSS_QDSP6SS_PWR_CTL_L2TAG_SLP_NRET_N_BMSK                   0x10000
#define HWIO_MSS_QDSP6SS_PWR_CTL_L2TAG_SLP_NRET_N_SHFT                      0x10
#define HWIO_MSS_QDSP6SS_PWR_CTL_L2DATA_SLP_NRET_N_BMSK                      0x7
#define HWIO_MSS_QDSP6SS_PWR_CTL_L2DATA_SLP_NRET_N_SHFT                      0x0

#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_ADDR                            (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000034)
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_RMSK                                0x3fff
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_CGC_OVERRIDE_ADDR, HWIO_MSS_QDSP6SS_CGC_OVERRIDE_RMSK)
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_CGC_OVERRIDE_ADDR, m)
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_CGC_OVERRIDE_ADDR,v)
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_CGC_OVERRIDE_ADDR,m,v,HWIO_MSS_QDSP6SS_CGC_OVERRIDE_IN)
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_AXIS_CLK_EN_BMSK                    0x2000
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_AXIS_CLK_EN_SHFT                       0xd
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_AXIM_CLK_EN_BMSK                    0x1000
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_AXIM_CLK_EN_SHFT                       0xc
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_AHBM_CLK_EN_BMSK                     0x800
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_AHBM_CLK_EN_SHFT                       0xb
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_AHBS_CLK_EN_BMSK                     0x400
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_AHBS_CLK_EN_SHFT                       0xa
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_PRIV_AHBS_CLK_EN_BMSK                0x300
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_PRIV_AHBS_CLK_EN_SHFT                  0x8
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_TCXO_CLK_EN_BMSK                      0x80
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_TCXO_CLK_EN_SHFT                       0x7
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_L2VIC_AHBS_CLK_EN_BMSK                0x60
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_L2VIC_AHBS_CLK_EN_SHFT                 0x5
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_TEST_AHBS_CLK_EN_BMSK                 0x10
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_TEST_AHBS_CLK_EN_SHFT                  0x4
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_PUB_AHBS_CLK_EN_BMSK                   0x8
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_PUB_AHBS_CLK_EN_SHFT                   0x3
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_CORE_BUS_EN_BMSK                       0x4
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_CORE_BUS_EN_SHFT                       0x2
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_CORE_RCLK_EN_BMSK                      0x2
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_CORE_RCLK_EN_SHFT                      0x1
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_CORE_CLK_EN_BMSK                       0x1
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_CORE_CLK_EN_SHFT                       0x0

#define HWIO_MSS_QDSP6SS_NMI_ADDR                                     (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000040)
#define HWIO_MSS_QDSP6SS_NMI_RMSK                                            0x3
#define HWIO_MSS_QDSP6SS_NMI_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_NMI_ADDR,v)
#define HWIO_MSS_QDSP6SS_NMI_CLEAR_STATUS_BMSK                               0x2
#define HWIO_MSS_QDSP6SS_NMI_CLEAR_STATUS_SHFT                               0x1
#define HWIO_MSS_QDSP6SS_NMI_SET_NMI_BMSK                                    0x1
#define HWIO_MSS_QDSP6SS_NMI_SET_NMI_SHFT                                    0x0

#define HWIO_MSS_QDSP6SS_NMI_STATUS_ADDR                              (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000044)
#define HWIO_MSS_QDSP6SS_NMI_STATUS_RMSK                                     0x3
#define HWIO_MSS_QDSP6SS_NMI_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_NMI_STATUS_ADDR, HWIO_MSS_QDSP6SS_NMI_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_NMI_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_NMI_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_NMI_STATUS_WDOG_TRIG_BMSK                           0x2
#define HWIO_MSS_QDSP6SS_NMI_STATUS_WDOG_TRIG_SHFT                           0x1
#define HWIO_MSS_QDSP6SS_NMI_STATUS_PUBCSR_TRIG_BMSK                         0x1
#define HWIO_MSS_QDSP6SS_NMI_STATUS_PUBCSR_TRIG_SHFT                         0x0

#define HWIO_MSS_QDSP6SS_LDO_CFG0_ADDR                                (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000050)
#define HWIO_MSS_QDSP6SS_LDO_CFG0_RMSK                                 0x7ffffff
#define HWIO_MSS_QDSP6SS_LDO_CFG0_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_CFG0_ADDR, HWIO_MSS_QDSP6SS_LDO_CFG0_RMSK)
#define HWIO_MSS_QDSP6SS_LDO_CFG0_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_CFG0_ADDR, m)
#define HWIO_MSS_QDSP6SS_LDO_CFG0_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_LDO_CFG0_ADDR,v)
#define HWIO_MSS_QDSP6SS_LDO_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_LDO_CFG0_ADDR,m,v,HWIO_MSS_QDSP6SS_LDO_CFG0_IN)
#define HWIO_MSS_QDSP6SS_LDO_CFG0_LDO_TEST_BMSK                        0x4000000
#define HWIO_MSS_QDSP6SS_LDO_CFG0_LDO_TEST_SHFT                             0x1a
#define HWIO_MSS_QDSP6SS_LDO_CFG0_BYP_LDO_EXT_BMSK                     0x2000000
#define HWIO_MSS_QDSP6SS_LDO_CFG0_BYP_LDO_EXT_SHFT                          0x19
#define HWIO_MSS_QDSP6SS_LDO_CFG0_IDAC_EN_BMSK                         0x1000000
#define HWIO_MSS_QDSP6SS_LDO_CFG0_IDAC_EN_SHFT                              0x18
#define HWIO_MSS_QDSP6SS_LDO_CFG0_LDO_CTL2_BMSK                         0xff0000
#define HWIO_MSS_QDSP6SS_LDO_CFG0_LDO_CTL2_SHFT                             0x10
#define HWIO_MSS_QDSP6SS_LDO_CFG0_LDO_CTL1_BMSK                           0xff00
#define HWIO_MSS_QDSP6SS_LDO_CFG0_LDO_CTL1_SHFT                              0x8
#define HWIO_MSS_QDSP6SS_LDO_CFG0_LDO_CTL0_BMSK                             0xff
#define HWIO_MSS_QDSP6SS_LDO_CFG0_LDO_CTL0_SHFT                              0x0

#define HWIO_MSS_QDSP6SS_LDO_CFG1_ADDR                                (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000054)
#define HWIO_MSS_QDSP6SS_LDO_CFG1_RMSK                                  0xffffff
#define HWIO_MSS_QDSP6SS_LDO_CFG1_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_CFG1_ADDR, HWIO_MSS_QDSP6SS_LDO_CFG1_RMSK)
#define HWIO_MSS_QDSP6SS_LDO_CFG1_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_CFG1_ADDR, m)
#define HWIO_MSS_QDSP6SS_LDO_CFG1_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_LDO_CFG1_ADDR,v)
#define HWIO_MSS_QDSP6SS_LDO_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_LDO_CFG1_ADDR,m,v,HWIO_MSS_QDSP6SS_LDO_CFG1_IN)
#define HWIO_MSS_QDSP6SS_LDO_CFG1_DIG_RSV2_BMSK                         0xff0000
#define HWIO_MSS_QDSP6SS_LDO_CFG1_DIG_RSV2_SHFT                             0x10
#define HWIO_MSS_QDSP6SS_LDO_CFG1_DIG_RSV1_BMSK                           0xff00
#define HWIO_MSS_QDSP6SS_LDO_CFG1_DIG_RSV1_SHFT                              0x8
#define HWIO_MSS_QDSP6SS_LDO_CFG1_DIG_CTL_BMSK                              0xff
#define HWIO_MSS_QDSP6SS_LDO_CFG1_DIG_CTL_SHFT                               0x0

#define HWIO_MSS_QDSP6SS_LDO_CFG2_ADDR                                (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000058)
#define HWIO_MSS_QDSP6SS_LDO_CFG2_RMSK                                    0xffff
#define HWIO_MSS_QDSP6SS_LDO_CFG2_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_CFG2_ADDR, HWIO_MSS_QDSP6SS_LDO_CFG2_RMSK)
#define HWIO_MSS_QDSP6SS_LDO_CFG2_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_CFG2_ADDR, m)
#define HWIO_MSS_QDSP6SS_LDO_CFG2_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_LDO_CFG2_ADDR,v)
#define HWIO_MSS_QDSP6SS_LDO_CFG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_LDO_CFG2_ADDR,m,v,HWIO_MSS_QDSP6SS_LDO_CFG2_IN)
#define HWIO_MSS_QDSP6SS_LDO_CFG2_RDAC_CTL2_BMSK                          0xff00
#define HWIO_MSS_QDSP6SS_LDO_CFG2_RDAC_CTL2_SHFT                             0x8
#define HWIO_MSS_QDSP6SS_LDO_CFG2_RDAC_CTL1_BMSK                            0xff
#define HWIO_MSS_QDSP6SS_LDO_CFG2_RDAC_CTL1_SHFT                             0x0

#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_ADDR                            (MSS_QDSP6SS_PUB_REG_BASE      + 0x0000005c)
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_RMSK                                0x7f7f
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_VREF_SET_ADDR, HWIO_MSS_QDSP6SS_LDO_VREF_SET_RMSK)
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_VREF_SET_ADDR, m)
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_LDO_VREF_SET_ADDR,v)
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_LDO_VREF_SET_ADDR,m,v,HWIO_MSS_QDSP6SS_LDO_VREF_SET_IN)
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_VREF_RET_BMSK                       0x7f00
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_VREF_RET_SHFT                          0x8
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_VREF_LDO_BMSK                         0x7f
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_VREF_LDO_SHFT                          0x0

#define HWIO_MSS_QDSP6SS_STRAP_TCM_ADDR                               (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000100)
#define HWIO_MSS_QDSP6SS_STRAP_TCM_RMSK                                0xffc0000
#define HWIO_MSS_QDSP6SS_STRAP_TCM_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_TCM_ADDR, HWIO_MSS_QDSP6SS_STRAP_TCM_RMSK)
#define HWIO_MSS_QDSP6SS_STRAP_TCM_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_TCM_ADDR, m)
#define HWIO_MSS_QDSP6SS_STRAP_TCM_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_STRAP_TCM_ADDR,v)
#define HWIO_MSS_QDSP6SS_STRAP_TCM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_STRAP_TCM_ADDR,m,v,HWIO_MSS_QDSP6SS_STRAP_TCM_IN)
#define HWIO_MSS_QDSP6SS_STRAP_TCM_BASE_ADDR_BMSK                      0xffc0000
#define HWIO_MSS_QDSP6SS_STRAP_TCM_BASE_ADDR_SHFT                           0x12

#define HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_ADDR                          (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000104)
#define HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_RMSK                           0xfffc000
#define HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_ADDR, HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_RMSK)
#define HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_ADDR, m)
#define HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_ADDR,v)
#define HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_ADDR,m,v,HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_IN)
#define HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_UPPER_ADDR_BMSK                0xfffc000
#define HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_UPPER_ADDR_SHFT                      0xe

#define HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_ADDR                          (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000108)
#define HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_RMSK                           0xfffc000
#define HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_ADDR, HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_RMSK)
#define HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_ADDR, m)
#define HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_ADDR,v)
#define HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_ADDR,m,v,HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_IN)
#define HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_LOWER_ADDR_BMSK                0xfffc000
#define HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_LOWER_ADDR_SHFT                      0xe

#define HWIO_MSS_QDSP6SS_STRAP_ACC_ADDR                               (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000110)
#define HWIO_MSS_QDSP6SS_STRAP_ACC_RMSK                               0xffffffff
#define HWIO_MSS_QDSP6SS_STRAP_ACC_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_ACC_ADDR, HWIO_MSS_QDSP6SS_STRAP_ACC_RMSK)
#define HWIO_MSS_QDSP6SS_STRAP_ACC_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_ACC_ADDR, m)
#define HWIO_MSS_QDSP6SS_STRAP_ACC_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_STRAP_ACC_ADDR,v)
#define HWIO_MSS_QDSP6SS_STRAP_ACC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_STRAP_ACC_ADDR,m,v,HWIO_MSS_QDSP6SS_STRAP_ACC_IN)
#define HWIO_MSS_QDSP6SS_STRAP_ACC_DATA_BMSK                          0xffffffff
#define HWIO_MSS_QDSP6SS_STRAP_ACC_DATA_SHFT                                 0x0

#define HWIO_MSS_QDSP6SS_DCC_CTRL_ADDR                                (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000114)
#define HWIO_MSS_QDSP6SS_DCC_CTRL_RMSK                                0xffffffff
#define HWIO_MSS_QDSP6SS_DCC_CTRL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_DCC_CTRL_ADDR, HWIO_MSS_QDSP6SS_DCC_CTRL_RMSK)
#define HWIO_MSS_QDSP6SS_DCC_CTRL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_DCC_CTRL_ADDR, m)
#define HWIO_MSS_QDSP6SS_DCC_CTRL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_DCC_CTRL_ADDR,v)
#define HWIO_MSS_QDSP6SS_DCC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_DCC_CTRL_ADDR,m,v,HWIO_MSS_QDSP6SS_DCC_CTRL_IN)
#define HWIO_MSS_QDSP6SS_DCC_CTRL_CTRL_BMSK                           0xffffffff
#define HWIO_MSS_QDSP6SS_DCC_CTRL_CTRL_SHFT                                  0x0

#define HWIO_MSS_QDSP6SS_EXT_WAKEUP_ADDR                              (MSS_QDSP6SS_PUB_REG_BASE      + 0x00001000)
#define HWIO_MSS_QDSP6SS_EXT_WAKEUP_RMSK                                     0x1
#define HWIO_MSS_QDSP6SS_EXT_WAKEUP_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_EXT_WAKEUP_ADDR,v)
#define HWIO_MSS_QDSP6SS_EXT_WAKEUP_CMD_BMSK                                 0x1
#define HWIO_MSS_QDSP6SS_EXT_WAKEUP_CMD_SHFT                                 0x0

#define HWIO_MSS_QDSP6SS_EXT_PD_ADDR                                  (MSS_QDSP6SS_PUB_REG_BASE      + 0x00001004)
#define HWIO_MSS_QDSP6SS_EXT_PD_RMSK                                         0x1
#define HWIO_MSS_QDSP6SS_EXT_PD_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_EXT_PD_ADDR,v)
#define HWIO_MSS_QDSP6SS_EXT_PD_CMD_BMSK                                     0x1
#define HWIO_MSS_QDSP6SS_EXT_PD_CMD_SHFT                                     0x0

#define HWIO_MSS_QDSP6SS_FLL_CTL_ADDR                                 (MSS_QDSP6SS_PUB_REG_BASE      + 0x00001010)
#define HWIO_MSS_QDSP6SS_FLL_CTL_RMSK                                  0xfffffff
#define HWIO_MSS_QDSP6SS_FLL_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_FLL_CTL_ADDR, HWIO_MSS_QDSP6SS_FLL_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_FLL_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_FLL_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_FLL_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_FLL_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_FLL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_FLL_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_FLL_CTL_IN)
#define HWIO_MSS_QDSP6SS_FLL_CTL_M_BMSK                                0xfff0000
#define HWIO_MSS_QDSP6SS_FLL_CTL_M_SHFT                                     0x10
#define HWIO_MSS_QDSP6SS_FLL_CTL_N_BMSK                                   0xf800
#define HWIO_MSS_QDSP6SS_FLL_CTL_N_SHFT                                      0xb
#define HWIO_MSS_QDSP6SS_FLL_CTL_DCO_INIT_BMSK                             0x7f8
#define HWIO_MSS_QDSP6SS_FLL_CTL_DCO_INIT_SHFT                               0x3
#define HWIO_MSS_QDSP6SS_FLL_CTL_CLK_SEL_BMSK                                0x4
#define HWIO_MSS_QDSP6SS_FLL_CTL_CLK_SEL_SHFT                                0x2
#define HWIO_MSS_QDSP6SS_FLL_CTL_RESET_BMSK                                  0x2
#define HWIO_MSS_QDSP6SS_FLL_CTL_RESET_SHFT                                  0x1
#define HWIO_MSS_QDSP6SS_FLL_CTL_ENA_BMSK                                    0x1
#define HWIO_MSS_QDSP6SS_FLL_CTL_ENA_SHFT                                    0x0

#define HWIO_MSS_QDSP6SS_FLL_STATUS_ADDR                              (MSS_QDSP6SS_PUB_REG_BASE      + 0x00001014)
#define HWIO_MSS_QDSP6SS_FLL_STATUS_RMSK                                   0x1ff
#define HWIO_MSS_QDSP6SS_FLL_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_FLL_STATUS_ADDR, HWIO_MSS_QDSP6SS_FLL_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_FLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_FLL_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_FLL_STATUS_DCO_CURRENT_BMSK                       0x1fe
#define HWIO_MSS_QDSP6SS_FLL_STATUS_DCO_CURRENT_SHFT                         0x1
#define HWIO_MSS_QDSP6SS_FLL_STATUS_LOCKED_BMSK                              0x1
#define HWIO_MSS_QDSP6SS_FLL_STATUS_LOCKED_SHFT                              0x0

#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_ADDR                            (MSS_QDSP6SS_PUB_REG_BASE      + 0x00002000)
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_RMSK                            0xff03ffff
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_TEST_BUS_CTL_ADDR, HWIO_MSS_QDSP6SS_TEST_BUS_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_TEST_BUS_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_TEST_BUS_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_TEST_BUS_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_TEST_BUS_CTL_IN)
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_SPARE_CFG_BMSK                  0xff000000
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_SPARE_CFG_SHFT                        0x18
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_TESTBUS_EN_BMSK                    0x20000
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_TESTBUS_EN_SHFT                       0x11
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_Q6_SEL_BMSK                        0x10000
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_Q6_SEL_SHFT                           0x10
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_SEL_BMSK                            0xffff
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_SEL_SHFT                               0x0

#define HWIO_MSS_QDSP6SS_TEST_BUS_VALUE_ADDR                          (MSS_QDSP6SS_PUB_REG_BASE      + 0x00002004)
#define HWIO_MSS_QDSP6SS_TEST_BUS_VALUE_RMSK                          0xffffffff
#define HWIO_MSS_QDSP6SS_TEST_BUS_VALUE_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_TEST_BUS_VALUE_ADDR, HWIO_MSS_QDSP6SS_TEST_BUS_VALUE_RMSK)
#define HWIO_MSS_QDSP6SS_TEST_BUS_VALUE_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_TEST_BUS_VALUE_ADDR, m)
#define HWIO_MSS_QDSP6SS_TEST_BUS_VALUE_VALUE_BMSK                    0xffffffff
#define HWIO_MSS_QDSP6SS_TEST_BUS_VALUE_VALUE_SHFT                           0x0

#define HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_ADDR                          (MSS_QDSP6SS_PUB_REG_BASE      + 0x00002008)
#define HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_RMSK                          0xffffffff
#define HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_ADDR, HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_RMSK)
#define HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_ADDR, m)
#define HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_ADDR,v)
#define HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_ADDR,m,v,HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_IN)
#define HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_VALUE_BMSK                    0xffffffff
#define HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_VALUE_SHFT                           0x0

#define HWIO_MSS_QDSP6SS_PWRDBG_CTL_ADDR                              (MSS_QDSP6SS_PUB_REG_BASE      + 0x0000200c)
#define HWIO_MSS_QDSP6SS_PWRDBG_CTL_RMSK                                     0x1
#define HWIO_MSS_QDSP6SS_PWRDBG_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PWRDBG_CTL_ADDR, HWIO_MSS_QDSP6SS_PWRDBG_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_PWRDBG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PWRDBG_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_PWRDBG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PWRDBG_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_PWRDBG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PWRDBG_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_PWRDBG_CTL_IN)
#define HWIO_MSS_QDSP6SS_PWRDBG_CTL_ENA_BMSK                                 0x1
#define HWIO_MSS_QDSP6SS_PWRDBG_CTL_ENA_SHFT                                 0x0

#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_ADDR                            (MSS_QDSP6SS_PUB_REG_BASE      + 0x00002010)
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_RMSK                                  0x7f
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_TEST_CLK_CTL_ADDR, HWIO_MSS_QDSP6SS_TEST_CLK_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_TEST_CLK_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_TEST_CLK_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_TEST_CLK_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_TEST_CLK_CTL_IN)
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_EN_BMSK                    0x40
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_EN_SHFT                     0x6
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_INV_BMSK                   0x20
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_INV_SHFT                    0x5
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_DBG_CLK_SEL_BMSK                      0x1f
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_DBG_CLK_SEL_SHFT                       0x0

#define HWIO_MSS_QDSP6SS_PWRDBG_STATUS_ADDR                           (MSS_QDSP6SS_PUB_REG_BASE      + 0x00002014)
#define HWIO_MSS_QDSP6SS_PWRDBG_STATUS_RMSK                           0xffffffff
#define HWIO_MSS_QDSP6SS_PWRDBG_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PWRDBG_STATUS_ADDR, HWIO_MSS_QDSP6SS_PWRDBG_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_PWRDBG_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PWRDBG_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_PWRDBG_STATUS_STATUS_BMSK                    0xffffffff
#define HWIO_MSS_QDSP6SS_PWRDBG_STATUS_STATUS_SHFT                           0x0

#define HWIO_MSS_QDSP6SS_LDO_ATEST_ADDR                               (MSS_QDSP6SS_PUB_REG_BASE      + 0x00002018)
#define HWIO_MSS_QDSP6SS_LDO_ATEST_RMSK                                  0x3ffff
#define HWIO_MSS_QDSP6SS_LDO_ATEST_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_ATEST_ADDR, HWIO_MSS_QDSP6SS_LDO_ATEST_RMSK)
#define HWIO_MSS_QDSP6SS_LDO_ATEST_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_ATEST_ADDR, m)
#define HWIO_MSS_QDSP6SS_LDO_ATEST_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_LDO_ATEST_ADDR,v)
#define HWIO_MSS_QDSP6SS_LDO_ATEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_LDO_ATEST_ADDR,m,v,HWIO_MSS_QDSP6SS_LDO_ATEST_IN)
#define HWIO_MSS_QDSP6SS_LDO_ATEST_ATEST1_EN_BMSK                        0x20000
#define HWIO_MSS_QDSP6SS_LDO_ATEST_ATEST1_EN_SHFT                           0x11
#define HWIO_MSS_QDSP6SS_LDO_ATEST_ATEST0_EN_BMSK                        0x10000
#define HWIO_MSS_QDSP6SS_LDO_ATEST_ATEST0_EN_SHFT                           0x10
#define HWIO_MSS_QDSP6SS_LDO_ATEST_ATEST1_SEL_BMSK                        0xff00
#define HWIO_MSS_QDSP6SS_LDO_ATEST_ATEST1_SEL_SHFT                           0x8
#define HWIO_MSS_QDSP6SS_LDO_ATEST_ATEST0_SEL_BMSK                          0xff
#define HWIO_MSS_QDSP6SS_LDO_ATEST_ATEST0_SEL_SHFT                           0x0

#define HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_ADDR                          (MSS_QDSP6SS_PUB_REG_BASE      + 0x0000201c)
#define HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_RMSK                          0xffffffff
#define HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_ADDR, HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_VALUE_BMSK                    0xffffffff
#define HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_VALUE_SHFT                           0x0

#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ADDR                            (MSS_QDSP6SS_PUB_REG_BASE      + 0x00002020)
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_RMSK                              0xffffff
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ADDR, HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_RMSK)
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ADDR, m)
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ADDR,v)
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ADDR,m,v,HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_IN)
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_COUNT_BMSK                  0xffffe0
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_COUNT_SHFT                       0x5
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_MUX_SEL_BMSK                          0x10
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_MUX_SEL_SHFT                           0x4
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CLK_R_ENA_BMSK                   0x8
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CLK_R_ENA_SHFT                   0x3
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CGC_EN_BMSK                      0x4
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CGC_EN_SHFT                      0x2
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_BYPASS_BMSK                      0x2
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_BYPASS_SHFT                      0x1
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_SW_RESET_BMSK                    0x1
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_SW_RESET_SHFT                    0x0


#endif /* __CLM_HWIO8974_H__ */
