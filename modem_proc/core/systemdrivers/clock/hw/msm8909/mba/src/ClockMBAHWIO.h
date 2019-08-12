#ifndef __CLOCKMBAHWIO_H__
#define __CLOCKMBAHWIO_H__
/*
===========================================================================
*/
/**
  @file ClockMBAHWIO.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    GCC_CLK_CTL_REG
    MSS_PERPH
    MSS_QDSP6SS_PUB
    QDSP6SS_QTMR_F0_0
    TCSR_TCSR_REGS

  'Include' filters applied: QDSP6SS_QTMR_V1_CNTP_TVAL_0[QDSP6SS_QTMR_F0_0] QDSP6SS_QTMR_V1_CNTP_CTL_0[QDSP6SS_QTMR_F0_0] MSS_QDSP6SS_GFMUX_STATUS[MSS_QDSP6SS_PUB] MSS_QDSP6SS_GFMUX_CTL[MSS_QDSP6SS_PUB] MSS_QDSP6SS_CORE_CMD_RCGR[MSS_QDSP6SS_PUB] MSS_QDSP6SS_CORE_CFG_RCGR[MSS_QDSP6SS_PUB] MSS_QDSP6SS_PLL_MODE[MSS_QDSP6SS_PUB] MSS_QDSP6SS_PLL_L_VAL[MSS_QDSP6SS_PUB] MSS_QDSP6SS_PLL_ALPHA_VAL[MSS_QDSP6SS_PUB] MSS_QDSP6SS_PLL_ALPHA_VAL_U[MSS_QDSP6SS_PUB] MSS_QDSP6SS_PLL_USER_CTL[MSS_QDSP6SS_PUB] MSS_QDSP6SS_PLL_USER_CTL_U[MSS_QDSP6SS_PUB] MSS_QDSP6SS_PLL_CONFIG_CTL[MSS_QDSP6SS_PUB] MSS_QDSP6SS_PLL_TEST_CTL_U[MSS_QDSP6SS_PUB] MSS_BUS_CMD_RCGR[MSS_PERPH] MSS_BUS_CFG_RCGR[MSS_PERPH] MSS_NAV_SNOC_AXI_CBCR[MSS_PERPH] MSS_BUS_NAV_CE_BRIDGE_CBCR[MSS_PERPH] MSS_BUS_MODEM_BRIDGE_CX_CBCR[MSS_PERPH] MSS_BUS_CRYPTO_CBCR[MSS_PERPH] MSS_MPLL1_CONFIG_CTL[MSS_PERPH] GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE[GCC_CLK_CTL_REG] GCC_PRNG_AHB_CBCR[GCC_CLK_CTL_REG] GCC_DEHR_CBCR[GCC_CLK_CTL_REG] MSS_BUS_CMD_RCGR[GCC_CLK_CTL_REG] TCSR_SOC_HW_VERSION[TCSR_TCSR_REGS] 
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

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/clock/hw/msm8909/mba/src/ClockMBAHWIO.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

#include <msmhwiobase.h>

/*
 *  definitions
 */
#define PROC_CLK_BRANCH_ENA_VOTE   GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE



/*----------------------------------------------------------------------------
 * MODULE: GCC_CLK_CTL_REG
 *--------------------------------------------------------------------------*/

#define GCC_CLK_CTL_REG_REG_BASE                                                                (CLK_CTL_BASE      + 0x00000000)

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

#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0001b004)
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_RMSK                                                 0x3f7ff
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

/*----------------------------------------------------------------------------
 * MODULE: MSS_PERPH
 *--------------------------------------------------------------------------*/

#define MSS_PERPH_REG_BASE                                                  (MSS_TOP_BASE      + 0x00180000)

#define HWIO_MSS_MPLL1_CONFIG_CTL_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001034)
#define HWIO_MSS_MPLL1_CONFIG_CTL_RMSK                                      0xffffffff
#define HWIO_MSS_MPLL1_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR, HWIO_MSS_MPLL1_CONFIG_CTL_RMSK)
#define HWIO_MSS_MPLL1_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR, m)
#define HWIO_MSS_MPLL1_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR,v)
#define HWIO_MSS_MPLL1_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR,m,v,HWIO_MSS_MPLL1_CONFIG_CTL_IN)
#define HWIO_MSS_MPLL1_CONFIG_CTL_SR2_PLL_FIELDS_BMSK                       0xffffffff
#define HWIO_MSS_MPLL1_CONFIG_CTL_SR2_PLL_FIELDS_SHFT                              0x0

#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_ADDR                                (MSS_PERPH_REG_BASE      + 0x000010a8)
#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_RMSK                                0x80000001
#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_ADDR, HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_RMSK)
#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_ADDR, m)
#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_ADDR,v)
#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_ADDR,m,v,HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_IN)
#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_CLKOFF_BMSK                         0x80000000
#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_CLKOFF_SHFT                               0x1f
#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_CLKEN_BMSK                                 0x1
#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_CLKEN_SHFT                                 0x0

#define HWIO_MSS_BUS_CRYPTO_CBCR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x000010ac)
#define HWIO_MSS_BUS_CRYPTO_CBCR_RMSK                                       0x80000001
#define HWIO_MSS_BUS_CRYPTO_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR, HWIO_MSS_BUS_CRYPTO_CBCR_RMSK)
#define HWIO_MSS_BUS_CRYPTO_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR, m)
#define HWIO_MSS_BUS_CRYPTO_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR,v)
#define HWIO_MSS_BUS_CRYPTO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR,m,v,HWIO_MSS_BUS_CRYPTO_CBCR_IN)
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKOFF_BMSK                                0x80000000
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKOFF_SHFT                                      0x1f
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKEN_BMSK                                        0x1
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKEN_SHFT                                        0x0

#define HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_ADDR                              (MSS_PERPH_REG_BASE      + 0x000010c0)
#define HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_RMSK                              0x80000000
#define HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_ADDR, HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_RMSK)
#define HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_ADDR, m)
#define HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_CLKOFF_BMSK                       0x80000000
#define HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_CLKOFF_SHFT                             0x1f

#define HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR                                     (MSS_PERPH_REG_BASE      + 0x000010dc)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_RMSK                                     0x80000001
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_IN          \
        in_dword_masked(HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR, HWIO_MSS_NAV_SNOC_AXI_CBCR_RMSK)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR, m)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR,v)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR,m,v,HWIO_MSS_NAV_SNOC_AXI_CBCR_IN)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_CLKOFF_BMSK                              0x80000000
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_CLKOFF_SHFT                                    0x1f
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_CLKEN_BMSK                                      0x1
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_CLKEN_SHFT                                      0x0

#define HWIO_MSS_BUS_CMD_RCGR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001108)
#define HWIO_MSS_BUS_CMD_RCGR_RMSK                                          0x80000001
#define HWIO_MSS_BUS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CMD_RCGR_ADDR, HWIO_MSS_BUS_CMD_RCGR_RMSK)
#define HWIO_MSS_BUS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CMD_RCGR_ADDR, m)
#define HWIO_MSS_BUS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CMD_RCGR_ADDR,v)
#define HWIO_MSS_BUS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CMD_RCGR_ADDR,m,v,HWIO_MSS_BUS_CMD_RCGR_IN)
#define HWIO_MSS_BUS_CMD_RCGR_ROOT_OFF_BMSK                                 0x80000000
#define HWIO_MSS_BUS_CMD_RCGR_ROOT_OFF_SHFT                                       0x1f
#define HWIO_MSS_BUS_CMD_RCGR_UPDATE_BMSK                                          0x1
#define HWIO_MSS_BUS_CMD_RCGR_UPDATE_SHFT                                          0x0

#define HWIO_MSS_BUS_CFG_RCGR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x0000110c)
#define HWIO_MSS_BUS_CFG_RCGR_RMSK                                               0x71f
#define HWIO_MSS_BUS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CFG_RCGR_ADDR, HWIO_MSS_BUS_CFG_RCGR_RMSK)
#define HWIO_MSS_BUS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CFG_RCGR_ADDR, m)
#define HWIO_MSS_BUS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CFG_RCGR_ADDR,v)
#define HWIO_MSS_BUS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CFG_RCGR_ADDR,m,v,HWIO_MSS_BUS_CFG_RCGR_IN)
#define HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_BMSK                                       0x700
#define HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_SHFT                                         0x8
#define HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_BMSK                                        0x1f
#define HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_SHFT                                         0x0

/*----------------------------------------------------------------------------
 * MODULE: MSS_QDSP6SS_PUB
 *--------------------------------------------------------------------------*/

#define MSS_QDSP6SS_PUB_REG_BASE                                                (MSS_TOP_BASE      + 0x00080000)

#define HWIO_MSS_QDSP6SS_GFMUX_CTL_ADDR                                         (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000020)
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_RMSK                                              0x30f
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_GFMUX_CTL_ADDR, HWIO_MSS_QDSP6SS_GFMUX_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_GFMUX_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_GFMUX_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_GFMUX_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_GFMUX_CTL_IN)
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_IDLE_CORE_CLK_EN_BMSK                             0x200
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_IDLE_CORE_CLK_EN_SHFT                               0x9
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_SRC_SWITCH_CLK_OVR_BMSK                           0x100
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_SRC_SWITCH_CLK_OVR_SHFT                             0x8
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_CLK_SRC_SEL_BMSK                                    0xc
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_CLK_SRC_SEL_SHFT                                    0x2
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_CLK_ENA_BMSK                                        0x2
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_CLK_ENA_SHFT                                        0x1
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_CLK_ARES_BMSK                                       0x1
#define HWIO_MSS_QDSP6SS_GFMUX_CTL_CLK_ARES_SHFT                                       0x0

#define HWIO_MSS_QDSP6SS_GFMUX_STATUS_ADDR                                      (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000024)
#define HWIO_MSS_QDSP6SS_GFMUX_STATUS_RMSK                                            0x1f
#define HWIO_MSS_QDSP6SS_GFMUX_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_GFMUX_STATUS_ADDR, HWIO_MSS_QDSP6SS_GFMUX_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_GFMUX_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_GFMUX_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_GFMUX_STATUS_SEL_STATUS_BMSK                                 0x1e
#define HWIO_MSS_QDSP6SS_GFMUX_STATUS_SEL_STATUS_SHFT                                  0x1
#define HWIO_MSS_QDSP6SS_GFMUX_STATUS_SWITCH_STATUS_BMSK                               0x1
#define HWIO_MSS_QDSP6SS_GFMUX_STATUS_SWITCH_STATUS_SHFT                               0x0

#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ADDR                                     (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000028)
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_RMSK                                     0x80000013
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ADDR, HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_RMSK)
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ADDR, m)
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ADDR,v)
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ADDR,m,v,HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_IN)
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ROOT_OFF_BMSK                            0x80000000
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ROOT_OFF_SHFT                                  0x1f
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                            0x10
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                             0x4
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ROOT_EN_BMSK                                    0x2
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ROOT_EN_SHFT                                    0x1
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_UPDATE_BMSK                                     0x1
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_UPDATE_SHFT                                     0x0

#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_ADDR                                     (MSS_QDSP6SS_PUB_REG_BASE      + 0x0000002c)
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_RMSK                                          0x71f
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_ADDR, HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_RMSK)
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_ADDR, m)
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_ADDR,v)
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_ADDR,m,v,HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_IN)
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_SRC_SEL_BMSK                                  0x700
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_SRC_SEL_SHFT                                    0x8
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_SRC_DIV_BMSK                                   0x1f
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_SRC_DIV_SHFT                                    0x0

#define HWIO_MSS_QDSP6SS_PLL_MODE_ADDR                                          (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000200)
#define HWIO_MSS_QDSP6SS_PLL_MODE_RMSK                                          0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_MODE_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_MODE_ADDR, HWIO_MSS_QDSP6SS_PLL_MODE_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_MODE_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_MODE_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_MODE_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_MODE_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_MODE_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_MODE_IN)
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_LOCK_DET_BMSK                             0x80000000
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_LOCK_DET_SHFT                                   0x1f
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_ACTIVE_FLAG_BMSK                          0x40000000
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_ACTIVE_FLAG_SHFT                                0x1e
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_ACK_LATCH_BMSK                            0x20000000
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_ACK_LATCH_SHFT                                  0x1d
#define HWIO_MSS_QDSP6SS_PLL_MODE_RESERVE_BITS28_24_BMSK                        0x1f000000
#define HWIO_MSS_QDSP6SS_PLL_MODE_RESERVE_BITS28_24_SHFT                              0x18
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_BMSK                 0x800000
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_SHFT                     0x17
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_UPDATE_BMSK                                 0x400000
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_UPDATE_SHFT                                     0x16
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                         0x200000
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                             0x15
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                           0x100000
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                               0x14
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_BIAS_COUNT_BMSK                              0xfc000
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_BIAS_COUNT_SHFT                                  0xe
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_LOCK_COUNT_BMSK                               0x3f00
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_LOCK_COUNT_SHFT                                  0x8
#define HWIO_MSS_QDSP6SS_PLL_MODE_RESERVE_BITS7_4_BMSK                                0xf0
#define HWIO_MSS_QDSP6SS_PLL_MODE_RESERVE_BITS7_4_SHFT                                 0x4
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_PLLTEST_BMSK                                     0x8
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_PLLTEST_SHFT                                     0x3
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_RESET_N_BMSK                                     0x4
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_RESET_N_SHFT                                     0x2
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_BYPASSNL_BMSK                                    0x2
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_BYPASSNL_SHFT                                    0x1
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_OUTCTRL_BMSK                                     0x1
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_OUTCTRL_SHFT                                     0x0

#define HWIO_MSS_QDSP6SS_PLL_L_VAL_ADDR                                         (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000204)
#define HWIO_MSS_QDSP6SS_PLL_L_VAL_RMSK                                             0xffff
#define HWIO_MSS_QDSP6SS_PLL_L_VAL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_L_VAL_ADDR, HWIO_MSS_QDSP6SS_PLL_L_VAL_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_L_VAL_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_L_VAL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_L_VAL_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_L_VAL_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_L_VAL_IN)
#define HWIO_MSS_QDSP6SS_PLL_L_VAL_PLL_L_BMSK                                       0xffff
#define HWIO_MSS_QDSP6SS_PLL_L_VAL_PLL_L_SHFT                                          0x0

#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_ADDR                                     (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000208)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_RMSK                                     0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_ADDR, HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_IN)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_PLL_ALPHA_31_0_BMSK                      0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_PLL_ALPHA_31_0_SHFT                             0x0

#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_ADDR                                   (MSS_QDSP6SS_PUB_REG_BASE      + 0x0000020c)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_RMSK                                         0xff
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_ADDR, HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_IN)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_PLL_ALPHA_39_32_BMSK                         0xff
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_PLL_ALPHA_39_32_SHFT                          0x0

#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_ADDR                                      (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000210)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RMSK                                      0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_USER_CTL_ADDR, HWIO_MSS_QDSP6SS_PLL_USER_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_USER_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_USER_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_USER_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_USER_CTL_IN)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS31_28_BMSK                    0xf0000000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS31_28_SHFT                          0x1c
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_SSC_MODE_CONTROL_BMSK                      0x8000000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_SSC_MODE_CONTROL_SHFT                           0x1b
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS26_25_BMSK                     0x6000000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS26_25_SHFT                          0x19
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_ALPHA_EN_BMSK                              0x1000000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_ALPHA_EN_SHFT                                   0x18
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS23_22_BMSK                      0xc00000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS23_22_SHFT                          0x16
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_VCO_SEL_BMSK                                0x300000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_VCO_SEL_SHFT                                    0x14
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS19_15_BMSK                       0xf8000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS19_15_SHFT                           0xf
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                            0x7000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                               0xc
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_POST_DIV_RATIO_BMSK                            0xf00
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_POST_DIV_RATIO_SHFT                              0x8
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_OUTPUT_INV_BMSK                                 0x80
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_OUTPUT_INV_SHFT                                  0x7
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS6_5_BMSK                            0x60
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS6_5_SHFT                             0x5
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_TEST_BMSK                             0x10
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_TEST_SHFT                              0x4
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_EARLY_BMSK                             0x8
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_EARLY_SHFT                             0x3
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_AUX2_BMSK                              0x4
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_AUX2_SHFT                              0x2
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_AUX_BMSK                               0x2
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_AUX_SHFT                               0x1
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_MAIN_BMSK                              0x1
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_MAIN_SHFT                              0x0

#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_ADDR                                    (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000214)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_RMSK                                    0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_ADDR, HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_IN)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_CALIBRATION_L_BMSK                      0xffff0000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_CALIBRATION_L_SHFT                            0x10
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_RESERVE_BITS15_12_BMSK                      0xf000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_RESERVE_BITS15_12_SHFT                         0xc
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_BMSK                  0x800
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_SHFT                    0xb
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_STATUS_REGISTER_BMSK                         0x700
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_STATUS_REGISTER_SHFT                           0x8
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_DSM_BMSK                                      0x80
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_DSM_SHFT                                       0x7
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_WRITE_STATE_BMSK                              0x40
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_WRITE_STATE_SHFT                               0x6
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_TARGET_CTL_BMSK                               0x38
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_TARGET_CTL_SHFT                                0x3
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_LOCK_DET_BMSK                                  0x4
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_LOCK_DET_SHFT                                  0x2
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_FREEZE_PLL_BMSK                                0x2
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_FREEZE_PLL_SHFT                                0x1
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_TOGGLE_DET_BMSK                                0x1
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_TOGGLE_DET_SHFT                                0x0

#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_ADDR                                    (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000218)
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_RMSK                                    0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_ADDR, HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_IN)
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_SINGLE_DMET_MODE_ENABLE_BMSK            0x80000000
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_SINGLE_DMET_MODE_ENABLE_SHFT                  0x1f
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_DMET_WINDOW_ENABLE_BMSK                 0x40000000
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_DMET_WINDOW_ENABLE_SHFT                       0x1e
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_INTER_BMSK            0x3c000000
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_INTER_SHFT                  0x1a
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_TOGGLE_DET_THRESHOLD_BMSK                0x3800000
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_TOGGLE_DET_THRESHOLD_SHFT                     0x17
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_BMSK                    0x700000
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_SHFT                        0x14
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_LOCK_DET_THRESHOLD_BMSK                    0xff000
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_LOCK_DET_THRESHOLD_SHFT                        0xc
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_LOCK_DET_SAMPLE_SIZE_BMSK                    0xf00
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_LOCK_DET_SAMPLE_SIZE_SHFT                      0x8
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_GLITCH_THRESHOLD_BMSK                         0xc0
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_GLITCH_THRESHOLD_SHFT                          0x6
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_REF_CYCLE_BMSK                                0x30
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_REF_CYCLE_SHFT                                 0x4
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_KFN_BMSK                                       0xf
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_KFN_SHFT                                       0x0

#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_ADDR                                    (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000220)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_RMSK                                    0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_ADDR, HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_IN)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_RESERVE_BITS31_14_BMSK                  0xffffc000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_RESERVE_BITS31_14_SHFT                         0xe
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_OVERRIDE_FINE_FCW_MSB_BMSK                  0x2000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_OVERRIDE_FINE_FCW_MSB_SHFT                     0xd
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_DTEST_MODE_SEL_BMSK                         0x1800
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_DTEST_MODE_SEL_SHFT                            0xb
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_NMO_OSC_SEL_BMSK                             0x600
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_NMO_OSC_SEL_SHFT                               0x9
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_NMO_EN_BMSK                                  0x100
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_NMO_EN_SHFT                                    0x8
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_NOISE_MAG_BMSK                                0xe0
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_NOISE_MAG_SHFT                                 0x5
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_NOISE_GEN_BMSK                                0x10
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_NOISE_GEN_SHFT                                 0x4
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_OSC_BIAS_GND_BMSK                              0x8
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_OSC_BIAS_GND_SHFT                              0x3
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_BMSK                          0x6
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_SHFT                          0x1
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_CAL_CODE_UPDATE_BMSK                           0x1
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_CAL_CODE_UPDATE_SHFT                           0x0

/*----------------------------------------------------------------------------
 * MODULE: QDSP6SS_QTMR_F0_0
 *--------------------------------------------------------------------------*/

#define QDSP6SS_QTMR_F0_0_REG_BASE                                     (MSS_TOP_BASE      + 0x00121000)

#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_ADDR                          (QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000028)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_RMSK                          0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_CNTP_TVAL_BMSK                0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_CNTP_TVAL_SHFT                       0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_ADDR                           (QDSP6SS_QTMR_F0_0_REG_BASE      + 0x0000002c)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_RMSK                                  0x7
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_ISTAT_BMSK                            0x4
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_ISTAT_SHFT                            0x2
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_IMSK_BMSK                             0x2
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_IMSK_SHFT                             0x1
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_EN_BMSK                               0x1
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_EN_SHFT                               0x0

/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_REGS
 *--------------------------------------------------------------------------*/

#define TCSR_TCSR_REGS_REG_BASE                                                                                           (CORE_TOP_CSR_BASE      + 0x00037000)

#define HWIO_TCSR_SOC_HW_VERSION_ADDR                                                                                     (TCSR_TCSR_REGS_REG_BASE      + 0x00016000)
#define HWIO_TCSR_SOC_HW_VERSION_RMSK                                                                                     0xffffffff
#define HWIO_TCSR_SOC_HW_VERSION_IN          \
        in_dword_masked(HWIO_TCSR_SOC_HW_VERSION_ADDR, HWIO_TCSR_SOC_HW_VERSION_RMSK)
#define HWIO_TCSR_SOC_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_TCSR_SOC_HW_VERSION_ADDR, m)
#define HWIO_TCSR_SOC_HW_VERSION_FAMILY_NUMBER_BMSK                                                                       0xf0000000
#define HWIO_TCSR_SOC_HW_VERSION_FAMILY_NUMBER_SHFT                                                                             0x1c
#define HWIO_TCSR_SOC_HW_VERSION_DEVICE_NUMBER_BMSK                                                                        0xfff0000
#define HWIO_TCSR_SOC_HW_VERSION_DEVICE_NUMBER_SHFT                                                                             0x10
#define HWIO_TCSR_SOC_HW_VERSION_MAJOR_VERSION_BMSK                                                                           0xff00
#define HWIO_TCSR_SOC_HW_VERSION_MAJOR_VERSION_SHFT                                                                              0x8
#define HWIO_TCSR_SOC_HW_VERSION_MINOR_VERSION_BMSK                                                                             0xff
#define HWIO_TCSR_SOC_HW_VERSION_MINOR_VERSION_SHFT                                                                              0x0

#endif /* __CLOCKMBAHWIO_H__ */
