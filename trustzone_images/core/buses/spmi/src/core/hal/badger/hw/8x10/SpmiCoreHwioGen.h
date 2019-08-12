#ifndef __SPMICOREHWIOGEN_H__
#define __SPMICOREHWIOGEN_H__
/*
===========================================================================
*/
/**
  @file SpmiCoreHwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    PMIC_ARB_SPMI_CFG
    PMIC_ARB_SPMI_PIC
    PMIC_ARB_SPMI_SPMI_GENI_CFG

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

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spmi/src/core/hal/badger/hw/8x10/SpmiCoreHwioGen.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: PMIC_ARB_SPMI_CFG
 *--------------------------------------------------------------------------*/

#define PMIC_ARB_SPMI_CFG_REG_BASE                                                             (PMIC_ARB_BASE      + 0x0000a700)

#define HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR(m)                                    (PMIC_ARB_SPMI_CFG_REG_BASE      + 0x00000000 + 0x4 * (m))
#define HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_RMSK                                              0x7
#define HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_MAXm                                              255
#define HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_INI(m)        \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR(m), HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_RMSK)
#define HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_INMI(m,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR(m), mask)
#define HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_OUTI(m,val)    \
        out_dword(HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR(m),val)
#define HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR(m),mask,val,HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_INI(m))
#define HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_PERIPH2OWNER_BMSK                                 0x7
#define HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_PERIPH2OWNER_SHFT                                 0x0

#define HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_ADDR(k)                                          (PMIC_ARB_SPMI_CFG_REG_BASE      + 0x00000400 + 0x4 * (k))
#define HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_RMSK                                               0x3fffff
#define HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_MAXk                                                    254
#define HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_INI(k)        \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_ADDR(k), HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_RMSK)
#define HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_INMI(k,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_ADDR(k), mask)
#define HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_OUTI(k,val)    \
        out_dword(HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_ADDR(k),val)
#define HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_OUTMI(k,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_ADDR(k),mask,val,HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_INI(k))
#define HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BIT_INDEX_BMSK                                     0x3c0000
#define HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BIT_INDEX_SHFT                                         0x12
#define HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_0_FLAG_BMSK                           0x20000
#define HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_0_FLAG_SHFT                              0x11
#define HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_0_BMSK                                0x1fe00
#define HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_0_SHFT                                    0x9
#define HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_1_FLAG_BMSK                             0x100
#define HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_1_FLAG_SHFT                               0x8
#define HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_1_BMSK                                   0xff
#define HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_1_SHFT                                    0x0

#define HWIO_PMIC_ARB_SPMI_MID_REG_ADDR                                                        (PMIC_ARB_SPMI_CFG_REG_BASE      + 0x00000800)
#define HWIO_PMIC_ARB_SPMI_MID_REG_RMSK                                                               0x3
#define HWIO_PMIC_ARB_SPMI_MID_REG_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_MID_REG_ADDR, HWIO_PMIC_ARB_SPMI_MID_REG_RMSK)
#define HWIO_PMIC_ARB_SPMI_MID_REG_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_MID_REG_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_MID_REG_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_MID_REG_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_MID_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_MID_REG_ADDR,m,v,HWIO_PMIC_ARB_SPMI_MID_REG_IN)
#define HWIO_PMIC_ARB_SPMI_MID_REG_MID_BMSK                                                           0x3
#define HWIO_PMIC_ARB_SPMI_MID_REG_MID_SHFT                                                           0x0

#define HWIO_PMIC_ARB_SPMI_CFG_REG_ADDR                                                        (PMIC_ARB_SPMI_CFG_REG_BASE      + 0x00000804)
#define HWIO_PMIC_ARB_SPMI_CFG_REG_RMSK                                                           0x1ffff
#define HWIO_PMIC_ARB_SPMI_CFG_REG_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_CFG_REG_ADDR, HWIO_PMIC_ARB_SPMI_CFG_REG_RMSK)
#define HWIO_PMIC_ARB_SPMI_CFG_REG_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_CFG_REG_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_CFG_REG_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_CFG_REG_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_CFG_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_CFG_REG_ADDR,m,v,HWIO_PMIC_ARB_SPMI_CFG_REG_IN)
#define HWIO_PMIC_ARB_SPMI_CFG_REG_ARBITER_CTRL_BMSK                                              0x1ffc0
#define HWIO_PMIC_ARB_SPMI_CFG_REG_ARBITER_CTRL_SHFT                                                  0x6
#define HWIO_PMIC_ARB_SPMI_CFG_REG_FORCE_ARB_AFTER_MASTER_TO_BMSK                                    0x20
#define HWIO_PMIC_ARB_SPMI_CFG_REG_FORCE_ARB_AFTER_MASTER_TO_SHFT                                     0x5
#define HWIO_PMIC_ARB_SPMI_CFG_REG_BUS_IDLE_CONN_MODE_BMSK                                           0x10
#define HWIO_PMIC_ARB_SPMI_CFG_REG_BUS_IDLE_CONN_MODE_SHFT                                            0x4
#define HWIO_PMIC_ARB_SPMI_CFG_REG_FORCE_MASTER_WRITE_ON_ERROR_BMSK                                   0x8
#define HWIO_PMIC_ARB_SPMI_CFG_REG_FORCE_MASTER_WRITE_ON_ERROR_SHFT                                   0x3
#define HWIO_PMIC_ARB_SPMI_CFG_REG_FORCE_MPM_CLK_REQ_BMSK                                             0x4
#define HWIO_PMIC_ARB_SPMI_CFG_REG_FORCE_MPM_CLK_REQ_SHFT                                             0x2
#define HWIO_PMIC_ARB_SPMI_CFG_REG_ARBITER_BYPASS_BMSK                                                0x2
#define HWIO_PMIC_ARB_SPMI_CFG_REG_ARBITER_BYPASS_SHFT                                                0x1
#define HWIO_PMIC_ARB_SPMI_CFG_REG_ARBITER_ENABLE_BMSK                                                0x1
#define HWIO_PMIC_ARB_SPMI_CFG_REG_ARBITER_ENABLE_SHFT                                                0x0

#define HWIO_PMIC_ARB_SPMI_SEC_DISABLE_REG_ADDR                                                (PMIC_ARB_SPMI_CFG_REG_BASE      + 0x00000808)
#define HWIO_PMIC_ARB_SPMI_SEC_DISABLE_REG_RMSK                                                       0x1
#define HWIO_PMIC_ARB_SPMI_SEC_DISABLE_REG_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_SEC_DISABLE_REG_ADDR, HWIO_PMIC_ARB_SPMI_SEC_DISABLE_REG_RMSK)
#define HWIO_PMIC_ARB_SPMI_SEC_DISABLE_REG_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_SEC_DISABLE_REG_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_SEC_DISABLE_REG_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_SEC_DISABLE_REG_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_SEC_DISABLE_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_SEC_DISABLE_REG_ADDR,m,v,HWIO_PMIC_ARB_SPMI_SEC_DISABLE_REG_IN)
#define HWIO_PMIC_ARB_SPMI_SEC_DISABLE_REG_DISABLE_SECURITY_BMSK                                      0x1
#define HWIO_PMIC_ARB_SPMI_SEC_DISABLE_REG_DISABLE_SECURITY_SHFT                                      0x0

#define HWIO_PMIC_ARB_SPMI_HW_VERSION_ADDR                                                     (PMIC_ARB_SPMI_CFG_REG_BASE      + 0x0000080c)
#define HWIO_PMIC_ARB_SPMI_HW_VERSION_RMSK                                                     0xffffffff
#define HWIO_PMIC_ARB_SPMI_HW_VERSION_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_HW_VERSION_ADDR, HWIO_PMIC_ARB_SPMI_HW_VERSION_RMSK)
#define HWIO_PMIC_ARB_SPMI_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_HW_VERSION_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_HW_VERSION_HW_VERSION_MAJOR_BMSK                                    0xf0000000
#define HWIO_PMIC_ARB_SPMI_HW_VERSION_HW_VERSION_MAJOR_SHFT                                          0x1c
#define HWIO_PMIC_ARB_SPMI_HW_VERSION_HW_VERSION_MINOR_BMSK                                     0xfff0000
#define HWIO_PMIC_ARB_SPMI_HW_VERSION_HW_VERSION_MINOR_SHFT                                          0x10
#define HWIO_PMIC_ARB_SPMI_HW_VERSION_HW_VERSION_STEP_BMSK                                         0xffff
#define HWIO_PMIC_ARB_SPMI_HW_VERSION_HW_VERSION_STEP_SHFT                                            0x0

#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_ADDR                                                       (PMIC_ARB_SPMI_CFG_REG_BASE      + 0x00000810)
#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_RMSK                                                             0x7f
#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_CGC_CTRL_ADDR, HWIO_PMIC_ARB_SPMI_CGC_CTRL_RMSK)
#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_CGC_CTRL_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_CGC_CTRL_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_CGC_CTRL_ADDR,m,v,HWIO_PMIC_ARB_SPMI_CGC_CTRL_IN)
#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_MAP_LOGIC_CLK_CGC_ON_BMSK                                        0x40
#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_MAP_LOGIC_CLK_CGC_ON_SHFT                                         0x6
#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_RPU_CLK_CGC_ON_BMSK                                              0x20
#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_RPU_CLK_CGC_ON_SHFT                                               0x5
#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_MWB_CLK_CGC_ON_BMSK                                              0x10
#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_MWB_CLK_CGC_ON_SHFT                                               0x4
#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_PIC_CLK_CGC_ON_BMSK                                               0x8
#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_PIC_CLK_CGC_ON_SHFT                                               0x3
#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_PAC_CLK_CGC_ON_BMSK                                               0x4
#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_PAC_CLK_CGC_ON_SHFT                                               0x2
#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_CFG_AHB_BRIDGE_WR_CLK_CGC_ON_BMSK                                 0x2
#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_CFG_AHB_BRIDGE_WR_CLK_CGC_ON_SHFT                                 0x1
#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_CFG_AHB_BRIDGE_CLK_CGC_ON_BMSK                                    0x1
#define HWIO_PMIC_ARB_SPMI_CGC_CTRL_CFG_AHB_BRIDGE_CLK_CGC_ON_SHFT                                    0x0

#define HWIO_PMIC_ARB_SPMI_MWB_ENABLE_REG_ADDR                                                 (PMIC_ARB_SPMI_CFG_REG_BASE      + 0x00000814)
#define HWIO_PMIC_ARB_SPMI_MWB_ENABLE_REG_RMSK                                                        0x1
#define HWIO_PMIC_ARB_SPMI_MWB_ENABLE_REG_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_MWB_ENABLE_REG_ADDR, HWIO_PMIC_ARB_SPMI_MWB_ENABLE_REG_RMSK)
#define HWIO_PMIC_ARB_SPMI_MWB_ENABLE_REG_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_MWB_ENABLE_REG_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_MWB_ENABLE_REG_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_MWB_ENABLE_REG_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_MWB_ENABLE_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_MWB_ENABLE_REG_ADDR,m,v,HWIO_PMIC_ARB_SPMI_MWB_ENABLE_REG_IN)
#define HWIO_PMIC_ARB_SPMI_MWB_ENABLE_REG_MWB_ENABLE_BMSK                                             0x1
#define HWIO_PMIC_ARB_SPMI_MWB_ENABLE_REG_MWB_ENABLE_SHFT                                             0x0

#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_ADDR                                            (PMIC_ARB_SPMI_CFG_REG_BASE      + 0x00000820)
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_RMSK                                                 0xfff
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_ADDR, HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_RMSK)
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_ARBITER_DISCONNECTED_BMSK                            0x800
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_ARBITER_DISCONNECTED_SHFT                              0xb
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_ARBITER_CONNECTED_BMSK                               0x400
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_ARBITER_CONNECTED_SHFT                                 0xa
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_PERIH_IRQ_LOST_BMSK                                  0x200
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_PERIH_IRQ_LOST_SHFT                                    0x9
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_UNEXPECTED_SSC_BMSK                                  0x100
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_UNEXPECTED_SSC_SHFT                                    0x8
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_NO_RESPONSE_DATA_FRAME_DETECTED_BMSK                  0x80
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_NO_RESPONSE_DATA_FRAME_DETECTED_SHFT                   0x7
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_NO_RESPONSE_CMD_FRAME_DETECTED_BMSK                   0x40
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_NO_RESPONSE_CMD_FRAME_DETECTED_SHFT                    0x6
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_FALSE_MASTER_ARBITRATION_WIN_BMSK                     0x20
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_FALSE_MASTER_ARBITRATION_WIN_SHFT                      0x5
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_FALSE_BUS_REQUEST_BMSK                                0x10
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_FALSE_BUS_REQUEST_SHFT                                 0x4
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_UNSUPPORTED_COMMAND_BMSK                               0x8
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_UNSUPPORTED_COMMAND_SHFT                               0x3
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_DATA_ADDR_FRAME_PARITY_ERROR_BMSK                      0x4
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_DATA_ADDR_FRAME_PARITY_ERROR_SHFT                      0x2
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_SLAVE_CMD_FRAME_PARITY_ERROR_BMSK                      0x2
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_SLAVE_CMD_FRAME_PARITY_ERROR_SHFT                      0x1
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_MASTER_CMD_FRAME_PARITY_ERROR_BMSK                     0x1
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_MASTER_CMD_FRAME_PARITY_ERROR_SHFT                     0x0

#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_ADDR                                            (PMIC_ARB_SPMI_CFG_REG_BASE      + 0x00000824)
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_RMSK                                                 0xfff
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_ADDR, HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_RMSK)
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_ADDR,m,v,HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_IN)
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_ARBITER_DISCONNECTED_BMSK                            0x800
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_ARBITER_DISCONNECTED_SHFT                              0xb
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_ARBITER_CONNECTED_BMSK                               0x400
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_ARBITER_CONNECTED_SHFT                                 0xa
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_PERIH_IRQ_LOST_BMSK                                  0x200
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_PERIH_IRQ_LOST_SHFT                                    0x9
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_UNEXPECTED_SSC_BMSK                                  0x100
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_UNEXPECTED_SSC_SHFT                                    0x8
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_NO_RESPONSE_DATA_FRAME_DETECTED_BMSK                  0x80
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_NO_RESPONSE_DATA_FRAME_DETECTED_SHFT                   0x7
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_NO_RESPONSE_CMD_FRAME_DETECTED_BMSK                   0x40
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_NO_RESPONSE_CMD_FRAME_DETECTED_SHFT                    0x6
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_FALSE_MASTER_ARBITRATION_WIN_BMSK                     0x20
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_FALSE_MASTER_ARBITRATION_WIN_SHFT                      0x5
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_FALSE_BUS_REQUEST_BMSK                                0x10
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_FALSE_BUS_REQUEST_SHFT                                 0x4
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_UNSUPPORTED_COMMAND_BMSK                               0x8
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_UNSUPPORTED_COMMAND_SHFT                               0x3
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_DATA_ADDR_FRAME_PARITY_ERROR_BMSK                      0x4
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_DATA_ADDR_FRAME_PARITY_ERROR_SHFT                      0x2
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_SLAVE_CMD_FRAME_PARITY_ERROR_BMSK                      0x2
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_SLAVE_CMD_FRAME_PARITY_ERROR_SHFT                      0x1
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_MASTER_CMD_FRAME_PARITY_ERROR_BMSK                     0x1
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_ENABLE_MASTER_CMD_FRAME_PARITY_ERROR_SHFT                     0x0

#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_ADDR                                             (PMIC_ARB_SPMI_CFG_REG_BASE      + 0x00000828)
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_RMSK                                                  0xfff
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_ARBITER_DISCONNECTED_BMSK                             0x800
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_ARBITER_DISCONNECTED_SHFT                               0xb
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_ARBITER_CONNECTED_BMSK                                0x400
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_ARBITER_CONNECTED_SHFT                                  0xa
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_PERIH_IRQ_LOST_BMSK                                   0x200
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_PERIH_IRQ_LOST_SHFT                                     0x9
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_UNEXPECTED_SSC_BMSK                                   0x100
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_UNEXPECTED_SSC_SHFT                                     0x8
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_NO_RESPONSE_DATA_FRAME_DETECTED_BMSK                   0x80
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_NO_RESPONSE_DATA_FRAME_DETECTED_SHFT                    0x7
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_NO_RESPONSE_CMD_FRAME_DETECTED_BMSK                    0x40
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_NO_RESPONSE_CMD_FRAME_DETECTED_SHFT                     0x6
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_FALSE_MASTER_ARBITRATION_WIN_BMSK                      0x20
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_FALSE_MASTER_ARBITRATION_WIN_SHFT                       0x5
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_FALSE_BUS_REQUEST_BMSK                                 0x10
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_FALSE_BUS_REQUEST_SHFT                                  0x4
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_UNSUPPORTED_COMMAND_BMSK                                0x8
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_UNSUPPORTED_COMMAND_SHFT                                0x3
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_DATA_ADDR_FRAME_PARITY_ERROR_BMSK                       0x4
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_DATA_ADDR_FRAME_PARITY_ERROR_SHFT                       0x2
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_SLAVE_CMD_FRAME_PARITY_ERROR_BMSK                       0x2
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_SLAVE_CMD_FRAME_PARITY_ERROR_SHFT                       0x1
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_MASTER_CMD_FRAME_PARITY_ERROR_BMSK                      0x1
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_CLEAR_MASTER_CMD_FRAME_PARITY_ERROR_SHFT                      0x0

#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_ADDR                                            (PMIC_ARB_SPMI_CFG_REG_BASE      + 0x0000082c)
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_RMSK                                                 0xfff
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_ARBITER_DISCONNECTED_BMSK                            0x800
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_ARBITER_DISCONNECTED_SHFT                              0xb
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_ARBITER_CONNECTED_BMSK                               0x400
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_ARBITER_CONNECTED_SHFT                                 0xa
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_PERIH_IRQ_LOST_BMSK                                  0x200
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_PERIH_IRQ_LOST_SHFT                                    0x9
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_UNEXPECTED_SSC_BMSK                                  0x100
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_UNEXPECTED_SSC_SHFT                                    0x8
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_NO_RESPONSE_DATA_FRAME_DETECTED_BMSK                  0x80
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_NO_RESPONSE_DATA_FRAME_DETECTED_SHFT                   0x7
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_NO_RESPONSE_CMD_FRAME_DETECTED_BMSK                   0x40
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_NO_RESPONSE_CMD_FRAME_DETECTED_SHFT                    0x6
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_FALSE_MASTER_ARBITRATION_WIN_BMSK                     0x20
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_FALSE_MASTER_ARBITRATION_WIN_SHFT                      0x5
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_FALSE_BUS_REQUEST_BMSK                                0x10
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_FALSE_BUS_REQUEST_SHFT                                 0x4
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_UNSUPPORTED_COMMAND_BMSK                               0x8
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_UNSUPPORTED_COMMAND_SHFT                               0x3
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_DATA_ADDR_FRAME_PARITY_ERROR_BMSK                      0x4
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_DATA_ADDR_FRAME_PARITY_ERROR_SHFT                      0x2
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_SLAVE_CMD_FRAME_PARITY_ERROR_BMSK                      0x2
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_SLAVE_CMD_FRAME_PARITY_ERROR_SHFT                      0x1
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_MASTER_CMD_FRAME_PARITY_ERROR_BMSK                     0x1
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_SET_MASTER_CMD_FRAME_PARITY_ERROR_SHFT                     0x0

#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_ADDR                                          (PMIC_ARB_SPMI_CFG_REG_BASE      + 0x00000830)
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_RMSK                                               0xfff
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_ARBITER_DISCONNECTED_BMSK                          0x800
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_ARBITER_DISCONNECTED_SHFT                            0xb
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_ARBITER_CONNECTED_BMSK                             0x400
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_ARBITER_CONNECTED_SHFT                               0xa
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_PERIH_IRQ_LOST_BMSK                                0x200
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_PERIH_IRQ_LOST_SHFT                                  0x9
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_UNEXPECTED_SSC_BMSK                                0x100
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_UNEXPECTED_SSC_SHFT                                  0x8
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_NO_RESPONSE_DATA_FRAME_DETECTED_BMSK                0x80
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_NO_RESPONSE_DATA_FRAME_DETECTED_SHFT                 0x7
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_NO_RESPONSE_CMD_FRAME_DETECTED_BMSK                 0x40
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_NO_RESPONSE_CMD_FRAME_DETECTED_SHFT                  0x6
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_FALSE_MASTER_ARBITRATION_WIN_BMSK                   0x20
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_FALSE_MASTER_ARBITRATION_WIN_SHFT                    0x5
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_FALSE_BUS_REQUEST_BMSK                              0x10
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_FALSE_BUS_REQUEST_SHFT                               0x4
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_UNSUPPORTED_COMMAND_BMSK                             0x8
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_UNSUPPORTED_COMMAND_SHFT                             0x3
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_DATA_ADDR_FRAME_PARITY_ERROR_BMSK                    0x4
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_DATA_ADDR_FRAME_PARITY_ERROR_SHFT                    0x2
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_SLAVE_CMD_FRAME_PARITY_ERROR_BMSK                    0x2
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_SLAVE_CMD_FRAME_PARITY_ERROR_SHFT                    0x1
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_MASTER_CMD_FRAME_PARITY_ERROR_BMSK                   0x1
#define HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_EN_CLEAR_MASTER_CMD_FRAME_PARITY_ERROR_SHFT                   0x0

#define HWIO_PMIC_ARB_SPMI_CHAR_CFG_ADDR                                                       (PMIC_ARB_SPMI_CFG_REG_BASE      + 0x00000840)
#define HWIO_PMIC_ARB_SPMI_CHAR_CFG_RMSK                                                          0xfff11
#define HWIO_PMIC_ARB_SPMI_CHAR_CFG_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_CHAR_CFG_ADDR, HWIO_PMIC_ARB_SPMI_CHAR_CFG_RMSK)
#define HWIO_PMIC_ARB_SPMI_CHAR_CFG_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_CHAR_CFG_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_CHAR_CFG_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_CHAR_CFG_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_CHAR_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_CHAR_CFG_ADDR,m,v,HWIO_PMIC_ARB_SPMI_CHAR_CFG_IN)
#define HWIO_PMIC_ARB_SPMI_CHAR_CFG_CHAR_MODE_BMSK                                                0xff000
#define HWIO_PMIC_ARB_SPMI_CHAR_CFG_CHAR_MODE_SHFT                                                    0xc
#define HWIO_PMIC_ARB_SPMI_CHAR_CFG_CHAR_STATUS_BMSK                                                0xf00
#define HWIO_PMIC_ARB_SPMI_CHAR_CFG_CHAR_STATUS_SHFT                                                  0x8
#define HWIO_PMIC_ARB_SPMI_CHAR_CFG_DIRECTION_BMSK                                                   0x10
#define HWIO_PMIC_ARB_SPMI_CHAR_CFG_DIRECTION_SHFT                                                    0x4
#define HWIO_PMIC_ARB_SPMI_CHAR_CFG_ENABLE_BMSK                                                       0x1
#define HWIO_PMIC_ARB_SPMI_CHAR_CFG_ENABLE_SHFT                                                       0x0

#define HWIO_PMIC_ARB_SPMI_CHAR_DATA_n_ADDR(n)                                                 (PMIC_ARB_SPMI_CFG_REG_BASE      + 0x00000844 + 0x4 * (n))
#define HWIO_PMIC_ARB_SPMI_CHAR_DATA_n_RMSK                                                        0xffff
#define HWIO_PMIC_ARB_SPMI_CHAR_DATA_n_MAXn                                                             1
#define HWIO_PMIC_ARB_SPMI_CHAR_DATA_n_INI(n)        \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_CHAR_DATA_n_ADDR(n), HWIO_PMIC_ARB_SPMI_CHAR_DATA_n_RMSK)
#define HWIO_PMIC_ARB_SPMI_CHAR_DATA_n_INMI(n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_CHAR_DATA_n_ADDR(n), mask)
#define HWIO_PMIC_ARB_SPMI_CHAR_DATA_n_OUTI(n,val)    \
        out_dword(HWIO_PMIC_ARB_SPMI_CHAR_DATA_n_ADDR(n),val)
#define HWIO_PMIC_ARB_SPMI_CHAR_DATA_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_CHAR_DATA_n_ADDR(n),mask,val,HWIO_PMIC_ARB_SPMI_CHAR_DATA_n_INI(n))
#define HWIO_PMIC_ARB_SPMI_CHAR_DATA_n_DIN_ACTUAL_BMSK                                             0xff00
#define HWIO_PMIC_ARB_SPMI_CHAR_DATA_n_DIN_ACTUAL_SHFT                                                0x8
#define HWIO_PMIC_ARB_SPMI_CHAR_DATA_n_DOUT_DATA_DIN_EXP_BMSK                                        0xff
#define HWIO_PMIC_ARB_SPMI_CHAR_DATA_n_DOUT_DATA_DIN_EXP_SHFT                                         0x0

#define HWIO_PMIC_ARB_SPMI_TEST_BUS_CTRL_ADDR                                                  (PMIC_ARB_SPMI_CFG_REG_BASE      + 0x0000084c)
#define HWIO_PMIC_ARB_SPMI_TEST_BUS_CTRL_RMSK                                                       0x3ff
#define HWIO_PMIC_ARB_SPMI_TEST_BUS_CTRL_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_TEST_BUS_CTRL_ADDR, HWIO_PMIC_ARB_SPMI_TEST_BUS_CTRL_RMSK)
#define HWIO_PMIC_ARB_SPMI_TEST_BUS_CTRL_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_TEST_BUS_CTRL_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_TEST_BUS_CTRL_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_TEST_BUS_CTRL_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_TEST_BUS_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_TEST_BUS_CTRL_ADDR,m,v,HWIO_PMIC_ARB_SPMI_TEST_BUS_CTRL_IN)
#define HWIO_PMIC_ARB_SPMI_TEST_BUS_CTRL_TEST_BUS_ARB_SEL_BMSK                                      0x3c0
#define HWIO_PMIC_ARB_SPMI_TEST_BUS_CTRL_TEST_BUS_ARB_SEL_SHFT                                        0x6
#define HWIO_PMIC_ARB_SPMI_TEST_BUS_CTRL_TEST_BUS_INT_SEL_BMSK                                       0x3c
#define HWIO_PMIC_ARB_SPMI_TEST_BUS_CTRL_TEST_BUS_INT_SEL_SHFT                                        0x2
#define HWIO_PMIC_ARB_SPMI_TEST_BUS_CTRL_TEST_BUS_SEL_BMSK                                            0x3
#define HWIO_PMIC_ARB_SPMI_TEST_BUS_CTRL_TEST_BUS_SEL_SHFT                                            0x0

#define HWIO_PMIC_ARB_SPMI_HW_SW_EVENTS_CTRL_ADDR                                              (PMIC_ARB_SPMI_CFG_REG_BASE      + 0x00000850)
#define HWIO_PMIC_ARB_SPMI_HW_SW_EVENTS_CTRL_RMSK                                                     0x7
#define HWIO_PMIC_ARB_SPMI_HW_SW_EVENTS_CTRL_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_HW_SW_EVENTS_CTRL_ADDR, HWIO_PMIC_ARB_SPMI_HW_SW_EVENTS_CTRL_RMSK)
#define HWIO_PMIC_ARB_SPMI_HW_SW_EVENTS_CTRL_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_HW_SW_EVENTS_CTRL_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_HW_SW_EVENTS_CTRL_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_HW_SW_EVENTS_CTRL_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_HW_SW_EVENTS_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_HW_SW_EVENTS_CTRL_ADDR,m,v,HWIO_PMIC_ARB_SPMI_HW_SW_EVENTS_CTRL_IN)
#define HWIO_PMIC_ARB_SPMI_HW_SW_EVENTS_CTRL_HW_SW_EVENTS_SEL_BMSK                                    0x7
#define HWIO_PMIC_ARB_SPMI_HW_SW_EVENTS_CTRL_HW_SW_EVENTS_SEL_SHFT                                    0x0

/*----------------------------------------------------------------------------
 * MODULE: PMIC_ARB_SPMI_PIC
 *--------------------------------------------------------------------------*/

#define PMIC_ARB_SPMI_PIC_REG_BASE                                             (PMIC_ARB_BASE      + 0x0000b000)

#define HWIO_PMIC_ARB_SPMI_PIC_OWNERm_ACC_STATUSn_ADDR(m,n)                    (PMIC_ARB_SPMI_PIC_REG_BASE      + 0x00000000 + 0x20 * (m) + 0x4 * (n))
#define HWIO_PMIC_ARB_SPMI_PIC_OWNERm_ACC_STATUSn_RMSK                         0xffffffff
#define HWIO_PMIC_ARB_SPMI_PIC_OWNERm_ACC_STATUSn_MAXm                                  5
#define HWIO_PMIC_ARB_SPMI_PIC_OWNERm_ACC_STATUSn_MAXn                                  7
#define HWIO_PMIC_ARB_SPMI_PIC_OWNERm_ACC_STATUSn_INI2(m,n)        \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_PIC_OWNERm_ACC_STATUSn_ADDR(m,n), HWIO_PMIC_ARB_SPMI_PIC_OWNERm_ACC_STATUSn_RMSK)
#define HWIO_PMIC_ARB_SPMI_PIC_OWNERm_ACC_STATUSn_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_PIC_OWNERm_ACC_STATUSn_ADDR(m,n), mask)
#define HWIO_PMIC_ARB_SPMI_PIC_OWNERm_ACC_STATUSn_INT_ACC_STATUS_BMSK          0xffffffff
#define HWIO_PMIC_ARB_SPMI_PIC_OWNERm_ACC_STATUSn_INT_ACC_STATUS_SHFT                 0x0

#define HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_ADDR(n)                             (PMIC_ARB_SPMI_PIC_REG_BASE      + 0x00000200 + 0x4 * (n))
#define HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_RMSK                                       0x1
#define HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_MAXn                                       255
#define HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_INI(n)        \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_ADDR(n), HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_RMSK)
#define HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_INMI(n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_ADDR(n), mask)
#define HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_OUTI(n,val)    \
        out_dword(HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_ADDR(n),val)
#define HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_ADDR(n),mask,val,HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_INI(n))
#define HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_INT_ACC_ENABLE_BMSK                        0x1
#define HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_INT_ACC_ENABLE_SHFT                        0x0

#define HWIO_PMIC_ARB_SPMI_PIC_IRQ_STATUSn_ADDR(n)                             (PMIC_ARB_SPMI_PIC_REG_BASE      + 0x00000600 + 0x4 * (n))
#define HWIO_PMIC_ARB_SPMI_PIC_IRQ_STATUSn_RMSK                                      0xff
#define HWIO_PMIC_ARB_SPMI_PIC_IRQ_STATUSn_MAXn                                       255
#define HWIO_PMIC_ARB_SPMI_PIC_IRQ_STATUSn_INI(n)        \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_PIC_IRQ_STATUSn_ADDR(n), HWIO_PMIC_ARB_SPMI_PIC_IRQ_STATUSn_RMSK)
#define HWIO_PMIC_ARB_SPMI_PIC_IRQ_STATUSn_INMI(n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_PIC_IRQ_STATUSn_ADDR(n), mask)
#define HWIO_PMIC_ARB_SPMI_PIC_IRQ_STATUSn_INT_STATUS_BMSK                           0xff
#define HWIO_PMIC_ARB_SPMI_PIC_IRQ_STATUSn_INT_STATUS_SHFT                            0x0

#define HWIO_PMIC_ARB_SPMI_PIC_IRQ_CLEARn_ADDR(n)                              (PMIC_ARB_SPMI_PIC_REG_BASE      + 0x00000a00 + 0x4 * (n))
#define HWIO_PMIC_ARB_SPMI_PIC_IRQ_CLEARn_RMSK                                       0xff
#define HWIO_PMIC_ARB_SPMI_PIC_IRQ_CLEARn_MAXn                                        255
#define HWIO_PMIC_ARB_SPMI_PIC_IRQ_CLEARn_OUTI(n,val)    \
        out_dword(HWIO_PMIC_ARB_SPMI_PIC_IRQ_CLEARn_ADDR(n),val)
#define HWIO_PMIC_ARB_SPMI_PIC_IRQ_CLEARn_INT_CLEAR_BMSK                             0xff
#define HWIO_PMIC_ARB_SPMI_PIC_IRQ_CLEARn_INT_CLEAR_SHFT                              0x0

/*----------------------------------------------------------------------------
 * MODULE: PMIC_ARB_SPMI_SPMI_GENI_CFG
 *--------------------------------------------------------------------------*/

#define PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE                                        (PMIC_ARB_BASE      + 0x0000a000)

#define HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_ADDR                                       (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000000)
#define HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_RMSK                                              0x1
#define HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_SER_CLK_SEL_BMSK                                  0x1
#define HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_SER_CLK_SEL_SHFT                                  0x0

#define HWIO_PMIC_ARB_SPMI_GENI_HW_VERSION_ADDR                                     (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000004)
#define HWIO_PMIC_ARB_SPMI_GENI_HW_VERSION_RMSK                                     0xffffffff
#define HWIO_PMIC_ARB_SPMI_GENI_HW_VERSION_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_HW_VERSION_ADDR, HWIO_PMIC_ARB_SPMI_GENI_HW_VERSION_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_HW_VERSION_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_HW_VERSION_MAJOR_BMSK                               0xf0000000
#define HWIO_PMIC_ARB_SPMI_GENI_HW_VERSION_MAJOR_SHFT                                     0x1c
#define HWIO_PMIC_ARB_SPMI_GENI_HW_VERSION_MINOR_BMSK                                0xfff0000
#define HWIO_PMIC_ARB_SPMI_GENI_HW_VERSION_MINOR_SHFT                                     0x10
#define HWIO_PMIC_ARB_SPMI_GENI_HW_VERSION_STEP_BMSK                                    0xffff
#define HWIO_PMIC_ARB_SPMI_GENI_HW_VERSION_STEP_SHFT                                       0x0

#define HWIO_PMIC_ARB_SPMI_GENI_FW_REVISION_ADDR                                    (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000008)
#define HWIO_PMIC_ARB_SPMI_GENI_FW_REVISION_RMSK                                    0xffffffff
#define HWIO_PMIC_ARB_SPMI_GENI_FW_REVISION_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_FW_REVISION_ADDR, HWIO_PMIC_ARB_SPMI_GENI_FW_REVISION_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_FW_REVISION_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_FW_REVISION_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_FW_REVISION_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_FW_REVISION_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_FW_REVISION_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_FW_REVISION_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_FW_REVISION_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_FW_REVISION_MAJOR_REVISION_BMSK                     0xff000000
#define HWIO_PMIC_ARB_SPMI_GENI_FW_REVISION_MAJOR_REVISION_SHFT                           0x18
#define HWIO_PMIC_ARB_SPMI_GENI_FW_REVISION_VERSION_BMSK                              0xffffff
#define HWIO_PMIC_ARB_SPMI_GENI_FW_REVISION_VERSION_SHFT                                   0x0

#define HWIO_PMIC_ARB_SPMI_GENI_FORCE_DEFAULT_REG_ADDR                              (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x0000000c)
#define HWIO_PMIC_ARB_SPMI_GENI_FORCE_DEFAULT_REG_RMSK                                     0x1
#define HWIO_PMIC_ARB_SPMI_GENI_FORCE_DEFAULT_REG_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_FORCE_DEFAULT_REG_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_FORCE_DEFAULT_REG_FORCE_DEFAULT_BMSK                       0x1
#define HWIO_PMIC_ARB_SPMI_GENI_FORCE_DEFAULT_REG_FORCE_DEFAULT_SHFT                       0x0

#define HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_ADDR                                    (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000010)
#define HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_RMSK                                           0x7
#define HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_ADDR, HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_SOE2_EN_BMSK                                   0x4
#define HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_SOE2_EN_SHFT                                   0x2
#define HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_SOE1_EN_BMSK                                   0x2
#define HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_SOE1_EN_SHFT                                   0x1
#define HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_SOE0_EN_BMSK                                   0x1
#define HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_SOE0_EN_SHFT                                   0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_ADDR                                       (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000014)
#define HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_RMSK                                            0x30f
#define HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_PROG_RAM_SCLK_OFF_BMSK                          0x200
#define HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_PROG_RAM_SCLK_OFF_SHFT                            0x9
#define HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_PROG_RAM_HCLK_OFF_BMSK                          0x100
#define HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_PROG_RAM_HCLK_OFF_SHFT                            0x8
#define HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_SCLK_CGC_ON_BMSK                                  0x8
#define HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_SCLK_CGC_ON_SHFT                                  0x3
#define HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_DATA_AHB_CLK_CGC_ON_BMSK                          0x4
#define HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_DATA_AHB_CLK_CGC_ON_SHFT                          0x2
#define HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_CFG_AHB_WR_CLK_CGC_ON_BMSK                        0x2
#define HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_CFG_AHB_WR_CLK_CGC_ON_SHFT                        0x1
#define HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_CFG_AHB_CLK_CGC_ON_BMSK                           0x1
#define HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_CFG_AHB_CLK_CGC_ON_SHFT                           0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_ADDR                                       (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000018)
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_RMSK                                          0xfff11
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_CHAR_MODE_BMSK                                0xff000
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_CHAR_MODE_SHFT                                    0xc
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_CHAR_STATUS_BMSK                                0xf00
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_CHAR_STATUS_SHFT                                  0x8
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_DIRECTION_BMSK                                   0x10
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_DIRECTION_SHFT                                    0x4
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_ENABLE_BMSK                                       0x1
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_CFG_ENABLE_SHFT                                       0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_DATA_n_ADDR(n)                                 (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x0000001c + 0x4 * (n))
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_DATA_n_RMSK                                        0xffff
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_DATA_n_MAXn                                             1
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_DATA_n_INI(n)        \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CHAR_DATA_n_ADDR(n), HWIO_PMIC_ARB_SPMI_GENI_CHAR_DATA_n_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_DATA_n_INMI(n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CHAR_DATA_n_ADDR(n), mask)
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_DATA_n_OUTI(n,val)    \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CHAR_DATA_n_ADDR(n),val)
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_DATA_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CHAR_DATA_n_ADDR(n),mask,val,HWIO_PMIC_ARB_SPMI_GENI_CHAR_DATA_n_INI(n))
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_DATA_n_DIN_ACTUAL_BMSK                             0xff00
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_DATA_n_DIN_ACTUAL_SHFT                                0x8
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_DATA_n_DOUT_DATA_DIN_EXP_BMSK                        0xff
#define HWIO_PMIC_ARB_SPMI_GENI_CHAR_DATA_n_DOUT_DATA_DIN_EXP_SHFT                         0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CTRL_ADDR                                           (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000024)
#define HWIO_PMIC_ARB_SPMI_GENI_CTRL_RMSK                                                  0x1
#define HWIO_PMIC_ARB_SPMI_GENI_CTRL_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CTRL_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CTRL_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CTRL_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CTRL_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CTRL_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CTRL_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CTRL_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CTRL_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CTRL_GENI_DISABLE_BMSK                                     0x1
#define HWIO_PMIC_ARB_SPMI_GENI_CTRL_GENI_DISABLE_SHFT                                     0x0

#define HWIO_PMIC_ARB_SPMI_GENI_STATUS_ADDR                                         (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000028)
#define HWIO_PMIC_ARB_SPMI_GENI_STATUS_RMSK                                               0xff
#define HWIO_PMIC_ARB_SPMI_GENI_STATUS_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_STATUS_ADDR, HWIO_PMIC_ARB_SPMI_GENI_STATUS_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_STATUS_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_STATUS_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_STATUS_GENI_CMD_FSM_STATE_BMSK                            0xf0
#define HWIO_PMIC_ARB_SPMI_GENI_STATUS_GENI_CMD_FSM_STATE_SHFT                             0x4
#define HWIO_PMIC_ARB_SPMI_GENI_STATUS_NOT_USED_BITS_BMSK                                  0xe
#define HWIO_PMIC_ARB_SPMI_GENI_STATUS_NOT_USED_BITS_SHFT                                  0x1
#define HWIO_PMIC_ARB_SPMI_GENI_STATUS_GENI_CMD_ACTIVE_BMSK                                0x1
#define HWIO_PMIC_ARB_SPMI_GENI_STATUS_GENI_CMD_ACTIVE_SHFT                                0x0

#define HWIO_PMIC_ARB_SPMI_GENI_TEST_BUS_CTRL_ADDR                                  (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x0000002c)
#define HWIO_PMIC_ARB_SPMI_GENI_TEST_BUS_CTRL_RMSK                                         0xf
#define HWIO_PMIC_ARB_SPMI_GENI_TEST_BUS_CTRL_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_TEST_BUS_CTRL_ADDR, HWIO_PMIC_ARB_SPMI_GENI_TEST_BUS_CTRL_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_TEST_BUS_CTRL_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_TEST_BUS_CTRL_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_TEST_BUS_CTRL_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_TEST_BUS_CTRL_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_TEST_BUS_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_TEST_BUS_CTRL_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_TEST_BUS_CTRL_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_TEST_BUS_CTRL_TEST_BUS_SEL_BMSK                            0xf
#define HWIO_PMIC_ARB_SPMI_GENI_TEST_BUS_CTRL_TEST_BUS_SEL_SHFT                            0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG0_ADDR                                       (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000100)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG0_RMSK                                        0xfffffff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG0_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG0_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CFG_REG0_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG0_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG0_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG0_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG0_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG0_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CFG_REG0_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG0_PRIM_CONTEXT_VEC_1_BMSK                     0xfffc000
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG0_PRIM_CONTEXT_VEC_1_SHFT                           0xe
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG0_PRIM_CONTEXT_VEC_0_BMSK                        0x3fff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG0_PRIM_CONTEXT_VEC_0_SHFT                           0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG1_ADDR                                       (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000104)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG1_RMSK                                        0xfffffff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG1_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG1_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CFG_REG1_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG1_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG1_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG1_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG1_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG1_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CFG_REG1_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG1_PRIM_CONTEXT_VEC_3_BMSK                     0xfffc000
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG1_PRIM_CONTEXT_VEC_3_SHFT                           0xe
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG1_PRIM_CONTEXT_VEC_2_BMSK                        0x3fff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG1_PRIM_CONTEXT_VEC_2_SHFT                           0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG2_ADDR                                       (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000108)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG2_RMSK                                        0xfffffff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG2_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG2_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CFG_REG2_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG2_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG2_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG2_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG2_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG2_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CFG_REG2_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG2_PRIM_CONTEXT_VEC_5_BMSK                     0xfffc000
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG2_PRIM_CONTEXT_VEC_5_SHFT                           0xe
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG2_PRIM_CONTEXT_VEC_4_BMSK                        0x3fff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG2_PRIM_CONTEXT_VEC_4_SHFT                           0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG3_ADDR                                       (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x0000010c)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG3_RMSK                                        0xfffffff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG3_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG3_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CFG_REG3_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG3_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG3_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG3_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG3_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG3_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CFG_REG3_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG3_PRIM_CONTEXT_VEC_7_BMSK                     0xfffc000
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG3_PRIM_CONTEXT_VEC_7_SHFT                           0xe
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG3_PRIM_CONTEXT_VEC_6_BMSK                        0x3fff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG3_PRIM_CONTEXT_VEC_6_SHFT                           0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG4_ADDR                                       (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000110)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG4_RMSK                                        0xfffffff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG4_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG4_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CFG_REG4_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG4_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG4_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG4_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG4_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG4_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CFG_REG4_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG4_PRIM_CONTEXT_VEC_9_BMSK                     0xfffc000
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG4_PRIM_CONTEXT_VEC_9_SHFT                           0xe
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG4_PRIM_CONTEXT_VEC_8_BMSK                        0x3fff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG4_PRIM_CONTEXT_VEC_8_SHFT                           0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG5_ADDR                                       (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000114)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG5_RMSK                                        0xfffffff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG5_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG5_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CFG_REG5_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG5_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG5_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG5_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG5_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG5_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CFG_REG5_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG5_PRIM_CONTEXT_VEC_11_BMSK                    0xfffc000
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG5_PRIM_CONTEXT_VEC_11_SHFT                          0xe
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG5_PRIM_CONTEXT_VEC_10_BMSK                       0x3fff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG5_PRIM_CONTEXT_VEC_10_SHFT                          0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG6_ADDR                                       (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000118)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG6_RMSK                                        0xfffffff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG6_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG6_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CFG_REG6_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG6_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG6_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG6_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG6_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG6_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CFG_REG6_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG6_PRIM_CONTEXT_VEC_13_BMSK                    0xfffc000
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG6_PRIM_CONTEXT_VEC_13_SHFT                          0xe
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG6_PRIM_CONTEXT_VEC_12_BMSK                       0x3fff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG6_PRIM_CONTEXT_VEC_12_SHFT                          0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG7_ADDR                                       (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x0000011c)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG7_RMSK                                        0xfffffff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG7_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG7_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CFG_REG7_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG7_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG7_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG7_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG7_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG7_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CFG_REG7_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG7_PRIM_CONTEXT_VEC_15_BMSK                    0xfffc000
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG7_PRIM_CONTEXT_VEC_15_SHFT                          0xe
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG7_PRIM_CONTEXT_VEC_14_BMSK                       0x3fff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG7_PRIM_CONTEXT_VEC_14_SHFT                          0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG8_ADDR                                       (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000120)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG8_RMSK                                        0xfffffff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG8_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG8_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CFG_REG8_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG8_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG8_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG8_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG8_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG8_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CFG_REG8_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG8_PRIM_CONTEXT_VEC_17_BMSK                    0xfffc000
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG8_PRIM_CONTEXT_VEC_17_SHFT                          0xe
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG8_PRIM_CONTEXT_VEC_16_BMSK                       0x3fff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG8_PRIM_CONTEXT_VEC_16_SHFT                          0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG9_ADDR                                       (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000124)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG9_RMSK                                        0xfffffff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG9_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG9_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CFG_REG9_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG9_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG9_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG9_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG9_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG9_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CFG_REG9_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG9_PRIM_CONTEXT_VEC_19_BMSK                    0xfffc000
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG9_PRIM_CONTEXT_VEC_19_SHFT                          0xe
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG9_PRIM_CONTEXT_VEC_18_BMSK                       0x3fff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG9_PRIM_CONTEXT_VEC_18_SHFT                          0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG10_ADDR                                      (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000128)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG10_RMSK                                       0xfffffff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG10_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG10_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CFG_REG10_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG10_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG10_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG10_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG10_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG10_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CFG_REG10_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG10_PRIM_CONTEXT_VEC_21_BMSK                   0xfffc000
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG10_PRIM_CONTEXT_VEC_21_SHFT                         0xe
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG10_PRIM_CONTEXT_VEC_20_BMSK                      0x3fff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG10_PRIM_CONTEXT_VEC_20_SHFT                         0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG11_ADDR                                      (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x0000012c)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG11_RMSK                                       0xfffffff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG11_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG11_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CFG_REG11_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG11_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG11_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG11_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG11_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG11_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CFG_REG11_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG11_PRIM_CONTEXT_VEC_23_BMSK                   0xfffc000
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG11_PRIM_CONTEXT_VEC_23_SHFT                         0xe
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG11_PRIM_CONTEXT_VEC_22_BMSK                      0x3fff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG11_PRIM_CONTEXT_VEC_22_SHFT                         0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_ADDR                                      (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000130)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_RMSK                                          0x7fff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_COND_COMP_IN_VEC_BMSK                         0x7f80
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_COND_COMP_IN_VEC_SHFT                            0x7
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_DATA_CNT_MODE_BMSK                              0x40
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_DATA_CNT_MODE_SHFT                               0x6
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_DATA_CNT_EN_BITS_BMSK                           0x3c
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_DATA_CNT_EN_BITS_SHFT                            0x2
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_EXT_ARB_EN_BMSK                                  0x2
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_EXT_ARB_EN_SHFT                                  0x1
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_PROG_RAM_SEC_WORD_EN_BMSK                        0x1
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG12_PROG_RAM_SEC_WORD_EN_SHFT                        0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG13_ADDR                                      (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000134)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG13_RMSK                                         0x3ffff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG13_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG13_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CFG_REG13_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG13_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG13_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG13_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG13_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG13_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG13_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CFG_REG13_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG13_TX_PARAM_TABLE_VEC_1_BMSK                    0x3fe00
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG13_TX_PARAM_TABLE_VEC_1_SHFT                        0x9
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG13_TX_PARAM_TABLE_VEC_0_BMSK                      0x1ff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG13_TX_PARAM_TABLE_VEC_0_SHFT                        0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG14_ADDR                                      (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000138)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG14_RMSK                                         0x3ffff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG14_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG14_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CFG_REG14_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG14_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG14_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG14_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG14_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG14_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG14_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CFG_REG14_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG14_TX_PARAM_TABLE_VEC_3_BMSK                    0x3fe00
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG14_TX_PARAM_TABLE_VEC_3_SHFT                        0x9
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG14_TX_PARAM_TABLE_VEC_2_BMSK                      0x1ff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG14_TX_PARAM_TABLE_VEC_2_SHFT                        0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG15_ADDR                                      (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x0000013c)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG15_RMSK                                         0xfffff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG15_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG15_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CFG_REG15_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG15_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG15_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG15_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG15_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG15_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG15_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CFG_REG15_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG15_TX_DATA_TABLE_VEC_1_BMSK                     0xffc00
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG15_TX_DATA_TABLE_VEC_1_SHFT                         0xa
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG15_TX_DATA_TABLE_VEC_0_BMSK                       0x3ff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG15_TX_DATA_TABLE_VEC_0_SHFT                         0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG16_ADDR                                      (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000140)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG16_RMSK                                         0xfffff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG16_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG16_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CFG_REG16_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG16_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG16_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG16_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG16_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG16_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG16_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CFG_REG16_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG16_TX_DATA_TABLE_VEC_3_BMSK                     0xffc00
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG16_TX_DATA_TABLE_VEC_3_SHFT                         0xa
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG16_TX_DATA_TABLE_VEC_2_BMSK                       0x3ff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG16_TX_DATA_TABLE_VEC_2_SHFT                         0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_ADDR                                      (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000144)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_RMSK                                        0x3fffff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_RX_SIN_SEL_BMSK                             0x380000
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_RX_SIN_SEL_SHFT                                 0x13
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_SOE_EDGE_SEL_BMSK                         0x40000
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_SOE_EDGE_SEL_SHFT                            0x12
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_SOUT_EDGE_SEL_BMSK                        0x20000
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_SOUT_EDGE_SEL_SHFT                           0x11
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_DEFAULT_PRIM_SOE_VALUE_BMSK               0x10000
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_DEFAULT_PRIM_SOE_VALUE_SHFT                  0x10
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_DEFAULT_PRIM_SOUT_VALUE_BMSK               0x8000
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_DEFAULT_PRIM_SOUT_VALUE_SHFT                  0xf
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_DEFAULT_SOE_VALUE_BMSK                     0x4000
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_DEFAULT_SOE_VALUE_SHFT                        0xe
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_DEFAULT_SOUT_VALUE_BMSK                    0x2000
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_DEFAULT_SOUT_VALUE_SHFT                       0xd
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_CONST1_EFF_SIZE_BMSK                       0x1c00
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_CONST1_EFF_SIZE_SHFT                          0xa
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_CONST1_REG_BMSK                             0x3fc
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_CONST1_REG_SHFT                               0x2
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_PAR_MODE_BMSK                                 0x2
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_PAR_MODE_SHFT                                 0x1
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_PAR_CALC_EN_BMSK                              0x1
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG17_TX_PAR_CALC_EN_SHFT                              0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_ADDR                                      (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000148)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_RMSK                                           0xfff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_IN          \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_INM(m)      \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_ADDR, m)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_OUT(v)      \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_ADDR,v)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_ADDR,m,v,HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_IN)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_DRIVE_DEFAULT_ON_START_EN_BMSK                 0x800
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_DRIVE_DEFAULT_ON_START_EN_SHFT                   0xb
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_EXT_SECURITY_EN_BMSK                           0x400
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_EXT_SECURITY_EN_SHFT                             0xa
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_DATA_CNT_INIT_VALUE_INCR_BMSK                  0x200
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_DATA_CNT_INIT_VALUE_INCR_SHFT                    0x9
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_TX_THRESHOLD_BMSK                              0x180
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_TX_THRESHOLD_SHFT                                0x7
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_IO2_CONST_EFF_SIZE_BMSK                         0x40
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_IO2_CONST_EFF_SIZE_SHFT                          0x6
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_IO2_CONST_REG_BMSK                              0x30
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_IO2_CONST_REG_SHFT                               0x4
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_IO2_DEFAULT_PRIM_SOE_VALUE_BMSK                  0x8
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_IO2_DEFAULT_PRIM_SOE_VALUE_SHFT                  0x3
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_IO2_DEFAULT_PRIM_SOUT_VALUE_BMSK                 0x4
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_IO2_DEFAULT_PRIM_SOUT_VALUE_SHFT                 0x2
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_IO2_DEFAULT_SOE_VALUE_BMSK                       0x2
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_IO2_DEFAULT_SOE_VALUE_SHFT                       0x1
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_IO2_DEFAULT_SOUT_VALUE_BMSK                      0x1
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_REG18_IO2_DEFAULT_SOUT_VALUE_SHFT                      0x0

#define HWIO_PMIC_ARB_SPMI_GENI_CFG_RAMn_ADDR(n)                                    (PMIC_ARB_SPMI_SPMI_GENI_CFG_REG_BASE      + 0x00000200 + 0x4 * (n))
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_RAMn_RMSK                                         0x1fffff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_RAMn_MAXn                                              255
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_RAMn_INI(n)        \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_RAMn_ADDR(n), HWIO_PMIC_ARB_SPMI_GENI_CFG_RAMn_RMSK)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_RAMn_INMI(n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_SPMI_GENI_CFG_RAMn_ADDR(n), mask)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_RAMn_OUTI(n,val)    \
        out_dword(HWIO_PMIC_ARB_SPMI_GENI_CFG_RAMn_ADDR(n),val)
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_RAMn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_ARB_SPMI_GENI_CFG_RAMn_ADDR(n),mask,val,HWIO_PMIC_ARB_SPMI_GENI_CFG_RAMn_INI(n))
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_RAMn_TBD_BMSK                                     0x1fffff
#define HWIO_PMIC_ARB_SPMI_GENI_CFG_RAMn_TBD_SHFT                                          0x0


#endif /* __SPMICOREHWIO_H__ */
