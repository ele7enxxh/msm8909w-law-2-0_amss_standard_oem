#ifndef __TEMP_BIMC_M_HWIO_H__
#define __TEMP_BIMC_M_HWIO_H__
/*
===========================================================================
*/
/**
  @file temp_bimc_m_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    BIMC_M_APP_MPORT
    BIMC_M_SYS0_MPORT
    BIMC_M_SYS1_MPORT
    BIMC_M_DSP_MPORT

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

  $Header: //components/rel/rpm.bf/2.1.1/core/power/sleep/src/temp_bimc_m_hwio.h#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: BIMC_M_APP_MPORT
 *--------------------------------------------------------------------------*/

#define BIMC_M_APP_MPORT_REG_BASE                                                        (BIMC_BASE      + 0x00008000)

#define HWIO_BIMC_M_APP_MPORT_COMPONENT_INFO_ADDR                                        (BIMC_M_APP_MPORT_REG_BASE      + 0x00000000)
#define HWIO_BIMC_M_APP_MPORT_COMPONENT_INFO_RMSK                                          0xffffff
#define HWIO_BIMC_M_APP_MPORT_COMPONENT_INFO_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_COMPONENT_INFO_ADDR, HWIO_BIMC_M_APP_MPORT_COMPONENT_INFO_RMSK)
#define HWIO_BIMC_M_APP_MPORT_COMPONENT_INFO_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_COMPONENT_INFO_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_COMPONENT_INFO_INSTANCE_BMSK                                 0xff0000
#define HWIO_BIMC_M_APP_MPORT_COMPONENT_INFO_INSTANCE_SHFT                                     0x10
#define HWIO_BIMC_M_APP_MPORT_COMPONENT_INFO_SUB_TYPE_BMSK                                   0xff00
#define HWIO_BIMC_M_APP_MPORT_COMPONENT_INFO_SUB_TYPE_SHFT                                      0x8
#define HWIO_BIMC_M_APP_MPORT_COMPONENT_INFO_TYPE_BMSK                                         0xff
#define HWIO_BIMC_M_APP_MPORT_COMPONENT_INFO_TYPE_SHFT                                          0x0

#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_0_ADDR                                  (BIMC_M_APP_MPORT_REG_BASE      + 0x00000020)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_0_RMSK                                  0xff00ffff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_0_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_0_ADDR, HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_0_RMSK)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_0_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_0_SYNC_MODE_BMSK                        0xff000000
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_0_SYNC_MODE_SHFT                              0x18
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_0_CONNECTION_TYPE_BMSK                      0xff00
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_0_CONNECTION_TYPE_SHFT                         0x8
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_0_FUNCTIONALITY_BMSK                          0xff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_0_FUNCTIONALITY_SHFT                           0x0

#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_1_ADDR                                  (BIMC_M_APP_MPORT_REG_BASE      + 0x00000030)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_1_RMSK                                  0xffffffff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_1_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_1_ADDR, HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_1_RMSK)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_1_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_1_SWAY_CONNECTIVITY_BMSK                0xffffffff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_1_SWAY_CONNECTIVITY_SHFT                       0x0

#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2A_ADDR                                 (BIMC_M_APP_MPORT_REG_BASE      + 0x00000040)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2A_RMSK                                 0xffffffff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2A_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2A_ADDR, HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2A_RMSK)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2A_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2A_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2A_M_DATA_WIDTH_BMSK                    0xffff0000
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2A_M_DATA_WIDTH_SHFT                          0x10
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2A_M_TID_WIDTH_BMSK                         0xff00
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2A_M_TID_WIDTH_SHFT                            0x8
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2A_M_MID_WIDTH_BMSK                           0xff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2A_M_MID_WIDTH_SHFT                            0x0

#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2B_ADDR                                 (BIMC_M_APP_MPORT_REG_BASE      + 0x00000044)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2B_RMSK                                     0xffff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2B_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2B_ADDR, HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2B_RMSK)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2B_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2B_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2B_C_DATA_WIDTH_BMSK                        0xffff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_2B_C_DATA_WIDTH_SHFT                           0x0

#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_3_ADDR                                  (BIMC_M_APP_MPORT_REG_BASE      + 0x00000050)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_3_RMSK                                  0xffffffff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_3_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_3_ADDR, HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_3_RMSK)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_3_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_3_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_3_RCH_DEPTH_BMSK                        0xff000000
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_3_RCH_DEPTH_SHFT                              0x18
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_3_BCH_DEPTH_BMSK                          0xff0000
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_3_BCH_DEPTH_SHFT                              0x10
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_3_WCH_DEPTH_BMSK                            0xff00
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_3_WCH_DEPTH_SHFT                               0x8
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_3_ACH_DEPTH_BMSK                              0xff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_3_ACH_DEPTH_SHFT                               0x0

#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_4_ADDR                                  (BIMC_M_APP_MPORT_REG_BASE      + 0x00000060)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_4_RMSK                                      0xffff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_4_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_4_ADDR, HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_4_RMSK)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_4_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_4_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_4_REORDER_BUFFER_DEPTH_BMSK                 0xff00
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_4_REORDER_BUFFER_DEPTH_SHFT                    0x8
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_4_REORDER_TABLE_DEPTH_BMSK                    0xff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_4_REORDER_TABLE_DEPTH_SHFT                     0x0

#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5A_ADDR                                 (BIMC_M_APP_MPORT_REG_BASE      + 0x00000070)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5A_RMSK                                 0xffffffff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5A_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5A_ADDR, HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5A_RMSK)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5A_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5A_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5A_ACH_PIPELINE_STAGES_BMSK             0xffffffff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5A_ACH_PIPELINE_STAGES_SHFT                    0x0

#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5B_ADDR                                 (BIMC_M_APP_MPORT_REG_BASE      + 0x00000074)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5B_RMSK                                 0xffffffff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5B_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5B_ADDR, HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5B_RMSK)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5B_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5B_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5B_WCH_PIPELINE_STAGES_BMSK             0xffffffff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5B_WCH_PIPELINE_STAGES_SHFT                    0x0

#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5C_ADDR                                 (BIMC_M_APP_MPORT_REG_BASE      + 0x00000078)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5C_RMSK                                 0xffffffff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5C_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5C_ADDR, HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5C_RMSK)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5C_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5C_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5C_BCH_PIPELINE_STAGES_BMSK             0xffffffff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5C_BCH_PIPELINE_STAGES_SHFT                    0x0

#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5D_ADDR                                 (BIMC_M_APP_MPORT_REG_BASE      + 0x0000007c)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5D_RMSK                                 0xffffffff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5D_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5D_ADDR, HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5D_RMSK)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5D_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5D_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5D_RCH_PIPELINE_STAGES_BMSK             0xffffffff
#define HWIO_BIMC_M_APP_MPORT_CONFIGURATION_INFO_5D_RCH_PIPELINE_STAGES_SHFT                    0x0

#define HWIO_BIMC_M_APP_MPORT_INTERRUPT_STATUS_ADDR                                      (BIMC_M_APP_MPORT_REG_BASE      + 0x00000100)
#define HWIO_BIMC_M_APP_MPORT_INTERRUPT_STATUS_RMSK                                             0x3
#define HWIO_BIMC_M_APP_MPORT_INTERRUPT_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_INTERRUPT_STATUS_ADDR, HWIO_BIMC_M_APP_MPORT_INTERRUPT_STATUS_RMSK)
#define HWIO_BIMC_M_APP_MPORT_INTERRUPT_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_INTERRUPT_STATUS_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_INTERRUPT_STATUS_RFU_BMSK                                         0x3
#define HWIO_BIMC_M_APP_MPORT_INTERRUPT_STATUS_RFU_SHFT                                         0x0

#define HWIO_BIMC_M_APP_MPORT_INTERRUPT_CLEAR_ADDR                                       (BIMC_M_APP_MPORT_REG_BASE      + 0x00000108)
#define HWIO_BIMC_M_APP_MPORT_INTERRUPT_CLEAR_RMSK                                              0x3
#define HWIO_BIMC_M_APP_MPORT_INTERRUPT_CLEAR_OUT(v)      \
        out_dword(HWIO_BIMC_M_APP_MPORT_INTERRUPT_CLEAR_ADDR,v)
#define HWIO_BIMC_M_APP_MPORT_INTERRUPT_CLEAR_RFU_BMSK                                          0x3
#define HWIO_BIMC_M_APP_MPORT_INTERRUPT_CLEAR_RFU_SHFT                                          0x0

#define HWIO_BIMC_M_APP_MPORT_INTERRUPT_ENABLE_ADDR                                      (BIMC_M_APP_MPORT_REG_BASE      + 0x0000010c)
#define HWIO_BIMC_M_APP_MPORT_INTERRUPT_ENABLE_RMSK                                             0x3
#define HWIO_BIMC_M_APP_MPORT_INTERRUPT_ENABLE_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_INTERRUPT_ENABLE_ADDR, HWIO_BIMC_M_APP_MPORT_INTERRUPT_ENABLE_RMSK)
#define HWIO_BIMC_M_APP_MPORT_INTERRUPT_ENABLE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_INTERRUPT_ENABLE_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_INTERRUPT_ENABLE_OUT(v)      \
        out_dword(HWIO_BIMC_M_APP_MPORT_INTERRUPT_ENABLE_ADDR,v)
#define HWIO_BIMC_M_APP_MPORT_INTERRUPT_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_APP_MPORT_INTERRUPT_ENABLE_ADDR,m,v,HWIO_BIMC_M_APP_MPORT_INTERRUPT_ENABLE_IN)
#define HWIO_BIMC_M_APP_MPORT_INTERRUPT_ENABLE_RFU_BMSK                                         0x3
#define HWIO_BIMC_M_APP_MPORT_INTERRUPT_ENABLE_RFU_SHFT                                         0x0

#define HWIO_BIMC_M_APP_MPORT_CLOCK_CTRL_ADDR                                            (BIMC_M_APP_MPORT_REG_BASE      + 0x00000200)
#define HWIO_BIMC_M_APP_MPORT_CLOCK_CTRL_RMSK                                                   0x3
#define HWIO_BIMC_M_APP_MPORT_CLOCK_CTRL_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CLOCK_CTRL_ADDR, HWIO_BIMC_M_APP_MPORT_CLOCK_CTRL_RMSK)
#define HWIO_BIMC_M_APP_MPORT_CLOCK_CTRL_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CLOCK_CTRL_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_CLOCK_CTRL_OUT(v)      \
        out_dword(HWIO_BIMC_M_APP_MPORT_CLOCK_CTRL_ADDR,v)
#define HWIO_BIMC_M_APP_MPORT_CLOCK_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_APP_MPORT_CLOCK_CTRL_ADDR,m,v,HWIO_BIMC_M_APP_MPORT_CLOCK_CTRL_IN)
#define HWIO_BIMC_M_APP_MPORT_CLOCK_CTRL_MASTER_CLOCK_GATING_EN_BMSK                            0x2
#define HWIO_BIMC_M_APP_MPORT_CLOCK_CTRL_MASTER_CLOCK_GATING_EN_SHFT                            0x1
#define HWIO_BIMC_M_APP_MPORT_CLOCK_CTRL_CORE_CLOCK_GATING_EN_BMSK                              0x1
#define HWIO_BIMC_M_APP_MPORT_CLOCK_CTRL_CORE_CLOCK_GATING_EN_SHFT                              0x0

#define HWIO_BIMC_M_APP_MPORT_CDC_CTRL_ADDR                                              (BIMC_M_APP_MPORT_REG_BASE      + 0x00000208)
#define HWIO_BIMC_M_APP_MPORT_CDC_CTRL_RMSK                                                     0x3
#define HWIO_BIMC_M_APP_MPORT_CDC_CTRL_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CDC_CTRL_ADDR, HWIO_BIMC_M_APP_MPORT_CDC_CTRL_RMSK)
#define HWIO_BIMC_M_APP_MPORT_CDC_CTRL_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_CDC_CTRL_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_CDC_CTRL_OUT(v)      \
        out_dword(HWIO_BIMC_M_APP_MPORT_CDC_CTRL_ADDR,v)
#define HWIO_BIMC_M_APP_MPORT_CDC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_APP_MPORT_CDC_CTRL_ADDR,m,v,HWIO_BIMC_M_APP_MPORT_CDC_CTRL_IN)
#define HWIO_BIMC_M_APP_MPORT_CDC_CTRL_MASTER_CLOCK_DEMETA_SEL_BMSK                             0x2
#define HWIO_BIMC_M_APP_MPORT_CDC_CTRL_MASTER_CLOCK_DEMETA_SEL_SHFT                             0x1
#define HWIO_BIMC_M_APP_MPORT_CDC_CTRL_CORE_CLOCK_DEMETA_SEL_BMSK                               0x1
#define HWIO_BIMC_M_APP_MPORT_CDC_CTRL_CORE_CLOCK_DEMETA_SEL_SHFT                               0x0

#define HWIO_BIMC_M_APP_MPORT_MODE_ADDR                                                  (BIMC_M_APP_MPORT_REG_BASE      + 0x00000210)
#define HWIO_BIMC_M_APP_MPORT_MODE_RMSK                                                  0xf00fff17
#define HWIO_BIMC_M_APP_MPORT_MODE_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_MODE_ADDR, HWIO_BIMC_M_APP_MPORT_MODE_RMSK)
#define HWIO_BIMC_M_APP_MPORT_MODE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_MODE_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_MODE_OUT(v)      \
        out_dword(HWIO_BIMC_M_APP_MPORT_MODE_ADDR,v)
#define HWIO_BIMC_M_APP_MPORT_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_APP_MPORT_MODE_ADDR,m,v,HWIO_BIMC_M_APP_MPORT_MODE_IN)
#define HWIO_BIMC_M_APP_MPORT_MODE_WR_GATHER_BEATS_BMSK                                  0xf0000000
#define HWIO_BIMC_M_APP_MPORT_MODE_WR_GATHER_BEATS_SHFT                                        0x1c
#define HWIO_BIMC_M_APP_MPORT_MODE_RD_GATHER_BEATS_BMSK                                     0xf0000
#define HWIO_BIMC_M_APP_MPORT_MODE_RD_GATHER_BEATS_SHFT                                        0x10
#define HWIO_BIMC_M_APP_MPORT_MODE_RCSSH_BMSK                                                0x8000
#define HWIO_BIMC_M_APP_MPORT_MODE_RCSSH_SHFT                                                   0xf
#define HWIO_BIMC_M_APP_MPORT_MODE_RCOSH_BMSK                                                0x4000
#define HWIO_BIMC_M_APP_MPORT_MODE_RCOSH_SHFT                                                   0xe
#define HWIO_BIMC_M_APP_MPORT_MODE_RCISH_BMSK                                                0x2000
#define HWIO_BIMC_M_APP_MPORT_MODE_RCISH_SHFT                                                   0xd
#define HWIO_BIMC_M_APP_MPORT_MODE_RCNSH_BMSK                                                0x1000
#define HWIO_BIMC_M_APP_MPORT_MODE_RCNSH_SHFT                                                   0xc
#define HWIO_BIMC_M_APP_MPORT_MODE_RNCSSH_BMSK                                                0x800
#define HWIO_BIMC_M_APP_MPORT_MODE_RNCSSH_SHFT                                                  0xb
#define HWIO_BIMC_M_APP_MPORT_MODE_RNCOSH_BMSK                                                0x400
#define HWIO_BIMC_M_APP_MPORT_MODE_RNCOSH_SHFT                                                  0xa
#define HWIO_BIMC_M_APP_MPORT_MODE_RNCISH_BMSK                                                0x200
#define HWIO_BIMC_M_APP_MPORT_MODE_RNCISH_SHFT                                                  0x9
#define HWIO_BIMC_M_APP_MPORT_MODE_RNCNSH_BMSK                                                0x100
#define HWIO_BIMC_M_APP_MPORT_MODE_RNCNSH_SHFT                                                  0x8
#define HWIO_BIMC_M_APP_MPORT_MODE_NARROW_WRITES_BMSK                                          0x10
#define HWIO_BIMC_M_APP_MPORT_MODE_NARROW_WRITES_SHFT                                           0x4
#define HWIO_BIMC_M_APP_MPORT_MODE_FIODV_BMSK                                                   0x4
#define HWIO_BIMC_M_APP_MPORT_MODE_FIODV_SHFT                                                   0x2
#define HWIO_BIMC_M_APP_MPORT_MODE_FIOSO_BMSK                                                   0x2
#define HWIO_BIMC_M_APP_MPORT_MODE_FIOSO_SHFT                                                   0x1
#define HWIO_BIMC_M_APP_MPORT_MODE_ORDERING_MODEL_BMSK                                          0x1
#define HWIO_BIMC_M_APP_MPORT_MODE_ORDERING_MODEL_SHFT                                          0x0

#define HWIO_BIMC_M_APP_MPORT_WR_WAY_CROSSING_ADDR                                       (BIMC_M_APP_MPORT_REG_BASE      + 0x00000220)
#define HWIO_BIMC_M_APP_MPORT_WR_WAY_CROSSING_RMSK                                              0xf
#define HWIO_BIMC_M_APP_MPORT_WR_WAY_CROSSING_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_WR_WAY_CROSSING_ADDR, HWIO_BIMC_M_APP_MPORT_WR_WAY_CROSSING_RMSK)
#define HWIO_BIMC_M_APP_MPORT_WR_WAY_CROSSING_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_WR_WAY_CROSSING_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_WR_WAY_CROSSING_OUT(v)      \
        out_dword(HWIO_BIMC_M_APP_MPORT_WR_WAY_CROSSING_ADDR,v)
#define HWIO_BIMC_M_APP_MPORT_WR_WAY_CROSSING_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_APP_MPORT_WR_WAY_CROSSING_ADDR,m,v,HWIO_BIMC_M_APP_MPORT_WR_WAY_CROSSING_IN)
#define HWIO_BIMC_M_APP_MPORT_WR_WAY_CROSSING_REG_BMSK                                          0xf
#define HWIO_BIMC_M_APP_MPORT_WR_WAY_CROSSING_REG_SHFT                                          0x0

#define HWIO_BIMC_M_APP_MPORT_PRIORITYLVL_OVERRIDE_ADDR                                  (BIMC_M_APP_MPORT_REG_BASE      + 0x00000230)
#define HWIO_BIMC_M_APP_MPORT_PRIORITYLVL_OVERRIDE_RMSK                                       0x301
#define HWIO_BIMC_M_APP_MPORT_PRIORITYLVL_OVERRIDE_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_PRIORITYLVL_OVERRIDE_ADDR, HWIO_BIMC_M_APP_MPORT_PRIORITYLVL_OVERRIDE_RMSK)
#define HWIO_BIMC_M_APP_MPORT_PRIORITYLVL_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_PRIORITYLVL_OVERRIDE_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_PRIORITYLVL_OVERRIDE_OUT(v)      \
        out_dword(HWIO_BIMC_M_APP_MPORT_PRIORITYLVL_OVERRIDE_ADDR,v)
#define HWIO_BIMC_M_APP_MPORT_PRIORITYLVL_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_APP_MPORT_PRIORITYLVL_OVERRIDE_ADDR,m,v,HWIO_BIMC_M_APP_MPORT_PRIORITYLVL_OVERRIDE_IN)
#define HWIO_BIMC_M_APP_MPORT_PRIORITYLVL_OVERRIDE_PRIORITYLVL_BMSK                           0x300
#define HWIO_BIMC_M_APP_MPORT_PRIORITYLVL_OVERRIDE_PRIORITYLVL_SHFT                             0x8
#define HWIO_BIMC_M_APP_MPORT_PRIORITYLVL_OVERRIDE_OVERRIDE_PRIORITYLVL_BMSK                    0x1
#define HWIO_BIMC_M_APP_MPORT_PRIORITYLVL_OVERRIDE_OVERRIDE_PRIORITYLVL_SHFT                    0x0

#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_ADDR                                 (BIMC_M_APP_MPORT_REG_BASE      + 0x00000240)
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_RMSK                                  0x3071f7f
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_ADDR, HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_RMSK)
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_OUT(v)      \
        out_dword(HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_ADDR,v)
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_ADDR,m,v,HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_IN)
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_AREQPRIORITY_BMSK                     0x3000000
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_AREQPRIORITY_SHFT                          0x18
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_AMEMTYPE_BMSK                           0x70000
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_AMEMTYPE_SHFT                              0x10
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_ATRANSIENT_BMSK                          0x1000
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_ATRANSIENT_SHFT                             0xc
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_ASHARED_BMSK                              0x800
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_ASHARED_SHFT                                0xb
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_AREDIRECT_BMSK                            0x400
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_AREDIRECT_SHFT                              0xa
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_AOOO_BMSK                                 0x200
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_AOOO_SHFT                                   0x9
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_AINNERSHARED_BMSK                         0x100
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_AINNERSHARED_SHFT                           0x8
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_BMSK                 0x40
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_SHFT                  0x6
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_BMSK                     0x20
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_SHFT                      0x5
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_BMSK                   0x10
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_SHFT                    0x4
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_ASHARED_BMSK                       0x8
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_ASHARED_SHFT                       0x3
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_BMSK                     0x4
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_SHFT                     0x2
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AOOO_BMSK                          0x2
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AOOO_SHFT                          0x1
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_BMSK                  0x1
#define HWIO_BIMC_M_APP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_SHFT                  0x0

#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_ADDR                                (BIMC_M_APP_MPORT_REG_BASE      + 0x00000250)
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_RMSK                                 0x3071f7f
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_ADDR, HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_RMSK)
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_OUT(v)      \
        out_dword(HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_ADDR,v)
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_ADDR,m,v,HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_IN)
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_AREQPRIORITY_BMSK                    0x3000000
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_AREQPRIORITY_SHFT                         0x18
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_AMEMTYPE_BMSK                          0x70000
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_AMEMTYPE_SHFT                             0x10
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_ATRANSIENT_BMSK                         0x1000
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_ATRANSIENT_SHFT                            0xc
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_ASHARED_BMSK                             0x800
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_ASHARED_SHFT                               0xb
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_AREDIRECT_BMSK                           0x400
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_AREDIRECT_SHFT                             0xa
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_AOOO_BMSK                                0x200
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_AOOO_SHFT                                  0x9
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_AINNERSHARED_BMSK                        0x100
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_AINNERSHARED_SHFT                          0x8
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_BMSK                0x40
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_SHFT                 0x6
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_BMSK                    0x20
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_SHFT                     0x5
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_BMSK                  0x10
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_SHFT                   0x4
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_ASHARED_BMSK                      0x8
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_ASHARED_SHFT                      0x3
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_BMSK                    0x4
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_SHFT                    0x2
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AOOO_BMSK                         0x2
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AOOO_SHFT                         0x1
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_BMSK                 0x1
#define HWIO_BIMC_M_APP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_SHFT                 0x0

#define HWIO_BIMC_M_APP_MPORT_BKE_ENABLE_ADDR                                            (BIMC_M_APP_MPORT_REG_BASE      + 0x00000300)
#define HWIO_BIMC_M_APP_MPORT_BKE_ENABLE_RMSK                                                   0x1
#define HWIO_BIMC_M_APP_MPORT_BKE_ENABLE_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_ENABLE_ADDR, HWIO_BIMC_M_APP_MPORT_BKE_ENABLE_RMSK)
#define HWIO_BIMC_M_APP_MPORT_BKE_ENABLE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_ENABLE_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_BKE_ENABLE_OUT(v)      \
        out_dword(HWIO_BIMC_M_APP_MPORT_BKE_ENABLE_ADDR,v)
#define HWIO_BIMC_M_APP_MPORT_BKE_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_APP_MPORT_BKE_ENABLE_ADDR,m,v,HWIO_BIMC_M_APP_MPORT_BKE_ENABLE_IN)
#define HWIO_BIMC_M_APP_MPORT_BKE_ENABLE_ENABLE_BMSK                                            0x1
#define HWIO_BIMC_M_APP_MPORT_BKE_ENABLE_ENABLE_SHFT                                            0x0

#define HWIO_BIMC_M_APP_MPORT_BKE_GRANT_PERIOD_ADDR                                      (BIMC_M_APP_MPORT_REG_BASE      + 0x00000304)
#define HWIO_BIMC_M_APP_MPORT_BKE_GRANT_PERIOD_RMSK                                           0x3ff
#define HWIO_BIMC_M_APP_MPORT_BKE_GRANT_PERIOD_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_GRANT_PERIOD_ADDR, HWIO_BIMC_M_APP_MPORT_BKE_GRANT_PERIOD_RMSK)
#define HWIO_BIMC_M_APP_MPORT_BKE_GRANT_PERIOD_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_GRANT_PERIOD_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_BKE_GRANT_PERIOD_OUT(v)      \
        out_dword(HWIO_BIMC_M_APP_MPORT_BKE_GRANT_PERIOD_ADDR,v)
#define HWIO_BIMC_M_APP_MPORT_BKE_GRANT_PERIOD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_APP_MPORT_BKE_GRANT_PERIOD_ADDR,m,v,HWIO_BIMC_M_APP_MPORT_BKE_GRANT_PERIOD_IN)
#define HWIO_BIMC_M_APP_MPORT_BKE_GRANT_PERIOD_GRANT_PERIOD_BMSK                              0x3ff
#define HWIO_BIMC_M_APP_MPORT_BKE_GRANT_PERIOD_GRANT_PERIOD_SHFT                                0x0

#define HWIO_BIMC_M_APP_MPORT_BKE_GRANT_COUNT_ADDR                                       (BIMC_M_APP_MPORT_REG_BASE      + 0x00000308)
#define HWIO_BIMC_M_APP_MPORT_BKE_GRANT_COUNT_RMSK                                           0xffff
#define HWIO_BIMC_M_APP_MPORT_BKE_GRANT_COUNT_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_GRANT_COUNT_ADDR, HWIO_BIMC_M_APP_MPORT_BKE_GRANT_COUNT_RMSK)
#define HWIO_BIMC_M_APP_MPORT_BKE_GRANT_COUNT_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_GRANT_COUNT_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_BKE_GRANT_COUNT_OUT(v)      \
        out_dword(HWIO_BIMC_M_APP_MPORT_BKE_GRANT_COUNT_ADDR,v)
#define HWIO_BIMC_M_APP_MPORT_BKE_GRANT_COUNT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_APP_MPORT_BKE_GRANT_COUNT_ADDR,m,v,HWIO_BIMC_M_APP_MPORT_BKE_GRANT_COUNT_IN)
#define HWIO_BIMC_M_APP_MPORT_BKE_GRANT_COUNT_GRANT_COUNT_BMSK                               0xffff
#define HWIO_BIMC_M_APP_MPORT_BKE_GRANT_COUNT_GRANT_COUNT_SHFT                                  0x0

#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_HIGH_ADDR                                    (BIMC_M_APP_MPORT_REG_BASE      + 0x00000320)
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_HIGH_RMSK                                        0xffff
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_HIGH_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_HIGH_ADDR, HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_HIGH_RMSK)
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_HIGH_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_HIGH_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_HIGH_OUT(v)      \
        out_dword(HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_HIGH_ADDR,v)
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_HIGH_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_HIGH_ADDR,m,v,HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_HIGH_IN)
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_HIGH_THRESHOLD_BMSK                              0xffff
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_HIGH_THRESHOLD_SHFT                                 0x0

#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_MEDIUM_ADDR                                  (BIMC_M_APP_MPORT_REG_BASE      + 0x00000324)
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_MEDIUM_RMSK                                      0xffff
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_MEDIUM_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_MEDIUM_ADDR, HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_MEDIUM_RMSK)
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_MEDIUM_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_MEDIUM_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_MEDIUM_OUT(v)      \
        out_dword(HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_MEDIUM_ADDR,v)
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_MEDIUM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_MEDIUM_ADDR,m,v,HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_MEDIUM_IN)
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_MEDIUM_THRESHOLD_BMSK                            0xffff
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_MEDIUM_THRESHOLD_SHFT                               0x0

#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_LOW_ADDR                                     (BIMC_M_APP_MPORT_REG_BASE      + 0x00000328)
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_LOW_RMSK                                         0xffff
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_LOW_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_LOW_ADDR, HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_LOW_RMSK)
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_LOW_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_LOW_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_LOW_OUT(v)      \
        out_dword(HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_LOW_ADDR,v)
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_LOW_ADDR,m,v,HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_LOW_IN)
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_LOW_THRESHOLD_BMSK                               0xffff
#define HWIO_BIMC_M_APP_MPORT_BKE_THRESHOLD_LOW_THRESHOLD_SHFT                                  0x0

#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_0_ADDR                                          (BIMC_M_APP_MPORT_REG_BASE      + 0x00000340)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_0_RMSK                                          0x80000303
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_0_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_0_ADDR, HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_0_RMSK)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_0_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_0_OUT(v)      \
        out_dword(HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_0_ADDR,v)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_0_ADDR,m,v,HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_0_IN)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_0_LIMIT_COMMANDS_BMSK                           0x80000000
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_0_LIMIT_COMMANDS_SHFT                                 0x1f
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_0_AREQPRIORITY_BMSK                                  0x300
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_0_AREQPRIORITY_SHFT                                    0x8
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_0_PRIORITYLVL_BMSK                                     0x3
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_0_PRIORITYLVL_SHFT                                     0x0

#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_1_ADDR                                          (BIMC_M_APP_MPORT_REG_BASE      + 0x00000344)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_1_RMSK                                          0x80000303
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_1_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_1_ADDR, HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_1_RMSK)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_1_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_1_OUT(v)      \
        out_dword(HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_1_ADDR,v)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_1_ADDR,m,v,HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_1_IN)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_1_LIMIT_COMMANDS_BMSK                           0x80000000
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_1_LIMIT_COMMANDS_SHFT                                 0x1f
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_1_AREQPRIORITY_BMSK                                  0x300
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_1_AREQPRIORITY_SHFT                                    0x8
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_1_PRIORITYLVL_BMSK                                     0x3
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_1_PRIORITYLVL_SHFT                                     0x0

#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_2_ADDR                                          (BIMC_M_APP_MPORT_REG_BASE      + 0x00000348)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_2_RMSK                                          0x80000303
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_2_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_2_ADDR, HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_2_RMSK)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_2_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_2_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_2_OUT(v)      \
        out_dword(HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_2_ADDR,v)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_2_ADDR,m,v,HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_2_IN)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_2_LIMIT_COMMANDS_BMSK                           0x80000000
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_2_LIMIT_COMMANDS_SHFT                                 0x1f
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_2_AREQPRIORITY_BMSK                                  0x300
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_2_AREQPRIORITY_SHFT                                    0x8
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_2_PRIORITYLVL_BMSK                                     0x3
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_2_PRIORITYLVL_SHFT                                     0x0

#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_3_ADDR                                          (BIMC_M_APP_MPORT_REG_BASE      + 0x0000034c)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_3_RMSK                                               0x303
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_3_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_3_ADDR, HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_3_RMSK)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_3_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_3_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_3_OUT(v)      \
        out_dword(HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_3_ADDR,v)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_3_ADDR,m,v,HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_3_IN)
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_3_AREQPRIORITY_BMSK                                  0x300
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_3_AREQPRIORITY_SHFT                                    0x8
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_3_PRIORITYLVL_BMSK                                     0x3
#define HWIO_BIMC_M_APP_MPORT_BKE_HEALTH_3_PRIORITYLVL_SHFT                                     0x0

#define HWIO_BIMC_M_APP_MPORT_STATUS_2A_ADDR                                             (BIMC_M_APP_MPORT_REG_BASE      + 0x00000420)
#define HWIO_BIMC_M_APP_MPORT_STATUS_2A_RMSK                                               0xff00ff
#define HWIO_BIMC_M_APP_MPORT_STATUS_2A_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_STATUS_2A_ADDR, HWIO_BIMC_M_APP_MPORT_STATUS_2A_RMSK)
#define HWIO_BIMC_M_APP_MPORT_STATUS_2A_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_STATUS_2A_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_STATUS_2A_WR_PENDING_PRE_BUF_BMSK                            0xff0000
#define HWIO_BIMC_M_APP_MPORT_STATUS_2A_WR_PENDING_PRE_BUF_SHFT                                0x10
#define HWIO_BIMC_M_APP_MPORT_STATUS_2A_RD_PENDING_PRE_BUF_BMSK                                0xff
#define HWIO_BIMC_M_APP_MPORT_STATUS_2A_RD_PENDING_PRE_BUF_SHFT                                 0x0

#define HWIO_BIMC_M_APP_MPORT_STATUS_2B_ADDR                                             (BIMC_M_APP_MPORT_REG_BASE      + 0x00000424)
#define HWIO_BIMC_M_APP_MPORT_STATUS_2B_RMSK                                               0xff00ff
#define HWIO_BIMC_M_APP_MPORT_STATUS_2B_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_STATUS_2B_ADDR, HWIO_BIMC_M_APP_MPORT_STATUS_2B_RMSK)
#define HWIO_BIMC_M_APP_MPORT_STATUS_2B_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_STATUS_2B_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_STATUS_2B_WR_PENDING_POST_BUF_BMSK                           0xff0000
#define HWIO_BIMC_M_APP_MPORT_STATUS_2B_WR_PENDING_POST_BUF_SHFT                               0x10
#define HWIO_BIMC_M_APP_MPORT_STATUS_2B_RD_PENDING_POST_BUF_BMSK                               0xff
#define HWIO_BIMC_M_APP_MPORT_STATUS_2B_RD_PENDING_POST_BUF_SHFT                                0x0

#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_ADDR                                             (BIMC_M_APP_MPORT_REG_BASE      + 0x00000430)
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_RMSK                                             0xff333333
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_STATUS_3A_ADDR, HWIO_BIMC_M_APP_MPORT_STATUS_3A_RMSK)
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_STATUS_3A_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_RCB_DATA_RD_FULL_BMSK                            0x80000000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_RCB_DATA_RD_FULL_SHFT                                  0x1f
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_RCB_DATA_RD_EMPTY_BMSK                           0x40000000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_RCB_DATA_RD_EMPTY_SHFT                                 0x1e
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_RCB_CTRL_RD_FULL_BMSK                            0x20000000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_RCB_CTRL_RD_FULL_SHFT                                  0x1d
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_RCB_CTRL_RD_EMPTY_BMSK                           0x10000000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_RCB_CTRL_RD_EMPTY_SHFT                                 0x1c
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_RCB_DATA_WR_FULL_BMSK                             0x8000000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_RCB_DATA_WR_FULL_SHFT                                  0x1b
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_RCB_DATA_WR_EMPTY_BMSK                            0x4000000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_RCB_DATA_WR_EMPTY_SHFT                                 0x1a
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_RCB_CTRL_WR_FULL_BMSK                             0x2000000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_RCB_CTRL_WR_FULL_SHFT                                  0x19
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_RCB_CTRL_WR_EMPTY_BMSK                            0x1000000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_RCB_CTRL_WR_EMPTY_SHFT                                 0x18
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_BCB_RD_FULL_BMSK                                   0x200000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_BCB_RD_FULL_SHFT                                       0x15
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_BCB_RD_EMPTY_BMSK                                  0x100000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_BCB_RD_EMPTY_SHFT                                      0x14
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_BCB_WR_FULL_BMSK                                    0x20000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_BCB_WR_FULL_SHFT                                       0x11
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_BCB_WR_EMPTY_BMSK                                   0x10000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_BCB_WR_EMPTY_SHFT                                      0x10
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_WCB_RD_FULL_BMSK                                     0x2000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_WCB_RD_FULL_SHFT                                        0xd
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_WCB_RD_EMPTY_BMSK                                    0x1000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_WCB_RD_EMPTY_SHFT                                       0xc
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_WCB_WR_FULL_BMSK                                      0x200
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_WCB_WR_FULL_SHFT                                        0x9
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_WCB_WR_EMPTY_BMSK                                     0x100
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_WCB_WR_EMPTY_SHFT                                       0x8
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_ACB_RD_FULL_BMSK                                       0x20
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_ACB_RD_FULL_SHFT                                        0x5
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_ACB_RD_EMPTY_BMSK                                      0x10
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_ACB_RD_EMPTY_SHFT                                       0x4
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_ACB_WR_FULL_BMSK                                        0x2
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_ACB_WR_FULL_SHFT                                        0x1
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_ACB_WR_EMPTY_BMSK                                       0x1
#define HWIO_BIMC_M_APP_MPORT_STATUS_3A_ACB_WR_EMPTY_SHFT                                       0x0

#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_ADDR                                             (BIMC_M_APP_MPORT_REG_BASE      + 0x00000434)
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_RMSK                                               0x333333
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_IN          \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_STATUS_3B_ADDR, HWIO_BIMC_M_APP_MPORT_STATUS_3B_RMSK)
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_APP_MPORT_STATUS_3B_ADDR, m)
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_RGB_RD_FULL_BMSK                                   0x200000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_RGB_RD_FULL_SHFT                                       0x15
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_RGB_RD_EMPTY_BMSK                                  0x100000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_RGB_RD_EMPTY_SHFT                                      0x14
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_RGB_WR_FULL_BMSK                                    0x20000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_RGB_WR_FULL_SHFT                                       0x11
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_RGB_WR_EMPTY_BMSK                                   0x10000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_RGB_WR_EMPTY_SHFT                                      0x10
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_W2AB_RD_FULL_BMSK                                    0x2000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_W2AB_RD_FULL_SHFT                                       0xd
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_W2AB_RD_EMPTY_BMSK                                   0x1000
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_W2AB_RD_EMPTY_SHFT                                      0xc
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_W2AB_WR_FULL_BMSK                                     0x200
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_W2AB_WR_FULL_SHFT                                       0x9
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_W2AB_WR_EMPTY_BMSK                                    0x100
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_W2AB_WR_EMPTY_SHFT                                      0x8
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_A2WB_RD_FULL_BMSK                                      0x20
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_A2WB_RD_FULL_SHFT                                       0x5
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_A2WB_RD_EMPTY_BMSK                                     0x10
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_A2WB_RD_EMPTY_SHFT                                      0x4
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_A2WB_WR_FULL_BMSK                                       0x2
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_A2WB_WR_FULL_SHFT                                       0x1
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_A2WB_WR_EMPTY_BMSK                                      0x1
#define HWIO_BIMC_M_APP_MPORT_STATUS_3B_A2WB_WR_EMPTY_SHFT                                      0x0

/*----------------------------------------------------------------------------
 * MODULE: BIMC_M_SYS0_MPORT
 *--------------------------------------------------------------------------*/

#define BIMC_M_SYS0_MPORT_REG_BASE                                                        (BIMC_BASE      + 0x00014000)

#define HWIO_BIMC_M_SYS0_MPORT_COMPONENT_INFO_ADDR                                        (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000000)
#define HWIO_BIMC_M_SYS0_MPORT_COMPONENT_INFO_RMSK                                          0xffffff
#define HWIO_BIMC_M_SYS0_MPORT_COMPONENT_INFO_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_COMPONENT_INFO_ADDR, HWIO_BIMC_M_SYS0_MPORT_COMPONENT_INFO_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_COMPONENT_INFO_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_COMPONENT_INFO_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_COMPONENT_INFO_INSTANCE_BMSK                                 0xff0000
#define HWIO_BIMC_M_SYS0_MPORT_COMPONENT_INFO_INSTANCE_SHFT                                     0x10
#define HWIO_BIMC_M_SYS0_MPORT_COMPONENT_INFO_SUB_TYPE_BMSK                                   0xff00
#define HWIO_BIMC_M_SYS0_MPORT_COMPONENT_INFO_SUB_TYPE_SHFT                                      0x8
#define HWIO_BIMC_M_SYS0_MPORT_COMPONENT_INFO_TYPE_BMSK                                         0xff
#define HWIO_BIMC_M_SYS0_MPORT_COMPONENT_INFO_TYPE_SHFT                                          0x0

#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_0_ADDR                                  (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000020)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_0_RMSK                                  0xff00ffff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_0_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_0_ADDR, HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_0_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_0_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_0_SYNC_MODE_BMSK                        0xff000000
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_0_SYNC_MODE_SHFT                              0x18
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_0_CONNECTION_TYPE_BMSK                      0xff00
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_0_CONNECTION_TYPE_SHFT                         0x8
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_0_FUNCTIONALITY_BMSK                          0xff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_0_FUNCTIONALITY_SHFT                           0x0

#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_1_ADDR                                  (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000030)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_1_RMSK                                  0xffffffff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_1_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_1_ADDR, HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_1_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_1_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_1_SWAY_CONNECTIVITY_BMSK                0xffffffff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_1_SWAY_CONNECTIVITY_SHFT                       0x0

#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2A_ADDR                                 (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000040)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2A_RMSK                                 0xffffffff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2A_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2A_ADDR, HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2A_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2A_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2A_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2A_M_DATA_WIDTH_BMSK                    0xffff0000
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2A_M_DATA_WIDTH_SHFT                          0x10
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2A_M_TID_WIDTH_BMSK                         0xff00
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2A_M_TID_WIDTH_SHFT                            0x8
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2A_M_MID_WIDTH_BMSK                           0xff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2A_M_MID_WIDTH_SHFT                            0x0

#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2B_ADDR                                 (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000044)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2B_RMSK                                     0xffff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2B_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2B_ADDR, HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2B_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2B_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2B_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2B_C_DATA_WIDTH_BMSK                        0xffff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_2B_C_DATA_WIDTH_SHFT                           0x0

#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_3_ADDR                                  (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000050)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_3_RMSK                                  0xffffffff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_3_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_3_ADDR, HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_3_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_3_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_3_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_3_RCH_DEPTH_BMSK                        0xff000000
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_3_RCH_DEPTH_SHFT                              0x18
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_3_BCH_DEPTH_BMSK                          0xff0000
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_3_BCH_DEPTH_SHFT                              0x10
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_3_WCH_DEPTH_BMSK                            0xff00
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_3_WCH_DEPTH_SHFT                               0x8
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_3_ACH_DEPTH_BMSK                              0xff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_3_ACH_DEPTH_SHFT                               0x0

#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_4_ADDR                                  (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000060)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_4_RMSK                                      0xffff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_4_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_4_ADDR, HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_4_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_4_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_4_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_4_REORDER_BUFFER_DEPTH_BMSK                 0xff00
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_4_REORDER_BUFFER_DEPTH_SHFT                    0x8
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_4_REORDER_TABLE_DEPTH_BMSK                    0xff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_4_REORDER_TABLE_DEPTH_SHFT                     0x0

#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5A_ADDR                                 (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000070)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5A_RMSK                                 0xffffffff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5A_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5A_ADDR, HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5A_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5A_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5A_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5A_ACH_PIPELINE_STAGES_BMSK             0xffffffff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5A_ACH_PIPELINE_STAGES_SHFT                    0x0

#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5B_ADDR                                 (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000074)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5B_RMSK                                 0xffffffff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5B_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5B_ADDR, HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5B_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5B_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5B_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5B_WCH_PIPELINE_STAGES_BMSK             0xffffffff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5B_WCH_PIPELINE_STAGES_SHFT                    0x0

#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5C_ADDR                                 (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000078)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5C_RMSK                                 0xffffffff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5C_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5C_ADDR, HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5C_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5C_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5C_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5C_BCH_PIPELINE_STAGES_BMSK             0xffffffff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5C_BCH_PIPELINE_STAGES_SHFT                    0x0

#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5D_ADDR                                 (BIMC_M_SYS0_MPORT_REG_BASE      + 0x0000007c)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5D_RMSK                                 0xffffffff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5D_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5D_ADDR, HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5D_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5D_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5D_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5D_RCH_PIPELINE_STAGES_BMSK             0xffffffff
#define HWIO_BIMC_M_SYS0_MPORT_CONFIGURATION_INFO_5D_RCH_PIPELINE_STAGES_SHFT                    0x0

#define HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_STATUS_ADDR                                      (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000100)
#define HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_STATUS_RMSK                                             0x3
#define HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_STATUS_ADDR, HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_STATUS_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_STATUS_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_STATUS_RFU_BMSK                                         0x3
#define HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_STATUS_RFU_SHFT                                         0x0

#define HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_CLEAR_ADDR                                       (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000108)
#define HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_CLEAR_RMSK                                              0x3
#define HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_CLEAR_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_CLEAR_ADDR,v)
#define HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_CLEAR_RFU_BMSK                                          0x3
#define HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_CLEAR_RFU_SHFT                                          0x0

#define HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_ENABLE_ADDR                                      (BIMC_M_SYS0_MPORT_REG_BASE      + 0x0000010c)
#define HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_ENABLE_RMSK                                             0x3
#define HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_ENABLE_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_ENABLE_ADDR, HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_ENABLE_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_ENABLE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_ENABLE_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_ENABLE_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_ENABLE_ADDR,v)
#define HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_ENABLE_ADDR,m,v,HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_ENABLE_IN)
#define HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_ENABLE_RFU_BMSK                                         0x3
#define HWIO_BIMC_M_SYS0_MPORT_INTERRUPT_ENABLE_RFU_SHFT                                         0x0

#define HWIO_BIMC_M_SYS0_MPORT_CLOCK_CTRL_ADDR                                            (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000200)
#define HWIO_BIMC_M_SYS0_MPORT_CLOCK_CTRL_RMSK                                                   0x3
#define HWIO_BIMC_M_SYS0_MPORT_CLOCK_CTRL_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CLOCK_CTRL_ADDR, HWIO_BIMC_M_SYS0_MPORT_CLOCK_CTRL_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_CLOCK_CTRL_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CLOCK_CTRL_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_CLOCK_CTRL_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS0_MPORT_CLOCK_CTRL_ADDR,v)
#define HWIO_BIMC_M_SYS0_MPORT_CLOCK_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS0_MPORT_CLOCK_CTRL_ADDR,m,v,HWIO_BIMC_M_SYS0_MPORT_CLOCK_CTRL_IN)
#define HWIO_BIMC_M_SYS0_MPORT_CLOCK_CTRL_MASTER_CLOCK_GATING_EN_BMSK                            0x2
#define HWIO_BIMC_M_SYS0_MPORT_CLOCK_CTRL_MASTER_CLOCK_GATING_EN_SHFT                            0x1
#define HWIO_BIMC_M_SYS0_MPORT_CLOCK_CTRL_CORE_CLOCK_GATING_EN_BMSK                              0x1
#define HWIO_BIMC_M_SYS0_MPORT_CLOCK_CTRL_CORE_CLOCK_GATING_EN_SHFT                              0x0

#define HWIO_BIMC_M_SYS0_MPORT_CDC_CTRL_ADDR                                              (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000208)
#define HWIO_BIMC_M_SYS0_MPORT_CDC_CTRL_RMSK                                                     0x3
#define HWIO_BIMC_M_SYS0_MPORT_CDC_CTRL_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CDC_CTRL_ADDR, HWIO_BIMC_M_SYS0_MPORT_CDC_CTRL_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_CDC_CTRL_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_CDC_CTRL_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_CDC_CTRL_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS0_MPORT_CDC_CTRL_ADDR,v)
#define HWIO_BIMC_M_SYS0_MPORT_CDC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS0_MPORT_CDC_CTRL_ADDR,m,v,HWIO_BIMC_M_SYS0_MPORT_CDC_CTRL_IN)
#define HWIO_BIMC_M_SYS0_MPORT_CDC_CTRL_MASTER_CLOCK_DEMETA_SEL_BMSK                             0x2
#define HWIO_BIMC_M_SYS0_MPORT_CDC_CTRL_MASTER_CLOCK_DEMETA_SEL_SHFT                             0x1
#define HWIO_BIMC_M_SYS0_MPORT_CDC_CTRL_CORE_CLOCK_DEMETA_SEL_BMSK                               0x1
#define HWIO_BIMC_M_SYS0_MPORT_CDC_CTRL_CORE_CLOCK_DEMETA_SEL_SHFT                               0x0

#define HWIO_BIMC_M_SYS0_MPORT_MODE_ADDR                                                  (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000210)
#define HWIO_BIMC_M_SYS0_MPORT_MODE_RMSK                                                  0xf00fff17
#define HWIO_BIMC_M_SYS0_MPORT_MODE_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_MODE_ADDR, HWIO_BIMC_M_SYS0_MPORT_MODE_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_MODE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_MODE_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_MODE_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS0_MPORT_MODE_ADDR,v)
#define HWIO_BIMC_M_SYS0_MPORT_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS0_MPORT_MODE_ADDR,m,v,HWIO_BIMC_M_SYS0_MPORT_MODE_IN)
#define HWIO_BIMC_M_SYS0_MPORT_MODE_WR_GATHER_BEATS_BMSK                                  0xf0000000
#define HWIO_BIMC_M_SYS0_MPORT_MODE_WR_GATHER_BEATS_SHFT                                        0x1c
#define HWIO_BIMC_M_SYS0_MPORT_MODE_RD_GATHER_BEATS_BMSK                                     0xf0000
#define HWIO_BIMC_M_SYS0_MPORT_MODE_RD_GATHER_BEATS_SHFT                                        0x10
#define HWIO_BIMC_M_SYS0_MPORT_MODE_RCSSH_BMSK                                                0x8000
#define HWIO_BIMC_M_SYS0_MPORT_MODE_RCSSH_SHFT                                                   0xf
#define HWIO_BIMC_M_SYS0_MPORT_MODE_RCOSH_BMSK                                                0x4000
#define HWIO_BIMC_M_SYS0_MPORT_MODE_RCOSH_SHFT                                                   0xe
#define HWIO_BIMC_M_SYS0_MPORT_MODE_RCISH_BMSK                                                0x2000
#define HWIO_BIMC_M_SYS0_MPORT_MODE_RCISH_SHFT                                                   0xd
#define HWIO_BIMC_M_SYS0_MPORT_MODE_RCNSH_BMSK                                                0x1000
#define HWIO_BIMC_M_SYS0_MPORT_MODE_RCNSH_SHFT                                                   0xc
#define HWIO_BIMC_M_SYS0_MPORT_MODE_RNCSSH_BMSK                                                0x800
#define HWIO_BIMC_M_SYS0_MPORT_MODE_RNCSSH_SHFT                                                  0xb
#define HWIO_BIMC_M_SYS0_MPORT_MODE_RNCOSH_BMSK                                                0x400
#define HWIO_BIMC_M_SYS0_MPORT_MODE_RNCOSH_SHFT                                                  0xa
#define HWIO_BIMC_M_SYS0_MPORT_MODE_RNCISH_BMSK                                                0x200
#define HWIO_BIMC_M_SYS0_MPORT_MODE_RNCISH_SHFT                                                  0x9
#define HWIO_BIMC_M_SYS0_MPORT_MODE_RNCNSH_BMSK                                                0x100
#define HWIO_BIMC_M_SYS0_MPORT_MODE_RNCNSH_SHFT                                                  0x8
#define HWIO_BIMC_M_SYS0_MPORT_MODE_NARROW_WRITES_BMSK                                          0x10
#define HWIO_BIMC_M_SYS0_MPORT_MODE_NARROW_WRITES_SHFT                                           0x4
#define HWIO_BIMC_M_SYS0_MPORT_MODE_FIODV_BMSK                                                   0x4
#define HWIO_BIMC_M_SYS0_MPORT_MODE_FIODV_SHFT                                                   0x2
#define HWIO_BIMC_M_SYS0_MPORT_MODE_FIOSO_BMSK                                                   0x2
#define HWIO_BIMC_M_SYS0_MPORT_MODE_FIOSO_SHFT                                                   0x1
#define HWIO_BIMC_M_SYS0_MPORT_MODE_ORDERING_MODEL_BMSK                                          0x1
#define HWIO_BIMC_M_SYS0_MPORT_MODE_ORDERING_MODEL_SHFT                                          0x0

#define HWIO_BIMC_M_SYS0_MPORT_WR_WAY_CROSSING_ADDR                                       (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000220)
#define HWIO_BIMC_M_SYS0_MPORT_WR_WAY_CROSSING_RMSK                                              0xf
#define HWIO_BIMC_M_SYS0_MPORT_WR_WAY_CROSSING_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_WR_WAY_CROSSING_ADDR, HWIO_BIMC_M_SYS0_MPORT_WR_WAY_CROSSING_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_WR_WAY_CROSSING_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_WR_WAY_CROSSING_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_WR_WAY_CROSSING_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS0_MPORT_WR_WAY_CROSSING_ADDR,v)
#define HWIO_BIMC_M_SYS0_MPORT_WR_WAY_CROSSING_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS0_MPORT_WR_WAY_CROSSING_ADDR,m,v,HWIO_BIMC_M_SYS0_MPORT_WR_WAY_CROSSING_IN)
#define HWIO_BIMC_M_SYS0_MPORT_WR_WAY_CROSSING_REG_BMSK                                          0xf
#define HWIO_BIMC_M_SYS0_MPORT_WR_WAY_CROSSING_REG_SHFT                                          0x0

#define HWIO_BIMC_M_SYS0_MPORT_PRIORITYLVL_OVERRIDE_ADDR                                  (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000230)
#define HWIO_BIMC_M_SYS0_MPORT_PRIORITYLVL_OVERRIDE_RMSK                                       0x301
#define HWIO_BIMC_M_SYS0_MPORT_PRIORITYLVL_OVERRIDE_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_PRIORITYLVL_OVERRIDE_ADDR, HWIO_BIMC_M_SYS0_MPORT_PRIORITYLVL_OVERRIDE_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_PRIORITYLVL_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_PRIORITYLVL_OVERRIDE_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_PRIORITYLVL_OVERRIDE_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS0_MPORT_PRIORITYLVL_OVERRIDE_ADDR,v)
#define HWIO_BIMC_M_SYS0_MPORT_PRIORITYLVL_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS0_MPORT_PRIORITYLVL_OVERRIDE_ADDR,m,v,HWIO_BIMC_M_SYS0_MPORT_PRIORITYLVL_OVERRIDE_IN)
#define HWIO_BIMC_M_SYS0_MPORT_PRIORITYLVL_OVERRIDE_PRIORITYLVL_BMSK                           0x300
#define HWIO_BIMC_M_SYS0_MPORT_PRIORITYLVL_OVERRIDE_PRIORITYLVL_SHFT                             0x8
#define HWIO_BIMC_M_SYS0_MPORT_PRIORITYLVL_OVERRIDE_OVERRIDE_PRIORITYLVL_BMSK                    0x1
#define HWIO_BIMC_M_SYS0_MPORT_PRIORITYLVL_OVERRIDE_OVERRIDE_PRIORITYLVL_SHFT                    0x0

#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_ADDR                                 (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000240)
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_RMSK                                  0x3071f7f
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_ADDR, HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_ADDR,v)
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_ADDR,m,v,HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_IN)
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_AREQPRIORITY_BMSK                     0x3000000
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_AREQPRIORITY_SHFT                          0x18
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_AMEMTYPE_BMSK                           0x70000
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_AMEMTYPE_SHFT                              0x10
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_ATRANSIENT_BMSK                          0x1000
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_ATRANSIENT_SHFT                             0xc
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_ASHARED_BMSK                              0x800
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_ASHARED_SHFT                                0xb
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_AREDIRECT_BMSK                            0x400
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_AREDIRECT_SHFT                              0xa
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_AOOO_BMSK                                 0x200
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_AOOO_SHFT                                   0x9
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_AINNERSHARED_BMSK                         0x100
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_AINNERSHARED_SHFT                           0x8
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_BMSK                 0x40
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_SHFT                  0x6
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_BMSK                     0x20
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_SHFT                      0x5
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_BMSK                   0x10
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_SHFT                    0x4
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_ASHARED_BMSK                       0x8
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_ASHARED_SHFT                       0x3
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_BMSK                     0x4
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_SHFT                     0x2
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AOOO_BMSK                          0x2
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AOOO_SHFT                          0x1
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_BMSK                  0x1
#define HWIO_BIMC_M_SYS0_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_SHFT                  0x0

#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_ADDR                                (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000250)
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_RMSK                                 0x3071f7f
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_ADDR, HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_ADDR,v)
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_ADDR,m,v,HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_IN)
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_AREQPRIORITY_BMSK                    0x3000000
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_AREQPRIORITY_SHFT                         0x18
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_AMEMTYPE_BMSK                          0x70000
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_AMEMTYPE_SHFT                             0x10
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_ATRANSIENT_BMSK                         0x1000
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_ATRANSIENT_SHFT                            0xc
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_ASHARED_BMSK                             0x800
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_ASHARED_SHFT                               0xb
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_AREDIRECT_BMSK                           0x400
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_AREDIRECT_SHFT                             0xa
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_AOOO_BMSK                                0x200
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_AOOO_SHFT                                  0x9
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_AINNERSHARED_BMSK                        0x100
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_AINNERSHARED_SHFT                          0x8
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_BMSK                0x40
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_SHFT                 0x6
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_BMSK                    0x20
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_SHFT                     0x5
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_BMSK                  0x10
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_SHFT                   0x4
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_ASHARED_BMSK                      0x8
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_ASHARED_SHFT                      0x3
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_BMSK                    0x4
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_SHFT                    0x2
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AOOO_BMSK                         0x2
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AOOO_SHFT                         0x1
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_BMSK                 0x1
#define HWIO_BIMC_M_SYS0_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_SHFT                 0x0

#define HWIO_BIMC_M_SYS0_MPORT_BKE_ENABLE_ADDR                                            (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000300)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_ENABLE_RMSK                                                   0x1
#define HWIO_BIMC_M_SYS0_MPORT_BKE_ENABLE_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_ENABLE_ADDR, HWIO_BIMC_M_SYS0_MPORT_BKE_ENABLE_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_ENABLE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_ENABLE_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_ENABLE_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS0_MPORT_BKE_ENABLE_ADDR,v)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS0_MPORT_BKE_ENABLE_ADDR,m,v,HWIO_BIMC_M_SYS0_MPORT_BKE_ENABLE_IN)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_ENABLE_ENABLE_BMSK                                            0x1
#define HWIO_BIMC_M_SYS0_MPORT_BKE_ENABLE_ENABLE_SHFT                                            0x0

#define HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_PERIOD_ADDR                                      (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000304)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_PERIOD_RMSK                                           0x3ff
#define HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_PERIOD_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_PERIOD_ADDR, HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_PERIOD_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_PERIOD_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_PERIOD_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_PERIOD_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_PERIOD_ADDR,v)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_PERIOD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_PERIOD_ADDR,m,v,HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_PERIOD_IN)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_PERIOD_GRANT_PERIOD_BMSK                              0x3ff
#define HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_PERIOD_GRANT_PERIOD_SHFT                                0x0

#define HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_COUNT_ADDR                                       (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000308)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_COUNT_RMSK                                           0xffff
#define HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_COUNT_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_COUNT_ADDR, HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_COUNT_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_COUNT_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_COUNT_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_COUNT_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_COUNT_ADDR,v)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_COUNT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_COUNT_ADDR,m,v,HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_COUNT_IN)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_COUNT_GRANT_COUNT_BMSK                               0xffff
#define HWIO_BIMC_M_SYS0_MPORT_BKE_GRANT_COUNT_GRANT_COUNT_SHFT                                  0x0

#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_HIGH_ADDR                                    (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000320)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_HIGH_RMSK                                        0xffff
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_HIGH_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_HIGH_ADDR, HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_HIGH_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_HIGH_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_HIGH_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_HIGH_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_HIGH_ADDR,v)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_HIGH_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_HIGH_ADDR,m,v,HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_HIGH_IN)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_HIGH_THRESHOLD_BMSK                              0xffff
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_HIGH_THRESHOLD_SHFT                                 0x0

#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_MEDIUM_ADDR                                  (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000324)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_MEDIUM_RMSK                                      0xffff
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_MEDIUM_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_MEDIUM_ADDR, HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_MEDIUM_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_MEDIUM_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_MEDIUM_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_MEDIUM_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_MEDIUM_ADDR,v)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_MEDIUM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_MEDIUM_ADDR,m,v,HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_MEDIUM_IN)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_MEDIUM_THRESHOLD_BMSK                            0xffff
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_MEDIUM_THRESHOLD_SHFT                               0x0

#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_LOW_ADDR                                     (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000328)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_LOW_RMSK                                         0xffff
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_LOW_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_LOW_ADDR, HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_LOW_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_LOW_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_LOW_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_LOW_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_LOW_ADDR,v)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_LOW_ADDR,m,v,HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_LOW_IN)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_LOW_THRESHOLD_BMSK                               0xffff
#define HWIO_BIMC_M_SYS0_MPORT_BKE_THRESHOLD_LOW_THRESHOLD_SHFT                                  0x0

#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_0_ADDR                                          (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000340)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_0_RMSK                                          0x80000303
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_0_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_0_ADDR, HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_0_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_0_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_0_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_0_ADDR,v)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_0_ADDR,m,v,HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_0_IN)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_0_LIMIT_COMMANDS_BMSK                           0x80000000
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_0_LIMIT_COMMANDS_SHFT                                 0x1f
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_0_AREQPRIORITY_BMSK                                  0x300
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_0_AREQPRIORITY_SHFT                                    0x8
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_0_PRIORITYLVL_BMSK                                     0x3
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_0_PRIORITYLVL_SHFT                                     0x0

#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_1_ADDR                                          (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000344)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_1_RMSK                                          0x80000303
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_1_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_1_ADDR, HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_1_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_1_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_1_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_1_ADDR,v)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_1_ADDR,m,v,HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_1_IN)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_1_LIMIT_COMMANDS_BMSK                           0x80000000
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_1_LIMIT_COMMANDS_SHFT                                 0x1f
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_1_AREQPRIORITY_BMSK                                  0x300
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_1_AREQPRIORITY_SHFT                                    0x8
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_1_PRIORITYLVL_BMSK                                     0x3
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_1_PRIORITYLVL_SHFT                                     0x0

#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_2_ADDR                                          (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000348)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_2_RMSK                                          0x80000303
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_2_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_2_ADDR, HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_2_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_2_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_2_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_2_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_2_ADDR,v)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_2_ADDR,m,v,HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_2_IN)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_2_LIMIT_COMMANDS_BMSK                           0x80000000
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_2_LIMIT_COMMANDS_SHFT                                 0x1f
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_2_AREQPRIORITY_BMSK                                  0x300
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_2_AREQPRIORITY_SHFT                                    0x8
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_2_PRIORITYLVL_BMSK                                     0x3
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_2_PRIORITYLVL_SHFT                                     0x0

#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_3_ADDR                                          (BIMC_M_SYS0_MPORT_REG_BASE      + 0x0000034c)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_3_RMSK                                               0x303
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_3_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_3_ADDR, HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_3_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_3_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_3_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_3_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_3_ADDR,v)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_3_ADDR,m,v,HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_3_IN)
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_3_AREQPRIORITY_BMSK                                  0x300
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_3_AREQPRIORITY_SHFT                                    0x8
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_3_PRIORITYLVL_BMSK                                     0x3
#define HWIO_BIMC_M_SYS0_MPORT_BKE_HEALTH_3_PRIORITYLVL_SHFT                                     0x0

#define HWIO_BIMC_M_SYS0_MPORT_STATUS_2A_ADDR                                             (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000420)
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_2A_RMSK                                               0xff00ff
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_2A_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_STATUS_2A_ADDR, HWIO_BIMC_M_SYS0_MPORT_STATUS_2A_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_2A_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_STATUS_2A_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_2A_WR_PENDING_PRE_BUF_BMSK                            0xff0000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_2A_WR_PENDING_PRE_BUF_SHFT                                0x10
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_2A_RD_PENDING_PRE_BUF_BMSK                                0xff
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_2A_RD_PENDING_PRE_BUF_SHFT                                 0x0

#define HWIO_BIMC_M_SYS0_MPORT_STATUS_2B_ADDR                                             (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000424)
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_2B_RMSK                                               0xff00ff
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_2B_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_STATUS_2B_ADDR, HWIO_BIMC_M_SYS0_MPORT_STATUS_2B_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_2B_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_STATUS_2B_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_2B_WR_PENDING_POST_BUF_BMSK                           0xff0000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_2B_WR_PENDING_POST_BUF_SHFT                               0x10
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_2B_RD_PENDING_POST_BUF_BMSK                               0xff
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_2B_RD_PENDING_POST_BUF_SHFT                                0x0

#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_ADDR                                             (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000430)
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_RMSK                                             0xff333333
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_ADDR, HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_RCB_DATA_RD_FULL_BMSK                            0x80000000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_RCB_DATA_RD_FULL_SHFT                                  0x1f
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_RCB_DATA_RD_EMPTY_BMSK                           0x40000000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_RCB_DATA_RD_EMPTY_SHFT                                 0x1e
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_RCB_CTRL_RD_FULL_BMSK                            0x20000000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_RCB_CTRL_RD_FULL_SHFT                                  0x1d
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_RCB_CTRL_RD_EMPTY_BMSK                           0x10000000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_RCB_CTRL_RD_EMPTY_SHFT                                 0x1c
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_RCB_DATA_WR_FULL_BMSK                             0x8000000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_RCB_DATA_WR_FULL_SHFT                                  0x1b
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_RCB_DATA_WR_EMPTY_BMSK                            0x4000000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_RCB_DATA_WR_EMPTY_SHFT                                 0x1a
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_RCB_CTRL_WR_FULL_BMSK                             0x2000000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_RCB_CTRL_WR_FULL_SHFT                                  0x19
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_RCB_CTRL_WR_EMPTY_BMSK                            0x1000000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_RCB_CTRL_WR_EMPTY_SHFT                                 0x18
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_BCB_RD_FULL_BMSK                                   0x200000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_BCB_RD_FULL_SHFT                                       0x15
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_BCB_RD_EMPTY_BMSK                                  0x100000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_BCB_RD_EMPTY_SHFT                                      0x14
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_BCB_WR_FULL_BMSK                                    0x20000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_BCB_WR_FULL_SHFT                                       0x11
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_BCB_WR_EMPTY_BMSK                                   0x10000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_BCB_WR_EMPTY_SHFT                                      0x10
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_WCB_RD_FULL_BMSK                                     0x2000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_WCB_RD_FULL_SHFT                                        0xd
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_WCB_RD_EMPTY_BMSK                                    0x1000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_WCB_RD_EMPTY_SHFT                                       0xc
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_WCB_WR_FULL_BMSK                                      0x200
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_WCB_WR_FULL_SHFT                                        0x9
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_WCB_WR_EMPTY_BMSK                                     0x100
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_WCB_WR_EMPTY_SHFT                                       0x8
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_ACB_RD_FULL_BMSK                                       0x20
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_ACB_RD_FULL_SHFT                                        0x5
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_ACB_RD_EMPTY_BMSK                                      0x10
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_ACB_RD_EMPTY_SHFT                                       0x4
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_ACB_WR_FULL_BMSK                                        0x2
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_ACB_WR_FULL_SHFT                                        0x1
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_ACB_WR_EMPTY_BMSK                                       0x1
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3A_ACB_WR_EMPTY_SHFT                                       0x0

#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_ADDR                                             (BIMC_M_SYS0_MPORT_REG_BASE      + 0x00000434)
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_RMSK                                               0x333333
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_ADDR, HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_RMSK)
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_ADDR, m)
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_RGB_RD_FULL_BMSK                                   0x200000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_RGB_RD_FULL_SHFT                                       0x15
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_RGB_RD_EMPTY_BMSK                                  0x100000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_RGB_RD_EMPTY_SHFT                                      0x14
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_RGB_WR_FULL_BMSK                                    0x20000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_RGB_WR_FULL_SHFT                                       0x11
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_RGB_WR_EMPTY_BMSK                                   0x10000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_RGB_WR_EMPTY_SHFT                                      0x10
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_W2AB_RD_FULL_BMSK                                    0x2000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_W2AB_RD_FULL_SHFT                                       0xd
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_W2AB_RD_EMPTY_BMSK                                   0x1000
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_W2AB_RD_EMPTY_SHFT                                      0xc
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_W2AB_WR_FULL_BMSK                                     0x200
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_W2AB_WR_FULL_SHFT                                       0x9
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_W2AB_WR_EMPTY_BMSK                                    0x100
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_W2AB_WR_EMPTY_SHFT                                      0x8
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_A2WB_RD_FULL_BMSK                                      0x20
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_A2WB_RD_FULL_SHFT                                       0x5
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_A2WB_RD_EMPTY_BMSK                                     0x10
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_A2WB_RD_EMPTY_SHFT                                      0x4
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_A2WB_WR_FULL_BMSK                                       0x2
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_A2WB_WR_FULL_SHFT                                       0x1
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_A2WB_WR_EMPTY_BMSK                                      0x1
#define HWIO_BIMC_M_SYS0_MPORT_STATUS_3B_A2WB_WR_EMPTY_SHFT                                      0x0

/*----------------------------------------------------------------------------
 * MODULE: BIMC_M_SYS1_MPORT
 *--------------------------------------------------------------------------*/

#define BIMC_M_SYS1_MPORT_REG_BASE                                                        (BIMC_BASE      + 0x00018000)

#define HWIO_BIMC_M_SYS1_MPORT_COMPONENT_INFO_ADDR                                        (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000000)
#define HWIO_BIMC_M_SYS1_MPORT_COMPONENT_INFO_RMSK                                          0xffffff
#define HWIO_BIMC_M_SYS1_MPORT_COMPONENT_INFO_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_COMPONENT_INFO_ADDR, HWIO_BIMC_M_SYS1_MPORT_COMPONENT_INFO_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_COMPONENT_INFO_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_COMPONENT_INFO_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_COMPONENT_INFO_INSTANCE_BMSK                                 0xff0000
#define HWIO_BIMC_M_SYS1_MPORT_COMPONENT_INFO_INSTANCE_SHFT                                     0x10
#define HWIO_BIMC_M_SYS1_MPORT_COMPONENT_INFO_SUB_TYPE_BMSK                                   0xff00
#define HWIO_BIMC_M_SYS1_MPORT_COMPONENT_INFO_SUB_TYPE_SHFT                                      0x8
#define HWIO_BIMC_M_SYS1_MPORT_COMPONENT_INFO_TYPE_BMSK                                         0xff
#define HWIO_BIMC_M_SYS1_MPORT_COMPONENT_INFO_TYPE_SHFT                                          0x0

#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_0_ADDR                                  (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000020)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_0_RMSK                                  0xff00ffff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_0_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_0_ADDR, HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_0_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_0_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_0_SYNC_MODE_BMSK                        0xff000000
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_0_SYNC_MODE_SHFT                              0x18
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_0_CONNECTION_TYPE_BMSK                      0xff00
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_0_CONNECTION_TYPE_SHFT                         0x8
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_0_FUNCTIONALITY_BMSK                          0xff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_0_FUNCTIONALITY_SHFT                           0x0

#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_1_ADDR                                  (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000030)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_1_RMSK                                  0xffffffff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_1_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_1_ADDR, HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_1_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_1_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_1_SWAY_CONNECTIVITY_BMSK                0xffffffff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_1_SWAY_CONNECTIVITY_SHFT                       0x0

#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2A_ADDR                                 (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000040)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2A_RMSK                                 0xffffffff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2A_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2A_ADDR, HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2A_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2A_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2A_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2A_M_DATA_WIDTH_BMSK                    0xffff0000
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2A_M_DATA_WIDTH_SHFT                          0x10
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2A_M_TID_WIDTH_BMSK                         0xff00
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2A_M_TID_WIDTH_SHFT                            0x8
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2A_M_MID_WIDTH_BMSK                           0xff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2A_M_MID_WIDTH_SHFT                            0x0

#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2B_ADDR                                 (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000044)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2B_RMSK                                     0xffff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2B_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2B_ADDR, HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2B_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2B_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2B_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2B_C_DATA_WIDTH_BMSK                        0xffff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_2B_C_DATA_WIDTH_SHFT                           0x0

#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_3_ADDR                                  (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000050)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_3_RMSK                                  0xffffffff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_3_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_3_ADDR, HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_3_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_3_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_3_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_3_RCH_DEPTH_BMSK                        0xff000000
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_3_RCH_DEPTH_SHFT                              0x18
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_3_BCH_DEPTH_BMSK                          0xff0000
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_3_BCH_DEPTH_SHFT                              0x10
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_3_WCH_DEPTH_BMSK                            0xff00
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_3_WCH_DEPTH_SHFT                               0x8
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_3_ACH_DEPTH_BMSK                              0xff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_3_ACH_DEPTH_SHFT                               0x0

#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_4_ADDR                                  (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000060)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_4_RMSK                                      0xffff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_4_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_4_ADDR, HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_4_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_4_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_4_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_4_REORDER_BUFFER_DEPTH_BMSK                 0xff00
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_4_REORDER_BUFFER_DEPTH_SHFT                    0x8
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_4_REORDER_TABLE_DEPTH_BMSK                    0xff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_4_REORDER_TABLE_DEPTH_SHFT                     0x0

#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5A_ADDR                                 (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000070)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5A_RMSK                                 0xffffffff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5A_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5A_ADDR, HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5A_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5A_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5A_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5A_ACH_PIPELINE_STAGES_BMSK             0xffffffff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5A_ACH_PIPELINE_STAGES_SHFT                    0x0

#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5B_ADDR                                 (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000074)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5B_RMSK                                 0xffffffff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5B_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5B_ADDR, HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5B_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5B_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5B_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5B_WCH_PIPELINE_STAGES_BMSK             0xffffffff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5B_WCH_PIPELINE_STAGES_SHFT                    0x0

#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5C_ADDR                                 (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000078)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5C_RMSK                                 0xffffffff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5C_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5C_ADDR, HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5C_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5C_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5C_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5C_BCH_PIPELINE_STAGES_BMSK             0xffffffff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5C_BCH_PIPELINE_STAGES_SHFT                    0x0

#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5D_ADDR                                 (BIMC_M_SYS1_MPORT_REG_BASE      + 0x0000007c)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5D_RMSK                                 0xffffffff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5D_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5D_ADDR, HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5D_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5D_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5D_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5D_RCH_PIPELINE_STAGES_BMSK             0xffffffff
#define HWIO_BIMC_M_SYS1_MPORT_CONFIGURATION_INFO_5D_RCH_PIPELINE_STAGES_SHFT                    0x0

#define HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_STATUS_ADDR                                      (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000100)
#define HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_STATUS_RMSK                                             0x3
#define HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_STATUS_ADDR, HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_STATUS_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_STATUS_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_STATUS_RFU_BMSK                                         0x3
#define HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_STATUS_RFU_SHFT                                         0x0

#define HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_CLEAR_ADDR                                       (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000108)
#define HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_CLEAR_RMSK                                              0x3
#define HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_CLEAR_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_CLEAR_ADDR,v)
#define HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_CLEAR_RFU_BMSK                                          0x3
#define HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_CLEAR_RFU_SHFT                                          0x0

#define HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_ENABLE_ADDR                                      (BIMC_M_SYS1_MPORT_REG_BASE      + 0x0000010c)
#define HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_ENABLE_RMSK                                             0x3
#define HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_ENABLE_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_ENABLE_ADDR, HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_ENABLE_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_ENABLE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_ENABLE_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_ENABLE_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_ENABLE_ADDR,v)
#define HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_ENABLE_ADDR,m,v,HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_ENABLE_IN)
#define HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_ENABLE_RFU_BMSK                                         0x3
#define HWIO_BIMC_M_SYS1_MPORT_INTERRUPT_ENABLE_RFU_SHFT                                         0x0

#define HWIO_BIMC_M_SYS1_MPORT_CLOCK_CTRL_ADDR                                            (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000200)
#define HWIO_BIMC_M_SYS1_MPORT_CLOCK_CTRL_RMSK                                                   0x3
#define HWIO_BIMC_M_SYS1_MPORT_CLOCK_CTRL_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CLOCK_CTRL_ADDR, HWIO_BIMC_M_SYS1_MPORT_CLOCK_CTRL_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_CLOCK_CTRL_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CLOCK_CTRL_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_CLOCK_CTRL_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS1_MPORT_CLOCK_CTRL_ADDR,v)
#define HWIO_BIMC_M_SYS1_MPORT_CLOCK_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS1_MPORT_CLOCK_CTRL_ADDR,m,v,HWIO_BIMC_M_SYS1_MPORT_CLOCK_CTRL_IN)
#define HWIO_BIMC_M_SYS1_MPORT_CLOCK_CTRL_MASTER_CLOCK_GATING_EN_BMSK                            0x2
#define HWIO_BIMC_M_SYS1_MPORT_CLOCK_CTRL_MASTER_CLOCK_GATING_EN_SHFT                            0x1
#define HWIO_BIMC_M_SYS1_MPORT_CLOCK_CTRL_CORE_CLOCK_GATING_EN_BMSK                              0x1
#define HWIO_BIMC_M_SYS1_MPORT_CLOCK_CTRL_CORE_CLOCK_GATING_EN_SHFT                              0x0

#define HWIO_BIMC_M_SYS1_MPORT_CDC_CTRL_ADDR                                              (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000208)
#define HWIO_BIMC_M_SYS1_MPORT_CDC_CTRL_RMSK                                                     0x3
#define HWIO_BIMC_M_SYS1_MPORT_CDC_CTRL_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CDC_CTRL_ADDR, HWIO_BIMC_M_SYS1_MPORT_CDC_CTRL_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_CDC_CTRL_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_CDC_CTRL_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_CDC_CTRL_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS1_MPORT_CDC_CTRL_ADDR,v)
#define HWIO_BIMC_M_SYS1_MPORT_CDC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS1_MPORT_CDC_CTRL_ADDR,m,v,HWIO_BIMC_M_SYS1_MPORT_CDC_CTRL_IN)
#define HWIO_BIMC_M_SYS1_MPORT_CDC_CTRL_MASTER_CLOCK_DEMETA_SEL_BMSK                             0x2
#define HWIO_BIMC_M_SYS1_MPORT_CDC_CTRL_MASTER_CLOCK_DEMETA_SEL_SHFT                             0x1
#define HWIO_BIMC_M_SYS1_MPORT_CDC_CTRL_CORE_CLOCK_DEMETA_SEL_BMSK                               0x1
#define HWIO_BIMC_M_SYS1_MPORT_CDC_CTRL_CORE_CLOCK_DEMETA_SEL_SHFT                               0x0

#define HWIO_BIMC_M_SYS1_MPORT_MODE_ADDR                                                  (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000210)
#define HWIO_BIMC_M_SYS1_MPORT_MODE_RMSK                                                  0xf00fff17
#define HWIO_BIMC_M_SYS1_MPORT_MODE_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_MODE_ADDR, HWIO_BIMC_M_SYS1_MPORT_MODE_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_MODE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_MODE_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_MODE_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS1_MPORT_MODE_ADDR,v)
#define HWIO_BIMC_M_SYS1_MPORT_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS1_MPORT_MODE_ADDR,m,v,HWIO_BIMC_M_SYS1_MPORT_MODE_IN)
#define HWIO_BIMC_M_SYS1_MPORT_MODE_WR_GATHER_BEATS_BMSK                                  0xf0000000
#define HWIO_BIMC_M_SYS1_MPORT_MODE_WR_GATHER_BEATS_SHFT                                        0x1c
#define HWIO_BIMC_M_SYS1_MPORT_MODE_RD_GATHER_BEATS_BMSK                                     0xf0000
#define HWIO_BIMC_M_SYS1_MPORT_MODE_RD_GATHER_BEATS_SHFT                                        0x10
#define HWIO_BIMC_M_SYS1_MPORT_MODE_RCSSH_BMSK                                                0x8000
#define HWIO_BIMC_M_SYS1_MPORT_MODE_RCSSH_SHFT                                                   0xf
#define HWIO_BIMC_M_SYS1_MPORT_MODE_RCOSH_BMSK                                                0x4000
#define HWIO_BIMC_M_SYS1_MPORT_MODE_RCOSH_SHFT                                                   0xe
#define HWIO_BIMC_M_SYS1_MPORT_MODE_RCISH_BMSK                                                0x2000
#define HWIO_BIMC_M_SYS1_MPORT_MODE_RCISH_SHFT                                                   0xd
#define HWIO_BIMC_M_SYS1_MPORT_MODE_RCNSH_BMSK                                                0x1000
#define HWIO_BIMC_M_SYS1_MPORT_MODE_RCNSH_SHFT                                                   0xc
#define HWIO_BIMC_M_SYS1_MPORT_MODE_RNCSSH_BMSK                                                0x800
#define HWIO_BIMC_M_SYS1_MPORT_MODE_RNCSSH_SHFT                                                  0xb
#define HWIO_BIMC_M_SYS1_MPORT_MODE_RNCOSH_BMSK                                                0x400
#define HWIO_BIMC_M_SYS1_MPORT_MODE_RNCOSH_SHFT                                                  0xa
#define HWIO_BIMC_M_SYS1_MPORT_MODE_RNCISH_BMSK                                                0x200
#define HWIO_BIMC_M_SYS1_MPORT_MODE_RNCISH_SHFT                                                  0x9
#define HWIO_BIMC_M_SYS1_MPORT_MODE_RNCNSH_BMSK                                                0x100
#define HWIO_BIMC_M_SYS1_MPORT_MODE_RNCNSH_SHFT                                                  0x8
#define HWIO_BIMC_M_SYS1_MPORT_MODE_NARROW_WRITES_BMSK                                          0x10
#define HWIO_BIMC_M_SYS1_MPORT_MODE_NARROW_WRITES_SHFT                                           0x4
#define HWIO_BIMC_M_SYS1_MPORT_MODE_FIODV_BMSK                                                   0x4
#define HWIO_BIMC_M_SYS1_MPORT_MODE_FIODV_SHFT                                                   0x2
#define HWIO_BIMC_M_SYS1_MPORT_MODE_FIOSO_BMSK                                                   0x2
#define HWIO_BIMC_M_SYS1_MPORT_MODE_FIOSO_SHFT                                                   0x1
#define HWIO_BIMC_M_SYS1_MPORT_MODE_ORDERING_MODEL_BMSK                                          0x1
#define HWIO_BIMC_M_SYS1_MPORT_MODE_ORDERING_MODEL_SHFT                                          0x0

#define HWIO_BIMC_M_SYS1_MPORT_WR_WAY_CROSSING_ADDR                                       (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000220)
#define HWIO_BIMC_M_SYS1_MPORT_WR_WAY_CROSSING_RMSK                                              0xf
#define HWIO_BIMC_M_SYS1_MPORT_WR_WAY_CROSSING_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_WR_WAY_CROSSING_ADDR, HWIO_BIMC_M_SYS1_MPORT_WR_WAY_CROSSING_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_WR_WAY_CROSSING_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_WR_WAY_CROSSING_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_WR_WAY_CROSSING_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS1_MPORT_WR_WAY_CROSSING_ADDR,v)
#define HWIO_BIMC_M_SYS1_MPORT_WR_WAY_CROSSING_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS1_MPORT_WR_WAY_CROSSING_ADDR,m,v,HWIO_BIMC_M_SYS1_MPORT_WR_WAY_CROSSING_IN)
#define HWIO_BIMC_M_SYS1_MPORT_WR_WAY_CROSSING_REG_BMSK                                          0xf
#define HWIO_BIMC_M_SYS1_MPORT_WR_WAY_CROSSING_REG_SHFT                                          0x0

#define HWIO_BIMC_M_SYS1_MPORT_PRIORITYLVL_OVERRIDE_ADDR                                  (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000230)
#define HWIO_BIMC_M_SYS1_MPORT_PRIORITYLVL_OVERRIDE_RMSK                                       0x301
#define HWIO_BIMC_M_SYS1_MPORT_PRIORITYLVL_OVERRIDE_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_PRIORITYLVL_OVERRIDE_ADDR, HWIO_BIMC_M_SYS1_MPORT_PRIORITYLVL_OVERRIDE_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_PRIORITYLVL_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_PRIORITYLVL_OVERRIDE_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_PRIORITYLVL_OVERRIDE_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS1_MPORT_PRIORITYLVL_OVERRIDE_ADDR,v)
#define HWIO_BIMC_M_SYS1_MPORT_PRIORITYLVL_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS1_MPORT_PRIORITYLVL_OVERRIDE_ADDR,m,v,HWIO_BIMC_M_SYS1_MPORT_PRIORITYLVL_OVERRIDE_IN)
#define HWIO_BIMC_M_SYS1_MPORT_PRIORITYLVL_OVERRIDE_PRIORITYLVL_BMSK                           0x300
#define HWIO_BIMC_M_SYS1_MPORT_PRIORITYLVL_OVERRIDE_PRIORITYLVL_SHFT                             0x8
#define HWIO_BIMC_M_SYS1_MPORT_PRIORITYLVL_OVERRIDE_OVERRIDE_PRIORITYLVL_BMSK                    0x1
#define HWIO_BIMC_M_SYS1_MPORT_PRIORITYLVL_OVERRIDE_OVERRIDE_PRIORITYLVL_SHFT                    0x0

#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_ADDR                                 (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000240)
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_RMSK                                  0x3071f7f
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_ADDR, HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_ADDR,v)
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_ADDR,m,v,HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_IN)
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_AREQPRIORITY_BMSK                     0x3000000
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_AREQPRIORITY_SHFT                          0x18
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_AMEMTYPE_BMSK                           0x70000
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_AMEMTYPE_SHFT                              0x10
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_ATRANSIENT_BMSK                          0x1000
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_ATRANSIENT_SHFT                             0xc
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_ASHARED_BMSK                              0x800
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_ASHARED_SHFT                                0xb
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_AREDIRECT_BMSK                            0x400
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_AREDIRECT_SHFT                              0xa
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_AOOO_BMSK                                 0x200
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_AOOO_SHFT                                   0x9
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_AINNERSHARED_BMSK                         0x100
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_AINNERSHARED_SHFT                           0x8
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_BMSK                 0x40
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_SHFT                  0x6
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_BMSK                     0x20
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_SHFT                      0x5
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_BMSK                   0x10
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_SHFT                    0x4
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_ASHARED_BMSK                       0x8
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_ASHARED_SHFT                       0x3
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_BMSK                     0x4
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_SHFT                     0x2
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AOOO_BMSK                          0x2
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AOOO_SHFT                          0x1
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_BMSK                  0x1
#define HWIO_BIMC_M_SYS1_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_SHFT                  0x0

#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_ADDR                                (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000250)
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_RMSK                                 0x3071f7f
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_ADDR, HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_ADDR,v)
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_ADDR,m,v,HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_IN)
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_AREQPRIORITY_BMSK                    0x3000000
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_AREQPRIORITY_SHFT                         0x18
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_AMEMTYPE_BMSK                          0x70000
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_AMEMTYPE_SHFT                             0x10
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_ATRANSIENT_BMSK                         0x1000
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_ATRANSIENT_SHFT                            0xc
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_ASHARED_BMSK                             0x800
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_ASHARED_SHFT                               0xb
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_AREDIRECT_BMSK                           0x400
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_AREDIRECT_SHFT                             0xa
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_AOOO_BMSK                                0x200
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_AOOO_SHFT                                  0x9
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_AINNERSHARED_BMSK                        0x100
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_AINNERSHARED_SHFT                          0x8
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_BMSK                0x40
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_SHFT                 0x6
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_BMSK                    0x20
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_SHFT                     0x5
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_BMSK                  0x10
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_SHFT                   0x4
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_ASHARED_BMSK                      0x8
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_ASHARED_SHFT                      0x3
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_BMSK                    0x4
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_SHFT                    0x2
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AOOO_BMSK                         0x2
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AOOO_SHFT                         0x1
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_BMSK                 0x1
#define HWIO_BIMC_M_SYS1_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_SHFT                 0x0

#define HWIO_BIMC_M_SYS1_MPORT_BKE_ENABLE_ADDR                                            (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000300)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_ENABLE_RMSK                                                   0x1
#define HWIO_BIMC_M_SYS1_MPORT_BKE_ENABLE_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_ENABLE_ADDR, HWIO_BIMC_M_SYS1_MPORT_BKE_ENABLE_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_ENABLE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_ENABLE_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_ENABLE_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS1_MPORT_BKE_ENABLE_ADDR,v)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS1_MPORT_BKE_ENABLE_ADDR,m,v,HWIO_BIMC_M_SYS1_MPORT_BKE_ENABLE_IN)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_ENABLE_ENABLE_BMSK                                            0x1
#define HWIO_BIMC_M_SYS1_MPORT_BKE_ENABLE_ENABLE_SHFT                                            0x0

#define HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_PERIOD_ADDR                                      (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000304)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_PERIOD_RMSK                                           0x3ff
#define HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_PERIOD_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_PERIOD_ADDR, HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_PERIOD_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_PERIOD_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_PERIOD_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_PERIOD_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_PERIOD_ADDR,v)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_PERIOD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_PERIOD_ADDR,m,v,HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_PERIOD_IN)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_PERIOD_GRANT_PERIOD_BMSK                              0x3ff
#define HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_PERIOD_GRANT_PERIOD_SHFT                                0x0

#define HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_COUNT_ADDR                                       (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000308)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_COUNT_RMSK                                           0xffff
#define HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_COUNT_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_COUNT_ADDR, HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_COUNT_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_COUNT_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_COUNT_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_COUNT_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_COUNT_ADDR,v)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_COUNT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_COUNT_ADDR,m,v,HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_COUNT_IN)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_COUNT_GRANT_COUNT_BMSK                               0xffff
#define HWIO_BIMC_M_SYS1_MPORT_BKE_GRANT_COUNT_GRANT_COUNT_SHFT                                  0x0

#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_HIGH_ADDR                                    (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000320)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_HIGH_RMSK                                        0xffff
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_HIGH_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_HIGH_ADDR, HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_HIGH_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_HIGH_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_HIGH_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_HIGH_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_HIGH_ADDR,v)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_HIGH_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_HIGH_ADDR,m,v,HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_HIGH_IN)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_HIGH_THRESHOLD_BMSK                              0xffff
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_HIGH_THRESHOLD_SHFT                                 0x0

#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_MEDIUM_ADDR                                  (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000324)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_MEDIUM_RMSK                                      0xffff
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_MEDIUM_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_MEDIUM_ADDR, HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_MEDIUM_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_MEDIUM_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_MEDIUM_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_MEDIUM_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_MEDIUM_ADDR,v)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_MEDIUM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_MEDIUM_ADDR,m,v,HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_MEDIUM_IN)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_MEDIUM_THRESHOLD_BMSK                            0xffff
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_MEDIUM_THRESHOLD_SHFT                               0x0

#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_LOW_ADDR                                     (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000328)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_LOW_RMSK                                         0xffff
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_LOW_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_LOW_ADDR, HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_LOW_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_LOW_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_LOW_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_LOW_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_LOW_ADDR,v)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_LOW_ADDR,m,v,HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_LOW_IN)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_LOW_THRESHOLD_BMSK                               0xffff
#define HWIO_BIMC_M_SYS1_MPORT_BKE_THRESHOLD_LOW_THRESHOLD_SHFT                                  0x0

#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_0_ADDR                                          (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000340)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_0_RMSK                                          0x80000303
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_0_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_0_ADDR, HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_0_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_0_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_0_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_0_ADDR,v)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_0_ADDR,m,v,HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_0_IN)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_0_LIMIT_COMMANDS_BMSK                           0x80000000
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_0_LIMIT_COMMANDS_SHFT                                 0x1f
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_0_AREQPRIORITY_BMSK                                  0x300
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_0_AREQPRIORITY_SHFT                                    0x8
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_0_PRIORITYLVL_BMSK                                     0x3
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_0_PRIORITYLVL_SHFT                                     0x0

#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_1_ADDR                                          (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000344)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_1_RMSK                                          0x80000303
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_1_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_1_ADDR, HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_1_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_1_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_1_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_1_ADDR,v)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_1_ADDR,m,v,HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_1_IN)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_1_LIMIT_COMMANDS_BMSK                           0x80000000
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_1_LIMIT_COMMANDS_SHFT                                 0x1f
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_1_AREQPRIORITY_BMSK                                  0x300
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_1_AREQPRIORITY_SHFT                                    0x8
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_1_PRIORITYLVL_BMSK                                     0x3
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_1_PRIORITYLVL_SHFT                                     0x0

#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_2_ADDR                                          (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000348)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_2_RMSK                                          0x80000303
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_2_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_2_ADDR, HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_2_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_2_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_2_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_2_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_2_ADDR,v)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_2_ADDR,m,v,HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_2_IN)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_2_LIMIT_COMMANDS_BMSK                           0x80000000
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_2_LIMIT_COMMANDS_SHFT                                 0x1f
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_2_AREQPRIORITY_BMSK                                  0x300
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_2_AREQPRIORITY_SHFT                                    0x8
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_2_PRIORITYLVL_BMSK                                     0x3
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_2_PRIORITYLVL_SHFT                                     0x0

#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_3_ADDR                                          (BIMC_M_SYS1_MPORT_REG_BASE      + 0x0000034c)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_3_RMSK                                               0x303
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_3_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_3_ADDR, HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_3_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_3_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_3_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_3_OUT(v)      \
        out_dword(HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_3_ADDR,v)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_3_ADDR,m,v,HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_3_IN)
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_3_AREQPRIORITY_BMSK                                  0x300
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_3_AREQPRIORITY_SHFT                                    0x8
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_3_PRIORITYLVL_BMSK                                     0x3
#define HWIO_BIMC_M_SYS1_MPORT_BKE_HEALTH_3_PRIORITYLVL_SHFT                                     0x0

#define HWIO_BIMC_M_SYS1_MPORT_STATUS_2A_ADDR                                             (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000420)
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_2A_RMSK                                               0xff00ff
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_2A_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_STATUS_2A_ADDR, HWIO_BIMC_M_SYS1_MPORT_STATUS_2A_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_2A_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_STATUS_2A_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_2A_WR_PENDING_PRE_BUF_BMSK                            0xff0000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_2A_WR_PENDING_PRE_BUF_SHFT                                0x10
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_2A_RD_PENDING_PRE_BUF_BMSK                                0xff
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_2A_RD_PENDING_PRE_BUF_SHFT                                 0x0

#define HWIO_BIMC_M_SYS1_MPORT_STATUS_2B_ADDR                                             (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000424)
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_2B_RMSK                                               0xff00ff
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_2B_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_STATUS_2B_ADDR, HWIO_BIMC_M_SYS1_MPORT_STATUS_2B_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_2B_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_STATUS_2B_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_2B_WR_PENDING_POST_BUF_BMSK                           0xff0000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_2B_WR_PENDING_POST_BUF_SHFT                               0x10
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_2B_RD_PENDING_POST_BUF_BMSK                               0xff
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_2B_RD_PENDING_POST_BUF_SHFT                                0x0

#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_ADDR                                             (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000430)
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_RMSK                                             0xff333333
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_ADDR, HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_RCB_DATA_RD_FULL_BMSK                            0x80000000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_RCB_DATA_RD_FULL_SHFT                                  0x1f
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_RCB_DATA_RD_EMPTY_BMSK                           0x40000000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_RCB_DATA_RD_EMPTY_SHFT                                 0x1e
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_RCB_CTRL_RD_FULL_BMSK                            0x20000000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_RCB_CTRL_RD_FULL_SHFT                                  0x1d
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_RCB_CTRL_RD_EMPTY_BMSK                           0x10000000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_RCB_CTRL_RD_EMPTY_SHFT                                 0x1c
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_RCB_DATA_WR_FULL_BMSK                             0x8000000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_RCB_DATA_WR_FULL_SHFT                                  0x1b
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_RCB_DATA_WR_EMPTY_BMSK                            0x4000000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_RCB_DATA_WR_EMPTY_SHFT                                 0x1a
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_RCB_CTRL_WR_FULL_BMSK                             0x2000000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_RCB_CTRL_WR_FULL_SHFT                                  0x19
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_RCB_CTRL_WR_EMPTY_BMSK                            0x1000000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_RCB_CTRL_WR_EMPTY_SHFT                                 0x18
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_BCB_RD_FULL_BMSK                                   0x200000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_BCB_RD_FULL_SHFT                                       0x15
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_BCB_RD_EMPTY_BMSK                                  0x100000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_BCB_RD_EMPTY_SHFT                                      0x14
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_BCB_WR_FULL_BMSK                                    0x20000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_BCB_WR_FULL_SHFT                                       0x11
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_BCB_WR_EMPTY_BMSK                                   0x10000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_BCB_WR_EMPTY_SHFT                                      0x10
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_WCB_RD_FULL_BMSK                                     0x2000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_WCB_RD_FULL_SHFT                                        0xd
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_WCB_RD_EMPTY_BMSK                                    0x1000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_WCB_RD_EMPTY_SHFT                                       0xc
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_WCB_WR_FULL_BMSK                                      0x200
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_WCB_WR_FULL_SHFT                                        0x9
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_WCB_WR_EMPTY_BMSK                                     0x100
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_WCB_WR_EMPTY_SHFT                                       0x8
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_ACB_RD_FULL_BMSK                                       0x20
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_ACB_RD_FULL_SHFT                                        0x5
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_ACB_RD_EMPTY_BMSK                                      0x10
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_ACB_RD_EMPTY_SHFT                                       0x4
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_ACB_WR_FULL_BMSK                                        0x2
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_ACB_WR_FULL_SHFT                                        0x1
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_ACB_WR_EMPTY_BMSK                                       0x1
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3A_ACB_WR_EMPTY_SHFT                                       0x0

#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_ADDR                                             (BIMC_M_SYS1_MPORT_REG_BASE      + 0x00000434)
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_RMSK                                               0x333333
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_IN          \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_ADDR, HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_RMSK)
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_ADDR, m)
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_RGB_RD_FULL_BMSK                                   0x200000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_RGB_RD_FULL_SHFT                                       0x15
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_RGB_RD_EMPTY_BMSK                                  0x100000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_RGB_RD_EMPTY_SHFT                                      0x14
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_RGB_WR_FULL_BMSK                                    0x20000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_RGB_WR_FULL_SHFT                                       0x11
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_RGB_WR_EMPTY_BMSK                                   0x10000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_RGB_WR_EMPTY_SHFT                                      0x10
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_W2AB_RD_FULL_BMSK                                    0x2000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_W2AB_RD_FULL_SHFT                                       0xd
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_W2AB_RD_EMPTY_BMSK                                   0x1000
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_W2AB_RD_EMPTY_SHFT                                      0xc
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_W2AB_WR_FULL_BMSK                                     0x200
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_W2AB_WR_FULL_SHFT                                       0x9
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_W2AB_WR_EMPTY_BMSK                                    0x100
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_W2AB_WR_EMPTY_SHFT                                      0x8
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_A2WB_RD_FULL_BMSK                                      0x20
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_A2WB_RD_FULL_SHFT                                       0x5
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_A2WB_RD_EMPTY_BMSK                                     0x10
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_A2WB_RD_EMPTY_SHFT                                      0x4
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_A2WB_WR_FULL_BMSK                                       0x2
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_A2WB_WR_FULL_SHFT                                       0x1
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_A2WB_WR_EMPTY_BMSK                                      0x1
#define HWIO_BIMC_M_SYS1_MPORT_STATUS_3B_A2WB_WR_EMPTY_SHFT                                      0x0

/*----------------------------------------------------------------------------
 * MODULE: BIMC_M_DSP_MPORT
 *--------------------------------------------------------------------------*/

#define BIMC_M_DSP_MPORT_REG_BASE                                                        (BIMC_BASE      + 0x0000c000)

#define HWIO_BIMC_M_DSP_MPORT_COMPONENT_INFO_ADDR                                        (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000000)
#define HWIO_BIMC_M_DSP_MPORT_COMPONENT_INFO_RMSK                                          0xffffff
#define HWIO_BIMC_M_DSP_MPORT_COMPONENT_INFO_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_COMPONENT_INFO_ADDR, HWIO_BIMC_M_DSP_MPORT_COMPONENT_INFO_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_COMPONENT_INFO_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_COMPONENT_INFO_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_COMPONENT_INFO_INSTANCE_BMSK                                 0xff0000
#define HWIO_BIMC_M_DSP_MPORT_COMPONENT_INFO_INSTANCE_SHFT                                     0x10
#define HWIO_BIMC_M_DSP_MPORT_COMPONENT_INFO_SUB_TYPE_BMSK                                   0xff00
#define HWIO_BIMC_M_DSP_MPORT_COMPONENT_INFO_SUB_TYPE_SHFT                                      0x8
#define HWIO_BIMC_M_DSP_MPORT_COMPONENT_INFO_TYPE_BMSK                                         0xff
#define HWIO_BIMC_M_DSP_MPORT_COMPONENT_INFO_TYPE_SHFT                                          0x0

#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_0_ADDR                                  (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000020)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_0_RMSK                                  0xff00ffff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_0_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_0_ADDR, HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_0_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_0_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_0_SYNC_MODE_BMSK                        0xff000000
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_0_SYNC_MODE_SHFT                              0x18
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_0_CONNECTION_TYPE_BMSK                      0xff00
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_0_CONNECTION_TYPE_SHFT                         0x8
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_0_FUNCTIONALITY_BMSK                          0xff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_0_FUNCTIONALITY_SHFT                           0x0

#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_1_ADDR                                  (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000030)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_1_RMSK                                  0xffffffff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_1_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_1_ADDR, HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_1_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_1_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_1_SWAY_CONNECTIVITY_BMSK                0xffffffff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_1_SWAY_CONNECTIVITY_SHFT                       0x0

#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2A_ADDR                                 (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000040)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2A_RMSK                                 0xffffffff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2A_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2A_ADDR, HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2A_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2A_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2A_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2A_M_DATA_WIDTH_BMSK                    0xffff0000
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2A_M_DATA_WIDTH_SHFT                          0x10
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2A_M_TID_WIDTH_BMSK                         0xff00
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2A_M_TID_WIDTH_SHFT                            0x8
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2A_M_MID_WIDTH_BMSK                           0xff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2A_M_MID_WIDTH_SHFT                            0x0

#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2B_ADDR                                 (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000044)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2B_RMSK                                     0xffff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2B_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2B_ADDR, HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2B_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2B_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2B_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2B_C_DATA_WIDTH_BMSK                        0xffff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_2B_C_DATA_WIDTH_SHFT                           0x0

#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_3_ADDR                                  (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000050)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_3_RMSK                                  0xffffffff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_3_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_3_ADDR, HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_3_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_3_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_3_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_3_RCH_DEPTH_BMSK                        0xff000000
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_3_RCH_DEPTH_SHFT                              0x18
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_3_BCH_DEPTH_BMSK                          0xff0000
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_3_BCH_DEPTH_SHFT                              0x10
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_3_WCH_DEPTH_BMSK                            0xff00
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_3_WCH_DEPTH_SHFT                               0x8
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_3_ACH_DEPTH_BMSK                              0xff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_3_ACH_DEPTH_SHFT                               0x0

#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_4_ADDR                                  (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000060)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_4_RMSK                                      0xffff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_4_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_4_ADDR, HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_4_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_4_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_4_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_4_REORDER_BUFFER_DEPTH_BMSK                 0xff00
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_4_REORDER_BUFFER_DEPTH_SHFT                    0x8
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_4_REORDER_TABLE_DEPTH_BMSK                    0xff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_4_REORDER_TABLE_DEPTH_SHFT                     0x0

#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5A_ADDR                                 (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000070)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5A_RMSK                                 0xffffffff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5A_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5A_ADDR, HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5A_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5A_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5A_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5A_ACH_PIPELINE_STAGES_BMSK             0xffffffff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5A_ACH_PIPELINE_STAGES_SHFT                    0x0

#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5B_ADDR                                 (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000074)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5B_RMSK                                 0xffffffff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5B_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5B_ADDR, HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5B_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5B_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5B_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5B_WCH_PIPELINE_STAGES_BMSK             0xffffffff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5B_WCH_PIPELINE_STAGES_SHFT                    0x0

#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5C_ADDR                                 (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000078)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5C_RMSK                                 0xffffffff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5C_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5C_ADDR, HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5C_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5C_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5C_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5C_BCH_PIPELINE_STAGES_BMSK             0xffffffff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5C_BCH_PIPELINE_STAGES_SHFT                    0x0

#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5D_ADDR                                 (BIMC_M_DSP_MPORT_REG_BASE      + 0x0000007c)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5D_RMSK                                 0xffffffff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5D_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5D_ADDR, HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5D_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5D_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5D_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5D_RCH_PIPELINE_STAGES_BMSK             0xffffffff
#define HWIO_BIMC_M_DSP_MPORT_CONFIGURATION_INFO_5D_RCH_PIPELINE_STAGES_SHFT                    0x0

#define HWIO_BIMC_M_DSP_MPORT_INTERRUPT_STATUS_ADDR                                      (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000100)
#define HWIO_BIMC_M_DSP_MPORT_INTERRUPT_STATUS_RMSK                                             0x3
#define HWIO_BIMC_M_DSP_MPORT_INTERRUPT_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_INTERRUPT_STATUS_ADDR, HWIO_BIMC_M_DSP_MPORT_INTERRUPT_STATUS_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_INTERRUPT_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_INTERRUPT_STATUS_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_INTERRUPT_STATUS_RFU_BMSK                                         0x3
#define HWIO_BIMC_M_DSP_MPORT_INTERRUPT_STATUS_RFU_SHFT                                         0x0

#define HWIO_BIMC_M_DSP_MPORT_INTERRUPT_CLEAR_ADDR                                       (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000108)
#define HWIO_BIMC_M_DSP_MPORT_INTERRUPT_CLEAR_RMSK                                              0x3
#define HWIO_BIMC_M_DSP_MPORT_INTERRUPT_CLEAR_OUT(v)      \
        out_dword(HWIO_BIMC_M_DSP_MPORT_INTERRUPT_CLEAR_ADDR,v)
#define HWIO_BIMC_M_DSP_MPORT_INTERRUPT_CLEAR_RFU_BMSK                                          0x3
#define HWIO_BIMC_M_DSP_MPORT_INTERRUPT_CLEAR_RFU_SHFT                                          0x0

#define HWIO_BIMC_M_DSP_MPORT_INTERRUPT_ENABLE_ADDR                                      (BIMC_M_DSP_MPORT_REG_BASE      + 0x0000010c)
#define HWIO_BIMC_M_DSP_MPORT_INTERRUPT_ENABLE_RMSK                                             0x3
#define HWIO_BIMC_M_DSP_MPORT_INTERRUPT_ENABLE_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_INTERRUPT_ENABLE_ADDR, HWIO_BIMC_M_DSP_MPORT_INTERRUPT_ENABLE_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_INTERRUPT_ENABLE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_INTERRUPT_ENABLE_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_INTERRUPT_ENABLE_OUT(v)      \
        out_dword(HWIO_BIMC_M_DSP_MPORT_INTERRUPT_ENABLE_ADDR,v)
#define HWIO_BIMC_M_DSP_MPORT_INTERRUPT_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_DSP_MPORT_INTERRUPT_ENABLE_ADDR,m,v,HWIO_BIMC_M_DSP_MPORT_INTERRUPT_ENABLE_IN)
#define HWIO_BIMC_M_DSP_MPORT_INTERRUPT_ENABLE_RFU_BMSK                                         0x3
#define HWIO_BIMC_M_DSP_MPORT_INTERRUPT_ENABLE_RFU_SHFT                                         0x0

#define HWIO_BIMC_M_DSP_MPORT_CLOCK_CTRL_ADDR                                            (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000200)
#define HWIO_BIMC_M_DSP_MPORT_CLOCK_CTRL_RMSK                                                   0x3
#define HWIO_BIMC_M_DSP_MPORT_CLOCK_CTRL_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CLOCK_CTRL_ADDR, HWIO_BIMC_M_DSP_MPORT_CLOCK_CTRL_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_CLOCK_CTRL_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CLOCK_CTRL_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_CLOCK_CTRL_OUT(v)      \
        out_dword(HWIO_BIMC_M_DSP_MPORT_CLOCK_CTRL_ADDR,v)
#define HWIO_BIMC_M_DSP_MPORT_CLOCK_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_DSP_MPORT_CLOCK_CTRL_ADDR,m,v,HWIO_BIMC_M_DSP_MPORT_CLOCK_CTRL_IN)
#define HWIO_BIMC_M_DSP_MPORT_CLOCK_CTRL_MASTER_CLOCK_GATING_EN_BMSK                            0x2
#define HWIO_BIMC_M_DSP_MPORT_CLOCK_CTRL_MASTER_CLOCK_GATING_EN_SHFT                            0x1
#define HWIO_BIMC_M_DSP_MPORT_CLOCK_CTRL_CORE_CLOCK_GATING_EN_BMSK                              0x1
#define HWIO_BIMC_M_DSP_MPORT_CLOCK_CTRL_CORE_CLOCK_GATING_EN_SHFT                              0x0

#define HWIO_BIMC_M_DSP_MPORT_CDC_CTRL_ADDR                                              (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000208)
#define HWIO_BIMC_M_DSP_MPORT_CDC_CTRL_RMSK                                                     0x3
#define HWIO_BIMC_M_DSP_MPORT_CDC_CTRL_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CDC_CTRL_ADDR, HWIO_BIMC_M_DSP_MPORT_CDC_CTRL_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_CDC_CTRL_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_CDC_CTRL_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_CDC_CTRL_OUT(v)      \
        out_dword(HWIO_BIMC_M_DSP_MPORT_CDC_CTRL_ADDR,v)
#define HWIO_BIMC_M_DSP_MPORT_CDC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_DSP_MPORT_CDC_CTRL_ADDR,m,v,HWIO_BIMC_M_DSP_MPORT_CDC_CTRL_IN)
#define HWIO_BIMC_M_DSP_MPORT_CDC_CTRL_MASTER_CLOCK_DEMETA_SEL_BMSK                             0x2
#define HWIO_BIMC_M_DSP_MPORT_CDC_CTRL_MASTER_CLOCK_DEMETA_SEL_SHFT                             0x1
#define HWIO_BIMC_M_DSP_MPORT_CDC_CTRL_CORE_CLOCK_DEMETA_SEL_BMSK                               0x1
#define HWIO_BIMC_M_DSP_MPORT_CDC_CTRL_CORE_CLOCK_DEMETA_SEL_SHFT                               0x0

#define HWIO_BIMC_M_DSP_MPORT_MODE_ADDR                                                  (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000210)
#define HWIO_BIMC_M_DSP_MPORT_MODE_RMSK                                                  0xf00fff17
#define HWIO_BIMC_M_DSP_MPORT_MODE_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_MODE_ADDR, HWIO_BIMC_M_DSP_MPORT_MODE_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_MODE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_MODE_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_MODE_OUT(v)      \
        out_dword(HWIO_BIMC_M_DSP_MPORT_MODE_ADDR,v)
#define HWIO_BIMC_M_DSP_MPORT_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_DSP_MPORT_MODE_ADDR,m,v,HWIO_BIMC_M_DSP_MPORT_MODE_IN)
#define HWIO_BIMC_M_DSP_MPORT_MODE_WR_GATHER_BEATS_BMSK                                  0xf0000000
#define HWIO_BIMC_M_DSP_MPORT_MODE_WR_GATHER_BEATS_SHFT                                        0x1c
#define HWIO_BIMC_M_DSP_MPORT_MODE_RD_GATHER_BEATS_BMSK                                     0xf0000
#define HWIO_BIMC_M_DSP_MPORT_MODE_RD_GATHER_BEATS_SHFT                                        0x10
#define HWIO_BIMC_M_DSP_MPORT_MODE_RCSSH_BMSK                                                0x8000
#define HWIO_BIMC_M_DSP_MPORT_MODE_RCSSH_SHFT                                                   0xf
#define HWIO_BIMC_M_DSP_MPORT_MODE_RCOSH_BMSK                                                0x4000
#define HWIO_BIMC_M_DSP_MPORT_MODE_RCOSH_SHFT                                                   0xe
#define HWIO_BIMC_M_DSP_MPORT_MODE_RCISH_BMSK                                                0x2000
#define HWIO_BIMC_M_DSP_MPORT_MODE_RCISH_SHFT                                                   0xd
#define HWIO_BIMC_M_DSP_MPORT_MODE_RCNSH_BMSK                                                0x1000
#define HWIO_BIMC_M_DSP_MPORT_MODE_RCNSH_SHFT                                                   0xc
#define HWIO_BIMC_M_DSP_MPORT_MODE_RNCSSH_BMSK                                                0x800
#define HWIO_BIMC_M_DSP_MPORT_MODE_RNCSSH_SHFT                                                  0xb
#define HWIO_BIMC_M_DSP_MPORT_MODE_RNCOSH_BMSK                                                0x400
#define HWIO_BIMC_M_DSP_MPORT_MODE_RNCOSH_SHFT                                                  0xa
#define HWIO_BIMC_M_DSP_MPORT_MODE_RNCISH_BMSK                                                0x200
#define HWIO_BIMC_M_DSP_MPORT_MODE_RNCISH_SHFT                                                  0x9
#define HWIO_BIMC_M_DSP_MPORT_MODE_RNCNSH_BMSK                                                0x100
#define HWIO_BIMC_M_DSP_MPORT_MODE_RNCNSH_SHFT                                                  0x8
#define HWIO_BIMC_M_DSP_MPORT_MODE_NARROW_WRITES_BMSK                                          0x10
#define HWIO_BIMC_M_DSP_MPORT_MODE_NARROW_WRITES_SHFT                                           0x4
#define HWIO_BIMC_M_DSP_MPORT_MODE_FIODV_BMSK                                                   0x4
#define HWIO_BIMC_M_DSP_MPORT_MODE_FIODV_SHFT                                                   0x2
#define HWIO_BIMC_M_DSP_MPORT_MODE_FIOSO_BMSK                                                   0x2
#define HWIO_BIMC_M_DSP_MPORT_MODE_FIOSO_SHFT                                                   0x1
#define HWIO_BIMC_M_DSP_MPORT_MODE_ORDERING_MODEL_BMSK                                          0x1
#define HWIO_BIMC_M_DSP_MPORT_MODE_ORDERING_MODEL_SHFT                                          0x0

#define HWIO_BIMC_M_DSP_MPORT_WR_WAY_CROSSING_ADDR                                       (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000220)
#define HWIO_BIMC_M_DSP_MPORT_WR_WAY_CROSSING_RMSK                                              0xf
#define HWIO_BIMC_M_DSP_MPORT_WR_WAY_CROSSING_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_WR_WAY_CROSSING_ADDR, HWIO_BIMC_M_DSP_MPORT_WR_WAY_CROSSING_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_WR_WAY_CROSSING_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_WR_WAY_CROSSING_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_WR_WAY_CROSSING_OUT(v)      \
        out_dword(HWIO_BIMC_M_DSP_MPORT_WR_WAY_CROSSING_ADDR,v)
#define HWIO_BIMC_M_DSP_MPORT_WR_WAY_CROSSING_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_DSP_MPORT_WR_WAY_CROSSING_ADDR,m,v,HWIO_BIMC_M_DSP_MPORT_WR_WAY_CROSSING_IN)
#define HWIO_BIMC_M_DSP_MPORT_WR_WAY_CROSSING_REG_BMSK                                          0xf
#define HWIO_BIMC_M_DSP_MPORT_WR_WAY_CROSSING_REG_SHFT                                          0x0

#define HWIO_BIMC_M_DSP_MPORT_PRIORITYLVL_OVERRIDE_ADDR                                  (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000230)
#define HWIO_BIMC_M_DSP_MPORT_PRIORITYLVL_OVERRIDE_RMSK                                       0x301
#define HWIO_BIMC_M_DSP_MPORT_PRIORITYLVL_OVERRIDE_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_PRIORITYLVL_OVERRIDE_ADDR, HWIO_BIMC_M_DSP_MPORT_PRIORITYLVL_OVERRIDE_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_PRIORITYLVL_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_PRIORITYLVL_OVERRIDE_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_PRIORITYLVL_OVERRIDE_OUT(v)      \
        out_dword(HWIO_BIMC_M_DSP_MPORT_PRIORITYLVL_OVERRIDE_ADDR,v)
#define HWIO_BIMC_M_DSP_MPORT_PRIORITYLVL_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_DSP_MPORT_PRIORITYLVL_OVERRIDE_ADDR,m,v,HWIO_BIMC_M_DSP_MPORT_PRIORITYLVL_OVERRIDE_IN)
#define HWIO_BIMC_M_DSP_MPORT_PRIORITYLVL_OVERRIDE_PRIORITYLVL_BMSK                           0x300
#define HWIO_BIMC_M_DSP_MPORT_PRIORITYLVL_OVERRIDE_PRIORITYLVL_SHFT                             0x8
#define HWIO_BIMC_M_DSP_MPORT_PRIORITYLVL_OVERRIDE_OVERRIDE_PRIORITYLVL_BMSK                    0x1
#define HWIO_BIMC_M_DSP_MPORT_PRIORITYLVL_OVERRIDE_OVERRIDE_PRIORITYLVL_SHFT                    0x0

#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_ADDR                                 (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000240)
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_RMSK                                  0x3071f7f
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_ADDR, HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_OUT(v)      \
        out_dword(HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_ADDR,v)
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_ADDR,m,v,HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_IN)
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_AREQPRIORITY_BMSK                     0x3000000
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_AREQPRIORITY_SHFT                          0x18
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_AMEMTYPE_BMSK                           0x70000
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_AMEMTYPE_SHFT                              0x10
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_ATRANSIENT_BMSK                          0x1000
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_ATRANSIENT_SHFT                             0xc
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_ASHARED_BMSK                              0x800
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_ASHARED_SHFT                                0xb
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_AREDIRECT_BMSK                            0x400
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_AREDIRECT_SHFT                              0xa
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_AOOO_BMSK                                 0x200
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_AOOO_SHFT                                   0x9
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_AINNERSHARED_BMSK                         0x100
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_AINNERSHARED_SHFT                           0x8
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_BMSK                 0x40
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_SHFT                  0x6
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_BMSK                     0x20
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_SHFT                      0x5
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_BMSK                   0x10
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_SHFT                    0x4
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_ASHARED_BMSK                       0x8
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_ASHARED_SHFT                       0x3
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_BMSK                     0x4
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_SHFT                     0x2
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AOOO_BMSK                          0x2
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AOOO_SHFT                          0x1
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_BMSK                  0x1
#define HWIO_BIMC_M_DSP_MPORT_READ_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_SHFT                  0x0

#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_ADDR                                (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000250)
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_RMSK                                 0x3071f7f
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_ADDR, HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_OUT(v)      \
        out_dword(HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_ADDR,v)
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_ADDR,m,v,HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_IN)
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_AREQPRIORITY_BMSK                    0x3000000
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_AREQPRIORITY_SHFT                         0x18
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_AMEMTYPE_BMSK                          0x70000
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_AMEMTYPE_SHFT                             0x10
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_ATRANSIENT_BMSK                         0x1000
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_ATRANSIENT_SHFT                            0xc
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_ASHARED_BMSK                             0x800
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_ASHARED_SHFT                               0xb
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_AREDIRECT_BMSK                           0x400
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_AREDIRECT_SHFT                             0xa
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_AOOO_BMSK                                0x200
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_AOOO_SHFT                                  0x9
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_AINNERSHARED_BMSK                        0x100
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_AINNERSHARED_SHFT                          0x8
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_BMSK                0x40
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_SHFT                 0x6
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_BMSK                    0x20
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_SHFT                     0x5
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_BMSK                  0x10
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_SHFT                   0x4
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_ASHARED_BMSK                      0x8
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_ASHARED_SHFT                      0x3
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_BMSK                    0x4
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_SHFT                    0x2
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AOOO_BMSK                         0x2
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AOOO_SHFT                         0x1
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_BMSK                 0x1
#define HWIO_BIMC_M_DSP_MPORT_WRITE_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_SHFT                 0x0

#define HWIO_BIMC_M_DSP_MPORT_BKE_ENABLE_ADDR                                            (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000300)
#define HWIO_BIMC_M_DSP_MPORT_BKE_ENABLE_RMSK                                                   0x1
#define HWIO_BIMC_M_DSP_MPORT_BKE_ENABLE_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_ENABLE_ADDR, HWIO_BIMC_M_DSP_MPORT_BKE_ENABLE_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_BKE_ENABLE_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_ENABLE_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_BKE_ENABLE_OUT(v)      \
        out_dword(HWIO_BIMC_M_DSP_MPORT_BKE_ENABLE_ADDR,v)
#define HWIO_BIMC_M_DSP_MPORT_BKE_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_DSP_MPORT_BKE_ENABLE_ADDR,m,v,HWIO_BIMC_M_DSP_MPORT_BKE_ENABLE_IN)
#define HWIO_BIMC_M_DSP_MPORT_BKE_ENABLE_ENABLE_BMSK                                            0x1
#define HWIO_BIMC_M_DSP_MPORT_BKE_ENABLE_ENABLE_SHFT                                            0x0

#define HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_PERIOD_ADDR                                      (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000304)
#define HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_PERIOD_RMSK                                           0x3ff
#define HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_PERIOD_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_PERIOD_ADDR, HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_PERIOD_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_PERIOD_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_PERIOD_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_PERIOD_OUT(v)      \
        out_dword(HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_PERIOD_ADDR,v)
#define HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_PERIOD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_PERIOD_ADDR,m,v,HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_PERIOD_IN)
#define HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_PERIOD_GRANT_PERIOD_BMSK                              0x3ff
#define HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_PERIOD_GRANT_PERIOD_SHFT                                0x0

#define HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_COUNT_ADDR                                       (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000308)
#define HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_COUNT_RMSK                                           0xffff
#define HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_COUNT_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_COUNT_ADDR, HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_COUNT_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_COUNT_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_COUNT_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_COUNT_OUT(v)      \
        out_dword(HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_COUNT_ADDR,v)
#define HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_COUNT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_COUNT_ADDR,m,v,HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_COUNT_IN)
#define HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_COUNT_GRANT_COUNT_BMSK                               0xffff
#define HWIO_BIMC_M_DSP_MPORT_BKE_GRANT_COUNT_GRANT_COUNT_SHFT                                  0x0

#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_HIGH_ADDR                                    (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000320)
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_HIGH_RMSK                                        0xffff
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_HIGH_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_HIGH_ADDR, HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_HIGH_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_HIGH_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_HIGH_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_HIGH_OUT(v)      \
        out_dword(HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_HIGH_ADDR,v)
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_HIGH_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_HIGH_ADDR,m,v,HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_HIGH_IN)
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_HIGH_THRESHOLD_BMSK                              0xffff
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_HIGH_THRESHOLD_SHFT                                 0x0

#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_MEDIUM_ADDR                                  (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000324)
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_MEDIUM_RMSK                                      0xffff
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_MEDIUM_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_MEDIUM_ADDR, HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_MEDIUM_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_MEDIUM_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_MEDIUM_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_MEDIUM_OUT(v)      \
        out_dword(HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_MEDIUM_ADDR,v)
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_MEDIUM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_MEDIUM_ADDR,m,v,HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_MEDIUM_IN)
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_MEDIUM_THRESHOLD_BMSK                            0xffff
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_MEDIUM_THRESHOLD_SHFT                               0x0

#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_LOW_ADDR                                     (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000328)
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_LOW_RMSK                                         0xffff
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_LOW_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_LOW_ADDR, HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_LOW_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_LOW_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_LOW_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_LOW_OUT(v)      \
        out_dword(HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_LOW_ADDR,v)
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_LOW_ADDR,m,v,HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_LOW_IN)
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_LOW_THRESHOLD_BMSK                               0xffff
#define HWIO_BIMC_M_DSP_MPORT_BKE_THRESHOLD_LOW_THRESHOLD_SHFT                                  0x0

#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_0_ADDR                                          (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000340)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_0_RMSK                                          0x80000303
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_0_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_0_ADDR, HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_0_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_0_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_0_OUT(v)      \
        out_dword(HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_0_ADDR,v)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_0_ADDR,m,v,HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_0_IN)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_0_LIMIT_COMMANDS_BMSK                           0x80000000
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_0_LIMIT_COMMANDS_SHFT                                 0x1f
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_0_AREQPRIORITY_BMSK                                  0x300
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_0_AREQPRIORITY_SHFT                                    0x8
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_0_PRIORITYLVL_BMSK                                     0x3
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_0_PRIORITYLVL_SHFT                                     0x0

#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_1_ADDR                                          (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000344)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_1_RMSK                                          0x80000303
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_1_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_1_ADDR, HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_1_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_1_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_1_OUT(v)      \
        out_dword(HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_1_ADDR,v)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_1_ADDR,m,v,HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_1_IN)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_1_LIMIT_COMMANDS_BMSK                           0x80000000
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_1_LIMIT_COMMANDS_SHFT                                 0x1f
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_1_AREQPRIORITY_BMSK                                  0x300
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_1_AREQPRIORITY_SHFT                                    0x8
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_1_PRIORITYLVL_BMSK                                     0x3
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_1_PRIORITYLVL_SHFT                                     0x0

#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_2_ADDR                                          (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000348)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_2_RMSK                                          0x80000303
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_2_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_2_ADDR, HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_2_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_2_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_2_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_2_OUT(v)      \
        out_dword(HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_2_ADDR,v)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_2_ADDR,m,v,HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_2_IN)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_2_LIMIT_COMMANDS_BMSK                           0x80000000
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_2_LIMIT_COMMANDS_SHFT                                 0x1f
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_2_AREQPRIORITY_BMSK                                  0x300
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_2_AREQPRIORITY_SHFT                                    0x8
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_2_PRIORITYLVL_BMSK                                     0x3
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_2_PRIORITYLVL_SHFT                                     0x0

#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_3_ADDR                                          (BIMC_M_DSP_MPORT_REG_BASE      + 0x0000034c)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_3_RMSK                                               0x303
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_3_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_3_ADDR, HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_3_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_3_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_3_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_3_OUT(v)      \
        out_dword(HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_3_ADDR,v)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_3_ADDR,m,v,HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_3_IN)
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_3_AREQPRIORITY_BMSK                                  0x300
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_3_AREQPRIORITY_SHFT                                    0x8
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_3_PRIORITYLVL_BMSK                                     0x3
#define HWIO_BIMC_M_DSP_MPORT_BKE_HEALTH_3_PRIORITYLVL_SHFT                                     0x0

#define HWIO_BIMC_M_DSP_MPORT_STATUS_2A_ADDR                                             (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000420)
#define HWIO_BIMC_M_DSP_MPORT_STATUS_2A_RMSK                                               0xff00ff
#define HWIO_BIMC_M_DSP_MPORT_STATUS_2A_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_STATUS_2A_ADDR, HWIO_BIMC_M_DSP_MPORT_STATUS_2A_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_STATUS_2A_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_STATUS_2A_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_STATUS_2A_WR_PENDING_PRE_BUF_BMSK                            0xff0000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_2A_WR_PENDING_PRE_BUF_SHFT                                0x10
#define HWIO_BIMC_M_DSP_MPORT_STATUS_2A_RD_PENDING_PRE_BUF_BMSK                                0xff
#define HWIO_BIMC_M_DSP_MPORT_STATUS_2A_RD_PENDING_PRE_BUF_SHFT                                 0x0

#define HWIO_BIMC_M_DSP_MPORT_STATUS_2B_ADDR                                             (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000424)
#define HWIO_BIMC_M_DSP_MPORT_STATUS_2B_RMSK                                               0xff00ff
#define HWIO_BIMC_M_DSP_MPORT_STATUS_2B_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_STATUS_2B_ADDR, HWIO_BIMC_M_DSP_MPORT_STATUS_2B_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_STATUS_2B_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_STATUS_2B_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_STATUS_2B_WR_PENDING_POST_BUF_BMSK                           0xff0000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_2B_WR_PENDING_POST_BUF_SHFT                               0x10
#define HWIO_BIMC_M_DSP_MPORT_STATUS_2B_RD_PENDING_POST_BUF_BMSK                               0xff
#define HWIO_BIMC_M_DSP_MPORT_STATUS_2B_RD_PENDING_POST_BUF_SHFT                                0x0

#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_ADDR                                             (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000430)
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_RMSK                                             0xff333333
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_STATUS_3A_ADDR, HWIO_BIMC_M_DSP_MPORT_STATUS_3A_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_STATUS_3A_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_RCB_DATA_RD_FULL_BMSK                            0x80000000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_RCB_DATA_RD_FULL_SHFT                                  0x1f
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_RCB_DATA_RD_EMPTY_BMSK                           0x40000000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_RCB_DATA_RD_EMPTY_SHFT                                 0x1e
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_RCB_CTRL_RD_FULL_BMSK                            0x20000000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_RCB_CTRL_RD_FULL_SHFT                                  0x1d
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_RCB_CTRL_RD_EMPTY_BMSK                           0x10000000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_RCB_CTRL_RD_EMPTY_SHFT                                 0x1c
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_RCB_DATA_WR_FULL_BMSK                             0x8000000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_RCB_DATA_WR_FULL_SHFT                                  0x1b
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_RCB_DATA_WR_EMPTY_BMSK                            0x4000000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_RCB_DATA_WR_EMPTY_SHFT                                 0x1a
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_RCB_CTRL_WR_FULL_BMSK                             0x2000000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_RCB_CTRL_WR_FULL_SHFT                                  0x19
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_RCB_CTRL_WR_EMPTY_BMSK                            0x1000000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_RCB_CTRL_WR_EMPTY_SHFT                                 0x18
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_BCB_RD_FULL_BMSK                                   0x200000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_BCB_RD_FULL_SHFT                                       0x15
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_BCB_RD_EMPTY_BMSK                                  0x100000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_BCB_RD_EMPTY_SHFT                                      0x14
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_BCB_WR_FULL_BMSK                                    0x20000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_BCB_WR_FULL_SHFT                                       0x11
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_BCB_WR_EMPTY_BMSK                                   0x10000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_BCB_WR_EMPTY_SHFT                                      0x10
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_WCB_RD_FULL_BMSK                                     0x2000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_WCB_RD_FULL_SHFT                                        0xd
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_WCB_RD_EMPTY_BMSK                                    0x1000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_WCB_RD_EMPTY_SHFT                                       0xc
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_WCB_WR_FULL_BMSK                                      0x200
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_WCB_WR_FULL_SHFT                                        0x9
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_WCB_WR_EMPTY_BMSK                                     0x100
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_WCB_WR_EMPTY_SHFT                                       0x8
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_ACB_RD_FULL_BMSK                                       0x20
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_ACB_RD_FULL_SHFT                                        0x5
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_ACB_RD_EMPTY_BMSK                                      0x10
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_ACB_RD_EMPTY_SHFT                                       0x4
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_ACB_WR_FULL_BMSK                                        0x2
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_ACB_WR_FULL_SHFT                                        0x1
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_ACB_WR_EMPTY_BMSK                                       0x1
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3A_ACB_WR_EMPTY_SHFT                                       0x0

#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_ADDR                                             (BIMC_M_DSP_MPORT_REG_BASE      + 0x00000434)
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_RMSK                                               0x333333
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_IN          \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_STATUS_3B_ADDR, HWIO_BIMC_M_DSP_MPORT_STATUS_3B_RMSK)
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_INM(m)      \
        in_dword_masked(HWIO_BIMC_M_DSP_MPORT_STATUS_3B_ADDR, m)
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_RGB_RD_FULL_BMSK                                   0x200000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_RGB_RD_FULL_SHFT                                       0x15
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_RGB_RD_EMPTY_BMSK                                  0x100000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_RGB_RD_EMPTY_SHFT                                      0x14
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_RGB_WR_FULL_BMSK                                    0x20000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_RGB_WR_FULL_SHFT                                       0x11
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_RGB_WR_EMPTY_BMSK                                   0x10000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_RGB_WR_EMPTY_SHFT                                      0x10
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_W2AB_RD_FULL_BMSK                                    0x2000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_W2AB_RD_FULL_SHFT                                       0xd
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_W2AB_RD_EMPTY_BMSK                                   0x1000
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_W2AB_RD_EMPTY_SHFT                                      0xc
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_W2AB_WR_FULL_BMSK                                     0x200
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_W2AB_WR_FULL_SHFT                                       0x9
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_W2AB_WR_EMPTY_BMSK                                    0x100
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_W2AB_WR_EMPTY_SHFT                                      0x8
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_A2WB_RD_FULL_BMSK                                      0x20
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_A2WB_RD_FULL_SHFT                                       0x5
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_A2WB_RD_EMPTY_BMSK                                     0x10
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_A2WB_RD_EMPTY_SHFT                                      0x4
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_A2WB_WR_FULL_BMSK                                       0x2
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_A2WB_WR_FULL_SHFT                                       0x1
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_A2WB_WR_EMPTY_BMSK                                      0x1
#define HWIO_BIMC_M_DSP_MPORT_STATUS_3B_A2WB_WR_EMPTY_SHFT                                      0x0


#endif /* __TEMP_BIMC_M_HWIO_H__ */
