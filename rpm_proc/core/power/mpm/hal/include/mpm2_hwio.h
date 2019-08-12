#ifndef __MPM2_HWIO_H__
#define __MPM2_HWIO_H__
/*
===========================================================================
*/
/**
  @file mpm2_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    MPM2_MPM
    MPM2_G_CTRL_CNTR
    MPM2_G_RD_CNTR
    QDSS_QDSSCSR

  'Include' filters applied: LOCK[QDSS_QDSSCSR] PWRREQIGNORE[QDSS_QDSSCSR] 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/hal/include/mpm2_hwio.h#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: MPM2_MPM
 *--------------------------------------------------------------------------*/

#define MPM2_MPM_REG_BASE                                                     (MPM2_MPM_BASE      + 0x00000000)

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_ADDR                                  (MPM2_MPM_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_RMSK                                  0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_DELAY_BMSK                            0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_DELAY_SHFT                                  0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_SLAVE_ID_BMSK                          0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_SLAVE_ID_SHFT                               0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_SLAVE_ADDR_BMSK                         0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_SLAVE_ADDR_SHFT                              0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_SLAVE_DATA_BMSK                             0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_SLAVE_DATA_SHFT                              0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_ADDR                                  (MPM2_MPM_REG_BASE      + 0x00000004)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_RMSK                                  0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_DELAY_BMSK                            0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_DELAY_SHFT                                  0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_SLAVE_ID_BMSK                          0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_SLAVE_ID_SHFT                               0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_SLAVE_ADDR_BMSK                         0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_SLAVE_ADDR_SHFT                              0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_SLAVE_DATA_BMSK                             0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_SLAVE_DATA_SHFT                              0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_ADDR                                  (MPM2_MPM_REG_BASE      + 0x00000008)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_RMSK                                  0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_DELAY_BMSK                            0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_DELAY_SHFT                                  0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_SLAVE_ID_BMSK                          0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_SLAVE_ID_SHFT                               0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_SLAVE_ADDR_BMSK                         0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_SLAVE_ADDR_SHFT                              0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_SLAVE_DATA_BMSK                             0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_SLAVE_DATA_SHFT                              0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_ADDR                                  (MPM2_MPM_REG_BASE      + 0x0000000c)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_RMSK                                  0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_DELAY_BMSK                            0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_DELAY_SHFT                                  0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_SLAVE_ID_BMSK                          0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_SLAVE_ID_SHFT                               0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_SLAVE_ADDR_BMSK                         0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_SLAVE_ADDR_SHFT                              0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_SLAVE_DATA_BMSK                             0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_SLAVE_DATA_SHFT                              0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_ADDR                                  (MPM2_MPM_REG_BASE      + 0x00000010)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_RMSK                                  0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_DELAY_BMSK                            0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_DELAY_SHFT                                  0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_SLAVE_ID_BMSK                          0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_SLAVE_ID_SHFT                               0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_SLAVE_ADDR_BMSK                         0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_SLAVE_ADDR_SHFT                              0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_SLAVE_DATA_BMSK                             0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_SLAVE_DATA_SHFT                              0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_ADDR                                  (MPM2_MPM_REG_BASE      + 0x00000014)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_RMSK                                  0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_DELAY_BMSK                            0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_DELAY_SHFT                                  0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_SLAVE_ID_BMSK                          0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_SLAVE_ID_SHFT                               0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_SLAVE_ADDR_BMSK                         0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_SLAVE_ADDR_SHFT                              0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_SLAVE_DATA_BMSK                             0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_SLAVE_DATA_SHFT                              0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_ADDR                                  (MPM2_MPM_REG_BASE      + 0x00000018)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_RMSK                                  0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_DELAY_BMSK                            0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_DELAY_SHFT                                  0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_SLAVE_ID_BMSK                          0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_SLAVE_ID_SHFT                               0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_SLAVE_ADDR_BMSK                         0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_SLAVE_ADDR_SHFT                              0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_SLAVE_DATA_BMSK                             0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_SLAVE_DATA_SHFT                              0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_ADDR                                  (MPM2_MPM_REG_BASE      + 0x0000001c)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_RMSK                                  0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_DELAY_BMSK                            0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_DELAY_SHFT                                  0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_SLAVE_ID_BMSK                          0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_SLAVE_ID_SHFT                               0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_SLAVE_ADDR_BMSK                         0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_SLAVE_ADDR_SHFT                              0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_SLAVE_DATA_BMSK                             0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_SLAVE_DATA_SHFT                              0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_ADDR                                  (MPM2_MPM_REG_BASE      + 0x00000020)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_RMSK                                  0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_DELAY_BMSK                            0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_DELAY_SHFT                                  0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_SLAVE_ID_BMSK                          0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_SLAVE_ID_SHFT                               0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_SLAVE_ADDR_BMSK                         0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_SLAVE_ADDR_SHFT                              0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_SLAVE_DATA_BMSK                             0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_SLAVE_DATA_SHFT                              0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_ADDR                                 (MPM2_MPM_REG_BASE      + 0x00000024)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_ADDR                                 (MPM2_MPM_REG_BASE      + 0x00000028)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_ADDR                                 (MPM2_MPM_REG_BASE      + 0x0000002c)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_ADDR                                 (MPM2_MPM_REG_BASE      + 0x00000030)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_ADDR                                 (MPM2_MPM_REG_BASE      + 0x00000034)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_ADDR                                 (MPM2_MPM_REG_BASE      + 0x00000038)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_ADDR                                 (MPM2_MPM_REG_BASE      + 0x0000003c)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_ADDR                                 (MPM2_MPM_REG_BASE      + 0x00000040)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_ADDR                                 (MPM2_MPM_REG_BASE      + 0x00000044)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_ADDR                                 (MPM2_MPM_REG_BASE      + 0x00000048)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_ADDR                                 (MPM2_MPM_REG_BASE      + 0x0000004c)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_ADDR                                 (MPM2_MPM_REG_BASE      + 0x00000050)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_ADDR                                (MPM2_MPM_REG_BASE      + 0x00000054)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_RMSK                                0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_DELAY_BMSK                          0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_DELAY_SHFT                                0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_SLAVE_ID_BMSK                        0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_SLAVE_ID_SHFT                             0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_SLAVE_ADDR_BMSK                       0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_SLAVE_ADDR_SHFT                            0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_SLAVE_DATA_BMSK                           0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_SLAVE_DATA_SHFT                            0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_ADDR                                (MPM2_MPM_REG_BASE      + 0x00000058)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_RMSK                                0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_DELAY_BMSK                          0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_DELAY_SHFT                                0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_SLAVE_ID_BMSK                        0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_SLAVE_ID_SHFT                             0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_SLAVE_ADDR_BMSK                       0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_SLAVE_ADDR_SHFT                            0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_SLAVE_DATA_BMSK                           0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_SLAVE_DATA_SHFT                            0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_ADDR                                (MPM2_MPM_REG_BASE      + 0x0000005c)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_RMSK                                0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_DELAY_BMSK                          0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_DELAY_SHFT                                0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_SLAVE_ID_BMSK                        0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_SLAVE_ID_SHFT                             0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_SLAVE_ADDR_BMSK                       0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_SLAVE_ADDR_SHFT                            0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_SLAVE_DATA_BMSK                           0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_SLAVE_DATA_SHFT                            0x0

#define HWIO_MPM2_MPM_SPMI_CMD_CFG_ADDR                                       (MPM2_MPM_REG_BASE      + 0x00000060)
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_RMSK                                             0xff
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_IN          \
        in_dword_masked(HWIO_MPM2_MPM_SPMI_CMD_CFG_ADDR, HWIO_MPM2_MPM_SPMI_CMD_CFG_RMSK)
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_SPMI_CMD_CFG_ADDR, m)
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_SPMI_CMD_CFG_ADDR,v)
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_SPMI_CMD_CFG_ADDR,m,v,HWIO_MPM2_MPM_SPMI_CMD_CFG_IN)
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_NUM_CMD_OFF_BMSK                                 0xf0
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_NUM_CMD_OFF_SHFT                                  0x4
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_NUM_CMD_ON_BMSK                                   0xf
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_NUM_CMD_ON_SHFT                                   0x0

#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_ADDR                                    (MPM2_MPM_REG_BASE      + 0x00000064)
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_RMSK                                         0x7ff
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_IN          \
        in_dword_masked(HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_ADDR, HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_RMSK)
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_ADDR, m)
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_ADDR,v)
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_ADDR,m,v,HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_IN)
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_XO_DIV_BMSK                                  0x600
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_XO_DIV_SHFT                                    0x9
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_RO_DIV_BMSK                                  0x1ff
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_RO_DIV_SHFT                                    0x0

#define HWIO_MPM2_MPM_CXO_CTRL_ADDR                                           (MPM2_MPM_REG_BASE      + 0x00000068)
#define HWIO_MPM2_MPM_CXO_CTRL_RMSK                                                  0x3
#define HWIO_MPM2_MPM_CXO_CTRL_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CXO_CTRL_ADDR, HWIO_MPM2_MPM_CXO_CTRL_RMSK)
#define HWIO_MPM2_MPM_CXO_CTRL_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CXO_CTRL_ADDR, m)
#define HWIO_MPM2_MPM_CXO_CTRL_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CXO_CTRL_ADDR,v)
#define HWIO_MPM2_MPM_CXO_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CXO_CTRL_ADDR,m,v,HWIO_MPM2_MPM_CXO_CTRL_IN)
#define HWIO_MPM2_MPM_CXO_CTRL_CXOPAD_DIS_BMSK                                       0x2
#define HWIO_MPM2_MPM_CXO_CTRL_CXOPAD_DIS_SHFT                                       0x1
#define HWIO_MPM2_MPM_CXO_CTRL_CXO_DIS_BMSK                                          0x1
#define HWIO_MPM2_MPM_CXO_CTRL_CXO_DIS_SHFT                                          0x0

#define HWIO_MPM2_MPM_PXO_OSC_CTRL_ADDR                                       (MPM2_MPM_REG_BASE      + 0x0000006c)
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_RMSK                                             0x7f
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PXO_OSC_CTRL_ADDR, HWIO_MPM2_MPM_PXO_OSC_CTRL_RMSK)
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PXO_OSC_CTRL_ADDR, m)
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PXO_OSC_CTRL_ADDR,v)
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PXO_OSC_CTRL_ADDR,m,v,HWIO_MPM2_MPM_PXO_OSC_CTRL_IN)
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXO_OSC_GAIN_BMSK                                0x70
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXO_OSC_GAIN_SHFT                                 0x4
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXOPAD_CORE_DIS_BMSK                              0x8
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXOPAD_CORE_DIS_SHFT                              0x3
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXO_OSC_SLEEP_BMSK                                0x4
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXO_OSC_SLEEP_SHFT                                0x2
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXOPAD_HV_DIS_BMSK                                0x2
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXOPAD_HV_DIS_SHFT                                0x1
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXO_OSC_RF_BYPASS_BMSK                            0x1
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXO_OSC_RF_BYPASS_SHFT                            0x0

#define HWIO_MPM2_MPM_LOW_POWER_CFG_ADDR                                      (MPM2_MPM_REG_BASE      + 0x00000070)
#define HWIO_MPM2_MPM_LOW_POWER_CFG_RMSK                                         0xf5fff
#define HWIO_MPM2_MPM_LOW_POWER_CFG_IN          \
        in_dword_masked(HWIO_MPM2_MPM_LOW_POWER_CFG_ADDR, HWIO_MPM2_MPM_LOW_POWER_CFG_RMSK)
#define HWIO_MPM2_MPM_LOW_POWER_CFG_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_LOW_POWER_CFG_ADDR, m)
#define HWIO_MPM2_MPM_LOW_POWER_CFG_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_LOW_POWER_CFG_ADDR,v)
#define HWIO_MPM2_MPM_LOW_POWER_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_LOW_POWER_CFG_ADDR,m,v,HWIO_MPM2_MPM_LOW_POWER_CFG_IN)
#define HWIO_MPM2_MPM_LOW_POWER_CFG_SMEM_EN_BMSK                                 0x80000
#define HWIO_MPM2_MPM_LOW_POWER_CFG_SMEM_EN_SHFT                                    0x13
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_FREEZE_IO_M_BMSK                     0x40000
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_FREEZE_IO_M_SHFT                        0x12
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_CLAMP_MEM_BMSK                       0x20000
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_CLAMP_MEM_SHFT                          0x11
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_HW_RESTORED_BMSK                     0x10000
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_HW_RESTORED_SHFT                        0x10
#define HWIO_MPM2_MPM_LOW_POWER_CFG_ENABLE_CXOPAD_GATING_BMSK                     0x4000
#define HWIO_MPM2_MPM_LOW_POWER_CFG_ENABLE_CXOPAD_GATING_SHFT                        0xe
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_TRST_N_BMSK                           0x1000
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_TRST_N_SHFT                              0xc
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_CORE_RESET_BMSK                        0x800
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_CORE_RESET_SHFT                          0xb
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_CLAMPS_BMSK                            0x400
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_CLAMPS_SHFT                              0xa
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_FREEZE_BMSK                            0x200
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_FREEZE_SHFT                              0x9
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DEBUG_ON_BMSK                                  0x100
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DEBUG_ON_SHFT                                    0x8
#define HWIO_MPM2_MPM_LOW_POWER_CFG_SW_EBI1_CTL_ENABLE_BMSK                         0x80
#define HWIO_MPM2_MPM_LOW_POWER_CFG_SW_EBI1_CTL_ENABLE_SHFT                          0x7
#define HWIO_MPM2_MPM_LOW_POWER_CFG_SW_EBI1_CTL_VALUE_BMSK                          0x40
#define HWIO_MPM2_MPM_LOW_POWER_CFG_SW_EBI1_CTL_VALUE_SHFT                           0x6
#define HWIO_MPM2_MPM_LOW_POWER_CFG_VDDMX_MIN_EN_BMSK                               0x20
#define HWIO_MPM2_MPM_LOW_POWER_CFG_VDDMX_MIN_EN_SHFT                                0x5
#define HWIO_MPM2_MPM_LOW_POWER_CFG_VDDMX_PC_EN_BMSK                                0x10
#define HWIO_MPM2_MPM_LOW_POWER_CFG_VDDMX_PC_EN_SHFT                                 0x4
#define HWIO_MPM2_MPM_LOW_POWER_CFG_VDDCX_MIN_EN_BMSK                                0x8
#define HWIO_MPM2_MPM_LOW_POWER_CFG_VDDCX_MIN_EN_SHFT                                0x3
#define HWIO_MPM2_MPM_LOW_POWER_CFG_VDDCX_PC_EN_BMSK                                 0x4
#define HWIO_MPM2_MPM_LOW_POWER_CFG_VDDCX_PC_EN_SHFT                                 0x2
#define HWIO_MPM2_MPM_LOW_POWER_CFG_PXO_SD_EN_BMSK                                   0x2
#define HWIO_MPM2_MPM_LOW_POWER_CFG_PXO_SD_EN_SHFT                                   0x1
#define HWIO_MPM2_MPM_LOW_POWER_CFG_CXO_SD_EN_BMSK                                   0x1
#define HWIO_MPM2_MPM_LOW_POWER_CFG_CXO_SD_EN_SHFT                                   0x0

#define HWIO_MPM2_MPM_LOW_POWER_STATUS_ADDR                                   (MPM2_MPM_REG_BASE      + 0x00000074)
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_RMSK                                       0x3fff
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_IN          \
        in_dword_masked(HWIO_MPM2_MPM_LOW_POWER_STATUS_ADDR, HWIO_MPM2_MPM_LOW_POWER_STATUS_RMSK)
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_LOW_POWER_STATUS_ADDR, m)
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_SPMI_SEQ_FSM_BMSK                          0x3000
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_SPMI_SEQ_FSM_SHFT                             0xc
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_SMEM_FSM_STATE_BMSK                         0xe00
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_SMEM_FSM_STATE_SHFT                           0x9
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_VDDMX_DOWN_BMSK                             0x100
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_VDDMX_DOWN_SHFT                               0x8
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_VDDCX_DOWN_BMSK                              0x80
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_VDDCX_DOWN_SHFT                               0x7
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_PXO_DOWN_BMSK                                0x40
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_PXO_DOWN_SHFT                                 0x6
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_CXO_DOWN_BMSK                                0x20
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_CXO_DOWN_SHFT                                 0x5
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_POWER_DOWN_BMSK                              0x10
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_POWER_DOWN_SHFT                               0x4
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_FSM_STATE_BMSK                                0xf
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_FSM_STATE_SHFT                                0x0

#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_ADDR                              (MPM2_MPM_REG_BASE      + 0x00000078)
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_RMSK                              0xffffffff
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_ADDR, HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_RMSK)
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_ADDR, m)
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_ADDR,v)
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_ADDR,m,v,HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_IN)
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_CXO_WARMUP_TIME_BMSK              0xffff0000
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_CXO_WARMUP_TIME_SHFT                    0x10
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_POWER_RAMPUP_TIME_BMSK                0xffff
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_POWER_RAMPUP_TIME_SHFT                   0x0

#define HWIO_MPM2_MPM_PXO_WARMUP_TIME_ADDR                                    (MPM2_MPM_REG_BASE      + 0x0000007c)
#define HWIO_MPM2_MPM_PXO_WARMUP_TIME_RMSK                                        0xffff
#define HWIO_MPM2_MPM_PXO_WARMUP_TIME_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PXO_WARMUP_TIME_ADDR, HWIO_MPM2_MPM_PXO_WARMUP_TIME_RMSK)
#define HWIO_MPM2_MPM_PXO_WARMUP_TIME_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PXO_WARMUP_TIME_ADDR, m)
#define HWIO_MPM2_MPM_PXO_WARMUP_TIME_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PXO_WARMUP_TIME_ADDR,v)
#define HWIO_MPM2_MPM_PXO_WARMUP_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PXO_WARMUP_TIME_ADDR,m,v,HWIO_MPM2_MPM_PXO_WARMUP_TIME_IN)
#define HWIO_MPM2_MPM_PXO_WARMUP_TIME_PXO_WARMUP_TIME_BMSK                        0xffff
#define HWIO_MPM2_MPM_PXO_WARMUP_TIME_PXO_WARMUP_TIME_SHFT                           0x0

#define HWIO_MPM2_MPM_HARDWARE_RESTORED_ADDR                                  (MPM2_MPM_REG_BASE      + 0x00000080)
#define HWIO_MPM2_MPM_HARDWARE_RESTORED_RMSK                                         0x1
#define HWIO_MPM2_MPM_HARDWARE_RESTORED_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_HARDWARE_RESTORED_ADDR,v)
#define HWIO_MPM2_MPM_HARDWARE_RESTORED_DATA_BMSK                                    0x1
#define HWIO_MPM2_MPM_HARDWARE_RESTORED_DATA_SHFT                                    0x0

#define HWIO_MPM2_MPM_INT_CLEAR_1_ADDR                                        (MPM2_MPM_REG_BASE      + 0x00000084)
#define HWIO_MPM2_MPM_INT_CLEAR_1_RMSK                                        0xffffffff
#define HWIO_MPM2_MPM_INT_CLEAR_1_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_INT_CLEAR_1_ADDR,v)
#define HWIO_MPM2_MPM_INT_CLEAR_1_INT_CLEAR_1_BMSK                            0xffffffff
#define HWIO_MPM2_MPM_INT_CLEAR_1_INT_CLEAR_1_SHFT                                   0x0

#define HWIO_MPM2_MPM_INT_CLEAR_2_ADDR                                        (MPM2_MPM_REG_BASE      + 0x00000088)
#define HWIO_MPM2_MPM_INT_CLEAR_2_RMSK                                        0xffffffff
#define HWIO_MPM2_MPM_INT_CLEAR_2_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_INT_CLEAR_2_ADDR,v)
#define HWIO_MPM2_MPM_INT_CLEAR_2_INT_CLEAR_2_BMSK                            0xffffffff
#define HWIO_MPM2_MPM_INT_CLEAR_2_INT_CLEAR_2_SHFT                                   0x0

#define HWIO_MPM2_MPM_INT_EN_1_ADDR                                           (MPM2_MPM_REG_BASE      + 0x0000008c)
#define HWIO_MPM2_MPM_INT_EN_1_RMSK                                           0xffffffff
#define HWIO_MPM2_MPM_INT_EN_1_IN          \
        in_dword_masked(HWIO_MPM2_MPM_INT_EN_1_ADDR, HWIO_MPM2_MPM_INT_EN_1_RMSK)
#define HWIO_MPM2_MPM_INT_EN_1_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_INT_EN_1_ADDR, m)
#define HWIO_MPM2_MPM_INT_EN_1_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_INT_EN_1_ADDR,v)
#define HWIO_MPM2_MPM_INT_EN_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_INT_EN_1_ADDR,m,v,HWIO_MPM2_MPM_INT_EN_1_IN)
#define HWIO_MPM2_MPM_INT_EN_1_INT_EN_1_BMSK                                  0xffffffff
#define HWIO_MPM2_MPM_INT_EN_1_INT_EN_1_SHFT                                         0x0

#define HWIO_MPM2_MPM_INT_EN_2_ADDR                                           (MPM2_MPM_REG_BASE      + 0x00000090)
#define HWIO_MPM2_MPM_INT_EN_2_RMSK                                           0xffffffff
#define HWIO_MPM2_MPM_INT_EN_2_IN          \
        in_dword_masked(HWIO_MPM2_MPM_INT_EN_2_ADDR, HWIO_MPM2_MPM_INT_EN_2_RMSK)
#define HWIO_MPM2_MPM_INT_EN_2_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_INT_EN_2_ADDR, m)
#define HWIO_MPM2_MPM_INT_EN_2_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_INT_EN_2_ADDR,v)
#define HWIO_MPM2_MPM_INT_EN_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_INT_EN_2_ADDR,m,v,HWIO_MPM2_MPM_INT_EN_2_IN)
#define HWIO_MPM2_MPM_INT_EN_2_INT_EN_2_BMSK                                  0xffffffff
#define HWIO_MPM2_MPM_INT_EN_2_INT_EN_2_SHFT                                         0x0

#define HWIO_MPM2_MPM_INT_POLARITY_1_ADDR                                     (MPM2_MPM_REG_BASE      + 0x00000094)
#define HWIO_MPM2_MPM_INT_POLARITY_1_RMSK                                     0xffffffff
#define HWIO_MPM2_MPM_INT_POLARITY_1_IN          \
        in_dword_masked(HWIO_MPM2_MPM_INT_POLARITY_1_ADDR, HWIO_MPM2_MPM_INT_POLARITY_1_RMSK)
#define HWIO_MPM2_MPM_INT_POLARITY_1_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_INT_POLARITY_1_ADDR, m)
#define HWIO_MPM2_MPM_INT_POLARITY_1_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_INT_POLARITY_1_ADDR,v)
#define HWIO_MPM2_MPM_INT_POLARITY_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_INT_POLARITY_1_ADDR,m,v,HWIO_MPM2_MPM_INT_POLARITY_1_IN)
#define HWIO_MPM2_MPM_INT_POLARITY_1_INT_POLARITY_1_BMSK                      0xffffffff
#define HWIO_MPM2_MPM_INT_POLARITY_1_INT_POLARITY_1_SHFT                             0x0

#define HWIO_MPM2_MPM_INT_POLARITY_2_ADDR                                     (MPM2_MPM_REG_BASE      + 0x00000098)
#define HWIO_MPM2_MPM_INT_POLARITY_2_RMSK                                     0xffffffff
#define HWIO_MPM2_MPM_INT_POLARITY_2_IN          \
        in_dword_masked(HWIO_MPM2_MPM_INT_POLARITY_2_ADDR, HWIO_MPM2_MPM_INT_POLARITY_2_RMSK)
#define HWIO_MPM2_MPM_INT_POLARITY_2_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_INT_POLARITY_2_ADDR, m)
#define HWIO_MPM2_MPM_INT_POLARITY_2_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_INT_POLARITY_2_ADDR,v)
#define HWIO_MPM2_MPM_INT_POLARITY_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_INT_POLARITY_2_ADDR,m,v,HWIO_MPM2_MPM_INT_POLARITY_2_IN)
#define HWIO_MPM2_MPM_INT_POLARITY_2_INT_POLARITY_2_BMSK                      0xffffffff
#define HWIO_MPM2_MPM_INT_POLARITY_2_INT_POLARITY_2_SHFT                             0x0

#define HWIO_MPM2_MPM_DETECT_CTL_1_ADDR                                       (MPM2_MPM_REG_BASE      + 0x0000009c)
#define HWIO_MPM2_MPM_DETECT_CTL_1_RMSK                                       0xffffffff
#define HWIO_MPM2_MPM_DETECT_CTL_1_IN          \
        in_dword_masked(HWIO_MPM2_MPM_DETECT_CTL_1_ADDR, HWIO_MPM2_MPM_DETECT_CTL_1_RMSK)
#define HWIO_MPM2_MPM_DETECT_CTL_1_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_DETECT_CTL_1_ADDR, m)
#define HWIO_MPM2_MPM_DETECT_CTL_1_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_DETECT_CTL_1_ADDR,v)
#define HWIO_MPM2_MPM_DETECT_CTL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_DETECT_CTL_1_ADDR,m,v,HWIO_MPM2_MPM_DETECT_CTL_1_IN)
#define HWIO_MPM2_MPM_DETECT_CTL_1_DETECT_CTL_1_BMSK                          0xffffffff
#define HWIO_MPM2_MPM_DETECT_CTL_1_DETECT_CTL_1_SHFT                                 0x0

#define HWIO_MPM2_MPM_DETECT_CTL_2_ADDR                                       (MPM2_MPM_REG_BASE      + 0x000000a0)
#define HWIO_MPM2_MPM_DETECT_CTL_2_RMSK                                       0xffffffff
#define HWIO_MPM2_MPM_DETECT_CTL_2_IN          \
        in_dword_masked(HWIO_MPM2_MPM_DETECT_CTL_2_ADDR, HWIO_MPM2_MPM_DETECT_CTL_2_RMSK)
#define HWIO_MPM2_MPM_DETECT_CTL_2_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_DETECT_CTL_2_ADDR, m)
#define HWIO_MPM2_MPM_DETECT_CTL_2_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_DETECT_CTL_2_ADDR,v)
#define HWIO_MPM2_MPM_DETECT_CTL_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_DETECT_CTL_2_ADDR,m,v,HWIO_MPM2_MPM_DETECT_CTL_2_IN)
#define HWIO_MPM2_MPM_DETECT_CTL_2_DETECT_CTL_2_BMSK                          0xffffffff
#define HWIO_MPM2_MPM_DETECT_CTL_2_DETECT_CTL_2_SHFT                                 0x0

#define HWIO_MPM2_MPM_DETECT_CTL_3_ADDR                                       (MPM2_MPM_REG_BASE      + 0x000000a4)
#define HWIO_MPM2_MPM_DETECT_CTL_3_RMSK                                       0xffffffff
#define HWIO_MPM2_MPM_DETECT_CTL_3_IN          \
        in_dword_masked(HWIO_MPM2_MPM_DETECT_CTL_3_ADDR, HWIO_MPM2_MPM_DETECT_CTL_3_RMSK)
#define HWIO_MPM2_MPM_DETECT_CTL_3_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_DETECT_CTL_3_ADDR, m)
#define HWIO_MPM2_MPM_DETECT_CTL_3_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_DETECT_CTL_3_ADDR,v)
#define HWIO_MPM2_MPM_DETECT_CTL_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_DETECT_CTL_3_ADDR,m,v,HWIO_MPM2_MPM_DETECT_CTL_3_IN)
#define HWIO_MPM2_MPM_DETECT_CTL_3_DETECT_CTL_3_BMSK                          0xffffffff
#define HWIO_MPM2_MPM_DETECT_CTL_3_DETECT_CTL_3_SHFT                                 0x0

#define HWIO_MPM2_MPM_DETECT_CTL_4_ADDR                                       (MPM2_MPM_REG_BASE      + 0x000000a8)
#define HWIO_MPM2_MPM_DETECT_CTL_4_RMSK                                       0xffffffff
#define HWIO_MPM2_MPM_DETECT_CTL_4_IN          \
        in_dword_masked(HWIO_MPM2_MPM_DETECT_CTL_4_ADDR, HWIO_MPM2_MPM_DETECT_CTL_4_RMSK)
#define HWIO_MPM2_MPM_DETECT_CTL_4_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_DETECT_CTL_4_ADDR, m)
#define HWIO_MPM2_MPM_DETECT_CTL_4_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_DETECT_CTL_4_ADDR,v)
#define HWIO_MPM2_MPM_DETECT_CTL_4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_DETECT_CTL_4_ADDR,m,v,HWIO_MPM2_MPM_DETECT_CTL_4_IN)
#define HWIO_MPM2_MPM_DETECT_CTL_4_DETECT_CTL_4_BMSK                          0xffffffff
#define HWIO_MPM2_MPM_DETECT_CTL_4_DETECT_CTL_4_SHFT                                 0x0

#define HWIO_MPM2_MPM_INT_STATUS_1_ADDR                                       (MPM2_MPM_REG_BASE      + 0x000000ac)
#define HWIO_MPM2_MPM_INT_STATUS_1_RMSK                                       0xffffffff
#define HWIO_MPM2_MPM_INT_STATUS_1_IN          \
        in_dword_masked(HWIO_MPM2_MPM_INT_STATUS_1_ADDR, HWIO_MPM2_MPM_INT_STATUS_1_RMSK)
#define HWIO_MPM2_MPM_INT_STATUS_1_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_INT_STATUS_1_ADDR, m)
#define HWIO_MPM2_MPM_INT_STATUS_1_INT_STATUS_1_BMSK                          0xffffffff
#define HWIO_MPM2_MPM_INT_STATUS_1_INT_STATUS_1_SHFT                                 0x0

#define HWIO_MPM2_MPM_INT_STATUS_2_ADDR                                       (MPM2_MPM_REG_BASE      + 0x000000b0)
#define HWIO_MPM2_MPM_INT_STATUS_2_RMSK                                       0xffffffff
#define HWIO_MPM2_MPM_INT_STATUS_2_IN          \
        in_dword_masked(HWIO_MPM2_MPM_INT_STATUS_2_ADDR, HWIO_MPM2_MPM_INT_STATUS_2_RMSK)
#define HWIO_MPM2_MPM_INT_STATUS_2_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_INT_STATUS_2_ADDR, m)
#define HWIO_MPM2_MPM_INT_STATUS_2_INT_STATUS_2_BMSK                          0xffffffff
#define HWIO_MPM2_MPM_INT_STATUS_2_INT_STATUS_2_SHFT                                 0x0

#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_ADDR                                 (MPM2_MPM_REG_BASE      + 0x000000b4)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_IN          \
        in_dword_masked(HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_ADDR, HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_RMSK)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_ADDR, m)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_ADDR,v)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_ADDR,m,v,HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_IN)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_MPM_INT_WKUP_CLK_SEL_1_BMSK          0xffffffff
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_MPM_INT_WKUP_CLK_SEL_1_SHFT                 0x0

#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_ADDR                                 (MPM2_MPM_REG_BASE      + 0x000000b8)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_IN          \
        in_dword_masked(HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_ADDR, HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_RMSK)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_ADDR, m)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_ADDR,v)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_ADDR,m,v,HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_IN)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_MPM_INT_WKUP_CLK_SEL_2_BMSK          0xffffffff
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_MPM_INT_WKUP_CLK_SEL_2_SHFT                 0x0

#define HWIO_MPM2_MPM_DEBUG_BUS_EN_ADDR                                       (MPM2_MPM_REG_BASE      + 0x000000bc)
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_RMSK                                             0x1f
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_IN          \
        in_dword_masked(HWIO_MPM2_MPM_DEBUG_BUS_EN_ADDR, HWIO_MPM2_MPM_DEBUG_BUS_EN_RMSK)
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_DEBUG_BUS_EN_ADDR, m)
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_DEBUG_BUS_EN_ADDR,v)
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_DEBUG_BUS_EN_ADDR,m,v,HWIO_MPM2_MPM_DEBUG_BUS_EN_IN)
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_QTMR_TEST_BUS_EN_BMSK                            0x10
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_QTMR_TEST_BUS_EN_SHFT                             0x4
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_TEST_BUS_S_EN_BMSK                                0x8
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_TEST_BUS_S_EN_SHFT                                0x3
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_TEST_BUS_M_EN_BMSK                                0x4
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_TEST_BUS_M_EN_SHFT                                0x2
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_DEBUG_EN_1_BMSK                                   0x2
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_DEBUG_EN_1_SHFT                                   0x1
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_DEBUG_EN_BMSK                                     0x1
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_DEBUG_EN_SHFT                                     0x0

#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_ADDR                                   (MPM2_MPM_REG_BASE      + 0x000000c0)
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_RMSK                                          0xf
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_ADDR, HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_RMSK)
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_ADDR, m)
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_ADDR,v)
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_ADDR,m,v,HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_IN)
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_ENABLE_BMSK                                   0x8
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_ENABLE_SHFT                                   0x3
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_TIME_BMSK                                     0x7
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_TIME_SHFT                                     0x0

#define HWIO_MPM2_MPM_WARM_BOOT_CFG_ADDR                                      (MPM2_MPM_REG_BASE      + 0x000000c4)
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_RMSK                                             0x3
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_IN          \
        in_dword_masked(HWIO_MPM2_MPM_WARM_BOOT_CFG_ADDR, HWIO_MPM2_MPM_WARM_BOOT_CFG_RMSK)
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_WARM_BOOT_CFG_ADDR, m)
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_WARM_BOOT_CFG_ADDR,v)
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_WARM_BOOT_CFG_ADDR,m,v,HWIO_MPM2_MPM_WARM_BOOT_CFG_IN)
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_ENABLE_EBI2_BMSK                                 0x2
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_ENABLE_EBI2_SHFT                                 0x1
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_ENABLE_EBI1_BMSK                                 0x1
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_ENABLE_EBI1_SHFT                                 0x0

#define HWIO_MPM2_MPM_REGn_FILE_ADDR(n)                                       (MPM2_MPM_REG_BASE      + 0x000000c8 + 0x4 * (n))
#define HWIO_MPM2_MPM_REGn_FILE_RMSK                                          0xffffffff
#define HWIO_MPM2_MPM_REGn_FILE_MAXn                                                   8
#define HWIO_MPM2_MPM_REGn_FILE_INI(n)        \
        in_dword_masked(HWIO_MPM2_MPM_REGn_FILE_ADDR(n), HWIO_MPM2_MPM_REGn_FILE_RMSK)
#define HWIO_MPM2_MPM_REGn_FILE_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_MPM_REGn_FILE_ADDR(n), mask)
#define HWIO_MPM2_MPM_REGn_FILE_OUTI(n,val)    \
        out_dword(HWIO_MPM2_MPM_REGn_FILE_ADDR(n),val)
#define HWIO_MPM2_MPM_REGn_FILE_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_MPM_REGn_FILE_ADDR(n),mask,val,HWIO_MPM2_MPM_REGn_FILE_INI(n))
#define HWIO_MPM2_MPM_REGn_FILE_DATA_BMSK                                     0xffffffff
#define HWIO_MPM2_MPM_REGn_FILE_DATA_SHFT                                            0x0

#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_ADDR                                   (MPM2_MPM_REG_BASE      + 0x000000ec)
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_RMSK                                          0x7
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CNTR_CLK_CONTROL_ADDR, HWIO_MPM2_MPM_CNTR_CLK_CONTROL_RMSK)
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CNTR_CLK_CONTROL_ADDR, m)
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CNTR_CLK_CONTROL_ADDR,v)
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CNTR_CLK_CONTROL_ADDR,m,v,HWIO_MPM2_MPM_CNTR_CLK_CONTROL_IN)
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_CLK_STATUS_BMSK                               0x4
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_CLK_STATUS_SHFT                               0x2
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_SW_OVERWRITE_SWITCH_BMSK                      0x2
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_SW_OVERWRITE_SWITCH_SHFT                      0x1
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_SW_CLK_SWITCH_BMSK                            0x1
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_SW_CLK_SWITCH_SHFT                            0x0

#define HWIO_MPM2_MPM_CNTR_FRAC_ADDR                                          (MPM2_MPM_REG_BASE      + 0x000000f0)
#define HWIO_MPM2_MPM_CNTR_FRAC_RMSK                                              0xffff
#define HWIO_MPM2_MPM_CNTR_FRAC_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CNTR_FRAC_ADDR, HWIO_MPM2_MPM_CNTR_FRAC_RMSK)
#define HWIO_MPM2_MPM_CNTR_FRAC_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CNTR_FRAC_ADDR, m)
#define HWIO_MPM2_MPM_CNTR_FRAC_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CNTR_FRAC_ADDR,v)
#define HWIO_MPM2_MPM_CNTR_FRAC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CNTR_FRAC_ADDR,m,v,HWIO_MPM2_MPM_CNTR_FRAC_IN)
#define HWIO_MPM2_MPM_CNTR_FRAC_LOAD_VAL_BMSK                                     0xffff
#define HWIO_MPM2_MPM_CNTR_FRAC_LOAD_VAL_SHFT                                        0x0

#define HWIO_MPM2_MPM_CNTR_INCR_DATA_ADDR                                     (MPM2_MPM_REG_BASE      + 0x000000f4)
#define HWIO_MPM2_MPM_CNTR_INCR_DATA_RMSK                                     0xffffffff
#define HWIO_MPM2_MPM_CNTR_INCR_DATA_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CNTR_INCR_DATA_ADDR, HWIO_MPM2_MPM_CNTR_INCR_DATA_RMSK)
#define HWIO_MPM2_MPM_CNTR_INCR_DATA_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CNTR_INCR_DATA_ADDR, m)
#define HWIO_MPM2_MPM_CNTR_INCR_DATA_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CNTR_INCR_DATA_ADDR,v)
#define HWIO_MPM2_MPM_CNTR_INCR_DATA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CNTR_INCR_DATA_ADDR,m,v,HWIO_MPM2_MPM_CNTR_INCR_DATA_IN)
#define HWIO_MPM2_MPM_CNTR_INCR_DATA_MULT_BMSK                                0xffffffff
#define HWIO_MPM2_MPM_CNTR_INCR_DATA_MULT_SHFT                                       0x0

#define HWIO_MPM2_MPM_HW_ID_ADDR                                              (MPM2_MPM_REG_BASE      + 0x000000f8)
#define HWIO_MPM2_MPM_HW_ID_RMSK                                              0xffffffff
#define HWIO_MPM2_MPM_HW_ID_IN          \
        in_dword_masked(HWIO_MPM2_MPM_HW_ID_ADDR, HWIO_MPM2_MPM_HW_ID_RMSK)
#define HWIO_MPM2_MPM_HW_ID_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_HW_ID_ADDR, m)
#define HWIO_MPM2_MPM_HW_ID_MAJOR_BMSK                                        0xf0000000
#define HWIO_MPM2_MPM_HW_ID_MAJOR_SHFT                                              0x1c
#define HWIO_MPM2_MPM_HW_ID_MINOR_BMSK                                         0xfff0000
#define HWIO_MPM2_MPM_HW_ID_MINOR_SHFT                                              0x10
#define HWIO_MPM2_MPM_HW_ID_STEP_BMSK                                             0xffff
#define HWIO_MPM2_MPM_HW_ID_STEP_SHFT                                                0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_G_CTRL_CNTR
 *--------------------------------------------------------------------------*/

#define MPM2_G_CTRL_CNTR_REG_BASE                             (MPM2_MPM_BASE      + 0x00001000)

#define HWIO_MPM2_MPM_CONTROL_CNTCR_ADDR                      (MPM2_G_CTRL_CNTR_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_CONTROL_CNTCR_RMSK                           0x103
#define HWIO_MPM2_MPM_CONTROL_CNTCR_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTCR_ADDR, HWIO_MPM2_MPM_CONTROL_CNTCR_RMSK)
#define HWIO_MPM2_MPM_CONTROL_CNTCR_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTCR_ADDR, m)
#define HWIO_MPM2_MPM_CONTROL_CNTCR_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CONTROL_CNTCR_ADDR,v)
#define HWIO_MPM2_MPM_CONTROL_CNTCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CONTROL_CNTCR_ADDR,m,v,HWIO_MPM2_MPM_CONTROL_CNTCR_IN)
#define HWIO_MPM2_MPM_CONTROL_CNTCR_FCREQ_BMSK                     0x100
#define HWIO_MPM2_MPM_CONTROL_CNTCR_FCREQ_SHFT                       0x8
#define HWIO_MPM2_MPM_CONTROL_CNTCR_HDBG_BMSK                        0x2
#define HWIO_MPM2_MPM_CONTROL_CNTCR_HDBG_SHFT                        0x1
#define HWIO_MPM2_MPM_CONTROL_CNTCR_EN_BMSK                          0x1
#define HWIO_MPM2_MPM_CONTROL_CNTCR_EN_SHFT                          0x0

#define HWIO_MPM2_MPM_CONTROL_CNTSR_ADDR                      (MPM2_G_CTRL_CNTR_REG_BASE      + 0x00000004)
#define HWIO_MPM2_MPM_CONTROL_CNTSR_RMSK                           0x102
#define HWIO_MPM2_MPM_CONTROL_CNTSR_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTSR_ADDR, HWIO_MPM2_MPM_CONTROL_CNTSR_RMSK)
#define HWIO_MPM2_MPM_CONTROL_CNTSR_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTSR_ADDR, m)
#define HWIO_MPM2_MPM_CONTROL_CNTSR_FCACK_BMSK                     0x100
#define HWIO_MPM2_MPM_CONTROL_CNTSR_FCACK_SHFT                       0x8
#define HWIO_MPM2_MPM_CONTROL_CNTSR_HDBG_BMSK                        0x2
#define HWIO_MPM2_MPM_CONTROL_CNTSR_HDBG_SHFT                        0x1

#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_ADDR                   (MPM2_G_CTRL_CNTR_REG_BASE      + 0x00000008)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_RMSK                   0xffffffff
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTCV_LO_ADDR, HWIO_MPM2_MPM_CONTROL_CNTCV_LO_RMSK)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTCV_LO_ADDR, m)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CONTROL_CNTCV_LO_ADDR,v)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CONTROL_CNTCV_LO_ADDR,m,v,HWIO_MPM2_MPM_CONTROL_CNTCV_LO_IN)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_LOAD_VAL_BMSK          0xffffffff
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_LOAD_VAL_SHFT                 0x0

#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_ADDR                   (MPM2_G_CTRL_CNTR_REG_BASE      + 0x0000000c)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_RMSK                     0xffffff
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTCV_HI_ADDR, HWIO_MPM2_MPM_CONTROL_CNTCV_HI_RMSK)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTCV_HI_ADDR, m)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CONTROL_CNTCV_HI_ADDR,v)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CONTROL_CNTCV_HI_ADDR,m,v,HWIO_MPM2_MPM_CONTROL_CNTCV_HI_IN)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_LOAD_VAL_BMSK            0xffffff
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_LOAD_VAL_SHFT                 0x0

#define HWIO_MPM2_MPM_CONTROL_CNTFID0_ADDR                    (MPM2_G_CTRL_CNTR_REG_BASE      + 0x00000020)
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_RMSK                    0xffffffff
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTFID0_ADDR, HWIO_MPM2_MPM_CONTROL_CNTFID0_RMSK)
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTFID0_ADDR, m)
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CONTROL_CNTFID0_ADDR,v)
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CONTROL_CNTFID0_ADDR,m,v,HWIO_MPM2_MPM_CONTROL_CNTFID0_IN)
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_FREQ_BMSK               0xffffffff
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_FREQ_SHFT                      0x0

#define HWIO_MPM2_MPM_CONTROL_ID_ADDR                         (MPM2_G_CTRL_CNTR_REG_BASE      + 0x00000fd0)
#define HWIO_MPM2_MPM_CONTROL_ID_RMSK                         0xffffffff
#define HWIO_MPM2_MPM_CONTROL_ID_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_ID_ADDR, HWIO_MPM2_MPM_CONTROL_ID_RMSK)
#define HWIO_MPM2_MPM_CONTROL_ID_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_ID_ADDR, m)
#define HWIO_MPM2_MPM_CONTROL_ID_MAJOR_BMSK                   0xf0000000
#define HWIO_MPM2_MPM_CONTROL_ID_MAJOR_SHFT                         0x1c
#define HWIO_MPM2_MPM_CONTROL_ID_MINOR_BMSK                    0xfff0000
#define HWIO_MPM2_MPM_CONTROL_ID_MINOR_SHFT                         0x10
#define HWIO_MPM2_MPM_CONTROL_ID_STEP_BMSK                        0xffff
#define HWIO_MPM2_MPM_CONTROL_ID_STEP_SHFT                           0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_G_RD_CNTR
 *--------------------------------------------------------------------------*/

#define MPM2_G_RD_CNTR_REG_BASE                         (MPM2_MPM_BASE      + 0x00002000)

#define HWIO_MPM2_MPM_READ_CNTCV_LO_ADDR                (MPM2_G_RD_CNTR_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_READ_CNTCV_LO_RMSK                0xffffffff
#define HWIO_MPM2_MPM_READ_CNTCV_LO_IN          \
        in_dword_masked(HWIO_MPM2_MPM_READ_CNTCV_LO_ADDR, HWIO_MPM2_MPM_READ_CNTCV_LO_RMSK)
#define HWIO_MPM2_MPM_READ_CNTCV_LO_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_READ_CNTCV_LO_ADDR, m)
#define HWIO_MPM2_MPM_READ_CNTCV_LO_VALUE_BMSK          0xffffffff
#define HWIO_MPM2_MPM_READ_CNTCV_LO_VALUE_SHFT                 0x0

#define HWIO_MPM2_MPM_READ_CNTCV_HI_ADDR                (MPM2_G_RD_CNTR_REG_BASE      + 0x00000004)
#define HWIO_MPM2_MPM_READ_CNTCV_HI_RMSK                  0xffffff
#define HWIO_MPM2_MPM_READ_CNTCV_HI_IN          \
        in_dword_masked(HWIO_MPM2_MPM_READ_CNTCV_HI_ADDR, HWIO_MPM2_MPM_READ_CNTCV_HI_RMSK)
#define HWIO_MPM2_MPM_READ_CNTCV_HI_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_READ_CNTCV_HI_ADDR, m)
#define HWIO_MPM2_MPM_READ_CNTCV_HI_VALUE_BMSK            0xffffff
#define HWIO_MPM2_MPM_READ_CNTCV_HI_VALUE_SHFT                 0x0

#define HWIO_MPM2_MPM_READ_ID_ADDR                      (MPM2_G_RD_CNTR_REG_BASE      + 0x00000fd0)
#define HWIO_MPM2_MPM_READ_ID_RMSK                      0xffffffff
#define HWIO_MPM2_MPM_READ_ID_IN          \
        in_dword_masked(HWIO_MPM2_MPM_READ_ID_ADDR, HWIO_MPM2_MPM_READ_ID_RMSK)
#define HWIO_MPM2_MPM_READ_ID_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_READ_ID_ADDR, m)
#define HWIO_MPM2_MPM_READ_ID_MAJOR_BMSK                0xf0000000
#define HWIO_MPM2_MPM_READ_ID_MAJOR_SHFT                      0x1c
#define HWIO_MPM2_MPM_READ_ID_MINOR_BMSK                 0xfff0000
#define HWIO_MPM2_MPM_READ_ID_MINOR_SHFT                      0x10
#define HWIO_MPM2_MPM_READ_ID_STEP_BMSK                     0xffff
#define HWIO_MPM2_MPM_READ_ID_STEP_SHFT                        0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_TSYNC
 *--------------------------------------------------------------------------*/

#define MPM2_TSYNC_REG_BASE                                  (MPM2_MPM_BASE      + 0x00006000)

#define HWIO_MPM2_MPM_TSYNC_CTL_ADDR                         (MPM2_TSYNC_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_TSYNC_CTL_RMSK                              0x7ff
#define HWIO_MPM2_MPM_TSYNC_CTL_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_CTL_ADDR, HWIO_MPM2_MPM_TSYNC_CTL_RMSK)
#define HWIO_MPM2_MPM_TSYNC_CTL_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_CTL_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_CTL_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_TSYNC_CTL_ADDR,v)
#define HWIO_MPM2_MPM_TSYNC_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_TSYNC_CTL_ADDR,m,v,HWIO_MPM2_MPM_TSYNC_CTL_IN)
#define HWIO_MPM2_MPM_TSYNC_CTL_WIDTH_BMSK                        0x7fc
#define HWIO_MPM2_MPM_TSYNC_CTL_WIDTH_SHFT                          0x2
#define HWIO_MPM2_MPM_TSYNC_CTL_IRQ_BMSK                            0x2
#define HWIO_MPM2_MPM_TSYNC_CTL_IRQ_SHFT                            0x1
#define HWIO_MPM2_MPM_TSYNC_CTL_ENABLE_BMSK                         0x1
#define HWIO_MPM2_MPM_TSYNC_CTL_ENABLE_SHFT                         0x0

#define HWIO_MPM2_MPM_TSYNC_CMD_ADDR                         (MPM2_TSYNC_REG_BASE      + 0x00000004)
#define HWIO_MPM2_MPM_TSYNC_CMD_RMSK                                0x1
#define HWIO_MPM2_MPM_TSYNC_CMD_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_TSYNC_CMD_ADDR,v)
#define HWIO_MPM2_MPM_TSYNC_CMD_START_BMSK                          0x1
#define HWIO_MPM2_MPM_TSYNC_CMD_START_SHFT                          0x0

#define HWIO_MPM2_MPM_TSYNC_START_ADDR                       (MPM2_TSYNC_REG_BASE      + 0x00000008)
#define HWIO_MPM2_MPM_TSYNC_START_RMSK                        0x7ffffff
#define HWIO_MPM2_MPM_TSYNC_START_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_START_ADDR, HWIO_MPM2_MPM_TSYNC_START_RMSK)
#define HWIO_MPM2_MPM_TSYNC_START_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_START_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_START_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_TSYNC_START_ADDR,v)
#define HWIO_MPM2_MPM_TSYNC_START_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_TSYNC_START_ADDR,m,v,HWIO_MPM2_MPM_TSYNC_START_IN)
#define HWIO_MPM2_MPM_TSYNC_START_TIME_BMSK                   0x7ffffff
#define HWIO_MPM2_MPM_TSYNC_START_TIME_SHFT                         0x0

#define HWIO_MPM2_MPM_TSYNC_START_PHASE_ADDR                 (MPM2_TSYNC_REG_BASE      + 0x0000000c)
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_RMSK                       0xff
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_START_PHASE_ADDR, HWIO_MPM2_MPM_TSYNC_START_PHASE_RMSK)
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_START_PHASE_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_TSYNC_START_PHASE_ADDR,v)
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_TSYNC_START_PHASE_ADDR,m,v,HWIO_MPM2_MPM_TSYNC_START_PHASE_IN)
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_PHASE_BMSK                 0xff
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_PHASE_SHFT                  0x0

#define HWIO_MPM2_MPM_TSYNC_PERIOD_ADDR                      (MPM2_TSYNC_REG_BASE      + 0x00000010)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_RMSK                       0x7ffffff
#define HWIO_MPM2_MPM_TSYNC_PERIOD_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_PERIOD_ADDR, HWIO_MPM2_MPM_TSYNC_PERIOD_RMSK)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_PERIOD_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_TSYNC_PERIOD_ADDR,v)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_TSYNC_PERIOD_ADDR,m,v,HWIO_MPM2_MPM_TSYNC_PERIOD_IN)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PERIOD_BMSK                0x7ffffff
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PERIOD_SHFT                      0x0

#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_ADDR                (MPM2_TSYNC_REG_BASE      + 0x00000014)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_RMSK                      0xff
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_ADDR, HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_RMSK)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_ADDR,v)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_ADDR,m,v,HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_IN)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_PHASE_BMSK                0xff
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_PHASE_SHFT                 0x0

#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_ADDR                 (MPM2_TSYNC_REG_BASE      + 0x00000018)
#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_RMSK                  0x7ffffff
#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_STATUS_TIME_ADDR, HWIO_MPM2_MPM_TSYNC_STATUS_TIME_RMSK)
#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_STATUS_TIME_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_TIME_BMSK             0x7ffffff
#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_TIME_SHFT                   0x0

#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_ADDR                (MPM2_TSYNC_REG_BASE      + 0x0000001c)
#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_RMSK                      0xff
#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_ADDR, HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_RMSK)
#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_PHASE_BMSK                0xff
#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_PHASE_SHFT                 0x0

#define HWIO_MPM2_MPM_GRFC_CXO_EN_ADDR                       (MPM2_TSYNC_REG_BASE      + 0x00000020)
#define HWIO_MPM2_MPM_GRFC_CXO_EN_RMSK                              0x1
#define HWIO_MPM2_MPM_GRFC_CXO_EN_IN          \
        in_dword_masked(HWIO_MPM2_MPM_GRFC_CXO_EN_ADDR, HWIO_MPM2_MPM_GRFC_CXO_EN_RMSK)
#define HWIO_MPM2_MPM_GRFC_CXO_EN_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_GRFC_CXO_EN_ADDR, m)
#define HWIO_MPM2_MPM_GRFC_CXO_EN_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_GRFC_CXO_EN_ADDR,v)
#define HWIO_MPM2_MPM_GRFC_CXO_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_GRFC_CXO_EN_ADDR,m,v,HWIO_MPM2_MPM_GRFC_CXO_EN_IN)
#define HWIO_MPM2_MPM_GRFC_CXO_EN_MUX_SEL_BMSK                      0x1
#define HWIO_MPM2_MPM_GRFC_CXO_EN_MUX_SEL_SHFT                      0x0

/*----------------------------------------------------------------------------
 * MODULE: QDSS_QDSSCSR
 *--------------------------------------------------------------------------*/

#define QDSS_QDSSCSR_REG_BASE                                             (QDSS_QDSS_BASE      + 0x00002000)

#define HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_ADDR                        (QDSS_QDSSCSR_REG_BASE      + 0x00000060)
#define HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_RMSK                        0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_ADDR, HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_IN)
#define HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_PWRREQIGNORE_BMSK           0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_PWRREQIGNORE_SHFT                  0x0

#define HWIO_QDSS_CS_QDSSCSR_LOCKACCESS_ADDR                              (QDSS_QDSSCSR_REG_BASE      + 0x00000fb0)
#define HWIO_QDSS_CS_QDSSCSR_LOCKACCESS_RMSK                              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_LOCKACCESS_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_LOCKACCESS_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_LOCKACCESS_LOCKACCESS_BMSK                   0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_LOCKACCESS_LOCKACCESS_SHFT                          0x0

#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_ADDR                              (QDSS_QDSSCSR_REG_BASE      + 0x00000fb4)
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_RMSK                              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_ADDR, HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_RFU_BMSK                          0xfffffff8
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_RFU_SHFT                                 0x3
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_FIELD_8_BIT_LOCK_BMSK                    0x4
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_FIELD_8_BIT_LOCK_SHFT                    0x2
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_ACCESS_BMSK                              0x2
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_ACCESS_SHFT                              0x1
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_LOCK_CONTROL_BMSK                        0x1
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_LOCK_CONTROL_SHFT                        0x0


#define MPM2_TSENS_REG_BASE                                                                  (MPM2_MPM_BASE      + 0x00008000)

#define HWIO_MPM2_TSENS_CTRL_ADDR                                                            (MPM2_TSENS_REG_BASE      + 0x00000000)
#define HWIO_MPM2_TSENS_CTRL_RMSK                                                             0xffc3fff
#define HWIO_MPM2_TSENS_CTRL_ATTR                                                                   0x3
#define HWIO_MPM2_TSENS_CTRL_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_CTRL_ADDR, HWIO_MPM2_TSENS_CTRL_RMSK)
#define HWIO_MPM2_TSENS_CTRL_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_CTRL_ADDR, m)
#define HWIO_MPM2_TSENS_CTRL_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_CTRL_ADDR,v)
#define HWIO_MPM2_TSENS_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_CTRL_ADDR,m,v,HWIO_MPM2_TSENS_CTRL_IN)
#define HWIO_MPM2_TSENS_CTRL_TSENS_CLAMP_BMSK                                                 0x8000000
#define HWIO_MPM2_TSENS_CTRL_TSENS_CLAMP_SHFT                                                      0x1b
#define HWIO_MPM2_TSENS_CTRL_TSENS_BYPASS_EN_BMSK                                             0x4000000
#define HWIO_MPM2_TSENS_CTRL_TSENS_BYPASS_EN_SHFT                                                  0x1a
#define HWIO_MPM2_TSENS_CTRL_MEASURE_PERIOD_BMSK                                              0x3fc0000
#define HWIO_MPM2_TSENS_CTRL_MEASURE_PERIOD_SHFT                                                   0x12
#define HWIO_MPM2_TSENS_CTRL_SENSOR10_EN_BMSK                                                    0x2000
#define HWIO_MPM2_TSENS_CTRL_SENSOR10_EN_SHFT                                                       0xd
#define HWIO_MPM2_TSENS_CTRL_SENSOR9_EN_BMSK                                                     0x1000
#define HWIO_MPM2_TSENS_CTRL_SENSOR9_EN_SHFT                                                        0xc
#define HWIO_MPM2_TSENS_CTRL_SENSOR8_EN_BMSK                                                      0x800
#define HWIO_MPM2_TSENS_CTRL_SENSOR8_EN_SHFT                                                        0xb
#define HWIO_MPM2_TSENS_CTRL_SENSOR7_EN_BMSK                                                      0x400
#define HWIO_MPM2_TSENS_CTRL_SENSOR7_EN_SHFT                                                        0xa
#define HWIO_MPM2_TSENS_CTRL_SENSOR6_EN_BMSK                                                      0x200
#define HWIO_MPM2_TSENS_CTRL_SENSOR6_EN_SHFT                                                        0x9
#define HWIO_MPM2_TSENS_CTRL_SENSOR5_EN_BMSK                                                      0x100
#define HWIO_MPM2_TSENS_CTRL_SENSOR5_EN_SHFT                                                        0x8
#define HWIO_MPM2_TSENS_CTRL_SENSOR4_EN_BMSK                                                       0x80
#define HWIO_MPM2_TSENS_CTRL_SENSOR4_EN_SHFT                                                        0x7
#define HWIO_MPM2_TSENS_CTRL_SENSOR3_EN_BMSK                                                       0x40
#define HWIO_MPM2_TSENS_CTRL_SENSOR3_EN_SHFT                                                        0x6
#define HWIO_MPM2_TSENS_CTRL_SENSOR2_EN_BMSK                                                       0x20
#define HWIO_MPM2_TSENS_CTRL_SENSOR2_EN_SHFT                                                        0x5
#define HWIO_MPM2_TSENS_CTRL_SENSOR1_EN_BMSK                                                       0x10
#define HWIO_MPM2_TSENS_CTRL_SENSOR1_EN_SHFT                                                        0x4
#define HWIO_MPM2_TSENS_CTRL_SENSOR0_EN_BMSK                                                        0x8
#define HWIO_MPM2_TSENS_CTRL_SENSOR0_EN_SHFT                                                        0x3
#define HWIO_MPM2_TSENS_CTRL_TSENS_ADC_CLK_SEL_BMSK                                                 0x4
#define HWIO_MPM2_TSENS_CTRL_TSENS_ADC_CLK_SEL_SHFT                                                 0x2
#define HWIO_MPM2_TSENS_CTRL_TSENS_SW_RST_BMSK                                                      0x2
#define HWIO_MPM2_TSENS_CTRL_TSENS_SW_RST_SHFT                                                      0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_EN_BMSK                                                          0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_EN_SHFT                                                          0x0


#endif /* __MPM2_HWIO_H__ */

