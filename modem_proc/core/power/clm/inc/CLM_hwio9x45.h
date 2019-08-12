#ifndef __CLM_HWIO9X45_H__
#define __CLM_HWIO9X45_H__
/*
===========================================================================
*/
/**
  @file CLM_hwio9x45.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    MSS_QDSP6SS_PUB

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

  $Header: //components/rel/core.mpss/3.5.c12.3/power/clm/inc/CLM_hwio9x45.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: MSS_QDSP6SS_PUB
 *--------------------------------------------------------------------------*/

#define MSS_QDSP6SS_PUB_REG_BASE                                                (MODEM_TOP_BASE      + 0x00080000)

#define HWIO_MSS_QDSP6SS_VERSION_ADDR                                           (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000000)
#define HWIO_MSS_QDSP6SS_VERSION_RMSK                                           0xffffffff
#define HWIO_MSS_QDSP6SS_VERSION_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_VERSION_ADDR, HWIO_MSS_QDSP6SS_VERSION_RMSK)
#define HWIO_MSS_QDSP6SS_VERSION_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_VERSION_ADDR, m)
#define HWIO_MSS_QDSP6SS_VERSION_MAJOR_BMSK                                     0xf0000000
#define HWIO_MSS_QDSP6SS_VERSION_MAJOR_SHFT                                           0x1c
#define HWIO_MSS_QDSP6SS_VERSION_MINOR_BMSK                                      0xfff0000
#define HWIO_MSS_QDSP6SS_VERSION_MINOR_SHFT                                           0x10
#define HWIO_MSS_QDSP6SS_VERSION_STEP_BMSK                                          0xffff
#define HWIO_MSS_QDSP6SS_VERSION_STEP_SHFT                                             0x0

#define HWIO_MSS_QDSP6SS_CONFIG_ID_ADDR                                         (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000004)
#define HWIO_MSS_QDSP6SS_CONFIG_ID_RMSK                                         0xfff1ffff
#define HWIO_MSS_QDSP6SS_CONFIG_ID_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_CONFIG_ID_ADDR, HWIO_MSS_QDSP6SS_CONFIG_ID_RMSK)
#define HWIO_MSS_QDSP6SS_CONFIG_ID_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_CONFIG_ID_ADDR, m)
#define HWIO_MSS_QDSP6SS_CONFIG_ID_NUM_L2VIC_SRC_BMSK                           0xfff00000
#define HWIO_MSS_QDSP6SS_CONFIG_ID_NUM_L2VIC_SRC_SHFT                                 0x14
#define HWIO_MSS_QDSP6SS_CONFIG_ID_ECS_PRESENT_BMSK                                0x10000
#define HWIO_MSS_QDSP6SS_CONFIG_ID_ECS_PRESENT_SHFT                                   0x10
#define HWIO_MSS_QDSP6SS_CONFIG_ID_PMIC_IF_PRESENT_BMSK                             0x8000
#define HWIO_MSS_QDSP6SS_CONFIG_ID_PMIC_IF_PRESENT_SHFT                                0xf
#define HWIO_MSS_QDSP6SS_CONFIG_ID_CORE_ON_VSSCCX_BMSK                              0x4000
#define HWIO_MSS_QDSP6SS_CONFIG_ID_CORE_ON_VSSCCX_SHFT                                 0xe
#define HWIO_MSS_QDSP6SS_CONFIG_ID_CORE_ON_VDDCX_BMSK                               0x2000
#define HWIO_MSS_QDSP6SS_CONFIG_ID_CORE_ON_VDDCX_SHFT                                  0xd
#define HWIO_MSS_QDSP6SS_CONFIG_ID_APU_PRESENT_BMSK                                 0x1000
#define HWIO_MSS_QDSP6SS_CONFIG_ID_APU_PRESENT_SHFT                                    0xc
#define HWIO_MSS_QDSP6SS_CONFIG_ID_LDO_PRESENT_BMSK                                  0x800
#define HWIO_MSS_QDSP6SS_CONFIG_ID_LDO_PRESENT_SHFT                                    0xb
#define HWIO_MSS_QDSP6SS_CONFIG_ID_BHS_PRESENT_BMSK                                  0x400
#define HWIO_MSS_QDSP6SS_CONFIG_ID_BHS_PRESENT_SHFT                                    0xa
#define HWIO_MSS_QDSP6SS_CONFIG_ID_PLL_VOTE_BMSK                                     0x200
#define HWIO_MSS_QDSP6SS_CONFIG_ID_PLL_VOTE_SHFT                                       0x9
#define HWIO_MSS_QDSP6SS_CONFIG_ID_PLL_PRESENT_BMSK                                  0x100
#define HWIO_MSS_QDSP6SS_CONFIG_ID_PLL_PRESENT_SHFT                                    0x8
#define HWIO_MSS_QDSP6SS_CONFIG_ID_CORE_TYPE_BMSK                                     0xf0
#define HWIO_MSS_QDSP6SS_CONFIG_ID_CORE_TYPE_SHFT                                      0x4
#define HWIO_MSS_QDSP6SS_CONFIG_ID_L2_SIZE_BMSK                                        0xf
#define HWIO_MSS_QDSP6SS_CONFIG_ID_L2_SIZE_SHFT                                        0x0

#define HWIO_MSS_QDSP6SS_RST_EVB_ADDR                                           (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000010)
#define HWIO_MSS_QDSP6SS_RST_EVB_RMSK                                            0xffffff0
#define HWIO_MSS_QDSP6SS_RST_EVB_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_RST_EVB_ADDR, HWIO_MSS_QDSP6SS_RST_EVB_RMSK)
#define HWIO_MSS_QDSP6SS_RST_EVB_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_RST_EVB_ADDR, m)
#define HWIO_MSS_QDSP6SS_RST_EVB_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_RST_EVB_ADDR,v)
#define HWIO_MSS_QDSP6SS_RST_EVB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_RST_EVB_ADDR,m,v,HWIO_MSS_QDSP6SS_RST_EVB_IN)
#define HWIO_MSS_QDSP6SS_RST_EVB_EVB_BMSK                                        0xffffff0
#define HWIO_MSS_QDSP6SS_RST_EVB_EVB_SHFT                                              0x4

#define HWIO_MSS_QDSP6SS_RESET_ADDR                                             (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000014)
#define HWIO_MSS_QDSP6SS_RESET_RMSK                                                 0x1ff7
#define HWIO_MSS_QDSP6SS_RESET_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_RESET_ADDR, HWIO_MSS_QDSP6SS_RESET_RMSK)
#define HWIO_MSS_QDSP6SS_RESET_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_RESET_ADDR, m)
#define HWIO_MSS_QDSP6SS_RESET_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_RESET_ADDR,v)
#define HWIO_MSS_QDSP6SS_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_RESET_ADDR,m,v,HWIO_MSS_QDSP6SS_RESET_IN)
#define HWIO_MSS_QDSP6SS_RESET_CORE_ARCH_ARES_BMSK                                  0x1000
#define HWIO_MSS_QDSP6SS_RESET_CORE_ARCH_ARES_SHFT                                     0xc
#define HWIO_MSS_QDSP6SS_RESET_CORE_NONRET_ARES_BMSK                                 0x800
#define HWIO_MSS_QDSP6SS_RESET_CORE_NONRET_ARES_SHFT                                   0xb
#define HWIO_MSS_QDSP6SS_RESET_CORE_RET_ARES_BMSK                                    0x400
#define HWIO_MSS_QDSP6SS_RESET_CORE_RET_ARES_SHFT                                      0xa
#define HWIO_MSS_QDSP6SS_RESET_AXIM2_ARES_BMSK                                       0x200
#define HWIO_MSS_QDSP6SS_RESET_AXIM2_ARES_SHFT                                         0x9
#define HWIO_MSS_QDSP6SS_RESET_AXIS_ARES_BMSK                                        0x100
#define HWIO_MSS_QDSP6SS_RESET_AXIS_ARES_SHFT                                          0x8
#define HWIO_MSS_QDSP6SS_RESET_ATBM_ARES_BMSK                                         0x80
#define HWIO_MSS_QDSP6SS_RESET_ATBM_ARES_SHFT                                          0x7
#define HWIO_MSS_QDSP6SS_RESET_AHBM_ARES_BMSK                                         0x40
#define HWIO_MSS_QDSP6SS_RESET_AHBM_ARES_SHFT                                          0x6
#define HWIO_MSS_QDSP6SS_RESET_AXIM_ARES_BMSK                                         0x20
#define HWIO_MSS_QDSP6SS_RESET_AXIM_ARES_SHFT                                          0x5
#define HWIO_MSS_QDSP6SS_RESET_ETM_ISDB_ARES_BMSK                                     0x10
#define HWIO_MSS_QDSP6SS_RESET_ETM_ISDB_ARES_SHFT                                      0x4
#define HWIO_MSS_QDSP6SS_RESET_BUS_ARES_ENA_BMSK                                       0x4
#define HWIO_MSS_QDSP6SS_RESET_BUS_ARES_ENA_SHFT                                       0x2
#define HWIO_MSS_QDSP6SS_RESET_CORE_ARES_BMSK                                          0x2
#define HWIO_MSS_QDSP6SS_RESET_CORE_ARES_SHFT                                          0x1
#define HWIO_MSS_QDSP6SS_RESET_STOP_CORE_BMSK                                          0x1
#define HWIO_MSS_QDSP6SS_RESET_STOP_CORE_SHFT                                          0x0

#define HWIO_MSS_QDSP6SS_DBG_CFG_ADDR                                           (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000018)
#define HWIO_MSS_QDSP6SS_DBG_CFG_RMSK                                           0xffff0003
#define HWIO_MSS_QDSP6SS_DBG_CFG_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_DBG_CFG_ADDR, HWIO_MSS_QDSP6SS_DBG_CFG_RMSK)
#define HWIO_MSS_QDSP6SS_DBG_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_DBG_CFG_ADDR, m)
#define HWIO_MSS_QDSP6SS_DBG_CFG_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_DBG_CFG_ADDR,v)
#define HWIO_MSS_QDSP6SS_DBG_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_DBG_CFG_ADDR,m,v,HWIO_MSS_QDSP6SS_DBG_CFG_IN)
#define HWIO_MSS_QDSP6SS_DBG_CFG_DBG_SW_REG_BMSK                                0xff000000
#define HWIO_MSS_QDSP6SS_DBG_CFG_DBG_SW_REG_SHFT                                      0x18
#define HWIO_MSS_QDSP6SS_DBG_CFG_DBG_SPARE_BMSK                                   0xff0000
#define HWIO_MSS_QDSP6SS_DBG_CFG_DBG_SPARE_SHFT                                       0x10
#define HWIO_MSS_QDSP6SS_DBG_CFG_CTI_TIHS_ENA_BMSK                                     0x2
#define HWIO_MSS_QDSP6SS_DBG_CFG_CTI_TIHS_ENA_SHFT                                     0x1
#define HWIO_MSS_QDSP6SS_DBG_CFG_DBG_ARES_ENA_BMSK                                     0x1
#define HWIO_MSS_QDSP6SS_DBG_CFG_DBG_ARES_ENA_SHFT                                     0x0

#define HWIO_MSS_QDSP6SS_RET_CFG_ADDR                                           (MSS_QDSP6SS_PUB_REG_BASE      + 0x0000001c)
#define HWIO_MSS_QDSP6SS_RET_CFG_RMSK                                                  0x3
#define HWIO_MSS_QDSP6SS_RET_CFG_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_RET_CFG_ADDR, HWIO_MSS_QDSP6SS_RET_CFG_RMSK)
#define HWIO_MSS_QDSP6SS_RET_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_RET_CFG_ADDR, m)
#define HWIO_MSS_QDSP6SS_RET_CFG_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_RET_CFG_ADDR,v)
#define HWIO_MSS_QDSP6SS_RET_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_RET_CFG_ADDR,m,v,HWIO_MSS_QDSP6SS_RET_CFG_IN)
#define HWIO_MSS_QDSP6SS_RET_CFG_ARCH_ARES_ENA_BMSK                                    0x2
#define HWIO_MSS_QDSP6SS_RET_CFG_ARCH_ARES_ENA_SHFT                                    0x1
#define HWIO_MSS_QDSP6SS_RET_CFG_RET_ARES_ENA_BMSK                                     0x1
#define HWIO_MSS_QDSP6SS_RET_CFG_RET_ARES_ENA_SHFT                                     0x0

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

#define HWIO_MSS_QDSP6SS_PWR_CTL_ADDR                                           (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000030)
#define HWIO_MSS_QDSP6SS_PWR_CTL_RMSK                                            0x77fffff
#define HWIO_MSS_QDSP6SS_PWR_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PWR_CTL_ADDR, HWIO_MSS_QDSP6SS_PWR_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_PWR_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PWR_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_PWR_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PWR_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_PWR_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PWR_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_PWR_CTL_IN)
#define HWIO_MSS_QDSP6SS_PWR_CTL_LDO_PWR_UP_BMSK                                 0x4000000
#define HWIO_MSS_QDSP6SS_PWR_CTL_LDO_PWR_UP_SHFT                                      0x1a
#define HWIO_MSS_QDSP6SS_PWR_CTL_LDO_BYP_BMSK                                    0x2000000
#define HWIO_MSS_QDSP6SS_PWR_CTL_LDO_BYP_SHFT                                         0x19
#define HWIO_MSS_QDSP6SS_PWR_CTL_BHS_ON_BMSK                                     0x1000000
#define HWIO_MSS_QDSP6SS_PWR_CTL_BHS_ON_SHFT                                          0x18
#define HWIO_MSS_QDSP6SS_PWR_CTL_CLAMP_QMC_MEM_BMSK                               0x400000
#define HWIO_MSS_QDSP6SS_PWR_CTL_CLAMP_QMC_MEM_SHFT                                   0x16
#define HWIO_MSS_QDSP6SS_PWR_CTL_CLAMP_WL_BMSK                                    0x200000
#define HWIO_MSS_QDSP6SS_PWR_CTL_CLAMP_WL_SHFT                                        0x15
#define HWIO_MSS_QDSP6SS_PWR_CTL_CLAMP_IO_BMSK                                    0x100000
#define HWIO_MSS_QDSP6SS_PWR_CTL_CLAMP_IO_SHFT                                        0x14
#define HWIO_MSS_QDSP6SS_PWR_CTL_SLP_RET_N_BMSK                                    0x80000
#define HWIO_MSS_QDSP6SS_PWR_CTL_SLP_RET_N_SHFT                                       0x13
#define HWIO_MSS_QDSP6SS_PWR_CTL_L2DATA_STBY_N_BMSK                                0x40000
#define HWIO_MSS_QDSP6SS_PWR_CTL_L2DATA_STBY_N_SHFT                                   0x12
#define HWIO_MSS_QDSP6SS_PWR_CTL_ETB_SLP_NRET_N_BMSK                               0x20000
#define HWIO_MSS_QDSP6SS_PWR_CTL_ETB_SLP_NRET_N_SHFT                                  0x11
#define HWIO_MSS_QDSP6SS_PWR_CTL_L1IU_SLP_NRET_N_BMSK                              0x18000
#define HWIO_MSS_QDSP6SS_PWR_CTL_L1IU_SLP_NRET_N_SHFT                                  0xf
#define HWIO_MSS_QDSP6SS_PWR_CTL_L1DU_SLP_NRET_N_BMSK                               0x6000
#define HWIO_MSS_QDSP6SS_PWR_CTL_L1DU_SLP_NRET_N_SHFT                                  0xd
#define HWIO_MSS_QDSP6SS_PWR_CTL_L2PLRU_SLP_NRET_N_BMSK                             0x1000
#define HWIO_MSS_QDSP6SS_PWR_CTL_L2PLRU_SLP_NRET_N_SHFT                                0xc
#define HWIO_MSS_QDSP6SS_PWR_CTL_L2TAG_SLP_NRET_N_BMSK                               0xf00
#define HWIO_MSS_QDSP6SS_PWR_CTL_L2TAG_SLP_NRET_N_SHFT                                 0x8
#define HWIO_MSS_QDSP6SS_PWR_CTL_L2DATA_SLP_NRET_N_BMSK                               0xff
#define HWIO_MSS_QDSP6SS_PWR_CTL_L2DATA_SLP_NRET_N_SHFT                                0x0

#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_ADDR                                      (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000034)
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_RMSK                                          0xffff
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_CGC_OVERRIDE_ADDR, HWIO_MSS_QDSP6SS_CGC_OVERRIDE_RMSK)
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_CGC_OVERRIDE_ADDR, m)
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_CGC_OVERRIDE_ADDR,v)
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_CGC_OVERRIDE_ADDR,m,v,HWIO_MSS_QDSP6SS_CGC_OVERRIDE_IN)
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_CP_CLK_EN_BMSK                                0x8000
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_CP_CLK_EN_SHFT                                   0xf
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_AXIM2_CLK_EN_BMSK                             0x4000
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_AXIM2_CLK_EN_SHFT                                0xe
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_AXIS_CLK_EN_BMSK                              0x2000
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_AXIS_CLK_EN_SHFT                                 0xd
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_AXIM_CLK_EN_BMSK                              0x1000
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_AXIM_CLK_EN_SHFT                                 0xc
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_AHBM_CLK_EN_BMSK                               0x800
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_AHBM_CLK_EN_SHFT                                 0xb
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_AHBS_CLK_EN_BMSK                               0x400
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_AHBS_CLK_EN_SHFT                                 0xa
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_PRIV_AHBS_CLK_EN_BMSK                          0x300
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_PRIV_AHBS_CLK_EN_SHFT                            0x8
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_TCXO_CLK_EN_BMSK                                0x80
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_TCXO_CLK_EN_SHFT                                 0x7
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_L2VIC_AHBS_CLK_EN_BMSK                          0x60
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_L2VIC_AHBS_CLK_EN_SHFT                           0x5
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_TEST_AHBS_CLK_EN_BMSK                           0x10
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_TEST_AHBS_CLK_EN_SHFT                            0x4
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_PUB_AHBS_CLK_EN_BMSK                             0x8
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_PUB_AHBS_CLK_EN_SHFT                             0x3
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_CORE_BUS_EN_BMSK                                 0x4
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_CORE_BUS_EN_SHFT                                 0x2
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_CORE_RCLK_EN_BMSK                                0x2
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_CORE_RCLK_EN_SHFT                                0x1
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_CORE_CLK_EN_BMSK                                 0x1
#define HWIO_MSS_QDSP6SS_CGC_OVERRIDE_CORE_CLK_EN_SHFT                                 0x0

#define HWIO_MSS_QDSP6SS_XO_CBCR_ADDR                                           (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000038)
#define HWIO_MSS_QDSP6SS_XO_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_QDSP6SS_XO_CBCR_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_XO_CBCR_ADDR, HWIO_MSS_QDSP6SS_XO_CBCR_RMSK)
#define HWIO_MSS_QDSP6SS_XO_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_XO_CBCR_ADDR, m)
#define HWIO_MSS_QDSP6SS_XO_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_XO_CBCR_ADDR,v)
#define HWIO_MSS_QDSP6SS_XO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_XO_CBCR_ADDR,m,v,HWIO_MSS_QDSP6SS_XO_CBCR_IN)
#define HWIO_MSS_QDSP6SS_XO_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_QDSP6SS_XO_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_QDSP6SS_XO_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_QDSP6SS_XO_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_QDSP6SS_SLEEP_CBCR_ADDR                                        (MSS_QDSP6SS_PUB_REG_BASE      + 0x0000003c)
#define HWIO_MSS_QDSP6SS_SLEEP_CBCR_RMSK                                        0x80000001
#define HWIO_MSS_QDSP6SS_SLEEP_CBCR_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_SLEEP_CBCR_ADDR, HWIO_MSS_QDSP6SS_SLEEP_CBCR_RMSK)
#define HWIO_MSS_QDSP6SS_SLEEP_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_SLEEP_CBCR_ADDR, m)
#define HWIO_MSS_QDSP6SS_SLEEP_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_SLEEP_CBCR_ADDR,v)
#define HWIO_MSS_QDSP6SS_SLEEP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_SLEEP_CBCR_ADDR,m,v,HWIO_MSS_QDSP6SS_SLEEP_CBCR_IN)
#define HWIO_MSS_QDSP6SS_SLEEP_CBCR_CLKOFF_BMSK                                 0x80000000
#define HWIO_MSS_QDSP6SS_SLEEP_CBCR_CLKOFF_SHFT                                       0x1f
#define HWIO_MSS_QDSP6SS_SLEEP_CBCR_CLKEN_BMSK                                         0x1
#define HWIO_MSS_QDSP6SS_SLEEP_CBCR_CLKEN_SHFT                                         0x0

#define HWIO_MSS_QDSP6SS_NMI_ADDR                                               (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000040)
#define HWIO_MSS_QDSP6SS_NMI_RMSK                                                      0x3
#define HWIO_MSS_QDSP6SS_NMI_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_NMI_ADDR,v)
#define HWIO_MSS_QDSP6SS_NMI_CLEAR_STATUS_BMSK                                         0x2
#define HWIO_MSS_QDSP6SS_NMI_CLEAR_STATUS_SHFT                                         0x1
#define HWIO_MSS_QDSP6SS_NMI_SET_NMI_BMSK                                              0x1
#define HWIO_MSS_QDSP6SS_NMI_SET_NMI_SHFT                                              0x0

#define HWIO_MSS_QDSP6SS_NMI_STATUS_ADDR                                        (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000044)
#define HWIO_MSS_QDSP6SS_NMI_STATUS_RMSK                                               0x3
#define HWIO_MSS_QDSP6SS_NMI_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_NMI_STATUS_ADDR, HWIO_MSS_QDSP6SS_NMI_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_NMI_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_NMI_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_NMI_STATUS_WDOG_TRIG_BMSK                                     0x2
#define HWIO_MSS_QDSP6SS_NMI_STATUS_WDOG_TRIG_SHFT                                     0x1
#define HWIO_MSS_QDSP6SS_NMI_STATUS_PUBCSR_TRIG_BMSK                                   0x1
#define HWIO_MSS_QDSP6SS_NMI_STATUS_PUBCSR_TRIG_SHFT                                   0x0

#define HWIO_MSS_QDSP6SS_CLOCK_SPDM_MON_ADDR                                    (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000048)
#define HWIO_MSS_QDSP6SS_CLOCK_SPDM_MON_RMSK                                           0x1
#define HWIO_MSS_QDSP6SS_CLOCK_SPDM_MON_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_CLOCK_SPDM_MON_ADDR, HWIO_MSS_QDSP6SS_CLOCK_SPDM_MON_RMSK)
#define HWIO_MSS_QDSP6SS_CLOCK_SPDM_MON_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_CLOCK_SPDM_MON_ADDR, m)
#define HWIO_MSS_QDSP6SS_CLOCK_SPDM_MON_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_CLOCK_SPDM_MON_ADDR,v)
#define HWIO_MSS_QDSP6SS_CLOCK_SPDM_MON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_CLOCK_SPDM_MON_ADDR,m,v,HWIO_MSS_QDSP6SS_CLOCK_SPDM_MON_IN)
#define HWIO_MSS_QDSP6SS_CLOCK_SPDM_MON_Q6_MON_CLKEN_BMSK                              0x1
#define HWIO_MSS_QDSP6SS_CLOCK_SPDM_MON_Q6_MON_CLKEN_SHFT                              0x0

#define HWIO_MSS_QDSP6SS_STATERET_CTL_ADDR                                      (MSS_QDSP6SS_PUB_REG_BASE      + 0x0000004c)
#define HWIO_MSS_QDSP6SS_STATERET_CTL_RMSK                                             0xf
#define HWIO_MSS_QDSP6SS_STATERET_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_STATERET_CTL_ADDR, HWIO_MSS_QDSP6SS_STATERET_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_STATERET_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_STATERET_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_STATERET_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_STATERET_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_STATERET_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_STATERET_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_STATERET_CTL_IN)
#define HWIO_MSS_QDSP6SS_STATERET_CTL_WAKEUP_IN_BMSK                                   0x8
#define HWIO_MSS_QDSP6SS_STATERET_CTL_WAKEUP_IN_SHFT                                   0x3
#define HWIO_MSS_QDSP6SS_STATERET_CTL_WAKE_IRQ_BMSK                                    0x4
#define HWIO_MSS_QDSP6SS_STATERET_CTL_WAKE_IRQ_SHFT                                    0x2
#define HWIO_MSS_QDSP6SS_STATERET_CTL_RESTORE_BMSK                                     0x2
#define HWIO_MSS_QDSP6SS_STATERET_CTL_RESTORE_SHFT                                     0x1
#define HWIO_MSS_QDSP6SS_STATERET_CTL_SAVE_BMSK                                        0x1
#define HWIO_MSS_QDSP6SS_STATERET_CTL_SAVE_SHFT                                        0x0

#define HWIO_MSS_QDSP6SS_LDO_CFG0_ADDR                                          (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000050)
#define HWIO_MSS_QDSP6SS_LDO_CFG0_RMSK                                           0x500ff00
#define HWIO_MSS_QDSP6SS_LDO_CFG0_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_CFG0_ADDR, HWIO_MSS_QDSP6SS_LDO_CFG0_RMSK)
#define HWIO_MSS_QDSP6SS_LDO_CFG0_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_CFG0_ADDR, m)
#define HWIO_MSS_QDSP6SS_LDO_CFG0_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_LDO_CFG0_ADDR,v)
#define HWIO_MSS_QDSP6SS_LDO_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_LDO_CFG0_ADDR,m,v,HWIO_MSS_QDSP6SS_LDO_CFG0_IN)
#define HWIO_MSS_QDSP6SS_LDO_CFG0_LDO_TEST_BMSK                                  0x4000000
#define HWIO_MSS_QDSP6SS_LDO_CFG0_LDO_TEST_SHFT                                       0x1a
#define HWIO_MSS_QDSP6SS_LDO_CFG0_IDAC_EN_BMSK                                   0x1000000
#define HWIO_MSS_QDSP6SS_LDO_CFG0_IDAC_EN_SHFT                                        0x18
#define HWIO_MSS_QDSP6SS_LDO_CFG0_LDO_CTL1_BMSK                                     0xff00
#define HWIO_MSS_QDSP6SS_LDO_CFG0_LDO_CTL1_SHFT                                        0x8

#define HWIO_MSS_QDSP6SS_LDO_CFG1_ADDR                                          (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000054)
#define HWIO_MSS_QDSP6SS_LDO_CFG1_RMSK                                              0x3fff
#define HWIO_MSS_QDSP6SS_LDO_CFG1_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_CFG1_ADDR, HWIO_MSS_QDSP6SS_LDO_CFG1_RMSK)
#define HWIO_MSS_QDSP6SS_LDO_CFG1_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_CFG1_ADDR, m)
#define HWIO_MSS_QDSP6SS_LDO_CFG1_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_LDO_CFG1_ADDR,v)
#define HWIO_MSS_QDSP6SS_LDO_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_LDO_CFG1_ADDR,m,v,HWIO_MSS_QDSP6SS_LDO_CFG1_IN)
#define HWIO_MSS_QDSP6SS_LDO_CFG1_LDO_VREF_TRIM_SEL_BMSK                            0x2000
#define HWIO_MSS_QDSP6SS_LDO_CFG1_LDO_VREF_TRIM_SEL_SHFT                               0xd
#define HWIO_MSS_QDSP6SS_LDO_CFG1_LDO_VREF_TRIM_SW_BMSK                             0x1f00
#define HWIO_MSS_QDSP6SS_LDO_CFG1_LDO_VREF_TRIM_SW_SHFT                                0x8
#define HWIO_MSS_QDSP6SS_LDO_CFG1_DIG_CTL_BMSK                                        0xff
#define HWIO_MSS_QDSP6SS_LDO_CFG1_DIG_CTL_SHFT                                         0x0

#define HWIO_MSS_QDSP6SS_LDO_CFG2_ADDR                                          (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000058)
#define HWIO_MSS_QDSP6SS_LDO_CFG2_RMSK                                              0xffff
#define HWIO_MSS_QDSP6SS_LDO_CFG2_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_CFG2_ADDR, HWIO_MSS_QDSP6SS_LDO_CFG2_RMSK)
#define HWIO_MSS_QDSP6SS_LDO_CFG2_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_CFG2_ADDR, m)
#define HWIO_MSS_QDSP6SS_LDO_CFG2_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_LDO_CFG2_ADDR,v)
#define HWIO_MSS_QDSP6SS_LDO_CFG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_LDO_CFG2_ADDR,m,v,HWIO_MSS_QDSP6SS_LDO_CFG2_IN)
#define HWIO_MSS_QDSP6SS_LDO_CFG2_REF_CTL_BMSK                                      0xff00
#define HWIO_MSS_QDSP6SS_LDO_CFG2_REF_CTL_SHFT                                         0x8
#define HWIO_MSS_QDSP6SS_LDO_CFG2_IDAC_CTL1_BMSK                                      0xff
#define HWIO_MSS_QDSP6SS_LDO_CFG2_IDAC_CTL1_SHFT                                       0x0

#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_ADDR                                      (MSS_QDSP6SS_PUB_REG_BASE      + 0x0000005c)
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_RMSK                                         0x37f7f
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_VREF_SET_ADDR, HWIO_MSS_QDSP6SS_LDO_VREF_SET_RMSK)
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_VREF_SET_ADDR, m)
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_LDO_VREF_SET_ADDR,v)
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_LDO_VREF_SET_ADDR,m,v,HWIO_MSS_QDSP6SS_LDO_VREF_SET_IN)
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_LDO_VREF_SEL_BMSK                            0x20000
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_LDO_VREF_SEL_SHFT                               0x11
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_LDO_VREF_SEL_RST_BMSK                        0x10000
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_LDO_VREF_SEL_RST_SHFT                           0x10
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_VREF_RET_BMSK                                 0x7f00
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_VREF_RET_SHFT                                    0x8
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_VREF_LDO_BMSK                                   0x7f
#define HWIO_MSS_QDSP6SS_LDO_VREF_SET_VREF_LDO_SHFT                                    0x0

#define HWIO_MSS_QDSP6SS_LDO_VREF_TEST_CFG_ADDR                                 (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000060)
#define HWIO_MSS_QDSP6SS_LDO_VREF_TEST_CFG_RMSK                                   0xffffff
#define HWIO_MSS_QDSP6SS_LDO_VREF_TEST_CFG_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_VREF_TEST_CFG_ADDR, HWIO_MSS_QDSP6SS_LDO_VREF_TEST_CFG_RMSK)
#define HWIO_MSS_QDSP6SS_LDO_VREF_TEST_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_VREF_TEST_CFG_ADDR, m)
#define HWIO_MSS_QDSP6SS_LDO_VREF_TEST_CFG_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_LDO_VREF_TEST_CFG_ADDR,v)
#define HWIO_MSS_QDSP6SS_LDO_VREF_TEST_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_LDO_VREF_TEST_CFG_ADDR,m,v,HWIO_MSS_QDSP6SS_LDO_VREF_TEST_CFG_IN)
#define HWIO_MSS_QDSP6SS_LDO_VREF_TEST_CFG_LD_MSB_BMSK                            0xff0000
#define HWIO_MSS_QDSP6SS_LDO_VREF_TEST_CFG_LD_MSB_SHFT                                0x10
#define HWIO_MSS_QDSP6SS_LDO_VREF_TEST_CFG_LD_LSB_BMSK                              0xff00
#define HWIO_MSS_QDSP6SS_LDO_VREF_TEST_CFG_LD_LSB_SHFT                                 0x8
#define HWIO_MSS_QDSP6SS_LDO_VREF_TEST_CFG_OSC_CTL_BMSK                               0xff
#define HWIO_MSS_QDSP6SS_LDO_VREF_TEST_CFG_OSC_CTL_SHFT                                0x0

#define HWIO_MSS_QDSP6SS_LDO_VREF_CMD_ADDR                                      (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000064)
#define HWIO_MSS_QDSP6SS_LDO_VREF_CMD_RMSK                                             0x1
#define HWIO_MSS_QDSP6SS_LDO_VREF_CMD_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_LDO_VREF_CMD_ADDR,v)
#define HWIO_MSS_QDSP6SS_LDO_VREF_CMD_LDO_VREF_SEL_UPDATE_BMSK                         0x1
#define HWIO_MSS_QDSP6SS_LDO_VREF_CMD_LDO_VREF_SEL_UPDATE_SHFT                         0x0

#define HWIO_MSS_QDSP6SS_QMC_SVS_CTL_ADDR                                       (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000068)
#define HWIO_MSS_QDSP6SS_QMC_SVS_CTL_RMSK                                              0x3
#define HWIO_MSS_QDSP6SS_QMC_SVS_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_QMC_SVS_CTL_ADDR, HWIO_MSS_QDSP6SS_QMC_SVS_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_QMC_SVS_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_QMC_SVS_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_QMC_SVS_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_QMC_SVS_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_QMC_SVS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_QMC_SVS_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_QMC_SVS_CTL_IN)
#define HWIO_MSS_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_SEL_BMSK                              0x2
#define HWIO_MSS_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_SEL_SHFT                              0x1
#define HWIO_MSS_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_BMSK                                  0x1
#define HWIO_MSS_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_SHFT                                  0x0

#define HWIO_MSS_QDSP6SS_BHS_STATUS_ADDR                                        (MSS_QDSP6SS_PUB_REG_BASE      + 0x0000006c)
#define HWIO_MSS_QDSP6SS_BHS_STATUS_RMSK                                               0x1
#define HWIO_MSS_QDSP6SS_BHS_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_BHS_STATUS_ADDR, HWIO_MSS_QDSP6SS_BHS_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_BHS_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_BHS_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_BHS_STATUS_BHS_EN_REST_ACK_BMSK                               0x1
#define HWIO_MSS_QDSP6SS_BHS_STATUS_BHS_EN_REST_ACK_SHFT                               0x0

#define HWIO_MSS_QDSP6SS_LDO_EFUSE_STATUS_ADDR                                  (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000070)
#define HWIO_MSS_QDSP6SS_LDO_EFUSE_STATUS_RMSK                                        0x3f
#define HWIO_MSS_QDSP6SS_LDO_EFUSE_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_EFUSE_STATUS_ADDR, HWIO_MSS_QDSP6SS_LDO_EFUSE_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_LDO_EFUSE_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_EFUSE_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_LDO_EFUSE_STATUS_LDO_VREF_TRIM_BMSK                          0x3e
#define HWIO_MSS_QDSP6SS_LDO_EFUSE_STATUS_LDO_VREF_TRIM_SHFT                           0x1
#define HWIO_MSS_QDSP6SS_LDO_EFUSE_STATUS_LDO_EN_BMSK                                  0x1
#define HWIO_MSS_QDSP6SS_LDO_EFUSE_STATUS_LDO_EN_SHFT                                  0x0

#define HWIO_MSS_QDSP6SS_BHS_CTL_ADDR                                           (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000074)
#define HWIO_MSS_QDSP6SS_BHS_CTL_RMSK                                                  0xf
#define HWIO_MSS_QDSP6SS_BHS_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_BHS_CTL_ADDR, HWIO_MSS_QDSP6SS_BHS_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_BHS_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_BHS_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_BHS_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_BHS_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_BHS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_BHS_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_BHS_CTL_IN)
#define HWIO_MSS_QDSP6SS_BHS_CTL_DRIVE_SEL_BMSK                                        0xf
#define HWIO_MSS_QDSP6SS_BHS_CTL_DRIVE_SEL_SHFT                                        0x0

#define HWIO_MSS_QDSP6SS_SPM_SW_TRIG_ADDR                                       (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000080)
#define HWIO_MSS_QDSP6SS_SPM_SW_TRIG_RMSK                                              0x3
#define HWIO_MSS_QDSP6SS_SPM_SW_TRIG_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_SPM_SW_TRIG_ADDR,v)
#define HWIO_MSS_QDSP6SS_SPM_SW_TRIG_CMD_BMSK                                          0x3
#define HWIO_MSS_QDSP6SS_SPM_SW_TRIG_CMD_SHFT                                          0x0

#define HWIO_MSS_QDSP6SS_SPM_TRIG_STATUS_ADDR                                   (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000084)
#define HWIO_MSS_QDSP6SS_SPM_TRIG_STATUS_RMSK                                          0x3
#define HWIO_MSS_QDSP6SS_SPM_TRIG_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPM_TRIG_STATUS_ADDR, HWIO_MSS_QDSP6SS_SPM_TRIG_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_SPM_TRIG_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPM_TRIG_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_SPM_TRIG_STATUS_BUSY_BMSK                                     0x3
#define HWIO_MSS_QDSP6SS_SPM_TRIG_STATUS_BUSY_SHFT                                     0x0

#define HWIO_MSS_QDSP6SS_INTF_HALTREQ_ADDR                                      (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000088)
#define HWIO_MSS_QDSP6SS_INTF_HALTREQ_RMSK                                             0x7
#define HWIO_MSS_QDSP6SS_INTF_HALTREQ_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_INTF_HALTREQ_ADDR, HWIO_MSS_QDSP6SS_INTF_HALTREQ_RMSK)
#define HWIO_MSS_QDSP6SS_INTF_HALTREQ_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_INTF_HALTREQ_ADDR, m)
#define HWIO_MSS_QDSP6SS_INTF_HALTREQ_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_INTF_HALTREQ_ADDR,v)
#define HWIO_MSS_QDSP6SS_INTF_HALTREQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_INTF_HALTREQ_ADDR,m,v,HWIO_MSS_QDSP6SS_INTF_HALTREQ_IN)
#define HWIO_MSS_QDSP6SS_INTF_HALTREQ_AXIM2_BMSK                                       0x4
#define HWIO_MSS_QDSP6SS_INTF_HALTREQ_AXIM2_SHFT                                       0x2
#define HWIO_MSS_QDSP6SS_INTF_HALTREQ_AXIM_BMSK                                        0x2
#define HWIO_MSS_QDSP6SS_INTF_HALTREQ_AXIM_SHFT                                        0x1
#define HWIO_MSS_QDSP6SS_INTF_HALTREQ_ALL_BMSK                                         0x1
#define HWIO_MSS_QDSP6SS_INTF_HALTREQ_ALL_SHFT                                         0x0

#define HWIO_MSS_QDSP6SS_INTF_HALTACK_ADDR                                      (MSS_QDSP6SS_PUB_REG_BASE      + 0x0000008c)
#define HWIO_MSS_QDSP6SS_INTF_HALTACK_RMSK                                             0x7
#define HWIO_MSS_QDSP6SS_INTF_HALTACK_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_INTF_HALTACK_ADDR, HWIO_MSS_QDSP6SS_INTF_HALTACK_RMSK)
#define HWIO_MSS_QDSP6SS_INTF_HALTACK_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_INTF_HALTACK_ADDR, m)
#define HWIO_MSS_QDSP6SS_INTF_HALTACK_AXIM2_BMSK                                       0x4
#define HWIO_MSS_QDSP6SS_INTF_HALTACK_AXIM2_SHFT                                       0x2
#define HWIO_MSS_QDSP6SS_INTF_HALTACK_AXIM_BMSK                                        0x2
#define HWIO_MSS_QDSP6SS_INTF_HALTACK_AXIM_SHFT                                        0x1
#define HWIO_MSS_QDSP6SS_INTF_HALTACK_ALL_BMSK                                         0x1
#define HWIO_MSS_QDSP6SS_INTF_HALTACK_ALL_SHFT                                         0x0

#define HWIO_MSS_QDSP6SS_INTFCLAMP_SET_ADDR                                     (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000090)
#define HWIO_MSS_QDSP6SS_INTFCLAMP_SET_RMSK                                            0xf
#define HWIO_MSS_QDSP6SS_INTFCLAMP_SET_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_INTFCLAMP_SET_ADDR,v)
#define HWIO_MSS_QDSP6SS_INTFCLAMP_SET_AXIM2_CLAMP_E_BMSK                              0x8
#define HWIO_MSS_QDSP6SS_INTFCLAMP_SET_AXIM2_CLAMP_E_SHFT                              0x3
#define HWIO_MSS_QDSP6SS_INTFCLAMP_SET_AXIM2_CLAMP_L_BMSK                              0x4
#define HWIO_MSS_QDSP6SS_INTFCLAMP_SET_AXIM2_CLAMP_L_SHFT                              0x2
#define HWIO_MSS_QDSP6SS_INTFCLAMP_SET_AXIM_CLAMP_E_BMSK                               0x2
#define HWIO_MSS_QDSP6SS_INTFCLAMP_SET_AXIM_CLAMP_E_SHFT                               0x1
#define HWIO_MSS_QDSP6SS_INTFCLAMP_SET_AXIM_CLAMP_L_BMSK                               0x1
#define HWIO_MSS_QDSP6SS_INTFCLAMP_SET_AXIM_CLAMP_L_SHFT                               0x0

#define HWIO_MSS_QDSP6SS_INTFCLAMP_CLEAR_ADDR                                   (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000094)
#define HWIO_MSS_QDSP6SS_INTFCLAMP_CLEAR_RMSK                                          0xf
#define HWIO_MSS_QDSP6SS_INTFCLAMP_CLEAR_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_INTFCLAMP_CLEAR_ADDR,v)
#define HWIO_MSS_QDSP6SS_INTFCLAMP_CLEAR_AXIM2_CLAMP_E_BMSK                            0x8
#define HWIO_MSS_QDSP6SS_INTFCLAMP_CLEAR_AXIM2_CLAMP_E_SHFT                            0x3
#define HWIO_MSS_QDSP6SS_INTFCLAMP_CLEAR_AXIM2_CLAMP_L_BMSK                            0x4
#define HWIO_MSS_QDSP6SS_INTFCLAMP_CLEAR_AXIM2_CLAMP_L_SHFT                            0x2
#define HWIO_MSS_QDSP6SS_INTFCLAMP_CLEAR_AXIM_CLAMP_E_BMSK                             0x2
#define HWIO_MSS_QDSP6SS_INTFCLAMP_CLEAR_AXIM_CLAMP_E_SHFT                             0x1
#define HWIO_MSS_QDSP6SS_INTFCLAMP_CLEAR_AXIM_CLAMP_L_BMSK                             0x1
#define HWIO_MSS_QDSP6SS_INTFCLAMP_CLEAR_AXIM_CLAMP_L_SHFT                             0x0

#define HWIO_MSS_QDSP6SS_INTFCLAMP_STATUS_ADDR                                  (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000098)
#define HWIO_MSS_QDSP6SS_INTFCLAMP_STATUS_RMSK                                         0xf
#define HWIO_MSS_QDSP6SS_INTFCLAMP_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_INTFCLAMP_STATUS_ADDR, HWIO_MSS_QDSP6SS_INTFCLAMP_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_INTFCLAMP_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_INTFCLAMP_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_INTFCLAMP_STATUS_AXIM2_CLAMP_E_BMSK                           0x8
#define HWIO_MSS_QDSP6SS_INTFCLAMP_STATUS_AXIM2_CLAMP_E_SHFT                           0x3
#define HWIO_MSS_QDSP6SS_INTFCLAMP_STATUS_AXIM2_CLAMP_L_BMSK                           0x4
#define HWIO_MSS_QDSP6SS_INTFCLAMP_STATUS_AXIM2_CLAMP_L_SHFT                           0x2
#define HWIO_MSS_QDSP6SS_INTFCLAMP_STATUS_AXIM_CLAMP_E_BMSK                            0x2
#define HWIO_MSS_QDSP6SS_INTFCLAMP_STATUS_AXIM_CLAMP_E_SHFT                            0x1
#define HWIO_MSS_QDSP6SS_INTFCLAMP_STATUS_AXIM_CLAMP_L_BMSK                            0x1
#define HWIO_MSS_QDSP6SS_INTFCLAMP_STATUS_AXIM_CLAMP_L_SHFT                            0x0

#define HWIO_MSS_QDSP6SS_INTF_FIFO_RESET_ADDR                                   (MSS_QDSP6SS_PUB_REG_BASE      + 0x000000a0)
#define HWIO_MSS_QDSP6SS_INTF_FIFO_RESET_RMSK                                          0x3
#define HWIO_MSS_QDSP6SS_INTF_FIFO_RESET_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_INTF_FIFO_RESET_ADDR, HWIO_MSS_QDSP6SS_INTF_FIFO_RESET_RMSK)
#define HWIO_MSS_QDSP6SS_INTF_FIFO_RESET_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_INTF_FIFO_RESET_ADDR, m)
#define HWIO_MSS_QDSP6SS_INTF_FIFO_RESET_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_INTF_FIFO_RESET_ADDR,v)
#define HWIO_MSS_QDSP6SS_INTF_FIFO_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_INTF_FIFO_RESET_ADDR,m,v,HWIO_MSS_QDSP6SS_INTF_FIFO_RESET_IN)
#define HWIO_MSS_QDSP6SS_INTF_FIFO_RESET_AXIM2_BMSK                                    0x2
#define HWIO_MSS_QDSP6SS_INTF_FIFO_RESET_AXIM2_SHFT                                    0x1
#define HWIO_MSS_QDSP6SS_INTF_FIFO_RESET_AXIM_BMSK                                     0x1
#define HWIO_MSS_QDSP6SS_INTF_FIFO_RESET_AXIM_SHFT                                     0x0

#define HWIO_MSS_QDSP6SS_CP_PWR_CTL_ADDR                                        (MSS_QDSP6SS_PUB_REG_BASE      + 0x000000f0)
#define HWIO_MSS_QDSP6SS_CP_PWR_CTL_RMSK                                         0x17800ff
#define HWIO_MSS_QDSP6SS_CP_PWR_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_CP_PWR_CTL_ADDR, HWIO_MSS_QDSP6SS_CP_PWR_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_CP_PWR_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_CP_PWR_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_CP_PWR_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_CP_PWR_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_CP_PWR_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_CP_PWR_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_CP_PWR_CTL_IN)
#define HWIO_MSS_QDSP6SS_CP_PWR_CTL_BHS_ON_BMSK                                  0x1000000
#define HWIO_MSS_QDSP6SS_CP_PWR_CTL_BHS_ON_SHFT                                       0x18
#define HWIO_MSS_QDSP6SS_CP_PWR_CTL_CLAMP_QMC_MEM_BMSK                            0x400000
#define HWIO_MSS_QDSP6SS_CP_PWR_CTL_CLAMP_QMC_MEM_SHFT                                0x16
#define HWIO_MSS_QDSP6SS_CP_PWR_CTL_CLAMP_WL_BMSK                                 0x200000
#define HWIO_MSS_QDSP6SS_CP_PWR_CTL_CLAMP_WL_SHFT                                     0x15
#define HWIO_MSS_QDSP6SS_CP_PWR_CTL_CLAMP_IO_BMSK                                 0x100000
#define HWIO_MSS_QDSP6SS_CP_PWR_CTL_CLAMP_IO_SHFT                                     0x14
#define HWIO_MSS_QDSP6SS_CP_PWR_CTL_SLP_RET_N_BMSK                                 0x80000
#define HWIO_MSS_QDSP6SS_CP_PWR_CTL_SLP_RET_N_SHFT                                    0x13
#define HWIO_MSS_QDSP6SS_CP_PWR_CTL_L1S_SLP_NRET_N_BMSK                               0xff
#define HWIO_MSS_QDSP6SS_CP_PWR_CTL_L1S_SLP_NRET_N_SHFT                                0x0

#define HWIO_MSS_QDSP6SS_STRAP_TCM_ADDR                                         (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000100)
#define HWIO_MSS_QDSP6SS_STRAP_TCM_RMSK                                         0xfffc0000
#define HWIO_MSS_QDSP6SS_STRAP_TCM_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_TCM_ADDR, HWIO_MSS_QDSP6SS_STRAP_TCM_RMSK)
#define HWIO_MSS_QDSP6SS_STRAP_TCM_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_TCM_ADDR, m)
#define HWIO_MSS_QDSP6SS_STRAP_TCM_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_STRAP_TCM_ADDR,v)
#define HWIO_MSS_QDSP6SS_STRAP_TCM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_STRAP_TCM_ADDR,m,v,HWIO_MSS_QDSP6SS_STRAP_TCM_IN)
#define HWIO_MSS_QDSP6SS_STRAP_TCM_BASE_ADDR_BMSK                               0xfffc0000
#define HWIO_MSS_QDSP6SS_STRAP_TCM_BASE_ADDR_SHFT                                     0x12

#define HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_ADDR                                    (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000104)
#define HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_RMSK                                    0xffffc000
#define HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_ADDR, HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_RMSK)
#define HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_ADDR, m)
#define HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_ADDR,v)
#define HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_ADDR,m,v,HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_IN)
#define HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_UPPER_ADDR_BMSK                         0xffffc000
#define HWIO_MSS_QDSP6SS_STRAP_AHBUPPER_UPPER_ADDR_SHFT                                0xe

#define HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_ADDR                                    (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000108)
#define HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_RMSK                                    0xffffc000
#define HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_ADDR, HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_RMSK)
#define HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_ADDR, m)
#define HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_ADDR,v)
#define HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_ADDR,m,v,HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_IN)
#define HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_LOWER_ADDR_BMSK                         0xffffc000
#define HWIO_MSS_QDSP6SS_STRAP_AHBLOWER_LOWER_ADDR_SHFT                                0xe

#define HWIO_MSS_QDSP6SS_STRAP_ACC_ADDR                                         (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000110)
#define HWIO_MSS_QDSP6SS_STRAP_ACC_RMSK                                         0xffffffff
#define HWIO_MSS_QDSP6SS_STRAP_ACC_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_ACC_ADDR, HWIO_MSS_QDSP6SS_STRAP_ACC_RMSK)
#define HWIO_MSS_QDSP6SS_STRAP_ACC_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_ACC_ADDR, m)
#define HWIO_MSS_QDSP6SS_STRAP_ACC_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_STRAP_ACC_ADDR,v)
#define HWIO_MSS_QDSP6SS_STRAP_ACC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_STRAP_ACC_ADDR,m,v,HWIO_MSS_QDSP6SS_STRAP_ACC_IN)
#define HWIO_MSS_QDSP6SS_STRAP_ACC_DATA_BMSK                                    0xffffffff
#define HWIO_MSS_QDSP6SS_STRAP_ACC_DATA_SHFT                                           0x0

#define HWIO_MSS_QDSP6SS_STRAP_QMC_ACC_ADDR                                     (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000114)
#define HWIO_MSS_QDSP6SS_STRAP_QMC_ACC_RMSK                                     0x800000ff
#define HWIO_MSS_QDSP6SS_STRAP_QMC_ACC_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_QMC_ACC_ADDR, HWIO_MSS_QDSP6SS_STRAP_QMC_ACC_RMSK)
#define HWIO_MSS_QDSP6SS_STRAP_QMC_ACC_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_QMC_ACC_ADDR, m)
#define HWIO_MSS_QDSP6SS_STRAP_QMC_ACC_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_STRAP_QMC_ACC_ADDR,v)
#define HWIO_MSS_QDSP6SS_STRAP_QMC_ACC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_STRAP_QMC_ACC_ADDR,m,v,HWIO_MSS_QDSP6SS_STRAP_QMC_ACC_IN)
#define HWIO_MSS_QDSP6SS_STRAP_QMC_ACC_STRAP_QMC_ACC_SEL_BMSK                   0x80000000
#define HWIO_MSS_QDSP6SS_STRAP_QMC_ACC_STRAP_QMC_ACC_SEL_SHFT                         0x1f
#define HWIO_MSS_QDSP6SS_STRAP_QMC_ACC_STRAP_QMC_ACC_BMSK                             0xff
#define HWIO_MSS_QDSP6SS_STRAP_QMC_ACC_STRAP_QMC_ACC_SHFT                              0x0

#define HWIO_MSS_QDSP6SS_DCC_CTRL_ADDR                                          (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000118)
#define HWIO_MSS_QDSP6SS_DCC_CTRL_RMSK                                          0xffffffff
#define HWIO_MSS_QDSP6SS_DCC_CTRL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_DCC_CTRL_ADDR, HWIO_MSS_QDSP6SS_DCC_CTRL_RMSK)
#define HWIO_MSS_QDSP6SS_DCC_CTRL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_DCC_CTRL_ADDR, m)
#define HWIO_MSS_QDSP6SS_DCC_CTRL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_DCC_CTRL_ADDR,v)
#define HWIO_MSS_QDSP6SS_DCC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_DCC_CTRL_ADDR,m,v,HWIO_MSS_QDSP6SS_DCC_CTRL_IN)
#define HWIO_MSS_QDSP6SS_DCC_CTRL_CTRL_BMSK                                     0xffffffff
#define HWIO_MSS_QDSP6SS_DCC_CTRL_CTRL_SHFT                                            0x0

#define HWIO_MSS_QDSP6SS_STRAP_AXIM2UPPER_ADDR                                  (MSS_QDSP6SS_PUB_REG_BASE      + 0x0000011c)
#define HWIO_MSS_QDSP6SS_STRAP_AXIM2UPPER_RMSK                                  0xffffc000
#define HWIO_MSS_QDSP6SS_STRAP_AXIM2UPPER_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_AXIM2UPPER_ADDR, HWIO_MSS_QDSP6SS_STRAP_AXIM2UPPER_RMSK)
#define HWIO_MSS_QDSP6SS_STRAP_AXIM2UPPER_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_AXIM2UPPER_ADDR, m)
#define HWIO_MSS_QDSP6SS_STRAP_AXIM2UPPER_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_STRAP_AXIM2UPPER_ADDR,v)
#define HWIO_MSS_QDSP6SS_STRAP_AXIM2UPPER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_STRAP_AXIM2UPPER_ADDR,m,v,HWIO_MSS_QDSP6SS_STRAP_AXIM2UPPER_IN)
#define HWIO_MSS_QDSP6SS_STRAP_AXIM2UPPER_UPPER_ADDR_BMSK                       0xffffc000
#define HWIO_MSS_QDSP6SS_STRAP_AXIM2UPPER_UPPER_ADDR_SHFT                              0xe

#define HWIO_MSS_QDSP6SS_STRAP_AXIM2LOWER_ADDR                                  (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000120)
#define HWIO_MSS_QDSP6SS_STRAP_AXIM2LOWER_RMSK                                  0xffffc000
#define HWIO_MSS_QDSP6SS_STRAP_AXIM2LOWER_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_AXIM2LOWER_ADDR, HWIO_MSS_QDSP6SS_STRAP_AXIM2LOWER_RMSK)
#define HWIO_MSS_QDSP6SS_STRAP_AXIM2LOWER_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_STRAP_AXIM2LOWER_ADDR, m)
#define HWIO_MSS_QDSP6SS_STRAP_AXIM2LOWER_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_STRAP_AXIM2LOWER_ADDR,v)
#define HWIO_MSS_QDSP6SS_STRAP_AXIM2LOWER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_STRAP_AXIM2LOWER_ADDR,m,v,HWIO_MSS_QDSP6SS_STRAP_AXIM2LOWER_IN)
#define HWIO_MSS_QDSP6SS_STRAP_AXIM2LOWER_LOWER_ADDR_BMSK                       0xffffc000
#define HWIO_MSS_QDSP6SS_STRAP_AXIM2LOWER_LOWER_ADDR_SHFT                              0xe

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
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_RESERVE_BITS31_12_BMSK                  0xfffff000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_RESERVE_BITS31_12_SHFT                         0xc
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
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_RESERVE_BITS7_6_BMSK                          0xc0
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_RESERVE_BITS7_6_SHFT                           0x6
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_REF_CYCLE_BMSK                                0x30
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_REF_CYCLE_SHFT                                 0x4
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_KFN_BMSK                                       0xf
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_KFN_SHFT                                       0x0

#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ADDR                                      (MSS_QDSP6SS_PUB_REG_BASE      + 0x0000021c)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_RMSK                                      0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ADDR, HWIO_MSS_QDSP6SS_PLL_TEST_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_TEST_CTL_IN)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_BIAS_GEN_TRIM_BMSK                        0xe0000000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_BIAS_GEN_TRIM_SHFT                              0x1d
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_DCO_BMSK                                  0x10000000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_DCO_SHFT                                        0x1c
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_PROCESS_CALB_BMSK                          0xc000000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_PROCESS_CALB_SHFT                               0x1a
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_OVERRIDE_PROCESS_CALB_BMSK                 0x2000000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_OVERRIDE_PROCESS_CALB_SHFT                      0x19
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_FINE_FCW_BMSK                              0x1e00000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_FINE_FCW_SHFT                                   0x15
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_OVERRIDE_FINE_FCW_BMSK                      0x100000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_OVERRIDE_FINE_FCW_SHFT                          0x14
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_COARSE_FCW_BMSK                              0xfe000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_COARSE_FCW_SHFT                                  0xd
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_OVERRIDE_COARSE_BMSK                          0x1000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_OVERRIDE_COARSE_SHFT                             0xc
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_DISABLE_LFSR_BMSK                              0x800
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_DISABLE_LFSR_SHFT                                0xb
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_DTEST_SEL_BMSK                                 0x700
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_DTEST_SEL_SHFT                                   0x8
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_DTEST_EN_BMSK                                   0x80
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_DTEST_EN_SHFT                                    0x7
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_BYP_TESTAMP_BMSK                                0x40
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_BYP_TESTAMP_SHFT                                 0x6
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ATEST1_SEL_BMSK                                 0x30
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ATEST1_SEL_SHFT                                  0x4
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ATEST0_SEL_BMSK                                  0xc
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ATEST0_SEL_SHFT                                  0x2
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ATEST1_EN_BMSK                                   0x2
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ATEST1_EN_SHFT                                   0x1
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ATEST0_EN_BMSK                                   0x1
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ATEST0_EN_SHFT                                   0x0

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

#define HWIO_MSS_QDSP6SS_PLL_STATUS_ADDR                                        (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000224)
#define HWIO_MSS_QDSP6SS_PLL_STATUS_RMSK                                        0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_STATUS_ADDR, HWIO_MSS_QDSP6SS_PLL_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_STATUS_STATUS_31_0_BMSK                            0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_STATUS_STATUS_31_0_SHFT                                   0x0

#define HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_ADDR                                      (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000228)
#define HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_RMSK                                      0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_ADDR, HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_IN)
#define HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_FREQUENCY_CTL_WORD_BMSK                   0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_FREQUENCY_CTL_WORD_SHFT                          0x0

#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR                                (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000240)
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CFG_RMSK                                       0x3
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CFG_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR, HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CFG_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CFG_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CFG_IN)
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CFG_RCG2PLL_UPD_EN_BMSK                        0x2
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CFG_RCG2PLL_UPD_EN_SHFT                        0x1
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CFG_PLL2RCG_UPD_EN_BMSK                        0x1
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CFG_PLL2RCG_UPD_EN_SHFT                        0x0

#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR                             (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000244)
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_RMSK                                   0x1f
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR, HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_IN)
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_MATCH_VALUE_BMSK                       0x1f
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_MATCH_VALUE_SHFT                        0x0

#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CMD_ADDR                                (MSS_QDSP6SS_PUB_REG_BASE      + 0x00000248)
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CMD_RMSK                                       0x1
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CMD_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CMD_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CMD_CMD_BMSK                                   0x1
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_CMD_CMD_SHFT                                   0x0

#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_STATUS_ADDR                             (MSS_QDSP6SS_PUB_REG_BASE      + 0x0000023c)
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_STATUS_RMSK                                    0x3
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_STATUS_ADDR, HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_STATUS_RCG2PLL_UPD_STATUS_BMSK                 0x2
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_STATUS_RCG2PLL_UPD_STATUS_SHFT                 0x1
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_STATUS_PLL2RCG_UPD_STATUS_BMSK                 0x1
#define HWIO_MSS_QDSP6SS_PLL_RCG_UPDATE_STATUS_PLL2RCG_UPD_STATUS_SHFT                 0x0

#define HWIO_MSS_QDSP6SS_EXT_WAKEUP_ADDR                                        (MSS_QDSP6SS_PUB_REG_BASE      + 0x00001000)
#define HWIO_MSS_QDSP6SS_EXT_WAKEUP_RMSK                                               0x1
#define HWIO_MSS_QDSP6SS_EXT_WAKEUP_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_EXT_WAKEUP_ADDR,v)
#define HWIO_MSS_QDSP6SS_EXT_WAKEUP_CMD_BMSK                                           0x1
#define HWIO_MSS_QDSP6SS_EXT_WAKEUP_CMD_SHFT                                           0x0

#define HWIO_MSS_QDSP6SS_EXT_PD_ADDR                                            (MSS_QDSP6SS_PUB_REG_BASE      + 0x00001004)
#define HWIO_MSS_QDSP6SS_EXT_PD_RMSK                                                   0x1
#define HWIO_MSS_QDSP6SS_EXT_PD_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_EXT_PD_ADDR,v)
#define HWIO_MSS_QDSP6SS_EXT_PD_CMD_BMSK                                               0x1
#define HWIO_MSS_QDSP6SS_EXT_PD_CMD_SHFT                                               0x0

#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_ADDR                                      (MSS_QDSP6SS_PUB_REG_BASE      + 0x00002000)
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_RMSK                                      0xff03ffff
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_TEST_BUS_CTL_ADDR, HWIO_MSS_QDSP6SS_TEST_BUS_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_TEST_BUS_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_TEST_BUS_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_TEST_BUS_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_TEST_BUS_CTL_IN)
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_SPARE_CFG_BMSK                            0xff000000
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_SPARE_CFG_SHFT                                  0x18
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_TESTBUS_EN_BMSK                              0x20000
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_TESTBUS_EN_SHFT                                 0x11
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_Q6_SEL_BMSK                                  0x10000
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_Q6_SEL_SHFT                                     0x10
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_SEL_BMSK                                      0xffff
#define HWIO_MSS_QDSP6SS_TEST_BUS_CTL_SEL_SHFT                                         0x0

#define HWIO_MSS_QDSP6SS_TEST_BUS_VALUE_ADDR                                    (MSS_QDSP6SS_PUB_REG_BASE      + 0x00002004)
#define HWIO_MSS_QDSP6SS_TEST_BUS_VALUE_RMSK                                    0xffffffff
#define HWIO_MSS_QDSP6SS_TEST_BUS_VALUE_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_TEST_BUS_VALUE_ADDR, HWIO_MSS_QDSP6SS_TEST_BUS_VALUE_RMSK)
#define HWIO_MSS_QDSP6SS_TEST_BUS_VALUE_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_TEST_BUS_VALUE_ADDR, m)
#define HWIO_MSS_QDSP6SS_TEST_BUS_VALUE_VALUE_BMSK                              0xffffffff
#define HWIO_MSS_QDSP6SS_TEST_BUS_VALUE_VALUE_SHFT                                     0x0

#define HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_ADDR                                    (MSS_QDSP6SS_PUB_REG_BASE      + 0x00002008)
#define HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_RMSK                                    0xffffffff
#define HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_ADDR, HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_RMSK)
#define HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_ADDR, m)
#define HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_ADDR,v)
#define HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_ADDR,m,v,HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_IN)
#define HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_VALUE_BMSK                              0xffffffff
#define HWIO_MSS_QDSP6SS_TEST_BUS_WDATA_VALUE_SHFT                                     0x0

#define HWIO_MSS_QDSP6SS_PWRDBG_CTL_ADDR                                        (MSS_QDSP6SS_PUB_REG_BASE      + 0x0000200c)
#define HWIO_MSS_QDSP6SS_PWRDBG_CTL_RMSK                                               0x1
#define HWIO_MSS_QDSP6SS_PWRDBG_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PWRDBG_CTL_ADDR, HWIO_MSS_QDSP6SS_PWRDBG_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_PWRDBG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PWRDBG_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_PWRDBG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PWRDBG_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_PWRDBG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PWRDBG_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_PWRDBG_CTL_IN)
#define HWIO_MSS_QDSP6SS_PWRDBG_CTL_ENA_BMSK                                           0x1
#define HWIO_MSS_QDSP6SS_PWRDBG_CTL_ENA_SHFT                                           0x0

#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_ADDR                                      (MSS_QDSP6SS_PUB_REG_BASE      + 0x00002010)
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_RMSK                                            0x7f
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_TEST_CLK_CTL_ADDR, HWIO_MSS_QDSP6SS_TEST_CLK_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_TEST_CLK_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_TEST_CLK_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_TEST_CLK_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_TEST_CLK_CTL_IN)
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_EN_BMSK                              0x40
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_EN_SHFT                               0x6
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_INV_BMSK                             0x20
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_INV_SHFT                              0x5
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_DBG_CLK_SEL_BMSK                                0x1f
#define HWIO_MSS_QDSP6SS_TEST_CLK_CTL_DBG_CLK_SEL_SHFT                                 0x0

#define HWIO_MSS_QDSP6SS_PWRDBG_STATUS_ADDR                                     (MSS_QDSP6SS_PUB_REG_BASE      + 0x00002014)
#define HWIO_MSS_QDSP6SS_PWRDBG_STATUS_RMSK                                     0xffffffff
#define HWIO_MSS_QDSP6SS_PWRDBG_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PWRDBG_STATUS_ADDR, HWIO_MSS_QDSP6SS_PWRDBG_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_PWRDBG_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PWRDBG_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_PWRDBG_STATUS_STATUS_BMSK                              0xffffffff
#define HWIO_MSS_QDSP6SS_PWRDBG_STATUS_STATUS_SHFT                                     0x0

#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ADDR                                      (MSS_QDSP6SS_PUB_REG_BASE      + 0x00002018)
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_RMSK                                        0xffffff
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ADDR, HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_RMSK)
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ADDR, m)
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ADDR,v)
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ADDR,m,v,HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_IN)
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_COUNT_BMSK                            0xffffe0
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_COUNT_SHFT                                 0x5
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_MUX_SEL_BMSK                                    0x10
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_MUX_SEL_SHFT                                     0x4
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CLK_R_ENA_BMSK                             0x8
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CLK_R_ENA_SHFT                             0x3
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CGC_EN_BMSK                                0x4
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CGC_EN_SHFT                                0x2
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_BYPASS_BMSK                                0x2
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_BYPASS_SHFT                                0x1
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_SW_RESET_BMSK                              0x1
#define HWIO_MSS_QDSP6SS_ZMEAS_CONFIG_ZMEAS_SW_RESET_SHFT                              0x0

#define HWIO_MSS_QDSP6SS_LDO_ATEST_ADDR                                         (MSS_QDSP6SS_PUB_REG_BASE      + 0x0000201c)
#define HWIO_MSS_QDSP6SS_LDO_ATEST_RMSK                                            0x3ffff
#define HWIO_MSS_QDSP6SS_LDO_ATEST_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_ATEST_ADDR, HWIO_MSS_QDSP6SS_LDO_ATEST_RMSK)
#define HWIO_MSS_QDSP6SS_LDO_ATEST_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_ATEST_ADDR, m)
#define HWIO_MSS_QDSP6SS_LDO_ATEST_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_LDO_ATEST_ADDR,v)
#define HWIO_MSS_QDSP6SS_LDO_ATEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_LDO_ATEST_ADDR,m,v,HWIO_MSS_QDSP6SS_LDO_ATEST_IN)
#define HWIO_MSS_QDSP6SS_LDO_ATEST_ATEST1_EN_BMSK                                  0x20000
#define HWIO_MSS_QDSP6SS_LDO_ATEST_ATEST1_EN_SHFT                                     0x11
#define HWIO_MSS_QDSP6SS_LDO_ATEST_ATEST0_EN_BMSK                                  0x10000
#define HWIO_MSS_QDSP6SS_LDO_ATEST_ATEST0_EN_SHFT                                     0x10
#define HWIO_MSS_QDSP6SS_LDO_ATEST_ATEST1_SEL_BMSK                                  0xff00
#define HWIO_MSS_QDSP6SS_LDO_ATEST_ATEST1_SEL_SHFT                                     0x8
#define HWIO_MSS_QDSP6SS_LDO_ATEST_ATEST0_SEL_BMSK                                    0xff
#define HWIO_MSS_QDSP6SS_LDO_ATEST_ATEST0_SEL_SHFT                                     0x0

#define HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_LOW_ADDR                                (MSS_QDSP6SS_PUB_REG_BASE      + 0x00002020)
#define HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_LOW_RMSK                                0xffffffff
#define HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_LOW_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_LOW_ADDR, HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_LOW_RMSK)
#define HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_LOW_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_LOW_ADDR, m)
#define HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_LOW_VALUE_BMSK                          0xffffffff
#define HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_LOW_VALUE_SHFT                                 0x0

#define HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_HIGH_ADDR                               (MSS_QDSP6SS_PUB_REG_BASE      + 0x00002024)
#define HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_HIGH_RMSK                               0xffffffff
#define HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_HIGH_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_HIGH_ADDR, HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_HIGH_RMSK)
#define HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_HIGH_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_HIGH_ADDR, m)
#define HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_HIGH_VALUE_BMSK                         0xffffffff
#define HWIO_MSS_QDSP6SS_LDO_DAC_STATUS_HIGH_VALUE_SHFT                                0x0

#define HWIO_MSS_QDSP6SS_CORE_STATUS_ADDR                                       (MSS_QDSP6SS_PUB_REG_BASE      + 0x00002028)
#define HWIO_MSS_QDSP6SS_CORE_STATUS_RMSK                                              0x7
#define HWIO_MSS_QDSP6SS_CORE_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_CORE_STATUS_ADDR, HWIO_MSS_QDSP6SS_CORE_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_CORE_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_CORE_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_CORE_STATUS_CP_IDLE_BMSK                                      0x4
#define HWIO_MSS_QDSP6SS_CORE_STATUS_CP_IDLE_SHFT                                      0x2
#define HWIO_MSS_QDSP6SS_CORE_STATUS_CORE_IN_SLEEP_BMSK                                0x2
#define HWIO_MSS_QDSP6SS_CORE_STATUS_CORE_IN_SLEEP_SHFT                                0x1
#define HWIO_MSS_QDSP6SS_CORE_STATUS_CORE_STATE_BMSK                                   0x1
#define HWIO_MSS_QDSP6SS_CORE_STATUS_CORE_STATE_SHFT                                   0x0


#endif /* __CLM_HWIO9X45_H__ */
