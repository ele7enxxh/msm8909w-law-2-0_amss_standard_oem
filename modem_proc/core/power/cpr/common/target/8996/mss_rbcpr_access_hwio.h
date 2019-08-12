#ifndef __MSS_RBCPR_ACCESS_HWIO_H__
#define __MSS_RBCPR_ACCESS_HWIO_H__
/*
===========================================================================
*/
/**
  @file mss_rbcpr_access_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MSM8996 (Istari) v2 [istari_v2.1_p3q2r16.7]
 
  This file contains HWIO register definitions for the following modules:
    MPSS_PERPH
    MSS_CPR3

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
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

  ===========================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/power/cpr/common/target/8996/mss_rbcpr_access_hwio.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: MPSS_PERPH
 *--------------------------------------------------------------------------*/

#define MPSS_PERPH_REG_BASE                                                   (MODEM_TOP_BASE      + 0x001a8000)

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR                                   (MPSS_PERPH_REG_BASE      + 0x00000000)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_RMSK                                          0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_IN          \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR, HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_RMSK)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_INM(m)      \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR, m)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_OUT(v)      \
        out_dword(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR,v)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR,m,v,HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_IN)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_EN_BMSK                                       0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_EN_SHFT                                       0x0

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR                               (MPSS_PERPH_REG_BASE      + 0x00000004)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_RMSK                                      0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_IN          \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR, HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_RMSK)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR, m)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_STATUS_BMSK                               0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_STATUS_SHFT                               0x0

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_ADDR                                  (MPSS_PERPH_REG_BASE      + 0x00000008)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_RMSK                                         0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_OUT(v)      \
        out_dword(HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_ADDR,v)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_CMD_BMSK                                     0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_CMD_SHFT                                     0x0

#define HWIO_MSS_MSA_CRYPTO_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x0000000c)
#define HWIO_MSS_MSA_CRYPTO_RMSK                                                     0xf
#define HWIO_MSS_MSA_CRYPTO_IN          \
        in_dword_masked(HWIO_MSS_MSA_CRYPTO_ADDR, HWIO_MSS_MSA_CRYPTO_RMSK)
#define HWIO_MSS_MSA_CRYPTO_INM(m)      \
        in_dword_masked(HWIO_MSS_MSA_CRYPTO_ADDR, m)
#define HWIO_MSS_MSA_CRYPTO_OUT(v)      \
        out_dword(HWIO_MSS_MSA_CRYPTO_ADDR,v)
#define HWIO_MSS_MSA_CRYPTO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MSA_CRYPTO_ADDR,m,v,HWIO_MSS_MSA_CRYPTO_IN)
#define HWIO_MSS_MSA_CRYPTO_MSA_CRYPTO_BMSK                                          0xf
#define HWIO_MSS_MSA_CRYPTO_MSA_CRYPTO_SHFT                                          0x0

#define HWIO_MSS_MSA_ADDR                                                     (MPSS_PERPH_REG_BASE      + 0x00000010)
#define HWIO_MSS_MSA_RMSK                                                            0xf
#define HWIO_MSS_MSA_IN          \
        in_dword_masked(HWIO_MSS_MSA_ADDR, HWIO_MSS_MSA_RMSK)
#define HWIO_MSS_MSA_INM(m)      \
        in_dword_masked(HWIO_MSS_MSA_ADDR, m)
#define HWIO_MSS_MSA_OUT(v)      \
        out_dword(HWIO_MSS_MSA_ADDR,v)
#define HWIO_MSS_MSA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MSA_ADDR,m,v,HWIO_MSS_MSA_IN)
#define HWIO_MSS_MSA_CONFIG_UNLOCK2_BMSK                                             0x8
#define HWIO_MSS_MSA_CONFIG_UNLOCK2_SHFT                                             0x3
#define HWIO_MSS_MSA_FORCE_Q6_MSA_BMSK                                               0x4
#define HWIO_MSS_MSA_FORCE_Q6_MSA_SHFT                                               0x2
#define HWIO_MSS_MSA_MBA_OK_BMSK                                                     0x2
#define HWIO_MSS_MSA_MBA_OK_SHFT                                                     0x1
#define HWIO_MSS_MSA_CONFIG_LOCK_BMSK                                                0x1
#define HWIO_MSS_MSA_CONFIG_LOCK_SHFT                                                0x0

#define HWIO_MSS_CLOCK_SPDM_MON_ADDR                                          (MPSS_PERPH_REG_BASE      + 0x00000014)
#define HWIO_MSS_CLOCK_SPDM_MON_RMSK                                                 0x1
#define HWIO_MSS_CLOCK_SPDM_MON_IN          \
        in_dword_masked(HWIO_MSS_CLOCK_SPDM_MON_ADDR, HWIO_MSS_CLOCK_SPDM_MON_RMSK)
#define HWIO_MSS_CLOCK_SPDM_MON_INM(m)      \
        in_dword_masked(HWIO_MSS_CLOCK_SPDM_MON_ADDR, m)
#define HWIO_MSS_CLOCK_SPDM_MON_OUT(v)      \
        out_dword(HWIO_MSS_CLOCK_SPDM_MON_ADDR,v)
#define HWIO_MSS_CLOCK_SPDM_MON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLOCK_SPDM_MON_ADDR,m,v,HWIO_MSS_CLOCK_SPDM_MON_IN)
#define HWIO_MSS_CLOCK_SPDM_MON_BUS_MON_CLKEN_BMSK                                   0x1
#define HWIO_MSS_CLOCK_SPDM_MON_BUS_MON_CLKEN_SHFT                                   0x0

#define HWIO_MSS_DEBUG_CLOCK_CTL_ADDR                                         (MPSS_PERPH_REG_BASE      + 0x00000018)
#define HWIO_MSS_DEBUG_CLOCK_CTL_RMSK                                         0xff8003ff
#define HWIO_MSS_DEBUG_CLOCK_CTL_IN          \
        in_dword_masked(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR, HWIO_MSS_DEBUG_CLOCK_CTL_RMSK)
#define HWIO_MSS_DEBUG_CLOCK_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR, m)
#define HWIO_MSS_DEBUG_CLOCK_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR,v)
#define HWIO_MSS_DEBUG_CLOCK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR,m,v,HWIO_MSS_DEBUG_CLOCK_CTL_IN)
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLL_RESET_SEL_BMSK                           0xe0000000
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLL_RESET_SEL_SHFT                                 0x1d
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLL_BYPASS_SEL_BMSK                          0x1c000000
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLL_BYPASS_SEL_SHFT                                0x1a
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLLOUT_LV_TEST_SEL_BMSK                       0x3800000
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLLOUT_LV_TEST_SEL_SHFT                            0x17
#define HWIO_MSS_DEBUG_CLOCK_CTL_MODEM_MUX_SEL_BMSK                                0x3c0
#define HWIO_MSS_DEBUG_CLOCK_CTL_MODEM_MUX_SEL_SHFT                                  0x6
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL6_MUX_SEL_BMSK                            0x20
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL6_MUX_SEL_SHFT                             0x5
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL5_MUX_SEL_BMSK                            0x10
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL5_MUX_SEL_SHFT                             0x4
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL4_MUX_SEL_BMSK                             0x8
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL4_MUX_SEL_SHFT                             0x3
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL3_MUX_SEL_BMSK                             0x4
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL3_MUX_SEL_SHFT                             0x2
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL2_MUX_SEL_BMSK                             0x2
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL2_MUX_SEL_SHFT                             0x1
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL1_MUX_SEL_BMSK                             0x1
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL1_MUX_SEL_SHFT                             0x0

#define HWIO_MSS_DEBUG_CTL_ADDR                                               (MPSS_PERPH_REG_BASE      + 0x0000001c)
#define HWIO_MSS_DEBUG_CTL_RMSK                                                      0xf
#define HWIO_MSS_DEBUG_CTL_IN          \
        in_dword_masked(HWIO_MSS_DEBUG_CTL_ADDR, HWIO_MSS_DEBUG_CTL_RMSK)
#define HWIO_MSS_DEBUG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DEBUG_CTL_ADDR, m)
#define HWIO_MSS_DEBUG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DEBUG_CTL_ADDR,v)
#define HWIO_MSS_DEBUG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DEBUG_CTL_ADDR,m,v,HWIO_MSS_DEBUG_CTL_IN)
#define HWIO_MSS_DEBUG_CTL_SMMU_BYPASS_CHECKER_DISABLE_BMSK                          0x8
#define HWIO_MSS_DEBUG_CTL_SMMU_BYPASS_CHECKER_DISABLE_SHFT                          0x3
#define HWIO_MSS_DEBUG_CTL_DAC_DISABLE_ON_Q6_DBG_BMSK                                0x4
#define HWIO_MSS_DEBUG_CTL_DAC_DISABLE_ON_Q6_DBG_SHFT                                0x2
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_DBG_BMSK                                  0x2
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_DBG_SHFT                                  0x1
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_WDOG_BMSK                                 0x1
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_WDOG_SHFT                                 0x0

#define HWIO_MSS_DBG_BUS_CTL_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00000020)
#define HWIO_MSS_DBG_BUS_CTL_RMSK                                                0x7ffff
#define HWIO_MSS_DBG_BUS_CTL_IN          \
        in_dword_masked(HWIO_MSS_DBG_BUS_CTL_ADDR, HWIO_MSS_DBG_BUS_CTL_RMSK)
#define HWIO_MSS_DBG_BUS_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DBG_BUS_CTL_ADDR, m)
#define HWIO_MSS_DBG_BUS_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DBG_BUS_CTL_ADDR,v)
#define HWIO_MSS_DBG_BUS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DBG_BUS_CTL_ADDR,m,v,HWIO_MSS_DBG_BUS_CTL_IN)
#define HWIO_MSS_DBG_BUS_CTL_FABRIC_BRIDGES_SEL_BMSK                             0x70000
#define HWIO_MSS_DBG_BUS_CTL_FABRIC_BRIDGES_SEL_SHFT                                0x10
#define HWIO_MSS_DBG_BUS_CTL_AXI2AXI_DMA_SEL_BMSK                                 0xe000
#define HWIO_MSS_DBG_BUS_CTL_AXI2AXI_DMA_SEL_SHFT                                    0xd
#define HWIO_MSS_DBG_BUS_CTL_AHB2AXI_NAV_SEL_BMSK                                 0x1c00
#define HWIO_MSS_DBG_BUS_CTL_AHB2AXI_NAV_SEL_SHFT                                    0xa
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_1X1_CNOC_SEL_BMSK                             0x300
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_1X1_CNOC_SEL_SHFT                               0x8
#define HWIO_MSS_DBG_BUS_CTL_MODEM_DBG_BUS_SEL_BMSK                                 0xf0
#define HWIO_MSS_DBG_BUS_CTL_MODEM_DBG_BUS_SEL_SHFT                                  0x4
#define HWIO_MSS_DBG_BUS_CTL_MPSS_DBG_BUS_SEL_BMSK                                   0xf
#define HWIO_MSS_DBG_BUS_CTL_MPSS_DBG_BUS_SEL_SHFT                                   0x0

#define HWIO_MSS_NAV_AXI_HALTREQ_ADDR                                         (MPSS_PERPH_REG_BASE      + 0x00000024)
#define HWIO_MSS_NAV_AXI_HALTREQ_RMSK                                                0x7
#define HWIO_MSS_NAV_AXI_HALTREQ_IN          \
        in_dword_masked(HWIO_MSS_NAV_AXI_HALTREQ_ADDR, HWIO_MSS_NAV_AXI_HALTREQ_RMSK)
#define HWIO_MSS_NAV_AXI_HALTREQ_INM(m)      \
        in_dword_masked(HWIO_MSS_NAV_AXI_HALTREQ_ADDR, m)
#define HWIO_MSS_NAV_AXI_HALTREQ_OUT(v)      \
        out_dword(HWIO_MSS_NAV_AXI_HALTREQ_ADDR,v)
#define HWIO_MSS_NAV_AXI_HALTREQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_NAV_AXI_HALTREQ_ADDR,m,v,HWIO_MSS_NAV_AXI_HALTREQ_IN)
#define HWIO_MSS_NAV_AXI_HALTREQ_IDLE_BMSK                                           0x4
#define HWIO_MSS_NAV_AXI_HALTREQ_IDLE_SHFT                                           0x2
#define HWIO_MSS_NAV_AXI_HALTREQ_HALTACK_BMSK                                        0x2
#define HWIO_MSS_NAV_AXI_HALTREQ_HALTACK_SHFT                                        0x1
#define HWIO_MSS_NAV_AXI_HALTREQ_HALTREQ_BMSK                                        0x1
#define HWIO_MSS_NAV_AXI_HALTREQ_HALTREQ_SHFT                                        0x0

#define HWIO_MSS_BIMC_DANGER_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00000028)
#define HWIO_MSS_BIMC_DANGER_RMSK                                                    0x7
#define HWIO_MSS_BIMC_DANGER_IN          \
        in_dword_masked(HWIO_MSS_BIMC_DANGER_ADDR, HWIO_MSS_BIMC_DANGER_RMSK)
#define HWIO_MSS_BIMC_DANGER_INM(m)      \
        in_dword_masked(HWIO_MSS_BIMC_DANGER_ADDR, m)
#define HWIO_MSS_BIMC_DANGER_OUT(v)      \
        out_dword(HWIO_MSS_BIMC_DANGER_ADDR,v)
#define HWIO_MSS_BIMC_DANGER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIMC_DANGER_ADDR,m,v,HWIO_MSS_BIMC_DANGER_IN)
#define HWIO_MSS_BIMC_DANGER_MSS_SAVE_BMSK                                           0x4
#define HWIO_MSS_BIMC_DANGER_MSS_SAVE_SHFT                                           0x2
#define HWIO_MSS_BIMC_DANGER_MSS_DANGER_BMSK                                         0x3
#define HWIO_MSS_BIMC_DANGER_MSS_DANGER_SHFT                                         0x0

#define HWIO_MSS_HW_VERSION_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x0000002c)
#define HWIO_MSS_HW_VERSION_RMSK                                              0xffffffff
#define HWIO_MSS_HW_VERSION_IN          \
        in_dword_masked(HWIO_MSS_HW_VERSION_ADDR, HWIO_MSS_HW_VERSION_RMSK)
#define HWIO_MSS_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_MSS_HW_VERSION_ADDR, m)
#define HWIO_MSS_HW_VERSION_MAJOR_BMSK                                        0xf0000000
#define HWIO_MSS_HW_VERSION_MAJOR_SHFT                                              0x1c
#define HWIO_MSS_HW_VERSION_MINOR_BMSK                                         0xfff0000
#define HWIO_MSS_HW_VERSION_MINOR_SHFT                                              0x10
#define HWIO_MSS_HW_VERSION_STEP_BMSK                                             0xffff
#define HWIO_MSS_HW_VERSION_STEP_SHFT                                                0x0

#define HWIO_MSS_RELAY_MSG_SHADOW0_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x00000400)
#define HWIO_MSS_RELAY_MSG_SHADOW0_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW0_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR, HWIO_MSS_RELAY_MSG_SHADOW0_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW0_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW0_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW0_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW0_RELAY_MSG_SHADOW0_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW0_RELAY_MSG_SHADOW0_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW1_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x00000404)
#define HWIO_MSS_RELAY_MSG_SHADOW1_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW1_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR, HWIO_MSS_RELAY_MSG_SHADOW1_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW1_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW1_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW1_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW1_RELAY_MSG_SHADOW1_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW1_RELAY_MSG_SHADOW1_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW2_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x00000408)
#define HWIO_MSS_RELAY_MSG_SHADOW2_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW2_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR, HWIO_MSS_RELAY_MSG_SHADOW2_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW2_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW2_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW2_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW2_RELAY_MSG_SHADOW2_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW2_RELAY_MSG_SHADOW2_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW3_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x0000040c)
#define HWIO_MSS_RELAY_MSG_SHADOW3_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW3_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR, HWIO_MSS_RELAY_MSG_SHADOW3_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW3_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW3_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW3_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW3_RELAY_MSG_SHADOW3_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW3_RELAY_MSG_SHADOW3_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW4_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x00000410)
#define HWIO_MSS_RELAY_MSG_SHADOW4_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW4_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR, HWIO_MSS_RELAY_MSG_SHADOW4_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW4_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW4_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW4_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW4_RELAY_MSG_SHADOW4_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW4_RELAY_MSG_SHADOW4_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW5_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x00000414)
#define HWIO_MSS_RELAY_MSG_SHADOW5_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW5_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR, HWIO_MSS_RELAY_MSG_SHADOW5_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW5_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW5_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW5_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW5_RELAY_MSG_SHADOW5_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW5_RELAY_MSG_SHADOW5_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW6_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x00000418)
#define HWIO_MSS_RELAY_MSG_SHADOW6_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW6_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR, HWIO_MSS_RELAY_MSG_SHADOW6_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW6_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW6_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW6_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW6_RELAY_MSG_SHADOW6_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW6_RELAY_MSG_SHADOW6_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW7_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x0000041c)
#define HWIO_MSS_RELAY_MSG_SHADOW7_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW7_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR, HWIO_MSS_RELAY_MSG_SHADOW7_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW7_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW7_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW7_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW7_RELAY_MSG_SHADOW7_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW7_RELAY_MSG_SHADOW7_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW8_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x00000420)
#define HWIO_MSS_RELAY_MSG_SHADOW8_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW8_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR, HWIO_MSS_RELAY_MSG_SHADOW8_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW8_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW8_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW8_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW8_RELAY_MSG_SHADOW8_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW8_RELAY_MSG_SHADOW8_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW9_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x00000424)
#define HWIO_MSS_RELAY_MSG_SHADOW9_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW9_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW9_ADDR, HWIO_MSS_RELAY_MSG_SHADOW9_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW9_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW9_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW9_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW9_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW9_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW9_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW9_RELAY_MSG_SHADOW9_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW9_RELAY_MSG_SHADOW9_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW10_ADDR                                      (MPSS_PERPH_REG_BASE      + 0x00000428)
#define HWIO_MSS_RELAY_MSG_SHADOW10_RMSK                                      0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW10_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW10_ADDR, HWIO_MSS_RELAY_MSG_SHADOW10_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW10_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW10_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW10_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW10_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW10_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW10_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW10_RELAY_MSG_SHADOW10_BMSK                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW10_RELAY_MSG_SHADOW10_SHFT                          0x0

#define HWIO_MSS_RELAY_MSG_SHADOW11_ADDR                                      (MPSS_PERPH_REG_BASE      + 0x0000042c)
#define HWIO_MSS_RELAY_MSG_SHADOW11_RMSK                                      0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW11_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW11_ADDR, HWIO_MSS_RELAY_MSG_SHADOW11_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW11_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW11_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW11_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW11_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW11_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW11_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW11_RELAY_MSG_SHADOW11_BMSK                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW11_RELAY_MSG_SHADOW11_SHFT                          0x0

#define HWIO_MSS_RELAY_MSG_SHADOW12_ADDR                                      (MPSS_PERPH_REG_BASE      + 0x00000430)
#define HWIO_MSS_RELAY_MSG_SHADOW12_RMSK                                      0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW12_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW12_ADDR, HWIO_MSS_RELAY_MSG_SHADOW12_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW12_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW12_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW12_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW12_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW12_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW12_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW12_RELAY_MSG_SHADOW12_BMSK                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW12_RELAY_MSG_SHADOW12_SHFT                          0x0

#define HWIO_MSS_RELAY_MSG_SHADOW13_ADDR                                      (MPSS_PERPH_REG_BASE      + 0x00000434)
#define HWIO_MSS_RELAY_MSG_SHADOW13_RMSK                                      0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW13_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW13_ADDR, HWIO_MSS_RELAY_MSG_SHADOW13_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW13_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW13_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW13_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW13_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW13_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW13_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW13_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW13_RELAY_MSG_SHADOW13_BMSK                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW13_RELAY_MSG_SHADOW13_SHFT                          0x0

#define HWIO_MSS_RELAY_MSG_SHADOW14_ADDR                                      (MPSS_PERPH_REG_BASE      + 0x00000438)
#define HWIO_MSS_RELAY_MSG_SHADOW14_RMSK                                      0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW14_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW14_ADDR, HWIO_MSS_RELAY_MSG_SHADOW14_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW14_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW14_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW14_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW14_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW14_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW14_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW14_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW14_RELAY_MSG_SHADOW14_BMSK                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW14_RELAY_MSG_SHADOW14_SHFT                          0x0

#define HWIO_MSS_RELAY_MSG_SHADOW15_ADDR                                      (MPSS_PERPH_REG_BASE      + 0x0000043c)
#define HWIO_MSS_RELAY_MSG_SHADOW15_RMSK                                      0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW15_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW15_ADDR, HWIO_MSS_RELAY_MSG_SHADOW15_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW15_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW15_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW15_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW15_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW15_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW15_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW15_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW15_RELAY_MSG_SHADOW15_BMSK                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW15_RELAY_MSG_SHADOW15_SHFT                          0x0

#define HWIO_MSS_OFFLINE_PLL_MODE_ADDR                                        (MPSS_PERPH_REG_BASE      + 0x00001000)
#define HWIO_MSS_OFFLINE_PLL_MODE_RMSK                                        0xffffffff
#define HWIO_MSS_OFFLINE_PLL_MODE_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_MODE_ADDR, HWIO_MSS_OFFLINE_PLL_MODE_RMSK)
#define HWIO_MSS_OFFLINE_PLL_MODE_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_MODE_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_MODE_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_MODE_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_MODE_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_MODE_IN)
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_LOCK_DET_BMSK                           0x80000000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_LOCK_DET_SHFT                                 0x1f
#define HWIO_MSS_OFFLINE_PLL_MODE_RESERVE_BIT30_BMSK                          0x40000000
#define HWIO_MSS_OFFLINE_PLL_MODE_RESERVE_BIT30_SHFT                                0x1e
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_ACK_LATCH_BMSK                          0x20000000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_ACK_LATCH_SHFT                                0x1d
#define HWIO_MSS_OFFLINE_PLL_MODE_RESERVE_BIT28_24_BMSK                       0x1f000000
#define HWIO_MSS_OFFLINE_PLL_MODE_RESERVE_BIT28_24_SHFT                             0x18
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_BMSK               0x800000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_SHFT                   0x17
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_UPDATE_BMSK                               0x400000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_UPDATE_SHFT                                   0x16
#define HWIO_MSS_OFFLINE_PLL_MODE_RESERVE_BIT21_4_BMSK                          0x3ffff0
#define HWIO_MSS_OFFLINE_PLL_MODE_RESERVE_BIT21_4_SHFT                               0x4
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_PLLTEST_BMSK                                   0x8
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_PLLTEST_SHFT                                   0x3
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_RESET_N_BMSK                                   0x4
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_RESET_N_SHFT                                   0x2
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_BYPASSNL_BMSK                                  0x2
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_BYPASSNL_SHFT                                  0x1
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_OUTCTRL_BMSK                                   0x1
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_OUTCTRL_SHFT                                   0x0

#define HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x00001004)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_RMSK                                           0xffff
#define HWIO_MSS_OFFLINE_PLL_L_VAL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR, HWIO_MSS_OFFLINE_PLL_L_VAL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_L_VAL_IN)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_PLL_L_BMSK                                     0xffff
#define HWIO_MSS_OFFLINE_PLL_L_VAL_PLL_L_SHFT                                        0x0

#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_ADDR                                   (MPSS_PERPH_REG_BASE      + 0x00001008)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_RMSK                                   0xffffffff
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_ADDR, HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_IN)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_PLL_ALPHA_31_0_BMSK                    0xffffffff
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_PLL_ALPHA_31_0_SHFT                           0x0

#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_ADDR                                 (MPSS_PERPH_REG_BASE      + 0x0000100c)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_RMSK                                       0xff
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_ADDR, HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_RMSK)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_IN)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_PLL_ALPHA_39_32_BMSK                       0xff
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_PLL_ALPHA_39_32_SHFT                        0x0

#define HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR                                    (MPSS_PERPH_REG_BASE      + 0x00001010)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RMSK                                    0xffffffff
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR, HWIO_MSS_OFFLINE_PLL_USER_CTL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_USER_CTL_IN)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BIT31_28_BMSK                   0xf0000000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BIT31_28_SHFT                         0x1c
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_SSC_MODE_CONTROL_BMSK                    0x8000000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_SSC_MODE_CONTROL_SHFT                         0x1b
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS26_25_BMSK                   0x6000000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS26_25_SHFT                        0x19
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_ALPHA_EN_BMSK                            0x1000000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_ALPHA_EN_SHFT                                 0x18
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS23_22_BMSK                    0xc00000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS23_22_SHFT                        0x16
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_VCO_SEL_BMSK                              0x300000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_VCO_SEL_SHFT                                  0x14
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS19_15_BMSK                     0xf8000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS19_15_SHFT                         0xf
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                          0x7000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                             0xc
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_POST_DIV_RATIO_BMSK                          0xf00
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_POST_DIV_RATIO_SHFT                            0x8
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_OUTPUT_INV_BMSK                               0x80
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_OUTPUT_INV_SHFT                                0x7
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS6_5_BMSK                          0x60
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS6_5_SHFT                           0x5
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_TEST_BMSK                           0x10
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_TEST_SHFT                            0x4
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_EARLY_BMSK                           0x8
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_EARLY_SHFT                           0x3
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_AUX2_BMSK                            0x4
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_AUX2_SHFT                            0x2
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_AUX_BMSK                             0x2
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_AUX_SHFT                             0x1
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_MAIN_BMSK                            0x1
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_MAIN_SHFT                            0x0

#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR                                  (MPSS_PERPH_REG_BASE      + 0x00001014)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_RMSK                                  0xffffffff
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR, HWIO_MSS_OFFLINE_PLL_USER_CTL_U_RMSK)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_USER_CTL_U_IN)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_RESERVE_BITS31_12_BMSK                0xfffff000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_RESERVE_BITS31_12_SHFT                       0xc
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_LATCH_INTERFACE_BMSK                       0x800
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_LATCH_INTERFACE_SHFT                         0xb
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_STATUS_REGISTER_BMSK                       0x700
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_STATUS_REGISTER_SHFT                         0x8
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_DSM_BMSK                                    0x80
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_DSM_SHFT                                     0x7
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_WRITE_STATE_BMSK                            0x40
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_WRITE_STATE_SHFT                             0x6
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_TARGET_CTL_BMSK                             0x38
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_TARGET_CTL_SHFT                              0x3
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_LOCKDET_BMSK                                 0x4
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_LOCKDET_SHFT                                 0x2
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_FREEZE_PLL_BMSK                              0x2
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_FREEZE_PLL_SHFT                              0x1
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_TOGGLE_DET_BMSK                              0x1
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_TOGGLE_DET_SHFT                              0x0

#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR                                  (MPSS_PERPH_REG_BASE      + 0x00001018)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_RMSK                                  0xffffffff
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR, HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_IN)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_SINGLE_DMET_MODE_ENABLE_BMSK          0x80000000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_SINGLE_DMET_MODE_ENABLE_SHFT                0x1f
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_DMET_WINDOW_ENABLE_BMSK               0x40000000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_DMET_WINDOW_ENABLE_SHFT                     0x1e
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_INTER_BMSK          0x3c000000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_INTER_SHFT                0x1a
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_TOGGLE_DET_THRESHOLD_BMSK              0x3800000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_TOGGLE_DET_THRESHOLD_SHFT                   0x17
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_BMSK                  0x700000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_SHFT                      0x14
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_LOCK_DET_THRESHOLD_BMSK                  0xff000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_LOCK_DET_THRESHOLD_SHFT                      0xc
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_LOCK_DET_SAMPLE_SIZE_BMSK                  0xf00
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_LOCK_DET_SAMPLE_SIZE_SHFT                    0x8
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_RESERVE_BITS7_6_BMSK                        0xc0
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_RESERVE_BITS7_6_SHFT                         0x6
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_REF_CYCLE_BMSK                              0x30
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_REF_CYCLE_SHFT                               0x4
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_KFN_BMSK                                     0xf
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_KFN_SHFT                                     0x0

#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR                                    (MPSS_PERPH_REG_BASE      + 0x0000101c)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_RMSK                                    0xffffffff
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR, HWIO_MSS_OFFLINE_PLL_TEST_CTL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_TEST_CTL_IN)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_BIAS_GEN_TRIM_BMSK                      0xe0000000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_BIAS_GEN_TRIM_SHFT                            0x1d
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_PROCESS_CALB_BMSK                       0x1c000000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_PROCESS_CALB_SHFT                             0x1a
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_PROCESS_CALB_BMSK               0x2000000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_PROCESS_CALB_SHFT                    0x19
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_FINE_FCW_BMSK                            0x1f00000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_FINE_FCW_SHFT                                 0x14
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_FINE_FCW_BMSK                     0x80000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_FINE_FCW_SHFT                        0x13
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_COARSE_FCW_BMSK                            0x7e000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_COARSE_FCW_SHFT                                0xd
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_COARSE_BMSK                        0x1000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_COARSE_SHFT                           0xc
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DISABLE_LFSR_BMSK                            0x800
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DISABLE_LFSR_SHFT                              0xb
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DTEST_SEL_BMSK                               0x700
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DTEST_SEL_SHFT                                 0x8
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DTEST_EN_BMSK                                 0x80
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DTEST_EN_SHFT                                  0x7
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_BYP_TESTAMP_BMSK                              0x40
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_BYP_TESTAMP_SHFT                               0x6
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST1_SEL_BMSK                               0x30
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST1_SEL_SHFT                                0x4
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST0_SEL_BMSK                                0xc
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST0_SEL_SHFT                                0x2
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST1_EN_BMSK                                 0x2
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST1_EN_SHFT                                 0x1
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST0_EN_BMSK                                 0x1
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST0_EN_SHFT                                 0x0

#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR                                  (MPSS_PERPH_REG_BASE      + 0x00001020)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_RMSK                                  0xfffffeff
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR, HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_RMSK)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_IN)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_RESERVE_BIT31_22_BMSK                 0xffc00000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_RESERVE_BIT31_22_SHFT                       0x16
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_GLITCH_DET_CNT_BMSK                     0x300000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_GLITCH_DET_CNT_SHFT                         0x14
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DISABLE_GLITCH_DET_BMSK                  0x80000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DISABLE_GLITCH_DET_SHFT                     0x13
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DTEST_MODE_SEL_EXT_BMSK                  0x60000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DTEST_MODE_SEL_EXT_SHFT                     0x11
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DITHER_SEL_BMSK                          0x18000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DITHER_SEL_SHFT                              0xf
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_PROCESS_CALB_SEL_BMSK                     0x4000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_PROCESS_CALB_SEL_SHFT                        0xe
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OVERRIDE_FINE_FCW_MSB_BMSK                0x2000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OVERRIDE_FINE_FCW_MSB_SHFT                   0xd
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DTEST_MODE_SEL_BMSK                       0x1800
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DTEST_MODE_SEL_SHFT                          0xb
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NMO_OSC_SEL_BMSK                           0x600
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NMO_OSC_SEL_SHFT                             0x9
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NOISE_MAG_BMSK                              0xe0
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NOISE_MAG_SHFT                               0x5
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NOISE_GEN_BMSK                              0x10
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NOISE_GEN_SHFT                               0x4
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OSC_BIAS_GND_BMSK                            0x8
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OSC_BIAS_GND_SHFT                            0x3
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_BMSK                        0x6
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_SHFT                        0x1
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_CAL_CODE_UPDATE_BMSK                         0x1
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_CAL_CODE_UPDATE_SHFT                         0x0

#define HWIO_MSS_OFFLINE_PLL_STATUS_ADDR                                      (MPSS_PERPH_REG_BASE      + 0x00001024)
#define HWIO_MSS_OFFLINE_PLL_STATUS_RMSK                                      0xffffffff
#define HWIO_MSS_OFFLINE_PLL_STATUS_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_STATUS_ADDR, HWIO_MSS_OFFLINE_PLL_STATUS_RMSK)
#define HWIO_MSS_OFFLINE_PLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_STATUS_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_STATUS_STATUS_31_0_BMSK                          0xffffffff
#define HWIO_MSS_OFFLINE_PLL_STATUS_STATUS_31_0_SHFT                                 0x0

#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR                                    (MPSS_PERPH_REG_BASE      + 0x00001028)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_RMSK                                    0xffffffff
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR, HWIO_MSS_OFFLINE_PLL_FREQ_CTL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_FREQ_CTL_IN)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_PLL_FREQ_CONTROL_WORD_BMSK              0xffffffff
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_PLL_FREQ_CONTROL_WORD_SHFT                     0x0

#define HWIO_MSS_CLAMP_MEM_ADDR                                               (MPSS_PERPH_REG_BASE      + 0x0000102c)
#define HWIO_MSS_CLAMP_MEM_RMSK                                                      0xf
#define HWIO_MSS_CLAMP_MEM_IN          \
        in_dword_masked(HWIO_MSS_CLAMP_MEM_ADDR, HWIO_MSS_CLAMP_MEM_RMSK)
#define HWIO_MSS_CLAMP_MEM_INM(m)      \
        in_dword_masked(HWIO_MSS_CLAMP_MEM_ADDR, m)
#define HWIO_MSS_CLAMP_MEM_OUT(v)      \
        out_dword(HWIO_MSS_CLAMP_MEM_ADDR,v)
#define HWIO_MSS_CLAMP_MEM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLAMP_MEM_ADDR,m,v,HWIO_MSS_CLAMP_MEM_IN)
#define HWIO_MSS_CLAMP_MEM_UNCLAMP_ALL_BMSK                                          0x8
#define HWIO_MSS_CLAMP_MEM_UNCLAMP_ALL_SHFT                                          0x3
#define HWIO_MSS_CLAMP_MEM_SPARE_2_TO_1_BMSK                                         0x6
#define HWIO_MSS_CLAMP_MEM_SPARE_2_TO_1_SHFT                                         0x1
#define HWIO_MSS_CLAMP_MEM_CLAMP_MEM_BMSK                                            0x1
#define HWIO_MSS_CLAMP_MEM_CLAMP_MEM_SHFT                                            0x0

#define HWIO_MSS_CLAMP_IO_ADDR                                                (MPSS_PERPH_REG_BASE      + 0x00001030)
#define HWIO_MSS_CLAMP_IO_RMSK                                                       0xf
#define HWIO_MSS_CLAMP_IO_IN          \
        in_dword_masked(HWIO_MSS_CLAMP_IO_ADDR, HWIO_MSS_CLAMP_IO_RMSK)
#define HWIO_MSS_CLAMP_IO_INM(m)      \
        in_dword_masked(HWIO_MSS_CLAMP_IO_ADDR, m)
#define HWIO_MSS_CLAMP_IO_OUT(v)      \
        out_dword(HWIO_MSS_CLAMP_IO_ADDR,v)
#define HWIO_MSS_CLAMP_IO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLAMP_IO_ADDR,m,v,HWIO_MSS_CLAMP_IO_IN)
#define HWIO_MSS_CLAMP_IO_UNCLAMP_ALL_BMSK                                           0x8
#define HWIO_MSS_CLAMP_IO_UNCLAMP_ALL_SHFT                                           0x3
#define HWIO_MSS_CLAMP_IO_SPARE_2_TO_1_BMSK                                          0x6
#define HWIO_MSS_CLAMP_IO_SPARE_2_TO_1_SHFT                                          0x1
#define HWIO_MSS_CLAMP_IO_MODEM_BMSK                                                 0x1
#define HWIO_MSS_CLAMP_IO_MODEM_SHFT                                                 0x0

#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR                                    (MPSS_PERPH_REG_BASE      + 0x00001034)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_RMSK                                    0xffffffff
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR, HWIO_MSS_OFFLINE_MEM_SLP_CNTL_RMSK)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR, m)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR,v)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR,m,v,HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IN)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_CLUSTER_D_SLP_NRET_N_BMSK               0xc0000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_CLUSTER_D_SLP_NRET_N_SHFT                     0x1e
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_CLUSTER_D_SLP_RET_N_BMSK                0x20000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_CLUSTER_D_SLP_RET_N_SHFT                      0x1d
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_CLUSTER_C_SLP_NRET_N_BMSK               0x18000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_CLUSTER_C_SLP_NRET_N_SHFT                     0x1b
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_CLUSTER_C_SLP_RET_N_BMSK                 0x4000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_CLUSTER_C_SLP_RET_N_SHFT                      0x1a
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_CLUSTER_B0_SLP_NRET_N_BMSK               0x3000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_CLUSTER_B0_SLP_NRET_N_SHFT                    0x18
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_CLUSTER_B0_SLP_RET_N_BMSK                 0x800000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_CLUSTER_B0_SLP_RET_N_SHFT                     0x17
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_CLUSTER_A_SLP_NRET_N_BMSK                 0x600000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_CLUSTER_A_SLP_NRET_N_SHFT                     0x15
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_CLUSTER_A_SLP_RET_N_BMSK                  0x100000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_CLUSTER_A_SLP_RET_N_SHFT                      0x14
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SLICE_D_DMEM_NRET_N_BMSK                   0xc0000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SLICE_D_DMEM_NRET_N_SHFT                      0x12
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SLICE_D_DMEM_RET_N_BMSK                    0x30000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SLICE_D_DMEM_RET_N_SHFT                       0x10
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SLICE_C_DMEM_NRET_N_BMSK                    0xc000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SLICE_C_DMEM_NRET_N_SHFT                       0xe
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SLICE_C_DMEM_RET_N_BMSK                     0x3000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SLICE_C_DMEM_RET_N_SHFT                        0xc
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SLICE_B0_DMEM_NRET_N_BMSK                    0xc00
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SLICE_B0_DMEM_NRET_N_SHFT                      0xa
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SLICE_B0_DMEM_RET_N_BMSK                     0x300
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SLICE_B0_DMEM_RET_N_SHFT                       0x8
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SLICE_A_DMEM_NRET_N_BMSK                      0xe0
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SLICE_A_DMEM_NRET_N_SHFT                       0x5
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SLICE_A_DMEM_RET_N_BMSK                       0x1c
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SLICE_A_DMEM_RET_N_SHFT                        0x2
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_DB_CTL_PDMEM_SLP_NRET_N_BMSK                   0x2
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_DB_CTL_PDMEM_SLP_NRET_N_SHFT                   0x1
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_DB_CTL_PDMEM_SLP_RET_N_BMSK                    0x1
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_DB_CTL_PDMEM_SLP_RET_N_SHFT                    0x0

#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR                                   (MPSS_PERPH_REG_BASE      + 0x00001038)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_RMSK                                   0xffffffff
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR, HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_RMSK)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR, m)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR,v)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR,m,v,HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_IN)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_SPARE_31_12_BMSK                       0xfffff000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_SPARE_31_12_SHFT                              0xc
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLP_NRET_N_BMSK                        0xf00
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLP_NRET_N_SHFT                          0x8
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLP_RET_N_BMSK                          0xf0
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLP_RET_N_SHFT                           0x4
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_COMMON_NRET_N_BMSK                            0x8
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_COMMON_NRET_N_SHFT                            0x3
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_COMMON_RET_N_BMSK                             0x4
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_COMMON_RET_N_SHFT                             0x2
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_SPARE_1_0_BMSK                                0x3
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_SPARE_1_0_SHFT                                0x0

#define HWIO_MSS_MODEM_BCR_ADDR                                               (MPSS_PERPH_REG_BASE      + 0x0000103c)
#define HWIO_MSS_MODEM_BCR_RMSK                                               0x80000001
#define HWIO_MSS_MODEM_BCR_IN          \
        in_dword_masked(HWIO_MSS_MODEM_BCR_ADDR, HWIO_MSS_MODEM_BCR_RMSK)
#define HWIO_MSS_MODEM_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MODEM_BCR_ADDR, m)
#define HWIO_MSS_MODEM_BCR_OUT(v)      \
        out_dword(HWIO_MSS_MODEM_BCR_ADDR,v)
#define HWIO_MSS_MODEM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MODEM_BCR_ADDR,m,v,HWIO_MSS_MODEM_BCR_IN)
#define HWIO_MSS_MODEM_BCR_STATUS_BMSK                                        0x80000000
#define HWIO_MSS_MODEM_BCR_STATUS_SHFT                                              0x1f
#define HWIO_MSS_MODEM_BCR_BLK_ARES_BMSK                                             0x1
#define HWIO_MSS_MODEM_BCR_BLK_ARES_SHFT                                             0x0

#define HWIO_MSS_AXI_MNOC_CBCR_ADDR                                           (MPSS_PERPH_REG_BASE      + 0x00001040)
#define HWIO_MSS_AXI_MNOC_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_AXI_MNOC_CBCR_IN          \
        in_dword_masked(HWIO_MSS_AXI_MNOC_CBCR_ADDR, HWIO_MSS_AXI_MNOC_CBCR_RMSK)
#define HWIO_MSS_AXI_MNOC_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_AXI_MNOC_CBCR_ADDR, m)
#define HWIO_MSS_AXI_MNOC_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_AXI_MNOC_CBCR_ADDR,v)
#define HWIO_MSS_AXI_MNOC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AXI_MNOC_CBCR_ADDR,m,v,HWIO_MSS_AXI_MNOC_CBCR_IN)
#define HWIO_MSS_AXI_MNOC_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_AXI_MNOC_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_AXI_MNOC_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_AXI_MNOC_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_XO_MDM_CBCR_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00001044)
#define HWIO_MSS_XO_MDM_CBCR_RMSK                                             0x80000001
#define HWIO_MSS_XO_MDM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_MDM_CBCR_ADDR, HWIO_MSS_XO_MDM_CBCR_RMSK)
#define HWIO_MSS_XO_MDM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_MDM_CBCR_ADDR, m)
#define HWIO_MSS_XO_MDM_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_MDM_CBCR_ADDR,v)
#define HWIO_MSS_XO_MDM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_MDM_CBCR_ADDR,m,v,HWIO_MSS_XO_MDM_CBCR_IN)
#define HWIO_MSS_XO_MDM_CBCR_CLKOFF_BMSK                                      0x80000000
#define HWIO_MSS_XO_MDM_CBCR_CLKOFF_SHFT                                            0x1f
#define HWIO_MSS_XO_MDM_CBCR_CLKEN_BMSK                                              0x1
#define HWIO_MSS_XO_MDM_CBCR_CLKEN_SHFT                                              0x0

#define HWIO_MSS_BUS_CFG_XO_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x00001048)
#define HWIO_MSS_BUS_CFG_XO_RMSK                                                     0x1
#define HWIO_MSS_BUS_CFG_XO_IN          \
        in_dword_masked(HWIO_MSS_BUS_CFG_XO_ADDR, HWIO_MSS_BUS_CFG_XO_RMSK)
#define HWIO_MSS_BUS_CFG_XO_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CFG_XO_ADDR, m)
#define HWIO_MSS_BUS_CFG_XO_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CFG_XO_ADDR,v)
#define HWIO_MSS_BUS_CFG_XO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CFG_XO_ADDR,m,v,HWIO_MSS_BUS_CFG_XO_IN)
#define HWIO_MSS_BUS_CFG_XO_DISABLE_SPM_XO_EN_BMSK                                   0x1
#define HWIO_MSS_BUS_CFG_XO_DISABLE_SPM_XO_EN_SHFT                                   0x0

#define HWIO_MSS_FABRIC_AXI2AHB_IDLE_ADDR                                     (MPSS_PERPH_REG_BASE      + 0x0000104c)
#define HWIO_MSS_FABRIC_AXI2AHB_IDLE_RMSK                                            0x3
#define HWIO_MSS_FABRIC_AXI2AHB_IDLE_IN          \
        in_dword_masked(HWIO_MSS_FABRIC_AXI2AHB_IDLE_ADDR, HWIO_MSS_FABRIC_AXI2AHB_IDLE_RMSK)
#define HWIO_MSS_FABRIC_AXI2AHB_IDLE_INM(m)      \
        in_dword_masked(HWIO_MSS_FABRIC_AXI2AHB_IDLE_ADDR, m)
#define HWIO_MSS_FABRIC_AXI2AHB_IDLE_AHB_S_IDLE_BMSK                                 0x2
#define HWIO_MSS_FABRIC_AXI2AHB_IDLE_AHB_S_IDLE_SHFT                                 0x1
#define HWIO_MSS_FABRIC_AXI2AHB_IDLE_AXI_M_IDLE_BMSK                                 0x1
#define HWIO_MSS_FABRIC_AXI2AHB_IDLE_AXI_M_IDLE_SHFT                                 0x0

#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_ADDR                              (MPSS_PERPH_REG_BASE      + 0x00001050)
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_RMSK                              0xffffffff
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_ADDR, HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_IN)
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_SPARE_31_11_BMSK                  0xfffff800
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_SPARE_31_11_SHFT                         0xb
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_DIVIDER_SEL_BMSK                       0x400
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_DIVIDER_SEL_SHFT                         0xa
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_TDEC_ODIV_MBIST_BMSK                   0x3e0
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_TDEC_ODIV_MBIST_SHFT                     0x5
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_PROC_ODIV_MBIST_BMSK                    0x1f
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_PROC_ODIV_MBIST_SHFT                     0x0

#define HWIO_MSS_OFFLINE_CLK_CTL_ADDR                                         (MPSS_PERPH_REG_BASE      + 0x00001054)
#define HWIO_MSS_OFFLINE_CLK_CTL_RMSK                                                0x3
#define HWIO_MSS_OFFLINE_CLK_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_CLK_CTL_ADDR, HWIO_MSS_OFFLINE_CLK_CTL_RMSK)
#define HWIO_MSS_OFFLINE_CLK_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_CLK_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_CLK_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_CLK_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_CLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_CLK_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_CLK_CTL_IN)
#define HWIO_MSS_OFFLINE_CLK_CTL_CLK_TDEC_ENABLE_BMSK                                0x2
#define HWIO_MSS_OFFLINE_CLK_CTL_CLK_TDEC_ENABLE_SHFT                                0x1
#define HWIO_MSS_OFFLINE_CLK_CTL_CLK_PROC_MDM_ENABLE_BMSK                            0x1
#define HWIO_MSS_OFFLINE_CLK_CTL_CLK_PROC_MDM_ENABLE_SHFT                            0x0

#define HWIO_MSS_BUS_CMD_RCGR_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x00002000)
#define HWIO_MSS_BUS_CMD_RCGR_RMSK                                            0x80000001
#define HWIO_MSS_BUS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CMD_RCGR_ADDR, HWIO_MSS_BUS_CMD_RCGR_RMSK)
#define HWIO_MSS_BUS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CMD_RCGR_ADDR, m)
#define HWIO_MSS_BUS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CMD_RCGR_ADDR,v)
#define HWIO_MSS_BUS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CMD_RCGR_ADDR,m,v,HWIO_MSS_BUS_CMD_RCGR_IN)
#define HWIO_MSS_BUS_CMD_RCGR_ROOT_OFF_BMSK                                   0x80000000
#define HWIO_MSS_BUS_CMD_RCGR_ROOT_OFF_SHFT                                         0x1f
#define HWIO_MSS_BUS_CMD_RCGR_UPDATE_BMSK                                            0x1
#define HWIO_MSS_BUS_CMD_RCGR_UPDATE_SHFT                                            0x0

#define HWIO_MSS_BUS_CFG_RCGR_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x00002004)
#define HWIO_MSS_BUS_CFG_RCGR_RMSK                                                 0x71f
#define HWIO_MSS_BUS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CFG_RCGR_ADDR, HWIO_MSS_BUS_CFG_RCGR_RMSK)
#define HWIO_MSS_BUS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CFG_RCGR_ADDR, m)
#define HWIO_MSS_BUS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CFG_RCGR_ADDR,v)
#define HWIO_MSS_BUS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CFG_RCGR_ADDR,m,v,HWIO_MSS_BUS_CFG_RCGR_IN)
#define HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_BMSK                                         0x700
#define HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_SHFT                                           0x8
#define HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_BMSK                                          0x1f
#define HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_SHFT                                           0x0

#define HWIO_MSS_UIM_CMD_RCGR_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x00002008)
#define HWIO_MSS_UIM_CMD_RCGR_RMSK                                            0x80000003
#define HWIO_MSS_UIM_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM_CMD_RCGR_ADDR, HWIO_MSS_UIM_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM_CMD_RCGR_IN)
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_OFF_BMSK                                   0x80000000
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_OFF_SHFT                                         0x1f
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_EN_BMSK                                           0x2
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_EN_SHFT                                           0x1
#define HWIO_MSS_UIM_CMD_RCGR_UPDATE_BMSK                                            0x1
#define HWIO_MSS_UIM_CMD_RCGR_UPDATE_SHFT                                            0x0

#define HWIO_MSS_UIM_CFG_RCGR_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x0000200c)
#define HWIO_MSS_UIM_CFG_RCGR_RMSK                                                  0x1f
#define HWIO_MSS_UIM_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM_CFG_RCGR_ADDR, HWIO_MSS_UIM_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM_CFG_RCGR_IN)
#define HWIO_MSS_UIM_CFG_RCGR_SRC_DIV_BMSK                                          0x1f
#define HWIO_MSS_UIM_CFG_RCGR_SRC_DIV_SHFT                                           0x0

#define HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x00002010)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_RMSK                                       0x80000003
#define HWIO_MSS_UIM0_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM0_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM0_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_MSS_UIM0_MND_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_MSS_UIM0_MND_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x00002014)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_RMSK                                           0x3000
#define HWIO_MSS_UIM0_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM0_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM0_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_MODE_BMSK                                      0x3000
#define HWIO_MSS_UIM0_MND_CFG_RCGR_MODE_SHFT                                         0xc

#define HWIO_MSS_UIM0_MND_M_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x00002018)
#define HWIO_MSS_UIM0_MND_M_RMSK                                                  0xffff
#define HWIO_MSS_UIM0_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_M_ADDR, HWIO_MSS_UIM0_MND_M_RMSK)
#define HWIO_MSS_UIM0_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_M_ADDR, m)
#define HWIO_MSS_UIM0_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_M_ADDR,v)
#define HWIO_MSS_UIM0_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_M_ADDR,m,v,HWIO_MSS_UIM0_MND_M_IN)
#define HWIO_MSS_UIM0_MND_M_M_BMSK                                                0xffff
#define HWIO_MSS_UIM0_MND_M_M_SHFT                                                   0x0

#define HWIO_MSS_UIM0_MND_N_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x0000201c)
#define HWIO_MSS_UIM0_MND_N_RMSK                                                  0xffff
#define HWIO_MSS_UIM0_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_N_ADDR, HWIO_MSS_UIM0_MND_N_RMSK)
#define HWIO_MSS_UIM0_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_N_ADDR, m)
#define HWIO_MSS_UIM0_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_N_ADDR,v)
#define HWIO_MSS_UIM0_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_N_ADDR,m,v,HWIO_MSS_UIM0_MND_N_IN)
#define HWIO_MSS_UIM0_MND_N_NOT_N_MINUS_M_BMSK                                    0xffff
#define HWIO_MSS_UIM0_MND_N_NOT_N_MINUS_M_SHFT                                       0x0

#define HWIO_MSS_UIM0_MND_D_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x00002020)
#define HWIO_MSS_UIM0_MND_D_RMSK                                                  0xffff
#define HWIO_MSS_UIM0_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_D_ADDR, HWIO_MSS_UIM0_MND_D_RMSK)
#define HWIO_MSS_UIM0_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_D_ADDR, m)
#define HWIO_MSS_UIM0_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_D_ADDR,v)
#define HWIO_MSS_UIM0_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_D_ADDR,m,v,HWIO_MSS_UIM0_MND_D_IN)
#define HWIO_MSS_UIM0_MND_D_NOT_2D_BMSK                                           0xffff
#define HWIO_MSS_UIM0_MND_D_NOT_2D_SHFT                                              0x0

#define HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR                                      (MPSS_PERPH_REG_BASE      + 0x00002024)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR, HWIO_MSS_CARD_SRC_UIM0_CBCR_RMSK)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR,m,v,HWIO_MSS_CARD_SRC_UIM0_CBCR_IN)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR                                      (MPSS_PERPH_REG_BASE      + 0x00002028)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_UART_BIT_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR, HWIO_MSS_UART_BIT_UIM0_CBCR_RMSK)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR,m,v,HWIO_MSS_UART_BIT_UIM0_CBCR_IN)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_UART_BIT_UIM0_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_UART_BIT_UIM0_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_UART_BIT_UIM0_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_BUS_UIM0_CBCR_ADDR                                           (MPSS_PERPH_REG_BASE      + 0x0000202c)
#define HWIO_MSS_BUS_UIM0_CBCR_RMSK                                           0x80007ff1
#define HWIO_MSS_BUS_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM0_CBCR_ADDR, HWIO_MSS_BUS_UIM0_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM0_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM0_CBCR_IN)
#define HWIO_MSS_BUS_UIM0_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_UIM0_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_MSS_BUS_UIM0_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_MSS_BUS_UIM0_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_MSS_BUS_UIM0_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_MSS_BUS_UIM0_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_MSS_BUS_UIM0_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_UIM0_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x00002030)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_RMSK                                       0x80000003
#define HWIO_MSS_UIM1_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM1_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM1_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_MSS_UIM1_MND_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_MSS_UIM1_MND_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x00002034)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_RMSK                                           0x3000
#define HWIO_MSS_UIM1_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM1_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM1_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_MODE_BMSK                                      0x3000
#define HWIO_MSS_UIM1_MND_CFG_RCGR_MODE_SHFT                                         0xc

#define HWIO_MSS_UIM1_MND_M_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x00002038)
#define HWIO_MSS_UIM1_MND_M_RMSK                                                  0xffff
#define HWIO_MSS_UIM1_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_M_ADDR, HWIO_MSS_UIM1_MND_M_RMSK)
#define HWIO_MSS_UIM1_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_M_ADDR, m)
#define HWIO_MSS_UIM1_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_M_ADDR,v)
#define HWIO_MSS_UIM1_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_M_ADDR,m,v,HWIO_MSS_UIM1_MND_M_IN)
#define HWIO_MSS_UIM1_MND_M_M_BMSK                                                0xffff
#define HWIO_MSS_UIM1_MND_M_M_SHFT                                                   0x0

#define HWIO_MSS_UIM1_MND_N_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x0000203c)
#define HWIO_MSS_UIM1_MND_N_RMSK                                                  0xffff
#define HWIO_MSS_UIM1_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_N_ADDR, HWIO_MSS_UIM1_MND_N_RMSK)
#define HWIO_MSS_UIM1_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_N_ADDR, m)
#define HWIO_MSS_UIM1_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_N_ADDR,v)
#define HWIO_MSS_UIM1_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_N_ADDR,m,v,HWIO_MSS_UIM1_MND_N_IN)
#define HWIO_MSS_UIM1_MND_N_NOT_N_MINUS_M_BMSK                                    0xffff
#define HWIO_MSS_UIM1_MND_N_NOT_N_MINUS_M_SHFT                                       0x0

#define HWIO_MSS_UIM1_MND_D_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x00002040)
#define HWIO_MSS_UIM1_MND_D_RMSK                                                  0xffff
#define HWIO_MSS_UIM1_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_D_ADDR, HWIO_MSS_UIM1_MND_D_RMSK)
#define HWIO_MSS_UIM1_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_D_ADDR, m)
#define HWIO_MSS_UIM1_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_D_ADDR,v)
#define HWIO_MSS_UIM1_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_D_ADDR,m,v,HWIO_MSS_UIM1_MND_D_IN)
#define HWIO_MSS_UIM1_MND_D_NOT_2D_BMSK                                           0xffff
#define HWIO_MSS_UIM1_MND_D_NOT_2D_SHFT                                              0x0

#define HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR                                      (MPSS_PERPH_REG_BASE      + 0x00002044)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR, HWIO_MSS_CARD_SRC_UIM1_CBCR_RMSK)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR,m,v,HWIO_MSS_CARD_SRC_UIM1_CBCR_IN)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR                                      (MPSS_PERPH_REG_BASE      + 0x00002048)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_UART_BIT_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR, HWIO_MSS_UART_BIT_UIM1_CBCR_RMSK)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR,m,v,HWIO_MSS_UART_BIT_UIM1_CBCR_IN)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_UART_BIT_UIM1_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_UART_BIT_UIM1_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_UART_BIT_UIM1_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_BUS_UIM1_CBCR_ADDR                                           (MPSS_PERPH_REG_BASE      + 0x0000204c)
#define HWIO_MSS_BUS_UIM1_CBCR_RMSK                                           0x80007ff1
#define HWIO_MSS_BUS_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM1_CBCR_ADDR, HWIO_MSS_BUS_UIM1_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM1_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM1_CBCR_IN)
#define HWIO_MSS_BUS_UIM1_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_UIM1_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_MSS_BUS_UIM1_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_MSS_BUS_UIM1_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_MSS_BUS_UIM1_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_MSS_BUS_UIM1_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_MSS_BUS_UIM1_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_UIM1_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x00002050)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_RMSK                                       0x80000003
#define HWIO_MSS_UIM2_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM2_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM2_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_MSS_UIM2_MND_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_MSS_UIM2_MND_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x00002054)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_RMSK                                           0x3000
#define HWIO_MSS_UIM2_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM2_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM2_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_MODE_BMSK                                      0x3000
#define HWIO_MSS_UIM2_MND_CFG_RCGR_MODE_SHFT                                         0xc

#define HWIO_MSS_UIM2_MND_M_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x00002058)
#define HWIO_MSS_UIM2_MND_M_RMSK                                                  0xffff
#define HWIO_MSS_UIM2_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_M_ADDR, HWIO_MSS_UIM2_MND_M_RMSK)
#define HWIO_MSS_UIM2_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_M_ADDR, m)
#define HWIO_MSS_UIM2_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_M_ADDR,v)
#define HWIO_MSS_UIM2_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_M_ADDR,m,v,HWIO_MSS_UIM2_MND_M_IN)
#define HWIO_MSS_UIM2_MND_M_M_BMSK                                                0xffff
#define HWIO_MSS_UIM2_MND_M_M_SHFT                                                   0x0

#define HWIO_MSS_UIM2_MND_N_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x0000205c)
#define HWIO_MSS_UIM2_MND_N_RMSK                                                  0xffff
#define HWIO_MSS_UIM2_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_N_ADDR, HWIO_MSS_UIM2_MND_N_RMSK)
#define HWIO_MSS_UIM2_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_N_ADDR, m)
#define HWIO_MSS_UIM2_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_N_ADDR,v)
#define HWIO_MSS_UIM2_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_N_ADDR,m,v,HWIO_MSS_UIM2_MND_N_IN)
#define HWIO_MSS_UIM2_MND_N_NOT_N_MINUS_M_BMSK                                    0xffff
#define HWIO_MSS_UIM2_MND_N_NOT_N_MINUS_M_SHFT                                       0x0

#define HWIO_MSS_UIM2_MND_D_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x00002060)
#define HWIO_MSS_UIM2_MND_D_RMSK                                                  0xffff
#define HWIO_MSS_UIM2_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_D_ADDR, HWIO_MSS_UIM2_MND_D_RMSK)
#define HWIO_MSS_UIM2_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_D_ADDR, m)
#define HWIO_MSS_UIM2_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_D_ADDR,v)
#define HWIO_MSS_UIM2_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_D_ADDR,m,v,HWIO_MSS_UIM2_MND_D_IN)
#define HWIO_MSS_UIM2_MND_D_NOT_2D_BMSK                                           0xffff
#define HWIO_MSS_UIM2_MND_D_NOT_2D_SHFT                                              0x0

#define HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR                                      (MPSS_PERPH_REG_BASE      + 0x00002064)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_IN          \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR, HWIO_MSS_CARD_SRC_UIM2_CBCR_RMSK)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR, m)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR,v)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR,m,v,HWIO_MSS_CARD_SRC_UIM2_CBCR_IN)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR                                      (MPSS_PERPH_REG_BASE      + 0x00002068)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_UART_BIT_UIM2_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR, HWIO_MSS_UART_BIT_UIM2_CBCR_RMSK)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR, m)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR,v)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR,m,v,HWIO_MSS_UART_BIT_UIM2_CBCR_IN)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_UART_BIT_UIM2_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_UART_BIT_UIM2_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_UART_BIT_UIM2_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_BUS_UIM2_CBCR_ADDR                                           (MPSS_PERPH_REG_BASE      + 0x0000206c)
#define HWIO_MSS_BUS_UIM2_CBCR_RMSK                                           0x80007ff1
#define HWIO_MSS_BUS_UIM2_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM2_CBCR_ADDR, HWIO_MSS_BUS_UIM2_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM2_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM2_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM2_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM2_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM2_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM2_CBCR_IN)
#define HWIO_MSS_BUS_UIM2_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_UIM2_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_MSS_BUS_UIM2_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_MSS_BUS_UIM2_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_MSS_BUS_UIM2_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_MSS_BUS_UIM2_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_MSS_BUS_UIM2_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_UIM2_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x00002070)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_RMSK                                       0x80000003
#define HWIO_MSS_UIM3_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM3_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM3_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_MSS_UIM3_MND_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_MSS_UIM3_MND_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_MSS_UIM3_MND_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_MSS_UIM3_MND_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_MSS_UIM3_MND_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x00002074)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_RMSK                                           0x3000
#define HWIO_MSS_UIM3_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM3_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM3_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_MODE_BMSK                                      0x3000
#define HWIO_MSS_UIM3_MND_CFG_RCGR_MODE_SHFT                                         0xc

#define HWIO_MSS_UIM3_MND_M_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x00002078)
#define HWIO_MSS_UIM3_MND_M_RMSK                                                  0xffff
#define HWIO_MSS_UIM3_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_M_ADDR, HWIO_MSS_UIM3_MND_M_RMSK)
#define HWIO_MSS_UIM3_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_M_ADDR, m)
#define HWIO_MSS_UIM3_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_M_ADDR,v)
#define HWIO_MSS_UIM3_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_M_ADDR,m,v,HWIO_MSS_UIM3_MND_M_IN)
#define HWIO_MSS_UIM3_MND_M_M_BMSK                                                0xffff
#define HWIO_MSS_UIM3_MND_M_M_SHFT                                                   0x0

#define HWIO_MSS_UIM3_MND_N_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x0000207c)
#define HWIO_MSS_UIM3_MND_N_RMSK                                                  0xffff
#define HWIO_MSS_UIM3_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_N_ADDR, HWIO_MSS_UIM3_MND_N_RMSK)
#define HWIO_MSS_UIM3_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_N_ADDR, m)
#define HWIO_MSS_UIM3_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_N_ADDR,v)
#define HWIO_MSS_UIM3_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_N_ADDR,m,v,HWIO_MSS_UIM3_MND_N_IN)
#define HWIO_MSS_UIM3_MND_N_NOT_N_MINUS_M_BMSK                                    0xffff
#define HWIO_MSS_UIM3_MND_N_NOT_N_MINUS_M_SHFT                                       0x0

#define HWIO_MSS_UIM3_MND_D_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x00002080)
#define HWIO_MSS_UIM3_MND_D_RMSK                                                  0xffff
#define HWIO_MSS_UIM3_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_D_ADDR, HWIO_MSS_UIM3_MND_D_RMSK)
#define HWIO_MSS_UIM3_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_D_ADDR, m)
#define HWIO_MSS_UIM3_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_D_ADDR,v)
#define HWIO_MSS_UIM3_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_D_ADDR,m,v,HWIO_MSS_UIM3_MND_D_IN)
#define HWIO_MSS_UIM3_MND_D_NOT_2D_BMSK                                           0xffff
#define HWIO_MSS_UIM3_MND_D_NOT_2D_SHFT                                              0x0

#define HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR                                      (MPSS_PERPH_REG_BASE      + 0x00002084)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_IN          \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR, HWIO_MSS_CARD_SRC_UIM3_CBCR_RMSK)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR, m)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR,v)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR,m,v,HWIO_MSS_CARD_SRC_UIM3_CBCR_IN)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR                                      (MPSS_PERPH_REG_BASE      + 0x00002088)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_UART_BIT_UIM3_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR, HWIO_MSS_UART_BIT_UIM3_CBCR_RMSK)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR, m)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR,v)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR,m,v,HWIO_MSS_UART_BIT_UIM3_CBCR_IN)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_UART_BIT_UIM3_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_UART_BIT_UIM3_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_UART_BIT_UIM3_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_BUS_UIM3_CBCR_ADDR                                           (MPSS_PERPH_REG_BASE      + 0x0000208c)
#define HWIO_MSS_BUS_UIM3_CBCR_RMSK                                           0x80007ff1
#define HWIO_MSS_BUS_UIM3_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM3_CBCR_ADDR, HWIO_MSS_BUS_UIM3_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM3_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM3_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM3_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM3_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM3_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM3_CBCR_IN)
#define HWIO_MSS_BUS_UIM3_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_UIM3_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_MSS_BUS_UIM3_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_MSS_BUS_UIM3_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_MSS_BUS_UIM3_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_MSS_BUS_UIM3_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_MSS_BUS_UIM3_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_UIM3_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_BUS_Q6SS_CBCR_ADDR                                           (MPSS_PERPH_REG_BASE      + 0x00002090)
#define HWIO_MSS_BUS_Q6SS_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_BUS_Q6SS_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_Q6SS_CBCR_ADDR, HWIO_MSS_BUS_Q6SS_CBCR_RMSK)
#define HWIO_MSS_BUS_Q6SS_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_Q6SS_CBCR_ADDR, m)
#define HWIO_MSS_BUS_Q6SS_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_Q6SS_CBCR_ADDR,v)
#define HWIO_MSS_BUS_Q6SS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_Q6SS_CBCR_ADDR,m,v,HWIO_MSS_BUS_Q6SS_CBCR_IN)
#define HWIO_MSS_BUS_Q6SS_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_Q6SS_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_Q6SS_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_Q6SS_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_NAV_BCR_ADDR                                                 (MPSS_PERPH_REG_BASE      + 0x00002094)
#define HWIO_MSS_NAV_BCR_RMSK                                                 0x80000001
#define HWIO_MSS_NAV_BCR_IN          \
        in_dword_masked(HWIO_MSS_NAV_BCR_ADDR, HWIO_MSS_NAV_BCR_RMSK)
#define HWIO_MSS_NAV_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_NAV_BCR_ADDR, m)
#define HWIO_MSS_NAV_BCR_OUT(v)      \
        out_dword(HWIO_MSS_NAV_BCR_ADDR,v)
#define HWIO_MSS_NAV_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_NAV_BCR_ADDR,m,v,HWIO_MSS_NAV_BCR_IN)
#define HWIO_MSS_NAV_BCR_STATUS_BMSK                                          0x80000000
#define HWIO_MSS_NAV_BCR_STATUS_SHFT                                                0x1f
#define HWIO_MSS_NAV_BCR_BLK_ARES_BMSK                                               0x1
#define HWIO_MSS_NAV_BCR_BLK_ARES_SHFT                                               0x0

#define HWIO_MSS_BUS_NAV_CBCR_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x00002098)
#define HWIO_MSS_BUS_NAV_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_BUS_NAV_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_NAV_CBCR_ADDR, HWIO_MSS_BUS_NAV_CBCR_RMSK)
#define HWIO_MSS_BUS_NAV_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_NAV_CBCR_ADDR, m)
#define HWIO_MSS_BUS_NAV_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_NAV_CBCR_ADDR,v)
#define HWIO_MSS_BUS_NAV_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_NAV_CBCR_ADDR,m,v,HWIO_MSS_BUS_NAV_CBCR_IN)
#define HWIO_MSS_BUS_NAV_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_BUS_NAV_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_BUS_NAV_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_BUS_NAV_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_AXI_NAV_CBCR_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x0000209c)
#define HWIO_MSS_AXI_NAV_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_AXI_NAV_CBCR_IN          \
        in_dword_masked(HWIO_MSS_AXI_NAV_CBCR_ADDR, HWIO_MSS_AXI_NAV_CBCR_RMSK)
#define HWIO_MSS_AXI_NAV_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_AXI_NAV_CBCR_ADDR, m)
#define HWIO_MSS_AXI_NAV_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_AXI_NAV_CBCR_ADDR,v)
#define HWIO_MSS_AXI_NAV_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AXI_NAV_CBCR_ADDR,m,v,HWIO_MSS_AXI_NAV_CBCR_IN)
#define HWIO_MSS_AXI_NAV_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_AXI_NAV_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_AXI_NAV_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_AXI_NAV_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_XO_NAV_CBCR_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x000020a0)
#define HWIO_MSS_XO_NAV_CBCR_RMSK                                             0x80000001
#define HWIO_MSS_XO_NAV_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_NAV_CBCR_ADDR, HWIO_MSS_XO_NAV_CBCR_RMSK)
#define HWIO_MSS_XO_NAV_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_NAV_CBCR_ADDR, m)
#define HWIO_MSS_XO_NAV_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_NAV_CBCR_ADDR,v)
#define HWIO_MSS_XO_NAV_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_NAV_CBCR_ADDR,m,v,HWIO_MSS_XO_NAV_CBCR_IN)
#define HWIO_MSS_XO_NAV_CBCR_CLKOFF_BMSK                                      0x80000000
#define HWIO_MSS_XO_NAV_CBCR_CLKOFF_SHFT                                            0x1f
#define HWIO_MSS_XO_NAV_CBCR_CLKEN_BMSK                                              0x1
#define HWIO_MSS_XO_NAV_CBCR_CLKEN_SHFT                                              0x0

#define HWIO_MSS_BUS_CRYPTO_CBCR_ADDR                                         (MPSS_PERPH_REG_BASE      + 0x000020a4)
#define HWIO_MSS_BUS_CRYPTO_CBCR_RMSK                                         0x80000001
#define HWIO_MSS_BUS_CRYPTO_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR, HWIO_MSS_BUS_CRYPTO_CBCR_RMSK)
#define HWIO_MSS_BUS_CRYPTO_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR, m)
#define HWIO_MSS_BUS_CRYPTO_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR,v)
#define HWIO_MSS_BUS_CRYPTO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR,m,v,HWIO_MSS_BUS_CRYPTO_CBCR_IN)
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKOFF_BMSK                                  0x80000000
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKOFF_SHFT                                        0x1f
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKEN_BMSK                                          0x1
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKEN_SHFT                                          0x0

#define HWIO_MSS_AXI_CRYPTO_CBCR_ADDR                                         (MPSS_PERPH_REG_BASE      + 0x000020a8)
#define HWIO_MSS_AXI_CRYPTO_CBCR_RMSK                                         0x80000001
#define HWIO_MSS_AXI_CRYPTO_CBCR_IN          \
        in_dword_masked(HWIO_MSS_AXI_CRYPTO_CBCR_ADDR, HWIO_MSS_AXI_CRYPTO_CBCR_RMSK)
#define HWIO_MSS_AXI_CRYPTO_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_AXI_CRYPTO_CBCR_ADDR, m)
#define HWIO_MSS_AXI_CRYPTO_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_AXI_CRYPTO_CBCR_ADDR,v)
#define HWIO_MSS_AXI_CRYPTO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AXI_CRYPTO_CBCR_ADDR,m,v,HWIO_MSS_AXI_CRYPTO_CBCR_IN)
#define HWIO_MSS_AXI_CRYPTO_CBCR_CLKOFF_BMSK                                  0x80000000
#define HWIO_MSS_AXI_CRYPTO_CBCR_CLKOFF_SHFT                                        0x1f
#define HWIO_MSS_AXI_CRYPTO_CBCR_CLKEN_BMSK                                          0x1
#define HWIO_MSS_AXI_CRYPTO_CBCR_CLKEN_SHFT                                          0x0

#define HWIO_MSS_AXI_DMA_CBCR_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x000020ac)
#define HWIO_MSS_AXI_DMA_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_AXI_DMA_CBCR_IN          \
        in_dword_masked(HWIO_MSS_AXI_DMA_CBCR_ADDR, HWIO_MSS_AXI_DMA_CBCR_RMSK)
#define HWIO_MSS_AXI_DMA_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_AXI_DMA_CBCR_ADDR, m)
#define HWIO_MSS_AXI_DMA_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_AXI_DMA_CBCR_ADDR,v)
#define HWIO_MSS_AXI_DMA_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AXI_DMA_CBCR_ADDR,m,v,HWIO_MSS_AXI_DMA_CBCR_IN)
#define HWIO_MSS_AXI_DMA_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_AXI_DMA_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_AXI_DMA_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_AXI_DMA_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR                                   (MPSS_PERPH_REG_BASE      + 0x000020b0)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_RMSK                                   0x80000003
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR, HWIO_MSS_BIT_COXM_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_CMD_RCGR_IN)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_OFF_BMSK                          0x80000000
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_OFF_SHFT                                0x1f
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_EN_BMSK                                  0x2
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_EN_SHFT                                  0x1
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_UPDATE_BMSK                                   0x1
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_UPDATE_SHFT                                   0x0

#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR                                   (MPSS_PERPH_REG_BASE      + 0x000020b4)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_RMSK                                       0x371f
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR, HWIO_MSS_BIT_COXM_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_CFG_RCGR_IN)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_MODE_BMSK                                  0x3000
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_MODE_SHFT                                     0xc
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_SEL_BMSK                                0x700
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_SEL_SHFT                                  0x8
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_DIV_BMSK                                 0x1f
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_DIV_SHFT                                  0x0

#define HWIO_MSS_BIT_COXM_MND_M_ADDR                                          (MPSS_PERPH_REG_BASE      + 0x000020b8)
#define HWIO_MSS_BIT_COXM_MND_M_RMSK                                              0xffff
#define HWIO_MSS_BIT_COXM_MND_M_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_M_ADDR, HWIO_MSS_BIT_COXM_MND_M_RMSK)
#define HWIO_MSS_BIT_COXM_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_M_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_M_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_M_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_M_IN)
#define HWIO_MSS_BIT_COXM_MND_M_M_BMSK                                            0xffff
#define HWIO_MSS_BIT_COXM_MND_M_M_SHFT                                               0x0

#define HWIO_MSS_BIT_COXM_MND_N_ADDR                                          (MPSS_PERPH_REG_BASE      + 0x000020bc)
#define HWIO_MSS_BIT_COXM_MND_N_RMSK                                              0xffff
#define HWIO_MSS_BIT_COXM_MND_N_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_N_ADDR, HWIO_MSS_BIT_COXM_MND_N_RMSK)
#define HWIO_MSS_BIT_COXM_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_N_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_N_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_N_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_N_IN)
#define HWIO_MSS_BIT_COXM_MND_N_NOT_N_MINUS_M_BMSK                                0xffff
#define HWIO_MSS_BIT_COXM_MND_N_NOT_N_MINUS_M_SHFT                                   0x0

#define HWIO_MSS_BIT_COXM_MND_D_ADDR                                          (MPSS_PERPH_REG_BASE      + 0x000020c0)
#define HWIO_MSS_BIT_COXM_MND_D_RMSK                                              0xffff
#define HWIO_MSS_BIT_COXM_MND_D_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_D_ADDR, HWIO_MSS_BIT_COXM_MND_D_RMSK)
#define HWIO_MSS_BIT_COXM_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_D_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_D_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_D_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_D_IN)
#define HWIO_MSS_BIT_COXM_MND_D_NOT_2D_BMSK                                       0xffff
#define HWIO_MSS_BIT_COXM_MND_D_NOT_2D_SHFT                                          0x0

#define HWIO_MSS_BUS_COXM_CBCR_ADDR                                           (MPSS_PERPH_REG_BASE      + 0x000020c4)
#define HWIO_MSS_BUS_COXM_CBCR_RMSK                                           0x80007ff1
#define HWIO_MSS_BUS_COXM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_COXM_CBCR_ADDR, HWIO_MSS_BUS_COXM_CBCR_RMSK)
#define HWIO_MSS_BUS_COXM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_COXM_CBCR_ADDR, m)
#define HWIO_MSS_BUS_COXM_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_COXM_CBCR_ADDR,v)
#define HWIO_MSS_BUS_COXM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_COXM_CBCR_ADDR,m,v,HWIO_MSS_BUS_COXM_CBCR_IN)
#define HWIO_MSS_BUS_COXM_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_COXM_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_MSS_BUS_COXM_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_MSS_BUS_COXM_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_MSS_BUS_COXM_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_MSS_BUS_COXM_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_MSS_BUS_COXM_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_COXM_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_BIT_COXM_CBCR_ADDR                                           (MPSS_PERPH_REG_BASE      + 0x000020c8)
#define HWIO_MSS_BIT_COXM_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_BIT_COXM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_CBCR_ADDR, HWIO_MSS_BIT_COXM_CBCR_RMSK)
#define HWIO_MSS_BIT_COXM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_CBCR_ADDR, m)
#define HWIO_MSS_BIT_COXM_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_CBCR_ADDR,v)
#define HWIO_MSS_BIT_COXM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_CBCR_ADDR,m,v,HWIO_MSS_BIT_COXM_CBCR_IN)
#define HWIO_MSS_BIT_COXM_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BIT_COXM_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BIT_COXM_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BIT_COXM_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR                                      (MPSS_PERPH_REG_BASE      + 0x000020cc)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_RMSK                                      0x80000003
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR, HWIO_MSS_RBCPR_REF_CMD_RCGR_RMSK)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR, m)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR,v)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR,m,v,HWIO_MSS_RBCPR_REF_CMD_RCGR_IN)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ROOT_OFF_BMSK                             0x80000000
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ROOT_OFF_SHFT                                   0x1f
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ROOT_EN_BMSK                                     0x2
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ROOT_EN_SHFT                                     0x1
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_UPDATE_BMSK                                      0x1
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_UPDATE_SHFT                                      0x0

#define HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR                                      (MPSS_PERPH_REG_BASE      + 0x000020d0)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_RMSK                                           0x71f
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR, HWIO_MSS_RBCPR_REF_CFG_RCGR_RMSK)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR, m)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR,v)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR,m,v,HWIO_MSS_RBCPR_REF_CFG_RCGR_IN)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_SRC_SEL_BMSK                                   0x700
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_SRC_SEL_SHFT                                     0x8
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_SRC_DIV_BMSK                                    0x1f
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_SRC_DIV_SHFT                                     0x0

#define HWIO_MSS_BUS_RBCPR_CBCR_ADDR                                          (MPSS_PERPH_REG_BASE      + 0x000020d4)
#define HWIO_MSS_BUS_RBCPR_CBCR_RMSK                                          0x80000001
#define HWIO_MSS_BUS_RBCPR_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_CBCR_ADDR, HWIO_MSS_BUS_RBCPR_CBCR_RMSK)
#define HWIO_MSS_BUS_RBCPR_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_CBCR_ADDR, m)
#define HWIO_MSS_BUS_RBCPR_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_RBCPR_CBCR_ADDR,v)
#define HWIO_MSS_BUS_RBCPR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_RBCPR_CBCR_ADDR,m,v,HWIO_MSS_BUS_RBCPR_CBCR_IN)
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKOFF_BMSK                                   0x80000000
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKOFF_SHFT                                         0x1f
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKEN_BMSK                                           0x1
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKEN_SHFT                                           0x0

#define HWIO_MSS_RBCPR_REF_CBCR_ADDR                                          (MPSS_PERPH_REG_BASE      + 0x000020d8)
#define HWIO_MSS_RBCPR_REF_CBCR_RMSK                                          0x80000001
#define HWIO_MSS_RBCPR_REF_CBCR_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CBCR_ADDR, HWIO_MSS_RBCPR_REF_CBCR_RMSK)
#define HWIO_MSS_RBCPR_REF_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CBCR_ADDR, m)
#define HWIO_MSS_RBCPR_REF_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_REF_CBCR_ADDR,v)
#define HWIO_MSS_RBCPR_REF_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_REF_CBCR_ADDR,m,v,HWIO_MSS_RBCPR_REF_CBCR_IN)
#define HWIO_MSS_RBCPR_REF_CBCR_CLKOFF_BMSK                                   0x80000000
#define HWIO_MSS_RBCPR_REF_CBCR_CLKOFF_SHFT                                         0x1f
#define HWIO_MSS_RBCPR_REF_CBCR_CLKEN_BMSK                                           0x1
#define HWIO_MSS_RBCPR_REF_CBCR_CLKEN_SHFT                                           0x0

#define HWIO_MSS_BUS_MGPI_CBCR_ADDR                                           (MPSS_PERPH_REG_BASE      + 0x000020dc)
#define HWIO_MSS_BUS_MGPI_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_BUS_MGPI_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_MGPI_CBCR_ADDR, HWIO_MSS_BUS_MGPI_CBCR_RMSK)
#define HWIO_MSS_BUS_MGPI_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MGPI_CBCR_ADDR, m)
#define HWIO_MSS_BUS_MGPI_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_MGPI_CBCR_ADDR,v)
#define HWIO_MSS_BUS_MGPI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_MGPI_CBCR_ADDR,m,v,HWIO_MSS_BUS_MGPI_CBCR_IN)
#define HWIO_MSS_BUS_MGPI_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_MGPI_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_MGPI_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_MGPI_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR                                  (MPSS_PERPH_REG_BASE      + 0x000020e0)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_RMSK                                  0x80000001
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR, HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_RMSK)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR, m)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR,v)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR,m,v,HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_IN)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKOFF_BMSK                           0x80000000
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKOFF_SHFT                                 0x1f
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKEN_BMSK                                   0x1
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKEN_SHFT                                   0x0

#define HWIO_MSS_BUS_MVC_CBCR_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x000020e4)
#define HWIO_MSS_BUS_MVC_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_BUS_MVC_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_MVC_CBCR_ADDR, HWIO_MSS_BUS_MVC_CBCR_RMSK)
#define HWIO_MSS_BUS_MVC_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MVC_CBCR_ADDR, m)
#define HWIO_MSS_BUS_MVC_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_MVC_CBCR_ADDR,v)
#define HWIO_MSS_BUS_MVC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_MVC_CBCR_ADDR,m,v,HWIO_MSS_BUS_MVC_CBCR_IN)
#define HWIO_MSS_BUS_MVC_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_BUS_MVC_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_BUS_MVC_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_BUS_MVC_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_BUS_STMR_CBCR_ADDR                                           (MPSS_PERPH_REG_BASE      + 0x000020e8)
#define HWIO_MSS_BUS_STMR_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_BUS_STMR_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_STMR_CBCR_ADDR, HWIO_MSS_BUS_STMR_CBCR_RMSK)
#define HWIO_MSS_BUS_STMR_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_STMR_CBCR_ADDR, m)
#define HWIO_MSS_BUS_STMR_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_STMR_CBCR_ADDR,v)
#define HWIO_MSS_BUS_STMR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_STMR_CBCR_ADDR,m,v,HWIO_MSS_BUS_STMR_CBCR_IN)
#define HWIO_MSS_BUS_STMR_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_STMR_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_STMR_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_STMR_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_XO_CX_CBCR_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x000020ec)
#define HWIO_MSS_XO_CX_CBCR_RMSK                                              0x80000001
#define HWIO_MSS_XO_CX_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_CX_CBCR_ADDR, HWIO_MSS_XO_CX_CBCR_RMSK)
#define HWIO_MSS_XO_CX_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_CX_CBCR_ADDR, m)
#define HWIO_MSS_XO_CX_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_CX_CBCR_ADDR,v)
#define HWIO_MSS_XO_CX_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_CX_CBCR_ADDR,m,v,HWIO_MSS_XO_CX_CBCR_IN)
#define HWIO_MSS_XO_CX_CBCR_CLKOFF_BMSK                                       0x80000000
#define HWIO_MSS_XO_CX_CBCR_CLKOFF_SHFT                                             0x1f
#define HWIO_MSS_XO_CX_CBCR_CLKEN_BMSK                                               0x1
#define HWIO_MSS_XO_CX_CBCR_CLKEN_SHFT                                               0x0

#define HWIO_MSS_BUS_RFC_CBCR_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x000020f0)
#define HWIO_MSS_BUS_RFC_CBCR_RMSK                                            0x80007ff1
#define HWIO_MSS_BUS_RFC_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_RFC_CBCR_ADDR, HWIO_MSS_BUS_RFC_CBCR_RMSK)
#define HWIO_MSS_BUS_RFC_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_RFC_CBCR_ADDR, m)
#define HWIO_MSS_BUS_RFC_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_RFC_CBCR_ADDR,v)
#define HWIO_MSS_BUS_RFC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_RFC_CBCR_ADDR,m,v,HWIO_MSS_BUS_RFC_CBCR_IN)
#define HWIO_MSS_BUS_RFC_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_BUS_RFC_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_BUS_RFC_CBCR_FORCE_MEM_CORE_ON_BMSK                              0x4000
#define HWIO_MSS_BUS_RFC_CBCR_FORCE_MEM_CORE_ON_SHFT                                 0xe
#define HWIO_MSS_BUS_RFC_CBCR_FORCE_MEM_PERIPH_ON_BMSK                            0x2000
#define HWIO_MSS_BUS_RFC_CBCR_FORCE_MEM_PERIPH_ON_SHFT                               0xd
#define HWIO_MSS_BUS_RFC_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                           0x1000
#define HWIO_MSS_BUS_RFC_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                              0xc
#define HWIO_MSS_BUS_RFC_CBCR_WAKEUP_BMSK                                          0xf00
#define HWIO_MSS_BUS_RFC_CBCR_WAKEUP_SHFT                                            0x8
#define HWIO_MSS_BUS_RFC_CBCR_SLEEP_BMSK                                            0xf0
#define HWIO_MSS_BUS_RFC_CBCR_SLEEP_SHFT                                             0x4
#define HWIO_MSS_BUS_RFC_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_BUS_RFC_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_SW_FEATURE_FUSES_ADDR                                        (MPSS_PERPH_REG_BASE      + 0x000020f4)
#define HWIO_MSS_SW_FEATURE_FUSES_RMSK                                        0xffffffff
#define HWIO_MSS_SW_FEATURE_FUSES_IN          \
        in_dword_masked(HWIO_MSS_SW_FEATURE_FUSES_ADDR, HWIO_MSS_SW_FEATURE_FUSES_RMSK)
#define HWIO_MSS_SW_FEATURE_FUSES_INM(m)      \
        in_dword_masked(HWIO_MSS_SW_FEATURE_FUSES_ADDR, m)
#define HWIO_MSS_SW_FEATURE_FUSES_MSS_SW_FEATURE_FUSES_31_0_BMSK              0xffffffff
#define HWIO_MSS_SW_FEATURE_FUSES_MSS_SW_FEATURE_FUSES_31_0_SHFT                     0x0

#define HWIO_MSS_SW_FEATURE_FUSES2_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x000020f8)
#define HWIO_MSS_SW_FEATURE_FUSES2_RMSK                                       0xffffffff
#define HWIO_MSS_SW_FEATURE_FUSES2_IN          \
        in_dword_masked(HWIO_MSS_SW_FEATURE_FUSES2_ADDR, HWIO_MSS_SW_FEATURE_FUSES2_RMSK)
#define HWIO_MSS_SW_FEATURE_FUSES2_INM(m)      \
        in_dword_masked(HWIO_MSS_SW_FEATURE_FUSES2_ADDR, m)
#define HWIO_MSS_SW_FEATURE_FUSES2_MSS_SW_FEATURE_FUSES2_31_0_BMSK            0xffffffff
#define HWIO_MSS_SW_FEATURE_FUSES2_MSS_SW_FEATURE_FUSES2_31_0_SHFT                   0x0

#define HWIO_MSS_RESERVE_01_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x000020fc)
#define HWIO_MSS_RESERVE_01_RMSK                                              0xffffffff
#define HWIO_MSS_RESERVE_01_IN          \
        in_dword_masked(HWIO_MSS_RESERVE_01_ADDR, HWIO_MSS_RESERVE_01_RMSK)
#define HWIO_MSS_RESERVE_01_INM(m)      \
        in_dword_masked(HWIO_MSS_RESERVE_01_ADDR, m)
#define HWIO_MSS_RESERVE_01_OUT(v)      \
        out_dword(HWIO_MSS_RESERVE_01_ADDR,v)
#define HWIO_MSS_RESERVE_01_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RESERVE_01_ADDR,m,v,HWIO_MSS_RESERVE_01_IN)
#define HWIO_MSS_RESERVE_01_MSS_RESERVE_01_BMSK                               0xffffffff
#define HWIO_MSS_RESERVE_01_MSS_RESERVE_01_SHFT                                      0x0

#define HWIO_MSS_RESERVE_02_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x00002100)
#define HWIO_MSS_RESERVE_02_RMSK                                              0xffffffff
#define HWIO_MSS_RESERVE_02_IN          \
        in_dword_masked(HWIO_MSS_RESERVE_02_ADDR, HWIO_MSS_RESERVE_02_RMSK)
#define HWIO_MSS_RESERVE_02_INM(m)      \
        in_dword_masked(HWIO_MSS_RESERVE_02_ADDR, m)
#define HWIO_MSS_RESERVE_02_OUT(v)      \
        out_dword(HWIO_MSS_RESERVE_02_ADDR,v)
#define HWIO_MSS_RESERVE_02_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RESERVE_02_ADDR,m,v,HWIO_MSS_RESERVE_02_IN)
#define HWIO_MSS_RESERVE_02_MSS_RESERVE_02_BMSK                               0xffffffff
#define HWIO_MSS_RESERVE_02_MSS_RESERVE_02_SHFT                                      0x0

/*----------------------------------------------------------------------------
 * MODULE: MSS_CPR3
 *--------------------------------------------------------------------------*/

#define MSS_CPR3_REG_BASE                                                    (MODEM_TOP_BASE      + 0x001a4000)

#define HWIO_MSS_CPR_VERSION_ADDR                                            (MSS_CPR3_REG_BASE      + 0x00000000)
#define HWIO_MSS_CPR_VERSION_RMSK                                            0xffffffff
#define HWIO_MSS_CPR_VERSION_IN          \
        in_dword_masked(HWIO_MSS_CPR_VERSION_ADDR, HWIO_MSS_CPR_VERSION_RMSK)
#define HWIO_MSS_CPR_VERSION_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_VERSION_ADDR, m)
#define HWIO_MSS_CPR_VERSION_CPR_VERSION_BMSK                                0xffffffff
#define HWIO_MSS_CPR_VERSION_CPR_VERSION_SHFT                                       0x0

#define HWIO_MSS_CPR_FSM_CTL_ADDR                                            (MSS_CPR3_REG_BASE      + 0x00000004)
#define HWIO_MSS_CPR_FSM_CTL_RMSK                                            0xffffffff
#define HWIO_MSS_CPR_FSM_CTL_IN          \
        in_dword_masked(HWIO_MSS_CPR_FSM_CTL_ADDR, HWIO_MSS_CPR_FSM_CTL_RMSK)
#define HWIO_MSS_CPR_FSM_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_FSM_CTL_ADDR, m)
#define HWIO_MSS_CPR_FSM_CTL_OUT(v)      \
        out_dword(HWIO_MSS_CPR_FSM_CTL_ADDR,v)
#define HWIO_MSS_CPR_FSM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_FSM_CTL_ADDR,m,v,HWIO_MSS_CPR_FSM_CTL_IN)
#define HWIO_MSS_CPR_FSM_CTL_COUNT_REPEAT_BMSK                               0xfffffe00
#define HWIO_MSS_CPR_FSM_CTL_COUNT_REPEAT_SHFT                                      0x9
#define HWIO_MSS_CPR_FSM_CTL_SET_HTOL_MODE_BMSK                                   0x100
#define HWIO_MSS_CPR_FSM_CTL_SET_HTOL_MODE_SHFT                                     0x8
#define HWIO_MSS_CPR_FSM_CTL_COUNT_MODE_BMSK                                       0xc0
#define HWIO_MSS_CPR_FSM_CTL_COUNT_MODE_SHFT                                        0x6
#define HWIO_MSS_CPR_FSM_CTL_IDLE_CLOCKS_BMSK                                      0x3e
#define HWIO_MSS_CPR_FSM_CTL_IDLE_CLOCKS_SHFT                                       0x1
#define HWIO_MSS_CPR_FSM_CTL_LOOP_EN_BMSK                                           0x1
#define HWIO_MSS_CPR_FSM_CTL_LOOP_EN_SHFT                                           0x0

#define HWIO_MSS_CPR_FSM_STA_ADDR                                            (MSS_CPR3_REG_BASE      + 0x00000008)
#define HWIO_MSS_CPR_FSM_STA_RMSK                                                   0x3
#define HWIO_MSS_CPR_FSM_STA_IN          \
        in_dword_masked(HWIO_MSS_CPR_FSM_STA_ADDR, HWIO_MSS_CPR_FSM_STA_RMSK)
#define HWIO_MSS_CPR_FSM_STA_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_FSM_STA_ADDR, m)
#define HWIO_MSS_CPR_FSM_STA_PAGE_IS_AGE_BMSK                                       0x2
#define HWIO_MSS_CPR_FSM_STA_PAGE_IS_AGE_SHFT                                       0x1
#define HWIO_MSS_CPR_FSM_STA_BUSY_BMSK                                              0x1
#define HWIO_MSS_CPR_FSM_STA_BUSY_SHFT                                              0x0

#define HWIO_MSS_CPR_TIMER_AUTO_CONT_ADDR                                    (MSS_CPR3_REG_BASE      + 0x0000000c)
#define HWIO_MSS_CPR_TIMER_AUTO_CONT_RMSK                                    0xffffffff
#define HWIO_MSS_CPR_TIMER_AUTO_CONT_IN          \
        in_dword_masked(HWIO_MSS_CPR_TIMER_AUTO_CONT_ADDR, HWIO_MSS_CPR_TIMER_AUTO_CONT_RMSK)
#define HWIO_MSS_CPR_TIMER_AUTO_CONT_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_TIMER_AUTO_CONT_ADDR, m)
#define HWIO_MSS_CPR_TIMER_AUTO_CONT_OUT(v)      \
        out_dword(HWIO_MSS_CPR_TIMER_AUTO_CONT_ADDR,v)
#define HWIO_MSS_CPR_TIMER_AUTO_CONT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_TIMER_AUTO_CONT_ADDR,m,v,HWIO_MSS_CPR_TIMER_AUTO_CONT_IN)
#define HWIO_MSS_CPR_TIMER_AUTO_CONT_AUTO_CONT_INTERVAL_BMSK                 0xffffffff
#define HWIO_MSS_CPR_TIMER_AUTO_CONT_AUTO_CONT_INTERVAL_SHFT                        0x0

#define HWIO_MSS_CPR_TIMER_CLAMP_ADDR                                        (MSS_CPR3_REG_BASE      + 0x00000010)
#define HWIO_MSS_CPR_TIMER_CLAMP_RMSK                                        0xe00000ff
#define HWIO_MSS_CPR_TIMER_CLAMP_IN          \
        in_dword_masked(HWIO_MSS_CPR_TIMER_CLAMP_ADDR, HWIO_MSS_CPR_TIMER_CLAMP_RMSK)
#define HWIO_MSS_CPR_TIMER_CLAMP_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_TIMER_CLAMP_ADDR, m)
#define HWIO_MSS_CPR_TIMER_CLAMP_OUT(v)      \
        out_dword(HWIO_MSS_CPR_TIMER_CLAMP_ADDR,v)
#define HWIO_MSS_CPR_TIMER_CLAMP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_TIMER_CLAMP_ADDR,m,v,HWIO_MSS_CPR_TIMER_CLAMP_IN)
#define HWIO_MSS_CPR_TIMER_CLAMP_CPR_DISABLE_VALID_BMSK                      0x80000000
#define HWIO_MSS_CPR_TIMER_CLAMP_CPR_DISABLE_VALID_SHFT                            0x1f
#define HWIO_MSS_CPR_TIMER_CLAMP_GDHS_RESTART_ENABLE_BMSK                    0x40000000
#define HWIO_MSS_CPR_TIMER_CLAMP_GDHS_RESTART_ENABLE_SHFT                          0x1e
#define HWIO_MSS_CPR_TIMER_CLAMP_BHS_ON_RESTART_ENABLE_BMSK                  0x20000000
#define HWIO_MSS_CPR_TIMER_CLAMP_BHS_ON_RESTART_ENABLE_SHFT                        0x1d
#define HWIO_MSS_CPR_TIMER_CLAMP_CLAMP_TIMER_INTERVAL_BMSK                         0xff
#define HWIO_MSS_CPR_TIMER_CLAMP_CLAMP_TIMER_INTERVAL_SHFT                          0x0

#define HWIO_MSS_CPR_STEP_QUOT_INIT_ADDR                                     (MSS_CPR3_REG_BASE      + 0x00000014)
#define HWIO_MSS_CPR_STEP_QUOT_INIT_RMSK                                          0xfff
#define HWIO_MSS_CPR_STEP_QUOT_INIT_IN          \
        in_dword_masked(HWIO_MSS_CPR_STEP_QUOT_INIT_ADDR, HWIO_MSS_CPR_STEP_QUOT_INIT_RMSK)
#define HWIO_MSS_CPR_STEP_QUOT_INIT_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_STEP_QUOT_INIT_ADDR, m)
#define HWIO_MSS_CPR_STEP_QUOT_INIT_OUT(v)      \
        out_dword(HWIO_MSS_CPR_STEP_QUOT_INIT_ADDR,v)
#define HWIO_MSS_CPR_STEP_QUOT_INIT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_STEP_QUOT_INIT_ADDR,m,v,HWIO_MSS_CPR_STEP_QUOT_INIT_IN)
#define HWIO_MSS_CPR_STEP_QUOT_INIT_STEP_QUOT_INIT_MAX_BMSK                       0xfc0
#define HWIO_MSS_CPR_STEP_QUOT_INIT_STEP_QUOT_INIT_MAX_SHFT                         0x6
#define HWIO_MSS_CPR_STEP_QUOT_INIT_STEP_QUOT_INIT_MIN_BMSK                        0x3f
#define HWIO_MSS_CPR_STEP_QUOT_INIT_STEP_QUOT_INIT_MIN_SHFT                         0x0

#define HWIO_MSS_CPR_BIST_CHAIN_CHECK0_ADDR                                  (MSS_CPR3_REG_BASE      + 0x00000020)
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK0_RMSK                                  0x80000fff
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK0_IN          \
        in_dword_masked(HWIO_MSS_CPR_BIST_CHAIN_CHECK0_ADDR, HWIO_MSS_CPR_BIST_CHAIN_CHECK0_RMSK)
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK0_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_BIST_CHAIN_CHECK0_ADDR, m)
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK0_OUT(v)      \
        out_dword(HWIO_MSS_CPR_BIST_CHAIN_CHECK0_ADDR,v)
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_BIST_CHAIN_CHECK0_ADDR,m,v,HWIO_MSS_CPR_BIST_CHAIN_CHECK0_IN)
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK0_EN_CHAIN_CHECK_BMSK                   0x80000000
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK0_EN_CHAIN_CHECK_SHFT                         0x1f
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK0_SCLK_CNT_EXPECTED_BMSK                     0xfff
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK0_SCLK_CNT_EXPECTED_SHFT                       0x0

#define HWIO_MSS_CPR_BIST_CHAIN_CHECK1_ADDR                                  (MSS_CPR3_REG_BASE      + 0x00000024)
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK1_RMSK                                  0xe0ffffff
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK1_IN          \
        in_dword_masked(HWIO_MSS_CPR_BIST_CHAIN_CHECK1_ADDR, HWIO_MSS_CPR_BIST_CHAIN_CHECK1_RMSK)
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK1_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_BIST_CHAIN_CHECK1_ADDR, m)
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK1_SCLK_CNT_DONE_BMSK                    0x80000000
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK1_SCLK_CNT_DONE_SHFT                          0x1f
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK1_SCLK_CNT_FAIL_BMSK                    0x60000000
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK1_SCLK_CNT_FAIL_SHFT                          0x1d
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK1_SCLK_CNT1_BMSK                          0xfff000
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK1_SCLK_CNT1_SHFT                               0xc
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK1_SCLK_CNT0_BMSK                             0xfff
#define HWIO_MSS_CPR_BIST_CHAIN_CHECK1_SCLK_CNT0_SHFT                               0x0

#define HWIO_MSS_CPR_BIST_QUOT_CHECK0_ADDR                                   (MSS_CPR3_REG_BASE      + 0x00000028)
#define HWIO_MSS_CPR_BIST_QUOT_CHECK0_RMSK                                   0x80ffffff
#define HWIO_MSS_CPR_BIST_QUOT_CHECK0_IN          \
        in_dword_masked(HWIO_MSS_CPR_BIST_QUOT_CHECK0_ADDR, HWIO_MSS_CPR_BIST_QUOT_CHECK0_RMSK)
#define HWIO_MSS_CPR_BIST_QUOT_CHECK0_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_BIST_QUOT_CHECK0_ADDR, m)
#define HWIO_MSS_CPR_BIST_QUOT_CHECK0_OUT(v)      \
        out_dword(HWIO_MSS_CPR_BIST_QUOT_CHECK0_ADDR,v)
#define HWIO_MSS_CPR_BIST_QUOT_CHECK0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_BIST_QUOT_CHECK0_ADDR,m,v,HWIO_MSS_CPR_BIST_QUOT_CHECK0_IN)
#define HWIO_MSS_CPR_BIST_QUOT_CHECK0_FAIL_BMSK                              0x80000000
#define HWIO_MSS_CPR_BIST_QUOT_CHECK0_FAIL_SHFT                                    0x1f
#define HWIO_MSS_CPR_BIST_QUOT_CHECK0_QUOT_ABSMAX_BMSK                         0xfff000
#define HWIO_MSS_CPR_BIST_QUOT_CHECK0_QUOT_ABSMAX_SHFT                              0xc
#define HWIO_MSS_CPR_BIST_QUOT_CHECK0_QUOT_ABSMIN_BMSK                            0xfff
#define HWIO_MSS_CPR_BIST_QUOT_CHECK0_QUOT_ABSMIN_SHFT                              0x0

#define HWIO_MSS_CPR_BIST_QUOT_CHECK1_ADDR                                   (MSS_CPR3_REG_BASE      + 0x0000002c)
#define HWIO_MSS_CPR_BIST_QUOT_CHECK1_RMSK                                   0xffffffff
#define HWIO_MSS_CPR_BIST_QUOT_CHECK1_IN          \
        in_dword_masked(HWIO_MSS_CPR_BIST_QUOT_CHECK1_ADDR, HWIO_MSS_CPR_BIST_QUOT_CHECK1_RMSK)
#define HWIO_MSS_CPR_BIST_QUOT_CHECK1_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_BIST_QUOT_CHECK1_ADDR, m)
#define HWIO_MSS_CPR_BIST_QUOT_CHECK1_FAIL_SEL_MAX_BMSK                      0xf0000000
#define HWIO_MSS_CPR_BIST_QUOT_CHECK1_FAIL_SEL_MAX_SHFT                            0x1c
#define HWIO_MSS_CPR_BIST_QUOT_CHECK1_FAIL_SEL_MIN_BMSK                       0xf000000
#define HWIO_MSS_CPR_BIST_QUOT_CHECK1_FAIL_SEL_MIN_SHFT                            0x18
#define HWIO_MSS_CPR_BIST_QUOT_CHECK1_FAIL_QUOT_MAX_BMSK                       0xfff000
#define HWIO_MSS_CPR_BIST_QUOT_CHECK1_FAIL_QUOT_MAX_SHFT                            0xc
#define HWIO_MSS_CPR_BIST_QUOT_CHECK1_FAIL_QUOT_MIN_BMSK                          0xfff
#define HWIO_MSS_CPR_BIST_QUOT_CHECK1_FAIL_QUOT_MIN_SHFT                            0x0

#define HWIO_MSS_CPR_BIST_FAIL_MAP_MIN_n_ADDR(n)                             (MSS_CPR3_REG_BASE      + 0x00000030 + 0x4 * (n))
#define HWIO_MSS_CPR_BIST_FAIL_MAP_MIN_n_RMSK                                0xffffffff
#define HWIO_MSS_CPR_BIST_FAIL_MAP_MIN_n_MAXn                                         0
#define HWIO_MSS_CPR_BIST_FAIL_MAP_MIN_n_INI(n)        \
        in_dword_masked(HWIO_MSS_CPR_BIST_FAIL_MAP_MIN_n_ADDR(n), HWIO_MSS_CPR_BIST_FAIL_MAP_MIN_n_RMSK)
#define HWIO_MSS_CPR_BIST_FAIL_MAP_MIN_n_INMI(n,mask)    \
        in_dword_masked(HWIO_MSS_CPR_BIST_FAIL_MAP_MIN_n_ADDR(n), mask)
#define HWIO_MSS_CPR_BIST_FAIL_MAP_MIN_n_BIST_FAIL_MAP_MIN_BMSK              0xffffffff
#define HWIO_MSS_CPR_BIST_FAIL_MAP_MIN_n_BIST_FAIL_MAP_MIN_SHFT                     0x0

#define HWIO_MSS_CPR_BIST_FAIL_MAP_MAX_n_ADDR(n)                             (MSS_CPR3_REG_BASE      + 0x00000050 + 0x4 * (n))
#define HWIO_MSS_CPR_BIST_FAIL_MAP_MAX_n_RMSK                                0xffffffff
#define HWIO_MSS_CPR_BIST_FAIL_MAP_MAX_n_MAXn                                         0
#define HWIO_MSS_CPR_BIST_FAIL_MAP_MAX_n_INI(n)        \
        in_dword_masked(HWIO_MSS_CPR_BIST_FAIL_MAP_MAX_n_ADDR(n), HWIO_MSS_CPR_BIST_FAIL_MAP_MAX_n_RMSK)
#define HWIO_MSS_CPR_BIST_FAIL_MAP_MAX_n_INMI(n,mask)    \
        in_dword_masked(HWIO_MSS_CPR_BIST_FAIL_MAP_MAX_n_ADDR(n), mask)
#define HWIO_MSS_CPR_BIST_FAIL_MAP_MAX_n_BIST_FAIL_MAP_MAX_BMSK              0xffffffff
#define HWIO_MSS_CPR_BIST_FAIL_MAP_MAX_n_BIST_FAIL_MAP_MAX_SHFT                     0x0

#define HWIO_MSS_CPR_DEBUG_ADDR                                              (MSS_CPR3_REG_BASE      + 0x00000070)
#define HWIO_MSS_CPR_DEBUG_RMSK                                                    0x3f
#define HWIO_MSS_CPR_DEBUG_IN          \
        in_dword_masked(HWIO_MSS_CPR_DEBUG_ADDR, HWIO_MSS_CPR_DEBUG_RMSK)
#define HWIO_MSS_CPR_DEBUG_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_DEBUG_ADDR, m)
#define HWIO_MSS_CPR_DEBUG_OUT(v)      \
        out_dword(HWIO_MSS_CPR_DEBUG_ADDR,v)
#define HWIO_MSS_CPR_DEBUG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_DEBUG_ADDR,m,v,HWIO_MSS_CPR_DEBUG_IN)
#define HWIO_MSS_CPR_DEBUG_CPR_LOG_THREAD_SEL_BMSK                                 0x38
#define HWIO_MSS_CPR_DEBUG_CPR_LOG_THREAD_SEL_SHFT                                  0x3
#define HWIO_MSS_CPR_DEBUG_DEBUG_BUS_SEL_BMSK                                       0x7
#define HWIO_MSS_CPR_DEBUG_DEBUG_BUS_SEL_SHFT                                       0x0

#define HWIO_MSS_CPR_DEBUG_LOG_n_ADDR(n)                                     (MSS_CPR3_REG_BASE      + 0x00000080 + 0x4 * (n))
#define HWIO_MSS_CPR_DEBUG_LOG_n_RMSK                                        0x3fff000e
#define HWIO_MSS_CPR_DEBUG_LOG_n_MAXn                                                 7
#define HWIO_MSS_CPR_DEBUG_LOG_n_INI(n)        \
        in_dword_masked(HWIO_MSS_CPR_DEBUG_LOG_n_ADDR(n), HWIO_MSS_CPR_DEBUG_LOG_n_RMSK)
#define HWIO_MSS_CPR_DEBUG_LOG_n_INMI(n,mask)    \
        in_dword_masked(HWIO_MSS_CPR_DEBUG_LOG_n_ADDR(n), mask)
#define HWIO_MSS_CPR_DEBUG_LOG_n_MID_COUNT_BMSK                              0x3fff0000
#define HWIO_MSS_CPR_DEBUG_LOG_n_MID_COUNT_SHFT                                    0x10
#define HWIO_MSS_CPR_DEBUG_LOG_n_UP_FLAG_BMSK                                       0x8
#define HWIO_MSS_CPR_DEBUG_LOG_n_UP_FLAG_SHFT                                       0x3
#define HWIO_MSS_CPR_DEBUG_LOG_n_MID_FLAG_BMSK                                      0x4
#define HWIO_MSS_CPR_DEBUG_LOG_n_MID_FLAG_SHFT                                      0x2
#define HWIO_MSS_CPR_DEBUG_LOG_n_DOWN_FLAG_BMSK                                     0x2
#define HWIO_MSS_CPR_DEBUG_LOG_n_DOWN_FLAG_SHFT                                     0x1

#define HWIO_MSS_CPR_GCNT0_ADDR                                              (MSS_CPR3_REG_BASE      + 0x000000a0)
#define HWIO_MSS_CPR_GCNT0_RMSK                                                   0x3ff
#define HWIO_MSS_CPR_GCNT0_IN          \
        in_dword_masked(HWIO_MSS_CPR_GCNT0_ADDR, HWIO_MSS_CPR_GCNT0_RMSK)
#define HWIO_MSS_CPR_GCNT0_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_GCNT0_ADDR, m)
#define HWIO_MSS_CPR_GCNT0_OUT(v)      \
        out_dword(HWIO_MSS_CPR_GCNT0_ADDR,v)
#define HWIO_MSS_CPR_GCNT0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_GCNT0_ADDR,m,v,HWIO_MSS_CPR_GCNT0_IN)
#define HWIO_MSS_CPR_GCNT0_GCNT0_BMSK                                             0x3ff
#define HWIO_MSS_CPR_GCNT0_GCNT0_SHFT                                               0x0

#define HWIO_MSS_CPR_GCNT1_ADDR                                              (MSS_CPR3_REG_BASE      + 0x000000a4)
#define HWIO_MSS_CPR_GCNT1_RMSK                                                   0x3ff
#define HWIO_MSS_CPR_GCNT1_IN          \
        in_dword_masked(HWIO_MSS_CPR_GCNT1_ADDR, HWIO_MSS_CPR_GCNT1_RMSK)
#define HWIO_MSS_CPR_GCNT1_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_GCNT1_ADDR, m)
#define HWIO_MSS_CPR_GCNT1_OUT(v)      \
        out_dword(HWIO_MSS_CPR_GCNT1_ADDR,v)
#define HWIO_MSS_CPR_GCNT1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_GCNT1_ADDR,m,v,HWIO_MSS_CPR_GCNT1_IN)
#define HWIO_MSS_CPR_GCNT1_GCNT1_BMSK                                             0x3ff
#define HWIO_MSS_CPR_GCNT1_GCNT1_SHFT                                               0x0

#define HWIO_MSS_CPR_GCNT2_ADDR                                              (MSS_CPR3_REG_BASE      + 0x000000a8)
#define HWIO_MSS_CPR_GCNT2_RMSK                                                   0x3ff
#define HWIO_MSS_CPR_GCNT2_IN          \
        in_dword_masked(HWIO_MSS_CPR_GCNT2_ADDR, HWIO_MSS_CPR_GCNT2_RMSK)
#define HWIO_MSS_CPR_GCNT2_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_GCNT2_ADDR, m)
#define HWIO_MSS_CPR_GCNT2_OUT(v)      \
        out_dword(HWIO_MSS_CPR_GCNT2_ADDR,v)
#define HWIO_MSS_CPR_GCNT2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_GCNT2_ADDR,m,v,HWIO_MSS_CPR_GCNT2_IN)
#define HWIO_MSS_CPR_GCNT2_GCNT2_BMSK                                             0x3ff
#define HWIO_MSS_CPR_GCNT2_GCNT2_SHFT                                               0x0

#define HWIO_MSS_CPR_GCNT3_ADDR                                              (MSS_CPR3_REG_BASE      + 0x000000ac)
#define HWIO_MSS_CPR_GCNT3_RMSK                                                   0x3ff
#define HWIO_MSS_CPR_GCNT3_IN          \
        in_dword_masked(HWIO_MSS_CPR_GCNT3_ADDR, HWIO_MSS_CPR_GCNT3_RMSK)
#define HWIO_MSS_CPR_GCNT3_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_GCNT3_ADDR, m)
#define HWIO_MSS_CPR_GCNT3_OUT(v)      \
        out_dword(HWIO_MSS_CPR_GCNT3_ADDR,v)
#define HWIO_MSS_CPR_GCNT3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_GCNT3_ADDR,m,v,HWIO_MSS_CPR_GCNT3_IN)
#define HWIO_MSS_CPR_GCNT3_GCNT3_BMSK                                             0x3ff
#define HWIO_MSS_CPR_GCNT3_GCNT3_SHFT                                               0x0

#define HWIO_MSS_CPR_GCNT4_ADDR                                              (MSS_CPR3_REG_BASE      + 0x000000b0)
#define HWIO_MSS_CPR_GCNT4_RMSK                                                   0x3ff
#define HWIO_MSS_CPR_GCNT4_IN          \
        in_dword_masked(HWIO_MSS_CPR_GCNT4_ADDR, HWIO_MSS_CPR_GCNT4_RMSK)
#define HWIO_MSS_CPR_GCNT4_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_GCNT4_ADDR, m)
#define HWIO_MSS_CPR_GCNT4_OUT(v)      \
        out_dword(HWIO_MSS_CPR_GCNT4_ADDR,v)
#define HWIO_MSS_CPR_GCNT4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_GCNT4_ADDR,m,v,HWIO_MSS_CPR_GCNT4_IN)
#define HWIO_MSS_CPR_GCNT4_GCNT4_BMSK                                             0x3ff
#define HWIO_MSS_CPR_GCNT4_GCNT4_SHFT                                               0x0

#define HWIO_MSS_CPR_GCNT5_ADDR                                              (MSS_CPR3_REG_BASE      + 0x000000b4)
#define HWIO_MSS_CPR_GCNT5_RMSK                                                   0x3ff
#define HWIO_MSS_CPR_GCNT5_IN          \
        in_dword_masked(HWIO_MSS_CPR_GCNT5_ADDR, HWIO_MSS_CPR_GCNT5_RMSK)
#define HWIO_MSS_CPR_GCNT5_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_GCNT5_ADDR, m)
#define HWIO_MSS_CPR_GCNT5_OUT(v)      \
        out_dword(HWIO_MSS_CPR_GCNT5_ADDR,v)
#define HWIO_MSS_CPR_GCNT5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_GCNT5_ADDR,m,v,HWIO_MSS_CPR_GCNT5_IN)
#define HWIO_MSS_CPR_GCNT5_GCNT5_BMSK                                             0x3ff
#define HWIO_MSS_CPR_GCNT5_GCNT5_SHFT                                               0x0

#define HWIO_MSS_CPR_GCNT6_ADDR                                              (MSS_CPR3_REG_BASE      + 0x000000b8)
#define HWIO_MSS_CPR_GCNT6_RMSK                                                   0x3ff
#define HWIO_MSS_CPR_GCNT6_IN          \
        in_dword_masked(HWIO_MSS_CPR_GCNT6_ADDR, HWIO_MSS_CPR_GCNT6_RMSK)
#define HWIO_MSS_CPR_GCNT6_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_GCNT6_ADDR, m)
#define HWIO_MSS_CPR_GCNT6_OUT(v)      \
        out_dword(HWIO_MSS_CPR_GCNT6_ADDR,v)
#define HWIO_MSS_CPR_GCNT6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_GCNT6_ADDR,m,v,HWIO_MSS_CPR_GCNT6_IN)
#define HWIO_MSS_CPR_GCNT6_GCNT6_BMSK                                             0x3ff
#define HWIO_MSS_CPR_GCNT6_GCNT6_SHFT                                               0x0

#define HWIO_MSS_CPR_GCNT7_ADDR                                              (MSS_CPR3_REG_BASE      + 0x000000bc)
#define HWIO_MSS_CPR_GCNT7_RMSK                                                   0x3ff
#define HWIO_MSS_CPR_GCNT7_IN          \
        in_dword_masked(HWIO_MSS_CPR_GCNT7_ADDR, HWIO_MSS_CPR_GCNT7_RMSK)
#define HWIO_MSS_CPR_GCNT7_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_GCNT7_ADDR, m)
#define HWIO_MSS_CPR_GCNT7_OUT(v)      \
        out_dword(HWIO_MSS_CPR_GCNT7_ADDR,v)
#define HWIO_MSS_CPR_GCNT7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_GCNT7_ADDR,m,v,HWIO_MSS_CPR_GCNT7_IN)
#define HWIO_MSS_CPR_GCNT7_GCNT7_BMSK                                             0x3ff
#define HWIO_MSS_CPR_GCNT7_GCNT7_SHFT                                               0x0

#define HWIO_MSS_CPR_GCNT8_ADDR                                              (MSS_CPR3_REG_BASE      + 0x000000c0)
#define HWIO_MSS_CPR_GCNT8_RMSK                                                   0x3ff
#define HWIO_MSS_CPR_GCNT8_IN          \
        in_dword_masked(HWIO_MSS_CPR_GCNT8_ADDR, HWIO_MSS_CPR_GCNT8_RMSK)
#define HWIO_MSS_CPR_GCNT8_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_GCNT8_ADDR, m)
#define HWIO_MSS_CPR_GCNT8_OUT(v)      \
        out_dword(HWIO_MSS_CPR_GCNT8_ADDR,v)
#define HWIO_MSS_CPR_GCNT8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_GCNT8_ADDR,m,v,HWIO_MSS_CPR_GCNT8_IN)
#define HWIO_MSS_CPR_GCNT8_GCNT8_BMSK                                             0x3ff
#define HWIO_MSS_CPR_GCNT8_GCNT8_SHFT                                               0x0

#define HWIO_MSS_CPR_GCNT9_ADDR                                              (MSS_CPR3_REG_BASE      + 0x000000c4)
#define HWIO_MSS_CPR_GCNT9_RMSK                                                   0x3ff
#define HWIO_MSS_CPR_GCNT9_IN          \
        in_dword_masked(HWIO_MSS_CPR_GCNT9_ADDR, HWIO_MSS_CPR_GCNT9_RMSK)
#define HWIO_MSS_CPR_GCNT9_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_GCNT9_ADDR, m)
#define HWIO_MSS_CPR_GCNT9_OUT(v)      \
        out_dword(HWIO_MSS_CPR_GCNT9_ADDR,v)
#define HWIO_MSS_CPR_GCNT9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_GCNT9_ADDR,m,v,HWIO_MSS_CPR_GCNT9_IN)
#define HWIO_MSS_CPR_GCNT9_GCNT9_BMSK                                             0x3ff
#define HWIO_MSS_CPR_GCNT9_GCNT9_SHFT                                               0x0

#define HWIO_MSS_CPR_GCNT10_ADDR                                             (MSS_CPR3_REG_BASE      + 0x000000c8)
#define HWIO_MSS_CPR_GCNT10_RMSK                                                  0x3ff
#define HWIO_MSS_CPR_GCNT10_IN          \
        in_dword_masked(HWIO_MSS_CPR_GCNT10_ADDR, HWIO_MSS_CPR_GCNT10_RMSK)
#define HWIO_MSS_CPR_GCNT10_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_GCNT10_ADDR, m)
#define HWIO_MSS_CPR_GCNT10_OUT(v)      \
        out_dword(HWIO_MSS_CPR_GCNT10_ADDR,v)
#define HWIO_MSS_CPR_GCNT10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_GCNT10_ADDR,m,v,HWIO_MSS_CPR_GCNT10_IN)
#define HWIO_MSS_CPR_GCNT10_GCNT10_BMSK                                           0x3ff
#define HWIO_MSS_CPR_GCNT10_GCNT10_SHFT                                             0x0

#define HWIO_MSS_CPR_GCNT11_ADDR                                             (MSS_CPR3_REG_BASE      + 0x000000cc)
#define HWIO_MSS_CPR_GCNT11_RMSK                                                  0x3ff
#define HWIO_MSS_CPR_GCNT11_IN          \
        in_dword_masked(HWIO_MSS_CPR_GCNT11_ADDR, HWIO_MSS_CPR_GCNT11_RMSK)
#define HWIO_MSS_CPR_GCNT11_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_GCNT11_ADDR, m)
#define HWIO_MSS_CPR_GCNT11_OUT(v)      \
        out_dword(HWIO_MSS_CPR_GCNT11_ADDR,v)
#define HWIO_MSS_CPR_GCNT11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_GCNT11_ADDR,m,v,HWIO_MSS_CPR_GCNT11_IN)
#define HWIO_MSS_CPR_GCNT11_GCNT11_BMSK                                           0x3ff
#define HWIO_MSS_CPR_GCNT11_GCNT11_SHFT                                             0x0

#define HWIO_MSS_CPR_GCNT12_ADDR                                             (MSS_CPR3_REG_BASE      + 0x000000d0)
#define HWIO_MSS_CPR_GCNT12_RMSK                                                  0x3ff
#define HWIO_MSS_CPR_GCNT12_IN          \
        in_dword_masked(HWIO_MSS_CPR_GCNT12_ADDR, HWIO_MSS_CPR_GCNT12_RMSK)
#define HWIO_MSS_CPR_GCNT12_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_GCNT12_ADDR, m)
#define HWIO_MSS_CPR_GCNT12_OUT(v)      \
        out_dword(HWIO_MSS_CPR_GCNT12_ADDR,v)
#define HWIO_MSS_CPR_GCNT12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_GCNT12_ADDR,m,v,HWIO_MSS_CPR_GCNT12_IN)
#define HWIO_MSS_CPR_GCNT12_GCNT12_BMSK                                           0x3ff
#define HWIO_MSS_CPR_GCNT12_GCNT12_SHFT                                             0x0

#define HWIO_MSS_CPR_GCNT13_ADDR                                             (MSS_CPR3_REG_BASE      + 0x000000d4)
#define HWIO_MSS_CPR_GCNT13_RMSK                                                  0x3ff
#define HWIO_MSS_CPR_GCNT13_IN          \
        in_dword_masked(HWIO_MSS_CPR_GCNT13_ADDR, HWIO_MSS_CPR_GCNT13_RMSK)
#define HWIO_MSS_CPR_GCNT13_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_GCNT13_ADDR, m)
#define HWIO_MSS_CPR_GCNT13_OUT(v)      \
        out_dword(HWIO_MSS_CPR_GCNT13_ADDR,v)
#define HWIO_MSS_CPR_GCNT13_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_GCNT13_ADDR,m,v,HWIO_MSS_CPR_GCNT13_IN)
#define HWIO_MSS_CPR_GCNT13_GCNT13_BMSK                                           0x3ff
#define HWIO_MSS_CPR_GCNT13_GCNT13_SHFT                                             0x0

#define HWIO_MSS_CPR_GCNT14_ADDR                                             (MSS_CPR3_REG_BASE      + 0x000000d8)
#define HWIO_MSS_CPR_GCNT14_RMSK                                                  0x3ff
#define HWIO_MSS_CPR_GCNT14_IN          \
        in_dword_masked(HWIO_MSS_CPR_GCNT14_ADDR, HWIO_MSS_CPR_GCNT14_RMSK)
#define HWIO_MSS_CPR_GCNT14_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_GCNT14_ADDR, m)
#define HWIO_MSS_CPR_GCNT14_OUT(v)      \
        out_dword(HWIO_MSS_CPR_GCNT14_ADDR,v)
#define HWIO_MSS_CPR_GCNT14_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_GCNT14_ADDR,m,v,HWIO_MSS_CPR_GCNT14_IN)
#define HWIO_MSS_CPR_GCNT14_GCNT14_BMSK                                           0x3ff
#define HWIO_MSS_CPR_GCNT14_GCNT14_SHFT                                             0x0

#define HWIO_MSS_CPR_GCNT15_ADDR                                             (MSS_CPR3_REG_BASE      + 0x000000dc)
#define HWIO_MSS_CPR_GCNT15_RMSK                                                  0x3ff
#define HWIO_MSS_CPR_GCNT15_IN          \
        in_dword_masked(HWIO_MSS_CPR_GCNT15_ADDR, HWIO_MSS_CPR_GCNT15_RMSK)
#define HWIO_MSS_CPR_GCNT15_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_GCNT15_ADDR, m)
#define HWIO_MSS_CPR_GCNT15_OUT(v)      \
        out_dword(HWIO_MSS_CPR_GCNT15_ADDR,v)
#define HWIO_MSS_CPR_GCNT15_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_GCNT15_ADDR,m,v,HWIO_MSS_CPR_GCNT15_IN)
#define HWIO_MSS_CPR_GCNT15_GCNT15_BMSK                                           0x3ff
#define HWIO_MSS_CPR_GCNT15_GCNT15_SHFT                                             0x0

#define HWIO_MSS_CPR_SENSOR_BYPASS_WRITEn_ADDR(n)                            (MSS_CPR3_REG_BASE      + 0x000000e0 + 0x4 * (n))
#define HWIO_MSS_CPR_SENSOR_BYPASS_WRITEn_RMSK                               0xffffffff
#define HWIO_MSS_CPR_SENSOR_BYPASS_WRITEn_MAXn                                        0
#define HWIO_MSS_CPR_SENSOR_BYPASS_WRITEn_OUTI(n,val)    \
        out_dword(HWIO_MSS_CPR_SENSOR_BYPASS_WRITEn_ADDR(n),val)
#define HWIO_MSS_CPR_SENSOR_BYPASS_WRITEn_SENSOR_BYPASS_BMSK                 0xffffffff
#define HWIO_MSS_CPR_SENSOR_BYPASS_WRITEn_SENSOR_BYPASS_SHFT                        0x0

#define HWIO_MSS_CPR_SENSOR_BYPASS_READn_ADDR(n)                             (MSS_CPR3_REG_BASE      + 0x00000100 + 0x4 * (n))
#define HWIO_MSS_CPR_SENSOR_BYPASS_READn_RMSK                                0xffffffff
#define HWIO_MSS_CPR_SENSOR_BYPASS_READn_MAXn                                         0
#define HWIO_MSS_CPR_SENSOR_BYPASS_READn_INI(n)        \
        in_dword_masked(HWIO_MSS_CPR_SENSOR_BYPASS_READn_ADDR(n), HWIO_MSS_CPR_SENSOR_BYPASS_READn_RMSK)
#define HWIO_MSS_CPR_SENSOR_BYPASS_READn_INMI(n,mask)    \
        in_dword_masked(HWIO_MSS_CPR_SENSOR_BYPASS_READn_ADDR(n), mask)
#define HWIO_MSS_CPR_SENSOR_BYPASS_READn_SENSOR_BYPASS_BMSK                  0xffffffff
#define HWIO_MSS_CPR_SENSOR_BYPASS_READn_SENSOR_BYPASS_SHFT                         0x0

#define HWIO_MSS_CPR_SENSOR_MASK_WRITEn_ADDR(n)                              (MSS_CPR3_REG_BASE      + 0x00000120 + 0x4 * (n))
#define HWIO_MSS_CPR_SENSOR_MASK_WRITEn_RMSK                                 0xffffffff
#define HWIO_MSS_CPR_SENSOR_MASK_WRITEn_MAXn                                          0
#define HWIO_MSS_CPR_SENSOR_MASK_WRITEn_OUTI(n,val)    \
        out_dword(HWIO_MSS_CPR_SENSOR_MASK_WRITEn_ADDR(n),val)
#define HWIO_MSS_CPR_SENSOR_MASK_WRITEn_SENSOR_MASK_BMSK                     0xffffffff
#define HWIO_MSS_CPR_SENSOR_MASK_WRITEn_SENSOR_MASK_SHFT                            0x0

#define HWIO_MSS_CPR_SENSOR_MASK_READn_ADDR(n)                               (MSS_CPR3_REG_BASE      + 0x00000140 + 0x4 * (n))
#define HWIO_MSS_CPR_SENSOR_MASK_READn_RMSK                                  0xffffffff
#define HWIO_MSS_CPR_SENSOR_MASK_READn_MAXn                                           0
#define HWIO_MSS_CPR_SENSOR_MASK_READn_INI(n)        \
        in_dword_masked(HWIO_MSS_CPR_SENSOR_MASK_READn_ADDR(n), HWIO_MSS_CPR_SENSOR_MASK_READn_RMSK)
#define HWIO_MSS_CPR_SENSOR_MASK_READn_INMI(n,mask)    \
        in_dword_masked(HWIO_MSS_CPR_SENSOR_MASK_READn_ADDR(n), mask)
#define HWIO_MSS_CPR_SENSOR_MASK_READn_SENSOR_MASK_BMSK                      0xffffffff
#define HWIO_MSS_CPR_SENSOR_MASK_READn_SENSOR_MASK_SHFT                             0x0

#define HWIO_MSS_CPR_SENSOR_THREAD_n_ADDR(n)                                 (MSS_CPR3_REG_BASE      + 0x00000200 + 0x4 * (n))
#define HWIO_MSS_CPR_SENSOR_THREAD_n_RMSK                                           0x1
#define HWIO_MSS_CPR_SENSOR_THREAD_n_MAXn                                            20
#define HWIO_MSS_CPR_SENSOR_THREAD_n_INI(n)        \
        in_dword_masked(HWIO_MSS_CPR_SENSOR_THREAD_n_ADDR(n), HWIO_MSS_CPR_SENSOR_THREAD_n_RMSK)
#define HWIO_MSS_CPR_SENSOR_THREAD_n_INMI(n,mask)    \
        in_dword_masked(HWIO_MSS_CPR_SENSOR_THREAD_n_ADDR(n), mask)
#define HWIO_MSS_CPR_SENSOR_THREAD_n_OUTI(n,val)    \
        out_dword(HWIO_MSS_CPR_SENSOR_THREAD_n_ADDR(n),val)
#define HWIO_MSS_CPR_SENSOR_THREAD_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_SENSOR_THREAD_n_ADDR(n),mask,val,HWIO_MSS_CPR_SENSOR_THREAD_n_INI(n))
#define HWIO_MSS_CPR_SENSOR_THREAD_n_THREAD_ID_BMSK                                 0x1
#define HWIO_MSS_CPR_SENSOR_THREAD_n_THREAD_ID_SHFT                                 0x0

#define HWIO_MSS_CPR_HW_STARVATION_THRESHOLD_ADDR                            (MSS_CPR3_REG_BASE      + 0x000007fc)
#define HWIO_MSS_CPR_HW_STARVATION_THRESHOLD_RMSK                            0xffffffff
#define HWIO_MSS_CPR_HW_STARVATION_THRESHOLD_IN          \
        in_dword_masked(HWIO_MSS_CPR_HW_STARVATION_THRESHOLD_ADDR, HWIO_MSS_CPR_HW_STARVATION_THRESHOLD_RMSK)
#define HWIO_MSS_CPR_HW_STARVATION_THRESHOLD_INM(m)      \
        in_dword_masked(HWIO_MSS_CPR_HW_STARVATION_THRESHOLD_ADDR, m)
#define HWIO_MSS_CPR_HW_STARVATION_THRESHOLD_OUT(v)      \
        out_dword(HWIO_MSS_CPR_HW_STARVATION_THRESHOLD_ADDR,v)
#define HWIO_MSS_CPR_HW_STARVATION_THRESHOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CPR_HW_STARVATION_THRESHOLD_ADDR,m,v,HWIO_MSS_CPR_HW_STARVATION_THRESHOLD_IN)
#define HWIO_MSS_CPR_HW_STARVATION_THRESHOLD_THRESHOLD_BMSK                  0xffffffff
#define HWIO_MSS_CPR_HW_STARVATION_THRESHOLD_THRESHOLD_SHFT                         0x0

#define HWIO_MSS_CPR_CONT_CMD_t_ADDR(t)                                      (MSS_CPR3_REG_BASE      + 0x00000800 + 0x440 * (t))
#define HWIO_MSS_CPR_CONT_CMD_t_RMSK                                                0x1
#define HWIO_MSS_CPR_CONT_CMD_t_MAXt                                                  0
#define HWIO_MSS_CPR_CONT_CMD_t_OUTI(t,val)    \
        out_dword(HWIO_MSS_CPR_CONT_CMD_t_ADDR(t),val)
#define HWIO_MSS_CPR_CONT_CMD_t_VDD_CHANGED_ONE_STEP_BMSK                           0x1
#define HWIO_MSS_CPR_CONT_CMD_t_VDD_CHANGED_ONE_STEP_SHFT                           0x0

#define HWIO_MSS_CPR_THRESHOLD_t_ADDR(t)                                     (MSS_CPR3_REG_BASE      + 0x00000808 + 0x440 * (t))
#define HWIO_MSS_CPR_THRESHOLD_t_RMSK                                           0x3ffff
#define HWIO_MSS_CPR_THRESHOLD_t_MAXt                                                 0
#define HWIO_MSS_CPR_THRESHOLD_t_INI(t)        \
        in_dword_masked(HWIO_MSS_CPR_THRESHOLD_t_ADDR(t), HWIO_MSS_CPR_THRESHOLD_t_RMSK)
#define HWIO_MSS_CPR_THRESHOLD_t_INMI(t,mask)    \
        in_dword_masked(HWIO_MSS_CPR_THRESHOLD_t_ADDR(t), mask)
#define HWIO_MSS_CPR_THRESHOLD_t_OUTI(t,val)    \
        out_dword(HWIO_MSS_CPR_THRESHOLD_t_ADDR(t),val)
#define HWIO_MSS_CPR_THRESHOLD_t_OUTMI(t,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_THRESHOLD_t_ADDR(t),mask,val,HWIO_MSS_CPR_THRESHOLD_t_INI(t))
#define HWIO_MSS_CPR_THRESHOLD_t_UP_THRESHOLD_BMSK                              0x3e000
#define HWIO_MSS_CPR_THRESHOLD_t_UP_THRESHOLD_SHFT                                  0xd
#define HWIO_MSS_CPR_THRESHOLD_t_DN_THRESHOLD_BMSK                               0x1f00
#define HWIO_MSS_CPR_THRESHOLD_t_DN_THRESHOLD_SHFT                                  0x8
#define HWIO_MSS_CPR_THRESHOLD_t_CONSECUTIVE_UP_BMSK                               0xf0
#define HWIO_MSS_CPR_THRESHOLD_t_CONSECUTIVE_UP_SHFT                                0x4
#define HWIO_MSS_CPR_THRESHOLD_t_CONSECUTIVE_DN_BMSK                                0xf
#define HWIO_MSS_CPR_THRESHOLD_t_CONSECUTIVE_DN_SHFT                                0x0

#define HWIO_MSS_CPR_MASK_THREAD_t_ADDR(t)                                   (MSS_CPR3_REG_BASE      + 0x0000080c + 0x440 * (t))
#define HWIO_MSS_CPR_MASK_THREAD_t_RMSK                                      0x8000ffff
#define HWIO_MSS_CPR_MASK_THREAD_t_MAXt                                               0
#define HWIO_MSS_CPR_MASK_THREAD_t_INI(t)        \
        in_dword_masked(HWIO_MSS_CPR_MASK_THREAD_t_ADDR(t), HWIO_MSS_CPR_MASK_THREAD_t_RMSK)
#define HWIO_MSS_CPR_MASK_THREAD_t_INMI(t,mask)    \
        in_dword_masked(HWIO_MSS_CPR_MASK_THREAD_t_ADDR(t), mask)
#define HWIO_MSS_CPR_MASK_THREAD_t_OUTI(t,val)    \
        out_dword(HWIO_MSS_CPR_MASK_THREAD_t_ADDR(t),val)
#define HWIO_MSS_CPR_MASK_THREAD_t_OUTMI(t,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_MASK_THREAD_t_ADDR(t),mask,val,HWIO_MSS_CPR_MASK_THREAD_t_INI(t))
#define HWIO_MSS_CPR_MASK_THREAD_t_DISABLE_THREAD_BMSK                       0x80000000
#define HWIO_MSS_CPR_MASK_THREAD_t_DISABLE_THREAD_SHFT                             0x1f
#define HWIO_MSS_CPR_MASK_THREAD_t_RO_MASK4THREAD_BMSK                           0xffff
#define HWIO_MSS_CPR_MASK_THREAD_t_RO_MASK4THREAD_SHFT                              0x0

#define HWIO_MSS_CPR_RESULT0_t_ADDR(t)                                       (MSS_CPR3_REG_BASE      + 0x00000810 + 0x440 * (t))
#define HWIO_MSS_CPR_RESULT0_t_RMSK                                            0x1fffff
#define HWIO_MSS_CPR_RESULT0_t_MAXt                                                   0
#define HWIO_MSS_CPR_RESULT0_t_INI(t)        \
        in_dword_masked(HWIO_MSS_CPR_RESULT0_t_ADDR(t), HWIO_MSS_CPR_RESULT0_t_RMSK)
#define HWIO_MSS_CPR_RESULT0_t_INMI(t,mask)    \
        in_dword_masked(HWIO_MSS_CPR_RESULT0_t_ADDR(t), mask)
#define HWIO_MSS_CPR_RESULT0_t_ERROR_LT_0_BMSK                                 0x100000
#define HWIO_MSS_CPR_RESULT0_t_ERROR_LT_0_SHFT                                     0x14
#define HWIO_MSS_CPR_RESULT0_t_ERROR_BMSK                                       0xfff00
#define HWIO_MSS_CPR_RESULT0_t_ERROR_SHFT                                           0x8
#define HWIO_MSS_CPR_RESULT0_t_ERROR_STEPS_BMSK                                    0xf8
#define HWIO_MSS_CPR_RESULT0_t_ERROR_STEPS_SHFT                                     0x3
#define HWIO_MSS_CPR_RESULT0_t_STEP_UP_BMSK                                         0x4
#define HWIO_MSS_CPR_RESULT0_t_STEP_UP_SHFT                                         0x2
#define HWIO_MSS_CPR_RESULT0_t_STEP_DN_BMSK                                         0x2
#define HWIO_MSS_CPR_RESULT0_t_STEP_DN_SHFT                                         0x1
#define HWIO_MSS_CPR_RESULT0_t_BUSY_BMSK                                            0x1
#define HWIO_MSS_CPR_RESULT0_t_BUSY_SHFT                                            0x0

#define HWIO_MSS_CPR_RESULT1_t_ADDR(t)                                       (MSS_CPR3_REG_BASE      + 0x00000814 + 0x440 * (t))
#define HWIO_MSS_CPR_RESULT1_t_RMSK                                          0xffffffff
#define HWIO_MSS_CPR_RESULT1_t_MAXt                                                   0
#define HWIO_MSS_CPR_RESULT1_t_INI(t)        \
        in_dword_masked(HWIO_MSS_CPR_RESULT1_t_ADDR(t), HWIO_MSS_CPR_RESULT1_t_RMSK)
#define HWIO_MSS_CPR_RESULT1_t_INMI(t,mask)    \
        in_dword_masked(HWIO_MSS_CPR_RESULT1_t_ADDR(t), mask)
#define HWIO_MSS_CPR_RESULT1_t_SEL_MAX_BMSK                                  0xf0000000
#define HWIO_MSS_CPR_RESULT1_t_SEL_MAX_SHFT                                        0x1c
#define HWIO_MSS_CPR_RESULT1_t_SEL_MIN_BMSK                                   0xf000000
#define HWIO_MSS_CPR_RESULT1_t_SEL_MIN_SHFT                                        0x18
#define HWIO_MSS_CPR_RESULT1_t_QUOT_MAX_BMSK                                   0xfff000
#define HWIO_MSS_CPR_RESULT1_t_QUOT_MAX_SHFT                                        0xc
#define HWIO_MSS_CPR_RESULT1_t_QUOT_MIN_BMSK                                      0xfff
#define HWIO_MSS_CPR_RESULT1_t_QUOT_MIN_SHFT                                        0x0

#define HWIO_MSS_CPR_RESULT2_t_ADDR(t)                                       (MSS_CPR3_REG_BASE      + 0x00000818 + 0x440 * (t))
#define HWIO_MSS_CPR_RESULT2_t_RMSK                                          0xffff0fff
#define HWIO_MSS_CPR_RESULT2_t_MAXt                                                   0
#define HWIO_MSS_CPR_RESULT2_t_INI(t)        \
        in_dword_masked(HWIO_MSS_CPR_RESULT2_t_ADDR(t), HWIO_MSS_CPR_RESULT2_t_RMSK)
#define HWIO_MSS_CPR_RESULT2_t_INMI(t,mask)    \
        in_dword_masked(HWIO_MSS_CPR_RESULT2_t_ADDR(t), mask)
#define HWIO_MSS_CPR_RESULT2_t_SENSOR_MAX_BMSK                               0xff000000
#define HWIO_MSS_CPR_RESULT2_t_SENSOR_MAX_SHFT                                     0x18
#define HWIO_MSS_CPR_RESULT2_t_SENSOR_MIN_BMSK                                 0xff0000
#define HWIO_MSS_CPR_RESULT2_t_SENSOR_MIN_SHFT                                     0x10
#define HWIO_MSS_CPR_RESULT2_t_STEP_QUOT_MAX_BMSK                                 0xfc0
#define HWIO_MSS_CPR_RESULT2_t_STEP_QUOT_MAX_SHFT                                   0x6
#define HWIO_MSS_CPR_RESULT2_t_STEP_QUOT_MIN_BMSK                                  0x3f
#define HWIO_MSS_CPR_RESULT2_t_STEP_QUOT_MIN_SHFT                                   0x0

#define HWIO_MSS_CPR_IRQ_EN_t_ADDR(t)                                        (MSS_CPR3_REG_BASE      + 0x0000081c + 0x440 * (t))
#define HWIO_MSS_CPR_IRQ_EN_t_RMSK                                           0xc000000f
#define HWIO_MSS_CPR_IRQ_EN_t_MAXt                                                    0
#define HWIO_MSS_CPR_IRQ_EN_t_INI(t)        \
        in_dword_masked(HWIO_MSS_CPR_IRQ_EN_t_ADDR(t), HWIO_MSS_CPR_IRQ_EN_t_RMSK)
#define HWIO_MSS_CPR_IRQ_EN_t_INMI(t,mask)    \
        in_dword_masked(HWIO_MSS_CPR_IRQ_EN_t_ADDR(t), mask)
#define HWIO_MSS_CPR_IRQ_EN_t_OUTI(t,val)    \
        out_dword(HWIO_MSS_CPR_IRQ_EN_t_ADDR(t),val)
#define HWIO_MSS_CPR_IRQ_EN_t_OUTMI(t,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_IRQ_EN_t_ADDR(t),mask,val,HWIO_MSS_CPR_IRQ_EN_t_INI(t))
#define HWIO_MSS_CPR_IRQ_EN_t_CLAMP_CHANGE_WHILE_BUSY_EN_BMSK                0x80000000
#define HWIO_MSS_CPR_IRQ_EN_t_CLAMP_CHANGE_WHILE_BUSY_EN_SHFT                      0x1f
#define HWIO_MSS_CPR_IRQ_EN_t_STARVATION_ERROR_EN_BMSK                       0x40000000
#define HWIO_MSS_CPR_IRQ_EN_t_STARVATION_ERROR_EN_SHFT                             0x1e
#define HWIO_MSS_CPR_IRQ_EN_t_UP_FLAG_EN_BMSK                                       0x8
#define HWIO_MSS_CPR_IRQ_EN_t_UP_FLAG_EN_SHFT                                       0x3
#define HWIO_MSS_CPR_IRQ_EN_t_MID_FLAG_EN_BMSK                                      0x4
#define HWIO_MSS_CPR_IRQ_EN_t_MID_FLAG_EN_SHFT                                      0x2
#define HWIO_MSS_CPR_IRQ_EN_t_DOWN_FLAG_EN_BMSK                                     0x2
#define HWIO_MSS_CPR_IRQ_EN_t_DOWN_FLAG_EN_SHFT                                     0x1
#define HWIO_MSS_CPR_IRQ_EN_t_CPR_DONE_EN_BMSK                                      0x1
#define HWIO_MSS_CPR_IRQ_EN_t_CPR_DONE_EN_SHFT                                      0x0

#define HWIO_MSS_CPR_IRQ_CLEAR_t_ADDR(t)                                     (MSS_CPR3_REG_BASE      + 0x00000820 + 0x440 * (t))
#define HWIO_MSS_CPR_IRQ_CLEAR_t_RMSK                                        0xc000000f
#define HWIO_MSS_CPR_IRQ_CLEAR_t_MAXt                                                 0
#define HWIO_MSS_CPR_IRQ_CLEAR_t_OUTI(t,val)    \
        out_dword(HWIO_MSS_CPR_IRQ_CLEAR_t_ADDR(t),val)
#define HWIO_MSS_CPR_IRQ_CLEAR_t_CLAMP_CHANGE_WHILE_BUSY_CLEAR_BMSK          0x80000000
#define HWIO_MSS_CPR_IRQ_CLEAR_t_CLAMP_CHANGE_WHILE_BUSY_CLEAR_SHFT                0x1f
#define HWIO_MSS_CPR_IRQ_CLEAR_t_STARVATION_ERROR_CLEAR_BMSK                 0x40000000
#define HWIO_MSS_CPR_IRQ_CLEAR_t_STARVATION_ERROR_CLEAR_SHFT                       0x1e
#define HWIO_MSS_CPR_IRQ_CLEAR_t_UP_FLAG_CLEAR_BMSK                                 0x8
#define HWIO_MSS_CPR_IRQ_CLEAR_t_UP_FLAG_CLEAR_SHFT                                 0x3
#define HWIO_MSS_CPR_IRQ_CLEAR_t_MID_FLAG_CLEAR_BMSK                                0x4
#define HWIO_MSS_CPR_IRQ_CLEAR_t_MID_FLAG_CLEAR_SHFT                                0x2
#define HWIO_MSS_CPR_IRQ_CLEAR_t_DOWN_FLAG_CLEAR_BMSK                               0x2
#define HWIO_MSS_CPR_IRQ_CLEAR_t_DOWN_FLAG_CLEAR_SHFT                               0x1
#define HWIO_MSS_CPR_IRQ_CLEAR_t_CPR_DONE_CLEAR_BMSK                                0x1
#define HWIO_MSS_CPR_IRQ_CLEAR_t_CPR_DONE_CLEAR_SHFT                                0x0

#define HWIO_MSS_CPR_IRQ_STATUS_t_ADDR(t)                                    (MSS_CPR3_REG_BASE      + 0x00000824 + 0x440 * (t))
#define HWIO_MSS_CPR_IRQ_STATUS_t_RMSK                                       0xc000000f
#define HWIO_MSS_CPR_IRQ_STATUS_t_MAXt                                                0
#define HWIO_MSS_CPR_IRQ_STATUS_t_INI(t)        \
        in_dword_masked(HWIO_MSS_CPR_IRQ_STATUS_t_ADDR(t), HWIO_MSS_CPR_IRQ_STATUS_t_RMSK)
#define HWIO_MSS_CPR_IRQ_STATUS_t_INMI(t,mask)    \
        in_dword_masked(HWIO_MSS_CPR_IRQ_STATUS_t_ADDR(t), mask)
#define HWIO_MSS_CPR_IRQ_STATUS_t_CLAMP_CHANGE_WHILE_BUSY_BMSK               0x80000000
#define HWIO_MSS_CPR_IRQ_STATUS_t_CLAMP_CHANGE_WHILE_BUSY_SHFT                     0x1f
#define HWIO_MSS_CPR_IRQ_STATUS_t_STARVATION_ERROR_BMSK                      0x40000000
#define HWIO_MSS_CPR_IRQ_STATUS_t_STARVATION_ERROR_SHFT                            0x1e
#define HWIO_MSS_CPR_IRQ_STATUS_t_UP_FLAG_BMSK                                      0x8
#define HWIO_MSS_CPR_IRQ_STATUS_t_UP_FLAG_SHFT                                      0x3
#define HWIO_MSS_CPR_IRQ_STATUS_t_MID_FLAG_BMSK                                     0x4
#define HWIO_MSS_CPR_IRQ_STATUS_t_MID_FLAG_SHFT                                     0x2
#define HWIO_MSS_CPR_IRQ_STATUS_t_DOWN_FLAG_BMSK                                    0x2
#define HWIO_MSS_CPR_IRQ_STATUS_t_DOWN_FLAG_SHFT                                    0x1
#define HWIO_MSS_CPR_IRQ_STATUS_t_CPR_DONE_BMSK                                     0x1
#define HWIO_MSS_CPR_IRQ_STATUS_t_CPR_DONE_SHFT                                     0x0

#define HWIO_MSS_CPR_TARGET_MODE_SELECT_t_ADDR(t)                            (MSS_CPR3_REG_BASE      + 0x00000828 + 0x440 * (t))
#define HWIO_MSS_CPR_TARGET_MODE_SELECT_t_RMSK                                      0xf
#define HWIO_MSS_CPR_TARGET_MODE_SELECT_t_MAXt                                        0
#define HWIO_MSS_CPR_TARGET_MODE_SELECT_t_INI(t)        \
        in_dword_masked(HWIO_MSS_CPR_TARGET_MODE_SELECT_t_ADDR(t), HWIO_MSS_CPR_TARGET_MODE_SELECT_t_RMSK)
#define HWIO_MSS_CPR_TARGET_MODE_SELECT_t_INMI(t,mask)    \
        in_dword_masked(HWIO_MSS_CPR_TARGET_MODE_SELECT_t_ADDR(t), mask)
#define HWIO_MSS_CPR_TARGET_MODE_SELECT_t_OUTI(t,val)    \
        out_dword(HWIO_MSS_CPR_TARGET_MODE_SELECT_t_ADDR(t),val)
#define HWIO_MSS_CPR_TARGET_MODE_SELECT_t_OUTMI(t,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_TARGET_MODE_SELECT_t_ADDR(t),mask,val,HWIO_MSS_CPR_TARGET_MODE_SELECT_t_INI(t))
#define HWIO_MSS_CPR_TARGET_MODE_SELECT_t_TARGET_MODE_SELECT_BMSK                   0xf
#define HWIO_MSS_CPR_TARGET_MODE_SELECT_t_TARGET_MODE_SELECT_SHFT                   0x0

#define HWIO_MSS_CPR_TARGET0_t_m_ADDR(t,m)                                   (MSS_CPR3_REG_BASE      + 0x00000840 + 0x440 * (t) + 0x40 * (m))
#define HWIO_MSS_CPR_TARGET0_t_m_RMSK                                             0xfff
#define HWIO_MSS_CPR_TARGET0_t_m_MAXt                                                 0
#define HWIO_MSS_CPR_TARGET0_t_m_MAXm                                                 3
#define HWIO_MSS_CPR_TARGET0_t_m_INI2(t,m)        \
        in_dword_masked(HWIO_MSS_CPR_TARGET0_t_m_ADDR(t,m), HWIO_MSS_CPR_TARGET0_t_m_RMSK)
#define HWIO_MSS_CPR_TARGET0_t_m_INMI2(t,m,mask)    \
        in_dword_masked(HWIO_MSS_CPR_TARGET0_t_m_ADDR(t,m), mask)
#define HWIO_MSS_CPR_TARGET0_t_m_OUTI2(t,m,val)    \
        out_dword(HWIO_MSS_CPR_TARGET0_t_m_ADDR(t,m),val)
#define HWIO_MSS_CPR_TARGET0_t_m_OUTMI2(t,m,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_TARGET0_t_m_ADDR(t,m),mask,val,HWIO_MSS_CPR_TARGET0_t_m_INI2(t,m))
#define HWIO_MSS_CPR_TARGET0_t_m_TARGET0_BMSK                                     0xfff
#define HWIO_MSS_CPR_TARGET0_t_m_TARGET0_SHFT                                       0x0

#define HWIO_MSS_CPR_TARGET1_t_m_ADDR(t,m)                                   (MSS_CPR3_REG_BASE      + 0x00000844 + 0x440 * (t) + 0x40 * (m))
#define HWIO_MSS_CPR_TARGET1_t_m_RMSK                                             0xfff
#define HWIO_MSS_CPR_TARGET1_t_m_MAXt                                                 0
#define HWIO_MSS_CPR_TARGET1_t_m_MAXm                                                 3
#define HWIO_MSS_CPR_TARGET1_t_m_INI2(t,m)        \
        in_dword_masked(HWIO_MSS_CPR_TARGET1_t_m_ADDR(t,m), HWIO_MSS_CPR_TARGET1_t_m_RMSK)
#define HWIO_MSS_CPR_TARGET1_t_m_INMI2(t,m,mask)    \
        in_dword_masked(HWIO_MSS_CPR_TARGET1_t_m_ADDR(t,m), mask)
#define HWIO_MSS_CPR_TARGET1_t_m_OUTI2(t,m,val)    \
        out_dword(HWIO_MSS_CPR_TARGET1_t_m_ADDR(t,m),val)
#define HWIO_MSS_CPR_TARGET1_t_m_OUTMI2(t,m,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_TARGET1_t_m_ADDR(t,m),mask,val,HWIO_MSS_CPR_TARGET1_t_m_INI2(t,m))
#define HWIO_MSS_CPR_TARGET1_t_m_TARGET1_BMSK                                     0xfff
#define HWIO_MSS_CPR_TARGET1_t_m_TARGET1_SHFT                                       0x0

#define HWIO_MSS_CPR_TARGET2_t_m_ADDR(t,m)                                   (MSS_CPR3_REG_BASE      + 0x00000848 + 0x440 * (t) + 0x40 * (m))
#define HWIO_MSS_CPR_TARGET2_t_m_RMSK                                             0xfff
#define HWIO_MSS_CPR_TARGET2_t_m_MAXt                                                 0
#define HWIO_MSS_CPR_TARGET2_t_m_MAXm                                                 3
#define HWIO_MSS_CPR_TARGET2_t_m_INI2(t,m)        \
        in_dword_masked(HWIO_MSS_CPR_TARGET2_t_m_ADDR(t,m), HWIO_MSS_CPR_TARGET2_t_m_RMSK)
#define HWIO_MSS_CPR_TARGET2_t_m_INMI2(t,m,mask)    \
        in_dword_masked(HWIO_MSS_CPR_TARGET2_t_m_ADDR(t,m), mask)
#define HWIO_MSS_CPR_TARGET2_t_m_OUTI2(t,m,val)    \
        out_dword(HWIO_MSS_CPR_TARGET2_t_m_ADDR(t,m),val)
#define HWIO_MSS_CPR_TARGET2_t_m_OUTMI2(t,m,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_TARGET2_t_m_ADDR(t,m),mask,val,HWIO_MSS_CPR_TARGET2_t_m_INI2(t,m))
#define HWIO_MSS_CPR_TARGET2_t_m_TARGET2_BMSK                                     0xfff
#define HWIO_MSS_CPR_TARGET2_t_m_TARGET2_SHFT                                       0x0

#define HWIO_MSS_CPR_TARGET3_t_m_ADDR(t,m)                                   (MSS_CPR3_REG_BASE      + 0x0000084c + 0x440 * (t) + 0x40 * (m))
#define HWIO_MSS_CPR_TARGET3_t_m_RMSK                                             0xfff
#define HWIO_MSS_CPR_TARGET3_t_m_MAXt                                                 0
#define HWIO_MSS_CPR_TARGET3_t_m_MAXm                                                 3
#define HWIO_MSS_CPR_TARGET3_t_m_INI2(t,m)        \
        in_dword_masked(HWIO_MSS_CPR_TARGET3_t_m_ADDR(t,m), HWIO_MSS_CPR_TARGET3_t_m_RMSK)
#define HWIO_MSS_CPR_TARGET3_t_m_INMI2(t,m,mask)    \
        in_dword_masked(HWIO_MSS_CPR_TARGET3_t_m_ADDR(t,m), mask)
#define HWIO_MSS_CPR_TARGET3_t_m_OUTI2(t,m,val)    \
        out_dword(HWIO_MSS_CPR_TARGET3_t_m_ADDR(t,m),val)
#define HWIO_MSS_CPR_TARGET3_t_m_OUTMI2(t,m,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_TARGET3_t_m_ADDR(t,m),mask,val,HWIO_MSS_CPR_TARGET3_t_m_INI2(t,m))
#define HWIO_MSS_CPR_TARGET3_t_m_TARGET3_BMSK                                     0xfff
#define HWIO_MSS_CPR_TARGET3_t_m_TARGET3_SHFT                                       0x0

#define HWIO_MSS_CPR_TARGET4_t_m_ADDR(t,m)                                   (MSS_CPR3_REG_BASE      + 0x00000850 + 0x440 * (t) + 0x40 * (m))
#define HWIO_MSS_CPR_TARGET4_t_m_RMSK                                             0xfff
#define HWIO_MSS_CPR_TARGET4_t_m_MAXt                                                 0
#define HWIO_MSS_CPR_TARGET4_t_m_MAXm                                                 3
#define HWIO_MSS_CPR_TARGET4_t_m_INI2(t,m)        \
        in_dword_masked(HWIO_MSS_CPR_TARGET4_t_m_ADDR(t,m), HWIO_MSS_CPR_TARGET4_t_m_RMSK)
#define HWIO_MSS_CPR_TARGET4_t_m_INMI2(t,m,mask)    \
        in_dword_masked(HWIO_MSS_CPR_TARGET4_t_m_ADDR(t,m), mask)
#define HWIO_MSS_CPR_TARGET4_t_m_OUTI2(t,m,val)    \
        out_dword(HWIO_MSS_CPR_TARGET4_t_m_ADDR(t,m),val)
#define HWIO_MSS_CPR_TARGET4_t_m_OUTMI2(t,m,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_TARGET4_t_m_ADDR(t,m),mask,val,HWIO_MSS_CPR_TARGET4_t_m_INI2(t,m))
#define HWIO_MSS_CPR_TARGET4_t_m_TARGET4_BMSK                                     0xfff
#define HWIO_MSS_CPR_TARGET4_t_m_TARGET4_SHFT                                       0x0

#define HWIO_MSS_CPR_TARGET5_t_m_ADDR(t,m)                                   (MSS_CPR3_REG_BASE      + 0x00000854 + 0x440 * (t) + 0x40 * (m))
#define HWIO_MSS_CPR_TARGET5_t_m_RMSK                                             0xfff
#define HWIO_MSS_CPR_TARGET5_t_m_MAXt                                                 0
#define HWIO_MSS_CPR_TARGET5_t_m_MAXm                                                 3
#define HWIO_MSS_CPR_TARGET5_t_m_INI2(t,m)        \
        in_dword_masked(HWIO_MSS_CPR_TARGET5_t_m_ADDR(t,m), HWIO_MSS_CPR_TARGET5_t_m_RMSK)
#define HWIO_MSS_CPR_TARGET5_t_m_INMI2(t,m,mask)    \
        in_dword_masked(HWIO_MSS_CPR_TARGET5_t_m_ADDR(t,m), mask)
#define HWIO_MSS_CPR_TARGET5_t_m_OUTI2(t,m,val)    \
        out_dword(HWIO_MSS_CPR_TARGET5_t_m_ADDR(t,m),val)
#define HWIO_MSS_CPR_TARGET5_t_m_OUTMI2(t,m,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_TARGET5_t_m_ADDR(t,m),mask,val,HWIO_MSS_CPR_TARGET5_t_m_INI2(t,m))
#define HWIO_MSS_CPR_TARGET5_t_m_TARGET5_BMSK                                     0xfff
#define HWIO_MSS_CPR_TARGET5_t_m_TARGET5_SHFT                                       0x0

#define HWIO_MSS_CPR_TARGET6_t_m_ADDR(t,m)                                   (MSS_CPR3_REG_BASE      + 0x00000858 + 0x440 * (t) + 0x40 * (m))
#define HWIO_MSS_CPR_TARGET6_t_m_RMSK                                             0xfff
#define HWIO_MSS_CPR_TARGET6_t_m_MAXt                                                 0
#define HWIO_MSS_CPR_TARGET6_t_m_MAXm                                                 3
#define HWIO_MSS_CPR_TARGET6_t_m_INI2(t,m)        \
        in_dword_masked(HWIO_MSS_CPR_TARGET6_t_m_ADDR(t,m), HWIO_MSS_CPR_TARGET6_t_m_RMSK)
#define HWIO_MSS_CPR_TARGET6_t_m_INMI2(t,m,mask)    \
        in_dword_masked(HWIO_MSS_CPR_TARGET6_t_m_ADDR(t,m), mask)
#define HWIO_MSS_CPR_TARGET6_t_m_OUTI2(t,m,val)    \
        out_dword(HWIO_MSS_CPR_TARGET6_t_m_ADDR(t,m),val)
#define HWIO_MSS_CPR_TARGET6_t_m_OUTMI2(t,m,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_TARGET6_t_m_ADDR(t,m),mask,val,HWIO_MSS_CPR_TARGET6_t_m_INI2(t,m))
#define HWIO_MSS_CPR_TARGET6_t_m_TARGET6_BMSK                                     0xfff
#define HWIO_MSS_CPR_TARGET6_t_m_TARGET6_SHFT                                       0x0

#define HWIO_MSS_CPR_TARGET7_t_m_ADDR(t,m)                                   (MSS_CPR3_REG_BASE      + 0x0000085c + 0x440 * (t) + 0x40 * (m))
#define HWIO_MSS_CPR_TARGET7_t_m_RMSK                                             0xfff
#define HWIO_MSS_CPR_TARGET7_t_m_MAXt                                                 0
#define HWIO_MSS_CPR_TARGET7_t_m_MAXm                                                 3
#define HWIO_MSS_CPR_TARGET7_t_m_INI2(t,m)        \
        in_dword_masked(HWIO_MSS_CPR_TARGET7_t_m_ADDR(t,m), HWIO_MSS_CPR_TARGET7_t_m_RMSK)
#define HWIO_MSS_CPR_TARGET7_t_m_INMI2(t,m,mask)    \
        in_dword_masked(HWIO_MSS_CPR_TARGET7_t_m_ADDR(t,m), mask)
#define HWIO_MSS_CPR_TARGET7_t_m_OUTI2(t,m,val)    \
        out_dword(HWIO_MSS_CPR_TARGET7_t_m_ADDR(t,m),val)
#define HWIO_MSS_CPR_TARGET7_t_m_OUTMI2(t,m,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_TARGET7_t_m_ADDR(t,m),mask,val,HWIO_MSS_CPR_TARGET7_t_m_INI2(t,m))
#define HWIO_MSS_CPR_TARGET7_t_m_TARGET7_BMSK                                     0xfff
#define HWIO_MSS_CPR_TARGET7_t_m_TARGET7_SHFT                                       0x0

#define HWIO_MSS_CPR_TARGET8_t_m_ADDR(t,m)                                   (MSS_CPR3_REG_BASE      + 0x00000860 + 0x440 * (t) + 0x40 * (m))
#define HWIO_MSS_CPR_TARGET8_t_m_RMSK                                             0xfff
#define HWIO_MSS_CPR_TARGET8_t_m_MAXt                                                 0
#define HWIO_MSS_CPR_TARGET8_t_m_MAXm                                                 3
#define HWIO_MSS_CPR_TARGET8_t_m_INI2(t,m)        \
        in_dword_masked(HWIO_MSS_CPR_TARGET8_t_m_ADDR(t,m), HWIO_MSS_CPR_TARGET8_t_m_RMSK)
#define HWIO_MSS_CPR_TARGET8_t_m_INMI2(t,m,mask)    \
        in_dword_masked(HWIO_MSS_CPR_TARGET8_t_m_ADDR(t,m), mask)
#define HWIO_MSS_CPR_TARGET8_t_m_OUTI2(t,m,val)    \
        out_dword(HWIO_MSS_CPR_TARGET8_t_m_ADDR(t,m),val)
#define HWIO_MSS_CPR_TARGET8_t_m_OUTMI2(t,m,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_TARGET8_t_m_ADDR(t,m),mask,val,HWIO_MSS_CPR_TARGET8_t_m_INI2(t,m))
#define HWIO_MSS_CPR_TARGET8_t_m_TARGET8_BMSK                                     0xfff
#define HWIO_MSS_CPR_TARGET8_t_m_TARGET8_SHFT                                       0x0

#define HWIO_MSS_CPR_TARGET9_t_m_ADDR(t,m)                                   (MSS_CPR3_REG_BASE      + 0x00000864 + 0x440 * (t) + 0x40 * (m))
#define HWIO_MSS_CPR_TARGET9_t_m_RMSK                                             0xfff
#define HWIO_MSS_CPR_TARGET9_t_m_MAXt                                                 0
#define HWIO_MSS_CPR_TARGET9_t_m_MAXm                                                 3
#define HWIO_MSS_CPR_TARGET9_t_m_INI2(t,m)        \
        in_dword_masked(HWIO_MSS_CPR_TARGET9_t_m_ADDR(t,m), HWIO_MSS_CPR_TARGET9_t_m_RMSK)
#define HWIO_MSS_CPR_TARGET9_t_m_INMI2(t,m,mask)    \
        in_dword_masked(HWIO_MSS_CPR_TARGET9_t_m_ADDR(t,m), mask)
#define HWIO_MSS_CPR_TARGET9_t_m_OUTI2(t,m,val)    \
        out_dword(HWIO_MSS_CPR_TARGET9_t_m_ADDR(t,m),val)
#define HWIO_MSS_CPR_TARGET9_t_m_OUTMI2(t,m,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_TARGET9_t_m_ADDR(t,m),mask,val,HWIO_MSS_CPR_TARGET9_t_m_INI2(t,m))
#define HWIO_MSS_CPR_TARGET9_t_m_TARGET9_BMSK                                     0xfff
#define HWIO_MSS_CPR_TARGET9_t_m_TARGET9_SHFT                                       0x0

#define HWIO_MSS_CPR_TARGET10_t_m_ADDR(t,m)                                  (MSS_CPR3_REG_BASE      + 0x00000868 + 0x440 * (t) + 0x40 * (m))
#define HWIO_MSS_CPR_TARGET10_t_m_RMSK                                            0xfff
#define HWIO_MSS_CPR_TARGET10_t_m_MAXt                                                0
#define HWIO_MSS_CPR_TARGET10_t_m_MAXm                                                3
#define HWIO_MSS_CPR_TARGET10_t_m_INI2(t,m)        \
        in_dword_masked(HWIO_MSS_CPR_TARGET10_t_m_ADDR(t,m), HWIO_MSS_CPR_TARGET10_t_m_RMSK)
#define HWIO_MSS_CPR_TARGET10_t_m_INMI2(t,m,mask)    \
        in_dword_masked(HWIO_MSS_CPR_TARGET10_t_m_ADDR(t,m), mask)
#define HWIO_MSS_CPR_TARGET10_t_m_OUTI2(t,m,val)    \
        out_dword(HWIO_MSS_CPR_TARGET10_t_m_ADDR(t,m),val)
#define HWIO_MSS_CPR_TARGET10_t_m_OUTMI2(t,m,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_TARGET10_t_m_ADDR(t,m),mask,val,HWIO_MSS_CPR_TARGET10_t_m_INI2(t,m))
#define HWIO_MSS_CPR_TARGET10_t_m_TARGET10_BMSK                                   0xfff
#define HWIO_MSS_CPR_TARGET10_t_m_TARGET10_SHFT                                     0x0

#define HWIO_MSS_CPR_TARGET11_t_m_ADDR(t,m)                                  (MSS_CPR3_REG_BASE      + 0x0000086c + 0x440 * (t) + 0x40 * (m))
#define HWIO_MSS_CPR_TARGET11_t_m_RMSK                                            0xfff
#define HWIO_MSS_CPR_TARGET11_t_m_MAXt                                                0
#define HWIO_MSS_CPR_TARGET11_t_m_MAXm                                                3
#define HWIO_MSS_CPR_TARGET11_t_m_INI2(t,m)        \
        in_dword_masked(HWIO_MSS_CPR_TARGET11_t_m_ADDR(t,m), HWIO_MSS_CPR_TARGET11_t_m_RMSK)
#define HWIO_MSS_CPR_TARGET11_t_m_INMI2(t,m,mask)    \
        in_dword_masked(HWIO_MSS_CPR_TARGET11_t_m_ADDR(t,m), mask)
#define HWIO_MSS_CPR_TARGET11_t_m_OUTI2(t,m,val)    \
        out_dword(HWIO_MSS_CPR_TARGET11_t_m_ADDR(t,m),val)
#define HWIO_MSS_CPR_TARGET11_t_m_OUTMI2(t,m,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_TARGET11_t_m_ADDR(t,m),mask,val,HWIO_MSS_CPR_TARGET11_t_m_INI2(t,m))
#define HWIO_MSS_CPR_TARGET11_t_m_TARGET11_BMSK                                   0xfff
#define HWIO_MSS_CPR_TARGET11_t_m_TARGET11_SHFT                                     0x0

#define HWIO_MSS_CPR_TARGET12_t_m_ADDR(t,m)                                  (MSS_CPR3_REG_BASE      + 0x00000870 + 0x440 * (t) + 0x40 * (m))
#define HWIO_MSS_CPR_TARGET12_t_m_RMSK                                            0xfff
#define HWIO_MSS_CPR_TARGET12_t_m_MAXt                                                0
#define HWIO_MSS_CPR_TARGET12_t_m_MAXm                                                3
#define HWIO_MSS_CPR_TARGET12_t_m_INI2(t,m)        \
        in_dword_masked(HWIO_MSS_CPR_TARGET12_t_m_ADDR(t,m), HWIO_MSS_CPR_TARGET12_t_m_RMSK)
#define HWIO_MSS_CPR_TARGET12_t_m_INMI2(t,m,mask)    \
        in_dword_masked(HWIO_MSS_CPR_TARGET12_t_m_ADDR(t,m), mask)
#define HWIO_MSS_CPR_TARGET12_t_m_OUTI2(t,m,val)    \
        out_dword(HWIO_MSS_CPR_TARGET12_t_m_ADDR(t,m),val)
#define HWIO_MSS_CPR_TARGET12_t_m_OUTMI2(t,m,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_TARGET12_t_m_ADDR(t,m),mask,val,HWIO_MSS_CPR_TARGET12_t_m_INI2(t,m))
#define HWIO_MSS_CPR_TARGET12_t_m_TARGET12_BMSK                                   0xfff
#define HWIO_MSS_CPR_TARGET12_t_m_TARGET12_SHFT                                     0x0

#define HWIO_MSS_CPR_TARGET13_t_m_ADDR(t,m)                                  (MSS_CPR3_REG_BASE      + 0x00000874 + 0x440 * (t) + 0x40 * (m))
#define HWIO_MSS_CPR_TARGET13_t_m_RMSK                                            0xfff
#define HWIO_MSS_CPR_TARGET13_t_m_MAXt                                                0
#define HWIO_MSS_CPR_TARGET13_t_m_MAXm                                                3
#define HWIO_MSS_CPR_TARGET13_t_m_INI2(t,m)        \
        in_dword_masked(HWIO_MSS_CPR_TARGET13_t_m_ADDR(t,m), HWIO_MSS_CPR_TARGET13_t_m_RMSK)
#define HWIO_MSS_CPR_TARGET13_t_m_INMI2(t,m,mask)    \
        in_dword_masked(HWIO_MSS_CPR_TARGET13_t_m_ADDR(t,m), mask)
#define HWIO_MSS_CPR_TARGET13_t_m_OUTI2(t,m,val)    \
        out_dword(HWIO_MSS_CPR_TARGET13_t_m_ADDR(t,m),val)
#define HWIO_MSS_CPR_TARGET13_t_m_OUTMI2(t,m,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_TARGET13_t_m_ADDR(t,m),mask,val,HWIO_MSS_CPR_TARGET13_t_m_INI2(t,m))
#define HWIO_MSS_CPR_TARGET13_t_m_TARGET13_BMSK                                   0xfff
#define HWIO_MSS_CPR_TARGET13_t_m_TARGET13_SHFT                                     0x0

#define HWIO_MSS_CPR_TARGET14_t_m_ADDR(t,m)                                  (MSS_CPR3_REG_BASE      + 0x00000878 + 0x440 * (t) + 0x40 * (m))
#define HWIO_MSS_CPR_TARGET14_t_m_RMSK                                            0xfff
#define HWIO_MSS_CPR_TARGET14_t_m_MAXt                                                0
#define HWIO_MSS_CPR_TARGET14_t_m_MAXm                                                3
#define HWIO_MSS_CPR_TARGET14_t_m_INI2(t,m)        \
        in_dword_masked(HWIO_MSS_CPR_TARGET14_t_m_ADDR(t,m), HWIO_MSS_CPR_TARGET14_t_m_RMSK)
#define HWIO_MSS_CPR_TARGET14_t_m_INMI2(t,m,mask)    \
        in_dword_masked(HWIO_MSS_CPR_TARGET14_t_m_ADDR(t,m), mask)
#define HWIO_MSS_CPR_TARGET14_t_m_OUTI2(t,m,val)    \
        out_dword(HWIO_MSS_CPR_TARGET14_t_m_ADDR(t,m),val)
#define HWIO_MSS_CPR_TARGET14_t_m_OUTMI2(t,m,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_TARGET14_t_m_ADDR(t,m),mask,val,HWIO_MSS_CPR_TARGET14_t_m_INI2(t,m))
#define HWIO_MSS_CPR_TARGET14_t_m_TARGET14_BMSK                                   0xfff
#define HWIO_MSS_CPR_TARGET14_t_m_TARGET14_SHFT                                     0x0

#define HWIO_MSS_CPR_TARGET15_t_m_ADDR(t,m)                                  (MSS_CPR3_REG_BASE      + 0x0000087c + 0x440 * (t) + 0x40 * (m))
#define HWIO_MSS_CPR_TARGET15_t_m_RMSK                                            0xfff
#define HWIO_MSS_CPR_TARGET15_t_m_MAXt                                                0
#define HWIO_MSS_CPR_TARGET15_t_m_MAXm                                                3
#define HWIO_MSS_CPR_TARGET15_t_m_INI2(t,m)        \
        in_dword_masked(HWIO_MSS_CPR_TARGET15_t_m_ADDR(t,m), HWIO_MSS_CPR_TARGET15_t_m_RMSK)
#define HWIO_MSS_CPR_TARGET15_t_m_INMI2(t,m,mask)    \
        in_dword_masked(HWIO_MSS_CPR_TARGET15_t_m_ADDR(t,m), mask)
#define HWIO_MSS_CPR_TARGET15_t_m_OUTI2(t,m,val)    \
        out_dword(HWIO_MSS_CPR_TARGET15_t_m_ADDR(t,m),val)
#define HWIO_MSS_CPR_TARGET15_t_m_OUTMI2(t,m,mask,val) \
        out_dword_masked_ns(HWIO_MSS_CPR_TARGET15_t_m_ADDR(t,m),mask,val,HWIO_MSS_CPR_TARGET15_t_m_INI2(t,m))
#define HWIO_MSS_CPR_TARGET15_t_m_TARGET15_BMSK                                   0xfff
#define HWIO_MSS_CPR_TARGET15_t_m_TARGET15_SHFT                                     0x0


#endif /* __MSS_RBCPR_ACCESS_HWIO_H__ */
