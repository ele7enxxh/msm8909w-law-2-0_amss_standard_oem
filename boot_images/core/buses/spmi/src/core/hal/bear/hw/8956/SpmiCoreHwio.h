#ifndef __SPMICOREHWIO_H__
#define __SPMICOREHWIO_H__
/*
===========================================================================
*/
/**
  @file SpmiCoreHwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MSM8956 (Eldarion) [eldarion_v1.0_p2q1r62]
 
  This file contains HWIO register definitions for the following modules:
    SPMI_GENI_CFG
    SPMI_CFG
    SPMI_PIC

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

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/spmi/src/core/hal/bear/hw/8956/SpmiCoreHwio.h#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: SPMI_GENI_CFG
 *--------------------------------------------------------------------------*/

#define SPMI_GENI_CFG_REG_BASE                                             (PMIC_ARB_BASE      + 0x0000a000)

#define HWIO_SPMI_GENI_CLK_CTRL_ADDR                                       (SPMI_GENI_CFG_REG_BASE      + 0x00000000)
#define HWIO_SPMI_GENI_CLK_CTRL_RMSK                                              0x1
#define HWIO_SPMI_GENI_CLK_CTRL_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CLK_CTRL_ADDR, HWIO_SPMI_GENI_CLK_CTRL_RMSK)
#define HWIO_SPMI_GENI_CLK_CTRL_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CLK_CTRL_ADDR, m)
#define HWIO_SPMI_GENI_CLK_CTRL_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CLK_CTRL_ADDR,v)
#define HWIO_SPMI_GENI_CLK_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CLK_CTRL_ADDR,m,v,HWIO_SPMI_GENI_CLK_CTRL_IN)
#define HWIO_SPMI_GENI_CLK_CTRL_SER_CLK_SEL_BMSK                                  0x1
#define HWIO_SPMI_GENI_CLK_CTRL_SER_CLK_SEL_SHFT                                  0x0

#define HWIO_SPMI_GENI_HW_VERSION_ADDR                                     (SPMI_GENI_CFG_REG_BASE      + 0x00000004)
#define HWIO_SPMI_GENI_HW_VERSION_RMSK                                     0xffffffff
#define HWIO_SPMI_GENI_HW_VERSION_IN          \
        in_dword_masked(HWIO_SPMI_GENI_HW_VERSION_ADDR, HWIO_SPMI_GENI_HW_VERSION_RMSK)
#define HWIO_SPMI_GENI_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_HW_VERSION_ADDR, m)
#define HWIO_SPMI_GENI_HW_VERSION_MAJOR_BMSK                               0xf0000000
#define HWIO_SPMI_GENI_HW_VERSION_MAJOR_SHFT                                     0x1c
#define HWIO_SPMI_GENI_HW_VERSION_MINOR_BMSK                                0xfff0000
#define HWIO_SPMI_GENI_HW_VERSION_MINOR_SHFT                                     0x10
#define HWIO_SPMI_GENI_HW_VERSION_STEP_BMSK                                    0xffff
#define HWIO_SPMI_GENI_HW_VERSION_STEP_SHFT                                       0x0

#define HWIO_SPMI_GENI_FW_REVISION_ADDR                                    (SPMI_GENI_CFG_REG_BASE      + 0x00000008)
#define HWIO_SPMI_GENI_FW_REVISION_RMSK                                        0xffff
#define HWIO_SPMI_GENI_FW_REVISION_IN          \
        in_dword_masked(HWIO_SPMI_GENI_FW_REVISION_ADDR, HWIO_SPMI_GENI_FW_REVISION_RMSK)
#define HWIO_SPMI_GENI_FW_REVISION_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_FW_REVISION_ADDR, m)
#define HWIO_SPMI_GENI_FW_REVISION_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_FW_REVISION_ADDR,v)
#define HWIO_SPMI_GENI_FW_REVISION_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_FW_REVISION_ADDR,m,v,HWIO_SPMI_GENI_FW_REVISION_IN)
#define HWIO_SPMI_GENI_FW_REVISION_PROTOCOL_BMSK                               0xffc0
#define HWIO_SPMI_GENI_FW_REVISION_PROTOCOL_SHFT                                  0x6
#define HWIO_SPMI_GENI_FW_REVISION_VERSION_BMSK                                  0x3f
#define HWIO_SPMI_GENI_FW_REVISION_VERSION_SHFT                                   0x0

#define HWIO_SPMI_GENI_S_FW_REVISION_ADDR                                  (SPMI_GENI_CFG_REG_BASE      + 0x0000000c)
#define HWIO_SPMI_GENI_S_FW_REVISION_RMSK                                      0xffff
#define HWIO_SPMI_GENI_S_FW_REVISION_IN          \
        in_dword_masked(HWIO_SPMI_GENI_S_FW_REVISION_ADDR, HWIO_SPMI_GENI_S_FW_REVISION_RMSK)
#define HWIO_SPMI_GENI_S_FW_REVISION_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_S_FW_REVISION_ADDR, m)
#define HWIO_SPMI_GENI_S_FW_REVISION_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_S_FW_REVISION_ADDR,v)
#define HWIO_SPMI_GENI_S_FW_REVISION_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_S_FW_REVISION_ADDR,m,v,HWIO_SPMI_GENI_S_FW_REVISION_IN)
#define HWIO_SPMI_GENI_S_FW_REVISION_PROTOCOL_BMSK                             0xffc0
#define HWIO_SPMI_GENI_S_FW_REVISION_PROTOCOL_SHFT                                0x6
#define HWIO_SPMI_GENI_S_FW_REVISION_VERSION_BMSK                                0x3f
#define HWIO_SPMI_GENI_S_FW_REVISION_VERSION_SHFT                                 0x0

#define HWIO_SPMI_GENI_FORCE_DEFAULT_REG_ADDR                              (SPMI_GENI_CFG_REG_BASE      + 0x00000010)
#define HWIO_SPMI_GENI_FORCE_DEFAULT_REG_RMSK                                     0x1
#define HWIO_SPMI_GENI_FORCE_DEFAULT_REG_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_FORCE_DEFAULT_REG_ADDR,v)
#define HWIO_SPMI_GENI_FORCE_DEFAULT_REG_FORCE_DEFAULT_BMSK                       0x1
#define HWIO_SPMI_GENI_FORCE_DEFAULT_REG_FORCE_DEFAULT_SHFT                       0x0

#define HWIO_SPMI_GENI_OUTPUT_CTRL_ADDR                                    (SPMI_GENI_CFG_REG_BASE      + 0x00000014)
#define HWIO_SPMI_GENI_OUTPUT_CTRL_RMSK                                           0x7
#define HWIO_SPMI_GENI_OUTPUT_CTRL_IN          \
        in_dword_masked(HWIO_SPMI_GENI_OUTPUT_CTRL_ADDR, HWIO_SPMI_GENI_OUTPUT_CTRL_RMSK)
#define HWIO_SPMI_GENI_OUTPUT_CTRL_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_OUTPUT_CTRL_ADDR, m)
#define HWIO_SPMI_GENI_OUTPUT_CTRL_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_OUTPUT_CTRL_ADDR,v)
#define HWIO_SPMI_GENI_OUTPUT_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_OUTPUT_CTRL_ADDR,m,v,HWIO_SPMI_GENI_OUTPUT_CTRL_IN)
#define HWIO_SPMI_GENI_OUTPUT_CTRL_SOE2_EN_BMSK                                   0x4
#define HWIO_SPMI_GENI_OUTPUT_CTRL_SOE2_EN_SHFT                                   0x2
#define HWIO_SPMI_GENI_OUTPUT_CTRL_SOE1_EN_BMSK                                   0x2
#define HWIO_SPMI_GENI_OUTPUT_CTRL_SOE1_EN_SHFT                                   0x1
#define HWIO_SPMI_GENI_OUTPUT_CTRL_SOE0_EN_BMSK                                   0x1
#define HWIO_SPMI_GENI_OUTPUT_CTRL_SOE0_EN_SHFT                                   0x0

#define HWIO_SPMI_GENI_CGC_CTRL_ADDR                                       (SPMI_GENI_CFG_REG_BASE      + 0x00000018)
#define HWIO_SPMI_GENI_CGC_CTRL_RMSK                                            0x37f
#define HWIO_SPMI_GENI_CGC_CTRL_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CGC_CTRL_ADDR, HWIO_SPMI_GENI_CGC_CTRL_RMSK)
#define HWIO_SPMI_GENI_CGC_CTRL_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CGC_CTRL_ADDR, m)
#define HWIO_SPMI_GENI_CGC_CTRL_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CGC_CTRL_ADDR,v)
#define HWIO_SPMI_GENI_CGC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CGC_CTRL_ADDR,m,v,HWIO_SPMI_GENI_CGC_CTRL_IN)
#define HWIO_SPMI_GENI_CGC_CTRL_PROG_RAM_SCLK_OFF_BMSK                          0x200
#define HWIO_SPMI_GENI_CGC_CTRL_PROG_RAM_SCLK_OFF_SHFT                            0x9
#define HWIO_SPMI_GENI_CGC_CTRL_PROG_RAM_HCLK_OFF_BMSK                          0x100
#define HWIO_SPMI_GENI_CGC_CTRL_PROG_RAM_HCLK_OFF_SHFT                            0x8
#define HWIO_SPMI_GENI_CGC_CTRL_EXT_CLK_CGC_ON_BMSK                              0x40
#define HWIO_SPMI_GENI_CGC_CTRL_EXT_CLK_CGC_ON_SHFT                               0x6
#define HWIO_SPMI_GENI_CGC_CTRL_RX_CLK_CGC_ON_BMSK                               0x20
#define HWIO_SPMI_GENI_CGC_CTRL_RX_CLK_CGC_ON_SHFT                                0x5
#define HWIO_SPMI_GENI_CGC_CTRL_TX_CLK_CGC_ON_BMSK                               0x10
#define HWIO_SPMI_GENI_CGC_CTRL_TX_CLK_CGC_ON_SHFT                                0x4
#define HWIO_SPMI_GENI_CGC_CTRL_SCLK_CGC_ON_BMSK                                  0x8
#define HWIO_SPMI_GENI_CGC_CTRL_SCLK_CGC_ON_SHFT                                  0x3
#define HWIO_SPMI_GENI_CGC_CTRL_DATA_AHB_CLK_CGC_ON_BMSK                          0x4
#define HWIO_SPMI_GENI_CGC_CTRL_DATA_AHB_CLK_CGC_ON_SHFT                          0x2
#define HWIO_SPMI_GENI_CGC_CTRL_CFG_AHB_WR_CLK_CGC_ON_BMSK                        0x2
#define HWIO_SPMI_GENI_CGC_CTRL_CFG_AHB_WR_CLK_CGC_ON_SHFT                        0x1
#define HWIO_SPMI_GENI_CGC_CTRL_CFG_AHB_CLK_CGC_ON_BMSK                           0x1
#define HWIO_SPMI_GENI_CGC_CTRL_CFG_AHB_CLK_CGC_ON_SHFT                           0x0

#define HWIO_SPMI_GENI_CHAR_CFG_ADDR                                       (SPMI_GENI_CFG_REG_BASE      + 0x0000001c)
#define HWIO_SPMI_GENI_CHAR_CFG_RMSK                                          0xfff11
#define HWIO_SPMI_GENI_CHAR_CFG_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CHAR_CFG_ADDR, HWIO_SPMI_GENI_CHAR_CFG_RMSK)
#define HWIO_SPMI_GENI_CHAR_CFG_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CHAR_CFG_ADDR, m)
#define HWIO_SPMI_GENI_CHAR_CFG_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CHAR_CFG_ADDR,v)
#define HWIO_SPMI_GENI_CHAR_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CHAR_CFG_ADDR,m,v,HWIO_SPMI_GENI_CHAR_CFG_IN)
#define HWIO_SPMI_GENI_CHAR_CFG_CHAR_MODE_BMSK                                0xff000
#define HWIO_SPMI_GENI_CHAR_CFG_CHAR_MODE_SHFT                                    0xc
#define HWIO_SPMI_GENI_CHAR_CFG_CHAR_STATUS_BMSK                                0xf00
#define HWIO_SPMI_GENI_CHAR_CFG_CHAR_STATUS_SHFT                                  0x8
#define HWIO_SPMI_GENI_CHAR_CFG_DIRECTION_BMSK                                   0x10
#define HWIO_SPMI_GENI_CHAR_CFG_DIRECTION_SHFT                                    0x4
#define HWIO_SPMI_GENI_CHAR_CFG_ENABLE_BMSK                                       0x1
#define HWIO_SPMI_GENI_CHAR_CFG_ENABLE_SHFT                                       0x0

#define HWIO_SPMI_GENI_CHAR_DATA_n_ADDR(n)                                 (SPMI_GENI_CFG_REG_BASE      + 0x00000020 + 0x4 * (n))
#define HWIO_SPMI_GENI_CHAR_DATA_n_RMSK                                        0xffff
#define HWIO_SPMI_GENI_CHAR_DATA_n_MAXn                                             1
#define HWIO_SPMI_GENI_CHAR_DATA_n_INI(n)        \
        in_dword_masked(HWIO_SPMI_GENI_CHAR_DATA_n_ADDR(n), HWIO_SPMI_GENI_CHAR_DATA_n_RMSK)
#define HWIO_SPMI_GENI_CHAR_DATA_n_INMI(n,mask)    \
        in_dword_masked(HWIO_SPMI_GENI_CHAR_DATA_n_ADDR(n), mask)
#define HWIO_SPMI_GENI_CHAR_DATA_n_OUTI(n,val)    \
        out_dword(HWIO_SPMI_GENI_CHAR_DATA_n_ADDR(n),val)
#define HWIO_SPMI_GENI_CHAR_DATA_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CHAR_DATA_n_ADDR(n),mask,val,HWIO_SPMI_GENI_CHAR_DATA_n_INI(n))
#define HWIO_SPMI_GENI_CHAR_DATA_n_DIN_ACTUAL_BMSK                             0xff00
#define HWIO_SPMI_GENI_CHAR_DATA_n_DIN_ACTUAL_SHFT                                0x8
#define HWIO_SPMI_GENI_CHAR_DATA_n_DOUT_DATA_DIN_EXP_BMSK                        0xff
#define HWIO_SPMI_GENI_CHAR_DATA_n_DOUT_DATA_DIN_EXP_SHFT                         0x0

#define HWIO_SPMI_GENI_CTRL_ADDR                                           (SPMI_GENI_CFG_REG_BASE      + 0x00000028)
#define HWIO_SPMI_GENI_CTRL_RMSK                                                  0x3
#define HWIO_SPMI_GENI_CTRL_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CTRL_ADDR, HWIO_SPMI_GENI_CTRL_RMSK)
#define HWIO_SPMI_GENI_CTRL_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CTRL_ADDR, m)
#define HWIO_SPMI_GENI_CTRL_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CTRL_ADDR,v)
#define HWIO_SPMI_GENI_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CTRL_ADDR,m,v,HWIO_SPMI_GENI_CTRL_IN)
#define HWIO_SPMI_GENI_CTRL_S_GENI_DISABLE_BMSK                                   0x2
#define HWIO_SPMI_GENI_CTRL_S_GENI_DISABLE_SHFT                                   0x1
#define HWIO_SPMI_GENI_CTRL_M_GENI_DISABLE_BMSK                                   0x1
#define HWIO_SPMI_GENI_CTRL_M_GENI_DISABLE_SHFT                                   0x0

#define HWIO_SPMI_GENI_STATUS_ADDR                                         (SPMI_GENI_CFG_REG_BASE      + 0x0000002c)
#define HWIO_SPMI_GENI_STATUS_RMSK                                           0x1fffff
#define HWIO_SPMI_GENI_STATUS_IN          \
        in_dword_masked(HWIO_SPMI_GENI_STATUS_ADDR, HWIO_SPMI_GENI_STATUS_RMSK)
#define HWIO_SPMI_GENI_STATUS_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_STATUS_ADDR, m)
#define HWIO_SPMI_GENI_STATUS_S_GENI_CMD_FSM_STATE_BMSK                      0x1f0000
#define HWIO_SPMI_GENI_STATUS_S_GENI_CMD_FSM_STATE_SHFT                          0x10
#define HWIO_SPMI_GENI_STATUS_NOT_USED_BITS_3_BMSK                             0xe000
#define HWIO_SPMI_GENI_STATUS_NOT_USED_BITS_3_SHFT                                0xd
#define HWIO_SPMI_GENI_STATUS_S_GENI_CMD_ACTIVE_BMSK                           0x1000
#define HWIO_SPMI_GENI_STATUS_S_GENI_CMD_ACTIVE_SHFT                              0xc
#define HWIO_SPMI_GENI_STATUS_NOT_USED_BITS_2_BMSK                              0xe00
#define HWIO_SPMI_GENI_STATUS_NOT_USED_BITS_2_SHFT                                0x9
#define HWIO_SPMI_GENI_STATUS_M_GENI_CMD_FSM_STATE_BMSK                         0x1f0
#define HWIO_SPMI_GENI_STATUS_M_GENI_CMD_FSM_STATE_SHFT                           0x4
#define HWIO_SPMI_GENI_STATUS_NOT_USED_BITS_1_BMSK                                0xe
#define HWIO_SPMI_GENI_STATUS_NOT_USED_BITS_1_SHFT                                0x1
#define HWIO_SPMI_GENI_STATUS_M_GENI_CMD_ACTIVE_BMSK                              0x1
#define HWIO_SPMI_GENI_STATUS_M_GENI_CMD_ACTIVE_SHFT                              0x0

#define HWIO_SPMI_GENI_TEST_BUS_CTRL_ADDR                                  (SPMI_GENI_CFG_REG_BASE      + 0x00000030)
#define HWIO_SPMI_GENI_TEST_BUS_CTRL_RMSK                                        0x1f
#define HWIO_SPMI_GENI_TEST_BUS_CTRL_IN          \
        in_dword_masked(HWIO_SPMI_GENI_TEST_BUS_CTRL_ADDR, HWIO_SPMI_GENI_TEST_BUS_CTRL_RMSK)
#define HWIO_SPMI_GENI_TEST_BUS_CTRL_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_TEST_BUS_CTRL_ADDR, m)
#define HWIO_SPMI_GENI_TEST_BUS_CTRL_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_TEST_BUS_CTRL_ADDR,v)
#define HWIO_SPMI_GENI_TEST_BUS_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_TEST_BUS_CTRL_ADDR,m,v,HWIO_SPMI_GENI_TEST_BUS_CTRL_IN)
#define HWIO_SPMI_GENI_TEST_BUS_CTRL_TEST_BUS_SEL_BMSK                           0x1f
#define HWIO_SPMI_GENI_TEST_BUS_CTRL_TEST_BUS_SEL_SHFT                            0x0

#define HWIO_SPMI_GENI_SER_CLK_CFG_ADDR                                    (SPMI_GENI_CFG_REG_BASE      + 0x00000034)
#define HWIO_SPMI_GENI_SER_CLK_CFG_RMSK                                     0x3ffffff
#define HWIO_SPMI_GENI_SER_CLK_CFG_IN          \
        in_dword_masked(HWIO_SPMI_GENI_SER_CLK_CFG_ADDR, HWIO_SPMI_GENI_SER_CLK_CFG_RMSK)
#define HWIO_SPMI_GENI_SER_CLK_CFG_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_SER_CLK_CFG_ADDR, m)
#define HWIO_SPMI_GENI_SER_CLK_CFG_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_SER_CLK_CFG_ADDR,v)
#define HWIO_SPMI_GENI_SER_CLK_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_SER_CLK_CFG_ADDR,m,v,HWIO_SPMI_GENI_SER_CLK_CFG_IN)
#define HWIO_SPMI_GENI_SER_CLK_CFG_TX_CLK_DIV_VALUE_BMSK                    0x3ffc000
#define HWIO_SPMI_GENI_SER_CLK_CFG_TX_CLK_DIV_VALUE_SHFT                          0xe
#define HWIO_SPMI_GENI_SER_CLK_CFG_RX_CLK_DIV_VALUE_BMSK                       0x3ffc
#define HWIO_SPMI_GENI_SER_CLK_CFG_RX_CLK_DIV_VALUE_SHFT                          0x2
#define HWIO_SPMI_GENI_SER_CLK_CFG_TX_SER_CLK_EN_BMSK                             0x2
#define HWIO_SPMI_GENI_SER_CLK_CFG_TX_SER_CLK_EN_SHFT                             0x1
#define HWIO_SPMI_GENI_SER_CLK_CFG_RX_SER_CLK_EN_BMSK                             0x1
#define HWIO_SPMI_GENI_SER_CLK_CFG_RX_SER_CLK_EN_SHFT                             0x0

#define HWIO_SPMI_GENI_TX_WATERMARK_REG_ADDR                               (SPMI_GENI_CFG_REG_BASE      + 0x00000038)
#define HWIO_SPMI_GENI_TX_WATERMARK_REG_RMSK                                      0xf
#define HWIO_SPMI_GENI_TX_WATERMARK_REG_IN          \
        in_dword_masked(HWIO_SPMI_GENI_TX_WATERMARK_REG_ADDR, HWIO_SPMI_GENI_TX_WATERMARK_REG_RMSK)
#define HWIO_SPMI_GENI_TX_WATERMARK_REG_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_TX_WATERMARK_REG_ADDR, m)
#define HWIO_SPMI_GENI_TX_WATERMARK_REG_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_TX_WATERMARK_REG_ADDR,v)
#define HWIO_SPMI_GENI_TX_WATERMARK_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_TX_WATERMARK_REG_ADDR,m,v,HWIO_SPMI_GENI_TX_WATERMARK_REG_IN)
#define HWIO_SPMI_GENI_TX_WATERMARK_REG_TX_WATERMARK_BMSK                         0xf
#define HWIO_SPMI_GENI_TX_WATERMARK_REG_TX_WATERMARK_SHFT                         0x0

#define HWIO_SPMI_GENI_RX_WATERMARK_REG_ADDR                               (SPMI_GENI_CFG_REG_BASE      + 0x0000003c)
#define HWIO_SPMI_GENI_RX_WATERMARK_REG_RMSK                                      0xf
#define HWIO_SPMI_GENI_RX_WATERMARK_REG_IN          \
        in_dword_masked(HWIO_SPMI_GENI_RX_WATERMARK_REG_ADDR, HWIO_SPMI_GENI_RX_WATERMARK_REG_RMSK)
#define HWIO_SPMI_GENI_RX_WATERMARK_REG_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_RX_WATERMARK_REG_ADDR, m)
#define HWIO_SPMI_GENI_RX_WATERMARK_REG_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_RX_WATERMARK_REG_ADDR,v)
#define HWIO_SPMI_GENI_RX_WATERMARK_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_RX_WATERMARK_REG_ADDR,m,v,HWIO_SPMI_GENI_RX_WATERMARK_REG_IN)
#define HWIO_SPMI_GENI_RX_WATERMARK_REG_RX_WATERMARK_BMSK                         0xf
#define HWIO_SPMI_GENI_RX_WATERMARK_REG_RX_WATERMARK_SHFT                         0x0

#define HWIO_SPMI_GENI_STOP_REG_ADDR                                       (SPMI_GENI_CFG_REG_BASE      + 0x00000040)
#define HWIO_SPMI_GENI_STOP_REG_RMSK                                              0x3
#define HWIO_SPMI_GENI_STOP_REG_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_STOP_REG_ADDR,v)
#define HWIO_SPMI_GENI_STOP_REG_S_GENI_STOP_BMSK                                  0x2
#define HWIO_SPMI_GENI_STOP_REG_S_GENI_STOP_SHFT                                  0x1
#define HWIO_SPMI_GENI_STOP_REG_M_GENI_STOP_BMSK                                  0x1
#define HWIO_SPMI_GENI_STOP_REG_M_GENI_STOP_SHFT                                  0x0

#define HWIO_SPMI_GENI_SW_COMP_REG_ADDR                                    (SPMI_GENI_CFG_REG_BASE      + 0x00000044)
#define HWIO_SPMI_GENI_SW_COMP_REG_RMSK                                           0x7
#define HWIO_SPMI_GENI_SW_COMP_REG_IN          \
        in_dword_masked(HWIO_SPMI_GENI_SW_COMP_REG_ADDR, HWIO_SPMI_GENI_SW_COMP_REG_RMSK)
#define HWIO_SPMI_GENI_SW_COMP_REG_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_SW_COMP_REG_ADDR, m)
#define HWIO_SPMI_GENI_SW_COMP_REG_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_SW_COMP_REG_ADDR,v)
#define HWIO_SPMI_GENI_SW_COMP_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_SW_COMP_REG_ADDR,m,v,HWIO_SPMI_GENI_SW_COMP_REG_IN)
#define HWIO_SPMI_GENI_SW_COMP_REG_SW_COMP_BMSK                                   0x7
#define HWIO_SPMI_GENI_SW_COMP_REG_SW_COMP_SHFT                                   0x0

#define HWIO_SPMI_GENI_PROG_ROM_CTRL_REG_ADDR                              (SPMI_GENI_CFG_REG_BASE      + 0x00000048)
#define HWIO_SPMI_GENI_PROG_ROM_CTRL_REG_RMSK                                     0x1
#define HWIO_SPMI_GENI_PROG_ROM_CTRL_REG_IN          \
        in_dword_masked(HWIO_SPMI_GENI_PROG_ROM_CTRL_REG_ADDR, HWIO_SPMI_GENI_PROG_ROM_CTRL_REG_RMSK)
#define HWIO_SPMI_GENI_PROG_ROM_CTRL_REG_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_PROG_ROM_CTRL_REG_ADDR, m)
#define HWIO_SPMI_GENI_PROG_ROM_CTRL_REG_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_PROG_ROM_CTRL_REG_ADDR,v)
#define HWIO_SPMI_GENI_PROG_ROM_CTRL_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_PROG_ROM_CTRL_REG_ADDR,m,v,HWIO_SPMI_GENI_PROG_ROM_CTRL_REG_IN)
#define HWIO_SPMI_GENI_PROG_ROM_CTRL_REG_PROG_ROM_EN_BMSK                         0x1
#define HWIO_SPMI_GENI_PROG_ROM_CTRL_REG_PROG_ROM_EN_SHFT                         0x0

#define HWIO_SPMI_GENI_CFG_REG0_ADDR                                       (SPMI_GENI_CFG_REG_BASE      + 0x00000100)
#define HWIO_SPMI_GENI_CFG_REG0_RMSK                                       0x3ffff9ff
#define HWIO_SPMI_GENI_CFG_REG0_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG0_ADDR, HWIO_SPMI_GENI_CFG_REG0_RMSK)
#define HWIO_SPMI_GENI_CFG_REG0_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG0_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG0_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG0_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG0_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG0_IN)
#define HWIO_SPMI_GENI_CFG_REG0_GENI_TX_MODE_BMSK                          0x20000000
#define HWIO_SPMI_GENI_CFG_REG0_GENI_TX_MODE_SHFT                                0x1d
#define HWIO_SPMI_GENI_CFG_REG0_COND_COMP_IN_0_SEL_RST_VAL_BMSK            0x1fe00000
#define HWIO_SPMI_GENI_CFG_REG0_COND_COMP_IN_0_SEL_RST_VAL_SHFT                  0x15
#define HWIO_SPMI_GENI_CFG_REG0_COND_COMP_IN_VEC_BMSK                        0x1fe000
#define HWIO_SPMI_GENI_CFG_REG0_COND_COMP_IN_VEC_SHFT                             0xd
#define HWIO_SPMI_GENI_CFG_REG0_M_DATA_CNT_INIT_VALUE_INCR_BMSK                0x1000
#define HWIO_SPMI_GENI_CFG_REG0_M_DATA_CNT_INIT_VALUE_INCR_SHFT                   0xc
#define HWIO_SPMI_GENI_CFG_REG0_DATA_CNT_MODE_BMSK                              0x800
#define HWIO_SPMI_GENI_CFG_REG0_DATA_CNT_MODE_SHFT                                0xb
#define HWIO_SPMI_GENI_CFG_REG0_M_DATA_CNT_EN_BITS_BMSK                         0x1f8
#define HWIO_SPMI_GENI_CFG_REG0_M_DATA_CNT_EN_BITS_SHFT                           0x3
#define HWIO_SPMI_GENI_CFG_REG0_EXT_SECURITY_EN_BMSK                              0x4
#define HWIO_SPMI_GENI_CFG_REG0_EXT_SECURITY_EN_SHFT                              0x2
#define HWIO_SPMI_GENI_CFG_REG0_EXT_ARB_EN_BMSK                                   0x2
#define HWIO_SPMI_GENI_CFG_REG0_EXT_ARB_EN_SHFT                                   0x1
#define HWIO_SPMI_GENI_CFG_REG0_M_PROG_RAM_SEC_WORD_EN_BMSK                       0x1
#define HWIO_SPMI_GENI_CFG_REG0_M_PROG_RAM_SEC_WORD_EN_SHFT                       0x0

#define HWIO_SPMI_GENI_CFG_REG1_ADDR                                       (SPMI_GENI_CFG_REG_BASE      + 0x00000104)
#define HWIO_SPMI_GENI_CFG_REG1_RMSK                                          0x3ffff
#define HWIO_SPMI_GENI_CFG_REG1_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG1_ADDR, HWIO_SPMI_GENI_CFG_REG1_RMSK)
#define HWIO_SPMI_GENI_CFG_REG1_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG1_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG1_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG1_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG1_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG1_IN)
#define HWIO_SPMI_GENI_CFG_REG1_TX_PARAM_TABLE_VEC_1_BMSK                     0x3fe00
#define HWIO_SPMI_GENI_CFG_REG1_TX_PARAM_TABLE_VEC_1_SHFT                         0x9
#define HWIO_SPMI_GENI_CFG_REG1_TX_PARAM_TABLE_VEC_0_BMSK                       0x1ff
#define HWIO_SPMI_GENI_CFG_REG1_TX_PARAM_TABLE_VEC_0_SHFT                         0x0

#define HWIO_SPMI_GENI_CFG_REG2_ADDR                                       (SPMI_GENI_CFG_REG_BASE      + 0x00000108)
#define HWIO_SPMI_GENI_CFG_REG2_RMSK                                          0x3ffff
#define HWIO_SPMI_GENI_CFG_REG2_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG2_ADDR, HWIO_SPMI_GENI_CFG_REG2_RMSK)
#define HWIO_SPMI_GENI_CFG_REG2_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG2_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG2_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG2_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG2_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG2_IN)
#define HWIO_SPMI_GENI_CFG_REG2_TX_PARAM_TABLE_VEC_3_BMSK                     0x3fe00
#define HWIO_SPMI_GENI_CFG_REG2_TX_PARAM_TABLE_VEC_3_SHFT                         0x9
#define HWIO_SPMI_GENI_CFG_REG2_TX_PARAM_TABLE_VEC_2_BMSK                       0x1ff
#define HWIO_SPMI_GENI_CFG_REG2_TX_PARAM_TABLE_VEC_2_SHFT                         0x0

#define HWIO_SPMI_GENI_CFG_REG3_ADDR                                       (SPMI_GENI_CFG_REG_BASE      + 0x0000010c)
#define HWIO_SPMI_GENI_CFG_REG3_RMSK                                          0xfffff
#define HWIO_SPMI_GENI_CFG_REG3_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG3_ADDR, HWIO_SPMI_GENI_CFG_REG3_RMSK)
#define HWIO_SPMI_GENI_CFG_REG3_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG3_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG3_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG3_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG3_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG3_IN)
#define HWIO_SPMI_GENI_CFG_REG3_TX_DATA_TABLE_VEC_1_BMSK                      0xffc00
#define HWIO_SPMI_GENI_CFG_REG3_TX_DATA_TABLE_VEC_1_SHFT                          0xa
#define HWIO_SPMI_GENI_CFG_REG3_TX_DATA_TABLE_VEC_0_BMSK                        0x3ff
#define HWIO_SPMI_GENI_CFG_REG3_TX_DATA_TABLE_VEC_0_SHFT                          0x0

#define HWIO_SPMI_GENI_CFG_REG4_ADDR                                       (SPMI_GENI_CFG_REG_BASE      + 0x00000110)
#define HWIO_SPMI_GENI_CFG_REG4_RMSK                                          0xfffff
#define HWIO_SPMI_GENI_CFG_REG4_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG4_ADDR, HWIO_SPMI_GENI_CFG_REG4_RMSK)
#define HWIO_SPMI_GENI_CFG_REG4_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG4_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG4_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG4_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG4_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG4_IN)
#define HWIO_SPMI_GENI_CFG_REG4_TX_DATA_TABLE_VEC_3_BMSK                      0xffc00
#define HWIO_SPMI_GENI_CFG_REG4_TX_DATA_TABLE_VEC_3_SHFT                          0xa
#define HWIO_SPMI_GENI_CFG_REG4_TX_DATA_TABLE_VEC_2_BMSK                        0x3ff
#define HWIO_SPMI_GENI_CFG_REG4_TX_DATA_TABLE_VEC_2_SHFT                          0x0

#define HWIO_SPMI_GENI_CFG_REG5_ADDR                                       (SPMI_GENI_CFG_REG_BASE      + 0x00000114)
#define HWIO_SPMI_GENI_CFG_REG5_RMSK                                       0x3fdc03ff
#define HWIO_SPMI_GENI_CFG_REG5_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG5_ADDR, HWIO_SPMI_GENI_CFG_REG5_RMSK)
#define HWIO_SPMI_GENI_CFG_REG5_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG5_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG5_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG5_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG5_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG5_IN)
#define HWIO_SPMI_GENI_CFG_REG5_DRIVE_DEFAULT_ON_START_EN_BMSK             0x20000000
#define HWIO_SPMI_GENI_CFG_REG5_DRIVE_DEFAULT_ON_START_EN_SHFT                   0x1d
#define HWIO_SPMI_GENI_CFG_REG5_TX_BP_MODE_BMSK                            0x10000000
#define HWIO_SPMI_GENI_CFG_REG5_TX_BP_MODE_SHFT                                  0x1c
#define HWIO_SPMI_GENI_CFG_REG5_TX_SOE_EDGE_SEL_BMSK                        0x8000000
#define HWIO_SPMI_GENI_CFG_REG5_TX_SOE_EDGE_SEL_SHFT                             0x1b
#define HWIO_SPMI_GENI_CFG_REG5_TX_SOUT_EDGE_SEL_BMSK                       0x4000000
#define HWIO_SPMI_GENI_CFG_REG5_TX_SOUT_EDGE_SEL_SHFT                            0x1a
#define HWIO_SPMI_GENI_CFG_REG5_TX_DEFAULT_PRIM_SOE_VALUE_BMSK              0x2000000
#define HWIO_SPMI_GENI_CFG_REG5_TX_DEFAULT_PRIM_SOE_VALUE_SHFT                   0x19
#define HWIO_SPMI_GENI_CFG_REG5_TX_DEFAULT_PRIM_SOUT_VALUE_BMSK             0x1000000
#define HWIO_SPMI_GENI_CFG_REG5_TX_DEFAULT_PRIM_SOUT_VALUE_SHFT                  0x18
#define HWIO_SPMI_GENI_CFG_REG5_TX_DEFAULT_SOE_VALUE_BMSK                    0x800000
#define HWIO_SPMI_GENI_CFG_REG5_TX_DEFAULT_SOE_VALUE_SHFT                        0x17
#define HWIO_SPMI_GENI_CFG_REG5_TX_DEFAULT_SOUT_VALUE_BMSK                   0x400000
#define HWIO_SPMI_GENI_CFG_REG5_TX_DEFAULT_SOUT_VALUE_SHFT                       0x16
#define HWIO_SPMI_GENI_CFG_REG5_TX_CONST1_EFF_SIZE_BMSK                      0x1c0000
#define HWIO_SPMI_GENI_CFG_REG5_TX_CONST1_EFF_SIZE_SHFT                          0x12
#define HWIO_SPMI_GENI_CFG_REG5_TX_CONST1_REG_BMSK                              0x3fc
#define HWIO_SPMI_GENI_CFG_REG5_TX_CONST1_REG_SHFT                                0x2
#define HWIO_SPMI_GENI_CFG_REG5_TX_PAR_MODE_BMSK                                  0x2
#define HWIO_SPMI_GENI_CFG_REG5_TX_PAR_MODE_SHFT                                  0x1
#define HWIO_SPMI_GENI_CFG_REG5_TX_PAR_CALC_EN_BMSK                               0x1
#define HWIO_SPMI_GENI_CFG_REG5_TX_PAR_CALC_EN_SHFT                               0x0

#define HWIO_SPMI_GENI_CFG_REG6_ADDR                                       (SPMI_GENI_CFG_REG_BASE      + 0x00000118)
#define HWIO_SPMI_GENI_CFG_REG6_RMSK                                       0xe3fbfd3f
#define HWIO_SPMI_GENI_CFG_REG6_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG6_ADDR, HWIO_SPMI_GENI_CFG_REG6_RMSK)
#define HWIO_SPMI_GENI_CFG_REG6_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG6_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG6_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG6_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG6_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG6_IN)
#define HWIO_SPMI_GENI_CFG_REG6_M_TIME_CNT_FR_DIV_BMSK                     0xc0000000
#define HWIO_SPMI_GENI_CFG_REG6_M_TIME_CNT_FR_DIV_SHFT                           0x1e
#define HWIO_SPMI_GENI_CFG_REG6_M_TIME_CNT_EN_FR_SEL_BMSK                  0x20000000
#define HWIO_SPMI_GENI_CFG_REG6_M_TIME_CNT_EN_FR_SEL_SHFT                        0x1d
#define HWIO_SPMI_GENI_CFG_REG6_M_GP_CNT_INIT_VAL_BMSK                      0x3e00000
#define HWIO_SPMI_GENI_CFG_REG6_M_GP_CNT_INIT_VAL_SHFT                           0x15
#define HWIO_SPMI_GENI_CFG_REG6_M_GP_CNT_DIRECTION_BMSK                      0x100000
#define HWIO_SPMI_GENI_CFG_REG6_M_GP_CNT_DIRECTION_SHFT                          0x14
#define HWIO_SPMI_GENI_CFG_REG6_TX_SOUT_MODUL_START_VAL_BMSK                  0x80000
#define HWIO_SPMI_GENI_CFG_REG6_TX_SOUT_MODUL_START_VAL_SHFT                     0x13
#define HWIO_SPMI_GENI_CFG_REG6_TX_SOUT_MODUL_STOP_DUR_BMSK                   0x30000
#define HWIO_SPMI_GENI_CFG_REG6_TX_SOUT_MODUL_STOP_DUR_SHFT                      0x10
#define HWIO_SPMI_GENI_CFG_REG6_TX_SOUT_MODUL_EN_BMSK                          0x8000
#define HWIO_SPMI_GENI_CFG_REG6_TX_SOUT_MODUL_EN_SHFT                             0xf
#define HWIO_SPMI_GENI_CFG_REG6_TX_LOAD_PAR_EN_BMSK                            0x4000
#define HWIO_SPMI_GENI_CFG_REG6_TX_LOAD_PAR_EN_SHFT                               0xe
#define HWIO_SPMI_GENI_CFG_REG6_TX_PARAM_PAR_EN_BMSK                           0x1000
#define HWIO_SPMI_GENI_CFG_REG6_TX_PARAM_PAR_EN_SHFT                              0xc
#define HWIO_SPMI_GENI_CFG_REG6_TX_FIFO_PAR_EN_BMSK                             0x400
#define HWIO_SPMI_GENI_CFG_REG6_TX_FIFO_PAR_EN_SHFT                               0xa
#define HWIO_SPMI_GENI_CFG_REG6_IO2_CONST_EFF_SIZE_BMSK                         0x100
#define HWIO_SPMI_GENI_CFG_REG6_IO2_CONST_EFF_SIZE_SHFT                           0x8
#define HWIO_SPMI_GENI_CFG_REG6_IO2_CONST_REG_BMSK                               0x30
#define HWIO_SPMI_GENI_CFG_REG6_IO2_CONST_REG_SHFT                                0x4
#define HWIO_SPMI_GENI_CFG_REG6_IO2_DEFAULT_PRIM_SOE_VALUE_BMSK                   0x8
#define HWIO_SPMI_GENI_CFG_REG6_IO2_DEFAULT_PRIM_SOE_VALUE_SHFT                   0x3
#define HWIO_SPMI_GENI_CFG_REG6_IO2_DEFAULT_PRIM_SOUT_VALUE_BMSK                  0x4
#define HWIO_SPMI_GENI_CFG_REG6_IO2_DEFAULT_PRIM_SOUT_VALUE_SHFT                  0x2
#define HWIO_SPMI_GENI_CFG_REG6_IO2_DEFAULT_SOE_VALUE_BMSK                        0x2
#define HWIO_SPMI_GENI_CFG_REG6_IO2_DEFAULT_SOE_VALUE_SHFT                        0x1
#define HWIO_SPMI_GENI_CFG_REG6_IO2_DEFAULT_SOUT_VALUE_BMSK                       0x1
#define HWIO_SPMI_GENI_CFG_REG6_IO2_DEFAULT_SOUT_VALUE_SHFT                       0x0

#define HWIO_SPMI_GENI_CFG_REG7_ADDR                                       (SPMI_GENI_CFG_REG_BASE      + 0x0000011c)
#define HWIO_SPMI_GENI_CFG_REG7_RMSK                                        0x7efffff
#define HWIO_SPMI_GENI_CFG_REG7_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG7_ADDR, HWIO_SPMI_GENI_CFG_REG7_RMSK)
#define HWIO_SPMI_GENI_CFG_REG7_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG7_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG7_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG7_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG7_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG7_IN)
#define HWIO_SPMI_GENI_CFG_REG7_GENI_RX_MODE_BMSK                           0x4000000
#define HWIO_SPMI_GENI_CFG_REG7_GENI_RX_MODE_SHFT                                0x1a
#define HWIO_SPMI_GENI_CFG_REG7_RX_PAR_MODE_BMSK                            0x2000000
#define HWIO_SPMI_GENI_CFG_REG7_RX_PAR_MODE_SHFT                                 0x19
#define HWIO_SPMI_GENI_CFG_REG7_RX_PAR_CALC_EN_BMSK                         0x1000000
#define HWIO_SPMI_GENI_CFG_REG7_RX_PAR_CALC_EN_SHFT                              0x18
#define HWIO_SPMI_GENI_CFG_REG7_RX_IO_SIN_SEL_BMSK                           0xe00000
#define HWIO_SPMI_GENI_CFG_REG7_RX_IO_SIN_SEL_SHFT                               0x15
#define HWIO_SPMI_GENI_CFG_REG7_RX_SIN_SEL_BMSK                               0xc0000
#define HWIO_SPMI_GENI_CFG_REG7_RX_SIN_SEL_SHFT                                  0x12
#define HWIO_SPMI_GENI_CFG_REG7_RX_DATA_SRC_BMSK                              0x20000
#define HWIO_SPMI_GENI_CFG_REG7_RX_DATA_SRC_SHFT                                 0x11
#define HWIO_SPMI_GENI_CFG_REG7_S_COND_COMP_IN_0_SEL_RST_VAL_BMSK             0x1fe00
#define HWIO_SPMI_GENI_CFG_REG7_S_COND_COMP_IN_0_SEL_RST_VAL_SHFT                 0x9
#define HWIO_SPMI_GENI_CFG_REG7_S_COND_COMP_IN_VEC_BMSK                         0x1fe
#define HWIO_SPMI_GENI_CFG_REG7_S_COND_COMP_IN_VEC_SHFT                           0x1
#define HWIO_SPMI_GENI_CFG_REG7_S_DATA_CNT_MODE_BMSK                              0x1
#define HWIO_SPMI_GENI_CFG_REG7_S_DATA_CNT_MODE_SHFT                              0x0

#define HWIO_SPMI_GENI_CFG_REG8_ADDR                                       (SPMI_GENI_CFG_REG_BASE      + 0x00000120)
#define HWIO_SPMI_GENI_CFG_REG8_RMSK                                           0xfe7f
#define HWIO_SPMI_GENI_CFG_REG8_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG8_ADDR, HWIO_SPMI_GENI_CFG_REG8_RMSK)
#define HWIO_SPMI_GENI_CFG_REG8_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG8_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG8_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG8_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG8_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG8_IN)
#define HWIO_SPMI_GENI_CFG_REG8_S_GP_CNT_INIT_VAL_BMSK                         0xf800
#define HWIO_SPMI_GENI_CFG_REG8_S_GP_CNT_INIT_VAL_SHFT                            0xb
#define HWIO_SPMI_GENI_CFG_REG8_S_GP_CNT_DIRECTION_BMSK                         0x400
#define HWIO_SPMI_GENI_CFG_REG8_S_GP_CNT_DIRECTION_SHFT                           0xa
#define HWIO_SPMI_GENI_CFG_REG8_S_DATA_CNT_INIT_VALUE_INCR_BMSK                 0x200
#define HWIO_SPMI_GENI_CFG_REG8_S_DATA_CNT_INIT_VALUE_INCR_SHFT                   0x9
#define HWIO_SPMI_GENI_CFG_REG8_S_DATA_CNT_EN_BITS_BMSK                          0x7e
#define HWIO_SPMI_GENI_CFG_REG8_S_DATA_CNT_EN_BITS_SHFT                           0x1
#define HWIO_SPMI_GENI_CFG_REG8_S_PROG_RAM_SEC_WORD_EN_BMSK                       0x1
#define HWIO_SPMI_GENI_CFG_REG8_S_PROG_RAM_SEC_WORD_EN_SHFT                       0x0

#define HWIO_SPMI_GENI_CFG_REG9_ADDR                                       (SPMI_GENI_CFG_REG_BASE      + 0x00000124)
#define HWIO_SPMI_GENI_CFG_REG9_RMSK                                          0xfffff
#define HWIO_SPMI_GENI_CFG_REG9_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG9_ADDR, HWIO_SPMI_GENI_CFG_REG9_RMSK)
#define HWIO_SPMI_GENI_CFG_REG9_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG9_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG9_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG9_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG9_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG9_IN)
#define HWIO_SPMI_GENI_CFG_REG9_RX_DATA_TABLE_VEC_1_BMSK                      0xffc00
#define HWIO_SPMI_GENI_CFG_REG9_RX_DATA_TABLE_VEC_1_SHFT                          0xa
#define HWIO_SPMI_GENI_CFG_REG9_RX_DATA_TABLE_VEC_0_BMSK                        0x3ff
#define HWIO_SPMI_GENI_CFG_REG9_RX_DATA_TABLE_VEC_0_SHFT                          0x0

#define HWIO_SPMI_GENI_CFG_REG10_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x00000128)
#define HWIO_SPMI_GENI_CFG_REG10_RMSK                                         0xfffff
#define HWIO_SPMI_GENI_CFG_REG10_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG10_ADDR, HWIO_SPMI_GENI_CFG_REG10_RMSK)
#define HWIO_SPMI_GENI_CFG_REG10_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG10_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG10_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG10_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG10_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG10_IN)
#define HWIO_SPMI_GENI_CFG_REG10_RX_DATA_TABLE_VEC_3_BMSK                     0xffc00
#define HWIO_SPMI_GENI_CFG_REG10_RX_DATA_TABLE_VEC_3_SHFT                         0xa
#define HWIO_SPMI_GENI_CFG_REG10_RX_DATA_TABLE_VEC_2_BMSK                       0x3ff
#define HWIO_SPMI_GENI_CFG_REG10_RX_DATA_TABLE_VEC_2_SHFT                         0x0

#define HWIO_SPMI_GENI_CFG_REG11_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x0000012c)
#define HWIO_SPMI_GENI_CFG_REG11_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG11_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG11_ADDR, HWIO_SPMI_GENI_CFG_REG11_RMSK)
#define HWIO_SPMI_GENI_CFG_REG11_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG11_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG11_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG11_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG11_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG11_IN)
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_11_TX_DURATION_BMSK                    0xc00000
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_11_TX_DURATION_SHFT                        0x16
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_10_TX_DURATION_BMSK                    0x300000
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_10_TX_DURATION_SHFT                        0x14
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_9_TX_DURATION_BMSK                      0xc0000
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_9_TX_DURATION_SHFT                         0x12
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_8_TX_DURATION_BMSK                      0x30000
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_8_TX_DURATION_SHFT                         0x10
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_7_TX_DURATION_BMSK                       0xc000
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_7_TX_DURATION_SHFT                          0xe
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_6_TX_DURATION_BMSK                       0x3000
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_6_TX_DURATION_SHFT                          0xc
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_5_TX_DURATION_BMSK                        0xc00
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_5_TX_DURATION_SHFT                          0xa
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_4_TX_DURATION_BMSK                        0x300
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_4_TX_DURATION_SHFT                          0x8
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_3_TX_DURATION_BMSK                         0xc0
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_3_TX_DURATION_SHFT                          0x6
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_2_TX_DURATION_BMSK                         0x30
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_2_TX_DURATION_SHFT                          0x4
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_1_TX_DURATION_BMSK                          0xc
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_1_TX_DURATION_SHFT                          0x2
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_0_TX_DURATION_BMSK                          0x3
#define HWIO_SPMI_GENI_CFG_REG11_PRIM_0_TX_DURATION_SHFT                          0x0

#define HWIO_SPMI_GENI_CFG_REG12_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x00000130)
#define HWIO_SPMI_GENI_CFG_REG12_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG12_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG12_ADDR, HWIO_SPMI_GENI_CFG_REG12_RMSK)
#define HWIO_SPMI_GENI_CFG_REG12_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG12_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG12_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG12_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG12_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG12_IN)
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_23_TX_DURATION_BMSK                    0xc00000
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_23_TX_DURATION_SHFT                        0x16
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_22_TX_DURATION_BMSK                    0x300000
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_22_TX_DURATION_SHFT                        0x14
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_21_TX_DURATION_BMSK                     0xc0000
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_21_TX_DURATION_SHFT                        0x12
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_20_TX_DURATION_BMSK                     0x30000
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_20_TX_DURATION_SHFT                        0x10
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_19_TX_DURATION_BMSK                      0xc000
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_19_TX_DURATION_SHFT                         0xe
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_18_TX_DURATION_BMSK                      0x3000
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_18_TX_DURATION_SHFT                         0xc
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_17_TX_DURATION_BMSK                       0xc00
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_17_TX_DURATION_SHFT                         0xa
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_16_TX_DURATION_BMSK                       0x300
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_16_TX_DURATION_SHFT                         0x8
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_15_TX_DURATION_BMSK                        0xc0
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_15_TX_DURATION_SHFT                         0x6
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_14_TX_DURATION_BMSK                        0x30
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_14_TX_DURATION_SHFT                         0x4
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_13_TX_DURATION_BMSK                         0xc
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_13_TX_DURATION_SHFT                         0x2
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_12_TX_DURATION_BMSK                         0x3
#define HWIO_SPMI_GENI_CFG_REG12_PRIM_12_TX_DURATION_SHFT                         0x0

#define HWIO_SPMI_GENI_CFG_REG13_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x00000134)
#define HWIO_SPMI_GENI_CFG_REG13_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG13_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG13_ADDR, HWIO_SPMI_GENI_CFG_REG13_RMSK)
#define HWIO_SPMI_GENI_CFG_REG13_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG13_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG13_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG13_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG13_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG13_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG13_IN)
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_11_RX_DURATION_BMSK                    0xc00000
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_11_RX_DURATION_SHFT                        0x16
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_10_RX_DURATION_BMSK                    0x300000
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_10_RX_DURATION_SHFT                        0x14
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_9_RX_DURATION_BMSK                      0xc0000
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_9_RX_DURATION_SHFT                         0x12
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_8_RX_DURATION_BMSK                      0x30000
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_8_RX_DURATION_SHFT                         0x10
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_7_RX_DURATION_BMSK                       0xc000
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_7_RX_DURATION_SHFT                          0xe
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_6_RX_DURATION_BMSK                       0x3000
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_6_RX_DURATION_SHFT                          0xc
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_5_RX_DURATION_BMSK                        0xc00
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_5_RX_DURATION_SHFT                          0xa
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_4_RX_DURATION_BMSK                        0x300
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_4_RX_DURATION_SHFT                          0x8
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_3_RX_DURATION_BMSK                         0xc0
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_3_RX_DURATION_SHFT                          0x6
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_2_RX_DURATION_BMSK                         0x30
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_2_RX_DURATION_SHFT                          0x4
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_1_RX_DURATION_BMSK                          0xc
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_1_RX_DURATION_SHFT                          0x2
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_0_RX_DURATION_BMSK                          0x3
#define HWIO_SPMI_GENI_CFG_REG13_PRIM_0_RX_DURATION_SHFT                          0x0

#define HWIO_SPMI_GENI_CFG_REG14_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x00000138)
#define HWIO_SPMI_GENI_CFG_REG14_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG14_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG14_ADDR, HWIO_SPMI_GENI_CFG_REG14_RMSK)
#define HWIO_SPMI_GENI_CFG_REG14_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG14_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG14_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG14_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG14_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG14_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG14_IN)
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_23_RX_DURATION_BMSK                    0xc00000
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_23_RX_DURATION_SHFT                        0x16
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_22_RX_DURATION_BMSK                    0x300000
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_22_RX_DURATION_SHFT                        0x14
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_21_RX_DURATION_BMSK                     0xc0000
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_21_RX_DURATION_SHFT                        0x12
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_20_RX_DURATION_BMSK                     0x30000
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_20_RX_DURATION_SHFT                        0x10
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_19_RX_DURATION_BMSK                      0xc000
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_19_RX_DURATION_SHFT                         0xe
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_18_RX_DURATION_BMSK                      0x3000
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_18_RX_DURATION_SHFT                         0xc
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_17_RX_DURATION_BMSK                       0xc00
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_17_RX_DURATION_SHFT                         0xa
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_16_RX_DURATION_BMSK                       0x300
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_16_RX_DURATION_SHFT                         0x8
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_15_RX_DURATION_BMSK                        0xc0
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_15_RX_DURATION_SHFT                         0x6
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_14_RX_DURATION_BMSK                        0x30
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_14_RX_DURATION_SHFT                         0x4
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_13_RX_DURATION_BMSK                         0xc
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_13_RX_DURATION_SHFT                         0x2
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_12_RX_DURATION_BMSK                         0x3
#define HWIO_SPMI_GENI_CFG_REG14_PRIM_12_RX_DURATION_SHFT                         0x0

#define HWIO_SPMI_GENI_CFG_REG15_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x0000013c)
#define HWIO_SPMI_GENI_CFG_REG15_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG15_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG15_ADDR, HWIO_SPMI_GENI_CFG_REG15_RMSK)
#define HWIO_SPMI_GENI_CFG_REG15_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG15_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG15_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG15_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG15_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG15_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG15_IN)
#define HWIO_SPMI_GENI_CFG_REG15_PRIM_7_TX_FUNC_SEL_BMSK                     0xe00000
#define HWIO_SPMI_GENI_CFG_REG15_PRIM_7_TX_FUNC_SEL_SHFT                         0x15
#define HWIO_SPMI_GENI_CFG_REG15_PRIM_6_TX_FUNC_SEL_BMSK                     0x1c0000
#define HWIO_SPMI_GENI_CFG_REG15_PRIM_6_TX_FUNC_SEL_SHFT                         0x12
#define HWIO_SPMI_GENI_CFG_REG15_PRIM_5_TX_FUNC_SEL_BMSK                      0x38000
#define HWIO_SPMI_GENI_CFG_REG15_PRIM_5_TX_FUNC_SEL_SHFT                          0xf
#define HWIO_SPMI_GENI_CFG_REG15_PRIM_4_TX_FUNC_SEL_BMSK                       0x7000
#define HWIO_SPMI_GENI_CFG_REG15_PRIM_4_TX_FUNC_SEL_SHFT                          0xc
#define HWIO_SPMI_GENI_CFG_REG15_PRIM_3_TX_FUNC_SEL_BMSK                        0xe00
#define HWIO_SPMI_GENI_CFG_REG15_PRIM_3_TX_FUNC_SEL_SHFT                          0x9
#define HWIO_SPMI_GENI_CFG_REG15_PRIM_2_TX_FUNC_SEL_BMSK                        0x1c0
#define HWIO_SPMI_GENI_CFG_REG15_PRIM_2_TX_FUNC_SEL_SHFT                          0x6
#define HWIO_SPMI_GENI_CFG_REG15_PRIM_1_TX_FUNC_SEL_BMSK                         0x38
#define HWIO_SPMI_GENI_CFG_REG15_PRIM_1_TX_FUNC_SEL_SHFT                          0x3
#define HWIO_SPMI_GENI_CFG_REG15_PRIM_0_TX_FUNC_SEL_BMSK                          0x7
#define HWIO_SPMI_GENI_CFG_REG15_PRIM_0_TX_FUNC_SEL_SHFT                          0x0

#define HWIO_SPMI_GENI_CFG_REG16_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x00000140)
#define HWIO_SPMI_GENI_CFG_REG16_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG16_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG16_ADDR, HWIO_SPMI_GENI_CFG_REG16_RMSK)
#define HWIO_SPMI_GENI_CFG_REG16_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG16_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG16_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG16_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG16_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG16_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG16_IN)
#define HWIO_SPMI_GENI_CFG_REG16_PRIM_15_TX_FUNC_SEL_BMSK                    0xe00000
#define HWIO_SPMI_GENI_CFG_REG16_PRIM_15_TX_FUNC_SEL_SHFT                        0x15
#define HWIO_SPMI_GENI_CFG_REG16_PRIM_14_TX_FUNC_SEL_BMSK                    0x1c0000
#define HWIO_SPMI_GENI_CFG_REG16_PRIM_14_TX_FUNC_SEL_SHFT                        0x12
#define HWIO_SPMI_GENI_CFG_REG16_PRIM_13_TX_FUNC_SEL_BMSK                     0x38000
#define HWIO_SPMI_GENI_CFG_REG16_PRIM_13_TX_FUNC_SEL_SHFT                         0xf
#define HWIO_SPMI_GENI_CFG_REG16_PRIM_12_TX_FUNC_SEL_BMSK                      0x7000
#define HWIO_SPMI_GENI_CFG_REG16_PRIM_12_TX_FUNC_SEL_SHFT                         0xc
#define HWIO_SPMI_GENI_CFG_REG16_PRIM_11_TX_FUNC_SEL_BMSK                       0xe00
#define HWIO_SPMI_GENI_CFG_REG16_PRIM_11_TX_FUNC_SEL_SHFT                         0x9
#define HWIO_SPMI_GENI_CFG_REG16_PRIM_10_TX_FUNC_SEL_BMSK                       0x1c0
#define HWIO_SPMI_GENI_CFG_REG16_PRIM_10_TX_FUNC_SEL_SHFT                         0x6
#define HWIO_SPMI_GENI_CFG_REG16_PRIM_9_TX_FUNC_SEL_BMSK                         0x38
#define HWIO_SPMI_GENI_CFG_REG16_PRIM_9_TX_FUNC_SEL_SHFT                          0x3
#define HWIO_SPMI_GENI_CFG_REG16_PRIM_8_TX_FUNC_SEL_BMSK                          0x7
#define HWIO_SPMI_GENI_CFG_REG16_PRIM_8_TX_FUNC_SEL_SHFT                          0x0

#define HWIO_SPMI_GENI_CFG_REG17_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x00000144)
#define HWIO_SPMI_GENI_CFG_REG17_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG17_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG17_ADDR, HWIO_SPMI_GENI_CFG_REG17_RMSK)
#define HWIO_SPMI_GENI_CFG_REG17_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG17_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG17_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG17_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG17_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG17_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG17_IN)
#define HWIO_SPMI_GENI_CFG_REG17_PRIM_23_TX_FUNC_SEL_BMSK                    0xe00000
#define HWIO_SPMI_GENI_CFG_REG17_PRIM_23_TX_FUNC_SEL_SHFT                        0x15
#define HWIO_SPMI_GENI_CFG_REG17_PRIM_22_TX_FUNC_SEL_BMSK                    0x1c0000
#define HWIO_SPMI_GENI_CFG_REG17_PRIM_22_TX_FUNC_SEL_SHFT                        0x12
#define HWIO_SPMI_GENI_CFG_REG17_PRIM_21_TX_FUNC_SEL_BMSK                     0x38000
#define HWIO_SPMI_GENI_CFG_REG17_PRIM_21_TX_FUNC_SEL_SHFT                         0xf
#define HWIO_SPMI_GENI_CFG_REG17_PRIM_20_TX_FUNC_SEL_BMSK                      0x7000
#define HWIO_SPMI_GENI_CFG_REG17_PRIM_20_TX_FUNC_SEL_SHFT                         0xc
#define HWIO_SPMI_GENI_CFG_REG17_PRIM_19_TX_FUNC_SEL_BMSK                       0xe00
#define HWIO_SPMI_GENI_CFG_REG17_PRIM_19_TX_FUNC_SEL_SHFT                         0x9
#define HWIO_SPMI_GENI_CFG_REG17_PRIM_18_TX_FUNC_SEL_BMSK                       0x1c0
#define HWIO_SPMI_GENI_CFG_REG17_PRIM_18_TX_FUNC_SEL_SHFT                         0x6
#define HWIO_SPMI_GENI_CFG_REG17_PRIM_17_TX_FUNC_SEL_BMSK                        0x38
#define HWIO_SPMI_GENI_CFG_REG17_PRIM_17_TX_FUNC_SEL_SHFT                         0x3
#define HWIO_SPMI_GENI_CFG_REG17_PRIM_16_TX_FUNC_SEL_BMSK                         0x7
#define HWIO_SPMI_GENI_CFG_REG17_PRIM_16_TX_FUNC_SEL_SHFT                         0x0

#define HWIO_SPMI_GENI_CFG_REG18_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x00000148)
#define HWIO_SPMI_GENI_CFG_REG18_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG18_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG18_ADDR, HWIO_SPMI_GENI_CFG_REG18_RMSK)
#define HWIO_SPMI_GENI_CFG_REG18_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG18_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG18_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG18_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG18_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG18_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG18_IN)
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_23_TX_PAR_FIRST_BMSK                   0x800000
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_23_TX_PAR_FIRST_SHFT                       0x17
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_22_TX_PAR_FIRST_BMSK                   0x400000
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_22_TX_PAR_FIRST_SHFT                       0x16
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_21_TX_PAR_FIRST_BMSK                   0x200000
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_21_TX_PAR_FIRST_SHFT                       0x15
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_20_TX_PAR_FIRST_BMSK                   0x100000
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_20_TX_PAR_FIRST_SHFT                       0x14
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_19_TX_PAR_FIRST_BMSK                    0x80000
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_19_TX_PAR_FIRST_SHFT                       0x13
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_18_TX_PAR_FIRST_BMSK                    0x40000
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_18_TX_PAR_FIRST_SHFT                       0x12
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_17_TX_PAR_FIRST_BMSK                    0x20000
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_17_TX_PAR_FIRST_SHFT                       0x11
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_16_TX_PAR_FIRST_BMSK                    0x10000
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_16_TX_PAR_FIRST_SHFT                       0x10
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_15_TX_PAR_FIRST_BMSK                     0x8000
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_15_TX_PAR_FIRST_SHFT                        0xf
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_14_TX_PAR_FIRST_BMSK                     0x4000
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_14_TX_PAR_FIRST_SHFT                        0xe
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_13_TX_PAR_FIRST_BMSK                     0x2000
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_13_TX_PAR_FIRST_SHFT                        0xd
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_12_TX_PAR_FIRST_BMSK                     0x1000
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_12_TX_PAR_FIRST_SHFT                        0xc
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_11_TX_PAR_FIRST_BMSK                      0x800
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_11_TX_PAR_FIRST_SHFT                        0xb
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_10_TX_PAR_FIRST_BMSK                      0x400
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_10_TX_PAR_FIRST_SHFT                        0xa
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_9_TX_PAR_FIRST_BMSK                       0x200
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_9_TX_PAR_FIRST_SHFT                         0x9
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_8_TX_PAR_FIRST_BMSK                       0x100
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_8_TX_PAR_FIRST_SHFT                         0x8
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_7_TX_PAR_FIRST_BMSK                        0x80
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_7_TX_PAR_FIRST_SHFT                         0x7
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_6_TX_PAR_FIRST_BMSK                        0x40
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_6_TX_PAR_FIRST_SHFT                         0x6
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_5_TX_PAR_FIRST_BMSK                        0x20
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_5_TX_PAR_FIRST_SHFT                         0x5
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_4_TX_PAR_FIRST_BMSK                        0x10
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_4_TX_PAR_FIRST_SHFT                         0x4
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_3_TX_PAR_FIRST_BMSK                         0x8
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_3_TX_PAR_FIRST_SHFT                         0x3
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_2_TX_PAR_FIRST_BMSK                         0x4
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_2_TX_PAR_FIRST_SHFT                         0x2
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_1_TX_PAR_FIRST_BMSK                         0x2
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_1_TX_PAR_FIRST_SHFT                         0x1
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_0_TX_PAR_FIRST_BMSK                         0x1
#define HWIO_SPMI_GENI_CFG_REG18_PRIM_0_TX_PAR_FIRST_SHFT                         0x0

#define HWIO_SPMI_GENI_CFG_REG19_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x0000014c)
#define HWIO_SPMI_GENI_CFG_REG19_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG19_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG19_ADDR, HWIO_SPMI_GENI_CFG_REG19_RMSK)
#define HWIO_SPMI_GENI_CFG_REG19_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG19_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG19_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG19_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG19_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG19_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG19_IN)

#define HWIO_SPMI_GENI_CFG_REG20_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x00000150)
#define HWIO_SPMI_GENI_CFG_REG20_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG20_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG20_ADDR, HWIO_SPMI_GENI_CFG_REG20_RMSK)
#define HWIO_SPMI_GENI_CFG_REG20_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG20_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG20_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG20_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG20_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG20_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG20_IN)

#define HWIO_SPMI_GENI_CFG_REG21_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x00000154)
#define HWIO_SPMI_GENI_CFG_REG21_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG21_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG21_ADDR, HWIO_SPMI_GENI_CFG_REG21_RMSK)
#define HWIO_SPMI_GENI_CFG_REG21_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG21_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG21_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG21_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG21_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG21_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG21_IN)
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_23_RX_SI_EN_BMSK                       0x800000
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_23_RX_SI_EN_SHFT                           0x17
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_22_RX_SI_EN_BMSK                       0x400000
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_22_RX_SI_EN_SHFT                           0x16
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_21_RX_SI_EN_BMSK                       0x200000
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_21_RX_SI_EN_SHFT                           0x15
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_20_RX_SI_EN_BMSK                       0x100000
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_20_RX_SI_EN_SHFT                           0x14
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_19_RX_SI_EN_BMSK                        0x80000
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_19_RX_SI_EN_SHFT                           0x13
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_18_RX_SI_EN_BMSK                        0x40000
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_18_RX_SI_EN_SHFT                           0x12
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_17_RX_SI_EN_BMSK                        0x20000
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_17_RX_SI_EN_SHFT                           0x11
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_16_RX_SI_EN_BMSK                        0x10000
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_16_RX_SI_EN_SHFT                           0x10
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_15_RX_SI_EN_BMSK                         0x8000
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_15_RX_SI_EN_SHFT                            0xf
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_14_RX_SI_EN_BMSK                         0x4000
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_14_RX_SI_EN_SHFT                            0xe
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_13_RX_SI_EN_BMSK                         0x2000
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_13_RX_SI_EN_SHFT                            0xd
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_12_RX_SI_EN_BMSK                         0x1000
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_12_RX_SI_EN_SHFT                            0xc
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_11_RX_SI_EN_BMSK                          0x800
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_11_RX_SI_EN_SHFT                            0xb
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_10_RX_SI_EN_BMSK                          0x400
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_10_RX_SI_EN_SHFT                            0xa
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_9_RX_SI_EN_BMSK                           0x200
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_9_RX_SI_EN_SHFT                             0x9
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_8_RX_SI_EN_BMSK                           0x100
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_8_RX_SI_EN_SHFT                             0x8
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_7_RX_SI_EN_BMSK                            0x80
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_7_RX_SI_EN_SHFT                             0x7
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_6_RX_SI_EN_BMSK                            0x40
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_6_RX_SI_EN_SHFT                             0x6
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_5_RX_SI_EN_BMSK                            0x20
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_5_RX_SI_EN_SHFT                             0x5
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_4_RX_SI_EN_BMSK                            0x10
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_4_RX_SI_EN_SHFT                             0x4
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_3_RX_SI_EN_BMSK                             0x8
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_3_RX_SI_EN_SHFT                             0x3
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_2_RX_SI_EN_BMSK                             0x4
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_2_RX_SI_EN_SHFT                             0x2
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_1_RX_SI_EN_BMSK                             0x2
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_1_RX_SI_EN_SHFT                             0x1
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_0_RX_SI_EN_BMSK                             0x1
#define HWIO_SPMI_GENI_CFG_REG21_PRIM_0_RX_SI_EN_SHFT                             0x0

#define HWIO_SPMI_GENI_CFG_REG22_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x00000158)
#define HWIO_SPMI_GENI_CFG_REG22_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG22_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG22_ADDR, HWIO_SPMI_GENI_CFG_REG22_RMSK)
#define HWIO_SPMI_GENI_CFG_REG22_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG22_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG22_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG22_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG22_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG22_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG22_IN)
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_23_RX_PAR_EN_BMSK                      0x800000
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_23_RX_PAR_EN_SHFT                          0x17
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_22_RX_PAR_EN_BMSK                      0x400000
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_22_RX_PAR_EN_SHFT                          0x16
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_21_RX_PAR_EN_BMSK                      0x200000
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_21_RX_PAR_EN_SHFT                          0x15
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_20_RX_PAR_EN_BMSK                      0x100000
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_20_RX_PAR_EN_SHFT                          0x14
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_19_RX_PAR_EN_BMSK                       0x80000
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_19_RX_PAR_EN_SHFT                          0x13
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_18_RX_PAR_EN_BMSK                       0x40000
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_18_RX_PAR_EN_SHFT                          0x12
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_17_RX_PAR_EN_BMSK                       0x20000
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_17_RX_PAR_EN_SHFT                          0x11
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_16_RX_PAR_EN_BMSK                       0x10000
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_16_RX_PAR_EN_SHFT                          0x10
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_15_RX_PAR_EN_BMSK                        0x8000
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_15_RX_PAR_EN_SHFT                           0xf
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_14_RX_PAR_EN_BMSK                        0x4000
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_14_RX_PAR_EN_SHFT                           0xe
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_13_RX_PAR_EN_BMSK                        0x2000
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_13_RX_PAR_EN_SHFT                           0xd
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_12_RX_PAR_EN_BMSK                        0x1000
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_12_RX_PAR_EN_SHFT                           0xc
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_11_RX_PAR_EN_BMSK                         0x800
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_11_RX_PAR_EN_SHFT                           0xb
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_10_RX_PAR_EN_BMSK                         0x400
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_10_RX_PAR_EN_SHFT                           0xa
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_9_RX_PAR_EN_BMSK                          0x200
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_9_RX_PAR_EN_SHFT                            0x9
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_8_RX_PAR_EN_BMSK                          0x100
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_8_RX_PAR_EN_SHFT                            0x8
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_7_RX_PAR_EN_BMSK                           0x80
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_7_RX_PAR_EN_SHFT                            0x7
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_6_RX_PAR_EN_BMSK                           0x40
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_6_RX_PAR_EN_SHFT                            0x6
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_5_RX_PAR_EN_BMSK                           0x20
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_5_RX_PAR_EN_SHFT                            0x5
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_4_RX_PAR_EN_BMSK                           0x10
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_4_RX_PAR_EN_SHFT                            0x4
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_3_RX_PAR_EN_BMSK                            0x8
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_3_RX_PAR_EN_SHFT                            0x3
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_2_RX_PAR_EN_BMSK                            0x4
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_2_RX_PAR_EN_SHFT                            0x2
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_1_RX_PAR_EN_BMSK                            0x2
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_1_RX_PAR_EN_SHFT                            0x1
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_0_RX_PAR_EN_BMSK                            0x1
#define HWIO_SPMI_GENI_CFG_REG22_PRIM_0_RX_PAR_EN_SHFT                            0x0

#define HWIO_SPMI_GENI_CFG_REG23_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x0000015c)
#define HWIO_SPMI_GENI_CFG_REG23_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG23_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG23_ADDR, HWIO_SPMI_GENI_CFG_REG23_RMSK)
#define HWIO_SPMI_GENI_CFG_REG23_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG23_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG23_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG23_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG23_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG23_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG23_IN)
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_23_RX_PAR_FIRST_BMSK                   0x800000
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_23_RX_PAR_FIRST_SHFT                       0x17
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_22_RX_PAR_FIRST_BMSK                   0x400000
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_22_RX_PAR_FIRST_SHFT                       0x16
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_21_RX_PAR_FIRST_BMSK                   0x200000
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_21_RX_PAR_FIRST_SHFT                       0x15
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_20_RX_PAR_FIRST_BMSK                   0x100000
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_20_RX_PAR_FIRST_SHFT                       0x14
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_19_RX_PAR_FIRST_BMSK                    0x80000
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_19_RX_PAR_FIRST_SHFT                       0x13
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_18_RX_PAR_FIRST_BMSK                    0x40000
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_18_RX_PAR_FIRST_SHFT                       0x12
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_17_RX_PAR_FIRST_BMSK                    0x20000
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_17_RX_PAR_FIRST_SHFT                       0x11
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_16_RX_PAR_FIRST_BMSK                    0x10000
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_16_RX_PAR_FIRST_SHFT                       0x10
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_15_RX_PAR_FIRST_BMSK                     0x8000
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_15_RX_PAR_FIRST_SHFT                        0xf
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_14_RX_PAR_FIRST_BMSK                     0x4000
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_14_RX_PAR_FIRST_SHFT                        0xe
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_13_RX_PAR_FIRST_BMSK                     0x2000
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_13_RX_PAR_FIRST_SHFT                        0xd
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_12_RX_PAR_FIRST_BMSK                     0x1000
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_12_RX_PAR_FIRST_SHFT                        0xc
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_11_RX_PAR_FIRST_BMSK                      0x800
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_11_RX_PAR_FIRST_SHFT                        0xb
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_10_RX_PAR_FIRST_BMSK                      0x400
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_10_RX_PAR_FIRST_SHFT                        0xa
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_9_RX_PAR_FIRST_BMSK                       0x200
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_9_RX_PAR_FIRST_SHFT                         0x9
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_8_RX_PAR_FIRST_BMSK                       0x100
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_8_RX_PAR_FIRST_SHFT                         0x8
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_7_RX_PAR_FIRST_BMSK                        0x80
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_7_RX_PAR_FIRST_SHFT                         0x7
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_6_RX_PAR_FIRST_BMSK                        0x40
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_6_RX_PAR_FIRST_SHFT                         0x6
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_5_RX_PAR_FIRST_BMSK                        0x20
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_5_RX_PAR_FIRST_SHFT                         0x5
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_4_RX_PAR_FIRST_BMSK                        0x10
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_4_RX_PAR_FIRST_SHFT                         0x4
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_3_RX_PAR_FIRST_BMSK                         0x8
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_3_RX_PAR_FIRST_SHFT                         0x3
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_2_RX_PAR_FIRST_BMSK                         0x4
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_2_RX_PAR_FIRST_SHFT                         0x2
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_1_RX_PAR_FIRST_BMSK                         0x2
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_1_RX_PAR_FIRST_SHFT                         0x1
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_0_RX_PAR_FIRST_BMSK                         0x1
#define HWIO_SPMI_GENI_CFG_REG23_PRIM_0_RX_PAR_FIRST_SHFT                         0x0

#define HWIO_SPMI_GENI_CFG_REG24_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x00000160)
#define HWIO_SPMI_GENI_CFG_REG24_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG24_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG24_ADDR, HWIO_SPMI_GENI_CFG_REG24_RMSK)
#define HWIO_SPMI_GENI_CFG_REG24_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG24_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG24_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG24_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG24_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG24_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG24_IN)
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_23_ACC_PAR_EN_BMSK                     0x800000
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_23_ACC_PAR_EN_SHFT                         0x17
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_22_ACC_PAR_EN_BMSK                     0x400000
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_22_ACC_PAR_EN_SHFT                         0x16
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_21_ACC_PAR_EN_BMSK                     0x200000
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_21_ACC_PAR_EN_SHFT                         0x15
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_20_ACC_PAR_EN_BMSK                     0x100000
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_20_ACC_PAR_EN_SHFT                         0x14
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_19_ACC_PAR_EN_BMSK                      0x80000
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_19_ACC_PAR_EN_SHFT                         0x13
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_18_ACC_PAR_EN_BMSK                      0x40000
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_18_ACC_PAR_EN_SHFT                         0x12
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_17_ACC_PAR_EN_BMSK                      0x20000
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_17_ACC_PAR_EN_SHFT                         0x11
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_16_ACC_PAR_EN_BMSK                      0x10000
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_16_ACC_PAR_EN_SHFT                         0x10
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_15_ACC_PAR_EN_BMSK                       0x8000
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_15_ACC_PAR_EN_SHFT                          0xf
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_14_ACC_PAR_EN_BMSK                       0x4000
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_14_ACC_PAR_EN_SHFT                          0xe
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_13_ACC_PAR_EN_BMSK                       0x2000
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_13_ACC_PAR_EN_SHFT                          0xd
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_12_ACC_PAR_EN_BMSK                       0x1000
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_12_ACC_PAR_EN_SHFT                          0xc
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_11_ACC_PAR_EN_BMSK                        0x800
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_11_ACC_PAR_EN_SHFT                          0xb
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_10_ACC_PAR_EN_BMSK                        0x400
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_10_ACC_PAR_EN_SHFT                          0xa
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_9_ACC_PAR_EN_BMSK                         0x200
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_9_ACC_PAR_EN_SHFT                           0x9
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_8_ACC_PAR_EN_BMSK                         0x100
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_8_ACC_PAR_EN_SHFT                           0x8
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_7_ACC_PAR_EN_BMSK                          0x80
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_7_ACC_PAR_EN_SHFT                           0x7
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_6_ACC_PAR_EN_BMSK                          0x40
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_6_ACC_PAR_EN_SHFT                           0x6
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_5_ACC_PAR_EN_BMSK                          0x20
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_5_ACC_PAR_EN_SHFT                           0x5
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_4_ACC_PAR_EN_BMSK                          0x10
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_4_ACC_PAR_EN_SHFT                           0x4
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_3_ACC_PAR_EN_BMSK                           0x8
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_3_ACC_PAR_EN_SHFT                           0x3
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_2_ACC_PAR_EN_BMSK                           0x4
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_2_ACC_PAR_EN_SHFT                           0x2
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_1_ACC_PAR_EN_BMSK                           0x2
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_1_ACC_PAR_EN_SHFT                           0x1
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_0_ACC_PAR_EN_BMSK                           0x1
#define HWIO_SPMI_GENI_CFG_REG24_PRIM_0_ACC_PAR_EN_SHFT                           0x0

#define HWIO_SPMI_GENI_CFG_REG25_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x00000164)
#define HWIO_SPMI_GENI_CFG_REG25_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG25_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG25_ADDR, HWIO_SPMI_GENI_CFG_REG25_RMSK)
#define HWIO_SPMI_GENI_CFG_REG25_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG25_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG25_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG25_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG25_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG25_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG25_IN)
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_11_IO2_FUNC_SEL_BMSK                   0xc00000
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_11_IO2_FUNC_SEL_SHFT                       0x16
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_10_IO2_FUNC_SEL_BMSK                   0x300000
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_10_IO2_FUNC_SEL_SHFT                       0x14
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_9_IO2_FUNC_SEL_BMSK                     0xc0000
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_9_IO2_FUNC_SEL_SHFT                        0x12
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_8_IO2_FUNC_SEL_BMSK                     0x30000
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_8_IO2_FUNC_SEL_SHFT                        0x10
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_7_IO2_FUNC_SEL_BMSK                      0xc000
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_7_IO2_FUNC_SEL_SHFT                         0xe
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_6_IO2_FUNC_SEL_BMSK                      0x3000
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_6_IO2_FUNC_SEL_SHFT                         0xc
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_5_IO2_FUNC_SEL_BMSK                       0xc00
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_5_IO2_FUNC_SEL_SHFT                         0xa
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_4_IO2_FUNC_SEL_BMSK                       0x300
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_4_IO2_FUNC_SEL_SHFT                         0x8
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_3_IO2_FUNC_SEL_BMSK                        0xc0
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_3_IO2_FUNC_SEL_SHFT                         0x6
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_2_IO2_FUNC_SEL_BMSK                        0x30
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_2_IO2_FUNC_SEL_SHFT                         0x4
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_1_IO2_FUNC_SEL_BMSK                         0xc
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_1_IO2_FUNC_SEL_SHFT                         0x2
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_0_IO2_FUNC_SEL_BMSK                         0x3
#define HWIO_SPMI_GENI_CFG_REG25_PRIM_0_IO2_FUNC_SEL_SHFT                         0x0

#define HWIO_SPMI_GENI_CFG_REG26_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x00000168)
#define HWIO_SPMI_GENI_CFG_REG26_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG26_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG26_ADDR, HWIO_SPMI_GENI_CFG_REG26_RMSK)
#define HWIO_SPMI_GENI_CFG_REG26_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG26_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG26_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG26_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG26_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG26_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG26_IN)
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_23_IO2_FUNC_SEL_BMSK                   0xc00000
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_23_IO2_FUNC_SEL_SHFT                       0x16
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_22_IO2_FUNC_SEL_BMSK                   0x300000
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_22_IO2_FUNC_SEL_SHFT                       0x14
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_21_IO2_FUNC_SEL_BMSK                    0xc0000
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_21_IO2_FUNC_SEL_SHFT                       0x12
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_20_IO2_FUNC_SEL_BMSK                    0x30000
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_20_IO2_FUNC_SEL_SHFT                       0x10
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_19_IO2_FUNC_SEL_BMSK                     0xc000
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_19_IO2_FUNC_SEL_SHFT                        0xe
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_18_IO2_FUNC_SEL_BMSK                     0x3000
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_18_IO2_FUNC_SEL_SHFT                        0xc
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_17_IO2_FUNC_SEL_BMSK                      0xc00
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_17_IO2_FUNC_SEL_SHFT                        0xa
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_16_IO2_FUNC_SEL_BMSK                      0x300
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_16_IO2_FUNC_SEL_SHFT                        0x8
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_15_IO2_FUNC_SEL_BMSK                       0xc0
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_15_IO2_FUNC_SEL_SHFT                        0x6
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_14_IO2_FUNC_SEL_BMSK                       0x30
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_14_IO2_FUNC_SEL_SHFT                        0x4
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_13_IO2_FUNC_SEL_BMSK                        0xc
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_13_IO2_FUNC_SEL_SHFT                        0x2
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_12_IO2_FUNC_SEL_BMSK                        0x3
#define HWIO_SPMI_GENI_CFG_REG26_PRIM_12_IO2_FUNC_SEL_SHFT                        0x0

#define HWIO_SPMI_GENI_CFG_REG27_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x0000016c)
#define HWIO_SPMI_GENI_CFG_REG27_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG27_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG27_ADDR, HWIO_SPMI_GENI_CFG_REG27_RMSK)
#define HWIO_SPMI_GENI_CFG_REG27_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG27_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG27_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG27_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG27_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG27_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG27_IN)
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_23_TX_FORCE_DATA_VALUE_BMSK            0x800000
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_23_TX_FORCE_DATA_VALUE_SHFT                0x17
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_22_TX_FORCE_DATA_VALUE_BMSK            0x400000
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_22_TX_FORCE_DATA_VALUE_SHFT                0x16
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_21_TX_FORCE_DATA_VALUE_BMSK            0x200000
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_21_TX_FORCE_DATA_VALUE_SHFT                0x15
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_20_TX_FORCE_DATA_VALUE_BMSK            0x100000
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_20_TX_FORCE_DATA_VALUE_SHFT                0x14
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_19_TX_FORCE_DATA_VALUE_BMSK             0x80000
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_19_TX_FORCE_DATA_VALUE_SHFT                0x13
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_18_TX_FORCE_DATA_VALUE_BMSK             0x40000
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_18_TX_FORCE_DATA_VALUE_SHFT                0x12
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_17_TX_FORCE_DATA_VALUE_BMSK             0x20000
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_17_TX_FORCE_DATA_VALUE_SHFT                0x11
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_16_TX_FORCE_DATA_VALUE_BMSK             0x10000
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_16_TX_FORCE_DATA_VALUE_SHFT                0x10
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_15_TX_FORCE_DATA_VALUE_BMSK              0x8000
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_15_TX_FORCE_DATA_VALUE_SHFT                 0xf
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_14_TX_FORCE_DATA_VALUE_BMSK              0x4000
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_14_TX_FORCE_DATA_VALUE_SHFT                 0xe
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_13_TX_FORCE_DATA_VALUE_BMSK              0x2000
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_13_TX_FORCE_DATA_VALUE_SHFT                 0xd
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_12_TX_FORCE_DATA_VALUE_BMSK              0x1000
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_12_TX_FORCE_DATA_VALUE_SHFT                 0xc
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_11_TX_FORCE_DATA_VALUE_BMSK               0x800
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_11_TX_FORCE_DATA_VALUE_SHFT                 0xb
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_10_TX_FORCE_DATA_VALUE_BMSK               0x400
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_10_TX_FORCE_DATA_VALUE_SHFT                 0xa
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_9_TX_FORCE_DATA_VALUE_BMSK                0x200
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_9_TX_FORCE_DATA_VALUE_SHFT                  0x9
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_8_TX_FORCE_DATA_VALUE_BMSK                0x100
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_8_TX_FORCE_DATA_VALUE_SHFT                  0x8
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_7_TX_FORCE_DATA_VALUE_BMSK                 0x80
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_7_TX_FORCE_DATA_VALUE_SHFT                  0x7
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_6_TX_FORCE_DATA_VALUE_BMSK                 0x40
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_6_TX_FORCE_DATA_VALUE_SHFT                  0x6
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_5_TX_FORCE_DATA_VALUE_BMSK                 0x20
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_5_TX_FORCE_DATA_VALUE_SHFT                  0x5
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_4_TX_FORCE_DATA_VALUE_BMSK                 0x10
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_4_TX_FORCE_DATA_VALUE_SHFT                  0x4
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_3_TX_FORCE_DATA_VALUE_BMSK                  0x8
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_3_TX_FORCE_DATA_VALUE_SHFT                  0x3
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_2_TX_FORCE_DATA_VALUE_BMSK                  0x4
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_2_TX_FORCE_DATA_VALUE_SHFT                  0x2
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_1_TX_FORCE_DATA_VALUE_BMSK                  0x2
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_1_TX_FORCE_DATA_VALUE_SHFT                  0x1
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_0_TX_FORCE_DATA_VALUE_BMSK                  0x1
#define HWIO_SPMI_GENI_CFG_REG27_PRIM_0_TX_FORCE_DATA_VALUE_SHFT                  0x0

#define HWIO_SPMI_GENI_CFG_REG28_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x00000170)
#define HWIO_SPMI_GENI_CFG_REG28_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG28_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG28_ADDR, HWIO_SPMI_GENI_CFG_REG28_RMSK)
#define HWIO_SPMI_GENI_CFG_REG28_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG28_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG28_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG28_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG28_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG28_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG28_IN)
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_23_RX_FORCE_DATA_VALUE_BMSK            0x800000
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_23_RX_FORCE_DATA_VALUE_SHFT                0x17
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_22_RX_FORCE_DATA_VALUE_BMSK            0x400000
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_22_RX_FORCE_DATA_VALUE_SHFT                0x16
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_21_RX_FORCE_DATA_VALUE_BMSK            0x200000
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_21_RX_FORCE_DATA_VALUE_SHFT                0x15
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_20_RX_FORCE_DATA_VALUE_BMSK            0x100000
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_20_RX_FORCE_DATA_VALUE_SHFT                0x14
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_19_RX_FORCE_DATA_VALUE_BMSK             0x80000
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_19_RX_FORCE_DATA_VALUE_SHFT                0x13
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_18_RX_FORCE_DATA_VALUE_BMSK             0x40000
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_18_RX_FORCE_DATA_VALUE_SHFT                0x12
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_17_RX_FORCE_DATA_VALUE_BMSK             0x20000
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_17_RX_FORCE_DATA_VALUE_SHFT                0x11
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_16_RX_FORCE_DATA_VALUE_BMSK             0x10000
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_16_RX_FORCE_DATA_VALUE_SHFT                0x10
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_15_RX_FORCE_DATA_VALUE_BMSK              0x8000
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_15_RX_FORCE_DATA_VALUE_SHFT                 0xf
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_14_RX_FORCE_DATA_VALUE_BMSK              0x4000
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_14_RX_FORCE_DATA_VALUE_SHFT                 0xe
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_13_RX_FORCE_DATA_VALUE_BMSK              0x2000
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_13_RX_FORCE_DATA_VALUE_SHFT                 0xd
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_12_RX_FORCE_DATA_VALUE_BMSK              0x1000
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_12_RX_FORCE_DATA_VALUE_SHFT                 0xc
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_11_RX_FORCE_DATA_VALUE_BMSK               0x800
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_11_RX_FORCE_DATA_VALUE_SHFT                 0xb
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_10_RX_FORCE_DATA_VALUE_BMSK               0x400
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_10_RX_FORCE_DATA_VALUE_SHFT                 0xa
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_9_RX_FORCE_DATA_VALUE_BMSK                0x200
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_9_RX_FORCE_DATA_VALUE_SHFT                  0x9
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_8_RX_FORCE_DATA_VALUE_BMSK                0x100
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_8_RX_FORCE_DATA_VALUE_SHFT                  0x8
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_7_RX_FORCE_DATA_VALUE_BMSK                 0x80
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_7_RX_FORCE_DATA_VALUE_SHFT                  0x7
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_6_RX_FORCE_DATA_VALUE_BMSK                 0x40
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_6_RX_FORCE_DATA_VALUE_SHFT                  0x6
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_5_RX_FORCE_DATA_VALUE_BMSK                 0x20
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_5_RX_FORCE_DATA_VALUE_SHFT                  0x5
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_4_RX_FORCE_DATA_VALUE_BMSK                 0x10
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_4_RX_FORCE_DATA_VALUE_SHFT                  0x4
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_3_RX_FORCE_DATA_VALUE_BMSK                  0x8
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_3_RX_FORCE_DATA_VALUE_SHFT                  0x3
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_2_RX_FORCE_DATA_VALUE_BMSK                  0x4
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_2_RX_FORCE_DATA_VALUE_SHFT                  0x2
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_1_RX_FORCE_DATA_VALUE_BMSK                  0x2
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_1_RX_FORCE_DATA_VALUE_SHFT                  0x1
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_0_RX_FORCE_DATA_VALUE_BMSK                  0x1
#define HWIO_SPMI_GENI_CFG_REG28_PRIM_0_RX_FORCE_DATA_VALUE_SHFT                  0x0

#define HWIO_SPMI_GENI_CFG_REG29_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x00000174)
#define HWIO_SPMI_GENI_CFG_REG29_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG29_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG29_ADDR, HWIO_SPMI_GENI_CFG_REG29_RMSK)
#define HWIO_SPMI_GENI_CFG_REG29_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG29_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG29_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG29_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG29_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG29_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG29_IN)
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_23_TX_TIME_COUNTER_EN_BMSK             0x800000
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_23_TX_TIME_COUNTER_EN_SHFT                 0x17
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_22_TX_TIME_COUNTER_EN_BMSK             0x400000
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_22_TX_TIME_COUNTER_EN_SHFT                 0x16
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_21_TX_TIME_COUNTER_EN_BMSK             0x200000
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_21_TX_TIME_COUNTER_EN_SHFT                 0x15
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_20_TX_TIME_COUNTER_EN_BMSK             0x100000
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_20_TX_TIME_COUNTER_EN_SHFT                 0x14
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_19_TX_TIME_COUNTER_EN_BMSK              0x80000
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_19_TX_TIME_COUNTER_EN_SHFT                 0x13
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_18_TX_TIME_COUNTER_EN_BMSK              0x40000
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_18_TX_TIME_COUNTER_EN_SHFT                 0x12
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_17_TX_TIME_COUNTER_EN_BMSK              0x20000
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_17_TX_TIME_COUNTER_EN_SHFT                 0x11
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_16_TX_TIME_COUNTER_EN_BMSK              0x10000
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_16_TX_TIME_COUNTER_EN_SHFT                 0x10
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_15_TX_TIME_COUNTER_EN_BMSK               0x8000
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_15_TX_TIME_COUNTER_EN_SHFT                  0xf
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_14_TX_TIME_COUNTER_EN_BMSK               0x4000
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_14_TX_TIME_COUNTER_EN_SHFT                  0xe
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_13_TX_TIME_COUNTER_EN_BMSK               0x2000
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_13_TX_TIME_COUNTER_EN_SHFT                  0xd
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_12_TX_TIME_COUNTER_EN_BMSK               0x1000
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_12_TX_TIME_COUNTER_EN_SHFT                  0xc
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_11_TX_TIME_COUNTER_EN_BMSK                0x800
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_11_TX_TIME_COUNTER_EN_SHFT                  0xb
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_10_TX_TIME_COUNTER_EN_BMSK                0x400
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_10_TX_TIME_COUNTER_EN_SHFT                  0xa
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_9_TX_TIME_COUNTER_EN_BMSK                 0x200
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_9_TX_TIME_COUNTER_EN_SHFT                   0x9
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_8_TX_TIME_COUNTER_EN_BMSK                 0x100
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_8_TX_TIME_COUNTER_EN_SHFT                   0x8
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_7_TX_TIME_COUNTER_EN_BMSK                  0x80
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_7_TX_TIME_COUNTER_EN_SHFT                   0x7
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_6_TX_TIME_COUNTER_EN_BMSK                  0x40
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_6_TX_TIME_COUNTER_EN_SHFT                   0x6
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_5_TX_TIME_COUNTER_EN_BMSK                  0x20
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_5_TX_TIME_COUNTER_EN_SHFT                   0x5
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_4_TX_TIME_COUNTER_EN_BMSK                  0x10
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_4_TX_TIME_COUNTER_EN_SHFT                   0x4
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_3_TX_TIME_COUNTER_EN_BMSK                   0x8
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_3_TX_TIME_COUNTER_EN_SHFT                   0x3
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_2_TX_TIME_COUNTER_EN_BMSK                   0x4
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_2_TX_TIME_COUNTER_EN_SHFT                   0x2
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_1_TX_TIME_COUNTER_EN_BMSK                   0x2
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_1_TX_TIME_COUNTER_EN_SHFT                   0x1
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_0_TX_TIME_COUNTER_EN_BMSK                   0x1
#define HWIO_SPMI_GENI_CFG_REG29_PRIM_0_TX_TIME_COUNTER_EN_SHFT                   0x0

#define HWIO_SPMI_GENI_CFG_REG30_ADDR                                      (SPMI_GENI_CFG_REG_BASE      + 0x00000178)
#define HWIO_SPMI_GENI_CFG_REG30_RMSK                                        0xffffff
#define HWIO_SPMI_GENI_CFG_REG30_IN          \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG30_ADDR, HWIO_SPMI_GENI_CFG_REG30_RMSK)
#define HWIO_SPMI_GENI_CFG_REG30_INM(m)      \
        in_dword_masked(HWIO_SPMI_GENI_CFG_REG30_ADDR, m)
#define HWIO_SPMI_GENI_CFG_REG30_OUT(v)      \
        out_dword(HWIO_SPMI_GENI_CFG_REG30_ADDR,v)
#define HWIO_SPMI_GENI_CFG_REG30_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_REG30_ADDR,m,v,HWIO_SPMI_GENI_CFG_REG30_IN)
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_23_RX_TIME_COUNTER_EN_BMSK             0x800000
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_23_RX_TIME_COUNTER_EN_SHFT                 0x17
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_22_RX_TIME_COUNTER_EN_BMSK             0x400000
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_22_RX_TIME_COUNTER_EN_SHFT                 0x16
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_21_RX_TIME_COUNTER_EN_BMSK             0x200000
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_21_RX_TIME_COUNTER_EN_SHFT                 0x15
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_20_RX_TIME_COUNTER_EN_BMSK             0x100000
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_20_RX_TIME_COUNTER_EN_SHFT                 0x14
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_19_RX_TIME_COUNTER_EN_BMSK              0x80000
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_19_RX_TIME_COUNTER_EN_SHFT                 0x13
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_18_RX_TIME_COUNTER_EN_BMSK              0x40000
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_18_RX_TIME_COUNTER_EN_SHFT                 0x12
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_17_RX_TIME_COUNTER_EN_BMSK              0x20000
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_17_RX_TIME_COUNTER_EN_SHFT                 0x11
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_16_RX_TIME_COUNTER_EN_BMSK              0x10000
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_16_RX_TIME_COUNTER_EN_SHFT                 0x10
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_15_RX_TIME_COUNTER_EN_BMSK               0x8000
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_15_RX_TIME_COUNTER_EN_SHFT                  0xf
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_14_RX_TIME_COUNTER_EN_BMSK               0x4000
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_14_RX_TIME_COUNTER_EN_SHFT                  0xe
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_13_RX_TIME_COUNTER_EN_BMSK               0x2000
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_13_RX_TIME_COUNTER_EN_SHFT                  0xd
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_12_RX_TIME_COUNTER_EN_BMSK               0x1000
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_12_RX_TIME_COUNTER_EN_SHFT                  0xc
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_11_RX_TIME_COUNTER_EN_BMSK                0x800
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_11_RX_TIME_COUNTER_EN_SHFT                  0xb
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_10_RX_TIME_COUNTER_EN_BMSK                0x400
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_10_RX_TIME_COUNTER_EN_SHFT                  0xa
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_9_RX_TIME_COUNTER_EN_BMSK                 0x200
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_9_RX_TIME_COUNTER_EN_SHFT                   0x9
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_8_RX_TIME_COUNTER_EN_BMSK                 0x100
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_8_RX_TIME_COUNTER_EN_SHFT                   0x8
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_7_RX_TIME_COUNTER_EN_BMSK                  0x80
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_7_RX_TIME_COUNTER_EN_SHFT                   0x7
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_6_RX_TIME_COUNTER_EN_BMSK                  0x40
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_6_RX_TIME_COUNTER_EN_SHFT                   0x6
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_5_RX_TIME_COUNTER_EN_BMSK                  0x20
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_5_RX_TIME_COUNTER_EN_SHFT                   0x5
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_4_RX_TIME_COUNTER_EN_BMSK                  0x10
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_4_RX_TIME_COUNTER_EN_SHFT                   0x4
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_3_RX_TIME_COUNTER_EN_BMSK                   0x8
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_3_RX_TIME_COUNTER_EN_SHFT                   0x3
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_2_RX_TIME_COUNTER_EN_BMSK                   0x4
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_2_RX_TIME_COUNTER_EN_SHFT                   0x2
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_1_RX_TIME_COUNTER_EN_BMSK                   0x2
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_1_RX_TIME_COUNTER_EN_SHFT                   0x1
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_0_RX_TIME_COUNTER_EN_BMSK                   0x1
#define HWIO_SPMI_GENI_CFG_REG30_PRIM_0_RX_TIME_COUNTER_EN_SHFT                   0x0

#define HWIO_SPMI_GENI_CFG_RAMn_ADDR(n)                                    (SPMI_GENI_CFG_REG_BASE      + 0x00000200 + 0x4 * (n))
#define HWIO_SPMI_GENI_CFG_RAMn_RMSK                                         0x1fffff
#define HWIO_SPMI_GENI_CFG_RAMn_MAXn                                              255
#define HWIO_SPMI_GENI_CFG_RAMn_INI(n)        \
        in_dword_masked(HWIO_SPMI_GENI_CFG_RAMn_ADDR(n), HWIO_SPMI_GENI_CFG_RAMn_RMSK)
#define HWIO_SPMI_GENI_CFG_RAMn_INMI(n,mask)    \
        in_dword_masked(HWIO_SPMI_GENI_CFG_RAMn_ADDR(n), mask)
#define HWIO_SPMI_GENI_CFG_RAMn_OUTI(n,val)    \
        out_dword(HWIO_SPMI_GENI_CFG_RAMn_ADDR(n),val)
#define HWIO_SPMI_GENI_CFG_RAMn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SPMI_GENI_CFG_RAMn_ADDR(n),mask,val,HWIO_SPMI_GENI_CFG_RAMn_INI(n))
#define HWIO_SPMI_GENI_CFG_RAMn_TBD_BMSK                                     0x1fffff
#define HWIO_SPMI_GENI_CFG_RAMn_TBD_SHFT                                          0x0

/*----------------------------------------------------------------------------
 * MODULE: SPMI_CFG
 *--------------------------------------------------------------------------*/

#define SPMI_CFG_REG_BASE                                                             (PMIC_ARB_BASE      + 0x0000a700)

#define HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR(m)                                    (SPMI_CFG_REG_BASE      + 0x00000000 + 0x4 * (m))
#define HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_RMSK                                        0xfff0007
#define HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_MAXm                                              255
#define HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_INI(m)        \
        in_dword_masked(HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR(m), HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_RMSK)
#define HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_INMI(m,mask)    \
        in_dword_masked(HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR(m), mask)
#define HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_OUTI(m,val)    \
        out_dword(HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR(m),val)
#define HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR(m),mask,val,HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_INI(m))
#define HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_APID2PPID_BMSK                              0xfff0000
#define HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_APID2PPID_SHFT                                   0x10
#define HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_PERIPH2OWNER_BMSK                                 0x7
#define HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_PERIPH2OWNER_SHFT                                 0x0

#define HWIO_SPMI_MAPPING_TABLE_REGk_ADDR(k)                                          (SPMI_CFG_REG_BASE      + 0x00000400 + 0x4 * (k))
#define HWIO_SPMI_MAPPING_TABLE_REGk_RMSK                                               0x3fffff
#define HWIO_SPMI_MAPPING_TABLE_REGk_MAXk                                                    254
#define HWIO_SPMI_MAPPING_TABLE_REGk_INI(k)        \
        in_dword_masked(HWIO_SPMI_MAPPING_TABLE_REGk_ADDR(k), HWIO_SPMI_MAPPING_TABLE_REGk_RMSK)
#define HWIO_SPMI_MAPPING_TABLE_REGk_INMI(k,mask)    \
        in_dword_masked(HWIO_SPMI_MAPPING_TABLE_REGk_ADDR(k), mask)
#define HWIO_SPMI_MAPPING_TABLE_REGk_OUTI(k,val)    \
        out_dword(HWIO_SPMI_MAPPING_TABLE_REGk_ADDR(k),val)
#define HWIO_SPMI_MAPPING_TABLE_REGk_OUTMI(k,mask,val) \
        out_dword_masked_ns(HWIO_SPMI_MAPPING_TABLE_REGk_ADDR(k),mask,val,HWIO_SPMI_MAPPING_TABLE_REGk_INI(k))
#define HWIO_SPMI_MAPPING_TABLE_REGk_BIT_INDEX_BMSK                                     0x3c0000
#define HWIO_SPMI_MAPPING_TABLE_REGk_BIT_INDEX_SHFT                                         0x12
#define HWIO_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_0_FLAG_BMSK                           0x20000
#define HWIO_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_0_FLAG_SHFT                              0x11
#define HWIO_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_0_BMSK                                0x1fe00
#define HWIO_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_0_SHFT                                    0x9
#define HWIO_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_1_FLAG_BMSK                             0x100
#define HWIO_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_1_FLAG_SHFT                               0x8
#define HWIO_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_1_BMSK                                   0xff
#define HWIO_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_1_SHFT                                    0x0

#define HWIO_SPMI_MID_REG_ADDR                                                        (SPMI_CFG_REG_BASE      + 0x00000800)
#define HWIO_SPMI_MID_REG_RMSK                                                               0x3
#define HWIO_SPMI_MID_REG_IN          \
        in_dword_masked(HWIO_SPMI_MID_REG_ADDR, HWIO_SPMI_MID_REG_RMSK)
#define HWIO_SPMI_MID_REG_INM(m)      \
        in_dword_masked(HWIO_SPMI_MID_REG_ADDR, m)
#define HWIO_SPMI_MID_REG_OUT(v)      \
        out_dword(HWIO_SPMI_MID_REG_ADDR,v)
#define HWIO_SPMI_MID_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_MID_REG_ADDR,m,v,HWIO_SPMI_MID_REG_IN)
#define HWIO_SPMI_MID_REG_MID_BMSK                                                           0x3
#define HWIO_SPMI_MID_REG_MID_SHFT                                                           0x0

#define HWIO_SPMI_CFG_REG_ADDR                                                        (SPMI_CFG_REG_BASE      + 0x00000804)
#define HWIO_SPMI_CFG_REG_RMSK                                                           0x1ffff
#define HWIO_SPMI_CFG_REG_IN          \
        in_dword_masked(HWIO_SPMI_CFG_REG_ADDR, HWIO_SPMI_CFG_REG_RMSK)
#define HWIO_SPMI_CFG_REG_INM(m)      \
        in_dword_masked(HWIO_SPMI_CFG_REG_ADDR, m)
#define HWIO_SPMI_CFG_REG_OUT(v)      \
        out_dword(HWIO_SPMI_CFG_REG_ADDR,v)
#define HWIO_SPMI_CFG_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_CFG_REG_ADDR,m,v,HWIO_SPMI_CFG_REG_IN)
#define HWIO_SPMI_CFG_REG_ARBITER_CTRL_BMSK                                              0x1f000
#define HWIO_SPMI_CFG_REG_ARBITER_CTRL_SHFT                                                  0xc
#define HWIO_SPMI_CFG_REG_ASYNC_SSC_DET_INT_DIS_BMSK                                       0x800
#define HWIO_SPMI_CFG_REG_ASYNC_SSC_DET_INT_DIS_SHFT                                         0xb
#define HWIO_SPMI_CFG_REG_SSC_Q1_DELAY_DIS_BMSK                                            0x400
#define HWIO_SPMI_CFG_REG_SSC_Q1_DELAY_DIS_SHFT                                              0xa
#define HWIO_SPMI_CFG_REG_ARB_UNKNOWN_EN_BMSK                                              0x200
#define HWIO_SPMI_CFG_REG_ARB_UNKNOWN_EN_SHFT                                                0x9
#define HWIO_SPMI_CFG_REG_SSC_WINDOW_EN_BMSK                                               0x100
#define HWIO_SPMI_CFG_REG_SSC_WINDOW_EN_SHFT                                                 0x8
#define HWIO_SPMI_CFG_REG_SSC_DET_INT_DIS_BMSK                                              0x80
#define HWIO_SPMI_CFG_REG_SSC_DET_INT_DIS_SHFT                                               0x7
#define HWIO_SPMI_CFG_REG_FORCE_MPM_CLK_REQ_IMM_BMSK                                        0x40
#define HWIO_SPMI_CFG_REG_FORCE_MPM_CLK_REQ_IMM_SHFT                                         0x6
#define HWIO_SPMI_CFG_REG_FORCE_ARB_AFTER_MASTER_TO_BMSK                                    0x20
#define HWIO_SPMI_CFG_REG_FORCE_ARB_AFTER_MASTER_TO_SHFT                                     0x5
#define HWIO_SPMI_CFG_REG_BUS_IDLE_CONN_MODE_BMSK                                           0x10
#define HWIO_SPMI_CFG_REG_BUS_IDLE_CONN_MODE_SHFT                                            0x4
#define HWIO_SPMI_CFG_REG_FORCE_MASTER_WRITE_ON_ERROR_BMSK                                   0x8
#define HWIO_SPMI_CFG_REG_FORCE_MASTER_WRITE_ON_ERROR_SHFT                                   0x3
#define HWIO_SPMI_CFG_REG_FORCE_MPM_CLK_REQ_BMSK                                             0x4
#define HWIO_SPMI_CFG_REG_FORCE_MPM_CLK_REQ_SHFT                                             0x2
#define HWIO_SPMI_CFG_REG_ARBITER_BYPASS_BMSK                                                0x2
#define HWIO_SPMI_CFG_REG_ARBITER_BYPASS_SHFT                                                0x1
#define HWIO_SPMI_CFG_REG_ARBITER_ENABLE_BMSK                                                0x1
#define HWIO_SPMI_CFG_REG_ARBITER_ENABLE_SHFT                                                0x0

#define HWIO_SPMI_SEC_DISABLE_REG_ADDR                                                (SPMI_CFG_REG_BASE      + 0x00000808)
#define HWIO_SPMI_SEC_DISABLE_REG_RMSK                                                       0x1
#define HWIO_SPMI_SEC_DISABLE_REG_IN          \
        in_dword_masked(HWIO_SPMI_SEC_DISABLE_REG_ADDR, HWIO_SPMI_SEC_DISABLE_REG_RMSK)
#define HWIO_SPMI_SEC_DISABLE_REG_INM(m)      \
        in_dword_masked(HWIO_SPMI_SEC_DISABLE_REG_ADDR, m)
#define HWIO_SPMI_SEC_DISABLE_REG_OUT(v)      \
        out_dword(HWIO_SPMI_SEC_DISABLE_REG_ADDR,v)
#define HWIO_SPMI_SEC_DISABLE_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_SEC_DISABLE_REG_ADDR,m,v,HWIO_SPMI_SEC_DISABLE_REG_IN)
#define HWIO_SPMI_SEC_DISABLE_REG_DISABLE_SECURITY_BMSK                                      0x1
#define HWIO_SPMI_SEC_DISABLE_REG_DISABLE_SECURITY_SHFT                                      0x0

#define HWIO_SPMI_HW_VERSION_ADDR                                                     (SPMI_CFG_REG_BASE      + 0x0000080c)
#define HWIO_SPMI_HW_VERSION_RMSK                                                     0xffffffff
#define HWIO_SPMI_HW_VERSION_IN          \
        in_dword_masked(HWIO_SPMI_HW_VERSION_ADDR, HWIO_SPMI_HW_VERSION_RMSK)
#define HWIO_SPMI_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_SPMI_HW_VERSION_ADDR, m)
#define HWIO_SPMI_HW_VERSION_HW_VERSION_MAJOR_BMSK                                    0xf0000000
#define HWIO_SPMI_HW_VERSION_HW_VERSION_MAJOR_SHFT                                          0x1c
#define HWIO_SPMI_HW_VERSION_HW_VERSION_MINOR_BMSK                                     0xfff0000
#define HWIO_SPMI_HW_VERSION_HW_VERSION_MINOR_SHFT                                          0x10
#define HWIO_SPMI_HW_VERSION_HW_VERSION_STEP_BMSK                                         0xffff
#define HWIO_SPMI_HW_VERSION_HW_VERSION_STEP_SHFT                                            0x0

#define HWIO_SPMI_CGC_CTRL_ADDR                                                       (SPMI_CFG_REG_BASE      + 0x00000810)
#define HWIO_SPMI_CGC_CTRL_RMSK                                                             0x7f
#define HWIO_SPMI_CGC_CTRL_IN          \
        in_dword_masked(HWIO_SPMI_CGC_CTRL_ADDR, HWIO_SPMI_CGC_CTRL_RMSK)
#define HWIO_SPMI_CGC_CTRL_INM(m)      \
        in_dword_masked(HWIO_SPMI_CGC_CTRL_ADDR, m)
#define HWIO_SPMI_CGC_CTRL_OUT(v)      \
        out_dword(HWIO_SPMI_CGC_CTRL_ADDR,v)
#define HWIO_SPMI_CGC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_CGC_CTRL_ADDR,m,v,HWIO_SPMI_CGC_CTRL_IN)
#define HWIO_SPMI_CGC_CTRL_MAP_LOGIC_CLK_CGC_ON_BMSK                                        0x40
#define HWIO_SPMI_CGC_CTRL_MAP_LOGIC_CLK_CGC_ON_SHFT                                         0x6
#define HWIO_SPMI_CGC_CTRL_RPU_CLK_CGC_ON_BMSK                                              0x20
#define HWIO_SPMI_CGC_CTRL_RPU_CLK_CGC_ON_SHFT                                               0x5
#define HWIO_SPMI_CGC_CTRL_MWB_CLK_CGC_ON_BMSK                                              0x10
#define HWIO_SPMI_CGC_CTRL_MWB_CLK_CGC_ON_SHFT                                               0x4
#define HWIO_SPMI_CGC_CTRL_PIC_CLK_CGC_ON_BMSK                                               0x8
#define HWIO_SPMI_CGC_CTRL_PIC_CLK_CGC_ON_SHFT                                               0x3
#define HWIO_SPMI_CGC_CTRL_PAC_CLK_CGC_ON_BMSK                                               0x4
#define HWIO_SPMI_CGC_CTRL_PAC_CLK_CGC_ON_SHFT                                               0x2
#define HWIO_SPMI_CGC_CTRL_CFG_AHB_BRIDGE_WR_CLK_CGC_ON_BMSK                                 0x2
#define HWIO_SPMI_CGC_CTRL_CFG_AHB_BRIDGE_WR_CLK_CGC_ON_SHFT                                 0x1
#define HWIO_SPMI_CGC_CTRL_CFG_AHB_BRIDGE_CLK_CGC_ON_BMSK                                    0x1
#define HWIO_SPMI_CGC_CTRL_CFG_AHB_BRIDGE_CLK_CGC_ON_SHFT                                    0x0

#define HWIO_SPMI_MWB_ENABLE_REG_ADDR                                                 (SPMI_CFG_REG_BASE      + 0x00000814)
#define HWIO_SPMI_MWB_ENABLE_REG_RMSK                                                        0x1
#define HWIO_SPMI_MWB_ENABLE_REG_IN          \
        in_dword_masked(HWIO_SPMI_MWB_ENABLE_REG_ADDR, HWIO_SPMI_MWB_ENABLE_REG_RMSK)
#define HWIO_SPMI_MWB_ENABLE_REG_INM(m)      \
        in_dword_masked(HWIO_SPMI_MWB_ENABLE_REG_ADDR, m)
#define HWIO_SPMI_MWB_ENABLE_REG_OUT(v)      \
        out_dword(HWIO_SPMI_MWB_ENABLE_REG_ADDR,v)
#define HWIO_SPMI_MWB_ENABLE_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_MWB_ENABLE_REG_ADDR,m,v,HWIO_SPMI_MWB_ENABLE_REG_IN)
#define HWIO_SPMI_MWB_ENABLE_REG_MWB_ENABLE_BMSK                                             0x1
#define HWIO_SPMI_MWB_ENABLE_REG_MWB_ENABLE_SHFT                                             0x0

#define HWIO_SPMI_CHAR_CFG_ADDR                                                       (SPMI_CFG_REG_BASE      + 0x00000840)
#define HWIO_SPMI_CHAR_CFG_RMSK                                                          0xfff11
#define HWIO_SPMI_CHAR_CFG_IN          \
        in_dword_masked(HWIO_SPMI_CHAR_CFG_ADDR, HWIO_SPMI_CHAR_CFG_RMSK)
#define HWIO_SPMI_CHAR_CFG_INM(m)      \
        in_dword_masked(HWIO_SPMI_CHAR_CFG_ADDR, m)
#define HWIO_SPMI_CHAR_CFG_OUT(v)      \
        out_dword(HWIO_SPMI_CHAR_CFG_ADDR,v)
#define HWIO_SPMI_CHAR_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_CHAR_CFG_ADDR,m,v,HWIO_SPMI_CHAR_CFG_IN)
#define HWIO_SPMI_CHAR_CFG_CHAR_MODE_BMSK                                                0xff000
#define HWIO_SPMI_CHAR_CFG_CHAR_MODE_SHFT                                                    0xc
#define HWIO_SPMI_CHAR_CFG_CHAR_STATUS_BMSK                                                0xf00
#define HWIO_SPMI_CHAR_CFG_CHAR_STATUS_SHFT                                                  0x8
#define HWIO_SPMI_CHAR_CFG_DIRECTION_BMSK                                                   0x10
#define HWIO_SPMI_CHAR_CFG_DIRECTION_SHFT                                                    0x4
#define HWIO_SPMI_CHAR_CFG_ENABLE_BMSK                                                       0x1
#define HWIO_SPMI_CHAR_CFG_ENABLE_SHFT                                                       0x0

#define HWIO_SPMI_CHAR_DATA_n_ADDR(n)                                                 (SPMI_CFG_REG_BASE      + 0x00000844 + 0x4 * (n))
#define HWIO_SPMI_CHAR_DATA_n_RMSK                                                        0xffff
#define HWIO_SPMI_CHAR_DATA_n_MAXn                                                             1
#define HWIO_SPMI_CHAR_DATA_n_INI(n)        \
        in_dword_masked(HWIO_SPMI_CHAR_DATA_n_ADDR(n), HWIO_SPMI_CHAR_DATA_n_RMSK)
#define HWIO_SPMI_CHAR_DATA_n_INMI(n,mask)    \
        in_dword_masked(HWIO_SPMI_CHAR_DATA_n_ADDR(n), mask)
#define HWIO_SPMI_CHAR_DATA_n_OUTI(n,val)    \
        out_dword(HWIO_SPMI_CHAR_DATA_n_ADDR(n),val)
#define HWIO_SPMI_CHAR_DATA_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SPMI_CHAR_DATA_n_ADDR(n),mask,val,HWIO_SPMI_CHAR_DATA_n_INI(n))
#define HWIO_SPMI_CHAR_DATA_n_DIN_ACTUAL_BMSK                                             0xff00
#define HWIO_SPMI_CHAR_DATA_n_DIN_ACTUAL_SHFT                                                0x8
#define HWIO_SPMI_CHAR_DATA_n_DOUT_DATA_DIN_EXP_BMSK                                        0xff
#define HWIO_SPMI_CHAR_DATA_n_DOUT_DATA_DIN_EXP_SHFT                                         0x0

#define HWIO_SPMI_CMPR_EN_REG_ADDR                                                    (SPMI_CFG_REG_BASE      + 0x00000854)
#define HWIO_SPMI_CMPR_EN_REG_RMSK                                                           0x1
#define HWIO_SPMI_CMPR_EN_REG_IN          \
        in_dword_masked(HWIO_SPMI_CMPR_EN_REG_ADDR, HWIO_SPMI_CMPR_EN_REG_RMSK)
#define HWIO_SPMI_CMPR_EN_REG_INM(m)      \
        in_dword_masked(HWIO_SPMI_CMPR_EN_REG_ADDR, m)
#define HWIO_SPMI_CMPR_EN_REG_OUT(v)      \
        out_dword(HWIO_SPMI_CMPR_EN_REG_ADDR,v)
#define HWIO_SPMI_CMPR_EN_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_CMPR_EN_REG_ADDR,m,v,HWIO_SPMI_CMPR_EN_REG_IN)
#define HWIO_SPMI_CMPR_EN_REG_CMPR_ENABLE_BMSK                                               0x1
#define HWIO_SPMI_CMPR_EN_REG_CMPR_ENABLE_SHFT                                               0x0

#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_ADDR                                            (SPMI_CFG_REG_BASE      + 0x00000900)
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_RMSK                                                 0xfff
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_IN          \
        in_dword_masked(HWIO_SPMI_PROTOCOL_IRQ_STATUS_ADDR, HWIO_SPMI_PROTOCOL_IRQ_STATUS_RMSK)
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_INM(m)      \
        in_dword_masked(HWIO_SPMI_PROTOCOL_IRQ_STATUS_ADDR, m)
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_ARBITER_DISCONNECTED_BMSK                            0x800
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_ARBITER_DISCONNECTED_SHFT                              0xb
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_ARBITER_CONNECTED_BMSK                               0x400
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_ARBITER_CONNECTED_SHFT                                 0xa
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_PERIH_IRQ_LOST_BMSK                                  0x200
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_PERIH_IRQ_LOST_SHFT                                    0x9
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_UNEXPECTED_SSC_BMSK                                  0x100
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_UNEXPECTED_SSC_SHFT                                    0x8
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_NO_RESPONSE_DATA_FRAME_DETECTED_BMSK                  0x80
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_NO_RESPONSE_DATA_FRAME_DETECTED_SHFT                   0x7
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_NO_RESPONSE_CMD_FRAME_DETECTED_BMSK                   0x40
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_NO_RESPONSE_CMD_FRAME_DETECTED_SHFT                    0x6
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_FALSE_MASTER_ARBITRATION_WIN_BMSK                     0x20
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_FALSE_MASTER_ARBITRATION_WIN_SHFT                      0x5
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_FALSE_BUS_REQUEST_BMSK                                0x10
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_FALSE_BUS_REQUEST_SHFT                                 0x4
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_UNSUPPORTED_COMMAND_BMSK                               0x8
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_UNSUPPORTED_COMMAND_SHFT                               0x3
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_DATA_ADDR_FRAME_PARITY_ERROR_BMSK                      0x4
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_DATA_ADDR_FRAME_PARITY_ERROR_SHFT                      0x2
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_SLAVE_CMD_FRAME_PARITY_ERROR_BMSK                      0x2
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_SLAVE_CMD_FRAME_PARITY_ERROR_SHFT                      0x1
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_MASTER_CMD_FRAME_PARITY_ERROR_BMSK                     0x1
#define HWIO_SPMI_PROTOCOL_IRQ_STATUS_MASTER_CMD_FRAME_PARITY_ERROR_SHFT                     0x0

#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_ADDR                                            (SPMI_CFG_REG_BASE      + 0x00000904)
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_RMSK                                                 0xfff
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_IN          \
        in_dword_masked(HWIO_SPMI_PROTOCOL_IRQ_ENABLE_ADDR, HWIO_SPMI_PROTOCOL_IRQ_ENABLE_RMSK)
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_INM(m)      \
        in_dword_masked(HWIO_SPMI_PROTOCOL_IRQ_ENABLE_ADDR, m)
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_OUT(v)      \
        out_dword(HWIO_SPMI_PROTOCOL_IRQ_ENABLE_ADDR,v)
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_PROTOCOL_IRQ_ENABLE_ADDR,m,v,HWIO_SPMI_PROTOCOL_IRQ_ENABLE_IN)
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_ARBITER_DISCONNECTED_BMSK                            0x800
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_ARBITER_DISCONNECTED_SHFT                              0xb
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_ARBITER_CONNECTED_BMSK                               0x400
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_ARBITER_CONNECTED_SHFT                                 0xa
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_PERIH_IRQ_LOST_BMSK                                  0x200
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_PERIH_IRQ_LOST_SHFT                                    0x9
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_UNEXPECTED_SSC_BMSK                                  0x100
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_UNEXPECTED_SSC_SHFT                                    0x8
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_NO_RESPONSE_DATA_FRAME_DETECTED_BMSK                  0x80
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_NO_RESPONSE_DATA_FRAME_DETECTED_SHFT                   0x7
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_NO_RESPONSE_CMD_FRAME_DETECTED_BMSK                   0x40
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_NO_RESPONSE_CMD_FRAME_DETECTED_SHFT                    0x6
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_FALSE_MASTER_ARBITRATION_WIN_BMSK                     0x20
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_FALSE_MASTER_ARBITRATION_WIN_SHFT                      0x5
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_FALSE_BUS_REQUEST_BMSK                                0x10
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_FALSE_BUS_REQUEST_SHFT                                 0x4
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_UNSUPPORTED_COMMAND_BMSK                               0x8
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_UNSUPPORTED_COMMAND_SHFT                               0x3
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_DATA_ADDR_FRAME_PARITY_ERROR_BMSK                      0x4
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_DATA_ADDR_FRAME_PARITY_ERROR_SHFT                      0x2
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_SLAVE_CMD_FRAME_PARITY_ERROR_BMSK                      0x2
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_SLAVE_CMD_FRAME_PARITY_ERROR_SHFT                      0x1
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_MASTER_CMD_FRAME_PARITY_ERROR_BMSK                     0x1
#define HWIO_SPMI_PROTOCOL_IRQ_ENABLE_MASTER_CMD_FRAME_PARITY_ERROR_SHFT                     0x0

#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_ADDR                                             (SPMI_CFG_REG_BASE      + 0x00000908)
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_RMSK                                                  0xfff
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_OUT(v)      \
        out_dword(HWIO_SPMI_PROTOCOL_IRQ_CLEAR_ADDR,v)
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_ARBITER_DISCONNECTED_BMSK                             0x800
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_ARBITER_DISCONNECTED_SHFT                               0xb
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_ARBITER_CONNECTED_BMSK                                0x400
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_ARBITER_CONNECTED_SHFT                                  0xa
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_PERIH_IRQ_LOST_BMSK                                   0x200
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_PERIH_IRQ_LOST_SHFT                                     0x9
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_UNEXPECTED_SSC_BMSK                                   0x100
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_UNEXPECTED_SSC_SHFT                                     0x8
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_NO_RESPONSE_DATA_FRAME_DETECTED_BMSK                   0x80
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_NO_RESPONSE_DATA_FRAME_DETECTED_SHFT                    0x7
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_NO_RESPONSE_CMD_FRAME_DETECTED_BMSK                    0x40
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_NO_RESPONSE_CMD_FRAME_DETECTED_SHFT                     0x6
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_FALSE_MASTER_ARBITRATION_WIN_BMSK                      0x20
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_FALSE_MASTER_ARBITRATION_WIN_SHFT                       0x5
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_FALSE_BUS_REQUEST_BMSK                                 0x10
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_FALSE_BUS_REQUEST_SHFT                                  0x4
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_UNSUPPORTED_COMMAND_BMSK                                0x8
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_UNSUPPORTED_COMMAND_SHFT                                0x3
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_DATA_ADDR_FRAME_PARITY_ERROR_BMSK                       0x4
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_DATA_ADDR_FRAME_PARITY_ERROR_SHFT                       0x2
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_SLAVE_CMD_FRAME_PARITY_ERROR_BMSK                       0x2
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_SLAVE_CMD_FRAME_PARITY_ERROR_SHFT                       0x1
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_MASTER_CMD_FRAME_PARITY_ERROR_BMSK                      0x1
#define HWIO_SPMI_PROTOCOL_IRQ_CLEAR_MASTER_CMD_FRAME_PARITY_ERROR_SHFT                      0x0

#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_ADDR                                            (SPMI_CFG_REG_BASE      + 0x0000090c)
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_RMSK                                                 0xfff
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_OUT(v)      \
        out_dword(HWIO_SPMI_PROTOCOL_IRQ_EN_SET_ADDR,v)
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_ARBITER_DISCONNECTED_BMSK                            0x800
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_ARBITER_DISCONNECTED_SHFT                              0xb
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_ARBITER_CONNECTED_BMSK                               0x400
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_ARBITER_CONNECTED_SHFT                                 0xa
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_PERIH_IRQ_LOST_BMSK                                  0x200
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_PERIH_IRQ_LOST_SHFT                                    0x9
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_UNEXPECTED_SSC_BMSK                                  0x100
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_UNEXPECTED_SSC_SHFT                                    0x8
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_NO_RESPONSE_DATA_FRAME_DETECTED_BMSK                  0x80
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_NO_RESPONSE_DATA_FRAME_DETECTED_SHFT                   0x7
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_NO_RESPONSE_CMD_FRAME_DETECTED_BMSK                   0x40
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_NO_RESPONSE_CMD_FRAME_DETECTED_SHFT                    0x6
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_FALSE_MASTER_ARBITRATION_WIN_BMSK                     0x20
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_FALSE_MASTER_ARBITRATION_WIN_SHFT                      0x5
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_FALSE_BUS_REQUEST_BMSK                                0x10
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_FALSE_BUS_REQUEST_SHFT                                 0x4
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_UNSUPPORTED_COMMAND_BMSK                               0x8
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_UNSUPPORTED_COMMAND_SHFT                               0x3
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_DATA_ADDR_FRAME_PARITY_ERROR_BMSK                      0x4
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_DATA_ADDR_FRAME_PARITY_ERROR_SHFT                      0x2
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_SLAVE_CMD_FRAME_PARITY_ERROR_BMSK                      0x2
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_SLAVE_CMD_FRAME_PARITY_ERROR_SHFT                      0x1
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_MASTER_CMD_FRAME_PARITY_ERROR_BMSK                     0x1
#define HWIO_SPMI_PROTOCOL_IRQ_EN_SET_MASTER_CMD_FRAME_PARITY_ERROR_SHFT                     0x0

#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_ADDR                                          (SPMI_CFG_REG_BASE      + 0x00000910)
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_RMSK                                               0xfff
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_OUT(v)      \
        out_dword(HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_ADDR,v)
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_ARBITER_DISCONNECTED_BMSK                          0x800
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_ARBITER_DISCONNECTED_SHFT                            0xb
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_ARBITER_CONNECTED_BMSK                             0x400
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_ARBITER_CONNECTED_SHFT                               0xa
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_PERIH_IRQ_LOST_BMSK                                0x200
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_PERIH_IRQ_LOST_SHFT                                  0x9
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_UNEXPECTED_SSC_BMSK                                0x100
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_UNEXPECTED_SSC_SHFT                                  0x8
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_NO_RESPONSE_DATA_FRAME_DETECTED_BMSK                0x80
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_NO_RESPONSE_DATA_FRAME_DETECTED_SHFT                 0x7
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_NO_RESPONSE_CMD_FRAME_DETECTED_BMSK                 0x40
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_NO_RESPONSE_CMD_FRAME_DETECTED_SHFT                  0x6
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_FALSE_MASTER_ARBITRATION_WIN_BMSK                   0x20
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_FALSE_MASTER_ARBITRATION_WIN_SHFT                    0x5
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_FALSE_BUS_REQUEST_BMSK                              0x10
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_FALSE_BUS_REQUEST_SHFT                               0x4
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_UNSUPPORTED_COMMAND_BMSK                             0x8
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_UNSUPPORTED_COMMAND_SHFT                             0x3
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_DATA_ADDR_FRAME_PARITY_ERROR_BMSK                    0x4
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_DATA_ADDR_FRAME_PARITY_ERROR_SHFT                    0x2
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_SLAVE_CMD_FRAME_PARITY_ERROR_BMSK                    0x2
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_SLAVE_CMD_FRAME_PARITY_ERROR_SHFT                    0x1
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_MASTER_CMD_FRAME_PARITY_ERROR_BMSK                   0x1
#define HWIO_SPMI_PROTOCOL_IRQ_EN_CLEAR_MASTER_CMD_FRAME_PARITY_ERROR_SHFT                   0x0

#define HWIO_SPMI_TEST_BUS_CTRL_ADDR                                                  (SPMI_CFG_REG_BASE      + 0x00001900)
#define HWIO_SPMI_TEST_BUS_CTRL_RMSK                                                       0x3ff
#define HWIO_SPMI_TEST_BUS_CTRL_IN          \
        in_dword_masked(HWIO_SPMI_TEST_BUS_CTRL_ADDR, HWIO_SPMI_TEST_BUS_CTRL_RMSK)
#define HWIO_SPMI_TEST_BUS_CTRL_INM(m)      \
        in_dword_masked(HWIO_SPMI_TEST_BUS_CTRL_ADDR, m)
#define HWIO_SPMI_TEST_BUS_CTRL_OUT(v)      \
        out_dword(HWIO_SPMI_TEST_BUS_CTRL_ADDR,v)
#define HWIO_SPMI_TEST_BUS_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_TEST_BUS_CTRL_ADDR,m,v,HWIO_SPMI_TEST_BUS_CTRL_IN)
#define HWIO_SPMI_TEST_BUS_CTRL_TEST_BUS_ARB_SEL_BMSK                                      0x3c0
#define HWIO_SPMI_TEST_BUS_CTRL_TEST_BUS_ARB_SEL_SHFT                                        0x6
#define HWIO_SPMI_TEST_BUS_CTRL_TEST_BUS_INT_SEL_BMSK                                       0x3c
#define HWIO_SPMI_TEST_BUS_CTRL_TEST_BUS_INT_SEL_SHFT                                        0x2
#define HWIO_SPMI_TEST_BUS_CTRL_TEST_BUS_SEL_BMSK                                            0x3
#define HWIO_SPMI_TEST_BUS_CTRL_TEST_BUS_SEL_SHFT                                            0x0

#define HWIO_SPMI_HW_SW_EVENTS_CTRL_ADDR                                              (SPMI_CFG_REG_BASE      + 0x00001904)
#define HWIO_SPMI_HW_SW_EVENTS_CTRL_RMSK                                                     0x7
#define HWIO_SPMI_HW_SW_EVENTS_CTRL_IN          \
        in_dword_masked(HWIO_SPMI_HW_SW_EVENTS_CTRL_ADDR, HWIO_SPMI_HW_SW_EVENTS_CTRL_RMSK)
#define HWIO_SPMI_HW_SW_EVENTS_CTRL_INM(m)      \
        in_dword_masked(HWIO_SPMI_HW_SW_EVENTS_CTRL_ADDR, m)
#define HWIO_SPMI_HW_SW_EVENTS_CTRL_OUT(v)      \
        out_dword(HWIO_SPMI_HW_SW_EVENTS_CTRL_ADDR,v)
#define HWIO_SPMI_HW_SW_EVENTS_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPMI_HW_SW_EVENTS_CTRL_ADDR,m,v,HWIO_SPMI_HW_SW_EVENTS_CTRL_IN)
#define HWIO_SPMI_HW_SW_EVENTS_CTRL_HW_SW_EVENTS_SEL_BMSK                                    0x7
#define HWIO_SPMI_HW_SW_EVENTS_CTRL_HW_SW_EVENTS_SEL_SHFT                                    0x0

#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_ADDR(j)                                   (SPMI_CFG_REG_BASE      + 0x00001908 + 0x4 * (j))
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_RMSK                                      0x81818181
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_MAXj                                               7
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_INI(j)        \
        in_dword_masked(HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_ADDR(j), HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_RMSK)
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_INMI(j,mask)    \
        in_dword_masked(HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_ADDR(j), mask)
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_OUTI(j,val)    \
        out_dword(HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_ADDR(j),val)
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_OUTMI(j,mask,val) \
        out_dword_masked_ns(HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_ADDR(j),mask,val,HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_INI(j))
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_HW_EVENTS_EN_D_BMSK                       0x80000000
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_HW_EVENTS_EN_D_SHFT                             0x1f
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_HW_EVENTS_SEL_D_BMSK                       0x1000000
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_HW_EVENTS_SEL_D_SHFT                            0x18
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_HW_EVENTS_EN_C_BMSK                         0x800000
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_HW_EVENTS_EN_C_SHFT                             0x17
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_HW_EVENTS_SEL_C_BMSK                         0x10000
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_HW_EVENTS_SEL_C_SHFT                            0x10
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_HW_EVENTS_EN_B_BMSK                           0x8000
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_HW_EVENTS_EN_B_SHFT                              0xf
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_HW_EVENTS_SEL_B_BMSK                           0x100
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_HW_EVENTS_SEL_B_SHFT                             0x8
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_HW_EVENTS_EN_A_BMSK                             0x80
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_HW_EVENTS_EN_A_SHFT                              0x7
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_HW_EVENTS_SEL_A_BMSK                             0x1
#define HWIO_SPMI_HW_SW_EVENTS_BITWISE_REGj_HW_EVENTS_SEL_A_SHFT                             0x0

/*----------------------------------------------------------------------------
 * MODULE: SPMI_PIC
 *--------------------------------------------------------------------------*/

#define SPMI_PIC_REG_BASE                                             (PMIC_ARB_BASE      + 0x01800000)

#define HWIO_SPMI_PIC_OWNERm_ACC_STATUSn_ADDR(m,n)                    (SPMI_PIC_REG_BASE      + 0x00100000 + 0x1000 * (m) + 0x4 * (n))
#define HWIO_SPMI_PIC_OWNERm_ACC_STATUSn_RMSK                         0xffffffff
#define HWIO_SPMI_PIC_OWNERm_ACC_STATUSn_MAXn                                  7
#define HWIO_SPMI_PIC_OWNERm_ACC_STATUSn_MAXm                                  6
#define HWIO_SPMI_PIC_OWNERm_ACC_STATUSn_INI2(m,n)        \
        in_dword_masked(HWIO_SPMI_PIC_OWNERm_ACC_STATUSn_ADDR(m,n), HWIO_SPMI_PIC_OWNERm_ACC_STATUSn_RMSK)
#define HWIO_SPMI_PIC_OWNERm_ACC_STATUSn_INMI2(m,n,mask)    \
        in_dword_masked(HWIO_SPMI_PIC_OWNERm_ACC_STATUSn_ADDR(m,n), mask)
#define HWIO_SPMI_PIC_OWNERm_ACC_STATUSn_INT_ACC_STATUS_BMSK          0xffffffff
#define HWIO_SPMI_PIC_OWNERm_ACC_STATUSn_INT_ACC_STATUS_SHFT                 0x0

#define HWIO_SPMI_PIC_ACC_ENABLEn_ADDR(n)                             (SPMI_PIC_REG_BASE      + 0x00000000 + 0x1000 * (n))
#define HWIO_SPMI_PIC_ACC_ENABLEn_RMSK                                       0x1
#define HWIO_SPMI_PIC_ACC_ENABLEn_MAXn                                       223
#define HWIO_SPMI_PIC_ACC_ENABLEn_INI(n)        \
        in_dword_masked(HWIO_SPMI_PIC_ACC_ENABLEn_ADDR(n), HWIO_SPMI_PIC_ACC_ENABLEn_RMSK)
#define HWIO_SPMI_PIC_ACC_ENABLEn_INMI(n,mask)    \
        in_dword_masked(HWIO_SPMI_PIC_ACC_ENABLEn_ADDR(n), mask)
#define HWIO_SPMI_PIC_ACC_ENABLEn_OUTI(n,val)    \
        out_dword(HWIO_SPMI_PIC_ACC_ENABLEn_ADDR(n),val)
#define HWIO_SPMI_PIC_ACC_ENABLEn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SPMI_PIC_ACC_ENABLEn_ADDR(n),mask,val,HWIO_SPMI_PIC_ACC_ENABLEn_INI(n))
#define HWIO_SPMI_PIC_ACC_ENABLEn_INT_ACC_ENABLE_BMSK                        0x1
#define HWIO_SPMI_PIC_ACC_ENABLEn_INT_ACC_ENABLE_SHFT                        0x0

#define HWIO_SPMI_PIC_IRQ_STATUSn_ADDR(n)                             (SPMI_PIC_REG_BASE      + 0x00000004 + 0x1000 * (n))
#define HWIO_SPMI_PIC_IRQ_STATUSn_RMSK                                      0xff
#define HWIO_SPMI_PIC_IRQ_STATUSn_MAXn                                       223
#define HWIO_SPMI_PIC_IRQ_STATUSn_INI(n)        \
        in_dword_masked(HWIO_SPMI_PIC_IRQ_STATUSn_ADDR(n), HWIO_SPMI_PIC_IRQ_STATUSn_RMSK)
#define HWIO_SPMI_PIC_IRQ_STATUSn_INMI(n,mask)    \
        in_dword_masked(HWIO_SPMI_PIC_IRQ_STATUSn_ADDR(n), mask)
#define HWIO_SPMI_PIC_IRQ_STATUSn_INT_STATUS_BMSK                           0xff
#define HWIO_SPMI_PIC_IRQ_STATUSn_INT_STATUS_SHFT                            0x0

#define HWIO_SPMI_PIC_IRQ_CLEARn_ADDR(n)                              (SPMI_PIC_REG_BASE      + 0x00000008 + 0x1000 * (n))
#define HWIO_SPMI_PIC_IRQ_CLEARn_RMSK                                       0xff
#define HWIO_SPMI_PIC_IRQ_CLEARn_MAXn                                        223
#define HWIO_SPMI_PIC_IRQ_CLEARn_OUTI(n,val)    \
        out_dword(HWIO_SPMI_PIC_IRQ_CLEARn_ADDR(n),val)
#define HWIO_SPMI_PIC_IRQ_CLEARn_INT_CLEAR_BMSK                             0xff
#define HWIO_SPMI_PIC_IRQ_CLEARn_INT_CLEAR_SHFT                              0x0


#endif /* __SPMICOREHWIO_H__ */
