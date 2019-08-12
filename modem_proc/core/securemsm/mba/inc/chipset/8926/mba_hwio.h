#ifndef __MBA_HWIO_H__
#define __MBA_HWIO_H__
/*
===========================================================================
*/
/**
  @file mba_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    MSS_RMB
    DEHR_BIMC
    DEHR_VMIDMT
    MSS_PERPH
    SECURITY_CONTROL_CORE
    BIMC_S_DDR0_SCMO
    TLMM_CSR
    PERIPH_SS_QPIC_QPIC_VMIDMT

  'Include' filters applied: <none>
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/mba/inc/chipset/8926/mba_hwio.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

#include "msmhwiobase.h"

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
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_ERR_ADDR_BMSK                                0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_ERR_ADDR_SHFT                                       0x0

#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ADDR                                    (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000128)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_RMSK                                    0xff1fffff
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ADDR, HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ERR_ATID_BMSK                           0xff000000
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ERR_ATID_SHFT                                 0x18
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ERR_AVMID_BMSK                            0x1f0000
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ERR_AVMID_SHFT                                0x10
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

#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_ADDR                                        (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000200)
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_RMSK                                        0xffff1111
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_ADDR, HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_IN)
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_RFU_31_17_BMSK                              0xfffe0000
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_RFU_31_17_SHFT                                    0x11
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
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_RMSK                                      0xff
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_ADDR, HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_IN)
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
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_RFU_15_8_BMSK                                0xff00
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_RFU_15_8_SHFT                                   0x8
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_ADDR_BASE_BMSK                                 0xfc
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
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_RFU_15_8_BMSK                                0xff00
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_RFU_15_8_SHFT                                   0x8
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_ADDR_MASK_BMSK                                 0xfc
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
#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_RCH_PORTS_BMSK                              0xffffffff
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
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RFU_31_30_BMSK                            0xc0000000
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RFU_31_30_SHFT                                  0x1e
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH1_FIFO_BKPR_HI_TH_BMSK                 0x3f000000
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH1_FIFO_BKPR_HI_TH_SHFT                       0x18
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RFU_23_22_BMSK                              0xc00000
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RFU_23_22_SHFT                                  0x16
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH1_FIFO_BKPR_LO_TH_BMSK                   0x3f0000
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH1_FIFO_BKPR_LO_TH_SHFT                       0x10
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RFU_15_14_BMSK                                0xc000
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RFU_15_14_SHFT                                   0xe
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH0_FIFO_BKPR_HI_TH_BMSK                     0x3f00
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH0_FIFO_BKPR_HI_TH_SHFT                        0x8
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RFU_7_6_BMSK                                    0xc0
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RFU_7_6_SHFT                                     0x6
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH0_FIFO_BKPR_LO_TH_BMSK                       0x3f
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
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RFU_31_29_BMSK                               0xe0000000
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RFU_31_29_SHFT                                     0x1d
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
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_19_17_BMSK                               0xe0000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_19_17_SHFT                                  0x11
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_MASK_CMDOUT_PRI_BMSK                         0x10000
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
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_RMSK                                            0xff
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_ADDR, HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_IN)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_RFU_7_5_BMSK                                    0xe0
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_RFU_7_5_SHFT                                     0x5
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_FLUSH_CMD_TIMEOUT_BMSK                          0x1f
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_FLUSH_CMD_TIMEOUT_SHFT                           0x0

/*----------------------------------------------------------------------------
 * MODULE: DEHR_BIMC
 *--------------------------------------------------------------------------*/
#ifndef DEHR_BIMC_HM_BASE
#define DEHR_BIMC_HM_BASE DEHR_BIMC_WRAPPER_BASE
#endif

#define DEHR_BIMC_REG_BASE                                               (DEHR_BIMC_HM_BASE      + 0x00002000)

#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_ADDR                              (DEHR_BIMC_REG_BASE      + 0x00000000)
#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_RMSK                                0xffffff
#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_IN          \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_HW_VERSION_ADDR, HWIO_DEHR_BIMC_DEHR_HW_VERSION_RMSK)
#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_HW_VERSION_ADDR, m)
#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_MAJOR_BMSK                          0xff0000
#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_MAJOR_SHFT                              0x10
#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_MINOR_BMSK                            0xff00
#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_MINOR_SHFT                               0x8
#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_STEP_BMSK                               0xff
#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_STEP_SHFT                                0x0

#define HWIO_DEHR_BIMC_DEHR_HW_INFO_ADDR                                 (DEHR_BIMC_REG_BASE      + 0x00000004)
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_RMSK                                 0xffffffff
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_IN          \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_HW_INFO_ADDR, HWIO_DEHR_BIMC_DEHR_HW_INFO_RMSK)
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_INM(m)      \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_HW_INFO_ADDR, m)
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_MAJOR_BMSK                           0xff000000
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_MAJOR_SHFT                                 0x18
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_BRANCH_BMSK                            0xff0000
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_BRANCH_SHFT                                0x10
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_MINOR_BMSK                               0xff00
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_MINOR_SHFT                                  0x8
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_ECO_BMSK                                   0xff
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_ECO_SHFT                                    0x0

#define HWIO_DEHR_BIMC_DEHR_CGC_CFG_ADDR                                 (DEHR_BIMC_REG_BASE      + 0x00000010)
#define HWIO_DEHR_BIMC_DEHR_CGC_CFG_RMSK                                        0x1
#define HWIO_DEHR_BIMC_DEHR_CGC_CFG_IN          \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_CGC_CFG_ADDR, HWIO_DEHR_BIMC_DEHR_CGC_CFG_RMSK)
#define HWIO_DEHR_BIMC_DEHR_CGC_CFG_INM(m)      \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_CGC_CFG_ADDR, m)
#define HWIO_DEHR_BIMC_DEHR_CGC_CFG_OUT(v)      \
        out_dword(HWIO_DEHR_BIMC_DEHR_CGC_CFG_ADDR,v)
#define HWIO_DEHR_BIMC_DEHR_CGC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_BIMC_DEHR_CGC_CFG_ADDR,m,v,HWIO_DEHR_BIMC_DEHR_CGC_CFG_IN)
#define HWIO_DEHR_BIMC_DEHR_CGC_CFG_SW_CGC_DISABLE_BMSK                         0x1
#define HWIO_DEHR_BIMC_DEHR_CGC_CFG_SW_CGC_DISABLE_SHFT                         0x0

#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_ADDR                                 (DEHR_BIMC_REG_BASE      + 0x00000100)
#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_RMSK                                       0x11
#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_IN          \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_SEC_CFG_ADDR, HWIO_DEHR_BIMC_DEHR_SEC_CFG_RMSK)
#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_INM(m)      \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_SEC_CFG_ADDR, m)
#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_OUT(v)      \
        out_dword(HWIO_DEHR_BIMC_DEHR_SEC_CFG_ADDR,v)
#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_BIMC_DEHR_SEC_CFG_ADDR,m,v,HWIO_DEHR_BIMC_DEHR_SEC_CFG_IN)
#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_DEHR_ENA_BMSK                              0x10
#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_DEHR_ENA_SHFT                               0x4
#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_DEHR_LOCK_BMSK                              0x1
#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_DEHR_LOCK_SHFT                              0x0

#define HWIO_DEHR_BIMC_DEHR_RAM_CFG_ADDR                                 (DEHR_BIMC_REG_BASE      + 0x00000140)
#define HWIO_DEHR_BIMC_DEHR_RAM_CFG_RMSK                                     0xffff
#define HWIO_DEHR_BIMC_DEHR_RAM_CFG_IN          \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_RAM_CFG_ADDR, HWIO_DEHR_BIMC_DEHR_RAM_CFG_RMSK)
#define HWIO_DEHR_BIMC_DEHR_RAM_CFG_INM(m)      \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_RAM_CFG_ADDR, m)
#define HWIO_DEHR_BIMC_DEHR_RAM_CFG_OUT(v)      \
        out_dword(HWIO_DEHR_BIMC_DEHR_RAM_CFG_ADDR,v)
#define HWIO_DEHR_BIMC_DEHR_RAM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_BIMC_DEHR_RAM_CFG_ADDR,m,v,HWIO_DEHR_BIMC_DEHR_RAM_CFG_IN)
#define HWIO_DEHR_BIMC_DEHR_RAM_CFG_CFG_BASE_ADDR_BMSK                       0xffff
#define HWIO_DEHR_BIMC_DEHR_RAM_CFG_CFG_BASE_ADDR_SHFT                          0x0

#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_ADDR                              (DEHR_BIMC_REG_BASE      + 0x00000200)
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_RMSK                                0x110011
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_IN          \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_DMA_STATUS_ADDR, HWIO_DEHR_BIMC_DEHR_DMA_STATUS_RMSK)
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_INM(m)      \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_DMA_STATUS_ADDR, m)
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_OUT(v)      \
        out_dword(HWIO_DEHR_BIMC_DEHR_DMA_STATUS_ADDR,v)
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_BIMC_DEHR_DMA_STATUS_ADDR,m,v,HWIO_DEHR_BIMC_DEHR_DMA_STATUS_IN)
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_RESTORE_IN_PROGRESS_BMSK            0x100000
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_RESTORE_IN_PROGRESS_SHFT                0x14
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_RESTORE_DONE_BMSK                    0x10000
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_RESTORE_DONE_SHFT                       0x10
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_SAVE_IN_PROGRESS_BMSK                   0x10
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_SAVE_IN_PROGRESS_SHFT                    0x4
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_SAVE_DONE_BMSK                           0x1
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_SAVE_DONE_SHFT                           0x0

#define HWIO_DEHR_BIMC_DEHR_DBG_CFG_ADDR                                 (DEHR_BIMC_REG_BASE      + 0x00000400)
#define HWIO_DEHR_BIMC_DEHR_DBG_CFG_RMSK                                        0x1
#define HWIO_DEHR_BIMC_DEHR_DBG_CFG_IN          \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_DBG_CFG_ADDR, HWIO_DEHR_BIMC_DEHR_DBG_CFG_RMSK)
#define HWIO_DEHR_BIMC_DEHR_DBG_CFG_INM(m)      \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_DBG_CFG_ADDR, m)
#define HWIO_DEHR_BIMC_DEHR_DBG_CFG_OUT(v)      \
        out_dword(HWIO_DEHR_BIMC_DEHR_DBG_CFG_ADDR,v)
#define HWIO_DEHR_BIMC_DEHR_DBG_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_BIMC_DEHR_DBG_CFG_ADDR,m,v,HWIO_DEHR_BIMC_DEHR_DBG_CFG_IN)
#define HWIO_DEHR_BIMC_DEHR_DBG_CFG_DBG_ENA_BMSK                                0x1
#define HWIO_DEHR_BIMC_DEHR_DBG_CFG_DBG_ENA_SHFT                                0x0

#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_ADDR                              (DEHR_BIMC_REG_BASE      + 0x00000404)
#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_RMSK                                   0x101
#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_IN          \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_ADDR, HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_RMSK)
#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_INM(m)      \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_ADDR, m)
#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_OUT(v)      \
        out_dword(HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_ADDR,v)
#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_ADDR,m,v,HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_IN)
#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_SW_RESTORE_REQ_BMSK                    0x100
#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_SW_RESTORE_REQ_SHFT                      0x8
#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_SW_SAVE_REQ_BMSK                         0x1
#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_SW_SAVE_REQ_SHFT                         0x0

#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_ADDR                                (DEHR_BIMC_REG_BASE      + 0x00000408)
#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_RMSK                                0xffffffff
#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_IN          \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_DBG_ADDR_ADDR, HWIO_DEHR_BIMC_DEHR_DBG_ADDR_RMSK)
#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_INM(m)      \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_DBG_ADDR_ADDR, m)
#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_OUT(v)      \
        out_dword(HWIO_DEHR_BIMC_DEHR_DBG_ADDR_ADDR,v)
#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_BIMC_DEHR_DBG_ADDR_ADDR,m,v,HWIO_DEHR_BIMC_DEHR_DBG_ADDR_IN)
#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_LINK_END_ADDR_BMSK                  0xffff0000
#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_LINK_END_ADDR_SHFT                        0x10
#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_LINK_START_ADDR_BMSK                    0xffff
#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_LINK_START_ADDR_SHFT                       0x0

/*----------------------------------------------------------------------------
 * MODULE: DEHR_VMIDMT
 *--------------------------------------------------------------------------*/

#define DEHR_VMIDMT_REG_BASE                                      (DEHR_BIMC_HM_BASE      + 0x00001000)

#define HWIO_DEHR_VMIDMT_SCR0_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000000)
#define HWIO_DEHR_VMIDMT_SCR0_RMSK                                0x3fd707f5
#define HWIO_DEHR_VMIDMT_SCR0_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_SCR0_ADDR, HWIO_DEHR_VMIDMT_SCR0_RMSK)
#define HWIO_DEHR_VMIDMT_SCR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SCR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_SCR0_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_SCR0_ADDR,v)
#define HWIO_DEHR_VMIDMT_SCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_SCR0_ADDR,m,v,HWIO_DEHR_VMIDMT_SCR0_IN)
#define HWIO_DEHR_VMIDMT_SCR0_NSCFG_BMSK                          0x30000000
#define HWIO_DEHR_VMIDMT_SCR0_NSCFG_SHFT                                0x1c
#define HWIO_DEHR_VMIDMT_SCR0_WACFG_BMSK                           0xc000000
#define HWIO_DEHR_VMIDMT_SCR0_WACFG_SHFT                                0x1a
#define HWIO_DEHR_VMIDMT_SCR0_RACFG_BMSK                           0x3000000
#define HWIO_DEHR_VMIDMT_SCR0_RACFG_SHFT                                0x18
#define HWIO_DEHR_VMIDMT_SCR0_SHCFG_BMSK                            0xc00000
#define HWIO_DEHR_VMIDMT_SCR0_SHCFG_SHFT                                0x16
#define HWIO_DEHR_VMIDMT_SCR0_MTCFG_BMSK                            0x100000
#define HWIO_DEHR_VMIDMT_SCR0_MTCFG_SHFT                                0x14
#define HWIO_DEHR_VMIDMT_SCR0_MEMATTR_BMSK                           0x70000
#define HWIO_DEHR_VMIDMT_SCR0_MEMATTR_SHFT                              0x10
#define HWIO_DEHR_VMIDMT_SCR0_USFCFG_BMSK                              0x400
#define HWIO_DEHR_VMIDMT_SCR0_USFCFG_SHFT                                0xa
#define HWIO_DEHR_VMIDMT_SCR0_GSE_BMSK                                 0x200
#define HWIO_DEHR_VMIDMT_SCR0_GSE_SHFT                                   0x9
#define HWIO_DEHR_VMIDMT_SCR0_STALLD_BMSK                              0x100
#define HWIO_DEHR_VMIDMT_SCR0_STALLD_SHFT                                0x8
#define HWIO_DEHR_VMIDMT_SCR0_TRANSIENTCFG_BMSK                         0xc0
#define HWIO_DEHR_VMIDMT_SCR0_TRANSIENTCFG_SHFT                          0x6
#define HWIO_DEHR_VMIDMT_SCR0_GCFGFIE_BMSK                              0x20
#define HWIO_DEHR_VMIDMT_SCR0_GCFGFIE_SHFT                               0x5
#define HWIO_DEHR_VMIDMT_SCR0_GCFGERE_BMSK                              0x10
#define HWIO_DEHR_VMIDMT_SCR0_GCFGERE_SHFT                               0x4
#define HWIO_DEHR_VMIDMT_SCR0_GFIE_BMSK                                  0x4
#define HWIO_DEHR_VMIDMT_SCR0_GFIE_SHFT                                  0x2
#define HWIO_DEHR_VMIDMT_SCR0_CLIENTPD_BMSK                              0x1
#define HWIO_DEHR_VMIDMT_SCR0_CLIENTPD_SHFT                              0x0

#define HWIO_DEHR_VMIDMT_SCR1_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000004)
#define HWIO_DEHR_VMIDMT_SCR1_RMSK                                 0x1000100
#define HWIO_DEHR_VMIDMT_SCR1_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_SCR1_ADDR, HWIO_DEHR_VMIDMT_SCR1_RMSK)
#define HWIO_DEHR_VMIDMT_SCR1_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SCR1_ADDR, m)
#define HWIO_DEHR_VMIDMT_SCR1_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_SCR1_ADDR,v)
#define HWIO_DEHR_VMIDMT_SCR1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_SCR1_ADDR,m,v,HWIO_DEHR_VMIDMT_SCR1_IN)
#define HWIO_DEHR_VMIDMT_SCR1_GASRAE_BMSK                          0x1000000
#define HWIO_DEHR_VMIDMT_SCR1_GASRAE_SHFT                               0x18
#define HWIO_DEHR_VMIDMT_SCR1_NSNUMSMRGO_BMSK                          0x100
#define HWIO_DEHR_VMIDMT_SCR1_NSNUMSMRGO_SHFT                            0x8

#define HWIO_DEHR_VMIDMT_SCR2_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000008)
#define HWIO_DEHR_VMIDMT_SCR2_RMSK                                      0x1f
#define HWIO_DEHR_VMIDMT_SCR2_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_SCR2_ADDR, HWIO_DEHR_VMIDMT_SCR2_RMSK)
#define HWIO_DEHR_VMIDMT_SCR2_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SCR2_ADDR, m)
#define HWIO_DEHR_VMIDMT_SCR2_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_SCR2_ADDR,v)
#define HWIO_DEHR_VMIDMT_SCR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_SCR2_ADDR,m,v,HWIO_DEHR_VMIDMT_SCR2_IN)
#define HWIO_DEHR_VMIDMT_SCR2_BPVMID_BMSK                               0x1f
#define HWIO_DEHR_VMIDMT_SCR2_BPVMID_SHFT                                0x0

#define HWIO_DEHR_VMIDMT_SACR_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000010)
#define HWIO_DEHR_VMIDMT_SACR_RMSK                                0x70000013
#define HWIO_DEHR_VMIDMT_SACR_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_SACR_ADDR, HWIO_DEHR_VMIDMT_SACR_RMSK)
#define HWIO_DEHR_VMIDMT_SACR_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SACR_ADDR, m)
#define HWIO_DEHR_VMIDMT_SACR_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_SACR_ADDR,v)
#define HWIO_DEHR_VMIDMT_SACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_SACR_ADDR,m,v,HWIO_DEHR_VMIDMT_SACR_IN)
#define HWIO_DEHR_VMIDMT_SACR_BPRCNSH_BMSK                        0x40000000
#define HWIO_DEHR_VMIDMT_SACR_BPRCNSH_SHFT                              0x1e
#define HWIO_DEHR_VMIDMT_SACR_BPRCISH_BMSK                        0x20000000
#define HWIO_DEHR_VMIDMT_SACR_BPRCISH_SHFT                              0x1d
#define HWIO_DEHR_VMIDMT_SACR_BPRCOSH_BMSK                        0x10000000
#define HWIO_DEHR_VMIDMT_SACR_BPRCOSH_SHFT                              0x1c
#define HWIO_DEHR_VMIDMT_SACR_BPREQPRIORITYCFG_BMSK                     0x10
#define HWIO_DEHR_VMIDMT_SACR_BPREQPRIORITYCFG_SHFT                      0x4
#define HWIO_DEHR_VMIDMT_SACR_BPREQPRIORITY_BMSK                         0x3
#define HWIO_DEHR_VMIDMT_SACR_BPREQPRIORITY_SHFT                         0x0

#define HWIO_DEHR_VMIDMT_SIDR0_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000020)
#define HWIO_DEHR_VMIDMT_SIDR0_RMSK                               0x88001eff
#define HWIO_DEHR_VMIDMT_SIDR0_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_SIDR0_ADDR, HWIO_DEHR_VMIDMT_SIDR0_RMSK)
#define HWIO_DEHR_VMIDMT_SIDR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SIDR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_SIDR0_SES_BMSK                           0x80000000
#define HWIO_DEHR_VMIDMT_SIDR0_SES_SHFT                                 0x1f
#define HWIO_DEHR_VMIDMT_SIDR0_SMS_BMSK                            0x8000000
#define HWIO_DEHR_VMIDMT_SIDR0_SMS_SHFT                                 0x1b
#define HWIO_DEHR_VMIDMT_SIDR0_NUMSIDB_BMSK                           0x1e00
#define HWIO_DEHR_VMIDMT_SIDR0_NUMSIDB_SHFT                              0x9
#define HWIO_DEHR_VMIDMT_SIDR0_NUMSMRG_BMSK                             0xff
#define HWIO_DEHR_VMIDMT_SIDR0_NUMSMRG_SHFT                              0x0

#define HWIO_DEHR_VMIDMT_SIDR1_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000024)
#define HWIO_DEHR_VMIDMT_SIDR1_RMSK                                   0x9f00
#define HWIO_DEHR_VMIDMT_SIDR1_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_SIDR1_ADDR, HWIO_DEHR_VMIDMT_SIDR1_RMSK)
#define HWIO_DEHR_VMIDMT_SIDR1_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SIDR1_ADDR, m)
#define HWIO_DEHR_VMIDMT_SIDR1_SMCD_BMSK                              0x8000
#define HWIO_DEHR_VMIDMT_SIDR1_SMCD_SHFT                                 0xf
#define HWIO_DEHR_VMIDMT_SIDR1_SSDTP_BMSK                             0x1000
#define HWIO_DEHR_VMIDMT_SIDR1_SSDTP_SHFT                                0xc
#define HWIO_DEHR_VMIDMT_SIDR1_NUMSSDNDX_BMSK                          0xf00
#define HWIO_DEHR_VMIDMT_SIDR1_NUMSSDNDX_SHFT                            0x8

#define HWIO_DEHR_VMIDMT_SIDR2_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000028)
#define HWIO_DEHR_VMIDMT_SIDR2_RMSK                                     0xff
#define HWIO_DEHR_VMIDMT_SIDR2_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_SIDR2_ADDR, HWIO_DEHR_VMIDMT_SIDR2_RMSK)
#define HWIO_DEHR_VMIDMT_SIDR2_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SIDR2_ADDR, m)
#define HWIO_DEHR_VMIDMT_SIDR2_OAS_BMSK                                 0xf0
#define HWIO_DEHR_VMIDMT_SIDR2_OAS_SHFT                                  0x4
#define HWIO_DEHR_VMIDMT_SIDR2_IAS_BMSK                                  0xf
#define HWIO_DEHR_VMIDMT_SIDR2_IAS_SHFT                                  0x0

#define HWIO_DEHR_VMIDMT_SIDR4_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000030)
#define HWIO_DEHR_VMIDMT_SIDR4_RMSK                               0xffffffff
#define HWIO_DEHR_VMIDMT_SIDR4_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_SIDR4_ADDR, HWIO_DEHR_VMIDMT_SIDR4_RMSK)
#define HWIO_DEHR_VMIDMT_SIDR4_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SIDR4_ADDR, m)
#define HWIO_DEHR_VMIDMT_SIDR4_MAJOR_BMSK                         0xf0000000
#define HWIO_DEHR_VMIDMT_SIDR4_MAJOR_SHFT                               0x1c
#define HWIO_DEHR_VMIDMT_SIDR4_MINOR_BMSK                          0xfff0000
#define HWIO_DEHR_VMIDMT_SIDR4_MINOR_SHFT                               0x10
#define HWIO_DEHR_VMIDMT_SIDR4_STEP_BMSK                              0xffff
#define HWIO_DEHR_VMIDMT_SIDR4_STEP_SHFT                                 0x0

#define HWIO_DEHR_VMIDMT_SIDR5_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000034)
#define HWIO_DEHR_VMIDMT_SIDR5_RMSK                                 0xff03ff
#define HWIO_DEHR_VMIDMT_SIDR5_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_SIDR5_ADDR, HWIO_DEHR_VMIDMT_SIDR5_RMSK)
#define HWIO_DEHR_VMIDMT_SIDR5_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SIDR5_ADDR, m)
#define HWIO_DEHR_VMIDMT_SIDR5_NUMMSDRB_BMSK                        0xff0000
#define HWIO_DEHR_VMIDMT_SIDR5_NUMMSDRB_SHFT                            0x10
#define HWIO_DEHR_VMIDMT_SIDR5_MSAE_BMSK                               0x200
#define HWIO_DEHR_VMIDMT_SIDR5_MSAE_SHFT                                 0x9
#define HWIO_DEHR_VMIDMT_SIDR5_QRIBE_BMSK                              0x100
#define HWIO_DEHR_VMIDMT_SIDR5_QRIBE_SHFT                                0x8
#define HWIO_DEHR_VMIDMT_SIDR5_NVMID_BMSK                               0xff
#define HWIO_DEHR_VMIDMT_SIDR5_NVMID_SHFT                                0x0

#define HWIO_DEHR_VMIDMT_SIDR7_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x0000003c)
#define HWIO_DEHR_VMIDMT_SIDR7_RMSK                                     0xff
#define HWIO_DEHR_VMIDMT_SIDR7_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_SIDR7_ADDR, HWIO_DEHR_VMIDMT_SIDR7_RMSK)
#define HWIO_DEHR_VMIDMT_SIDR7_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SIDR7_ADDR, m)
#define HWIO_DEHR_VMIDMT_SIDR7_MAJOR_BMSK                               0xf0
#define HWIO_DEHR_VMIDMT_SIDR7_MAJOR_SHFT                                0x4
#define HWIO_DEHR_VMIDMT_SIDR7_MINOR_BMSK                                0xf
#define HWIO_DEHR_VMIDMT_SIDR7_MINOR_SHFT                                0x0

#define HWIO_DEHR_VMIDMT_SGFAR0_ADDR                              (DEHR_VMIDMT_REG_BASE      + 0x00000040)
#define HWIO_DEHR_VMIDMT_SGFAR0_RMSK                              0xffffffff
#define HWIO_DEHR_VMIDMT_SGFAR0_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_SGFAR0_ADDR, HWIO_DEHR_VMIDMT_SGFAR0_RMSK)
#define HWIO_DEHR_VMIDMT_SGFAR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SGFAR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_SGFAR0_SGFEA0_BMSK                       0xffffffff
#define HWIO_DEHR_VMIDMT_SGFAR0_SGFEA0_SHFT                              0x0

#define HWIO_DEHR_VMIDMT_SGFSR_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000048)
#define HWIO_DEHR_VMIDMT_SGFSR_RMSK                               0xc0000022
#define HWIO_DEHR_VMIDMT_SGFSR_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_SGFSR_ADDR, HWIO_DEHR_VMIDMT_SGFSR_RMSK)
#define HWIO_DEHR_VMIDMT_SGFSR_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SGFSR_ADDR, m)
#define HWIO_DEHR_VMIDMT_SGFSR_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_SGFSR_ADDR,v)
#define HWIO_DEHR_VMIDMT_SGFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_SGFSR_ADDR,m,v,HWIO_DEHR_VMIDMT_SGFSR_IN)
#define HWIO_DEHR_VMIDMT_SGFSR_MULTI_CLIENT_BMSK                  0x80000000
#define HWIO_DEHR_VMIDMT_SGFSR_MULTI_CLIENT_SHFT                        0x1f
#define HWIO_DEHR_VMIDMT_SGFSR_MULTI_CFG_BMSK                     0x40000000
#define HWIO_DEHR_VMIDMT_SGFSR_MULTI_CFG_SHFT                           0x1e
#define HWIO_DEHR_VMIDMT_SGFSR_CAF_BMSK                                 0x20
#define HWIO_DEHR_VMIDMT_SGFSR_CAF_SHFT                                  0x5
#define HWIO_DEHR_VMIDMT_SGFSR_USF_BMSK                                  0x2
#define HWIO_DEHR_VMIDMT_SGFSR_USF_SHFT                                  0x1

#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_ADDR                        (DEHR_VMIDMT_REG_BASE      + 0x0000004c)
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_RMSK                        0xc0000022
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_SGFSRRESTORE_ADDR, HWIO_DEHR_VMIDMT_SGFSRRESTORE_RMSK)
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SGFSRRESTORE_ADDR, m)
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_SGFSRRESTORE_ADDR,v)
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_SGFSRRESTORE_ADDR,m,v,HWIO_DEHR_VMIDMT_SGFSRRESTORE_IN)
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_MULTI_CLIENT_BMSK           0x80000000
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_MULTI_CLIENT_SHFT                 0x1f
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_MULTI_CFG_BMSK              0x40000000
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_MULTI_CFG_SHFT                    0x1e
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_CAF_BMSK                          0x20
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_CAF_SHFT                           0x5
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_USF_BMSK                           0x2
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_USF_SHFT                           0x1

#define HWIO_DEHR_VMIDMT_SGFSYNDR0_ADDR                           (DEHR_VMIDMT_REG_BASE      + 0x00000050)
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_RMSK                                0x132
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_SGFSYNDR0_ADDR, HWIO_DEHR_VMIDMT_SGFSYNDR0_RMSK)
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SGFSYNDR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_MSSSELFAUTH_BMSK                    0x100
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_MSSSELFAUTH_SHFT                      0x8
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_NSATTR_BMSK                          0x20
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_NSATTR_SHFT                           0x5
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_NSSTATE_BMSK                         0x10
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_NSSTATE_SHFT                          0x4
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_WNR_BMSK                              0x2
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_WNR_SHFT                              0x1

#define HWIO_DEHR_VMIDMT_SGFSYNDR1_ADDR                           (DEHR_VMIDMT_REG_BASE      + 0x00000054)
#define HWIO_DEHR_VMIDMT_SGFSYNDR1_RMSK                            0x1010001
#define HWIO_DEHR_VMIDMT_SGFSYNDR1_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_SGFSYNDR1_ADDR, HWIO_DEHR_VMIDMT_SGFSYNDR1_RMSK)
#define HWIO_DEHR_VMIDMT_SGFSYNDR1_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SGFSYNDR1_ADDR, m)
#define HWIO_DEHR_VMIDMT_SGFSYNDR1_MSDINDEX_BMSK                   0x1000000
#define HWIO_DEHR_VMIDMT_SGFSYNDR1_MSDINDEX_SHFT                        0x18
#define HWIO_DEHR_VMIDMT_SGFSYNDR1_SSDINDEX_BMSK                     0x10000
#define HWIO_DEHR_VMIDMT_SGFSYNDR1_SSDINDEX_SHFT                        0x10
#define HWIO_DEHR_VMIDMT_SGFSYNDR1_STREAMINDEX_BMSK                      0x1
#define HWIO_DEHR_VMIDMT_SGFSYNDR1_STREAMINDEX_SHFT                      0x0

#define HWIO_DEHR_VMIDMT_SGFSYNDR2_ADDR                           (DEHR_VMIDMT_REG_BASE      + 0x00000058)
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_RMSK                           0x3f1fffff
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_SGFSYNDR2_ADDR, HWIO_DEHR_VMIDMT_SGFSYNDR2_RMSK)
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SGFSYNDR2_ADDR, m)
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_ATID_BMSK                      0x3f000000
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_ATID_SHFT                            0x18
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_AVMID_BMSK                       0x1f0000
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_AVMID_SHFT                           0x10
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_ABID_BMSK                          0xe000
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_ABID_SHFT                             0xd
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_APID_BMSK                          0x1f00
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_APID_SHFT                             0x8
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_AMID_BMSK                            0xff
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_AMID_SHFT                             0x0

#define HWIO_DEHR_VMIDMT_VMIDMTSCR0_ADDR                          (DEHR_VMIDMT_REG_BASE      + 0x00000090)
#define HWIO_DEHR_VMIDMT_VMIDMTSCR0_RMSK                                 0x1
#define HWIO_DEHR_VMIDMT_VMIDMTSCR0_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_VMIDMTSCR0_ADDR, HWIO_DEHR_VMIDMT_VMIDMTSCR0_RMSK)
#define HWIO_DEHR_VMIDMT_VMIDMTSCR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_VMIDMTSCR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_VMIDMTSCR0_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_VMIDMTSCR0_ADDR,v)
#define HWIO_DEHR_VMIDMT_VMIDMTSCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_VMIDMTSCR0_ADDR,m,v,HWIO_DEHR_VMIDMT_VMIDMTSCR0_IN)
#define HWIO_DEHR_VMIDMT_VMIDMTSCR0_CLKONOFFE_BMSK                       0x1
#define HWIO_DEHR_VMIDMT_VMIDMTSCR0_CLKONOFFE_SHFT                       0x0

#define HWIO_DEHR_VMIDMT_CR0_ADDR                                 (DEHR_VMIDMT_REG_BASE      + 0x00000000)
#define HWIO_DEHR_VMIDMT_CR0_RMSK                                  0xfd70ff5
#define HWIO_DEHR_VMIDMT_CR0_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_CR0_ADDR, HWIO_DEHR_VMIDMT_CR0_RMSK)
#define HWIO_DEHR_VMIDMT_CR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_CR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_CR0_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_CR0_ADDR,v)
#define HWIO_DEHR_VMIDMT_CR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_CR0_ADDR,m,v,HWIO_DEHR_VMIDMT_CR0_IN)
#define HWIO_DEHR_VMIDMT_CR0_WACFG_BMSK                            0xc000000
#define HWIO_DEHR_VMIDMT_CR0_WACFG_SHFT                                 0x1a
#define HWIO_DEHR_VMIDMT_CR0_RACFG_BMSK                            0x3000000
#define HWIO_DEHR_VMIDMT_CR0_RACFG_SHFT                                 0x18
#define HWIO_DEHR_VMIDMT_CR0_SHCFG_BMSK                             0xc00000
#define HWIO_DEHR_VMIDMT_CR0_SHCFG_SHFT                                 0x16
#define HWIO_DEHR_VMIDMT_CR0_MTCFG_BMSK                             0x100000
#define HWIO_DEHR_VMIDMT_CR0_MTCFG_SHFT                                 0x14
#define HWIO_DEHR_VMIDMT_CR0_MEMATTR_BMSK                            0x70000
#define HWIO_DEHR_VMIDMT_CR0_MEMATTR_SHFT                               0x10
#define HWIO_DEHR_VMIDMT_CR0_VMIDPNE_BMSK                              0x800
#define HWIO_DEHR_VMIDMT_CR0_VMIDPNE_SHFT                                0xb
#define HWIO_DEHR_VMIDMT_CR0_USFCFG_BMSK                               0x400
#define HWIO_DEHR_VMIDMT_CR0_USFCFG_SHFT                                 0xa
#define HWIO_DEHR_VMIDMT_CR0_GSE_BMSK                                  0x200
#define HWIO_DEHR_VMIDMT_CR0_GSE_SHFT                                    0x9
#define HWIO_DEHR_VMIDMT_CR0_STALLD_BMSK                               0x100
#define HWIO_DEHR_VMIDMT_CR0_STALLD_SHFT                                 0x8
#define HWIO_DEHR_VMIDMT_CR0_TRANSIENTCFG_BMSK                          0xc0
#define HWIO_DEHR_VMIDMT_CR0_TRANSIENTCFG_SHFT                           0x6
#define HWIO_DEHR_VMIDMT_CR0_GCFGFIE_BMSK                               0x20
#define HWIO_DEHR_VMIDMT_CR0_GCFGFIE_SHFT                                0x5
#define HWIO_DEHR_VMIDMT_CR0_GCFGERE_BMSK                               0x10
#define HWIO_DEHR_VMIDMT_CR0_GCFGERE_SHFT                                0x4
#define HWIO_DEHR_VMIDMT_CR0_GFIE_BMSK                                   0x4
#define HWIO_DEHR_VMIDMT_CR0_GFIE_SHFT                                   0x2
#define HWIO_DEHR_VMIDMT_CR0_CLIENTPD_BMSK                               0x1
#define HWIO_DEHR_VMIDMT_CR0_CLIENTPD_SHFT                               0x0

#define HWIO_DEHR_VMIDMT_CR2_ADDR                                 (DEHR_VMIDMT_REG_BASE      + 0x00000008)
#define HWIO_DEHR_VMIDMT_CR2_RMSK                                       0x1f
#define HWIO_DEHR_VMIDMT_CR2_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_CR2_ADDR, HWIO_DEHR_VMIDMT_CR2_RMSK)
#define HWIO_DEHR_VMIDMT_CR2_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_CR2_ADDR, m)
#define HWIO_DEHR_VMIDMT_CR2_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_CR2_ADDR,v)
#define HWIO_DEHR_VMIDMT_CR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_CR2_ADDR,m,v,HWIO_DEHR_VMIDMT_CR2_IN)
#define HWIO_DEHR_VMIDMT_CR2_BPVMID_BMSK                                0x1f
#define HWIO_DEHR_VMIDMT_CR2_BPVMID_SHFT                                 0x0

#define HWIO_DEHR_VMIDMT_ACR_ADDR                                 (DEHR_VMIDMT_REG_BASE      + 0x00000010)
#define HWIO_DEHR_VMIDMT_ACR_RMSK                                 0x70000013
#define HWIO_DEHR_VMIDMT_ACR_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_ACR_ADDR, HWIO_DEHR_VMIDMT_ACR_RMSK)
#define HWIO_DEHR_VMIDMT_ACR_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_ACR_ADDR, m)
#define HWIO_DEHR_VMIDMT_ACR_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_ACR_ADDR,v)
#define HWIO_DEHR_VMIDMT_ACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_ACR_ADDR,m,v,HWIO_DEHR_VMIDMT_ACR_IN)
#define HWIO_DEHR_VMIDMT_ACR_BPRCNSH_BMSK                         0x40000000
#define HWIO_DEHR_VMIDMT_ACR_BPRCNSH_SHFT                               0x1e
#define HWIO_DEHR_VMIDMT_ACR_BPRCISH_BMSK                         0x20000000
#define HWIO_DEHR_VMIDMT_ACR_BPRCISH_SHFT                               0x1d
#define HWIO_DEHR_VMIDMT_ACR_BPRCOSH_BMSK                         0x10000000
#define HWIO_DEHR_VMIDMT_ACR_BPRCOSH_SHFT                               0x1c
#define HWIO_DEHR_VMIDMT_ACR_BPREQPRIORITYCFG_BMSK                      0x10
#define HWIO_DEHR_VMIDMT_ACR_BPREQPRIORITYCFG_SHFT                       0x4
#define HWIO_DEHR_VMIDMT_ACR_BPREQPRIORITY_BMSK                          0x3
#define HWIO_DEHR_VMIDMT_ACR_BPREQPRIORITY_SHFT                          0x0

#define HWIO_DEHR_VMIDMT_IDR0_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000020)
#define HWIO_DEHR_VMIDMT_IDR0_RMSK                                 0x8001eff
#define HWIO_DEHR_VMIDMT_IDR0_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_IDR0_ADDR, HWIO_DEHR_VMIDMT_IDR0_RMSK)
#define HWIO_DEHR_VMIDMT_IDR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_IDR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_IDR0_SMS_BMSK                             0x8000000
#define HWIO_DEHR_VMIDMT_IDR0_SMS_SHFT                                  0x1b
#define HWIO_DEHR_VMIDMT_IDR0_NUMSIDB_BMSK                            0x1e00
#define HWIO_DEHR_VMIDMT_IDR0_NUMSIDB_SHFT                               0x9
#define HWIO_DEHR_VMIDMT_IDR0_NUMSMRG_BMSK                              0xff
#define HWIO_DEHR_VMIDMT_IDR0_NUMSMRG_SHFT                               0x0

#define HWIO_DEHR_VMIDMT_IDR1_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000024)
#define HWIO_DEHR_VMIDMT_IDR1_RMSK                                    0x9f00
#define HWIO_DEHR_VMIDMT_IDR1_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_IDR1_ADDR, HWIO_DEHR_VMIDMT_IDR1_RMSK)
#define HWIO_DEHR_VMIDMT_IDR1_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_IDR1_ADDR, m)
#define HWIO_DEHR_VMIDMT_IDR1_SMCD_BMSK                               0x8000
#define HWIO_DEHR_VMIDMT_IDR1_SMCD_SHFT                                  0xf
#define HWIO_DEHR_VMIDMT_IDR1_SSDTP_BMSK                              0x1000
#define HWIO_DEHR_VMIDMT_IDR1_SSDTP_SHFT                                 0xc
#define HWIO_DEHR_VMIDMT_IDR1_NUMSSDNDX_BMSK                           0xf00
#define HWIO_DEHR_VMIDMT_IDR1_NUMSSDNDX_SHFT                             0x8

#define HWIO_DEHR_VMIDMT_IDR2_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000028)
#define HWIO_DEHR_VMIDMT_IDR2_RMSK                                      0xff
#define HWIO_DEHR_VMIDMT_IDR2_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_IDR2_ADDR, HWIO_DEHR_VMIDMT_IDR2_RMSK)
#define HWIO_DEHR_VMIDMT_IDR2_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_IDR2_ADDR, m)
#define HWIO_DEHR_VMIDMT_IDR2_OAS_BMSK                                  0xf0
#define HWIO_DEHR_VMIDMT_IDR2_OAS_SHFT                                   0x4
#define HWIO_DEHR_VMIDMT_IDR2_IAS_BMSK                                   0xf
#define HWIO_DEHR_VMIDMT_IDR2_IAS_SHFT                                   0x0

#define HWIO_DEHR_VMIDMT_IDR4_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000030)
#define HWIO_DEHR_VMIDMT_IDR4_RMSK                                0xffffffff
#define HWIO_DEHR_VMIDMT_IDR4_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_IDR4_ADDR, HWIO_DEHR_VMIDMT_IDR4_RMSK)
#define HWIO_DEHR_VMIDMT_IDR4_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_IDR4_ADDR, m)
#define HWIO_DEHR_VMIDMT_IDR4_MAJOR_BMSK                          0xf0000000
#define HWIO_DEHR_VMIDMT_IDR4_MAJOR_SHFT                                0x1c
#define HWIO_DEHR_VMIDMT_IDR4_MINOR_BMSK                           0xfff0000
#define HWIO_DEHR_VMIDMT_IDR4_MINOR_SHFT                                0x10
#define HWIO_DEHR_VMIDMT_IDR4_STEP_BMSK                               0xffff
#define HWIO_DEHR_VMIDMT_IDR4_STEP_SHFT                                  0x0

#define HWIO_DEHR_VMIDMT_IDR5_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000034)
#define HWIO_DEHR_VMIDMT_IDR5_RMSK                                  0xff03ff
#define HWIO_DEHR_VMIDMT_IDR5_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_IDR5_ADDR, HWIO_DEHR_VMIDMT_IDR5_RMSK)
#define HWIO_DEHR_VMIDMT_IDR5_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_IDR5_ADDR, m)
#define HWIO_DEHR_VMIDMT_IDR5_NUMMSDRB_BMSK                         0xff0000
#define HWIO_DEHR_VMIDMT_IDR5_NUMMSDRB_SHFT                             0x10
#define HWIO_DEHR_VMIDMT_IDR5_MSAE_BMSK                                0x200
#define HWIO_DEHR_VMIDMT_IDR5_MSAE_SHFT                                  0x9
#define HWIO_DEHR_VMIDMT_IDR5_QRIBE_BMSK                               0x100
#define HWIO_DEHR_VMIDMT_IDR5_QRIBE_SHFT                                 0x8
#define HWIO_DEHR_VMIDMT_IDR5_NVMID_BMSK                                0xff
#define HWIO_DEHR_VMIDMT_IDR5_NVMID_SHFT                                 0x0

#define HWIO_DEHR_VMIDMT_IDR7_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x0000003c)
#define HWIO_DEHR_VMIDMT_IDR7_RMSK                                      0xff
#define HWIO_DEHR_VMIDMT_IDR7_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_IDR7_ADDR, HWIO_DEHR_VMIDMT_IDR7_RMSK)
#define HWIO_DEHR_VMIDMT_IDR7_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_IDR7_ADDR, m)
#define HWIO_DEHR_VMIDMT_IDR7_MAJOR_BMSK                                0xf0
#define HWIO_DEHR_VMIDMT_IDR7_MAJOR_SHFT                                 0x4
#define HWIO_DEHR_VMIDMT_IDR7_MINOR_BMSK                                 0xf
#define HWIO_DEHR_VMIDMT_IDR7_MINOR_SHFT                                 0x0

#define HWIO_DEHR_VMIDMT_GFAR0_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000040)
#define HWIO_DEHR_VMIDMT_GFAR0_RMSK                               0xffffffff
#define HWIO_DEHR_VMIDMT_GFAR0_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_GFAR0_ADDR, HWIO_DEHR_VMIDMT_GFAR0_RMSK)
#define HWIO_DEHR_VMIDMT_GFAR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_GFAR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_GFAR0_GFEA0_BMSK                         0xffffffff
#define HWIO_DEHR_VMIDMT_GFAR0_GFEA0_SHFT                                0x0

#define HWIO_DEHR_VMIDMT_GFSR_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000048)
#define HWIO_DEHR_VMIDMT_GFSR_RMSK                                0xc00000a2
#define HWIO_DEHR_VMIDMT_GFSR_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_GFSR_ADDR, HWIO_DEHR_VMIDMT_GFSR_RMSK)
#define HWIO_DEHR_VMIDMT_GFSR_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_GFSR_ADDR, m)
#define HWIO_DEHR_VMIDMT_GFSR_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_GFSR_ADDR,v)
#define HWIO_DEHR_VMIDMT_GFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_GFSR_ADDR,m,v,HWIO_DEHR_VMIDMT_GFSR_IN)
#define HWIO_DEHR_VMIDMT_GFSR_MULTI_CLIENT_BMSK                   0x80000000
#define HWIO_DEHR_VMIDMT_GFSR_MULTI_CLIENT_SHFT                         0x1f
#define HWIO_DEHR_VMIDMT_GFSR_MULTI_CFG_BMSK                      0x40000000
#define HWIO_DEHR_VMIDMT_GFSR_MULTI_CFG_SHFT                            0x1e
#define HWIO_DEHR_VMIDMT_GFSR_PF_BMSK                                   0x80
#define HWIO_DEHR_VMIDMT_GFSR_PF_SHFT                                    0x7
#define HWIO_DEHR_VMIDMT_GFSR_CAF_BMSK                                  0x20
#define HWIO_DEHR_VMIDMT_GFSR_CAF_SHFT                                   0x5
#define HWIO_DEHR_VMIDMT_GFSR_USF_BMSK                                   0x2
#define HWIO_DEHR_VMIDMT_GFSR_USF_SHFT                                   0x1

#define HWIO_DEHR_VMIDMT_GFSRRESTORE_ADDR                         (DEHR_VMIDMT_REG_BASE      + 0x0000004c)
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_RMSK                         0xc00000a2
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_GFSRRESTORE_ADDR, HWIO_DEHR_VMIDMT_GFSRRESTORE_RMSK)
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_GFSRRESTORE_ADDR, m)
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_GFSRRESTORE_ADDR,v)
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_GFSRRESTORE_ADDR,m,v,HWIO_DEHR_VMIDMT_GFSRRESTORE_IN)
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_MULTI_CLIENT_BMSK            0x80000000
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_MULTI_CLIENT_SHFT                  0x1f
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_MULTI_CFG_BMSK               0x40000000
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_MULTI_CFG_SHFT                     0x1e
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_PF_BMSK                            0x80
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_PF_SHFT                             0x7
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_CAF_BMSK                           0x20
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_CAF_SHFT                            0x5
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_USF_BMSK                            0x2
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_USF_SHFT                            0x1

#define HWIO_DEHR_VMIDMT_GFSYNDR0_ADDR                            (DEHR_VMIDMT_REG_BASE      + 0x00000050)
#define HWIO_DEHR_VMIDMT_GFSYNDR0_RMSK                                 0x132
#define HWIO_DEHR_VMIDMT_GFSYNDR0_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_GFSYNDR0_ADDR, HWIO_DEHR_VMIDMT_GFSYNDR0_RMSK)
#define HWIO_DEHR_VMIDMT_GFSYNDR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_GFSYNDR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_GFSYNDR0_MSSSELFAUTH_BMSK                     0x100
#define HWIO_DEHR_VMIDMT_GFSYNDR0_MSSSELFAUTH_SHFT                       0x8
#define HWIO_DEHR_VMIDMT_GFSYNDR0_NSATTR_BMSK                           0x20
#define HWIO_DEHR_VMIDMT_GFSYNDR0_NSATTR_SHFT                            0x5
#define HWIO_DEHR_VMIDMT_GFSYNDR0_NSSTATE_BMSK                          0x10
#define HWIO_DEHR_VMIDMT_GFSYNDR0_NSSTATE_SHFT                           0x4
#define HWIO_DEHR_VMIDMT_GFSYNDR0_WNR_BMSK                               0x2
#define HWIO_DEHR_VMIDMT_GFSYNDR0_WNR_SHFT                               0x1

#define HWIO_DEHR_VMIDMT_GFSYNDR1_ADDR                            (DEHR_VMIDMT_REG_BASE      + 0x00000054)
#define HWIO_DEHR_VMIDMT_GFSYNDR1_RMSK                             0x1010001
#define HWIO_DEHR_VMIDMT_GFSYNDR1_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_GFSYNDR1_ADDR, HWIO_DEHR_VMIDMT_GFSYNDR1_RMSK)
#define HWIO_DEHR_VMIDMT_GFSYNDR1_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_GFSYNDR1_ADDR, m)
#define HWIO_DEHR_VMIDMT_GFSYNDR1_MSDINDEX_BMSK                    0x1000000
#define HWIO_DEHR_VMIDMT_GFSYNDR1_MSDINDEX_SHFT                         0x18
#define HWIO_DEHR_VMIDMT_GFSYNDR1_SSDINDEX_BMSK                      0x10000
#define HWIO_DEHR_VMIDMT_GFSYNDR1_SSDINDEX_SHFT                         0x10
#define HWIO_DEHR_VMIDMT_GFSYNDR1_STREAMINDEX_BMSK                       0x1
#define HWIO_DEHR_VMIDMT_GFSYNDR1_STREAMINDEX_SHFT                       0x0

#define HWIO_DEHR_VMIDMT_GFSYNDR2_ADDR                            (DEHR_VMIDMT_REG_BASE      + 0x00000058)
#define HWIO_DEHR_VMIDMT_GFSYNDR2_RMSK                            0x3f1fffff
#define HWIO_DEHR_VMIDMT_GFSYNDR2_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_GFSYNDR2_ADDR, HWIO_DEHR_VMIDMT_GFSYNDR2_RMSK)
#define HWIO_DEHR_VMIDMT_GFSYNDR2_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_GFSYNDR2_ADDR, m)
#define HWIO_DEHR_VMIDMT_GFSYNDR2_ATID_BMSK                       0x3f000000
#define HWIO_DEHR_VMIDMT_GFSYNDR2_ATID_SHFT                             0x18
#define HWIO_DEHR_VMIDMT_GFSYNDR2_AVMID_BMSK                        0x1f0000
#define HWIO_DEHR_VMIDMT_GFSYNDR2_AVMID_SHFT                            0x10
#define HWIO_DEHR_VMIDMT_GFSYNDR2_ABID_BMSK                           0xe000
#define HWIO_DEHR_VMIDMT_GFSYNDR2_ABID_SHFT                              0xd
#define HWIO_DEHR_VMIDMT_GFSYNDR2_APID_BMSK                           0x1f00
#define HWIO_DEHR_VMIDMT_GFSYNDR2_APID_SHFT                              0x8
#define HWIO_DEHR_VMIDMT_GFSYNDR2_AMID_BMSK                             0xff
#define HWIO_DEHR_VMIDMT_GFSYNDR2_AMID_SHFT                              0x0

#define HWIO_DEHR_VMIDMT_VMIDMTCR0_ADDR                           (DEHR_VMIDMT_REG_BASE      + 0x00000090)
#define HWIO_DEHR_VMIDMT_VMIDMTCR0_RMSK                                  0x1
#define HWIO_DEHR_VMIDMT_VMIDMTCR0_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_VMIDMTCR0_ADDR, HWIO_DEHR_VMIDMT_VMIDMTCR0_RMSK)
#define HWIO_DEHR_VMIDMT_VMIDMTCR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_VMIDMTCR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_VMIDMTCR0_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_VMIDMTCR0_ADDR,v)
#define HWIO_DEHR_VMIDMT_VMIDMTCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_VMIDMTCR0_ADDR,m,v,HWIO_DEHR_VMIDMT_VMIDMTCR0_IN)
#define HWIO_DEHR_VMIDMT_VMIDMTCR0_CLKONOFFE_BMSK                        0x1
#define HWIO_DEHR_VMIDMT_VMIDMTCR0_CLKONOFFE_SHFT                        0x0

#define HWIO_DEHR_VMIDMT_VMIDMTACR_ADDR                           (DEHR_VMIDMT_REG_BASE      + 0x0000009c)
#define HWIO_DEHR_VMIDMT_VMIDMTACR_RMSK                           0xffffffff
#define HWIO_DEHR_VMIDMT_VMIDMTACR_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_VMIDMTACR_ADDR, HWIO_DEHR_VMIDMT_VMIDMTACR_RMSK)
#define HWIO_DEHR_VMIDMT_VMIDMTACR_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_VMIDMTACR_ADDR, m)
#define HWIO_DEHR_VMIDMT_VMIDMTACR_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_VMIDMTACR_ADDR,v)
#define HWIO_DEHR_VMIDMT_VMIDMTACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_VMIDMTACR_ADDR,m,v,HWIO_DEHR_VMIDMT_VMIDMTACR_IN)
#define HWIO_DEHR_VMIDMT_VMIDMTACR_RWE_BMSK                       0xffffffff
#define HWIO_DEHR_VMIDMT_VMIDMTACR_RWE_SHFT                              0x0

#define HWIO_DEHR_VMIDMT_NSCR0_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000400)
#define HWIO_DEHR_VMIDMT_NSCR0_RMSK                                0xfd70ff5
#define HWIO_DEHR_VMIDMT_NSCR0_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSCR0_ADDR, HWIO_DEHR_VMIDMT_NSCR0_RMSK)
#define HWIO_DEHR_VMIDMT_NSCR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSCR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSCR0_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_NSCR0_ADDR,v)
#define HWIO_DEHR_VMIDMT_NSCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_NSCR0_ADDR,m,v,HWIO_DEHR_VMIDMT_NSCR0_IN)
#define HWIO_DEHR_VMIDMT_NSCR0_WACFG_BMSK                          0xc000000
#define HWIO_DEHR_VMIDMT_NSCR0_WACFG_SHFT                               0x1a
#define HWIO_DEHR_VMIDMT_NSCR0_RACFG_BMSK                          0x3000000
#define HWIO_DEHR_VMIDMT_NSCR0_RACFG_SHFT                               0x18
#define HWIO_DEHR_VMIDMT_NSCR0_SHCFG_BMSK                           0xc00000
#define HWIO_DEHR_VMIDMT_NSCR0_SHCFG_SHFT                               0x16
#define HWIO_DEHR_VMIDMT_NSCR0_MTCFG_BMSK                           0x100000
#define HWIO_DEHR_VMIDMT_NSCR0_MTCFG_SHFT                               0x14
#define HWIO_DEHR_VMIDMT_NSCR0_MEMATTR_BMSK                          0x70000
#define HWIO_DEHR_VMIDMT_NSCR0_MEMATTR_SHFT                             0x10
#define HWIO_DEHR_VMIDMT_NSCR0_VMIDPNE_BMSK                            0x800
#define HWIO_DEHR_VMIDMT_NSCR0_VMIDPNE_SHFT                              0xb
#define HWIO_DEHR_VMIDMT_NSCR0_USFCFG_BMSK                             0x400
#define HWIO_DEHR_VMIDMT_NSCR0_USFCFG_SHFT                               0xa
#define HWIO_DEHR_VMIDMT_NSCR0_GSE_BMSK                                0x200
#define HWIO_DEHR_VMIDMT_NSCR0_GSE_SHFT                                  0x9
#define HWIO_DEHR_VMIDMT_NSCR0_STALLD_BMSK                             0x100
#define HWIO_DEHR_VMIDMT_NSCR0_STALLD_SHFT                               0x8
#define HWIO_DEHR_VMIDMT_NSCR0_TRANSIENTCFG_BMSK                        0xc0
#define HWIO_DEHR_VMIDMT_NSCR0_TRANSIENTCFG_SHFT                         0x6
#define HWIO_DEHR_VMIDMT_NSCR0_GCFGFIE_BMSK                             0x20
#define HWIO_DEHR_VMIDMT_NSCR0_GCFGFIE_SHFT                              0x5
#define HWIO_DEHR_VMIDMT_NSCR0_GCFGERE_BMSK                             0x10
#define HWIO_DEHR_VMIDMT_NSCR0_GCFGERE_SHFT                              0x4
#define HWIO_DEHR_VMIDMT_NSCR0_GFIE_BMSK                                 0x4
#define HWIO_DEHR_VMIDMT_NSCR0_GFIE_SHFT                                 0x2
#define HWIO_DEHR_VMIDMT_NSCR0_CLIENTPD_BMSK                             0x1
#define HWIO_DEHR_VMIDMT_NSCR0_CLIENTPD_SHFT                             0x0

#define HWIO_DEHR_VMIDMT_NSCR2_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000408)
#define HWIO_DEHR_VMIDMT_NSCR2_RMSK                                     0x1f
#define HWIO_DEHR_VMIDMT_NSCR2_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSCR2_ADDR, HWIO_DEHR_VMIDMT_NSCR2_RMSK)
#define HWIO_DEHR_VMIDMT_NSCR2_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSCR2_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSCR2_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_NSCR2_ADDR,v)
#define HWIO_DEHR_VMIDMT_NSCR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_NSCR2_ADDR,m,v,HWIO_DEHR_VMIDMT_NSCR2_IN)
#define HWIO_DEHR_VMIDMT_NSCR2_BPVMID_BMSK                              0x1f
#define HWIO_DEHR_VMIDMT_NSCR2_BPVMID_SHFT                               0x0

#define HWIO_DEHR_VMIDMT_NSACR_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000410)
#define HWIO_DEHR_VMIDMT_NSACR_RMSK                               0x70000013
#define HWIO_DEHR_VMIDMT_NSACR_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSACR_ADDR, HWIO_DEHR_VMIDMT_NSACR_RMSK)
#define HWIO_DEHR_VMIDMT_NSACR_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSACR_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSACR_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_NSACR_ADDR,v)
#define HWIO_DEHR_VMIDMT_NSACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_NSACR_ADDR,m,v,HWIO_DEHR_VMIDMT_NSACR_IN)
#define HWIO_DEHR_VMIDMT_NSACR_BPRCNSH_BMSK                       0x40000000
#define HWIO_DEHR_VMIDMT_NSACR_BPRCNSH_SHFT                             0x1e
#define HWIO_DEHR_VMIDMT_NSACR_BPRCISH_BMSK                       0x20000000
#define HWIO_DEHR_VMIDMT_NSACR_BPRCISH_SHFT                             0x1d
#define HWIO_DEHR_VMIDMT_NSACR_BPRCOSH_BMSK                       0x10000000
#define HWIO_DEHR_VMIDMT_NSACR_BPRCOSH_SHFT                             0x1c
#define HWIO_DEHR_VMIDMT_NSACR_BPREQPRIORITYCFG_BMSK                    0x10
#define HWIO_DEHR_VMIDMT_NSACR_BPREQPRIORITYCFG_SHFT                     0x4
#define HWIO_DEHR_VMIDMT_NSACR_BPREQPRIORITY_BMSK                        0x3
#define HWIO_DEHR_VMIDMT_NSACR_BPREQPRIORITY_SHFT                        0x0

#define HWIO_DEHR_VMIDMT_NSGFAR0_ADDR                             (DEHR_VMIDMT_REG_BASE      + 0x00000440)
#define HWIO_DEHR_VMIDMT_NSGFAR0_RMSK                             0xffffffff
#define HWIO_DEHR_VMIDMT_NSGFAR0_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSGFAR0_ADDR, HWIO_DEHR_VMIDMT_NSGFAR0_RMSK)
#define HWIO_DEHR_VMIDMT_NSGFAR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSGFAR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSGFAR0_GFEA0_BMSK                       0xffffffff
#define HWIO_DEHR_VMIDMT_NSGFAR0_GFEA0_SHFT                              0x0

#define HWIO_DEHR_VMIDMT_NSGFSR_ADDR                              (DEHR_VMIDMT_REG_BASE      + 0x00000448)
#define HWIO_DEHR_VMIDMT_NSGFSR_RMSK                              0xc00000a2
#define HWIO_DEHR_VMIDMT_NSGFSR_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSGFSR_ADDR, HWIO_DEHR_VMIDMT_NSGFSR_RMSK)
#define HWIO_DEHR_VMIDMT_NSGFSR_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSGFSR_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSGFSR_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_NSGFSR_ADDR,v)
#define HWIO_DEHR_VMIDMT_NSGFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_NSGFSR_ADDR,m,v,HWIO_DEHR_VMIDMT_NSGFSR_IN)
#define HWIO_DEHR_VMIDMT_NSGFSR_MULTI_CLIENT_BMSK                 0x80000000
#define HWIO_DEHR_VMIDMT_NSGFSR_MULTI_CLIENT_SHFT                       0x1f
#define HWIO_DEHR_VMIDMT_NSGFSR_MULTI_CFG_BMSK                    0x40000000
#define HWIO_DEHR_VMIDMT_NSGFSR_MULTI_CFG_SHFT                          0x1e
#define HWIO_DEHR_VMIDMT_NSGFSR_PF_BMSK                                 0x80
#define HWIO_DEHR_VMIDMT_NSGFSR_PF_SHFT                                  0x7
#define HWIO_DEHR_VMIDMT_NSGFSR_CAF_BMSK                                0x20
#define HWIO_DEHR_VMIDMT_NSGFSR_CAF_SHFT                                 0x5
#define HWIO_DEHR_VMIDMT_NSGFSR_USF_BMSK                                 0x2
#define HWIO_DEHR_VMIDMT_NSGFSR_USF_SHFT                                 0x1

#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_ADDR                       (DEHR_VMIDMT_REG_BASE      + 0x0000044c)
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_RMSK                       0xc00000a2
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSGFSRRESTORE_ADDR, HWIO_DEHR_VMIDMT_NSGFSRRESTORE_RMSK)
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSGFSRRESTORE_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_NSGFSRRESTORE_ADDR,v)
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_NSGFSRRESTORE_ADDR,m,v,HWIO_DEHR_VMIDMT_NSGFSRRESTORE_IN)
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_MULTI_CLIENT_BMSK          0x80000000
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_MULTI_CLIENT_SHFT                0x1f
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_MULTI_CFG_BMSK             0x40000000
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_MULTI_CFG_SHFT                   0x1e
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_PF_BMSK                          0x80
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_PF_SHFT                           0x7
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_CAF_BMSK                         0x20
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_CAF_SHFT                          0x5
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_USF_BMSK                          0x2
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_USF_SHFT                          0x1

#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_ADDR                          (DEHR_VMIDMT_REG_BASE      + 0x00000450)
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_RMSK                               0x132
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSGFSYNDR0_ADDR, HWIO_DEHR_VMIDMT_NSGFSYNDR0_RMSK)
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSGFSYNDR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_MSSSELFAUTH_BMSK                   0x100
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_MSSSELFAUTH_SHFT                     0x8
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_NSATTR_BMSK                         0x20
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_NSATTR_SHFT                          0x5
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_NSSTATE_BMSK                        0x10
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_NSSTATE_SHFT                         0x4
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_WNR_BMSK                             0x2
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_WNR_SHFT                             0x1

#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_ADDR                          (DEHR_VMIDMT_REG_BASE      + 0x00000454)
#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_RMSK                           0x1010001
#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSGFSYNDR1_ADDR, HWIO_DEHR_VMIDMT_NSGFSYNDR1_RMSK)
#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSGFSYNDR1_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_MSDINDEX_BMSK                  0x1000000
#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_MSDINDEX_SHFT                       0x18
#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_SSDINDEX_BMSK                    0x10000
#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_SSDINDEX_SHFT                       0x10
#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_STREAMINDEX_BMSK                     0x1
#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_STREAMINDEX_SHFT                     0x0

#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_ADDR                          (DEHR_VMIDMT_REG_BASE      + 0x00000458)
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_RMSK                          0x3f1fffff
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSGFSYNDR2_ADDR, HWIO_DEHR_VMIDMT_NSGFSYNDR2_RMSK)
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSGFSYNDR2_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_ATID_BMSK                     0x3f000000
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_ATID_SHFT                           0x18
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_AVMID_BMSK                      0x1f0000
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_AVMID_SHFT                          0x10
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_ABID_BMSK                         0xe000
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_ABID_SHFT                            0xd
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_APID_BMSK                         0x1f00
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_APID_SHFT                            0x8
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_AMID_BMSK                           0xff
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_AMID_SHFT                            0x0

#define HWIO_DEHR_VMIDMT_NSVMIDMTCR0_ADDR                         (DEHR_VMIDMT_REG_BASE      + 0x00000490)
#define HWIO_DEHR_VMIDMT_NSVMIDMTCR0_RMSK                                0x1
#define HWIO_DEHR_VMIDMT_NSVMIDMTCR0_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSVMIDMTCR0_ADDR, HWIO_DEHR_VMIDMT_NSVMIDMTCR0_RMSK)
#define HWIO_DEHR_VMIDMT_NSVMIDMTCR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSVMIDMTCR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSVMIDMTCR0_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_NSVMIDMTCR0_ADDR,v)
#define HWIO_DEHR_VMIDMT_NSVMIDMTCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_NSVMIDMTCR0_ADDR,m,v,HWIO_DEHR_VMIDMT_NSVMIDMTCR0_IN)
#define HWIO_DEHR_VMIDMT_NSVMIDMTCR0_CLKONOFFE_BMSK                      0x1
#define HWIO_DEHR_VMIDMT_NSVMIDMTCR0_CLKONOFFE_SHFT                      0x0

#define HWIO_DEHR_VMIDMT_SSDR0_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000080)
#define HWIO_DEHR_VMIDMT_SSDR0_RMSK                                      0x1
#define HWIO_DEHR_VMIDMT_SSDR0_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_SSDR0_ADDR, HWIO_DEHR_VMIDMT_SSDR0_RMSK)
#define HWIO_DEHR_VMIDMT_SSDR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SSDR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_SSDR0_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_SSDR0_ADDR,v)
#define HWIO_DEHR_VMIDMT_SSDR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_SSDR0_ADDR,m,v,HWIO_DEHR_VMIDMT_SSDR0_IN)
#define HWIO_DEHR_VMIDMT_SSDR0_RWE_BMSK                                  0x1
#define HWIO_DEHR_VMIDMT_SSDR0_RWE_SHFT                                  0x0

#define HWIO_DEHR_VMIDMT_MSDR0_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000480)
#define HWIO_DEHR_VMIDMT_MSDR0_RMSK                                      0x1
#define HWIO_DEHR_VMIDMT_MSDR0_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_MSDR0_ADDR, HWIO_DEHR_VMIDMT_MSDR0_RMSK)
#define HWIO_DEHR_VMIDMT_MSDR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_MSDR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_MSDR0_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_MSDR0_ADDR,v)
#define HWIO_DEHR_VMIDMT_MSDR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_MSDR0_ADDR,m,v,HWIO_DEHR_VMIDMT_MSDR0_IN)
#define HWIO_DEHR_VMIDMT_MSDR0_RWE_BMSK                                  0x1
#define HWIO_DEHR_VMIDMT_MSDR0_RWE_SHFT                                  0x0

#define HWIO_DEHR_VMIDMT_MCR_ADDR                                 (DEHR_VMIDMT_REG_BASE      + 0x00000494)
#define HWIO_DEHR_VMIDMT_MCR_RMSK                                        0x7
#define HWIO_DEHR_VMIDMT_MCR_IN          \
        in_dword_masked(HWIO_DEHR_VMIDMT_MCR_ADDR, HWIO_DEHR_VMIDMT_MCR_RMSK)
#define HWIO_DEHR_VMIDMT_MCR_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_MCR_ADDR, m)
#define HWIO_DEHR_VMIDMT_MCR_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_MCR_ADDR,v)
#define HWIO_DEHR_VMIDMT_MCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_MCR_ADDR,m,v,HWIO_DEHR_VMIDMT_MCR_IN)
#define HWIO_DEHR_VMIDMT_MCR_CLKONOFFE_BMSK                              0x4
#define HWIO_DEHR_VMIDMT_MCR_CLKONOFFE_SHFT                              0x2
#define HWIO_DEHR_VMIDMT_MCR_BPMSACFG_BMSK                               0x2
#define HWIO_DEHR_VMIDMT_MCR_BPMSACFG_SHFT                               0x1
#define HWIO_DEHR_VMIDMT_MCR_BPSMSACFG_BMSK                              0x1
#define HWIO_DEHR_VMIDMT_MCR_BPSMSACFG_SHFT                              0x0

#define HWIO_DEHR_VMIDMT_S2VRn_ADDR(n)                            (DEHR_VMIDMT_REG_BASE      + 0x00000c00 + 0x4 * (n))
#define HWIO_DEHR_VMIDMT_S2VRn_RMSK                               0x30ff7b1f
#define HWIO_DEHR_VMIDMT_S2VRn_MAXn                                        0
#define HWIO_DEHR_VMIDMT_S2VRn_INI(n)        \
        in_dword_masked(HWIO_DEHR_VMIDMT_S2VRn_ADDR(n), HWIO_DEHR_VMIDMT_S2VRn_RMSK)
#define HWIO_DEHR_VMIDMT_S2VRn_INMI(n,mask)    \
        in_dword_masked(HWIO_DEHR_VMIDMT_S2VRn_ADDR(n), mask)
#define HWIO_DEHR_VMIDMT_S2VRn_OUTI(n,val)    \
        out_dword(HWIO_DEHR_VMIDMT_S2VRn_ADDR(n),val)
#define HWIO_DEHR_VMIDMT_S2VRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_S2VRn_ADDR(n),mask,val,HWIO_DEHR_VMIDMT_S2VRn_INI(n))
#define HWIO_DEHR_VMIDMT_S2VRn_TRANSIENTCFG_BMSK                  0x30000000
#define HWIO_DEHR_VMIDMT_S2VRn_TRANSIENTCFG_SHFT                        0x1c
#define HWIO_DEHR_VMIDMT_S2VRn_WACFG_BMSK                           0xc00000
#define HWIO_DEHR_VMIDMT_S2VRn_WACFG_SHFT                               0x16
#define HWIO_DEHR_VMIDMT_S2VRn_RACFG_BMSK                           0x300000
#define HWIO_DEHR_VMIDMT_S2VRn_RACFG_SHFT                               0x14
#define HWIO_DEHR_VMIDMT_S2VRn_NSCFG_BMSK                            0xc0000
#define HWIO_DEHR_VMIDMT_S2VRn_NSCFG_SHFT                               0x12
#define HWIO_DEHR_VMIDMT_S2VRn_TYPE_BMSK                             0x30000
#define HWIO_DEHR_VMIDMT_S2VRn_TYPE_SHFT                                0x10
#define HWIO_DEHR_VMIDMT_S2VRn_MEMATTR_BMSK                           0x7000
#define HWIO_DEHR_VMIDMT_S2VRn_MEMATTR_SHFT                              0xc
#define HWIO_DEHR_VMIDMT_S2VRn_MTCFG_BMSK                              0x800
#define HWIO_DEHR_VMIDMT_S2VRn_MTCFG_SHFT                                0xb
#define HWIO_DEHR_VMIDMT_S2VRn_SHCFG_BMSK                              0x300
#define HWIO_DEHR_VMIDMT_S2VRn_SHCFG_SHFT                                0x8
#define HWIO_DEHR_VMIDMT_S2VRn_VMID_BMSK                                0x1f
#define HWIO_DEHR_VMIDMT_S2VRn_VMID_SHFT                                 0x0

#define HWIO_DEHR_VMIDMT_AS2VRn_ADDR(n)                           (DEHR_VMIDMT_REG_BASE      + 0x00000e00 + 0x4 * (n))
#define HWIO_DEHR_VMIDMT_AS2VRn_RMSK                              0x70000013
#define HWIO_DEHR_VMIDMT_AS2VRn_MAXn                                       0
#define HWIO_DEHR_VMIDMT_AS2VRn_INI(n)        \
        in_dword_masked(HWIO_DEHR_VMIDMT_AS2VRn_ADDR(n), HWIO_DEHR_VMIDMT_AS2VRn_RMSK)
#define HWIO_DEHR_VMIDMT_AS2VRn_INMI(n,mask)    \
        in_dword_masked(HWIO_DEHR_VMIDMT_AS2VRn_ADDR(n), mask)
#define HWIO_DEHR_VMIDMT_AS2VRn_OUTI(n,val)    \
        out_dword(HWIO_DEHR_VMIDMT_AS2VRn_ADDR(n),val)
#define HWIO_DEHR_VMIDMT_AS2VRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_AS2VRn_ADDR(n),mask,val,HWIO_DEHR_VMIDMT_AS2VRn_INI(n))
#define HWIO_DEHR_VMIDMT_AS2VRn_RCNSH_BMSK                        0x40000000
#define HWIO_DEHR_VMIDMT_AS2VRn_RCNSH_SHFT                              0x1e
#define HWIO_DEHR_VMIDMT_AS2VRn_RCISH_BMSK                        0x20000000
#define HWIO_DEHR_VMIDMT_AS2VRn_RCISH_SHFT                              0x1d
#define HWIO_DEHR_VMIDMT_AS2VRn_RCOSH_BMSK                        0x10000000
#define HWIO_DEHR_VMIDMT_AS2VRn_RCOSH_SHFT                              0x1c
#define HWIO_DEHR_VMIDMT_AS2VRn_REQPRIORITYCFG_BMSK                     0x10
#define HWIO_DEHR_VMIDMT_AS2VRn_REQPRIORITYCFG_SHFT                      0x4
#define HWIO_DEHR_VMIDMT_AS2VRn_REQPRIORITY_BMSK                         0x3
#define HWIO_DEHR_VMIDMT_AS2VRn_REQPRIORITY_SHFT                         0x0

/*----------------------------------------------------------------------------
 * MODULE: MSS_RMB
 *--------------------------------------------------------------------------*/

#define MSS_RMB_REG_BASE                                                   (MSS_TOP_BASE      + 0x00020000)

#define HWIO_MSS_RELAY_MSG_BUFFER_00_ADDR                                  (MSS_RMB_REG_BASE      + 0x00000000)
#define HWIO_MSS_RELAY_MSG_BUFFER_00_RMSK                                  0xffffffff
#define HWIO_MSS_RELAY_MSG_BUFFER_00_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_BUFFER_00_ADDR, HWIO_MSS_RELAY_MSG_BUFFER_00_RMSK)
#define HWIO_MSS_RELAY_MSG_BUFFER_00_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_BUFFER_00_ADDR, m)
#define HWIO_MSS_RELAY_MSG_BUFFER_00_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_BUFFER_00_ADDR,v)
#define HWIO_MSS_RELAY_MSG_BUFFER_00_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_BUFFER_00_ADDR,m,v,HWIO_MSS_RELAY_MSG_BUFFER_00_IN)
#define HWIO_MSS_RELAY_MSG_BUFFER_00_MSS_RELAY_MSG_BUFFER_00_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_BUFFER_00_MSS_RELAY_MSG_BUFFER_00_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_BUFFER_01_ADDR                                  (MSS_RMB_REG_BASE      + 0x00000004)
#define HWIO_MSS_RELAY_MSG_BUFFER_01_RMSK                                  0xffffffff
#define HWIO_MSS_RELAY_MSG_BUFFER_01_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_BUFFER_01_ADDR, HWIO_MSS_RELAY_MSG_BUFFER_01_RMSK)
#define HWIO_MSS_RELAY_MSG_BUFFER_01_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_BUFFER_01_ADDR, m)
#define HWIO_MSS_RELAY_MSG_BUFFER_01_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_BUFFER_01_ADDR,v)
#define HWIO_MSS_RELAY_MSG_BUFFER_01_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_BUFFER_01_ADDR,m,v,HWIO_MSS_RELAY_MSG_BUFFER_01_IN)
#define HWIO_MSS_RELAY_MSG_BUFFER_01_MSS_RELAY_MSG_BUFFER_01_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_BUFFER_01_MSS_RELAY_MSG_BUFFER_01_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_BUFFER_02_ADDR                                  (MSS_RMB_REG_BASE      + 0x00000008)
#define HWIO_MSS_RELAY_MSG_BUFFER_02_RMSK                                  0xffffffff
#define HWIO_MSS_RELAY_MSG_BUFFER_02_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_BUFFER_02_ADDR, HWIO_MSS_RELAY_MSG_BUFFER_02_RMSK)
#define HWIO_MSS_RELAY_MSG_BUFFER_02_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_BUFFER_02_ADDR, m)
#define HWIO_MSS_RELAY_MSG_BUFFER_02_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_BUFFER_02_ADDR,v)
#define HWIO_MSS_RELAY_MSG_BUFFER_02_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_BUFFER_02_ADDR,m,v,HWIO_MSS_RELAY_MSG_BUFFER_02_IN)
#define HWIO_MSS_RELAY_MSG_BUFFER_02_MSS_RELAY_MSG_BUFFER_02_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_BUFFER_02_MSS_RELAY_MSG_BUFFER_02_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_BUFFER_03_ADDR                                  (MSS_RMB_REG_BASE      + 0x0000000c)
#define HWIO_MSS_RELAY_MSG_BUFFER_03_RMSK                                  0xffffffff
#define HWIO_MSS_RELAY_MSG_BUFFER_03_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_BUFFER_03_ADDR, HWIO_MSS_RELAY_MSG_BUFFER_03_RMSK)
#define HWIO_MSS_RELAY_MSG_BUFFER_03_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_BUFFER_03_ADDR, m)
#define HWIO_MSS_RELAY_MSG_BUFFER_03_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_BUFFER_03_ADDR,v)
#define HWIO_MSS_RELAY_MSG_BUFFER_03_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_BUFFER_03_ADDR,m,v,HWIO_MSS_RELAY_MSG_BUFFER_03_IN)
#define HWIO_MSS_RELAY_MSG_BUFFER_03_MSS_RELAY_MSG_BUFFER_03_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_BUFFER_03_MSS_RELAY_MSG_BUFFER_03_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_BUFFER_04_ADDR                                  (MSS_RMB_REG_BASE      + 0x00000010)
#define HWIO_MSS_RELAY_MSG_BUFFER_04_RMSK                                  0xffffffff
#define HWIO_MSS_RELAY_MSG_BUFFER_04_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_BUFFER_04_ADDR, HWIO_MSS_RELAY_MSG_BUFFER_04_RMSK)
#define HWIO_MSS_RELAY_MSG_BUFFER_04_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_BUFFER_04_ADDR, m)
#define HWIO_MSS_RELAY_MSG_BUFFER_04_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_BUFFER_04_ADDR,v)
#define HWIO_MSS_RELAY_MSG_BUFFER_04_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_BUFFER_04_ADDR,m,v,HWIO_MSS_RELAY_MSG_BUFFER_04_IN)
#define HWIO_MSS_RELAY_MSG_BUFFER_04_MSS_RELAY_MSG_BUFFER_04_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_BUFFER_04_MSS_RELAY_MSG_BUFFER_04_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_BUFFER_05_ADDR                                  (MSS_RMB_REG_BASE      + 0x00000014)
#define HWIO_MSS_RELAY_MSG_BUFFER_05_RMSK                                  0xffffffff
#define HWIO_MSS_RELAY_MSG_BUFFER_05_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_BUFFER_05_ADDR, HWIO_MSS_RELAY_MSG_BUFFER_05_RMSK)
#define HWIO_MSS_RELAY_MSG_BUFFER_05_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_BUFFER_05_ADDR, m)
#define HWIO_MSS_RELAY_MSG_BUFFER_05_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_BUFFER_05_ADDR,v)
#define HWIO_MSS_RELAY_MSG_BUFFER_05_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_BUFFER_05_ADDR,m,v,HWIO_MSS_RELAY_MSG_BUFFER_05_IN)
#define HWIO_MSS_RELAY_MSG_BUFFER_05_MSS_RELAY_MSG_BUFFER_05_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_BUFFER_05_MSS_RELAY_MSG_BUFFER_05_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_BUFFER_06_ADDR                                  (MSS_RMB_REG_BASE      + 0x00000018)
#define HWIO_MSS_RELAY_MSG_BUFFER_06_RMSK                                  0xffffffff
#define HWIO_MSS_RELAY_MSG_BUFFER_06_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_BUFFER_06_ADDR, HWIO_MSS_RELAY_MSG_BUFFER_06_RMSK)
#define HWIO_MSS_RELAY_MSG_BUFFER_06_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_BUFFER_06_ADDR, m)
#define HWIO_MSS_RELAY_MSG_BUFFER_06_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_BUFFER_06_ADDR,v)
#define HWIO_MSS_RELAY_MSG_BUFFER_06_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_BUFFER_06_ADDR,m,v,HWIO_MSS_RELAY_MSG_BUFFER_06_IN)
#define HWIO_MSS_RELAY_MSG_BUFFER_06_MSS_RELAY_MSG_BUFFER_06_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_BUFFER_06_MSS_RELAY_MSG_BUFFER_06_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_BUFFER_07_ADDR                                  (MSS_RMB_REG_BASE      + 0x0000001c)
#define HWIO_MSS_RELAY_MSG_BUFFER_07_RMSK                                  0xffffffff
#define HWIO_MSS_RELAY_MSG_BUFFER_07_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_BUFFER_07_ADDR, HWIO_MSS_RELAY_MSG_BUFFER_07_RMSK)
#define HWIO_MSS_RELAY_MSG_BUFFER_07_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_BUFFER_07_ADDR, m)
#define HWIO_MSS_RELAY_MSG_BUFFER_07_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_BUFFER_07_ADDR,v)
#define HWIO_MSS_RELAY_MSG_BUFFER_07_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_BUFFER_07_ADDR,m,v,HWIO_MSS_RELAY_MSG_BUFFER_07_IN)
#define HWIO_MSS_RELAY_MSG_BUFFER_07_MSS_RELAY_MSG_BUFFER_07_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_BUFFER_07_MSS_RELAY_MSG_BUFFER_07_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_BUFFER_08_ADDR                                  (MSS_RMB_REG_BASE      + 0x00000020)
#define HWIO_MSS_RELAY_MSG_BUFFER_08_RMSK                                  0xffffffff
#define HWIO_MSS_RELAY_MSG_BUFFER_08_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_BUFFER_08_ADDR, HWIO_MSS_RELAY_MSG_BUFFER_08_RMSK)
#define HWIO_MSS_RELAY_MSG_BUFFER_08_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_BUFFER_08_ADDR, m)
#define HWIO_MSS_RELAY_MSG_BUFFER_08_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_BUFFER_08_ADDR,v)
#define HWIO_MSS_RELAY_MSG_BUFFER_08_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_BUFFER_08_ADDR,m,v,HWIO_MSS_RELAY_MSG_BUFFER_08_IN)
#define HWIO_MSS_RELAY_MSG_BUFFER_08_MSS_RELAY_MSG_BUFFER_08_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_BUFFER_08_MSS_RELAY_MSG_BUFFER_08_SHFT                 0x0

/*----------------------------------------------------------------------------
 * MODULE: MSS_PERPH
 *--------------------------------------------------------------------------*/

#define MSS_PERPH_REG_BASE                                                (MSS_TOP_BASE      + 0x00180000)

#define HWIO_MSS_ENABLE_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00000000)
#define HWIO_MSS_ENABLE_RMSK                                                    0xff
#define HWIO_MSS_ENABLE_IN          \
        in_dword_masked(HWIO_MSS_ENABLE_ADDR, HWIO_MSS_ENABLE_RMSK)
#define HWIO_MSS_ENABLE_INM(m)      \
        in_dword_masked(HWIO_MSS_ENABLE_ADDR, m)
#define HWIO_MSS_ENABLE_OUT(v)      \
        out_dword(HWIO_MSS_ENABLE_ADDR,v)
#define HWIO_MSS_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_ENABLE_ADDR,m,v,HWIO_MSS_ENABLE_IN)
#define HWIO_MSS_ENABLE_COXM_BMSK                                               0x80
#define HWIO_MSS_ENABLE_COXM_SHFT                                                0x7
#define HWIO_MSS_ENABLE_RBCPR_BMSK                                              0x40
#define HWIO_MSS_ENABLE_RBCPR_SHFT                                               0x6
#define HWIO_MSS_ENABLE_CRYPTO5_BMSK                                            0x20
#define HWIO_MSS_ENABLE_CRYPTO5_SHFT                                             0x5
#define HWIO_MSS_ENABLE_UIM1_BMSK                                               0x10
#define HWIO_MSS_ENABLE_UIM1_SHFT                                                0x4
#define HWIO_MSS_ENABLE_UIM0_BMSK                                                0x8
#define HWIO_MSS_ENABLE_UIM0_SHFT                                                0x3
#define HWIO_MSS_ENABLE_NAV_BMSK                                                 0x4
#define HWIO_MSS_ENABLE_NAV_SHFT                                                 0x2
#define HWIO_MSS_ENABLE_Q6_BMSK                                                  0x2
#define HWIO_MSS_ENABLE_Q6_SHFT                                                  0x1
#define HWIO_MSS_ENABLE_MODEM_BMSK                                               0x1
#define HWIO_MSS_ENABLE_MODEM_SHFT                                               0x0

#define HWIO_MSS_CLAMP_MEM_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00000004)
#define HWIO_MSS_CLAMP_MEM_RMSK                                                  0x7
#define HWIO_MSS_CLAMP_MEM_IN          \
        in_dword_masked(HWIO_MSS_CLAMP_MEM_ADDR, HWIO_MSS_CLAMP_MEM_RMSK)
#define HWIO_MSS_CLAMP_MEM_INM(m)      \
        in_dword_masked(HWIO_MSS_CLAMP_MEM_ADDR, m)
#define HWIO_MSS_CLAMP_MEM_OUT(v)      \
        out_dword(HWIO_MSS_CLAMP_MEM_ADDR,v)
#define HWIO_MSS_CLAMP_MEM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLAMP_MEM_ADDR,m,v,HWIO_MSS_CLAMP_MEM_IN)
#define HWIO_MSS_CLAMP_MEM_SPARE_BMSK                                            0x4
#define HWIO_MSS_CLAMP_MEM_SPARE_SHFT                                            0x2
#define HWIO_MSS_CLAMP_MEM_UNCLAMP_ALL_BMSK                                      0x2
#define HWIO_MSS_CLAMP_MEM_UNCLAMP_ALL_SHFT                                      0x1
#define HWIO_MSS_CLAMP_MEM_HM_CLAMP_BMSK                                         0x1
#define HWIO_MSS_CLAMP_MEM_HM_CLAMP_SHFT                                         0x0

#define HWIO_MSS_CLAMP_IO_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00000008)
#define HWIO_MSS_CLAMP_IO_RMSK                                                  0xff
#define HWIO_MSS_CLAMP_IO_IN          \
        in_dword_masked(HWIO_MSS_CLAMP_IO_ADDR, HWIO_MSS_CLAMP_IO_RMSK)
#define HWIO_MSS_CLAMP_IO_INM(m)      \
        in_dword_masked(HWIO_MSS_CLAMP_IO_ADDR, m)
#define HWIO_MSS_CLAMP_IO_OUT(v)      \
        out_dword(HWIO_MSS_CLAMP_IO_ADDR,v)
#define HWIO_MSS_CLAMP_IO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLAMP_IO_ADDR,m,v,HWIO_MSS_CLAMP_IO_IN)
#define HWIO_MSS_CLAMP_IO_SPARE_7_BMSK                                          0x80
#define HWIO_MSS_CLAMP_IO_SPARE_7_SHFT                                           0x7
#define HWIO_MSS_CLAMP_IO_UNCLAMP_ALL_BMSK                                      0x40
#define HWIO_MSS_CLAMP_IO_UNCLAMP_ALL_SHFT                                       0x6
#define HWIO_MSS_CLAMP_IO_RBCPR_BMSK                                            0x20
#define HWIO_MSS_CLAMP_IO_RBCPR_SHFT                                             0x5
#define HWIO_MSS_CLAMP_IO_BBRX_ADC_BMSK                                         0x10
#define HWIO_MSS_CLAMP_IO_BBRX_ADC_SHFT                                          0x4
#define HWIO_MSS_CLAMP_IO_GNSS_DAC_BMSK                                          0x8
#define HWIO_MSS_CLAMP_IO_GNSS_DAC_SHFT                                          0x3
#define HWIO_MSS_CLAMP_IO_COM_COMP_BMSK                                          0x4
#define HWIO_MSS_CLAMP_IO_COM_COMP_SHFT                                          0x2
#define HWIO_MSS_CLAMP_IO_NC_HM_BMSK                                             0x2
#define HWIO_MSS_CLAMP_IO_NC_HM_SHFT                                             0x1
#define HWIO_MSS_CLAMP_IO_MODEM_BMSK                                             0x1
#define HWIO_MSS_CLAMP_IO_MODEM_SHFT                                             0x0

#define HWIO_MSS_BUS_AHB2AHB_CFG_ADDR                                     (MSS_PERPH_REG_BASE      + 0x0000000c)
#define HWIO_MSS_BUS_AHB2AHB_CFG_RMSK                                            0x3
#define HWIO_MSS_BUS_AHB2AHB_CFG_IN          \
        in_dword_masked(HWIO_MSS_BUS_AHB2AHB_CFG_ADDR, HWIO_MSS_BUS_AHB2AHB_CFG_RMSK)
#define HWIO_MSS_BUS_AHB2AHB_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_AHB2AHB_CFG_ADDR, m)
#define HWIO_MSS_BUS_AHB2AHB_CFG_OUT(v)      \
        out_dword(HWIO_MSS_BUS_AHB2AHB_CFG_ADDR,v)
#define HWIO_MSS_BUS_AHB2AHB_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_AHB2AHB_CFG_ADDR,m,v,HWIO_MSS_BUS_AHB2AHB_CFG_IN)
#define HWIO_MSS_BUS_AHB2AHB_CFG_POST_EN_AHB2AHB_NAV_BMSK                        0x2
#define HWIO_MSS_BUS_AHB2AHB_CFG_POST_EN_AHB2AHB_NAV_SHFT                        0x1
#define HWIO_MSS_BUS_AHB2AHB_CFG_POST_EN_AHB2AHB_BMSK                            0x1
#define HWIO_MSS_BUS_AHB2AHB_CFG_POST_EN_AHB2AHB_SHFT                            0x0

#define HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR                                    (MSS_PERPH_REG_BASE      + 0x00000010)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_RMSK                                           0xf
#define HWIO_MSS_BUS_MAXI2AXI_CFG_IN          \
        in_dword_masked(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR, HWIO_MSS_BUS_MAXI2AXI_CFG_RMSK)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR, m)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_OUT(v)      \
        out_dword(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR,v)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR,m,v,HWIO_MSS_BUS_MAXI2AXI_CFG_IN)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_NAV_AREQPRIORITY_BMSK                    0xc
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_NAV_AREQPRIORITY_SHFT                    0x2
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_CRYPTO_AREQPRIORITY_BMSK                 0x3
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_CRYPTO_AREQPRIORITY_SHFT                 0x0

#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR                                 (MSS_PERPH_REG_BASE      + 0x00000014)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_RMSK                                       0xff
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_IN          \
        in_dword_masked(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR, HWIO_MSS_CUSTOM_MEM_ARRSTBYN_RMSK)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_INM(m)      \
        in_dword_masked(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR, m)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_OUT(v)      \
        out_dword(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR,v)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR,m,v,HWIO_MSS_CUSTOM_MEM_ARRSTBYN_IN)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_CTL_BMSK                                   0xff
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_CTL_SHFT                                    0x0

#define HWIO_MSS_ANALOG_IP_TEST_CTL_ADDR                                  (MSS_PERPH_REG_BASE      + 0x00000018)
#define HWIO_MSS_ANALOG_IP_TEST_CTL_RMSK                                         0x3
#define HWIO_MSS_ANALOG_IP_TEST_CTL_IN          \
        in_dword_masked(HWIO_MSS_ANALOG_IP_TEST_CTL_ADDR, HWIO_MSS_ANALOG_IP_TEST_CTL_RMSK)
#define HWIO_MSS_ANALOG_IP_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_ANALOG_IP_TEST_CTL_ADDR, m)
#define HWIO_MSS_ANALOG_IP_TEST_CTL_OUT(v)      \
        out_dword(HWIO_MSS_ANALOG_IP_TEST_CTL_ADDR,v)
#define HWIO_MSS_ANALOG_IP_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_ANALOG_IP_TEST_CTL_ADDR,m,v,HWIO_MSS_ANALOG_IP_TEST_CTL_IN)
#define HWIO_MSS_ANALOG_IP_TEST_CTL_EXTERNAL_IQDATA_EN_BMSK                      0x2
#define HWIO_MSS_ANALOG_IP_TEST_CTL_EXTERNAL_IQDATA_EN_SHFT                      0x1
#define HWIO_MSS_ANALOG_IP_TEST_CTL_EXTERNAL_Y1Y2_EN_BMSK                        0x1
#define HWIO_MSS_ANALOG_IP_TEST_CTL_EXTERNAL_Y1Y2_EN_SHFT                        0x0

#define HWIO_MSS_ATB_ID_ADDR                                              (MSS_PERPH_REG_BASE      + 0x0000001c)
#define HWIO_MSS_ATB_ID_RMSK                                                    0x7f
#define HWIO_MSS_ATB_ID_IN          \
        in_dword_masked(HWIO_MSS_ATB_ID_ADDR, HWIO_MSS_ATB_ID_RMSK)
#define HWIO_MSS_ATB_ID_INM(m)      \
        in_dword_masked(HWIO_MSS_ATB_ID_ADDR, m)
#define HWIO_MSS_ATB_ID_OUT(v)      \
        out_dword(HWIO_MSS_ATB_ID_ADDR,v)
#define HWIO_MSS_ATB_ID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_ATB_ID_ADDR,m,v,HWIO_MSS_ATB_ID_IN)
#define HWIO_MSS_ATB_ID_ATB_ID_BMSK                                             0x7f
#define HWIO_MSS_ATB_ID_ATB_ID_SHFT                                              0x0

#define HWIO_MSS_DBG_BUS_CTL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00000020)
#define HWIO_MSS_DBG_BUS_CTL_RMSK                                            0x7ffff
#define HWIO_MSS_DBG_BUS_CTL_IN          \
        in_dword_masked(HWIO_MSS_DBG_BUS_CTL_ADDR, HWIO_MSS_DBG_BUS_CTL_RMSK)
#define HWIO_MSS_DBG_BUS_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DBG_BUS_CTL_ADDR, m)
#define HWIO_MSS_DBG_BUS_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DBG_BUS_CTL_ADDR,v)
#define HWIO_MSS_DBG_BUS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DBG_BUS_CTL_ADDR,m,v,HWIO_MSS_DBG_BUS_CTL_IN)
#define HWIO_MSS_DBG_BUS_CTL_BRIC_AXI2AXI_NAV_SEL_BMSK                       0x70000
#define HWIO_MSS_DBG_BUS_CTL_BRIC_AXI2AXI_NAV_SEL_SHFT                          0x10
#define HWIO_MSS_DBG_BUS_CTL_AHB2AXI_NAV_SEL_BMSK                             0xe000
#define HWIO_MSS_DBG_BUS_CTL_AHB2AXI_NAV_SEL_SHFT                                0xd
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_NAV_CONFIG_SEL_BMSK                      0x1800
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_NAV_CONFIG_SEL_SHFT                         0xb
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_SEL_BMSK                                  0x600
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_SEL_SHFT                                    0x9
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_BUS_TOP_SEL_BMSK                          0x1e0
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_BUS_TOP_SEL_SHFT                            0x5
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_BUS_NC_HM_SEL_BMSK                         0x1c
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_BUS_NC_HM_SEL_SHFT                          0x2
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_GPIO_ATB_SEL_BMSK                           0x3
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_GPIO_ATB_SEL_SHFT                           0x0

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR                               (MSS_PERPH_REG_BASE      + 0x00000024)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_RMSK                                      0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_IN          \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR, HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_RMSK)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_INM(m)      \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR, m)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_OUT(v)      \
        out_dword(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR,v)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR,m,v,HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_IN)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_EN_BMSK                                   0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_EN_SHFT                                   0x0

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR                           (MSS_PERPH_REG_BASE      + 0x00000028)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_RMSK                                  0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_IN          \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR, HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_RMSK)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR, m)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_STATUS_BMSK                           0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_STATUS_SHFT                           0x0

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_ADDR                              (MSS_PERPH_REG_BASE      + 0x0000002c)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_RMSK                                     0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_OUT(v)      \
        out_dword(HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_ADDR,v)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_CMD_BMSK                                 0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_CMD_SHFT                                 0x0

#define HWIO_MSS_BUS_CTL_CFG_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00000030)
#define HWIO_MSS_BUS_CTL_CFG_RMSK                                                0x1
#define HWIO_MSS_BUS_CTL_CFG_IN          \
        in_dword_masked(HWIO_MSS_BUS_CTL_CFG_ADDR, HWIO_MSS_BUS_CTL_CFG_RMSK)
#define HWIO_MSS_BUS_CTL_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CTL_CFG_ADDR, m)
#define HWIO_MSS_BUS_CTL_CFG_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CTL_CFG_ADDR,v)
#define HWIO_MSS_BUS_CTL_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CTL_CFG_ADDR,m,v,HWIO_MSS_BUS_CTL_CFG_IN)
#define HWIO_MSS_BUS_CTL_CFG_Q6_FORCE_UNBUFFERED_BMSK                            0x1
#define HWIO_MSS_BUS_CTL_CFG_Q6_FORCE_UNBUFFERED_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW0_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000034)
#define HWIO_MSS_RELAY_MSG_SHADOW0_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW0_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR, HWIO_MSS_RELAY_MSG_SHADOW0_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW0_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW0_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW0_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW0_RELAY_MSG_SHADOW0_BMSK                 0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW0_RELAY_MSG_SHADOW0_SHFT                        0x0

#define HWIO_MSS_RELAY_MSG_SHADOW1_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000038)
#define HWIO_MSS_RELAY_MSG_SHADOW1_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW1_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR, HWIO_MSS_RELAY_MSG_SHADOW1_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW1_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW1_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW1_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW1_RELAY_MSG_SHADOW1_BMSK                 0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW1_RELAY_MSG_SHADOW1_SHFT                        0x0

#define HWIO_MSS_RELAY_MSG_SHADOW2_ADDR                                   (MSS_PERPH_REG_BASE      + 0x0000003c)
#define HWIO_MSS_RELAY_MSG_SHADOW2_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW2_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR, HWIO_MSS_RELAY_MSG_SHADOW2_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW2_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW2_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW2_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW2_RELAY_MSG_SHADOW2_BMSK                 0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW2_RELAY_MSG_SHADOW2_SHFT                        0x0

#define HWIO_MSS_RELAY_MSG_SHADOW3_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000040)
#define HWIO_MSS_RELAY_MSG_SHADOW3_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW3_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR, HWIO_MSS_RELAY_MSG_SHADOW3_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW3_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW3_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW3_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW3_RELAY_MSG_SHADOW3_BMSK                 0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW3_RELAY_MSG_SHADOW3_SHFT                        0x0

#define HWIO_MSS_RELAY_MSG_SHADOW4_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000044)
#define HWIO_MSS_RELAY_MSG_SHADOW4_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW4_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR, HWIO_MSS_RELAY_MSG_SHADOW4_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW4_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW4_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW4_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW4_RELAY_MSG_SHADOW4_BMSK                 0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW4_RELAY_MSG_SHADOW4_SHFT                        0x0

#define HWIO_MSS_RELAY_MSG_SHADOW5_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000048)
#define HWIO_MSS_RELAY_MSG_SHADOW5_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW5_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR, HWIO_MSS_RELAY_MSG_SHADOW5_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW5_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW5_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW5_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW5_RELAY_MSG_SHADOW5_BMSK                 0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW5_RELAY_MSG_SHADOW5_SHFT                        0x0

#define HWIO_MSS_RELAY_MSG_SHADOW6_ADDR                                   (MSS_PERPH_REG_BASE      + 0x0000004c)
#define HWIO_MSS_RELAY_MSG_SHADOW6_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW6_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR, HWIO_MSS_RELAY_MSG_SHADOW6_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW6_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW6_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW6_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW6_RELAY_MSG_SHADOW6_BMSK                 0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW6_RELAY_MSG_SHADOW6_SHFT                        0x0

#define HWIO_MSS_RELAY_MSG_SHADOW7_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000050)
#define HWIO_MSS_RELAY_MSG_SHADOW7_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW7_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR, HWIO_MSS_RELAY_MSG_SHADOW7_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW7_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW7_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW7_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW7_RELAY_MSG_SHADOW7_BMSK                 0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW7_RELAY_MSG_SHADOW7_SHFT                        0x0

#define HWIO_MSS_RELAY_MSG_SHADOW8_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000054)
#define HWIO_MSS_RELAY_MSG_SHADOW8_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW8_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR, HWIO_MSS_RELAY_MSG_SHADOW8_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW8_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW8_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW8_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW8_RELAY_MSG_SHADOW8_BMSK                 0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW8_RELAY_MSG_SHADOW8_SHFT                        0x0

#define HWIO_MSS_MSA_ADDR                                                 (MSS_PERPH_REG_BASE      + 0x00000058)
#define HWIO_MSS_MSA_RMSK                                                        0x3
#define HWIO_MSS_MSA_IN          \
        in_dword_masked(HWIO_MSS_MSA_ADDR, HWIO_MSS_MSA_RMSK)
#define HWIO_MSS_MSA_INM(m)      \
        in_dword_masked(HWIO_MSS_MSA_ADDR, m)
#define HWIO_MSS_MSA_OUT(v)      \
        out_dword(HWIO_MSS_MSA_ADDR,v)
#define HWIO_MSS_MSA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MSA_ADDR,m,v,HWIO_MSS_MSA_IN)
#define HWIO_MSS_MSA_MBA_OK_BMSK                                                 0x2
#define HWIO_MSS_MSA_MBA_OK_SHFT                                                 0x1
#define HWIO_MSS_MSA_CONFIG_LOCK_BMSK                                            0x1
#define HWIO_MSS_MSA_CONFIG_LOCK_SHFT                                            0x0

#define HWIO_MSS_HW_VERSION_ADDR                                          (MSS_PERPH_REG_BASE      + 0x0000005c)
#define HWIO_MSS_HW_VERSION_RMSK                                          0xffffffff
#define HWIO_MSS_HW_VERSION_IN          \
        in_dword_masked(HWIO_MSS_HW_VERSION_ADDR, HWIO_MSS_HW_VERSION_RMSK)
#define HWIO_MSS_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_MSS_HW_VERSION_ADDR, m)
#define HWIO_MSS_HW_VERSION_MAJOR_BMSK                                    0xf0000000
#define HWIO_MSS_HW_VERSION_MAJOR_SHFT                                          0x1c
#define HWIO_MSS_HW_VERSION_MINOR_BMSK                                     0xfff0000
#define HWIO_MSS_HW_VERSION_MINOR_SHFT                                          0x10
#define HWIO_MSS_HW_VERSION_STEP_BMSK                                         0xffff
#define HWIO_MSS_HW_VERSION_STEP_SHFT                                            0x0

#define HWIO_MSS_DIME_MEM_SLP_CNTL_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000060)
#define HWIO_MSS_DIME_MEM_SLP_CNTL_RMSK                                    0x302030f
#define HWIO_MSS_DIME_MEM_SLP_CNTL_IN          \
        in_dword_masked(HWIO_MSS_DIME_MEM_SLP_CNTL_ADDR, HWIO_MSS_DIME_MEM_SLP_CNTL_RMSK)
#define HWIO_MSS_DIME_MEM_SLP_CNTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DIME_MEM_SLP_CNTL_ADDR, m)
#define HWIO_MSS_DIME_MEM_SLP_CNTL_OUT(v)      \
        out_dword(HWIO_MSS_DIME_MEM_SLP_CNTL_ADDR,v)
#define HWIO_MSS_DIME_MEM_SLP_CNTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DIME_MEM_SLP_CNTL_ADDR,m,v,HWIO_MSS_DIME_MEM_SLP_CNTL_IN)
#define HWIO_MSS_DIME_MEM_SLP_CNTL_MEM_SLP_SPM_BMSK                        0x2000000
#define HWIO_MSS_DIME_MEM_SLP_CNTL_MEM_SLP_SPM_SHFT                             0x19
#define HWIO_MSS_DIME_MEM_SLP_CNTL_MEM_SLP_NOW_BMSK                        0x1000000
#define HWIO_MSS_DIME_MEM_SLP_CNTL_MEM_SLP_NOW_SHFT                             0x18
#define HWIO_MSS_DIME_MEM_SLP_CNTL_ALL_NR_SLP_NRET_N_BMSK                    0x20000
#define HWIO_MSS_DIME_MEM_SLP_CNTL_ALL_NR_SLP_NRET_N_SHFT                       0x11
#define HWIO_MSS_DIME_MEM_SLP_CNTL_CCS_SLP_NRET_N_BMSK                         0x200
#define HWIO_MSS_DIME_MEM_SLP_CNTL_CCS_SLP_NRET_N_SHFT                           0x9
#define HWIO_MSS_DIME_MEM_SLP_CNTL_CCS_SLP_RET_N_BMSK                          0x100
#define HWIO_MSS_DIME_MEM_SLP_CNTL_CCS_SLP_RET_N_SHFT                            0x8
#define HWIO_MSS_DIME_MEM_SLP_CNTL_VPE1_SLP_NRET_N_BMSK                          0x8
#define HWIO_MSS_DIME_MEM_SLP_CNTL_VPE1_SLP_NRET_N_SHFT                          0x3
#define HWIO_MSS_DIME_MEM_SLP_CNTL_VPE1_SLP_RET_N_BMSK                           0x4
#define HWIO_MSS_DIME_MEM_SLP_CNTL_VPE1_SLP_RET_N_SHFT                           0x2
#define HWIO_MSS_DIME_MEM_SLP_CNTL_VPE0_SLP_NRET_N_BMSK                          0x2
#define HWIO_MSS_DIME_MEM_SLP_CNTL_VPE0_SLP_NRET_N_SHFT                          0x1
#define HWIO_MSS_DIME_MEM_SLP_CNTL_VPE0_SLP_RET_N_BMSK                           0x1
#define HWIO_MSS_DIME_MEM_SLP_CNTL_VPE0_SLP_RET_N_SHFT                           0x0

#define HWIO_MSS_CLOCK_SPDM_MON_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00000064)
#define HWIO_MSS_CLOCK_SPDM_MON_RMSK                                             0x3
#define HWIO_MSS_CLOCK_SPDM_MON_IN          \
        in_dword_masked(HWIO_MSS_CLOCK_SPDM_MON_ADDR, HWIO_MSS_CLOCK_SPDM_MON_RMSK)
#define HWIO_MSS_CLOCK_SPDM_MON_INM(m)      \
        in_dword_masked(HWIO_MSS_CLOCK_SPDM_MON_ADDR, m)
#define HWIO_MSS_CLOCK_SPDM_MON_OUT(v)      \
        out_dword(HWIO_MSS_CLOCK_SPDM_MON_ADDR,v)
#define HWIO_MSS_CLOCK_SPDM_MON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLOCK_SPDM_MON_ADDR,m,v,HWIO_MSS_CLOCK_SPDM_MON_IN)
#define HWIO_MSS_CLOCK_SPDM_MON_Q6_MON_CLKEN_BMSK                                0x2
#define HWIO_MSS_CLOCK_SPDM_MON_Q6_MON_CLKEN_SHFT                                0x1
#define HWIO_MSS_CLOCK_SPDM_MON_BUS_MON_CLKEN_BMSK                               0x1
#define HWIO_MSS_CLOCK_SPDM_MON_BUS_MON_CLKEN_SHFT                               0x0

#define HWIO_MSS_BBRX0_MUX_SEL_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00000068)
#define HWIO_MSS_BBRX0_MUX_SEL_RMSK                                              0x3
#define HWIO_MSS_BBRX0_MUX_SEL_IN          \
        in_dword_masked(HWIO_MSS_BBRX0_MUX_SEL_ADDR, HWIO_MSS_BBRX0_MUX_SEL_RMSK)
#define HWIO_MSS_BBRX0_MUX_SEL_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX0_MUX_SEL_ADDR, m)
#define HWIO_MSS_BBRX0_MUX_SEL_OUT(v)      \
        out_dword(HWIO_MSS_BBRX0_MUX_SEL_ADDR,v)
#define HWIO_MSS_BBRX0_MUX_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX0_MUX_SEL_ADDR,m,v,HWIO_MSS_BBRX0_MUX_SEL_IN)
#define HWIO_MSS_BBRX0_MUX_SEL_SECOND_MUX_SEL_BMSK                               0x2
#define HWIO_MSS_BBRX0_MUX_SEL_SECOND_MUX_SEL_SHFT                               0x1
#define HWIO_MSS_BBRX0_MUX_SEL_FIRST_MUX_SEL_BMSK                                0x1
#define HWIO_MSS_BBRX0_MUX_SEL_FIRST_MUX_SEL_SHFT                                0x0

#define HWIO_MSS_BBRX1_MUX_SEL_ADDR                                       (MSS_PERPH_REG_BASE      + 0x0000006c)
#define HWIO_MSS_BBRX1_MUX_SEL_RMSK                                              0x3
#define HWIO_MSS_BBRX1_MUX_SEL_IN          \
        in_dword_masked(HWIO_MSS_BBRX1_MUX_SEL_ADDR, HWIO_MSS_BBRX1_MUX_SEL_RMSK)
#define HWIO_MSS_BBRX1_MUX_SEL_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX1_MUX_SEL_ADDR, m)
#define HWIO_MSS_BBRX1_MUX_SEL_OUT(v)      \
        out_dword(HWIO_MSS_BBRX1_MUX_SEL_ADDR,v)
#define HWIO_MSS_BBRX1_MUX_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX1_MUX_SEL_ADDR,m,v,HWIO_MSS_BBRX1_MUX_SEL_IN)
#define HWIO_MSS_BBRX1_MUX_SEL_SECOND_MUX_SEL_BMSK                               0x2
#define HWIO_MSS_BBRX1_MUX_SEL_SECOND_MUX_SEL_SHFT                               0x1
#define HWIO_MSS_BBRX1_MUX_SEL_FIRST_MUX_SEL_BMSK                                0x1
#define HWIO_MSS_BBRX1_MUX_SEL_FIRST_MUX_SEL_SHFT                                0x0

#define HWIO_MSS_BBRX2_MUX_SEL_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00000070)
#define HWIO_MSS_BBRX2_MUX_SEL_RMSK                                              0x3
#define HWIO_MSS_BBRX2_MUX_SEL_IN          \
        in_dword_masked(HWIO_MSS_BBRX2_MUX_SEL_ADDR, HWIO_MSS_BBRX2_MUX_SEL_RMSK)
#define HWIO_MSS_BBRX2_MUX_SEL_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX2_MUX_SEL_ADDR, m)
#define HWIO_MSS_BBRX2_MUX_SEL_OUT(v)      \
        out_dword(HWIO_MSS_BBRX2_MUX_SEL_ADDR,v)
#define HWIO_MSS_BBRX2_MUX_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX2_MUX_SEL_ADDR,m,v,HWIO_MSS_BBRX2_MUX_SEL_IN)
#define HWIO_MSS_BBRX2_MUX_SEL_SECOND_MUX_SEL_BMSK                               0x2
#define HWIO_MSS_BBRX2_MUX_SEL_SECOND_MUX_SEL_SHFT                               0x1
#define HWIO_MSS_BBRX2_MUX_SEL_FIRST_MUX_SEL_BMSK                                0x1
#define HWIO_MSS_BBRX2_MUX_SEL_FIRST_MUX_SEL_SHFT                                0x0

#define HWIO_MSS_BBRX3_MUX_SEL_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00000074)
#define HWIO_MSS_BBRX3_MUX_SEL_RMSK                                              0x3
#define HWIO_MSS_BBRX3_MUX_SEL_IN          \
        in_dword_masked(HWIO_MSS_BBRX3_MUX_SEL_ADDR, HWIO_MSS_BBRX3_MUX_SEL_RMSK)
#define HWIO_MSS_BBRX3_MUX_SEL_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX3_MUX_SEL_ADDR, m)
#define HWIO_MSS_BBRX3_MUX_SEL_OUT(v)      \
        out_dword(HWIO_MSS_BBRX3_MUX_SEL_ADDR,v)
#define HWIO_MSS_BBRX3_MUX_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX3_MUX_SEL_ADDR,m,v,HWIO_MSS_BBRX3_MUX_SEL_IN)
#define HWIO_MSS_BBRX3_MUX_SEL_SECOND_MUX_SEL_BMSK                               0x2
#define HWIO_MSS_BBRX3_MUX_SEL_SECOND_MUX_SEL_SHFT                               0x1
#define HWIO_MSS_BBRX3_MUX_SEL_FIRST_MUX_SEL_BMSK                                0x1
#define HWIO_MSS_BBRX3_MUX_SEL_FIRST_MUX_SEL_SHFT                                0x0

#define HWIO_MSS_DEBUG_CLOCK_CTL_ADDR                                     (MSS_PERPH_REG_BASE      + 0x00000078)
#define HWIO_MSS_DEBUG_CLOCK_CTL_RMSK                                           0x3f
#define HWIO_MSS_DEBUG_CLOCK_CTL_IN          \
        in_dword_masked(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR, HWIO_MSS_DEBUG_CLOCK_CTL_RMSK)
#define HWIO_MSS_DEBUG_CLOCK_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR, m)
#define HWIO_MSS_DEBUG_CLOCK_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR,v)
#define HWIO_MSS_DEBUG_CLOCK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR,m,v,HWIO_MSS_DEBUG_CLOCK_CTL_IN)
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_MUX_SEL_BMSK                               0x20
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_MUX_SEL_SHFT                                0x5
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL5_MUX_SEL_BMSK                        0x10
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL5_MUX_SEL_SHFT                         0x4
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL4_MUX_SEL_BMSK                         0x8
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL4_MUX_SEL_SHFT                         0x3
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL3_MUX_SEL_BMSK                         0x4
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL3_MUX_SEL_SHFT                         0x2
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL2_MUX_SEL_BMSK                         0x2
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL2_MUX_SEL_SHFT                         0x1
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL1_MUX_SEL_BMSK                         0x1
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL1_MUX_SEL_SHFT                         0x0

#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_ADDR                              (MSS_PERPH_REG_BASE      + 0x0000007c)
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_RMSK                                     0x3
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_IN          \
        in_dword_masked(HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_ADDR, HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_RMSK)
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_ADDR, m)
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_OUT(v)      \
        out_dword(HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_ADDR,v)
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_ADDR,m,v,HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_IN)
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_PLLTEST_BMSK                             0x2
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_PLLTEST_SHFT                             0x1
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_EXT_CLOCK_MUX_SEL_BMSK                   0x1
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_EXT_CLOCK_MUX_SEL_SHFT                   0x0

#define HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_ADDR                              (MSS_PERPH_REG_BASE      + 0x00000080)
#define HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_RMSK                                     0x1
#define HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_IN          \
        in_dword_masked(HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_ADDR, HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_RMSK)
#define HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_INM(m)      \
        in_dword_masked(HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_ADDR, m)
#define HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_OUT(v)      \
        out_dword(HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_ADDR,v)
#define HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_ADDR,m,v,HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_IN)
#define HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_ENABLE_BMSK                              0x1
#define HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_ENABLE_SHFT                              0x0

#define HWIO_MSS_GNSSADC_BIST_CONFIG_ADDR                                 (MSS_PERPH_REG_BASE      + 0x00000084)
#define HWIO_MSS_GNSSADC_BIST_CONFIG_RMSK                                       0x7f
#define HWIO_MSS_GNSSADC_BIST_CONFIG_IN          \
        in_dword_masked(HWIO_MSS_GNSSADC_BIST_CONFIG_ADDR, HWIO_MSS_GNSSADC_BIST_CONFIG_RMSK)
#define HWIO_MSS_GNSSADC_BIST_CONFIG_INM(m)      \
        in_dword_masked(HWIO_MSS_GNSSADC_BIST_CONFIG_ADDR, m)
#define HWIO_MSS_GNSSADC_BIST_CONFIG_OUT(v)      \
        out_dword(HWIO_MSS_GNSSADC_BIST_CONFIG_ADDR,v)
#define HWIO_MSS_GNSSADC_BIST_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_GNSSADC_BIST_CONFIG_ADDR,m,v,HWIO_MSS_GNSSADC_BIST_CONFIG_IN)
#define HWIO_MSS_GNSSADC_BIST_CONFIG_GNSSADC_EXT_CLK_SEL_BMSK                   0x40
#define HWIO_MSS_GNSSADC_BIST_CONFIG_GNSSADC_EXT_CLK_SEL_SHFT                    0x6
#define HWIO_MSS_GNSSADC_BIST_CONFIG_GNSSADC_BIST_EN_BMSK                       0x20
#define HWIO_MSS_GNSSADC_BIST_CONFIG_GNSSADC_BIST_EN_SHFT                        0x5
#define HWIO_MSS_GNSSADC_BIST_CONFIG_HITSIDEAL_BMSK                             0x1f
#define HWIO_MSS_GNSSADC_BIST_CONFIG_HITSIDEAL_SHFT                              0x0

#define HWIO_MSS_GNSSADC_BIST_STATUS_1_ADDR                               (MSS_PERPH_REG_BASE      + 0x00000088)
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_RMSK                                0xfffffff
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_IN          \
        in_dword_masked(HWIO_MSS_GNSSADC_BIST_STATUS_1_ADDR, HWIO_MSS_GNSSADC_BIST_STATUS_1_RMSK)
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_INM(m)      \
        in_dword_masked(HWIO_MSS_GNSSADC_BIST_STATUS_1_ADDR, m)
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_GNSSADC_INL_I_MAX_BMSK              0xff80000
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_GNSSADC_INL_I_MAX_SHFT                   0x13
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_GNSSADC_INL_I_MIN_BMSK                0x7fc00
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_GNSSADC_INL_I_MIN_SHFT                    0xa
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_GNSSADC_DNL_I_POS_BMSK                  0x3e0
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_GNSSADC_DNL_I_POS_SHFT                    0x5
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_GNSSADC_DNL_I_NEG_BMSK                   0x1f
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_GNSSADC_DNL_I_NEG_SHFT                    0x0

#define HWIO_MSS_GNSSADC_BIST_STATUS_2_ADDR                               (MSS_PERPH_REG_BASE      + 0x0000008c)
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_RMSK                               0xffffffff
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_IN          \
        in_dword_masked(HWIO_MSS_GNSSADC_BIST_STATUS_2_ADDR, HWIO_MSS_GNSSADC_BIST_STATUS_2_RMSK)
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_INM(m)      \
        in_dword_masked(HWIO_MSS_GNSSADC_BIST_STATUS_2_ADDR, m)
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_GNSSADC_DNL_Q_POS_BMSK             0xf8000000
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_GNSSADC_DNL_Q_POS_SHFT                   0x1b
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_GNSSADC_GAIN_I_BMSK                 0x7fc0000
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_GNSSADC_GAIN_I_SHFT                      0x12
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_GNSSADC_INL_Q_MAX_BMSK                0x3fe00
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_GNSSADC_INL_Q_MAX_SHFT                    0x9
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_GNSSADC_INL_Q_MIN_BMSK                  0x1ff
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_GNSSADC_INL_Q_MIN_SHFT                    0x0

#define HWIO_MSS_GNSSADC_BIST_STATUS_3_ADDR                               (MSS_PERPH_REG_BASE      + 0x00000090)
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_RMSK                               0xffffffff
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_IN          \
        in_dword_masked(HWIO_MSS_GNSSADC_BIST_STATUS_3_ADDR, HWIO_MSS_GNSSADC_BIST_STATUS_3_RMSK)
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_INM(m)      \
        in_dword_masked(HWIO_MSS_GNSSADC_BIST_STATUS_3_ADDR, m)
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_GNSSADC_DNL_Q_NEG_BMSK             0xf8000000
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_GNSSADC_DNL_Q_NEG_SHFT                   0x1b
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_GNSSADC_OFFSET_I_BMSK               0x7fc0000
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_GNSSADC_OFFSET_I_SHFT                    0x12
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_GNSSADC_GAIN_Q_BMSK                   0x3fe00
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_GNSSADC_GAIN_Q_SHFT                       0x9
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_GNSSADC_OFFSET_Q_BMSK                   0x1ff
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_GNSSADC_OFFSET_Q_SHFT                     0x0

#define HWIO_MSS_BBRX_CTL_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00000094)
#define HWIO_MSS_BBRX_CTL_RMSK                                                   0x3
#define HWIO_MSS_BBRX_CTL_IN          \
        in_dword_masked(HWIO_MSS_BBRX_CTL_ADDR, HWIO_MSS_BBRX_CTL_RMSK)
#define HWIO_MSS_BBRX_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX_CTL_ADDR, m)
#define HWIO_MSS_BBRX_CTL_OUT(v)      \
        out_dword(HWIO_MSS_BBRX_CTL_ADDR,v)
#define HWIO_MSS_BBRX_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX_CTL_ADDR,m,v,HWIO_MSS_BBRX_CTL_IN)
#define HWIO_MSS_BBRX_CTL_BBRX_HS_TEST_MUX_CTL_BMSK                              0x3
#define HWIO_MSS_BBRX_CTL_BBRX_HS_TEST_MUX_CTL_SHFT                              0x0

#define HWIO_MSS_DEBUG_CTL_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00000098)
#define HWIO_MSS_DEBUG_CTL_RMSK                                                  0x7
#define HWIO_MSS_DEBUG_CTL_IN          \
        in_dword_masked(HWIO_MSS_DEBUG_CTL_ADDR, HWIO_MSS_DEBUG_CTL_RMSK)
#define HWIO_MSS_DEBUG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DEBUG_CTL_ADDR, m)
#define HWIO_MSS_DEBUG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DEBUG_CTL_ADDR,v)
#define HWIO_MSS_DEBUG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DEBUG_CTL_ADDR,m,v,HWIO_MSS_DEBUG_CTL_IN)
#define HWIO_MSS_DEBUG_CTL_DAC_DISABLE_ON_Q6_DBG_BMSK                            0x4
#define HWIO_MSS_DEBUG_CTL_DAC_DISABLE_ON_Q6_DBG_SHFT                            0x2
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_DBG_BMSK                              0x2
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_DBG_SHFT                              0x1
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_WDOG_BMSK                             0x1
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_WDOG_SHFT                             0x0

#define HWIO_MSS_MPLL0_MODE_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001000)
#define HWIO_MSS_MPLL0_MODE_RMSK                                            0x3fff3f
#define HWIO_MSS_MPLL0_MODE_IN          \
        in_dword_masked(HWIO_MSS_MPLL0_MODE_ADDR, HWIO_MSS_MPLL0_MODE_RMSK)
#define HWIO_MSS_MPLL0_MODE_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL0_MODE_ADDR, m)
#define HWIO_MSS_MPLL0_MODE_OUT(v)      \
        out_dword(HWIO_MSS_MPLL0_MODE_ADDR,v)
#define HWIO_MSS_MPLL0_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL0_MODE_ADDR,m,v,HWIO_MSS_MPLL0_MODE_IN)
#define HWIO_MSS_MPLL0_MODE_PLL_VOTE_FSM_RESET_BMSK                         0x200000
#define HWIO_MSS_MPLL0_MODE_PLL_VOTE_FSM_RESET_SHFT                             0x15
#define HWIO_MSS_MPLL0_MODE_PLL_VOTE_FSM_ENA_BMSK                           0x100000
#define HWIO_MSS_MPLL0_MODE_PLL_VOTE_FSM_ENA_SHFT                               0x14
#define HWIO_MSS_MPLL0_MODE_PLL_BIAS_COUNT_BMSK                              0xfc000
#define HWIO_MSS_MPLL0_MODE_PLL_BIAS_COUNT_SHFT                                  0xe
#define HWIO_MSS_MPLL0_MODE_PLL_LOCK_COUNT_BMSK                               0x3f00
#define HWIO_MSS_MPLL0_MODE_PLL_LOCK_COUNT_SHFT                                  0x8
#define HWIO_MSS_MPLL0_MODE_PLL_REF_XO_SEL_BMSK                                 0x30
#define HWIO_MSS_MPLL0_MODE_PLL_REF_XO_SEL_SHFT                                  0x4
#define HWIO_MSS_MPLL0_MODE_PLL_PLLTEST_BMSK                                     0x8
#define HWIO_MSS_MPLL0_MODE_PLL_PLLTEST_SHFT                                     0x3
#define HWIO_MSS_MPLL0_MODE_PLL_RESET_N_BMSK                                     0x4
#define HWIO_MSS_MPLL0_MODE_PLL_RESET_N_SHFT                                     0x2
#define HWIO_MSS_MPLL0_MODE_PLL_BYPASSNL_BMSK                                    0x2
#define HWIO_MSS_MPLL0_MODE_PLL_BYPASSNL_SHFT                                    0x1
#define HWIO_MSS_MPLL0_MODE_PLL_OUTCTRL_BMSK                                     0x1
#define HWIO_MSS_MPLL0_MODE_PLL_OUTCTRL_SHFT                                     0x0

#define HWIO_MSS_MPLL0_L_VAL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001004)
#define HWIO_MSS_MPLL0_L_VAL_RMSK                                               0xff
#define HWIO_MSS_MPLL0_L_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL0_L_VAL_ADDR, HWIO_MSS_MPLL0_L_VAL_RMSK)
#define HWIO_MSS_MPLL0_L_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL0_L_VAL_ADDR, m)
#define HWIO_MSS_MPLL0_L_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL0_L_VAL_ADDR,v)
#define HWIO_MSS_MPLL0_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL0_L_VAL_ADDR,m,v,HWIO_MSS_MPLL0_L_VAL_IN)
#define HWIO_MSS_MPLL0_L_VAL_PLL_L_BMSK                                         0xff
#define HWIO_MSS_MPLL0_L_VAL_PLL_L_SHFT                                          0x0

#define HWIO_MSS_MPLL0_M_VAL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001008)
#define HWIO_MSS_MPLL0_M_VAL_RMSK                                            0x7ffff
#define HWIO_MSS_MPLL0_M_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL0_M_VAL_ADDR, HWIO_MSS_MPLL0_M_VAL_RMSK)
#define HWIO_MSS_MPLL0_M_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL0_M_VAL_ADDR, m)
#define HWIO_MSS_MPLL0_M_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL0_M_VAL_ADDR,v)
#define HWIO_MSS_MPLL0_M_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL0_M_VAL_ADDR,m,v,HWIO_MSS_MPLL0_M_VAL_IN)
#define HWIO_MSS_MPLL0_M_VAL_PLL_M_BMSK                                      0x7ffff
#define HWIO_MSS_MPLL0_M_VAL_PLL_M_SHFT                                          0x0

#define HWIO_MSS_MPLL0_N_VAL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x0000100c)
#define HWIO_MSS_MPLL0_N_VAL_RMSK                                            0x7ffff
#define HWIO_MSS_MPLL0_N_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL0_N_VAL_ADDR, HWIO_MSS_MPLL0_N_VAL_RMSK)
#define HWIO_MSS_MPLL0_N_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL0_N_VAL_ADDR, m)
#define HWIO_MSS_MPLL0_N_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL0_N_VAL_ADDR,v)
#define HWIO_MSS_MPLL0_N_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL0_N_VAL_ADDR,m,v,HWIO_MSS_MPLL0_N_VAL_IN)
#define HWIO_MSS_MPLL0_N_VAL_PLL_N_BMSK                                      0x7ffff
#define HWIO_MSS_MPLL0_N_VAL_PLL_N_SHFT                                          0x0

#define HWIO_MSS_MPLL0_USER_CTL_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001010)
#define HWIO_MSS_MPLL0_USER_CTL_RMSK                                      0xff3073ff
#define HWIO_MSS_MPLL0_USER_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL0_USER_CTL_ADDR, HWIO_MSS_MPLL0_USER_CTL_RMSK)
#define HWIO_MSS_MPLL0_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL0_USER_CTL_ADDR, m)
#define HWIO_MSS_MPLL0_USER_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL0_USER_CTL_ADDR,v)
#define HWIO_MSS_MPLL0_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL0_USER_CTL_ADDR,m,v,HWIO_MSS_MPLL0_USER_CTL_IN)
#define HWIO_MSS_MPLL0_USER_CTL_RESERVE_31_25_BMSK                        0xfe000000
#define HWIO_MSS_MPLL0_USER_CTL_RESERVE_31_25_SHFT                              0x19
#define HWIO_MSS_MPLL0_USER_CTL_MN_EN_BMSK                                 0x1000000
#define HWIO_MSS_MPLL0_USER_CTL_MN_EN_SHFT                                      0x18
#define HWIO_MSS_MPLL0_USER_CTL_VCO_SEL_BMSK                                0x300000
#define HWIO_MSS_MPLL0_USER_CTL_VCO_SEL_SHFT                                    0x14
#define HWIO_MSS_MPLL0_USER_CTL_PRE_DIV_RATIO_BMSK                            0x7000
#define HWIO_MSS_MPLL0_USER_CTL_PRE_DIV_RATIO_SHFT                               0xc
#define HWIO_MSS_MPLL0_USER_CTL_POST_DIV_RATIO_BMSK                            0x300
#define HWIO_MSS_MPLL0_USER_CTL_POST_DIV_RATIO_SHFT                              0x8
#define HWIO_MSS_MPLL0_USER_CTL_OUTPUT_INV_BMSK                                 0x80
#define HWIO_MSS_MPLL0_USER_CTL_OUTPUT_INV_SHFT                                  0x7
#define HWIO_MSS_MPLL0_USER_CTL_PLLOUT_DIFF_90_BMSK                             0x40
#define HWIO_MSS_MPLL0_USER_CTL_PLLOUT_DIFF_90_SHFT                              0x6
#define HWIO_MSS_MPLL0_USER_CTL_PLLOUT_DIFF_0_BMSK                              0x20
#define HWIO_MSS_MPLL0_USER_CTL_PLLOUT_DIFF_0_SHFT                               0x5
#define HWIO_MSS_MPLL0_USER_CTL_PLLOUT_LV_TEST_BMSK                             0x10
#define HWIO_MSS_MPLL0_USER_CTL_PLLOUT_LV_TEST_SHFT                              0x4
#define HWIO_MSS_MPLL0_USER_CTL_PLLOUT_LV_EARLY_BMSK                             0x8
#define HWIO_MSS_MPLL0_USER_CTL_PLLOUT_LV_EARLY_SHFT                             0x3
#define HWIO_MSS_MPLL0_USER_CTL_PLLOUT_LV_AUX2_BMSK                              0x4
#define HWIO_MSS_MPLL0_USER_CTL_PLLOUT_LV_AUX2_SHFT                              0x2
#define HWIO_MSS_MPLL0_USER_CTL_PLLOUT_LV_AUX_BMSK                               0x2
#define HWIO_MSS_MPLL0_USER_CTL_PLLOUT_LV_AUX_SHFT                               0x1
#define HWIO_MSS_MPLL0_USER_CTL_PLLOUT_LV_MAIN_BMSK                              0x1
#define HWIO_MSS_MPLL0_USER_CTL_PLLOUT_LV_MAIN_SHFT                              0x0

#define HWIO_MSS_MPLL0_CONFIG_CTL_ADDR                                    (MSS_PERPH_REG_BASE      + 0x00001014)
#define HWIO_MSS_MPLL0_CONFIG_CTL_RMSK                                    0xfffcfff0
#define HWIO_MSS_MPLL0_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL0_CONFIG_CTL_ADDR, HWIO_MSS_MPLL0_CONFIG_CTL_RMSK)
#define HWIO_MSS_MPLL0_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL0_CONFIG_CTL_ADDR, m)
#define HWIO_MSS_MPLL0_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL0_CONFIG_CTL_ADDR,v)
#define HWIO_MSS_MPLL0_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL0_CONFIG_CTL_ADDR,m,v,HWIO_MSS_MPLL0_CONFIG_CTL_IN)
#define HWIO_MSS_MPLL0_CONFIG_CTL_RESERVE_31_24_BMSK                      0xff000000
#define HWIO_MSS_MPLL0_CONFIG_CTL_RESERVE_31_24_SHFT                            0x18
#define HWIO_MSS_MPLL0_CONFIG_CTL_PFD_DZSEL_BMSK                            0xc00000
#define HWIO_MSS_MPLL0_CONFIG_CTL_PFD_DZSEL_SHFT                                0x16
#define HWIO_MSS_MPLL0_CONFIG_CTL_ICP_DIV_BMSK                              0x300000
#define HWIO_MSS_MPLL0_CONFIG_CTL_ICP_DIV_SHFT                                  0x14
#define HWIO_MSS_MPLL0_CONFIG_CTL_IREG_DIV_BMSK                              0xc0000
#define HWIO_MSS_MPLL0_CONFIG_CTL_IREG_DIV_SHFT                                 0x12
#define HWIO_MSS_MPLL0_CONFIG_CTL_VREG_REF_MODE_BMSK                          0x8000
#define HWIO_MSS_MPLL0_CONFIG_CTL_VREG_REF_MODE_SHFT                             0xf
#define HWIO_MSS_MPLL0_CONFIG_CTL_VCO_REF_MODE_BMSK                           0x4000
#define HWIO_MSS_MPLL0_CONFIG_CTL_VCO_REF_MODE_SHFT                              0xe
#define HWIO_MSS_MPLL0_CONFIG_CTL_CFG_LOCKDET_BMSK                            0x3000
#define HWIO_MSS_MPLL0_CONFIG_CTL_CFG_LOCKDET_SHFT                               0xc
#define HWIO_MSS_MPLL0_CONFIG_CTL_FORCE_ISEED_BMSK                             0x800
#define HWIO_MSS_MPLL0_CONFIG_CTL_FORCE_ISEED_SHFT                               0xb
#define HWIO_MSS_MPLL0_CONFIG_CTL_COARSE_GM_BMSK                               0x600
#define HWIO_MSS_MPLL0_CONFIG_CTL_COARSE_GM_SHFT                                 0x9
#define HWIO_MSS_MPLL0_CONFIG_CTL_FILTER_LOOP_ZERO_BMSK                        0x100
#define HWIO_MSS_MPLL0_CONFIG_CTL_FILTER_LOOP_ZERO_SHFT                          0x8
#define HWIO_MSS_MPLL0_CONFIG_CTL_FILTER_LOOP_MODE_BMSK                         0x80
#define HWIO_MSS_MPLL0_CONFIG_CTL_FILTER_LOOP_MODE_SHFT                          0x7
#define HWIO_MSS_MPLL0_CONFIG_CTL_FILTER_RESISTOR_BMSK                          0x60
#define HWIO_MSS_MPLL0_CONFIG_CTL_FILTER_RESISTOR_SHFT                           0x5
#define HWIO_MSS_MPLL0_CONFIG_CTL_GMC_SLEW_MODE_BMSK                            0x10
#define HWIO_MSS_MPLL0_CONFIG_CTL_GMC_SLEW_MODE_SHFT                             0x4

#define HWIO_MSS_MPLL0_TEST_CTL_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001018)
#define HWIO_MSS_MPLL0_TEST_CTL_RMSK                                      0xffffffff
#define HWIO_MSS_MPLL0_TEST_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL0_TEST_CTL_ADDR, HWIO_MSS_MPLL0_TEST_CTL_RMSK)
#define HWIO_MSS_MPLL0_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL0_TEST_CTL_ADDR, m)
#define HWIO_MSS_MPLL0_TEST_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL0_TEST_CTL_ADDR,v)
#define HWIO_MSS_MPLL0_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL0_TEST_CTL_ADDR,m,v,HWIO_MSS_MPLL0_TEST_CTL_IN)
#define HWIO_MSS_MPLL0_TEST_CTL_RESERVE_31_21_BMSK                        0xffe00000
#define HWIO_MSS_MPLL0_TEST_CTL_RESERVE_31_21_SHFT                              0x15
#define HWIO_MSS_MPLL0_TEST_CTL_NGEN_CFG_BMSK                               0x1c0000
#define HWIO_MSS_MPLL0_TEST_CTL_NGEN_CFG_SHFT                                   0x12
#define HWIO_MSS_MPLL0_TEST_CTL_NGEN_EN_BMSK                                 0x20000
#define HWIO_MSS_MPLL0_TEST_CTL_NGEN_EN_SHFT                                    0x11
#define HWIO_MSS_MPLL0_TEST_CTL_NMOSC_FREQ_CTRL_BMSK                         0x18000
#define HWIO_MSS_MPLL0_TEST_CTL_NMOSC_FREQ_CTRL_SHFT                             0xf
#define HWIO_MSS_MPLL0_TEST_CTL_NMOSC_EN_BMSK                                 0x4000
#define HWIO_MSS_MPLL0_TEST_CTL_NMOSC_EN_SHFT                                    0xe
#define HWIO_MSS_MPLL0_TEST_CTL_FORCE_PFD_UP_BMSK                             0x2000
#define HWIO_MSS_MPLL0_TEST_CTL_FORCE_PFD_UP_SHFT                                0xd
#define HWIO_MSS_MPLL0_TEST_CTL_FORCE_PFD_DOWN_BMSK                           0x1000
#define HWIO_MSS_MPLL0_TEST_CTL_FORCE_PFD_DOWN_SHFT                              0xc
#define HWIO_MSS_MPLL0_TEST_CTL_TEST_OUT_SEL_BMSK                              0x800
#define HWIO_MSS_MPLL0_TEST_CTL_TEST_OUT_SEL_SHFT                                0xb
#define HWIO_MSS_MPLL0_TEST_CTL_ICP_TST_EN_BMSK                                0x400
#define HWIO_MSS_MPLL0_TEST_CTL_ICP_TST_EN_SHFT                                  0xa
#define HWIO_MSS_MPLL0_TEST_CTL_ICP_EXT_SEL_BMSK                               0x200
#define HWIO_MSS_MPLL0_TEST_CTL_ICP_EXT_SEL_SHFT                                 0x9
#define HWIO_MSS_MPLL0_TEST_CTL_DTEST_SEL_BMSK                                 0x180
#define HWIO_MSS_MPLL0_TEST_CTL_DTEST_SEL_SHFT                                   0x7
#define HWIO_MSS_MPLL0_TEST_CTL_BYP_TESTAMP_BMSK                                0x40
#define HWIO_MSS_MPLL0_TEST_CTL_BYP_TESTAMP_SHFT                                 0x6
#define HWIO_MSS_MPLL0_TEST_CTL_ATEST1_SEL_BMSK                                 0x30
#define HWIO_MSS_MPLL0_TEST_CTL_ATEST1_SEL_SHFT                                  0x4
#define HWIO_MSS_MPLL0_TEST_CTL_ATEST0_SEL_BMSK                                  0xc
#define HWIO_MSS_MPLL0_TEST_CTL_ATEST0_SEL_SHFT                                  0x2
#define HWIO_MSS_MPLL0_TEST_CTL_ATEST1_EN_BMSK                                   0x2
#define HWIO_MSS_MPLL0_TEST_CTL_ATEST1_EN_SHFT                                   0x1
#define HWIO_MSS_MPLL0_TEST_CTL_ATEST0_EN_BMSK                                   0x1
#define HWIO_MSS_MPLL0_TEST_CTL_ATEST0_EN_SHFT                                   0x0

#define HWIO_MSS_MPLL0_STATUS_ADDR                                        (MSS_PERPH_REG_BASE      + 0x0000101c)
#define HWIO_MSS_MPLL0_STATUS_RMSK                                           0x3ffff
#define HWIO_MSS_MPLL0_STATUS_IN          \
        in_dword_masked(HWIO_MSS_MPLL0_STATUS_ADDR, HWIO_MSS_MPLL0_STATUS_RMSK)
#define HWIO_MSS_MPLL0_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL0_STATUS_ADDR, m)
#define HWIO_MSS_MPLL0_STATUS_PLL_ACTIVE_FLAG_BMSK                           0x20000
#define HWIO_MSS_MPLL0_STATUS_PLL_ACTIVE_FLAG_SHFT                              0x11
#define HWIO_MSS_MPLL0_STATUS_PLL_LOCK_DET_BMSK                              0x10000
#define HWIO_MSS_MPLL0_STATUS_PLL_LOCK_DET_SHFT                                 0x10
#define HWIO_MSS_MPLL0_STATUS_PLL_D_BMSK                                      0xffff
#define HWIO_MSS_MPLL0_STATUS_PLL_D_SHFT                                         0x0

#define HWIO_MSS_MPLL1_MODE_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001020)
#define HWIO_MSS_MPLL1_MODE_RMSK                                            0x3fff3f
#define HWIO_MSS_MPLL1_MODE_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_MODE_ADDR, HWIO_MSS_MPLL1_MODE_RMSK)
#define HWIO_MSS_MPLL1_MODE_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_MODE_ADDR, m)
#define HWIO_MSS_MPLL1_MODE_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_MODE_ADDR,v)
#define HWIO_MSS_MPLL1_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_MODE_ADDR,m,v,HWIO_MSS_MPLL1_MODE_IN)
#define HWIO_MSS_MPLL1_MODE_PLL_VOTE_FSM_RESET_BMSK                         0x200000
#define HWIO_MSS_MPLL1_MODE_PLL_VOTE_FSM_RESET_SHFT                             0x15
#define HWIO_MSS_MPLL1_MODE_PLL_VOTE_FSM_ENA_BMSK                           0x100000
#define HWIO_MSS_MPLL1_MODE_PLL_VOTE_FSM_ENA_SHFT                               0x14
#define HWIO_MSS_MPLL1_MODE_PLL_BIAS_COUNT_BMSK                              0xfc000
#define HWIO_MSS_MPLL1_MODE_PLL_BIAS_COUNT_SHFT                                  0xe
#define HWIO_MSS_MPLL1_MODE_PLL_LOCK_COUNT_BMSK                               0x3f00
#define HWIO_MSS_MPLL1_MODE_PLL_LOCK_COUNT_SHFT                                  0x8
#define HWIO_MSS_MPLL1_MODE_PLL_REF_XO_SEL_BMSK                                 0x30
#define HWIO_MSS_MPLL1_MODE_PLL_REF_XO_SEL_SHFT                                  0x4
#define HWIO_MSS_MPLL1_MODE_PLL_PLLTEST_BMSK                                     0x8
#define HWIO_MSS_MPLL1_MODE_PLL_PLLTEST_SHFT                                     0x3
#define HWIO_MSS_MPLL1_MODE_PLL_RESET_N_BMSK                                     0x4
#define HWIO_MSS_MPLL1_MODE_PLL_RESET_N_SHFT                                     0x2
#define HWIO_MSS_MPLL1_MODE_PLL_BYPASSNL_BMSK                                    0x2
#define HWIO_MSS_MPLL1_MODE_PLL_BYPASSNL_SHFT                                    0x1
#define HWIO_MSS_MPLL1_MODE_PLL_OUTCTRL_BMSK                                     0x1
#define HWIO_MSS_MPLL1_MODE_PLL_OUTCTRL_SHFT                                     0x0

#define HWIO_MSS_MPLL1_L_VAL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001024)
#define HWIO_MSS_MPLL1_L_VAL_RMSK                                               0x7f
#define HWIO_MSS_MPLL1_L_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_L_VAL_ADDR, HWIO_MSS_MPLL1_L_VAL_RMSK)
#define HWIO_MSS_MPLL1_L_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_L_VAL_ADDR, m)
#define HWIO_MSS_MPLL1_L_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_L_VAL_ADDR,v)
#define HWIO_MSS_MPLL1_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_L_VAL_ADDR,m,v,HWIO_MSS_MPLL1_L_VAL_IN)
#define HWIO_MSS_MPLL1_L_VAL_PLL_L_BMSK                                         0x7f
#define HWIO_MSS_MPLL1_L_VAL_PLL_L_SHFT                                          0x0

#define HWIO_MSS_MPLL1_M_VAL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001028)
#define HWIO_MSS_MPLL1_M_VAL_RMSK                                            0x7ffff
#define HWIO_MSS_MPLL1_M_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_M_VAL_ADDR, HWIO_MSS_MPLL1_M_VAL_RMSK)
#define HWIO_MSS_MPLL1_M_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_M_VAL_ADDR, m)
#define HWIO_MSS_MPLL1_M_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_M_VAL_ADDR,v)
#define HWIO_MSS_MPLL1_M_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_M_VAL_ADDR,m,v,HWIO_MSS_MPLL1_M_VAL_IN)
#define HWIO_MSS_MPLL1_M_VAL_PLL_M_BMSK                                      0x7ffff
#define HWIO_MSS_MPLL1_M_VAL_PLL_M_SHFT                                          0x0

#define HWIO_MSS_MPLL1_N_VAL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x0000102c)
#define HWIO_MSS_MPLL1_N_VAL_RMSK                                            0x7ffff
#define HWIO_MSS_MPLL1_N_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_N_VAL_ADDR, HWIO_MSS_MPLL1_N_VAL_RMSK)
#define HWIO_MSS_MPLL1_N_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_N_VAL_ADDR, m)
#define HWIO_MSS_MPLL1_N_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_N_VAL_ADDR,v)
#define HWIO_MSS_MPLL1_N_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_N_VAL_ADDR,m,v,HWIO_MSS_MPLL1_N_VAL_IN)
#define HWIO_MSS_MPLL1_N_VAL_PLL_N_BMSK                                      0x7ffff
#define HWIO_MSS_MPLL1_N_VAL_PLL_N_SHFT                                          0x0

#define HWIO_MSS_MPLL1_USER_CTL_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001030)
#define HWIO_MSS_MPLL1_USER_CTL_RMSK                                      0xffffffff
#define HWIO_MSS_MPLL1_USER_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_USER_CTL_ADDR, HWIO_MSS_MPLL1_USER_CTL_RMSK)
#define HWIO_MSS_MPLL1_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_USER_CTL_ADDR, m)
#define HWIO_MSS_MPLL1_USER_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_USER_CTL_ADDR,v)
#define HWIO_MSS_MPLL1_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_USER_CTL_ADDR,m,v,HWIO_MSS_MPLL1_USER_CTL_IN)
#define HWIO_MSS_MPLL1_USER_CTL_ADDITIVE_FACTOR_BMSK                      0xc0000000
#define HWIO_MSS_MPLL1_USER_CTL_ADDITIVE_FACTOR_SHFT                            0x1e
#define HWIO_MSS_MPLL1_USER_CTL_VCO_DOUBLER_QUADRUPLER_MODE_BMSK          0x30000000
#define HWIO_MSS_MPLL1_USER_CTL_VCO_DOUBLER_QUADRUPLER_MODE_SHFT                0x1c
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BIT_27_25_BMSK                     0xe000000
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BIT_27_25_SHFT                          0x19
#define HWIO_MSS_MPLL1_USER_CTL_MN_EN_BMSK                                 0x1000000
#define HWIO_MSS_MPLL1_USER_CTL_MN_EN_SHFT                                      0x18
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_23_13_BMSK                     0xffe000
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_23_13_SHFT                          0xd
#define HWIO_MSS_MPLL1_USER_CTL_PREDIV2_EN_BMSK                               0x1000
#define HWIO_MSS_MPLL1_USER_CTL_PREDIV2_EN_SHFT                                  0xc
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_11_10_BMSK                        0xc00
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_11_10_SHFT                          0xa
#define HWIO_MSS_MPLL1_USER_CTL_POSTDIV_CTL_BMSK                               0x300
#define HWIO_MSS_MPLL1_USER_CTL_POSTDIV_CTL_SHFT                                 0x8
#define HWIO_MSS_MPLL1_USER_CTL_INV_OUTPUT_BMSK                                 0x80
#define HWIO_MSS_MPLL1_USER_CTL_INV_OUTPUT_SHFT                                  0x7
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BIT_6_5_BMSK                            0x60
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BIT_6_5_SHFT                             0x5
#define HWIO_MSS_MPLL1_USER_CTL_LVTEST_EN_BMSK                                  0x10
#define HWIO_MSS_MPLL1_USER_CTL_LVTEST_EN_SHFT                                   0x4
#define HWIO_MSS_MPLL1_USER_CTL_LVEARLY_EN_BMSK                                  0x8
#define HWIO_MSS_MPLL1_USER_CTL_LVEARLY_EN_SHFT                                  0x3
#define HWIO_MSS_MPLL1_USER_CTL_LVAUX2_EN_BMSK                                   0x4
#define HWIO_MSS_MPLL1_USER_CTL_LVAUX2_EN_SHFT                                   0x2
#define HWIO_MSS_MPLL1_USER_CTL_LVAUX_EN_BMSK                                    0x2
#define HWIO_MSS_MPLL1_USER_CTL_LVAUX_EN_SHFT                                    0x1
#define HWIO_MSS_MPLL1_USER_CTL_LVMAIN_EN_BMSK                                   0x1
#define HWIO_MSS_MPLL1_USER_CTL_LVMAIN_EN_SHFT                                   0x0

#define HWIO_MSS_MPLL1_CONFIG_CTL_ADDR                                    (MSS_PERPH_REG_BASE      + 0x00001034)
#define HWIO_MSS_MPLL1_CONFIG_CTL_RMSK                                    0xffffffff
#define HWIO_MSS_MPLL1_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR, HWIO_MSS_MPLL1_CONFIG_CTL_RMSK)
#define HWIO_MSS_MPLL1_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR, m)
#define HWIO_MSS_MPLL1_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR,v)
#define HWIO_MSS_MPLL1_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR,m,v,HWIO_MSS_MPLL1_CONFIG_CTL_IN)
#define HWIO_MSS_MPLL1_CONFIG_CTL_MULTIPLICATIONFACTOR_BMSK               0xc0000000
#define HWIO_MSS_MPLL1_CONFIG_CTL_MULTIPLICATIONFACTOR_SHFT                     0x1e
#define HWIO_MSS_MPLL1_CONFIG_CTL_RESERVE_BIT_29_27_BMSK                  0x38000000
#define HWIO_MSS_MPLL1_CONFIG_CTL_RESERVE_BIT_29_27_SHFT                        0x1b
#define HWIO_MSS_MPLL1_CONFIG_CTL_OVDREFCFG_BMSK                           0x6000000
#define HWIO_MSS_MPLL1_CONFIG_CTL_OVDREFCFG_SHFT                                0x19
#define HWIO_MSS_MPLL1_CONFIG_CTL_RING_OSC_LATCH_EN_BMSK                   0x1000000
#define HWIO_MSS_MPLL1_CONFIG_CTL_RING_OSC_LATCH_EN_SHFT                        0x18
#define HWIO_MSS_MPLL1_CONFIG_CTL_RESERVE_BIT_23_18_BMSK                    0xfc0000
#define HWIO_MSS_MPLL1_CONFIG_CTL_RESERVE_BIT_23_18_SHFT                        0x12
#define HWIO_MSS_MPLL1_CONFIG_CTL_PDN_OVDCNT_BMSK                            0x20000
#define HWIO_MSS_MPLL1_CONFIG_CTL_PDN_OVDCNT_SHFT                               0x11
#define HWIO_MSS_MPLL1_CONFIG_CTL_CFG_OVDCNT_BMSK                            0x18000
#define HWIO_MSS_MPLL1_CONFIG_CTL_CFG_OVDCNT_SHFT                                0xf
#define HWIO_MSS_MPLL1_CONFIG_CTL_RVSIG_DEL_BMSK                              0x6000
#define HWIO_MSS_MPLL1_CONFIG_CTL_RVSIG_DEL_SHFT                                 0xd
#define HWIO_MSS_MPLL1_CONFIG_CTL_PFD_DZSEL_BMSK                              0x1800
#define HWIO_MSS_MPLL1_CONFIG_CTL_PFD_DZSEL_SHFT                                 0xb
#define HWIO_MSS_MPLL1_CONFIG_CTL_FORCE_ISEED_BMSK                             0x400
#define HWIO_MSS_MPLL1_CONFIG_CTL_FORCE_ISEED_SHFT                               0xa
#define HWIO_MSS_MPLL1_CONFIG_CTL_ICP_SEED_SEL_BMSK                            0x300
#define HWIO_MSS_MPLL1_CONFIG_CTL_ICP_SEED_SEL_SHFT                              0x8
#define HWIO_MSS_MPLL1_CONFIG_CTL_SEL_IREG_REP_BMSK                             0xc0
#define HWIO_MSS_MPLL1_CONFIG_CTL_SEL_IREG_REP_SHFT                              0x6
#define HWIO_MSS_MPLL1_CONFIG_CTL_SEL_IREG_OSC_BMSK                             0x30
#define HWIO_MSS_MPLL1_CONFIG_CTL_SEL_IREG_OSC_SHFT                              0x4
#define HWIO_MSS_MPLL1_CONFIG_CTL_CAPSEL_BMSK                                    0xc
#define HWIO_MSS_MPLL1_CONFIG_CTL_CAPSEL_SHFT                                    0x2
#define HWIO_MSS_MPLL1_CONFIG_CTL_CFG_LOCKDET_BMSK                               0x3
#define HWIO_MSS_MPLL1_CONFIG_CTL_CFG_LOCKDET_SHFT                               0x0

#define HWIO_MSS_MPLL1_TEST_CTL_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001038)
#define HWIO_MSS_MPLL1_TEST_CTL_RMSK                                      0xffffffff
#define HWIO_MSS_MPLL1_TEST_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_TEST_CTL_ADDR, HWIO_MSS_MPLL1_TEST_CTL_RMSK)
#define HWIO_MSS_MPLL1_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_TEST_CTL_ADDR, m)
#define HWIO_MSS_MPLL1_TEST_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_TEST_CTL_ADDR,v)
#define HWIO_MSS_MPLL1_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_TEST_CTL_ADDR,m,v,HWIO_MSS_MPLL1_TEST_CTL_IN)
#define HWIO_MSS_MPLL1_TEST_CTL_RESERVE_31_10_BMSK                        0xfffffc00
#define HWIO_MSS_MPLL1_TEST_CTL_RESERVE_31_10_SHFT                               0xa
#define HWIO_MSS_MPLL1_TEST_CTL_IEXT_SEL_BMSK                                  0x200
#define HWIO_MSS_MPLL1_TEST_CTL_IEXT_SEL_SHFT                                    0x9
#define HWIO_MSS_MPLL1_TEST_CTL_DTEST_SEL_BMSK                                 0x180
#define HWIO_MSS_MPLL1_TEST_CTL_DTEST_SEL_SHFT                                   0x7
#define HWIO_MSS_MPLL1_TEST_CTL_BYP_TESTAMP_BMSK                                0x40
#define HWIO_MSS_MPLL1_TEST_CTL_BYP_TESTAMP_SHFT                                 0x6
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST1_SEL_BMSK                                 0x30
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST1_SEL_SHFT                                  0x4
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST0_SEL_BMSK                                  0xc
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST0_SEL_SHFT                                  0x2
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST1_EN_BMSK                                   0x2
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST1_EN_SHFT                                   0x1
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST0_EN_BMSK                                   0x1
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST0_EN_SHFT                                   0x0

#define HWIO_MSS_MPLL1_STATUS_ADDR                                        (MSS_PERPH_REG_BASE      + 0x0000103c)
#define HWIO_MSS_MPLL1_STATUS_RMSK                                           0x3ffff
#define HWIO_MSS_MPLL1_STATUS_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_STATUS_ADDR, HWIO_MSS_MPLL1_STATUS_RMSK)
#define HWIO_MSS_MPLL1_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_STATUS_ADDR, m)
#define HWIO_MSS_MPLL1_STATUS_PLL_ACTIVE_FLAG_BMSK                           0x20000
#define HWIO_MSS_MPLL1_STATUS_PLL_ACTIVE_FLAG_SHFT                              0x11
#define HWIO_MSS_MPLL1_STATUS_PLL_LOCK_DET_BMSK                              0x10000
#define HWIO_MSS_MPLL1_STATUS_PLL_LOCK_DET_SHFT                                 0x10
#define HWIO_MSS_MPLL1_STATUS_PLL_D_BMSK                                      0xffff
#define HWIO_MSS_MPLL1_STATUS_PLL_D_SHFT                                         0x0

#define HWIO_MSS_MPLL2_MODE_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001040)
#define HWIO_MSS_MPLL2_MODE_RMSK                                            0x3fff3f
#define HWIO_MSS_MPLL2_MODE_IN          \
        in_dword_masked(HWIO_MSS_MPLL2_MODE_ADDR, HWIO_MSS_MPLL2_MODE_RMSK)
#define HWIO_MSS_MPLL2_MODE_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL2_MODE_ADDR, m)
#define HWIO_MSS_MPLL2_MODE_OUT(v)      \
        out_dword(HWIO_MSS_MPLL2_MODE_ADDR,v)
#define HWIO_MSS_MPLL2_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL2_MODE_ADDR,m,v,HWIO_MSS_MPLL2_MODE_IN)
#define HWIO_MSS_MPLL2_MODE_PLL_VOTE_FSM_RESET_BMSK                         0x200000
#define HWIO_MSS_MPLL2_MODE_PLL_VOTE_FSM_RESET_SHFT                             0x15
#define HWIO_MSS_MPLL2_MODE_PLL_VOTE_FSM_ENA_BMSK                           0x100000
#define HWIO_MSS_MPLL2_MODE_PLL_VOTE_FSM_ENA_SHFT                               0x14
#define HWIO_MSS_MPLL2_MODE_PLL_BIAS_COUNT_BMSK                              0xfc000
#define HWIO_MSS_MPLL2_MODE_PLL_BIAS_COUNT_SHFT                                  0xe
#define HWIO_MSS_MPLL2_MODE_PLL_LOCK_COUNT_BMSK                               0x3f00
#define HWIO_MSS_MPLL2_MODE_PLL_LOCK_COUNT_SHFT                                  0x8
#define HWIO_MSS_MPLL2_MODE_PLL_REF_XO_SEL_BMSK                                 0x30
#define HWIO_MSS_MPLL2_MODE_PLL_REF_XO_SEL_SHFT                                  0x4
#define HWIO_MSS_MPLL2_MODE_PLL_PLLTEST_BMSK                                     0x8
#define HWIO_MSS_MPLL2_MODE_PLL_PLLTEST_SHFT                                     0x3
#define HWIO_MSS_MPLL2_MODE_PLL_RESET_N_BMSK                                     0x4
#define HWIO_MSS_MPLL2_MODE_PLL_RESET_N_SHFT                                     0x2
#define HWIO_MSS_MPLL2_MODE_PLL_BYPASSNL_BMSK                                    0x2
#define HWIO_MSS_MPLL2_MODE_PLL_BYPASSNL_SHFT                                    0x1
#define HWIO_MSS_MPLL2_MODE_PLL_OUTCTRL_BMSK                                     0x1
#define HWIO_MSS_MPLL2_MODE_PLL_OUTCTRL_SHFT                                     0x0

#define HWIO_MSS_MPLL2_L_VAL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001044)
#define HWIO_MSS_MPLL2_L_VAL_RMSK                                               0xff
#define HWIO_MSS_MPLL2_L_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL2_L_VAL_ADDR, HWIO_MSS_MPLL2_L_VAL_RMSK)
#define HWIO_MSS_MPLL2_L_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL2_L_VAL_ADDR, m)
#define HWIO_MSS_MPLL2_L_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL2_L_VAL_ADDR,v)
#define HWIO_MSS_MPLL2_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL2_L_VAL_ADDR,m,v,HWIO_MSS_MPLL2_L_VAL_IN)
#define HWIO_MSS_MPLL2_L_VAL_PLL_L_BMSK                                         0xff
#define HWIO_MSS_MPLL2_L_VAL_PLL_L_SHFT                                          0x0

#define HWIO_MSS_MPLL2_M_VAL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001048)
#define HWIO_MSS_MPLL2_M_VAL_RMSK                                            0x7ffff
#define HWIO_MSS_MPLL2_M_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL2_M_VAL_ADDR, HWIO_MSS_MPLL2_M_VAL_RMSK)
#define HWIO_MSS_MPLL2_M_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL2_M_VAL_ADDR, m)
#define HWIO_MSS_MPLL2_M_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL2_M_VAL_ADDR,v)
#define HWIO_MSS_MPLL2_M_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL2_M_VAL_ADDR,m,v,HWIO_MSS_MPLL2_M_VAL_IN)
#define HWIO_MSS_MPLL2_M_VAL_PLL_M_BMSK                                      0x7ffff
#define HWIO_MSS_MPLL2_M_VAL_PLL_M_SHFT                                          0x0

#define HWIO_MSS_MPLL2_N_VAL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x0000104c)
#define HWIO_MSS_MPLL2_N_VAL_RMSK                                            0x7ffff
#define HWIO_MSS_MPLL2_N_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL2_N_VAL_ADDR, HWIO_MSS_MPLL2_N_VAL_RMSK)
#define HWIO_MSS_MPLL2_N_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL2_N_VAL_ADDR, m)
#define HWIO_MSS_MPLL2_N_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL2_N_VAL_ADDR,v)
#define HWIO_MSS_MPLL2_N_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL2_N_VAL_ADDR,m,v,HWIO_MSS_MPLL2_N_VAL_IN)
#define HWIO_MSS_MPLL2_N_VAL_PLL_N_BMSK                                      0x7ffff
#define HWIO_MSS_MPLL2_N_VAL_PLL_N_SHFT                                          0x0

#define HWIO_MSS_MPLL2_USER_CTL_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001050)
#define HWIO_MSS_MPLL2_USER_CTL_RMSK                                      0xff3073ff
#define HWIO_MSS_MPLL2_USER_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL2_USER_CTL_ADDR, HWIO_MSS_MPLL2_USER_CTL_RMSK)
#define HWIO_MSS_MPLL2_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL2_USER_CTL_ADDR, m)
#define HWIO_MSS_MPLL2_USER_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL2_USER_CTL_ADDR,v)
#define HWIO_MSS_MPLL2_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL2_USER_CTL_ADDR,m,v,HWIO_MSS_MPLL2_USER_CTL_IN)
#define HWIO_MSS_MPLL2_USER_CTL_RESERVE_31_25_BMSK                        0xfe000000
#define HWIO_MSS_MPLL2_USER_CTL_RESERVE_31_25_SHFT                              0x19
#define HWIO_MSS_MPLL2_USER_CTL_MN_EN_BMSK                                 0x1000000
#define HWIO_MSS_MPLL2_USER_CTL_MN_EN_SHFT                                      0x18
#define HWIO_MSS_MPLL2_USER_CTL_VCO_SEL_BMSK                                0x300000
#define HWIO_MSS_MPLL2_USER_CTL_VCO_SEL_SHFT                                    0x14
#define HWIO_MSS_MPLL2_USER_CTL_PRE_DIV_RATIO_BMSK                            0x7000
#define HWIO_MSS_MPLL2_USER_CTL_PRE_DIV_RATIO_SHFT                               0xc
#define HWIO_MSS_MPLL2_USER_CTL_POST_DIV_RATIO_BMSK                            0x300
#define HWIO_MSS_MPLL2_USER_CTL_POST_DIV_RATIO_SHFT                              0x8
#define HWIO_MSS_MPLL2_USER_CTL_OUTPUT_INV_BMSK                                 0x80
#define HWIO_MSS_MPLL2_USER_CTL_OUTPUT_INV_SHFT                                  0x7
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_DIFF_90_BMSK                             0x40
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_DIFF_90_SHFT                              0x6
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_DIFF_0_BMSK                              0x20
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_DIFF_0_SHFT                               0x5
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_TEST_BMSK                             0x10
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_TEST_SHFT                              0x4
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_EARLY_BMSK                             0x8
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_EARLY_SHFT                             0x3
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_AUX2_BMSK                              0x4
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_AUX2_SHFT                              0x2
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_AUX_BMSK                               0x2
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_AUX_SHFT                               0x1
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_MAIN_BMSK                              0x1
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_MAIN_SHFT                              0x0

#define HWIO_MSS_MPLL2_CONFIG_CTL_ADDR                                    (MSS_PERPH_REG_BASE      + 0x00001054)
#define HWIO_MSS_MPLL2_CONFIG_CTL_RMSK                                    0xfffcfff0
#define HWIO_MSS_MPLL2_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL2_CONFIG_CTL_ADDR, HWIO_MSS_MPLL2_CONFIG_CTL_RMSK)
#define HWIO_MSS_MPLL2_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL2_CONFIG_CTL_ADDR, m)
#define HWIO_MSS_MPLL2_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL2_CONFIG_CTL_ADDR,v)
#define HWIO_MSS_MPLL2_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL2_CONFIG_CTL_ADDR,m,v,HWIO_MSS_MPLL2_CONFIG_CTL_IN)
#define HWIO_MSS_MPLL2_CONFIG_CTL_RESERVE_31_24_BMSK                      0xff000000
#define HWIO_MSS_MPLL2_CONFIG_CTL_RESERVE_31_24_SHFT                            0x18
#define HWIO_MSS_MPLL2_CONFIG_CTL_PFD_DZSEL_BMSK                            0xc00000
#define HWIO_MSS_MPLL2_CONFIG_CTL_PFD_DZSEL_SHFT                                0x16
#define HWIO_MSS_MPLL2_CONFIG_CTL_ICP_DIV_BMSK                              0x300000
#define HWIO_MSS_MPLL2_CONFIG_CTL_ICP_DIV_SHFT                                  0x14
#define HWIO_MSS_MPLL2_CONFIG_CTL_IREG_DIV_BMSK                              0xc0000
#define HWIO_MSS_MPLL2_CONFIG_CTL_IREG_DIV_SHFT                                 0x12
#define HWIO_MSS_MPLL2_CONFIG_CTL_VREG_REF_MODE_BMSK                          0x8000
#define HWIO_MSS_MPLL2_CONFIG_CTL_VREG_REF_MODE_SHFT                             0xf
#define HWIO_MSS_MPLL2_CONFIG_CTL_VCO_REF_MODE_BMSK                           0x4000
#define HWIO_MSS_MPLL2_CONFIG_CTL_VCO_REF_MODE_SHFT                              0xe
#define HWIO_MSS_MPLL2_CONFIG_CTL_CFG_LOCKDET_BMSK                            0x3000
#define HWIO_MSS_MPLL2_CONFIG_CTL_CFG_LOCKDET_SHFT                               0xc
#define HWIO_MSS_MPLL2_CONFIG_CTL_FORCE_ISEED_BMSK                             0x800
#define HWIO_MSS_MPLL2_CONFIG_CTL_FORCE_ISEED_SHFT                               0xb
#define HWIO_MSS_MPLL2_CONFIG_CTL_COARSE_GM_BMSK                               0x600
#define HWIO_MSS_MPLL2_CONFIG_CTL_COARSE_GM_SHFT                                 0x9
#define HWIO_MSS_MPLL2_CONFIG_CTL_FILTER_LOOP_ZERO_BMSK                        0x100
#define HWIO_MSS_MPLL2_CONFIG_CTL_FILTER_LOOP_ZERO_SHFT                          0x8
#define HWIO_MSS_MPLL2_CONFIG_CTL_FILTER_LOOP_MODE_BMSK                         0x80
#define HWIO_MSS_MPLL2_CONFIG_CTL_FILTER_LOOP_MODE_SHFT                          0x7
#define HWIO_MSS_MPLL2_CONFIG_CTL_FILTER_RESISTOR_BMSK                          0x60
#define HWIO_MSS_MPLL2_CONFIG_CTL_FILTER_RESISTOR_SHFT                           0x5
#define HWIO_MSS_MPLL2_CONFIG_CTL_GMC_SLEW_MODE_BMSK                            0x10
#define HWIO_MSS_MPLL2_CONFIG_CTL_GMC_SLEW_MODE_SHFT                             0x4

#define HWIO_MSS_MPLL2_TEST_CTL_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001058)
#define HWIO_MSS_MPLL2_TEST_CTL_RMSK                                      0xffffffff
#define HWIO_MSS_MPLL2_TEST_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL2_TEST_CTL_ADDR, HWIO_MSS_MPLL2_TEST_CTL_RMSK)
#define HWIO_MSS_MPLL2_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL2_TEST_CTL_ADDR, m)
#define HWIO_MSS_MPLL2_TEST_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL2_TEST_CTL_ADDR,v)
#define HWIO_MSS_MPLL2_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL2_TEST_CTL_ADDR,m,v,HWIO_MSS_MPLL2_TEST_CTL_IN)
#define HWIO_MSS_MPLL2_TEST_CTL_RESERVE_31_21_BMSK                        0xffe00000
#define HWIO_MSS_MPLL2_TEST_CTL_RESERVE_31_21_SHFT                              0x15
#define HWIO_MSS_MPLL2_TEST_CTL_NGEN_CFG_BMSK                               0x1c0000
#define HWIO_MSS_MPLL2_TEST_CTL_NGEN_CFG_SHFT                                   0x12
#define HWIO_MSS_MPLL2_TEST_CTL_NGEN_EN_BMSK                                 0x20000
#define HWIO_MSS_MPLL2_TEST_CTL_NGEN_EN_SHFT                                    0x11
#define HWIO_MSS_MPLL2_TEST_CTL_NMOSC_FREQ_CTRL_BMSK                         0x18000
#define HWIO_MSS_MPLL2_TEST_CTL_NMOSC_FREQ_CTRL_SHFT                             0xf
#define HWIO_MSS_MPLL2_TEST_CTL_NMOSC_EN_BMSK                                 0x4000
#define HWIO_MSS_MPLL2_TEST_CTL_NMOSC_EN_SHFT                                    0xe
#define HWIO_MSS_MPLL2_TEST_CTL_FORCE_PFD_UP_BMSK                             0x2000
#define HWIO_MSS_MPLL2_TEST_CTL_FORCE_PFD_UP_SHFT                                0xd
#define HWIO_MSS_MPLL2_TEST_CTL_FORCE_PFD_DOWN_BMSK                           0x1000
#define HWIO_MSS_MPLL2_TEST_CTL_FORCE_PFD_DOWN_SHFT                              0xc
#define HWIO_MSS_MPLL2_TEST_CTL_TEST_OUT_SEL_BMSK                              0x800
#define HWIO_MSS_MPLL2_TEST_CTL_TEST_OUT_SEL_SHFT                                0xb
#define HWIO_MSS_MPLL2_TEST_CTL_ICP_TST_EN_BMSK                                0x400
#define HWIO_MSS_MPLL2_TEST_CTL_ICP_TST_EN_SHFT                                  0xa
#define HWIO_MSS_MPLL2_TEST_CTL_ICP_EXT_SEL_BMSK                               0x200
#define HWIO_MSS_MPLL2_TEST_CTL_ICP_EXT_SEL_SHFT                                 0x9
#define HWIO_MSS_MPLL2_TEST_CTL_DTEST_SEL_BMSK                                 0x180
#define HWIO_MSS_MPLL2_TEST_CTL_DTEST_SEL_SHFT                                   0x7
#define HWIO_MSS_MPLL2_TEST_CTL_BYP_TESTAMP_BMSK                                0x40
#define HWIO_MSS_MPLL2_TEST_CTL_BYP_TESTAMP_SHFT                                 0x6
#define HWIO_MSS_MPLL2_TEST_CTL_ATEST1_SEL_BMSK                                 0x30
#define HWIO_MSS_MPLL2_TEST_CTL_ATEST1_SEL_SHFT                                  0x4
#define HWIO_MSS_MPLL2_TEST_CTL_ATEST0_SEL_BMSK                                  0xc
#define HWIO_MSS_MPLL2_TEST_CTL_ATEST0_SEL_SHFT                                  0x2
#define HWIO_MSS_MPLL2_TEST_CTL_ATEST1_EN_BMSK                                   0x2
#define HWIO_MSS_MPLL2_TEST_CTL_ATEST1_EN_SHFT                                   0x1
#define HWIO_MSS_MPLL2_TEST_CTL_ATEST0_EN_BMSK                                   0x1
#define HWIO_MSS_MPLL2_TEST_CTL_ATEST0_EN_SHFT                                   0x0

#define HWIO_MSS_MPLL2_STATUS_ADDR                                        (MSS_PERPH_REG_BASE      + 0x0000105c)
#define HWIO_MSS_MPLL2_STATUS_RMSK                                           0x3ffff
#define HWIO_MSS_MPLL2_STATUS_IN          \
        in_dword_masked(HWIO_MSS_MPLL2_STATUS_ADDR, HWIO_MSS_MPLL2_STATUS_RMSK)
#define HWIO_MSS_MPLL2_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL2_STATUS_ADDR, m)
#define HWIO_MSS_MPLL2_STATUS_PLL_ACTIVE_FLAG_BMSK                           0x20000
#define HWIO_MSS_MPLL2_STATUS_PLL_ACTIVE_FLAG_SHFT                              0x11
#define HWIO_MSS_MPLL2_STATUS_PLL_LOCK_DET_BMSK                              0x10000
#define HWIO_MSS_MPLL2_STATUS_PLL_LOCK_DET_SHFT                                 0x10
#define HWIO_MSS_MPLL2_STATUS_PLL_D_BMSK                                      0xffff
#define HWIO_MSS_MPLL2_STATUS_PLL_D_SHFT                                         0x0

#define HWIO_MSS_UIM0_BCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001060)
#define HWIO_MSS_UIM0_BCR_RMSK                                                   0x1
#define HWIO_MSS_UIM0_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_BCR_ADDR, HWIO_MSS_UIM0_BCR_RMSK)
#define HWIO_MSS_UIM0_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_BCR_ADDR, m)
#define HWIO_MSS_UIM0_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_BCR_ADDR,v)
#define HWIO_MSS_UIM0_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_BCR_ADDR,m,v,HWIO_MSS_UIM0_BCR_IN)
#define HWIO_MSS_UIM0_BCR_BLK_ARES_BMSK                                          0x1
#define HWIO_MSS_UIM0_BCR_BLK_ARES_SHFT                                          0x0

#define HWIO_MSS_UIM1_BCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001064)
#define HWIO_MSS_UIM1_BCR_RMSK                                                   0x1
#define HWIO_MSS_UIM1_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_BCR_ADDR, HWIO_MSS_UIM1_BCR_RMSK)
#define HWIO_MSS_UIM1_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_BCR_ADDR, m)
#define HWIO_MSS_UIM1_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_BCR_ADDR,v)
#define HWIO_MSS_UIM1_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_BCR_ADDR,m,v,HWIO_MSS_UIM1_BCR_IN)
#define HWIO_MSS_UIM1_BCR_BLK_ARES_BMSK                                          0x1
#define HWIO_MSS_UIM1_BCR_BLK_ARES_SHFT                                          0x0

#define HWIO_MSS_Q6SS_BCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001068)
#define HWIO_MSS_Q6SS_BCR_RMSK                                                   0x1
#define HWIO_MSS_Q6SS_BCR_IN          \
        in_dword_masked(HWIO_MSS_Q6SS_BCR_ADDR, HWIO_MSS_Q6SS_BCR_RMSK)
#define HWIO_MSS_Q6SS_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_Q6SS_BCR_ADDR, m)
#define HWIO_MSS_Q6SS_BCR_OUT(v)      \
        out_dword(HWIO_MSS_Q6SS_BCR_ADDR,v)
#define HWIO_MSS_Q6SS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_Q6SS_BCR_ADDR,m,v,HWIO_MSS_Q6SS_BCR_IN)
#define HWIO_MSS_Q6SS_BCR_BLK_ARES_BMSK                                          0x1
#define HWIO_MSS_Q6SS_BCR_BLK_ARES_SHFT                                          0x0

#define HWIO_MSS_NC_HM_BCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x0000106c)
#define HWIO_MSS_NC_HM_BCR_RMSK                                                  0x1
#define HWIO_MSS_NC_HM_BCR_IN          \
        in_dword_masked(HWIO_MSS_NC_HM_BCR_ADDR, HWIO_MSS_NC_HM_BCR_RMSK)
#define HWIO_MSS_NC_HM_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_NC_HM_BCR_ADDR, m)
#define HWIO_MSS_NC_HM_BCR_OUT(v)      \
        out_dword(HWIO_MSS_NC_HM_BCR_ADDR,v)
#define HWIO_MSS_NC_HM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_NC_HM_BCR_ADDR,m,v,HWIO_MSS_NC_HM_BCR_IN)
#define HWIO_MSS_NC_HM_BCR_BLK_ARES_BMSK                                         0x1
#define HWIO_MSS_NC_HM_BCR_BLK_ARES_SHFT                                         0x0

#define HWIO_MSS_RBCPR_BCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00001070)
#define HWIO_MSS_RBCPR_BCR_RMSK                                                  0x1
#define HWIO_MSS_RBCPR_BCR_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_BCR_ADDR, HWIO_MSS_RBCPR_BCR_RMSK)
#define HWIO_MSS_RBCPR_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_BCR_ADDR, m)
#define HWIO_MSS_RBCPR_BCR_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_BCR_ADDR,v)
#define HWIO_MSS_RBCPR_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_BCR_ADDR,m,v,HWIO_MSS_RBCPR_BCR_IN)
#define HWIO_MSS_RBCPR_BCR_BLK_ARES_BMSK                                         0x1
#define HWIO_MSS_RBCPR_BCR_BLK_ARES_SHFT                                         0x0

#define HWIO_MSS_COXM_BCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001074)
#define HWIO_MSS_COXM_BCR_RMSK                                                   0x1
#define HWIO_MSS_COXM_BCR_IN          \
        in_dword_masked(HWIO_MSS_COXM_BCR_ADDR, HWIO_MSS_COXM_BCR_RMSK)
#define HWIO_MSS_COXM_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_COXM_BCR_ADDR, m)
#define HWIO_MSS_COXM_BCR_OUT(v)      \
        out_dword(HWIO_MSS_COXM_BCR_ADDR,v)
#define HWIO_MSS_COXM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_COXM_BCR_ADDR,m,v,HWIO_MSS_COXM_BCR_IN)
#define HWIO_MSS_COXM_BCR_BLK_ARES_BMSK                                          0x1
#define HWIO_MSS_COXM_BCR_BLK_ARES_SHFT                                          0x0

#define HWIO_MSS_UIM0_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00001078)
#define HWIO_MSS_UIM0_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_CBCR_ADDR, HWIO_MSS_UIM0_CBCR_RMSK)
#define HWIO_MSS_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_CBCR_ADDR,m,v,HWIO_MSS_UIM0_CBCR_IN)
#define HWIO_MSS_UIM0_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_UIM0_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_UIM0_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_UIM0_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_UIM1_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x0000107c)
#define HWIO_MSS_UIM1_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_CBCR_ADDR, HWIO_MSS_UIM1_CBCR_RMSK)
#define HWIO_MSS_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_CBCR_ADDR,m,v,HWIO_MSS_UIM1_CBCR_IN)
#define HWIO_MSS_UIM1_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_UIM1_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_UIM1_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_UIM1_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_XO_UIM0_CBCR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001080)
#define HWIO_MSS_XO_UIM0_CBCR_RMSK                                        0x80000001
#define HWIO_MSS_XO_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_UIM0_CBCR_ADDR, HWIO_MSS_XO_UIM0_CBCR_RMSK)
#define HWIO_MSS_XO_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_XO_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_XO_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_UIM0_CBCR_ADDR,m,v,HWIO_MSS_XO_UIM0_CBCR_IN)
#define HWIO_MSS_XO_UIM0_CBCR_CLKOFF_BMSK                                 0x80000000
#define HWIO_MSS_XO_UIM0_CBCR_CLKOFF_SHFT                                       0x1f
#define HWIO_MSS_XO_UIM0_CBCR_CLKEN_BMSK                                         0x1
#define HWIO_MSS_XO_UIM0_CBCR_CLKEN_SHFT                                         0x0

#define HWIO_MSS_XO_UIM1_CBCR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001084)
#define HWIO_MSS_XO_UIM1_CBCR_RMSK                                        0x80000001
#define HWIO_MSS_XO_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_UIM1_CBCR_ADDR, HWIO_MSS_XO_UIM1_CBCR_RMSK)
#define HWIO_MSS_XO_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_XO_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_XO_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_UIM1_CBCR_ADDR,m,v,HWIO_MSS_XO_UIM1_CBCR_IN)
#define HWIO_MSS_XO_UIM1_CBCR_CLKOFF_BMSK                                 0x80000000
#define HWIO_MSS_XO_UIM1_CBCR_CLKOFF_SHFT                                       0x1f
#define HWIO_MSS_XO_UIM1_CBCR_CLKEN_BMSK                                         0x1
#define HWIO_MSS_XO_UIM1_CBCR_CLKEN_SHFT                                         0x0

#define HWIO_MSS_XO_MODEM_CX_TO_LS_CBCR_ADDR                              (MSS_PERPH_REG_BASE      + 0x00001088)
#define HWIO_MSS_XO_MODEM_CX_TO_LS_CBCR_RMSK                              0x80000000
#define HWIO_MSS_XO_MODEM_CX_TO_LS_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_MODEM_CX_TO_LS_CBCR_ADDR, HWIO_MSS_XO_MODEM_CX_TO_LS_CBCR_RMSK)
#define HWIO_MSS_XO_MODEM_CX_TO_LS_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_MODEM_CX_TO_LS_CBCR_ADDR, m)
#define HWIO_MSS_XO_MODEM_CX_TO_LS_CBCR_CLKOFF_BMSK                       0x80000000
#define HWIO_MSS_XO_MODEM_CX_TO_LS_CBCR_CLKOFF_SHFT                             0x1f

#define HWIO_MSS_XO_Q6_CBCR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x0000108c)
#define HWIO_MSS_XO_Q6_CBCR_RMSK                                          0x80000001
#define HWIO_MSS_XO_Q6_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_Q6_CBCR_ADDR, HWIO_MSS_XO_Q6_CBCR_RMSK)
#define HWIO_MSS_XO_Q6_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_Q6_CBCR_ADDR, m)
#define HWIO_MSS_XO_Q6_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_Q6_CBCR_ADDR,v)
#define HWIO_MSS_XO_Q6_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_Q6_CBCR_ADDR,m,v,HWIO_MSS_XO_Q6_CBCR_IN)
#define HWIO_MSS_XO_Q6_CBCR_CLKOFF_BMSK                                   0x80000000
#define HWIO_MSS_XO_Q6_CBCR_CLKOFF_SHFT                                         0x1f
#define HWIO_MSS_XO_Q6_CBCR_CLKEN_BMSK                                           0x1
#define HWIO_MSS_XO_Q6_CBCR_CLKEN_SHFT                                           0x0

#define HWIO_MSS_BUS_UIM0_CBCR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00001090)
#define HWIO_MSS_BUS_UIM0_CBCR_RMSK                                       0x80007ff1
#define HWIO_MSS_BUS_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM0_CBCR_ADDR, HWIO_MSS_BUS_UIM0_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM0_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM0_CBCR_IN)
#define HWIO_MSS_BUS_UIM0_CBCR_CLKOFF_BMSK                                0x80000000
#define HWIO_MSS_BUS_UIM0_CBCR_CLKOFF_SHFT                                      0x1f
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_CORE_ON_BMSK                         0x4000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_CORE_ON_SHFT                            0xe
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_ON_BMSK                       0x2000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_ON_SHFT                          0xd
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                      0x1000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                         0xc
#define HWIO_MSS_BUS_UIM0_CBCR_WAKEUP_BMSK                                     0xf00
#define HWIO_MSS_BUS_UIM0_CBCR_WAKEUP_SHFT                                       0x8
#define HWIO_MSS_BUS_UIM0_CBCR_SLEEP_BMSK                                       0xf0
#define HWIO_MSS_BUS_UIM0_CBCR_SLEEP_SHFT                                        0x4
#define HWIO_MSS_BUS_UIM0_CBCR_CLKEN_BMSK                                        0x1
#define HWIO_MSS_BUS_UIM0_CBCR_CLKEN_SHFT                                        0x0

#define HWIO_MSS_BUS_UIM1_CBCR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00001094)
#define HWIO_MSS_BUS_UIM1_CBCR_RMSK                                       0x80007ff1
#define HWIO_MSS_BUS_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM1_CBCR_ADDR, HWIO_MSS_BUS_UIM1_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM1_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM1_CBCR_IN)
#define HWIO_MSS_BUS_UIM1_CBCR_CLKOFF_BMSK                                0x80000000
#define HWIO_MSS_BUS_UIM1_CBCR_CLKOFF_SHFT                                      0x1f
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_CORE_ON_BMSK                         0x4000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_CORE_ON_SHFT                            0xe
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_ON_BMSK                       0x2000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_ON_SHFT                          0xd
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                      0x1000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                         0xc
#define HWIO_MSS_BUS_UIM1_CBCR_WAKEUP_BMSK                                     0xf00
#define HWIO_MSS_BUS_UIM1_CBCR_WAKEUP_SHFT                                       0x8
#define HWIO_MSS_BUS_UIM1_CBCR_SLEEP_BMSK                                       0xf0
#define HWIO_MSS_BUS_UIM1_CBCR_SLEEP_SHFT                                        0x4
#define HWIO_MSS_BUS_UIM1_CBCR_CLKEN_BMSK                                        0x1
#define HWIO_MSS_BUS_UIM1_CBCR_CLKEN_SHFT                                        0x0

#define HWIO_MSS_BUS_CSR_CBCR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001098)
#define HWIO_MSS_BUS_CSR_CBCR_RMSK                                        0x80000001
#define HWIO_MSS_BUS_CSR_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CSR_CBCR_ADDR, HWIO_MSS_BUS_CSR_CBCR_RMSK)
#define HWIO_MSS_BUS_CSR_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CSR_CBCR_ADDR, m)
#define HWIO_MSS_BUS_CSR_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CSR_CBCR_ADDR,v)
#define HWIO_MSS_BUS_CSR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CSR_CBCR_ADDR,m,v,HWIO_MSS_BUS_CSR_CBCR_IN)
#define HWIO_MSS_BUS_CSR_CBCR_CLKOFF_BMSK                                 0x80000000
#define HWIO_MSS_BUS_CSR_CBCR_CLKOFF_SHFT                                       0x1f
#define HWIO_MSS_BUS_CSR_CBCR_CLKEN_BMSK                                         0x1
#define HWIO_MSS_BUS_CSR_CBCR_CLKEN_SHFT                                         0x0

#define HWIO_MSS_BUS_BRIDGE_CBCR_ADDR                                     (MSS_PERPH_REG_BASE      + 0x0000109c)
#define HWIO_MSS_BUS_BRIDGE_CBCR_RMSK                                     0x80000001
#define HWIO_MSS_BUS_BRIDGE_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_BRIDGE_CBCR_ADDR, HWIO_MSS_BUS_BRIDGE_CBCR_RMSK)
#define HWIO_MSS_BUS_BRIDGE_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_BRIDGE_CBCR_ADDR, m)
#define HWIO_MSS_BUS_BRIDGE_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_BRIDGE_CBCR_ADDR,v)
#define HWIO_MSS_BUS_BRIDGE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_BRIDGE_CBCR_ADDR,m,v,HWIO_MSS_BUS_BRIDGE_CBCR_IN)
#define HWIO_MSS_BUS_BRIDGE_CBCR_CLKOFF_BMSK                              0x80000000
#define HWIO_MSS_BUS_BRIDGE_CBCR_CLKOFF_SHFT                                    0x1f
#define HWIO_MSS_BUS_BRIDGE_CBCR_CLKEN_BMSK                                      0x1
#define HWIO_MSS_BUS_BRIDGE_CBCR_CLKEN_SHFT                                      0x0

#define HWIO_MSS_BUS_MODEM_CX_TO_LS_CBCR_ADDR                             (MSS_PERPH_REG_BASE      + 0x000010a0)
#define HWIO_MSS_BUS_MODEM_CX_TO_LS_CBCR_RMSK                             0x80000000
#define HWIO_MSS_BUS_MODEM_CX_TO_LS_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_MODEM_CX_TO_LS_CBCR_ADDR, HWIO_MSS_BUS_MODEM_CX_TO_LS_CBCR_RMSK)
#define HWIO_MSS_BUS_MODEM_CX_TO_LS_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MODEM_CX_TO_LS_CBCR_ADDR, m)
#define HWIO_MSS_BUS_MODEM_CX_TO_LS_CBCR_CLKOFF_BMSK                      0x80000000
#define HWIO_MSS_BUS_MODEM_CX_TO_LS_CBCR_CLKOFF_SHFT                            0x1f

#define HWIO_MSS_BUS_Q6_CBCR_ADDR                                         (MSS_PERPH_REG_BASE      + 0x000010a4)
#define HWIO_MSS_BUS_Q6_CBCR_RMSK                                         0x80000001
#define HWIO_MSS_BUS_Q6_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_Q6_CBCR_ADDR, HWIO_MSS_BUS_Q6_CBCR_RMSK)
#define HWIO_MSS_BUS_Q6_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_Q6_CBCR_ADDR, m)
#define HWIO_MSS_BUS_Q6_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_Q6_CBCR_ADDR,v)
#define HWIO_MSS_BUS_Q6_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_Q6_CBCR_ADDR,m,v,HWIO_MSS_BUS_Q6_CBCR_IN)
#define HWIO_MSS_BUS_Q6_CBCR_CLKOFF_BMSK                                  0x80000000
#define HWIO_MSS_BUS_Q6_CBCR_CLKOFF_SHFT                                        0x1f
#define HWIO_MSS_BUS_Q6_CBCR_CLKEN_BMSK                                          0x1
#define HWIO_MSS_BUS_Q6_CBCR_CLKEN_SHFT                                          0x0

#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_ADDR                               (MSS_PERPH_REG_BASE      + 0x000010a8)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_RMSK                               0x80000001
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_ADDR, HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_RMSK)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_ADDR, m)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_ADDR,v)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_ADDR,m,v,HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_IN)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_CLKOFF_BMSK                        0x80000000
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_CLKOFF_SHFT                              0x1f
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_CLKEN_BMSK                                0x1
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_CLKEN_SHFT                                0x0

#define HWIO_MSS_BUS_CRYPTO_CBCR_ADDR                                     (MSS_PERPH_REG_BASE      + 0x000010ac)
#define HWIO_MSS_BUS_CRYPTO_CBCR_RMSK                                     0x80000001
#define HWIO_MSS_BUS_CRYPTO_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR, HWIO_MSS_BUS_CRYPTO_CBCR_RMSK)
#define HWIO_MSS_BUS_CRYPTO_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR, m)
#define HWIO_MSS_BUS_CRYPTO_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR,v)
#define HWIO_MSS_BUS_CRYPTO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR,m,v,HWIO_MSS_BUS_CRYPTO_CBCR_IN)
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKOFF_BMSK                              0x80000000
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKOFF_SHFT                                    0x1f
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKEN_BMSK                                      0x1
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKEN_SHFT                                      0x0

#define HWIO_MSS_BUS_NAV_CBCR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x000010b0)
#define HWIO_MSS_BUS_NAV_CBCR_RMSK                                        0x80000001
#define HWIO_MSS_BUS_NAV_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_NAV_CBCR_ADDR, HWIO_MSS_BUS_NAV_CBCR_RMSK)
#define HWIO_MSS_BUS_NAV_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_NAV_CBCR_ADDR, m)
#define HWIO_MSS_BUS_NAV_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_NAV_CBCR_ADDR,v)
#define HWIO_MSS_BUS_NAV_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_NAV_CBCR_ADDR,m,v,HWIO_MSS_BUS_NAV_CBCR_IN)
#define HWIO_MSS_BUS_NAV_CBCR_CLKOFF_BMSK                                 0x80000000
#define HWIO_MSS_BUS_NAV_CBCR_CLKOFF_SHFT                                       0x1f
#define HWIO_MSS_BUS_NAV_CBCR_CLKEN_BMSK                                         0x1
#define HWIO_MSS_BUS_NAV_CBCR_CLKEN_SHFT                                         0x0

#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_ADDR                            (MSS_PERPH_REG_BASE      + 0x000010b4)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_RMSK                            0x80000001
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_ADDR, HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_RMSK)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_ADDR, m)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_ADDR,v)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_ADDR,m,v,HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_IN)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_CLKOFF_BMSK                     0x80000000
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_CLKOFF_SHFT                           0x1f
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_CLKEN_BMSK                             0x1
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_CLKEN_SHFT                             0x0

#define HWIO_MSS_BUS_ATB_CBCR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x000010b8)
#define HWIO_MSS_BUS_ATB_CBCR_RMSK                                        0x80000001
#define HWIO_MSS_BUS_ATB_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_ATB_CBCR_ADDR, HWIO_MSS_BUS_ATB_CBCR_RMSK)
#define HWIO_MSS_BUS_ATB_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_ATB_CBCR_ADDR, m)
#define HWIO_MSS_BUS_ATB_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_ATB_CBCR_ADDR,v)
#define HWIO_MSS_BUS_ATB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_ATB_CBCR_ADDR,m,v,HWIO_MSS_BUS_ATB_CBCR_IN)
#define HWIO_MSS_BUS_ATB_CBCR_CLKOFF_BMSK                                 0x80000000
#define HWIO_MSS_BUS_ATB_CBCR_CLKOFF_SHFT                                       0x1f
#define HWIO_MSS_BUS_ATB_CBCR_CLKEN_BMSK                                         0x1
#define HWIO_MSS_BUS_ATB_CBCR_CLKEN_SHFT                                         0x0

#define HWIO_MSS_BUS_COXM_CBCR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x000010bc)
#define HWIO_MSS_BUS_COXM_CBCR_RMSK                                       0x80007ff1
#define HWIO_MSS_BUS_COXM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_COXM_CBCR_ADDR, HWIO_MSS_BUS_COXM_CBCR_RMSK)
#define HWIO_MSS_BUS_COXM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_COXM_CBCR_ADDR, m)
#define HWIO_MSS_BUS_COXM_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_COXM_CBCR_ADDR,v)
#define HWIO_MSS_BUS_COXM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_COXM_CBCR_ADDR,m,v,HWIO_MSS_BUS_COXM_CBCR_IN)
#define HWIO_MSS_BUS_COXM_CBCR_CLKOFF_BMSK                                0x80000000
#define HWIO_MSS_BUS_COXM_CBCR_CLKOFF_SHFT                                      0x1f
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_CORE_ON_BMSK                         0x4000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_CORE_ON_SHFT                            0xe
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_ON_BMSK                       0x2000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_ON_SHFT                          0xd
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                      0x1000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                         0xc
#define HWIO_MSS_BUS_COXM_CBCR_WAKEUP_BMSK                                     0xf00
#define HWIO_MSS_BUS_COXM_CBCR_WAKEUP_SHFT                                       0x8
#define HWIO_MSS_BUS_COXM_CBCR_SLEEP_BMSK                                       0xf0
#define HWIO_MSS_BUS_COXM_CBCR_SLEEP_SHFT                                        0x4
#define HWIO_MSS_BUS_COXM_CBCR_CLKEN_BMSK                                        0x1
#define HWIO_MSS_BUS_COXM_CBCR_CLKEN_SHFT                                        0x0

#define HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_ADDR                            (MSS_PERPH_REG_BASE      + 0x000010c0)
#define HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_RMSK                            0x80000000
#define HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_ADDR, HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_RMSK)
#define HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_ADDR, m)
#define HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_CLKOFF_BMSK                     0x80000000
#define HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_CLKOFF_SHFT                           0x1f

#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR                              (MSS_PERPH_REG_BASE      + 0x000010c4)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_RMSK                              0x80000001
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR, HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_RMSK)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR, m)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR,v)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR,m,v,HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_IN)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKOFF_BMSK                       0x80000000
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKOFF_SHFT                             0x1f
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKEN_BMSK                               0x1
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKEN_SHFT                               0x0

#define HWIO_MSS_BUS_RBCPR_CBCR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x000010c8)
#define HWIO_MSS_BUS_RBCPR_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_BUS_RBCPR_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_CBCR_ADDR, HWIO_MSS_BUS_RBCPR_CBCR_RMSK)
#define HWIO_MSS_BUS_RBCPR_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_CBCR_ADDR, m)
#define HWIO_MSS_BUS_RBCPR_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_RBCPR_CBCR_ADDR,v)
#define HWIO_MSS_BUS_RBCPR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_RBCPR_CBCR_ADDR,m,v,HWIO_MSS_BUS_RBCPR_CBCR_IN)
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_BUS_COMBODAC_COMP_CBCR_ADDR                              (MSS_PERPH_REG_BASE      + 0x000010cc)
#define HWIO_MSS_BUS_COMBODAC_COMP_CBCR_RMSK                              0x80000000
#define HWIO_MSS_BUS_COMBODAC_COMP_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_COMBODAC_COMP_CBCR_ADDR, HWIO_MSS_BUS_COMBODAC_COMP_CBCR_RMSK)
#define HWIO_MSS_BUS_COMBODAC_COMP_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_COMBODAC_COMP_CBCR_ADDR, m)
#define HWIO_MSS_BUS_COMBODAC_COMP_CBCR_CLKOFF_BMSK                       0x80000000
#define HWIO_MSS_BUS_COMBODAC_COMP_CBCR_CLKOFF_SHFT                             0x1f

#define HWIO_MSS_BUS_RBCPR_REF_CBCR_ADDR                                  (MSS_PERPH_REG_BASE      + 0x000010d0)
#define HWIO_MSS_BUS_RBCPR_REF_CBCR_RMSK                                  0x80000001
#define HWIO_MSS_BUS_RBCPR_REF_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_REF_CBCR_ADDR, HWIO_MSS_BUS_RBCPR_REF_CBCR_RMSK)
#define HWIO_MSS_BUS_RBCPR_REF_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_REF_CBCR_ADDR, m)
#define HWIO_MSS_BUS_RBCPR_REF_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_RBCPR_REF_CBCR_ADDR,v)
#define HWIO_MSS_BUS_RBCPR_REF_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_RBCPR_REF_CBCR_ADDR,m,v,HWIO_MSS_BUS_RBCPR_REF_CBCR_IN)
#define HWIO_MSS_BUS_RBCPR_REF_CBCR_CLKOFF_BMSK                           0x80000000
#define HWIO_MSS_BUS_RBCPR_REF_CBCR_CLKOFF_SHFT                                 0x1f
#define HWIO_MSS_BUS_RBCPR_REF_CBCR_CLKEN_BMSK                                   0x1
#define HWIO_MSS_BUS_RBCPR_REF_CBCR_CLKEN_SHFT                                   0x0

#define HWIO_MSS_MODEM_CFG_AHB_CBCR_ADDR                                  (MSS_PERPH_REG_BASE      + 0x000010d4)
#define HWIO_MSS_MODEM_CFG_AHB_CBCR_RMSK                                  0x80000000
#define HWIO_MSS_MODEM_CFG_AHB_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MODEM_CFG_AHB_CBCR_ADDR, HWIO_MSS_MODEM_CFG_AHB_CBCR_RMSK)
#define HWIO_MSS_MODEM_CFG_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MODEM_CFG_AHB_CBCR_ADDR, m)
#define HWIO_MSS_MODEM_CFG_AHB_CBCR_CLKOFF_BMSK                           0x80000000
#define HWIO_MSS_MODEM_CFG_AHB_CBCR_CLKOFF_SHFT                                 0x1f

#define HWIO_MSS_MODEM_SNOC_AXI_CBCR_ADDR                                 (MSS_PERPH_REG_BASE      + 0x000010d8)
#define HWIO_MSS_MODEM_SNOC_AXI_CBCR_RMSK                                 0x80000000
#define HWIO_MSS_MODEM_SNOC_AXI_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MODEM_SNOC_AXI_CBCR_ADDR, HWIO_MSS_MODEM_SNOC_AXI_CBCR_RMSK)
#define HWIO_MSS_MODEM_SNOC_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MODEM_SNOC_AXI_CBCR_ADDR, m)
#define HWIO_MSS_MODEM_SNOC_AXI_CBCR_CLKOFF_BMSK                          0x80000000
#define HWIO_MSS_MODEM_SNOC_AXI_CBCR_CLKOFF_SHFT                                0x1f

#define HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR                                   (MSS_PERPH_REG_BASE      + 0x000010dc)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_RMSK                                   0x80000001
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_IN          \
        in_dword_masked(HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR, HWIO_MSS_NAV_SNOC_AXI_CBCR_RMSK)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR, m)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR,v)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR,m,v,HWIO_MSS_NAV_SNOC_AXI_CBCR_IN)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_CLKOFF_BMSK                            0x80000000
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_CLKOFF_SHFT                                  0x1f
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_CLKEN_BMSK                                    0x1
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_CLKEN_SHFT                                    0x0

#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_ADDR                                (MSS_PERPH_REG_BASE      + 0x000010e0)
#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_RMSK                                0x80000001
#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_IN          \
        in_dword_masked(HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_ADDR, HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_RMSK)
#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_ADDR, m)
#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_ADDR,v)
#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_ADDR,m,v,HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_IN)
#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_CLKOFF_BMSK                         0x80000000
#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_CLKOFF_SHFT                               0x1f
#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_CLKEN_BMSK                                 0x1
#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_CLKEN_SHFT                                 0x0

#define HWIO_MSS_MPLL0_MAIN_MODEM_CX_TO_LS_CBCR_ADDR                      (MSS_PERPH_REG_BASE      + 0x000010e4)
#define HWIO_MSS_MPLL0_MAIN_MODEM_CX_TO_LS_CBCR_RMSK                      0x80000001
#define HWIO_MSS_MPLL0_MAIN_MODEM_CX_TO_LS_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL0_MAIN_MODEM_CX_TO_LS_CBCR_ADDR, HWIO_MSS_MPLL0_MAIN_MODEM_CX_TO_LS_CBCR_RMSK)
#define HWIO_MSS_MPLL0_MAIN_MODEM_CX_TO_LS_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL0_MAIN_MODEM_CX_TO_LS_CBCR_ADDR, m)
#define HWIO_MSS_MPLL0_MAIN_MODEM_CX_TO_LS_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_MPLL0_MAIN_MODEM_CX_TO_LS_CBCR_ADDR,v)
#define HWIO_MSS_MPLL0_MAIN_MODEM_CX_TO_LS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL0_MAIN_MODEM_CX_TO_LS_CBCR_ADDR,m,v,HWIO_MSS_MPLL0_MAIN_MODEM_CX_TO_LS_CBCR_IN)
#define HWIO_MSS_MPLL0_MAIN_MODEM_CX_TO_LS_CBCR_CLKOFF_BMSK               0x80000000
#define HWIO_MSS_MPLL0_MAIN_MODEM_CX_TO_LS_CBCR_CLKOFF_SHFT                     0x1f
#define HWIO_MSS_MPLL0_MAIN_MODEM_CX_TO_LS_CBCR_CLKEN_BMSK                       0x1
#define HWIO_MSS_MPLL0_MAIN_MODEM_CX_TO_LS_CBCR_CLKEN_SHFT                       0x0

#define HWIO_MSS_MPLL1_MAIN_CBCR_ADDR                                     (MSS_PERPH_REG_BASE      + 0x000010e8)
#define HWIO_MSS_MPLL1_MAIN_CBCR_RMSK                                     0x80000001
#define HWIO_MSS_MPLL1_MAIN_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_MAIN_CBCR_ADDR, HWIO_MSS_MPLL1_MAIN_CBCR_RMSK)
#define HWIO_MSS_MPLL1_MAIN_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_MAIN_CBCR_ADDR, m)
#define HWIO_MSS_MPLL1_MAIN_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_MAIN_CBCR_ADDR,v)
#define HWIO_MSS_MPLL1_MAIN_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_MAIN_CBCR_ADDR,m,v,HWIO_MSS_MPLL1_MAIN_CBCR_IN)
#define HWIO_MSS_MPLL1_MAIN_CBCR_CLKOFF_BMSK                              0x80000000
#define HWIO_MSS_MPLL1_MAIN_CBCR_CLKOFF_SHFT                                    0x1f
#define HWIO_MSS_MPLL1_MAIN_CBCR_CLKEN_BMSK                                      0x1
#define HWIO_MSS_MPLL1_MAIN_CBCR_CLKEN_SHFT                                      0x0

#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_ADDR                                 (MSS_PERPH_REG_BASE      + 0x000010ec)
#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_RMSK                                 0x80000000
#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_MAIN_BUS_CBCR_ADDR, HWIO_MSS_MPLL1_MAIN_BUS_CBCR_RMSK)
#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_MAIN_BUS_CBCR_ADDR, m)
#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_CLKOFF_BMSK                          0x80000000
#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_CLKOFF_SHFT                                0x1f

#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_ADDR                               (MSS_PERPH_REG_BASE      + 0x000010f0)
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_RMSK                               0x80000001
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_ADDR, HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_RMSK)
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_ADDR, m)
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_ADDR,v)
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_ADDR,m,v,HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_IN)
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_CLKOFF_BMSK                        0x80000000
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_CLKOFF_SHFT                              0x1f
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_CLKEN_BMSK                                0x1
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_CLKEN_SHFT                                0x0

#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_ADDR                               (MSS_PERPH_REG_BASE      + 0x000010f4)
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_RMSK                               0x80000001
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_ADDR, HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_RMSK)
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_ADDR, m)
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_ADDR,v)
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_ADDR,m,v,HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_IN)
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_CLKOFF_BMSK                        0x80000000
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_CLKOFF_SHFT                              0x1f
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_CLKEN_BMSK                                0x1
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_CLKEN_SHFT                                0x0

#define HWIO_MSS_BIT_COXM_CBCR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x000010f8)
#define HWIO_MSS_BIT_COXM_CBCR_RMSK                                       0x80000001
#define HWIO_MSS_BIT_COXM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_CBCR_ADDR, HWIO_MSS_BIT_COXM_CBCR_RMSK)
#define HWIO_MSS_BIT_COXM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_CBCR_ADDR, m)
#define HWIO_MSS_BIT_COXM_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_CBCR_ADDR,v)
#define HWIO_MSS_BIT_COXM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_CBCR_ADDR,m,v,HWIO_MSS_BIT_COXM_CBCR_IN)
#define HWIO_MSS_BIT_COXM_CBCR_CLKOFF_BMSK                                0x80000000
#define HWIO_MSS_BIT_COXM_CBCR_CLKOFF_SHFT                                      0x1f
#define HWIO_MSS_BIT_COXM_CBCR_CLKEN_BMSK                                        0x1
#define HWIO_MSS_BIT_COXM_CBCR_CLKEN_SHFT                                        0x0

#define HWIO_MSS_SLEEP_Q6_CBCR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x000010fc)
#define HWIO_MSS_SLEEP_Q6_CBCR_RMSK                                       0x80000001
#define HWIO_MSS_SLEEP_Q6_CBCR_IN          \
        in_dword_masked(HWIO_MSS_SLEEP_Q6_CBCR_ADDR, HWIO_MSS_SLEEP_Q6_CBCR_RMSK)
#define HWIO_MSS_SLEEP_Q6_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_SLEEP_Q6_CBCR_ADDR, m)
#define HWIO_MSS_SLEEP_Q6_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_SLEEP_Q6_CBCR_ADDR,v)
#define HWIO_MSS_SLEEP_Q6_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_SLEEP_Q6_CBCR_ADDR,m,v,HWIO_MSS_SLEEP_Q6_CBCR_IN)
#define HWIO_MSS_SLEEP_Q6_CBCR_CLKOFF_BMSK                                0x80000000
#define HWIO_MSS_SLEEP_Q6_CBCR_CLKOFF_SHFT                                      0x1f
#define HWIO_MSS_SLEEP_Q6_CBCR_CLKEN_BMSK                                        0x1
#define HWIO_MSS_SLEEP_Q6_CBCR_CLKEN_SHFT                                        0x0

#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_ADDR                           (MSS_PERPH_REG_BASE      + 0x00001100)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_RMSK                           0x80000001
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_ADDR, HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_RMSK)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_ADDR, m)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_ADDR,v)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_ADDR,m,v,HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_IN)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_CLKOFF_BMSK                    0x80000000
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_CLKOFF_SHFT                          0x1f
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_CLKEN_BMSK                            0x1
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_CLKEN_SHFT                            0x0

#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_ADDR                           (MSS_PERPH_REG_BASE      + 0x00001104)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_RMSK                           0x80000001
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_ADDR, HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_RMSK)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_ADDR, m)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_ADDR,v)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_ADDR,m,v,HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_IN)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_CLKOFF_BMSK                    0x80000000
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_CLKOFF_SHFT                          0x1f
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_CLKEN_BMSK                            0x1
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_CLKEN_SHFT                            0x0

#define HWIO_MSS_BUS_CMD_RCGR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001108)
#define HWIO_MSS_BUS_CMD_RCGR_RMSK                                        0x80000001
#define HWIO_MSS_BUS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CMD_RCGR_ADDR, HWIO_MSS_BUS_CMD_RCGR_RMSK)
#define HWIO_MSS_BUS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CMD_RCGR_ADDR, m)
#define HWIO_MSS_BUS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CMD_RCGR_ADDR,v)
#define HWIO_MSS_BUS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CMD_RCGR_ADDR,m,v,HWIO_MSS_BUS_CMD_RCGR_IN)
#define HWIO_MSS_BUS_CMD_RCGR_ROOT_OFF_BMSK                               0x80000000
#define HWIO_MSS_BUS_CMD_RCGR_ROOT_OFF_SHFT                                     0x1f
#define HWIO_MSS_BUS_CMD_RCGR_UPDATE_BMSK                                        0x1
#define HWIO_MSS_BUS_CMD_RCGR_UPDATE_SHFT                                        0x0

#define HWIO_MSS_BUS_CFG_RCGR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x0000110c)
#define HWIO_MSS_BUS_CFG_RCGR_RMSK                                             0x71f
#define HWIO_MSS_BUS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CFG_RCGR_ADDR, HWIO_MSS_BUS_CFG_RCGR_RMSK)
#define HWIO_MSS_BUS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CFG_RCGR_ADDR, m)
#define HWIO_MSS_BUS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CFG_RCGR_ADDR,v)
#define HWIO_MSS_BUS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CFG_RCGR_ADDR,m,v,HWIO_MSS_BUS_CFG_RCGR_IN)
#define HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_BMSK                                     0x700
#define HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_SHFT                                       0x8
#define HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_BMSK                                      0x1f
#define HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_SHFT                                       0x0

#define HWIO_MSS_Q6_CMD_RCGR_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001110)
#define HWIO_MSS_Q6_CMD_RCGR_RMSK                                         0x80000003
#define HWIO_MSS_Q6_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_Q6_CMD_RCGR_ADDR, HWIO_MSS_Q6_CMD_RCGR_RMSK)
#define HWIO_MSS_Q6_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_Q6_CMD_RCGR_ADDR, m)
#define HWIO_MSS_Q6_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_Q6_CMD_RCGR_ADDR,v)
#define HWIO_MSS_Q6_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_Q6_CMD_RCGR_ADDR,m,v,HWIO_MSS_Q6_CMD_RCGR_IN)
#define HWIO_MSS_Q6_CMD_RCGR_ROOT_OFF_BMSK                                0x80000000
#define HWIO_MSS_Q6_CMD_RCGR_ROOT_OFF_SHFT                                      0x1f
#define HWIO_MSS_Q6_CMD_RCGR_ROOT_EN_BMSK                                        0x2
#define HWIO_MSS_Q6_CMD_RCGR_ROOT_EN_SHFT                                        0x1
#define HWIO_MSS_Q6_CMD_RCGR_UPDATE_BMSK                                         0x1
#define HWIO_MSS_Q6_CMD_RCGR_UPDATE_SHFT                                         0x0

#define HWIO_MSS_Q6_CFG_RCGR_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001114)
#define HWIO_MSS_Q6_CFG_RCGR_RMSK                                              0x71f
#define HWIO_MSS_Q6_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_Q6_CFG_RCGR_ADDR, HWIO_MSS_Q6_CFG_RCGR_RMSK)
#define HWIO_MSS_Q6_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_Q6_CFG_RCGR_ADDR, m)
#define HWIO_MSS_Q6_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_Q6_CFG_RCGR_ADDR,v)
#define HWIO_MSS_Q6_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_Q6_CFG_RCGR_ADDR,m,v,HWIO_MSS_Q6_CFG_RCGR_IN)
#define HWIO_MSS_Q6_CFG_RCGR_SRC_SEL_BMSK                                      0x700
#define HWIO_MSS_Q6_CFG_RCGR_SRC_SEL_SHFT                                        0x8
#define HWIO_MSS_Q6_CFG_RCGR_SRC_DIV_BMSK                                       0x1f
#define HWIO_MSS_Q6_CFG_RCGR_SRC_DIV_SHFT                                        0x0

#define HWIO_MSS_UIM_CMD_RCGR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001118)
#define HWIO_MSS_UIM_CMD_RCGR_RMSK                                        0x80000003
#define HWIO_MSS_UIM_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM_CMD_RCGR_ADDR, HWIO_MSS_UIM_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM_CMD_RCGR_IN)
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_OFF_BMSK                               0x80000000
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_OFF_SHFT                                     0x1f
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_EN_BMSK                                       0x2
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_EN_SHFT                                       0x1
#define HWIO_MSS_UIM_CMD_RCGR_UPDATE_BMSK                                        0x1
#define HWIO_MSS_UIM_CMD_RCGR_UPDATE_SHFT                                        0x0

#define HWIO_MSS_UIM_CFG_RCGR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x0000111c)
#define HWIO_MSS_UIM_CFG_RCGR_RMSK                                              0x1f
#define HWIO_MSS_UIM_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM_CFG_RCGR_ADDR, HWIO_MSS_UIM_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM_CFG_RCGR_IN)
#define HWIO_MSS_UIM_CFG_RCGR_SRC_DIV_BMSK                                      0x1f
#define HWIO_MSS_UIM_CFG_RCGR_SRC_DIV_SHFT                                       0x0

#define HWIO_MSS_BBRX0_ACGC_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001120)
#define HWIO_MSS_BBRX0_ACGC_RMSK                                                 0x2
#define HWIO_MSS_BBRX0_ACGC_IN          \
        in_dword_masked(HWIO_MSS_BBRX0_ACGC_ADDR, HWIO_MSS_BBRX0_ACGC_RMSK)
#define HWIO_MSS_BBRX0_ACGC_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX0_ACGC_ADDR, m)
#define HWIO_MSS_BBRX0_ACGC_CLKOFF_BMSK                                          0x2
#define HWIO_MSS_BBRX0_ACGC_CLKOFF_SHFT                                          0x1

#define HWIO_MSS_BBRX1_ACGC_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001124)
#define HWIO_MSS_BBRX1_ACGC_RMSK                                                 0x2
#define HWIO_MSS_BBRX1_ACGC_IN          \
        in_dword_masked(HWIO_MSS_BBRX1_ACGC_ADDR, HWIO_MSS_BBRX1_ACGC_RMSK)
#define HWIO_MSS_BBRX1_ACGC_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX1_ACGC_ADDR, m)
#define HWIO_MSS_BBRX1_ACGC_CLKOFF_BMSK                                          0x2
#define HWIO_MSS_BBRX1_ACGC_CLKOFF_SHFT                                          0x1

#define HWIO_MSS_BBRX2_ACGC_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001128)
#define HWIO_MSS_BBRX2_ACGC_RMSK                                                 0x2
#define HWIO_MSS_BBRX2_ACGC_IN          \
        in_dword_masked(HWIO_MSS_BBRX2_ACGC_ADDR, HWIO_MSS_BBRX2_ACGC_RMSK)
#define HWIO_MSS_BBRX2_ACGC_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX2_ACGC_ADDR, m)
#define HWIO_MSS_BBRX2_ACGC_CLKOFF_BMSK                                          0x2
#define HWIO_MSS_BBRX2_ACGC_CLKOFF_SHFT                                          0x1

#define HWIO_MSS_BBRX3_ACGC_ADDR                                          (MSS_PERPH_REG_BASE      + 0x0000112c)
#define HWIO_MSS_BBRX3_ACGC_RMSK                                                 0x2
#define HWIO_MSS_BBRX3_ACGC_IN          \
        in_dword_masked(HWIO_MSS_BBRX3_ACGC_ADDR, HWIO_MSS_BBRX3_ACGC_RMSK)
#define HWIO_MSS_BBRX3_ACGC_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX3_ACGC_ADDR, m)
#define HWIO_MSS_BBRX3_ACGC_CLKOFF_BMSK                                          0x2
#define HWIO_MSS_BBRX3_ACGC_CLKOFF_SHFT                                          0x1

#define HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_ADDR                               (MSS_PERPH_REG_BASE      + 0x00001130)
#define HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_RMSK                               0x8fffffff
#define HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_ADDR, HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_IN)
#define HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_ROOT_OFF_BMSK                      0x80000000
#define HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_ROOT_OFF_SHFT                            0x1f
#define HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_NOT_N_MINUS_M_BMSK                  0xffff000
#define HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_NOT_N_MINUS_M_SHFT                        0xc
#define HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_M_VALUE_BMSK                            0xff0
#define HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_M_VALUE_SHFT                              0x4
#define HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_MODE_BMSK                                 0xc
#define HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_MODE_SHFT                                 0x2
#define HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_ROOT_EN_BMSK                              0x2
#define HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_ROOT_EN_SHFT                              0x1
#define HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_UPDATE_BMSK                               0x1
#define HWIO_MSS_UIM0_MND_CMD_CFG_RCGR_UPDATE_SHFT                               0x0

#define HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_ADDR                               (MSS_PERPH_REG_BASE      + 0x00001134)
#define HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_RMSK                               0x8fffffff
#define HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_ADDR, HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_IN)
#define HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_ROOT_OFF_BMSK                      0x80000000
#define HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_ROOT_OFF_SHFT                            0x1f
#define HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_NOT_N_MINUS_M_BMSK                  0xffff000
#define HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_NOT_N_MINUS_M_SHFT                        0xc
#define HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_M_VALUE_BMSK                            0xff0
#define HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_M_VALUE_SHFT                              0x4
#define HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_MODE_BMSK                                 0xc
#define HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_MODE_SHFT                                 0x2
#define HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_ROOT_EN_BMSK                              0x2
#define HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_ROOT_EN_SHFT                              0x1
#define HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_UPDATE_BMSK                               0x1
#define HWIO_MSS_UIM1_MND_CMD_CFG_RCGR_UPDATE_SHFT                               0x0

#define HWIO_MSS_BBRX0_MISC_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001138)
#define HWIO_MSS_BBRX0_MISC_RMSK                                                 0xf
#define HWIO_MSS_BBRX0_MISC_IN          \
        in_dword_masked(HWIO_MSS_BBRX0_MISC_ADDR, HWIO_MSS_BBRX0_MISC_RMSK)
#define HWIO_MSS_BBRX0_MISC_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX0_MISC_ADDR, m)
#define HWIO_MSS_BBRX0_MISC_OUT(v)      \
        out_dword(HWIO_MSS_BBRX0_MISC_ADDR,v)
#define HWIO_MSS_BBRX0_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX0_MISC_ADDR,m,v,HWIO_MSS_BBRX0_MISC_IN)
#define HWIO_MSS_BBRX0_MISC_SRC_DIV_BMSK                                         0xf
#define HWIO_MSS_BBRX0_MISC_SRC_DIV_SHFT                                         0x0

#define HWIO_MSS_BBRX1_MISC_ADDR                                          (MSS_PERPH_REG_BASE      + 0x0000113c)
#define HWIO_MSS_BBRX1_MISC_RMSK                                                 0xf
#define HWIO_MSS_BBRX1_MISC_IN          \
        in_dword_masked(HWIO_MSS_BBRX1_MISC_ADDR, HWIO_MSS_BBRX1_MISC_RMSK)
#define HWIO_MSS_BBRX1_MISC_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX1_MISC_ADDR, m)
#define HWIO_MSS_BBRX1_MISC_OUT(v)      \
        out_dword(HWIO_MSS_BBRX1_MISC_ADDR,v)
#define HWIO_MSS_BBRX1_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX1_MISC_ADDR,m,v,HWIO_MSS_BBRX1_MISC_IN)
#define HWIO_MSS_BBRX1_MISC_SRC_DIV_BMSK                                         0xf
#define HWIO_MSS_BBRX1_MISC_SRC_DIV_SHFT                                         0x0

#define HWIO_MSS_BBRX2_MISC_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001140)
#define HWIO_MSS_BBRX2_MISC_RMSK                                                 0xf
#define HWIO_MSS_BBRX2_MISC_IN          \
        in_dword_masked(HWIO_MSS_BBRX2_MISC_ADDR, HWIO_MSS_BBRX2_MISC_RMSK)
#define HWIO_MSS_BBRX2_MISC_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX2_MISC_ADDR, m)
#define HWIO_MSS_BBRX2_MISC_OUT(v)      \
        out_dword(HWIO_MSS_BBRX2_MISC_ADDR,v)
#define HWIO_MSS_BBRX2_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX2_MISC_ADDR,m,v,HWIO_MSS_BBRX2_MISC_IN)
#define HWIO_MSS_BBRX2_MISC_SRC_DIV_BMSK                                         0xf
#define HWIO_MSS_BBRX2_MISC_SRC_DIV_SHFT                                         0x0

#define HWIO_MSS_BBRX3_MISC_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001144)
#define HWIO_MSS_BBRX3_MISC_RMSK                                                 0xf
#define HWIO_MSS_BBRX3_MISC_IN          \
        in_dword_masked(HWIO_MSS_BBRX3_MISC_ADDR, HWIO_MSS_BBRX3_MISC_RMSK)
#define HWIO_MSS_BBRX3_MISC_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX3_MISC_ADDR, m)
#define HWIO_MSS_BBRX3_MISC_OUT(v)      \
        out_dword(HWIO_MSS_BBRX3_MISC_ADDR,v)
#define HWIO_MSS_BBRX3_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX3_MISC_ADDR,m,v,HWIO_MSS_BBRX3_MISC_IN)
#define HWIO_MSS_BBRX3_MISC_SRC_DIV_BMSK                                         0xf
#define HWIO_MSS_BBRX3_MISC_SRC_DIV_SHFT                                         0x0

#define HWIO_MSS_BIT_COXM_DIV_MISC_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00001148)
#define HWIO_MSS_BIT_COXM_DIV_MISC_RMSK                                          0xf
#define HWIO_MSS_BIT_COXM_DIV_MISC_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_DIV_MISC_ADDR, HWIO_MSS_BIT_COXM_DIV_MISC_RMSK)
#define HWIO_MSS_BIT_COXM_DIV_MISC_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_DIV_MISC_ADDR, m)
#define HWIO_MSS_BIT_COXM_DIV_MISC_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_DIV_MISC_ADDR,v)
#define HWIO_MSS_BIT_COXM_DIV_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_DIV_MISC_ADDR,m,v,HWIO_MSS_BIT_COXM_DIV_MISC_IN)
#define HWIO_MSS_BIT_COXM_DIV_MISC_SRC_DIV_BMSK                                  0xf
#define HWIO_MSS_BIT_COXM_DIV_MISC_SRC_DIV_SHFT                                  0x0

#define HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_ADDR                              (MSS_PERPH_REG_BASE      + 0x0000114c)
#define HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_RMSK                                     0x3
#define HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_IN          \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_ADDR, HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_RMSK)
#define HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_ADDR, m)
#define HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_OUT(v)      \
        out_dword(HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_ADDR,v)
#define HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_ADDR,m,v,HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_IN)
#define HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_SRC_DIV_BMSK                             0x3
#define HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_SRC_DIV_SHFT                             0x0

#define HWIO_MSS_TCSR_ACC_SEL_ADDR                                        (MSS_PERPH_REG_BASE      + 0x0000f000)
#define HWIO_MSS_TCSR_ACC_SEL_RMSK                                               0x3
#define HWIO_MSS_TCSR_ACC_SEL_IN          \
        in_dword_masked(HWIO_MSS_TCSR_ACC_SEL_ADDR, HWIO_MSS_TCSR_ACC_SEL_RMSK)
#define HWIO_MSS_TCSR_ACC_SEL_INM(m)      \
        in_dword_masked(HWIO_MSS_TCSR_ACC_SEL_ADDR, m)
#define HWIO_MSS_TCSR_ACC_SEL_OUT(v)      \
        out_dword(HWIO_MSS_TCSR_ACC_SEL_ADDR,v)
#define HWIO_MSS_TCSR_ACC_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_TCSR_ACC_SEL_ADDR,m,v,HWIO_MSS_TCSR_ACC_SEL_IN)
#define HWIO_MSS_TCSR_ACC_SEL_ACC_MEM_SEL_BMSK                                   0x3
#define HWIO_MSS_TCSR_ACC_SEL_ACC_MEM_SEL_SHFT                                   0x0

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                                             (SECURITY_CONTROL_BASE      + 0x00000000)

#define HWIO_QFPROM_RAW_ACC_PRIVATEn_ADDR(n)                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000000 + 0x4 * (n))
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_RMSK                                                                          0xffffffff
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_MAXn                                                                                  39
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ACC_PRIVATEn_ADDR(n), HWIO_QFPROM_RAW_ACC_PRIVATEn_RMSK)
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ACC_PRIVATEn_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_ACC_PRIVATE_BMSK                                                              0xffffffff
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_ACC_PRIVATE_SHFT                                                                     0x0

#define HWIO_QFPROM_RAW_JTAG_ID_LSB_ADDR                                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000a0)
#define HWIO_QFPROM_RAW_JTAG_ID_LSB_RMSK                                                                              0xfffff
#define HWIO_QFPROM_RAW_JTAG_ID_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_JTAG_ID_LSB_ADDR, HWIO_QFPROM_RAW_JTAG_ID_LSB_RMSK)
#define HWIO_QFPROM_RAW_JTAG_ID_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_JTAG_ID_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_JTAG_ID_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_JTAG_ID_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_JTAG_ID_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_JTAG_ID_LSB_ADDR,m,v,HWIO_QFPROM_RAW_JTAG_ID_LSB_IN)
#define HWIO_QFPROM_RAW_JTAG_ID_LSB_JTAG_ID_BMSK                                                                      0xfffff
#define HWIO_QFPROM_RAW_JTAG_ID_LSB_JTAG_ID_SHFT                                                                          0x0

#define HWIO_QFPROM_RAW_JTAG_ID_MSB_ADDR                                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000a4)
#define HWIO_QFPROM_RAW_JTAG_ID_MSB_RMSK                                                                           0xffffffff
#define HWIO_QFPROM_RAW_JTAG_ID_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_JTAG_ID_MSB_ADDR, HWIO_QFPROM_RAW_JTAG_ID_MSB_RMSK)
#define HWIO_QFPROM_RAW_JTAG_ID_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_JTAG_ID_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_JTAG_ID_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_JTAG_ID_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_JTAG_ID_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_JTAG_ID_MSB_ADDR,m,v,HWIO_QFPROM_RAW_JTAG_ID_MSB_IN)
#define HWIO_QFPROM_RAW_JTAG_ID_MSB_QFPROM_RAW_JTAG_ID_MSB_BMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_JTAG_ID_MSB_QFPROM_RAW_JTAG_ID_MSB_SHFT                                                           0x0

#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000a8)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR, HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RMSK)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR,m,v,HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IN)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CUST_PRIV_KEY1_BMSK                                                         0x80000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CUST_PRIV_KEY1_SHFT                                                               0x1f
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CUST_PRIV_KEY0_BMSK                                                         0x40000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CUST_PRIV_KEY0_SHFT                                                               0x1e
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_HDCP_KEY_BMSK                                                               0x20000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_HDCP_KEY_SHFT                                                                     0x1d
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE28_BMSK                                                                0x10000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE28_SHFT                                                                      0x1c
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE27_BMSK                                                                 0x8000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE27_SHFT                                                                      0x1b
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_USB_VID_PID_BMSK                                                             0x4000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_USB_VID_PID_SHFT                                                                  0x1a
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_QC_SEC_BOOT_BMSK                                                             0x2000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_QC_SEC_BOOT_SHFT                                                                  0x19
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_SEC_BOOT_BMSK                                                            0x1000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_SEC_BOOT_SHFT                                                                 0x18
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_PK_HASH_BMSK                                                              0x800000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_PK_HASH_SHFT                                                                  0x17
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                                   0x400000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                                       0x16
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                                   0x200000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                                       0x15
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_BOOT_ROM_PATCH_BMSK                                                           0x100000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_BOOT_ROM_PATCH_SHFT                                                               0x14
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE19_BMSK                                                                   0x80000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE19_SHFT                                                                      0x13
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SERIAL_NUM_BMSK                                                                0x40000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SERIAL_NUM_SHFT                                                                   0x12
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CUST_KEY_BMSK                                                                  0x20000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CUST_KEY_SHFT                                                                     0x11
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_INTAGLIO_GLUEBIT_BMSK                                                          0x10000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_INTAGLIO_GLUEBIT_SHFT                                                             0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEC_EN_BMSK                                                                     0x8000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEC_EN_SHFT                                                                        0xf
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CALIB_BMSK                                                                      0x4000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CALIB_SHFT                                                                         0xe
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_ACCEL_BMSK                                                                  0x2000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_ACCEL_SHFT                                                                     0xd
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_DEBUG_KEY_BMSK                                                                  0x1000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_DEBUG_KEY_SHFT                                                                     0xc
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_REDUN_BMSK                                                                   0x800
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_REDUN_SHFT                                                                     0xb
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEAT_CONFIG_BMSK                                                                 0x400
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEAT_CONFIG_SHFT                                                                   0xa
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_CONFIG_BMSK                                                                  0x200
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_CONFIG_SHFT                                                                    0x9
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IMEI_ESN2_BMSK                                                                   0x100
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IMEI_ESN2_SHFT                                                                     0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IMEI_ESN1_BMSK                                                                    0x80
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IMEI_ESN1_SHFT                                                                     0x7
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IMEI_ESN0_BMSK                                                                    0x40
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IMEI_ESN0_SHFT                                                                     0x6
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MSA_ANTI_ROLLBACK_BMSK                                                            0x20
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MSA_ANTI_ROLLBACK_SHFT                                                             0x5
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_AP_ANTI_ROLLBACK_BMSK                                                             0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_AP_ANTI_ROLLBACK_SHFT                                                              0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PTE_BMSK                                                                           0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PTE_SHFT                                                                           0x3
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RD_WR_PERM_BMSK                                                                    0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RD_WR_PERM_SHFT                                                                    0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_JTAG_ID_BMSK                                                                       0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_JTAG_ID_SHFT                                                                       0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ACC_PRIVATE_BMSK                                                                   0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ACC_PRIVATE_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000ac)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR, HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RMSK)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR,m,v,HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IN)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CUST_PRIV_KEY1_BMSK                                                         0x80000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CUST_PRIV_KEY1_SHFT                                                               0x1f
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CUST_PRIV_KEY0_BMSK                                                         0x40000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CUST_PRIV_KEY0_SHFT                                                               0x1e
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_HDCP_KEY_BMSK                                                               0x20000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_HDCP_KEY_SHFT                                                                     0x1d
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE28_BMSK                                                                0x10000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE28_SHFT                                                                      0x1c
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE27_BMSK                                                                 0x8000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE27_SHFT                                                                      0x1b
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_USB_VID_PID_BMSK                                                             0x4000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_USB_VID_PID_SHFT                                                                  0x1a
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_QC_SEC_BOOT_BMSK                                                             0x2000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_QC_SEC_BOOT_SHFT                                                                  0x19
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_SEC_BOOT_BMSK                                                            0x1000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_SEC_BOOT_SHFT                                                                 0x18
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_PK_HASH_BMSK                                                              0x800000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_PK_HASH_SHFT                                                                  0x17
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_BMSK                                                   0x400000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_SHFT                                                       0x16
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_BMSK                                                   0x200000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_SHFT                                                       0x15
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_BOOT_ROM_PATCH_BMSK                                                           0x100000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_BOOT_ROM_PATCH_SHFT                                                               0x14
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE19_BMSK                                                                   0x80000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE19_SHFT                                                                      0x13
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SERIAL_NUM_BMSK                                                                0x40000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SERIAL_NUM_SHFT                                                                   0x12
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CUST_KEY_BMSK                                                                  0x20000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CUST_KEY_SHFT                                                                     0x11
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_INTAGLIO_GLUEBIT_BMSK                                                          0x10000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_INTAGLIO_GLUEBIT_SHFT                                                             0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEC_EN_BMSK                                                                     0x8000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEC_EN_SHFT                                                                        0xf
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CALIB_BMSK                                                                      0x4000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CALIB_SHFT                                                                         0xe
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_ACCEL_BMSK                                                                  0x2000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_ACCEL_SHFT                                                                     0xd
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_DEBUG_KEY_BMSK                                                                  0x1000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_DEBUG_KEY_SHFT                                                                     0xc
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_REDUN_BMSK                                                                   0x800
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_REDUN_SHFT                                                                     0xb
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEAT_CONFIG_BMSK                                                                 0x400
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEAT_CONFIG_SHFT                                                                   0xa
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_CONFIG_BMSK                                                                  0x200
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_CONFIG_SHFT                                                                    0x9
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IMEI_ESN2_BMSK                                                                   0x100
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IMEI_ESN2_SHFT                                                                     0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IMEI_ESN1_BMSK                                                                    0x80
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IMEI_ESN1_SHFT                                                                     0x7
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IMEI_ESN0_BMSK                                                                    0x40
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IMEI_ESN0_SHFT                                                                     0x6
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MSA_ANTI_ROLLBACK_BMSK                                                            0x20
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MSA_ANTI_ROLLBACK_SHFT                                                             0x5
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_AP_ANTI_ROLLBACK_BMSK                                                             0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_AP_ANTI_ROLLBACK_SHFT                                                              0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PTE_BMSK                                                                           0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PTE_SHFT                                                                           0x3
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RD_WR_PERM_BMSK                                                                    0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RD_WR_PERM_SHFT                                                                    0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_JTAG_ID_BMSK                                                                       0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_JTAG_ID_SHFT                                                                       0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ACC_PRIVATE_BMSK                                                                   0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ACC_PRIVATE_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_PTE_LSB_ADDR                                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000b0)
#define HWIO_QFPROM_RAW_PTE_LSB_RMSK                                                                               0xffffffff
#define HWIO_QFPROM_RAW_PTE_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_LSB_ADDR, HWIO_QFPROM_RAW_PTE_LSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_LSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_LSB_IN)
#define HWIO_QFPROM_RAW_PTE_LSB_QFPROM_RAW_PTE_LSB_BMSK                                                            0xffffffff
#define HWIO_QFPROM_RAW_PTE_LSB_QFPROM_RAW_PTE_LSB_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_PTE_MSB_ADDR                                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000b4)
#define HWIO_QFPROM_RAW_PTE_MSB_RMSK                                                                                      0xf
#define HWIO_QFPROM_RAW_PTE_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_MSB_ADDR, HWIO_QFPROM_RAW_PTE_MSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_MSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_MSB_IN)
#define HWIO_QFPROM_RAW_PTE_MSB_SPEED_BIN_BMSK                                                                            0xf
#define HWIO_QFPROM_RAW_PTE_MSB_SPEED_BIN_SHFT                                                                            0x0

#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000b8)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_RMSK                                                             0xffffffff
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_TZ_BMSK                                                          0xffff0000
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_TZ_SHFT                                                                0x10
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_SBL1_BMSK                                                            0xffff
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_SBL1_SHFT                                                               0x0

#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000bc)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_RMSK                                                             0xffffffff
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_BMSK                        0xffffffff
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_SHFT                               0x0

#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000c0)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_RMSK                                                             0xffffffff
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_APPSBL0_BMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_APPSBL0_SHFT                                                            0x0

#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000c4)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_RMSK                                                              0x3ffffff
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_RPM_BMSK                                                          0x3fc0000
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_RPM_SHFT                                                               0x12
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_APPSBL1_BMSK                                                        0x3ffff
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_APPSBL1_SHFT                                                            0x0

#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000c8)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_ADDR, HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_RMSK)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_IN)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_MSS_BMSK                                                             0xffff0000
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_MSS_SHFT                                                                   0x10
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_MBA_BMSK                                                                 0xffff
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_MBA_SHFT                                                                    0x0

#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000cc)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_ADDR, HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_RMSK)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_IN)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_BMSK                                0xffffffff
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_SHFT                                       0x0

#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000d0)
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_RMSK                                                                         0xffffffff
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN0_LSB_ADDR, HWIO_QFPROM_RAW_IMEI_ESN0_LSB_RMSK)
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_IMEI_ESN0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_IMEI_ESN0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_IMEI_ESN0_LSB_IN)
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_IMEI_ESN_DATA0_BMSK                                                          0xffffffff
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_IMEI_ESN_DATA0_SHFT                                                                 0x0

#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000d4)
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_RMSK                                                                          0x7ffffff
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN0_MSB_ADDR, HWIO_QFPROM_RAW_IMEI_ESN0_MSB_RMSK)
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_IMEI_ESN0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_IMEI_ESN0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_IMEI_ESN0_MSB_IN)
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_IMEI_ESN_DATA1_BMSK                                                           0x7ffffff
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_IMEI_ESN_DATA1_SHFT                                                                 0x0

#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000d8)
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_RMSK                                                                         0xffffffff
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN1_LSB_ADDR, HWIO_QFPROM_RAW_IMEI_ESN1_LSB_RMSK)
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_IMEI_ESN1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_IMEI_ESN1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_IMEI_ESN1_LSB_IN)
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_IMEI_ESN_DATA0_BMSK                                                          0xffffffff
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_IMEI_ESN_DATA0_SHFT                                                                 0x0

#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000dc)
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_RMSK                                                                          0x7ffffff
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN1_MSB_ADDR, HWIO_QFPROM_RAW_IMEI_ESN1_MSB_RMSK)
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_IMEI_ESN1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_IMEI_ESN1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_IMEI_ESN1_MSB_IN)
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_IMEI_ESN_DATA1_BMSK                                                           0x7ffffff
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_IMEI_ESN_DATA1_SHFT                                                                 0x0

#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000e0)
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_RMSK                                                                         0xffffffff
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN2_LSB_ADDR, HWIO_QFPROM_RAW_IMEI_ESN2_LSB_RMSK)
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_IMEI_ESN2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_IMEI_ESN2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_IMEI_ESN2_LSB_IN)
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_IMEI_ESN_DATA0_BMSK                                                          0xffffffff
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_IMEI_ESN_DATA0_SHFT                                                                 0x0

#define HWIO_QFPROM_RAW_IMEI_ESN2_MSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000e4)
#define HWIO_QFPROM_RAW_IMEI_ESN2_MSB_RMSK                                                                          0x7ffffff
#define HWIO_QFPROM_RAW_IMEI_ESN2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN2_MSB_ADDR, HWIO_QFPROM_RAW_IMEI_ESN2_MSB_RMSK)
#define HWIO_QFPROM_RAW_IMEI_ESN2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_IMEI_ESN2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_IMEI_ESN2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_IMEI_ESN2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_IMEI_ESN2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_IMEI_ESN2_MSB_IN)
#define HWIO_QFPROM_RAW_IMEI_ESN2_MSB_IMEI_ESN_DATA1_BMSK                                                           0x7ffffff
#define HWIO_QFPROM_RAW_IMEI_ESN2_MSB_IMEI_ESN_DATA1_SHFT                                                                 0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000e8)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RMSK                                                                   0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE1_DISABLE_BMSK                                                    0x80000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE1_DISABLE_SHFT                                                          0x1f
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE0_DISABLE_BMSK                                                    0x40000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE0_DISABLE_SHFT                                                          0x1e
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_VENUS_0_DBGEN_DISABLE_BMSK                                             0x20000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_VENUS_0_DBGEN_DISABLE_SHFT                                                   0x1d
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RPM_DAPEN_DISABLE_BMSK                                                 0x10000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RPM_DAPEN_DISABLE_SHFT                                                       0x1c
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RPM_LPASS_WCSS_NIDEN_DISABLE_BMSK                                       0x8000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RPM_LPASS_WCSS_NIDEN_DISABLE_SHFT                                            0x1b
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RPM_DBGEN_DISABLE_BMSK                                                  0x4000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RPM_DBGEN_DISABLE_SHFT                                                       0x1a
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_WCSS_DBGEN_DISABLE_BMSK                                                 0x2000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_WCSS_DBGEN_DISABLE_SHFT                                                      0x19
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_LPASS_DBGEN_DISABLE_BMSK                                                0x1000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_LPASS_DBGEN_DISABLE_SHFT                                                     0x18
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_MSS_NIDEN_DISABLE_BMSK                                                   0x800000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_MSS_NIDEN_DISABLE_SHFT                                                       0x17
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_MSS_DBGEN_DISABLE_BMSK                                                   0x400000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_MSS_DBGEN_DISABLE_SHFT                                                       0x16
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_BMSK                                                   0x200000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_SHFT                                                       0x15
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE1_BMSK                                                              0x1e0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE1_SHFT                                                                  0x11
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_BMSK                                        0x10000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_SHFT                                           0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_BMSK                                                   0x8000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_SHFT                                                      0xf
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_BMSK                                                             0x7c00
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SHFT                                                                0xa
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDP_ENUM_SKIP_BMSK                                                          0x200
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDP_ENUM_SKIP_SHFT                                                            0x9
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_BMSK                                                    0x100
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_SHFT                                                      0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_APPS_PBL_BOOT_SPEED_BMSK                                                     0xc0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_APPS_PBL_BOOT_SPEED_SHFT                                                      0x6
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_BMSK                                                  0x20
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_SHFT                                                   0x5
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE0_BMSK                                                                  0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE0_SHFT                                                                   0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_PMIC_POWER_DOWN_MSM_BMSK                                                  0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_PMIC_POWER_DOWN_MSM_SHFT                                                  0x3
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_USB_ENUM_TIMEOUT_BMSK                                                         0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_USB_ENUM_TIMEOUT_SHFT                                                         0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DLOAD_TIMEOUT_BMSK                                                            0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DLOAD_TIMEOUT_SHFT                                                            0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_BMSK                                                          0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_SHFT                                                          0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000ec)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RMSK                                                                   0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_WDOG_EN_BMSK                                                           0x80000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_WDOG_EN_SHFT                                                                 0x1f
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE2_BMSK                                                            0x78000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE2_SHFT                                                                  0x1b
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPDM_SECURE_MODE_BMSK                                                   0x4000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPDM_SECURE_MODE_SHFT                                                        0x1a
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IMEI_ESN_SELECT_BMSK                                                    0x3000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IMEI_ESN_SELECT_SHFT                                                         0x18
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE1_BMSK                                                              0xc00000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE1_SHFT                                                                  0x16
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_CUST_PRIV_KEY_SELECT_BMSK                                                0x200000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_CUST_PRIV_KEY_SELECT_SHFT                                                    0x15
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SW_FUSE_PROG_DISABLE_BMSK                                                0x100000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SW_FUSE_PROG_DISABLE_SHFT                                                    0x14
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE0_BMSK                                                               0xffe00
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE0_SHFT                                                                   0x9
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_BMSK                                                   0x100
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_SHFT                                                     0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_BMSK                                                     0x80
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_SHFT                                                      0x7
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_BMSK                                                      0x40
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_SHFT                                                       0x6
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_BMSK                                                       0x20
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_SHFT                                                        0x5
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_BMSK                                                       0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_SHFT                                                        0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_BMSK                                                     0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_SHFT                                                     0x3
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_BMSK                                                      0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_SHFT                                                      0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_BMSK                                                       0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_SHFT                                                       0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_BMSK                                                       0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_SHFT                                                       0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000f0)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RMSK                                                                   0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_BMSK                                                         0xffff0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_SHFT                                                               0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_SPARE0_BMSK                                                                0xffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_SPARE0_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000f4)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_RMSK                                                                   0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_SPARE0_BMSK                                                            0xffff0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_SPARE0_SHFT                                                                  0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OEM_PRODUCT_ID_BMSK                                                        0xffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OEM_PRODUCT_ID_SHFT                                                           0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000f8)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_BMSK                                                     0x80000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_SHFT                                                           0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_EDP_DISABLE_BMSK                                                      0x40000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_EDP_DISABLE_SHFT                                                            0x1e
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_BMSK                                                    0x20000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_SHFT                                                          0x1d
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ISP_1_DISABLE_BMSK                                                    0x10000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ISP_1_DISABLE_SHFT                                                          0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_BMSK                                               0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_SHFT                                                    0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_BMSK                                               0x4000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_SHFT                                                    0x1a
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CSID_3_DISABLE_BMSK                                                    0x2000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CSID_3_DISABLE_SHFT                                                         0x19
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CSID_2_DISABLE_BMSK                                                    0x1000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CSID_2_DISABLE_SHFT                                                         0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MACROVISION_DISABLE_BMSK                                                0x800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MACROVISION_DISABLE_SHFT                                                    0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_SPARE_DISABLE_BMSK                                                0x700000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_SPARE_DISABLE_SHFT                                                    0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_BMSK                                                 0x80000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_SHFT                                                    0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_BMSK                                                 0x40000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_SHFT                                                    0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_BMSK                                                0x20000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_SHFT                                                   0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DC_DISABLE_BMSK                                               0x10000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DC_DISABLE_SHFT                                                  0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_MIMO_DISABLE_BMSK                                              0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_MIMO_DISABLE_SHFT                                                 0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT2_DISABLE_BMSK                                         0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT2_DISABLE_SHFT                                            0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT1_DISABLE_BMSK                                         0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT1_DISABLE_SHFT                                            0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_TDSCDMA_DISABLE_BMSK                                                0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_TDSCDMA_DISABLE_SHFT                                                   0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DISABLE_BMSK                                                    0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DISABLE_SHFT                                                      0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_DISABLE_BMSK                                                     0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_DISABLE_SHFT                                                       0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_WCDMA_DISABLE_BMSK                                                   0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_WCDMA_DISABLE_SHFT                                                     0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_DO_DISABLE_BMSK                                                      0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_DO_DISABLE_SHFT                                                        0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_1X_DISABLE_BMSK                                                       0x80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_1X_DISABLE_SHFT                                                        0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_BMSK                                                            0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_SHFT                                                             0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_GSM_DISABLE_BMSK                                                            0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_GSM_DISABLE_SHFT                                                             0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPI_SLAVE_DISABLE_BMSK                                                      0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPI_SLAVE_DISABLE_SHFT                                                       0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_BMSK                                                        0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_SHFT                                                        0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_BMSK                                                  0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000fc)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENDOR_LOCK_BMSK                                                      0xf0000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENDOR_LOCK_SHFT                                                            0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                                       0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                                            0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SPARE0_BMSK                                                            0x7ff8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SPARE0_SHFT                                                                  0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MULTIMEDIA_SPARE_DISABLE_BMSK                                             0x7800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MULTIMEDIA_SPARE_DISABLE_SHFT                                                0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_PCIE_DISABLE_BMSK                                                          0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_PCIE_DISABLE_SHFT                                                            0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SATA_DISABLE_BMSK                                                          0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SATA_DISABLE_SHFT                                                            0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ACC_DISABLE_BMSK                                                           0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ACC_DISABLE_SHFT                                                             0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VPE_DISABLE_BMSK                                                            0x80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VPE_DISABLE_SHFT                                                             0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VFE_DISABLE_BMSK                                                            0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VFE_DISABLE_SHFT                                                             0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_JPEG_ENCODER_DISABLE_BMSK                                                   0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_JPEG_ENCODER_DISABLE_SHFT                                                    0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_JPEG_DECODER_DISABLE_BMSK                                                   0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_JPEG_DECODER_DISABLE_SHFT                                                    0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_FIELD_3D_GRP_DISABLE_BMSK                                                    0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_FIELD_3D_GRP_DISABLE_SHFT                                                    0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VIDEO_ENCODER_DISABLE_BMSK                                                   0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VIDEO_ENCODER_DISABLE_SHFT                                                   0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VIDEO_CODEC_DISABLE_BMSK                                                     0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VIDEO_CODEC_DISABLE_SHFT                                                     0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_HDMI_DISABLE_BMSK                                                            0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_HDMI_DISABLE_SHFT                                                            0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000100)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SPARE1_BMSK                                                           0xffc00000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SPARE1_SHFT                                                                 0x16
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_BMSK                                            0x200000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_SHFT                                                0x15
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_BMSK                                             0x100000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_SHFT                                                 0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_BMSK                                               0x80000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_SHFT                                                  0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_BMSK                                                0x40000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_SHFT                                                   0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_BMSK                                                0x20000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_SHFT                                                   0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_BMSK                                             0x10000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_SHFT                                                0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_BMSK                                               0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_SHFT                                                  0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_BMSK                                                0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_SHFT                                                   0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_BMSK                                                0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_SHFT                                                   0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SPARE1_DISABLE_BMSK                                                    0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SPARE1_DISABLE_SHFT                                                       0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SPARE0_DISABLE_BMSK                                                     0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SPARE0_DISABLE_SHFT                                                       0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_BMSK                                              0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_SHFT                                                0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_BMSK                                                  0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_SHFT                                                    0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_LPASS_WCSS_NIDEN_DISABLE_BMSK                                       0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_LPASS_WCSS_NIDEN_DISABLE_SHFT                                         0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_BMSK                                                   0x80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_SHFT                                                    0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_BMSK                                                  0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_SHFT                                                   0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_LPASS_DBGEN_DISABLE_BMSK                                                 0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_LPASS_DBGEN_DISABLE_SHFT                                                  0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_BMSK                                                   0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_SHFT                                                    0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_BMSK                                                    0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_SHFT                                                    0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SPARE0_BMSK                                                                  0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SPARE0_SHFT                                                                  0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_BMSK                                                         0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_SHFT                                                         0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_BMSK                                                             0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_SHFT                                                             0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000104)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_BMSK                                           0xfffc0000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_SHFT                                                 0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_BMSK                                            0x3c000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_SHFT                                                0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_BMSK                                                    0x3fff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_SHFT                                                       0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000108)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_BMSK                                      0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_SHFT                                             0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000010c)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SW_EXPANSION_FUSES_BMSK                                               0xff000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SW_EXPANSION_FUSES_SHFT                                                     0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE0_BMSK                                                             0xf00000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE0_SHFT                                                                 0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_BMSK                                                0xf0000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_SHFT                                                   0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION3_BMSK                                                  0xf000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION3_SHFT                                                     0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION2_BMSK                                                   0xf00
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION2_SHFT                                                     0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION1_BMSK                                                    0xf0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION1_SHFT                                                     0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION0_BMSK                                                     0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION0_SHFT                                                     0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000110)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_MDSP_FW_DISABLE_BMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_MDSP_FW_DISABLE_SHFT                                                         0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000114)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_SPARE0_BMSK                                                           0xffff0000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_SPARE0_SHFT                                                                 0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RPM_PBL_BOOT_SPEED_BMSK                                                   0xc000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RPM_PBL_BOOT_SPEED_SHFT                                                      0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RPM_BOOT_FROM_ROM_BMSK                                                    0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RPM_BOOT_FROM_ROM_SHFT                                                       0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_APPS_BOOT_FROM_ROM_BMSK                                                   0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_APPS_BOOT_FROM_ROM_SHFT                                                      0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_MSA_ENA_BMSK                                                               0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_MSA_ENA_SHFT                                                                 0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_FORCE_MSA_AUTH_EN_BMSK                                                     0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_FORCE_MSA_AUTH_EN_SHFT                                                       0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_FLCB_SKIP_BMSK                                                             0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_FLCB_SKIP_SHFT                                                               0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_QCOM_PMIC_FEATURE_ENABLE_BMSK                                              0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_QCOM_PMIC_FEATURE_ENABLE_SHFT                                                0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_BOOT_ROM_CFG_BMSK                                                           0xe0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_BOOT_ROM_CFG_SHFT                                                            0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_STACKED_MEMORY_ID_BMSK                                                      0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_STACKED_MEMORY_ID_SHFT                                                       0x0

#define HWIO_QFPROM_RAW_MEM_REDUNn_ADDR(n)                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000118 + 0x4 * (n))
#define HWIO_QFPROM_RAW_MEM_REDUNn_RMSK                                                                            0xffffffff
#define HWIO_QFPROM_RAW_MEM_REDUNn_MAXn                                                                                    35
#define HWIO_QFPROM_RAW_MEM_REDUNn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_REDUNn_ADDR(n), HWIO_QFPROM_RAW_MEM_REDUNn_RMSK)
#define HWIO_QFPROM_RAW_MEM_REDUNn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_REDUNn_ADDR(n), mask)
#define HWIO_QFPROM_RAW_MEM_REDUNn_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_MEM_REDUNn_ADDR(n),val)
#define HWIO_QFPROM_RAW_MEM_REDUNn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_REDUNn_ADDR(n),mask,val,HWIO_QFPROM_RAW_MEM_REDUNn_INI(n))
#define HWIO_QFPROM_RAW_MEM_REDUNn_REDUN_DATA_BMSK                                                                 0xffffffff
#define HWIO_QFPROM_RAW_MEM_REDUNn_REDUN_DATA_SHFT                                                                        0x0

#define HWIO_QFPROM_RAW_DEBUG_KEY_LSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001a8)
#define HWIO_QFPROM_RAW_DEBUG_KEY_LSB_RMSK                                                                         0xffffffff
#define HWIO_QFPROM_RAW_DEBUG_KEY_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_DEBUG_KEY_LSB_ADDR, HWIO_QFPROM_RAW_DEBUG_KEY_LSB_RMSK)
#define HWIO_QFPROM_RAW_DEBUG_KEY_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_DEBUG_KEY_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_DEBUG_KEY_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_DEBUG_KEY_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_DEBUG_KEY_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_DEBUG_KEY_LSB_ADDR,m,v,HWIO_QFPROM_RAW_DEBUG_KEY_LSB_IN)
#define HWIO_QFPROM_RAW_DEBUG_KEY_LSB_KEY0_BMSK                                                                    0xffffffff
#define HWIO_QFPROM_RAW_DEBUG_KEY_LSB_KEY0_SHFT                                                                           0x0

#define HWIO_QFPROM_RAW_DEBUG_KEY_MSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001ac)
#define HWIO_QFPROM_RAW_DEBUG_KEY_MSB_RMSK                                                                         0xffffffff
#define HWIO_QFPROM_RAW_DEBUG_KEY_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_DEBUG_KEY_MSB_ADDR, HWIO_QFPROM_RAW_DEBUG_KEY_MSB_RMSK)
#define HWIO_QFPROM_RAW_DEBUG_KEY_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_DEBUG_KEY_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_DEBUG_KEY_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_DEBUG_KEY_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_DEBUG_KEY_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_DEBUG_KEY_MSB_ADDR,m,v,HWIO_QFPROM_RAW_DEBUG_KEY_MSB_IN)
#define HWIO_QFPROM_RAW_DEBUG_KEY_MSB_KEY1_BMSK                                                                    0xffffffff
#define HWIO_QFPROM_RAW_DEBUG_KEY_MSB_KEY1_SHFT                                                                           0x0

#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_ADDR                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001b0)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_RMSK                                                                    0xffffffff
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_MEM_ACCEL_BMSK                                                          0xffffffff
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_MEM_ACCEL_SHFT                                                                 0x0

#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_ADDR                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001b4)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_RMSK                                                                    0xffffffff
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_MEM_ACCEL_BMSK                                                          0xffffffff
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_MEM_ACCEL_SHFT                                                                 0x0

#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_ADDR                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001b8)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_RMSK                                                                        0x3fff
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_MEM_ACCEL_BMSK                                                              0x3fff
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_MEM_ACCEL_SHFT                                                                 0x0

#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_ADDR                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001bc)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_RMSK                                                                    0xffffffff
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_BMSK                                      0xffffffff
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_SHFT                                             0x0

#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001c0)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_SPARE_BMSK                                                                  0xffffe000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_SPARE_SHFT                                                                         0xd
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_CSI_PHY_BMSK                                                                    0x1f00
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_CSI_PHY_SHFT                                                                       0x8
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_DSI_PHY_BMSK                                                                      0xf0
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_DSI_PHY_SHFT                                                                       0x4
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_DSIPHY_PLL_BMSK                                                                    0xf
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_DSIPHY_PLL_SHFT                                                                    0x0

#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001c4)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TEMP_SENSOR3_BMSK                                                           0xff000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TEMP_SENSOR3_SHFT                                                                 0x18
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TEMP_SENSOR2_BMSK                                                             0xff0000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TEMP_SENSOR2_SHFT                                                                 0x10
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TEMP_SENSOR1_BMSK                                                               0xff00
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TEMP_SENSOR1_SHFT                                                                  0x8
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TEMP_SENSOR0_BMSK                                                                 0xff
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TEMP_SENSOR0_SHFT                                                                  0x0

#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001c8)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_SPARE_BMSK                                                                  0xfff00000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_SPARE_SHFT                                                                        0x14
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS3_LDO_VREF_TRIM_BMSK                                                       0xf8000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS3_LDO_VREF_TRIM_SHFT                                                           0xf
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS2_LDO_VREF_TRIM_BMSK                                                        0x7c00
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS2_LDO_VREF_TRIM_SHFT                                                           0xa
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS1_LDO_VREF_TRIM_BMSK                                                         0x3e0
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS1_LDO_VREF_TRIM_SHFT                                                           0x5
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS0_LDO_VREF_TRIM_BMSK                                                          0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS0_LDO_VREF_TRIM_SHFT                                                           0x0

#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001cc)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SPARE_BMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SPARE_SHFT                                                                         0x0

#define HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR                                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001d0)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_RMSK                                                                            0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR, HWIO_QFPROM_RAW_FEC_EN_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEC_EN_LSB_IN)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION31_FEC_EN_BMSK                                                            0x80000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION31_FEC_EN_SHFT                                                                  0x1f
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION30_FEC_EN_BMSK                                                            0x40000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION30_FEC_EN_SHFT                                                                  0x1e
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION29_FEC_EN_BMSK                                                            0x20000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION29_FEC_EN_SHFT                                                                  0x1d
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION28_FEC_EN_BMSK                                                            0x10000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION28_FEC_EN_SHFT                                                                  0x1c
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION27_FEC_EN_BMSK                                                             0x8000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION27_FEC_EN_SHFT                                                                  0x1b
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION26_FEC_EN_BMSK                                                             0x4000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION26_FEC_EN_SHFT                                                                  0x1a
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION25_FEC_EN_BMSK                                                             0x2000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION25_FEC_EN_SHFT                                                                  0x19
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION24_FEC_EN_BMSK                                                             0x1000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION24_FEC_EN_SHFT                                                                  0x18
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION23_FEC_EN_BMSK                                                              0x800000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION23_FEC_EN_SHFT                                                                  0x17
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION22_FEC_EN_BMSK                                                              0x400000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION22_FEC_EN_SHFT                                                                  0x16
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION21_FEC_EN_BMSK                                                              0x200000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION21_FEC_EN_SHFT                                                                  0x15
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION20_FEC_EN_BMSK                                                              0x100000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION20_FEC_EN_SHFT                                                                  0x14
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION19_FEC_EN_BMSK                                                               0x80000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION19_FEC_EN_SHFT                                                                  0x13
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION18_FEC_EN_BMSK                                                               0x40000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION18_FEC_EN_SHFT                                                                  0x12
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION17_FEC_EN_BMSK                                                               0x20000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION17_FEC_EN_SHFT                                                                  0x11
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION16_FEC_EN_BMSK                                                               0x10000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION16_FEC_EN_SHFT                                                                  0x10
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION15_FEC_EN_BMSK                                                                0x8000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION15_FEC_EN_SHFT                                                                   0xf
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION14_FEC_EN_BMSK                                                                0x4000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION14_FEC_EN_SHFT                                                                   0xe
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION13_FEC_EN_BMSK                                                                0x2000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION13_FEC_EN_SHFT                                                                   0xd
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION12_FEC_EN_BMSK                                                                0x1000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION12_FEC_EN_SHFT                                                                   0xc
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION11_FEC_EN_BMSK                                                                 0x800
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION11_FEC_EN_SHFT                                                                   0xb
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION10_FEC_EN_BMSK                                                                 0x400
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION10_FEC_EN_SHFT                                                                   0xa
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION9_FEC_EN_BMSK                                                                  0x200
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION9_FEC_EN_SHFT                                                                    0x9
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION8_FEC_EN_BMSK                                                                  0x100
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION8_FEC_EN_SHFT                                                                    0x8
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION7_FEC_EN_BMSK                                                                   0x80
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION7_FEC_EN_SHFT                                                                    0x7
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION6_FEC_EN_BMSK                                                                   0x40
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION6_FEC_EN_SHFT                                                                    0x6
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION5_FEC_EN_BMSK                                                                   0x20
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION5_FEC_EN_SHFT                                                                    0x5
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION4_FEC_EN_BMSK                                                                   0x10
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION4_FEC_EN_SHFT                                                                    0x4
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION3_FEC_EN_BMSK                                                                    0x8
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION3_FEC_EN_SHFT                                                                    0x3
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION2_FEC_EN_BMSK                                                                    0x4
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION2_FEC_EN_SHFT                                                                    0x2
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION1_FEC_EN_BMSK                                                                    0x2
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION1_FEC_EN_SHFT                                                                    0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION0_FEC_EN_BMSK                                                                    0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION0_FEC_EN_SHFT                                                                    0x0

#define HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR                                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001d4)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_RMSK                                                                            0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR, HWIO_QFPROM_RAW_FEC_EN_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEC_EN_MSB_IN)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_QFPROM_RAW_FEC_EN_MSB_BMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_MSB_QFPROM_RAW_FEC_EN_MSB_SHFT                                                             0x0

#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001d8)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_ADDR, HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_RMSK)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_ADDR,m,v,HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_IN)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_BMSK                                  0xffffffff
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_SHFT                                         0x0

#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001dc)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_ADDR, HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_RMSK)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_ADDR,m,v,HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_IN)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_BMSK                                  0xffffffff
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_SHFT                                         0x0

#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_ADDR(n)                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001e0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_RMSK                                                                     0xffffffff
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_MAXn                                                                              1
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_KEY_DATA_BMSK                                                            0xffffffff
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_KEY_DATA_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_ADDR(n)                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001e4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_RMSK                                                                     0xffffffff
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_MAXn                                                                              1
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_KEY_DATA_BMSK                                                            0xffffffff
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_KEY_DATA_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_SERIAL_NUM_LSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001f0)
#define HWIO_QFPROM_RAW_SERIAL_NUM_LSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_RAW_SERIAL_NUM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SERIAL_NUM_LSB_ADDR, HWIO_QFPROM_RAW_SERIAL_NUM_LSB_RMSK)
#define HWIO_QFPROM_RAW_SERIAL_NUM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SERIAL_NUM_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SERIAL_NUM_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SERIAL_NUM_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SERIAL_NUM_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SERIAL_NUM_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SERIAL_NUM_LSB_IN)
#define HWIO_QFPROM_RAW_SERIAL_NUM_LSB_SERIAL_NUM_BMSK                                                             0xffffffff
#define HWIO_QFPROM_RAW_SERIAL_NUM_LSB_SERIAL_NUM_SHFT                                                                    0x0

#define HWIO_QFPROM_RAW_SERIAL_NUM_MSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001f4)
#define HWIO_QFPROM_RAW_SERIAL_NUM_MSB_RMSK                                                                            0xffff
#define HWIO_QFPROM_RAW_SERIAL_NUM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SERIAL_NUM_MSB_ADDR, HWIO_QFPROM_RAW_SERIAL_NUM_MSB_RMSK)
#define HWIO_QFPROM_RAW_SERIAL_NUM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SERIAL_NUM_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SERIAL_NUM_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SERIAL_NUM_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SERIAL_NUM_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SERIAL_NUM_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SERIAL_NUM_MSB_IN)
#define HWIO_QFPROM_RAW_SERIAL_NUM_MSB_CHIP_ID_BMSK                                                                    0xffff
#define HWIO_QFPROM_RAW_SERIAL_NUM_MSB_CHIP_ID_SHFT                                                                       0x0

#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001f8)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_RMSK                                                                       0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG19_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG19_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_SPARE_BMSK                                                                 0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_SPARE_SHFT                                                                        0x0

#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001fc)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_RMSK                                                                       0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG19_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG19_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_SPARE_BMSK                                                                 0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_SPARE_SHFT                                                                        0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000200 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_RMSK                                                                    0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_MAXn                                                                            47
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_PATCH_DATA0_BMSK                                                        0xffff0000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_PATCH_DATA0_SHFT                                                              0x10
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_PATCH_ADDR_BMSK                                                             0xffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_PATCH_ADDR_SHFT                                                                0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000204 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RMSK                                                                    0x7f01ffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_MAXn                                                                            47
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_FEC_VALUE_BMSK                                                          0x7f000000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_FEC_VALUE_SHFT                                                                0x18
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_EN_BMSK                                                              0x10000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_EN_SHFT                                                                 0x10
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_DATA1_BMSK                                                            0xffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_DATA1_SHFT                                                               0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000380 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                                                3
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                                                    0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000384 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                                       0x7fffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                                                3
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_FEC_VALUE_BMSK                                             0x7f000000
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_FEC_VALUE_SHFT                                                   0x18
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                                               0xffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                                                    0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003a0)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_IN)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_BMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_SHFT                                                    0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003a4)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RMSK                                                       0x7f000000
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_FEC_VALUE_BMSK                                             0x7f000000
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_FEC_VALUE_SHFT                                                   0x18

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003a8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                                                3
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                                                    0x0

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003ac + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                                       0x7fffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                                                3
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_FEC_VALUE_BMSK                                             0x7f000000
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_FEC_VALUE_SHFT                                                   0x18
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                                               0xffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                                                    0x0

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003c8)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_IN)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_BMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_SHFT                                                    0x0

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003cc)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RMSK                                                       0x7f000000
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_FEC_VALUE_BMSK                                             0x7f000000
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_FEC_VALUE_SHFT                                                   0x18

#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_ADDR(n)                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003d0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_MAXn                                                                           3
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_HASH_DATA0_BMSK                                                       0xffffffff
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_HASH_DATA0_SHFT                                                              0x0

#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_ADDR(n)                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003d4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_RMSK                                                                  0x7fffffff
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_MAXn                                                                           3
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_FEC_VALUE_BMSK                                                        0x7f000000
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_FEC_VALUE_SHFT                                                              0x18
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_HASH_DATA1_BMSK                                                         0xffffff
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_HASH_DATA1_SHFT                                                              0x0

#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003f0)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_ADDR, HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_HASH_DATA0_BMSK                                                       0xffffffff
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_HASH_DATA0_SHFT                                                              0x0

#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003f4)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_RMSK                                                                  0x7f000000
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_FEC_VALUE_BMSK                                                        0x7f000000
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_FEC_VALUE_SHFT                                                              0x18

#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003f8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_MAXn                                                                          3
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT4_BMSK                                                       0xff000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT4_SHFT                                                             0x18
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT3_BMSK                                                         0xff0000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT3_SHFT                                                             0x10
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT2_BMSK                                                           0xff00
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT2_SHFT                                                              0x8
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT1_BMSK                                                             0xff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT1_SHFT                                                              0x0

#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003fc + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_RMSK                                                                 0x7fffffff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_MAXn                                                                          3
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_FEC_VALUE_BMSK                                                       0x7f000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_FEC_VALUE_SHFT                                                             0x18
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT7_BMSK                                                         0xff0000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT7_SHFT                                                             0x10
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT6_BMSK                                                           0xff00
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT6_SHFT                                                              0x8
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT5_BMSK                                                             0xff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT5_SHFT                                                              0x0

#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_ADDR(n)                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000418 + 0x8 * (n))
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_MAXn                                                                           3
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT4_BMSK                                                        0xff000000
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT4_SHFT                                                              0x18
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT3_BMSK                                                          0xff0000
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT3_SHFT                                                              0x10
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT2_BMSK                                                            0xff00
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT2_SHFT                                                               0x8
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT1_BMSK                                                              0xff
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT1_SHFT                                                               0x0

#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_ADDR(n)                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000041c + 0x8 * (n))
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_RMSK                                                                  0x7fffffff
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_MAXn                                                                           3
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_FEC_VALUE_BMSK                                                        0x7f000000
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_FEC_VALUE_SHFT                                                              0x18
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT7_BMSK                                                          0xff0000
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT7_SHFT                                                              0x10
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT6_BMSK                                                            0xff00
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT6_SHFT                                                               0x8
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT5_BMSK                                                              0xff
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT5_SHFT                                                               0x0

#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000438)
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_RMSK                                                                       0xffffffff
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_USB_VID_PID_LSB_ADDR, HWIO_QFPROM_RAW_USB_VID_PID_LSB_RMSK)
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_USB_VID_PID_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_USB_VID_PID_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_USB_VID_PID_LSB_ADDR,m,v,HWIO_QFPROM_RAW_USB_VID_PID_LSB_IN)
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_PID1_BMSK                                                                  0xffff0000
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_PID1_SHFT                                                                        0x10
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_PID0_BMSK                                                                      0xffff
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_PID0_SHFT                                                                         0x0

#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000043c)
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_RMSK                                                                       0x7f00ffff
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_USB_VID_PID_MSB_ADDR, HWIO_QFPROM_RAW_USB_VID_PID_MSB_RMSK)
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_USB_VID_PID_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_USB_VID_PID_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_USB_VID_PID_MSB_ADDR,m,v,HWIO_QFPROM_RAW_USB_VID_PID_MSB_IN)
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_FEC_VALUE_BMSK                                                             0x7f000000
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_FEC_VALUE_SHFT                                                                   0x18
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_VID_BMSK                                                                       0xffff
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_VID_SHFT                                                                          0x0

#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000440)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_SPARE_BMSK                                                            0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_SPARE_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000444)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_SPARE_BMSK                                                            0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_SPARE_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000448)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_SPARE_BMSK                                                            0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_SPARE_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000044c)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_SPARE_BMSK                                                            0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_SPARE_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000450)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_SPARE_BMSK                                                            0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_SPARE_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000454)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_SPARE_BMSK                                                            0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_SPARE_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000458)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_SPARE_BMSK                                                            0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_SPARE_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000045c)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_SPARE_BMSK                                                            0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_SPARE_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000460)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_SPARE_BMSK                                                            0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_SPARE_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000464)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_SPARE_BMSK                                                            0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_SPARE_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_ADDR                                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000468)
#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_RMSK                                                                          0xffffffff
#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KSV_LSB_ADDR, HWIO_QFPROM_RAW_HDCP_KSV_LSB_RMSK)
#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KSV_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_KSV0_BMSK                                                                     0xffffffff
#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_KSV0_SHFT                                                                            0x0

#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_ADDR                                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000046c)
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_RMSK                                                                          0x7f0000ff
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KSV_MSB_ADDR, HWIO_QFPROM_RAW_HDCP_KSV_MSB_RMSK)
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KSV_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_FEC_VALUE_BMSK                                                                0x7f000000
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_FEC_VALUE_SHFT                                                                      0x18
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_KSV1_BMSK                                                                           0xff
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_KSV1_SHFT                                                                            0x0

#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_ADDR(n)                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000468 + 0x8 * (n))
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_RMSK                                                                     0xffffffff
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_MAXn                                                                             40
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_KEY_DATA_BMSK                                                            0xffffffff
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_KEY_DATA_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_ADDR(n)                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000046c + 0x8 * (n))
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_RMSK                                                                     0x7fffffff
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_MAXn                                                                             40
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_FEC_VALUE_BMSK                                                           0x7f000000
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_FEC_VALUE_SHFT                                                                 0x18
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_KEY_DATA_BMSK                                                              0xffffff
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_KEY_DATA_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_ADDR(n)                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000005b0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_RMSK                                                               0xffffffff
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_MAXn                                                                       36
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_KEY_DATA_BMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_KEY_DATA_SHFT                                                             0x0

#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_ADDR(n)                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000005b4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_RMSK                                                               0x7fffffff
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_MAXn                                                                       36
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_FEC_VALUE_BMSK                                                     0x7f000000
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_FEC_VALUE_SHFT                                                           0x18
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_KEY_DATA_BMSK                                                        0xffffff
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_KEY_DATA_SHFT                                                             0x0

#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_ADDR(n)                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000006d8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_RMSK                                                               0xffffffff
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_MAXn                                                                       36
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_KEY_DATA_BMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_KEY_DATA_SHFT                                                             0x0

#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_ADDR(n)                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000006dc + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_RMSK                                                               0x7fffffff
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_MAXn                                                                       36
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_FEC_VALUE_BMSK                                                     0x7f000000
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_FEC_VALUE_SHFT                                                           0x18
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_KEY_DATA_BMSK                                                        0xffffff
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_KEY_DATA_SHFT                                                             0x0

#define HWIO_ACC_IR_ADDR                                                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002000)
#define HWIO_ACC_IR_RMSK                                                                                                 0x1f
#define HWIO_ACC_IR_OUT(v)      \
        out_dword(HWIO_ACC_IR_ADDR,v)
#define HWIO_ACC_IR_INSTRUCTION_BMSK                                                                                     0x1f
#define HWIO_ACC_IR_INSTRUCTION_SHFT                                                                                      0x0

#define HWIO_ACC_DR_ADDR                                                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002004)
#define HWIO_ACC_DR_RMSK                                                                                           0xffffffff
#define HWIO_ACC_DR_IN          \
        in_dword_masked(HWIO_ACC_DR_ADDR, HWIO_ACC_DR_RMSK)
#define HWIO_ACC_DR_INM(m)      \
        in_dword_masked(HWIO_ACC_DR_ADDR, m)
#define HWIO_ACC_DR_OUT(v)      \
        out_dword(HWIO_ACC_DR_ADDR,v)
#define HWIO_ACC_DR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_ACC_DR_ADDR,m,v,HWIO_ACC_DR_IN)
#define HWIO_ACC_DR_DR_BMSK                                                                                        0xffffffff
#define HWIO_ACC_DR_DR_SHFT                                                                                               0x0

#define HWIO_ACC_VERID_ADDR                                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002008)
#define HWIO_ACC_VERID_RMSK                                                                                            0xffff
#define HWIO_ACC_VERID_IN          \
        in_dword_masked(HWIO_ACC_VERID_ADDR, HWIO_ACC_VERID_RMSK)
#define HWIO_ACC_VERID_INM(m)      \
        in_dword_masked(HWIO_ACC_VERID_ADDR, m)
#define HWIO_ACC_VERID_FWVERID_BMSK                                                                                    0xff00
#define HWIO_ACC_VERID_FWVERID_SHFT                                                                                       0x8
#define HWIO_ACC_VERID_HWVERID_BMSK                                                                                      0xff
#define HWIO_ACC_VERID_HWVERID_SHFT                                                                                       0x0

#define HWIO_ACC_FEATSETn_ADDR(n)                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002010 + 0x4 * (n))
#define HWIO_ACC_FEATSETn_RMSK                                                                                     0xffffffff
#define HWIO_ACC_FEATSETn_MAXn                                                                                              7
#define HWIO_ACC_FEATSETn_INI(n)        \
        in_dword_masked(HWIO_ACC_FEATSETn_ADDR(n), HWIO_ACC_FEATSETn_RMSK)
#define HWIO_ACC_FEATSETn_INMI(n,mask)    \
        in_dword_masked(HWIO_ACC_FEATSETn_ADDR(n), mask)
#define HWIO_ACC_FEATSETn_FEAT_BMSK                                                                                0xffffffff
#define HWIO_ACC_FEATSETn_FEAT_SHFT                                                                                       0x0

#define HWIO_QFPROM_BLOW_TIMER_ADDR                                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002038)
#define HWIO_QFPROM_BLOW_TIMER_RMSK                                                                                     0xfff
#define HWIO_QFPROM_BLOW_TIMER_IN          \
        in_dword_masked(HWIO_QFPROM_BLOW_TIMER_ADDR, HWIO_QFPROM_BLOW_TIMER_RMSK)
#define HWIO_QFPROM_BLOW_TIMER_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BLOW_TIMER_ADDR, m)
#define HWIO_QFPROM_BLOW_TIMER_OUT(v)      \
        out_dword(HWIO_QFPROM_BLOW_TIMER_ADDR,v)
#define HWIO_QFPROM_BLOW_TIMER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_BLOW_TIMER_ADDR,m,v,HWIO_QFPROM_BLOW_TIMER_IN)
#define HWIO_QFPROM_BLOW_TIMER_BLOW_TIMER_BMSK                                                                          0xfff
#define HWIO_QFPROM_BLOW_TIMER_BLOW_TIMER_SHFT                                                                            0x0

#define HWIO_QFPROM_TEST_CTRL_ADDR                                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000203c)
#define HWIO_QFPROM_TEST_CTRL_RMSK                                                                                        0xf
#define HWIO_QFPROM_TEST_CTRL_IN          \
        in_dword_masked(HWIO_QFPROM_TEST_CTRL_ADDR, HWIO_QFPROM_TEST_CTRL_RMSK)
#define HWIO_QFPROM_TEST_CTRL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_TEST_CTRL_ADDR, m)
#define HWIO_QFPROM_TEST_CTRL_OUT(v)      \
        out_dword(HWIO_QFPROM_TEST_CTRL_ADDR,v)
#define HWIO_QFPROM_TEST_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_TEST_CTRL_ADDR,m,v,HWIO_QFPROM_TEST_CTRL_IN)
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_ROM_BMSK                                                                            0x8
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_ROM_SHFT                                                                            0x3
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_WL_BMSK                                                                             0x4
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_WL_SHFT                                                                             0x2
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_BL_BMSK                                                                             0x2
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_BL_SHFT                                                                             0x1
#define HWIO_QFPROM_TEST_CTRL_EN_FUSE_RES_MEAS_BMSK                                                                       0x1
#define HWIO_QFPROM_TEST_CTRL_EN_FUSE_RES_MEAS_SHFT                                                                       0x0

#define HWIO_QFPROM_ACCEL_ADDR                                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002040)
#define HWIO_QFPROM_ACCEL_RMSK                                                                                          0xfff
#define HWIO_QFPROM_ACCEL_IN          \
        in_dword_masked(HWIO_QFPROM_ACCEL_ADDR, HWIO_QFPROM_ACCEL_RMSK)
#define HWIO_QFPROM_ACCEL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_ACCEL_ADDR, m)
#define HWIO_QFPROM_ACCEL_OUT(v)      \
        out_dword(HWIO_QFPROM_ACCEL_ADDR,v)
#define HWIO_QFPROM_ACCEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_ACCEL_ADDR,m,v,HWIO_QFPROM_ACCEL_IN)
#define HWIO_QFPROM_ACCEL_QFPROM_GATELAST_BMSK                                                                          0x800
#define HWIO_QFPROM_ACCEL_QFPROM_GATELAST_SHFT                                                                            0xb
#define HWIO_QFPROM_ACCEL_QFPROM_TRIPPT_SEL_BMSK                                                                        0x700
#define HWIO_QFPROM_ACCEL_QFPROM_TRIPPT_SEL_SHFT                                                                          0x8
#define HWIO_QFPROM_ACCEL_QFPROM_ACCEL_BMSK                                                                              0xff
#define HWIO_QFPROM_ACCEL_QFPROM_ACCEL_SHFT                                                                               0x0

#define HWIO_ACC_STATE_ADDR                                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002044)
#define HWIO_ACC_STATE_RMSK                                                                                               0x7
#define HWIO_ACC_STATE_IN          \
        in_dword_masked(HWIO_ACC_STATE_ADDR, HWIO_ACC_STATE_RMSK)
#define HWIO_ACC_STATE_INM(m)      \
        in_dword_masked(HWIO_ACC_STATE_ADDR, m)
#define HWIO_ACC_STATE_ACC_READY_BMSK                                                                                     0x4
#define HWIO_ACC_STATE_ACC_READY_SHFT                                                                                     0x2
#define HWIO_ACC_STATE_ACC_LOCKED_BMSK                                                                                    0x2
#define HWIO_ACC_STATE_ACC_LOCKED_SHFT                                                                                    0x1
#define HWIO_ACC_STATE_ACC_STOP_BMSK                                                                                      0x1
#define HWIO_ACC_STATE_ACC_STOP_SHFT                                                                                      0x0

#define HWIO_QFPROM_BLOW_STATUS_ADDR                                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002048)
#define HWIO_QFPROM_BLOW_STATUS_RMSK                                                                                      0x3
#define HWIO_QFPROM_BLOW_STATUS_IN          \
        in_dword_masked(HWIO_QFPROM_BLOW_STATUS_ADDR, HWIO_QFPROM_BLOW_STATUS_RMSK)
#define HWIO_QFPROM_BLOW_STATUS_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BLOW_STATUS_ADDR, m)
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_WR_ERR_BMSK                                                                        0x2
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_WR_ERR_SHFT                                                                        0x1
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_BUSY_BMSK                                                                          0x1
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_BUSY_SHFT                                                                          0x0

#define HWIO_HW_KEY_STATUS_ADDR                                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000204c)
#define HWIO_HW_KEY_STATUS_RMSK                                                                                          0x1f
#define HWIO_HW_KEY_STATUS_IN          \
        in_dword_masked(HWIO_HW_KEY_STATUS_ADDR, HWIO_HW_KEY_STATUS_RMSK)
#define HWIO_HW_KEY_STATUS_INM(m)      \
        in_dword_masked(HWIO_HW_KEY_STATUS_ADDR, m)
#define HWIO_HW_KEY_STATUS_KDF_DONE_BMSK                                                                                 0x10
#define HWIO_HW_KEY_STATUS_KDF_DONE_SHFT                                                                                  0x4
#define HWIO_HW_KEY_STATUS_MSA_KEYS_BLOCKED_BMSK                                                                          0x8
#define HWIO_HW_KEY_STATUS_MSA_KEYS_BLOCKED_SHFT                                                                          0x3
#define HWIO_HW_KEY_STATUS_APPS_KEYS_BLOCKED_BMSK                                                                         0x4
#define HWIO_HW_KEY_STATUS_APPS_KEYS_BLOCKED_SHFT                                                                         0x2
#define HWIO_HW_KEY_STATUS_SEC_KEY_DERIVATION_KEY_BLOWN_BMSK                                                              0x2
#define HWIO_HW_KEY_STATUS_SEC_KEY_DERIVATION_KEY_BLOWN_SHFT                                                              0x1
#define HWIO_HW_KEY_STATUS_PRI_KEY_DERIVATION_KEY_BLOWN_BMSK                                                              0x1
#define HWIO_HW_KEY_STATUS_PRI_KEY_DERIVATION_KEY_BLOWN_SHFT                                                              0x0

#define HWIO_QFPROM_BIST_CTRL_ADDR                                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002050)
#define HWIO_QFPROM_BIST_CTRL_RMSK                                                                                       0x7f
#define HWIO_QFPROM_BIST_CTRL_IN          \
        in_dword_masked(HWIO_QFPROM_BIST_CTRL_ADDR, HWIO_QFPROM_BIST_CTRL_RMSK)
#define HWIO_QFPROM_BIST_CTRL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BIST_CTRL_ADDR, m)
#define HWIO_QFPROM_BIST_CTRL_OUT(v)      \
        out_dword(HWIO_QFPROM_BIST_CTRL_ADDR,v)
#define HWIO_QFPROM_BIST_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_BIST_CTRL_ADDR,m,v,HWIO_QFPROM_BIST_CTRL_IN)
#define HWIO_QFPROM_BIST_CTRL_AUTH_REGION_BMSK                                                                           0x7c
#define HWIO_QFPROM_BIST_CTRL_AUTH_REGION_SHFT                                                                            0x2
#define HWIO_QFPROM_BIST_CTRL_SHA_ENABLE_BMSK                                                                             0x2
#define HWIO_QFPROM_BIST_CTRL_SHA_ENABLE_SHFT                                                                             0x1
#define HWIO_QFPROM_BIST_CTRL_START_BMSK                                                                                  0x1
#define HWIO_QFPROM_BIST_CTRL_START_SHFT                                                                                  0x0

#define HWIO_QFPROM_BIST_ERROR_ADDR                                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002054)
#define HWIO_QFPROM_BIST_ERROR_RMSK                                                                                0xffffffff
#define HWIO_QFPROM_BIST_ERROR_IN          \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR_ADDR, HWIO_QFPROM_BIST_ERROR_RMSK)
#define HWIO_QFPROM_BIST_ERROR_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR_ADDR, m)
#define HWIO_QFPROM_BIST_ERROR_ERROR_BMSK                                                                          0xffffffff
#define HWIO_QFPROM_BIST_ERROR_ERROR_SHFT                                                                                 0x0

#define HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n)                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002060 + 0x4 * (n))
#define HWIO_QFPROM_HASH_SIGNATUREn_RMSK                                                                           0xffffffff
#define HWIO_QFPROM_HASH_SIGNATUREn_MAXn                                                                                    7
#define HWIO_QFPROM_HASH_SIGNATUREn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n), HWIO_QFPROM_HASH_SIGNATUREn_RMSK)
#define HWIO_QFPROM_HASH_SIGNATUREn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n), mask)
#define HWIO_QFPROM_HASH_SIGNATUREn_HASH_VALUE_BMSK                                                                0xffffffff
#define HWIO_QFPROM_HASH_SIGNATUREn_HASH_VALUE_SHFT                                                                       0x0

#define HWIO_FEC_ESR_ADDR                                                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002080)
#define HWIO_FEC_ESR_RMSK                                                                                               0xfff
#define HWIO_FEC_ESR_IN          \
        in_dword_masked(HWIO_FEC_ESR_ADDR, HWIO_FEC_ESR_RMSK)
#define HWIO_FEC_ESR_INM(m)      \
        in_dword_masked(HWIO_FEC_ESR_ADDR, m)
#define HWIO_FEC_ESR_OUT(v)      \
        out_dword(HWIO_FEC_ESR_ADDR,v)
#define HWIO_FEC_ESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FEC_ESR_ADDR,m,v,HWIO_FEC_ESR_IN)
#define HWIO_FEC_ESR_CORR_SW_ACC_BMSK                                                                                   0x800
#define HWIO_FEC_ESR_CORR_SW_ACC_SHFT                                                                                     0xb
#define HWIO_FEC_ESR_CORR_HDCP_BMSK                                                                                     0x400
#define HWIO_FEC_ESR_CORR_HDCP_SHFT                                                                                       0xa
#define HWIO_FEC_ESR_CORR_BOOT_ROM_BMSK                                                                                 0x200
#define HWIO_FEC_ESR_CORR_BOOT_ROM_SHFT                                                                                   0x9
#define HWIO_FEC_ESR_CORR_FUSE_SENSE_BMSK                                                                               0x100
#define HWIO_FEC_ESR_CORR_FUSE_SENSE_SHFT                                                                                 0x8
#define HWIO_FEC_ESR_CORR_MULT_BMSK                                                                                      0x80
#define HWIO_FEC_ESR_CORR_MULT_SHFT                                                                                       0x7
#define HWIO_FEC_ESR_CORR_SEEN_BMSK                                                                                      0x40
#define HWIO_FEC_ESR_CORR_SEEN_SHFT                                                                                       0x6
#define HWIO_FEC_ESR_ERR_SW_ACC_BMSK                                                                                     0x20
#define HWIO_FEC_ESR_ERR_SW_ACC_SHFT                                                                                      0x5
#define HWIO_FEC_ESR_ERR_HDCP_BMSK                                                                                       0x10
#define HWIO_FEC_ESR_ERR_HDCP_SHFT                                                                                        0x4
#define HWIO_FEC_ESR_ERR_BOOT_ROM_BMSK                                                                                    0x8
#define HWIO_FEC_ESR_ERR_BOOT_ROM_SHFT                                                                                    0x3
#define HWIO_FEC_ESR_ERR_FUSE_SENSE_BMSK                                                                                  0x4
#define HWIO_FEC_ESR_ERR_FUSE_SENSE_SHFT                                                                                  0x2
#define HWIO_FEC_ESR_ERR_MULT_BMSK                                                                                        0x2
#define HWIO_FEC_ESR_ERR_MULT_SHFT                                                                                        0x1
#define HWIO_FEC_ESR_ERR_SEEN_BMSK                                                                                        0x1
#define HWIO_FEC_ESR_ERR_SEEN_SHFT                                                                                        0x0

#define HWIO_FEC_EAR_ADDR                                                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002084)
#define HWIO_FEC_EAR_RMSK                                                                                          0xffffffff
#define HWIO_FEC_EAR_IN          \
        in_dword_masked(HWIO_FEC_EAR_ADDR, HWIO_FEC_EAR_RMSK)
#define HWIO_FEC_EAR_INM(m)      \
        in_dword_masked(HWIO_FEC_EAR_ADDR, m)
#define HWIO_FEC_EAR_CORR_ADDR_BMSK                                                                                0xffff0000
#define HWIO_FEC_EAR_CORR_ADDR_SHFT                                                                                      0x10
#define HWIO_FEC_EAR_ERR_ADDR_BMSK                                                                                     0xffff
#define HWIO_FEC_EAR_ERR_ADDR_SHFT                                                                                        0x0

#define HWIO_RESET_JDR_STATUS_ADDR                                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002088)
#define HWIO_RESET_JDR_STATUS_RMSK                                                                                        0x3
#define HWIO_RESET_JDR_STATUS_IN          \
        in_dword_masked(HWIO_RESET_JDR_STATUS_ADDR, HWIO_RESET_JDR_STATUS_RMSK)
#define HWIO_RESET_JDR_STATUS_INM(m)      \
        in_dword_masked(HWIO_RESET_JDR_STATUS_ADDR, m)
#define HWIO_RESET_JDR_STATUS_FORCE_RESET_BMSK                                                                            0x2
#define HWIO_RESET_JDR_STATUS_FORCE_RESET_SHFT                                                                            0x1
#define HWIO_RESET_JDR_STATUS_DISABLE_SYSTEM_RESET_BMSK                                                                   0x1
#define HWIO_RESET_JDR_STATUS_DISABLE_SYSTEM_RESET_SHFT                                                                   0x0

#define HWIO_QFPROM_ROM_ERROR_ADDR                                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002090)
#define HWIO_QFPROM_ROM_ERROR_RMSK                                                                                        0x1
#define HWIO_QFPROM_ROM_ERROR_IN          \
        in_dword_masked(HWIO_QFPROM_ROM_ERROR_ADDR, HWIO_QFPROM_ROM_ERROR_RMSK)
#define HWIO_QFPROM_ROM_ERROR_INM(m)      \
        in_dword_masked(HWIO_QFPROM_ROM_ERROR_ADDR, m)
#define HWIO_QFPROM_ROM_ERROR_ERROR_BMSK                                                                                  0x1
#define HWIO_QFPROM_ROM_ERROR_ERROR_SHFT                                                                                  0x0

#define HWIO_QFPROM_CORR_ACC_PRIVATEn_ADDR(n)                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004000 + 0x4 * (n))
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_RMSK                                                                         0xffffffff
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_MAXn                                                                                 39
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ACC_PRIVATEn_ADDR(n), HWIO_QFPROM_CORR_ACC_PRIVATEn_RMSK)
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ACC_PRIVATEn_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_ACC_PRIVATE_BMSK                                                             0xffffffff
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_ACC_PRIVATE_SHFT                                                                    0x0

#define HWIO_QFPROM_CORR_JTAG_ID_LSB_ADDR                                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040a0)
#define HWIO_QFPROM_CORR_JTAG_ID_LSB_RMSK                                                                             0xfffff
#define HWIO_QFPROM_CORR_JTAG_ID_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_LSB_ADDR, HWIO_QFPROM_CORR_JTAG_ID_LSB_RMSK)
#define HWIO_QFPROM_CORR_JTAG_ID_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_JTAG_ID_LSB_JTAG_ID_BMSK                                                                     0xfffff
#define HWIO_QFPROM_CORR_JTAG_ID_LSB_JTAG_ID_SHFT                                                                         0x0

#define HWIO_QFPROM_CORR_JTAG_ID_MSB_ADDR                                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040a4)
#define HWIO_QFPROM_CORR_JTAG_ID_MSB_RMSK                                                                          0xffffffff
#define HWIO_QFPROM_CORR_JTAG_ID_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_MSB_ADDR, HWIO_QFPROM_CORR_JTAG_ID_MSB_RMSK)
#define HWIO_QFPROM_CORR_JTAG_ID_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_JTAG_ID_MSB_QFPROM_CORR_JTAG_ID_MSB_BMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_JTAG_ID_MSB_QFPROM_CORR_JTAG_ID_MSB_SHFT                                                         0x0

#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040a8)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RMSK                                                                       0xffffffff
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR, HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RMSK)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CUST_PRIV_KEY1_BMSK                                                        0x80000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CUST_PRIV_KEY1_SHFT                                                              0x1f
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CUST_PRIV_KEY0_BMSK                                                        0x40000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CUST_PRIV_KEY0_SHFT                                                              0x1e
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_HDCP_KEY_BMSK                                                              0x20000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_HDCP_KEY_SHFT                                                                    0x1d
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE28_BMSK                                                               0x10000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE28_SHFT                                                                     0x1c
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE27_BMSK                                                                0x8000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE27_SHFT                                                                     0x1b
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_USB_VID_PID_BMSK                                                            0x4000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_USB_VID_PID_SHFT                                                                 0x1a
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_QC_SEC_BOOT_BMSK                                                            0x2000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_QC_SEC_BOOT_SHFT                                                                 0x19
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_SEC_BOOT_BMSK                                                           0x1000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_SEC_BOOT_SHFT                                                                0x18
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_PK_HASH_BMSK                                                             0x800000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_PK_HASH_SHFT                                                                 0x17
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                                  0x400000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                                      0x16
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                                  0x200000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                                      0x15
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_BOOT_ROM_PATCH_BMSK                                                          0x100000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_BOOT_ROM_PATCH_SHFT                                                              0x14
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE19_BMSK                                                                  0x80000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE19_SHFT                                                                     0x13
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SERIAL_NUM_BMSK                                                               0x40000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SERIAL_NUM_SHFT                                                                  0x12
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CUST_KEY_BMSK                                                                 0x20000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CUST_KEY_SHFT                                                                    0x11
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_INTAGLIO_GLUEBIT_BMSK                                                         0x10000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_INTAGLIO_GLUEBIT_SHFT                                                            0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEC_EN_BMSK                                                                    0x8000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEC_EN_SHFT                                                                       0xf
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB_BMSK                                                                     0x4000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB_SHFT                                                                        0xe
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_ACCEL_BMSK                                                                 0x2000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_ACCEL_SHFT                                                                    0xd
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_DEBUG_KEY_BMSK                                                                 0x1000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_DEBUG_KEY_SHFT                                                                    0xc
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_REDUN_BMSK                                                                  0x800
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_REDUN_SHFT                                                                    0xb
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEAT_CONFIG_BMSK                                                                0x400
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEAT_CONFIG_SHFT                                                                  0xa
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_CONFIG_BMSK                                                                 0x200
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_CONFIG_SHFT                                                                   0x9
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IMEI_ESN2_BMSK                                                                  0x100
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IMEI_ESN2_SHFT                                                                    0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IMEI_ESN1_BMSK                                                                   0x80
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IMEI_ESN1_SHFT                                                                    0x7
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IMEI_ESN0_BMSK                                                                   0x40
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IMEI_ESN0_SHFT                                                                    0x6
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MSA_ANTI_ROLLBACK_BMSK                                                           0x20
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MSA_ANTI_ROLLBACK_SHFT                                                            0x5
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_AP_ANTI_ROLLBACK_BMSK                                                            0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_AP_ANTI_ROLLBACK_SHFT                                                             0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PTE_BMSK                                                                          0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PTE_SHFT                                                                          0x3
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RD_WR_PERM_BMSK                                                                   0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RD_WR_PERM_SHFT                                                                   0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_JTAG_ID_BMSK                                                                      0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_JTAG_ID_SHFT                                                                      0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ACC_PRIVATE_BMSK                                                                  0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ACC_PRIVATE_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040ac)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RMSK                                                                       0xffffffff
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR, HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RMSK)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CUST_PRIV_KEY1_BMSK                                                        0x80000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CUST_PRIV_KEY1_SHFT                                                              0x1f
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CUST_PRIV_KEY0_BMSK                                                        0x40000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CUST_PRIV_KEY0_SHFT                                                              0x1e
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_HDCP_KEY_BMSK                                                              0x20000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_HDCP_KEY_SHFT                                                                    0x1d
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE28_BMSK                                                               0x10000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE28_SHFT                                                                     0x1c
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE27_BMSK                                                                0x8000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE27_SHFT                                                                     0x1b
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_USB_VID_PID_BMSK                                                            0x4000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_USB_VID_PID_SHFT                                                                 0x1a
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_QC_SEC_BOOT_BMSK                                                            0x2000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_QC_SEC_BOOT_SHFT                                                                 0x19
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_SEC_BOOT_BMSK                                                           0x1000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_SEC_BOOT_SHFT                                                                0x18
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_PK_HASH_BMSK                                                             0x800000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_PK_HASH_SHFT                                                                 0x17
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_BMSK                                                  0x400000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_SHFT                                                      0x16
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_BMSK                                                  0x200000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_SHFT                                                      0x15
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_BOOT_ROM_PATCH_BMSK                                                          0x100000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_BOOT_ROM_PATCH_SHFT                                                              0x14
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE19_BMSK                                                                  0x80000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE19_SHFT                                                                     0x13
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SERIAL_NUM_BMSK                                                               0x40000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SERIAL_NUM_SHFT                                                                  0x12
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CUST_KEY_BMSK                                                                 0x20000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CUST_KEY_SHFT                                                                    0x11
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_INTAGLIO_GLUEBIT_BMSK                                                         0x10000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_INTAGLIO_GLUEBIT_SHFT                                                            0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEC_EN_BMSK                                                                    0x8000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEC_EN_SHFT                                                                       0xf
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB_BMSK                                                                     0x4000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB_SHFT                                                                        0xe
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_ACCEL_BMSK                                                                 0x2000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_ACCEL_SHFT                                                                    0xd
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_DEBUG_KEY_BMSK                                                                 0x1000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_DEBUG_KEY_SHFT                                                                    0xc
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_REDUN_BMSK                                                                  0x800
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_REDUN_SHFT                                                                    0xb
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEAT_CONFIG_BMSK                                                                0x400
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEAT_CONFIG_SHFT                                                                  0xa
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_CONFIG_BMSK                                                                 0x200
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_CONFIG_SHFT                                                                   0x9
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IMEI_ESN2_BMSK                                                                  0x100
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IMEI_ESN2_SHFT                                                                    0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IMEI_ESN1_BMSK                                                                   0x80
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IMEI_ESN1_SHFT                                                                    0x7
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IMEI_ESN0_BMSK                                                                   0x40
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IMEI_ESN0_SHFT                                                                    0x6
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MSA_ANTI_ROLLBACK_BMSK                                                           0x20
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MSA_ANTI_ROLLBACK_SHFT                                                            0x5
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_AP_ANTI_ROLLBACK_BMSK                                                            0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_AP_ANTI_ROLLBACK_SHFT                                                             0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PTE_BMSK                                                                          0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PTE_SHFT                                                                          0x3
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RD_WR_PERM_BMSK                                                                   0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RD_WR_PERM_SHFT                                                                   0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_JTAG_ID_BMSK                                                                      0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_JTAG_ID_SHFT                                                                      0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ACC_PRIVATE_BMSK                                                                  0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ACC_PRIVATE_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_PTE_LSB_ADDR                                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040b0)
#define HWIO_QFPROM_CORR_PTE_LSB_RMSK                                                                              0xffffffff
#define HWIO_QFPROM_CORR_PTE_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_LSB_ADDR, HWIO_QFPROM_CORR_PTE_LSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_PTE_LSB_ADDR,v)
#define HWIO_QFPROM_CORR_PTE_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_PTE_LSB_ADDR,m,v,HWIO_QFPROM_CORR_PTE_LSB_IN)
#define HWIO_QFPROM_CORR_PTE_LSB_QFPROM_CORR_PTE_LSB_BMSK                                                          0xffffffff
#define HWIO_QFPROM_CORR_PTE_LSB_QFPROM_CORR_PTE_LSB_SHFT                                                                 0x0

#define HWIO_QFPROM_CORR_PTE_MSB_ADDR                                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040b4)
#define HWIO_QFPROM_CORR_PTE_MSB_RMSK                                                                                     0xf
#define HWIO_QFPROM_CORR_PTE_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_MSB_ADDR, HWIO_QFPROM_CORR_PTE_MSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_MSB_SPEED_BIN_BMSK                                                                           0xf
#define HWIO_QFPROM_CORR_PTE_MSB_SPEED_BIN_SHFT                                                                           0x0

#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040b8)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_RMSK                                                            0xffffffff
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_TZ_BMSK                                                         0xffff0000
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_TZ_SHFT                                                               0x10
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_SBL1_BMSK                                                           0xffff
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_SBL1_SHFT                                                              0x0

#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040bc)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_RMSK                                                            0xffffffff
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_BMSK                      0xffffffff
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_SHFT                             0x0

#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040c0)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_RMSK                                                            0xffffffff
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_APPSBL0_BMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_APPSBL0_SHFT                                                           0x0

#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040c4)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_RMSK                                                             0x3ffffff
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_RPM_BMSK                                                         0x3fc0000
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_RPM_SHFT                                                              0x12
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_APPSBL1_BMSK                                                       0x3ffff
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_APPSBL1_SHFT                                                           0x0

#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040c8)
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_RMSK                                                                0xffffffff
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_ADDR, HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_RMSK)
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_MSS_BMSK                                                            0xffff0000
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_MSS_SHFT                                                                  0x10
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_MBA_BMSK                                                                0xffff
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_MBA_SHFT                                                                   0x0

#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040cc)
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_RMSK                                                                0xffffffff
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_ADDR, HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_RMSK)
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_BMSK                              0xffffffff
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_SHFT                                     0x0

#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040d0)
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN0_LSB_ADDR, HWIO_QFPROM_CORR_IMEI_ESN0_LSB_RMSK)
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_IMEI_ESN_DATA0_BMSK                                                         0xffffffff
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_IMEI_ESN_DATA0_SHFT                                                                0x0

#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040d4)
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_RMSK                                                                         0x7ffffff
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN0_MSB_ADDR, HWIO_QFPROM_CORR_IMEI_ESN0_MSB_RMSK)
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_IMEI_ESN_DATA1_BMSK                                                          0x7ffffff
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_IMEI_ESN_DATA1_SHFT                                                                0x0

#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040d8)
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN1_LSB_ADDR, HWIO_QFPROM_CORR_IMEI_ESN1_LSB_RMSK)
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_IMEI_ESN_DATA0_BMSK                                                         0xffffffff
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_IMEI_ESN_DATA0_SHFT                                                                0x0

#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040dc)
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_RMSK                                                                         0x7ffffff
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN1_MSB_ADDR, HWIO_QFPROM_CORR_IMEI_ESN1_MSB_RMSK)
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_IMEI_ESN_DATA1_BMSK                                                          0x7ffffff
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_IMEI_ESN_DATA1_SHFT                                                                0x0

#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040e0)
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN2_LSB_ADDR, HWIO_QFPROM_CORR_IMEI_ESN2_LSB_RMSK)
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_IMEI_ESN_DATA0_BMSK                                                         0xffffffff
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_IMEI_ESN_DATA0_SHFT                                                                0x0

#define HWIO_QFPROM_CORR_IMEI_ESN2_MSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040e4)
#define HWIO_QFPROM_CORR_IMEI_ESN2_MSB_RMSK                                                                         0x7ffffff
#define HWIO_QFPROM_CORR_IMEI_ESN2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN2_MSB_ADDR, HWIO_QFPROM_CORR_IMEI_ESN2_MSB_RMSK)
#define HWIO_QFPROM_CORR_IMEI_ESN2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_IMEI_ESN2_MSB_IMEI_ESN_DATA1_BMSK                                                          0x7ffffff
#define HWIO_QFPROM_CORR_IMEI_ESN2_MSB_IMEI_ESN_DATA1_SHFT                                                                0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040e8)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE1_DISABLE_BMSK                                                   0x80000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE1_DISABLE_SHFT                                                         0x1f
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE0_DISABLE_BMSK                                                   0x40000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE0_DISABLE_SHFT                                                         0x1e
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_VENUS_0_DBGEN_DISABLE_BMSK                                            0x20000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_VENUS_0_DBGEN_DISABLE_SHFT                                                  0x1d
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RPM_DAPEN_DISABLE_BMSK                                                0x10000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RPM_DAPEN_DISABLE_SHFT                                                      0x1c
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RPM_LPASS_WCSS_NIDEN_DISABLE_BMSK                                      0x8000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RPM_LPASS_WCSS_NIDEN_DISABLE_SHFT                                           0x1b
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RPM_DBGEN_DISABLE_BMSK                                                 0x4000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RPM_DBGEN_DISABLE_SHFT                                                      0x1a
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_WCSS_DBGEN_DISABLE_BMSK                                                0x2000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_WCSS_DBGEN_DISABLE_SHFT                                                     0x19
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_LPASS_DBGEN_DISABLE_BMSK                                               0x1000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_LPASS_DBGEN_DISABLE_SHFT                                                    0x18
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_MSS_NIDEN_DISABLE_BMSK                                                  0x800000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_MSS_NIDEN_DISABLE_SHFT                                                      0x17
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_MSS_DBGEN_DISABLE_BMSK                                                  0x400000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_MSS_DBGEN_DISABLE_SHFT                                                      0x16
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_BMSK                                                  0x200000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_SHFT                                                      0x15
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE1_BMSK                                                             0x1e0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE1_SHFT                                                                 0x11
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_BMSK                                       0x10000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_SHFT                                          0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_BMSK                                                  0x8000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_SHFT                                                     0xf
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_BMSK                                                            0x7c00
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SHFT                                                               0xa
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDP_ENUM_SKIP_BMSK                                                         0x200
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDP_ENUM_SKIP_SHFT                                                           0x9
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_BMSK                                                   0x100
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_SHFT                                                     0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_APPS_PBL_BOOT_SPEED_BMSK                                                    0xc0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_APPS_PBL_BOOT_SPEED_SHFT                                                     0x6
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_BMSK                                                 0x20
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_SHFT                                                  0x5
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE0_BMSK                                                                 0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE0_SHFT                                                                  0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_PMIC_POWER_DOWN_MSM_BMSK                                                 0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_PMIC_POWER_DOWN_MSM_SHFT                                                 0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_USB_ENUM_TIMEOUT_BMSK                                                        0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_USB_ENUM_TIMEOUT_SHFT                                                        0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DLOAD_TIMEOUT_BMSK                                                           0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DLOAD_TIMEOUT_SHFT                                                           0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_BMSK                                                         0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_SHFT                                                         0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040ec)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_WDOG_EN_BMSK                                                          0x80000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_WDOG_EN_SHFT                                                                0x1f
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE2_BMSK                                                           0x78000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE2_SHFT                                                                 0x1b
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPDM_SECURE_MODE_BMSK                                                  0x4000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPDM_SECURE_MODE_SHFT                                                       0x1a
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_IMEI_ESN_SELECT_BMSK                                                   0x3000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_IMEI_ESN_SELECT_SHFT                                                        0x18
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE1_BMSK                                                             0xc00000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE1_SHFT                                                                 0x16
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_CUST_PRIV_KEY_SELECT_BMSK                                               0x200000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_CUST_PRIV_KEY_SELECT_SHFT                                                   0x15
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SW_FUSE_PROG_DISABLE_BMSK                                               0x100000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SW_FUSE_PROG_DISABLE_SHFT                                                   0x14
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE0_BMSK                                                              0xffe00
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE0_SHFT                                                                  0x9
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_BMSK                                                  0x100
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_SHFT                                                    0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_BMSK                                                    0x80
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_SHFT                                                     0x7
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_BMSK                                                     0x40
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_SHFT                                                      0x6
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_BMSK                                                      0x20
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_SHFT                                                       0x5
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_BMSK                                                      0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_SHFT                                                       0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_BMSK                                                    0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_SHFT                                                    0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_BMSK                                                     0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_SHFT                                                     0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_BMSK                                                      0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_SHFT                                                      0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_BMSK                                                      0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_SHFT                                                      0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040f0)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_BMSK                                                        0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_SHFT                                                              0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_SPARE0_BMSK                                                               0xffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_SPARE0_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040f4)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_SPARE0_BMSK                                                           0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_SPARE0_SHFT                                                                 0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_OEM_PRODUCT_ID_BMSK                                                       0xffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_OEM_PRODUCT_ID_SHFT                                                          0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040f8)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_BMSK                                                    0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_SHFT                                                          0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_EDP_DISABLE_BMSK                                                     0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_EDP_DISABLE_SHFT                                                           0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_BMSK                                                   0x20000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_SHFT                                                         0x1d
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ISP_1_DISABLE_BMSK                                                   0x10000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ISP_1_DISABLE_SHFT                                                         0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_BMSK                                              0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_SHFT                                                   0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_BMSK                                              0x4000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_SHFT                                                   0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_3_DISABLE_BMSK                                                   0x2000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_3_DISABLE_SHFT                                                        0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_2_DISABLE_BMSK                                                   0x1000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_2_DISABLE_SHFT                                                        0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MACROVISION_DISABLE_BMSK                                               0x800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MACROVISION_DISABLE_SHFT                                                   0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_SPARE_DISABLE_BMSK                                               0x700000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_SPARE_DISABLE_SHFT                                                   0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_BMSK                                                0x80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_SHFT                                                   0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_BMSK                                                0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_SHFT                                                   0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_BMSK                                               0x20000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_SHFT                                                  0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DC_DISABLE_BMSK                                              0x10000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DC_DISABLE_SHFT                                                 0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_MIMO_DISABLE_BMSK                                             0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_MIMO_DISABLE_SHFT                                                0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT2_DISABLE_BMSK                                        0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT2_DISABLE_SHFT                                           0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT1_DISABLE_BMSK                                        0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT1_DISABLE_SHFT                                           0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_TDSCDMA_DISABLE_BMSK                                               0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_TDSCDMA_DISABLE_SHFT                                                  0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DISABLE_BMSK                                                   0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DISABLE_SHFT                                                     0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_DISABLE_BMSK                                                    0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_DISABLE_SHFT                                                      0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_WCDMA_DISABLE_BMSK                                                  0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_WCDMA_DISABLE_SHFT                                                    0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_DO_DISABLE_BMSK                                                     0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_DO_DISABLE_SHFT                                                       0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_1X_DISABLE_BMSK                                                      0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_1X_DISABLE_SHFT                                                       0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_BMSK                                                           0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_SHFT                                                            0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_GSM_DISABLE_BMSK                                                           0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_GSM_DISABLE_SHFT                                                            0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPI_SLAVE_DISABLE_BMSK                                                     0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPI_SLAVE_DISABLE_SHFT                                                      0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_BMSK                                                       0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_SHFT                                                       0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_BMSK                                                 0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040fc)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENDOR_LOCK_BMSK                                                     0xf0000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENDOR_LOCK_SHFT                                                           0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                                      0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                                           0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SPARE0_BMSK                                                           0x7ff8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SPARE0_SHFT                                                                 0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MULTIMEDIA_SPARE_DISABLE_BMSK                                            0x7800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MULTIMEDIA_SPARE_DISABLE_SHFT                                               0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PCIE_DISABLE_BMSK                                                         0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PCIE_DISABLE_SHFT                                                           0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SATA_DISABLE_BMSK                                                         0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SATA_DISABLE_SHFT                                                           0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ACC_DISABLE_BMSK                                                          0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ACC_DISABLE_SHFT                                                            0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VPE_DISABLE_BMSK                                                           0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VPE_DISABLE_SHFT                                                            0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VFE_DISABLE_BMSK                                                           0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VFE_DISABLE_SHFT                                                            0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_JPEG_ENCODER_DISABLE_BMSK                                                  0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_JPEG_ENCODER_DISABLE_SHFT                                                   0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_JPEG_DECODER_DISABLE_BMSK                                                  0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_JPEG_DECODER_DISABLE_SHFT                                                   0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_FIELD_3D_GRP_DISABLE_BMSK                                                   0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_FIELD_3D_GRP_DISABLE_SHFT                                                   0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VIDEO_ENCODER_DISABLE_BMSK                                                  0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VIDEO_ENCODER_DISABLE_SHFT                                                  0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VIDEO_CODEC_DISABLE_BMSK                                                    0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VIDEO_CODEC_DISABLE_SHFT                                                    0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_HDMI_DISABLE_BMSK                                                           0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_HDMI_DISABLE_SHFT                                                           0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004100)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SPARE1_BMSK                                                          0xffc00000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SPARE1_SHFT                                                                0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_BMSK                                           0x200000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_SHFT                                               0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_BMSK                                            0x100000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_SHFT                                                0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_BMSK                                              0x80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_SHFT                                                 0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_BMSK                                               0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_SHFT                                                  0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_BMSK                                               0x20000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_SHFT                                                  0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_BMSK                                            0x10000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_SHFT                                               0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_BMSK                                              0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_SHFT                                                 0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_BMSK                                               0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_SHFT                                                  0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_BMSK                                               0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_SHFT                                                  0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SPARE1_DISABLE_BMSK                                                   0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SPARE1_DISABLE_SHFT                                                      0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SPARE0_DISABLE_BMSK                                                    0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SPARE0_DISABLE_SHFT                                                      0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_BMSK                                             0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_SHFT                                               0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_BMSK                                                 0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_SHFT                                                   0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_LPASS_WCSS_NIDEN_DISABLE_BMSK                                      0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_LPASS_WCSS_NIDEN_DISABLE_SHFT                                        0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_BMSK                                                  0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_SHFT                                                   0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_BMSK                                                 0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_SHFT                                                  0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_LPASS_DBGEN_DISABLE_BMSK                                                0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_LPASS_DBGEN_DISABLE_SHFT                                                 0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_BMSK                                                  0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_SHFT                                                   0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_BMSK                                                   0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_SHFT                                                   0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SPARE0_BMSK                                                                 0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SPARE0_SHFT                                                                 0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_BMSK                                                        0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_SHFT                                                        0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_BMSK                                                            0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_SHFT                                                            0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004104)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_BMSK                                          0xfffc0000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_SHFT                                                0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_BMSK                                           0x3c000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_SHFT                                               0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_BMSK                                                   0x3fff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_SHFT                                                      0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004108)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_SHFT                                            0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000410c)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SW_EXPANSION_FUSES_BMSK                                              0xff000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SW_EXPANSION_FUSES_SHFT                                                    0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE0_BMSK                                                            0xf00000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE0_SHFT                                                                0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_BMSK                                               0xf0000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_SHFT                                                  0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION3_BMSK                                                 0xf000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION3_SHFT                                                    0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION2_BMSK                                                  0xf00
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION2_SHFT                                                    0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION1_BMSK                                                   0xf0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION1_SHFT                                                    0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION0_BMSK                                                    0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION0_SHFT                                                    0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004110)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_MDSP_FW_DISABLE_BMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_MDSP_FW_DISABLE_SHFT                                                        0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004114)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_SPARE0_BMSK                                                          0xffff0000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_SPARE0_SHFT                                                                0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RPM_PBL_BOOT_SPEED_BMSK                                                  0xc000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RPM_PBL_BOOT_SPEED_SHFT                                                     0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RPM_BOOT_FROM_ROM_BMSK                                                   0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RPM_BOOT_FROM_ROM_SHFT                                                      0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_APPS_BOOT_FROM_ROM_BMSK                                                  0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_APPS_BOOT_FROM_ROM_SHFT                                                     0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_MSA_ENA_BMSK                                                              0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_MSA_ENA_SHFT                                                                0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_FORCE_MSA_AUTH_EN_BMSK                                                    0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_FORCE_MSA_AUTH_EN_SHFT                                                      0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_FLCB_SKIP_BMSK                                                            0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_FLCB_SKIP_SHFT                                                              0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_QCOM_PMIC_FEATURE_ENABLE_BMSK                                             0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_QCOM_PMIC_FEATURE_ENABLE_SHFT                                               0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_BOOT_ROM_CFG_BMSK                                                          0xe0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_BOOT_ROM_CFG_SHFT                                                           0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_STACKED_MEMORY_ID_BMSK                                                     0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_STACKED_MEMORY_ID_SHFT                                                      0x0

#define HWIO_QFPROM_CORR_MEM_REDUNn_ADDR(n)                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004118 + 0x4 * (n))
#define HWIO_QFPROM_CORR_MEM_REDUNn_RMSK                                                                           0xffffffff
#define HWIO_QFPROM_CORR_MEM_REDUNn_MAXn                                                                                   35
#define HWIO_QFPROM_CORR_MEM_REDUNn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_REDUNn_ADDR(n), HWIO_QFPROM_CORR_MEM_REDUNn_RMSK)
#define HWIO_QFPROM_CORR_MEM_REDUNn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_REDUNn_ADDR(n), mask)
#define HWIO_QFPROM_CORR_MEM_REDUNn_REDUN_DATA_BMSK                                                                0xffffffff
#define HWIO_QFPROM_CORR_MEM_REDUNn_REDUN_DATA_SHFT                                                                       0x0

#define HWIO_QFPROM_CORR_DEBUG_KEY_LSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041a8)
#define HWIO_QFPROM_CORR_DEBUG_KEY_LSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_CORR_DEBUG_KEY_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_DEBUG_KEY_LSB_ADDR, HWIO_QFPROM_CORR_DEBUG_KEY_LSB_RMSK)
#define HWIO_QFPROM_CORR_DEBUG_KEY_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_DEBUG_KEY_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_DEBUG_KEY_LSB_KEY0_BMSK                                                                   0xffffffff
#define HWIO_QFPROM_CORR_DEBUG_KEY_LSB_KEY0_SHFT                                                                          0x0

#define HWIO_QFPROM_CORR_DEBUG_KEY_MSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041ac)
#define HWIO_QFPROM_CORR_DEBUG_KEY_MSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_CORR_DEBUG_KEY_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_DEBUG_KEY_MSB_ADDR, HWIO_QFPROM_CORR_DEBUG_KEY_MSB_RMSK)
#define HWIO_QFPROM_CORR_DEBUG_KEY_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_DEBUG_KEY_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_DEBUG_KEY_MSB_KEY1_BMSK                                                                   0xffffffff
#define HWIO_QFPROM_CORR_DEBUG_KEY_MSB_KEY1_SHFT                                                                          0x0

#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b0)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_RMSK                                                                   0xffffffff
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_MEM_ACCEL_BMSK                                                         0xffffffff
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_MEM_ACCEL_SHFT                                                                0x0

#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b4)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_RMSK                                                                   0xffffffff
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_MEM_ACCEL_BMSK                                                         0xffffffff
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_MEM_ACCEL_SHFT                                                                0x0

#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b8)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_RMSK                                                                       0x3fff
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_MEM_ACCEL_BMSK                                                             0x3fff
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_MEM_ACCEL_SHFT                                                                0x0

#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041bc)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_RMSK                                                                   0xffffffff
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_BMSK                                    0xffffffff
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_SHFT                                           0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041c0)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK                                                                       0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_SPARE_BMSK                                                                 0xffffe000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_SPARE_SHFT                                                                        0xd
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_CSI_PHY_BMSK                                                                   0x1f00
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_CSI_PHY_SHFT                                                                      0x8
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_DSI_PHY_BMSK                                                                     0xf0
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_DSI_PHY_SHFT                                                                      0x4
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_DSIPHY_PLL_BMSK                                                                   0xf
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_DSIPHY_PLL_SHFT                                                                   0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041c4)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK                                                                       0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TEMP_SENSOR3_BMSK                                                          0xff000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TEMP_SENSOR3_SHFT                                                                0x18
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TEMP_SENSOR2_BMSK                                                            0xff0000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TEMP_SENSOR2_SHFT                                                                0x10
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TEMP_SENSOR1_BMSK                                                              0xff00
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TEMP_SENSOR1_SHFT                                                                 0x8
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TEMP_SENSOR0_BMSK                                                                0xff
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TEMP_SENSOR0_SHFT                                                                 0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041c8)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK                                                                       0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_SPARE_BMSK                                                                 0xfff00000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_SPARE_SHFT                                                                       0x14
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS3_LDO_VREF_TRIM_BMSK                                                      0xf8000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS3_LDO_VREF_TRIM_SHFT                                                          0xf
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS2_LDO_VREF_TRIM_BMSK                                                       0x7c00
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS2_LDO_VREF_TRIM_SHFT                                                          0xa
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS1_LDO_VREF_TRIM_BMSK                                                        0x3e0
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS1_LDO_VREF_TRIM_SHFT                                                          0x5
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS0_LDO_VREF_TRIM_BMSK                                                         0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS0_LDO_VREF_TRIM_SHFT                                                          0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041cc)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK                                                                       0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SPARE_BMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SPARE_SHFT                                                                        0x0

#define HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR                                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041d0)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_RMSK                                                                           0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR, HWIO_QFPROM_CORR_FEC_EN_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION31_FEC_EN_BMSK                                                           0x80000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION31_FEC_EN_SHFT                                                                 0x1f
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION30_FEC_EN_BMSK                                                           0x40000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION30_FEC_EN_SHFT                                                                 0x1e
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION29_FEC_EN_BMSK                                                           0x20000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION29_FEC_EN_SHFT                                                                 0x1d
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION28_FEC_EN_BMSK                                                           0x10000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION28_FEC_EN_SHFT                                                                 0x1c
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION27_FEC_EN_BMSK                                                            0x8000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION27_FEC_EN_SHFT                                                                 0x1b
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION26_FEC_EN_BMSK                                                            0x4000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION26_FEC_EN_SHFT                                                                 0x1a
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION25_FEC_EN_BMSK                                                            0x2000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION25_FEC_EN_SHFT                                                                 0x19
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION24_FEC_EN_BMSK                                                            0x1000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION24_FEC_EN_SHFT                                                                 0x18
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION23_FEC_EN_BMSK                                                             0x800000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION23_FEC_EN_SHFT                                                                 0x17
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION22_FEC_EN_BMSK                                                             0x400000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION22_FEC_EN_SHFT                                                                 0x16
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION21_FEC_EN_BMSK                                                             0x200000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION21_FEC_EN_SHFT                                                                 0x15
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION20_FEC_EN_BMSK                                                             0x100000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION20_FEC_EN_SHFT                                                                 0x14
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION19_FEC_EN_BMSK                                                              0x80000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION19_FEC_EN_SHFT                                                                 0x13
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION18_FEC_EN_BMSK                                                              0x40000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION18_FEC_EN_SHFT                                                                 0x12
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION17_FEC_EN_BMSK                                                              0x20000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION17_FEC_EN_SHFT                                                                 0x11
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION16_FEC_EN_BMSK                                                              0x10000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION16_FEC_EN_SHFT                                                                 0x10
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION15_FEC_EN_BMSK                                                               0x8000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION15_FEC_EN_SHFT                                                                  0xf
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION14_FEC_EN_BMSK                                                               0x4000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION14_FEC_EN_SHFT                                                                  0xe
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION13_FEC_EN_BMSK                                                               0x2000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION13_FEC_EN_SHFT                                                                  0xd
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION12_FEC_EN_BMSK                                                               0x1000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION12_FEC_EN_SHFT                                                                  0xc
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION11_FEC_EN_BMSK                                                                0x800
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION11_FEC_EN_SHFT                                                                  0xb
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION10_FEC_EN_BMSK                                                                0x400
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION10_FEC_EN_SHFT                                                                  0xa
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION9_FEC_EN_BMSK                                                                 0x200
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION9_FEC_EN_SHFT                                                                   0x9
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION8_FEC_EN_BMSK                                                                 0x100
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION8_FEC_EN_SHFT                                                                   0x8
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION7_FEC_EN_BMSK                                                                  0x80
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION7_FEC_EN_SHFT                                                                   0x7
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION6_FEC_EN_BMSK                                                                  0x40
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION6_FEC_EN_SHFT                                                                   0x6
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION5_FEC_EN_BMSK                                                                  0x20
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION5_FEC_EN_SHFT                                                                   0x5
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION4_FEC_EN_BMSK                                                                  0x10
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION4_FEC_EN_SHFT                                                                   0x4
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION3_FEC_EN_BMSK                                                                   0x8
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION3_FEC_EN_SHFT                                                                   0x3
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION2_FEC_EN_BMSK                                                                   0x4
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION2_FEC_EN_SHFT                                                                   0x2
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION1_FEC_EN_BMSK                                                                   0x2
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION1_FEC_EN_SHFT                                                                   0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION0_FEC_EN_BMSK                                                                   0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION0_FEC_EN_SHFT                                                                   0x0

#define HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR                                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041d4)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_RMSK                                                                           0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR, HWIO_QFPROM_CORR_FEC_EN_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_QFPROM_CORR_FEC_EN_MSB_BMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_MSB_QFPROM_CORR_FEC_EN_MSB_SHFT                                                           0x0

#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041d8)
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_ADDR, HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_RMSK)
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_BMSK                                0xffffffff
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_SHFT                                       0x0

#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041dc)
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_ADDR, HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_RMSK)
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_BMSK                                0xffffffff
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_SHFT                                       0x0

#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_ADDR(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041e0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_RMSK                                                                    0xffffffff
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_MAXn                                                                             1
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_KEY_DATA_BMSK                                                           0xffffffff
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_KEY_DATA_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_ADDR(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041e4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_RMSK                                                                    0xffffffff
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_MAXn                                                                             1
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_KEY_DATA_BMSK                                                           0xffffffff
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_KEY_DATA_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_SERIAL_NUM_LSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041f0)
#define HWIO_QFPROM_CORR_SERIAL_NUM_LSB_RMSK                                                                       0xffffffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_LSB_ADDR, HWIO_QFPROM_CORR_SERIAL_NUM_LSB_RMSK)
#define HWIO_QFPROM_CORR_SERIAL_NUM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SERIAL_NUM_LSB_SERIAL_NUM_BMSK                                                            0xffffffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_LSB_SERIAL_NUM_SHFT                                                                   0x0

#define HWIO_QFPROM_CORR_SERIAL_NUM_MSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041f4)
#define HWIO_QFPROM_CORR_SERIAL_NUM_MSB_RMSK                                                                           0xffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_MSB_ADDR, HWIO_QFPROM_CORR_SERIAL_NUM_MSB_RMSK)
#define HWIO_QFPROM_CORR_SERIAL_NUM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SERIAL_NUM_MSB_CHIP_ID_BMSK                                                                   0xffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_MSB_CHIP_ID_SHFT                                                                      0x0

#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041f8)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_RMSK                                                                      0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG19_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_SPARE_BMSK                                                                0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_SPARE_SHFT                                                                       0x0

#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041fc)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_RMSK                                                                      0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG19_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_SPARE_BMSK                                                                0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_SPARE_SHFT                                                                       0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n)                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004200 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_RMSK                                                                   0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_MAXn                                                                           47
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_DATA0_BMSK                                                       0xffff0000
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_DATA0_SHFT                                                             0x10
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_ADDR_BMSK                                                            0xffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_ADDR_SHFT                                                               0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n)                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004204 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RMSK                                                                      0x1ffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_MAXn                                                                           47
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_EN_BMSK                                                             0x10000
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_EN_SHFT                                                                0x10
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_DATA1_BMSK                                                           0xffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_DATA1_SHFT                                                              0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004380 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                                               3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004384 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                                        0xffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                                               3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                                              0xffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043a0)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043a4)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_BMSK          0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_SHFT                 0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043a8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                                               3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043ac + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                                        0xffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                                               3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                                              0xffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043c8)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043cc)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_BMSK          0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_SHFT                 0x0

#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_ADDR(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043d0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_MAXn                                                                          3
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_HASH_DATA0_BMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_HASH_DATA0_SHFT                                                             0x0

#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_ADDR(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043d4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_RMSK                                                                   0xffffff
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_MAXn                                                                          3
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_HASH_DATA1_BMSK                                                        0xffffff
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_HASH_DATA1_SHFT                                                             0x0

#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043f0)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_HASH_DATA0_BMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_HASH_DATA0_SHFT                                                             0x0

#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043f4)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_BMSK                                0xffffffff
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_SHFT                                       0x0

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_ADDR(n)                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043f8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_RMSK                                                                0xffffffff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_MAXn                                                                         3
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT4_BMSK                                                      0xff000000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT4_SHFT                                                            0x18
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT3_BMSK                                                        0xff0000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT3_SHFT                                                            0x10
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT2_BMSK                                                          0xff00
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT2_SHFT                                                             0x8
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT1_BMSK                                                            0xff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT1_SHFT                                                             0x0

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_ADDR(n)                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043fc + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_RMSK                                                                  0xffffff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_MAXn                                                                         3
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT7_BMSK                                                        0xff0000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT7_SHFT                                                            0x10
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT6_BMSK                                                          0xff00
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT6_SHFT                                                             0x8
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT5_BMSK                                                            0xff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT5_SHFT                                                             0x0

#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_ADDR(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004418 + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_MAXn                                                                          3
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT4_BMSK                                                       0xff000000
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT4_SHFT                                                             0x18
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT3_BMSK                                                         0xff0000
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT3_SHFT                                                             0x10
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT2_BMSK                                                           0xff00
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT2_SHFT                                                              0x8
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT1_BMSK                                                             0xff
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT1_SHFT                                                              0x0

#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_ADDR(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000441c + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_RMSK                                                                   0xffffff
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_MAXn                                                                          3
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT7_BMSK                                                         0xff0000
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT7_SHFT                                                             0x10
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT6_BMSK                                                           0xff00
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT6_SHFT                                                              0x8
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT5_BMSK                                                             0xff
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT5_SHFT                                                              0x0

#define HWIO_QFPROM_CORR_USB_VID_PID_LSB_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004438)
#define HWIO_QFPROM_CORR_USB_VID_PID_LSB_RMSK                                                                      0xffffffff
#define HWIO_QFPROM_CORR_USB_VID_PID_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_USB_VID_PID_LSB_ADDR, HWIO_QFPROM_CORR_USB_VID_PID_LSB_RMSK)
#define HWIO_QFPROM_CORR_USB_VID_PID_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_USB_VID_PID_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_USB_VID_PID_LSB_PID1_BMSK                                                                 0xffff0000
#define HWIO_QFPROM_CORR_USB_VID_PID_LSB_PID1_SHFT                                                                       0x10
#define HWIO_QFPROM_CORR_USB_VID_PID_LSB_PID0_BMSK                                                                     0xffff
#define HWIO_QFPROM_CORR_USB_VID_PID_LSB_PID0_SHFT                                                                        0x0

#define HWIO_QFPROM_CORR_USB_VID_PID_MSB_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000443c)
#define HWIO_QFPROM_CORR_USB_VID_PID_MSB_RMSK                                                                          0xffff
#define HWIO_QFPROM_CORR_USB_VID_PID_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_USB_VID_PID_MSB_ADDR, HWIO_QFPROM_CORR_USB_VID_PID_MSB_RMSK)
#define HWIO_QFPROM_CORR_USB_VID_PID_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_USB_VID_PID_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_USB_VID_PID_MSB_VID_BMSK                                                                      0xffff
#define HWIO_QFPROM_CORR_USB_VID_PID_MSB_VID_SHFT                                                                         0x0

#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004440)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_SPARE_BMSK                                                           0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_SPARE_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004444)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_SPARE_BMSK                                                           0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_SPARE_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004448)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_SPARE_BMSK                                                           0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_SPARE_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000444c)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_SPARE_BMSK                                                           0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_SPARE_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004450)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_SPARE_BMSK                                                           0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_SPARE_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004454)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_SPARE_BMSK                                                           0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_SPARE_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004458)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_SPARE_BMSK                                                           0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_SPARE_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000445c)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_SPARE_BMSK                                                           0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_SPARE_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004460)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_SPARE_BMSK                                                           0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_SPARE_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004464)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_SPARE_BMSK                                                           0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_SPARE_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004468)
#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_RMSK                                                                         0xffffffff
#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KSV_LSB_ADDR, HWIO_QFPROM_CORR_HDCP_KSV_LSB_RMSK)
#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KSV_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_KSV0_BMSK                                                                    0xffffffff
#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_KSV0_SHFT                                                                           0x0

#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000446c)
#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_RMSK                                                                               0xff
#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KSV_MSB_ADDR, HWIO_QFPROM_CORR_HDCP_KSV_MSB_RMSK)
#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KSV_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_KSV1_BMSK                                                                          0xff
#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_KSV1_SHFT                                                                           0x0

#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_ADDR(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004468 + 0x8 * (n))
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_RMSK                                                                    0xffffffff
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_MAXn                                                                            40
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_KEY_DATA_BMSK                                                           0xffffffff
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_KEY_DATA_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_ADDR(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000446c + 0x8 * (n))
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_RMSK                                                                      0xffffff
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_MAXn                                                                            40
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_KEY_DATA_BMSK                                                             0xffffff
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_KEY_DATA_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_ADDR(n)                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000045b0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_RMSK                                                              0xffffffff
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_MAXn                                                                      36
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_KEY_DATA_BMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_KEY_DATA_SHFT                                                            0x0

#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_ADDR(n)                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000045b4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_RMSK                                                                0xffffff
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_MAXn                                                                      36
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_KEY_DATA_BMSK                                                       0xffffff
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_KEY_DATA_SHFT                                                            0x0

#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_ADDR(n)                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000046d8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_RMSK                                                              0xffffffff
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_MAXn                                                                      36
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_KEY_DATA_BMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_KEY_DATA_SHFT                                                            0x0

#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_ADDR(n)                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000046dc + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_RMSK                                                                0xffffff
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_MAXn                                                                      36
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_KEY_DATA_BMSK                                                       0xffffff
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_KEY_DATA_SHFT                                                            0x0

#define HWIO_FEATURE_CONFIG0_ADDR                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006004)
#define HWIO_FEATURE_CONFIG0_RMSK                                                                                  0xffffffff
#define HWIO_FEATURE_CONFIG0_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG0_ADDR, HWIO_FEATURE_CONFIG0_RMSK)
#define HWIO_FEATURE_CONFIG0_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG0_ADDR, m)
#define HWIO_FEATURE_CONFIG0_HDCP_DISABLE_BMSK                                                                     0x80000000
#define HWIO_FEATURE_CONFIG0_HDCP_DISABLE_SHFT                                                                           0x1f
#define HWIO_FEATURE_CONFIG0_EDP_DISABLE_BMSK                                                                      0x40000000
#define HWIO_FEATURE_CONFIG0_EDP_DISABLE_SHFT                                                                            0x1e
#define HWIO_FEATURE_CONFIG0_DSI_1_DISABLE_BMSK                                                                    0x20000000
#define HWIO_FEATURE_CONFIG0_DSI_1_DISABLE_SHFT                                                                          0x1d
#define HWIO_FEATURE_CONFIG0_ISP_1_DISABLE_BMSK                                                                    0x10000000
#define HWIO_FEATURE_CONFIG0_ISP_1_DISABLE_SHFT                                                                          0x1c
#define HWIO_FEATURE_CONFIG0_VP8_DECODER_DISABLE_BMSK                                                               0x8000000
#define HWIO_FEATURE_CONFIG0_VP8_DECODER_DISABLE_SHFT                                                                    0x1b
#define HWIO_FEATURE_CONFIG0_VP8_ENCODER_DISABLE_BMSK                                                               0x4000000
#define HWIO_FEATURE_CONFIG0_VP8_ENCODER_DISABLE_SHFT                                                                    0x1a
#define HWIO_FEATURE_CONFIG0_CSID_3_DISABLE_BMSK                                                                    0x2000000
#define HWIO_FEATURE_CONFIG0_CSID_3_DISABLE_SHFT                                                                         0x19
#define HWIO_FEATURE_CONFIG0_CSID_2_DISABLE_BMSK                                                                    0x1000000
#define HWIO_FEATURE_CONFIG0_CSID_2_DISABLE_SHFT                                                                         0x18
#define HWIO_FEATURE_CONFIG0_MACROVISION_DISABLE_BMSK                                                                0x800000
#define HWIO_FEATURE_CONFIG0_MACROVISION_DISABLE_SHFT                                                                    0x17
#define HWIO_FEATURE_CONFIG0_MODEM_SPARE_DISABLE_BMSK                                                                0x700000
#define HWIO_FEATURE_CONFIG0_MODEM_SPARE_DISABLE_SHFT                                                                    0x14
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX3_DISABLE_BMSK                                                                 0x80000
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX3_DISABLE_SHFT                                                                    0x13
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX2_DISABLE_BMSK                                                                 0x40000
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX2_DISABLE_SHFT                                                                    0x12
#define HWIO_FEATURE_CONFIG0_MODEM_GLOBAL_DISABLE_BMSK                                                                0x20000
#define HWIO_FEATURE_CONFIG0_MODEM_GLOBAL_DISABLE_SHFT                                                                   0x11
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DC_DISABLE_BMSK                                                               0x10000
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DC_DISABLE_SHFT                                                                  0x10
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_MIMO_DISABLE_BMSK                                                              0x8000
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_MIMO_DISABLE_SHFT                                                                 0xf
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT2_DISABLE_BMSK                                                         0x4000
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT2_DISABLE_SHFT                                                            0xe
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT1_DISABLE_BMSK                                                         0x2000
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT1_DISABLE_SHFT                                                            0xd
#define HWIO_FEATURE_CONFIG0_MODEM_TDSCDMA_DISABLE_BMSK                                                                0x1000
#define HWIO_FEATURE_CONFIG0_MODEM_TDSCDMA_DISABLE_SHFT                                                                   0xc
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DISABLE_BMSK                                                                    0x800
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DISABLE_SHFT                                                                      0xb
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_DISABLE_BMSK                                                                     0x400
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_DISABLE_SHFT                                                                       0xa
#define HWIO_FEATURE_CONFIG0_MODEM_WCDMA_DISABLE_BMSK                                                                   0x200
#define HWIO_FEATURE_CONFIG0_MODEM_WCDMA_DISABLE_SHFT                                                                     0x9
#define HWIO_FEATURE_CONFIG0_MODEM_DO_DISABLE_BMSK                                                                      0x100
#define HWIO_FEATURE_CONFIG0_MODEM_DO_DISABLE_SHFT                                                                        0x8
#define HWIO_FEATURE_CONFIG0_MODEM_1X_DISABLE_BMSK                                                                       0x80
#define HWIO_FEATURE_CONFIG0_MODEM_1X_DISABLE_SHFT                                                                        0x7
#define HWIO_FEATURE_CONFIG0_NAV_DISABLE_BMSK                                                                            0x40
#define HWIO_FEATURE_CONFIG0_NAV_DISABLE_SHFT                                                                             0x6
#define HWIO_FEATURE_CONFIG0_GSM_DISABLE_BMSK                                                                            0x20
#define HWIO_FEATURE_CONFIG0_GSM_DISABLE_SHFT                                                                             0x5
#define HWIO_FEATURE_CONFIG0_SPI_SLAVE_DISABLE_BMSK                                                                      0x10
#define HWIO_FEATURE_CONFIG0_SPI_SLAVE_DISABLE_SHFT                                                                       0x4
#define HWIO_FEATURE_CONFIG0_APPS_ACG_DISABLE_BMSK                                                                        0x8
#define HWIO_FEATURE_CONFIG0_APPS_ACG_DISABLE_SHFT                                                                        0x3
#define HWIO_FEATURE_CONFIG0_BOOT_ROM_PATCH_DISABLE_BMSK                                                                  0x7
#define HWIO_FEATURE_CONFIG0_BOOT_ROM_PATCH_DISABLE_SHFT                                                                  0x0

#define HWIO_FEATURE_CONFIG1_ADDR                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006008)
#define HWIO_FEATURE_CONFIG1_RMSK                                                                                  0xffffffff
#define HWIO_FEATURE_CONFIG1_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, HWIO_FEATURE_CONFIG1_RMSK)
#define HWIO_FEATURE_CONFIG1_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, m)
#define HWIO_FEATURE_CONFIG1_VENDOR_LOCK_BMSK                                                                      0xf0000000
#define HWIO_FEATURE_CONFIG1_VENDOR_LOCK_SHFT                                                                            0x1c
#define HWIO_FEATURE_CONFIG1_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                                                       0x8000000
#define HWIO_FEATURE_CONFIG1_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                                                            0x1b
#define HWIO_FEATURE_CONFIG1_SPARE0_BMSK                                                                            0x7ff8000
#define HWIO_FEATURE_CONFIG1_SPARE0_SHFT                                                                                  0xf
#define HWIO_FEATURE_CONFIG1_MULTIMEDIA_SPARE_DISABLE_BMSK                                                             0x7800
#define HWIO_FEATURE_CONFIG1_MULTIMEDIA_SPARE_DISABLE_SHFT                                                                0xb
#define HWIO_FEATURE_CONFIG1_PCIE_DISABLE_BMSK                                                                          0x400
#define HWIO_FEATURE_CONFIG1_PCIE_DISABLE_SHFT                                                                            0xa
#define HWIO_FEATURE_CONFIG1_SATA_DISABLE_BMSK                                                                          0x200
#define HWIO_FEATURE_CONFIG1_SATA_DISABLE_SHFT                                                                            0x9
#define HWIO_FEATURE_CONFIG1_ACC_DISABLE_BMSK                                                                           0x100
#define HWIO_FEATURE_CONFIG1_ACC_DISABLE_SHFT                                                                             0x8
#define HWIO_FEATURE_CONFIG1_VPE_DISABLE_BMSK                                                                            0x80
#define HWIO_FEATURE_CONFIG1_VPE_DISABLE_SHFT                                                                             0x7
#define HWIO_FEATURE_CONFIG1_VFE_DISABLE_BMSK                                                                            0x40
#define HWIO_FEATURE_CONFIG1_VFE_DISABLE_SHFT                                                                             0x6
#define HWIO_FEATURE_CONFIG1_JPEG_ENCODER_DISABLE_BMSK                                                                   0x20
#define HWIO_FEATURE_CONFIG1_JPEG_ENCODER_DISABLE_SHFT                                                                    0x5
#define HWIO_FEATURE_CONFIG1_JPEG_DECODER_DISABLE_BMSK                                                                   0x10
#define HWIO_FEATURE_CONFIG1_JPEG_DECODER_DISABLE_SHFT                                                                    0x4
#define HWIO_FEATURE_CONFIG1_FIELD_3D_GRP_DISABLE_BMSK                                                                    0x8
#define HWIO_FEATURE_CONFIG1_FIELD_3D_GRP_DISABLE_SHFT                                                                    0x3
#define HWIO_FEATURE_CONFIG1_VIDEO_ENCODER_DISABLE_BMSK                                                                   0x4
#define HWIO_FEATURE_CONFIG1_VIDEO_ENCODER_DISABLE_SHFT                                                                   0x2
#define HWIO_FEATURE_CONFIG1_VIDEO_CODEC_DISABLE_BMSK                                                                     0x2
#define HWIO_FEATURE_CONFIG1_VIDEO_CODEC_DISABLE_SHFT                                                                     0x1
#define HWIO_FEATURE_CONFIG1_HDMI_DISABLE_BMSK                                                                            0x1
#define HWIO_FEATURE_CONFIG1_HDMI_DISABLE_SHFT                                                                            0x0

#define HWIO_FEATURE_CONFIG2_ADDR                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000600c)
#define HWIO_FEATURE_CONFIG2_RMSK                                                                                  0xffffffff
#define HWIO_FEATURE_CONFIG2_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR, HWIO_FEATURE_CONFIG2_RMSK)
#define HWIO_FEATURE_CONFIG2_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR, m)
#define HWIO_FEATURE_CONFIG2_SPARE1_BMSK                                                                           0xffc00000
#define HWIO_FEATURE_CONFIG2_SPARE1_SHFT                                                                                 0x16
#define HWIO_FEATURE_CONFIG2_QC_DAP_DEVICEEN_DISABLE_BMSK                                                            0x200000
#define HWIO_FEATURE_CONFIG2_QC_DAP_DEVICEEN_DISABLE_SHFT                                                                0x15
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPNIDEN_DISABLE_BMSK                                                             0x100000
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPNIDEN_DISABLE_SHFT                                                                 0x14
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPIDEN_DISABLE_BMSK                                                               0x80000
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPIDEN_DISABLE_SHFT                                                                  0x13
#define HWIO_FEATURE_CONFIG2_QC_DAP_NIDEN_DISABLE_BMSK                                                                0x40000
#define HWIO_FEATURE_CONFIG2_QC_DAP_NIDEN_DISABLE_SHFT                                                                   0x12
#define HWIO_FEATURE_CONFIG2_QC_DAP_DBGEN_DISABLE_BMSK                                                                0x20000
#define HWIO_FEATURE_CONFIG2_QC_DAP_DBGEN_DISABLE_SHFT                                                                   0x11
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPNIDEN_DISABLE_BMSK                                                             0x10000
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPNIDEN_DISABLE_SHFT                                                                0x10
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPIDEN_DISABLE_BMSK                                                               0x8000
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPIDEN_DISABLE_SHFT                                                                  0xf
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_BMSK                                                                0x4000
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_SHFT                                                                   0xe
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_BMSK                                                                0x2000
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_SHFT                                                                   0xd
#define HWIO_FEATURE_CONFIG2_QC_SPARE1_DISABLE_BMSK                                                                    0x1000
#define HWIO_FEATURE_CONFIG2_QC_SPARE1_DISABLE_SHFT                                                                       0xc
#define HWIO_FEATURE_CONFIG2_QC_SPARE0_DISABLE_BMSK                                                                     0x800
#define HWIO_FEATURE_CONFIG2_QC_SPARE0_DISABLE_SHFT                                                                       0xb
#define HWIO_FEATURE_CONFIG2_QC_VENUS_0_DBGEN_DISABLE_BMSK                                                              0x400
#define HWIO_FEATURE_CONFIG2_QC_VENUS_0_DBGEN_DISABLE_SHFT                                                                0xa
#define HWIO_FEATURE_CONFIG2_QC_RPM_DAPEN_DISABLE_BMSK                                                                  0x200
#define HWIO_FEATURE_CONFIG2_QC_RPM_DAPEN_DISABLE_SHFT                                                                    0x9
#define HWIO_FEATURE_CONFIG2_QC_RPM_LPASS_WCSS_NIDEN_DISABLE_BMSK                                                       0x100
#define HWIO_FEATURE_CONFIG2_QC_RPM_LPASS_WCSS_NIDEN_DISABLE_SHFT                                                         0x8
#define HWIO_FEATURE_CONFIG2_QC_RPM_DBGEN_DISABLE_BMSK                                                                   0x80
#define HWIO_FEATURE_CONFIG2_QC_RPM_DBGEN_DISABLE_SHFT                                                                    0x7
#define HWIO_FEATURE_CONFIG2_QC_WCSS_DBGEN_DISABLE_BMSK                                                                  0x40
#define HWIO_FEATURE_CONFIG2_QC_WCSS_DBGEN_DISABLE_SHFT                                                                   0x6
#define HWIO_FEATURE_CONFIG2_QC_LPASS_DBGEN_DISABLE_BMSK                                                                 0x20
#define HWIO_FEATURE_CONFIG2_QC_LPASS_DBGEN_DISABLE_SHFT                                                                  0x5
#define HWIO_FEATURE_CONFIG2_QC_MSS_NIDEN_DISABLE_BMSK                                                                   0x10
#define HWIO_FEATURE_CONFIG2_QC_MSS_NIDEN_DISABLE_SHFT                                                                    0x4
#define HWIO_FEATURE_CONFIG2_QC_MSS_DBGEN_DISABLE_BMSK                                                                    0x8
#define HWIO_FEATURE_CONFIG2_QC_MSS_DBGEN_DISABLE_SHFT                                                                    0x3
#define HWIO_FEATURE_CONFIG2_SPARE0_BMSK                                                                                  0x4
#define HWIO_FEATURE_CONFIG2_SPARE0_SHFT                                                                                  0x2
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_BMSK                                                                         0x2
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_SHFT                                                                         0x1
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_BMSK                                                                             0x1
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_SHFT                                                                             0x0

#define HWIO_FEATURE_CONFIG3_ADDR                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006010)
#define HWIO_FEATURE_CONFIG3_RMSK                                                                                  0xffffffff
#define HWIO_FEATURE_CONFIG3_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG3_ADDR, HWIO_FEATURE_CONFIG3_RMSK)
#define HWIO_FEATURE_CONFIG3_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG3_ADDR, m)
#define HWIO_FEATURE_CONFIG3_SEC_TAP_ACCESS_DISABLE_BMSK                                                           0xfffc0000
#define HWIO_FEATURE_CONFIG3_SEC_TAP_ACCESS_DISABLE_SHFT                                                                 0x12
#define HWIO_FEATURE_CONFIG3_TAP_CJI_CORE_SEL_DISABLE_BMSK                                                            0x3c000
#define HWIO_FEATURE_CONFIG3_TAP_CJI_CORE_SEL_DISABLE_SHFT                                                                0xe
#define HWIO_FEATURE_CONFIG3_TAP_INSTR_DISABLE_BMSK                                                                    0x3fff
#define HWIO_FEATURE_CONFIG3_TAP_INSTR_DISABLE_SHFT                                                                       0x0

#define HWIO_FEATURE_CONFIG4_ADDR                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006014)
#define HWIO_FEATURE_CONFIG4_RMSK                                                                                  0xffffffff
#define HWIO_FEATURE_CONFIG4_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG4_ADDR, HWIO_FEATURE_CONFIG4_RMSK)
#define HWIO_FEATURE_CONFIG4_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG4_ADDR, m)
#define HWIO_FEATURE_CONFIG4_TAP_GEN_SPARE_INSTR_DISABLE_BMSK                                                      0xffffffff
#define HWIO_FEATURE_CONFIG4_TAP_GEN_SPARE_INSTR_DISABLE_SHFT                                                             0x0

#define HWIO_FEATURE_CONFIG5_ADDR                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006018)
#define HWIO_FEATURE_CONFIG5_RMSK                                                                                  0xffffffff
#define HWIO_FEATURE_CONFIG5_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG5_ADDR, HWIO_FEATURE_CONFIG5_RMSK)
#define HWIO_FEATURE_CONFIG5_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG5_ADDR, m)
#define HWIO_FEATURE_CONFIG5_SW_EXPANSION_FUSES_BMSK                                                               0xff000000
#define HWIO_FEATURE_CONFIG5_SW_EXPANSION_FUSES_SHFT                                                                     0x18
#define HWIO_FEATURE_CONFIG5_SPARE0_BMSK                                                                             0xf00000
#define HWIO_FEATURE_CONFIG5_SPARE0_SHFT                                                                                 0x14
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_BMSK                                                                0xf0000
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_SHFT                                                                   0x10
#define HWIO_FEATURE_CONFIG5_APPS_CONFIGURATION3_BMSK                                                                  0xf000
#define HWIO_FEATURE_CONFIG5_APPS_CONFIGURATION3_SHFT                                                                     0xc
#define HWIO_FEATURE_CONFIG5_APPS_CONFIGURATION2_BMSK                                                                   0xf00
#define HWIO_FEATURE_CONFIG5_APPS_CONFIGURATION2_SHFT                                                                     0x8
#define HWIO_FEATURE_CONFIG5_APPS_CONFIGURATION1_BMSK                                                                    0xf0
#define HWIO_FEATURE_CONFIG5_APPS_CONFIGURATION1_SHFT                                                                     0x4
#define HWIO_FEATURE_CONFIG5_APPS_CONFIGURATION0_BMSK                                                                     0xf
#define HWIO_FEATURE_CONFIG5_APPS_CONFIGURATION0_SHFT                                                                     0x0

#define HWIO_FEATURE_CONFIG6_ADDR                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000601c)
#define HWIO_FEATURE_CONFIG6_RMSK                                                                                  0xffffffff
#define HWIO_FEATURE_CONFIG6_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG6_ADDR, HWIO_FEATURE_CONFIG6_RMSK)
#define HWIO_FEATURE_CONFIG6_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG6_ADDR, m)
#define HWIO_FEATURE_CONFIG6_MDSP_FW_DISABLE_BMSK                                                                  0xffffffff
#define HWIO_FEATURE_CONFIG6_MDSP_FW_DISABLE_SHFT                                                                         0x0

#define HWIO_FEATURE_CONFIG7_ADDR                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006020)
#define HWIO_FEATURE_CONFIG7_RMSK                                                                                  0xffffffff
#define HWIO_FEATURE_CONFIG7_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG7_ADDR, HWIO_FEATURE_CONFIG7_RMSK)
#define HWIO_FEATURE_CONFIG7_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG7_ADDR, m)
#define HWIO_FEATURE_CONFIG7_SPARE0_BMSK                                                                           0xffff0000
#define HWIO_FEATURE_CONFIG7_SPARE0_SHFT                                                                                 0x10
#define HWIO_FEATURE_CONFIG7_RPM_PBL_BOOT_SPEED_BMSK                                                                   0xc000
#define HWIO_FEATURE_CONFIG7_RPM_PBL_BOOT_SPEED_SHFT                                                                      0xe
#define HWIO_FEATURE_CONFIG7_RPM_BOOT_FROM_ROM_BMSK                                                                    0x2000
#define HWIO_FEATURE_CONFIG7_RPM_BOOT_FROM_ROM_SHFT                                                                       0xd
#define HWIO_FEATURE_CONFIG7_APPS_BOOT_FROM_ROM_BMSK                                                                   0x1000
#define HWIO_FEATURE_CONFIG7_APPS_BOOT_FROM_ROM_SHFT                                                                      0xc
#define HWIO_FEATURE_CONFIG7_MSA_ENA_BMSK                                                                               0x800
#define HWIO_FEATURE_CONFIG7_MSA_ENA_SHFT                                                                                 0xb
#define HWIO_FEATURE_CONFIG7_FORCE_MSA_AUTH_EN_BMSK                                                                     0x400
#define HWIO_FEATURE_CONFIG7_FORCE_MSA_AUTH_EN_SHFT                                                                       0xa
#define HWIO_FEATURE_CONFIG7_FLCB_SKIP_BMSK                                                                             0x200
#define HWIO_FEATURE_CONFIG7_FLCB_SKIP_SHFT                                                                               0x9
#define HWIO_FEATURE_CONFIG7_QCOM_PMIC_FEATURE_ENABLE_BMSK                                                              0x100
#define HWIO_FEATURE_CONFIG7_QCOM_PMIC_FEATURE_ENABLE_SHFT                                                                0x8
#define HWIO_FEATURE_CONFIG7_BOOT_ROM_CFG_BMSK                                                                           0xe0
#define HWIO_FEATURE_CONFIG7_BOOT_ROM_CFG_SHFT                                                                            0x5
#define HWIO_FEATURE_CONFIG7_STACKED_MEMORY_ID_BMSK                                                                      0x1f
#define HWIO_FEATURE_CONFIG7_STACKED_MEMORY_ID_SHFT                                                                       0x0

#define HWIO_OEM_CONFIG0_ADDR                                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006024)
#define HWIO_OEM_CONFIG0_RMSK                                                                                      0xffffffff
#define HWIO_OEM_CONFIG0_IN          \
        in_dword_masked(HWIO_OEM_CONFIG0_ADDR, HWIO_OEM_CONFIG0_RMSK)
#define HWIO_OEM_CONFIG0_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG0_ADDR, m)
#define HWIO_OEM_CONFIG0_SPARE1_DISABLE_BMSK                                                                       0x80000000
#define HWIO_OEM_CONFIG0_SPARE1_DISABLE_SHFT                                                                             0x1f
#define HWIO_OEM_CONFIG0_SPARE0_DISABLE_BMSK                                                                       0x40000000
#define HWIO_OEM_CONFIG0_SPARE0_DISABLE_SHFT                                                                             0x1e
#define HWIO_OEM_CONFIG0_VENUS_0_DBGEN_DISABLE_BMSK                                                                0x20000000
#define HWIO_OEM_CONFIG0_VENUS_0_DBGEN_DISABLE_SHFT                                                                      0x1d
#define HWIO_OEM_CONFIG0_RPM_DAPEN_DISABLE_BMSK                                                                    0x10000000
#define HWIO_OEM_CONFIG0_RPM_DAPEN_DISABLE_SHFT                                                                          0x1c
#define HWIO_OEM_CONFIG0_RPM_LPASS_WCSS_NIDEN_DISABLE_BMSK                                                          0x8000000
#define HWIO_OEM_CONFIG0_RPM_LPASS_WCSS_NIDEN_DISABLE_SHFT                                                               0x1b
#define HWIO_OEM_CONFIG0_RPM_DBGEN_DISABLE_BMSK                                                                     0x4000000
#define HWIO_OEM_CONFIG0_RPM_DBGEN_DISABLE_SHFT                                                                          0x1a
#define HWIO_OEM_CONFIG0_WCSS_DBGEN_DISABLE_BMSK                                                                    0x2000000
#define HWIO_OEM_CONFIG0_WCSS_DBGEN_DISABLE_SHFT                                                                         0x19
#define HWIO_OEM_CONFIG0_LPASS_DBGEN_DISABLE_BMSK                                                                   0x1000000
#define HWIO_OEM_CONFIG0_LPASS_DBGEN_DISABLE_SHFT                                                                        0x18
#define HWIO_OEM_CONFIG0_MSS_NIDEN_DISABLE_BMSK                                                                      0x800000
#define HWIO_OEM_CONFIG0_MSS_NIDEN_DISABLE_SHFT                                                                          0x17
#define HWIO_OEM_CONFIG0_MSS_DBGEN_DISABLE_BMSK                                                                      0x400000
#define HWIO_OEM_CONFIG0_MSS_DBGEN_DISABLE_SHFT                                                                          0x16
#define HWIO_OEM_CONFIG0_ALL_DEBUG_DISABLE_BMSK                                                                      0x200000
#define HWIO_OEM_CONFIG0_ALL_DEBUG_DISABLE_SHFT                                                                          0x15
#define HWIO_OEM_CONFIG0_SPARE1_BMSK                                                                                 0x1e0000
#define HWIO_OEM_CONFIG0_SPARE1_SHFT                                                                                     0x11
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_GPIO_DISABLE_BMSK                                                           0x10000
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_GPIO_DISABLE_SHFT                                                              0x10
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_EN_BMSK                                                                      0x8000
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_EN_SHFT                                                                         0xf
#define HWIO_OEM_CONFIG0_FAST_BOOT_BMSK                                                                                0x7c00
#define HWIO_OEM_CONFIG0_FAST_BOOT_SHFT                                                                                   0xa
#define HWIO_OEM_CONFIG0_SDP_ENUM_SKIP_BMSK                                                                             0x200
#define HWIO_OEM_CONFIG0_SDP_ENUM_SKIP_SHFT                                                                               0x9
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_BMSK                                                                       0x100
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_SHFT                                                                         0x8
#define HWIO_OEM_CONFIG0_APPS_PBL_BOOT_SPEED_BMSK                                                                        0xc0
#define HWIO_OEM_CONFIG0_APPS_PBL_BOOT_SPEED_SHFT                                                                         0x6
#define HWIO_OEM_CONFIG0_FORCE_USB_BOOT_DISABLE_BMSK                                                                     0x20
#define HWIO_OEM_CONFIG0_FORCE_USB_BOOT_DISABLE_SHFT                                                                      0x5
#define HWIO_OEM_CONFIG0_SPARE0_BMSK                                                                                     0x10
#define HWIO_OEM_CONFIG0_SPARE0_SHFT                                                                                      0x4
#define HWIO_OEM_CONFIG0_PBL_PMIC_POWER_DOWN_MSM_BMSK                                                                     0x8
#define HWIO_OEM_CONFIG0_PBL_PMIC_POWER_DOWN_MSM_SHFT                                                                     0x3
#define HWIO_OEM_CONFIG0_USB_ENUM_TIMEOUT_BMSK                                                                            0x4
#define HWIO_OEM_CONFIG0_USB_ENUM_TIMEOUT_SHFT                                                                            0x2
#define HWIO_OEM_CONFIG0_DLOAD_TIMEOUT_BMSK                                                                               0x2
#define HWIO_OEM_CONFIG0_DLOAD_TIMEOUT_SHFT                                                                               0x1
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_BMSK                                                                             0x1
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_SHFT                                                                             0x0

#define HWIO_OEM_CONFIG1_ADDR                                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006028)
#define HWIO_OEM_CONFIG1_RMSK                                                                                      0xffffffff
#define HWIO_OEM_CONFIG1_IN          \
        in_dword_masked(HWIO_OEM_CONFIG1_ADDR, HWIO_OEM_CONFIG1_RMSK)
#define HWIO_OEM_CONFIG1_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG1_ADDR, m)
#define HWIO_OEM_CONFIG1_WDOG_EN_BMSK                                                                              0x80000000
#define HWIO_OEM_CONFIG1_WDOG_EN_SHFT                                                                                    0x1f
#define HWIO_OEM_CONFIG1_SPARE2_BMSK                                                                               0x78000000
#define HWIO_OEM_CONFIG1_SPARE2_SHFT                                                                                     0x1b
#define HWIO_OEM_CONFIG1_SPDM_SECURE_MODE_BMSK                                                                      0x4000000
#define HWIO_OEM_CONFIG1_SPDM_SECURE_MODE_SHFT                                                                           0x1a
#define HWIO_OEM_CONFIG1_IMEI_ESN_SELECT_BMSK                                                                       0x3000000
#define HWIO_OEM_CONFIG1_IMEI_ESN_SELECT_SHFT                                                                            0x18
#define HWIO_OEM_CONFIG1_SPARE1_BMSK                                                                                 0xc00000
#define HWIO_OEM_CONFIG1_SPARE1_SHFT                                                                                     0x16
#define HWIO_OEM_CONFIG1_CUST_PRIV_KEY_SELECT_BMSK                                                                   0x200000
#define HWIO_OEM_CONFIG1_CUST_PRIV_KEY_SELECT_SHFT                                                                       0x15
#define HWIO_OEM_CONFIG1_SW_FUSE_PROG_DISABLE_BMSK                                                                   0x100000
#define HWIO_OEM_CONFIG1_SW_FUSE_PROG_DISABLE_SHFT                                                                       0x14
#define HWIO_OEM_CONFIG1_SPARE0_BMSK                                                                                  0xffe00
#define HWIO_OEM_CONFIG1_SPARE0_SHFT                                                                                      0x9
#define HWIO_OEM_CONFIG1_DAP_DEVICEEN_DISABLE_BMSK                                                                      0x100
#define HWIO_OEM_CONFIG1_DAP_DEVICEEN_DISABLE_SHFT                                                                        0x8
#define HWIO_OEM_CONFIG1_DAP_SPNIDEN_DISABLE_BMSK                                                                        0x80
#define HWIO_OEM_CONFIG1_DAP_SPNIDEN_DISABLE_SHFT                                                                         0x7
#define HWIO_OEM_CONFIG1_DAP_SPIDEN_DISABLE_BMSK                                                                         0x40
#define HWIO_OEM_CONFIG1_DAP_SPIDEN_DISABLE_SHFT                                                                          0x6
#define HWIO_OEM_CONFIG1_DAP_NIDEN_DISABLE_BMSK                                                                          0x20
#define HWIO_OEM_CONFIG1_DAP_NIDEN_DISABLE_SHFT                                                                           0x5
#define HWIO_OEM_CONFIG1_DAP_DBGEN_DISABLE_BMSK                                                                          0x10
#define HWIO_OEM_CONFIG1_DAP_DBGEN_DISABLE_SHFT                                                                           0x4
#define HWIO_OEM_CONFIG1_APPS_SPNIDEN_DISABLE_BMSK                                                                        0x8
#define HWIO_OEM_CONFIG1_APPS_SPNIDEN_DISABLE_SHFT                                                                        0x3
#define HWIO_OEM_CONFIG1_APPS_SPIDEN_DISABLE_BMSK                                                                         0x4
#define HWIO_OEM_CONFIG1_APPS_SPIDEN_DISABLE_SHFT                                                                         0x2
#define HWIO_OEM_CONFIG1_APPS_NIDEN_DISABLE_BMSK                                                                          0x2
#define HWIO_OEM_CONFIG1_APPS_NIDEN_DISABLE_SHFT                                                                          0x1
#define HWIO_OEM_CONFIG1_APPS_DBGEN_DISABLE_BMSK                                                                          0x1
#define HWIO_OEM_CONFIG1_APPS_DBGEN_DISABLE_SHFT                                                                          0x0

#define HWIO_OEM_CONFIG2_ADDR                                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000602c)
#define HWIO_OEM_CONFIG2_RMSK                                                                                      0xffffffff
#define HWIO_OEM_CONFIG2_IN          \
        in_dword_masked(HWIO_OEM_CONFIG2_ADDR, HWIO_OEM_CONFIG2_RMSK)
#define HWIO_OEM_CONFIG2_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG2_ADDR, m)
#define HWIO_OEM_CONFIG2_OEM_HW_ID_BMSK                                                                            0xffff0000
#define HWIO_OEM_CONFIG2_OEM_HW_ID_SHFT                                                                                  0x10
#define HWIO_OEM_CONFIG2_SPARE0_BMSK                                                                                   0xffff
#define HWIO_OEM_CONFIG2_SPARE0_SHFT                                                                                      0x0

#define HWIO_OEM_CONFIG3_ADDR                                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006030)
#define HWIO_OEM_CONFIG3_RMSK                                                                                      0xffffffff
#define HWIO_OEM_CONFIG3_IN          \
        in_dword_masked(HWIO_OEM_CONFIG3_ADDR, HWIO_OEM_CONFIG3_RMSK)
#define HWIO_OEM_CONFIG3_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG3_ADDR, m)
#define HWIO_OEM_CONFIG3_SPARE0_BMSK                                                                               0xffff0000
#define HWIO_OEM_CONFIG3_SPARE0_SHFT                                                                                     0x10
#define HWIO_OEM_CONFIG3_OEM_PRODUCT_ID_BMSK                                                                           0xffff
#define HWIO_OEM_CONFIG3_OEM_PRODUCT_ID_SHFT                                                                              0x0

#define HWIO_BOOT_CONFIG_ADDR                                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006034)
#define HWIO_BOOT_CONFIG_RMSK                                                                                           0x7ff
#define HWIO_BOOT_CONFIG_IN          \
        in_dword_masked(HWIO_BOOT_CONFIG_ADDR, HWIO_BOOT_CONFIG_RMSK)
#define HWIO_BOOT_CONFIG_INM(m)      \
        in_dword_masked(HWIO_BOOT_CONFIG_ADDR, m)
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_BMSK                                                                         0x400
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_SHFT                                                                           0xa
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_BMSK                                                                       0x300
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_SHFT                                                                         0x8
#define HWIO_BOOT_CONFIG_RPM_BOOT_FROM_ROM_BMSK                                                                          0x80
#define HWIO_BOOT_CONFIG_RPM_BOOT_FROM_ROM_SHFT                                                                           0x7
#define HWIO_BOOT_CONFIG_APPS_BOOT_FROM_ROM_BMSK                                                                         0x40
#define HWIO_BOOT_CONFIG_APPS_BOOT_FROM_ROM_SHFT                                                                          0x6
#define HWIO_BOOT_CONFIG_FAST_BOOT_BMSK                                                                                  0x3e
#define HWIO_BOOT_CONFIG_FAST_BOOT_SHFT                                                                                   0x1
#define HWIO_BOOT_CONFIG_WDOG_EN_BMSK                                                                                     0x1
#define HWIO_BOOT_CONFIG_WDOG_EN_SHFT                                                                                     0x0

#define HWIO_SECURE_BOOTn_ADDR(n)                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006038 + 0x4 * (n))
#define HWIO_SECURE_BOOTn_RMSK                                                                                          0x17f
#define HWIO_SECURE_BOOTn_MAXn                                                                                             28
#define HWIO_SECURE_BOOTn_INI(n)        \
        in_dword_masked(HWIO_SECURE_BOOTn_ADDR(n), HWIO_SECURE_BOOTn_RMSK)
#define HWIO_SECURE_BOOTn_INMI(n,mask)    \
        in_dword_masked(HWIO_SECURE_BOOTn_ADDR(n), mask)
#define HWIO_SECURE_BOOTn_FUSE_SRC_BMSK                                                                                 0x100
#define HWIO_SECURE_BOOTn_FUSE_SRC_SHFT                                                                                   0x8
#define HWIO_SECURE_BOOTn_USE_SERIAL_NUM_BMSK                                                                            0x40
#define HWIO_SECURE_BOOTn_USE_SERIAL_NUM_SHFT                                                                             0x6
#define HWIO_SECURE_BOOTn_AUTH_EN_BMSK                                                                                   0x20
#define HWIO_SECURE_BOOTn_AUTH_EN_SHFT                                                                                    0x5
#define HWIO_SECURE_BOOTn_PK_HASH_IN_FUSE_BMSK                                                                           0x10
#define HWIO_SECURE_BOOTn_PK_HASH_IN_FUSE_SHFT                                                                            0x4
#define HWIO_SECURE_BOOTn_ROM_PK_HASH_INDEX_BMSK                                                                          0xf
#define HWIO_SECURE_BOOTn_ROM_PK_HASH_INDEX_SHFT                                                                          0x0

#define HWIO_OVERRIDE_0_ADDR                                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060b0)
#define HWIO_OVERRIDE_0_RMSK                                                                                       0x10000003
#define HWIO_OVERRIDE_0_IN          \
        in_dword_masked(HWIO_OVERRIDE_0_ADDR, HWIO_OVERRIDE_0_RMSK)
#define HWIO_OVERRIDE_0_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_0_ADDR, m)
#define HWIO_OVERRIDE_0_OUT(v)      \
        out_dword(HWIO_OVERRIDE_0_ADDR,v)
#define HWIO_OVERRIDE_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_0_ADDR,m,v,HWIO_OVERRIDE_0_IN)
#define HWIO_OVERRIDE_0_TX_DISABLE_BMSK                                                                            0x10000000
#define HWIO_OVERRIDE_0_TX_DISABLE_SHFT                                                                                  0x1c
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_EN_BMSK                                                                          0x2
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_EN_SHFT                                                                          0x1
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_OVERRIDE_BMSK                                                                    0x1
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_OVERRIDE_SHFT                                                                    0x0

#define HWIO_OVERRIDE_1_ADDR                                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060b4)
#define HWIO_OVERRIDE_1_RMSK                                                                                       0xffffffff
#define HWIO_OVERRIDE_1_IN          \
        in_dword_masked(HWIO_OVERRIDE_1_ADDR, HWIO_OVERRIDE_1_RMSK)
#define HWIO_OVERRIDE_1_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_1_ADDR, m)
#define HWIO_OVERRIDE_1_OUT(v)      \
        out_dword(HWIO_OVERRIDE_1_ADDR,v)
#define HWIO_OVERRIDE_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_1_ADDR,m,v,HWIO_OVERRIDE_1_IN)
#define HWIO_OVERRIDE_1_OVERRIDE_1_BMSK                                                                            0xffffffff
#define HWIO_OVERRIDE_1_OVERRIDE_1_SHFT                                                                                   0x0

#define HWIO_OVERRIDE_2_ADDR                                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060b8)
#define HWIO_OVERRIDE_2_RMSK                                                                                         0x7ffc00
#define HWIO_OVERRIDE_2_IN          \
        in_dword_masked(HWIO_OVERRIDE_2_ADDR, HWIO_OVERRIDE_2_RMSK)
#define HWIO_OVERRIDE_2_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_2_ADDR, m)
#define HWIO_OVERRIDE_2_OUT(v)      \
        out_dword(HWIO_OVERRIDE_2_ADDR,v)
#define HWIO_OVERRIDE_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_2_ADDR,m,v,HWIO_OVERRIDE_2_IN)
#define HWIO_OVERRIDE_2_OVRID_DAP_DEVICEEN_DISABLE_BMSK                                                              0x400000
#define HWIO_OVERRIDE_2_OVRID_DAP_DEVICEEN_DISABLE_SHFT                                                                  0x16
#define HWIO_OVERRIDE_2_OVRID_DAP_NIDEN_DISABLE_BMSK                                                                 0x200000
#define HWIO_OVERRIDE_2_OVRID_DAP_NIDEN_DISABLE_SHFT                                                                     0x15
#define HWIO_OVERRIDE_2_OVRID_DAP_DBGEN_DISABLE_BMSK                                                                 0x100000
#define HWIO_OVERRIDE_2_OVRID_DAP_DBGEN_DISABLE_SHFT                                                                     0x14
#define HWIO_OVERRIDE_2_OVRID_APPS_NIDEN_DISABLE_BMSK                                                                 0x80000
#define HWIO_OVERRIDE_2_OVRID_APPS_NIDEN_DISABLE_SHFT                                                                    0x13
#define HWIO_OVERRIDE_2_OVRID_APPS_DBGEN_DISABLE_BMSK                                                                 0x40000
#define HWIO_OVERRIDE_2_OVRID_APPS_DBGEN_DISABLE_SHFT                                                                    0x12
#define HWIO_OVERRIDE_2_OVRID_SPARE1_DISABLE_BMSK                                                                     0x20000
#define HWIO_OVERRIDE_2_OVRID_SPARE1_DISABLE_SHFT                                                                        0x11
#define HWIO_OVERRIDE_2_OVRID_SPARE0_DISABLE_BMSK                                                                     0x10000
#define HWIO_OVERRIDE_2_OVRID_SPARE0_DISABLE_SHFT                                                                        0x10
#define HWIO_OVERRIDE_2_OVRID_VENUS_0_DBGEN_DISABLE_BMSK                                                               0x8000
#define HWIO_OVERRIDE_2_OVRID_VENUS_0_DBGEN_DISABLE_SHFT                                                                  0xf
#define HWIO_OVERRIDE_2_OVRID_RPM_DAPEN_DISABLE_BMSK                                                                   0x4000
#define HWIO_OVERRIDE_2_OVRID_RPM_DAPEN_DISABLE_SHFT                                                                      0xe
#define HWIO_OVERRIDE_2_OVRID_RPM_LPASS_WCSS_NIDEN_DISABLE_BMSK                                                        0x2000
#define HWIO_OVERRIDE_2_OVRID_RPM_LPASS_WCSS_NIDEN_DISABLE_SHFT                                                           0xd
#define HWIO_OVERRIDE_2_OVRID_RPM_DBGEN_DISABLE_BMSK                                                                   0x1000
#define HWIO_OVERRIDE_2_OVRID_RPM_DBGEN_DISABLE_SHFT                                                                      0xc
#define HWIO_OVERRIDE_2_OVRID_WCSS_DBGEN_DISABLE_BMSK                                                                   0x800
#define HWIO_OVERRIDE_2_OVRID_WCSS_DBGEN_DISABLE_SHFT                                                                     0xb
#define HWIO_OVERRIDE_2_OVRID_LPASS_DBGEN_DISABLE_BMSK                                                                  0x400
#define HWIO_OVERRIDE_2_OVRID_LPASS_DBGEN_DISABLE_SHFT                                                                    0xa

#define HWIO_OVERRIDE_3_ADDR                                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060bc)
#define HWIO_OVERRIDE_3_RMSK                                                                                             0x1f
#define HWIO_OVERRIDE_3_IN          \
        in_dword_masked(HWIO_OVERRIDE_3_ADDR, HWIO_OVERRIDE_3_RMSK)
#define HWIO_OVERRIDE_3_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_3_ADDR, m)
#define HWIO_OVERRIDE_3_OUT(v)      \
        out_dword(HWIO_OVERRIDE_3_ADDR,v)
#define HWIO_OVERRIDE_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_3_ADDR,m,v,HWIO_OVERRIDE_3_IN)
#define HWIO_OVERRIDE_3_OVRID_DAP_SPNIDEN_DISABLE_BMSK                                                                   0x10
#define HWIO_OVERRIDE_3_OVRID_DAP_SPNIDEN_DISABLE_SHFT                                                                    0x4
#define HWIO_OVERRIDE_3_OVRID_DAP_SPIDEN_DISABLE_BMSK                                                                     0x8
#define HWIO_OVERRIDE_3_OVRID_DAP_SPIDEN_DISABLE_SHFT                                                                     0x3
#define HWIO_OVERRIDE_3_OVRID_APPS_SPNIDEN_DISABLE_BMSK                                                                   0x4
#define HWIO_OVERRIDE_3_OVRID_APPS_SPNIDEN_DISABLE_SHFT                                                                   0x2
#define HWIO_OVERRIDE_3_OVRID_APPS_SPIDEN_DISABLE_BMSK                                                                    0x2
#define HWIO_OVERRIDE_3_OVRID_APPS_SPIDEN_DISABLE_SHFT                                                                    0x1
#define HWIO_OVERRIDE_3_OVRID_SPDM_SECURE_MODE_BMSK                                                                       0x1
#define HWIO_OVERRIDE_3_OVRID_SPDM_SECURE_MODE_SHFT                                                                       0x0

#define HWIO_OVERRIDE_4_ADDR                                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060c0)
#define HWIO_OVERRIDE_4_RMSK                                                                                              0x3
#define HWIO_OVERRIDE_4_IN          \
        in_dword_masked(HWIO_OVERRIDE_4_ADDR, HWIO_OVERRIDE_4_RMSK)
#define HWIO_OVERRIDE_4_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_4_ADDR, m)
#define HWIO_OVERRIDE_4_OUT(v)      \
        out_dword(HWIO_OVERRIDE_4_ADDR,v)
#define HWIO_OVERRIDE_4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_4_ADDR,m,v,HWIO_OVERRIDE_4_IN)
#define HWIO_OVERRIDE_4_OVRID_MSS_NIDEN_DISABLE_BMSK                                                                      0x2
#define HWIO_OVERRIDE_4_OVRID_MSS_NIDEN_DISABLE_SHFT                                                                      0x1
#define HWIO_OVERRIDE_4_OVRID_MSS_DBGEN_DISABLE_BMSK                                                                      0x1
#define HWIO_OVERRIDE_4_OVRID_MSS_DBGEN_DISABLE_SHFT                                                                      0x0

#define HWIO_CAPT_SEC_GPIO_ADDR                                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060c4)
#define HWIO_CAPT_SEC_GPIO_RMSK                                                                                       0x3ffff
#define HWIO_CAPT_SEC_GPIO_IN          \
        in_dword_masked(HWIO_CAPT_SEC_GPIO_ADDR, HWIO_CAPT_SEC_GPIO_RMSK)
#define HWIO_CAPT_SEC_GPIO_INM(m)      \
        in_dword_masked(HWIO_CAPT_SEC_GPIO_ADDR, m)
#define HWIO_CAPT_SEC_GPIO_OUT(v)      \
        out_dword(HWIO_CAPT_SEC_GPIO_ADDR,v)
#define HWIO_CAPT_SEC_GPIO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CAPT_SEC_GPIO_ADDR,m,v,HWIO_CAPT_SEC_GPIO_IN)
#define HWIO_CAPT_SEC_GPIO_SDC_EMMC_MODE1P2_EN_BMSK                                                                   0x20000
#define HWIO_CAPT_SEC_GPIO_SDC_EMMC_MODE1P2_EN_SHFT                                                                      0x11
#define HWIO_CAPT_SEC_GPIO_FORCE_USB_BOOT_GPIO_BMSK                                                                   0x10000
#define HWIO_CAPT_SEC_GPIO_FORCE_USB_BOOT_GPIO_SHFT                                                                      0x10
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_BMSK                                                                       0xffff
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_SHFT                                                                          0x0

#define HWIO_APP_PROC_CFG_ADDR                                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060c8)
#define HWIO_APP_PROC_CFG_RMSK                                                                                      0x1ffffff
#define HWIO_APP_PROC_CFG_IN          \
        in_dword_masked(HWIO_APP_PROC_CFG_ADDR, HWIO_APP_PROC_CFG_RMSK)
#define HWIO_APP_PROC_CFG_INM(m)      \
        in_dword_masked(HWIO_APP_PROC_CFG_ADDR, m)
#define HWIO_APP_PROC_CFG_OUT(v)      \
        out_dword(HWIO_APP_PROC_CFG_ADDR,v)
#define HWIO_APP_PROC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APP_PROC_CFG_ADDR,m,v,HWIO_APP_PROC_CFG_IN)
#define HWIO_APP_PROC_CFG_WCSS_DBG_NIDEN_BMSK                                                                       0x1000000
#define HWIO_APP_PROC_CFG_WCSS_DBG_NIDEN_SHFT                                                                            0x18
#define HWIO_APP_PROC_CFG_LPASS_DBG_NIDEN_BMSK                                                                       0x800000
#define HWIO_APP_PROC_CFG_LPASS_DBG_NIDEN_SHFT                                                                           0x17
#define HWIO_APP_PROC_CFG_RPM_DBG_NIDEN_BMSK                                                                         0x400000
#define HWIO_APP_PROC_CFG_RPM_DBG_NIDEN_SHFT                                                                             0x16
#define HWIO_APP_PROC_CFG_DAP_DBG_NIDEN_BMSK                                                                         0x200000
#define HWIO_APP_PROC_CFG_DAP_DBG_NIDEN_SHFT                                                                             0x15
#define HWIO_APP_PROC_CFG_DAP_DBG_SPNIDEN_BMSK                                                                       0x100000
#define HWIO_APP_PROC_CFG_DAP_DBG_SPNIDEN_SHFT                                                                           0x14
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC3_BMSK                                                                         0xf0000
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC3_SHFT                                                                            0x10
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC2_BMSK                                                                          0xf000
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC2_SHFT                                                                             0xc
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC1_BMSK                                                                           0xf00
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC1_SHFT                                                                             0x8
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC0_BMSK                                                                            0xf0
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC0_SHFT                                                                             0x4
#define HWIO_APP_PROC_CFG_APPS_DBG_NIDEN_BMSK                                                                             0x8
#define HWIO_APP_PROC_CFG_APPS_DBG_NIDEN_SHFT                                                                             0x3
#define HWIO_APP_PROC_CFG_APPS_DBG_SPNIDEN_BMSK                                                                           0x4
#define HWIO_APP_PROC_CFG_APPS_DBG_SPNIDEN_SHFT                                                                           0x2
#define HWIO_APP_PROC_CFG_APPS_CP15_DISABLE_BMSK                                                                          0x2
#define HWIO_APP_PROC_CFG_APPS_CP15_DISABLE_SHFT                                                                          0x1
#define HWIO_APP_PROC_CFG_APPS_CFG_NMFI_BMSK                                                                              0x1
#define HWIO_APP_PROC_CFG_APPS_CFG_NMFI_SHFT                                                                              0x0

#define HWIO_MSS_PROC_CFG_ADDR                                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060cc)
#define HWIO_MSS_PROC_CFG_RMSK                                                                                            0x1
#define HWIO_MSS_PROC_CFG_IN          \
        in_dword_masked(HWIO_MSS_PROC_CFG_ADDR, HWIO_MSS_PROC_CFG_RMSK)
#define HWIO_MSS_PROC_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_PROC_CFG_ADDR, m)
#define HWIO_MSS_PROC_CFG_OUT(v)      \
        out_dword(HWIO_MSS_PROC_CFG_ADDR,v)
#define HWIO_MSS_PROC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_PROC_CFG_ADDR,m,v,HWIO_MSS_PROC_CFG_IN)
#define HWIO_MSS_PROC_CFG_MSS_DBG_NIDEN_BMSK                                                                              0x1
#define HWIO_MSS_PROC_CFG_MSS_DBG_NIDEN_SHFT                                                                              0x0

#define HWIO_KDF_CFG_ADDR                                                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060d0)
#define HWIO_KDF_CFG_RMSK                                                                                                 0x1
#define HWIO_KDF_CFG_IN          \
        in_dword_masked(HWIO_KDF_CFG_ADDR, HWIO_KDF_CFG_RMSK)
#define HWIO_KDF_CFG_INM(m)      \
        in_dword_masked(HWIO_KDF_CFG_ADDR, m)
#define HWIO_KDF_CFG_OUT(v)      \
        out_dword(HWIO_KDF_CFG_ADDR,v)
#define HWIO_KDF_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_KDF_CFG_ADDR,m,v,HWIO_KDF_CFG_IN)
#define HWIO_KDF_CFG_CE1_CE2_SAME_HW_KEY_BMSK                                                                             0x1
#define HWIO_KDF_CFG_CE1_CE2_SAME_HW_KEY_SHFT                                                                             0x0

#define HWIO_QFPROM_CLK_CTL_ADDR                                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060d4)
#define HWIO_QFPROM_CLK_CTL_RMSK                                                                                          0x1
#define HWIO_QFPROM_CLK_CTL_IN          \
        in_dword_masked(HWIO_QFPROM_CLK_CTL_ADDR, HWIO_QFPROM_CLK_CTL_RMSK)
#define HWIO_QFPROM_CLK_CTL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CLK_CTL_ADDR, m)
#define HWIO_QFPROM_CLK_CTL_OUT(v)      \
        out_dword(HWIO_QFPROM_CLK_CTL_ADDR,v)
#define HWIO_QFPROM_CLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CLK_CTL_ADDR,m,v,HWIO_QFPROM_CLK_CTL_IN)
#define HWIO_QFPROM_CLK_CTL_CLK_HALT_BMSK                                                                                 0x1
#define HWIO_QFPROM_CLK_CTL_CLK_HALT_SHFT                                                                                 0x0

#define HWIO_HDCP_KSV_LSB_ADDR                                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060d8)
#define HWIO_HDCP_KSV_LSB_RMSK                                                                                     0xffffffff
#define HWIO_HDCP_KSV_LSB_IN          \
        in_dword_masked(HWIO_HDCP_KSV_LSB_ADDR, HWIO_HDCP_KSV_LSB_RMSK)
#define HWIO_HDCP_KSV_LSB_INM(m)      \
        in_dword_masked(HWIO_HDCP_KSV_LSB_ADDR, m)
#define HWIO_HDCP_KSV_LSB_KSV_LSB_BMSK                                                                             0xffffffff
#define HWIO_HDCP_KSV_LSB_KSV_LSB_SHFT                                                                                    0x0

#define HWIO_HDCP_KSV_MSB_ADDR                                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060dc)
#define HWIO_HDCP_KSV_MSB_RMSK                                                                                           0xff
#define HWIO_HDCP_KSV_MSB_IN          \
        in_dword_masked(HWIO_HDCP_KSV_MSB_ADDR, HWIO_HDCP_KSV_MSB_RMSK)
#define HWIO_HDCP_KSV_MSB_INM(m)      \
        in_dword_masked(HWIO_HDCP_KSV_MSB_ADDR, m)
#define HWIO_HDCP_KSV_MSB_KSV_MSB_BMSK                                                                                   0xff
#define HWIO_HDCP_KSV_MSB_KSV_MSB_SHFT                                                                                    0x0

#define HWIO_JTAG_ID_ADDR                                                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060e0)
#define HWIO_JTAG_ID_RMSK                                                                                          0xffffffff
#define HWIO_JTAG_ID_IN          \
        in_dword_masked(HWIO_JTAG_ID_ADDR, HWIO_JTAG_ID_RMSK)
#define HWIO_JTAG_ID_INM(m)      \
        in_dword_masked(HWIO_JTAG_ID_ADDR, m)
#define HWIO_JTAG_ID_JTAG_ID_BMSK                                                                                  0xffffffff
#define HWIO_JTAG_ID_JTAG_ID_SHFT                                                                                         0x0

#define HWIO_OEM_ID_ADDR                                                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060e4)
#define HWIO_OEM_ID_RMSK                                                                                           0xffffffff
#define HWIO_OEM_ID_IN          \
        in_dword_masked(HWIO_OEM_ID_ADDR, HWIO_OEM_ID_RMSK)
#define HWIO_OEM_ID_INM(m)      \
        in_dword_masked(HWIO_OEM_ID_ADDR, m)
#define HWIO_OEM_ID_OEM_ID_BMSK                                                                                    0xffff0000
#define HWIO_OEM_ID_OEM_ID_SHFT                                                                                          0x10
#define HWIO_OEM_ID_OEM_PRODUCT_ID_BMSK                                                                                0xffff
#define HWIO_OEM_ID_OEM_PRODUCT_ID_SHFT                                                                                   0x0

#define HWIO_TEST_BUS_SEL_ADDR                                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060e8)
#define HWIO_TEST_BUS_SEL_RMSK                                                                                            0x7
#define HWIO_TEST_BUS_SEL_IN          \
        in_dword_masked(HWIO_TEST_BUS_SEL_ADDR, HWIO_TEST_BUS_SEL_RMSK)
#define HWIO_TEST_BUS_SEL_INM(m)      \
        in_dword_masked(HWIO_TEST_BUS_SEL_ADDR, m)
#define HWIO_TEST_BUS_SEL_OUT(v)      \
        out_dword(HWIO_TEST_BUS_SEL_ADDR,v)
#define HWIO_TEST_BUS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TEST_BUS_SEL_ADDR,m,v,HWIO_TEST_BUS_SEL_IN)
#define HWIO_TEST_BUS_SEL_TEST_EN_BMSK                                                                                    0x4
#define HWIO_TEST_BUS_SEL_TEST_EN_SHFT                                                                                    0x2
#define HWIO_TEST_BUS_SEL_TEST_SELECT_BMSK                                                                                0x3
#define HWIO_TEST_BUS_SEL_TEST_SELECT_SHFT                                                                                0x0

#define HWIO_SPDM_DYN_SECURE_MODE_ADDR                                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060ec)
#define HWIO_SPDM_DYN_SECURE_MODE_RMSK                                                                                    0x1
#define HWIO_SPDM_DYN_SECURE_MODE_IN          \
        in_dword_masked(HWIO_SPDM_DYN_SECURE_MODE_ADDR, HWIO_SPDM_DYN_SECURE_MODE_RMSK)
#define HWIO_SPDM_DYN_SECURE_MODE_INM(m)      \
        in_dword_masked(HWIO_SPDM_DYN_SECURE_MODE_ADDR, m)
#define HWIO_SPDM_DYN_SECURE_MODE_OUT(v)      \
        out_dword(HWIO_SPDM_DYN_SECURE_MODE_ADDR,v)
#define HWIO_SPDM_DYN_SECURE_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPDM_DYN_SECURE_MODE_ADDR,m,v,HWIO_SPDM_DYN_SECURE_MODE_IN)
#define HWIO_SPDM_DYN_SECURE_MODE_SECURE_MODE_BMSK                                                                        0x1
#define HWIO_SPDM_DYN_SECURE_MODE_SECURE_MODE_SHFT                                                                        0x0

#define HWIO_BOOT_PARTITION_SELECT_ADDR                                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060f0)
#define HWIO_BOOT_PARTITION_SELECT_RMSK                                                                            0xffffffff
#define HWIO_BOOT_PARTITION_SELECT_IN          \
        in_dword_masked(HWIO_BOOT_PARTITION_SELECT_ADDR, HWIO_BOOT_PARTITION_SELECT_RMSK)
#define HWIO_BOOT_PARTITION_SELECT_INM(m)      \
        in_dword_masked(HWIO_BOOT_PARTITION_SELECT_ADDR, m)
#define HWIO_BOOT_PARTITION_SELECT_OUT(v)      \
        out_dword(HWIO_BOOT_PARTITION_SELECT_ADDR,v)
#define HWIO_BOOT_PARTITION_SELECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BOOT_PARTITION_SELECT_ADDR,m,v,HWIO_BOOT_PARTITION_SELECT_IN)
#define HWIO_BOOT_PARTITION_SELECT_PARTITION_BMSK                                                                  0xffffffff
#define HWIO_BOOT_PARTITION_SELECT_PARTITION_SHFT                                                                         0x0

/*----------------------------------------------------------------------------
 * MODULE: TLMM_CSR
 *--------------------------------------------------------------------------*/

#define TLMM_CSR_REG_BASE                                                (TLMM_BASE      + 0x00010000)

#define HWIO_TLMM_GPIO_CFGn_ADDR(n)                                      (TLMM_CSR_REG_BASE      + 0x00001000 + 0x10 * (n))
#define HWIO_TLMM_GPIO_CFGn_RMSK                                              0x7ff
#define HWIO_TLMM_GPIO_CFGn_MAXn                                                145
#define HWIO_TLMM_GPIO_CFGn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_CFGn_ADDR(n), HWIO_TLMM_GPIO_CFGn_RMSK)
#define HWIO_TLMM_GPIO_CFGn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_CFGn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_CFGn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_CFGn_ADDR(n),val)
#define HWIO_TLMM_GPIO_CFGn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFGn_ADDR(n),mask,val,HWIO_TLMM_GPIO_CFGn_INI(n))
#define HWIO_TLMM_GPIO_CFGn_GPIO_HIHYS_EN_BMSK                                0x400
#define HWIO_TLMM_GPIO_CFGn_GPIO_HIHYS_EN_SHFT                                  0xa
#define HWIO_TLMM_GPIO_CFGn_GPIO_OE_BMSK                                      0x200
#define HWIO_TLMM_GPIO_CFGn_GPIO_OE_SHFT                                        0x9
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_BMSK                                 0x1c0
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_SHFT                                   0x6
#define HWIO_TLMM_GPIO_CFGn_FUNC_SEL_BMSK                                      0x3c
#define HWIO_TLMM_GPIO_CFGn_FUNC_SEL_SHFT                                       0x2
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_BMSK                                      0x3
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_SHFT                                      0x0

#define HWIO_TLMM_GPIO_IN_OUTn_ADDR(n)                                   (TLMM_CSR_REG_BASE      + 0x00001004 + 0x10 * (n))
#define HWIO_TLMM_GPIO_IN_OUTn_RMSK                                             0x3
#define HWIO_TLMM_GPIO_IN_OUTn_MAXn                                             145
#define HWIO_TLMM_GPIO_IN_OUTn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n), HWIO_TLMM_GPIO_IN_OUTn_RMSK)
#define HWIO_TLMM_GPIO_IN_OUTn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_IN_OUTn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n),val)
#define HWIO_TLMM_GPIO_IN_OUTn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n),mask,val,HWIO_TLMM_GPIO_IN_OUTn_INI(n))
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_OUT_BMSK                                    0x2
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_OUT_SHFT                                    0x1
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_IN_BMSK                                     0x1
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_IN_SHFT                                     0x0

#define HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n)                                 (TLMM_CSR_REG_BASE      + 0x00001008 + 0x10 * (n))
#define HWIO_TLMM_GPIO_INTR_CFGn_RMSK                                         0x1ff
#define HWIO_TLMM_GPIO_INTR_CFGn_MAXn                                           145
#define HWIO_TLMM_GPIO_INTR_CFGn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n), HWIO_TLMM_GPIO_INTR_CFGn_RMSK)
#define HWIO_TLMM_GPIO_INTR_CFGn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_INTR_CFGn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n),val)
#define HWIO_TLMM_GPIO_INTR_CFGn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n),mask,val,HWIO_TLMM_GPIO_INTR_CFGn_INI(n))
#define HWIO_TLMM_GPIO_INTR_CFGn_DIR_CONN_EN_BMSK                             0x100
#define HWIO_TLMM_GPIO_INTR_CFGn_DIR_CONN_EN_SHFT                               0x8
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_BMSK                              0xe0
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_SHFT                               0x5
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_RAW_STATUS_EN_BMSK                       0x10
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_RAW_STATUS_EN_SHFT                        0x4
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_BMSK                             0xc
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_SHFT                             0x2
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_POL_CTL_BMSK                              0x2
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_POL_CTL_SHFT                              0x1
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_ENABLE_BMSK                               0x1
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_ENABLE_SHFT                               0x0

#define HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n)                              (TLMM_CSR_REG_BASE      + 0x0000100c + 0x10 * (n))
#define HWIO_TLMM_GPIO_INTR_STATUSn_RMSK                                        0x1
#define HWIO_TLMM_GPIO_INTR_STATUSn_MAXn                                        145
#define HWIO_TLMM_GPIO_INTR_STATUSn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n), HWIO_TLMM_GPIO_INTR_STATUSn_RMSK)
#define HWIO_TLMM_GPIO_INTR_STATUSn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_INTR_STATUSn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n),val)
#define HWIO_TLMM_GPIO_INTR_STATUSn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n),mask,val,HWIO_TLMM_GPIO_INTR_STATUSn_INI(n))
#define HWIO_TLMM_GPIO_INTR_STATUSn_INTR_STATUS_BMSK                            0x1
#define HWIO_TLMM_GPIO_INTR_STATUSn_INTR_STATUS_SHFT                            0x0

#define HWIO_TLMM_CLK_GATE_EN_ADDR                                       (TLMM_CSR_REG_BASE      + 0x00002000)
#define HWIO_TLMM_CLK_GATE_EN_RMSK                                              0x7
#define HWIO_TLMM_CLK_GATE_EN_IN          \
        in_dword_masked(HWIO_TLMM_CLK_GATE_EN_ADDR, HWIO_TLMM_CLK_GATE_EN_RMSK)
#define HWIO_TLMM_CLK_GATE_EN_INM(m)      \
        in_dword_masked(HWIO_TLMM_CLK_GATE_EN_ADDR, m)
#define HWIO_TLMM_CLK_GATE_EN_OUT(v)      \
        out_dword(HWIO_TLMM_CLK_GATE_EN_ADDR,v)
#define HWIO_TLMM_CLK_GATE_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_CLK_GATE_EN_ADDR,m,v,HWIO_TLMM_CLK_GATE_EN_IN)
#define HWIO_TLMM_CLK_GATE_EN_AHB_HCLK_EN_BMSK                                  0x4
#define HWIO_TLMM_CLK_GATE_EN_AHB_HCLK_EN_SHFT                                  0x2
#define HWIO_TLMM_CLK_GATE_EN_SUMMARY_INTR_EN_BMSK                              0x2
#define HWIO_TLMM_CLK_GATE_EN_SUMMARY_INTR_EN_SHFT                              0x1
#define HWIO_TLMM_CLK_GATE_EN_CRIF_READ_EN_BMSK                                 0x1
#define HWIO_TLMM_CLK_GATE_EN_CRIF_READ_EN_SHFT                                 0x0

#define HWIO_TLMM_IE_CTRL_DISABLE_ADDR                                   (TLMM_CSR_REG_BASE      + 0x00002004)
#define HWIO_TLMM_IE_CTRL_DISABLE_RMSK                                          0x1
#define HWIO_TLMM_IE_CTRL_DISABLE_IN          \
        in_dword_masked(HWIO_TLMM_IE_CTRL_DISABLE_ADDR, HWIO_TLMM_IE_CTRL_DISABLE_RMSK)
#define HWIO_TLMM_IE_CTRL_DISABLE_INM(m)      \
        in_dword_masked(HWIO_TLMM_IE_CTRL_DISABLE_ADDR, m)
#define HWIO_TLMM_IE_CTRL_DISABLE_OUT(v)      \
        out_dword(HWIO_TLMM_IE_CTRL_DISABLE_ADDR,v)
#define HWIO_TLMM_IE_CTRL_DISABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_IE_CTRL_DISABLE_ADDR,m,v,HWIO_TLMM_IE_CTRL_DISABLE_IN)
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_BMSK                          0x1
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_SHFT                          0x0

#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR                               (TLMM_CSR_REG_BASE      + 0x00002008)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_RMSK                               0xffffffff
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_IN          \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR, HWIO_TLMM_MPM_WAKEUP_INT_EN_0_RMSK)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR, m)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_OUT(v)      \
        out_dword(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR,v)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR,m,v,HWIO_TLMM_MPM_WAKEUP_INT_EN_0_IN)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_80_BMSK                       0x80000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_80_SHFT                             0x1f
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_79_BMSK                       0x40000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_79_SHFT                             0x1e
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_77_BMSK                       0x20000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_77_SHFT                             0x1d
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_75_BMSK                       0x10000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_75_SHFT                             0x1c
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_74_BMSK                        0x8000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_74_SHFT                             0x1b
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_73_BMSK                        0x4000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_73_SHFT                             0x1a
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_72_BMSK                        0x2000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_72_SHFT                             0x19
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_71_BMSK                        0x1000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_71_SHFT                             0x18
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_68_BMSK                         0x800000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_68_SHFT                             0x17
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_67_BMSK                         0x400000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_67_SHFT                             0x16
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_66_BMSK                         0x200000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_66_SHFT                             0x15
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_65_BMSK                         0x100000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_65_SHFT                             0x14
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_64_BMSK                          0x80000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_64_SHFT                             0x13
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_62_BMSK                          0x40000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_62_SHFT                             0x12
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_61_BMSK                          0x20000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_61_SHFT                             0x11
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_59_BMSK                          0x10000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_59_SHFT                             0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_54_BMSK                           0x8000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_54_SHFT                              0xf
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_50_BMSK                           0x4000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_50_SHFT                              0xe
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_46_BMSK                           0x2000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_46_SHFT                              0xd
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_44_BMSK                           0x1000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_44_SHFT                              0xc
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_42_BMSK                            0x800
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_42_SHFT                              0xb
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_37_BMSK                            0x400
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_37_SHFT                              0xa
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_35_BMSK                            0x200
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_35_SHFT                              0x9
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_34_BMSK                            0x100
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_34_SHFT                              0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_28_BMSK                             0x80
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_28_SHFT                              0x7
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_27_BMSK                             0x40
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_27_SHFT                              0x6
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_24_BMSK                             0x20
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_24_SHFT                              0x5
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_20_BMSK                             0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_20_SHFT                              0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_18_BMSK                              0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_18_SHFT                              0x3
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_9_BMSK                               0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_9_SHFT                               0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_5_BMSK                               0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_5_SHFT                               0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_1_BMSK                               0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_1_SHFT                               0x0

#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR                               (TLMM_CSR_REG_BASE      + 0x0000200c)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_RMSK                                    0xfff
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_IN          \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR, HWIO_TLMM_MPM_WAKEUP_INT_EN_1_RMSK)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR, m)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_OUT(v)      \
        out_dword(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR,v)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR,m,v,HWIO_TLMM_MPM_WAKEUP_INT_EN_1_IN)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_3_BMSK                        0x800
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_3_SHFT                          0xb
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_1_BMSK                        0x400
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_1_SHFT                          0xa
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC1_DATA_3_BMSK                        0x200
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC1_DATA_3_SHFT                          0x9
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC1_DATA_1_BMSK                        0x100
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC1_DATA_1_SHFT                          0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SRST_N_BMSK                              0x80
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SRST_N_SHFT                               0x7
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_144_BMSK                            0x40
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_144_SHFT                             0x6
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_102_BMSK                            0x20
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_102_SHFT                             0x5
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_95_BMSK                             0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_95_SHFT                              0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_93_BMSK                              0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_93_SHFT                              0x3
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_92_BMSK                              0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_92_SHFT                              0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_86_BMSK                              0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_86_SHFT                              0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_82_BMSK                              0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_82_SHFT                              0x0

#define HWIO_TLMM_INT_JTAG_CTL_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00002010)
#define HWIO_TLMM_INT_JTAG_CTL_RMSK                                            0x1f
#define HWIO_TLMM_INT_JTAG_CTL_IN          \
        in_dword_masked(HWIO_TLMM_INT_JTAG_CTL_ADDR, HWIO_TLMM_INT_JTAG_CTL_RMSK)
#define HWIO_TLMM_INT_JTAG_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_INT_JTAG_CTL_ADDR, m)
#define HWIO_TLMM_INT_JTAG_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_INT_JTAG_CTL_ADDR,v)
#define HWIO_TLMM_INT_JTAG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_INT_JTAG_CTL_ADDR,m,v,HWIO_TLMM_INT_JTAG_CTL_IN)
#define HWIO_TLMM_INT_JTAG_CTL_USB3_PHY_TAP_ENA_BMSK                           0x10
#define HWIO_TLMM_INT_JTAG_CTL_USB3_PHY_TAP_ENA_SHFT                            0x4
#define HWIO_TLMM_INT_JTAG_CTL_KPSS_TAP_ENA_BMSK                                0x8
#define HWIO_TLMM_INT_JTAG_CTL_KPSS_TAP_ENA_SHFT                                0x3
#define HWIO_TLMM_INT_JTAG_CTL_QDSS_TAP_ENA_BMSK                                0x4
#define HWIO_TLMM_INT_JTAG_CTL_QDSS_TAP_ENA_SHFT                                0x2
#define HWIO_TLMM_INT_JTAG_CTL_ACC_TAP_ENA_BMSK                                 0x2
#define HWIO_TLMM_INT_JTAG_CTL_ACC_TAP_ENA_SHFT                                 0x1
#define HWIO_TLMM_INT_JTAG_CTL_MSM_TAP_ENA_BMSK                                 0x1
#define HWIO_TLMM_INT_JTAG_CTL_MSM_TAP_ENA_SHFT                                 0x0

#define HWIO_TLMM_ETM_MODE_ADDR                                          (TLMM_CSR_REG_BASE      + 0x00002014)
#define HWIO_TLMM_ETM_MODE_RMSK                                                 0x1
#define HWIO_TLMM_ETM_MODE_IN          \
        in_dword_masked(HWIO_TLMM_ETM_MODE_ADDR, HWIO_TLMM_ETM_MODE_RMSK)
#define HWIO_TLMM_ETM_MODE_INM(m)      \
        in_dword_masked(HWIO_TLMM_ETM_MODE_ADDR, m)
#define HWIO_TLMM_ETM_MODE_OUT(v)      \
        out_dword(HWIO_TLMM_ETM_MODE_ADDR,v)
#define HWIO_TLMM_ETM_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_ETM_MODE_ADDR,m,v,HWIO_TLMM_ETM_MODE_IN)
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC2_BMSK                                 0x1
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC2_SHFT                                 0x0

#define HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR                                   (TLMM_CSR_REG_BASE      + 0x00002018)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_RMSK                                          0x3
#define HWIO_TLMM_DBG_BUS_OUT_SEL_IN          \
        in_dword_masked(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR, HWIO_TLMM_DBG_BUS_OUT_SEL_RMSK)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_INM(m)      \
        in_dword_masked(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR, m)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_OUT(v)      \
        out_dword(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR,v)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR,m,v,HWIO_TLMM_DBG_BUS_OUT_SEL_IN)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_COPY_SEL_BMSK                                 0x3
#define HWIO_TLMM_DBG_BUS_OUT_SEL_COPY_SEL_SHFT                                 0x0

#define HWIO_TLMM_CHIP_MODE_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00002020)
#define HWIO_TLMM_CHIP_MODE_RMSK                                                0x3
#define HWIO_TLMM_CHIP_MODE_IN          \
        in_dword_masked(HWIO_TLMM_CHIP_MODE_ADDR, HWIO_TLMM_CHIP_MODE_RMSK)
#define HWIO_TLMM_CHIP_MODE_INM(m)      \
        in_dword_masked(HWIO_TLMM_CHIP_MODE_ADDR, m)
#define HWIO_TLMM_CHIP_MODE_MODE1_PIN_BMSK                                      0x2
#define HWIO_TLMM_CHIP_MODE_MODE1_PIN_SHFT                                      0x1
#define HWIO_TLMM_CHIP_MODE_MODE0_PIN_BMSK                                      0x1
#define HWIO_TLMM_CHIP_MODE_MODE0_PIN_SHFT                                      0x0

#define HWIO_TLMM_SPARE_ADDR                                             (TLMM_CSR_REG_BASE      + 0x00002024)
#define HWIO_TLMM_SPARE_RMSK                                                   0xff
#define HWIO_TLMM_SPARE_IN          \
        in_dword_masked(HWIO_TLMM_SPARE_ADDR, HWIO_TLMM_SPARE_RMSK)
#define HWIO_TLMM_SPARE_INM(m)      \
        in_dword_masked(HWIO_TLMM_SPARE_ADDR, m)
#define HWIO_TLMM_SPARE_OUT(v)      \
        out_dword(HWIO_TLMM_SPARE_ADDR,v)
#define HWIO_TLMM_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SPARE_ADDR,m,v,HWIO_TLMM_SPARE_IN)
#define HWIO_TLMM_SPARE_MISC_BMSK                                              0x80
#define HWIO_TLMM_SPARE_MISC_SHFT                                               0x7
#define HWIO_TLMM_SPARE_PERIPH_CHAR_TEST_EN_BMSK                               0x40
#define HWIO_TLMM_SPARE_PERIPH_CHAR_TEST_EN_SHFT                                0x6
#define HWIO_TLMM_SPARE_GP1_CLK_SOURCE_SEL_BMSK                                0x20
#define HWIO_TLMM_SPARE_GP1_CLK_SOURCE_SEL_SHFT                                 0x5
#define HWIO_TLMM_SPARE_GP0_CLK_SOURCE_SEL_BMSK                                0x10
#define HWIO_TLMM_SPARE_GP0_CLK_SOURCE_SEL_SHFT                                 0x4
#define HWIO_TLMM_SPARE_MCLK3_SOURCE_SEL_BMSK                                   0x8
#define HWIO_TLMM_SPARE_MCLK3_SOURCE_SEL_SHFT                                   0x3
#define HWIO_TLMM_SPARE_MCLK2_SOURCE_SEL_BMSK                                   0x4
#define HWIO_TLMM_SPARE_MCLK2_SOURCE_SEL_SHFT                                   0x2
#define HWIO_TLMM_SPARE_MCLK1_SOURCE_SEL_BMSK                                   0x2
#define HWIO_TLMM_SPARE_MCLK1_SOURCE_SEL_SHFT                                   0x1
#define HWIO_TLMM_SPARE_MCLK0_SOURCE_SEL_BMSK                                   0x1
#define HWIO_TLMM_SPARE_MCLK0_SOURCE_SEL_SHFT                                   0x0

#define HWIO_TLMM_HW_REVISION_NUMBER_ADDR                                (TLMM_CSR_REG_BASE      + 0x00002028)
#define HWIO_TLMM_HW_REVISION_NUMBER_RMSK                                0xffffffff
#define HWIO_TLMM_HW_REVISION_NUMBER_IN          \
        in_dword_masked(HWIO_TLMM_HW_REVISION_NUMBER_ADDR, HWIO_TLMM_HW_REVISION_NUMBER_RMSK)
#define HWIO_TLMM_HW_REVISION_NUMBER_INM(m)      \
        in_dword_masked(HWIO_TLMM_HW_REVISION_NUMBER_ADDR, m)
#define HWIO_TLMM_HW_REVISION_NUMBER_VERSION_ID_BMSK                     0xf0000000
#define HWIO_TLMM_HW_REVISION_NUMBER_VERSION_ID_SHFT                           0x1c
#define HWIO_TLMM_HW_REVISION_NUMBER_PRODUCT_DEVICE_ID_BMSK               0xffff000
#define HWIO_TLMM_HW_REVISION_NUMBER_PRODUCT_DEVICE_ID_SHFT                     0xc
#define HWIO_TLMM_HW_REVISION_NUMBER_QUALCOMM_MFG_ID_BMSK                     0xffe
#define HWIO_TLMM_HW_REVISION_NUMBER_QUALCOMM_MFG_ID_SHFT                       0x1
#define HWIO_TLMM_HW_REVISION_NUMBER_START_BIT_BMSK                             0x1
#define HWIO_TLMM_HW_REVISION_NUMBER_START_BIT_SHFT                             0x0

#define HWIO_TLMM_SLIMBUS_CTL_ADDR                                       (TLMM_CSR_REG_BASE      + 0x00002030)
#define HWIO_TLMM_SLIMBUS_CTL_RMSK                                              0x3
#define HWIO_TLMM_SLIMBUS_CTL_IN          \
        in_dword_masked(HWIO_TLMM_SLIMBUS_CTL_ADDR, HWIO_TLMM_SLIMBUS_CTL_RMSK)
#define HWIO_TLMM_SLIMBUS_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_SLIMBUS_CTL_ADDR, m)
#define HWIO_TLMM_SLIMBUS_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_SLIMBUS_CTL_ADDR,v)
#define HWIO_TLMM_SLIMBUS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SLIMBUS_CTL_ADDR,m,v,HWIO_TLMM_SLIMBUS_CTL_IN)
#define HWIO_TLMM_SLIMBUS_CTL_SEN_EN_BMSK                                       0x2
#define HWIO_TLMM_SLIMBUS_CTL_SEN_EN_SHFT                                       0x1
#define HWIO_TLMM_SLIMBUS_CTL_SR_CTL_EN_BMSK                                    0x1
#define HWIO_TLMM_SLIMBUS_CTL_SR_CTL_EN_SHFT                                    0x0

#define HWIO_TLMM_RFFE_CTL_ADDR                                          (TLMM_CSR_REG_BASE      + 0x00002034)
#define HWIO_TLMM_RFFE_CTL_RMSK                                               0xfff
#define HWIO_TLMM_RFFE_CTL_IN          \
        in_dword_masked(HWIO_TLMM_RFFE_CTL_ADDR, HWIO_TLMM_RFFE_CTL_RMSK)
#define HWIO_TLMM_RFFE_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_RFFE_CTL_ADDR, m)
#define HWIO_TLMM_RFFE_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_RFFE_CTL_ADDR,v)
#define HWIO_TLMM_RFFE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_RFFE_CTL_ADDR,m,v,HWIO_TLMM_RFFE_CTL_IN)
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SEN_EN_BMSK                             0x800
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SEN_EN_SHFT                               0xb
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SR_CTL_EN_BMSK                          0x600
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SR_CTL_EN_SHFT                            0x9
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SEN_EN_BMSK                              0x100
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SEN_EN_SHFT                                0x8
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SR_CTL_EN_BMSK                            0xc0
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SR_CTL_EN_SHFT                             0x6
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SEN_EN_BMSK                              0x20
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SEN_EN_SHFT                               0x5
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SR_CTL_EN_BMSK                           0x18
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SR_CTL_EN_SHFT                            0x3
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SEN_EN_BMSK                                0x4
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SEN_EN_SHFT                                0x2
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SR_CTL_EN_BMSK                             0x3
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SR_CTL_EN_SHFT                             0x0

#define HWIO_TLMM_RESOUT_HDRV_CTL_ADDR                                   (TLMM_CSR_REG_BASE      + 0x00002038)
#define HWIO_TLMM_RESOUT_HDRV_CTL_RMSK                                          0x7
#define HWIO_TLMM_RESOUT_HDRV_CTL_IN          \
        in_dword_masked(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR, HWIO_TLMM_RESOUT_HDRV_CTL_RMSK)
#define HWIO_TLMM_RESOUT_HDRV_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR, m)
#define HWIO_TLMM_RESOUT_HDRV_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR,v)
#define HWIO_TLMM_RESOUT_HDRV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR,m,v,HWIO_TLMM_RESOUT_HDRV_CTL_IN)
#define HWIO_TLMM_RESOUT_HDRV_CTL_RESOUT_N_HDRV_BMSK                            0x7
#define HWIO_TLMM_RESOUT_HDRV_CTL_RESOUT_N_HDRV_SHFT                            0x0

#define HWIO_TLMM_JTAG_HDRV_CTL_ADDR                                     (TLMM_CSR_REG_BASE      + 0x0000203c)
#define HWIO_TLMM_JTAG_HDRV_CTL_RMSK                                          0x3ff
#define HWIO_TLMM_JTAG_HDRV_CTL_IN          \
        in_dword_masked(HWIO_TLMM_JTAG_HDRV_CTL_ADDR, HWIO_TLMM_JTAG_HDRV_CTL_RMSK)
#define HWIO_TLMM_JTAG_HDRV_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_JTAG_HDRV_CTL_ADDR, m)
#define HWIO_TLMM_JTAG_HDRV_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_JTAG_HDRV_CTL_ADDR,v)
#define HWIO_TLMM_JTAG_HDRV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_JTAG_HDRV_CTL_ADDR,m,v,HWIO_TLMM_JTAG_HDRV_CTL_IN)
#define HWIO_TLMM_JTAG_HDRV_CTL_TDI_PULL_BMSK                                 0x300
#define HWIO_TLMM_JTAG_HDRV_CTL_TDI_PULL_SHFT                                   0x8
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_PULL_BMSK                                  0xc0
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_PULL_SHFT                                   0x6
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_HDRV_BMSK                                  0x38
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_HDRV_SHFT                                   0x3
#define HWIO_TLMM_JTAG_HDRV_CTL_TDO_HDRV_BMSK                                   0x7
#define HWIO_TLMM_JTAG_HDRV_CTL_TDO_HDRV_SHFT                                   0x0

#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR                                (TLMM_CSR_REG_BASE      + 0x00002040)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_RMSK                                   0x3ffff
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR, HWIO_TLMM_PMIC_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_PMIC_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_SEN_EN_BMSK             0x20000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_SEN_EN_SHFT                0x11
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_SEN_EN_BMSK              0x10000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_SEN_EN_SHFT                 0x10
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_PULL_BMSK                0xc000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_PULL_SHFT                   0xe
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_PULL_BMSK                 0x3000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_PULL_SHFT                    0xc
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PSHOLD_HDRV_BMSK                         0xe00
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PSHOLD_HDRV_SHFT                           0x9
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_CXO_EN_HDRV_BMSK                         0x1c0
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_CXO_EN_HDRV_SHFT                           0x6
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_HDRV_BMSK                  0x38
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_HDRV_SHFT                   0x3
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_HDRV_BMSK                    0x7
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_HDRV_SHFT                    0x0

#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR                                (TLMM_CSR_REG_BASE      + 0x00002044)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_RMSK                                    0x7fff
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR, HWIO_TLMM_SDC1_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_SDC1_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_PULL_BMSK                      0x6000
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_PULL_SHFT                         0xd
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_PULL_BMSK                      0x1800
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_PULL_SHFT                         0xb
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_PULL_BMSK                      0x600
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_PULL_SHFT                        0x9
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_HDRV_BMSK                       0x1c0
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_HDRV_SHFT                         0x6
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_HDRV_BMSK                        0x38
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_HDRV_SHFT                         0x3
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_HDRV_BMSK                        0x7
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_HDRV_SHFT                        0x0

#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR                                (TLMM_CSR_REG_BASE      + 0x00002048)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_RMSK                                    0xffff
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR, HWIO_TLMM_SDC2_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_SDC2_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_PULL_BMSK                      0xc000
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_PULL_SHFT                         0xe
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_HYS_CTL_BMSK                       0x2000
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_HYS_CTL_SHFT                          0xd
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_PULL_BMSK                      0x1800
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_PULL_SHFT                         0xb
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_PULL_BMSK                      0x600
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_PULL_SHFT                        0x9
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_HDRV_BMSK                       0x1c0
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_HDRV_SHFT                         0x6
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_HDRV_BMSK                        0x38
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_HDRV_SHFT                         0x3
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_HDRV_BMSK                        0x7
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_HDRV_SHFT                        0x0

#define HWIO_TLMM_MODE_PULL_CTL_ADDR                                     (TLMM_CSR_REG_BASE      + 0x0000204c)
#define HWIO_TLMM_MODE_PULL_CTL_RMSK                                            0xf
#define HWIO_TLMM_MODE_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_MODE_PULL_CTL_ADDR, HWIO_TLMM_MODE_PULL_CTL_RMSK)
#define HWIO_TLMM_MODE_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_MODE_PULL_CTL_ADDR, m)
#define HWIO_TLMM_MODE_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_MODE_PULL_CTL_ADDR,v)
#define HWIO_TLMM_MODE_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_MODE_PULL_CTL_ADDR,m,v,HWIO_TLMM_MODE_PULL_CTL_IN)
#define HWIO_TLMM_MODE_PULL_CTL_MODE_1_PULL_BMSK                                0xc
#define HWIO_TLMM_MODE_PULL_CTL_MODE_1_PULL_SHFT                                0x2
#define HWIO_TLMM_MODE_PULL_CTL_MODE_0_PULL_BMSK                                0x3
#define HWIO_TLMM_MODE_PULL_CTL_MODE_0_PULL_SHFT                                0x0

#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ADDR                          (TLMM_CSR_REG_BASE      + 0x00002050)
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_RMSK                           0x3ffffff
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ADDR, HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_RMSK)
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ADDR, m)
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ADDR,v)
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ADDR,m,v,HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_IN)
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_CORE_CTL_EN_BMSK               0x2000000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_CORE_CTL_EN_SHFT                    0x19
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_VM_SHIFT_EN_BMSK               0x1000000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_VM_SHIFT_EN_SHFT                    0x18
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ODT_EN_BMSK                     0x800000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ODT_EN_SHFT                         0x17
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ODT_BMSK                        0x600000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ODT_SHFT                            0x15
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_DCC_BMSK                        0x1c0000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_DCC_SHFT                            0x12
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_NRXDEL_BMSK                      0x30000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_NRXDEL_SHFT                         0x10
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_PRXDEL_BMSK                       0xc000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_PRXDEL_SHFT                          0xe
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_NSLEW_BMSK                        0x3000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_NSLEW_SHFT                           0xc
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_PSLEW_BMSK                         0xc00
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_PSLEW_SHFT                           0xa
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_NCNT_BMSK                          0x3e0
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_NCNT_SHFT                            0x5
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_PCNT_BMSK                           0x1f
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_PCNT_SHFT                            0x0

#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ADDR                            (TLMM_CSR_REG_BASE      + 0x00002054)
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_RMSK                             0x3ffffff
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ADDR, HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_RMSK)
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ADDR, m)
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ADDR,v)
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ADDR,m,v,HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_IN)
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_CORE_CTL_EN_BMSK                 0x2000000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_CORE_CTL_EN_SHFT                      0x19
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_VM_SHIFT_EN_BMSK                 0x1000000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_VM_SHIFT_EN_SHFT                      0x18
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ODT_EN_BMSK                       0x800000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ODT_EN_SHFT                           0x17
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ODT_BMSK                          0x600000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ODT_SHFT                              0x15
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_DCC_BMSK                          0x1c0000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_DCC_SHFT                              0x12
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_NRXDEL_BMSK                        0x30000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_NRXDEL_SHFT                           0x10
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_PRXDEL_BMSK                         0xc000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_PRXDEL_SHFT                            0xe
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_NSLEW_BMSK                          0x3000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_NSLEW_SHFT                             0xc
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_PSLEW_BMSK                           0xc00
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_PSLEW_SHFT                             0xa
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_NCNT_BMSK                            0x3e0
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_NCNT_SHFT                              0x5
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_PCNT_BMSK                             0x1f
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_PCNT_SHFT                              0x0

#define HWIO_TLMM_HSIC_CAL_PAD_CTL_ADDR                                  (TLMM_CSR_REG_BASE      + 0x00002058)
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_RMSK                                         0x3
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_IN          \
        in_dword_masked(HWIO_TLMM_HSIC_CAL_PAD_CTL_ADDR, HWIO_TLMM_HSIC_CAL_PAD_CTL_RMSK)
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_HSIC_CAL_PAD_CTL_ADDR, m)
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_HSIC_CAL_PAD_CTL_ADDR,v)
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_HSIC_CAL_PAD_CTL_ADDR,m,v,HWIO_TLMM_HSIC_CAL_PAD_CTL_IN)
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_IMP_SEL_BMSK                                 0x3
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_IMP_SEL_SHFT                                 0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n)                        (TLMM_CSR_REG_BASE      + 0x00002100 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_RMSK                                0x1ff
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_MAXn                                    1
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_POLARITY_BMSK                       0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_POLARITY_SHFT                         0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_GPIO_SEL_BMSK                        0xff
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_GPIO_SEL_SHFT                         0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n)                     (TLMM_CSR_REG_BASE      + 0x00002200 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_RMSK                             0x1ff
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_MAXn                                 9
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_POLARITY_BMSK                    0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_POLARITY_SHFT                      0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_GPIO_SEL_BMSK                     0xff
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_GPIO_SEL_SHFT                      0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n)                     (TLMM_CSR_REG_BASE      + 0x00002500 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_RMSK                             0x1ff
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_MAXn                                 5
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_POLARITY_BMSK                    0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_POLARITY_SHFT                      0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_GPIO_SEL_BMSK                     0xff
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_GPIO_SEL_SHFT                      0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n)                         (TLMM_CSR_REG_BASE      + 0x00002700 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_RMSK                                 0x1ff
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_MAXn                                     0
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_POLARITY_BMSK                        0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_POLARITY_SHFT                          0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_GPIO_SEL_BMSK                         0xff
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_GPIO_SEL_SHFT                          0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_ADDR(n)                        (TLMM_CSR_REG_BASE      + 0x00002800 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_RMSK                                0x1ff
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_MAXn                                    7
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_POLARITY_BMSK                       0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_POLARITY_SHFT                         0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_GPIO_SEL_BMSK                        0xff
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_GPIO_SEL_SHFT                         0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n)                         (TLMM_CSR_REG_BASE      + 0x00002a00 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_RMSK                                 0x1ff
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_MAXn                                     1
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_POLARITY_BMSK                        0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_POLARITY_SHFT                          0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_GPIO_SEL_BMSK                         0xff
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_GPIO_SEL_SHFT                          0x0

#define HWIO_TLMM_GPIO_OUT_0_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00003000)
#define HWIO_TLMM_GPIO_OUT_0_RMSK                                        0xffffffff
#define HWIO_TLMM_GPIO_OUT_0_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_0_ADDR, HWIO_TLMM_GPIO_OUT_0_RMSK)
#define HWIO_TLMM_GPIO_OUT_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_0_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_0_ADDR,m,v,HWIO_TLMM_GPIO_OUT_0_IN)
#define HWIO_TLMM_GPIO_OUT_0_GPIO_OUT_BMSK                               0xffffffff
#define HWIO_TLMM_GPIO_OUT_0_GPIO_OUT_SHFT                                      0x0

#define HWIO_TLMM_GPIO_OUT_1_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00003004)
#define HWIO_TLMM_GPIO_OUT_1_RMSK                                        0xffffffff
#define HWIO_TLMM_GPIO_OUT_1_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_1_ADDR, HWIO_TLMM_GPIO_OUT_1_RMSK)
#define HWIO_TLMM_GPIO_OUT_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_1_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_1_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_1_ADDR,m,v,HWIO_TLMM_GPIO_OUT_1_IN)
#define HWIO_TLMM_GPIO_OUT_1_GPIO_OUT_BMSK                               0xffffffff
#define HWIO_TLMM_GPIO_OUT_1_GPIO_OUT_SHFT                                      0x0

#define HWIO_TLMM_GPIO_OUT_2_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00003008)
#define HWIO_TLMM_GPIO_OUT_2_RMSK                                        0xffffffff
#define HWIO_TLMM_GPIO_OUT_2_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_2_ADDR, HWIO_TLMM_GPIO_OUT_2_RMSK)
#define HWIO_TLMM_GPIO_OUT_2_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_2_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_2_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_2_ADDR,m,v,HWIO_TLMM_GPIO_OUT_2_IN)
#define HWIO_TLMM_GPIO_OUT_2_GPIO_OUT_BMSK                               0xffffffff
#define HWIO_TLMM_GPIO_OUT_2_GPIO_OUT_SHFT                                      0x0

#define HWIO_TLMM_GPIO_OUT_3_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0000300c)
#define HWIO_TLMM_GPIO_OUT_3_RMSK                                        0xffffffff
#define HWIO_TLMM_GPIO_OUT_3_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_3_ADDR, HWIO_TLMM_GPIO_OUT_3_RMSK)
#define HWIO_TLMM_GPIO_OUT_3_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_3_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_3_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_3_ADDR,m,v,HWIO_TLMM_GPIO_OUT_3_IN)
#define HWIO_TLMM_GPIO_OUT_3_GPIO_OUT_BMSK                               0xffffffff
#define HWIO_TLMM_GPIO_OUT_3_GPIO_OUT_SHFT                                      0x0

#define HWIO_TLMM_GPIO_OUT_4_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00003010)
#define HWIO_TLMM_GPIO_OUT_4_RMSK                                           0x3ffff
#define HWIO_TLMM_GPIO_OUT_4_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_4_ADDR, HWIO_TLMM_GPIO_OUT_4_RMSK)
#define HWIO_TLMM_GPIO_OUT_4_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_4_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_4_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_4_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_4_ADDR,m,v,HWIO_TLMM_GPIO_OUT_4_IN)
#define HWIO_TLMM_GPIO_OUT_4_GPIO_OUT_BMSK                                  0x3ffff
#define HWIO_TLMM_GPIO_OUT_4_GPIO_OUT_SHFT                                      0x0

#define HWIO_TLMM_GPIO_OUT_CLR_0_ADDR                                    (TLMM_CSR_REG_BASE      + 0x00003020)
#define HWIO_TLMM_GPIO_OUT_CLR_0_RMSK                                    0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_0_GPIO_OUT_CLR_BMSK                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_0_GPIO_OUT_CLR_SHFT                              0x0

#define HWIO_TLMM_GPIO_OUT_CLR_1_ADDR                                    (TLMM_CSR_REG_BASE      + 0x00003024)
#define HWIO_TLMM_GPIO_OUT_CLR_1_RMSK                                    0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_1_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_1_GPIO_OUT_CLR_BMSK                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_1_GPIO_OUT_CLR_SHFT                              0x0

#define HWIO_TLMM_GPIO_OUT_CLR_2_ADDR                                    (TLMM_CSR_REG_BASE      + 0x00003028)
#define HWIO_TLMM_GPIO_OUT_CLR_2_RMSK                                    0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_2_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_2_GPIO_OUT_CLR_BMSK                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_2_GPIO_OUT_CLR_SHFT                              0x0

#define HWIO_TLMM_GPIO_OUT_CLR_3_ADDR                                    (TLMM_CSR_REG_BASE      + 0x0000302c)
#define HWIO_TLMM_GPIO_OUT_CLR_3_RMSK                                    0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_3_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_3_GPIO_OUT_CLR_BMSK                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_3_GPIO_OUT_CLR_SHFT                              0x0

#define HWIO_TLMM_GPIO_OUT_CLR_4_ADDR                                    (TLMM_CSR_REG_BASE      + 0x00003030)
#define HWIO_TLMM_GPIO_OUT_CLR_4_RMSK                                       0x3ffff
#define HWIO_TLMM_GPIO_OUT_CLR_4_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_4_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_4_GPIO_OUT_CLR_BMSK                          0x3ffff
#define HWIO_TLMM_GPIO_OUT_CLR_4_GPIO_OUT_CLR_SHFT                              0x0

#define HWIO_TLMM_GPIO_OUT_SET_0_ADDR                                    (TLMM_CSR_REG_BASE      + 0x00003040)
#define HWIO_TLMM_GPIO_OUT_SET_0_RMSK                                    0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_0_GPIO_OUT_SET_BMSK                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_0_GPIO_OUT_SET_SHFT                              0x0

#define HWIO_TLMM_GPIO_OUT_SET_1_ADDR                                    (TLMM_CSR_REG_BASE      + 0x00003044)
#define HWIO_TLMM_GPIO_OUT_SET_1_RMSK                                    0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_1_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_1_GPIO_OUT_SET_BMSK                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_1_GPIO_OUT_SET_SHFT                              0x0

#define HWIO_TLMM_GPIO_OUT_SET_2_ADDR                                    (TLMM_CSR_REG_BASE      + 0x00003048)
#define HWIO_TLMM_GPIO_OUT_SET_2_RMSK                                    0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_2_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_2_GPIO_OUT_SET_BMSK                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_2_GPIO_OUT_SET_SHFT                              0x0

#define HWIO_TLMM_GPIO_OUT_SET_3_ADDR                                    (TLMM_CSR_REG_BASE      + 0x0000304c)
#define HWIO_TLMM_GPIO_OUT_SET_3_RMSK                                    0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_3_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_3_GPIO_OUT_SET_BMSK                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_3_GPIO_OUT_SET_SHFT                              0x0

#define HWIO_TLMM_GPIO_OUT_SET_4_ADDR                                    (TLMM_CSR_REG_BASE      + 0x00003050)
#define HWIO_TLMM_GPIO_OUT_SET_4_RMSK                                       0x3ffff
#define HWIO_TLMM_GPIO_OUT_SET_4_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_4_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_4_GPIO_OUT_SET_BMSK                          0x3ffff
#define HWIO_TLMM_GPIO_OUT_SET_4_GPIO_OUT_SET_SHFT                              0x0

#define HWIO_TLMM_GPIO_IN_0_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00003060)
#define HWIO_TLMM_GPIO_IN_0_RMSK                                         0xffffffff
#define HWIO_TLMM_GPIO_IN_0_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_0_ADDR, HWIO_TLMM_GPIO_IN_0_RMSK)
#define HWIO_TLMM_GPIO_IN_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_0_ADDR, m)
#define HWIO_TLMM_GPIO_IN_0_GPIO_IN_BMSK                                 0xffffffff
#define HWIO_TLMM_GPIO_IN_0_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_IN_1_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00003064)
#define HWIO_TLMM_GPIO_IN_1_RMSK                                         0xffffffff
#define HWIO_TLMM_GPIO_IN_1_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_1_ADDR, HWIO_TLMM_GPIO_IN_1_RMSK)
#define HWIO_TLMM_GPIO_IN_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_1_ADDR, m)
#define HWIO_TLMM_GPIO_IN_1_GPIO_IN_BMSK                                 0xffffffff
#define HWIO_TLMM_GPIO_IN_1_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_IN_2_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00003068)
#define HWIO_TLMM_GPIO_IN_2_RMSK                                         0xffffffff
#define HWIO_TLMM_GPIO_IN_2_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_2_ADDR, HWIO_TLMM_GPIO_IN_2_RMSK)
#define HWIO_TLMM_GPIO_IN_2_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_2_ADDR, m)
#define HWIO_TLMM_GPIO_IN_2_GPIO_IN_BMSK                                 0xffffffff
#define HWIO_TLMM_GPIO_IN_2_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_IN_3_ADDR                                         (TLMM_CSR_REG_BASE      + 0x0000306c)
#define HWIO_TLMM_GPIO_IN_3_RMSK                                         0xffffffff
#define HWIO_TLMM_GPIO_IN_3_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_3_ADDR, HWIO_TLMM_GPIO_IN_3_RMSK)
#define HWIO_TLMM_GPIO_IN_3_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_3_ADDR, m)
#define HWIO_TLMM_GPIO_IN_3_GPIO_IN_BMSK                                 0xffffffff
#define HWIO_TLMM_GPIO_IN_3_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_IN_4_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00003070)
#define HWIO_TLMM_GPIO_IN_4_RMSK                                            0x3ffff
#define HWIO_TLMM_GPIO_IN_4_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_4_ADDR, HWIO_TLMM_GPIO_IN_4_RMSK)
#define HWIO_TLMM_GPIO_IN_4_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_4_ADDR, m)
#define HWIO_TLMM_GPIO_IN_4_GPIO_IN_BMSK                                    0x3ffff
#define HWIO_TLMM_GPIO_IN_4_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_OE_0_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00003080)
#define HWIO_TLMM_GPIO_OE_0_RMSK                                         0xffffffff
#define HWIO_TLMM_GPIO_OE_0_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_0_ADDR, HWIO_TLMM_GPIO_OE_0_RMSK)
#define HWIO_TLMM_GPIO_OE_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_0_ADDR, m)
#define HWIO_TLMM_GPIO_OE_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_0_ADDR,m,v,HWIO_TLMM_GPIO_OE_0_IN)
#define HWIO_TLMM_GPIO_OE_0_GPIO_OE_BMSK                                 0xffffffff
#define HWIO_TLMM_GPIO_OE_0_GPIO_OE_SHFT                                        0x0

#define HWIO_TLMM_GPIO_OE_1_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00003084)
#define HWIO_TLMM_GPIO_OE_1_RMSK                                         0xffffffff
#define HWIO_TLMM_GPIO_OE_1_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_1_ADDR, HWIO_TLMM_GPIO_OE_1_RMSK)
#define HWIO_TLMM_GPIO_OE_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_1_ADDR, m)
#define HWIO_TLMM_GPIO_OE_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_1_ADDR,v)
#define HWIO_TLMM_GPIO_OE_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_1_ADDR,m,v,HWIO_TLMM_GPIO_OE_1_IN)
#define HWIO_TLMM_GPIO_OE_1_GPIO_OE_BMSK                                 0xffffffff
#define HWIO_TLMM_GPIO_OE_1_GPIO_OE_SHFT                                        0x0

#define HWIO_TLMM_GPIO_OE_2_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00003088)
#define HWIO_TLMM_GPIO_OE_2_RMSK                                         0xffffffff
#define HWIO_TLMM_GPIO_OE_2_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_2_ADDR, HWIO_TLMM_GPIO_OE_2_RMSK)
#define HWIO_TLMM_GPIO_OE_2_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_2_ADDR, m)
#define HWIO_TLMM_GPIO_OE_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_2_ADDR,v)
#define HWIO_TLMM_GPIO_OE_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_2_ADDR,m,v,HWIO_TLMM_GPIO_OE_2_IN)
#define HWIO_TLMM_GPIO_OE_2_GPIO_OE_BMSK                                 0xffffffff
#define HWIO_TLMM_GPIO_OE_2_GPIO_OE_SHFT                                        0x0

#define HWIO_TLMM_GPIO_OE_3_ADDR                                         (TLMM_CSR_REG_BASE      + 0x0000308c)
#define HWIO_TLMM_GPIO_OE_3_RMSK                                         0xffffffff
#define HWIO_TLMM_GPIO_OE_3_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_3_ADDR, HWIO_TLMM_GPIO_OE_3_RMSK)
#define HWIO_TLMM_GPIO_OE_3_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_3_ADDR, m)
#define HWIO_TLMM_GPIO_OE_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_3_ADDR,v)
#define HWIO_TLMM_GPIO_OE_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_3_ADDR,m,v,HWIO_TLMM_GPIO_OE_3_IN)
#define HWIO_TLMM_GPIO_OE_3_GPIO_OE_BMSK                                 0xffffffff
#define HWIO_TLMM_GPIO_OE_3_GPIO_OE_SHFT                                        0x0

#define HWIO_TLMM_GPIO_OE_4_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00003090)
#define HWIO_TLMM_GPIO_OE_4_RMSK                                            0x3ffff
#define HWIO_TLMM_GPIO_OE_4_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_4_ADDR, HWIO_TLMM_GPIO_OE_4_RMSK)
#define HWIO_TLMM_GPIO_OE_4_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_4_ADDR, m)
#define HWIO_TLMM_GPIO_OE_4_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_4_ADDR,v)
#define HWIO_TLMM_GPIO_OE_4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_4_ADDR,m,v,HWIO_TLMM_GPIO_OE_4_IN)
#define HWIO_TLMM_GPIO_OE_4_GPIO_OE_BMSK                                    0x3ffff
#define HWIO_TLMM_GPIO_OE_4_GPIO_OE_SHFT                                        0x0

#define HWIO_TLMM_GPIO_OE_CLR_0_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003100)
#define HWIO_TLMM_GPIO_OE_CLR_0_RMSK                                     0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_0_GPIO_OE_CLR_BMSK                         0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_0_GPIO_OE_CLR_SHFT                                0x0

#define HWIO_TLMM_GPIO_OE_CLR_1_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003104)
#define HWIO_TLMM_GPIO_OE_CLR_1_RMSK                                     0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_1_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_1_GPIO_OE_CLR_BMSK                         0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_1_GPIO_OE_CLR_SHFT                                0x0

#define HWIO_TLMM_GPIO_OE_CLR_2_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003108)
#define HWIO_TLMM_GPIO_OE_CLR_2_RMSK                                     0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_2_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_2_GPIO_OE_CLR_BMSK                         0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_2_GPIO_OE_CLR_SHFT                                0x0

#define HWIO_TLMM_GPIO_OE_CLR_3_ADDR                                     (TLMM_CSR_REG_BASE      + 0x0000310c)
#define HWIO_TLMM_GPIO_OE_CLR_3_RMSK                                     0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_3_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_3_GPIO_OE_CLR_BMSK                         0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_3_GPIO_OE_CLR_SHFT                                0x0

#define HWIO_TLMM_GPIO_OE_CLR_4_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003110)
#define HWIO_TLMM_GPIO_OE_CLR_4_RMSK                                        0x3ffff
#define HWIO_TLMM_GPIO_OE_CLR_4_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_4_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_4_GPIO_OE_CLR_BMSK                            0x3ffff
#define HWIO_TLMM_GPIO_OE_CLR_4_GPIO_OE_CLR_SHFT                                0x0

#define HWIO_TLMM_GPIO_OE_SET_0_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003120)
#define HWIO_TLMM_GPIO_OE_SET_0_RMSK                                     0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_0_GPIO_OE_SET_BMSK                         0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_0_GPIO_OE_SET_SHFT                                0x0

#define HWIO_TLMM_GPIO_OE_SET_1_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003124)
#define HWIO_TLMM_GPIO_OE_SET_1_RMSK                                     0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_1_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_1_GPIO_OE_SET_BMSK                         0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_1_GPIO_OE_SET_SHFT                                0x0

#define HWIO_TLMM_GPIO_OE_SET_2_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003128)
#define HWIO_TLMM_GPIO_OE_SET_2_RMSK                                     0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_2_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_2_GPIO_OE_SET_BMSK                         0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_2_GPIO_OE_SET_SHFT                                0x0

#define HWIO_TLMM_GPIO_OE_SET_3_ADDR                                     (TLMM_CSR_REG_BASE      + 0x0000312c)
#define HWIO_TLMM_GPIO_OE_SET_3_RMSK                                     0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_3_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_3_GPIO_OE_SET_BMSK                         0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_3_GPIO_OE_SET_SHFT                                0x0

#define HWIO_TLMM_GPIO_OE_SET_4_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003130)
#define HWIO_TLMM_GPIO_OE_SET_4_RMSK                                        0x3ffff
#define HWIO_TLMM_GPIO_OE_SET_4_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_4_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_4_GPIO_OE_SET_BMSK                            0x3ffff
#define HWIO_TLMM_GPIO_OE_SET_4_GPIO_OE_SET_SHFT                                0x0


/*----------------------------------------------------------------------------
 * MODULE: PERIPH_SS_QPIC_QPIC_VMIDMT
 *--------------------------------------------------------------------------*/
#ifndef COPSS_ELAN_BASE
#define COPSS_ELAN_BASE
#endif

#define PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE                                      (COPSS_ELAN_BASE      + 0x002c0000)

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_ADDR                                (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000000)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_RMSK                                0x3ff707f5
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_NSCFG_BMSK                          0x30000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_NSCFG_SHFT                                0x1c
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_WACFG_BMSK                           0xc000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_WACFG_SHFT                                0x1a
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_RACFG_BMSK                           0x3000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_RACFG_SHFT                                0x18
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_SHCFG_BMSK                            0xc00000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_SHCFG_SHFT                                0x16
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_SMCFCFG_BMSK                          0x200000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_SMCFCFG_SHFT                              0x15
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_MTCFG_BMSK                            0x100000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_MTCFG_SHFT                                0x14
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_MEMATTR_BMSK                           0x70000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_MEMATTR_SHFT                              0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_USFCFG_BMSK                              0x400
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_USFCFG_SHFT                                0xa
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_GSE_BMSK                                 0x200
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_GSE_SHFT                                   0x9
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_STALLD_BMSK                              0x100
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_STALLD_SHFT                                0x8
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_TRANSIENTCFG_BMSK                         0xc0
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_TRANSIENTCFG_SHFT                          0x6
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_GCFGFIE_BMSK                              0x20
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_GCFGFIE_SHFT                               0x5
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_GCFGERE_BMSK                              0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_GCFGERE_SHFT                               0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_GFIE_BMSK                                  0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_GFIE_SHFT                                  0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_CLIENTPD_BMSK                              0x1
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR0_CLIENTPD_SHFT                              0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR1_ADDR                                (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000004)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR1_RMSK                                 0x1000f00
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR1_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR1_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR1_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR1_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR1_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR1_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR1_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR1_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR1_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR1_GASRAE_BMSK                          0x1000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR1_GASRAE_SHFT                               0x18
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR1_NSNUMSMRGO_BMSK                          0xf00
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR1_NSNUMSMRGO_SHFT                            0x8

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR2_ADDR                                (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000008)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR2_RMSK                                      0x1f
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR2_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR2_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR2_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR2_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR2_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR2_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR2_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR2_BPVMID_BMSK                               0x1f
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SCR2_BPVMID_SHFT                                0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_ADDR                                (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000010)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_RMSK                                0x70000013
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_BPRCNSH_BMSK                        0x40000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_BPRCNSH_SHFT                              0x1e
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_BPRCISH_BMSK                        0x20000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_BPRCISH_SHFT                              0x1d
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_BPRCOSH_BMSK                        0x10000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_BPRCOSH_SHFT                              0x1c
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_BPREQPRIORITYCFG_BMSK                     0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_BPREQPRIORITYCFG_SHFT                      0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_BPREQPRIORITY_BMSK                         0x3
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SACR_BPREQPRIORITY_SHFT                         0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR0_ADDR                               (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000020)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR0_RMSK                               0x88001eff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR0_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR0_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR0_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR0_SES_BMSK                           0x80000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR0_SES_SHFT                                 0x1f
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR0_SMS_BMSK                            0x8000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR0_SMS_SHFT                                 0x1b
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR0_NUMSIDB_BMSK                           0x1e00
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR0_NUMSIDB_SHFT                              0x9
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR0_NUMSMRG_BMSK                             0xff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR0_NUMSMRG_SHFT                              0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR1_ADDR                               (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000024)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR1_RMSK                                   0x9f00
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR1_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR1_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR1_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR1_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR1_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR1_SMCD_BMSK                              0x8000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR1_SMCD_SHFT                                 0xf
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR1_SSDTP_BMSK                             0x1000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR1_SSDTP_SHFT                                0xc
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR1_NUMSSDNDX_BMSK                          0xf00
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR1_NUMSSDNDX_SHFT                            0x8

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR2_ADDR                               (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000028)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR2_RMSK                                     0xff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR2_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR2_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR2_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR2_OAS_BMSK                                 0xf0
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR2_OAS_SHFT                                  0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR2_IAS_BMSK                                  0xf
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR2_IAS_SHFT                                  0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR4_ADDR                               (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000030)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR4_RMSK                               0xffffffff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR4_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR4_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR4_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR4_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR4_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR4_MAJOR_BMSK                         0xf0000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR4_MAJOR_SHFT                               0x1c
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR4_MINOR_BMSK                          0xfff0000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR4_MINOR_SHFT                               0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR4_STEP_BMSK                              0xffff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR4_STEP_SHFT                                 0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR5_ADDR                               (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000034)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR5_RMSK                                 0xff03ff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR5_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR5_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR5_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR5_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR5_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR5_NUMMSDRB_BMSK                        0xff0000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR5_NUMMSDRB_SHFT                            0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR5_MSAE_BMSK                               0x200
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR5_MSAE_SHFT                                 0x9
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR5_QRIBE_BMSK                              0x100
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR5_QRIBE_SHFT                                0x8
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR5_NVMID_BMSK                               0xff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR5_NVMID_SHFT                                0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR7_ADDR                               (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x0000003c)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR7_RMSK                                     0xff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR7_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR7_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR7_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR7_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR7_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR7_MAJOR_BMSK                               0xf0
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR7_MAJOR_SHFT                                0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR7_MINOR_BMSK                                0xf
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SIDR7_MINOR_SHFT                                0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFAR0_ADDR                              (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000040)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFAR0_RMSK                              0xffffffff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFAR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFAR0_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFAR0_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFAR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFAR0_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFAR0_SGFEA0_BMSK                       0xffffffff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFAR0_SGFEA0_SHFT                              0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_ADDR                               (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000048)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_RMSK                               0xc0000026
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_MULTI_CLIENT_BMSK                  0x80000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_MULTI_CLIENT_SHFT                        0x1f
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_MULTI_CFG_BMSK                     0x40000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_MULTI_CFG_SHFT                           0x1e
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_CAF_BMSK                                 0x20
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_CAF_SHFT                                  0x5
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_SMCF_BMSK                                 0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_SMCF_SHFT                                 0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_USF_BMSK                                  0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSR_USF_SHFT                                  0x1

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_ADDR                        (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x0000004c)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_RMSK                        0xc0000026
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_MULTI_CLIENT_BMSK           0x80000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_MULTI_CLIENT_SHFT                 0x1f
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_MULTI_CFG_BMSK              0x40000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_MULTI_CFG_SHFT                    0x1e
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_CAF_BMSK                          0x20
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_CAF_SHFT                           0x5
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_SMCF_BMSK                          0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_SMCF_SHFT                          0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_USF_BMSK                           0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSRRESTORE_USF_SHFT                           0x1

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR0_ADDR                           (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000050)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR0_RMSK                                0x132
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR0_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR0_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR0_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR0_MSSSELFAUTH_BMSK                    0x100
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR0_MSSSELFAUTH_SHFT                      0x8
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR0_NSATTR_BMSK                          0x20
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR0_NSATTR_SHFT                           0x5
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR0_NSSTATE_BMSK                         0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR0_NSSTATE_SHFT                          0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR0_WNR_BMSK                              0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR0_WNR_SHFT                              0x1

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR1_ADDR                           (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000054)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR1_RMSK                            0x7070007
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR1_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR1_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR1_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR1_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR1_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR1_MSDINDEX_BMSK                   0x7000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR1_MSDINDEX_SHFT                        0x18
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR1_SSDINDEX_BMSK                     0x70000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR1_SSDINDEX_SHFT                        0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR1_STREAMINDEX_BMSK                      0x7
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR1_STREAMINDEX_SHFT                      0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR2_ADDR                           (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000058)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR2_RMSK                           0x3f1fffff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR2_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR2_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR2_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR2_ATID_BMSK                      0x3f000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR2_ATID_SHFT                            0x18
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR2_AVMID_BMSK                       0x1f0000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR2_AVMID_SHFT                           0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR2_ABID_BMSK                          0xe000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR2_ABID_SHFT                             0xd
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR2_APID_BMSK                          0x1f00
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR2_APID_SHFT                             0x8
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR2_AMID_BMSK                            0xff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SGFSYNDR2_AMID_SHFT                             0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTSCR0_ADDR                          (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000090)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTSCR0_RMSK                                 0x1
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTSCR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTSCR0_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTSCR0_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTSCR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTSCR0_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTSCR0_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTSCR0_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTSCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTSCR0_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTSCR0_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTSCR0_CLKONOFFE_BMSK                       0x1
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTSCR0_CLKONOFFE_SHFT                       0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_ADDR                                 (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000000)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_RMSK                                  0xff70ff5
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_WACFG_BMSK                            0xc000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_WACFG_SHFT                                 0x1a
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_RACFG_BMSK                            0x3000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_RACFG_SHFT                                 0x18
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_SHCFG_BMSK                             0xc00000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_SHCFG_SHFT                                 0x16
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_SMCFCFG_BMSK                           0x200000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_SMCFCFG_SHFT                               0x15
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_MTCFG_BMSK                             0x100000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_MTCFG_SHFT                                 0x14
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_MEMATTR_BMSK                            0x70000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_MEMATTR_SHFT                               0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_VMIDPNE_BMSK                              0x800
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_VMIDPNE_SHFT                                0xb
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_USFCFG_BMSK                               0x400
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_USFCFG_SHFT                                 0xa
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_GSE_BMSK                                  0x200
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_GSE_SHFT                                    0x9
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_STALLD_BMSK                               0x100
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_STALLD_SHFT                                 0x8
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_TRANSIENTCFG_BMSK                          0xc0
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_TRANSIENTCFG_SHFT                           0x6
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_GCFGFIE_BMSK                               0x20
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_GCFGFIE_SHFT                                0x5
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_GCFGERE_BMSK                               0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_GCFGERE_SHFT                                0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_GFIE_BMSK                                   0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_GFIE_SHFT                                   0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_CLIENTPD_BMSK                               0x1
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR0_CLIENTPD_SHFT                               0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR2_ADDR                                 (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000008)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR2_RMSK                                       0x1f
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR2_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR2_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR2_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR2_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR2_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR2_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR2_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR2_BPVMID_BMSK                                0x1f
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_CR2_BPVMID_SHFT                                 0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_ADDR                                 (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000010)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_RMSK                                 0x70000013
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_BPRCNSH_BMSK                         0x40000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_BPRCNSH_SHFT                               0x1e
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_BPRCISH_BMSK                         0x20000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_BPRCISH_SHFT                               0x1d
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_BPRCOSH_BMSK                         0x10000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_BPRCOSH_SHFT                               0x1c
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_BPREQPRIORITYCFG_BMSK                      0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_BPREQPRIORITYCFG_SHFT                       0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_BPREQPRIORITY_BMSK                          0x3
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_ACR_BPREQPRIORITY_SHFT                          0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR0_ADDR                                (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000020)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR0_RMSK                                 0x8001eff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR0_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR0_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR0_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR0_SMS_BMSK                             0x8000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR0_SMS_SHFT                                  0x1b
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR0_NUMSIDB_BMSK                            0x1e00
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR0_NUMSIDB_SHFT                               0x9
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR0_NUMSMRG_BMSK                              0xff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR0_NUMSMRG_SHFT                               0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR1_ADDR                                (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000024)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR1_RMSK                                    0x9f00
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR1_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR1_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR1_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR1_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR1_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR1_SMCD_BMSK                               0x8000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR1_SMCD_SHFT                                  0xf
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR1_SSDTP_BMSK                              0x1000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR1_SSDTP_SHFT                                 0xc
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR1_NUMSSDNDX_BMSK                           0xf00
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR1_NUMSSDNDX_SHFT                             0x8

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR2_ADDR                                (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000028)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR2_RMSK                                      0xff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR2_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR2_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR2_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR2_OAS_BMSK                                  0xf0
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR2_OAS_SHFT                                   0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR2_IAS_BMSK                                   0xf
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR2_IAS_SHFT                                   0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR4_ADDR                                (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000030)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR4_RMSK                                0xffffffff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR4_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR4_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR4_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR4_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR4_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR4_MAJOR_BMSK                          0xf0000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR4_MAJOR_SHFT                                0x1c
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR4_MINOR_BMSK                           0xfff0000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR4_MINOR_SHFT                                0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR4_STEP_BMSK                               0xffff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR4_STEP_SHFT                                  0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR5_ADDR                                (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000034)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR5_RMSK                                  0xff03ff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR5_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR5_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR5_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR5_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR5_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR5_NUMMSDRB_BMSK                         0xff0000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR5_NUMMSDRB_SHFT                             0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR5_MSAE_BMSK                                0x200
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR5_MSAE_SHFT                                  0x9
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR5_QRIBE_BMSK                               0x100
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR5_QRIBE_SHFT                                 0x8
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR5_NVMID_BMSK                                0xff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR5_NVMID_SHFT                                 0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR7_ADDR                                (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x0000003c)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR7_RMSK                                      0xff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR7_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR7_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR7_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR7_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR7_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR7_MAJOR_BMSK                                0xf0
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR7_MAJOR_SHFT                                 0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR7_MINOR_BMSK                                 0xf
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_IDR7_MINOR_SHFT                                 0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFAR0_ADDR                               (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000040)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFAR0_RMSK                               0xffffffff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFAR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFAR0_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFAR0_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFAR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFAR0_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFAR0_GFEA0_BMSK                         0xffffffff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFAR0_GFEA0_SHFT                                0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_ADDR                                (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000048)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_RMSK                                0xc00000a6
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_MULTI_CLIENT_BMSK                   0x80000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_MULTI_CLIENT_SHFT                         0x1f
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_MULTI_CFG_BMSK                      0x40000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_MULTI_CFG_SHFT                            0x1e
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_PF_BMSK                                   0x80
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_PF_SHFT                                    0x7
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_CAF_BMSK                                  0x20
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_CAF_SHFT                                   0x5
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_SMCF_BMSK                                  0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_SMCF_SHFT                                  0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_USF_BMSK                                   0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSR_USF_SHFT                                   0x1

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_ADDR                         (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x0000004c)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_RMSK                         0xc00000a6
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_MULTI_CLIENT_BMSK            0x80000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_MULTI_CLIENT_SHFT                  0x1f
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_MULTI_CFG_BMSK               0x40000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_MULTI_CFG_SHFT                     0x1e
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_PF_BMSK                            0x80
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_PF_SHFT                             0x7
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_CAF_BMSK                           0x20
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_CAF_SHFT                            0x5
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_SMCF_BMSK                           0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_SMCF_SHFT                           0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_USF_BMSK                            0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSRRESTORE_USF_SHFT                            0x1

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR0_ADDR                            (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000050)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR0_RMSK                                 0x132
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR0_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR0_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR0_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR0_MSSSELFAUTH_BMSK                     0x100
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR0_MSSSELFAUTH_SHFT                       0x8
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR0_NSATTR_BMSK                           0x20
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR0_NSATTR_SHFT                            0x5
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR0_NSSTATE_BMSK                          0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR0_NSSTATE_SHFT                           0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR0_WNR_BMSK                               0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR0_WNR_SHFT                               0x1

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR1_ADDR                            (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000054)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR1_RMSK                             0x7070007
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR1_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR1_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR1_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR1_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR1_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR1_MSDINDEX_BMSK                    0x7000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR1_MSDINDEX_SHFT                         0x18
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR1_SSDINDEX_BMSK                      0x70000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR1_SSDINDEX_SHFT                         0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR1_STREAMINDEX_BMSK                       0x7
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR1_STREAMINDEX_SHFT                       0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR2_ADDR                            (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000058)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR2_RMSK                            0x3f1fffff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR2_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR2_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR2_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR2_ATID_BMSK                       0x3f000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR2_ATID_SHFT                             0x18
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR2_AVMID_BMSK                        0x1f0000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR2_AVMID_SHFT                            0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR2_ABID_BMSK                           0xe000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR2_ABID_SHFT                              0xd
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR2_APID_BMSK                           0x1f00
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR2_APID_SHFT                              0x8
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR2_AMID_BMSK                             0xff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_GFSYNDR2_AMID_SHFT                              0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTCR0_ADDR                           (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000090)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTCR0_RMSK                                  0x1
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTCR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTCR0_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTCR0_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTCR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTCR0_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTCR0_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTCR0_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTCR0_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTCR0_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTCR0_CLKONOFFE_BMSK                        0x1
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTCR0_CLKONOFFE_SHFT                        0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTACR_ADDR                           (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x0000009c)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTACR_RMSK                           0xffffffff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTACR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTACR_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTACR_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTACR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTACR_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTACR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTACR_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTACR_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTACR_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTACR_RWE_BMSK                       0xffffffff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_VMIDMTACR_RWE_SHFT                              0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_ADDR                               (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000400)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_RMSK                                0xff70ff5
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_WACFG_BMSK                          0xc000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_WACFG_SHFT                               0x1a
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_RACFG_BMSK                          0x3000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_RACFG_SHFT                               0x18
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_SHCFG_BMSK                           0xc00000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_SHCFG_SHFT                               0x16
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_SMCFCFG_BMSK                         0x200000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_SMCFCFG_SHFT                             0x15
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_MTCFG_BMSK                           0x100000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_MTCFG_SHFT                               0x14
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_MEMATTR_BMSK                          0x70000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_MEMATTR_SHFT                             0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_VMIDPNE_BMSK                            0x800
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_VMIDPNE_SHFT                              0xb
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_USFCFG_BMSK                             0x400
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_USFCFG_SHFT                               0xa
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_GSE_BMSK                                0x200
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_GSE_SHFT                                  0x9
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_STALLD_BMSK                             0x100
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_STALLD_SHFT                               0x8
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_TRANSIENTCFG_BMSK                        0xc0
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_TRANSIENTCFG_SHFT                         0x6
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_GCFGFIE_BMSK                             0x20
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_GCFGFIE_SHFT                              0x5
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_GCFGERE_BMSK                             0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_GCFGERE_SHFT                              0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_GFIE_BMSK                                 0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_GFIE_SHFT                                 0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_CLIENTPD_BMSK                             0x1
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR0_CLIENTPD_SHFT                             0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR2_ADDR                               (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000408)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR2_RMSK                                     0x1f
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR2_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR2_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR2_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR2_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR2_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR2_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR2_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR2_BPVMID_BMSK                              0x1f
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSCR2_BPVMID_SHFT                               0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_ADDR                               (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000410)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_RMSK                               0x70000013
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_BPRCNSH_BMSK                       0x40000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_BPRCNSH_SHFT                             0x1e
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_BPRCISH_BMSK                       0x20000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_BPRCISH_SHFT                             0x1d
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_BPRCOSH_BMSK                       0x10000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_BPRCOSH_SHFT                             0x1c
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_BPREQPRIORITYCFG_BMSK                    0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_BPREQPRIORITYCFG_SHFT                     0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_BPREQPRIORITY_BMSK                        0x3
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSACR_BPREQPRIORITY_SHFT                        0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFAR0_ADDR                             (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000440)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFAR0_RMSK                             0xffffffff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFAR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFAR0_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFAR0_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFAR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFAR0_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFAR0_GFEA0_BMSK                       0xffffffff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFAR0_GFEA0_SHFT                              0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_ADDR                              (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000448)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_RMSK                              0xc00000a6
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_MULTI_CLIENT_BMSK                 0x80000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_MULTI_CLIENT_SHFT                       0x1f
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_MULTI_CFG_BMSK                    0x40000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_MULTI_CFG_SHFT                          0x1e
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_PF_BMSK                                 0x80
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_PF_SHFT                                  0x7
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_CAF_BMSK                                0x20
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_CAF_SHFT                                 0x5
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_SMCF_BMSK                                0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_SMCF_SHFT                                0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_USF_BMSK                                 0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSR_USF_SHFT                                 0x1

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_ADDR                       (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x0000044c)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_RMSK                       0xc00000a6
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_MULTI_CLIENT_BMSK          0x80000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_MULTI_CLIENT_SHFT                0x1f
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_MULTI_CFG_BMSK             0x40000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_MULTI_CFG_SHFT                   0x1e
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_PF_BMSK                          0x80
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_PF_SHFT                           0x7
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_CAF_BMSK                         0x20
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_CAF_SHFT                          0x5
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_SMCF_BMSK                         0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_SMCF_SHFT                         0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_USF_BMSK                          0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSRRESTORE_USF_SHFT                          0x1

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR0_ADDR                          (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000450)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR0_RMSK                               0x132
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR0_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR0_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR0_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR0_MSSSELFAUTH_BMSK                   0x100
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR0_MSSSELFAUTH_SHFT                     0x8
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR0_NSATTR_BMSK                         0x20
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR0_NSATTR_SHFT                          0x5
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR0_NSSTATE_BMSK                        0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR0_NSSTATE_SHFT                         0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR0_WNR_BMSK                             0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR0_WNR_SHFT                             0x1

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR1_ADDR                          (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000454)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR1_RMSK                           0x7070007
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR1_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR1_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR1_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR1_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR1_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR1_MSDINDEX_BMSK                  0x7000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR1_MSDINDEX_SHFT                       0x18
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR1_SSDINDEX_BMSK                    0x70000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR1_SSDINDEX_SHFT                       0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR1_STREAMINDEX_BMSK                     0x7
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR1_STREAMINDEX_SHFT                     0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR2_ADDR                          (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000458)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR2_RMSK                          0x3f1fffff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR2_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR2_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR2_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR2_ATID_BMSK                     0x3f000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR2_ATID_SHFT                           0x18
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR2_AVMID_BMSK                      0x1f0000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR2_AVMID_SHFT                          0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR2_ABID_BMSK                         0xe000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR2_ABID_SHFT                            0xd
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR2_APID_BMSK                         0x1f00
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR2_APID_SHFT                            0x8
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR2_AMID_BMSK                           0xff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSGFSYNDR2_AMID_SHFT                            0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSVMIDMTCR0_ADDR                         (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000490)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSVMIDMTCR0_RMSK                                0x1
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSVMIDMTCR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSVMIDMTCR0_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSVMIDMTCR0_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSVMIDMTCR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSVMIDMTCR0_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSVMIDMTCR0_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSVMIDMTCR0_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSVMIDMTCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSVMIDMTCR0_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSVMIDMTCR0_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSVMIDMTCR0_CLKONOFFE_BMSK                      0x1
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_NSVMIDMTCR0_CLKONOFFE_SHFT                      0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SSDR0_ADDR                               (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000080)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SSDR0_RMSK                                     0xff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SSDR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SSDR0_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SSDR0_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SSDR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SSDR0_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SSDR0_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SSDR0_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SSDR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SSDR0_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SSDR0_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SSDR0_RWE_BMSK                                 0xff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SSDR0_RWE_SHFT                                  0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MSDR0_ADDR                               (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000480)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MSDR0_RMSK                                     0xff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MSDR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MSDR0_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MSDR0_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MSDR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MSDR0_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MSDR0_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MSDR0_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MSDR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MSDR0_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MSDR0_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MSDR0_RWE_BMSK                                 0xff
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MSDR0_RWE_SHFT                                  0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MCR_ADDR                                 (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000494)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MCR_RMSK                                        0x7
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MCR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MCR_ADDR, HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MCR_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MCR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MCR_ADDR, m)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MCR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MCR_ADDR,v)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MCR_ADDR,m,v,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MCR_IN)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MCR_CLKONOFFE_BMSK                              0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MCR_CLKONOFFE_SHFT                              0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MCR_BPMSACFG_BMSK                               0x2
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MCR_BPMSACFG_SHFT                               0x1
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MCR_BPSMSACFG_BMSK                              0x1
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_MCR_BPSMSACFG_SHFT                              0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_ADDR(n)                            (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000c00 + 0x4 * (n))
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_RMSK                               0x30ff7b1f
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_MAXn                                        7
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_INI(n)        \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_ADDR(n), HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_INMI(n,mask)    \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_ADDR(n), mask)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_OUTI(n,val)    \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_ADDR(n),val)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_ADDR(n),mask,val,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_INI(n))
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_TRANSIENTCFG_BMSK                  0x30000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_TRANSIENTCFG_SHFT                        0x1c
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_WACFG_BMSK                           0xc00000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_WACFG_SHFT                               0x16
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_RACFG_BMSK                           0x300000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_RACFG_SHFT                               0x14
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_NSCFG_BMSK                            0xc0000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_NSCFG_SHFT                               0x12
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_TYPE_BMSK                             0x30000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_TYPE_SHFT                                0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_MEMATTR_BMSK                           0x7000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_MEMATTR_SHFT                              0xc
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_MTCFG_BMSK                              0x800
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_MTCFG_SHFT                                0xb
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_SHCFG_BMSK                              0x300
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_SHCFG_SHFT                                0x8
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_VMID_BMSK                                0x1f
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_S2VRn_VMID_SHFT                                 0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_ADDR(n)                           (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000e00 + 0x4 * (n))
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_RMSK                              0x70000013
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_MAXn                                       7
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_INI(n)        \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_ADDR(n), HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_INMI(n,mask)    \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_ADDR(n), mask)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_OUTI(n,val)    \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_ADDR(n),val)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_ADDR(n),mask,val,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_INI(n))
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_RCNSH_BMSK                        0x40000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_RCNSH_SHFT                              0x1e
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_RCISH_BMSK                        0x20000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_RCISH_SHFT                              0x1d
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_RCOSH_BMSK                        0x10000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_RCOSH_SHFT                              0x1c
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_REQPRIORITYCFG_BMSK                     0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_REQPRIORITYCFG_SHFT                      0x4
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_REQPRIORITY_BMSK                         0x3
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_AS2VRn_REQPRIORITY_SHFT                         0x0

#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SMRn_ADDR(n)                             (PERIPH_SS_QPIC_QPIC_VMIDMT_REG_BASE      + 0x00000800 + 0x4 * (n))
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SMRn_RMSK                                0x80070007
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SMRn_MAXn                                         7
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SMRn_INI(n)        \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SMRn_ADDR(n), HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SMRn_RMSK)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SMRn_INMI(n,mask)    \
        in_dword_masked(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SMRn_ADDR(n), mask)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SMRn_OUTI(n,val)    \
        out_dword(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SMRn_ADDR(n),val)
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SMRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SMRn_ADDR(n),mask,val,HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SMRn_INI(n))
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SMRn_VALID_BMSK                          0x80000000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SMRn_VALID_SHFT                                0x1f
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SMRn_MASK_BMSK                              0x70000
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SMRn_MASK_SHFT                                 0x10
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SMRn_ID_BMSK                                    0x7
#define HWIO_PERIPH_SS_QPIC_QPIC_VMIDMT_SMRn_ID_SHFT                                    0x0

/*----------------------------------------------------------------------------
 * MODULE: PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT
 *--------------------------------------------------------------------------*/

#define PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE                                      (COPSS_ELAN_BASE      + 0x00100000)
#define PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS                                 (COPSS_ELAN_BASE_PHYS + 0x00100000)
#define PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS                                 0x00100000

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_ADDR                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000000)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_PHYS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000000)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_OFFS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000000)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_RMSK                                0x3ff707f5
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_NSCFG_BMSK                          0x30000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_NSCFG_SHFT                                0x1c
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_WACFG_BMSK                           0xc000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_WACFG_SHFT                                0x1a
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_RACFG_BMSK                           0x3000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_RACFG_SHFT                                0x18
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_SHCFG_BMSK                            0xc00000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_SHCFG_SHFT                                0x16
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_SMCFCFG_BMSK                          0x200000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_SMCFCFG_SHFT                              0x15
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_MTCFG_BMSK                            0x100000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_MTCFG_SHFT                                0x14
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_MEMATTR_BMSK                           0x70000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_MEMATTR_SHFT                              0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_USFCFG_BMSK                              0x400
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_USFCFG_SHFT                                0xa
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_GSE_BMSK                                 0x200
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_GSE_SHFT                                   0x9
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_STALLD_BMSK                              0x100
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_STALLD_SHFT                                0x8
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_TRANSIENTCFG_BMSK                         0xc0
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_TRANSIENTCFG_SHFT                          0x6
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_GCFGFIE_BMSK                              0x20
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_GCFGFIE_SHFT                               0x5
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_GCFGERE_BMSK                              0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_GCFGERE_SHFT                               0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_GFIE_BMSK                                  0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_GFIE_SHFT                                  0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_CLIENTPD_BMSK                              0x1
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR0_CLIENTPD_SHFT                              0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR1_ADDR                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000004)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR1_PHYS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000004)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR1_OFFS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000004)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR1_RMSK                                 0x1001f00
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR1_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR1_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR1_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR1_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR1_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR1_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR1_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR1_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR1_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR1_GASRAE_BMSK                          0x1000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR1_GASRAE_SHFT                               0x18
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR1_NSNUMSMRGO_BMSK                         0x1f00
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR1_NSNUMSMRGO_SHFT                            0x8

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR2_ADDR                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000008)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR2_PHYS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000008)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR2_OFFS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000008)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR2_RMSK                                      0x1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR2_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR2_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR2_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR2_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR2_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR2_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR2_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR2_BPVMID_BMSK                               0x1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SCR2_BPVMID_SHFT                                0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_ADDR                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000010)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_PHYS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000010)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_OFFS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000010)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_RMSK                                0x70000013
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_BPRCNSH_BMSK                        0x40000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_BPRCNSH_SHFT                              0x1e
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_BPRCISH_BMSK                        0x20000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_BPRCISH_SHFT                              0x1d
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_BPRCOSH_BMSK                        0x10000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_BPRCOSH_SHFT                              0x1c
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_BPREQPRIORITYCFG_BMSK                     0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_BPREQPRIORITYCFG_SHFT                      0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_BPREQPRIORITY_BMSK                         0x3
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SACR_BPREQPRIORITY_SHFT                         0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR0_ADDR                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000020)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR0_PHYS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000020)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR0_OFFS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000020)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR0_RMSK                               0x88001eff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR0_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR0_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR0_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR0_SES_BMSK                           0x80000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR0_SES_SHFT                                 0x1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR0_SMS_BMSK                            0x8000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR0_SMS_SHFT                                 0x1b
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR0_NUMSIDB_BMSK                           0x1e00
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR0_NUMSIDB_SHFT                              0x9
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR0_NUMSMRG_BMSK                             0xff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR0_NUMSMRG_SHFT                              0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR1_ADDR                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000024)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR1_PHYS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000024)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR1_OFFS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000024)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR1_RMSK                                   0x9f00
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR1_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR1_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR1_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR1_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR1_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR1_SMCD_BMSK                              0x8000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR1_SMCD_SHFT                                 0xf
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR1_SSDTP_BMSK                             0x1000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR1_SSDTP_SHFT                                0xc
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR1_NUMSSDNDX_BMSK                          0xf00
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR1_NUMSSDNDX_SHFT                            0x8

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR2_ADDR                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000028)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR2_PHYS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000028)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR2_OFFS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000028)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR2_RMSK                                     0xff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR2_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR2_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR2_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR2_OAS_BMSK                                 0xf0
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR2_OAS_SHFT                                  0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR2_IAS_BMSK                                  0xf
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR2_IAS_SHFT                                  0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR4_ADDR                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000030)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR4_PHYS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000030)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR4_OFFS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000030)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR4_RMSK                               0xffffffff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR4_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR4_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR4_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR4_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR4_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR4_MAJOR_BMSK                         0xf0000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR4_MAJOR_SHFT                               0x1c
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR4_MINOR_BMSK                          0xfff0000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR4_MINOR_SHFT                               0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR4_STEP_BMSK                              0xffff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR4_STEP_SHFT                                 0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR5_ADDR                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000034)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR5_PHYS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000034)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR5_OFFS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000034)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR5_RMSK                                 0xff03ff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR5_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR5_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR5_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR5_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR5_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR5_NUMMSDRB_BMSK                        0xff0000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR5_NUMMSDRB_SHFT                            0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR5_MSAE_BMSK                               0x200
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR5_MSAE_SHFT                                 0x9
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR5_QRIBE_BMSK                              0x100
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR5_QRIBE_SHFT                                0x8
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR5_NVMID_BMSK                               0xff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR5_NVMID_SHFT                                0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR7_ADDR                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x0000003c)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR7_PHYS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x0000003c)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR7_OFFS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x0000003c)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR7_RMSK                                     0xff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR7_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR7_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR7_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR7_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR7_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR7_MAJOR_BMSK                               0xf0
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR7_MAJOR_SHFT                                0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR7_MINOR_BMSK                                0xf
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SIDR7_MINOR_SHFT                                0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFAR0_ADDR                              (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000040)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFAR0_PHYS                              (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000040)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFAR0_OFFS                              (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000040)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFAR0_RMSK                              0xffffffff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFAR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFAR0_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFAR0_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFAR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFAR0_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFAR0_SGFEA0_BMSK                       0xffffffff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFAR0_SGFEA0_SHFT                              0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_ADDR                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000048)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_PHYS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000048)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_OFFS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000048)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_RMSK                               0xc0000026
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_MULTI_CLIENT_BMSK                  0x80000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_MULTI_CLIENT_SHFT                        0x1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_MULTI_CFG_BMSK                     0x40000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_MULTI_CFG_SHFT                           0x1e
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_CAF_BMSK                                 0x20
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_CAF_SHFT                                  0x5
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_SMCF_BMSK                                 0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_SMCF_SHFT                                 0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_USF_BMSK                                  0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSR_USF_SHFT                                  0x1

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_ADDR                        (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x0000004c)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_PHYS                        (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x0000004c)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_OFFS                        (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x0000004c)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_RMSK                        0xc0000026
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_MULTI_CLIENT_BMSK           0x80000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_MULTI_CLIENT_SHFT                 0x1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_MULTI_CFG_BMSK              0x40000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_MULTI_CFG_SHFT                    0x1e
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_CAF_BMSK                          0x20
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_CAF_SHFT                           0x5
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_SMCF_BMSK                          0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_SMCF_SHFT                          0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_USF_BMSK                           0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSRRESTORE_USF_SHFT                           0x1

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR0_ADDR                           (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000050)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR0_PHYS                           (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000050)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR0_OFFS                           (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000050)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR0_RMSK                                0x132
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR0_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR0_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR0_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR0_MSSSELFAUTH_BMSK                    0x100
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR0_MSSSELFAUTH_SHFT                      0x8
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR0_NSATTR_BMSK                          0x20
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR0_NSATTR_SHFT                           0x5
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR0_NSSTATE_BMSK                         0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR0_NSSTATE_SHFT                          0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR0_WNR_BMSK                              0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR0_WNR_SHFT                              0x1

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR1_ADDR                           (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000054)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR1_PHYS                           (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000054)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR1_OFFS                           (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000054)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR1_RMSK                           0x1f1f001f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR1_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR1_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR1_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR1_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR1_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR1_MSDINDEX_BMSK                  0x1f000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR1_MSDINDEX_SHFT                        0x18
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR1_SSDINDEX_BMSK                    0x1f0000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR1_SSDINDEX_SHFT                        0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR1_STREAMINDEX_BMSK                     0x1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR1_STREAMINDEX_SHFT                      0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR2_ADDR                           (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000058)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR2_PHYS                           (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000058)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR2_OFFS                           (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000058)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR2_RMSK                           0x3f1fffff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR2_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR2_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR2_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR2_ATID_BMSK                      0x3f000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR2_ATID_SHFT                            0x18
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR2_AVMID_BMSK                       0x1f0000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR2_AVMID_SHFT                           0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR2_ABID_BMSK                          0xe000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR2_ABID_SHFT                             0xd
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR2_APID_BMSK                          0x1f00
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR2_APID_SHFT                             0x8
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR2_AMID_BMSK                            0xff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SGFSYNDR2_AMID_SHFT                             0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTSCR0_ADDR                          (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000090)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTSCR0_PHYS                          (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000090)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTSCR0_OFFS                          (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000090)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTSCR0_RMSK                                 0x1
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTSCR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTSCR0_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTSCR0_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTSCR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTSCR0_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTSCR0_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTSCR0_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTSCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTSCR0_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTSCR0_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTSCR0_CLKONOFFE_BMSK                       0x1
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTSCR0_CLKONOFFE_SHFT                       0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_ADDR                                 (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000000)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_PHYS                                 (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000000)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_OFFS                                 (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000000)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_RMSK                                  0xff70ff5
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_WACFG_BMSK                            0xc000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_WACFG_SHFT                                 0x1a
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_RACFG_BMSK                            0x3000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_RACFG_SHFT                                 0x18
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_SHCFG_BMSK                             0xc00000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_SHCFG_SHFT                                 0x16
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_SMCFCFG_BMSK                           0x200000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_SMCFCFG_SHFT                               0x15
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_MTCFG_BMSK                             0x100000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_MTCFG_SHFT                                 0x14
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_MEMATTR_BMSK                            0x70000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_MEMATTR_SHFT                               0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_VMIDPNE_BMSK                              0x800
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_VMIDPNE_SHFT                                0xb
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_USFCFG_BMSK                               0x400
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_USFCFG_SHFT                                 0xa
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_GSE_BMSK                                  0x200
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_GSE_SHFT                                    0x9
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_STALLD_BMSK                               0x100
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_STALLD_SHFT                                 0x8
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_TRANSIENTCFG_BMSK                          0xc0
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_TRANSIENTCFG_SHFT                           0x6
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_GCFGFIE_BMSK                               0x20
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_GCFGFIE_SHFT                                0x5
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_GCFGERE_BMSK                               0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_GCFGERE_SHFT                                0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_GFIE_BMSK                                   0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_GFIE_SHFT                                   0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_CLIENTPD_BMSK                               0x1
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR0_CLIENTPD_SHFT                               0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR2_ADDR                                 (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000008)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR2_PHYS                                 (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000008)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR2_OFFS                                 (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000008)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR2_RMSK                                       0x1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR2_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR2_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR2_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR2_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR2_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR2_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR2_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR2_BPVMID_BMSK                                0x1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_CR2_BPVMID_SHFT                                 0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_ADDR                                 (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000010)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_PHYS                                 (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000010)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_OFFS                                 (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000010)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_RMSK                                 0x70000013
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_BPRCNSH_BMSK                         0x40000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_BPRCNSH_SHFT                               0x1e
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_BPRCISH_BMSK                         0x20000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_BPRCISH_SHFT                               0x1d
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_BPRCOSH_BMSK                         0x10000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_BPRCOSH_SHFT                               0x1c
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_BPREQPRIORITYCFG_BMSK                      0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_BPREQPRIORITYCFG_SHFT                       0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_BPREQPRIORITY_BMSK                          0x3
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_ACR_BPREQPRIORITY_SHFT                          0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR0_ADDR                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000020)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR0_PHYS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000020)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR0_OFFS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000020)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR0_RMSK                                 0x8001eff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR0_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR0_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR0_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR0_SMS_BMSK                             0x8000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR0_SMS_SHFT                                  0x1b
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR0_NUMSIDB_BMSK                            0x1e00
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR0_NUMSIDB_SHFT                               0x9
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR0_NUMSMRG_BMSK                              0xff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR0_NUMSMRG_SHFT                               0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR1_ADDR                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000024)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR1_PHYS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000024)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR1_OFFS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000024)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR1_RMSK                                    0x9f00
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR1_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR1_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR1_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR1_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR1_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR1_SMCD_BMSK                               0x8000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR1_SMCD_SHFT                                  0xf
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR1_SSDTP_BMSK                              0x1000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR1_SSDTP_SHFT                                 0xc
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR1_NUMSSDNDX_BMSK                           0xf00
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR1_NUMSSDNDX_SHFT                             0x8

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR2_ADDR                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000028)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR2_PHYS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000028)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR2_OFFS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000028)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR2_RMSK                                      0xff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR2_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR2_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR2_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR2_OAS_BMSK                                  0xf0
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR2_OAS_SHFT                                   0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR2_IAS_BMSK                                   0xf
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR2_IAS_SHFT                                   0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR4_ADDR                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000030)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR4_PHYS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000030)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR4_OFFS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000030)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR4_RMSK                                0xffffffff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR4_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR4_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR4_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR4_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR4_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR4_MAJOR_BMSK                          0xf0000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR4_MAJOR_SHFT                                0x1c
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR4_MINOR_BMSK                           0xfff0000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR4_MINOR_SHFT                                0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR4_STEP_BMSK                               0xffff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR4_STEP_SHFT                                  0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR5_ADDR                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000034)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR5_PHYS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000034)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR5_OFFS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000034)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR5_RMSK                                  0xff03ff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR5_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR5_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR5_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR5_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR5_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR5_NUMMSDRB_BMSK                         0xff0000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR5_NUMMSDRB_SHFT                             0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR5_MSAE_BMSK                                0x200
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR5_MSAE_SHFT                                  0x9
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR5_QRIBE_BMSK                               0x100
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR5_QRIBE_SHFT                                 0x8
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR5_NVMID_BMSK                                0xff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR5_NVMID_SHFT                                 0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR7_ADDR                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x0000003c)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR7_PHYS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x0000003c)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR7_OFFS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x0000003c)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR7_RMSK                                      0xff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR7_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR7_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR7_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR7_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR7_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR7_MAJOR_BMSK                                0xf0
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR7_MAJOR_SHFT                                 0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR7_MINOR_BMSK                                 0xf
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_IDR7_MINOR_SHFT                                 0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFAR0_ADDR                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000040)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFAR0_PHYS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000040)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFAR0_OFFS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000040)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFAR0_RMSK                               0xffffffff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFAR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFAR0_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFAR0_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFAR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFAR0_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFAR0_GFEA0_BMSK                         0xffffffff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFAR0_GFEA0_SHFT                                0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_ADDR                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000048)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_PHYS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000048)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_OFFS                                (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000048)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_RMSK                                0xc00000a6
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_MULTI_CLIENT_BMSK                   0x80000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_MULTI_CLIENT_SHFT                         0x1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_MULTI_CFG_BMSK                      0x40000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_MULTI_CFG_SHFT                            0x1e
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_PF_BMSK                                   0x80
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_PF_SHFT                                    0x7
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_CAF_BMSK                                  0x20
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_CAF_SHFT                                   0x5
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_SMCF_BMSK                                  0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_SMCF_SHFT                                  0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_USF_BMSK                                   0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSR_USF_SHFT                                   0x1

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_ADDR                         (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x0000004c)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_PHYS                         (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x0000004c)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_OFFS                         (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x0000004c)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_RMSK                         0xc00000a6
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_MULTI_CLIENT_BMSK            0x80000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_MULTI_CLIENT_SHFT                  0x1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_MULTI_CFG_BMSK               0x40000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_MULTI_CFG_SHFT                     0x1e
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_PF_BMSK                            0x80
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_PF_SHFT                             0x7
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_CAF_BMSK                           0x20
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_CAF_SHFT                            0x5
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_SMCF_BMSK                           0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_SMCF_SHFT                           0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_USF_BMSK                            0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSRRESTORE_USF_SHFT                            0x1

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR0_ADDR                            (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000050)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR0_PHYS                            (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000050)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR0_OFFS                            (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000050)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR0_RMSK                                 0x132
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR0_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR0_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR0_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR0_MSSSELFAUTH_BMSK                     0x100
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR0_MSSSELFAUTH_SHFT                       0x8
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR0_NSATTR_BMSK                           0x20
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR0_NSATTR_SHFT                            0x5
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR0_NSSTATE_BMSK                          0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR0_NSSTATE_SHFT                           0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR0_WNR_BMSK                               0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR0_WNR_SHFT                               0x1

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR1_ADDR                            (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000054)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR1_PHYS                            (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000054)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR1_OFFS                            (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000054)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR1_RMSK                            0x1f1f001f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR1_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR1_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR1_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR1_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR1_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR1_MSDINDEX_BMSK                   0x1f000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR1_MSDINDEX_SHFT                         0x18
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR1_SSDINDEX_BMSK                     0x1f0000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR1_SSDINDEX_SHFT                         0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR1_STREAMINDEX_BMSK                      0x1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR1_STREAMINDEX_SHFT                       0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR2_ADDR                            (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000058)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR2_PHYS                            (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000058)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR2_OFFS                            (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000058)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR2_RMSK                            0x3f1fffff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR2_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR2_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR2_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR2_ATID_BMSK                       0x3f000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR2_ATID_SHFT                             0x18
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR2_AVMID_BMSK                        0x1f0000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR2_AVMID_SHFT                            0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR2_ABID_BMSK                           0xe000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR2_ABID_SHFT                              0xd
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR2_APID_BMSK                           0x1f00
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR2_APID_SHFT                              0x8
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR2_AMID_BMSK                             0xff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_GFSYNDR2_AMID_SHFT                              0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTCR0_ADDR                           (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000090)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTCR0_PHYS                           (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000090)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTCR0_OFFS                           (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000090)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTCR0_RMSK                                  0x1
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTCR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTCR0_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTCR0_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTCR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTCR0_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTCR0_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTCR0_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTCR0_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTCR0_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTCR0_CLKONOFFE_BMSK                        0x1
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTCR0_CLKONOFFE_SHFT                        0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTACR_ADDR                           (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x0000009c)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTACR_PHYS                           (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x0000009c)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTACR_OFFS                           (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x0000009c)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTACR_RMSK                           0xffffffff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTACR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTACR_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTACR_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTACR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTACR_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTACR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTACR_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTACR_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTACR_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTACR_RWE_BMSK                       0xffffffff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_VMIDMTACR_RWE_SHFT                              0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_ADDR                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000400)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_PHYS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000400)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_OFFS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000400)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_RMSK                                0xff70ff5
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_WACFG_BMSK                          0xc000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_WACFG_SHFT                               0x1a
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_RACFG_BMSK                          0x3000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_RACFG_SHFT                               0x18
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_SHCFG_BMSK                           0xc00000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_SHCFG_SHFT                               0x16
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_SMCFCFG_BMSK                         0x200000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_SMCFCFG_SHFT                             0x15
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_MTCFG_BMSK                           0x100000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_MTCFG_SHFT                               0x14
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_MEMATTR_BMSK                          0x70000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_MEMATTR_SHFT                             0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_VMIDPNE_BMSK                            0x800
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_VMIDPNE_SHFT                              0xb
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_USFCFG_BMSK                             0x400
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_USFCFG_SHFT                               0xa
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_GSE_BMSK                                0x200
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_GSE_SHFT                                  0x9
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_STALLD_BMSK                             0x100
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_STALLD_SHFT                               0x8
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_TRANSIENTCFG_BMSK                        0xc0
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_TRANSIENTCFG_SHFT                         0x6
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_GCFGFIE_BMSK                             0x20
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_GCFGFIE_SHFT                              0x5
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_GCFGERE_BMSK                             0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_GCFGERE_SHFT                              0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_GFIE_BMSK                                 0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_GFIE_SHFT                                 0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_CLIENTPD_BMSK                             0x1
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR0_CLIENTPD_SHFT                             0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR2_ADDR                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000408)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR2_PHYS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000408)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR2_OFFS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000408)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR2_RMSK                                     0x1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR2_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR2_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR2_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR2_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR2_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR2_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR2_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR2_BPVMID_BMSK                              0x1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSCR2_BPVMID_SHFT                               0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_ADDR                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000410)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_PHYS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000410)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_OFFS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000410)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_RMSK                               0x70000013
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_BPRCNSH_BMSK                       0x40000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_BPRCNSH_SHFT                             0x1e
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_BPRCISH_BMSK                       0x20000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_BPRCISH_SHFT                             0x1d
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_BPRCOSH_BMSK                       0x10000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_BPRCOSH_SHFT                             0x1c
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_BPREQPRIORITYCFG_BMSK                    0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_BPREQPRIORITYCFG_SHFT                     0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_BPREQPRIORITY_BMSK                        0x3
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSACR_BPREQPRIORITY_SHFT                        0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFAR0_ADDR                             (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000440)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFAR0_PHYS                             (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000440)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFAR0_OFFS                             (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000440)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFAR0_RMSK                             0xffffffff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFAR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFAR0_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFAR0_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFAR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFAR0_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFAR0_GFEA0_BMSK                       0xffffffff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFAR0_GFEA0_SHFT                              0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_ADDR                              (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000448)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_PHYS                              (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000448)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_OFFS                              (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000448)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_RMSK                              0xc00000a6
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_MULTI_CLIENT_BMSK                 0x80000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_MULTI_CLIENT_SHFT                       0x1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_MULTI_CFG_BMSK                    0x40000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_MULTI_CFG_SHFT                          0x1e
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_PF_BMSK                                 0x80
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_PF_SHFT                                  0x7
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_CAF_BMSK                                0x20
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_CAF_SHFT                                 0x5
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_SMCF_BMSK                                0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_SMCF_SHFT                                0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_USF_BMSK                                 0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSR_USF_SHFT                                 0x1

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_ADDR                       (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x0000044c)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_PHYS                       (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x0000044c)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_OFFS                       (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x0000044c)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_RMSK                       0xc00000a6
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_MULTI_CLIENT_BMSK          0x80000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_MULTI_CLIENT_SHFT                0x1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_MULTI_CFG_BMSK             0x40000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_MULTI_CFG_SHFT                   0x1e
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_PF_BMSK                          0x80
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_PF_SHFT                           0x7
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_CAF_BMSK                         0x20
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_CAF_SHFT                          0x5
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_SMCF_BMSK                         0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_SMCF_SHFT                         0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_USF_BMSK                          0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSRRESTORE_USF_SHFT                          0x1

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR0_ADDR                          (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000450)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR0_PHYS                          (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000450)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR0_OFFS                          (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000450)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR0_RMSK                               0x132
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR0_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR0_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR0_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR0_MSSSELFAUTH_BMSK                   0x100
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR0_MSSSELFAUTH_SHFT                     0x8
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR0_NSATTR_BMSK                         0x20
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR0_NSATTR_SHFT                          0x5
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR0_NSSTATE_BMSK                        0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR0_NSSTATE_SHFT                         0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR0_WNR_BMSK                             0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR0_WNR_SHFT                             0x1

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR1_ADDR                          (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000454)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR1_PHYS                          (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000454)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR1_OFFS                          (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000454)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR1_RMSK                          0x1f1f001f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR1_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR1_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR1_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR1_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR1_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR1_MSDINDEX_BMSK                 0x1f000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR1_MSDINDEX_SHFT                       0x18
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR1_SSDINDEX_BMSK                   0x1f0000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR1_SSDINDEX_SHFT                       0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR1_STREAMINDEX_BMSK                    0x1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR1_STREAMINDEX_SHFT                     0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR2_ADDR                          (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000458)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR2_PHYS                          (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000458)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR2_OFFS                          (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000458)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR2_RMSK                          0x3f1fffff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR2_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR2_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR2_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR2_ATID_BMSK                     0x3f000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR2_ATID_SHFT                           0x18
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR2_AVMID_BMSK                      0x1f0000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR2_AVMID_SHFT                          0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR2_ABID_BMSK                         0xe000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR2_ABID_SHFT                            0xd
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR2_APID_BMSK                         0x1f00
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR2_APID_SHFT                            0x8
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR2_AMID_BMSK                           0xff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSGFSYNDR2_AMID_SHFT                            0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSVMIDMTCR0_ADDR                         (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000490)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSVMIDMTCR0_PHYS                         (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000490)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSVMIDMTCR0_OFFS                         (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000490)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSVMIDMTCR0_RMSK                                0x1
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSVMIDMTCR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSVMIDMTCR0_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSVMIDMTCR0_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSVMIDMTCR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSVMIDMTCR0_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSVMIDMTCR0_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSVMIDMTCR0_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSVMIDMTCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSVMIDMTCR0_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSVMIDMTCR0_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSVMIDMTCR0_CLKONOFFE_BMSK                      0x1
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_NSVMIDMTCR0_CLKONOFFE_SHFT                      0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SSDR0_ADDR                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000080)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SSDR0_PHYS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000080)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SSDR0_OFFS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000080)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SSDR0_RMSK                               0xffffffff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SSDR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SSDR0_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SSDR0_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SSDR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SSDR0_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SSDR0_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SSDR0_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SSDR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SSDR0_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SSDR0_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SSDR0_RWE_BMSK                           0xffffffff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SSDR0_RWE_SHFT                                  0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MSDR0_ADDR                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000480)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MSDR0_PHYS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000480)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MSDR0_OFFS                               (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000480)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MSDR0_RMSK                                0x1ffffff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MSDR0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MSDR0_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MSDR0_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MSDR0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MSDR0_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MSDR0_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MSDR0_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MSDR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MSDR0_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MSDR0_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MSDR0_RWE_BMSK                            0x1ffffff
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MSDR0_RWE_SHFT                                  0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_ADDR                                 (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000494)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_PHYS                                 (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000494)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_OFFS                                 (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000494)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_RMSK                                        0x7
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_ADDR, HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_ADDR, m)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_ADDR,v)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_ADDR,m,v,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_IN)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_CLKONOFFE_BMSK                              0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_CLKONOFFE_SHFT                              0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_BPMSACFG_BMSK                               0x2
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_BPMSACFG_SHFT                               0x1
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_BPSMSACFG_BMSK                              0x1
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_MCR_BPSMSACFG_SHFT                              0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_ADDR(n)                            (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000c00 + 0x4 * (n))
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_PHYS(n)                            (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000c00 + 0x4 * (n))
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_OFFS(n)                            (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000c00 + 0x4 * (n))
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_RMSK                               0x30ff7b1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_MAXn                                       24
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_INI(n)        \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_ADDR(n), HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_INMI(n,mask)    \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_ADDR(n), mask)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_OUTI(n,val)    \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_ADDR(n),val)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_ADDR(n),mask,val,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_INI(n))
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_TRANSIENTCFG_BMSK                  0x30000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_TRANSIENTCFG_SHFT                        0x1c
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_WACFG_BMSK                           0xc00000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_WACFG_SHFT                               0x16
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_RACFG_BMSK                           0x300000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_RACFG_SHFT                               0x14
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_NSCFG_BMSK                            0xc0000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_NSCFG_SHFT                               0x12
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_TYPE_BMSK                             0x30000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_TYPE_SHFT                                0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_MEMATTR_BMSK                           0x7000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_MEMATTR_SHFT                              0xc
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_MTCFG_BMSK                              0x800
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_MTCFG_SHFT                                0xb
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_SHCFG_BMSK                              0x300
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_SHCFG_SHFT                                0x8
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_VMID_BMSK                                0x1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_S2VRn_VMID_SHFT                                 0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_ADDR(n)                           (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000e00 + 0x4 * (n))
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_PHYS(n)                           (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000e00 + 0x4 * (n))
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_OFFS(n)                           (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000e00 + 0x4 * (n))
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_RMSK                              0x70000013
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_MAXn                                      24
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_INI(n)        \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_ADDR(n), HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_INMI(n,mask)    \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_ADDR(n), mask)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_OUTI(n,val)    \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_ADDR(n),val)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_ADDR(n),mask,val,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_INI(n))
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_RCNSH_BMSK                        0x40000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_RCNSH_SHFT                              0x1e
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_RCISH_BMSK                        0x20000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_RCISH_SHFT                              0x1d
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_RCOSH_BMSK                        0x10000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_RCOSH_SHFT                              0x1c
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_REQPRIORITYCFG_BMSK                     0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_REQPRIORITYCFG_SHFT                      0x4
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_REQPRIORITY_BMSK                         0x3
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_AS2VRn_REQPRIORITY_SHFT                         0x0

#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_ADDR(n)                             (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE      + 0x00000800 + 0x4 * (n))
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_PHYS(n)                             (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_PHYS + 0x00000800 + 0x4 * (n))
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_OFFS(n)                             (PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_REG_BASE_OFFS + 0x00000800 + 0x4 * (n))
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_RMSK                                0x801f001f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_MAXn                                        24
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_INI(n)        \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_ADDR(n), HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_RMSK)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_INMI(n,mask)    \
        in_dword_masked(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_ADDR(n), mask)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_OUTI(n,val)    \
        out_dword(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_ADDR(n),val)
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_ADDR(n),mask,val,HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_INI(n))
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_VALID_BMSK                          0x80000000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_VALID_SHFT                                0x1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_MASK_BMSK                             0x1f0000
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_MASK_SHFT                                 0x10
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_ID_BMSK                                   0x1f
#define HWIO_PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT_SMRn_ID_SHFT                                    0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_SLP_CNTR
 *--------------------------------------------------------------------------*/

#define MPM2_SLP_CNTR_REG_BASE                                    (MPM2_MPM_BASE      + 0x00003000)

#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_ADDR               (MPM2_SLP_CNTR_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_RMSK               0xffffffff
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_IN          \
        in_dword_masked(HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_ADDR, HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_RMSK)
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_ADDR, m)
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_DATA_BMSK          0xffffffff
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_DATA_SHFT                 0x0

#endif /* __MBA_HWIO_H__ */
