#ifndef __HAL_SPM_HWIO_H__
#define __HAL_SPM_HWIO_H__
/*
===========================================================================
*/
/**
  @file HAL_spm_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    QDSP6SS_SAW2

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2013-2014 Qualcomm Technologies Incorporated.
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

  $Header: //components/rel/core.mpss/3.5.c12.3/power/spm/src/hal/hwio/9607/HALspm_hwio.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/
#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: QDSP6SS_SAW2
 *--------------------------------------------------------------------------*/

#define QDSP6SS_SAW2_REG_BASE                                    (MSS_TOP_BASE      + 0x00130000)

#define HWIO_QDSP6SS_SAW2_SECURE_ADDR                            (QDSP6SS_SAW2_REG_BASE      + 0x00000000)
#define HWIO_QDSP6SS_SAW2_SECURE_RMSK                                   0x7
#define HWIO_QDSP6SS_SAW2_SECURE_IN          \
        in_dword_masked(HWIO_QDSP6SS_SAW2_SECURE_ADDR, HWIO_QDSP6SS_SAW2_SECURE_RMSK)
#define HWIO_QDSP6SS_SAW2_SECURE_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_SAW2_SECURE_ADDR, m)
#define HWIO_QDSP6SS_SAW2_SECURE_OUT(v)      \
        out_dword(HWIO_QDSP6SS_SAW2_SECURE_ADDR,v)
#define HWIO_QDSP6SS_SAW2_SECURE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_SAW2_SECURE_ADDR,m,v,HWIO_QDSP6SS_SAW2_SECURE_IN)
#define HWIO_QDSP6SS_SAW2_SECURE_SAW_CTL_BMSK                           0x4
#define HWIO_QDSP6SS_SAW2_SECURE_SAW_CTL_SHFT                           0x2
#define HWIO_QDSP6SS_SAW2_SECURE_PWR_CTL_BMSK                           0x2
#define HWIO_QDSP6SS_SAW2_SECURE_PWR_CTL_SHFT                           0x1
#define HWIO_QDSP6SS_SAW2_SECURE_VLT_CTL_BMSK                           0x1
#define HWIO_QDSP6SS_SAW2_SECURE_VLT_CTL_SHFT                           0x0

#define HWIO_QDSP6SS_SAW2_ID_ADDR                                (QDSP6SS_SAW2_REG_BASE      + 0x00000004)
#define HWIO_QDSP6SS_SAW2_ID_RMSK                                0xff3f1f7f
#define HWIO_QDSP6SS_SAW2_ID_IN          \
        in_dword_masked(HWIO_QDSP6SS_SAW2_ID_ADDR, HWIO_QDSP6SS_SAW2_ID_RMSK)
#define HWIO_QDSP6SS_SAW2_ID_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_SAW2_ID_ADDR, m)
#define HWIO_QDSP6SS_SAW2_ID_NUM_SPM_ENTRY_BMSK                  0xff000000
#define HWIO_QDSP6SS_SAW2_ID_NUM_SPM_ENTRY_SHFT                        0x18
#define HWIO_QDSP6SS_SAW2_ID_NUM_PWR_CTL_BMSK                      0x3f0000
#define HWIO_QDSP6SS_SAW2_ID_NUM_PWR_CTL_SHFT                          0x10
#define HWIO_QDSP6SS_SAW2_ID_PMIC_DATA_WIDTH_BMSK                    0x1f00
#define HWIO_QDSP6SS_SAW2_ID_PMIC_DATA_WIDTH_SHFT                       0x8
#define HWIO_QDSP6SS_SAW2_ID_NUM_PMIC_DATA_BMSK                        0x70
#define HWIO_QDSP6SS_SAW2_ID_NUM_PMIC_DATA_SHFT                         0x4
#define HWIO_QDSP6SS_SAW2_ID_CFG_NS_ACCESS_BMSK                         0x8
#define HWIO_QDSP6SS_SAW2_ID_CFG_NS_ACCESS_SHFT                         0x3
#define HWIO_QDSP6SS_SAW2_ID_PMIC_ARB_INTF_BMSK                         0x4
#define HWIO_QDSP6SS_SAW2_ID_PMIC_ARB_INTF_SHFT                         0x2
#define HWIO_QDSP6SS_SAW2_ID_AVS_PRESENT_BMSK                           0x2
#define HWIO_QDSP6SS_SAW2_ID_AVS_PRESENT_SHFT                           0x1
#define HWIO_QDSP6SS_SAW2_ID_SPM_PRESENT_BMSK                           0x1
#define HWIO_QDSP6SS_SAW2_ID_SPM_PRESENT_SHFT                           0x0

#define HWIO_QDSP6SS_SAW2_CFG_ADDR                               (QDSP6SS_SAW2_REG_BASE      + 0x00000008)
#define HWIO_QDSP6SS_SAW2_CFG_RMSK                                    0x31f
#define HWIO_QDSP6SS_SAW2_CFG_IN          \
        in_dword_masked(HWIO_QDSP6SS_SAW2_CFG_ADDR, HWIO_QDSP6SS_SAW2_CFG_RMSK)
#define HWIO_QDSP6SS_SAW2_CFG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_SAW2_CFG_ADDR, m)
#define HWIO_QDSP6SS_SAW2_CFG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_SAW2_CFG_ADDR,v)
#define HWIO_QDSP6SS_SAW2_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_SAW2_CFG_ADDR,m,v,HWIO_QDSP6SS_SAW2_CFG_IN)
#define HWIO_QDSP6SS_SAW2_CFG_TEST_BUS_EN_BMSK                        0x200
#define HWIO_QDSP6SS_SAW2_CFG_TEST_BUS_EN_SHFT                          0x9
#define HWIO_QDSP6SS_SAW2_CFG_FRC_REF_CLK_ON_BMSK                     0x100
#define HWIO_QDSP6SS_SAW2_CFG_FRC_REF_CLK_ON_SHFT                       0x8
#define HWIO_QDSP6SS_SAW2_CFG_CLK_DIV_BMSK                             0x1f
#define HWIO_QDSP6SS_SAW2_CFG_CLK_DIV_SHFT                              0x0

#define HWIO_QDSP6SS_SAW2_SPM_STS_ADDR                           (QDSP6SS_SAW2_REG_BASE      + 0x0000000c)
#define HWIO_QDSP6SS_SAW2_SPM_STS_RMSK                           0xfffffdff
#define HWIO_QDSP6SS_SAW2_SPM_STS_IN          \
        in_dword_masked(HWIO_QDSP6SS_SAW2_SPM_STS_ADDR, HWIO_QDSP6SS_SAW2_SPM_STS_RMSK)
#define HWIO_QDSP6SS_SAW2_SPM_STS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_SAW2_SPM_STS_ADDR, m)
#define HWIO_QDSP6SS_SAW2_SPM_STS_CURR_PWR_CTL_BMSK              0xffff0000
#define HWIO_QDSP6SS_SAW2_SPM_STS_CURR_PWR_CTL_SHFT                    0x10
#define HWIO_QDSP6SS_SAW2_SPM_STS_SHTDWN_REQ_BMSK                    0x8000
#define HWIO_QDSP6SS_SAW2_SPM_STS_SHTDWN_REQ_SHFT                       0xf
#define HWIO_QDSP6SS_SAW2_SPM_STS_SHTDWN_ACK_BMSK                    0x4000
#define HWIO_QDSP6SS_SAW2_SPM_STS_SHTDWN_ACK_SHFT                       0xe
#define HWIO_QDSP6SS_SAW2_SPM_STS_BRNGUP_REQ_BMSK                    0x2000
#define HWIO_QDSP6SS_SAW2_SPM_STS_BRNGUP_REQ_SHFT                       0xd
#define HWIO_QDSP6SS_SAW2_SPM_STS_BRNGUP_ACK_BMSK                    0x1000
#define HWIO_QDSP6SS_SAW2_SPM_STS_BRNGUP_ACK_SHFT                       0xc
#define HWIO_QDSP6SS_SAW2_SPM_STS_RPM_STATE_BMSK                      0xc00
#define HWIO_QDSP6SS_SAW2_SPM_STS_RPM_STATE_SHFT                        0xa
#define HWIO_QDSP6SS_SAW2_SPM_STS_SPM_CMD_ADDR_BMSK                   0x1ff
#define HWIO_QDSP6SS_SAW2_SPM_STS_SPM_CMD_ADDR_SHFT                     0x0

#define HWIO_QDSP6SS_SAW2_AVS_STS_ADDR                           (QDSP6SS_SAW2_REG_BASE      + 0x00000010)
#define HWIO_QDSP6SS_SAW2_AVS_STS_RMSK                             0xff007f
#define HWIO_QDSP6SS_SAW2_AVS_STS_IN          \
        in_dword_masked(HWIO_QDSP6SS_SAW2_AVS_STS_ADDR, HWIO_QDSP6SS_SAW2_AVS_STS_RMSK)
#define HWIO_QDSP6SS_SAW2_AVS_STS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_SAW2_AVS_STS_ADDR, m)
#define HWIO_QDSP6SS_SAW2_AVS_STS_SPM_EVENT_BMSK                   0xff0000
#define HWIO_QDSP6SS_SAW2_AVS_STS_SPM_EVENT_SHFT                       0x10
#define HWIO_QDSP6SS_SAW2_AVS_STS_AVS_FSM_STATE_BMSK                   0x40
#define HWIO_QDSP6SS_SAW2_AVS_STS_AVS_FSM_STATE_SHFT                    0x6
#define HWIO_QDSP6SS_SAW2_AVS_STS_MAX_INT_BMSK                         0x20
#define HWIO_QDSP6SS_SAW2_AVS_STS_MAX_INT_SHFT                          0x5
#define HWIO_QDSP6SS_SAW2_AVS_STS_MIN_INT_BMSK                         0x10
#define HWIO_QDSP6SS_SAW2_AVS_STS_MIN_INT_SHFT                          0x4
#define HWIO_QDSP6SS_SAW2_AVS_STS_CPU_UP_BMSK                           0x8
#define HWIO_QDSP6SS_SAW2_AVS_STS_CPU_UP_SHFT                           0x3
#define HWIO_QDSP6SS_SAW2_AVS_STS_CPU_DN_BMSK                           0x4
#define HWIO_QDSP6SS_SAW2_AVS_STS_CPU_DN_SHFT                           0x2
#define HWIO_QDSP6SS_SAW2_AVS_STS_SW_WR_PEND_BMSK                       0x2
#define HWIO_QDSP6SS_SAW2_AVS_STS_SW_WR_PEND_SHFT                       0x1
#define HWIO_QDSP6SS_SAW2_AVS_STS_AVS_STATE_BMSK                        0x1
#define HWIO_QDSP6SS_SAW2_AVS_STS_AVS_STATE_SHFT                        0x0

#define HWIO_QDSP6SS_SAW2_PMIC_STS_ADDR                          (QDSP6SS_SAW2_REG_BASE      + 0x00000014)
#define HWIO_QDSP6SS_SAW2_PMIC_STS_RMSK                          0x3ff700ff
#define HWIO_QDSP6SS_SAW2_PMIC_STS_IN          \
        in_dword_masked(HWIO_QDSP6SS_SAW2_PMIC_STS_ADDR, HWIO_QDSP6SS_SAW2_PMIC_STS_RMSK)
#define HWIO_QDSP6SS_SAW2_PMIC_STS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_SAW2_PMIC_STS_ADDR, m)
#define HWIO_QDSP6SS_SAW2_PMIC_STS_CURR_DLY_BMSK                 0x3ff00000
#define HWIO_QDSP6SS_SAW2_PMIC_STS_CURR_DLY_SHFT                       0x14
#define HWIO_QDSP6SS_SAW2_PMIC_STS_CURR_PMIC_SIZE_BMSK              0x40000
#define HWIO_QDSP6SS_SAW2_PMIC_STS_CURR_PMIC_SIZE_SHFT                 0x12
#define HWIO_QDSP6SS_SAW2_PMIC_STS_PMIC_STATE_BMSK                  0x30000
#define HWIO_QDSP6SS_SAW2_PMIC_STS_PMIC_STATE_SHFT                     0x10
#define HWIO_QDSP6SS_SAW2_PMIC_STS_CURR_PMIC_DATA_BMSK                 0xff
#define HWIO_QDSP6SS_SAW2_PMIC_STS_CURR_PMIC_DATA_SHFT                  0x0

#define HWIO_QDSP6SS_SAW2_RST_ADDR                               (QDSP6SS_SAW2_REG_BASE      + 0x00000018)
#define HWIO_QDSP6SS_SAW2_RST_RMSK                                      0x1
#define HWIO_QDSP6SS_SAW2_RST_IN          \
        in_dword_masked(HWIO_QDSP6SS_SAW2_RST_ADDR, HWIO_QDSP6SS_SAW2_RST_RMSK)
#define HWIO_QDSP6SS_SAW2_RST_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_SAW2_RST_ADDR, m)
#define HWIO_QDSP6SS_SAW2_RST_OUT(v)      \
        out_dword(HWIO_QDSP6SS_SAW2_RST_ADDR,v)
#define HWIO_QDSP6SS_SAW2_RST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_SAW2_RST_ADDR,m,v,HWIO_QDSP6SS_SAW2_RST_IN)
#define HWIO_QDSP6SS_SAW2_RST_RST_BMSK                                  0x1
#define HWIO_QDSP6SS_SAW2_RST_RST_SHFT                                  0x0

#define HWIO_QDSP6SS_SAW2_VCTL_ADDR                              (QDSP6SS_SAW2_REG_BASE      + 0x0000001c)
#define HWIO_QDSP6SS_SAW2_VCTL_RMSK                                0x1700ff
#define HWIO_QDSP6SS_SAW2_VCTL_IN          \
        in_dword_masked(HWIO_QDSP6SS_SAW2_VCTL_ADDR, HWIO_QDSP6SS_SAW2_VCTL_RMSK)
#define HWIO_QDSP6SS_SAW2_VCTL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_SAW2_VCTL_ADDR, m)
#define HWIO_QDSP6SS_SAW2_VCTL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_SAW2_VCTL_ADDR,v)
#define HWIO_QDSP6SS_SAW2_VCTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_SAW2_VCTL_ADDR,m,v,HWIO_QDSP6SS_SAW2_VCTL_IN)
#define HWIO_QDSP6SS_SAW2_VCTL_SIZE_BMSK                           0x100000
#define HWIO_QDSP6SS_SAW2_VCTL_SIZE_SHFT                               0x14
#define HWIO_QDSP6SS_SAW2_VCTL_ADR_IDX_BMSK                         0x70000
#define HWIO_QDSP6SS_SAW2_VCTL_ADR_IDX_SHFT                            0x10
#define HWIO_QDSP6SS_SAW2_VCTL_PMIC_DATA_BMSK                          0xff
#define HWIO_QDSP6SS_SAW2_VCTL_PMIC_DATA_SHFT                           0x0

#define HWIO_QDSP6SS_SAW2_AVS_CTL_ADDR                           (QDSP6SS_SAW2_REG_BASE      + 0x00000020)
#define HWIO_QDSP6SS_SAW2_AVS_CTL_RMSK                               0x130f
#define HWIO_QDSP6SS_SAW2_AVS_CTL_IN          \
        in_dword_masked(HWIO_QDSP6SS_SAW2_AVS_CTL_ADDR, HWIO_QDSP6SS_SAW2_AVS_CTL_RMSK)
#define HWIO_QDSP6SS_SAW2_AVS_CTL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_SAW2_AVS_CTL_ADDR, m)
#define HWIO_QDSP6SS_SAW2_AVS_CTL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_SAW2_AVS_CTL_ADDR,v)
#define HWIO_QDSP6SS_SAW2_AVS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_SAW2_AVS_CTL_ADDR,m,v,HWIO_QDSP6SS_SAW2_AVS_CTL_IN)
#define HWIO_QDSP6SS_SAW2_AVS_CTL_VLVL_WIDTH_BMSK                    0x1000
#define HWIO_QDSP6SS_SAW2_AVS_CTL_VLVL_WIDTH_SHFT                       0xc
#define HWIO_QDSP6SS_SAW2_AVS_CTL_VLVL_STEP_BMSK                      0x300
#define HWIO_QDSP6SS_SAW2_AVS_CTL_VLVL_STEP_SHFT                        0x8
#define HWIO_QDSP6SS_SAW2_AVS_CTL_SW_DONE_INT_EN_BMSK                   0x8
#define HWIO_QDSP6SS_SAW2_AVS_CTL_SW_DONE_INT_EN_SHFT                   0x3
#define HWIO_QDSP6SS_SAW2_AVS_CTL_MAX_INT_EN_BMSK                       0x4
#define HWIO_QDSP6SS_SAW2_AVS_CTL_MAX_INT_EN_SHFT                       0x2
#define HWIO_QDSP6SS_SAW2_AVS_CTL_MIN_INT_EN_BMSK                       0x2
#define HWIO_QDSP6SS_SAW2_AVS_CTL_MIN_INT_EN_SHFT                       0x1
#define HWIO_QDSP6SS_SAW2_AVS_CTL_EN_BMSK                               0x1
#define HWIO_QDSP6SS_SAW2_AVS_CTL_EN_SHFT                               0x0

#define HWIO_QDSP6SS_SAW2_AVS_LIMIT_ADDR                         (QDSP6SS_SAW2_REG_BASE      + 0x00000024)
#define HWIO_QDSP6SS_SAW2_AVS_LIMIT_RMSK                           0xff00ff
#define HWIO_QDSP6SS_SAW2_AVS_LIMIT_IN          \
        in_dword_masked(HWIO_QDSP6SS_SAW2_AVS_LIMIT_ADDR, HWIO_QDSP6SS_SAW2_AVS_LIMIT_RMSK)
#define HWIO_QDSP6SS_SAW2_AVS_LIMIT_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_SAW2_AVS_LIMIT_ADDR, m)
#define HWIO_QDSP6SS_SAW2_AVS_LIMIT_OUT(v)      \
        out_dword(HWIO_QDSP6SS_SAW2_AVS_LIMIT_ADDR,v)
#define HWIO_QDSP6SS_SAW2_AVS_LIMIT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_SAW2_AVS_LIMIT_ADDR,m,v,HWIO_QDSP6SS_SAW2_AVS_LIMIT_IN)
#define HWIO_QDSP6SS_SAW2_AVS_LIMIT_MAX_VLVL_BMSK                  0xff0000
#define HWIO_QDSP6SS_SAW2_AVS_LIMIT_MAX_VLVL_SHFT                      0x10
#define HWIO_QDSP6SS_SAW2_AVS_LIMIT_MIN_VLVL_BMSK                      0xff
#define HWIO_QDSP6SS_SAW2_AVS_LIMIT_MIN_VLVL_SHFT                       0x0

#define HWIO_QDSP6SS_SAW2_AVS_DLY_ADDR                           (QDSP6SS_SAW2_REG_BASE      + 0x00000028)
#define HWIO_QDSP6SS_SAW2_AVS_DLY_RMSK                                0x3ff
#define HWIO_QDSP6SS_SAW2_AVS_DLY_IN          \
        in_dword_masked(HWIO_QDSP6SS_SAW2_AVS_DLY_ADDR, HWIO_QDSP6SS_SAW2_AVS_DLY_RMSK)
#define HWIO_QDSP6SS_SAW2_AVS_DLY_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_SAW2_AVS_DLY_ADDR, m)
#define HWIO_QDSP6SS_SAW2_AVS_DLY_OUT(v)      \
        out_dword(HWIO_QDSP6SS_SAW2_AVS_DLY_ADDR,v)
#define HWIO_QDSP6SS_SAW2_AVS_DLY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_SAW2_AVS_DLY_ADDR,m,v,HWIO_QDSP6SS_SAW2_AVS_DLY_IN)
#define HWIO_QDSP6SS_SAW2_AVS_DLY_AVS_DELAY_BMSK                      0x3ff
#define HWIO_QDSP6SS_SAW2_AVS_DLY_AVS_DELAY_SHFT                        0x0

#define HWIO_QDSP6SS_SAW2_AVS_HYSTERESIS_ADDR                    (QDSP6SS_SAW2_REG_BASE      + 0x0000002c)
#define HWIO_QDSP6SS_SAW2_AVS_HYSTERESIS_RMSK                      0xff00ff
#define HWIO_QDSP6SS_SAW2_AVS_HYSTERESIS_IN          \
        in_dword_masked(HWIO_QDSP6SS_SAW2_AVS_HYSTERESIS_ADDR, HWIO_QDSP6SS_SAW2_AVS_HYSTERESIS_RMSK)
#define HWIO_QDSP6SS_SAW2_AVS_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_SAW2_AVS_HYSTERESIS_ADDR, m)
#define HWIO_QDSP6SS_SAW2_AVS_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_QDSP6SS_SAW2_AVS_HYSTERESIS_ADDR,v)
#define HWIO_QDSP6SS_SAW2_AVS_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_SAW2_AVS_HYSTERESIS_ADDR,m,v,HWIO_QDSP6SS_SAW2_AVS_HYSTERESIS_IN)
#define HWIO_QDSP6SS_SAW2_AVS_HYSTERESIS_DN_COUNT_BMSK             0xff0000
#define HWIO_QDSP6SS_SAW2_AVS_HYSTERESIS_DN_COUNT_SHFT                 0x10
#define HWIO_QDSP6SS_SAW2_AVS_HYSTERESIS_UP_COUNT_BMSK                 0xff
#define HWIO_QDSP6SS_SAW2_AVS_HYSTERESIS_UP_COUNT_SHFT                  0x0

#define HWIO_QDSP6SS_SAW2_SPM_CTL_ADDR                           (QDSP6SS_SAW2_REG_BASE      + 0x00000030)
#define HWIO_QDSP6SS_SAW2_SPM_CTL_RMSK                           0xff031fff
#define HWIO_QDSP6SS_SAW2_SPM_CTL_IN          \
        in_dword_masked(HWIO_QDSP6SS_SAW2_SPM_CTL_ADDR, HWIO_QDSP6SS_SAW2_SPM_CTL_RMSK)
#define HWIO_QDSP6SS_SAW2_SPM_CTL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_SAW2_SPM_CTL_ADDR, m)
#define HWIO_QDSP6SS_SAW2_SPM_CTL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_SAW2_SPM_CTL_ADDR,v)
#define HWIO_QDSP6SS_SAW2_SPM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_SAW2_SPM_CTL_ADDR,m,v,HWIO_QDSP6SS_SAW2_SPM_CTL_IN)
#define HWIO_QDSP6SS_SAW2_SPM_CTL_EVENT_SYNC_BMSK                0xff000000
#define HWIO_QDSP6SS_SAW2_SPM_CTL_EVENT_SYNC_SHFT                      0x18
#define HWIO_QDSP6SS_SAW2_SPM_CTL_SLP_CMD_MODE_BMSK                 0x20000
#define HWIO_QDSP6SS_SAW2_SPM_CTL_SLP_CMD_MODE_SHFT                    0x11
#define HWIO_QDSP6SS_SAW2_SPM_CTL_SPM_SYS_PC_MODE_BMSK              0x10000
#define HWIO_QDSP6SS_SAW2_SPM_CTL_SPM_SYS_PC_MODE_SHFT                 0x10
#define HWIO_QDSP6SS_SAW2_SPM_CTL_SPM_START_ADR_BMSK                 0x1ff0
#define HWIO_QDSP6SS_SAW2_SPM_CTL_SPM_START_ADR_SHFT                    0x4
#define HWIO_QDSP6SS_SAW2_SPM_CTL_ISAR_BMSK                             0x8
#define HWIO_QDSP6SS_SAW2_SPM_CTL_ISAR_SHFT                             0x3
#define HWIO_QDSP6SS_SAW2_SPM_CTL_WAKEUP_CONFIG_BMSK                    0x6
#define HWIO_QDSP6SS_SAW2_SPM_CTL_WAKEUP_CONFIG_SHFT                    0x1
#define HWIO_QDSP6SS_SAW2_SPM_CTL_SPM_EN_BMSK                           0x1
#define HWIO_QDSP6SS_SAW2_SPM_CTL_SPM_EN_SHFT                           0x0

#define HWIO_QDSP6SS_SAW2_SPM_DLY_ADDR                           (QDSP6SS_SAW2_REG_BASE      + 0x00000034)
#define HWIO_QDSP6SS_SAW2_SPM_DLY_RMSK                           0x3fffffff
#define HWIO_QDSP6SS_SAW2_SPM_DLY_IN          \
        in_dword_masked(HWIO_QDSP6SS_SAW2_SPM_DLY_ADDR, HWIO_QDSP6SS_SAW2_SPM_DLY_RMSK)
#define HWIO_QDSP6SS_SAW2_SPM_DLY_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_SAW2_SPM_DLY_ADDR, m)
#define HWIO_QDSP6SS_SAW2_SPM_DLY_OUT(v)      \
        out_dword(HWIO_QDSP6SS_SAW2_SPM_DLY_ADDR,v)
#define HWIO_QDSP6SS_SAW2_SPM_DLY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_SAW2_SPM_DLY_ADDR,m,v,HWIO_QDSP6SS_SAW2_SPM_DLY_IN)
#define HWIO_QDSP6SS_SAW2_SPM_DLY_DLY3_BMSK                      0x3ff00000
#define HWIO_QDSP6SS_SAW2_SPM_DLY_DLY3_SHFT                            0x14
#define HWIO_QDSP6SS_SAW2_SPM_DLY_DLY2_BMSK                         0xffc00
#define HWIO_QDSP6SS_SAW2_SPM_DLY_DLY2_SHFT                             0xa
#define HWIO_QDSP6SS_SAW2_SPM_DLY_DLY1_BMSK                           0x3ff
#define HWIO_QDSP6SS_SAW2_SPM_DLY_DLY1_SHFT                             0x0

#define HWIO_QDSP6SS_SAW2_SPM_STS2_ADDR                          (QDSP6SS_SAW2_REG_BASE      + 0x00000038)
#define HWIO_QDSP6SS_SAW2_SPM_STS2_RMSK                          0xffffffff
#define HWIO_QDSP6SS_SAW2_SPM_STS2_IN          \
        in_dword_masked(HWIO_QDSP6SS_SAW2_SPM_STS2_ADDR, HWIO_QDSP6SS_SAW2_SPM_STS2_RMSK)
#define HWIO_QDSP6SS_SAW2_SPM_STS2_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_SAW2_SPM_STS2_ADDR, m)
#define HWIO_QDSP6SS_SAW2_SPM_STS2_CURR_PWR_CTL_BMSK             0xffffffff
#define HWIO_QDSP6SS_SAW2_SPM_STS2_CURR_PWR_CTL_SHFT                    0x0

#define HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_ADDR(n)            (QDSP6SS_SAW2_REG_BASE      + 0x00000400 + 0x4 * (n))
#define HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_RMSK               0xffffffff
#define HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_MAXn                       27
#define HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_INI(n)        \
        in_dword_masked(HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_ADDR(n), HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_RMSK)
#define HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_INMI(n,mask)    \
        in_dword_masked(HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_ADDR(n), mask)
#define HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_OUTI(n,val)    \
        out_dword(HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_ADDR(n),val)
#define HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_ADDR(n),mask,val,HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_INI(n))
#define HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_CMD3_BMSK          0xff000000
#define HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_CMD3_SHFT                0x18
#define HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_CMD2_BMSK            0xff0000
#define HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_CMD2_SHFT                0x10
#define HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_CMD1_BMSK              0xff00
#define HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_CMD1_SHFT                 0x8
#define HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_CMD0_BMSK                0xff
#define HWIO_QDSP6SS_SAW2_SPM_SLP_SEQ_ENTRY_n_CMD0_SHFT                 0x0

#define HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_ADDR(n)                (QDSP6SS_SAW2_REG_BASE      + 0x00000040 + 0x4 * (n))
#define HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_RMSK                    0x71700ff
#define HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_MAXn                            2
#define HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_INI(n)        \
        in_dword_masked(HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_ADDR(n), HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_RMSK)
#define HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_INMI(n,mask)    \
        in_dword_masked(HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_ADDR(n), mask)
#define HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_OUTI(n,val)    \
        out_dword(HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_ADDR(n),val)
#define HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_ADDR(n),mask,val,HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_INI(n))
#define HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_DLY_BMSK                0x7000000
#define HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_DLY_SHFT                     0x18
#define HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_SIZE_BMSK                0x100000
#define HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_SIZE_SHFT                    0x14
#define HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_ADR_IDX_BMSK              0x70000
#define HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_ADR_IDX_SHFT                 0x10
#define HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_DATA_BMSK                    0xff
#define HWIO_QDSP6SS_SAW2_SPM_PMIC_DATA_n_DATA_SHFT                     0x0

#define HWIO_QDSP6SS_SAW2_VERSION_ADDR                           (QDSP6SS_SAW2_REG_BASE      + 0x00000fd0)
#define HWIO_QDSP6SS_SAW2_VERSION_RMSK                           0xffffffff
#define HWIO_QDSP6SS_SAW2_VERSION_IN          \
        in_dword_masked(HWIO_QDSP6SS_SAW2_VERSION_ADDR, HWIO_QDSP6SS_SAW2_VERSION_RMSK)
#define HWIO_QDSP6SS_SAW2_VERSION_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_SAW2_VERSION_ADDR, m)
#define HWIO_QDSP6SS_SAW2_VERSION_MAJOR_BMSK                     0xf0000000
#define HWIO_QDSP6SS_SAW2_VERSION_MAJOR_SHFT                           0x1c
#define HWIO_QDSP6SS_SAW2_VERSION_MINOR_BMSK                      0xfff0000
#define HWIO_QDSP6SS_SAW2_VERSION_MINOR_SHFT                           0x10
#define HWIO_QDSP6SS_SAW2_VERSION_STEP_BMSK                          0xffff
#define HWIO_QDSP6SS_SAW2_VERSION_STEP_SHFT                             0x0


#endif /* __HAL_SPM_HWIO_H__ */
