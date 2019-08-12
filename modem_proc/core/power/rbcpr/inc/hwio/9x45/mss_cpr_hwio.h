#ifndef __MSS_CPR_HWIO_H__
#define __MSS_CPR_HWIO_H__
/*
===========================================================================
*/
/**
  @file mss_cpr_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    MPSS_PERPH
    MSS_RBCPR_WRAPPER
    SECURITY_CONTROL_CORE

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

  $Header: //components/rel/core.mpss/3.5.c12.3/power/rbcpr/inc/hwio/9x45/mss_cpr_hwio.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

/* 9x45 HWIO */
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
#define HWIO_MSS_DEBUG_CTL_RMSK                                                      0x7
#define HWIO_MSS_DEBUG_CTL_IN          \
        in_dword_masked(HWIO_MSS_DEBUG_CTL_ADDR, HWIO_MSS_DEBUG_CTL_RMSK)
#define HWIO_MSS_DEBUG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DEBUG_CTL_ADDR, m)
#define HWIO_MSS_DEBUG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DEBUG_CTL_ADDR,v)
#define HWIO_MSS_DEBUG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DEBUG_CTL_ADDR,m,v,HWIO_MSS_DEBUG_CTL_IN)
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
 * MODULE: MSS_RBCPR_WRAPPER
 *--------------------------------------------------------------------------*/

#define MSS_RBCPR_WRAPPER_REG_BASE                                          (MODEM_TOP_BASE      + 0x001b1000)

#define HWIO_MSS_RBCPR_VERSION_ADDR                                         (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000000)
#define HWIO_MSS_RBCPR_VERSION_RMSK                                         0xffffffff
#define HWIO_MSS_RBCPR_VERSION_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_VERSION_ADDR, HWIO_MSS_RBCPR_VERSION_RMSK)
#define HWIO_MSS_RBCPR_VERSION_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_VERSION_ADDR, m)
#define HWIO_MSS_RBCPR_VERSION_RBCPR_VERSION_BMSK                           0xffffffff
#define HWIO_MSS_RBCPR_VERSION_RBCPR_VERSION_SHFT                                  0x0

#define HWIO_MSS_RBCPR_SENSOR_MASK0_ADDR                                    (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000020)
#define HWIO_MSS_RBCPR_SENSOR_MASK0_RMSK                                    0xffffffff
#define HWIO_MSS_RBCPR_SENSOR_MASK0_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_SENSOR_MASK0_ADDR, HWIO_MSS_RBCPR_SENSOR_MASK0_RMSK)
#define HWIO_MSS_RBCPR_SENSOR_MASK0_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_SENSOR_MASK0_ADDR, m)
#define HWIO_MSS_RBCPR_SENSOR_MASK0_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_SENSOR_MASK0_ADDR,v)
#define HWIO_MSS_RBCPR_SENSOR_MASK0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_SENSOR_MASK0_ADDR,m,v,HWIO_MSS_RBCPR_SENSOR_MASK0_IN)
#define HWIO_MSS_RBCPR_SENSOR_MASK0_SENSOR_MASK_BMSK                        0xffffffff
#define HWIO_MSS_RBCPR_SENSOR_MASK0_SENSOR_MASK_SHFT                               0x0

#define HWIO_MSS_RBCPR_SENSOR_MASK1_ADDR                                    (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000024)
#define HWIO_MSS_RBCPR_SENSOR_MASK1_RMSK                                    0xffffffff
#define HWIO_MSS_RBCPR_SENSOR_MASK1_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_SENSOR_MASK1_ADDR, HWIO_MSS_RBCPR_SENSOR_MASK1_RMSK)
#define HWIO_MSS_RBCPR_SENSOR_MASK1_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_SENSOR_MASK1_ADDR, m)
#define HWIO_MSS_RBCPR_SENSOR_MASK1_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_SENSOR_MASK1_ADDR,v)
#define HWIO_MSS_RBCPR_SENSOR_MASK1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_SENSOR_MASK1_ADDR,m,v,HWIO_MSS_RBCPR_SENSOR_MASK1_IN)
#define HWIO_MSS_RBCPR_SENSOR_MASK1_SENSOR_MASK_BMSK                        0xffffffff
#define HWIO_MSS_RBCPR_SENSOR_MASK1_SENSOR_MASK_SHFT                               0x0

#define HWIO_MSS_RBCPR_SENSOR_MASK2_ADDR                                    (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000028)
#define HWIO_MSS_RBCPR_SENSOR_MASK2_RMSK                                    0xffffffff
#define HWIO_MSS_RBCPR_SENSOR_MASK2_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_SENSOR_MASK2_ADDR, HWIO_MSS_RBCPR_SENSOR_MASK2_RMSK)
#define HWIO_MSS_RBCPR_SENSOR_MASK2_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_SENSOR_MASK2_ADDR, m)
#define HWIO_MSS_RBCPR_SENSOR_MASK2_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_SENSOR_MASK2_ADDR,v)
#define HWIO_MSS_RBCPR_SENSOR_MASK2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_SENSOR_MASK2_ADDR,m,v,HWIO_MSS_RBCPR_SENSOR_MASK2_IN)
#define HWIO_MSS_RBCPR_SENSOR_MASK2_SENSOR_MASK_BMSK                        0xffffffff
#define HWIO_MSS_RBCPR_SENSOR_MASK2_SENSOR_MASK_SHFT                               0x0

#define HWIO_MSS_RBCPR_SENSOR_MASK3_ADDR                                    (MSS_RBCPR_WRAPPER_REG_BASE      + 0x0000002c)
#define HWIO_MSS_RBCPR_SENSOR_MASK3_RMSK                                    0xffffffff
#define HWIO_MSS_RBCPR_SENSOR_MASK3_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_SENSOR_MASK3_ADDR, HWIO_MSS_RBCPR_SENSOR_MASK3_RMSK)
#define HWIO_MSS_RBCPR_SENSOR_MASK3_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_SENSOR_MASK3_ADDR, m)
#define HWIO_MSS_RBCPR_SENSOR_MASK3_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_SENSOR_MASK3_ADDR,v)
#define HWIO_MSS_RBCPR_SENSOR_MASK3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_SENSOR_MASK3_ADDR,m,v,HWIO_MSS_RBCPR_SENSOR_MASK3_IN)
#define HWIO_MSS_RBCPR_SENSOR_MASK3_SENSOR_MASK_BMSK                        0xffffffff
#define HWIO_MSS_RBCPR_SENSOR_MASK3_SENSOR_MASK_SHFT                               0x0

#define HWIO_MSS_RBCPR_SENSOR_BYPASS0_ADDR                                  (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000030)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS0_RMSK                                  0xffffffff
#define HWIO_MSS_RBCPR_SENSOR_BYPASS0_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_SENSOR_BYPASS0_ADDR, HWIO_MSS_RBCPR_SENSOR_BYPASS0_RMSK)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS0_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_SENSOR_BYPASS0_ADDR, m)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS0_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_SENSOR_BYPASS0_ADDR,v)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_SENSOR_BYPASS0_ADDR,m,v,HWIO_MSS_RBCPR_SENSOR_BYPASS0_IN)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS0_SENSOR_BYPASS_BMSK                    0xffffffff
#define HWIO_MSS_RBCPR_SENSOR_BYPASS0_SENSOR_BYPASS_SHFT                           0x0

#define HWIO_MSS_RBCPR_SENSOR_BYPASS1_ADDR                                  (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000034)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS1_RMSK                                  0xffffffff
#define HWIO_MSS_RBCPR_SENSOR_BYPASS1_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_SENSOR_BYPASS1_ADDR, HWIO_MSS_RBCPR_SENSOR_BYPASS1_RMSK)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS1_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_SENSOR_BYPASS1_ADDR, m)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS1_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_SENSOR_BYPASS1_ADDR,v)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_SENSOR_BYPASS1_ADDR,m,v,HWIO_MSS_RBCPR_SENSOR_BYPASS1_IN)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS1_SENSOR_BYPASS_BMSK                    0xffffffff
#define HWIO_MSS_RBCPR_SENSOR_BYPASS1_SENSOR_BYPASS_SHFT                           0x0

#define HWIO_MSS_RBCPR_SENSOR_BYPASS2_ADDR                                  (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000038)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS2_RMSK                                  0xffffffff
#define HWIO_MSS_RBCPR_SENSOR_BYPASS2_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_SENSOR_BYPASS2_ADDR, HWIO_MSS_RBCPR_SENSOR_BYPASS2_RMSK)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS2_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_SENSOR_BYPASS2_ADDR, m)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS2_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_SENSOR_BYPASS2_ADDR,v)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_SENSOR_BYPASS2_ADDR,m,v,HWIO_MSS_RBCPR_SENSOR_BYPASS2_IN)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS2_SENSOR_BYPASS_BMSK                    0xffffffff
#define HWIO_MSS_RBCPR_SENSOR_BYPASS2_SENSOR_BYPASS_SHFT                           0x0

#define HWIO_MSS_RBCPR_SENSOR_BYPASS3_ADDR                                  (MSS_RBCPR_WRAPPER_REG_BASE      + 0x0000003c)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS3_RMSK                                  0xffffffff
#define HWIO_MSS_RBCPR_SENSOR_BYPASS3_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_SENSOR_BYPASS3_ADDR, HWIO_MSS_RBCPR_SENSOR_BYPASS3_RMSK)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS3_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_SENSOR_BYPASS3_ADDR, m)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS3_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_SENSOR_BYPASS3_ADDR,v)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_SENSOR_BYPASS3_ADDR,m,v,HWIO_MSS_RBCPR_SENSOR_BYPASS3_IN)
#define HWIO_MSS_RBCPR_SENSOR_BYPASS3_SENSOR_BYPASS_BMSK                    0xffffffff
#define HWIO_MSS_RBCPR_SENSOR_BYPASS3_SENSOR_BYPASS_SHFT                           0x0

#define HWIO_MSS_RBIF_PMIC_ADDR_ADDR                                        (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000040)
#define HWIO_MSS_RBIF_PMIC_ADDR_RMSK                                          0xc03c03
#define HWIO_MSS_RBIF_PMIC_ADDR_IN          \
        in_dword_masked(HWIO_MSS_RBIF_PMIC_ADDR_ADDR, HWIO_MSS_RBIF_PMIC_ADDR_RMSK)
#define HWIO_MSS_RBIF_PMIC_ADDR_INM(m)      \
        in_dword_masked(HWIO_MSS_RBIF_PMIC_ADDR_ADDR, m)
#define HWIO_MSS_RBIF_PMIC_ADDR_OUT(v)      \
        out_dword(HWIO_MSS_RBIF_PMIC_ADDR_ADDR,v)
#define HWIO_MSS_RBIF_PMIC_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBIF_PMIC_ADDR_ADDR,m,v,HWIO_MSS_RBIF_PMIC_ADDR_IN)
#define HWIO_MSS_RBIF_PMIC_ADDR_RBIF_PMIC_MX_DATA_MSB_BMSK                    0xc00000
#define HWIO_MSS_RBIF_PMIC_ADDR_RBIF_PMIC_MX_DATA_MSB_SHFT                        0x16
#define HWIO_MSS_RBIF_PMIC_ADDR_RBIF_PMIC_MX_ADDR_BMSK                          0x3000
#define HWIO_MSS_RBIF_PMIC_ADDR_RBIF_PMIC_MX_ADDR_SHFT                             0xc
#define HWIO_MSS_RBIF_PMIC_ADDR_RBIF_PMIC_CX_DATA_MSB_BMSK                       0xc00
#define HWIO_MSS_RBIF_PMIC_ADDR_RBIF_PMIC_CX_DATA_MSB_SHFT                         0xa
#define HWIO_MSS_RBIF_PMIC_ADDR_RBIF_PMIC_CX_ADDR_BMSK                             0x3
#define HWIO_MSS_RBIF_PMIC_ADDR_RBIF_PMIC_CX_ADDR_SHFT                             0x0

#define HWIO_MSS_RBCPR_TIMER_INTERVAL_ADDR                                  (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000044)
#define HWIO_MSS_RBCPR_TIMER_INTERVAL_RMSK                                  0xffffffff
#define HWIO_MSS_RBCPR_TIMER_INTERVAL_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_TIMER_INTERVAL_ADDR, HWIO_MSS_RBCPR_TIMER_INTERVAL_RMSK)
#define HWIO_MSS_RBCPR_TIMER_INTERVAL_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_TIMER_INTERVAL_ADDR, m)
#define HWIO_MSS_RBCPR_TIMER_INTERVAL_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_TIMER_INTERVAL_ADDR,v)
#define HWIO_MSS_RBCPR_TIMER_INTERVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_TIMER_INTERVAL_ADDR,m,v,HWIO_MSS_RBCPR_TIMER_INTERVAL_IN)
#define HWIO_MSS_RBCPR_TIMER_INTERVAL_INTERVAL_BMSK                         0xffffffff
#define HWIO_MSS_RBCPR_TIMER_INTERVAL_INTERVAL_SHFT                                0x0

#define HWIO_MSS_RBIF_LIMIT_ADDR                                            (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000048)
#define HWIO_MSS_RBIF_LIMIT_RMSK                                                 0xfff
#define HWIO_MSS_RBIF_LIMIT_IN          \
        in_dword_masked(HWIO_MSS_RBIF_LIMIT_ADDR, HWIO_MSS_RBIF_LIMIT_RMSK)
#define HWIO_MSS_RBIF_LIMIT_INM(m)      \
        in_dword_masked(HWIO_MSS_RBIF_LIMIT_ADDR, m)
#define HWIO_MSS_RBIF_LIMIT_OUT(v)      \
        out_dword(HWIO_MSS_RBIF_LIMIT_ADDR,v)
#define HWIO_MSS_RBIF_LIMIT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBIF_LIMIT_ADDR,m,v,HWIO_MSS_RBIF_LIMIT_IN)
#define HWIO_MSS_RBIF_LIMIT_CEILING_BMSK                                         0xfc0
#define HWIO_MSS_RBIF_LIMIT_CEILING_SHFT                                           0x6
#define HWIO_MSS_RBIF_LIMIT_FLOOR_BMSK                                            0x3f
#define HWIO_MSS_RBIF_LIMIT_FLOOR_SHFT                                             0x0

#define HWIO_MSS_RBIF_TIMER_ADJUST_ADDR                                     (MSS_RBCPR_WRAPPER_REG_BASE      + 0x0000004c)
#define HWIO_MSS_RBIF_TIMER_ADJUST_RMSK                                        0x1ffff
#define HWIO_MSS_RBIF_TIMER_ADJUST_IN          \
        in_dword_masked(HWIO_MSS_RBIF_TIMER_ADJUST_ADDR, HWIO_MSS_RBIF_TIMER_ADJUST_RMSK)
#define HWIO_MSS_RBIF_TIMER_ADJUST_INM(m)      \
        in_dword_masked(HWIO_MSS_RBIF_TIMER_ADJUST_ADDR, m)
#define HWIO_MSS_RBIF_TIMER_ADJUST_OUT(v)      \
        out_dword(HWIO_MSS_RBIF_TIMER_ADJUST_ADDR,v)
#define HWIO_MSS_RBIF_TIMER_ADJUST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBIF_TIMER_ADJUST_ADDR,m,v,HWIO_MSS_RBIF_TIMER_ADJUST_IN)
#define HWIO_MSS_RBIF_TIMER_ADJUST_AUTO_LOOP_DISABLE_BMSK                      0x10000
#define HWIO_MSS_RBIF_TIMER_ADJUST_AUTO_LOOP_DISABLE_SHFT                         0x10
#define HWIO_MSS_RBIF_TIMER_ADJUST_CLAMP_TIMER_INTERVAL_BMSK                    0xff00
#define HWIO_MSS_RBIF_TIMER_ADJUST_CLAMP_TIMER_INTERVAL_SHFT                       0x8
#define HWIO_MSS_RBIF_TIMER_ADJUST_CONSECUTIVE_DN_BMSK                            0xf0
#define HWIO_MSS_RBIF_TIMER_ADJUST_CONSECUTIVE_DN_SHFT                             0x4
#define HWIO_MSS_RBIF_TIMER_ADJUST_CONSECUTIVE_UP_BMSK                             0xf
#define HWIO_MSS_RBIF_TIMER_ADJUST_CONSECUTIVE_UP_SHFT                             0x0

#define HWIO_MSS_RBCPR_GCNT_TARGET0_ADDR                                    (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000060)
#define HWIO_MSS_RBCPR_GCNT_TARGET0_RMSK                                      0x3fffff
#define HWIO_MSS_RBCPR_GCNT_TARGET0_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_GCNT_TARGET0_ADDR, HWIO_MSS_RBCPR_GCNT_TARGET0_RMSK)
#define HWIO_MSS_RBCPR_GCNT_TARGET0_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_GCNT_TARGET0_ADDR, m)
#define HWIO_MSS_RBCPR_GCNT_TARGET0_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_GCNT_TARGET0_ADDR,v)
#define HWIO_MSS_RBCPR_GCNT_TARGET0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_GCNT_TARGET0_ADDR,m,v,HWIO_MSS_RBCPR_GCNT_TARGET0_IN)
#define HWIO_MSS_RBCPR_GCNT_TARGET0_GCNT0_BMSK                                0x3ff000
#define HWIO_MSS_RBCPR_GCNT_TARGET0_GCNT0_SHFT                                     0xc
#define HWIO_MSS_RBCPR_GCNT_TARGET0_TARGET0_BMSK                                 0xfff
#define HWIO_MSS_RBCPR_GCNT_TARGET0_TARGET0_SHFT                                   0x0

#define HWIO_MSS_RBCPR_GCNT_TARGET1_ADDR                                    (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000064)
#define HWIO_MSS_RBCPR_GCNT_TARGET1_RMSK                                      0x3fffff
#define HWIO_MSS_RBCPR_GCNT_TARGET1_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_GCNT_TARGET1_ADDR, HWIO_MSS_RBCPR_GCNT_TARGET1_RMSK)
#define HWIO_MSS_RBCPR_GCNT_TARGET1_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_GCNT_TARGET1_ADDR, m)
#define HWIO_MSS_RBCPR_GCNT_TARGET1_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_GCNT_TARGET1_ADDR,v)
#define HWIO_MSS_RBCPR_GCNT_TARGET1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_GCNT_TARGET1_ADDR,m,v,HWIO_MSS_RBCPR_GCNT_TARGET1_IN)
#define HWIO_MSS_RBCPR_GCNT_TARGET1_GCNT1_BMSK                                0x3ff000
#define HWIO_MSS_RBCPR_GCNT_TARGET1_GCNT1_SHFT                                     0xc
#define HWIO_MSS_RBCPR_GCNT_TARGET1_TARGET1_BMSK                                 0xfff
#define HWIO_MSS_RBCPR_GCNT_TARGET1_TARGET1_SHFT                                   0x0

#define HWIO_MSS_RBCPR_GCNT_TARGET2_ADDR                                    (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000068)
#define HWIO_MSS_RBCPR_GCNT_TARGET2_RMSK                                      0x3fffff
#define HWIO_MSS_RBCPR_GCNT_TARGET2_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_GCNT_TARGET2_ADDR, HWIO_MSS_RBCPR_GCNT_TARGET2_RMSK)
#define HWIO_MSS_RBCPR_GCNT_TARGET2_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_GCNT_TARGET2_ADDR, m)
#define HWIO_MSS_RBCPR_GCNT_TARGET2_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_GCNT_TARGET2_ADDR,v)
#define HWIO_MSS_RBCPR_GCNT_TARGET2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_GCNT_TARGET2_ADDR,m,v,HWIO_MSS_RBCPR_GCNT_TARGET2_IN)
#define HWIO_MSS_RBCPR_GCNT_TARGET2_GCNT2_BMSK                                0x3ff000
#define HWIO_MSS_RBCPR_GCNT_TARGET2_GCNT2_SHFT                                     0xc
#define HWIO_MSS_RBCPR_GCNT_TARGET2_TARGET2_BMSK                                 0xfff
#define HWIO_MSS_RBCPR_GCNT_TARGET2_TARGET2_SHFT                                   0x0

#define HWIO_MSS_RBCPR_GCNT_TARGET3_ADDR                                    (MSS_RBCPR_WRAPPER_REG_BASE      + 0x0000006c)
#define HWIO_MSS_RBCPR_GCNT_TARGET3_RMSK                                      0x3fffff
#define HWIO_MSS_RBCPR_GCNT_TARGET3_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_GCNT_TARGET3_ADDR, HWIO_MSS_RBCPR_GCNT_TARGET3_RMSK)
#define HWIO_MSS_RBCPR_GCNT_TARGET3_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_GCNT_TARGET3_ADDR, m)
#define HWIO_MSS_RBCPR_GCNT_TARGET3_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_GCNT_TARGET3_ADDR,v)
#define HWIO_MSS_RBCPR_GCNT_TARGET3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_GCNT_TARGET3_ADDR,m,v,HWIO_MSS_RBCPR_GCNT_TARGET3_IN)
#define HWIO_MSS_RBCPR_GCNT_TARGET3_GCNT3_BMSK                                0x3ff000
#define HWIO_MSS_RBCPR_GCNT_TARGET3_GCNT3_SHFT                                     0xc
#define HWIO_MSS_RBCPR_GCNT_TARGET3_TARGET3_BMSK                                 0xfff
#define HWIO_MSS_RBCPR_GCNT_TARGET3_TARGET3_SHFT                                   0x0

#define HWIO_MSS_RBCPR_GCNT_TARGET4_ADDR                                    (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000070)
#define HWIO_MSS_RBCPR_GCNT_TARGET4_RMSK                                      0x3fffff
#define HWIO_MSS_RBCPR_GCNT_TARGET4_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_GCNT_TARGET4_ADDR, HWIO_MSS_RBCPR_GCNT_TARGET4_RMSK)
#define HWIO_MSS_RBCPR_GCNT_TARGET4_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_GCNT_TARGET4_ADDR, m)
#define HWIO_MSS_RBCPR_GCNT_TARGET4_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_GCNT_TARGET4_ADDR,v)
#define HWIO_MSS_RBCPR_GCNT_TARGET4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_GCNT_TARGET4_ADDR,m,v,HWIO_MSS_RBCPR_GCNT_TARGET4_IN)
#define HWIO_MSS_RBCPR_GCNT_TARGET4_GCNT4_BMSK                                0x3ff000
#define HWIO_MSS_RBCPR_GCNT_TARGET4_GCNT4_SHFT                                     0xc
#define HWIO_MSS_RBCPR_GCNT_TARGET4_TARGET4_BMSK                                 0xfff
#define HWIO_MSS_RBCPR_GCNT_TARGET4_TARGET4_SHFT                                   0x0

#define HWIO_MSS_RBCPR_GCNT_TARGET5_ADDR                                    (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000074)
#define HWIO_MSS_RBCPR_GCNT_TARGET5_RMSK                                      0x3fffff
#define HWIO_MSS_RBCPR_GCNT_TARGET5_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_GCNT_TARGET5_ADDR, HWIO_MSS_RBCPR_GCNT_TARGET5_RMSK)
#define HWIO_MSS_RBCPR_GCNT_TARGET5_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_GCNT_TARGET5_ADDR, m)
#define HWIO_MSS_RBCPR_GCNT_TARGET5_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_GCNT_TARGET5_ADDR,v)
#define HWIO_MSS_RBCPR_GCNT_TARGET5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_GCNT_TARGET5_ADDR,m,v,HWIO_MSS_RBCPR_GCNT_TARGET5_IN)
#define HWIO_MSS_RBCPR_GCNT_TARGET5_GCNT5_BMSK                                0x3ff000
#define HWIO_MSS_RBCPR_GCNT_TARGET5_GCNT5_SHFT                                     0xc
#define HWIO_MSS_RBCPR_GCNT_TARGET5_TARGET5_BMSK                                 0xfff
#define HWIO_MSS_RBCPR_GCNT_TARGET5_TARGET5_SHFT                                   0x0

#define HWIO_MSS_RBCPR_GCNT_TARGET6_ADDR                                    (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000078)
#define HWIO_MSS_RBCPR_GCNT_TARGET6_RMSK                                      0x3fffff
#define HWIO_MSS_RBCPR_GCNT_TARGET6_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_GCNT_TARGET6_ADDR, HWIO_MSS_RBCPR_GCNT_TARGET6_RMSK)
#define HWIO_MSS_RBCPR_GCNT_TARGET6_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_GCNT_TARGET6_ADDR, m)
#define HWIO_MSS_RBCPR_GCNT_TARGET6_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_GCNT_TARGET6_ADDR,v)
#define HWIO_MSS_RBCPR_GCNT_TARGET6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_GCNT_TARGET6_ADDR,m,v,HWIO_MSS_RBCPR_GCNT_TARGET6_IN)
#define HWIO_MSS_RBCPR_GCNT_TARGET6_GCNT6_BMSK                                0x3ff000
#define HWIO_MSS_RBCPR_GCNT_TARGET6_GCNT6_SHFT                                     0xc
#define HWIO_MSS_RBCPR_GCNT_TARGET6_TARGET6_BMSK                                 0xfff
#define HWIO_MSS_RBCPR_GCNT_TARGET6_TARGET6_SHFT                                   0x0

#define HWIO_MSS_RBCPR_GCNT_TARGET7_ADDR                                    (MSS_RBCPR_WRAPPER_REG_BASE      + 0x0000007c)
#define HWIO_MSS_RBCPR_GCNT_TARGET7_RMSK                                      0x3fffff
#define HWIO_MSS_RBCPR_GCNT_TARGET7_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_GCNT_TARGET7_ADDR, HWIO_MSS_RBCPR_GCNT_TARGET7_RMSK)
#define HWIO_MSS_RBCPR_GCNT_TARGET7_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_GCNT_TARGET7_ADDR, m)
#define HWIO_MSS_RBCPR_GCNT_TARGET7_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_GCNT_TARGET7_ADDR,v)
#define HWIO_MSS_RBCPR_GCNT_TARGET7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_GCNT_TARGET7_ADDR,m,v,HWIO_MSS_RBCPR_GCNT_TARGET7_IN)
#define HWIO_MSS_RBCPR_GCNT_TARGET7_GCNT7_BMSK                                0x3ff000
#define HWIO_MSS_RBCPR_GCNT_TARGET7_GCNT7_SHFT                                     0xc
#define HWIO_MSS_RBCPR_GCNT_TARGET7_TARGET7_BMSK                                 0xfff
#define HWIO_MSS_RBCPR_GCNT_TARGET7_TARGET7_SHFT                                   0x0

#define HWIO_MSS_RBCPR_STEP_QUOT_ADDR                                       (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000080)
#define HWIO_MSS_RBCPR_STEP_QUOT_RMSK                                           0x1fff
#define HWIO_MSS_RBCPR_STEP_QUOT_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_STEP_QUOT_ADDR, HWIO_MSS_RBCPR_STEP_QUOT_RMSK)
#define HWIO_MSS_RBCPR_STEP_QUOT_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_STEP_QUOT_ADDR, m)
#define HWIO_MSS_RBCPR_STEP_QUOT_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_STEP_QUOT_ADDR,v)
#define HWIO_MSS_RBCPR_STEP_QUOT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_STEP_QUOT_ADDR,m,v,HWIO_MSS_RBCPR_STEP_QUOT_IN)
#define HWIO_MSS_RBCPR_STEP_QUOT_IDLE_CLOCKS_BMSK                               0x1f00
#define HWIO_MSS_RBCPR_STEP_QUOT_IDLE_CLOCKS_SHFT                                  0x8
#define HWIO_MSS_RBCPR_STEP_QUOT_STEP_QUOT_BMSK                                   0xff
#define HWIO_MSS_RBCPR_STEP_QUOT_STEP_QUOT_SHFT                                    0x0

#define HWIO_MSS_RBCPR_CTL_ADDR                                             (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000090)
#define HWIO_MSS_RBCPR_CTL_RMSK                                             0xff7fffff
#define HWIO_MSS_RBCPR_CTL_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_CTL_ADDR, HWIO_MSS_RBCPR_CTL_RMSK)
#define HWIO_MSS_RBCPR_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_CTL_ADDR, m)
#define HWIO_MSS_RBCPR_CTL_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_CTL_ADDR,v)
#define HWIO_MSS_RBCPR_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_CTL_ADDR,m,v,HWIO_MSS_RBCPR_CTL_IN)
#define HWIO_MSS_RBCPR_CTL_DN_THRESHOLD_BMSK                                0xf0000000
#define HWIO_MSS_RBCPR_CTL_DN_THRESHOLD_SHFT                                      0x1c
#define HWIO_MSS_RBCPR_CTL_UP_THRESHOLD_BMSK                                 0xf000000
#define HWIO_MSS_RBCPR_CTL_UP_THRESHOLD_SHFT                                      0x18
#define HWIO_MSS_RBCPR_CTL_MIN_MX_CX_DELTA_BMSK                               0x7c0000
#define HWIO_MSS_RBCPR_CTL_MIN_MX_CX_DELTA_SHFT                                   0x12
#define HWIO_MSS_RBCPR_CTL_CPR_DISABLE_VALID_BMSK                              0x20000
#define HWIO_MSS_RBCPR_CTL_CPR_DISABLE_VALID_SHFT                                 0x11
#define HWIO_MSS_RBCPR_CTL_MAX_MX_CX_DELTA_BMSK                                0x1f000
#define HWIO_MSS_RBCPR_CTL_MAX_MX_CX_DELTA_SHFT                                    0xc
#define HWIO_MSS_RBCPR_CTL_HW_TO_PMIC_MX_EN_BMSK                                 0x800
#define HWIO_MSS_RBCPR_CTL_HW_TO_PMIC_MX_EN_SHFT                                   0xb
#define HWIO_MSS_RBCPR_CTL_COUNT_MODE_BMSK                                       0x400
#define HWIO_MSS_RBCPR_CTL_COUNT_MODE_SHFT                                         0xa
#define HWIO_MSS_RBCPR_CTL_DEBUG_BUS_SEL_BMSK                                    0x380
#define HWIO_MSS_RBCPR_CTL_DEBUG_BUS_SEL_SHFT                                      0x7
#define HWIO_MSS_RBCPR_CTL_SW_AUTO_CONT_NACK_DN_EN_BMSK                           0x40
#define HWIO_MSS_RBCPR_CTL_SW_AUTO_CONT_NACK_DN_EN_SHFT                            0x6
#define HWIO_MSS_RBCPR_CTL_SW_AUTO_CONT_ACK_EN_BMSK                               0x20
#define HWIO_MSS_RBCPR_CTL_SW_AUTO_CONT_ACK_EN_SHFT                                0x5
#define HWIO_MSS_RBCPR_CTL_HW_TO_PMIC_EN_BMSK                                     0x10
#define HWIO_MSS_RBCPR_CTL_HW_TO_PMIC_EN_SHFT                                      0x4
#define HWIO_MSS_RBCPR_CTL_TIMER_EN_BMSK                                           0x8
#define HWIO_MSS_RBCPR_CTL_TIMER_EN_SHFT                                           0x3
#define HWIO_MSS_RBCPR_CTL_INCR_MODE_BMSK                                          0x4
#define HWIO_MSS_RBCPR_CTL_INCR_MODE_SHFT                                          0x2
#define HWIO_MSS_RBCPR_CTL_FORCE_TIMER_BMSK                                        0x2
#define HWIO_MSS_RBCPR_CTL_FORCE_TIMER_SHFT                                        0x1
#define HWIO_MSS_RBCPR_CTL_LOOP_EN_BMSK                                            0x1
#define HWIO_MSS_RBCPR_CTL_LOOP_EN_SHFT                                            0x0

#define HWIO_MSS_RBIF_SW_VLEVEL_ADDR                                        (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000094)
#define HWIO_MSS_RBIF_SW_VLEVEL_RMSK                                             0xfff
#define HWIO_MSS_RBIF_SW_VLEVEL_IN          \
        in_dword_masked(HWIO_MSS_RBIF_SW_VLEVEL_ADDR, HWIO_MSS_RBIF_SW_VLEVEL_RMSK)
#define HWIO_MSS_RBIF_SW_VLEVEL_INM(m)      \
        in_dword_masked(HWIO_MSS_RBIF_SW_VLEVEL_ADDR, m)
#define HWIO_MSS_RBIF_SW_VLEVEL_OUT(v)      \
        out_dword(HWIO_MSS_RBIF_SW_VLEVEL_ADDR,v)
#define HWIO_MSS_RBIF_SW_VLEVEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBIF_SW_VLEVEL_ADDR,m,v,HWIO_MSS_RBIF_SW_VLEVEL_IN)
#define HWIO_MSS_RBIF_SW_VLEVEL_SW_MX_VLEVEL_BMSK                                0xfc0
#define HWIO_MSS_RBIF_SW_VLEVEL_SW_MX_VLEVEL_SHFT                                  0x6
#define HWIO_MSS_RBIF_SW_VLEVEL_SW_VLEVEL_BMSK                                    0x3f
#define HWIO_MSS_RBIF_SW_VLEVEL_SW_VLEVEL_SHFT                                     0x0

#define HWIO_MSS_RBIF_CONT_ACK_CMD_ADDR                                     (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000098)
#define HWIO_MSS_RBIF_CONT_ACK_CMD_RMSK                                     0xffffffff
#define HWIO_MSS_RBIF_CONT_ACK_CMD_OUT(v)      \
        out_dword(HWIO_MSS_RBIF_CONT_ACK_CMD_ADDR,v)
#define HWIO_MSS_RBIF_CONT_ACK_CMD_RBIF_CONT_ACK_CMD_BMSK                   0xffffffff
#define HWIO_MSS_RBIF_CONT_ACK_CMD_RBIF_CONT_ACK_CMD_SHFT                          0x0

#define HWIO_MSS_RBIF_CONT_NACK_CMD_ADDR                                    (MSS_RBCPR_WRAPPER_REG_BASE      + 0x0000009c)
#define HWIO_MSS_RBIF_CONT_NACK_CMD_RMSK                                    0xffffffff
#define HWIO_MSS_RBIF_CONT_NACK_CMD_OUT(v)      \
        out_dword(HWIO_MSS_RBIF_CONT_NACK_CMD_ADDR,v)
#define HWIO_MSS_RBIF_CONT_NACK_CMD_RBIF_CONT_NACK_CMD_BMSK                 0xffffffff
#define HWIO_MSS_RBIF_CONT_NACK_CMD_RBIF_CONT_NACK_CMD_SHFT                        0x0

#define HWIO_MSS_RBCPR_RESULT_0_ADDR                                        (MSS_RBCPR_WRAPPER_REG_BASE      + 0x000000a0)
#define HWIO_MSS_RBCPR_RESULT_0_RMSK                                           0xfffff
#define HWIO_MSS_RBCPR_RESULT_0_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_RESULT_0_ADDR, HWIO_MSS_RBCPR_RESULT_0_RMSK)
#define HWIO_MSS_RBCPR_RESULT_0_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_RESULT_0_ADDR, m)
#define HWIO_MSS_RBCPR_RESULT_0_BUSY_BMSK                                      0x80000
#define HWIO_MSS_RBCPR_RESULT_0_BUSY_SHFT                                         0x13
#define HWIO_MSS_RBCPR_RESULT_0_ERROR_LT_0_BMSK                                0x40000
#define HWIO_MSS_RBCPR_RESULT_0_ERROR_LT_0_SHFT                                   0x12
#define HWIO_MSS_RBCPR_RESULT_0_ERROR_BMSK                                     0x3ffc0
#define HWIO_MSS_RBCPR_RESULT_0_ERROR_SHFT                                         0x6
#define HWIO_MSS_RBCPR_RESULT_0_ERROR_STEPS_BMSK                                  0x3c
#define HWIO_MSS_RBCPR_RESULT_0_ERROR_STEPS_SHFT                                   0x2
#define HWIO_MSS_RBCPR_RESULT_0_STEP_UP_BMSK                                       0x2
#define HWIO_MSS_RBCPR_RESULT_0_STEP_UP_SHFT                                       0x1
#define HWIO_MSS_RBCPR_RESULT_0_STEP_DN_BMSK                                       0x1
#define HWIO_MSS_RBCPR_RESULT_0_STEP_DN_SHFT                                       0x0

#define HWIO_MSS_RBCPR_RESULT_1_ADDR                                        (MSS_RBCPR_WRAPPER_REG_BASE      + 0x000000a4)
#define HWIO_MSS_RBCPR_RESULT_1_RMSK                                           0xfffff
#define HWIO_MSS_RBCPR_RESULT_1_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_RESULT_1_ADDR, HWIO_MSS_RBCPR_RESULT_1_RMSK)
#define HWIO_MSS_RBCPR_RESULT_1_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_RESULT_1_ADDR, m)
#define HWIO_MSS_RBCPR_RESULT_1_SENSOR_SLOW_BMSK                               0xfe000
#define HWIO_MSS_RBCPR_RESULT_1_SENSOR_SLOW_SHFT                                   0xd
#define HWIO_MSS_RBCPR_RESULT_1_SEL_SLOW_BMSK                                   0x1c00
#define HWIO_MSS_RBCPR_RESULT_1_SEL_SLOW_SHFT                                      0xa
#define HWIO_MSS_RBCPR_RESULT_1_SENSOR_FAST_BMSK                                 0x3f8
#define HWIO_MSS_RBCPR_RESULT_1_SENSOR_FAST_SHFT                                   0x3
#define HWIO_MSS_RBCPR_RESULT_1_SEL_FAST_BMSK                                      0x7
#define HWIO_MSS_RBCPR_RESULT_1_SEL_FAST_SHFT                                      0x0

#define HWIO_MSS_RBIF_IRQ_EN_0_ADDR                                         (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000100)
#define HWIO_MSS_RBIF_IRQ_EN_0_RMSK                                               0x7f
#define HWIO_MSS_RBIF_IRQ_EN_0_IN          \
        in_dword_masked(HWIO_MSS_RBIF_IRQ_EN_0_ADDR, HWIO_MSS_RBIF_IRQ_EN_0_RMSK)
#define HWIO_MSS_RBIF_IRQ_EN_0_INM(m)      \
        in_dword_masked(HWIO_MSS_RBIF_IRQ_EN_0_ADDR, m)
#define HWIO_MSS_RBIF_IRQ_EN_0_OUT(v)      \
        out_dword(HWIO_MSS_RBIF_IRQ_EN_0_ADDR,v)
#define HWIO_MSS_RBIF_IRQ_EN_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBIF_IRQ_EN_0_ADDR,m,v,HWIO_MSS_RBIF_IRQ_EN_0_IN)
#define HWIO_MSS_RBIF_IRQ_EN_0_CLAMP_CHANGE_WHILE_BUSY_EN_BMSK                    0x40
#define HWIO_MSS_RBIF_IRQ_EN_0_CLAMP_CHANGE_WHILE_BUSY_EN_SHFT                     0x6
#define HWIO_MSS_RBIF_IRQ_EN_0_MAX_FLAG_EN_BMSK                                   0x20
#define HWIO_MSS_RBIF_IRQ_EN_0_MAX_FLAG_EN_SHFT                                    0x5
#define HWIO_MSS_RBIF_IRQ_EN_0_UP_FLAG_EN_BMSK                                    0x10
#define HWIO_MSS_RBIF_IRQ_EN_0_UP_FLAG_EN_SHFT                                     0x4
#define HWIO_MSS_RBIF_IRQ_EN_0_MID_FLAG_EN_BMSK                                    0x8
#define HWIO_MSS_RBIF_IRQ_EN_0_MID_FLAG_EN_SHFT                                    0x3
#define HWIO_MSS_RBIF_IRQ_EN_0_DOWN_FLAG_EN_BMSK                                   0x4
#define HWIO_MSS_RBIF_IRQ_EN_0_DOWN_FLAG_EN_SHFT                                   0x2
#define HWIO_MSS_RBIF_IRQ_EN_0_MIN_FLAG_EN_BMSK                                    0x2
#define HWIO_MSS_RBIF_IRQ_EN_0_MIN_FLAG_EN_SHFT                                    0x1
#define HWIO_MSS_RBIF_IRQ_EN_0_RBCPR_DONE_EN_BMSK                                  0x1
#define HWIO_MSS_RBIF_IRQ_EN_0_RBCPR_DONE_EN_SHFT                                  0x0

#define HWIO_MSS_RBIF_IRQ_EN_1_ADDR                                         (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000104)
#define HWIO_MSS_RBIF_IRQ_EN_1_RMSK                                               0x7f
#define HWIO_MSS_RBIF_IRQ_EN_1_IN          \
        in_dword_masked(HWIO_MSS_RBIF_IRQ_EN_1_ADDR, HWIO_MSS_RBIF_IRQ_EN_1_RMSK)
#define HWIO_MSS_RBIF_IRQ_EN_1_INM(m)      \
        in_dword_masked(HWIO_MSS_RBIF_IRQ_EN_1_ADDR, m)
#define HWIO_MSS_RBIF_IRQ_EN_1_OUT(v)      \
        out_dword(HWIO_MSS_RBIF_IRQ_EN_1_ADDR,v)
#define HWIO_MSS_RBIF_IRQ_EN_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBIF_IRQ_EN_1_ADDR,m,v,HWIO_MSS_RBIF_IRQ_EN_1_IN)
#define HWIO_MSS_RBIF_IRQ_EN_1_CLAMP_CHANGE_WHILE_BUSY_EN_BMSK                    0x40
#define HWIO_MSS_RBIF_IRQ_EN_1_CLAMP_CHANGE_WHILE_BUSY_EN_SHFT                     0x6
#define HWIO_MSS_RBIF_IRQ_EN_1_MAX_FLAG_EN_BMSK                                   0x20
#define HWIO_MSS_RBIF_IRQ_EN_1_MAX_FLAG_EN_SHFT                                    0x5
#define HWIO_MSS_RBIF_IRQ_EN_1_UP_FLAG_EN_BMSK                                    0x10
#define HWIO_MSS_RBIF_IRQ_EN_1_UP_FLAG_EN_SHFT                                     0x4
#define HWIO_MSS_RBIF_IRQ_EN_1_MID_FLAG_EN_BMSK                                    0x8
#define HWIO_MSS_RBIF_IRQ_EN_1_MID_FLAG_EN_SHFT                                    0x3
#define HWIO_MSS_RBIF_IRQ_EN_1_DOWN_FLAG_EN_BMSK                                   0x4
#define HWIO_MSS_RBIF_IRQ_EN_1_DOWN_FLAG_EN_SHFT                                   0x2
#define HWIO_MSS_RBIF_IRQ_EN_1_MIN_FLAG_EN_BMSK                                    0x2
#define HWIO_MSS_RBIF_IRQ_EN_1_MIN_FLAG_EN_SHFT                                    0x1
#define HWIO_MSS_RBIF_IRQ_EN_1_RBCPR_DONE_EN_BMSK                                  0x1
#define HWIO_MSS_RBIF_IRQ_EN_1_RBCPR_DONE_EN_SHFT                                  0x0

#define HWIO_MSS_RBIF_IRQ_EN_2_ADDR                                         (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000108)
#define HWIO_MSS_RBIF_IRQ_EN_2_RMSK                                               0x7f
#define HWIO_MSS_RBIF_IRQ_EN_2_IN          \
        in_dword_masked(HWIO_MSS_RBIF_IRQ_EN_2_ADDR, HWIO_MSS_RBIF_IRQ_EN_2_RMSK)
#define HWIO_MSS_RBIF_IRQ_EN_2_INM(m)      \
        in_dword_masked(HWIO_MSS_RBIF_IRQ_EN_2_ADDR, m)
#define HWIO_MSS_RBIF_IRQ_EN_2_OUT(v)      \
        out_dword(HWIO_MSS_RBIF_IRQ_EN_2_ADDR,v)
#define HWIO_MSS_RBIF_IRQ_EN_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBIF_IRQ_EN_2_ADDR,m,v,HWIO_MSS_RBIF_IRQ_EN_2_IN)
#define HWIO_MSS_RBIF_IRQ_EN_2_CLAMP_CHANGE_WHILE_BUSY_EN_BMSK                    0x40
#define HWIO_MSS_RBIF_IRQ_EN_2_CLAMP_CHANGE_WHILE_BUSY_EN_SHFT                     0x6
#define HWIO_MSS_RBIF_IRQ_EN_2_MAX_FLAG_EN_BMSK                                   0x20
#define HWIO_MSS_RBIF_IRQ_EN_2_MAX_FLAG_EN_SHFT                                    0x5
#define HWIO_MSS_RBIF_IRQ_EN_2_UP_FLAG_EN_BMSK                                    0x10
#define HWIO_MSS_RBIF_IRQ_EN_2_UP_FLAG_EN_SHFT                                     0x4
#define HWIO_MSS_RBIF_IRQ_EN_2_MID_FLAG_EN_BMSK                                    0x8
#define HWIO_MSS_RBIF_IRQ_EN_2_MID_FLAG_EN_SHFT                                    0x3
#define HWIO_MSS_RBIF_IRQ_EN_2_DOWN_FLAG_EN_BMSK                                   0x4
#define HWIO_MSS_RBIF_IRQ_EN_2_DOWN_FLAG_EN_SHFT                                   0x2
#define HWIO_MSS_RBIF_IRQ_EN_2_MIN_FLAG_EN_BMSK                                    0x2
#define HWIO_MSS_RBIF_IRQ_EN_2_MIN_FLAG_EN_SHFT                                    0x1
#define HWIO_MSS_RBIF_IRQ_EN_2_RBCPR_DONE_EN_BMSK                                  0x1
#define HWIO_MSS_RBIF_IRQ_EN_2_RBCPR_DONE_EN_SHFT                                  0x0

#define HWIO_MSS_RBIF_IRQ_CLEAR_ADDR                                        (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000110)
#define HWIO_MSS_RBIF_IRQ_CLEAR_RMSK                                              0x7f
#define HWIO_MSS_RBIF_IRQ_CLEAR_OUT(v)      \
        out_dword(HWIO_MSS_RBIF_IRQ_CLEAR_ADDR,v)
#define HWIO_MSS_RBIF_IRQ_CLEAR_CLAMP_CHANGE_WHILE_BUSY_CLEAR_BMSK                0x40
#define HWIO_MSS_RBIF_IRQ_CLEAR_CLAMP_CHANGE_WHILE_BUSY_CLEAR_SHFT                 0x6
#define HWIO_MSS_RBIF_IRQ_CLEAR_MAX_FLAG_CLEAR_BMSK                               0x20
#define HWIO_MSS_RBIF_IRQ_CLEAR_MAX_FLAG_CLEAR_SHFT                                0x5
#define HWIO_MSS_RBIF_IRQ_CLEAR_UP_FLAG_CLEAR_BMSK                                0x10
#define HWIO_MSS_RBIF_IRQ_CLEAR_UP_FLAG_CLEAR_SHFT                                 0x4
#define HWIO_MSS_RBIF_IRQ_CLEAR_MID_FLAG_CLEAR_BMSK                                0x8
#define HWIO_MSS_RBIF_IRQ_CLEAR_MID_FLAG_CLEAR_SHFT                                0x3
#define HWIO_MSS_RBIF_IRQ_CLEAR_DOWN_FLAG_CLEAR_BMSK                               0x4
#define HWIO_MSS_RBIF_IRQ_CLEAR_DOWN_FLAG_CLEAR_SHFT                               0x2
#define HWIO_MSS_RBIF_IRQ_CLEAR_MIN_FLAG_CLEAR_BMSK                                0x2
#define HWIO_MSS_RBIF_IRQ_CLEAR_MIN_FLAG_CLEAR_SHFT                                0x1
#define HWIO_MSS_RBIF_IRQ_CLEAR_RBCPR_DONE_CLEAR_BMSK                              0x1
#define HWIO_MSS_RBIF_IRQ_CLEAR_RBCPR_DONE_CLEAR_SHFT                              0x0

#define HWIO_MSS_RBIF_IRQ_STATUS_ADDR                                       (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000114)
#define HWIO_MSS_RBIF_IRQ_STATUS_RMSK                                             0x7f
#define HWIO_MSS_RBIF_IRQ_STATUS_IN          \
        in_dword_masked(HWIO_MSS_RBIF_IRQ_STATUS_ADDR, HWIO_MSS_RBIF_IRQ_STATUS_RMSK)
#define HWIO_MSS_RBIF_IRQ_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_RBIF_IRQ_STATUS_ADDR, m)
#define HWIO_MSS_RBIF_IRQ_STATUS_CLAMP_CHANGE_WHILE_BUSY_BMSK                     0x40
#define HWIO_MSS_RBIF_IRQ_STATUS_CLAMP_CHANGE_WHILE_BUSY_SHFT                      0x6
#define HWIO_MSS_RBIF_IRQ_STATUS_MAX_FLAG_BMSK                                    0x20
#define HWIO_MSS_RBIF_IRQ_STATUS_MAX_FLAG_SHFT                                     0x5
#define HWIO_MSS_RBIF_IRQ_STATUS_UP_FLAG_BMSK                                     0x10
#define HWIO_MSS_RBIF_IRQ_STATUS_UP_FLAG_SHFT                                      0x4
#define HWIO_MSS_RBIF_IRQ_STATUS_MID_FLAG_BMSK                                     0x8
#define HWIO_MSS_RBIF_IRQ_STATUS_MID_FLAG_SHFT                                     0x3
#define HWIO_MSS_RBIF_IRQ_STATUS_DOWN_FLAG_BMSK                                    0x4
#define HWIO_MSS_RBIF_IRQ_STATUS_DOWN_FLAG_SHFT                                    0x2
#define HWIO_MSS_RBIF_IRQ_STATUS_MIN_FLAG_BMSK                                     0x2
#define HWIO_MSS_RBIF_IRQ_STATUS_MIN_FLAG_SHFT                                     0x1
#define HWIO_MSS_RBIF_IRQ_STATUS_RBCPR_DONE_BMSK                                   0x1
#define HWIO_MSS_RBIF_IRQ_STATUS_RBCPR_DONE_SHFT                                   0x0

#define HWIO_MSS_RBCPR_QUOT_AVG_ADDR                                        (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000118)
#define HWIO_MSS_RBCPR_QUOT_AVG_RMSK                                        0x3fffffff
#define HWIO_MSS_RBCPR_QUOT_AVG_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_QUOT_AVG_ADDR, HWIO_MSS_RBCPR_QUOT_AVG_RMSK)
#define HWIO_MSS_RBCPR_QUOT_AVG_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_QUOT_AVG_ADDR, m)
#define HWIO_MSS_RBCPR_QUOT_AVG_QUOT_SUM_BMSK                               0x3fffff00
#define HWIO_MSS_RBCPR_QUOT_AVG_QUOT_SUM_SHFT                                      0x8
#define HWIO_MSS_RBCPR_QUOT_AVG_SENSOR_SUM_BMSK                                   0xff
#define HWIO_MSS_RBCPR_QUOT_AVG_SENSOR_SUM_SHFT                                    0x0

#define HWIO_MSS_RBCPR_DEBUG0_ADDR                                          (MSS_RBCPR_WRAPPER_REG_BASE      + 0x0000011c)
#define HWIO_MSS_RBCPR_DEBUG0_RMSK                                          0xffffffff
#define HWIO_MSS_RBCPR_DEBUG0_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_DEBUG0_ADDR, HWIO_MSS_RBCPR_DEBUG0_RMSK)
#define HWIO_MSS_RBCPR_DEBUG0_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_DEBUG0_ADDR, m)
#define HWIO_MSS_RBCPR_DEBUG0_SCLK_CNT1_BMSK                                0xffe00000
#define HWIO_MSS_RBCPR_DEBUG0_SCLK_CNT1_SHFT                                      0x15
#define HWIO_MSS_RBCPR_DEBUG0_SCLK_CNT0_BMSK                                  0x1ffc00
#define HWIO_MSS_RBCPR_DEBUG0_SCLK_CNT0_SHFT                                       0xa
#define HWIO_MSS_RBCPR_DEBUG0_SCLK_CNT_FAIL_BMSK                                 0x300
#define HWIO_MSS_RBCPR_DEBUG0_SCLK_CNT_FAIL_SHFT                                   0x8
#define HWIO_MSS_RBCPR_DEBUG0_DBG_END_OF_CHAIN_DREG_BMSK                          0xc0
#define HWIO_MSS_RBCPR_DEBUG0_DBG_END_OF_CHAIN_DREG_SHFT                           0x6
#define HWIO_MSS_RBCPR_DEBUG0_DBG_SHIFT_RING_BMSK                                 0x3f
#define HWIO_MSS_RBCPR_DEBUG0_DBG_SHIFT_RING_SHFT                                  0x0

#define HWIO_MSS_RBCPR_DEBUG1_ADDR                                          (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000120)
#define HWIO_MSS_RBCPR_DEBUG1_RMSK                                          0xffffffff
#define HWIO_MSS_RBCPR_DEBUG1_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_DEBUG1_ADDR, HWIO_MSS_RBCPR_DEBUG1_RMSK)
#define HWIO_MSS_RBCPR_DEBUG1_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_DEBUG1_ADDR, m)
#define HWIO_MSS_RBCPR_DEBUG1_DBG_END_OF_CHAIN_ENA_BMSK                     0x80000000
#define HWIO_MSS_RBCPR_DEBUG1_DBG_END_OF_CHAIN_ENA_SHFT                           0x1f
#define HWIO_MSS_RBCPR_DEBUG1_DBG_VALID_BMSK                                0x40000000
#define HWIO_MSS_RBCPR_DEBUG1_DBG_VALID_SHFT                                      0x1e
#define HWIO_MSS_RBCPR_DEBUG1_DBG_END_OF_CHAIN_MODE_BMSK                    0x20000000
#define HWIO_MSS_RBCPR_DEBUG1_DBG_END_OF_CHAIN_MODE_SHFT                          0x1d
#define HWIO_MSS_RBCPR_DEBUG1_DBG_END_OF_CHAIN_DATA_BMSK                    0x18000000
#define HWIO_MSS_RBCPR_DEBUG1_DBG_END_OF_CHAIN_DATA_SHFT                          0x1b
#define HWIO_MSS_RBCPR_DEBUG1_DBG_FSM_STATE_BMSK                             0x7000000
#define HWIO_MSS_RBCPR_DEBUG1_DBG_FSM_STATE_SHFT                                  0x18
#define HWIO_MSS_RBCPR_DEBUG1_QUOT_SLOW_BMSK                                  0xfff000
#define HWIO_MSS_RBCPR_DEBUG1_QUOT_SLOW_SHFT                                       0xc
#define HWIO_MSS_RBCPR_DEBUG1_QUOT_FAST_BMSK                                     0xfff
#define HWIO_MSS_RBCPR_DEBUG1_QUOT_FAST_SHFT                                       0x0

#define HWIO_MSS_RBCPR_DEBUG2_ADDR                                          (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000124)
#define HWIO_MSS_RBCPR_DEBUG2_RMSK                                          0xffffffff
#define HWIO_MSS_RBCPR_DEBUG2_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_DEBUG2_ADDR, HWIO_MSS_RBCPR_DEBUG2_RMSK)
#define HWIO_MSS_RBCPR_DEBUG2_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_DEBUG2_ADDR, m)
#define HWIO_MSS_RBCPR_DEBUG2_DBG_SEL_DONE_BMSK                             0x80000000
#define HWIO_MSS_RBCPR_DEBUG2_DBG_SEL_DONE_SHFT                                   0x1f
#define HWIO_MSS_RBCPR_DEBUG2_DBG_SENSOR_DONE_BMSK                          0x40000000
#define HWIO_MSS_RBCPR_DEBUG2_DBG_SENSOR_DONE_SHFT                                0x1e
#define HWIO_MSS_RBCPR_DEBUG2_DBG_SENSOR_BMSK                               0x3f800000
#define HWIO_MSS_RBCPR_DEBUG2_DBG_SENSOR_SHFT                                     0x17
#define HWIO_MSS_RBCPR_DEBUG2_DBG_SEL_BMSK                                    0x700000
#define HWIO_MSS_RBCPR_DEBUG2_DBG_SEL_SHFT                                        0x14
#define HWIO_MSS_RBCPR_DEBUG2_DBG_QUOT_BMSK                                    0xfff00
#define HWIO_MSS_RBCPR_DEBUG2_DBG_QUOT_SHFT                                        0x8
#define HWIO_MSS_RBCPR_DEBUG2_DBG_STEP_QUOT_BMSK                                  0xff
#define HWIO_MSS_RBCPR_DEBUG2_DBG_STEP_QUOT_SHFT                                   0x0

#define HWIO_MSS_RBCPR_BIST_MINMAX_ADDR                                     (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000128)
#define HWIO_MSS_RBCPR_BIST_MINMAX_RMSK                                       0xffffff
#define HWIO_MSS_RBCPR_BIST_MINMAX_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_BIST_MINMAX_ADDR, HWIO_MSS_RBCPR_BIST_MINMAX_RMSK)
#define HWIO_MSS_RBCPR_BIST_MINMAX_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_BIST_MINMAX_ADDR, m)
#define HWIO_MSS_RBCPR_BIST_MINMAX_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_BIST_MINMAX_ADDR,v)
#define HWIO_MSS_RBCPR_BIST_MINMAX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_BIST_MINMAX_ADDR,m,v,HWIO_MSS_RBCPR_BIST_MINMAX_IN)
#define HWIO_MSS_RBCPR_BIST_MINMAX_BIST_QUOT_ABSMIN_BMSK                      0xfff000
#define HWIO_MSS_RBCPR_BIST_MINMAX_BIST_QUOT_ABSMIN_SHFT                           0xc
#define HWIO_MSS_RBCPR_BIST_MINMAX_BIST_QUOT_ABSMAX_BMSK                         0xfff
#define HWIO_MSS_RBCPR_BIST_MINMAX_BIST_QUOT_ABSMAX_SHFT                           0x0

#define HWIO_MSS_RBCPR_BIST_RESULT_ADDR                                     (MSS_RBCPR_WRAPPER_REG_BASE      + 0x0000012c)
#define HWIO_MSS_RBCPR_BIST_RESULT_RMSK                                        0x1ffff
#define HWIO_MSS_RBCPR_BIST_RESULT_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_BIST_RESULT_ADDR, HWIO_MSS_RBCPR_BIST_RESULT_RMSK)
#define HWIO_MSS_RBCPR_BIST_RESULT_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_BIST_RESULT_ADDR, m)
#define HWIO_MSS_RBCPR_BIST_RESULT_SENSOR_ELAB_DONE_BMSK                       0x10000
#define HWIO_MSS_RBCPR_BIST_RESULT_SENSOR_ELAB_DONE_SHFT                          0x10
#define HWIO_MSS_RBCPR_BIST_RESULT_BIST_FAIL_BMSK                               0x8000
#define HWIO_MSS_RBCPR_BIST_RESULT_BIST_FAIL_SHFT                                  0xf
#define HWIO_MSS_RBCPR_BIST_RESULT_BIST_FAIL_LASTSEL_BMSK                       0x7000
#define HWIO_MSS_RBCPR_BIST_RESULT_BIST_FAIL_LASTSEL_SHFT                          0xc
#define HWIO_MSS_RBCPR_BIST_RESULT_BIST_FAIL_LASTQUOT_BMSK                       0xfff
#define HWIO_MSS_RBCPR_BIST_RESULT_BIST_FAIL_LASTQUOT_SHFT                         0x0

#define HWIO_MSS_RBCPR_BIST_FAIL_MAP0_ADDR                                  (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000130)
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP0_RMSK                                  0xffffffff
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP0_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_BIST_FAIL_MAP0_ADDR, HWIO_MSS_RBCPR_BIST_FAIL_MAP0_RMSK)
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP0_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_BIST_FAIL_MAP0_ADDR, m)
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP0_BIST_FAIL_MAP_BMSK                    0xffffffff
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP0_BIST_FAIL_MAP_SHFT                           0x0

#define HWIO_MSS_RBCPR_BIST_FAIL_MAP1_ADDR                                  (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000134)
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP1_RMSK                                  0xffffffff
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP1_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_BIST_FAIL_MAP1_ADDR, HWIO_MSS_RBCPR_BIST_FAIL_MAP1_RMSK)
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP1_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_BIST_FAIL_MAP1_ADDR, m)
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP1_BIST_FAIL_MAP_BMSK                    0xffffffff
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP1_BIST_FAIL_MAP_SHFT                           0x0

#define HWIO_MSS_RBCPR_BIST_FAIL_MAP2_ADDR                                  (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000138)
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP2_RMSK                                  0xffffffff
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP2_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_BIST_FAIL_MAP2_ADDR, HWIO_MSS_RBCPR_BIST_FAIL_MAP2_RMSK)
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP2_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_BIST_FAIL_MAP2_ADDR, m)
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP2_BIST_FAIL_MAP_BMSK                    0xffffffff
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP2_BIST_FAIL_MAP_SHFT                           0x0

#define HWIO_MSS_RBCPR_BIST_FAIL_MAP3_ADDR                                  (MSS_RBCPR_WRAPPER_REG_BASE      + 0x0000013c)
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP3_RMSK                                  0xffffffff
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP3_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_BIST_FAIL_MAP3_ADDR, HWIO_MSS_RBCPR_BIST_FAIL_MAP3_RMSK)
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP3_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_BIST_FAIL_MAP3_ADDR, m)
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP3_BIST_FAIL_MAP_BMSK                    0xffffffff
#define HWIO_MSS_RBCPR_BIST_FAIL_MAP3_BIST_FAIL_MAP_SHFT                           0x0

#define HWIO_MSS_RBCPR_LOG_0_ADDR                                           (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000140)
#define HWIO_MSS_RBCPR_LOG_0_RMSK                                           0xfffff7ff
#define HWIO_MSS_RBCPR_LOG_0_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_LOG_0_ADDR, HWIO_MSS_RBCPR_LOG_0_RMSK)
#define HWIO_MSS_RBCPR_LOG_0_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_LOG_0_ADDR, m)
#define HWIO_MSS_RBCPR_LOG_0_MID_COUNT_BMSK                                 0xfffc0000
#define HWIO_MSS_RBCPR_LOG_0_MID_COUNT_SHFT                                       0x12
#define HWIO_MSS_RBCPR_LOG_0_MX_VLEVEL_BMSK                                    0x3f000
#define HWIO_MSS_RBCPR_LOG_0_MX_VLEVEL_SHFT                                        0xc
#define HWIO_MSS_RBCPR_LOG_0_MAX_FLAG_BMSK                                       0x400
#define HWIO_MSS_RBCPR_LOG_0_MAX_FLAG_SHFT                                         0xa
#define HWIO_MSS_RBCPR_LOG_0_UP_FLAG_BMSK                                        0x200
#define HWIO_MSS_RBCPR_LOG_0_UP_FLAG_SHFT                                          0x9
#define HWIO_MSS_RBCPR_LOG_0_MID_FLAG_BMSK                                       0x100
#define HWIO_MSS_RBCPR_LOG_0_MID_FLAG_SHFT                                         0x8
#define HWIO_MSS_RBCPR_LOG_0_DOWN_FLAG_BMSK                                       0x80
#define HWIO_MSS_RBCPR_LOG_0_DOWN_FLAG_SHFT                                        0x7
#define HWIO_MSS_RBCPR_LOG_0_MIN_FLAG_BMSK                                        0x40
#define HWIO_MSS_RBCPR_LOG_0_MIN_FLAG_SHFT                                         0x6
#define HWIO_MSS_RBCPR_LOG_0_VLEVEL_BMSK                                          0x3f
#define HWIO_MSS_RBCPR_LOG_0_VLEVEL_SHFT                                           0x0

#define HWIO_MSS_RBCPR_LOG_1_ADDR                                           (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000144)
#define HWIO_MSS_RBCPR_LOG_1_RMSK                                           0xfffff7ff
#define HWIO_MSS_RBCPR_LOG_1_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_LOG_1_ADDR, HWIO_MSS_RBCPR_LOG_1_RMSK)
#define HWIO_MSS_RBCPR_LOG_1_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_LOG_1_ADDR, m)
#define HWIO_MSS_RBCPR_LOG_1_MID_COUNT_BMSK                                 0xfffc0000
#define HWIO_MSS_RBCPR_LOG_1_MID_COUNT_SHFT                                       0x12
#define HWIO_MSS_RBCPR_LOG_1_MX_VLEVEL_BMSK                                    0x3f000
#define HWIO_MSS_RBCPR_LOG_1_MX_VLEVEL_SHFT                                        0xc
#define HWIO_MSS_RBCPR_LOG_1_MAX_FLAG_BMSK                                       0x400
#define HWIO_MSS_RBCPR_LOG_1_MAX_FLAG_SHFT                                         0xa
#define HWIO_MSS_RBCPR_LOG_1_UP_FLAG_BMSK                                        0x200
#define HWIO_MSS_RBCPR_LOG_1_UP_FLAG_SHFT                                          0x9
#define HWIO_MSS_RBCPR_LOG_1_MID_FLAG_BMSK                                       0x100
#define HWIO_MSS_RBCPR_LOG_1_MID_FLAG_SHFT                                         0x8
#define HWIO_MSS_RBCPR_LOG_1_DOWN_FLAG_BMSK                                       0x80
#define HWIO_MSS_RBCPR_LOG_1_DOWN_FLAG_SHFT                                        0x7
#define HWIO_MSS_RBCPR_LOG_1_MIN_FLAG_BMSK                                        0x40
#define HWIO_MSS_RBCPR_LOG_1_MIN_FLAG_SHFT                                         0x6
#define HWIO_MSS_RBCPR_LOG_1_VLEVEL_BMSK                                          0x3f
#define HWIO_MSS_RBCPR_LOG_1_VLEVEL_SHFT                                           0x0

#define HWIO_MSS_RBCPR_LOG_2_ADDR                                           (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000148)
#define HWIO_MSS_RBCPR_LOG_2_RMSK                                           0xfffff7ff
#define HWIO_MSS_RBCPR_LOG_2_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_LOG_2_ADDR, HWIO_MSS_RBCPR_LOG_2_RMSK)
#define HWIO_MSS_RBCPR_LOG_2_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_LOG_2_ADDR, m)
#define HWIO_MSS_RBCPR_LOG_2_MID_COUNT_BMSK                                 0xfffc0000
#define HWIO_MSS_RBCPR_LOG_2_MID_COUNT_SHFT                                       0x12
#define HWIO_MSS_RBCPR_LOG_2_MX_VLEVEL_BMSK                                    0x3f000
#define HWIO_MSS_RBCPR_LOG_2_MX_VLEVEL_SHFT                                        0xc
#define HWIO_MSS_RBCPR_LOG_2_MAX_FLAG_BMSK                                       0x400
#define HWIO_MSS_RBCPR_LOG_2_MAX_FLAG_SHFT                                         0xa
#define HWIO_MSS_RBCPR_LOG_2_UP_FLAG_BMSK                                        0x200
#define HWIO_MSS_RBCPR_LOG_2_UP_FLAG_SHFT                                          0x9
#define HWIO_MSS_RBCPR_LOG_2_MID_FLAG_BMSK                                       0x100
#define HWIO_MSS_RBCPR_LOG_2_MID_FLAG_SHFT                                         0x8
#define HWIO_MSS_RBCPR_LOG_2_DOWN_FLAG_BMSK                                       0x80
#define HWIO_MSS_RBCPR_LOG_2_DOWN_FLAG_SHFT                                        0x7
#define HWIO_MSS_RBCPR_LOG_2_MIN_FLAG_BMSK                                        0x40
#define HWIO_MSS_RBCPR_LOG_2_MIN_FLAG_SHFT                                         0x6
#define HWIO_MSS_RBCPR_LOG_2_VLEVEL_BMSK                                          0x3f
#define HWIO_MSS_RBCPR_LOG_2_VLEVEL_SHFT                                           0x0

#define HWIO_MSS_RBCPR_LOG_3_ADDR                                           (MSS_RBCPR_WRAPPER_REG_BASE      + 0x0000014c)
#define HWIO_MSS_RBCPR_LOG_3_RMSK                                           0xfffff7ff
#define HWIO_MSS_RBCPR_LOG_3_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_LOG_3_ADDR, HWIO_MSS_RBCPR_LOG_3_RMSK)
#define HWIO_MSS_RBCPR_LOG_3_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_LOG_3_ADDR, m)
#define HWIO_MSS_RBCPR_LOG_3_MID_COUNT_BMSK                                 0xfffc0000
#define HWIO_MSS_RBCPR_LOG_3_MID_COUNT_SHFT                                       0x12
#define HWIO_MSS_RBCPR_LOG_3_MX_VLEVEL_BMSK                                    0x3f000
#define HWIO_MSS_RBCPR_LOG_3_MX_VLEVEL_SHFT                                        0xc
#define HWIO_MSS_RBCPR_LOG_3_MAX_FLAG_BMSK                                       0x400
#define HWIO_MSS_RBCPR_LOG_3_MAX_FLAG_SHFT                                         0xa
#define HWIO_MSS_RBCPR_LOG_3_UP_FLAG_BMSK                                        0x200
#define HWIO_MSS_RBCPR_LOG_3_UP_FLAG_SHFT                                          0x9
#define HWIO_MSS_RBCPR_LOG_3_MID_FLAG_BMSK                                       0x100
#define HWIO_MSS_RBCPR_LOG_3_MID_FLAG_SHFT                                         0x8
#define HWIO_MSS_RBCPR_LOG_3_DOWN_FLAG_BMSK                                       0x80
#define HWIO_MSS_RBCPR_LOG_3_DOWN_FLAG_SHFT                                        0x7
#define HWIO_MSS_RBCPR_LOG_3_MIN_FLAG_BMSK                                        0x40
#define HWIO_MSS_RBCPR_LOG_3_MIN_FLAG_SHFT                                         0x6
#define HWIO_MSS_RBCPR_LOG_3_VLEVEL_BMSK                                          0x3f
#define HWIO_MSS_RBCPR_LOG_3_VLEVEL_SHFT                                           0x0

#define HWIO_MSS_RBCPR_LOG_4_ADDR                                           (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000150)
#define HWIO_MSS_RBCPR_LOG_4_RMSK                                           0xfffff7ff
#define HWIO_MSS_RBCPR_LOG_4_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_LOG_4_ADDR, HWIO_MSS_RBCPR_LOG_4_RMSK)
#define HWIO_MSS_RBCPR_LOG_4_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_LOG_4_ADDR, m)
#define HWIO_MSS_RBCPR_LOG_4_MID_COUNT_BMSK                                 0xfffc0000
#define HWIO_MSS_RBCPR_LOG_4_MID_COUNT_SHFT                                       0x12
#define HWIO_MSS_RBCPR_LOG_4_MX_VLEVEL_BMSK                                    0x3f000
#define HWIO_MSS_RBCPR_LOG_4_MX_VLEVEL_SHFT                                        0xc
#define HWIO_MSS_RBCPR_LOG_4_MAX_FLAG_BMSK                                       0x400
#define HWIO_MSS_RBCPR_LOG_4_MAX_FLAG_SHFT                                         0xa
#define HWIO_MSS_RBCPR_LOG_4_UP_FLAG_BMSK                                        0x200
#define HWIO_MSS_RBCPR_LOG_4_UP_FLAG_SHFT                                          0x9
#define HWIO_MSS_RBCPR_LOG_4_MID_FLAG_BMSK                                       0x100
#define HWIO_MSS_RBCPR_LOG_4_MID_FLAG_SHFT                                         0x8
#define HWIO_MSS_RBCPR_LOG_4_DOWN_FLAG_BMSK                                       0x80
#define HWIO_MSS_RBCPR_LOG_4_DOWN_FLAG_SHFT                                        0x7
#define HWIO_MSS_RBCPR_LOG_4_MIN_FLAG_BMSK                                        0x40
#define HWIO_MSS_RBCPR_LOG_4_MIN_FLAG_SHFT                                         0x6
#define HWIO_MSS_RBCPR_LOG_4_VLEVEL_BMSK                                          0x3f
#define HWIO_MSS_RBCPR_LOG_4_VLEVEL_SHFT                                           0x0

#define HWIO_MSS_RBCPR_LOG_5_ADDR                                           (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000154)
#define HWIO_MSS_RBCPR_LOG_5_RMSK                                           0xfffff7ff
#define HWIO_MSS_RBCPR_LOG_5_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_LOG_5_ADDR, HWIO_MSS_RBCPR_LOG_5_RMSK)
#define HWIO_MSS_RBCPR_LOG_5_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_LOG_5_ADDR, m)
#define HWIO_MSS_RBCPR_LOG_5_MID_COUNT_BMSK                                 0xfffc0000
#define HWIO_MSS_RBCPR_LOG_5_MID_COUNT_SHFT                                       0x12
#define HWIO_MSS_RBCPR_LOG_5_MX_VLEVEL_BMSK                                    0x3f000
#define HWIO_MSS_RBCPR_LOG_5_MX_VLEVEL_SHFT                                        0xc
#define HWIO_MSS_RBCPR_LOG_5_MAX_FLAG_BMSK                                       0x400
#define HWIO_MSS_RBCPR_LOG_5_MAX_FLAG_SHFT                                         0xa
#define HWIO_MSS_RBCPR_LOG_5_UP_FLAG_BMSK                                        0x200
#define HWIO_MSS_RBCPR_LOG_5_UP_FLAG_SHFT                                          0x9
#define HWIO_MSS_RBCPR_LOG_5_MID_FLAG_BMSK                                       0x100
#define HWIO_MSS_RBCPR_LOG_5_MID_FLAG_SHFT                                         0x8
#define HWIO_MSS_RBCPR_LOG_5_DOWN_FLAG_BMSK                                       0x80
#define HWIO_MSS_RBCPR_LOG_5_DOWN_FLAG_SHFT                                        0x7
#define HWIO_MSS_RBCPR_LOG_5_MIN_FLAG_BMSK                                        0x40
#define HWIO_MSS_RBCPR_LOG_5_MIN_FLAG_SHFT                                         0x6
#define HWIO_MSS_RBCPR_LOG_5_VLEVEL_BMSK                                          0x3f
#define HWIO_MSS_RBCPR_LOG_5_VLEVEL_SHFT                                           0x0

#define HWIO_MSS_RBCPR_LOG_6_ADDR                                           (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000158)
#define HWIO_MSS_RBCPR_LOG_6_RMSK                                           0xfffff7ff
#define HWIO_MSS_RBCPR_LOG_6_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_LOG_6_ADDR, HWIO_MSS_RBCPR_LOG_6_RMSK)
#define HWIO_MSS_RBCPR_LOG_6_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_LOG_6_ADDR, m)
#define HWIO_MSS_RBCPR_LOG_6_MID_COUNT_BMSK                                 0xfffc0000
#define HWIO_MSS_RBCPR_LOG_6_MID_COUNT_SHFT                                       0x12
#define HWIO_MSS_RBCPR_LOG_6_MX_VLEVEL_BMSK                                    0x3f000
#define HWIO_MSS_RBCPR_LOG_6_MX_VLEVEL_SHFT                                        0xc
#define HWIO_MSS_RBCPR_LOG_6_MAX_FLAG_BMSK                                       0x400
#define HWIO_MSS_RBCPR_LOG_6_MAX_FLAG_SHFT                                         0xa
#define HWIO_MSS_RBCPR_LOG_6_UP_FLAG_BMSK                                        0x200
#define HWIO_MSS_RBCPR_LOG_6_UP_FLAG_SHFT                                          0x9
#define HWIO_MSS_RBCPR_LOG_6_MID_FLAG_BMSK                                       0x100
#define HWIO_MSS_RBCPR_LOG_6_MID_FLAG_SHFT                                         0x8
#define HWIO_MSS_RBCPR_LOG_6_DOWN_FLAG_BMSK                                       0x80
#define HWIO_MSS_RBCPR_LOG_6_DOWN_FLAG_SHFT                                        0x7
#define HWIO_MSS_RBCPR_LOG_6_MIN_FLAG_BMSK                                        0x40
#define HWIO_MSS_RBCPR_LOG_6_MIN_FLAG_SHFT                                         0x6
#define HWIO_MSS_RBCPR_LOG_6_VLEVEL_BMSK                                          0x3f
#define HWIO_MSS_RBCPR_LOG_6_VLEVEL_SHFT                                           0x0

#define HWIO_MSS_RBCPR_LOG_7_ADDR                                           (MSS_RBCPR_WRAPPER_REG_BASE      + 0x0000015c)
#define HWIO_MSS_RBCPR_LOG_7_RMSK                                           0xfffff7ff
#define HWIO_MSS_RBCPR_LOG_7_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_LOG_7_ADDR, HWIO_MSS_RBCPR_LOG_7_RMSK)
#define HWIO_MSS_RBCPR_LOG_7_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_LOG_7_ADDR, m)
#define HWIO_MSS_RBCPR_LOG_7_MID_COUNT_BMSK                                 0xfffc0000
#define HWIO_MSS_RBCPR_LOG_7_MID_COUNT_SHFT                                       0x12
#define HWIO_MSS_RBCPR_LOG_7_MX_VLEVEL_BMSK                                    0x3f000
#define HWIO_MSS_RBCPR_LOG_7_MX_VLEVEL_SHFT                                        0xc
#define HWIO_MSS_RBCPR_LOG_7_MAX_FLAG_BMSK                                       0x400
#define HWIO_MSS_RBCPR_LOG_7_MAX_FLAG_SHFT                                         0xa
#define HWIO_MSS_RBCPR_LOG_7_UP_FLAG_BMSK                                        0x200
#define HWIO_MSS_RBCPR_LOG_7_UP_FLAG_SHFT                                          0x9
#define HWIO_MSS_RBCPR_LOG_7_MID_FLAG_BMSK                                       0x100
#define HWIO_MSS_RBCPR_LOG_7_MID_FLAG_SHFT                                         0x8
#define HWIO_MSS_RBCPR_LOG_7_DOWN_FLAG_BMSK                                       0x80
#define HWIO_MSS_RBCPR_LOG_7_DOWN_FLAG_SHFT                                        0x7
#define HWIO_MSS_RBCPR_LOG_7_MIN_FLAG_BMSK                                        0x40
#define HWIO_MSS_RBCPR_LOG_7_MIN_FLAG_SHFT                                         0x6
#define HWIO_MSS_RBCPR_LOG_7_VLEVEL_BMSK                                          0x3f
#define HWIO_MSS_RBCPR_LOG_7_VLEVEL_SHFT                                           0x0

#define HWIO_MSS_RBCPR_HTOL_AGE_ADDR                                        (MSS_RBCPR_WRAPPER_REG_BASE      + 0x00000160)
#define HWIO_MSS_RBCPR_HTOL_AGE_RMSK                                               0x7
#define HWIO_MSS_RBCPR_HTOL_AGE_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_HTOL_AGE_ADDR, HWIO_MSS_RBCPR_HTOL_AGE_RMSK)
#define HWIO_MSS_RBCPR_HTOL_AGE_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_HTOL_AGE_ADDR, m)
#define HWIO_MSS_RBCPR_HTOL_AGE_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_HTOL_AGE_ADDR,v)
#define HWIO_MSS_RBCPR_HTOL_AGE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_HTOL_AGE_ADDR,m,v,HWIO_MSS_RBCPR_HTOL_AGE_IN)
#define HWIO_MSS_RBCPR_HTOL_AGE_CPR_AGE_DATA_STATUS_BMSK                           0x4
#define HWIO_MSS_RBCPR_HTOL_AGE_CPR_AGE_DATA_STATUS_SHFT                           0x2
#define HWIO_MSS_RBCPR_HTOL_AGE_AGE_PAGE_BMSK                                      0x2
#define HWIO_MSS_RBCPR_HTOL_AGE_AGE_PAGE_SHFT                                      0x1
#define HWIO_MSS_RBCPR_HTOL_AGE_HTOL_MODE_BMSK                                     0x1
#define HWIO_MSS_RBCPR_HTOL_AGE_HTOL_MODE_SHFT                                     0x0

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                        (SECURITY_CONTROL_BASE      + 0x00000000)

#define HWIO_QFPROM_RAW_JTAG_ID_ADDR                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000000)
#define HWIO_QFPROM_RAW_JTAG_ID_RMSK                                                          0xffffffff
#define HWIO_QFPROM_RAW_JTAG_ID_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_JTAG_ID_ADDR, HWIO_QFPROM_RAW_JTAG_ID_RMSK)
#define HWIO_QFPROM_RAW_JTAG_ID_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_JTAG_ID_ADDR, m)
#define HWIO_QFPROM_RAW_JTAG_ID_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_JTAG_ID_ADDR,v)
#define HWIO_QFPROM_RAW_JTAG_ID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_JTAG_ID_ADDR,m,v,HWIO_QFPROM_RAW_JTAG_ID_IN)
#define HWIO_QFPROM_RAW_JTAG_ID_PTE_DATA0_BMSK                                                0xf0000000
#define HWIO_QFPROM_RAW_JTAG_ID_PTE_DATA0_SHFT                                                      0x1c
#define HWIO_QFPROM_RAW_JTAG_ID_FEATURE_ID_BMSK                                                0xff00000
#define HWIO_QFPROM_RAW_JTAG_ID_FEATURE_ID_SHFT                                                     0x14
#define HWIO_QFPROM_RAW_JTAG_ID_JTAG_ID_BMSK                                                     0xfffff
#define HWIO_QFPROM_RAW_JTAG_ID_JTAG_ID_SHFT                                                         0x0

#define HWIO_QFPROM_RAW_PTE1_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000004)
#define HWIO_QFPROM_RAW_PTE1_RMSK                                                             0xffffffff
#define HWIO_QFPROM_RAW_PTE1_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE1_ADDR, HWIO_QFPROM_RAW_PTE1_RMSK)
#define HWIO_QFPROM_RAW_PTE1_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE1_ADDR, m)
#define HWIO_QFPROM_RAW_PTE1_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE1_ADDR,v)
#define HWIO_QFPROM_RAW_PTE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE1_ADDR,m,v,HWIO_QFPROM_RAW_PTE1_IN)
#define HWIO_QFPROM_RAW_PTE1_IDDQ_BINNING_BMSK                                                0xfc000000
#define HWIO_QFPROM_RAW_PTE1_IDDQ_BINNING_SHFT                                                      0x1a
#define HWIO_QFPROM_RAW_PTE1_PROCESS_NODE_ID_BMSK                                              0x2000000
#define HWIO_QFPROM_RAW_PTE1_PROCESS_NODE_ID_SHFT                                                   0x19
#define HWIO_QFPROM_RAW_PTE1_PTE_DATA1_BMSK                                                    0x1ffffff
#define HWIO_QFPROM_RAW_PTE1_PTE_DATA1_SHFT                                                          0x0

#define HWIO_QFPROM_RAW_SERIAL_NUM_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000008)
#define HWIO_QFPROM_RAW_SERIAL_NUM_RMSK                                                       0xffffffff
#define HWIO_QFPROM_RAW_SERIAL_NUM_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SERIAL_NUM_ADDR, HWIO_QFPROM_RAW_SERIAL_NUM_RMSK)
#define HWIO_QFPROM_RAW_SERIAL_NUM_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SERIAL_NUM_ADDR, m)
#define HWIO_QFPROM_RAW_SERIAL_NUM_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SERIAL_NUM_ADDR,v)
#define HWIO_QFPROM_RAW_SERIAL_NUM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SERIAL_NUM_ADDR,m,v,HWIO_QFPROM_RAW_SERIAL_NUM_IN)
#define HWIO_QFPROM_RAW_SERIAL_NUM_SERIAL_NUM_BMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_SERIAL_NUM_SERIAL_NUM_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_PTE2_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000000c)
#define HWIO_QFPROM_RAW_PTE2_RMSK                                                             0xffffffff
#define HWIO_QFPROM_RAW_PTE2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE2_ADDR, HWIO_QFPROM_RAW_PTE2_RMSK)
#define HWIO_QFPROM_RAW_PTE2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE2_ADDR, m)
#define HWIO_QFPROM_RAW_PTE2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE2_ADDR,v)
#define HWIO_QFPROM_RAW_PTE2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE2_ADDR,m,v,HWIO_QFPROM_RAW_PTE2_IN)
#define HWIO_QFPROM_RAW_PTE2_WAFER_ID_BMSK                                                    0xf8000000
#define HWIO_QFPROM_RAW_PTE2_WAFER_ID_SHFT                                                          0x1b
#define HWIO_QFPROM_RAW_PTE2_DIE_X_BMSK                                                        0x7f80000
#define HWIO_QFPROM_RAW_PTE2_DIE_X_SHFT                                                             0x13
#define HWIO_QFPROM_RAW_PTE2_DIE_Y_BMSK                                                          0x7f800
#define HWIO_QFPROM_RAW_PTE2_DIE_Y_SHFT                                                              0xb
#define HWIO_QFPROM_RAW_PTE2_FOUNDRY_ID_BMSK                                                       0x700
#define HWIO_QFPROM_RAW_PTE2_FOUNDRY_ID_SHFT                                                         0x8
#define HWIO_QFPROM_RAW_PTE2_LOGIC_RETENTION_BMSK                                                   0xe0
#define HWIO_QFPROM_RAW_PTE2_LOGIC_RETENTION_SHFT                                                    0x5
#define HWIO_QFPROM_RAW_PTE2_SPEED_BIN_BMSK                                                         0x1c
#define HWIO_QFPROM_RAW_PTE2_SPEED_BIN_SHFT                                                          0x2
#define HWIO_QFPROM_RAW_PTE2_MX_RET_BIN_BMSK                                                         0x3
#define HWIO_QFPROM_RAW_PTE2_MX_RET_BIN_SHFT                                                         0x0

#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000010)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR, HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RMSK)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR,m,v,HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IN)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE_BMSK                                             0xffc00000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE_SHFT                                                   0x16
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE21_BMSK                                             0x200000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE21_SHFT                                                 0x15
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE20_BMSK                                             0x100000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE20_SHFT                                                 0x14
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE19_BMSK                                              0x80000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE19_SHFT                                                 0x13
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE18_BMSK                                              0x40000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE18_SHFT                                                 0x12
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE17_BMSK                                              0x20000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE17_SHFT                                                 0x11
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE16_BMSK                                              0x10000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE16_SHFT                                                 0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEC_EN_BMSK                                                0x8000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEC_EN_SHFT                                                   0xf
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_BOOT_ROM_PATCH_BMSK                                        0x4000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_BOOT_ROM_PATCH_SHFT                                           0xe
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_CONFIG_BMSK                                            0x2000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_CONFIG_SHFT                                               0xd
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CALIB_BMSK                                                 0x1000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CALIB_SHFT                                                    0xc
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PK_HASH_BMSK                                                0x800
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PK_HASH_SHFT                                                  0xb
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_QC_SEC_BOOT_BMSK                                            0x400
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_QC_SEC_BOOT_SHFT                                              0xa
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_SEC_BOOT_BMSK                                           0x200
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_SEC_BOOT_SHFT                                             0x9
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                 0x100
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                   0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                  0x80
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                   0x7
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEAT_CONFIG_BMSK                                             0x40
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEAT_CONFIG_SHFT                                              0x6
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_CONFIG_BMSK                                              0x20
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_CONFIG_SHFT                                               0x5
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_BMSK                                         0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_SHFT                                          0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_BMSK                                          0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_SHFT                                          0x3
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_BMSK                                          0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_SHFT                                          0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RD_WR_PERM_BMSK                                               0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RD_WR_PERM_SHFT                                               0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PTE_BMSK                                                      0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PTE_SHFT                                                      0x0

#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000014)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR, HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RMSK)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR,m,v,HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IN)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE_BMSK                                             0xffc00000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE_SHFT                                                   0x16
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE21_BMSK                                             0x200000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE21_SHFT                                                 0x15
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE20_BMSK                                             0x100000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE20_SHFT                                                 0x14
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE19_BMSK                                              0x80000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE19_SHFT                                                 0x13
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE18_BMSK                                              0x40000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE18_SHFT                                                 0x12
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE17_BMSK                                              0x20000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE17_SHFT                                                 0x11
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE16_BMSK                                              0x10000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE16_SHFT                                                 0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEC_EN_BMSK                                                0x8000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEC_EN_SHFT                                                   0xf
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_BOOT_ROM_PATCH_BMSK                                        0x4000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_BOOT_ROM_PATCH_SHFT                                           0xe
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_CONFIG_BMSK                                            0x2000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_CONFIG_SHFT                                               0xd
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CALIB_BMSK                                                 0x1000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CALIB_SHFT                                                    0xc
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PK_HASH_BMSK                                                0x800
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PK_HASH_SHFT                                                  0xb
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_QC_SEC_BOOT_BMSK                                            0x400
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_QC_SEC_BOOT_SHFT                                              0xa
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_SEC_BOOT_BMSK                                           0x200
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_SEC_BOOT_SHFT                                             0x9
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_BMSK                                 0x100
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_SHFT                                   0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_BMSK                                  0x80
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_SHFT                                   0x7
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEAT_CONFIG_BMSK                                             0x40
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEAT_CONFIG_SHFT                                              0x6
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_CONFIG_BMSK                                              0x20
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_CONFIG_SHFT                                               0x5
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_BMSK                                         0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_SHFT                                          0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_BMSK                                          0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_SHFT                                          0x3
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_BMSK                                          0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_SHFT                                          0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RD_WR_PERM_BMSK                                               0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RD_WR_PERM_SHFT                                               0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PTE_BMSK                                                      0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PTE_SHFT                                                      0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000018)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_PIL_SUBSYSTEM0_BMSK                               0xfc000000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_PIL_SUBSYSTEM0_SHFT                                     0x1a
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_TZ_BMSK                                            0x3fff000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_TZ_SHFT                                                  0xc
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_SBL1_BMSK                                              0xffe
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_SBL1_SHFT                                                0x1
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_RPMB_KEY_PROVISIONED_BMSK                                0x1
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_RPMB_KEY_PROVISIONED_SHFT                                0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000001c)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_APPSBL0_BMSK                                      0xfffc0000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_APPSBL0_SHFT                                            0x12
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_PIL_SUBSYSTEM1_BMSK                                  0x3ffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_PIL_SUBSYSTEM1_SHFT                                      0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000020)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_APPSBL1_BMSK                                      0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_APPSBL1_SHFT                                             0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000024)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_BMSK                      0xff000000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_SHFT                            0x18
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_HYPERVISOR_BMSK                                     0xfff000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_HYPERVISOR_SHFT                                          0xc
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RPM_BMSK                                               0xff0
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RPM_SHFT                                                 0x4
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_APPSBL2_BMSK                                             0xf
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_APPSBL2_SHFT                                             0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000028)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_MSS_BMSK                                          0xffff0000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_MSS_SHFT                                                0x10
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_MBA_BMSK                                              0xffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_MBA_SHFT                                                 0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000002c)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_SPARE0_BMSK                                       0xffffff00
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_SPARE0_SHFT                                              0x8
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_BMSK                      0xff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_SHFT                       0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000030)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE0_BMSK                                       0xff000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE0_SHFT                                             0x18
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG21_SECURE_BMSK                             0x800000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG21_SECURE_SHFT                                 0x17
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_BMSK                                0x400000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_SHFT                                    0x16
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_BMSK                            0x3c0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_SHFT                                0x12
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG20_SECURE_BMSK                              0x20000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG20_SECURE_SHFT                                 0x11
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG19_SECURE_BMSK                              0x10000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG19_SECURE_SHFT                                 0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG18_SECURE_BMSK                               0x8000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG18_SECURE_SHFT                                  0xf
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG17_SECURE_BMSK                               0x4000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG17_SECURE_SHFT                                  0xe
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG16_SECURE_BMSK                               0x2000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG16_SECURE_SHFT                                  0xd
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_WDOG_EN_BMSK                                          0x1000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_WDOG_EN_SHFT                                             0xc
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPDM_SECURE_MODE_BMSK                                  0x800
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPDM_SECURE_MODE_SHFT                                    0xb
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ALT_SD_PORT_FOR_BOOT_BMSK                              0x400
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ALT_SD_PORT_FOR_BOOT_SHFT                                0xa
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_BMSK                     0x200
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_SHFT                       0x9
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_BMSK                               0x100
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_SHFT                                 0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_BMSK                                          0xe0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SHFT                                           0x5
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_BMSK                                0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_SHFT                                 0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_BMSK                                 0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_SHFT                                 0x3
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_BMSK                                               0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_SHFT                                               0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_BMSK                                        0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_SHFT                                        0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_BMSK                                     0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_SHFT                                     0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000034)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE1_BMSK                                       0xff800000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE1_SHFT                                             0x17
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ANTI_ROLLBACK_FEATURE_EN_BMSK                       0x780000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ANTI_ROLLBACK_FEATURE_EN_SHFT                           0x13
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_BMSK                            0x40000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_SHFT                               0x12
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_BMSK                             0x20000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_SHFT                                0x11
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_BMSK                              0x10000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_SHFT                                 0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_BMSK                                0x8000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_SHFT                                   0xf
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_BMSK                                0x4000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_SHFT                                   0xe
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_BMSK                             0x2000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_SHFT                                0xd
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_BMSK                              0x1000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_SHFT                                 0xc
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_BMSK                                0x800
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_SHFT                                  0xb
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_BMSK                                0x400
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_SHFT                                  0xa
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE1_DISABLE_BMSK                                    0x200
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE1_DISABLE_SHFT                                      0x9
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE0_DISABLE_BMSK                                    0x100
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE0_DISABLE_SHFT                                      0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_VENUS_0_DBGEN_DISABLE_BMSK                              0x80
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_VENUS_0_DBGEN_DISABLE_SHFT                               0x7
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RPM_DAPEN_DISABLE_BMSK                                  0x40
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RPM_DAPEN_DISABLE_SHFT                                   0x6
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RPM_WCSS_NIDEN_DISABLE_BMSK                             0x20
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RPM_WCSS_NIDEN_DISABLE_SHFT                              0x5
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RPM_DBGEN_DISABLE_BMSK                                  0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RPM_DBGEN_DISABLE_SHFT                                   0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_WCSS_DBGEN_DISABLE_BMSK                                  0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_WCSS_DBGEN_DISABLE_SHFT                                  0x3
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_MSS_NIDEN_DISABLE_BMSK                                   0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_MSS_NIDEN_DISABLE_SHFT                                   0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_MSS_DBGEN_DISABLE_BMSK                                   0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_MSS_DBGEN_DISABLE_SHFT                                   0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ALL_DEBUG_DISABLE_BMSK                                   0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ALL_DEBUG_DISABLE_SHFT                                   0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000038)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_PRODUCT_ID_BMSK                               0xffff0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_PRODUCT_ID_SHFT                                     0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_BMSK                                        0xffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_SHFT                                           0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000003c)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_PERIPH_VID_BMSK                                   0xffff0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_PERIPH_VID_SHFT                                         0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_PERIPH_PID_BMSK                                       0xffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_PERIPH_PID_SHFT                                          0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000040)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_NAV_EN_BMSK                                 0x80000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_NAV_EN_SHFT                                       0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_LDO_EN_BMSK                                 0x40000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_LDO_EN_SHFT                                       0x1e
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_ECS_EN_BMSK                                 0x20000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_ECS_EN_SHFT                                       0x1d
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_ADCX_3_EN_BMSK                              0x10000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_ADCX_3_EN_SHFT                                    0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_ADCX_2_EN_BMSK                               0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_ADCX_2_EN_SHFT                                    0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_WCDMA_EN_BMSK                          0x6000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_WCDMA_EN_SHFT                               0x19
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_TDSCDMA_EN_BMSK                        0x1800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_TDSCDMA_EN_SHFT                             0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_UL_CA_BMSK                          0x400000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_UL_CA_SHFT                              0x16
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_EN_BMSK                             0x300000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_EN_SHFT                                 0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_ABOVE_CAT2_EN_BMSK                   0xc0000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_ABOVE_CAT2_EN_SHFT                      0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_ABOVE_CAT1_EN_BMSK                   0x30000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_ABOVE_CAT1_EN_SHFT                      0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_60MHZ_BMSK                            0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_60MHZ_SHFT                               0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_40MHZ_BMSK                            0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_40MHZ_SHFT                               0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_MIMO_EN_BMSK                         0x3000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_MIMO_EN_SHFT                            0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_EN_BMSK                               0xc00
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_EN_SHFT                                 0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_DC_EN_BMSK                            0x300
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_DC_EN_SHFT                              0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_GENRAN_EN_BMSK                              0xc0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_GENRAN_EN_SHFT                               0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_EN_BMSK                                     0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_EN_SHFT                                      0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_DSDA_EN_BMSK                                0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_DSDA_EN_SHFT                                 0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_DO_EN_BMSK                                   0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_DO_EN_SHFT                                   0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_1X_EN_BMSK                                   0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_1X_EN_SHFT                                   0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000044)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                 0x80000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                       0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MSMC_MDSP_FW_EN_BMSK                             0x7fff8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MSMC_MDSP_FW_EN_SHFT                                    0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MSMC_SPARE_BMSK                                      0x7000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MSMC_SPARE_SHFT                                         0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MSMC_MODEM_VU_EN_BMSK                                 0xfff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MSMC_MODEM_VU_EN_SHFT                                   0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000048)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ACC_DISABLE_BMSK                                 0x80000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ACC_DISABLE_SHFT                                       0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SHADOW_READ_DISABLE_BMSK                         0x40000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SHADOW_READ_DISABLE_SHFT                               0x1e
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SPARE1_BMSK                                      0x30000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SPARE1_SHFT                                            0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_FUSE_SMT_PERM_ENABLE_BMSK                         0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_FUSE_SMT_PERM_ENABLE_SHFT                              0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_FUSE_RCP_BYPASS_ENABLE_BMSK                       0x4000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_FUSE_RCP_BYPASS_ENABLE_SHFT                            0x1a
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_MSMC_SMMU_BYPASS_DISABLE_BMSK                     0x2000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_MSMC_SMMU_BYPASS_DISABLE_SHFT                          0x19
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_MSMC_GPIO_SLIMBUS_PD_DISABLE_BMSK                 0x1000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_MSMC_GPIO_SLIMBUS_PD_DISABLE_SHFT                      0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_FUSE_PCIE20_RC_DISABLE_BMSK                        0x800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_FUSE_PCIE20_RC_DISABLE_SHFT                            0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_MSMC_PCIE_DISABLE_BMSK                             0x400000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_MSMC_PCIE_DISABLE_SHFT                                 0x16
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_NDINT_DISABLE_BMSK                                 0x200000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_NDINT_DISABLE_SHFT                                     0x15
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_BMSK                       0x100000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_SHFT                           0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_BMSK                         0x80000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_SHFT                            0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_BMSK                          0x40000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_SHFT                             0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_BMSK                           0x20000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_SHFT                              0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_BMSK                           0x10000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_SHFT                              0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_BMSK                         0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_SHFT                            0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_BMSK                          0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_SHFT                             0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_BMSK                           0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_SHFT                              0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_BMSK                           0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_SHFT                              0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SPARE1_DISABLE_BMSK                                0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SPARE1_DISABLE_SHFT                                  0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SPARE0_DISABLE_BMSK                                0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SPARE0_DISABLE_SHFT                                  0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_BMSK                         0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_SHFT                           0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_BMSK                             0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_SHFT                               0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_WCSS_NIDEN_DISABLE_BMSK                         0x80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_WCSS_NIDEN_DISABLE_SHFT                          0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_BMSK                              0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_SHFT                               0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_BMSK                             0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_SHFT                              0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_BMSK                              0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_SHFT                               0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_BMSK                               0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_SHFT                               0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QDI_SPMI_DISABLE_BMSK                                   0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QDI_SPMI_DISABLE_SHFT                                   0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_BMSK                                    0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_SHFT                                    0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_BMSK                                        0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_SHFT                                        0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000004c)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_BMSK                      0xf8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_SHFT                            0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_BMSK                     0x4000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_SHFT                          0x1a
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_BMSK                            0x3ffc000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_SHFT                                  0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SPARE1_BMSK                                          0x3c00
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SPARE1_SHFT                                             0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_MODEM_PBL_PATCH_VERSION_BMSK                          0x3e0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_MODEM_PBL_PATCH_VERSION_SHFT                            0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_BMSK                            0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_SHFT                             0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000050)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_BMSK            0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_SHFT                   0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000054)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE3_BMSK                                      0xf8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE3_SHFT                                            0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_MODEM_PBL_PLL_CTRL_BMSK                           0x7800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_MODEM_PBL_PLL_CTRL_SHFT                                0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_BMSK                             0x780000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_SHFT                                 0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_CFG_BMSK                              0x40000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_CFG_SHFT                                 0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_DELAY_BMSK                            0x30000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_DELAY_SHFT                               0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE1_BMSK                                          0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE1_SHFT                                             0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_USB_SS_ENABLE_BMSK                                   0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_USB_SS_ENABLE_SHFT                                      0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_BMSK                             0x3000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_SHFT                                0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_ROM_BMSK                               0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_ROM_SHFT                                 0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_MSA_ENA_BMSK                                          0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_MSA_ENA_SHFT                                            0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_FORCE_MSA_AUTH_EN_BMSK                                0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_FORCE_MSA_AUTH_EN_SHFT                                  0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE_8_7_BMSK                                        0x180
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE_8_7_SHFT                                          0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_MODEM_BOOT_FROM_ROM_BMSK                               0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_MODEM_BOOT_FROM_ROM_SHFT                                0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_BMSK                            0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_SHFT                             0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_BMSK                 0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_SHFT                  0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000058 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                  0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                           3
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                        0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                               0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000005c + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                  0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                           3
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                        0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                               0x0

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000078 + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                  0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                           3
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                        0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                               0x0

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000007c + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                  0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                           3
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                        0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                               0x0

#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000098)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT4_BMSK                                  0xff000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT4_SHFT                                        0x18
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT3_BMSK                                    0xff0000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT3_SHFT                                        0x10
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT2_BMSK                                      0xff00
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT2_SHFT                                         0x8
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT1_BMSK                                        0xff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT1_SHFT                                         0x0

#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000009c)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SPARE0_BMSK                                     0x80000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SPARE0_SHFT                                           0x1f
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_FEC_VALUE_BMSK                                  0x7f000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_FEC_VALUE_SHFT                                        0x18
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT7_BMSK                                    0xff0000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT7_SHFT                                        0x10
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT6_BMSK                                      0xff00
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT6_SHFT                                         0x8
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT5_BMSK                                        0xff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT5_SHFT                                         0x0

#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000a0)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT4_BMSK                                   0xff000000
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT4_SHFT                                         0x18
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT3_BMSK                                     0xff0000
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT3_SHFT                                         0x10
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT2_BMSK                                       0xff00
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT2_SHFT                                          0x8
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT1_BMSK                                         0xff
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT1_SHFT                                          0x0

#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000a4)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_SPARE0_BMSK                                      0x80000000
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_SPARE0_SHFT                                            0x1f
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_FEC_VALUE_BMSK                                   0x7f000000
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_FEC_VALUE_SHFT                                         0x18
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT7_BMSK                                     0xff0000
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT7_SHFT                                         0x10
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT6_BMSK                                       0xff00
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT6_SHFT                                          0x8
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT5_BMSK                                         0xff
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT5_SHFT                                          0x0

#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000a8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_MAXn                                                          3
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_HASH_DATA0_BMSK                                      0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_HASH_DATA0_SHFT                                             0x0

#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000ac + 0x8 * (n))
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_MAXn                                                          3
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_SPARE0_BMSK                                          0x80000000
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_SPARE0_SHFT                                                0x1f
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_FEC_VALUE_BMSK                                       0x7f000000
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_FEC_VALUE_SHFT                                             0x18
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_HASH_DATA1_BMSK                                        0xffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_HASH_DATA1_SHFT                                             0x0

#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000c8)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_ADDR, HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_RMSK)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_IN)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_HASH_DATA0_BMSK                                      0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_HASH_DATA0_SHFT                                             0x0

#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000cc)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_RMSK                                                 0xff000000
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_SPARE0_BMSK                                          0x80000000
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_SPARE0_SHFT                                                0x1f
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_FEC_VALUE_BMSK                                       0x7f000000
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_FEC_VALUE_SHFT                                             0x18

#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000d0)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS1_POINT1_0_BMSK                                   0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS1_POINT1_0_SHFT                                         0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS0_POINT1_BMSK                                     0x7f800000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS0_POINT1_SHFT                                           0x17
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS_BASE1_BMSK                                         0x7fe000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS_BASE1_SHFT                                              0xd
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS_BASE0_BMSK                                           0x1ff8
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS_BASE0_SHFT                                              0x3
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS_CAL_SEL_BMSK                                            0x7
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS_CAL_SEL_SHFT                                            0x0

#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000d4)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RMSK                                                   0x7fffffff
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_FEC_BMSK                                               0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_FEC_SHFT                                                     0x18
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_SPARE_BMSK                                               0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_SPARE_SHFT                                                   0x17
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TSENS3_POINT1_BMSK                                       0x7f8000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TSENS3_POINT1_SHFT                                            0xf
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TSENS2_POINT1_BMSK                                         0x7f80
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TSENS2_POINT1_SHFT                                            0x7
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TSENS1_POINT1_1_7_BMSK                                       0x7f
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TSENS1_POINT1_1_7_SHFT                                        0x0

#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000d8)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_SVS2_TARGET_BMSK                                  0xfc000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_SVS2_TARGET_SHFT                                        0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_SVS_TARGET_BMSK                                    0x3f00000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_SVS_TARGET_SHFT                                         0x14
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_NOMINAL_TARGET_BMSK                                  0xfc000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_NOMINAL_TARGET_SHFT                                      0xe
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TURBO_TARGET_BMSK                                     0x3f00
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TURBO_TARGET_SHFT                                        0x8
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_TSENS4_POINT1_BMSK                                           0xff
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_TSENS4_POINT1_SHFT                                            0x0

#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000dc)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RMSK                                                   0x7fffffff
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_FEC_BMSK                                               0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_FEC_SHFT                                                     0x18
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR1_SVS2_TARGET_BMSK                                    0xfc0000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR1_SVS2_TARGET_SHFT                                        0x12
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR1_SVS_TARGET_BMSK                                      0x3f000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR1_SVS_TARGET_SHFT                                          0xc
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR1_NOMINAL_TARGET_BMSK                                    0xfc0
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR1_NOMINAL_TARGET_SHFT                                      0x6
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR1_TURBO_TARGET_BMSK                                       0x3f
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR1_TURBO_TARGET_SHFT                                        0x0

#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000e0)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_LDO_TARGET1_3_0_BMSK                                   0xf0000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_LDO_TARGET1_3_0_SHFT                                         0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_LDO_TARGET0_BMSK                                        0xfc00000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_LDO_TARGET0_SHFT                                             0x16
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_LDO_ENABLE_BMSK                                          0x200000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_LDO_ENABLE_SHFT                                              0x15
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR2_MODE_EXPANSION_BMSK                                 0x1f8000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR2_MODE_EXPANSION_SHFT                                      0xf
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR2_NOMINAL_TARGET_BMSK                                   0x7e00
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR2_NOMINAL_TARGET_SHFT                                      0x9
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR2_TURBO_TARGET_BMSK                                      0x1f8
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR2_TURBO_TARGET_SHFT                                        0x3
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR2_ENABLE_BMSK                                              0x4
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR2_ENABLE_SHFT                                              0x2
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR1_ENABLE_BMSK                                              0x2
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR1_ENABLE_SHFT                                              0x1
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR0_ENABLE_BMSK                                              0x1
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR0_ENABLE_SHFT                                              0x0

#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000e4)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RMSK                                                   0x7fffffff
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_FEC_BMSK                                               0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_FEC_SHFT                                                     0x18
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_SPARE_BMSK                                               0xfffffc
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_SPARE_SHFT                                                    0x2
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_LDO_TARGET1_5_4_BMSK                                          0x3
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_LDO_TARGET1_5_4_SHFT                                          0x0

#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000e8)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW3_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_BHS_SLOPE_0_BMSK                                   0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_BHS_SLOPE_0_SHFT                                         0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_BHS_INTERCEPT_BMSK                                 0x7fe00000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_BHS_INTERCEPT_SHFT                                       0x15
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_LDO_SLOPE_BMSK                                       0x1ff800
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_LDO_SLOPE_SHFT                                            0xb
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_LDO_INTERCEPT_BMSK                                      0x7fe
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_LDO_INTERCEPT_SHFT                                        0x1
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_ENABLE_BMSK                                               0x1
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_ENABLE_SHFT                                               0x0

#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000ec)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RMSK                                                   0x7fffffff
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW3_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_FEC_BMSK                                               0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_FEC_SHFT                                                     0x18
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_Q6SS0_LDO_VREF_TRIM_BMSK                                 0xf80000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_Q6SS0_LDO_VREF_TRIM_SHFT                                     0x13
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_SPARE_BMSK                                                0x60000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_SPARE_SHFT                                                   0x11
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ECS_MISC_BMSK                                             0x1fe00
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ECS_MISC_SHFT                                                 0x9
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ECS_BHS_SLOPE_9_1_BMSK                                      0x1ff
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ECS_BHS_SLOPE_9_1_SHFT                                        0x0

#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000f0)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW4_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW4_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B1M2_1_0_BMSK                                       0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B1M2_1_0_SHFT                                             0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B1M1_BMSK                                           0x38000000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B1M1_SHFT                                                 0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B1M0_BMSK                                            0x7000000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B1M0_SHFT                                                 0x18
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_G_B1_BMSK                                                0xe00000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_G_B1_SHFT                                                    0x15
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_SAR_LDO_ERR0_BMSK                                        0x180000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_SAR_LDO_ERR0_SHFT                                            0x13
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_CLK_LDO_ERR0_BMSK                                         0x60000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_CLK_LDO_ERR0_SHFT                                            0x11
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_VREF_ERR_B0_BMSK                                          0x18000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_VREF_ERR_B0_SHFT                                              0xf
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B0M3_BMSK                                               0x7000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B0M3_SHFT                                                  0xc
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B0M2_BMSK                                                0xe00
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B0M2_SHFT                                                  0x9
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B0M1_BMSK                                                0x1c0
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B0M1_SHFT                                                  0x6
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B0M0_BMSK                                                 0x38
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B0M0_SHFT                                                  0x3
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_G_B0_BMSK                                                     0x7
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_G_B0_SHFT                                                     0x0

#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000f4)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_RMSK                                                   0x7fffffff
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_FEC_BMSK                                               0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_FEC_SHFT                                                     0x18
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B2M3_1_0_BMSK                                         0xc00000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B2M3_1_0_SHFT                                             0x16
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B2M2_BMSK                                             0x380000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B2M2_SHFT                                                 0x13
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B2M1_BMSK                                              0x70000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B2M1_SHFT                                                 0x10
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B2M0_BMSK                                               0xe000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B2M0_SHFT                                                  0xd
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_G_B2_BMSK                                                  0x1c00
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_G_B2_SHFT                                                     0xa
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_SAR_LDO_ERR1_BMSK                                           0x300
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_SAR_LDO_ERR1_SHFT                                             0x8
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_CLK_LDO_ERR1_BMSK                                            0xc0
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_CLK_LDO_ERR1_SHFT                                             0x6
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_VREF_ERR_B1_BMSK                                             0x30
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_VREF_ERR_B1_SHFT                                              0x4
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B1M3_BMSK                                                  0xe
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B1M3_SHFT                                                  0x1
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B1M2_2_BMSK                                                0x1
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B1M2_2_SHFT                                                0x0

#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000f8)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW5_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW5_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B4M0_0_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B4M0_0_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_G_B4_BMSK                                              0x70000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_G_B4_SHFT                                                    0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_SAR_LDO_ERR3_BMSK                                       0xc000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_SAR_LDO_ERR3_SHFT                                            0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CLK_LDO_ERR3_BMSK                                       0x3000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CLK_LDO_ERR3_SHFT                                            0x18
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_VREF_ERR_B3_BMSK                                         0xc00000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_VREF_ERR_B3_SHFT                                             0x16
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B3M3_BMSK                                             0x380000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B3M3_SHFT                                                 0x13
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B3M2_BMSK                                              0x70000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B3M2_SHFT                                                 0x10
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B3M1_BMSK                                               0xe000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B3M1_SHFT                                                  0xd
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B3M0_BMSK                                               0x1c00
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B3M0_SHFT                                                  0xa
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_G_B3_BMSK                                                   0x380
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_G_B3_SHFT                                                     0x7
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_SAR_LDO_ERR2_BMSK                                            0x60
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_SAR_LDO_ERR2_SHFT                                             0x5
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CLK_LDO_ERR2_BMSK                                            0x18
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CLK_LDO_ERR2_SHFT                                             0x3
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_VREF_ERR_B2_BMSK                                              0x6
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_VREF_ERR_B2_SHFT                                              0x1
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B2M3_2_BMSK                                                0x1
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B2M3_2_SHFT                                                0x0

#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000fc)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RMSK                                                   0x7fffffff
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW5_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_FEC_BMSK                                               0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_FEC_SHFT                                                     0x18
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B5M1_0_BMSK                                           0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B5M1_0_SHFT                                               0x17
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B5M0_BMSK                                             0x700000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B5M0_SHFT                                                 0x14
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_G_B5_BMSK                                                 0xe0000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_G_B5_SHFT                                                    0x11
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_SAR_LDO_ERR4_BMSK                                         0x18000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_SAR_LDO_ERR4_SHFT                                             0xf
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CLK_LDO_ERR4_BMSK                                          0x6000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CLK_LDO_ERR4_SHFT                                             0xd
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_VREF_ERR_B4_BMSK                                           0x1800
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_VREF_ERR_B4_SHFT                                              0xb
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B4M3_BMSK                                                0x700
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B4M3_SHFT                                                  0x8
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B4M2_BMSK                                                 0xe0
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B4M2_SHFT                                                  0x5
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B4M1_BMSK                                                 0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B4M1_SHFT                                                  0x2
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B4M0_2_1_BMSK                                              0x3
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B4M0_2_1_SHFT                                              0x0

#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000100)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW6_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CLK_LDO_ERR6_0_BMSK                                    0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CLK_LDO_ERR6_0_SHFT                                          0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_VREF_ERR_B6_BMSK                                       0x60000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_VREF_ERR_B6_SHFT                                             0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B6M3_BMSK                                           0x1c000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B6M3_SHFT                                                 0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B6M2_BMSK                                            0x3800000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B6M2_SHFT                                                 0x17
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B6M1_BMSK                                             0x700000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B6M1_SHFT                                                 0x14
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B6M0_BMSK                                              0xe0000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B6M0_SHFT                                                 0x11
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_G_B6_BMSK                                                 0x1c000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_G_B6_SHFT                                                     0xe
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_SAR_LDO_ERR5_BMSK                                          0x3000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_SAR_LDO_ERR5_SHFT                                             0xc
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CLK_LDO_ERR5_BMSK                                           0xc00
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CLK_LDO_ERR5_SHFT                                             0xa
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_VREF_ERR_B5_BMSK                                            0x300
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_VREF_ERR_B5_SHFT                                              0x8
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B5M3_BMSK                                                 0xe0
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B5M3_SHFT                                                  0x5
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B5M2_BMSK                                                 0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B5M2_SHFT                                                  0x2
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B5M1_2_1_BMSK                                              0x3
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B5M1_2_1_SHFT                                              0x0

#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000104)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RMSK                                                   0x7fffffff
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW6_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_FEC_BMSK                                               0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_FEC_SHFT                                                     0x18
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_BMSK                                  0xff0000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_SHFT                                      0x10
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_BMSK                                    0xff00
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_SHFT                                       0x8
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_AVG_ERROR_BMSK                            0x80
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_AVG_ERROR_SHFT                             0x7
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_AVG_ERROR_BMSK                            0x40
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_AVG_ERROR_SHFT                             0x6
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC_0_1_FUSEFLAG_BMSK                                0x20
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC_0_1_FUSEFLAG_SHFT                                 0x5
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_OVERFLOW_BMSK                             0x10
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_OVERFLOW_SHFT                              0x4
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_OVERFLOW_BMSK                              0x8
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_OVERFLOW_SHFT                              0x3
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_SAR_LDO_ERR6_BMSK                                             0x6
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_SAR_LDO_ERR6_SHFT                                             0x1
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_CLK_LDO_ERR6_1_BMSK                                           0x1
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_CLK_LDO_ERR6_1_SHFT                                           0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_ADDR(n)                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000108 + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_MAXn                                                      15
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_REDUN_DATA_BMSK                                   0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_REDUN_DATA_SHFT                                          0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n)                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000010c + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_MAXn                                                      15
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_SPARE0_BMSK                                       0x80000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_SPARE0_SHFT                                             0x1f
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_FEC_VALUE_BMSK                                    0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_FEC_VALUE_SHFT                                          0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_REDUN_DATA_BMSK                                     0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_REDUN_DATA_SHFT                                          0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000188)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_MEM_ACCEL_COMPILER_BMSK                          0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_MEM_ACCEL_COMPILER_SHFT                                 0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000018c)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_RMSK                                             0x7fffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_FEC_VALUE_BMSK                                   0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_FEC_VALUE_SHFT                                         0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_SPARE0_BMSK                                        0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_SPARE0_SHFT                                             0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000190)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_MEM_ACCEL_CUSTOM_BMSK                            0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_MEM_ACCEL_CUSTOM_SHFT                                   0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000194)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_RMSK                                             0x7fffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_FEC_VALUE_BMSK                                   0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_FEC_VALUE_SHFT                                         0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_SPARE0_BMSK                                        0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_SPARE0_SHFT                                             0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000198)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_MEM_ACCEL_CUSTOM_BMSK                            0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_MEM_ACCEL_CUSTOM_SHFT                                   0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000019c)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_RMSK                                             0x7fffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_FEC_VALUE_BMSK                                   0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_FEC_VALUE_SHFT                                         0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_SPARE0_BMSK                                        0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_SPARE0_SHFT                                             0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001a0)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_MEM_ACCEL_COMPILER_BMSK                          0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_MEM_ACCEL_COMPILER_SHFT                                 0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001a4)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_RMSK                                             0x7fffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_FEC_VALUE_BMSK                                   0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_FEC_VALUE_SHFT                                         0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_SPARE0_BMSK                                        0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_SPARE0_SHFT                                             0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001a8)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_MEM_ACCEL_CUSTOM_BMSK                            0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_MEM_ACCEL_CUSTOM_SHFT                                   0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001ac)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_RMSK                                             0x7fffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_FEC_VALUE_BMSK                                   0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_FEC_VALUE_SHFT                                         0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_SPARE0_BMSK                                        0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_SPARE0_SHFT                                             0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001b0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_MAXn                                                       31
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_PATCH_DATA_BMSK                                    0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_PATCH_DATA_SHFT                                           0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001b4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_MAXn                                                       31
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_SPARE0_BMSK                                        0x80000000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_SPARE0_SHFT                                              0x1f
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_FEC_VALUE_BMSK                                     0x7f000000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_FEC_VALUE_SHFT                                           0x18
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_SPARE3_BMSK                                          0xfe0000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_SPARE3_SHFT                                              0x11
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_ADDR_BMSK                                       0x1fffe
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_ADDR_SHFT                                           0x1
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_EN_BMSK                                             0x1
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_EN_SHFT                                             0x0

#define HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002b0)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR, HWIO_QFPROM_RAW_FEC_EN_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEC_EN_LSB_IN)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION31_FEC_EN_BMSK                                       0x80000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION31_FEC_EN_SHFT                                             0x1f
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION30_FEC_EN_BMSK                                       0x40000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION30_FEC_EN_SHFT                                             0x1e
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION29_FEC_EN_BMSK                                       0x20000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION29_FEC_EN_SHFT                                             0x1d
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION28_FEC_EN_BMSK                                       0x10000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION28_FEC_EN_SHFT                                             0x1c
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION27_FEC_EN_BMSK                                        0x8000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION27_FEC_EN_SHFT                                             0x1b
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION26_FEC_EN_BMSK                                        0x4000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION26_FEC_EN_SHFT                                             0x1a
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION25_FEC_EN_BMSK                                        0x2000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION25_FEC_EN_SHFT                                             0x19
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION24_FEC_EN_BMSK                                        0x1000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION24_FEC_EN_SHFT                                             0x18
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION23_FEC_EN_BMSK                                         0x800000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION23_FEC_EN_SHFT                                             0x17
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION22_FEC_EN_BMSK                                         0x400000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION22_FEC_EN_SHFT                                             0x16
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION21_FEC_EN_BMSK                                         0x200000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION21_FEC_EN_SHFT                                             0x15
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION20_FEC_EN_BMSK                                         0x100000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION20_FEC_EN_SHFT                                             0x14
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION19_FEC_EN_BMSK                                          0x80000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION19_FEC_EN_SHFT                                             0x13
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION18_FEC_EN_BMSK                                          0x40000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION18_FEC_EN_SHFT                                             0x12
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION17_FEC_EN_BMSK                                          0x20000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION17_FEC_EN_SHFT                                             0x11
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION16_FEC_EN_BMSK                                          0x10000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION16_FEC_EN_SHFT                                             0x10
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION15_FEC_EN_BMSK                                           0x8000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION15_FEC_EN_SHFT                                              0xf
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION14_FEC_EN_BMSK                                           0x4000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION14_FEC_EN_SHFT                                              0xe
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION13_FEC_EN_BMSK                                           0x2000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION13_FEC_EN_SHFT                                              0xd
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION12_FEC_EN_BMSK                                           0x1000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION12_FEC_EN_SHFT                                              0xc
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION11_FEC_EN_BMSK                                            0x800
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION11_FEC_EN_SHFT                                              0xb
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION10_FEC_EN_BMSK                                            0x400
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION10_FEC_EN_SHFT                                              0xa
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION9_FEC_EN_BMSK                                             0x200
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION9_FEC_EN_SHFT                                               0x9
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION8_FEC_EN_BMSK                                             0x100
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION8_FEC_EN_SHFT                                               0x8
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION7_FEC_EN_BMSK                                              0x80
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION7_FEC_EN_SHFT                                               0x7
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION6_FEC_EN_BMSK                                              0x40
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION6_FEC_EN_SHFT                                               0x6
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION5_FEC_EN_BMSK                                              0x20
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION5_FEC_EN_SHFT                                               0x5
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION4_FEC_EN_BMSK                                              0x10
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION4_FEC_EN_SHFT                                               0x4
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION3_FEC_EN_BMSK                                               0x8
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION3_FEC_EN_SHFT                                               0x3
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION2_FEC_EN_BMSK                                               0x4
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION2_FEC_EN_SHFT                                               0x2
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION1_FEC_EN_BMSK                                               0x2
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION1_FEC_EN_SHFT                                               0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION0_FEC_EN_BMSK                                               0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION0_FEC_EN_SHFT                                               0x0

#define HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002b4)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR, HWIO_QFPROM_RAW_FEC_EN_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEC_EN_MSB_IN)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_FEC_EN_REDUNDANCY_BMSK                                     0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_MSB_FEC_EN_REDUNDANCY_SHFT                                            0x0

#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002b8)
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG16_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG16_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG16_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG16_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG16_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG16_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002bc)
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG16_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG16_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG16_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG16_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG16_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG16_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_SPARE0_BMSK                                           0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_SPARE0_SHFT                                                 0x1f
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_FEC_VALUE_BMSK                                        0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_FEC_VALUE_SHFT                                              0x18
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_SPARE1_BMSK                                             0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002c0)
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG17_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG17_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG17_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG17_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG17_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG17_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002c4)
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG17_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG17_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG17_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG17_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG17_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG17_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_SPARE0_BMSK                                           0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_SPARE0_SHFT                                                 0x1f
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_FEC_VALUE_BMSK                                        0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_FEC_VALUE_SHFT                                              0x18
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_SPARE1_BMSK                                             0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002c8)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG18_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG18_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002cc)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG18_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG18_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG18_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG18_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG18_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG18_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_SPARE0_BMSK                                           0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_SPARE0_SHFT                                                 0x1f
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_FEC_VALUE_BMSK                                        0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_FEC_VALUE_SHFT                                              0x18
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_SPARE1_BMSK                                             0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002d0)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG19_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG19_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002d4)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG19_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG19_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_SPARE0_BMSK                                           0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_SPARE0_SHFT                                                 0x1f
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_FEC_VALUE_BMSK                                        0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_FEC_VALUE_SHFT                                              0x18
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_SPARE1_BMSK                                             0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002d8)
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG20_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG20_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG20_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG20_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG20_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG20_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002dc)
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG20_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG20_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG20_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG20_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG20_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG20_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_SPARE0_BMSK                                           0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_SPARE0_SHFT                                                 0x1f
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_FEC_VALUE_BMSK                                        0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_FEC_VALUE_SHFT                                              0x18
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_SPARE1_BMSK                                             0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002e0)
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG21_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG21_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG21_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG21_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG21_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG21_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002e4)
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG21_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG21_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG21_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG21_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG21_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG21_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_SPARE0_BMSK                                           0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_SPARE0_SHFT                                                 0x1f
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_FEC_VALUE_BMSK                                        0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_FEC_VALUE_SHFT                                              0x18
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_SPARE1_BMSK                                             0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_ACC_PRIVATEn_ADDR(n)                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000400 + 0x4 * (n))
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_MAXn                                                             39
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ACC_PRIVATEn_ADDR(n), HWIO_QFPROM_RAW_ACC_PRIVATEn_RMSK)
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ACC_PRIVATEn_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_ACC_PRIVATE_BMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_ACC_PRIVATE_SHFT                                                0x0

#define HWIO_ACC_IR_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002000)
#define HWIO_ACC_IR_RMSK                                                                            0x1f
#define HWIO_ACC_IR_OUT(v)      \
        out_dword(HWIO_ACC_IR_ADDR,v)
#define HWIO_ACC_IR_INSTRUCTION_BMSK                                                                0x1f
#define HWIO_ACC_IR_INSTRUCTION_SHFT                                                                 0x0

#define HWIO_ACC_DR_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002004)
#define HWIO_ACC_DR_RMSK                                                                      0xffffffff
#define HWIO_ACC_DR_IN          \
        in_dword_masked(HWIO_ACC_DR_ADDR, HWIO_ACC_DR_RMSK)
#define HWIO_ACC_DR_INM(m)      \
        in_dword_masked(HWIO_ACC_DR_ADDR, m)
#define HWIO_ACC_DR_OUT(v)      \
        out_dword(HWIO_ACC_DR_ADDR,v)
#define HWIO_ACC_DR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_ACC_DR_ADDR,m,v,HWIO_ACC_DR_IN)
#define HWIO_ACC_DR_DR_BMSK                                                                   0xffffffff
#define HWIO_ACC_DR_DR_SHFT                                                                          0x0

#define HWIO_ACC_VERID_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002008)
#define HWIO_ACC_VERID_RMSK                                                                       0xffff
#define HWIO_ACC_VERID_IN          \
        in_dword_masked(HWIO_ACC_VERID_ADDR, HWIO_ACC_VERID_RMSK)
#define HWIO_ACC_VERID_INM(m)      \
        in_dword_masked(HWIO_ACC_VERID_ADDR, m)
#define HWIO_ACC_VERID_FWVERID_BMSK                                                               0xff00
#define HWIO_ACC_VERID_FWVERID_SHFT                                                                  0x8
#define HWIO_ACC_VERID_HWVERID_BMSK                                                                 0xff
#define HWIO_ACC_VERID_HWVERID_SHFT                                                                  0x0

#define HWIO_ACC_FEATSETn_ADDR(n)                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002010 + 0x4 * (n))
#define HWIO_ACC_FEATSETn_RMSK                                                                0xffffffff
#define HWIO_ACC_FEATSETn_MAXn                                                                         7
#define HWIO_ACC_FEATSETn_INI(n)        \
        in_dword_masked(HWIO_ACC_FEATSETn_ADDR(n), HWIO_ACC_FEATSETn_RMSK)
#define HWIO_ACC_FEATSETn_INMI(n,mask)    \
        in_dword_masked(HWIO_ACC_FEATSETn_ADDR(n), mask)
#define HWIO_ACC_FEATSETn_FEAT_BMSK                                                           0xffffffff
#define HWIO_ACC_FEATSETn_FEAT_SHFT                                                                  0x0

#define HWIO_ACC_STATE_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002038)
#define HWIO_ACC_STATE_RMSK                                                                          0x7
#define HWIO_ACC_STATE_IN          \
        in_dword_masked(HWIO_ACC_STATE_ADDR, HWIO_ACC_STATE_RMSK)
#define HWIO_ACC_STATE_INM(m)      \
        in_dword_masked(HWIO_ACC_STATE_ADDR, m)
#define HWIO_ACC_STATE_ACC_READY_BMSK                                                                0x4
#define HWIO_ACC_STATE_ACC_READY_SHFT                                                                0x2
#define HWIO_ACC_STATE_ACC_LOCKED_BMSK                                                               0x2
#define HWIO_ACC_STATE_ACC_LOCKED_SHFT                                                               0x1
#define HWIO_ACC_STATE_ACC_STOP_BMSK                                                                 0x1
#define HWIO_ACC_STATE_ACC_STOP_SHFT                                                                 0x0

#define HWIO_QFPROM_BLOW_TIMER_ADDR                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000203c)
#define HWIO_QFPROM_BLOW_TIMER_RMSK                                                                0xfff
#define HWIO_QFPROM_BLOW_TIMER_IN          \
        in_dword_masked(HWIO_QFPROM_BLOW_TIMER_ADDR, HWIO_QFPROM_BLOW_TIMER_RMSK)
#define HWIO_QFPROM_BLOW_TIMER_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BLOW_TIMER_ADDR, m)
#define HWIO_QFPROM_BLOW_TIMER_OUT(v)      \
        out_dword(HWIO_QFPROM_BLOW_TIMER_ADDR,v)
#define HWIO_QFPROM_BLOW_TIMER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_BLOW_TIMER_ADDR,m,v,HWIO_QFPROM_BLOW_TIMER_IN)
#define HWIO_QFPROM_BLOW_TIMER_BLOW_TIMER_BMSK                                                     0xfff
#define HWIO_QFPROM_BLOW_TIMER_BLOW_TIMER_SHFT                                                       0x0

#define HWIO_QFPROM_TEST_CTRL_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002040)
#define HWIO_QFPROM_TEST_CTRL_RMSK                                                                   0xf
#define HWIO_QFPROM_TEST_CTRL_IN          \
        in_dword_masked(HWIO_QFPROM_TEST_CTRL_ADDR, HWIO_QFPROM_TEST_CTRL_RMSK)
#define HWIO_QFPROM_TEST_CTRL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_TEST_CTRL_ADDR, m)
#define HWIO_QFPROM_TEST_CTRL_OUT(v)      \
        out_dword(HWIO_QFPROM_TEST_CTRL_ADDR,v)
#define HWIO_QFPROM_TEST_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_TEST_CTRL_ADDR,m,v,HWIO_QFPROM_TEST_CTRL_IN)
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_ROM_BMSK                                                       0x8
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_ROM_SHFT                                                       0x3
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_WL_BMSK                                                        0x4
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_WL_SHFT                                                        0x2
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_BL_BMSK                                                        0x2
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_BL_SHFT                                                        0x1
#define HWIO_QFPROM_TEST_CTRL_EN_FUSE_RES_MEAS_BMSK                                                  0x1
#define HWIO_QFPROM_TEST_CTRL_EN_FUSE_RES_MEAS_SHFT                                                  0x0

#define HWIO_QFPROM_ACCEL_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002044)
#define HWIO_QFPROM_ACCEL_RMSK                                                                     0xfff
#define HWIO_QFPROM_ACCEL_IN          \
        in_dword_masked(HWIO_QFPROM_ACCEL_ADDR, HWIO_QFPROM_ACCEL_RMSK)
#define HWIO_QFPROM_ACCEL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_ACCEL_ADDR, m)
#define HWIO_QFPROM_ACCEL_OUT(v)      \
        out_dword(HWIO_QFPROM_ACCEL_ADDR,v)
#define HWIO_QFPROM_ACCEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_ACCEL_ADDR,m,v,HWIO_QFPROM_ACCEL_IN)
#define HWIO_QFPROM_ACCEL_QFPROM_GATELAST_BMSK                                                     0x800
#define HWIO_QFPROM_ACCEL_QFPROM_GATELAST_SHFT                                                       0xb
#define HWIO_QFPROM_ACCEL_QFPROM_TRIPPT_SEL_BMSK                                                   0x700
#define HWIO_QFPROM_ACCEL_QFPROM_TRIPPT_SEL_SHFT                                                     0x8
#define HWIO_QFPROM_ACCEL_QFPROM_ACCEL_BMSK                                                         0xff
#define HWIO_QFPROM_ACCEL_QFPROM_ACCEL_SHFT                                                          0x0

#define HWIO_QFPROM_BLOW_STATUS_ADDR                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002048)
#define HWIO_QFPROM_BLOW_STATUS_RMSK                                                                 0x3
#define HWIO_QFPROM_BLOW_STATUS_IN          \
        in_dword_masked(HWIO_QFPROM_BLOW_STATUS_ADDR, HWIO_QFPROM_BLOW_STATUS_RMSK)
#define HWIO_QFPROM_BLOW_STATUS_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BLOW_STATUS_ADDR, m)
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_WR_ERR_BMSK                                                   0x2
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_WR_ERR_SHFT                                                   0x1
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_BUSY_BMSK                                                     0x1
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_BUSY_SHFT                                                     0x0

#define HWIO_QFPROM_ROM_ERROR_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000204c)
#define HWIO_QFPROM_ROM_ERROR_RMSK                                                                   0x1
#define HWIO_QFPROM_ROM_ERROR_IN          \
        in_dword_masked(HWIO_QFPROM_ROM_ERROR_ADDR, HWIO_QFPROM_ROM_ERROR_RMSK)
#define HWIO_QFPROM_ROM_ERROR_INM(m)      \
        in_dword_masked(HWIO_QFPROM_ROM_ERROR_ADDR, m)
#define HWIO_QFPROM_ROM_ERROR_ERROR_BMSK                                                             0x1
#define HWIO_QFPROM_ROM_ERROR_ERROR_SHFT                                                             0x0

#define HWIO_QFPROM_BIST_CTRL_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002050)
#define HWIO_QFPROM_BIST_CTRL_RMSK                                                                  0x7f
#define HWIO_QFPROM_BIST_CTRL_IN          \
        in_dword_masked(HWIO_QFPROM_BIST_CTRL_ADDR, HWIO_QFPROM_BIST_CTRL_RMSK)
#define HWIO_QFPROM_BIST_CTRL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BIST_CTRL_ADDR, m)
#define HWIO_QFPROM_BIST_CTRL_OUT(v)      \
        out_dword(HWIO_QFPROM_BIST_CTRL_ADDR,v)
#define HWIO_QFPROM_BIST_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_BIST_CTRL_ADDR,m,v,HWIO_QFPROM_BIST_CTRL_IN)
#define HWIO_QFPROM_BIST_CTRL_AUTH_REGION_BMSK                                                      0x7c
#define HWIO_QFPROM_BIST_CTRL_AUTH_REGION_SHFT                                                       0x2
#define HWIO_QFPROM_BIST_CTRL_SHA_ENABLE_BMSK                                                        0x2
#define HWIO_QFPROM_BIST_CTRL_SHA_ENABLE_SHFT                                                        0x1
#define HWIO_QFPROM_BIST_CTRL_START_BMSK                                                             0x1
#define HWIO_QFPROM_BIST_CTRL_START_SHFT                                                             0x0

#define HWIO_QFPROM_BIST_ERROR_ADDR                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002054)
#define HWIO_QFPROM_BIST_ERROR_RMSK                                                           0xffffffff
#define HWIO_QFPROM_BIST_ERROR_IN          \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR_ADDR, HWIO_QFPROM_BIST_ERROR_RMSK)
#define HWIO_QFPROM_BIST_ERROR_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR_ADDR, m)
#define HWIO_QFPROM_BIST_ERROR_ERROR_BMSK                                                     0xffffffff
#define HWIO_QFPROM_BIST_ERROR_ERROR_SHFT                                                            0x0

#define HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002060 + 0x4 * (n))
#define HWIO_QFPROM_HASH_SIGNATUREn_RMSK                                                      0xffffffff
#define HWIO_QFPROM_HASH_SIGNATUREn_MAXn                                                               7
#define HWIO_QFPROM_HASH_SIGNATUREn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n), HWIO_QFPROM_HASH_SIGNATUREn_RMSK)
#define HWIO_QFPROM_HASH_SIGNATUREn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n), mask)
#define HWIO_QFPROM_HASH_SIGNATUREn_HASH_VALUE_BMSK                                           0xffffffff
#define HWIO_QFPROM_HASH_SIGNATUREn_HASH_VALUE_SHFT                                                  0x0

#define HWIO_HW_KEY_STATUS_ADDR                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002080)
#define HWIO_HW_KEY_STATUS_RMSK                                                                     0x1f
#define HWIO_HW_KEY_STATUS_IN          \
        in_dword_masked(HWIO_HW_KEY_STATUS_ADDR, HWIO_HW_KEY_STATUS_RMSK)
#define HWIO_HW_KEY_STATUS_INM(m)      \
        in_dword_masked(HWIO_HW_KEY_STATUS_ADDR, m)
#define HWIO_HW_KEY_STATUS_KDF_DONE_BMSK                                                            0x10
#define HWIO_HW_KEY_STATUS_KDF_DONE_SHFT                                                             0x4
#define HWIO_HW_KEY_STATUS_MSA_KEYS_BLOCKED_BMSK                                                     0x8
#define HWIO_HW_KEY_STATUS_MSA_KEYS_BLOCKED_SHFT                                                     0x3
#define HWIO_HW_KEY_STATUS_APPS_KEYS_BLOCKED_BMSK                                                    0x4
#define HWIO_HW_KEY_STATUS_APPS_KEYS_BLOCKED_SHFT                                                    0x2
#define HWIO_HW_KEY_STATUS_SEC_KEY_DERIVATION_KEY_BLOWN_BMSK                                         0x2
#define HWIO_HW_KEY_STATUS_SEC_KEY_DERIVATION_KEY_BLOWN_SHFT                                         0x1
#define HWIO_HW_KEY_STATUS_PRI_KEY_DERIVATION_KEY_BLOWN_BMSK                                         0x1
#define HWIO_HW_KEY_STATUS_PRI_KEY_DERIVATION_KEY_BLOWN_SHFT                                         0x0

#define HWIO_RESET_JDR_STATUS_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002084)
#define HWIO_RESET_JDR_STATUS_RMSK                                                                   0x3
#define HWIO_RESET_JDR_STATUS_IN          \
        in_dword_masked(HWIO_RESET_JDR_STATUS_ADDR, HWIO_RESET_JDR_STATUS_RMSK)
#define HWIO_RESET_JDR_STATUS_INM(m)      \
        in_dword_masked(HWIO_RESET_JDR_STATUS_ADDR, m)
#define HWIO_RESET_JDR_STATUS_FORCE_RESET_BMSK                                                       0x2
#define HWIO_RESET_JDR_STATUS_FORCE_RESET_SHFT                                                       0x1
#define HWIO_RESET_JDR_STATUS_DISABLE_SYSTEM_RESET_BMSK                                              0x1
#define HWIO_RESET_JDR_STATUS_DISABLE_SYSTEM_RESET_SHFT                                              0x0

#define HWIO_FEC_ESR_ADDR                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002090)
#define HWIO_FEC_ESR_RMSK                                                                          0xfff
#define HWIO_FEC_ESR_IN          \
        in_dword_masked(HWIO_FEC_ESR_ADDR, HWIO_FEC_ESR_RMSK)
#define HWIO_FEC_ESR_INM(m)      \
        in_dword_masked(HWIO_FEC_ESR_ADDR, m)
#define HWIO_FEC_ESR_OUT(v)      \
        out_dword(HWIO_FEC_ESR_ADDR,v)
#define HWIO_FEC_ESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FEC_ESR_ADDR,m,v,HWIO_FEC_ESR_IN)
#define HWIO_FEC_ESR_CORR_SW_ACC_BMSK                                                              0x800
#define HWIO_FEC_ESR_CORR_SW_ACC_SHFT                                                                0xb
#define HWIO_FEC_ESR_CORR_BOOT_ROM_BMSK                                                            0x200
#define HWIO_FEC_ESR_CORR_BOOT_ROM_SHFT                                                              0x9
#define HWIO_FEC_ESR_CORR_FUSE_SENSE_BMSK                                                          0x100
#define HWIO_FEC_ESR_CORR_FUSE_SENSE_SHFT                                                            0x8
#define HWIO_FEC_ESR_CORR_MULT_BMSK                                                                 0x80
#define HWIO_FEC_ESR_CORR_MULT_SHFT                                                                  0x7
#define HWIO_FEC_ESR_CORR_SEEN_BMSK                                                                 0x40
#define HWIO_FEC_ESR_CORR_SEEN_SHFT                                                                  0x6
#define HWIO_FEC_ESR_ERR_SW_ACC_BMSK                                                                0x20
#define HWIO_FEC_ESR_ERR_SW_ACC_SHFT                                                                 0x5
#define HWIO_FEC_ESR_ERR_BOOT_ROM_BMSK                                                               0x8
#define HWIO_FEC_ESR_ERR_BOOT_ROM_SHFT                                                               0x3
#define HWIO_FEC_ESR_ERR_FUSE_SENSE_BMSK                                                             0x4
#define HWIO_FEC_ESR_ERR_FUSE_SENSE_SHFT                                                             0x2
#define HWIO_FEC_ESR_ERR_MULT_BMSK                                                                   0x2
#define HWIO_FEC_ESR_ERR_MULT_SHFT                                                                   0x1
#define HWIO_FEC_ESR_ERR_SEEN_BMSK                                                                   0x1
#define HWIO_FEC_ESR_ERR_SEEN_SHFT                                                                   0x0

#define HWIO_FEC_EAR_ADDR                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002094)
#define HWIO_FEC_EAR_RMSK                                                                     0xffffffff
#define HWIO_FEC_EAR_IN          \
        in_dword_masked(HWIO_FEC_EAR_ADDR, HWIO_FEC_EAR_RMSK)
#define HWIO_FEC_EAR_INM(m)      \
        in_dword_masked(HWIO_FEC_EAR_ADDR, m)
#define HWIO_FEC_EAR_CORR_ADDR_BMSK                                                           0xffff0000
#define HWIO_FEC_EAR_CORR_ADDR_SHFT                                                                 0x10
#define HWIO_FEC_EAR_ERR_ADDR_BMSK                                                                0xffff
#define HWIO_FEC_EAR_ERR_ADDR_SHFT                                                                   0x0

#define HWIO_QFPROM0_MATCH_STATUS_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002098)
#define HWIO_QFPROM0_MATCH_STATUS_RMSK                                                        0xffffffff
#define HWIO_QFPROM0_MATCH_STATUS_IN          \
        in_dword_masked(HWIO_QFPROM0_MATCH_STATUS_ADDR, HWIO_QFPROM0_MATCH_STATUS_RMSK)
#define HWIO_QFPROM0_MATCH_STATUS_INM(m)      \
        in_dword_masked(HWIO_QFPROM0_MATCH_STATUS_ADDR, m)
#define HWIO_QFPROM0_MATCH_STATUS_FLAG_BMSK                                                   0xffffffff
#define HWIO_QFPROM0_MATCH_STATUS_FLAG_SHFT                                                          0x0

#define HWIO_QFPROM1_MATCH_STATUS_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000209c)
#define HWIO_QFPROM1_MATCH_STATUS_RMSK                                                        0xffffffff
#define HWIO_QFPROM1_MATCH_STATUS_IN          \
        in_dword_masked(HWIO_QFPROM1_MATCH_STATUS_ADDR, HWIO_QFPROM1_MATCH_STATUS_RMSK)
#define HWIO_QFPROM1_MATCH_STATUS_INM(m)      \
        in_dword_masked(HWIO_QFPROM1_MATCH_STATUS_ADDR, m)
#define HWIO_QFPROM1_MATCH_STATUS_FLAG_BMSK                                                   0xffffffff
#define HWIO_QFPROM1_MATCH_STATUS_FLAG_SHFT                                                          0x0

#define HWIO_QFPROM_CORR_JTAG_ID_ADDR                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004000)
#define HWIO_QFPROM_CORR_JTAG_ID_RMSK                                                         0xffffffff
#define HWIO_QFPROM_CORR_JTAG_ID_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_ADDR, HWIO_QFPROM_CORR_JTAG_ID_RMSK)
#define HWIO_QFPROM_CORR_JTAG_ID_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_ADDR, m)
#define HWIO_QFPROM_CORR_JTAG_ID_PTE_DATA0_BMSK                                               0xf0000000
#define HWIO_QFPROM_CORR_JTAG_ID_PTE_DATA0_SHFT                                                     0x1c
#define HWIO_QFPROM_CORR_JTAG_ID_FEATURE_ID_BMSK                                               0xff00000
#define HWIO_QFPROM_CORR_JTAG_ID_FEATURE_ID_SHFT                                                    0x14
#define HWIO_QFPROM_CORR_JTAG_ID_JTAG_ID_BMSK                                                    0xfffff
#define HWIO_QFPROM_CORR_JTAG_ID_JTAG_ID_SHFT                                                        0x0

#define HWIO_QFPROM_CORR_PTE1_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004004)
#define HWIO_QFPROM_CORR_PTE1_RMSK                                                            0xffffffff
#define HWIO_QFPROM_CORR_PTE1_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE1_ADDR, HWIO_QFPROM_CORR_PTE1_RMSK)
#define HWIO_QFPROM_CORR_PTE1_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE1_ADDR, m)
#define HWIO_QFPROM_CORR_PTE1_IDDQ_BINNING_BMSK                                               0xfc000000
#define HWIO_QFPROM_CORR_PTE1_IDDQ_BINNING_SHFT                                                     0x1a
#define HWIO_QFPROM_CORR_PTE1_PROCESS_NODE_ID_BMSK                                             0x2000000
#define HWIO_QFPROM_CORR_PTE1_PROCESS_NODE_ID_SHFT                                                  0x19
#define HWIO_QFPROM_CORR_PTE1_PTE_DATA1_BMSK                                                   0x1ffffff
#define HWIO_QFPROM_CORR_PTE1_PTE_DATA1_SHFT                                                         0x0

#define HWIO_QFPROM_CORR_SERIAL_NUM_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004008)
#define HWIO_QFPROM_CORR_SERIAL_NUM_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_ADDR, HWIO_QFPROM_CORR_SERIAL_NUM_RMSK)
#define HWIO_QFPROM_CORR_SERIAL_NUM_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_ADDR, m)
#define HWIO_QFPROM_CORR_SERIAL_NUM_SERIAL_NUM_BMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_SERIAL_NUM_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_PTE2_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000400c)
#define HWIO_QFPROM_CORR_PTE2_RMSK                                                            0xffffffff
#define HWIO_QFPROM_CORR_PTE2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE2_ADDR, HWIO_QFPROM_CORR_PTE2_RMSK)
#define HWIO_QFPROM_CORR_PTE2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE2_ADDR, m)
#define HWIO_QFPROM_CORR_PTE2_WAFER_ID_BMSK                                                   0xf8000000
#define HWIO_QFPROM_CORR_PTE2_WAFER_ID_SHFT                                                         0x1b
#define HWIO_QFPROM_CORR_PTE2_DIE_X_BMSK                                                       0x7f80000
#define HWIO_QFPROM_CORR_PTE2_DIE_X_SHFT                                                            0x13
#define HWIO_QFPROM_CORR_PTE2_DIE_Y_BMSK                                                         0x7f800
#define HWIO_QFPROM_CORR_PTE2_DIE_Y_SHFT                                                             0xb
#define HWIO_QFPROM_CORR_PTE2_FOUNDRY_ID_BMSK                                                      0x700
#define HWIO_QFPROM_CORR_PTE2_FOUNDRY_ID_SHFT                                                        0x8
#define HWIO_QFPROM_CORR_PTE2_LOGIC_RETENTION_BMSK                                                  0xe0
#define HWIO_QFPROM_CORR_PTE2_LOGIC_RETENTION_SHFT                                                   0x5
#define HWIO_QFPROM_CORR_PTE2_SPEED_BIN_BMSK                                                        0x1c
#define HWIO_QFPROM_CORR_PTE2_SPEED_BIN_SHFT                                                         0x2
#define HWIO_QFPROM_CORR_PTE2_MX_RET_BIN_BMSK                                                        0x3
#define HWIO_QFPROM_CORR_PTE2_MX_RET_BIN_SHFT                                                        0x0

#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004010)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR, HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RMSK)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE_BMSK                                            0xffc00000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE_SHFT                                                  0x16
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE21_BMSK                                            0x200000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE21_SHFT                                                0x15
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE20_BMSK                                            0x100000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE20_SHFT                                                0x14
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE19_BMSK                                             0x80000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE19_SHFT                                                0x13
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE18_BMSK                                             0x40000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE18_SHFT                                                0x12
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE17_BMSK                                             0x20000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE17_SHFT                                                0x11
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE16_BMSK                                             0x10000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE16_SHFT                                                0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEC_EN_BMSK                                               0x8000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEC_EN_SHFT                                                  0xf
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_BOOT_ROM_PATCH_BMSK                                       0x4000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_BOOT_ROM_PATCH_SHFT                                          0xe
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_CONFIG_BMSK                                           0x2000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_CONFIG_SHFT                                              0xd
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB_BMSK                                                0x1000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB_SHFT                                                   0xc
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PK_HASH_BMSK                                               0x800
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PK_HASH_SHFT                                                 0xb
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_QC_SEC_BOOT_BMSK                                           0x400
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_QC_SEC_BOOT_SHFT                                             0xa
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_SEC_BOOT_BMSK                                          0x200
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_SEC_BOOT_SHFT                                            0x9
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                0x100
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                  0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                 0x80
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                  0x7
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEAT_CONFIG_BMSK                                            0x40
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEAT_CONFIG_SHFT                                             0x6
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_CONFIG_BMSK                                             0x20
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_CONFIG_SHFT                                              0x5
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_BMSK                                        0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_SHFT                                         0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_BMSK                                         0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_SHFT                                         0x3
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_BMSK                                         0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_SHFT                                         0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RD_WR_PERM_BMSK                                              0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RD_WR_PERM_SHFT                                              0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PTE_BMSK                                                     0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PTE_SHFT                                                     0x0

#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004014)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR, HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RMSK)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE_BMSK                                            0xffc00000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE_SHFT                                                  0x16
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE21_BMSK                                            0x200000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE21_SHFT                                                0x15
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE20_BMSK                                            0x100000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE20_SHFT                                                0x14
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE19_BMSK                                             0x80000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE19_SHFT                                                0x13
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE18_BMSK                                             0x40000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE18_SHFT                                                0x12
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE17_BMSK                                             0x20000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE17_SHFT                                                0x11
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE16_BMSK                                             0x10000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE16_SHFT                                                0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEC_EN_BMSK                                               0x8000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEC_EN_SHFT                                                  0xf
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_BOOT_ROM_PATCH_BMSK                                       0x4000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_BOOT_ROM_PATCH_SHFT                                          0xe
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_CONFIG_BMSK                                           0x2000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_CONFIG_SHFT                                              0xd
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB_BMSK                                                0x1000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB_SHFT                                                   0xc
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PK_HASH_BMSK                                               0x800
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PK_HASH_SHFT                                                 0xb
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_QC_SEC_BOOT_BMSK                                           0x400
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_QC_SEC_BOOT_SHFT                                             0xa
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_SEC_BOOT_BMSK                                          0x200
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_SEC_BOOT_SHFT                                            0x9
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_BMSK                                0x100
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_SHFT                                  0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_BMSK                                 0x80
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_SHFT                                  0x7
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEAT_CONFIG_BMSK                                            0x40
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEAT_CONFIG_SHFT                                             0x6
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_CONFIG_BMSK                                             0x20
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_CONFIG_SHFT                                              0x5
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_BMSK                                        0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_SHFT                                         0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_BMSK                                         0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_SHFT                                         0x3
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_BMSK                                         0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_SHFT                                         0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RD_WR_PERM_BMSK                                              0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RD_WR_PERM_SHFT                                              0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PTE_BMSK                                                     0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PTE_SHFT                                                     0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004018)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_PIL_SUBSYSTEM0_BMSK                              0xfc000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_PIL_SUBSYSTEM0_SHFT                                    0x1a
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_TZ_BMSK                                           0x3fff000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_TZ_SHFT                                                 0xc
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_SBL1_BMSK                                             0xffe
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_SBL1_SHFT                                               0x1
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RPMB_KEY_PROVISIONED_BMSK                               0x1
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RPMB_KEY_PROVISIONED_SHFT                               0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000401c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_APPSBL0_BMSK                                     0xfffc0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_APPSBL0_SHFT                                           0x12
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_PIL_SUBSYSTEM1_BMSK                                 0x3ffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_PIL_SUBSYSTEM1_SHFT                                     0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004020)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_APPSBL1_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_APPSBL1_SHFT                                            0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004024)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_BMSK                     0xff000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_SHFT                           0x18
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_HYPERVISOR_BMSK                                    0xfff000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_HYPERVISOR_SHFT                                         0xc
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RPM_BMSK                                              0xff0
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RPM_SHFT                                                0x4
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_APPSBL2_BMSK                                            0xf
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_APPSBL2_SHFT                                            0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004028)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MSS_BMSK                                         0xffff0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MSS_SHFT                                               0x10
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MBA_BMSK                                             0xffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MBA_SHFT                                                0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000402c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_SPARE0_BMSK                                      0xffffff00
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_SPARE0_SHFT                                             0x8
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_BMSK                     0xff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_SHFT                      0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004030)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE0_BMSK                                      0xff000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE0_SHFT                                            0x18
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG21_SECURE_BMSK                            0x800000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG21_SECURE_SHFT                                0x17
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_BMSK                               0x400000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_SHFT                                   0x16
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_BMSK                           0x3c0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_SHFT                               0x12
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG20_SECURE_BMSK                             0x20000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG20_SECURE_SHFT                                0x11
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG19_SECURE_BMSK                             0x10000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG19_SECURE_SHFT                                0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG18_SECURE_BMSK                              0x8000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG18_SECURE_SHFT                                 0xf
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG17_SECURE_BMSK                              0x4000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG17_SECURE_SHFT                                 0xe
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG16_SECURE_BMSK                              0x2000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG16_SECURE_SHFT                                 0xd
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_WDOG_EN_BMSK                                         0x1000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_WDOG_EN_SHFT                                            0xc
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPDM_SECURE_MODE_BMSK                                 0x800
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPDM_SECURE_MODE_SHFT                                   0xb
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ALT_SD_PORT_FOR_BOOT_BMSK                             0x400
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ALT_SD_PORT_FOR_BOOT_SHFT                               0xa
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_BMSK                    0x200
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_SHFT                      0x9
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_BMSK                              0x100
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_SHFT                                0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_BMSK                                         0xe0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SHFT                                          0x5
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_BMSK                               0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_SHFT                                0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_BMSK                                0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_SHFT                                0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_BMSK                                              0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_SHFT                                              0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_BMSK                                       0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_SHFT                                       0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_BMSK                                    0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_SHFT                                    0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004034)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE1_BMSK                                      0xff800000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE1_SHFT                                            0x17
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ANTI_ROLLBACK_FEATURE_EN_BMSK                      0x780000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ANTI_ROLLBACK_FEATURE_EN_SHFT                          0x13
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_BMSK                           0x40000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_SHFT                              0x12
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_BMSK                            0x20000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_SHFT                               0x11
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_BMSK                             0x10000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_SHFT                                0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_BMSK                               0x8000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_SHFT                                  0xf
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_BMSK                               0x4000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_SHFT                                  0xe
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_BMSK                            0x2000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_SHFT                               0xd
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_BMSK                             0x1000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_SHFT                                0xc
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_BMSK                               0x800
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_SHFT                                 0xb
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_BMSK                               0x400
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_SHFT                                 0xa
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE1_DISABLE_BMSK                                   0x200
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE1_DISABLE_SHFT                                     0x9
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE0_DISABLE_BMSK                                   0x100
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE0_DISABLE_SHFT                                     0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_VENUS_0_DBGEN_DISABLE_BMSK                             0x80
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_VENUS_0_DBGEN_DISABLE_SHFT                              0x7
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RPM_DAPEN_DISABLE_BMSK                                 0x40
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RPM_DAPEN_DISABLE_SHFT                                  0x6
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RPM_WCSS_NIDEN_DISABLE_BMSK                            0x20
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RPM_WCSS_NIDEN_DISABLE_SHFT                             0x5
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RPM_DBGEN_DISABLE_BMSK                                 0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RPM_DBGEN_DISABLE_SHFT                                  0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_WCSS_DBGEN_DISABLE_BMSK                                 0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_WCSS_DBGEN_DISABLE_SHFT                                 0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_MSS_NIDEN_DISABLE_BMSK                                  0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_MSS_NIDEN_DISABLE_SHFT                                  0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_MSS_DBGEN_DISABLE_BMSK                                  0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_MSS_DBGEN_DISABLE_SHFT                                  0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ALL_DEBUG_DISABLE_BMSK                                  0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ALL_DEBUG_DISABLE_SHFT                                  0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004038)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_PRODUCT_ID_BMSK                              0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_PRODUCT_ID_SHFT                                    0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_BMSK                                       0xffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_SHFT                                          0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000403c)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_PERIPH_VID_BMSK                                  0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_PERIPH_VID_SHFT                                        0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_PERIPH_PID_BMSK                                      0xffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_PERIPH_PID_SHFT                                         0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004040)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_NAV_EN_BMSK                                0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_NAV_EN_SHFT                                      0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_LDO_EN_BMSK                                0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_LDO_EN_SHFT                                      0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_ECS_EN_BMSK                                0x20000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_ECS_EN_SHFT                                      0x1d
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_ADCX_3_EN_BMSK                             0x10000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_ADCX_3_EN_SHFT                                   0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_ADCX_2_EN_BMSK                              0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_ADCX_2_EN_SHFT                                   0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_WCDMA_EN_BMSK                         0x6000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_WCDMA_EN_SHFT                              0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_TDSCDMA_EN_BMSK                       0x1800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_TDSCDMA_EN_SHFT                            0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_UL_CA_BMSK                         0x400000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_UL_CA_SHFT                             0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_EN_BMSK                            0x300000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_EN_SHFT                                0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_ABOVE_CAT2_EN_BMSK                  0xc0000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_ABOVE_CAT2_EN_SHFT                     0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_ABOVE_CAT1_EN_BMSK                  0x30000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_ABOVE_CAT1_EN_SHFT                     0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_60MHZ_BMSK                           0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_60MHZ_SHFT                              0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_40MHZ_BMSK                           0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_40MHZ_SHFT                              0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_MIMO_EN_BMSK                        0x3000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_MIMO_EN_SHFT                           0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_EN_BMSK                              0xc00
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_EN_SHFT                                0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_DC_EN_BMSK                           0x300
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_DC_EN_SHFT                             0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_GENRAN_EN_BMSK                             0xc0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_GENRAN_EN_SHFT                              0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_EN_BMSK                                    0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_EN_SHFT                                     0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_DSDA_EN_BMSK                               0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_DSDA_EN_SHFT                                0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_DO_EN_BMSK                                  0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_DO_EN_SHFT                                  0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_1X_EN_BMSK                                  0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_1X_EN_SHFT                                  0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004044)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                      0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MSMC_MDSP_FW_EN_BMSK                            0x7fff8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MSMC_MDSP_FW_EN_SHFT                                   0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MSMC_SPARE_BMSK                                     0x7000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MSMC_SPARE_SHFT                                        0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MSMC_MODEM_VU_EN_BMSK                                0xfff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MSMC_MODEM_VU_EN_SHFT                                  0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004048)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ACC_DISABLE_BMSK                                0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ACC_DISABLE_SHFT                                      0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SHADOW_READ_DISABLE_BMSK                        0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SHADOW_READ_DISABLE_SHFT                              0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SPARE1_BMSK                                     0x30000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SPARE1_SHFT                                           0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_FUSE_SMT_PERM_ENABLE_BMSK                        0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_FUSE_SMT_PERM_ENABLE_SHFT                             0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_FUSE_RCP_BYPASS_ENABLE_BMSK                      0x4000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_FUSE_RCP_BYPASS_ENABLE_SHFT                           0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MSMC_SMMU_BYPASS_DISABLE_BMSK                    0x2000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MSMC_SMMU_BYPASS_DISABLE_SHFT                         0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MSMC_GPIO_SLIMBUS_PD_DISABLE_BMSK                0x1000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MSMC_GPIO_SLIMBUS_PD_DISABLE_SHFT                     0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_FUSE_PCIE20_RC_DISABLE_BMSK                       0x800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_FUSE_PCIE20_RC_DISABLE_SHFT                           0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MSMC_PCIE_DISABLE_BMSK                            0x400000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MSMC_PCIE_DISABLE_SHFT                                0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_NDINT_DISABLE_BMSK                                0x200000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_NDINT_DISABLE_SHFT                                    0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_BMSK                      0x100000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_SHFT                          0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_BMSK                        0x80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_SHFT                           0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_BMSK                         0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_SHFT                            0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_BMSK                          0x20000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_SHFT                             0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_BMSK                          0x10000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_SHFT                             0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_BMSK                        0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_SHFT                           0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_BMSK                         0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_SHFT                            0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_BMSK                          0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_SHFT                             0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_BMSK                          0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_SHFT                             0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SPARE1_DISABLE_BMSK                               0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SPARE1_DISABLE_SHFT                                 0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SPARE0_DISABLE_BMSK                               0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SPARE0_DISABLE_SHFT                                 0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_BMSK                        0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_SHFT                          0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_BMSK                            0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_SHFT                              0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_WCSS_NIDEN_DISABLE_BMSK                        0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_WCSS_NIDEN_DISABLE_SHFT                         0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_BMSK                             0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_SHFT                              0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_BMSK                            0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_SHFT                             0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_BMSK                             0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_SHFT                              0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_BMSK                              0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_SHFT                              0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QDI_SPMI_DISABLE_BMSK                                  0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QDI_SPMI_DISABLE_SHFT                                  0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_BMSK                                   0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_SHFT                                   0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_BMSK                                       0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_SHFT                                       0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000404c)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_BMSK                     0xf8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_SHFT                           0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_BMSK                    0x4000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_SHFT                         0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_BMSK                           0x3ffc000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_SHFT                                 0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SPARE1_BMSK                                         0x3c00
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SPARE1_SHFT                                            0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_MODEM_PBL_PATCH_VERSION_BMSK                         0x3e0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_MODEM_PBL_PATCH_VERSION_SHFT                           0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_BMSK                           0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_SHFT                            0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004050)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_BMSK           0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_SHFT                  0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004054)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE3_BMSK                                     0xf8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE3_SHFT                                           0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_MODEM_PBL_PLL_CTRL_BMSK                          0x7800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_MODEM_PBL_PLL_CTRL_SHFT                               0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_BMSK                            0x780000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_SHFT                                0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_CFG_BMSK                             0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_CFG_SHFT                                0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_DELAY_BMSK                           0x30000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_DELAY_SHFT                              0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE1_BMSK                                         0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE1_SHFT                                            0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_USB_SS_ENABLE_BMSK                                  0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_USB_SS_ENABLE_SHFT                                     0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_BMSK                            0x3000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_SHFT                               0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_ROM_BMSK                              0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_ROM_SHFT                                0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_MSA_ENA_BMSK                                         0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_MSA_ENA_SHFT                                           0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_FORCE_MSA_AUTH_EN_BMSK                               0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_FORCE_MSA_AUTH_EN_SHFT                                 0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE_8_7_BMSK                                       0x180
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE_8_7_SHFT                                         0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_MODEM_BOOT_FROM_ROM_BMSK                              0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_MODEM_BOOT_FROM_ROM_SHFT                               0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_BMSK                           0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_SHFT                            0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_BMSK                0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_SHFT                 0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004058 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                          3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                       0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                              0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000405c + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                          3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                       0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                              0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004078 + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                          3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                       0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                              0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000407c + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                          3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                       0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                              0x0

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_ADDR                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004098)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT4_BMSK                                 0xff000000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT4_SHFT                                       0x18
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT3_BMSK                                   0xff0000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT3_SHFT                                       0x10
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT2_BMSK                                     0xff00
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT2_SHFT                                        0x8
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT1_BMSK                                       0xff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT1_SHFT                                        0x0

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_ADDR                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000409c)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_RMSK                                             0xffffff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT7_BMSK                                   0xff0000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT7_SHFT                                       0x10
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT6_BMSK                                     0xff00
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT6_SHFT                                        0x8
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT5_BMSK                                       0xff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT5_SHFT                                        0x0

#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040a0)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT4_BMSK                                  0xff000000
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT4_SHFT                                        0x18
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT3_BMSK                                    0xff0000
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT3_SHFT                                        0x10
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT2_BMSK                                      0xff00
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT2_SHFT                                         0x8
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT1_BMSK                                        0xff
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT1_SHFT                                         0x0

#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040a4)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_RMSK                                              0xffffff
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT7_BMSK                                    0xff0000
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT7_SHFT                                        0x10
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT6_BMSK                                      0xff00
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT6_SHFT                                         0x8
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT5_BMSK                                        0xff
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT5_SHFT                                         0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040a8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_MAXn                                                         3
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_HASH_DATA0_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_HASH_DATA0_SHFT                                            0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040ac + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_RMSK                                                  0xffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_MAXn                                                         3
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_HASH_DATA1_BMSK                                       0xffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_HASH_DATA1_SHFT                                            0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040c8)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_HASH_DATA0_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_HASH_DATA0_SHFT                                            0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040cc)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_RMSK                                                  0xffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_QFPROM_CORR_PK_HASH_ROW4_MSB_BMSK                     0xffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_QFPROM_CORR_PK_HASH_ROW4_MSB_SHFT                          0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040d0)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS1_POINT1_0_BMSK                                  0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS1_POINT1_0_SHFT                                        0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS0_POINT1_BMSK                                    0x7f800000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS0_POINT1_SHFT                                          0x17
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS_BASE1_BMSK                                        0x7fe000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS_BASE1_SHFT                                             0xd
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS_BASE0_BMSK                                          0x1ff8
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS_BASE0_SHFT                                             0x3
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS_CAL_SEL_BMSK                                           0x7
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS_CAL_SEL_SHFT                                           0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040d4)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK                                                  0x7fffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_FEC_BMSK                                              0x7f000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_FEC_SHFT                                                    0x18
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_SPARE_BMSK                                              0x800000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_SPARE_SHFT                                                  0x17
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS3_POINT1_BMSK                                      0x7f8000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS3_POINT1_SHFT                                           0xf
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS2_POINT1_BMSK                                        0x7f80
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS2_POINT1_SHFT                                           0x7
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS1_POINT1_1_7_BMSK                                      0x7f
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS1_POINT1_1_7_SHFT                                       0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040d8)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_SVS2_TARGET_BMSK                                 0xfc000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_SVS2_TARGET_SHFT                                       0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_SVS_TARGET_BMSK                                   0x3f00000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_SVS_TARGET_SHFT                                        0x14
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_NOMINAL_TARGET_BMSK                                 0xfc000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_NOMINAL_TARGET_SHFT                                     0xe
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TURBO_TARGET_BMSK                                    0x3f00
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TURBO_TARGET_SHFT                                       0x8
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS4_POINT1_BMSK                                          0xff
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS4_POINT1_SHFT                                           0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040dc)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK                                                  0x7fffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_FEC_BMSK                                              0x7f000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_FEC_SHFT                                                    0x18
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR1_SVS2_TARGET_BMSK                                   0xfc0000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR1_SVS2_TARGET_SHFT                                       0x12
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR1_SVS_TARGET_BMSK                                     0x3f000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR1_SVS_TARGET_SHFT                                         0xc
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR1_NOMINAL_TARGET_BMSK                                   0xfc0
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR1_NOMINAL_TARGET_SHFT                                     0x6
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR1_TURBO_TARGET_BMSK                                      0x3f
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR1_TURBO_TARGET_SHFT                                       0x0

#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040e0)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_LDO_TARGET1_3_0_BMSK                                  0xf0000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_LDO_TARGET1_3_0_SHFT                                        0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_LDO_TARGET0_BMSK                                       0xfc00000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_LDO_TARGET0_SHFT                                            0x16
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_LDO_ENABLE_BMSK                                         0x200000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_LDO_ENABLE_SHFT                                             0x15
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR2_MODE_EXPANSION_BMSK                                0x1f8000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR2_MODE_EXPANSION_SHFT                                     0xf
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR2_NOMINAL_TARGET_BMSK                                  0x7e00
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR2_NOMINAL_TARGET_SHFT                                     0x9
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR2_TURBO_TARGET_BMSK                                     0x1f8
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR2_TURBO_TARGET_SHFT                                       0x3
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR2_ENABLE_BMSK                                             0x4
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR2_ENABLE_SHFT                                             0x2
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR1_ENABLE_BMSK                                             0x2
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR1_ENABLE_SHFT                                             0x1
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_ENABLE_BMSK                                             0x1
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_ENABLE_SHFT                                             0x0

#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040e4)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RMSK                                                  0x7fffffff
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_FEC_BMSK                                              0x7f000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_FEC_SHFT                                                    0x18
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_SPARE_BMSK                                              0xfffffc
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_SPARE_SHFT                                                   0x2
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_LDO_TARGET1_5_4_BMSK                                         0x3
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_LDO_TARGET1_5_4_SHFT                                         0x0

#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040e8)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_BHS_SLOPE_0_BMSK                                  0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_BHS_SLOPE_0_SHFT                                        0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_BHS_INTERCEPT_BMSK                                0x7fe00000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_BHS_INTERCEPT_SHFT                                      0x15
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_LDO_SLOPE_BMSK                                      0x1ff800
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_LDO_SLOPE_SHFT                                           0xb
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_LDO_INTERCEPT_BMSK                                     0x7fe
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_LDO_INTERCEPT_SHFT                                       0x1
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_ENABLE_BMSK                                              0x1
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_ENABLE_SHFT                                              0x0

#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040ec)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RMSK                                                  0x7fffffff
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_FEC_BMSK                                              0x7f000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_FEC_SHFT                                                    0x18
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_Q6SS0_LDO_VREF_TRIM_BMSK                                0xf80000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_Q6SS0_LDO_VREF_TRIM_SHFT                                    0x13
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_SPARE_BMSK                                               0x60000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_SPARE_SHFT                                                  0x11
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ECS_MISC_BMSK                                            0x1fe00
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ECS_MISC_SHFT                                                0x9
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ECS_BHS_SLOPE_9_1_BMSK                                     0x1ff
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ECS_BHS_SLOPE_9_1_SHFT                                       0x0

#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040f0)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B1M2_1_0_BMSK                                      0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B1M2_1_0_SHFT                                            0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B1M1_BMSK                                          0x38000000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B1M1_SHFT                                                0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B1M0_BMSK                                           0x7000000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B1M0_SHFT                                                0x18
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_G_B1_BMSK                                               0xe00000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_G_B1_SHFT                                                   0x15
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_SAR_LDO_ERR0_BMSK                                       0x180000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_SAR_LDO_ERR0_SHFT                                           0x13
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CLK_LDO_ERR0_BMSK                                        0x60000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CLK_LDO_ERR0_SHFT                                           0x11
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_VREF_ERR_B0_BMSK                                         0x18000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_VREF_ERR_B0_SHFT                                             0xf
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B0M3_BMSK                                              0x7000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B0M3_SHFT                                                 0xc
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B0M2_BMSK                                               0xe00
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B0M2_SHFT                                                 0x9
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B0M1_BMSK                                               0x1c0
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B0M1_SHFT                                                 0x6
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B0M0_BMSK                                                0x38
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B0M0_SHFT                                                 0x3
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_G_B0_BMSK                                                    0x7
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_G_B0_SHFT                                                    0x0

#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040f4)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_RMSK                                                  0x7fffffff
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_FEC_BMSK                                              0x7f000000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_FEC_SHFT                                                    0x18
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B2M3_1_0_BMSK                                        0xc00000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B2M3_1_0_SHFT                                            0x16
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B2M2_BMSK                                            0x380000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B2M2_SHFT                                                0x13
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B2M1_BMSK                                             0x70000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B2M1_SHFT                                                0x10
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B2M0_BMSK                                              0xe000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B2M0_SHFT                                                 0xd
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_G_B2_BMSK                                                 0x1c00
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_G_B2_SHFT                                                    0xa
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_SAR_LDO_ERR1_BMSK                                          0x300
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_SAR_LDO_ERR1_SHFT                                            0x8
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CLK_LDO_ERR1_BMSK                                           0xc0
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CLK_LDO_ERR1_SHFT                                            0x6
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_VREF_ERR_B1_BMSK                                            0x30
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_VREF_ERR_B1_SHFT                                             0x4
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B1M3_BMSK                                                 0xe
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B1M3_SHFT                                                 0x1
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B1M2_2_BMSK                                               0x1
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B1M2_2_SHFT                                               0x0

#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040f8)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B4M0_0_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B4M0_0_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_G_B4_BMSK                                             0x70000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_G_B4_SHFT                                                   0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_SAR_LDO_ERR3_BMSK                                      0xc000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_SAR_LDO_ERR3_SHFT                                           0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CLK_LDO_ERR3_BMSK                                      0x3000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CLK_LDO_ERR3_SHFT                                           0x18
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_VREF_ERR_B3_BMSK                                        0xc00000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_VREF_ERR_B3_SHFT                                            0x16
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B3M3_BMSK                                            0x380000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B3M3_SHFT                                                0x13
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B3M2_BMSK                                             0x70000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B3M2_SHFT                                                0x10
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B3M1_BMSK                                              0xe000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B3M1_SHFT                                                 0xd
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B3M0_BMSK                                              0x1c00
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B3M0_SHFT                                                 0xa
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_G_B3_BMSK                                                  0x380
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_G_B3_SHFT                                                    0x7
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_SAR_LDO_ERR2_BMSK                                           0x60
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_SAR_LDO_ERR2_SHFT                                            0x5
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CLK_LDO_ERR2_BMSK                                           0x18
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CLK_LDO_ERR2_SHFT                                            0x3
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_VREF_ERR_B2_BMSK                                             0x6
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_VREF_ERR_B2_SHFT                                             0x1
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B2M3_2_BMSK                                               0x1
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B2M3_2_SHFT                                               0x0

#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040fc)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RMSK                                                  0x7fffffff
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_FEC_BMSK                                              0x7f000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_FEC_SHFT                                                    0x18
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B5M1_0_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B5M1_0_SHFT                                              0x17
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B5M0_BMSK                                            0x700000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B5M0_SHFT                                                0x14
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_G_B5_BMSK                                                0xe0000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_G_B5_SHFT                                                   0x11
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_SAR_LDO_ERR4_BMSK                                        0x18000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_SAR_LDO_ERR4_SHFT                                            0xf
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CLK_LDO_ERR4_BMSK                                         0x6000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CLK_LDO_ERR4_SHFT                                            0xd
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_VREF_ERR_B4_BMSK                                          0x1800
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_VREF_ERR_B4_SHFT                                             0xb
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B4M3_BMSK                                               0x700
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B4M3_SHFT                                                 0x8
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B4M2_BMSK                                                0xe0
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B4M2_SHFT                                                 0x5
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B4M1_BMSK                                                0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B4M1_SHFT                                                 0x2
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B4M0_2_1_BMSK                                             0x3
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B4M0_2_1_SHFT                                             0x0

#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004100)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CLK_LDO_ERR6_0_BMSK                                   0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CLK_LDO_ERR6_0_SHFT                                         0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_VREF_ERR_B6_BMSK                                      0x60000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_VREF_ERR_B6_SHFT                                            0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B6M3_BMSK                                          0x1c000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B6M3_SHFT                                                0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B6M2_BMSK                                           0x3800000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B6M2_SHFT                                                0x17
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B6M1_BMSK                                            0x700000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B6M1_SHFT                                                0x14
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B6M0_BMSK                                             0xe0000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B6M0_SHFT                                                0x11
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_G_B6_BMSK                                                0x1c000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_G_B6_SHFT                                                    0xe
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_SAR_LDO_ERR5_BMSK                                         0x3000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_SAR_LDO_ERR5_SHFT                                            0xc
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CLK_LDO_ERR5_BMSK                                          0xc00
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CLK_LDO_ERR5_SHFT                                            0xa
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_VREF_ERR_B5_BMSK                                           0x300
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_VREF_ERR_B5_SHFT                                             0x8
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B5M3_BMSK                                                0xe0
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B5M3_SHFT                                                 0x5
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B5M2_BMSK                                                0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B5M2_SHFT                                                 0x2
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B5M1_2_1_BMSK                                             0x3
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B5M1_2_1_SHFT                                             0x0

#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004104)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RMSK                                                  0x7fffffff
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_FEC_BMSK                                              0x7f000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_FEC_SHFT                                                    0x18
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_BMSK                                 0xff0000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_SHFT                                     0x10
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_BMSK                                   0xff00
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_SHFT                                      0x8
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_AVG_ERROR_BMSK                           0x80
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_AVG_ERROR_SHFT                            0x7
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_AVG_ERROR_BMSK                           0x40
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_AVG_ERROR_SHFT                            0x6
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC_0_1_FUSEFLAG_BMSK                               0x20
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC_0_1_FUSEFLAG_SHFT                                0x5
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_OVERFLOW_BMSK                            0x10
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_OVERFLOW_SHFT                             0x4
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_OVERFLOW_BMSK                             0x8
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_OVERFLOW_SHFT                             0x3
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_SAR_LDO_ERR6_BMSK                                            0x6
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_SAR_LDO_ERR6_SHFT                                            0x1
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_CLK_LDO_ERR6_1_BMSK                                          0x1
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_CLK_LDO_ERR6_1_SHFT                                          0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n)                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004108 + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_MAXn                                                     15
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_REDUN_DATA_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_REDUN_DATA_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n)                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000410c + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_RMSK                                               0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_MAXn                                                     15
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_REDUN_DATA_BMSK                                    0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_REDUN_DATA_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004188)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_MEM_ACCEL_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_MEM_ACCEL_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000418c)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_RMSK                                              0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_MEM_ACCEL_BMSK                                    0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_MEM_ACCEL_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004190)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_MEM_ACCEL_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_MEM_ACCEL_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004194)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_RMSK                                              0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_MEM_ACCEL_BMSK                                    0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_MEM_ACCEL_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004198)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_MEM_ACCEL_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_MEM_ACCEL_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000419c)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_RMSK                                              0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_MEM_ACCEL_BMSK                                    0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_MEM_ACCEL_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041a0)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_LSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_LSB_MEM_ACCEL_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_LSB_MEM_ACCEL_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041a4)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_MSB_RMSK                                              0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_MSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_MSB_MEM_ACCEL_BMSK                                    0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_MSB_MEM_ACCEL_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041a8)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_LSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_LSB_MEM_ACCEL_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_LSB_MEM_ACCEL_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041ac)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_MSB_RMSK                                              0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_MSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_MSB_MEM_ACCEL_BMSK                                    0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_MSB_MEM_ACCEL_SHFT                                         0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n)                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_MAXn                                                      31
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_DATA_BMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_DATA_SHFT                                          0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n)                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RMSK                                                0xffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_MAXn                                                      31
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_SPARE3_BMSK                                         0xfe0000
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_SPARE3_SHFT                                             0x11
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_ADDR_BMSK                                      0x1fffe
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_ADDR_SHFT                                          0x1
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_EN_BMSK                                            0x1
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_EN_SHFT                                            0x0

#define HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042b0)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR, HWIO_QFPROM_CORR_FEC_EN_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR,v)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR,m,v,HWIO_QFPROM_CORR_FEC_EN_LSB_IN)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION31_FEC_EN_BMSK                                      0x80000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION31_FEC_EN_SHFT                                            0x1f
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION30_FEC_EN_BMSK                                      0x40000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION30_FEC_EN_SHFT                                            0x1e
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION29_FEC_EN_BMSK                                      0x20000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION29_FEC_EN_SHFT                                            0x1d
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION28_FEC_EN_BMSK                                      0x10000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION28_FEC_EN_SHFT                                            0x1c
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION27_FEC_EN_BMSK                                       0x8000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION27_FEC_EN_SHFT                                            0x1b
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION26_FEC_EN_BMSK                                       0x4000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION26_FEC_EN_SHFT                                            0x1a
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION25_FEC_EN_BMSK                                       0x2000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION25_FEC_EN_SHFT                                            0x19
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION24_FEC_EN_BMSK                                       0x1000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION24_FEC_EN_SHFT                                            0x18
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION23_FEC_EN_BMSK                                        0x800000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION23_FEC_EN_SHFT                                            0x17
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION22_FEC_EN_BMSK                                        0x400000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION22_FEC_EN_SHFT                                            0x16
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION21_FEC_EN_BMSK                                        0x200000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION21_FEC_EN_SHFT                                            0x15
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION20_FEC_EN_BMSK                                        0x100000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION20_FEC_EN_SHFT                                            0x14
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION19_FEC_EN_BMSK                                         0x80000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION19_FEC_EN_SHFT                                            0x13
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION18_FEC_EN_BMSK                                         0x40000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION18_FEC_EN_SHFT                                            0x12
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION17_FEC_EN_BMSK                                         0x20000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION17_FEC_EN_SHFT                                            0x11
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION16_FEC_EN_BMSK                                         0x10000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION16_FEC_EN_SHFT                                            0x10
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION15_FEC_EN_BMSK                                          0x8000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION15_FEC_EN_SHFT                                             0xf
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION14_FEC_EN_BMSK                                          0x4000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION14_FEC_EN_SHFT                                             0xe
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION13_FEC_EN_BMSK                                          0x2000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION13_FEC_EN_SHFT                                             0xd
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION12_FEC_EN_BMSK                                          0x1000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION12_FEC_EN_SHFT                                             0xc
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION11_FEC_EN_BMSK                                           0x800
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION11_FEC_EN_SHFT                                             0xb
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION10_FEC_EN_BMSK                                           0x400
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION10_FEC_EN_SHFT                                             0xa
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION9_FEC_EN_BMSK                                            0x200
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION9_FEC_EN_SHFT                                              0x9
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION8_FEC_EN_BMSK                                            0x100
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION8_FEC_EN_SHFT                                              0x8
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION7_FEC_EN_BMSK                                             0x80
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION7_FEC_EN_SHFT                                              0x7
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION6_FEC_EN_BMSK                                             0x40
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION6_FEC_EN_SHFT                                              0x6
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION5_FEC_EN_BMSK                                             0x20
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION5_FEC_EN_SHFT                                              0x5
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION4_FEC_EN_BMSK                                             0x10
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION4_FEC_EN_SHFT                                              0x4
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION3_FEC_EN_BMSK                                              0x8
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION3_FEC_EN_SHFT                                              0x3
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION2_FEC_EN_BMSK                                              0x4
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION2_FEC_EN_SHFT                                              0x2
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION1_FEC_EN_BMSK                                              0x2
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION1_FEC_EN_SHFT                                              0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION0_FEC_EN_BMSK                                              0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION0_FEC_EN_SHFT                                              0x0

#define HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042b4)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR, HWIO_QFPROM_CORR_FEC_EN_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_FEC_EN_REDUNDANCY_BMSK                                    0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_MSB_FEC_EN_REDUNDANCY_SHFT                                           0x0

#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042b8)
#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG16_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG16_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG16_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042bc)
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG16_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG16_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG16_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_SPARE0_BMSK                                          0x80000000
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_SPARE0_SHFT                                                0x1f
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_FEC_VALUE_BMSK                                       0x7f000000
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_FEC_VALUE_SHFT                                             0x18
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_SPARE1_BMSK                                            0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042c0)
#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG17_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG17_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG17_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042c4)
#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG17_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG17_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG17_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_SPARE1_BMSK                                            0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042c8)
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG18_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042cc)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG18_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_SPARE1_BMSK                                            0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042d0)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG19_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042d4)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG19_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR,v)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR,m,v,HWIO_QFPROM_CORR_SPARE_REG19_MSB_IN)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_SPARE1_BMSK                                            0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042d8)
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG20_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG20_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG20_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_SPARE_REG20_LSB_ADDR,v)
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_SPARE_REG20_LSB_ADDR,m,v,HWIO_QFPROM_CORR_SPARE_REG20_LSB_IN)
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042dc)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG20_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG20_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG20_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_SPARE_REG20_MSB_ADDR,v)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_SPARE_REG20_MSB_ADDR,m,v,HWIO_QFPROM_CORR_SPARE_REG20_MSB_IN)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_SPARE1_BMSK                                            0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042e0)
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG21_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG21_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG21_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_SPARE_REG21_LSB_ADDR,v)
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_SPARE_REG21_LSB_ADDR,m,v,HWIO_QFPROM_CORR_SPARE_REG21_LSB_IN)
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042e4)
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG21_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG21_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG21_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_SPARE_REG21_MSB_ADDR,v)
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_SPARE_REG21_MSB_ADDR,m,v,HWIO_QFPROM_CORR_SPARE_REG21_MSB_IN)
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_SPARE1_BMSK                                            0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_ACC_PRIVATEn_ADDR(n)                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004400 + 0x4 * (n))
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_MAXn                                                            39
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ACC_PRIVATEn_ADDR(n), HWIO_QFPROM_CORR_ACC_PRIVATEn_RMSK)
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ACC_PRIVATEn_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_ACC_PRIVATE_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_ACC_PRIVATE_SHFT                                               0x0

#define HWIO_SEC_CTRL_HW_VERSION_ADDR                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006000)
#define HWIO_SEC_CTRL_HW_VERSION_RMSK                                                         0xffffffff
#define HWIO_SEC_CTRL_HW_VERSION_IN          \
        in_dword_masked(HWIO_SEC_CTRL_HW_VERSION_ADDR, HWIO_SEC_CTRL_HW_VERSION_RMSK)
#define HWIO_SEC_CTRL_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_SEC_CTRL_HW_VERSION_ADDR, m)
#define HWIO_SEC_CTRL_HW_VERSION_MAJOR_BMSK                                                   0xf0000000
#define HWIO_SEC_CTRL_HW_VERSION_MAJOR_SHFT                                                         0x1c
#define HWIO_SEC_CTRL_HW_VERSION_MINOR_BMSK                                                    0xfff0000
#define HWIO_SEC_CTRL_HW_VERSION_MINOR_SHFT                                                         0x10
#define HWIO_SEC_CTRL_HW_VERSION_STEP_BMSK                                                        0xffff
#define HWIO_SEC_CTRL_HW_VERSION_STEP_SHFT                                                           0x0

#define HWIO_FEATURE_CONFIG0_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006004)
#define HWIO_FEATURE_CONFIG0_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG0_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG0_ADDR, HWIO_FEATURE_CONFIG0_RMSK)
#define HWIO_FEATURE_CONFIG0_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG0_ADDR, m)
#define HWIO_FEATURE_CONFIG0_MSMC_NAV_EN_BMSK                                                 0x80000000
#define HWIO_FEATURE_CONFIG0_MSMC_NAV_EN_SHFT                                                       0x1f
#define HWIO_FEATURE_CONFIG0_MSMC_LDO_EN_BMSK                                                 0x40000000
#define HWIO_FEATURE_CONFIG0_MSMC_LDO_EN_SHFT                                                       0x1e
#define HWIO_FEATURE_CONFIG0_MSMC_ECS_EN_BMSK                                                 0x20000000
#define HWIO_FEATURE_CONFIG0_MSMC_ECS_EN_SHFT                                                       0x1d
#define HWIO_FEATURE_CONFIG0_MSMC_ADCX_3_EN_BMSK                                              0x10000000
#define HWIO_FEATURE_CONFIG0_MSMC_ADCX_3_EN_SHFT                                                    0x1c
#define HWIO_FEATURE_CONFIG0_MSMC_ADCX_2_EN_BMSK                                               0x8000000
#define HWIO_FEATURE_CONFIG0_MSMC_ADCX_2_EN_SHFT                                                    0x1b
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_WCDMA_EN_BMSK                                          0x6000000
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_WCDMA_EN_SHFT                                               0x19
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_TDSCDMA_EN_BMSK                                        0x1800000
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_TDSCDMA_EN_SHFT                                             0x17
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_LTE_UL_CA_BMSK                                          0x400000
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_LTE_UL_CA_SHFT                                              0x16
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_LTE_EN_BMSK                                             0x300000
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_LTE_EN_SHFT                                                 0x14
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_LTE_ABOVE_CAT2_EN_BMSK                                   0xc0000
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_LTE_ABOVE_CAT2_EN_SHFT                                      0x12
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_LTE_ABOVE_CAT1_EN_BMSK                                   0x30000
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_LTE_ABOVE_CAT1_EN_SHFT                                      0x10
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_LTE_60MHZ_BMSK                                            0x8000
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_LTE_60MHZ_SHFT                                               0xf
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_LTE_40MHZ_BMSK                                            0x4000
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_LTE_40MHZ_SHFT                                               0xe
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_HSPA_MIMO_EN_BMSK                                         0x3000
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_HSPA_MIMO_EN_SHFT                                            0xc
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_HSPA_EN_BMSK                                               0xc00
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_HSPA_EN_SHFT                                                 0xa
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_HSPA_DC_EN_BMSK                                            0x300
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_HSPA_DC_EN_SHFT                                              0x8
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_GENRAN_EN_BMSK                                              0xc0
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_GENRAN_EN_SHFT                                               0x6
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_EN_BMSK                                                     0x20
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_EN_SHFT                                                      0x5
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_DSDA_EN_BMSK                                                0x10
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_DSDA_EN_SHFT                                                 0x4
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_DO_EN_BMSK                                                   0xc
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_DO_EN_SHFT                                                   0x2
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_1X_EN_BMSK                                                   0x3
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_1X_EN_SHFT                                                   0x0

#define HWIO_FEATURE_CONFIG1_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006008)
#define HWIO_FEATURE_CONFIG1_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG1_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, HWIO_FEATURE_CONFIG1_RMSK)
#define HWIO_FEATURE_CONFIG1_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, m)
#define HWIO_FEATURE_CONFIG1_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                                 0x80000000
#define HWIO_FEATURE_CONFIG1_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                                       0x1f
#define HWIO_FEATURE_CONFIG1_MSMC_MDSP_FW_EN_BMSK                                             0x7fff8000
#define HWIO_FEATURE_CONFIG1_MSMC_MDSP_FW_EN_SHFT                                                    0xf
#define HWIO_FEATURE_CONFIG1_MSMC_SPARE_BMSK                                                      0x7000
#define HWIO_FEATURE_CONFIG1_MSMC_SPARE_SHFT                                                         0xc
#define HWIO_FEATURE_CONFIG1_MSMC_MODEM_VU_EN_BMSK                                                 0xfff
#define HWIO_FEATURE_CONFIG1_MSMC_MODEM_VU_EN_SHFT                                                   0x0

#define HWIO_FEATURE_CONFIG2_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000600c)
#define HWIO_FEATURE_CONFIG2_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG2_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR, HWIO_FEATURE_CONFIG2_RMSK)
#define HWIO_FEATURE_CONFIG2_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR, m)
#define HWIO_FEATURE_CONFIG2_ACC_DISABLE_BMSK                                                 0x80000000
#define HWIO_FEATURE_CONFIG2_ACC_DISABLE_SHFT                                                       0x1f
#define HWIO_FEATURE_CONFIG2_SHADOW_READ_DISABLE_BMSK                                         0x40000000
#define HWIO_FEATURE_CONFIG2_SHADOW_READ_DISABLE_SHFT                                               0x1e
#define HWIO_FEATURE_CONFIG2_SPARE1_BMSK                                                      0x30000000
#define HWIO_FEATURE_CONFIG2_SPARE1_SHFT                                                            0x1c
#define HWIO_FEATURE_CONFIG2_FUSE_SMT_PERM_ENABLE_BMSK                                         0x8000000
#define HWIO_FEATURE_CONFIG2_FUSE_SMT_PERM_ENABLE_SHFT                                              0x1b
#define HWIO_FEATURE_CONFIG2_FUSE_RCP_BYPASS_ENABLE_BMSK                                       0x4000000
#define HWIO_FEATURE_CONFIG2_FUSE_RCP_BYPASS_ENABLE_SHFT                                            0x1a
#define HWIO_FEATURE_CONFIG2_MSMC_SMMU_BYPASS_DISABLE_BMSK                                     0x2000000
#define HWIO_FEATURE_CONFIG2_MSMC_SMMU_BYPASS_DISABLE_SHFT                                          0x19
#define HWIO_FEATURE_CONFIG2_MSMC_GPIO_SLIMBUS_PD_DISABLE_BMSK                                 0x1000000
#define HWIO_FEATURE_CONFIG2_MSMC_GPIO_SLIMBUS_PD_DISABLE_SHFT                                      0x18
#define HWIO_FEATURE_CONFIG2_FUSE_PCIE20_RC_DISABLE_BMSK                                        0x800000
#define HWIO_FEATURE_CONFIG2_FUSE_PCIE20_RC_DISABLE_SHFT                                            0x17
#define HWIO_FEATURE_CONFIG2_MSMC_PCIE_DISABLE_BMSK                                             0x400000
#define HWIO_FEATURE_CONFIG2_MSMC_PCIE_DISABLE_SHFT                                                 0x16
#define HWIO_FEATURE_CONFIG2_NDINT_DISABLE_BMSK                                                 0x200000
#define HWIO_FEATURE_CONFIG2_NDINT_DISABLE_SHFT                                                     0x15
#define HWIO_FEATURE_CONFIG2_QC_DAP_DEVICEEN_DISABLE_BMSK                                       0x100000
#define HWIO_FEATURE_CONFIG2_QC_DAP_DEVICEEN_DISABLE_SHFT                                           0x14
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPNIDEN_DISABLE_BMSK                                         0x80000
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPNIDEN_DISABLE_SHFT                                            0x13
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPIDEN_DISABLE_BMSK                                          0x40000
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPIDEN_DISABLE_SHFT                                             0x12
#define HWIO_FEATURE_CONFIG2_QC_DAP_NIDEN_DISABLE_BMSK                                           0x20000
#define HWIO_FEATURE_CONFIG2_QC_DAP_NIDEN_DISABLE_SHFT                                              0x11
#define HWIO_FEATURE_CONFIG2_QC_DAP_DBGEN_DISABLE_BMSK                                           0x10000
#define HWIO_FEATURE_CONFIG2_QC_DAP_DBGEN_DISABLE_SHFT                                              0x10
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPNIDEN_DISABLE_BMSK                                         0x8000
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPNIDEN_DISABLE_SHFT                                            0xf
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPIDEN_DISABLE_BMSK                                          0x4000
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPIDEN_DISABLE_SHFT                                             0xe
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_BMSK                                           0x2000
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_SHFT                                              0xd
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_BMSK                                           0x1000
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_SHFT                                              0xc
#define HWIO_FEATURE_CONFIG2_QC_SPARE1_DISABLE_BMSK                                                0x800
#define HWIO_FEATURE_CONFIG2_QC_SPARE1_DISABLE_SHFT                                                  0xb
#define HWIO_FEATURE_CONFIG2_QC_SPARE0_DISABLE_BMSK                                                0x400
#define HWIO_FEATURE_CONFIG2_QC_SPARE0_DISABLE_SHFT                                                  0xa
#define HWIO_FEATURE_CONFIG2_QC_VENUS_0_DBGEN_DISABLE_BMSK                                         0x200
#define HWIO_FEATURE_CONFIG2_QC_VENUS_0_DBGEN_DISABLE_SHFT                                           0x9
#define HWIO_FEATURE_CONFIG2_QC_RPM_DAPEN_DISABLE_BMSK                                             0x100
#define HWIO_FEATURE_CONFIG2_QC_RPM_DAPEN_DISABLE_SHFT                                               0x8
#define HWIO_FEATURE_CONFIG2_QC_RPM_WCSS_NIDEN_DISABLE_BMSK                                         0x80
#define HWIO_FEATURE_CONFIG2_QC_RPM_WCSS_NIDEN_DISABLE_SHFT                                          0x7
#define HWIO_FEATURE_CONFIG2_QC_RPM_DBGEN_DISABLE_BMSK                                              0x40
#define HWIO_FEATURE_CONFIG2_QC_RPM_DBGEN_DISABLE_SHFT                                               0x6
#define HWIO_FEATURE_CONFIG2_QC_WCSS_DBGEN_DISABLE_BMSK                                             0x20
#define HWIO_FEATURE_CONFIG2_QC_WCSS_DBGEN_DISABLE_SHFT                                              0x5
#define HWIO_FEATURE_CONFIG2_QC_MSS_NIDEN_DISABLE_BMSK                                              0x10
#define HWIO_FEATURE_CONFIG2_QC_MSS_NIDEN_DISABLE_SHFT                                               0x4
#define HWIO_FEATURE_CONFIG2_QC_MSS_DBGEN_DISABLE_BMSK                                               0x8
#define HWIO_FEATURE_CONFIG2_QC_MSS_DBGEN_DISABLE_SHFT                                               0x3
#define HWIO_FEATURE_CONFIG2_QDI_SPMI_DISABLE_BMSK                                                   0x4
#define HWIO_FEATURE_CONFIG2_QDI_SPMI_DISABLE_SHFT                                                   0x2
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_BMSK                                                    0x2
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_SHFT                                                    0x1
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_BMSK                                                        0x1
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_SHFT                                                        0x0

#define HWIO_FEATURE_CONFIG3_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006010)
#define HWIO_FEATURE_CONFIG3_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG3_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG3_ADDR, HWIO_FEATURE_CONFIG3_RMSK)
#define HWIO_FEATURE_CONFIG3_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG3_ADDR, m)
#define HWIO_FEATURE_CONFIG3_SEC_TAP_ACCESS_DISABLE_BMSK                                      0xf8000000
#define HWIO_FEATURE_CONFIG3_SEC_TAP_ACCESS_DISABLE_SHFT                                            0x1b
#define HWIO_FEATURE_CONFIG3_TAP_CJI_CORE_SEL_DISABLE_BMSK                                     0x4000000
#define HWIO_FEATURE_CONFIG3_TAP_CJI_CORE_SEL_DISABLE_SHFT                                          0x1a
#define HWIO_FEATURE_CONFIG3_TAP_INSTR_DISABLE_BMSK                                            0x3ffc000
#define HWIO_FEATURE_CONFIG3_TAP_INSTR_DISABLE_SHFT                                                  0xe
#define HWIO_FEATURE_CONFIG3_SPARE1_BMSK                                                          0x3c00
#define HWIO_FEATURE_CONFIG3_SPARE1_SHFT                                                             0xa
#define HWIO_FEATURE_CONFIG3_MODEM_PBL_PATCH_VERSION_BMSK                                          0x3e0
#define HWIO_FEATURE_CONFIG3_MODEM_PBL_PATCH_VERSION_SHFT                                            0x5
#define HWIO_FEATURE_CONFIG3_APPS_PBL_PATCH_VERSION_BMSK                                            0x1f
#define HWIO_FEATURE_CONFIG3_APPS_PBL_PATCH_VERSION_SHFT                                             0x0

#define HWIO_FEATURE_CONFIG4_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006014)
#define HWIO_FEATURE_CONFIG4_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG4_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG4_ADDR, HWIO_FEATURE_CONFIG4_RMSK)
#define HWIO_FEATURE_CONFIG4_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG4_ADDR, m)
#define HWIO_FEATURE_CONFIG4_TAP_GEN_SPARE_INSTR_DISABLE_31_0_BMSK                            0xffffffff
#define HWIO_FEATURE_CONFIG4_TAP_GEN_SPARE_INSTR_DISABLE_31_0_SHFT                                   0x0

#define HWIO_FEATURE_CONFIG5_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006018)
#define HWIO_FEATURE_CONFIG5_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG5_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG5_ADDR, HWIO_FEATURE_CONFIG5_RMSK)
#define HWIO_FEATURE_CONFIG5_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG5_ADDR, m)
#define HWIO_FEATURE_CONFIG5_SPARE3_BMSK                                                      0xf8000000
#define HWIO_FEATURE_CONFIG5_SPARE3_SHFT                                                            0x1b
#define HWIO_FEATURE_CONFIG5_MODEM_PBL_PLL_CTRL_BMSK                                           0x7800000
#define HWIO_FEATURE_CONFIG5_MODEM_PBL_PLL_CTRL_SHFT                                                0x17
#define HWIO_FEATURE_CONFIG5_APPS_PBL_PLL_CTRL_BMSK                                             0x780000
#define HWIO_FEATURE_CONFIG5_APPS_PBL_PLL_CTRL_SHFT                                                 0x13
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_FSM_CFG_BMSK                                              0x40000
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_FSM_CFG_SHFT                                                 0x12
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_FSM_DELAY_BMSK                                            0x30000
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_FSM_DELAY_SHFT                                               0x10
#define HWIO_FEATURE_CONFIG5_SPARE1_BMSK                                                          0x8000
#define HWIO_FEATURE_CONFIG5_SPARE1_SHFT                                                             0xf
#define HWIO_FEATURE_CONFIG5_USB_SS_ENABLE_BMSK                                                   0x4000
#define HWIO_FEATURE_CONFIG5_USB_SS_ENABLE_SHFT                                                      0xe
#define HWIO_FEATURE_CONFIG5_APPS_PBL_BOOT_SPEED_BMSK                                             0x3000
#define HWIO_FEATURE_CONFIG5_APPS_PBL_BOOT_SPEED_SHFT                                                0xc
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_FROM_ROM_BMSK                                               0x800
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_FROM_ROM_SHFT                                                 0xb
#define HWIO_FEATURE_CONFIG5_MSA_ENA_BMSK                                                          0x400
#define HWIO_FEATURE_CONFIG5_MSA_ENA_SHFT                                                            0xa
#define HWIO_FEATURE_CONFIG5_FORCE_MSA_AUTH_EN_BMSK                                                0x200
#define HWIO_FEATURE_CONFIG5_FORCE_MSA_AUTH_EN_SHFT                                                  0x9
#define HWIO_FEATURE_CONFIG5_SPARE_8_7_BMSK                                                        0x180
#define HWIO_FEATURE_CONFIG5_SPARE_8_7_SHFT                                                          0x7
#define HWIO_FEATURE_CONFIG5_MODEM_BOOT_FROM_ROM_BMSK                                               0x40
#define HWIO_FEATURE_CONFIG5_MODEM_BOOT_FROM_ROM_SHFT                                                0x6
#define HWIO_FEATURE_CONFIG5_BOOT_ROM_PATCH_DISABLE_BMSK                                            0x20
#define HWIO_FEATURE_CONFIG5_BOOT_ROM_PATCH_DISABLE_SHFT                                             0x5
#define HWIO_FEATURE_CONFIG5_TAP_GEN_SPARE_INSTR_DISABLE_36_32_BMSK                                 0x1f
#define HWIO_FEATURE_CONFIG5_TAP_GEN_SPARE_INSTR_DISABLE_36_32_SHFT                                  0x0

#define HWIO_OEM_CONFIG0_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000601c)
#define HWIO_OEM_CONFIG0_RMSK                                                                 0xffffffff
#define HWIO_OEM_CONFIG0_IN          \
        in_dword_masked(HWIO_OEM_CONFIG0_ADDR, HWIO_OEM_CONFIG0_RMSK)
#define HWIO_OEM_CONFIG0_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG0_ADDR, m)
#define HWIO_OEM_CONFIG0_SPARE0_BMSK                                                          0xff000000
#define HWIO_OEM_CONFIG0_SPARE0_SHFT                                                                0x18
#define HWIO_OEM_CONFIG0_SPARE_REG21_SECURE_BMSK                                                0x800000
#define HWIO_OEM_CONFIG0_SPARE_REG21_SECURE_SHFT                                                    0x17
#define HWIO_OEM_CONFIG0_PBL_LOG_DISABLE_BMSK                                                   0x400000
#define HWIO_OEM_CONFIG0_PBL_LOG_DISABLE_SHFT                                                       0x16
#define HWIO_OEM_CONFIG0_ROOT_CERT_TOTAL_NUM_BMSK                                               0x3c0000
#define HWIO_OEM_CONFIG0_ROOT_CERT_TOTAL_NUM_SHFT                                                   0x12
#define HWIO_OEM_CONFIG0_SPARE_REG20_SECURE_BMSK                                                 0x20000
#define HWIO_OEM_CONFIG0_SPARE_REG20_SECURE_SHFT                                                    0x11
#define HWIO_OEM_CONFIG0_SPARE_REG19_SECURE_BMSK                                                 0x10000
#define HWIO_OEM_CONFIG0_SPARE_REG19_SECURE_SHFT                                                    0x10
#define HWIO_OEM_CONFIG0_SPARE_REG18_SECURE_BMSK                                                  0x8000
#define HWIO_OEM_CONFIG0_SPARE_REG18_SECURE_SHFT                                                     0xf
#define HWIO_OEM_CONFIG0_SPARE_REG17_SECURE_BMSK                                                  0x4000
#define HWIO_OEM_CONFIG0_SPARE_REG17_SECURE_SHFT                                                     0xe
#define HWIO_OEM_CONFIG0_SPARE_REG16_SECURE_BMSK                                                  0x2000
#define HWIO_OEM_CONFIG0_SPARE_REG16_SECURE_SHFT                                                     0xd
#define HWIO_OEM_CONFIG0_WDOG_EN_BMSK                                                             0x1000
#define HWIO_OEM_CONFIG0_WDOG_EN_SHFT                                                                0xc
#define HWIO_OEM_CONFIG0_SPDM_SECURE_MODE_BMSK                                                     0x800
#define HWIO_OEM_CONFIG0_SPDM_SECURE_MODE_SHFT                                                       0xb
#define HWIO_OEM_CONFIG0_ALT_SD_PORT_FOR_BOOT_BMSK                                                 0x400
#define HWIO_OEM_CONFIG0_ALT_SD_PORT_FOR_BOOT_SHFT                                                   0xa
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_GPIO_DISABLE_BMSK                                        0x200
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_GPIO_DISABLE_SHFT                                          0x9
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_EN_BMSK                                                  0x100
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_EN_SHFT                                                    0x8
#define HWIO_OEM_CONFIG0_FAST_BOOT_BMSK                                                             0xe0
#define HWIO_OEM_CONFIG0_FAST_BOOT_SHFT                                                              0x5
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_BMSK                                                   0x10
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_SHFT                                                    0x4
#define HWIO_OEM_CONFIG0_FORCE_DLOAD_DISABLE_BMSK                                                    0x8
#define HWIO_OEM_CONFIG0_FORCE_DLOAD_DISABLE_SHFT                                                    0x3
#define HWIO_OEM_CONFIG0_SPARE_BMSK                                                                  0x4
#define HWIO_OEM_CONFIG0_SPARE_SHFT                                                                  0x2
#define HWIO_OEM_CONFIG0_ENUM_TIMEOUT_BMSK                                                           0x2
#define HWIO_OEM_CONFIG0_ENUM_TIMEOUT_SHFT                                                           0x1
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_BMSK                                                        0x1
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_SHFT                                                        0x0

#define HWIO_OEM_CONFIG1_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006020)
#define HWIO_OEM_CONFIG1_RMSK                                                                 0xffffffff
#define HWIO_OEM_CONFIG1_IN          \
        in_dword_masked(HWIO_OEM_CONFIG1_ADDR, HWIO_OEM_CONFIG1_RMSK)
#define HWIO_OEM_CONFIG1_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG1_ADDR, m)
#define HWIO_OEM_CONFIG1_SPARE1_BMSK                                                          0xff800000
#define HWIO_OEM_CONFIG1_SPARE1_SHFT                                                                0x17
#define HWIO_OEM_CONFIG1_ANTI_ROLLBACK_FEATURE_EN_BMSK                                          0x780000
#define HWIO_OEM_CONFIG1_ANTI_ROLLBACK_FEATURE_EN_SHFT                                              0x13
#define HWIO_OEM_CONFIG1_DAP_DEVICEEN_DISABLE_BMSK                                               0x40000
#define HWIO_OEM_CONFIG1_DAP_DEVICEEN_DISABLE_SHFT                                                  0x12
#define HWIO_OEM_CONFIG1_DAP_SPNIDEN_DISABLE_BMSK                                                0x20000
#define HWIO_OEM_CONFIG1_DAP_SPNIDEN_DISABLE_SHFT                                                   0x11
#define HWIO_OEM_CONFIG1_DAP_SPIDEN_DISABLE_BMSK                                                 0x10000
#define HWIO_OEM_CONFIG1_DAP_SPIDEN_DISABLE_SHFT                                                    0x10
#define HWIO_OEM_CONFIG1_DAP_NIDEN_DISABLE_BMSK                                                   0x8000
#define HWIO_OEM_CONFIG1_DAP_NIDEN_DISABLE_SHFT                                                      0xf
#define HWIO_OEM_CONFIG1_DAP_DBGEN_DISABLE_BMSK                                                   0x4000
#define HWIO_OEM_CONFIG1_DAP_DBGEN_DISABLE_SHFT                                                      0xe
#define HWIO_OEM_CONFIG1_APPS_SPNIDEN_DISABLE_BMSK                                                0x2000
#define HWIO_OEM_CONFIG1_APPS_SPNIDEN_DISABLE_SHFT                                                   0xd
#define HWIO_OEM_CONFIG1_APPS_SPIDEN_DISABLE_BMSK                                                 0x1000
#define HWIO_OEM_CONFIG1_APPS_SPIDEN_DISABLE_SHFT                                                    0xc
#define HWIO_OEM_CONFIG1_APPS_NIDEN_DISABLE_BMSK                                                   0x800
#define HWIO_OEM_CONFIG1_APPS_NIDEN_DISABLE_SHFT                                                     0xb
#define HWIO_OEM_CONFIG1_APPS_DBGEN_DISABLE_BMSK                                                   0x400
#define HWIO_OEM_CONFIG1_APPS_DBGEN_DISABLE_SHFT                                                     0xa
#define HWIO_OEM_CONFIG1_SPARE1_DISABLE_BMSK                                                       0x200
#define HWIO_OEM_CONFIG1_SPARE1_DISABLE_SHFT                                                         0x9
#define HWIO_OEM_CONFIG1_SPARE0_DISABLE_BMSK                                                       0x100
#define HWIO_OEM_CONFIG1_SPARE0_DISABLE_SHFT                                                         0x8
#define HWIO_OEM_CONFIG1_VENUS_0_DBGEN_DISABLE_BMSK                                                 0x80
#define HWIO_OEM_CONFIG1_VENUS_0_DBGEN_DISABLE_SHFT                                                  0x7
#define HWIO_OEM_CONFIG1_RPM_DAPEN_DISABLE_BMSK                                                     0x40
#define HWIO_OEM_CONFIG1_RPM_DAPEN_DISABLE_SHFT                                                      0x6
#define HWIO_OEM_CONFIG1_RPM_WCSS_NIDEN_DISABLE_BMSK                                                0x20
#define HWIO_OEM_CONFIG1_RPM_WCSS_NIDEN_DISABLE_SHFT                                                 0x5
#define HWIO_OEM_CONFIG1_RPM_DBGEN_DISABLE_BMSK                                                     0x10
#define HWIO_OEM_CONFIG1_RPM_DBGEN_DISABLE_SHFT                                                      0x4
#define HWIO_OEM_CONFIG1_WCSS_DBGEN_DISABLE_BMSK                                                     0x8
#define HWIO_OEM_CONFIG1_WCSS_DBGEN_DISABLE_SHFT                                                     0x3
#define HWIO_OEM_CONFIG1_MSS_NIDEN_DISABLE_BMSK                                                      0x4
#define HWIO_OEM_CONFIG1_MSS_NIDEN_DISABLE_SHFT                                                      0x2
#define HWIO_OEM_CONFIG1_MSS_DBGEN_DISABLE_BMSK                                                      0x2
#define HWIO_OEM_CONFIG1_MSS_DBGEN_DISABLE_SHFT                                                      0x1
#define HWIO_OEM_CONFIG1_ALL_DEBUG_DISABLE_BMSK                                                      0x1
#define HWIO_OEM_CONFIG1_ALL_DEBUG_DISABLE_SHFT                                                      0x0

#define HWIO_OEM_CONFIG2_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006024)
#define HWIO_OEM_CONFIG2_RMSK                                                                 0xffffffff
#define HWIO_OEM_CONFIG2_IN          \
        in_dword_masked(HWIO_OEM_CONFIG2_ADDR, HWIO_OEM_CONFIG2_RMSK)
#define HWIO_OEM_CONFIG2_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG2_ADDR, m)
#define HWIO_OEM_CONFIG2_OEM_PRODUCT_ID_BMSK                                                  0xffff0000
#define HWIO_OEM_CONFIG2_OEM_PRODUCT_ID_SHFT                                                        0x10
#define HWIO_OEM_CONFIG2_OEM_HW_ID_BMSK                                                           0xffff
#define HWIO_OEM_CONFIG2_OEM_HW_ID_SHFT                                                              0x0

#define HWIO_OEM_CONFIG3_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006028)
#define HWIO_OEM_CONFIG3_RMSK                                                                 0xffffffff
#define HWIO_OEM_CONFIG3_IN          \
        in_dword_masked(HWIO_OEM_CONFIG3_ADDR, HWIO_OEM_CONFIG3_RMSK)
#define HWIO_OEM_CONFIG3_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG3_ADDR, m)
#define HWIO_OEM_CONFIG3_PERIPH_VID_BMSK                                                      0xffff0000
#define HWIO_OEM_CONFIG3_PERIPH_VID_SHFT                                                            0x10
#define HWIO_OEM_CONFIG3_PERIPH_PID_BMSK                                                          0xffff
#define HWIO_OEM_CONFIG3_PERIPH_PID_SHFT                                                             0x0

#define HWIO_BOOT_CONFIG_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000602c)
#define HWIO_BOOT_CONFIG_RMSK                                                                      0x3ff
#define HWIO_BOOT_CONFIG_IN          \
        in_dword_masked(HWIO_BOOT_CONFIG_ADDR, HWIO_BOOT_CONFIG_RMSK)
#define HWIO_BOOT_CONFIG_INM(m)      \
        in_dword_masked(HWIO_BOOT_CONFIG_ADDR, m)
#define HWIO_BOOT_CONFIG_USB_SS_ENABLE_BMSK                                                        0x200
#define HWIO_BOOT_CONFIG_USB_SS_ENABLE_SHFT                                                          0x9
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_BMSK                                                    0x100
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_SHFT                                                      0x8
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_BMSK                                                   0xc0
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_SHFT                                                    0x6
#define HWIO_BOOT_CONFIG_APPS_BOOT_FROM_ROM_BMSK                                                    0x20
#define HWIO_BOOT_CONFIG_APPS_BOOT_FROM_ROM_SHFT                                                     0x5
#define HWIO_BOOT_CONFIG_FAST_BOOT_BMSK                                                             0x1e
#define HWIO_BOOT_CONFIG_FAST_BOOT_SHFT                                                              0x1
#define HWIO_BOOT_CONFIG_WDOG_EN_BMSK                                                                0x1
#define HWIO_BOOT_CONFIG_WDOG_EN_SHFT                                                                0x0

#define HWIO_SECURE_BOOTn_ADDR(n)                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006030 + 0x4 * (n))
#define HWIO_SECURE_BOOTn_RMSK                                                                     0x17f
#define HWIO_SECURE_BOOTn_MAXn                                                                         7
#define HWIO_SECURE_BOOTn_INI(n)        \
        in_dword_masked(HWIO_SECURE_BOOTn_ADDR(n), HWIO_SECURE_BOOTn_RMSK)
#define HWIO_SECURE_BOOTn_INMI(n,mask)    \
        in_dword_masked(HWIO_SECURE_BOOTn_ADDR(n), mask)
#define HWIO_SECURE_BOOTn_FUSE_SRC_BMSK                                                            0x100
#define HWIO_SECURE_BOOTn_FUSE_SRC_SHFT                                                              0x8
#define HWIO_SECURE_BOOTn_USE_SERIAL_NUM_BMSK                                                       0x40
#define HWIO_SECURE_BOOTn_USE_SERIAL_NUM_SHFT                                                        0x6
#define HWIO_SECURE_BOOTn_AUTH_EN_BMSK                                                              0x20
#define HWIO_SECURE_BOOTn_AUTH_EN_SHFT                                                               0x5
#define HWIO_SECURE_BOOTn_PK_HASH_IN_FUSE_BMSK                                                      0x10
#define HWIO_SECURE_BOOTn_PK_HASH_IN_FUSE_SHFT                                                       0x4
#define HWIO_SECURE_BOOTn_ROM_PK_HASH_INDEX_BMSK                                                     0xf
#define HWIO_SECURE_BOOTn_ROM_PK_HASH_INDEX_SHFT                                                     0x0

#define HWIO_OVERRIDE_0_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006058)
#define HWIO_OVERRIDE_0_RMSK                                                                  0x10000003
#define HWIO_OVERRIDE_0_IN          \
        in_dword_masked(HWIO_OVERRIDE_0_ADDR, HWIO_OVERRIDE_0_RMSK)
#define HWIO_OVERRIDE_0_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_0_ADDR, m)
#define HWIO_OVERRIDE_0_OUT(v)      \
        out_dword(HWIO_OVERRIDE_0_ADDR,v)
#define HWIO_OVERRIDE_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_0_ADDR,m,v,HWIO_OVERRIDE_0_IN)
#define HWIO_OVERRIDE_0_TX_DISABLE_BMSK                                                       0x10000000
#define HWIO_OVERRIDE_0_TX_DISABLE_SHFT                                                             0x1c
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_EN_BMSK                                                     0x2
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_EN_SHFT                                                     0x1
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_OVERRIDE_BMSK                                               0x1
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_OVERRIDE_SHFT                                               0x0

#define HWIO_OVERRIDE_1_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000605c)
#define HWIO_OVERRIDE_1_RMSK                                                                  0xffffffff
#define HWIO_OVERRIDE_1_IN          \
        in_dword_masked(HWIO_OVERRIDE_1_ADDR, HWIO_OVERRIDE_1_RMSK)
#define HWIO_OVERRIDE_1_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_1_ADDR, m)
#define HWIO_OVERRIDE_1_OUT(v)      \
        out_dword(HWIO_OVERRIDE_1_ADDR,v)
#define HWIO_OVERRIDE_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_1_ADDR,m,v,HWIO_OVERRIDE_1_IN)
#define HWIO_OVERRIDE_1_OVERRIDE_1_BMSK                                                       0xffffffff
#define HWIO_OVERRIDE_1_OVERRIDE_1_SHFT                                                              0x0

#define HWIO_OVERRIDE_2_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006060)
#define HWIO_OVERRIDE_2_RMSK                                                                    0x3ffc00
#define HWIO_OVERRIDE_2_IN          \
        in_dword_masked(HWIO_OVERRIDE_2_ADDR, HWIO_OVERRIDE_2_RMSK)
#define HWIO_OVERRIDE_2_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_2_ADDR, m)
#define HWIO_OVERRIDE_2_OUT(v)      \
        out_dword(HWIO_OVERRIDE_2_ADDR,v)
#define HWIO_OVERRIDE_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_2_ADDR,m,v,HWIO_OVERRIDE_2_IN)
#define HWIO_OVERRIDE_2_OVRID_DAP_DEVICEEN_DISABLE_BMSK                                         0x200000
#define HWIO_OVERRIDE_2_OVRID_DAP_DEVICEEN_DISABLE_SHFT                                             0x15
#define HWIO_OVERRIDE_2_OVRID_DAP_NIDEN_DISABLE_BMSK                                            0x100000
#define HWIO_OVERRIDE_2_OVRID_DAP_NIDEN_DISABLE_SHFT                                                0x14
#define HWIO_OVERRIDE_2_OVRID_DAP_DBGEN_DISABLE_BMSK                                             0x80000
#define HWIO_OVERRIDE_2_OVRID_DAP_DBGEN_DISABLE_SHFT                                                0x13
#define HWIO_OVERRIDE_2_OVRID_APPS_NIDEN_DISABLE_BMSK                                            0x40000
#define HWIO_OVERRIDE_2_OVRID_APPS_NIDEN_DISABLE_SHFT                                               0x12
#define HWIO_OVERRIDE_2_OVRID_APPS_DBGEN_DISABLE_BMSK                                            0x20000
#define HWIO_OVERRIDE_2_OVRID_APPS_DBGEN_DISABLE_SHFT                                               0x11
#define HWIO_OVERRIDE_2_OVRID_SPARE1_DISABLE_BMSK                                                0x10000
#define HWIO_OVERRIDE_2_OVRID_SPARE1_DISABLE_SHFT                                                   0x10
#define HWIO_OVERRIDE_2_OVRID_SPARE0_DISABLE_BMSK                                                 0x8000
#define HWIO_OVERRIDE_2_OVRID_SPARE0_DISABLE_SHFT                                                    0xf
#define HWIO_OVERRIDE_2_OVRID_VENUS_0_DBGEN_DISABLE_BMSK                                          0x4000
#define HWIO_OVERRIDE_2_OVRID_VENUS_0_DBGEN_DISABLE_SHFT                                             0xe
#define HWIO_OVERRIDE_2_OVRID_RPM_DAPEN_DISABLE_BMSK                                              0x2000
#define HWIO_OVERRIDE_2_OVRID_RPM_DAPEN_DISABLE_SHFT                                                 0xd
#define HWIO_OVERRIDE_2_OVRID_RPM_WCSS_NIDEN_DISABLE_BMSK                                         0x1000
#define HWIO_OVERRIDE_2_OVRID_RPM_WCSS_NIDEN_DISABLE_SHFT                                            0xc
#define HWIO_OVERRIDE_2_OVRID_RPM_DBGEN_DISABLE_BMSK                                               0x800
#define HWIO_OVERRIDE_2_OVRID_RPM_DBGEN_DISABLE_SHFT                                                 0xb
#define HWIO_OVERRIDE_2_OVRID_WCSS_DBGEN_DISABLE_BMSK                                              0x400
#define HWIO_OVERRIDE_2_OVRID_WCSS_DBGEN_DISABLE_SHFT                                                0xa

#define HWIO_OVERRIDE_3_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006064)
#define HWIO_OVERRIDE_3_RMSK                                                                        0x1f
#define HWIO_OVERRIDE_3_IN          \
        in_dword_masked(HWIO_OVERRIDE_3_ADDR, HWIO_OVERRIDE_3_RMSK)
#define HWIO_OVERRIDE_3_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_3_ADDR, m)
#define HWIO_OVERRIDE_3_OUT(v)      \
        out_dword(HWIO_OVERRIDE_3_ADDR,v)
#define HWIO_OVERRIDE_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_3_ADDR,m,v,HWIO_OVERRIDE_3_IN)
#define HWIO_OVERRIDE_3_OVRID_DAP_SPNIDEN_DISABLE_BMSK                                              0x10
#define HWIO_OVERRIDE_3_OVRID_DAP_SPNIDEN_DISABLE_SHFT                                               0x4
#define HWIO_OVERRIDE_3_OVRID_DAP_SPIDEN_DISABLE_BMSK                                                0x8
#define HWIO_OVERRIDE_3_OVRID_DAP_SPIDEN_DISABLE_SHFT                                                0x3
#define HWIO_OVERRIDE_3_OVRID_APPS_SPNIDEN_DISABLE_BMSK                                              0x4
#define HWIO_OVERRIDE_3_OVRID_APPS_SPNIDEN_DISABLE_SHFT                                              0x2
#define HWIO_OVERRIDE_3_OVRID_APPS_SPIDEN_DISABLE_BMSK                                               0x2
#define HWIO_OVERRIDE_3_OVRID_APPS_SPIDEN_DISABLE_SHFT                                               0x1
#define HWIO_OVERRIDE_3_OVRID_SPDM_SECURE_MODE_BMSK                                                  0x1
#define HWIO_OVERRIDE_3_OVRID_SPDM_SECURE_MODE_SHFT                                                  0x0

#define HWIO_OVERRIDE_4_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006068)
#define HWIO_OVERRIDE_4_RMSK                                                                         0x3
#define HWIO_OVERRIDE_4_IN          \
        in_dword_masked(HWIO_OVERRIDE_4_ADDR, HWIO_OVERRIDE_4_RMSK)
#define HWIO_OVERRIDE_4_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_4_ADDR, m)
#define HWIO_OVERRIDE_4_OUT(v)      \
        out_dword(HWIO_OVERRIDE_4_ADDR,v)
#define HWIO_OVERRIDE_4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_4_ADDR,m,v,HWIO_OVERRIDE_4_IN)
#define HWIO_OVERRIDE_4_OVRID_MSS_NIDEN_DISABLE_BMSK                                                 0x2
#define HWIO_OVERRIDE_4_OVRID_MSS_NIDEN_DISABLE_SHFT                                                 0x1
#define HWIO_OVERRIDE_4_OVRID_MSS_DBGEN_DISABLE_BMSK                                                 0x1
#define HWIO_OVERRIDE_4_OVRID_MSS_DBGEN_DISABLE_SHFT                                                 0x0

#define HWIO_CAPT_SEC_GPIO_ADDR                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000606c)
#define HWIO_CAPT_SEC_GPIO_RMSK                                                                  0x1ffff
#define HWIO_CAPT_SEC_GPIO_IN          \
        in_dword_masked(HWIO_CAPT_SEC_GPIO_ADDR, HWIO_CAPT_SEC_GPIO_RMSK)
#define HWIO_CAPT_SEC_GPIO_INM(m)      \
        in_dword_masked(HWIO_CAPT_SEC_GPIO_ADDR, m)
#define HWIO_CAPT_SEC_GPIO_OUT(v)      \
        out_dword(HWIO_CAPT_SEC_GPIO_ADDR,v)
#define HWIO_CAPT_SEC_GPIO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CAPT_SEC_GPIO_ADDR,m,v,HWIO_CAPT_SEC_GPIO_IN)
#define HWIO_CAPT_SEC_GPIO_SDC_EMMC_MODE1P2_EN_BMSK                                              0x10000
#define HWIO_CAPT_SEC_GPIO_SDC_EMMC_MODE1P2_EN_SHFT                                                 0x10
#define HWIO_CAPT_SEC_GPIO_FORCE_USB_BOOT_GPIO_BMSK                                               0x8000
#define HWIO_CAPT_SEC_GPIO_FORCE_USB_BOOT_GPIO_SHFT                                                  0xf
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_BMSK                                                  0x7fff
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_SHFT                                                     0x0

#define HWIO_APP_PROC_CFG_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006070)
#define HWIO_APP_PROC_CFG_RMSK                                                                  0xffffff
#define HWIO_APP_PROC_CFG_IN          \
        in_dword_masked(HWIO_APP_PROC_CFG_ADDR, HWIO_APP_PROC_CFG_RMSK)
#define HWIO_APP_PROC_CFG_INM(m)      \
        in_dword_masked(HWIO_APP_PROC_CFG_ADDR, m)
#define HWIO_APP_PROC_CFG_OUT(v)      \
        out_dword(HWIO_APP_PROC_CFG_ADDR,v)
#define HWIO_APP_PROC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APP_PROC_CFG_ADDR,m,v,HWIO_APP_PROC_CFG_IN)
#define HWIO_APP_PROC_CFG_WCSS_DBG_NIDEN_BMSK                                                   0x800000
#define HWIO_APP_PROC_CFG_WCSS_DBG_NIDEN_SHFT                                                       0x17
#define HWIO_APP_PROC_CFG_RPM_DBG_NIDEN_BMSK                                                    0x400000
#define HWIO_APP_PROC_CFG_RPM_DBG_NIDEN_SHFT                                                        0x16
#define HWIO_APP_PROC_CFG_DAP_DBG_NIDEN_BMSK                                                    0x200000
#define HWIO_APP_PROC_CFG_DAP_DBG_NIDEN_SHFT                                                        0x15
#define HWIO_APP_PROC_CFG_DAP_DBG_SPNIDEN_BMSK                                                  0x100000
#define HWIO_APP_PROC_CFG_DAP_DBG_SPNIDEN_SHFT                                                      0x14
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC3_BMSK                                                    0xf0000
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC3_SHFT                                                       0x10
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC2_BMSK                                                     0xf000
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC2_SHFT                                                        0xc
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC1_BMSK                                                      0xf00
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC1_SHFT                                                        0x8
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC0_BMSK                                                       0xf0
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC0_SHFT                                                        0x4
#define HWIO_APP_PROC_CFG_APPS_DBG_NIDEN_BMSK                                                        0x8
#define HWIO_APP_PROC_CFG_APPS_DBG_NIDEN_SHFT                                                        0x3
#define HWIO_APP_PROC_CFG_APPS_DBG_SPNIDEN_BMSK                                                      0x4
#define HWIO_APP_PROC_CFG_APPS_DBG_SPNIDEN_SHFT                                                      0x2
#define HWIO_APP_PROC_CFG_APPS_CP15_DISABLE_BMSK                                                     0x2
#define HWIO_APP_PROC_CFG_APPS_CP15_DISABLE_SHFT                                                     0x1
#define HWIO_APP_PROC_CFG_APPS_CFG_NMFI_BMSK                                                         0x1
#define HWIO_APP_PROC_CFG_APPS_CFG_NMFI_SHFT                                                         0x0

#define HWIO_MSS_PROC_CFG_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006074)
#define HWIO_MSS_PROC_CFG_RMSK                                                                       0x1
#define HWIO_MSS_PROC_CFG_IN          \
        in_dword_masked(HWIO_MSS_PROC_CFG_ADDR, HWIO_MSS_PROC_CFG_RMSK)
#define HWIO_MSS_PROC_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_PROC_CFG_ADDR, m)
#define HWIO_MSS_PROC_CFG_OUT(v)      \
        out_dword(HWIO_MSS_PROC_CFG_ADDR,v)
#define HWIO_MSS_PROC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_PROC_CFG_ADDR,m,v,HWIO_MSS_PROC_CFG_IN)
#define HWIO_MSS_PROC_CFG_MSS_DBG_NIDEN_BMSK                                                         0x1
#define HWIO_MSS_PROC_CFG_MSS_DBG_NIDEN_SHFT                                                         0x0

#define HWIO_QFPROM_CLK_CTL_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006078)
#define HWIO_QFPROM_CLK_CTL_RMSK                                                                     0x1
#define HWIO_QFPROM_CLK_CTL_IN          \
        in_dword_masked(HWIO_QFPROM_CLK_CTL_ADDR, HWIO_QFPROM_CLK_CTL_RMSK)
#define HWIO_QFPROM_CLK_CTL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CLK_CTL_ADDR, m)
#define HWIO_QFPROM_CLK_CTL_OUT(v)      \
        out_dword(HWIO_QFPROM_CLK_CTL_ADDR,v)
#define HWIO_QFPROM_CLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CLK_CTL_ADDR,m,v,HWIO_QFPROM_CLK_CTL_IN)
#define HWIO_QFPROM_CLK_CTL_CLK_HALT_BMSK                                                            0x1
#define HWIO_QFPROM_CLK_CTL_CLK_HALT_SHFT                                                            0x0

#define HWIO_JTAG_ID_ADDR                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000607c)
#define HWIO_JTAG_ID_RMSK                                                                     0xffffffff
#define HWIO_JTAG_ID_IN          \
        in_dword_masked(HWIO_JTAG_ID_ADDR, HWIO_JTAG_ID_RMSK)
#define HWIO_JTAG_ID_INM(m)      \
        in_dword_masked(HWIO_JTAG_ID_ADDR, m)
#define HWIO_JTAG_ID_JTAG_ID_BMSK                                                             0xffffffff
#define HWIO_JTAG_ID_JTAG_ID_SHFT                                                                    0x0

#define HWIO_OEM_ID_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006080)
#define HWIO_OEM_ID_RMSK                                                                      0xffffffff
#define HWIO_OEM_ID_IN          \
        in_dword_masked(HWIO_OEM_ID_ADDR, HWIO_OEM_ID_RMSK)
#define HWIO_OEM_ID_INM(m)      \
        in_dword_masked(HWIO_OEM_ID_ADDR, m)
#define HWIO_OEM_ID_OEM_ID_BMSK                                                               0xffff0000
#define HWIO_OEM_ID_OEM_ID_SHFT                                                                     0x10
#define HWIO_OEM_ID_OEM_PRODUCT_ID_BMSK                                                           0xffff
#define HWIO_OEM_ID_OEM_PRODUCT_ID_SHFT                                                              0x0

#define HWIO_TEST_BUS_SEL_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006084)
#define HWIO_TEST_BUS_SEL_RMSK                                                                       0xf
#define HWIO_TEST_BUS_SEL_IN          \
        in_dword_masked(HWIO_TEST_BUS_SEL_ADDR, HWIO_TEST_BUS_SEL_RMSK)
#define HWIO_TEST_BUS_SEL_INM(m)      \
        in_dword_masked(HWIO_TEST_BUS_SEL_ADDR, m)
#define HWIO_TEST_BUS_SEL_OUT(v)      \
        out_dword(HWIO_TEST_BUS_SEL_ADDR,v)
#define HWIO_TEST_BUS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TEST_BUS_SEL_ADDR,m,v,HWIO_TEST_BUS_SEL_IN)
#define HWIO_TEST_BUS_SEL_TEST_EN_BMSK                                                               0x8
#define HWIO_TEST_BUS_SEL_TEST_EN_SHFT                                                               0x3
#define HWIO_TEST_BUS_SEL_TEST_SELECT_BMSK                                                           0x7
#define HWIO_TEST_BUS_SEL_TEST_SELECT_SHFT                                                           0x0

#define HWIO_SPDM_DYN_SECURE_MODE_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006088)
#define HWIO_SPDM_DYN_SECURE_MODE_RMSK                                                               0x1
#define HWIO_SPDM_DYN_SECURE_MODE_IN          \
        in_dword_masked(HWIO_SPDM_DYN_SECURE_MODE_ADDR, HWIO_SPDM_DYN_SECURE_MODE_RMSK)
#define HWIO_SPDM_DYN_SECURE_MODE_INM(m)      \
        in_dword_masked(HWIO_SPDM_DYN_SECURE_MODE_ADDR, m)
#define HWIO_SPDM_DYN_SECURE_MODE_OUT(v)      \
        out_dword(HWIO_SPDM_DYN_SECURE_MODE_ADDR,v)
#define HWIO_SPDM_DYN_SECURE_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPDM_DYN_SECURE_MODE_ADDR,m,v,HWIO_SPDM_DYN_SECURE_MODE_IN)
#define HWIO_SPDM_DYN_SECURE_MODE_SECURE_MODE_BMSK                                                   0x1
#define HWIO_SPDM_DYN_SECURE_MODE_SECURE_MODE_SHFT                                                   0x0


#endif /* __MSS_CPR_HWIO_H__ */
